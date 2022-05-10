#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include<time.h> 
#include <math.h>

#define         X_RESN  200       /* x resolution */
#define         Y_RESN  200       /* y resolution */
#define			ITERATION 100 	/* number of iteration */
#define         NUM_BODIES 200

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

typedef struct _thread_data{
	int thread_id;
}thread_data;

int NUM_THREADS;
Body bodylist[NUM_BODIES];
Body newblist[NUM_BODIES];
Velocity vellist[NUM_BODIES];
Velocity newvlist[NUM_BODIES];
pthread_barrier_t barrier;
int global = 0;

void generate_bodies(Body * bodylist);
Force cal_force_two(Body b1, Body b2);


void *cal_func(void *arg) {
    int t,i,j;
    Body bodyme;
    Force tempF;
    thread_data *input_data = (thread_data *)arg; 
    int thread_id=input_data->thread_id;
    int block = NUM_BODIES/NUM_THREADS;
    int startb = block*thread_id;
    int endb = block*(thread_id+1)-1;
    if (thread_id == NUM_THREADS-1) {
        endb = NUM_BODIES-1;
        block = NUM_BODIES-startb;
    }

    for (t = 0; t < ITERATION; t++) {
        for (i = startb; i <= endb; i++) {
            bodyme.x = bodylist[i].x;
            bodyme.y = bodylist[i].y;
            bodyme.mass = bodylist[i].mass;
            Force F;
            // calculate_force_routine
            for (j=0;j< NUM_BODIES;j++) {
                if (i!=j) {
                    tempF = cal_force_two(bodyme,bodylist[j]);
                    F.x += tempF.x;
                    F.y += tempF.y;
                }
            }
            newvlist[i].x = vellist[i].x + F.x*0.01/bodyme.mass;
            newvlist[i].y = vellist[i].y + F.y*0.01/bodyme.mass;

            newblist[i].x = bodyme.x + newvlist[i].x*0.01;
            newblist[i].y = bodyme.y + newvlist[i].y*0.01;

            if(newblist[i].x >= X_RESN){
                newblist[i].x = X_RESN - fmod(newblist[i].x,X_RESN);
                newvlist[i].x = -newvlist[i].x;
            } else if(newblist[i].x <= 0){
                newblist[i].x = fmod((-newblist[i].x),X_RESN);
                newvlist[i].x = -newvlist[i].x;
            }
            if(newblist[i].y >= Y_RESN){
                newblist[i].y = Y_RESN - fmod(newblist[i].y,Y_RESN);
                newvlist[i].y = -newvlist[i].y;
            } else if(newblist[i].y <= 0){
                newblist[i].y = fmod((-newblist[i].y),Y_RESN);
                newvlist[i].y = -newvlist[i].y;
            }
        }
        pthread_barrier_wait(&barrier);

        for (i = startb; i <= endb; i++) {
            bodylist[i].x = newblist[i].x;
            bodylist[i].y = newblist[i].y;
            vellist[i].x = newvlist[i].x;
            vellist[i].y = newvlist[i].y;
        }
        pthread_barrier_wait(&barrier);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    NUM_THREADS = argv[1] ? atoi(argv[1]):1;
    generate_bodies(bodylist);
    int i,rc;
		
	timeval start,end;
	gettimeofday(&start, 0);
    pthread_t thread[NUM_THREADS];
	thread_data input_data[NUM_THREADS];
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);

    for(i=0; i<NUM_THREADS; i++) {
        input_data[i].thread_id=i;
        rc = pthread_create(&thread[i],NULL,cal_func,&input_data[i]);
        if (rc) // if rc is not 0, the thread cannot be created
        {
            fprintf(stderr,"error:pthread_create,rc: %d\n",rc);
            return EXIT_FAILURE;
        }
    }

    for(i=0; i<NUM_THREADS; i++) {
        void *return_data;
        pthread_join(thread[i],&return_data); 
    }
    
    gettimeofday(&end, 0);

    double Time = (1000000. * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec)/1000000.;
    printf("Pthread: Threads: %d; Body numbers: %d; Runtime: %f\n", NUM_THREADS, NUM_BODIES, Time);
    return -1;
}

void generate_bodies(Body * bodylist) {
    srand(time(NULL));
    for (int i = 0; i < NUM_BODIES;i++){
    	bodylist[i].x = rand() % (X_RESN + 1);
    	bodylist[i].y = rand() % (Y_RESN + 1);
    	bodylist[i].mass = rand() % 20 + 1;
    }
}
Force cal_force_two(Body b1, Body b2) {
    Force resultF;
    double r_sq = pow(b1.x-b2.x, 2)+pow(b1.y-b2.y, 2);
    if (r_sq==0) r_sq = 0.1;
    resultF.x = (6.674*pow(10,-11)*b1.mass*b2.mass/r_sq)*(b2.x-b1.x)/sqrt(r_sq);
    resultF.y = (6.674*pow(10,-11)*b1.mass*b2.mass/r_sq)*(b2.y-b1.y)/sqrt(r_sq);
    return resultF;
}