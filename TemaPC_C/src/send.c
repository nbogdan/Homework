#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"
#include "Auxiliary.c"
#include "Hamming.c"
#include "Task0S.c"
#include "Task1S.c"
#include "Task2S.c"
#include "Task3S.c"
#include "Task4S.c"


int main(int argc, char *argv[])
{

	char *filename;
	int task_index, speed, delay;

	task_index = atoi(argv[1]);
	filename = argv[2];
	speed = atoi(argv[3]);
	delay = atoi(argv[4]);
	
	printf("[SENDER] Sender starts.\n");
	printf("[SENDER] Filename=%s, task_index=%d, speed=%d, delay=%d\n", filename, task_index, speed, delay);
	
	init(HOST, PORT1);
	
	
	switch(task_index)
	{
		case 0:
			Task0(speed, delay, filename);
			break;	
		case 1:
			Task1(speed, delay, filename);
			break;
		case 2:
			Task2(speed, delay, filename);
			break;
		case 3:
			Task3(speed, delay, filename);
			break;
		case 4:
			Task4(speed, delay, filename);
			break;
	}
	
	
	return 0;
}
