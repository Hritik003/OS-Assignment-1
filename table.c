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

void display_menu(){
    printf("Menu: \n");
    FILE *fptr;
    fptr = fopen("menu.txt","r");
    if (!fptr) {
        perror("Error occured in opening the menu.txt file");
        exit(1);
    }
    char cuisine[100];
    while (fgets(cuisine, 100, fptr) != NULL) {
        printf("%s", cuisine);
    }
    fclose(fptr);
}

void customer_order(int pipe_handler[], int customer_id){
    //customer closes the read end
    close(pipe_handler[READ_END]);

    //taking order from the customer
    int order_num;
    printf("Customer %d, enter the serial number(s) of the item(s) to order from the menu. Enter -1 when done:\n", customer_id);
    
    while(1){
        scanf("%d",&order_num);
        write(pipe_handler[WRITE_END], &order_num, sizeof(order_num));

        if (order_num == -1) break;
    }

    //customer closes the write end
    close(pipe_handler[WRITE_END]);
    exit(0);
}

int main(){
    int table_number;
    printf("Enter Table Number:");
    scanf("%d",&table_number);

    int pipe_handler[MAX_CUSTOMERS][2];
    int customer_pids[MAX_CUSTOMERS];
    

    //asking the number of customers
    // while(1){
        int num_of_customers;
        printf("Enter Number of Customers at Table (maximum no. of customers can be 5):");
        scanf("%d",&num_of_customers);

        if(num_of_customers==-1){
            perror("exitting since the number of customers entered is -1");
            exit(1);
        }

        //creating customer processes and seperate pipe for customer and table process

        for(int i=0;i<num_of_customers;i++){
            if(pipe(pipe_handler[i]) == -1){
                perror("Error in creating the pipe");
                exit(1);
            }


            //creating a process
            pid_t pid = fork();

            if(pid == 0){
                //customer process will order 
                display_menu();
                customer_order(pipe_handler[i], i + 1);//i+1 is the customer id
            }
            else if(pid > 0){
                //parent process
                customer_pids[i]=pid;
                // printf("this is parent");


            }
            else{
                perror("Error in forking a process for the customer");
            }

        }


    // }



}