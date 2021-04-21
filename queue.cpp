#include "simulation.h"
#include <stdio.h>
#include <stdlib.h>

Node* dev_create_node(Event* event) {
	Node* n = (Node*)malloc(sizeof(Node));
	if (n) {
		n->event = event;
		n->next = NULL;
		if (n) return n;
	}
	fprintf(stderr, "Failed to create Node\nSimulation Ending\n");
	exit(1);
	return NULL; // change this to a nice error msg
}

Queue* dev_create_queue() {
	Queue* q = (Queue*)malloc(sizeof(Queue));
	if (q) {
		q->head = NULL;
		q->size = 0;
		if (q) return q;
	}
	fprintf(stderr, "Failed to create Queue\nSimulation Ending\n");
	exit(1);
	return NULL; // change this to a nice error msg
} 

void dev_push(Queue* queue, Event* event) {
	Node* temp = queue->head;
	if (temp != NULL) {
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = dev_create_node(event);
	} 
	else {
		queue->head = dev_create_node(event);
	}
	queue->size++;
}

Event* dev_pop(Queue* queue) {
	Node* temp = queue->head;
	Event* event = temp->event;
	queue->head = temp->next;
	free(temp);
	queue->size--;
	return event;
}

int dev_get_shortest_queue(Queue* d_one, Queue* d_two) {
	if (d_one->size == d_two->size) return 0;
	if (d_one->size < d_two->size)  return 1;
	return 2;
}

bool dev_queue_empty(Queue* queue) {
	return (queue->head == NULL);
}
