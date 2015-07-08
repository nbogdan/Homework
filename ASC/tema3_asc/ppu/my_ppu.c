/*
	Tema 3 ASC

	Student:	NICULA Bogdan
	Grupa:		334CB
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <libspe2.h>
#include <pthread.h>
#include <string.h>
#include <libmisc.h>

#include "btc.h"

extern spe_program_handle_t my_spu;



typedef struct {
	short int *sourcePGM;	// adresa matrice(liniarizata) pgm sursa
	struct block* destBTC;	// adresa vector de structuri block
	short int *destPGM;	// adresa matrice(liniarizata) pgm destinatie
	int width, height;	/* dimensiuni pgm initial */
	int num_spus; /* nr spu-uri */
	int spu_id;	/* id spu*/
	char padding[4]; /* octeti folositi pentru ca dimensiunea structurii sa fie multiplu de 16
						padding[0] indica tipul operatiei: normal/double buffering */
} info_token;




/*
 * functie folosita pentru a rula thread-urile de executie ale SPU-urilor
 */
void *ppu_pthread_function(void* arg) {
   	unsigned int entry = SPE_DEFAULT_ENTRY;
	spe_context_ptr_t ctx;

	/* Creare context */
    if ((ctx = spe_context_create (0, NULL)) == NULL) {
        perror ("Failed creating context");
        exit (1);
    }

    /* Incarcare program in context */
    if (spe_program_load (ctx, &my_spu)) {
        perror ("Failed loading program");
        exit (1);
    }

    if (spe_context_run(ctx, &entry, 0, (void*)arg, (void*)sizeof(info_token), NULL) < 0) {
        perror ("Failed running context");
        exit (1);
    }


	/* Distrugere context */
	if (spe_context_destroy (ctx) != 0) {
	    perror("Failed destroying context");
	    exit (1);
	}

   pthread_exit(NULL);
}




int main(int argc, char **argv)
{
    int i, j, k, num_spus, mode;
    pthread_t *spe_threads;
   	info_token *spu_arg;
   	short int *intermediary_pixels;
   	struct img in_pgm, out_pgm;
   	struct c_img btc, in_btc;
   	struct timeval t_init, t_final;
   	double total_time;


    if(argc != 6)
    {
    	printf("Wrong number of arguments.\nUsage: %s mode num_spus in_file out.btc out.pgm\n", argv[0]);
       	return -1;
    }

    mode = atoi(argv[1]);
    if(mode != 0 && mode != 1)
    {
    	printf("Wrong mode. Should be 1 or 0.\n");
    	return -1;
    }
    printf("Mode %s\n", mode ? "double buffering" : "normal");
    

	num_spus = atoi(argv[2]);
    printf("Num SPUs %i\n", num_spus);
    if((num_spus & (num_spus - 1)) || num_spus > 8)
    {
    	printf("Recommended number of SPUs is 1, 2, 4 or 8.\n");
    }



    spe_threads = (pthread_t *)malloc(num_spus * sizeof(pthread_t));
    spu_arg = (info_token *)malloc_align(num_spus * sizeof(info_token), 4);


    /*
     * citire fisier de intrare si initializare imagine
     */
    
    read_pgm(argv[3], &in_pgm);
	btc.width = in_pgm.width;    
	btc.height = in_pgm.height;
	btc.blocks = malloc_align(in_pgm.width * in_pgm.height * sizeof(struct block) / 64, 4);

	out_pgm.width = in_pgm.width;    
	out_pgm.height = in_pgm.height;
	out_pgm.pixels = malloc_align(in_pgm.width * in_pgm.height * sizeof(short int), 4);
	intermediary_pixels = malloc_align(in_pgm.width * in_pgm.height * sizeof(short int), 4);
	
    /* copiez imaginea pgm intr-un buffer aliniat la 16 */
	memcpy(intermediary_pixels, in_pgm.pixels, in_pgm.width * in_pgm.height * sizeof(short int));
	
    
   	gettimeofday(&t_init, NULL);
    for(i=0; i < num_spus; i++) {
      
      /* setez informatii trimitere */

	      spu_arg[i].width = in_pgm.width;
	      spu_arg[i].height = in_pgm.height;
	      spu_arg[i].num_spus = num_spus;
	      spu_arg[i].sourcePGM = intermediary_pixels;
	      spu_arg[i].destBTC = btc.blocks;
	      spu_arg[i].destPGM = out_pgm.pixels;
	      spu_arg[i].spu_id = i;
	      spu_arg[i].padding[0] = (char)mode;

	    /* Creez cate un thread pentru fiecare SPE */
	    if (pthread_create (&spe_threads[i], NULL, &ppu_pthread_function, spu_arg+i)) {
	        perror ("Failed creating thread");
	        exit (1);
	    }
    }

   /* Astept ca thread-urile sa isi incheie executia. */
   for (i=0; i<num_spus; i++) {
      if (pthread_join (spe_threads[i], NULL)) {
         perror("Failed pthread_join");
         exit (1);
      }   
   }
   gettimeofday(&t_final, NULL);
   
   /* scriu rezultatele in fisierele de iesire */
   write_pgm(argv[5], &out_pgm);
   write_btc(argv[4], &btc);

   free_align(out_pgm.pixels);
   free_align(btc.blocks);
   free(in_pgm.pixels);
   free_align(intermediary_pixels);

   total_time = GET_TIME_DELTA(t_init, t_final);

   printf("Time: %lf\n", total_time);

   
   return 0;
}
