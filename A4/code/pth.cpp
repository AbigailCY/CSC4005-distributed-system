#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <time.h> 
#include <math.h>

#define			PRECISION  0.01
#define 		ITERATION 500

typedef struct _thread_data{
	int thread_id;
}thread_data;

double** create_map(int n);
void init_map(double ** map, int n);
void free_map(double ** map, int n);
void set_color(XColor * color);

int NUM_THREADS;
pthread_barrier_t barrier_s;
pthread_barrier_t barrier_m;
// pthread_mutex_t lock1;
int n;
double **map;
double **map_new;
bool cont = false;

void *cal_func(void *arg) {
    int i,j;
    int iter = 0;

    thread_data *input_data = (thread_data *)arg; 
    int thread_id=input_data->thread_id;
    int block = n/NUM_THREADS;
    int start_col = block*thread_id;
    int end_col = block*(thread_id+1)-1;
    if (thread_id == NUM_THREADS-1) {
        end_col = n-2;
        block = n-1-start_col;
    } else if (thread_id == 0) {
        start_col = 1;
        block -= 1;
    }

    do
    {
        for (j = start_col; j <= end_col; j++) {
            for (i = 1; i < n-1; i++) {
                if (map[i][j] == 100) continue;
                // pthread_mutex_lock(&lock1);
                map_new[i][j] = 0.25*(map[i-1][j]+map[i+1][j]+map[i][j-1]+map[i][j+1]);
                // pthread_mutex_unlock(&lock1);
                if (cont == false && abs(map_new[i][j]-map[i][j]) > PRECISION) {
                    cont = true;
                }
            }
        }
        pthread_barrier_wait(&barrier_s);
        

        for (j = start_col; j <= end_col; j++) {
            for (i = 1; i < n-1; i++) {
                map[i][j] = map_new[i][j];
            }
        }
        if (cont == false) break;
        pthread_barrier_wait(&barrier_m);
        
        iter ++;
    } while (iter < ITERATION);

}

int main(int argc, char *argv[]) {
    NUM_THREADS = argv[1] ? atoi(argv[1]):1;
    n = argv[2] ? atoi(argv[2]):200;
    int i,rc;

    timeval start,end;
	gettimeofday(&start, 0);
    map = create_map(n);
    map_new = create_map(n);
    init_map(map, n);
    init_map(map_new, n);

    // Window          win;                            /* initialization for a window */
    // unsigned
    // int             width, height,                  /* window size */
    //                 x, y,                           /* window position */
    //                 border_width,                   /*border width in pixels */
    //                 display_width, display_height,  /* size of screen */
    //                 screen;                         /* which screen */

    // char            *window_name = (char*)"Heat Distribution", *display_name = NULL;

    // GC              gc;
    // GC              gc_color[20];
    // unsigned
    // long            valuemask = 0;
    // XGCValues       values;
    // Display         *display;
    // XSizeHints      size_hints;
    // Pixmap          bitmap;
    // XPoint          points[800];
    // FILE            *fp, *fopen ();
    // char            str[100];
    
    // XSetWindowAttributes attr[1];
    
    // /* connect to Xserver */
    // if (  (display = XOpenDisplay (display_name)) == NULL ) {
    //     fprintf (stderr, "drawon: cannot connect to X server %s\n",
    //                         XDisplayName (display_name) );
    // exit (-1);
    // }
        
    // /* get screen size */
    // screen = DefaultScreen (display);
    // display_width = DisplayWidth (display, screen);
    // display_height = DisplayHeight (display, screen);

    // /* set window size */
    // width = n;
    // height = n;

    // /* set window position */
    // x = 0;
    // y = 0;

    // /* create opaque window */
    // border_width = 100;
    // win = XCreateSimpleWindow (display, RootWindow (display, screen),
    //                         x, y, width, height, border_width, 
    //                         BlackPixel (display, screen), WhitePixel (display, screen));

    // size_hints.flags = USPosition|USSize;
    // size_hints.x = x;
    // size_hints.y = y;
    // size_hints.width = width;
    // size_hints.height = height;
    // size_hints.min_width = 300;
    // size_hints.min_height = 300;
    
    // XSetNormalHints (display, win, &size_hints);
    // XStoreName(display, win, window_name);

    // /* create graphics context */

    // gc = XCreateGC (display, win, valuemask, &values);
    // XSetBackground (display, gc, WhitePixel (display, screen));
    // XSetForeground (display, gc, BlackPixel (display, screen));
    // XSetLineAttributes (display, gc, 100, LineSolid, CapRound, JoinRound);

    // attr[0].backing_store = Always;
    // attr[0].backing_planes = 1;
    // attr[0].backing_pixel = BlackPixel(display, screen);
    // XChangeWindowAttributes(display, win, CWBackingStore | CWBackingPlanes | CWBackingPixel, attr);

    // XMapWindow (display, win);
    // XSync(display, 0);

    // /* set the color*/
    // XColor color[21];
    // set_color(color);

    // Status rc1;
    // for(int i = 0; i < 21; i++){
    //     rc1 = XAllocColor(display,DefaultColormap(display, screen),&color[i]);
    // }

    // for(int i = 0; i < 21;i++){
    //     gc_color[i] = XCreateGC (display, win, valuemask, &values);
    //     XSetForeground (display, gc_color[i], color[i].pixel);
    //     XSetBackground (display, gc_color[i], WhitePixel (display, screen));
    //     XSetLineAttributes (display, gc_color[i], 100, LineSolid, CapRound, JoinRound);
    // }

    // int mycolor;
    // for(int i = 0; i < n;i++){
    //     for(int j = 0; j < n;j++){
    //         mycolor = map[i][j]/5;
    //         XDrawPoint (display, win, gc_color[mycolor], j, i);
    //         usleep(2);
    // }
    // }
    // XFlush (display);
    // sleep(1);

    pthread_t thread[NUM_THREADS];
	thread_data input_data[NUM_THREADS];
    pthread_barrier_init(&barrier_s, NULL, NUM_THREADS);
    pthread_barrier_init(&barrier_m, NULL, NUM_THREADS + 1);

    for(i=0; i<NUM_THREADS; i++) {
        input_data[i].thread_id=i;
        rc = pthread_create(&thread[i],NULL,cal_func,&input_data[i]);
        if (rc) // if rc is not 0, the thread cannot be created
        {
            fprintf(stderr,"error:pthread_create,rc: %d\n",rc);
            return EXIT_FAILURE;
        }
    }

    int iter = 0;
    do {
        cont = false;
        pthread_barrier_wait(&barrier_m);
        if (cont == false) break;

        // if (iter %50 == 0) {
        //     /* draw */
        //     XClearWindow(display, win);
        //     for(int i = 0; i < n;i++){
        //         for(int j = 0; j < n;j++){
        //             mycolor = int(map[i][j]/5);
        //             XDrawPoint (display, win, gc_color[mycolor], j, i);
        //             usleep(1);
        //         }
        //     }
        //     XFlush (display);
        // }

        iter ++;
    } while (iter < ITERATION);
    

    for(i=0; i<NUM_THREADS; i++) {
        void *return_data;
        pthread_join(thread[i],&return_data); 
    }
    
    gettimeofday(&end, 0);
    double Time = (1000000. * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec)/1000000;
    // printf("Name: Chen Yuan\nStudent ID: 117010038\nAssignment 4: Heat distribution\n");
    // printf("Pthread: plot size: %d; Runtime: %f;\n", n, Time);
    printf("%d %d %f\n",NUM_THREADS, n, Time);

    return -1;
}

double **create_map(int n) {
	int i;
	double **a;

	a = (double**) malloc(sizeof(double*)*n);
	for (i=0;i<n;i++) {
		a[i] = (double*) malloc(sizeof(double)*n);
	}
	return a;
}

void init_map(double **map, int n){
    int i, j;
    // wall
    for (i = 0; i < n; i++) {
        map[i][0] = 20.;
        map[i][n-1] = 20.;
        map[0][i] = 20;
        map[n-1][i] = 20;
        if (i != 0 && i != n-1) {
        for (j = 1; j < n-1; j++){
            map[i][j] = 0;
        }
        }
    }
    // fireplace: width: n/3; hight: n/5
    for (i = 1*n/3; i < 2*n/3; i++) {
        for (j = 0; j < n/5; j++) {
            map[j][i] = 100.;
        }
    }
}

void free_map(double **a, int n) {
	int i;
	for (i=0;i<n;i++) {
		free(a[i]);
	}
	free(a);
}

void set_color(XColor * color) {

    int B[21] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255,255,
        225, 200, 175, 150, 125, 100, 75, 50, 25, 0};
    int G[21] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int R[21] = {0, 25, 50, 75, 100, 125, 150,175, 200, 225, 
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

    for (int x = 0; x < 21; x++){
        color[x].blue=B[x]*256;
        color[x].green = G[x]*256;
        color[x].red = R[x]*256;
    }

}