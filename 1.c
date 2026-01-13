#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define NUM_EVENTS 8
#define MAX_CONCURRENT_EVENTS 3

#define PROPOSAL_SPOTS 3
#define DECOR_KITS 3
#define AV_KITS 4

typedef struct {
    int id;
    int planner_id;
    int need_proposal;
    int need_decor;
    int need_av;
    int duration;
} event_t;

event_t events[NUM_EVENTS];
sem_t sem_proposal, sem_decor, sem_av;
sem_t concurrent_events_sem;
pthread_mutex_t log_mutex;
int active_events = 0;

void* event_thread(void* arg) {
    event_t* ev = (event_t*)arg;
    
    pthread_mutex_lock(&log_mutex);
    printf("[WAITING] Event %d (Planner %d) - needs: P=%d, D=%d, AV=%d, dur=%ds\n",
           ev->id, ev->planner_id, ev->need_proposal, 
           ev->need_decor, ev->need_av, ev->duration);
    pthread_mutex_unlock(&log_mutex);
    
    sem_wait(&concurrent_events_sem);
    
    pthread_mutex_lock(&log_mutex);
    active_events++;
    printf("[ACQUIRED] Event %d got slot! Active events: %d/%d\n", 
           ev->id, active_events, MAX_CONCURRENT_EVENTS);
    pthread_mutex_unlock(&log_mutex);
    
    
    if (ev->need_proposal) {
        sem_wait(&sem_proposal);
        pthread_mutex_lock(&log_mutex);
        printf("  Event %d acquired proposal spot\n", ev->id);
        pthread_mutex_unlock(&log_mutex);
    }
    
    for (int d = 0; d < ev->need_decor; d++) {
        sem_wait(&sem_decor);
        pthread_mutex_lock(&log_mutex);
        printf("  Event %d acquired decor kit %d/%d\n", ev->id, d+1, ev->need_decor);
        pthread_mutex_unlock(&log_mutex);
    }
    
    for (int a = 0; a < ev->need_av; a++) {
        sem_wait(&sem_av);
        pthread_mutex_lock(&log_mutex);
        printf("  Event %d acquired AV kit %d/%d\n", ev->id, a+1, ev->need_av);
        pthread_mutex_unlock(&log_mutex);
    }
    
    
    pthread_mutex_lock(&log_mutex);
    printf("[RUNNING] Event %d (Planner %d) - All resources acquired! Executing for %d seconds...\n",
           ev->id, ev->planner_id, ev->duration);
    pthread_mutex_unlock(&log_mutex);
    
    
    sleep(ev->duration);
    
    
    if (ev->need_proposal) {
        sem_post(&sem_proposal);
        pthread_mutex_lock(&log_mutex);
        printf("  Event %d released proposal spot\n", ev->id);
        pthread_mutex_unlock(&log_mutex);
    }
    
    for (int d = 0; d < ev->need_decor; d++) {
        sem_post(&sem_decor);
        pthread_mutex_lock(&log_mutex);
        printf("  Event %d released decor kit %d/%d\n", ev->id, d+1, ev->need_decor);
        pthread_mutex_unlock(&log_mutex);
    }
    
    for (int a = 0; a < ev->need_av; a++) {
        sem_post(&sem_av);
        pthread_mutex_lock(&log_mutex);
        printf("  Event %d released AV kit %d/%d\n", ev->id, a+1, ev->need_av);
        pthread_mutex_unlock(&log_mutex);
    }
    
    
    pthread_mutex_lock(&log_mutex);
    active_events--;
    printf("[COMPLETED] Event %d finished! Active events now: %d/%d\n", 
           ev->id, active_events, MAX_CONCURRENT_EVENTS);
    pthread_mutex_unlock(&log_mutex);
    
    sem_post(&concurrent_events_sem);
    
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_t event_threads[NUM_EVENTS];
    
    sem_init(&sem_proposal, 0, PROPOSAL_SPOTS);
    sem_init(&sem_decor, 0, DECOR_KITS);
    sem_init(&sem_av, 0, AV_KITS);
    sem_init(&concurrent_events_sem, 0, MAX_CONCURRENT_EVENTS);
    pthread_mutex_init(&log_mutex, NULL);
    
    printf("===========================================\n");
    printf("   EVENT-THREAD MANAGEMENT SYSTEM\n");
    printf("===========================================\n\n");
    
    printf("SYSTEM CAPACITY:\n");
    printf(" Maximum concurrent events: %d\n", MAX_CONCURRENT_EVENTS);
    printf(" Available resources:\n");
    printf("  - Proposal spots: %d\n", PROPOSAL_SPOTS);
    printf("  - Decor kits: %d\n", DECOR_KITS);
    printf("  - AV kits: %d\n\n", AV_KITS);
    
    printf("GENERATING %d EVENTS:\n", NUM_EVENTS);
    printf("-------------------\n");
    
    
    for (int i = 0; i < NUM_EVENTS; i++) {
        events[i].id = i + 1;
        events[i].planner_id = 1 + rand() % 2; 
        events[i].need_proposal = rand() % 2;
        events[i].need_decor = 1 + rand() % 2;
        events[i].need_av = 1 + rand() % 2;
        events[i].duration = 1 + rand() % 2;
        
       
    }
    
    printf("\n===========================================\n");
    printf("   EXECUTION START - Only %d at a time!\n", MAX_CONCURRENT_EVENTS);
    printf("===========================================\n\n");
    
    // Create threads for all events
    for (int i = 0; i < NUM_EVENTS; i++) {
        pthread_create(&event_threads[i], NULL, event_thread, &events[i]);
        usleep(500000); 
    }
    
    // Wait for all event threads to complete
    for (int i = 0; i < NUM_EVENTS; i++) {
        pthread_join(event_threads[i], NULL);
    }
    
    
    sem_destroy(&sem_proposal);
    sem_destroy(&sem_decor);
    sem_destroy(&sem_av);
    sem_destroy(&concurrent_events_sem);
    pthread_mutex_destroy(&log_mutex);
    
    printf("\n===========================================\n");
    printf("   ALL %d EVENTS COMPLETED SUCCESSFULLY!\n", NUM_EVENTS);
    printf("===========================================\n");
    
    return 0;
}