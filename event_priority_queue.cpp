#include "simulation.h"
#include <stdio.h>
#include <stdlib.h>

PriorityQueue* pq_create_queue() {
	PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
	if (pq) {
		pq->head = NULL;
		pq->size = 0;
		if (pq) return pq;
	}
	fprintf(stderr, "Failed to create PriorityQueue\nSimulation Ending\n");
	exit(1);
	return NULL; // change this to a nice error msg	
}

PQNode* pq_create_node(Event* event) {
	PQNode* n = (PQNode*)malloc(sizeof(PQNode));
	if (n) {
		n->event = event;
		n->priority = event->time;
		n->next = NULL;
		if (n) return n;
	}
	fprintf(stderr, "Failed to create Node\nSimulation Ending\n");
	exit(1);
	return NULL; // change this to a nice error msg
}

Event* pq_pop(PriorityQueue* pq) {
	PQNode* temp = pq->head;
	Event* event = temp->event;
	pq->head = temp->next;
	pq->size--;
	free(temp);
	return event;
}

void pq_push(PriorityQueue* pq, Event* event) {
	PQNode* temp = pq->head;
	PQNode* new_node = pq_create_node(event);
	if (temp != NULL) {
		// create the PQ
		if (new_node->priority < temp->priority) {
			new_node->next = temp;
			pq->head = new_node;
			//free(temp); // figure out if this will cause damage
		}
		else {
			// write a helper fn() for these guys
			while (temp->next != NULL && temp->next->priority < new_node->priority) {
				temp = temp->next;
			}
			// found insertion point
			new_node->next = temp->next;
			temp->next = new_node;
			//free(temp);
		}
	}
	else {
		pq->head = new_node;
	}
	pq->size++;
}

bool pq_queue_empty(PriorityQueue* pq) {
	return (pq->head == NULL);
}