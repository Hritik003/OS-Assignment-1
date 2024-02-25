//only one instance of this is created

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>

#define MAX_CUSTOMERS 5
#define SHM_SIZE 1024
#define MAX_ORDERS 10

void create_waitershmptrs(int tables,int *waiter_shmptr_arr[],int waiter_shmid_arr[]){
    //func to create the shmptrs for waiters and put it in an array 
    //if this gives any issue, then put this code back into the main function
    for(int i=1;i<=tables;i++){
        // ftok to generate unique key for shared memory with each waiter
        key_t waiter_shmkey=ftok("hotelmanager.c",i);
        if(waiter_shmkey==-1){
            printf("error in ftok\n");
            exit(1);
        }

        int waiter_shmid=shmget(waiter_shmkey, 1024, 0666 | IPC_CREAT);
        if(waiter_shmid<0){
            perror("shmget error");
            exit(1);
        }

        int *waiter_shmptr=(int *)shmat(waiter_shmid, (void*)0, 0);
        if(waiter_shmptr==(int *)(-1)){
            perror("shmptr error");
            exit(1);
        }
        *waiter_shmptr=0;
        waiter_shmptr_arr[i]=waiter_shmptr;
        waiter_shmid_arr[i]=waiter_shmid;
        printf("Hello life %d %d\n",*(waiter_shmptr_arr[i]),waiter_shmid_arr[i]);
    }
}

void clear_waitershmptrs(int tables,int *waiter_shmptr_arr[MAX_CUSTOMERS+1],int waiter_shmid_arr[MAX_CUSTOMERS+1]){
    for(int i=1;i<=tables;i++){
        memset(waiter_shmptr_arr[i],0,SHM_SIZE);
        shmdt(waiter_shmptr_arr[i]);
        shmctl(waiter_shmid_arr[i],IPC_RMID,NULL);
    }
}

void calculate_profit(FILE *fptr,float t_earnings){
    float profit=0.0;
    float wages=0.0;

    wages = t_earnings*0.4;
    profit = t_earnings-wages;

    fprintf(fptr, "Total Earnings of Hotel: %.2f INR\n", t_earnings);
    fprintf(fptr, "Total Wages of Waiters: %.2f INR\n", wages);
    fprintf(fptr, "Total Profit: %.2f INR\n", profit);
    fclose(fptr);

    printf("Total Earnings of Hotel: %.2f INR\n", t_earnings);
    printf("Total Wages of Waiters: %.2f INR\n", wages);
    printf("Total Profit: %.2f INR\n", profit);

    printf("Thank you for visiting the Hotel!\n");
}

int main(){
    key_t termination_key = ftok("admin.c",'A');
    int termination_shmid = shmget(termination_key,SHM_SIZE,0666 | IPC_CREAT);
    if(termination_shmid<0){
        perror("error at termination shm");
        exit(1);
    }
    printf("shared memory id for hotel-admin: %d\n",termination_shmid);

    char *termination_shmptr =(char *)shmat(termination_shmid, (void*)0, 0);
    if(termination_shmptr == (char *)(-1)){
        perror("error in shmptr");
        exit(1);
    }

    // strcpy(termination_shmptr,"");

    int tables;
    float t_earnings=0.0;
    FILE *fptr;    

    printf("Enter the Total Number of Tables at the Hotel: ");
    scanf("%d",&tables);

    // int **waiter_shmptr_arr=initialize_ptrarr(tables);//array of shmptrs
    // int *waiter_shmid_arr=initialize_idarr(tables);
    int *waiter_shmptr_arr[MAX_CUSTOMERS+1];
    int waiter_shmid_arr[MAX_CUSTOMERS+1];
    
    fptr = fopen("earnings.txt","w");
    if(fptr== NULL){
        perror("unable to open file");
        exit(1);
    }

    while(1){
        create_waitershmptrs(tables,waiter_shmptr_arr,waiter_shmid_arr);
        
        int all_order_taken=0;
        while(!all_order_taken){
            for(int i=1;i<=tables;i++){
                if(*waiter_shmptr_arr[i]==0)all_order_taken=0;
                else{
                    all_order_taken=1;
                }
            }
        }
        
        for(int i=1;i<=tables;i++){
            printf("Earning from Table %d: %d INR\n", i, *waiter_shmptr_arr[i]);
            fprintf(fptr,"Earning from Table %d: %d INR\n", i, *waiter_shmptr_arr[i]);
            t_earnings+=*waiter_shmptr_arr[i];
        }

        // int no_customers;
        // while(no_customers<tables){
        //     no_customers=0;
        //     for(int i=1;i<=tables;i++){
        //         if(*waiter_shmptr_arr[i]==0)no_customers++;//to check this part again
        //         else{
        //             continue;
        //         }
        //     }
        // }
        clear_waitershmptrs(tables,waiter_shmptr_arr,waiter_shmid_arr);
        if(*termination_shmptr!='Y')break;
        else{
            printf("New set of customers!\n");
        }
    }

    /**---------------------------------CHECK HOTEL CLOSING CALL AND CALCULATE PROFIT---------------------------------*/
    calculate_profit(fptr,t_earnings);
    fclose(fptr); 
    printf("Thank you for visiting the Hotel!");
    *termination_shmptr=103;
   
    shmdt(termination_shmptr);
    shmctl(termination_shmid,IPC_RMID,NULL);

    return 0;
}