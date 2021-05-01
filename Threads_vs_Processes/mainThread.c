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
#include <stdbool.h>

//Declare thread threadtions and handlers
void* sig1thread(void* arg);
void* sig2thread(void* arg);
void* reportthread(void* arg);
void* sigGenthread(void* arg);

void signal1_handler(int signal);
void signal2_handler(int signal);
void report1_handler(int signal);
void report2_handler(int signal);


//create and Initialize mutex lock
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

//Initialize shared counters
int sig1received =0;
int sig2received =0;
int sig1sent=0;
int sig2sent=0;
int reportSent1 =0;
int reportRece1 =0;
int reportSent2 =0;
int reportRece2 =0;
int runType = 0;

//time variables
int count1 = 0;
int count2 =0;
double cTime1 =0;
double cTime2 =0;
double sysTime =0;
clock_t start ,end;
double sig1Av,sig2Av;



//Declare  threads
pthread_t sig1HandlerA;
pthread_t sig1HandlerB;
pthread_t sig2HandlerA;
pthread_t sig2HandlerB;
pthread_t reporter;
pthread_t sigGeneratorA;
pthread_t sigGeneratorB;
pthread_t sigGeneratorC;


int main(void) {
  runType =0; //0 means run for 30 seconds , 1 means run for 100,000 signals

//Block all signals for synchronous handling
sigset_t sigset;
sigemptyset(&sigset);
sigaddset(&sigset, SIGUSR1);
sigaddset(&sigset,  SIGUSR2);
pthread_sigmask(SIG_BLOCK, &sigset, NULL);



//Create threads
pthread_create(&reporter,NULL,reportthread,NULL);
pthread_create(&sig1HandlerA,NULL,sig1thread,NULL);
pthread_create(&sig1HandlerB,NULL,sig1thread,NULL);
pthread_create(&sig2HandlerA,NULL,sig2thread,NULL);
pthread_create(&sig2HandlerB,NULL,sig2thread,NULL);

pthread_create(&sigGeneratorA,NULL,sigGenthread,NULL);
pthread_create(&sigGeneratorB,NULL,sigGenthread,NULL);
pthread_create(&sigGeneratorC,NULL,sigGenthread,NULL);

if (runType == 0){

sleep(30);
printf("\nTotal Signals sent %d\nTotal signals recieved %d\n",(sig1sent+sig2sent), (sig1received+sig2received + reportRece1+reportRece2));
}
else{

//Hoin for when running for 100,000 signals is expected
pthread_join(sig1HandlerA, NULL);
pthread_join(sig1HandlerB, NULL);
pthread_join(sig2HandlerA, NULL);
pthread_join(sig2HandlerB, NULL);
pthread_join(reporter, NULL);
pthread_join(sigGeneratorA, NULL);
pthread_join(sigGeneratorB, NULL);
pthread_join(sigGeneratorC, NULL);

}
}
void* sig1thread(void* arg){

int returnVal =0;
int signal;
sigset_t set1;
sigemptyset(&set1);
sigaddset(&set1, SIGUSR1);

while(1){
  
  returnVal = sigwait(&set1, &signal);
  if(signal == SIGUSR1){
pthread_mutex_lock(&lock);
 sig1received = sig1received+1;
   puts("Sig1 received");
   pthread_mutex_unlock(&lock);

  }
  else{
    puts("error sig 1");
  }

  //Keep the threads running at 1 sec intervals to save cpu cycles
  //sleep(1);
}
exit(0);

}

void* sig2thread(void* arg){
  
int returnVal =0;
int signal;
sigset_t set2;
sigemptyset(&set2);
sigaddset(&set2, SIGUSR2);

while(1){
  
  returnVal = sigwait(&set2, &signal);
  if(signal == SIGUSR2){
pthread_mutex_lock(&lock);
 sig2received = sig2received+1;
   puts("Sig2 received");
   pthread_mutex_unlock(&lock);

  }else{
    puts("error sig 2");
  }
  //Keep the threads running at 1 sec intervals to save cpu cycles
  //sleep(1);
}
exit(0);
  
}
void* reportthread(void* arg){
  
int returnVal =0;
int signal;
sigset_t setR;
sigemptyset(&setR);
sigaddset(&setR, SIGUSR1);
sigaddset(&setR, SIGUSR2);

while(1){

  returnVal = sigwait(&setR, &signal);
  if(signal == SIGUSR1){

 pthread_mutex_lock(&lock);

 reportRece1 = reportRece1+1;
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


int limit = (reportRece1 + reportRece2)%10; //Only multiples of 10
if (limit == 0){
  sig1Av = cTime1/(count1);
  sig2Av = cTime2/count2;
  
  printf("\nSystem Time: %f\nSIGUSR1 Signal(s) sent: %d\nSIGUSR1 Signal(s) recieved: %d\nSIGUSR2 Signal(s) sent: %d\nSIGUSR2 Signal(s) recieved: %d\nSIGUSR1 Average reception time in 10 signal interval : %f\nSIGUSR2 Average reception time in 10 signal interval : %f\n",(cTime1+cTime2),sig1sent,reportRece1,sig2sent,reportRece2,sig1Av,sig2Av);
  // Reset after 10 signals received
 //cTime1=0;
 //count1 =0;
 //count2 =0;
 //cTime2 =0;
 
}


  pthread_mutex_unlock(&lock);
 
}
  

  if(signal == SIGUSR2){
  
 
 pthread_mutex_lock(&lock);
 reportRece2 = reportRece2+1;
  puts("Report 2 received");
//Set up 10 second signal handling report1_handler
//Increment cpu time after a signal is received
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


int limit = (reportRece1 + reportRece2)%10; //Only multiples of 10
if (limit == 0){
  sig1Av = cTime1/(count1);
  sig2Av = cTime2/count2;
  
  
  printf("\nSystem Time: %f\nSIGUSR1 Signal(s) sent: %d\nSIGUSR1 Signal(s) recieved by reporter thread: %d\nSIGUSR2 Signal(s) sent: %d\nSIGUSR2 Signal(s) recieved by reporter thread: %d\nSIGUSR1 Average reception time in 10 signal interval : %f\nSIGUSR2 Average reception time in 10 signal interval : %f\n",(cTime1+cTime2),sig1sent,reportRece1,sig2sent,reportRece2,sig1Av,sig2Av);
  
  // Reset after 10 signals received
 //cTime2=0;
 //count2 =0;
 //count1 =0;
 //cTime1 =0;
 
}
  pthread_mutex_unlock(&lock);
 
}
 //Keep the threads running at 1 sec intervals to save cpu cycles
  //sleep(1); 
}
exit(0);
}
void* sigGenthread(void* arg){

while(1 ==1){
 
  
//Let process wait for .01 to .1 seconds
srand(time(0));
usleep(((rand() % 10) + 1) * 10000);

int q = (rand() % 11);
if(q <5 ){
//Send signals
pthread_kill(reporter,SIGUSR1);
pthread_kill(sig1HandlerA,SIGUSR1);
pthread_kill(sig1HandlerB,SIGUSR1);


pthread_mutex_lock(&lock);
sig1sent = sig1sent+1;
puts("Sig1 Sent");

if((sig1sent + sig2sent) == 10000 && runType == 1){
printf("\nTotal Signals sent %d\nTotal signals recieved %d\n",(sig1sent+sig2sent), (sig1received+sig2received + reportRece1+reportRece2));
  exit(0);
}

pthread_mutex_unlock(&lock);


}
else{
  //Send signals
  pthread_kill(reporter,SIGUSR2);
 pthread_kill(sig2HandlerA,SIGUSR2);
pthread_kill(sig2HandlerB,SIGUSR2);


pthread_mutex_lock(&lock);
sig2sent = sig2sent+1;
puts("Sig2 Sent");
if((sig1sent + sig2sent) == 10000 && runType == 1){
printf("\nTotal Signals sent %d\nTotal signals recieved %d\n",(sig1sent+sig2sent), (sig1received+sig2received + reportRece1+reportRece2));
  exit(0);
}


pthread_mutex_unlock(&lock);


}
}


exit(0);
  
}







