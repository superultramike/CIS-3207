#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/wait.h>

// write info into the left side of the pipe whose write-end is given by pipe_write_end
void producer1(FILE *pipe_write_end) {
    // produce 100 elements
    for(int i=0; i<= 10; i++) {
        // send each element i through the left side stream of the pipe
        fprintf(pipe_write_end, "%d ", i);
        printf("Producer 1 made: %d\n", i);
    }
    // close the hatch since we are done
    fclose(pipe_write_end);
    // exit function
    exit(0);
}

// write info into the left side of the pipe whose write-end is given by pipe_write_end
void producer2(FILE *pipe_write_end) {
    // produce 100 elements
    for(int i=0; i<= 10; i++) {
        // send each element i through the left side stream of the pipe
        fprintf(pipe_write_end, "%d ", i);
        printf("Producer 2 made: %d\n", i);
    }
    // close the hatch since we are done
    fclose(pipe_write_end);
    // exit function
    exit(0);
}

// read info from right side of the pipe whose read-end is given by pipe_read_end and print to STDOUT
void consumer(FILE *pipe_read_end) {
    int n,k;
    while(1) {
        // read the right side of the pipe with fscanf and declare it as a variable to be used for STDOUT
        int n = fscanf(pipe_read_end, "%d", &k);
        // If fscanf was successful
        if(n == 1) printf("consumer: got %d\n", k);
        else break;
    }
    // close the hatch since we are done reading everything
    fclose(pipe_read_end);
    exit(0);
}

int main() {
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

    // fork the consumer
    consumer_id = fork();
    // child process
    if(consumer_id == 0) {
        // close the left side of the pipe since we are reading new data
        fclose(pipe_write_end);
        // call the consumer function to read the new data
        consumer(pipe_read_end);
    }

    // Close each side of the pipe and wait for each child process to finish
    fclose(pipe_read_end);
    fclose(pipe_write_end);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}
