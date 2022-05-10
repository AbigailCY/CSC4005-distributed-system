/* Sequential Mandelbrot program */
#include "mpi.h"
#include <cstdio>
#include <cstdlib>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <string.h>
#include <math.h>
#include <ctime>

#define	MASTER	0

typedef struct complextype
        {
        float real, imag;
        } Compl;

// void slave(int block);

int main(int argc, char *argv[]) {
	
	double start, end;
    int ntasks;                            /* total number of tasks in partition */
    int rank;                              /* task identifier */
    int len;                               /* length of hostname */
    char hostname[MPI_MAX_PROCESSOR_NAME]; /* hostname */
    MPI_Init(&argc, &argv);
    /* how many processes */
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    /* who am I */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(hostname, &len);

	int width = argv[1] ? atoi(argv[1]) : 200;
	int height = argv[2] ? atoi(argv[2]) : 200;
	int block = argv[3] ? atoi(argv[3]) : 1;
	/* Check if the block is valid */
	if (block * ntasks > width || block < 1) {
		printf ("Input block size is invalid -- should be >= 1 and <= %d.",  width/ntasks);
		exit (-1);
	}

	int * columns = (int *) malloc(sizeof(int) * block);
	int * results;

	if (rank == MASTER) {
		/* 
		Begin MPI distributing
		Calculate and draw points
		Value of column: 1 to width
		*/
		
		int num_running = 0;
		int column = 1;
		int i, j, pos;
		int * global_results = (int *) malloc(sizeof(int) * width * height);
		results = (int *) malloc(sizeof(int) * (height + 1) * block);
		MPI_Status status;
		start = MPI_Wtime();

		for (i = 1; i < ntasks; i ++) {
			for (j = 0; j < block; j++) {
				columns[j] = column;
				column+=1;
			}
			MPI_Send(columns, block, MPI_INT, i, 1, MPI_COMM_WORLD);
			num_running+=1;
		}

		while (num_running > 0) {
			MPI_Recv(results, block*(height+1), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			// printf("running: %d column: %d recv_ source: %d \n", num_running, column, status.MPI_SOURCE);
			num_running -=1;

			if (column <= width) {
				for (j = 0; j < block; j++) {
					if (column <= width ) {columns[j] = column;}
					else {columns[j] = 0;}
					column+=1;
				}
				MPI_Send(columns, block, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
				num_running+=1;
			} else {
				MPI_Send(columns, block, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD);
			}

			for(i=0; i < block; i++) {
				pos = i * (height+1);
				if (results[pos] == 0) continue;
				for (j=1; j <= height; j++) {
					if (results[pos+j] == 1)  {
						global_results[(results[pos]-1)*height+(j-1)] = 1;
						// XDrawPoint (display, win, gc, j-1, results[pos]-1);
					}
				}
			} 
		}
		end = MPI_Wtime();
		printf("MPI dynamic: Number of processes: %d; Display width: %d; Blocksize: %d; Runtime: %f\n", ntasks, width, block, end-start);


		Window          win;                            /* initialization for a window */
		unsigned
		// int             width, height,                  /* window size */
		int				x, y,                           /* window position */
						border_width,                   /*border width in pixels */
						display_width, display_height,  /* size of screen */
						screen;                         /* which screen */

		char            *window_name = (char*)"Mandelbrot Set", *display_name = NULL;
		GC              gc;
		unsigned
		long            valuemask = 0;
		XGCValues       values;
		Display         *display;
		XSizeHints      size_hints;
		Pixmap          bitmap;
		XPoint          points[800];
		FILE            *fp, *fopen ();
		char            str[100];
		
		XSetWindowAttributes attr[1];

		/* connect to Xserver */
		if (  (display = XOpenDisplay (display_name)) == NULL ) {
			fprintf (stderr, "drawon: cannot connect to X server %s\n",
								XDisplayName (display_name) );
			exit (-1);
		}

		/* get screen size */
		screen = DefaultScreen (display);
		display_width = DisplayWidth (display, screen);
		display_height = DisplayHeight (display, screen);

		/* set window position */
		x = 0;
		y = 0;

		/* create opaque window */
		border_width = 4;
		win = XCreateSimpleWindow (display, RootWindow (display, screen),
								x, y, width, height, border_width, 
								BlackPixel (display, screen), WhitePixel (display, screen));

		size_hints.flags = USPosition|USSize;
		size_hints.x = x;
		size_hints.y = y;
		size_hints.width = width;
		size_hints.height = height;
		size_hints.min_width = 150;
		size_hints.min_height = 150;
		
		XSetNormalHints (display, win, &size_hints);
		XStoreName(display, win, window_name);

		/* create graphics context */
		gc = XCreateGC (display, win, valuemask, &values);

		XSetBackground (display, gc, WhitePixel (display, screen));
		XSetForeground (display, gc, BlackPixel (display, screen));
		XSetLineAttributes (display, gc, 1, LineSolid, CapRound, JoinRound);

		attr[0].backing_store = Always;
		attr[0].backing_planes = 1;
		attr[0].backing_pixel = BlackPixel(display, screen);

		XChangeWindowAttributes(display, win, CWBackingStore | CWBackingPlanes | CWBackingPixel, attr);

		XMapWindow (display, win);
		XSync(display, 0);
		

		for (i=0;i<width;i++){
			for (j=0;j<height;j++) {
				if (global_results[i*height+j]==1) {
					XDrawPoint (display, win, gc, j, i);
					usleep(0.1);
				}
			}
		}
		XFlush (display);
		sleep(3);
		free(global_results);
		

	} else {
		/* Mandlebrot variables */
		Compl   z, c;
		float   lengthsq, temp;
		int 	pos, i, j, k, rank;
		MPI_Status status;

		MPI_Recv(columns, block, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		while (status.MPI_TAG == 1) {
			results = (int *) malloc(sizeof(int) * (height + 1) * block);
			for(i=0; i < block; i++) {
				pos = i * (height+1);
				results[pos] = columns[i];
				if (columns[i] == 0) continue;
				c.imag = (float) ( columns[i]-1 - width/2)/width*4;
				for (j=1; j<=height; j++) {
					z.real = z.imag = 0.0;
					c.real = (float)( j-1 - height/2)/height*4;               /* scale factors for 200 x 200 window */
					k = 0;

					do  {                                             /* iterate for pixel color */
					temp = z.real*z.real - z.imag*z.imag + c.real;
					z.imag = 2.0*z.real*z.imag + c.imag;
					z.real = temp;
					lengthsq = z.real*z.real+z.imag*z.imag;
					k++;
					} while (lengthsq < 4.0 && k < 100);
					if (k == 100) results[pos+j] = 1;
					// if (k == 100) XDrawPoint (display, win, gc, j, i);
				}
			}
			MPI_Send(results, block * (height+1), MPI_INT, MASTER, 0, MPI_COMM_WORLD);
			MPI_Recv(columns, block, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		}
	}
    
	MPI_Finalize();
	/* Program Finished */
	free(results);
	free(columns);
	return -1;

}