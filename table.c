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

int pipe_handler[MAX_CUSTOMERS][2];

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

void customer_order(int i){
        if(pipe(pipe_handler[i])==-1){
            perror("Error in creating the pipe");
            exit(1);
        }

        //creating a process
        pid_t pid = fork();

        if(pid == 0){
            //child process
            int order_num;
            printf("table pid is: %d\n",getppid());
            printf("Enter the serial number(s) of the item(s) to order from the menu. Enter -1 when done: \n");
            while(scanf("%d",&order_num)&&order_num!=-1){
                if(order_num<0 || order_num>4)printf("Please enter a valid order\n");
                close(pipe_handler[i][READ_END]);
                write(pipe_handler[i][WRITE_END],&order_num,sizeof(order_num));
            }
            close(pipe_handler[i][WRITE_END]);
            exit(0);
        }
}

int main(){
    int table_number;
    printf("Enter Table Number:");
    scanf("%d",&table_number);

    //asking the number of customers
    
    int num_of_customers;
    printf("Enter Number of Customers at Table (maximum no. of customers can be 5):");
    scanf("%d",&num_of_customers);

    if(num_of_customers==-1)return 0;//exit

    //displaying the contents of the menu
    display_menu();

    //creating customer processes and seperate pipe for customer and table process

    // for(int i=0;i<num_of_customers;i++){
    //     if(pipe(pipe_handler[i])==-1){
    //         perror("Error in creating the pipe");
    //         exit(1);
    //     }

    //     //creating a process
    //     pid_t pid = fork();

    //     if(pid == 0){
    //         //child process
    //         int order_num;
    //         printf("Enter the serial number(s) of the item(s) to order from the menu. Enter -1 when done: \n");
    //         while(scanf("%d",&order_num)&&order_num!=-1){
    //             close(pipe_handler[i][READ_END]);
    //             write(pipe_handler[i][WRITE_END],&order_num,sizeof(order_num));
    //         }
    //         close(pipe_handler[i][WRITE_END]);
    //         // exit(0);
    //     }
    //     else if(pid > 0){
    //         //parent proces

    //     }
    //     else{
    //         // perror("Error in forking a process for %dth customer on %dth table",i,table_number);
    //         perror("Error in forking a process\n");
    //     } 
    // }

    for(int i=0;i<num_of_customers;i++){
        customer_order(i);
        wait(NULL);
    }
}