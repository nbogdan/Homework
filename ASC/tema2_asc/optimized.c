#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "utils.h"


void optimized_multiply(double *matrix, double *vector, int isUpper, int isTranspose, int N)
{
	double *p, *v, *aux;
	int i;
	p = matrix;

	if(isTranspose == 0)
	{
		if(isUpper)	
			/* pentru matrice superior triunghiulara */
			for(i = 0; i < N; i++)
			{
				register double sum = 0;
				v = vector + i;
				p += i;
				
				for(; v - vector < N; v++, p++)
					sum += (*p) * (*v);	
				vector[i] = sum;
			}
		else
			/* pentru matrice inferior triunghiulara */
			for(i = N - 1; i >= 0; i--)
			{
				register double sum = 0;
				v = vector + i;
				p += i;
				
				for(; v - vector <= i; v++, p++)
					sum += (*p) * (*v);	
				vector[i] = sum;
			}
	}
	/* In cazul in care trebuie sa inmultim cu transpusa, 
	nu am mai facut toate optimizarile posibile */
	else
	{
		aux = calloc(N, sizeof(double));

		for(i = 0; i < N; i++)
		{
			register double sum = 0;
			v = vector;
			
			for(; v - vector < N; v++, p++)
				sum += (*p) * (*v);	
			aux[i] = sum;
		}
		for(i = 0; i < N; i++)
			vector[i] = aux[i];

		free(aux);
	}
}


int main(int argc, char* argv[]) 
{
	int i, upper, N;	
	double *matrix, *vector;
		
	struct timeval start, end;
	float total_time;

	if(argc < 3)
		return -1;

	N = atoi(argv[1]);
	
	if(strcmp(argv[2],"up")==0)
		upper = 1;
	else
		upper = 0;

	matrix = calloc(N*N, sizeof(double));

	if(matrix == NULL)
		return -1;

	vector = calloc(N, sizeof(double));
	if(vector == NULL)
		return -1;

	
	populateStructure(matrix,vector,NULL,upper, N);
	
	
	gettimeofday(&start, NULL);
	optimized_multiply(matrix, vector, upper, 0);
	gettimeofday(&end, NULL);
	total_time = ((end.tv_sec - start.tv_sec)*1000000.0f + end.tv_usec - start.tv_usec) / 1000000.0f;



	if(argc == 3)
		printf("%f\n", total_time);
	else if(argc == 4 && strcmp(argv[3],"debug") == 0)
	{
	
		for(i = 0; i < N ; i++)
        		printf("%8.2lf\n",vector[i]);
	    	printf("\n");
	}

	free(matrix);
	free(vector);

	return 0;
}
