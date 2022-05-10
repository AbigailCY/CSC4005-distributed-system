// 200 × 200, and with 200 bodies moving for a fixed number of iterations.
#include "mpi.h"
#include <cstdio>
#include <cstdlib>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <string.h>
#include <math.h>
#include <ctime>

#define 		MASTER 0
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

void generate_bodies(double * body_x, double * body_y, double * body_mass, int n);
Force cal_force_two(Body b1, double x, double y, double mass);

// let time interval to be 0.01
int main(int argc, char *argv[]){
    MPI_Init(&argc, &argv);
    int num_tasks;
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int len;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(hostname, &len);

    int num_bodies = argv[1] ? atoi(argv[1]):200;
    double body_x[num_bodies] = {0};
    double body_y[num_bodies] = {0};
    double body_mass[num_bodies] = {0};
    if(rank == MASTER){
    	generate_bodies(body_x,body_y,body_mass, num_bodies);
	}

    timeval start,end;
    gettimeofday(&start,NULL);
    // all bodies to each process
    MPI_Bcast(body_x,num_bodies,MPI_DOUBLE,MASTER,MPI_COMM_WORLD);
	MPI_Bcast(body_y,num_bodies,MPI_DOUBLE,MASTER,MPI_COMM_WORLD);
	MPI_Bcast(body_mass,num_bodies,MPI_DOUBLE,MASTER,MPI_COMM_WORLD);

    // bodies to compute in this process
    int sub_bodies = num_bodies/num_tasks;
    int first = sub_bodies*rank;
    // int last = first + sub_bodies;
    int remains = num_bodies%num_tasks;
    // for equally distributed bodies in each process
    Velocity vellist[sub_bodies];
    Velocity newvlist[sub_bodies];
    double new_body_x[sub_bodies];
    double new_body_y[sub_bodies];
    // for remaining bodies in MASTER
    Velocity remain_vlist[remains];
    Velocity remain_newvlist[remains];
    double reamin_body_x[remains];
    double reamin_body_y[remains];

    Body bodyme;
    Force tempF;
    int i,j,t;

    
    for (t = 0; t < ITERATION; t++) {
        // begin parallel computing
        for (i = 0; i < sub_bodies; i++) {
            bodyme.x = body_x[i+first];
            bodyme.y = body_y[i+first];
            bodyme.mass = body_mass[i+first];
            Force F;
            // calculate_force_routine
            for (j=0;j< num_bodies;j++) {
                if (i!=j) {
                    tempF = cal_force_two(bodyme,body_x[j], body_y[j], body_mass[j]);
                    F.x += tempF.x;
                    F.y += tempF.y;
                }
            }
            newvlist[i].x = vellist[i].x + F.x*0.01/bodyme.mass;
            newvlist[i].y = vellist[i].y + F.y*0.01/bodyme.mass;

            new_body_x[i] = bodyme.x + newvlist[i].x*0.01;
            new_body_y[i] = bodyme.y + newvlist[i].y*0.01;

            if(new_body_x[i] >= X_RESN){
                new_body_x[i] = X_RESN - fmod(new_body_x[i],X_RESN);
                newvlist[i].x = -newvlist[i].x;
            } else if(new_body_x[i] < 0){
                new_body_x[i] = fmod((-new_body_x[i]),X_RESN);
                newvlist[i].x = -newvlist[i].x;
            }
            if(new_body_y[i] >= Y_RESN){
                new_body_y[i] = Y_RESN - fmod(new_body_y[i],Y_RESN);
                newvlist[i].y = -newvlist[i].y;
            } else if(new_body_y[i] < 0){
                new_body_y[i] = fmod((-new_body_y[i]),Y_RESN);
                newvlist[i].y = -newvlist[i].y;
            }
        }

        for (i = 0; i < sub_bodies; i++) {
            vellist[i] = newvlist[i];
        }

        // in the iteration, account for the remaining bodies in MASTER node
        if (rank == MASTER && remains != 0) {
            for (i = 0; i < remains; i++) {
                bodyme.x = body_x[i+num_bodies-remains];
                bodyme.y = body_y[i+num_bodies-remains];
                bodyme.mass = body_mass[i+num_bodies-remains];
                Force F;
                // calculate_force_routine
                for (j=0;j< num_bodies;j++) {
                    if (i!=j) {
                        tempF = cal_force_two(bodyme,body_x[j], body_y[j], body_mass[j]);
                        F.x += tempF.x;
                        F.y += tempF.y;
                    }
                }
                remain_newvlist[i].x = remain_vlist[i].x + F.x*0.01/bodyme.mass;
                remain_newvlist[i].y = remain_vlist[i].y + F.y*0.01/bodyme.mass;
                reamin_body_x[i] = bodyme.x + remain_newvlist[i].x*0.01;
                reamin_body_x[i] = bodyme.y + remain_newvlist[i].y*0.01;

                if(reamin_body_x[i] >= X_RESN){
                    reamin_body_x[i] = X_RESN - fmod(reamin_body_x[i],X_RESN);
                    remain_newvlist[i].x = -remain_newvlist[i].x;
                } else if(reamin_body_x[i] < 0){
                    reamin_body_x[i] = fmod((-reamin_body_x[i]),X_RESN);
                    remain_newvlist[i].x = -remain_newvlist[i].x;
                }
                if(reamin_body_y[i] >= Y_RESN){
                    reamin_body_y[i] = Y_RESN - fmod(reamin_body_y[i],Y_RESN);
                    remain_newvlist[i].y = -remain_newvlist[i].y;
                } else if(reamin_body_y[i] < 0){
                    reamin_body_y[i] = fmod((-reamin_body_y[i]),Y_RESN);
                    remain_newvlist[i].y = -remain_newvlist[i].y;
                }
            }
            for (i = 0; i < remains; i++) {
                remain_vlist[i] = remain_newvlist[i];
                body_x[i+num_bodies-remains] = reamin_body_x[i];
                body_y[i+num_bodies-remains] = reamin_body_y[i];
            }
        }
        // begin synchronizing data
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgather(new_body_x,sub_bodies,MPI_DOUBLE,body_x,sub_bodies,MPI_DOUBLE,MPI_COMM_WORLD);
        MPI_Allgather(new_body_y,sub_bodies,MPI_DOUBLE,body_y,sub_bodies,MPI_DOUBLE,MPI_COMM_WORLD);
        MPI_Bcast(&body_x[num_bodies-remains],remains,MPI_DOUBLE,MASTER,MPI_COMM_WORLD);
        MPI_Bcast(&body_y[num_bodies-remains],remains,MPI_DOUBLE,MASTER,MPI_COMM_WORLD);
    }

    if(rank == MASTER){
        gettimeofday(&end,NULL);
        double Time = (1000000. * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec)/1000000;
        printf("MPI: Processes: %d; Number of bodies: %d; Runtime: %f\n",num_tasks, num_bodies, Time);
    }
    MPI_Finalize();
    return 0;
}

// x,y,mass are all random integers at initial state
// mass ranges from 1 to 20
void generate_bodies(double* body_x, double* body_y, double* body_mass, int n) {
    srand(time(NULL));
    for (int i = 0; i < n;i++){
    	body_x[i] = rand() % (X_RESN + 1);
    	body_y[i] = rand() % (Y_RESN + 1);
    	body_mass[i] = rand() % (20)+1;
    }
}

Force cal_force_two(Body b1, double x, double y, double mass) {
    Force resultF;
    double r_sq = pow(b1.x-x, 2)+pow(b1.y-y, 2);
    if (r_sq==0) return resultF;
    resultF.x = (6.674*b1.mass*mass/r_sq)*(x-b1.x)/sqrt(r_sq);
    resultF.y = (6.674*b1.mass*mass/r_sq)*(y-b1.y)/sqrt(r_sq);
    return resultF;
}

