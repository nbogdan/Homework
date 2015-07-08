#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <cblas.h>
#include "utils.h"

/*
 *Se presupune ca vom primi N ca variabila definita la compilare
 * */


#define CBLAS_TRANSPOSE_NOTR 111
#define CBLAS_TRANSPOSE_TR 112
#define CBLAS_UPPER 121
#define CBLAS_LOWER 122
#define CBLAS_DIAG_NONUNIT 131
#define CBLAS_DIAG_UNIT 132
#define CBLAS_ROW_MAJOR 101




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

	
	if(strcmp(argv[1],"up")==0)
		upper = 1;
	else
		upper = 0;
	populateStructure(matrix,vector,NULL,upper, N);
	
	gettimeofday(&start, NULL);
	if(upper == 1)
		cblas_dtrmv(CBLAS_ROW_MAJOR, CBLAS_UPPER, CBLAS_TRANSPOSE_NOTR, CBLAS_DIAG_NONUNIT, N, matrix, N, vector, 1);
	else
		cblas_dtrmv(CBLAS_ROW_MAJOR, CBLAS_LOWER, CBLAS_TRANSPOSE_NOTR, CBLAS_DIAG_NONUNIT, N, matrix, N, vector, 1);
	gettimeofday(&end, NULL);
	total_time = ((end.tv_sec - start.tv_sec)*1000000.0 + end.tv_usec - start.tv_usec) / 1000000.0;

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
