#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include<unistd.h>


int tobacco;     //0 if not on table, 1 if on table
int paper;       //0 if not on table, 1 if on table
int matches;     //0 if not on table, 1 if on table
int empty;         //0 if table is full

int smoker_number;          //To denote smoker number

//Semaphores
sem_t semaphore;
sem_t smokers;

void *agent();          //Agent Functionality
void *smoker();         //Smoker Functionality
int randomnum(int min, int max);          //Random numbers in a range

//Main function
int main(int argc, char **argv)
{
        srand(time(0));

        //Set up end time condition
        time_t end_time;
        time_t start = time(NULL);
        time_t end = 100;
        end_time = start + end;            //Terminate after 100 seconds

        int i;

        //Initially No Ingredient on Table
        tobacco = 0;
        paper = 0;
        matches = 0;
        empty= 1;          //Initially Empty Table

        smoker_number = 1;

        int n = 3;                      //number of smokers
        //Initialize semaphores
        sem_init(&semaphore, 0, 1);
        sem_init(&smokers, 0, 1);

        //Threads for agent and smokers
        pthread_t agent_thread;
        pthread_t smoker_thread[n];

        //Create the agent and smokers threads
        pthread_create(&agent_thread, NULL, agent, NULL);
        for(i = 0; i < n; i++)
        {
                pthread_create(&smoker_thread[i], NULL, smoker, NULL);
        }

        //Loop the main thread until timeout has been reached
        while(1)
        {
                if(start < end_time)       //Preventing program from running infinitely. Running only for specified amount of time.
                {
                        sleep(1);
                        start = time(NULL);
                }
                else{
                        pthread_cancel(agent_thread);    //cancel the Agent Thread
                        for(i = 0; i < n; i++)
                        {
                                pthread_cancel(smoker_thread[i]);        //Cancel the smokers threads
                        }
                        break;
                }
        }
}

void *agent()
{
        int decision;                   //To hold decision of agent

        while(1)
        {
                decision = (rand()%3)+1;                //Set decision randomly
                if(decision == 1)                       //Check the decision
                {
                        sem_wait(&semaphore);
                        if(empty==1){              //If empty Table place the following ingredients
                            tobacco=1;
                            paper = 1;
                            matches=0;
                            empty=0;                   //Table no longer empty
                            sem_post(&semaphore);
                            printf("Agent placed tobacco and paper on the table\n");
                        }else{
                            sem_post(&semaphore);
                        }
                }
                else if(decision == 2)                  //Check the decision
                {
                        sem_wait(&semaphore);
                        if(empty==1){              //If empty table place the following ingredients
                            tobacco = 1;
                            matches = 1;
                            paper=0;
                            empty=0;                   //Table no longer Empty
                            sem_post(&semaphore);
                            printf("Agent placed tobacco and matches on the table\n");
                        }else{
                        sem_post(&semaphore);
                        }
                }
                else if(decision ==3)                   //Check the decision
                {
                        sem_wait(&semaphore);
                        if(empty==1){              //If empty place the following ingredients
                            paper = 1;
                            matches = 1;
                            tobacco=0;
                            empty=0;                   //Table no longer empty
                            sem_post(&semaphore);
                            printf("Agent placed paper and matches on the table\n");
                        }else{
                            sem_post(&semaphore);
                        }
                }
        }
}

void *smoker()
{
        sem_wait(&smokers);
        int smoker_num = smoker_number;                   //Set the smoker number
        smoker_number= smoker_number+1;                   //Smoker number for other smoker
        sem_post(&smokers);

        //To hold smokers ingredients
        int has_tobacco = 0;
        int has_paper = 0;
        int has_matches = 0;
        int smoke_time;                  //To hold Smoking time

        if(smoker_num == 1)           //Smoker 1 has tobacco
        {
                has_tobacco = 1;
        }
        else if(smoker_num == 2)      //Smoker 2 has paper
        {
                has_paper = 1;
        }
        else if(smoker_num == 3)      //Smoker 3 has matches
        {
                has_matches = 1;
        }

        while(1)
        {
                if(has_tobacco == 1)                                                     //If smoker has Tobacco
                {
                        sem_wait(&semaphore);
                        if(paper == 1 && matches == 1)            //If needed ingredients present take it
                        {
                                paper = 0;
                                matches = 0;
                                empty=1;
                                sem_post(&semaphore);
                                printf("Smoker %d grabbed paper and matches and is smoking\n",smoker_num);
                                smoke_time = randomnum(3,6);                       //Smokes for random time
                                sleep(smoke_time);
                        }
                        else
                        {
                                sem_post(&semaphore);
                        }
                }
                else if(has_paper == 1)                                                  //If smoker has Paper
                {
                        sem_wait(&semaphore);
                        if(tobacco == 1 && matches == 1)          //If needed ingredients present take it
                        {
                                tobacco = 0;
                                matches = 0;
                                empty=1;
                                sem_post(&semaphore);
                                printf("Smoker %d grabbed tobacco and matches and is smoking\n",smoker_num);
                                smoke_time = randomnum(3,6);                       //Smokes for random time
                                sleep(smoke_time);
                        }
                        else
                        {
                                sem_post(&semaphore);
                        }
                }
                else if(has_matches == 1)                                        //If smoker has Matches
                {
                        sem_wait(&semaphore);
                        if(tobacco == 1 && paper == 1)            //If needed ingredients present take it
                        {
                                tobacco = 0;
                                paper = 0;
                                empty=1;
                                sem_post(&semaphore);
                                printf("Smoker %d grabbed tobacco and papers is smoking\n",smoker_num);
                                smoke_time = randomnum(3,6);                       //Smokes for random time
                                sleep(smoke_time);
                        }
                        else
                        {
                                sem_post(&semaphore);
                        }
                }
        }
}


//To Generate random number in range
int randomnum(int upper,int lower){
        return rand()%(upper-lower+1)+lower;                    //Generating random number in range
}
