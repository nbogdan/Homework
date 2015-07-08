#include "utils.h"


void populateStructure(double *matrix, double *vector, double *result, int upper, int size)
{
	int i;
	
	srand(0);
	for(i = 0; i < size * size; i++)
	{
		if(upper && i % size >= i / size)
		{
			matrix[i] = (double)(rand() % 1000000) / (double)100000; 
		}
		if(!upper && i % size <= i / size)
		{
			matrix[i] = (double)(rand() % 1000000) / (double)100000;
		}
		
	}
	if(result != NULL)
		for(i = 0; i < size; i++)
		{
			vector[i] =  (double)(rand() % 1000000) / (double)100000;
			result[i] = 0;
		}
	else
		for(i = 0; i < size; i++)
			vector[i] =  (double)(rand() % 1000000) / (double)100000;
		
}
