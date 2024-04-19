# producers_consumers# Producer-Consumer Problem

## Building

**Command line:**

- `make` to build. An executable called `pc` will be produced.

## Using the executable:

To run the program:

- `num_producers`: Number of producers (threads) generating events.
- `num_consumers`: Number of consumers (threads) consuming events.
- `num_events`: Number of events each producer will generate.
- `max_outstanding`: Maximum number of outstanding (produced but not yet consumed) events allowed at a time.

**Example:** `./pc 2 4 5 2`

This command runs the program with 2 producers and 4 consumers. Each producer generates 5 events, and the system allows a maximum of 2 events outstanding at a time.

## Files

- `pc.c`: Main program source file that includes the producer and consumer logic.
- `eventbuf.c`: Contains the definitions of functions to manage event buffers.
- `eventbuf.h`: Header file for `eventbuf.c`.
- `Makefile`: File which builds the executable.

## Data

This program uses a FIFO queue (`eventbuf`) to manage events:

- Events are integers uniquely identified by `producer_number * 100 + event_number`.

## Functions

**Overview of functions and their hierarchy:**

### `main()`

- Initializes semaphores and event buffer.
- Creates and starts producer and consumer threads.
- Waits for all threads to complete and cleans up resources.

### `producer(void *param)`

- Produces events and adds them to the event buffer.
- Uses semaphores to manage access to the buffer and coordinate with consumers.

### `consumer(void *param)`

- Consumes events from the event buffer as they become available.
- Uses semaphores to synchronize access to the buffer with producers.

### Event Buffer Functions (`eventbuf.h`)

- `eventbuf_create()`: Creates and initializes a new event buffer.
- `eventbuf_free()`: Frees the event buffer.
- `eventbuf_add()`: Adds a new event to the buffer.
- `eventbuf_get()`: Retrieves and removes an event from the buffer.
- `eventbuf_empty()`: Checks if the buffer is empty.

## Synchronization

- The system uses three semaphores:
  - `mutex`: Ensures mutual exclusion for access to the event buffer.
  - `items`: Counts the number of items (events) in the buffer.
  - `spaces`: Counts the number of available spaces in the buffer for new events.

## Notes

- The project simulates a producer-consumer scenario using threads and semaphores, which is useful in understanding synchronization in operating systems and concurrent programming.
