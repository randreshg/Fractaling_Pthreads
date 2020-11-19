
/***************************LIBRARY DEFINITION**********************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include "tpool.h"
#include <sys/time.h>

/**************************COMPILATION INSTRUCTION*****************************/
/*
1) Create object:
	gcc -pthread tpool.c xmem.c fractaling_pthreads.c -o fractaling
2) Run object<
	./fractaling ScreenResolution NumItr NumThreads
	Example: ./fractaling 1024 10000 3
*/
/******************************GLOBAL VARIABLES*******************************/
int **fractaling;
int MAX_ITR = 100, NUM_THRDS = 1;
float deltax, deltay;
int XMAX = 520, YMAX = 520;
float CX0 = -1.0;	//Initial x coordinate
float CY0 = -1.0;	//Initial y coordinate
float CYN = 1.0;	//Maximum y coordinate
float CXN = 1.0;	//Maximum x coordinate

/*******************************FUNCTIONS*************************************/
void calc_color(void *arg);
void parseArguments(int argc, char *argv[]);
/*****************************MAIN FUNCTION***********************************/
int main(int argc, char *argv[])
{
	parseArguments(argc, argv);
	printf("%i %f %f %f %f %i\n", YMAX, CX0, CXN, CY0, CYN, MAX_ITR);	
	//Initiate the time	
	struct timeval t0, t1, dt;
    gettimeofday(&t0, NULL);
	//Auxiliar variables
	deltax = (CXN - CX0) / XMAX;
	deltay = (CYN - CY0) / YMAX;	
	int i, j;
	//Dynamic memory to matrix
	fractaling = (int **)malloc(XMAX *sizeof(int*)); 
	for (i = 0; i < XMAX; ++i)
    	*(fractaling + i) = (int *)malloc(YMAX *sizeof(int));
	
	//Create threads 
	tpool_t *tm;
	tm = tpool_create(NUM_THRDS);
	for (j = 0; j < YMAX; ++j)			
		tpool_add_work(tm, calc_color, (void *)j);
	
	//Wait for work finished
	tpool_wait(tm);
	tpool_destroy(tm);
	//Calculate time taken
	gettimeofday(&t1, NULL);
    timersub(&t1, &t0, &dt);
	fprintf(stderr, "The process took %ld.%06ld seconds to execute\n", dt.tv_sec, dt.tv_usec);
	
	// Create a file which have the image in format ppm and write the result
	FILE *fp = fopen("fractaling.ppm", "w+");
	fprintf(fp, "P2\n%d %d\n%d\n", XMAX, YMAX, 255);
	for(j = 0; j < YMAX; ++j)
		for(i = 0; i < XMAX; ++i)		
			fprintf(fp, "%i ", *(*(fractaling + i) + j));
	free(fractaling);
	return 0;
}

/*******************************FUNCTIONS*************************************/
void calc_color(void *arg)
{
	//Calculate if the point belongs to Malderbort's set
	int itr, color, i, j = (int)arg;
	float cx, cy = CYN-j*deltay;
	double x, y, xx, yy, xy;	 	
	for (i = 0; i < XMAX; ++i)
	{
		cx = CX0 +i*deltax;				
		x = 0, y = 0, xx = 0, yy = 0;
		for(itr = 1; (itr < MAX_ITR) && ((xx + yy) < 4.0); itr++)
		{
			xx = x * x;
			yy = y * y;
			xy = x * y;
			x = xx - yy + cx;
			y = 2*xy + cy;
		}		
		//Assign color to the pixel
		if (itr == MAX_ITR)		
			color = 10;				
		else
			color = 255;		
		*(*(fractaling + i) + j) = color;
	}
}

void parseArguments(int argc, char *argv[])
{	
    if(argc > 1)
    {
		XMAX = atoi(argv[1]);
		YMAX = XMAX;		
        if(argc>2)
		{
			MAX_ITR = atoi(argv[2]);
			if(argc>3)
				NUM_THRDS = atoi(argv[3]);
		}        	
		
    }    
}
