#include "simulation.h"
#include <stdio.h>
#include <stdlib.h>

Event* create_event(event_type type, int id, int time) {
	Event* new_event = (Event*)malloc(sizeof(Event));
	if (new_event) {
		new_event->type = type;
		new_event->id = id;
		new_event->time = time;
		return new_event;
	}
	fprintf(stderr, "Failed to create Event\nSimulation Ending\n");
	exit(1);
}