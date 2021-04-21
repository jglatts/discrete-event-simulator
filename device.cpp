#include "simulation.h"
#include <stdio.h>
#include <stdlib.h>

Device* create_device(device_type dev_type, int dev_max, int dev_min) {
	Device* d = (Device*)malloc(sizeof(Device));
	if (d) {
		d->queue = dev_create_queue();
		d->current_event = NULL;
		d->type = dev_type;
		d->max = dev_max;
		d->min = dev_min;
		d->idle = true;
		return d;
	}
	fprintf(stderr, "Failed to create device\nSimulation Ending\n");
	exit(1);
	return NULL;
}

void device_set_seed(int seed) {
	srand(seed);
}

void device_handler(Device* dev, Event* event, PriorityQueue* pq, int time) {
	switch (dev->type)
	{
	case CPU:
		process_cpu(dev, event, pq, time);
		break;
	case DISK1:
		process_disk_one(dev, event, pq, time);
		break;
	case DISK2:
		process_disk_two(dev, event, pq, time);
		break;
	case NETWORK:
		process_network(dev, event, pq, time);
		break;
	default:
		break;
	}
}

void process_cpu(Device* dev, Event* event, PriorityQueue* pq, int time) {
	if (event->type == CPU_FINISH) {
		event->time = time;
		dev->current_event = NULL;
		dev->idle = true;
	}
	if (dev_queue_empty(dev->queue) && event->type == CPU_FINISH) return;
	// start to work on the next job
	if (!dev_queue_empty(dev->queue) && dev->idle) {
		Event* job_to_run = dev_pop(dev->queue);
		int fin_time = time + (rand() % (dev->max - dev->min + 1) + dev->min);
		Event* event_to_fin = create_event(CPU_FINISH, job_to_run->id, fin_time);
		pq_push(pq, event_to_fin);
		// make sure we're not adding same event twice
		if (event->type != CPU_FINISH) dev_push(dev->queue, event);
		dev->current_event = job_to_run;
		dev->idle = false;
		//printf("CPU Started EventID %d and EventID %d is leaving\n", job_to_run->id, event->id);
	}
	else if (dev->idle) {
		int fin_time = time + (rand() % (dev->max - dev->min + 1) + dev->min);
		Event* event_to_fin = create_event(CPU_FINISH, event->id, fin_time);
		pq_push(pq, event_to_fin);
		//printf("CPU is Working on EventID %d\n", event->id);
		//printf("Added finish time at %d\n", event_to_fin->time);
		//fprintf(LOG_FILE, "EventId %d Time %d\n", temp->event->id, temp->event->time);
		// add this event to the device's current event
		dev->current_event = event;
		dev->idle = false;
	}
	// CPU is busy so just ad to queue
	else {
		dev_push(dev->queue, event);
		//printf("Added EventID %d to CPU Queue\n", event->id);
	}
}

void process_disk_one(Device* dev, Event* event, PriorityQueue* pq, int time) {
	if (event->type == DISK1_FINISH) {
		event->time = time;	// update the job time when it's finished
		dev->current_event = NULL;
		dev->idle = true;
	}
	if (dev_queue_empty(dev->queue) && event->type == DISK1_FINISH) return;
	// start to work on the next job
	if (!dev_queue_empty(dev->queue) && dev->idle) {
		Event* job_to_run = dev_pop(dev->queue);
		int fin_time = time + (rand() % (dev->max - dev->min + 1) + dev->min);
		Event* event_to_fin = create_event(DISK1_FINISH, job_to_run->id, fin_time);
		pq_push(pq, event_to_fin);
		if (event->type != DISK1_FINISH) dev_push(dev->queue, event);
		dev->current_event = job_to_run;
		dev->idle = false;
		//printf("DISK1 Started EventID %d and EventID %d is leaving\n", job_to_run->id, event->id);
	}
	else if (dev->idle) {
		int fin_time = time + (rand() % (dev->max - dev->min + 1) + dev->min);
		Event* event_to_fin = create_event(DISK1_FINISH, event->id, fin_time);
		pq_push(pq, event_to_fin);
		//printf("DISK1 is Working on EventID %d\n", event->id);
		//printf("Added finish time at %d\n", event_to_fin->time);
		// add this event to the device's current event
		dev->current_event = event;
		dev->idle = false;
	}
	else {
		dev_push(dev->queue, event);
		//printf("Added EventID %d to DISK1 Queue\n", event->id);
	}
}

void process_disk_two(Device* dev, Event* event, PriorityQueue* pq, int time) {
	if (event->type == DISK2_FINISH) {
		event->time = time;	// update the job time when it's finished
		dev->current_event = NULL;
		dev->idle = true;
	}
	if (dev_queue_empty(dev->queue) && event->type == DISK2_FINISH) return;
	// start to work on the next job
	if (!dev_queue_empty(dev->queue) && dev->idle) {
		Event* job_to_run = dev_pop(dev->queue);
		int fin_time = time + (rand() % (dev->max - dev->min + 1) + dev->min);
		Event* event_to_fin = create_event(DISK2_FINISH, job_to_run->id, fin_time);
		pq_push(pq, event_to_fin);
		if (event->type != DISK2_FINISH) dev_push(dev->queue, event);
		dev->current_event = job_to_run;
		dev->idle = false;
		//printf("DISK2 Started EventID %d and EventID %d is leaving\n", job_to_run->id, event->id);
	}
	else if (dev->idle) {
		int fin_time = time + (rand() % (dev->max - dev->min + 1) + dev->min);
		Event* event_to_fin = create_event(DISK2_FINISH, event->id, fin_time);
		pq_push(pq, event_to_fin);
		//printf("DISK2 is Working on EventID %d\n", event->id);
		//printf("Added finish time at %d\n", event_to_fin->time);
		// add this event to the device's current event
		dev->current_event = event;
		dev->idle = false;
	}
	else {
		dev_push(dev->queue, event);
		//printf("Added EventID %d to DISK2 Queue\n", event->id);
	}
}

void process_network(Device* dev, Event* event, PriorityQueue* pq, int time) {
	if (event->type == NETWORK_FINISH) {
		event->time = time;	// update the job time when it's finished
		dev->current_event = NULL;
		dev->idle = true;
	}
	if (dev_queue_empty(dev->queue) && event->type == NETWORK_FINISH) return;
	// start to work on the next job
	if (!dev_queue_empty(dev->queue) && dev->idle) {
		Event* job_to_run = dev_pop(dev->queue);
		int fin_time = time + (rand() % (dev->max - dev->min + 1) + dev->min);
		Event* event_to_fin = create_event(NETWORK_FINISH, job_to_run->id, fin_time);
		pq_push(pq, event_to_fin);
		if (event->type != NETWORK_FINISH) dev_push(dev->queue, event);
		dev->current_event = job_to_run;
		dev->idle = false;
		//printf("NETWORK Started EventID %d and EventID %d is leaving\n", job_to_run->id, event->id);
	}
	else if (dev->idle) {
		int fin_time = time + (rand() % (dev->max - dev->min + 1) + dev->min);
		Event* event_to_fin = create_event(NETWORK_FINISH, event->id, fin_time);
		pq_push(pq, event_to_fin);
		//printf("NETWORK is Working on EventID %d\n", event->id);
		//printf("Added finish time at %d\n", event_to_fin->time);
		// add this event to the device's current event
		dev->current_event = event;
		dev->idle = false;
	}
	else {
		dev_push(dev->queue, event);
		//printf("Added EventID %d to NETWORK Queue\n", event->id);
	}
}
