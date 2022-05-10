// 200 × 200, and with 200 bodies moving for a fixed number of iterations.
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>


#define         X_RESN  200       /* x resolution */
#define         Y_RESN  200       /* y resolution */
#define			ITERATION 1000 	/* number of iteration */

// point: x,y,mass; velocity; F
typedef struct tagBody{
    double x = 0;
    double y = 0;
    double mass = 0;
}Body;

typedef struct tagForce{
    double x = 0;
    double y = 0;
}Force;

typedef struct tagVelocity{
    double x = 0;
    double y = 0;
}Velocity;

void generate_bodies(Body * bodylist, int n);
Force cal_force_two(Body b1, Body b2);

// let time interval to be 0.01
int main(int argc, char *argv[]){
    int i,j,t;
    Window          win;                            /* initialization for a window */
	unsigned
	int             width, height,                  /* window size */
					x, y,                           /* window position */
					border_width,                   /*border width in pixels */
					display_width, display_height,  /* size of screen */
					screen;                         /* which screen */
	char            *window_name = (char*)"N Body Simulation", *display_name = NULL;
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
    width = X_RESN;
    height = Y_RESN;
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

    int num_bodies = argv[1] ? atoi(argv[1]):200;
    Body bodylist[num_bodies];
    generate_bodies(bodylist, num_bodies);
    Body newblist[num_bodies];
    Velocity vellist[num_bodies];
    Velocity newvlist[num_bodies];
    Body bodyme;


    timeval start,end;
    gettimeofday(&start,NULL);

    for (i = 0; i < num_bodies; i++) {
        XDrawPoint (display, win, gc, bodylist[i].y, bodylist[i].x);
        usleep(1);
    }
    XFlush (display);

    for (t = 0; t < ITERATION; t++) {
        for (i = 0; i < num_bodies; i++) {
            bodyme = bodylist[i];
            Force F;
            for(j = 0; j < num_bodies; j++){
        			if(i == j) continue;
                    Body bodyhe = bodylist[j];
        			Force tempF = cal_force_two(bodyme,bodyhe);
        			F.x += tempF.x;
        			F.y += tempF.y;
        	}

            bodyme = bodylist[i];
            newvlist[i].x = vellist[i].x + F.x*0.1/bodyme.mass;
            newvlist[i].y = vellist[i].y + F.y*0.1/bodyme.mass;

            newblist[i].x = bodyme.x + newvlist[i].x*0.1;
            newblist[i].y = bodyme.y + newvlist[i].y*0.01;

            if(newblist[i].x >= X_RESN){
                newblist[i].x = X_RESN - fmod(newblist[i].x,X_RESN);
                newvlist[i].x = -newvlist[i].x;
            } else if(newblist[i].x < 0){
                newblist[i].x = fmod((-newblist[i].x),X_RESN);
                newvlist[i].x = -newvlist[i].x;
            }
            if(newblist[i].y >= Y_RESN){
                newblist[i].y = Y_RESN - fmod(newblist[i].y,Y_RESN);
                newvlist[i].y = -newvlist[i].y;
            } else if(newblist[i].y < 0){
                newblist[i].y = fmod((-newblist[i].y),Y_RESN);
                newvlist[i].y = -newvlist[i].y;
            }
        }
        XClearWindow(display, win);
        for (i = 0; i < num_bodies; i++) {
            bodylist[i].x = newblist[i].x;
            bodylist[i].y = newblist[i].y;
            vellist[i].x = newvlist[i].x;
            vellist[i].y = newvlist[i].y;
            XDrawPoint (display, win, gc, bodylist[i].y, bodylist[i].x);
            usleep(1);
        }
        XFlush (display);
    }
    gettimeofday(&end,NULL);
    double Time = (1000000. * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec)/1000000;
    printf("Sequential: Number bodies: %d; Runtime: %f\n", num_bodies, Time);
}

// x,y,mass are all random integers at initial state
// mass ranges from 1 to 20
void generate_bodies(Body * bodylist, int n) {
    srand(time(NULL));
    for (int i = 0; i < n;i++){
    	bodylist[i].x = rand() % (X_RESN - 1)+1;
    	bodylist[i].y = rand() % (Y_RESN - 1)+1;
        // bodylist[i].mass = 5;
    	bodylist[i].mass = (rand()%15+1);
    }
}
Force cal_force_two(Body b1, Body b2) {
    Force resultF;
    double r_sq = pow(b1.x-b2.x, 2)+pow(b1.y-b2.y, 2);
    if (r_sq==0) r_sq=0.01;
    double a = (((6.674*0.1)*b1.mass*b2.mass)/r_sq)*(b2.x-b1.x)/sqrt(r_sq);
    double b = (((6.674*0.1)*b1.mass*b2.mass)/r_sq)*(b2.y-b1.y)/sqrt(r_sq);
    resultF.x = a;
    resultF.y = b;
    return resultF;
}

