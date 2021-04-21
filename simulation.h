#ifndef SIMULATION__H
#define SIMULATION__H

typedef enum event_type {
	CPU_ARRIVAL = 1,
	CPU_FINISH,
	DISK1_ARRIVAL,
	DISK1_FINISH,
	DISK2_ARRIVAL,
	DISK2_FINISH,
	NETWORK_ARRIVAL,
	NETWORK_FINISH,
	NEW_JOB,
	SIM_FIN
} event_type;

typedef enum device_type {
	CPU = 1,
	DISK1,
	DISK2,
	NETWORK
} device_type;

typedef struct Event {
	int id;
	event_type type;
	int time;
	// add some data to get stats later on
} Event;

typedef struct Node {
	struct Node* next;
	struct Event* event;
} Node;

typedef struct Queue {
	struct Node* head;
	int size;
} Queue;

typedef struct PQNode {
	struct PQNode* next;
	struct Event* event;
	int priority;
} PQNode;

typedef struct PriorityQueue {
	struct PQNode* head;
	int size;
} PriorityQueue;

typedef struct Device {
	device_type type;
	Queue* queue;
	Event* current_event;
	int max;
	int min;
	int seed;		// used by CPU
	int quit_prob;	// used by CPU
	int net_prob;	// used by CPU
	bool idle;
} Device;

// Main Simulation functions
void print_queue();
bool init_simulation();
bool run_simulation();
bool read_config_file();
void write_to_log_file();
void write_to_stats_file();
void add_new_job();
void load_from_config_file();
void load_events_from_config();
void send_job_from_cpu(Event*);
void send_job_to_cpu(Event*);
void send_job_to_disk(Event*);
void send_job_to_network(Event*);
void quit_job(Event*);
void init_first_event();
void test_pq();
void test_queue();

// Device functions
Device* create_device(device_type, int, int);
void    device_set_seed(int);
void    device_handler(Device*, Event*, PriorityQueue*, int);
void    process_cpu(Device*, Event*, PriorityQueue*, int);
void    process_disk_one(Device*, Event*, PriorityQueue*, int);
void    process_disk_two(Device*, Event*, PriorityQueue*, int);
void    process_network(Device*, Event*, PriorityQueue*, int);

// Event functions
Event*   create_event(event_type, int, int);

// Event priority queue functions
PriorityQueue* pq_create_queue();
PQNode*        pq_create_node(Event*);
Event*         pq_pop(PriorityQueue*);
bool           pq_queue_empty(PriorityQueue*);
void           pq_push(PriorityQueue*, Event*);

// Device queue functions
Queue*         dev_create_queue();
Node*          dev_create_node(Event*);
Event*         dev_pop(Queue* queue);
void           dev_push(Queue*, Event*);
bool           dev_queue_empty(Queue*);
int            dev_get_shortest_queue(Queue*, Queue*);

#endif // !SIMULATION__H
