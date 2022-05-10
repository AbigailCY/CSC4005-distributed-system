/* mpi version heat simulation */
#include "mpi.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include<queue>
#include<time.h> 
#include<math.h>


#define 		MASTER      0
#define 		ITERATION   500
#define			PRECISION   0.01

void init_map(double * map, int n);
void set_color(XColor * color);

int main (int argc, char *argv[]){
    timeval start,end;
    gettimeofday(&start,NULL);
    int n = argv[1] ? atoi(argv[1]):200;

	MPI_Init(&argc, &argv);
    int num_tasks;
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    int myrank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    int len;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(hostname, &len);

    int block = n/num_tasks;
    int start_col = myrank*block;
    int end_col = block*(myrank+1)-1;
    if (myrank == num_tasks-1) {
        end_col = n-1;
    }

    double* map = (double *)malloc(n * n * sizeof(double));
    double* map_new = (double *)malloc(n * n * sizeof(double));
    init_map(map, n);
    init_map(map_new, n);
    double* map_local = (double *)malloc(block * n * sizeof(double));
    // double map_local[block][n] = {0};
    // bool global_cont = false;

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

	MPI_Status status;
    int iter;

	for (iter = 0; iter < ITERATION; iter++) {
        // bool cont = false;
        for(int i = start_col; i <= end_col; i++) {
            if(i == 0 || i == n - 1) continue;
            for(int j = 1; j < n - 1; j++){
                if(map[i*n + j] == 100) continue;
                map_new[i*n + j] = 0.25 * (map[(i-1)*n + j] + map[i*n + j-1] + map[(i+1)*n + j] + map[i*n + j+1]);
                // if (cont == false && abs(map[i*n + j] - map_new[i*n + j])>PRECISION) cont = true;
            }
        }
        for(int i = start_col; i <= end_col; i++){
            for(int j = 1; j < n - 1; j++){
                map[i*n + j] = map_new[i*n + j];
            }
        }
        // message sending and receiving 
        if(myrank > 0){
            MPI_Send(&map[start_col*n],n,MPI_DOUBLE,myrank - 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&map[(start_col-1)*n],n,MPI_DOUBLE,myrank - 1, 1, MPI_COMM_WORLD,&status);
        }
        if(myrank < num_tasks - 1){  //send to right and receive from right
            MPI_Send(&map[end_col*n], n, MPI_DOUBLE, myrank + 1, 1, MPI_COMM_WORLD);
            MPI_Recv(&map[(end_col+1) * n],n,MPI_DOUBLE,myrank + 1,0,MPI_COMM_WORLD,&status);
        }

        // MPI_Barrier(MPI_COMM_WORLD);
        // MPI_Allreduce(&cont, &global_cont, 1, MPI_C_BOOL, MPI_LOR, MPI_COMM_WORLD);
        
        // if(iter%10 == 0){
        //     memcpy(map_local,&map[start_col*n],sizeof(double)*block*n);
        //     MPI_Gather(map_local,block*n,MPI_DOUBLE,map,block*n,MPI_DOUBLE,MASTER,MPI_COMM_WORLD);
        //     if (n % num_tasks > 1 && myrank == num_tasks-1) {
        //         MPI_Send(&map[(block*num_tasks)*n], (n%num_tasks-1)*n, MPI_DOUBLE, MASTER, 2, MPI_COMM_WORLD);
        //     }
        //     if (myrank == MASTER) {
        //         if (n % num_tasks > 1) {
        //             MPI_Recv(&map[block*num_tasks*n], (n%num_tasks-1)*n, MPI_DOUBLE, num_tasks-1, 2, MPI_COMM_WORLD,&status);
        //         }
        //         // draw
        //         // XClearWindow(display, win);
        //         // for(int i = 0; i < n;i++){
        //         //     for(int j = 0; j < n;j++){
        //         //         mycolor = int(map[i*n+j]/5);
        //         //         XDrawPoint (display, win, gc_color[mycolor], j, i);
        //         //         usleep(1);
        //         //     }
        //         // }
        //         // XFlush (display);
        //     }
        // }
        // if (!global_cont) break;
	}

	MPI_Barrier(MPI_COMM_WORLD);
    if(myrank == MASTER){
        gettimeofday(&end,NULL);
        double Time = (1000000. * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec)/1000000;
        // printf("Name: Chen Yuan\nStudent ID: 117010038\nAssignment 4: Heat distribution\n");
        // printf("MPI: plot size: %d; Runtime: %f;\n", n, Time);
        printf("%d %d %f\n",num_tasks, n, Time);

        // for (int i = 0; i < 10; i++) {
        //     for (int j = 0; j < n; j++) {
        //         printf("%f ",map[i*n+j] );
        //     }
        //     printf("\n");
        // }
    }

    MPI_Finalize();
    return 0;

}

void init_map(double *map, int n){
    int i, j;
    // wall
    for (i = 0; i < n; i++) {
        map[i*n+0] = 20;
        map[i*n+n-1] = 20;
        map[0*n+i] = 20;
        map[(n-1)*n+i] = 20;
        if (i != 0 && i != n-1) {
            for (j = 1; j < n-1; j++){
                map[i*n+j] = 0;
            }
        }
    }
    // fireplace: width: n/3; hight: n/5
    for (i = 1*n/3; i < 2*n/3; i++) {
        for (j = 0; j < n/5; j++) {
            map[j*n+i] = 100;
        }
    }
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