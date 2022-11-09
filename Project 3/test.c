#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/wait.h>

typedef struct {
    int type;
} product;

// write info into the left side of the pipe whose write-end is given by pipe_write_end
void producer1(FILE *pipe_write_end) {
    product *args = (product *) args;
    args->type = 1;
    // produce 10 elements
    for(int i=0; i<= 10; i++) {
        // send each element i through the left side stream of the pipe
        fprintf(pipe_write_end, "%d ", args->type);
        printf("Prod type = %d\n", args->type);
    }
    // close the hatch since we are done
    fclose(pipe_write_end);
    // exit function
    exit(0);
}

// write info into the left side of the pipe whose write-end is given by pipe_write_end
void producer2(FILE *pipe_write_end) {
    product *args = (product *) args;
    args->type = 2;
    // produce 10 elements
    for(int i=0; i<= 10; i++) {
        // send each element i through the left side stream of the pipe
        fprintf(pipe_write_end, "%d ", args->type);
        printf("Prod type = %d\n", args->type);
    }
    // close the hatch since we are done
    fclose(pipe_write_end);
    // exit function
    exit(0);
}

// read info from right side of the pipe whose read-end is given by pipe_read_end and print to STDOUT
void consumer(FILE *pipe_read_end) {
    int k, m;
    while(1) {
        // read the right side of the pipe with fscanf and declare it as a variable to be used for STDOUT
        int n = fscanf(pipe_read_end, "%d", &k);
        // If fscanf was successful
        if(n == 1) printf("consumer got %d\n", k);
        else break;
    }
    // close the hatch since we are done reading everything
    fclose(pipe_read_end);
    exit(0);
}

int main() {
    product args = {0};
    int producer1_id, producer2_id, consumer_id;
    int pd[2];
    FILE *pipe_write_end, *pipe_read_end;

    // build the pipe by creating two FDs in the pd array
    // the first element of pd is used for reading data from the pipe
    // the second element of pd is used for writing data to the pipe
    pipe(pd);

    // the right side of the pipe
    // create a stream to read a text file
    pipe_read_end = fdopen(pd[0], "r");

    // the left side of the pipe
    // create a stream to write to a text file
    pipe_write_end = fdopen(pd[1], "w");

    // fork the 1st producer
    producer1_id = fork();
    //child process
    if(producer1_id == 0) {
        // close the right side of the pipe since we are writing new data
        fclose(pipe_read_end);
        // call the producer function to write new data
        producer1(pipe_write_end);
    }

    // fork the 2nd producer
    producer2_id = fork();
    //child process
    if(producer2_id == 0) {
        // close the right side of the pipe since we are writing new data
        fclose(pipe_read_end);
        // call the producer function to write new data
        producer2(pipe_write_end);
    }

    product p;

    int k, m;
    while(1) {
        // read the right side of the pipe with fscanf and declare it as a variable to be used for STDOUT
        printf("Retreived type: %d\n", p.type);
        int n = fscanf(pipe_read_end, "%d", &k);
        // If fscanf was successful
        if(n == 1) printf("consumer got %d\n", k);
        else exit(0);
    }

    // Close each side of the pipe and wait for each child process to finish
    fclose(pipe_read_end);
    fclose(pipe_write_end);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}
