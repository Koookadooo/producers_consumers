#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>          
#include <sys/stat.h>        
#include <unistd.h>
#include "eventbuf.h"

struct eventbuf *eb;

sem_t *mutex, *items, *spaces;

typedef struct {
    int id;
    int count;
} thread_param;

void *producer(void *param) {
    thread_param *p = (thread_param *)param;
    int event_id;

    for (int i = 0; i < p->count; i++) {
        event_id = p->id * 100 + i;

        sem_wait(spaces); // Wait for space
        sem_wait(mutex); // Lock the buffer

        printf("P%d: adding event %d\n", p->id, event_id);
        eventbuf_add(eb, event_id); // Add event to the buffer

        sem_post(mutex); // Unlock the buffer
        sem_post(items); // Signal that new item is added
    }

    printf("P%d: exiting\n", p->id);
    pthread_exit(NULL);
}

void *consumer(void *param) {
    thread_param *p = (thread_param *)param;
    int event_id;

    while (1) {
        sem_wait(items); // Wait for item
        sem_wait(mutex); // Lock the buffer

        if (eventbuf_empty(eb)) { // Check if the buffer is empty
            sem_post(mutex); // Unlock the buffer if it's time to exit
            break;
        }

        event_id = eventbuf_get(eb); // Get an event from the buffer
        printf("C%d: got event %d\n", p->id, event_id);

        sem_post(mutex); // Unlock the buffer
        sem_post(spaces); // Signal that space is freed
    }

    printf("C%d: exiting\n", p->id);
    pthread_exit(NULL);
}

sem_t *sem_open_temp(const char *name, int value)
{
    sem_t *sem;

    // Create the semaphore
    if ((sem = sem_open(name, O_CREAT, 0600, value)) == SEM_FAILED)
        return SEM_FAILED;

    // Unlink it so it will go away after this process exits
    if (sem_unlink(name) == -1) {
        sem_close(sem);
        return SEM_FAILED;
    }

    return sem;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: ./pc num_producers num_consumers num_events max_outstanding\n");
        return 1;
    }

    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);
    int num_events = atoi(argv[3]);
    int max_outstanding = atoi(argv[4]);

    // Initialize semaphores
    mutex = sem_open_temp("mutex", 1);
    items = sem_open_temp("items", 0);
    spaces = sem_open_temp("spaces", max_outstanding);

    pthread_t producers[num_producers], consumers[num_consumers];
    thread_param p_params[num_producers], c_params[num_consumers];

    eb = eventbuf_create(); // Create the event buffer

    // Create producer threads
    for (int i = 0; i < num_producers; i++) {
        p_params[i].id = i;
        p_params[i].count = num_events;
        pthread_create(&producers[i], NULL, producer, (void *)&p_params[i]);
    }

    // Create consumer threads
    for (int i = 0; i < num_consumers; i++) {
        c_params[i].id = i;
        pthread_create(&consumers[i], NULL, consumer, (void *)&c_params[i]);
    }

    // Wait for all producers to complete
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }

    // Post to items semaphore to wake up consumers to finish
    for (int i = 0; i < num_consumers; i++) {
        sem_post(items);
    }

    // Wait for all consumers to complete
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], NULL);
    }

    // Clean up
    eventbuf_free(eb); // Free the event buffer
    sem_close(mutex);
    sem_close(items);
    sem_close(spaces);

    return 0;
}
