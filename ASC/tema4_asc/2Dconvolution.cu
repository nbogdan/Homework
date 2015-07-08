/*
 * Copyright 1993-2006 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.
 *
 * This software and the information contained herein is PROPRIETARY and
 * CONFIDENTIAL to NVIDIA and is being provided under the terms and
 * conditions of a Non-Disclosure Agreement.  Any reproduction or
 * disclosure to any third party without the express written consent of
 * NVIDIA is prohibited.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.  This source code is a "commercial item" as
 * that term is defined at 48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer software" and "commercial computer software
 * documentation" as such terms are used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 */

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <helper_cuda.h>
#include <helper_timer.h>
#include <helper_functions.h>
#include <helper_math.h>

// includes, project
#include "2Dconvolution.h"


////////////////////////////////////////////////////////////////////////////////
// declarations, forward

extern "C"
void computeGold(float*, const float*, const float*, unsigned int, unsigned int);

Matrix AllocateDeviceMatrix(int width, int height);
Matrix AllocateMatrix(int width, int height);
void FreeDeviceMatrix(Matrix* M);
void FreeMatrix(Matrix* M);

void ConvolutionOnDevice(const Matrix M, const Matrix N, Matrix P);
void ConvolutionOnDeviceShared(const Matrix M, const Matrix N, Matrix P);

////////////////////////////////////////////////////////////////////////////////
// Înmulțirea fără memorie partajată
////////////////////////////////////////////////////////////////////////////////
__global__ void ConvolutionKernel(Matrix M, Matrix N, Matrix P)
{

    float newValue = 0;
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int i, j;
    

    if(row >= N.height || col >= N.width)
        return;

    for(i = 0; i < KERNEL_SIZE; i++)
        for(j = 0; j < KERNEL_SIZE; j++) {

            if(row + i - (KERNEL_SIZE / 2) >= 0 && col + j - (KERNEL_SIZE / 2) >= 0 &&
               row + i - (KERNEL_SIZE / 2) < N.height && col + j - (KERNEL_SIZE / 2) < N.width )
            {
                newValue += N.elements[(row  + i - KERNEL_SIZE / 2)* P.width + col + j - KERNEL_SIZE / 2] * M.elements[i * KERNEL_SIZE + j];
            }
        }
        
    P.elements[row * P.width + col] = newValue;    
}


////////////////////////////////////////////////////////////////////////////////
// Înmulțirea cu memorie partajată
////////////////////////////////////////////////////////////////////////////////
__global__ void ConvolutionKernelShared(Matrix M, Matrix N, Matrix P)
{
    
    __shared__ float Ns[(BLOCK_SIZE + KERNEL_SIZE - 1)*(BLOCK_SIZE + KERNEL_SIZE - 1)];
    __shared__ float Ms[KERNEL_SIZE * KERNEL_SIZE];

    int memorySize = BLOCK_SIZE + KERNEL_SIZE - 1;

    float newValue = 0;

    /* Randul si coloana aferente blocului */
    int blockRow = blockIdx.y * blockDim.y;
    int blockCol = blockIdx.x * blockDim.x;

    /* Randul si coloana aferente thread-ului */
    int row = blockRow + threadIdx.y;
    int col = blockCol + threadIdx.x;

    /* Pozitie thread in cadrul blocului */
    int id = threadIdx.y * blockDim.x + threadIdx.x;
    int i, j;
    
    /* Pozitie in Ns corespunzatoare thread-ului curent */
    int newRow = blockRow - (KERNEL_SIZE / 2) + (id / memorySize);
    int newCol = blockCol - (KERNEL_SIZE / 2) + (id % memorySize);
    
    /* Daca blocul este complet fiecare thread(256 intr-un bloc) va prelua
    cate o valoare din cele 400 disponibile intr-un bloc. */
    Ns[id] = 0;
       if((newRow >= 0 && newRow < N.height) &&
            (newCol >= 0 && newCol < N.width))
                Ns[id] = N.elements[newRow * N.width + newCol];

    
    /* Thread-urile preiau restul de 144 de valori ramase + matricea de kernel */
    id += BLOCK_SIZE * BLOCK_SIZE;
    newRow = blockRow - (KERNEL_SIZE / 2) + (id / memorySize);
    newCol = blockCol - (KERNEL_SIZE / 2) + (id % memorySize);
    if(id < memorySize * memorySize)
    {
        Ns[id] = 0;
        if((newRow >= 0 && newRow < N.height) &&
            ( newCol >= 0 && newCol < N.width))
                Ns[id] = N.elements[newRow * N.width + newCol];
    }
    else if(id < memorySize * memorySize + KERNEL_SIZE * KERNEL_SIZE)
    {
        Ms[id - memorySize * memorySize] = M.elements[id - memorySize * memorySize];
    }

    __syncthreads();/* In acest moment avem Ms si Ns disponibile. */

    if(row >= N.height || col >= N.width)
        return;
    
    /* Initializam valoarea din matricea rezultat pentru a nu avea surprize. */
    P.elements[row * P.width + col] = 0;
        
    /* Calculam noua valoare a pixel-ului. */
    for(i = 0; i < KERNEL_SIZE; i++)
        for(j = 0; j < KERNEL_SIZE; j++)
            newValue += Ns[(row + i - blockRow) * memorySize + col - blockCol + j] * Ms[i * KERNEL_SIZE + j];  
    
    P.elements[row * P.width + col] = newValue;
}

////////////////////////////////////////////////////////////////////////////////
// Returnează 1 dacă matricele sunt ~ egale
////////////////////////////////////////////////////////////////////////////////
int CompareMatrices(Matrix A, Matrix B)
{
    int i;
    if(A.width != B.width || A.height != B.height || A.pitch != B.pitch)
        return 0;
    int size = A.width * A.height;
    for(i = 0; i < size; i++)
        if(fabs(A.elements[i] - B.elements[i]) > MAX_ERR)
            return 0;
    return 1;
}
void GenerateRandomMatrix(Matrix m)
{
    int i;
    int size = m.width * m.height;

    srand(time(NULL));

    for(i = 0; i < size; i++)
        m.elements[i] = rand() / (float)RAND_MAX;
}

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) 
{
    int width = 0, height = 0;
    FILE *f, *out;
    if(argc < 2)
    {
        printf("Argumente prea puține, trimiteți id-ul testului care trebuie rulat\n");
        return 0;
    }
    char name[100];
    sprintf(name, "./tests/test_%s.txt", argv[1]);
    f = fopen(name, "r");
    out = fopen("out.txt", "a");
    fscanf(f, "%d%d", &width, &height);
    Matrix M;//kernel de pe host
    Matrix N;//matrice inițială de pe host
    Matrix P;//rezultat fără memorie partajată calculat pe GPU
    Matrix PS;//rezultatul cu memorie partajată calculat pe GPU
    
    M = AllocateMatrix(KERNEL_SIZE, KERNEL_SIZE);
    N = AllocateMatrix(width, height);        
    P = AllocateMatrix(width, height);
    PS = AllocateMatrix(width, height);

    GenerateRandomMatrix(M);
    GenerateRandomMatrix(N);
    

    //test dummy, pentru a elimina acea anomalie, vezi README
    printf("Test dummy:\t");
    ConvolutionOnDeviceShared(M, N, PS);

    // M * N pe device
    ConvolutionOnDevice(M, N, P);
    
    // M * N pe device cu memorie partajată
    ConvolutionOnDeviceShared(M, N, PS);

    // calculează rezultatul pe CPU pentru comparație
    Matrix reference = AllocateMatrix(P.width, P.height);

    StopWatchInterface *kernelTime = NULL;
    sdkCreateTimer(&kernelTime);
    sdkResetTimer(&kernelTime);
    sdkStartTimer(&kernelTime);
    computeGold(reference.elements, M.elements, N.elements, N.height, N.width);
    sdkStopTimer(&kernelTime);
    printf ("Timp execuție seriala: %f ms\n", sdkGetTimerValue(&kernelTime));

    int i;
    int x, y; x = y  = 0;

    for(i = 0; i < PS.width * PS.height; i++)
    {
        if(!(PS.elements[i] > reference.elements[i] + 0.1 || PS.elements[i] < reference.elements[i] - 0.1)) x++;
        y++;
    }

    // verifică dacă rezultatul obținut pe device este cel așteptat
    int res = CompareMatrices(reference, P);
    printf("Test global %s\n", (1 == res) ? "PASSED" : "FAILED");
    fprintf(out, "Test global %s %s\n", argv[1], (1 == res) ? "PASSED" : "FAILED");

    // verifică dacă rezultatul obținut pe device cu memorie partajată este cel așteptat
  //  int ress = CompareMatrices(reference, PS);
    int ress = CompareMatrices(reference, PS);
    printf("Test shared %s\n", (1 == ress) ? "PASSED" : "FAILED");
    fprintf(out, "Test shared %s %s\n", argv[1], (1 == ress) ? "PASSED" : "FAILED");
   
    // Free matrices
    FreeMatrix(&M);
    FreeMatrix(&N);
    FreeMatrix(&P);
    FreeMatrix(&PS);

    fclose(f);
    fclose(out);
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
//! Run a simple test for CUDA
////////////////////////////////////////////////////////////////////////////////
void ConvolutionOnDevice(const Matrix M, const Matrix N, Matrix P)
{
    Matrix Md, Nd, Pd; //matricele corespunzătoare de pe device
    int size;
    //pentru măsurarea timpului de execuție în kernel
    StopWatchInterface *kernelTime = NULL;
    sdkCreateTimer(&kernelTime);
    sdkResetTimer(&kernelTime);

    //Aloc matricele de pe device
    Md.width = M.width;
    Md.height = M.height;
    Md.pitch = M.pitch;
    size = M.width * M.height * sizeof(float);
    cudaMalloc( (void**) &Md.elements, size);
    cudaMemcpy( Md.elements, M.elements, size, cudaMemcpyHostToDevice);

    Nd.width = N.width;
    Nd.height = N.height;
    Nd.pitch = N.pitch;
    size = N.width * N.height * sizeof(float);
    cudaMalloc( (void**) &Nd.elements, size);
    cudaMemcpy( Nd.elements, N.elements, size, cudaMemcpyHostToDevice);


    Pd.width = P.width;
    Pd.height = P.height;
    Pd.pitch = P.pitch;
    size = P.width * P.height * sizeof(float);
    cudaMalloc( (void**) &Pd.elements, size);
    

    //Setez configurația de rulare a kernelului
    dim3 dimGrid(N.width/BLOCK_SIZE + 1, N.height/BLOCK_SIZE + 1);
    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE); // sqrt(BLOCK_SIZE)


    sdkStartTimer(&kernelTime);

    //Lansez în execuție kernelul    
    ConvolutionKernel<<<dimGrid, dimBlock>>>(Md, Nd, Pd);

    cudaThreadSynchronize();
    sdkStopTimer(&kernelTime);
    printf ("Timp execuție kernel: %f ms\n", sdkGetTimerValue(&kernelTime));

    //Copiez rezultatul pe host
    cudaMemcpy( P.elements, Pd.elements, size, cudaMemcpyDeviceToHost);

    //Eliberez memoria matricelor de pe device
    cudaFree(Md.elements);
    cudaFree(Nd.elements);
    cudaFree(Pd.elements);
}


void ConvolutionOnDeviceShared(const Matrix M, const Matrix N, Matrix P)
{
    Matrix Md, Nd, Pd; //matricele corespunzătoare de pe device
    int size;

    //pentru măsurarea timpului de execuție în kernel
    StopWatchInterface *kernelTime = NULL;
    sdkCreateTimer(&kernelTime);
    sdkResetTimer(&kernelTime);
    
    //Aloc matricele de pe device
    Md.width = M.width;
    Md.height = M.height;
    Md.pitch = M.pitch;
    size = M.width * M.height * sizeof(float);
    cudaMalloc( (void**) &Md.elements, size);
    cudaMemcpy( Md.elements, M.elements, size, cudaMemcpyHostToDevice);

    Nd.width = N.width;
    Nd.height = N.height;
    Nd.pitch = N.pitch;
    size = N.width * N.height * sizeof(float);
    cudaMalloc( (void**) &Nd.elements, size);
    cudaMemcpy( Nd.elements, N.elements, size, cudaMemcpyHostToDevice);

    Pd.width = P.width;
    Pd.height = P.height;
    Pd.pitch = P.pitch;
    size = P.width * P.height * sizeof(float);
    cudaMalloc( (void**) &Pd.elements, size);
    

   //Setez configurația de rulare a kernelului
    dim3 dimGrid(N.width/BLOCK_SIZE + 1, N.height/BLOCK_SIZE + 1);
    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE); // sqrt(BLOCK_SIZE)


    sdkStartTimer(&kernelTime);

    //Lansez în execuție kernelul    
    ConvolutionKernelShared<<<dimGrid, dimBlock>>>(Md, Nd, Pd);
       
    cudaThreadSynchronize();
    sdkStopTimer(&kernelTime);
    printf ("Timp execuție kernel cu memorie partajata: %f ms\n", sdkGetTimerValue(&kernelTime));
    
    //Copiez rezultatul pe host
    cudaMemcpy( P.elements, Pd.elements, size, cudaMemcpyDeviceToHost);

    //Eliberez memoria matricelor de pe device
    cudaFree(Md.elements);
    cudaFree(Nd.elements);
    cudaFree(Pd.elements);
}


// Alocă matrice pe host de dimensiune height*width
Matrix AllocateMatrix(int width, int height)
{
    Matrix M;
    M.width = M.pitch = width;
    M.height = height;
    int size = M.width * M.height;    
    M.elements = (float*) malloc(size*sizeof(float));
    return M;
}    

// Eliberează o matrice de pe device
void FreeDeviceMatrix(Matrix* M)
{
    cudaFree(M->elements);
    M->elements = NULL;
}

// Eliberează o matrice de pe host
void FreeMatrix(Matrix* M)
{
    free(M->elements);
    M->elements = NULL;
}