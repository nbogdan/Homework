#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "utils.h"


void simpleMultiply(double *matrix, double *vector, double *result, int upper, int isTranspose, int N)
{
	int i;
	

	if(isTranspose == 0)
	{
		for(i = 0; i < N * N; i++)
			if(upper && (i % N >= i / N))
				result[i / N] += matrix[i] * vector[i % N];
			else if(upper == 0 && (i % N <= i / N))
				result[i / N] += matrix[i] * vector[i % N];	
	}
	else if(isTranspose == 1)
		for(i = 0; i < N * N; i++)
			if(!upper && i % N >= i / N)
				result[i % N] += matrix[i] * vector[i / N];
			else if(upper && i % N <= i / N)
				result[i % N] += matrix[i] * vector[i / N];	

	for(i = 0; i < N; i++)
		vector[i] = result[i];
}



int main(int argc, char* argv[]) 
{
	int i, upper, N;	
	double *matrix, *vector, *result;
		
	struct timeval start, end;
	float total_time;

	if(argc < 3)
		return -1;

	/* dimensiunea matricei */
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
	
	result = calloc(N, sizeof(double));
	if(result == NULL)
		return -1;


	populateStructure(matrix, vector, result, upper, N);
	
	
	gettimeofday(&start, NULL);
	simpleMultiply(matrix, vector, result, upper, 0, N);

	gettimeofday(&end, NULL);
	total_time = ((end.tv_sec - start.tv_sec)*1000000.0f + end.tv_usec - start.tv_usec) / 1000000.0f;
	
	if(argc == 3)
                printf("%f\n", total_time);
        else if(argc == 4 && strcmp(argv[3], "debug") == 0)
        {
                for(i = 0; i < N ; i++)
                        printf("%8.2lf\n",vector[i]);
                printf("\n");
        }


	free(matrix);
	free(result);
	free(vector);

	return 0;
}
