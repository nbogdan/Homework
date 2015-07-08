/*
	Tema 3 ASC

	Student:	NICULA Bogdan
	Grupa:		334CB
*/

#include <stdio.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>
#include <libmisc.h>
#include "../serial/btc.h"
#include <math.h>

#define waitag(t) mfc_write_tag_mask(1<<t); mfc_read_tag_status_all();
#define BLOCKSIZE 64


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



int main(unsigned long long speid, unsigned long long argp, unsigned long long envp)
{
	uint32_t tag_id[2], tag_write;

	info_token p __attribute__ ((aligned(16)));
	uint32_t total_blocks, blocks_per_spu, blocks_to_take, current_block;
	uint32_t i, j, mode;
	short int input_buffer[2 * BLOCKSIZE] __attribute__ ((aligned(16))), *input;
	short int results_pgm[BLOCKSIZE] __attribute__ ((aligned(16)));
	short int output[BLOCKSIZE] __attribute__ ((aligned(16)));
    struct block results __attribute__ ((aligned(16)));

    float stddevVect[BLOCKSIZE] __attribute__ ((aligned(16)));
   
    vector signed short *vshort, vshortAux;
    vector float *vfloat, vfloatAux;

    uint32_t sum;
    float avg, stddev, q, m, a, b;


    tag_id[0] = mfc_tag_reserve();
	tag_id[1] = mfc_tag_reserve();
	tag_write = mfc_tag_reserve();
	if (tag_id[0]==MFC_TAG_INVALID){
		printf("SPU: ERROR can't allocate tag ID\n"); return -1;
	}
	if (tag_id[1]==MFC_TAG_INVALID){
		printf("SPU: ERROR can't allocate tag ID\n"); return -1;
	}
	if (tag_id[1]==MFC_TAG_INVALID){
		printf("SPU: ERROR can't allocate tag ID\n"); return -1;
	}

	/* preiau structura cu informatiile referitoare la blocurile ce vor
	fi luate */
	mfc_get(&p, argp, (uint32_t)sizeof(info_token), tag_id[0], 0, 0);
    waitag(tag_id[0]);

    mode = p.padding[0];
    total_blocks = p.width * p.height / BLOCKSIZE;
    blocks_per_spu = total_blocks / p.num_spus + 1;
    current_block = p.spu_id * blocks_per_spu;


    if(current_block >= total_blocks)
    	return 0;


    /* Aleg intre modul simplu(0) si cel de double buffering(1) */
    if(mode == 1)
	{
	    for (i = 0; i < 8; ++i)
		{
			int blockLine = (current_block / (p.width / 8)) * 8;
			int blockColumn = (current_block % (p.width / 8)) * 8;
			mfc_get(((void *)(input_buffer + 8 * i)), (unsigned int)(p.sourcePGM + blockLine * p.width + blockColumn + p.width * i), (uint32_t)8 * sizeof(short int), tag_id[0], 0, 0);
		}
		j = 1;
	}
	else
	{
		mode = 0;
		j = 0;
		
	}

    do
    {	
    	/* preiau blocuri procesare 
			-> pt double buffering voi alterna de la pas la pas intre a pune la inceputul lui input_buffer
			sau la input_buffer + 64 octeti.(respectiv a da waittag pe tag-ul 1 sau 0)
			NOTA: mereu se va da waitag() pe tag-ul aferent transferului efectuat la pasul anterior, nu la pasul curent!
			
			-> pt solutia normala, mode = 0, waitag se da mereu pe tag-ul 0, informatia este mereu adusa la
			inceputul, nu la jumatatea buffer-ului.
    	*/
    	if(current_block + 1 * mode < total_blocks && current_block + 1 * mode < (p.spu_id + 1) * blocks_per_spu)
			for (i = 0; i < 8; ++i)
	    	{
	    		int blockLine = ((current_block + 1 * mode) / (p.width / 8)) * 8;
	    		int blockColumn = ((current_block + 1 * mode) % (p.width / 8)) * 8;
	    		mfc_get(((void *)(input_buffer + 8 * i + j * BLOCKSIZE * mode)), (unsigned int)(p.sourcePGM + blockLine * p.width + blockColumn + p.width * i), (uint32_t)8 * sizeof(short int), tag_id[mode * j], 0, 0);
	    	}
	    	else break;
    	waitag(tag_id[mode*((j + 1) % 2)]);
    	
    	input = input_buffer + BLOCKSIZE * ((j + 1) % 2) * mode;

    	/*----------------------------------*/
    	/* Etapa de comprimare*/
    	/*----------------------------------*/

    	/* Copiez block-ul intr-un vector de float-uri */
    	for (i = 0; i < BLOCKSIZE; i++)
    	{
    		stddevVect[i] = (float) input[i];
    	}
    	vfloat = (vector float *) stddevVect;

    	/* Calculez media aritmetica */
    	vshort = (vector signed short *) input;

    	vshortAux = vshort[0];
    	for (i = 1; i < BLOCKSIZE / 8; i++)
    	{
    		vshortAux = vshortAux + vshort[i];
    	}

    	sum = vshortAux[0];
    	for(i = 1; i < 8; i++)
    		sum += vshortAux[i];

    	avg = (float)sum / BLOCKSIZE;


    	/* Calculez deviatia standard */
    	vfloatAux = spu_splats((float)avg);
    	
    	vfloat[0] = vfloat[0] - vfloatAux;
    	vfloat[0] = vfloat[0] * vfloat[0];
    	for(i = 1; i < BLOCKSIZE / 4; i++)
    	{
    		vfloat[i] = vfloat[i] - vfloatAux;
    		vfloat[i] = vfloat[i] * vfloat[i];
    		vfloat[0] = vfloat[0] + vfloat[i];
    	}

    	stddev = stddevVect[0] + stddevVect[1] + stddevVect[2] + stddevVect[3];
    	stddev = sqrt(stddev / BLOCKSIZE);
    	
    	/* Calculez a, b si masca de biti(bitplane) */
    	q = 0;
    	for(i = 0; i < BLOCKSIZE; i++)
		{	
			float aux = input[i] - avg;
			results.bitplane[i] = 1 - (fabs(aux) - aux) / (2 * fabs(aux));
			q += results.bitplane[i];	
		}
		
    	m = BLOCKSIZE;
    	a = (avg - stddev * sqrt(q/(m-q)));
    	b = (avg + stddev * sqrt((m-q)/q));
    	
    	
    	a = (fabs(a) + a) / 2;
    	b = 255 - (fabs(255 - b) + 255 - b) / 2;
    	

    	results.a = (char) a;
    	results.b = (char) b;
		
    	mfc_put(((void *)(&results)), (unsigned int)(p.destBTC + current_block), (uint32_t)sizeof(struct block), tag_write, 0, 0);
    	

		/*----------------------------------*/
    	/* Etapa de decomprimare*/
    	/*----------------------------------*/
		
		/* refac block-ul */
    	for(i = 0; i < BLOCKSIZE; i++)
    		output[i] = results.bitplane[i] * results.b + (1 - results.bitplane[i]) * results.a;

    	/* scriu block-ul la destinatie */
    	for (i = 0; i < 8; ++i)
    	{
    		int blockLine = (current_block / (p.width / 8)) * 8;
    		int blockColumn = (current_block % (p.width / 8)) * 8;
    		mfc_put(((void *)(output + 8 * i)), (unsigned int)(p.destPGM + blockLine * p.width + blockColumn + p.width * i), (uint32_t)8 * sizeof(short int), tag_write, 0, 0);
    	}

    	j = (j + 1) % 2;
    	current_block++;

    	waitag(tag_write);
    }while(current_block < total_blocks && current_block < (p.spu_id + 1) * blocks_per_spu);
    


	return 0;
}
