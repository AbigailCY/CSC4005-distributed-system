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
#define			ITERATION 100 	/* number of iteration */

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

    int num_bodies = argv[1] ? atoi(argv[1]):200;
    Body bodylist[num_bodies];
    generate_bodies(bodylist, num_bodies);
    Body newblist[num_bodies];
    Velocity vellist[num_bodies];
    Velocity newvlist[num_bodies];
    Force F[num_bodies];
    Force tempF;
    Body bodyme;

    timeval start,end;
    gettimeofday(&start,NULL);

    for (t = 0; t < ITERATION; t++) {
        for (i = 0; i < num_bodies; i++) {
            bodyme = bodylist[i];

            for (j=i+1;j< num_bodies;j++) {
                    tempF = cal_force_two(bodyme,bodylist[j]);
                    F[i].x += tempF.x;
                    F[i].y += tempF.y;
                    F[j].x -= tempF.x;
                    F[j].y -= tempF.y;
            }
        }

        for (i = 0; i < num_bodies; i++) {
            bodyme = bodylist[i];
            newvlist[i].x = vellist[i].x + F[i].x*0.01/bodyme.mass;
            newvlist[i].y = vellist[i].y + F[i].y*0.01/bodyme.mass;

            newblist[i].x = bodyme.x + newvlist[i].x*0.01;
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
        for (i = 0; i < num_bodies; i++) {
            bodylist[i].x = newblist[i].x;
            bodylist[i].y = newblist[i].y;
            vellist[i].x = newvlist[i].x;
            vellist[i].y = newvlist[i].y;
        }
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
    	bodylist[i].x = rand() % (X_RESN + 1);
    	bodylist[i].y = rand() % (Y_RESN + 1);
    	bodylist[i].mass = rand() % (20)+1;
    }
}
Force cal_force_two(Body b1, Body b2) {
    Force resultF;
    double r_sq = pow(b1.x-b2.x, 2)+pow(b1.y-b2.y, 2);
    if (r_sq==0)return resultF;
    resultF.x = (6.674*pow(10,-11)*b1.mass*b2.mass/r_sq)*(b2.x-b1.x)/sqrt(r_sq);
    resultF.y = (6.674*pow(10,-11)*b1.mass*b2.mass/r_sq)*(b2.y-b1.y)/sqrt(r_sq);
    return resultF;
}

