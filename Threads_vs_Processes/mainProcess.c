#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

//Functions
void signal1_handler(int signal);
void signal2_handler(int signal);
void report1_handler(int signal);
void report2_handler(int signal);




//initialize mutex and variables
pthread_mutex_t lock ;
pthread_mutexattr_t lockAtt;

//Initialize shared counters
int* sig1received =0;
int* sig2received =0;
int* sig1sent=0;
int* sig2sent=0;
int* reportSent1 =0;
int* reportRece1 =0;
int* reportSent2 =0;
int* reportRece2 =0;
int* runType = 0;


//time variables
int count1 = 0;
int count2 =0;
double cTime1 =0;
double cTime2 =0;
double sysTime =0;

clock_t start ,end;
double sig1Av,sig2Av;

pid_t pids[8];// child processes



int main(){


  sigset_t mask; //signal mask for blocking non involved processes
sigemptyset(&mask);
sigaddset(&mask, SIGUSR1);
sigaddset(&mask, SIGUSR2);

sigset_t mask1; //signal mask for blocking involved processes
sigemptyset(&mask1);
sigaddset(&mask1, SIGUSR1);
sigaddset(&mask1, SIGUSR2);


//Initialize sharable mutex lock between processes


pthread_mutexattr_init(&lockAtt);
pthread_mutexattr_setpshared(&lockAtt, PTHREAD_PROCESS_SHARED);
pthread_mutex_init(&lock, &lockAtt);
//lock1 = lock;

/* Clean up. 
pthread_mutex_destroy(lock);
pthread_mutexattr_destroy(&lockAtt); 
*/

//Initialize mask to empty and ad user sig1 and 2

//Shared counters for processes
sig1sent = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); 
sig2sent = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); 
sig1received = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); 
sig2received = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); 
reportSent1 = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); 
reportRece1 = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); 
reportSent2 = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); 
reportRece2 = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); 

runType = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); 

*runType =0;  //0 means run for 30 seconds , 1 means run for 100,000 signals



for(int i = 0; i < 8; ++i){

pids[i] = fork();

if(pids[i] == 0){
// run processes depending on value of i

if(i==0){ //Signal Process Handler A

//Reset and Block other signal
sigemptyset(&mask1);
sigaddset(&mask1, SIGUSR2);
sigprocmask(SIG_BLOCK, &mask1, NULL);



//initialize signal
signal(SIGUSR1, signal1_handler);

while(1==1){
  //Keep the process running at 1 sec intervals to save cpu cycles
  sleep(1);
}
exit(0);

}

 if(i ==1){ //Signal Process Handler A
 //Reset and Block other signal
sigemptyset(&mask1);
sigaddset(&mask1, SIGUSR2);
sigprocmask(SIG_BLOCK, &mask1, NULL);

//initialize signal
signal(SIGUSR1, signal1_handler);

while(1==1){
  //Keep the process running at 1 sec intervals to save cpu cycles
  sleep(1);
}
exit(0);
}
else if(i ==2){//Signal Process Handler B
// Reset and Block other signal
sigemptyset(&mask1);
sigaddset(&mask1, SIGUSR1);
sigprocmask(SIG_BLOCK, &mask1, NULL);

//initialize signal
signal(SIGUSR2, signal2_handler);

while(1==1){
  //Keep the process running at 1 sec intervals to save cpu cycles
  sleep(1);
}
exit(0);
}


else if(i ==3){//Signal Process Handler B

 //Reset and Block other signal
sigemptyset(&mask1);
sigaddset(&mask1, SIGUSR1);
sigprocmask(SIG_BLOCK, &mask1, NULL);

//initialize signal
signal(SIGUSR2, signal2_handler);

while(1==1){ 
  //Keep the process running at 1 sec intervals to save cpu cycles
  sleep(1);
}
exit(0);
}

else if(i ==4){ //Reporting process

//Recieives both signals
signal(SIGUSR1, report1_handler);
signal(SIGUSR2, report2_handler);


while(1==1){
  //Keep the process running at 1 sec intervals to save cpu cycles
  sleep(1);
}
exit(0);
}


else if(i ==5){ //Signal Process Generator 
sigprocmask(SIG_BLOCK, &mask, NULL);
while(1 ==1){
  
//Let process wait for .01 to .1 seconds
srand(time(0));
usleep(((rand() % 10) + 1) * 10000);

int q = (rand() % 11);
if(q <5 ){

kill(0,SIGUSR1);
pthread_mutex_lock(&lock);
*sig1sent = *sig1sent+1;

puts("Sig1 Sent");
//Exit after 100000 signals sent
if((*sig1sent + *sig2sent) == 10000 && *runType == 1){
printf("\nTotal Signals sent %d\nTotal signals recieved %d\n",(*sig1sent+*sig2sent), (*sig1received+*sig2received + *reportRece1+*reportRece2));
   kill(0,SIGTERM);exit(0);
}

pthread_mutex_unlock(&lock);


}
else{
  kill(0,SIGUSR2);
pthread_mutex_lock(&lock);
*sig2sent = *sig2sent+1;
puts("Sig2 Sent");
//Exit after 100000 signals sent
if((*sig1sent + *sig2sent) == 10000 && *runType == 1){
  printf("\nTotal Signals sent %d\nTotal signals recieved %d\n",(*sig1sent+*sig2sent), (*sig1received+*sig2received + *reportRece1+*reportRece2));
  kill(0,SIGTERM);exit(0);
}


pthread_mutex_unlock(&lock);


}
}


exit(0);
}
else if(i ==6){
sigprocmask(SIG_BLOCK, &mask, NULL);
while(1 ==1){
  
//Let process wait for .01 to .1 seconds
srand(time(0));
usleep(((rand() % 10) + 1) * 10000);

int q = (rand() % 11);
if(q <5 ){
  //printf("wawa %d\n", pids[1]);

kill(0,SIGUSR1);
pthread_mutex_lock(&lock);
*sig1sent = *sig1sent+1;
puts("Sig1 Sent");
//Exit after 100000 signals sent
if((*sig1sent + *sig2sent) == 10000 && *runType == 1){
  printf("\nTotal Signals sent %d\nTotal signals recieved %d\n",(*sig1sent+*sig2sent), (*sig1received+*sig2received + *reportRece1+*reportRece2));
   kill(0,SIGTERM);exit(0);
}


pthread_mutex_unlock(&lock);

}
else{
  kill(0,SIGUSR2);
pthread_mutex_lock(&lock);
*sig2sent = *sig2sent+1;
puts("Sig2 Sent");
//Exit after 100000 signals sent
if((*sig1sent + *sig2sent) == 10000 && *runType == 1){
  printf("\nTotal Signals sent %d\nTotal signals recieved %d\n",(*sig1sent+*sig2sent), (*sig1received+*sig2received + *reportRece1+*reportRece2));
  kill(0,SIGTERM); exit(0);
}


pthread_mutex_unlock(&lock);


}
}
exit(0);
}

else if(i ==7){
sigprocmask(SIG_BLOCK, &mask, NULL);
while(1 ==1){
  
//Let process wait for .01 to .1 seconds
srand(time(0));
usleep(((rand() % 10) + 1) * 10000);

int q = (rand() % 11);
if(q <5 ){
  //printf("wawa %d\n", pids[1]);

kill(0,SIGUSR1);
pthread_mutex_lock(&lock);
*sig1sent = *sig1sent+1;
puts("sig 1 sent ");
//Exit after 100000 signals sent
if((*sig1sent + *sig2sent) == 10000 && *runType == 1){
  printf("\nTotal Signals sent %d\nTotal signals recieved %d\n",(*sig1sent+*sig2sent), (*sig1received+*sig2received + *reportRece1+*reportRece2));
  kill(0,SIGTERM); exit(0);
}


//printf("%d\n",*sig1sent);
pthread_mutex_unlock(&lock);


}
else{
  kill(0,SIGUSR2);
pthread_mutex_lock(&lock);
*sig2sent = *sig2sent+1;
puts("Sig 2 Sent");
//Exit after 100000 signals sent
if((*sig1sent + *sig2sent) == 100000 && *runType == 1){
  printf("\nTotal Signals sent %d\nTotal signals recieved %d\n",(*sig1sent+*sig2sent), (*sig1received+*sig2received + *reportRece1+*reportRece2));
  kill(0,SIGTERM); exit(0);
}


pthread_mutex_unlock(&lock);


}
}

exit(0);
}




}
else//Parent
{
  //Wait for children after all processes are completed
  //sigprocmask(SIG_BLOCK, &mask, NULL);

  if(i == 7){
      //sleep(10);
    sigprocmask(SIG_BLOCK, &mask, NULL);  
    if(* runType ==0){
      sleep(30);
      printf("\nTotal Signals sent %d\nTotal signals recieved %d\n",(*sig1sent+*sig2sent), (*sig1received+*sig2received + *reportRece1+*reportRece2));
       kill(0,SIGTERM);
    }
    if(* runType ==1){
      wait(NULL);
    }

  

   

    //Terminate after 30 seconds
    // sleep(3);
    // printf("number of signals sent  %d",(*sig1sent + *sig2sent));
   // kill(0,SIGTERM);

    
  
 
  }
  
  //printf("hohoho");
// continue until i == 7, then wait
}
}

}
void signal1_handler(int signal){
if(signal == SIGUSR1){
 pthread_mutex_lock(&lock);
 *sig1received = *sig1received+1;
   puts("Sig1 received");
  // printf("s1r %d \n",*sig1received);
  pthread_mutex_unlock(&lock);

}

}
void signal2_handler(int signal){
if(signal == SIGUSR2){
 pthread_mutex_lock(&lock);
 *sig2received = *sig2received+1;
  puts("Sig2 received");
  pthread_mutex_unlock(&lock);
 
}
}
void report1_handler(int signal){
if(signal == SIGUSR1){
 pthread_mutex_lock(&lock);
 *reportRece1 = *reportRece1+1;
  puts("Report 1 received");
//Set up 10 second signal handling report1_handler
if(count1 == 0){
  start = clock();
  count1++;
}
else{
  end =clock();
  count1++;  
  cTime1 = cTime1 + ((double) (end - start)) / CLOCKS_PER_SEC;
  sysTime = cTime1 +  sysTime;
  start = clock();
}

//printf("Sum %d\n",(*reportRece1 + *reportRece2));
int limit = (*reportRece1 + *reportRece2)%10; //Only multiples of 10
if (limit == 0){
  sig1Av = cTime1/(count1);
  sig2Av = cTime2/count2;
  
  
  printf("\nSystem Time: %f\nSIGUSR1 Signal(s) sent: %d\nSIGUSR1 Signal(s) recieved : %d\nSIGUSR2 Signal(s) sent: %d\nSIGUSR2 Signal(s) recieved: %d\nSIGUSR1 Average reception time in 10 signal interval : %f\nSIGUSR2 Average reception time in 10 signal interval : %f\n",(cTime1+cTime2),*sig1sent,(*reportRece1+*sig1received),*sig2sent,(*reportRece2+*sig2received),sig1Av,sig2Av);
  
  // Reset after 10 signals received
 //cTime2=0;
 //count2 =0;
 //count1 =0;
 //cTime1 =0;
 
}


  pthread_mutex_unlock(&lock);
 
}
}
void report2_handler(int signal){
  
if(signal == SIGUSR2){
 
 pthread_mutex_lock(&lock);
 *reportRece2 = *reportRece2+1;
  puts("Report 2 received");
//Set up 10 second signal handling report1_handler
//Increment cpu time after a sifnal is received
if(count2 == 0){
  start = clock();
  count2++;
}
else{
  end =clock();
  count2++;  
  cTime2 = cTime2 + ((double) (end - start)) / CLOCKS_PER_SEC;
   sysTime = cTime2 +  sysTime;
  start = clock();
  
}

printf("Sum %d\n",(*reportRece1 + *reportRece2));
int limit = (*reportRece1 + *reportRece2)%10; //Only multiples of 10
if (limit == 0){
  sig1Av = cTime1/(count1);
  sig2Av = cTime2/count2;
  
  
  printf("\nSystem Time: %f\nSIGUSR1 Signal(s) sent: %d\nSIGUSR1 Signal(s) recieved : %d\nSIGUSR2 Signal(s) sent: %d\nSIGUSR2 Signal(s) recieved: %d\nSIGUSR1 Average reception time in 10 signal interval : %f\nSIGUSR2 Average reception time in 10 signal interval : %f\n",(cTime1+cTime2),*sig1sent,(*reportRece1+*sig1received),*sig2sent,(*reportRece2+*sig2received),sig1Av,sig2Av);
  
  // Reset after 10 signals received
 //cTime2=0;
 //count2 =0;
 //count1 =0;
 //cTime1 =0;
 
}
  pthread_mutex_unlock(&lock);
 
}
}