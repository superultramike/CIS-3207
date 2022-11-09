#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/wait.h>

#define MAX 20

typedef struct {
    int id;
    int type;
} product;

// Global count that confirms that you got all the elements from said buffer
int count1 = 0;
int count2 = 0;

int buffer1[20], fillIndex1;
int buffer2[20], fillIndex2;

// Buffer operations
// function to put product 1 type into queue
void put1(int value) {
    //printf("RAN\n");
    buffer1[fillIndex1] = value;
    fillIndex1 = (fillIndex1 + 1) % MAX;
    printf("Put1: %d\n", value);
    count1++;
}

// function to put product 2 type into queue
void put2(int value) {
    //printf("RAN\n");
    buffer2[fillIndex2] = value;
    fillIndex2 = (fillIndex2 + 1) % MAX;
    printf("Put2: %d\n", value);
    count2++;
}

// WRITE GET FUNCTION HERE

int main() {
    // DECLARE PTHREADS THERE

    product p;
    int pd[2];

    if(pipe(pd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int pid = fork();
    if(pid == 0) {
        // producer 1 write to the pipe
        close(pd[0]);
        int count = 0;
        while(count != 10) {
            p.id = count;
            p.type = 1;
            write(pd[1], &p, sizeof(p));
            count += 1;
            usleep(100000);
        }
    }
    else {
        int second_pid = fork();
        if(second_pid == 0) {
            // producer 2 write to the pipe
            close(pd[0]);
            int count = 0;
            while(count != 10) {
                p.id = count;
                p.type = 2;
                write(pd[1], &p, sizeof(p));
                count += 1;
                usleep(100000);
            }
        }
        // Distributor that reads the pipe and sends data to specific queue
        while(1) {
            // read the pipe
            if((read(pd[0], &p, sizeof(p)) > 0)) {
                //printf("Product ID: %d, Type: %d\n", p.id, p.type);
                if(p.type == 1) {
                    // put it in the product type 1 queue
                    //printf("type 1!\n");
                    put1(p.id);
                }
                if(p.type == 2) {
                    // put it in the product type 2 queue
                    //printf("typ 2!\n");
                    put2(p.id);
                }
            }
        }
        exit(0);
    }
}
