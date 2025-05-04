#ifndef HEADER_FILE
#define HEADER_FILE

struct element {
  int num_edition;
  int id_belt;
  int last;
  struct element *next;
};

struct queue* queue_init (int size, int belt);
/*
    * To create a queue
    * @param size: size of the queue
    * @param belt: id of the belt
    * @return: pointer to the first element of the queue
*/
int queue_destroy (struct queue *q);
/*
    * To destroy the queue and free the resources
    * @return: 0 if success, -1 if error
*/
int queue_put(struct queue *q, struct element *ele);
/*
    * To Enqueue an element
    * @param elem: pointer to the element to be added
    * @return: 0 if success, -1 if error
*/
struct element * queue_get(struct queue *q);
/*
    * To extract elements from the queue
    * @return: pointer to the element dequeued
*/
int queue_empty (struct queue *q);
/*
    * To check if the queue is empty
    * @return: 1 if empty, 0 if not
*/
int queue_full(struct queue *q);
/*
    * To check if the queue is full
    * @return: 1 if full, 0 if not (at least one element is free)
*/

#endif