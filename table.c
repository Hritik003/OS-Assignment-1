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
#define SHM_SIZE 1024
#define MAX_ORDERS 10

struct customer_data{
    int customer_id;
    int orders[MAX_ORDERS];
    int num_orders;
};
struct customer_data orders[MAX_CUSTOMERS];

void display_menu(){
    printf("<-------------------------------Menu--------------------------------->\n");
    printf(" \n");
    FILE *fptr;
    fptr = fopen("menu.txt","r");
    if (!fptr) {
        perror("Error occured in opening the menu.txt file");
        exit(1);
    }
    char cuisine[100];
    while (fgets(cuisine, 100, fptr) != NULL) {
        printf("%s\n", cuisine);
    }
    fclose(fptr);
    printf("<---------------------------------------------------------------->\n");
}

void customer_order(int pipe_handler[], int customer_id,struct customer_data *orders){
    //customer closes the read end
    close(pipe_handler[READ_END]);

    //taking order from the customer
    int order_num;
    int count_of_orders=0;
    printf("Customer %d, enter the serial number(s) of the item(s) to order from the menu. Enter -1 when done:\n", customer_id);
    
    while(1){
        scanf("%d",&order_num);
        write(pipe_handler[WRITE_END], &order_num, sizeof(order_num));

        if (order_num == -1) break;
        else {
            orders->orders[count_of_orders]=order_num;
            count_of_orders=count_of_orders+1;
        }
    }

    orders[customer_id].num_orders=count_of_orders;

    //customer closes the write end
    close(pipe_handler[WRITE_END]);
    exit(0);
}

int main(){
    int table_number;
    printf("Enter Table Number:");
    scanf("%d",&table_number);

    //shared memory
    key_t key = ftok("table.c",table_number);
    if(key==-1){
        printf("error in ftok");
        return 1;
    }

    int shmid;
    int *shmptr;
    
    shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
     if (shmid<0) {
        perror("shmget");
        exit(1);
    }

    shmptr = shmat(shmid, NULL, 0);
    if (shmptr== (int *)(-1)) {
        perror("shmat");
        exit(1);
    }

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


        display_menu();

        //creating a structured data for customer order details
        

        for(int i=0;i<num_of_customers;i++){
            

            if(pipe(pipe_handler[i]) == -1){
                perror("Error in creating the pipe");
                exit(1);
            }
            pid_t pid = fork();
            if(pid == 0){
                printf("customer %d pid is: %d\n",i+1,getpid());
                printf("table pid is: %d\n",getppid());
                orders[i].customer_id=i+1;
                customer_order(pipe_handler[i], i + 1,&orders[i]);
            }
            else if(pid > 0){
                customer_pids[i]=pid;
                close(pipe_handler[i][WRITE_END]); 
                // printf("this is parent");
            }
            else{
                perror("Error in forking a process for the customer");
            }
            wait(NULL);

        }



        for (int i=0; i<num_of_customers; i++) {
            int status;
            waitpid(customer_pids[i], &status, 0);
            int order_num;
            int check= 0;
            printf("\n Order recieved from Customer %d: ", i + 1);

            while (read(pipe_handler[i][READ_END], &order_num, sizeof(order_num)) > 0 ) {
                if(order_num!=-1){
                    printf("%d ", order_num);
                    check = 1;
                }
            }
            if(!check) printf("No order recieved from customer %d",i+1);
            printf("\n");
            close(pipe_handler[i][READ_END]);
        }


        memcpy(shmptr,orders,sizeof(struct customer_data)*MAX_CUSTOMERS);
        printf("MESSAGE STORED\n");


        printf("<---------------------------------------------------------->");
        printf("Waiting for the bill from the waiter...\n");
        while (((char *)shmptr)[0]=='\0'||strstr((char *)shmptr, "Bill")==NULL)sleep(1);
    // }



}