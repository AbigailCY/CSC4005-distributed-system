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

	int block = width/(ntasks-1);
	int num_cols = block;
	int i,j,k;

	if (rank == MASTER) {
		double start, end;
		
		MPI_Status status;
		int *global_result = (int *) malloc(sizeof(int) * width * height);
		start = MPI_Wtime();
		
		for (k = 1; k <= ntasks-1; k ++) {
			
			if (k == ntasks-1) {
				num_cols = block + width%(ntasks-1);
			}
			MPI_Recv(&global_result[(k-1)*block*height], num_cols*height, MPI_INT, k, 0, MPI_COMM_WORLD, &status);
		}

		end = MPI_Wtime();
		printf("MPI static: Number of processes: %d; Displaywidth: %d; Runtime: %f\n", ntasks, width, end-start);

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
				if (global_result[i*height+j]==1) {
					XDrawPoint (display, win, gc, j, i);
					usleep(0.1);
				}
			}
		}
		XFlush (display);
		sleep(3);
		free(global_result);
		

	} else {
		int start_col = block*(rank-1);
		int end_col = block*rank-1;
		if (rank == ntasks-1) end_col = width-1;
		num_cols = end_col - start_col + 1;
		int *result = (int *) malloc(sizeof(int) * num_cols * height);

		/* Mandlebrot variables */
		Compl   z, c;
		float   lengthsq, temp;

		//printf("start_index is %d\n",start_index);
		for (i=start_col;i<=end_col;i++){
			for (j = 0; j < height; j++) {
				z.real = z.imag = 0.0;
				c.real = (float) (j - height/2)/height*4;               /* scale factors for 800 x 800 window */
				c.imag = (float) (i - width/2)/width*4;
				k = 0;
				do  {                                             /* iterate for pixel color */
				temp = z.real*z.real - z.imag*z.imag + c.real;
				z.imag = 2.0*z.real*z.imag + c.imag;
				z.real = temp;
				lengthsq = z.real*z.real+z.imag*z.imag;
				k++;
				} while (lengthsq < 4.0 && k < 100);
				if (k == 100) {
					result[(i-start_col)*height+j] = 1;
				}
			}
		}
		MPI_Send(result, height * num_cols, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
		free(result);
	}
    
	MPI_Finalize();
	/* Program Finished */
	
	return -1;

}