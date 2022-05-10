/* Sequential Mandelbrot program */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

// #define         X_RESN  6400       /* x resolution */
// #define         Y_RESN  6400       /* y resolution */

typedef struct complextype
        {
        float real, imag;
        } Compl;


int main(int argc, char *argv[]) {
	int width = argv[1] ? atoi(argv[1]) : 200;
	int height = argv[2] ? atoi(argv[2]) : 200;

	time_t start, end;

	/* Mandlebrot variables */
	int i, j, k;
	Compl   z, c;
	float   lengthsq, temp;
	
	int * result = (int *) malloc(sizeof(int) * width * height);
		
	start = clock();
	/* Calculate and draw points */

	for(i=0; i < width; i++) 
	for(j=0; j < height; j++) {

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

	if (k == 100) result[i*height+j] = 1;
	// XDrawPoint (display, win, gc, j, i);
	}
	end = clock();
	double Time = double(end - start)/CLOCKS_PER_SEC;
	// printf("Name: Chen Yuan\nStudnetID:%d\nAssignment 2, Mandelbrot Set, Sequential implementation\n", 117010038);
	printf("Seq: Display width %d; Runtime: %f\n",width,Time);


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

	
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			if (result[i*height+j] == 1) XDrawPoint (display, win, gc, j, i);
			usleep(0.1);
		}
	}
	
	
	XFlush (display);
	sleep (3);
	free(result);

	/* Program Finished */
	return -1;

}