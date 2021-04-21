#include "simulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>    

#define CONFIG_PARAMS 15

// simulation core objects
PriorityQueue* pq = NULL;
Device* CPU_DEV = NULL;
Device* DISK1_DEV = NULL;
Device* DISK2_DEV = NULL;
Device* NETWORK_DEV = NULL;
FILE*   LOG_FILE = NULL;
FILE*   STATS_FILE = NULL;
// simulation core variables
int config_params[CONFIG_PARAMS];
int SEED;
int INIT_TIME;
int FIN_TIME;
int ARRIVE_MIN;
int ARRIVE_MAX;
int QUIT_PROB;
int NET_PROB;
int CPU_MIN;
int CPU_MAX;
int DISK1_MIN;
int DISK1_MAX;
int DISK2_MIN;
int DISK2_MAX;
int NETWORK_MIN;
int NETWORK_MAX;
int sim_time;
int job_id = 1;
bool flip = true;

bool init_simulation() {
	// init priority queue and devices
	LOG_FILE = fopen("log.txt", "w");
	STATS_FILE = fopen("stats.txt", "w");
	pq = pq_create_queue();
	if (read_config_file()) {
		CPU_DEV = create_device(CPU, CPU_MAX, CPU_MIN);
		DISK1_DEV = create_device(DISK1, DISK1_MAX, DISK1_MIN);
		DISK2_DEV = create_device(DISK2, DISK2_MAX, DISK2_MAX);
		NETWORK_DEV = create_device(NETWORK, NETWORK_MAX, NETWORK_MIN);
		sim_time = 0;
		job_id = 1;
		return true;    
	}
	return false;
}

void print_queue() {
	PQNode* temp = pq->head;
	fprintf(LOG_FILE, "%s\n", "Printing out Priority Queue");
	while (temp) {
		//printf("EventId %d Time %d\n", temp->event->id, temp->event->time);
		fprintf(LOG_FILE, "EventId %d Time %d\n", temp->event->id, temp->event->time);
		temp = temp->next;
	}
}

bool run_simulation() {
	if (!init_simulation()) return false;	
	// main simulation loop
	puts("\n\tSimulation Running");
	while (!pq_queue_empty(pq)) {
		fprintf(LOG_FILE, "\n\nCurrent Simulation Time = %d\n", sim_time);
		print_queue();
		Event* next_event = pq_pop(pq); // pop the next job to processs
		sim_time = next_event->time;    // update the simulation time
		// handle the job
		switch (next_event->type)
		{
		case NEW_JOB:
			//printf("Job Type = NEW_JOB: EventID = %d Time = %d\n", next_event->id, next_event->time);
			fprintf(LOG_FILE, "Job Type = NEW_JOB: EventID = %d Time = %d\n", next_event->id, next_event->time);
			add_new_job();
			break;
		case CPU_ARRIVAL:
			//printf("Job Type = CPU_ARRIVAL:  EventID = %d Time = %d\n", next_event->id, next_event->time);
			fprintf(LOG_FILE, "Job Type = CPU_ARRIVAL:  EventID = %d Time = %d\n", next_event->id, next_event->time);
			device_handler(CPU_DEV, next_event, pq, sim_time);
			break;
		case CPU_FINISH:
			//printf("Job Type = CPU_FINISH: EventID = %d Time = %d\n", next_event->id, next_event->time);
			fprintf(LOG_FILE, "Job Type = CPU_FINISH: EventID = %d Time = %d\n", next_event->id, next_event->time);
			device_handler(CPU_DEV, next_event, pq, sim_time);
			send_job_from_cpu(next_event);
			break;
		case DISK1_ARRIVAL:
			//printf("Job Type = DISK1_ARRIVAL: EventID = %d Time = %d\n", next_event->id, next_event->time);
			fprintf(LOG_FILE, "Job Type = DISK1_ARRIVAL: EventID = %d Time = %d\n", next_event->id, next_event->time);
			device_handler(DISK1_DEV, next_event, pq, sim_time);
			break;
		case DISK1_FINISH:
			//printf("Job Type = DISK1_FINISH: EventID = %d Time = %d\n", next_event->id, next_event->time);
			fprintf(LOG_FILE, "Job Type = DISK1_FINISH: EventID = %d Time = %d\n", next_event->id, next_event->time);
			device_handler(DISK1_DEV, next_event, pq, sim_time);
			send_job_to_cpu(next_event);
			break;
		case DISK2_ARRIVAL:
			//printf("Job Type = DISK2_ARRIVAL: EventID = %d Time = %d\n", next_event->id, next_event->time);
			device_handler(DISK2_DEV, next_event, pq, sim_time);
			break;
		case DISK2_FINISH:
			//printf("Job Type = DISK2_FINISH: EventID = %d Time = %d\n", next_event->id, next_event->time);
			fprintf(LOG_FILE, "Job Type = DISK2_FINISH: EventID = %d Time = %d\n", next_event->id, next_event->time);
			device_handler(DISK2_DEV, next_event, pq, sim_time);
			send_job_to_cpu(next_event);
			break;
		case NETWORK_ARRIVAL:
			//printf("Job Type = NETWORK_ARRIVAL: EventID = %d Time = %d\n", next_event->id, next_event->time);
			fprintf(LOG_FILE, "Job Type = NETWORK_ARRIVAL: EventID = %d Time = %d\n", next_event->id, next_event->time);
			device_handler(NETWORK_DEV, next_event, pq, sim_time);
			break;
		case NETWORK_FINISH:
			//printf("Job Type = NEW_JOB: NETWORK FINISH = %d Time = %d\n", next_event->id, next_event->time);
			fprintf(LOG_FILE, "Job Type = NETWORK FINISH: EventID = %d Time = %d\n", next_event->id, next_event->time);
			device_handler(NETWORK_DEV, next_event, pq, sim_time);
			send_job_to_cpu(next_event);
			break;
		case SIM_FIN:
			//printf("Job Type = SIM_FIN: EventID = %d Time = %d\n", next_event->id, next_event->time);
			fprintf(LOG_FILE, "Job Type = SIM_FIN: EventID = %d Time = %d\n", next_event->id, next_event->time);
			fclose(LOG_FILE);
			fclose(STATS_FILE);
			puts("\n\tSimulation Complete -> Please See log.txt and stats.txt for results\n");
			exit(1);
			break;
		default:
			break;
		}
		write_to_stats_file();
		write_to_log_file();
	}
	return true;   
}

void write_to_stats_file() {
	int avg = (pq->size + CPU_DEV->queue->size + DISK1_DEV->queue->size
		      + DISK2_DEV->queue->size + NETWORK_DEV->queue->size) / 5;
	fprintf(STATS_FILE,	"\n\n%s\n", ".......Examining Device Queues.......");
	fprintf(STATS_FILE, "Current Simulation Time   = %d\n", sim_time);
	fprintf(STATS_FILE, "Event Priority Queue Size = %d\n", pq->size);
	fprintf(STATS_FILE, "CPU Queue Size            = %d\n", CPU_DEV->queue->size);
	fprintf(STATS_FILE, "DISK1 Queue Size          = %d\n", DISK1_DEV->queue->size);
	fprintf(STATS_FILE, "DISK2 Queue Size          = %d\n", DISK2_DEV->queue->size);
	fprintf(STATS_FILE, "NETWORK Queue Size        = %d\n", NETWORK_DEV->queue->size);
	fprintf(STATS_FILE, "AVERAGE Queue Size        = %d\n", avg);
	fprintf(STATS_FILE, "IS CPU IDLE?              = %d\n", CPU_DEV->idle);
	if (!CPU_DEV->idle) {
		fprintf(STATS_FILE, "\tCPU is working on job# %d\n", CPU_DEV->current_event->id);
	}
	fprintf(STATS_FILE, "IS DISK1 IDLE?            = %d\n", DISK1_DEV->idle);
	if (!DISK1_DEV->idle) {
		fprintf(STATS_FILE, "\tDISK1 is working on job# %d\n", DISK1_DEV->current_event->id);
	}
	fprintf(STATS_FILE, "IS DISK2 IDLE?            = %d\n", DISK2_DEV->idle);
	if (!DISK2_DEV->idle) {
		fprintf(STATS_FILE, "\tDISK2 is working on job# %d\n", DISK2_DEV->current_event->id);
	}
	fprintf(STATS_FILE, "IS NETWORK IDLE?          = %d\n", NETWORK_DEV->idle);
	if (!NETWORK_DEV->idle) {
		fprintf(STATS_FILE, "\tNETWORK is working on job# %d\n", NETWORK_DEV->current_event->id);
	}
}

void write_to_log_file() {
	if (!CPU_DEV->idle) {
		fprintf(LOG_FILE, "CPU is working on job# %d\n", CPU_DEV->current_event->id);
	}
	if (!DISK1_DEV->idle) {
		fprintf(LOG_FILE, "DISK1 is working on job# %d\n", DISK1_DEV->current_event->id);
	}
	if (!DISK2_DEV->idle) {
		fprintf(LOG_FILE, "DISK2 is working on job# %d\n", DISK2_DEV->current_event->id);
	}
	if (!NETWORK_DEV->idle) {
		fprintf(LOG_FILE, "NETWORK is working on job# %d\n", NETWORK_DEV->current_event->id);
	}
}

void send_job_from_cpu(Event* event) {
	// figure out where to send the job
	int val = (rand() % 10) + 1;
	int quit_min = QUIT_PROB / 10;
	int net_min = NET_PROB / 10;
	if (net_min <= quit_min) {
		if (val >= 1 && val <= net_min)
			send_job_to_network(event);
		else if (val >= net_min + 1 && val <= net_min + quit_min)
			quit_job(event);
		else
			send_job_to_disk(event);
	}
	else {
		if (val >= 1 && val <= quit_min)
			quit_job(event);
		else if (val >= quit_min + 1 && val <= net_min + quit_min)
			send_job_to_network(event);
		else
			send_job_to_disk(event);
	}
}

void send_job_to_cpu(Event* event) {
	// job has finished on a device and is ready for the cpu queue
	//printf("Sent EventID %d to CPU\n", event->id);
	fprintf(LOG_FILE, "Sent EventID %d to CPU\n", event->id);
	Event* e = create_event(CPU_ARRIVAL, event->id, event->time);
	pq_push(pq, e);
}

void send_job_to_network(Event* event) {
	//printf("Sent EventID %d to NETWORK\n", event->id);
	fprintf(LOG_FILE, "Sent EventID %d to NETWORK\n", event->id);
	Event* e = create_event(NETWORK_ARRIVAL, event->id, event->time);
	pq_push(pq, e);
}

void send_job_to_disk(Event* event) {
	// figure out which queue is shortest
	int shortest = 0;
	if ((shortest = dev_get_shortest_queue(DISK1_DEV->queue, DISK2_DEV->queue)) == 0) {
		//printf("The DISK queues have equal size\n");
		//printf("Disk Queue Size = %d\n", DISK1_DEV->queue->size);
		fprintf(LOG_FILE, "The DISK queues have equal size\n");
		fprintf(LOG_FILE, "Disk Queue Size = %d\n", DISK1_DEV->queue->size);
		// queue sizes are equal so add to random one 
		if (flip) {
			Event* e = create_event(DISK1_ARRIVAL, event->id, event->time);
			pq_push(pq, e);
			//printf("Just sent EventID %d to DISK1", event->id);
			fprintf(LOG_FILE, "Just sent EventID %d to DISK1", event->id);
			flip = false;
		}
		else {
			Event* e = create_event(DISK2_ARRIVAL, event->id, event->time);
			pq_push(pq, e);
			//printf("Just sent EventID %d to DISK2", event->id);
			fprintf(LOG_FILE, "Just sent EventID %d to DISK2", event->id);
			flip = true;
		}
	}
	else if (shortest == 1) {
		//printf("DISK1 has shorter size\n");
		//printf("DISK1 Size = %d DISK2 Size = %d\n", DISK1_DEV->queue->size, DISK2_DEV->queue->size);
		fprintf(LOG_FILE, "DISK1 has shorter size\n");
		fprintf(LOG_FILE, "DISK1 Size = %d DISK2 Size = %d\n", DISK1_DEV->queue->size, DISK2_DEV->queue->size);
		Event* e = create_event(DISK1_ARRIVAL, event->id, event->time);
		pq_push(pq, e);
		//printf("Just sent EventID %d to DISK1", event->id);
		fprintf(LOG_FILE, "Just sent EventID %d to DISK1", event->id);
	}
	else {
		//printf("DISK2 has shorter size\n");
		//printf("DISK1 Size = %d DISK2 Size = %d\n", DISK1_DEV->queue->size, DISK2_DEV->queue->size);
		fprintf(LOG_FILE, "DISK2 has shorter size\n");
		fprintf(LOG_FILE, "DISK1 Size = %d DISK2 Size = %d\n", DISK1_DEV->queue->size, DISK2_DEV->queue->size);
		Event* e = create_event(DISK2_ARRIVAL, event->id, event->time);
		pq_push(pq, e);
		//printf("Just sent EventID %d to DISK2", event->id);
	}
}
void quit_job(Event* event) {
	// add more info to the log file
	//printf("EventID %d is terminating\n", event->id);
	fprintf(LOG_FILE, "EventID %d is terminating\n", event->id);
	free(event);
}

void add_new_job() {
	// variables to hold the jobID and time of simulation when this function was called
	int old_time = sim_time;
	int old_id = job_id;
	// create the new job and add it to the pq
	int new_job_time = sim_time + (rand() % (ARRIVE_MAX - ARRIVE_MIN + 1) + ARRIVE_MIN);
	//printf("Now Job Time = %d\nOld Time %d\n", new_job_time, old_time);
	fprintf(LOG_FILE, "Now Job Time = %d\nOld Time %d\n", new_job_time, old_time);
	Event* new_event = create_event(NEW_JOB, ++job_id, new_job_time);
	pq_push(pq, new_event);
	// add a CPU_ARRIVAL job to the pq whenever we pulled a new job from the pq
	Event* cpu_event = create_event(CPU_ARRIVAL, old_id, old_time);
	pq_push(pq, cpu_event);
}

bool read_config_file() {
	FILE* fp = NULL;
	int count = 0;
	if ((fp = fopen("config.txt", "r")) != NULL) {
		while (!feof(fp)) {
			char* buff = NULL;
			size_t len = 0;
			getline(&buff, &len, fp);
			char* token = strtok(buff, " ");
			while (token != NULL) {
				token = strtok(NULL, " ");
				config_params[count++] = atoi(token);
				break;
			}
		}
	}
	else {
		// make better error msg here
		printf("Failed to open config.txt file");
		return false;
	}
	// initiliaze the core simulation variables
	load_from_config_file();
	load_events_from_config();
	return true;
}

void load_from_config_file() {
	SEED = config_params[0];
	INIT_TIME = config_params[1];
	FIN_TIME = config_params[2];
	ARRIVE_MIN = config_params[3];
	ARRIVE_MAX = config_params[4];
	QUIT_PROB = config_params[5];
	NET_PROB = config_params[6];
	CPU_MIN = config_params[7];
	CPU_MAX = config_params[8];
	DISK1_MIN = config_params[9];
	DISK1_MAX = config_params[10];
	DISK2_MIN = config_params[11];
	DISK2_MAX = config_params[12];
	NETWORK_MIN = config_params[13];
	NETWORK_MAX = config_params[14];
	srand(SEED);
	device_set_seed(SEED);
}

// loads the first jobs of the simulation
// and the simulation finish time
void load_events_from_config() {
	//printf("Putting a new job at %d\nAnd a finish at %d\n", INIT_TIME, FIN_TIME);
	fprintf(LOG_FILE, "Putting a new job at %d\nAnd a finish at %d\n", INIT_TIME, FIN_TIME);
	Event* start_event = create_event(NEW_JOB, job_id, INIT_TIME);
	Event* end_sim_event = create_event(SIM_FIN, 0, FIN_TIME);
	pq_push(pq, start_event);
	pq_push(pq, end_sim_event);
}
