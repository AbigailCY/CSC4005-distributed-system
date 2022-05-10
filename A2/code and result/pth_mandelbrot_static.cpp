/* Sequential Mandelbrot program */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

// #define	X_RESN  200       /* x resolution */
// #define	Y_RESN  200       /* y resolution */
// #define	NUM_THREADS  2



typedef struct _thread_data{
	int thread_id;
	int width;
	int height;
	int num_thd;
	int *result;
}thread_data;

typedef struct complextype
        {
        float real, imag;
        } Compl;

// int * result = (int *) malloc(sizeof(int) * X_RESN * Y_RESN);

//thread function
void *cal_func(void *arg) {
	thread_data *input_data = (thread_data *)arg; 
	int height = input_data->height;
	int width = input_data->width;
	int thread_id=input_data->thread_id;
	int num_thd = input_data->num_thd;
	int *result = input_data->result;
	int block = width/num_thd;
	int start_col = block*thread_id;
	int end_col = block*(thread_id+1)-1;

	if (thread_id == num_thd-1) end_col = width-1;
	/* Mandlebrot variables */
	int i, j, k;
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
				input_data->result[i*height+j] = 1;
			}
		}
    }

	pthread_exit(NULL);
	//return local data to the main thread
}

int main(int argc, char *argv[]) {
	int nthreads = argv[1] ? atoi(argv[1]) : 1;
	int width = argv[2] ? atoi(argv[2]) : 200;
	int height = argv[3] ? atoi(argv[3]) : 200;

	int * result = (int *) malloc(sizeof(int) * width * height);
	pthread_t thread[nthreads];
	int i,j,rc;
	thread_data input_data[nthreads];
		
	timeval start,end;
	gettimeofday(&start, 0);
	//create threads
	for(i=0; i<nthreads; ++i) {
		input_data[i].thread_id=i;
		input_data[i].height = height;
		input_data[i].width = width;
		input_data[i].num_thd = nthreads;
		input_data[i].result = result;

		rc = pthread_create(&thread[i],NULL,cal_func,&input_data[i]);
        if (rc) // if rc is not 0, the thread cannot be created
		{
			fprintf(stderr,"error:pthread_create,rc: %d\n",rc);
			return EXIT_FAILURE;
		}
	}	
	for(i=0; i<nthreads; ++i) {
		pthread_join(thread[i],NULL); 
	}
	gettimeofday(&end, 0);
	double Time = (1000000. * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec)/1000000;
	printf("Pthread static: Threads: %d; Display width: %d; runtime: %f\n", nthreads, width, Time);
	

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
	size_hints.min_width = 200;
	size_hints.min_height = 200;
	
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
			if (result[i*height+j]==1) {
				XDrawPoint (display, win, gc, j, i);
				usleep(0.1);
			}
		}
	}

	free(result);
	XFlush (display);
	sleep (3);

	/* Program Finished */
	return -1;

}