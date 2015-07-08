#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"
#include "Auxiliary.c"
#include "Hamming.c"
#include "Task0R.c"
#include "Task1R.c"
#include "Task2R.c"
#include "Task3R.c"
#include "Task4R.c"


int main(int argc, char *argv[])
{
	int task_index;

	task_index = atoi(argv[1]);	
	printf("[RECEIVER] Receiver starts.\n");
	printf("[RECEIVER] Task index=%d\n", task_index);
		
	init(HOST, PORT2);
	
	switch(task_index)
	{
		case 0:
			Task0();
			break;	
		case 1:
			Task1();
			break;
		case 2:
			Task2();
			break;
		case 3:
			Task3();
			break;
		case 4:
			Task4();
			break;
	}
	

	return 0;
}
