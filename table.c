#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>

#define READ_END 0
#define WRITE_END 1
#define MAX_CUSTOMERS 5

int main(){
    int table_number;
    printf("Enter Table Number:");
    scanf("%d",&table_number);

    int pipe_handler[MAX_CUSTOMERS][2];
    

    //asking the number of customers
    while(1){
        int num_of_customers;
        printf("Enter Number of Customers at Table (maximum no. of customers can be 5):");
        scanf("%d",&num_of_customers);

        if(num_of_customers==-1)break;//exit

        //creating customer processes and seperate pipe for customer and table process

        for(int i=0;i<num_of_customers;i++){
            if(pipe(pipe_handler[i]==-1)){
                perror("Error in creating the pipe");
                exit(1);
            }


            //creating a process
            int pid = fork;

            if(pid == 0){
                //child process

            }
            else if(pid > 0){
                //parent proces

            }
            else{
                perror("Error in forking a process for %dth customer on %dth table",i,table_number);
            }

        }


    }



}