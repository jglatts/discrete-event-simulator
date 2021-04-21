  
simulation_make: main.cpp device.cpp event.cpp event_priority_queue.cpp queue.cpp simulation.cpp
	g++ -o main main.cpp device.cpp event.cpp event_priority_queue.cpp queue.cpp simulation.cpp -Wall -Werror