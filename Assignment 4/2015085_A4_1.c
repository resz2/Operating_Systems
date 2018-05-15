#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

struct node																// Node in a linked list
{
	int id;
	struct node *next;
}*t = NULL;

struct queue
{
	int no, time_slice;
	struct node *head;
};

struct process
{
	int id, time_complete, time_arrival, q_no, time_turnaround;
	bool done, registered;
};

void * create_thread();
void insert(int, int );
void delete(int, int );
void display(int );
int length(int );
bool all_queues_empty();
void main_thread();
void add_new_process_to_queue();
bool upper_queues_empty(int );
void boost();

int time = 0;

int q, n, t_boost;
struct queue *listq = NULL;												// List of queues
struct process *listp = NULL;											// List of processes
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

int main()
{	
	int  t_slice_q, t_complete, t_arrival;

	printf("No. of Queues: ");
	scanf ("%d", &q);													// no of queues
	listq = (struct queue *) malloc (q * sizeof (struct queue));	// A list of queues

	printf("Time Slice for each Queue: ");
	int i;
	for (i=0; i<q; i++) {												// initializing each queue
		scanf("%d", &t_slice_q);
		listq[i].no = i;											// 0-based indexing
		listq[i].time_slice = t_slice_q;							// time slice for each queue
		listq[i].head = NULL;
	}

	printf("Enter Boost Time: ");
	scanf ("%d", &t_boost);											// Priority Boost

	printf("No. of Processes: ");
	scanf ("%d",&n);													//no of processes

	listp = (struct process *) malloc (n * sizeof (struct process));		// A list of processes

	printf("Enter Time of Execution and Time of Arrival:\n");
	for (i=0; i<n; i++) {												// Putting the data about each process in the process list
		scanf ("%d", &t_complete);
		scanf ("%d", &t_arrival);
		listp[i].id = i;											// 0-based indexing
		listp[i].time_complete = t_complete;						// time of execution of a process
		listp[i].time_arrival = t_arrival;						// arrival time for each process
		listp[i].q_no = 0;										// initially each process enters the 0th queue.
		listp[i].done = false;									// Process has not completed yet
		listp[i].registered = false;								// Process has not started yet
		listp[i].time_turnaround = 0;
	}

	pthread_t tid[n];
	for (i=0; i<n; i++) {
		pthread_create (&tid[i], NULL, create_thread, NULL);
	}

	for (i=0; i<n; i++) {
		pthread_join (tid[i], NULL);
	}

	printf("Total time : %d\n", time);
	
	printf("\nTurnaround Time: \n");
	for (i=0; i<n; i++) {
		printf("Process %d: %d\n", i+1, listp[i].time_turnaround);
	}

	return 0;
}

void main_thread () {
	volatile int i = pthread_self()-1, k;
	add_new_process_to_queue();

	while (all_queues_empty() == true) {
		time++;
		if (time % t_boost == 0) {
			boost();
		}
		// printf("yes\n");
		// printf("%d\n",time);
		add_new_process_to_queue();
	}

	while (!upper_queues_empty(listp[i].q_no)) {
		printf("%d blocked\n", i);
		pthread_cond_wait(&c, &lock);
	}

	while (listp[i].time_arrival > time) {
		while (all_queues_empty() == true) {
			time++;
			if (time % t_boost == 0) {
				boost();
			}
			// printf("%d\n", time);
			add_new_process_to_queue();
		}	
		if (listp[i].time_arrival > time) {
			pthread_cond_wait(&c, &lock);
		}	
	}

	printf("%d has woken up\n", i);

	bool boosted = false;
	for (k=0; k<listq[listp[i].q_no].time_slice; k++) {
		if (listp[i].time_complete > 0) {
			listp[i].time_complete--;
			time++;
			if (time % t_boost == 0) {
				boost();
				boosted = true;
			}
			// printf("%d\n", time);
		}
		else {
			listp[i].done = true;
			break;
		}
	}
	if (listp[i].time_complete == 0) {
		printf("%d terminated\n", i);
		listp[i].done = true;
		listp[i].time_turnaround = time - listp[i].time_arrival;
		delete(listp[i].q_no, i);
	}
	else if (listp[i].done == false && boosted == false) {
		if (listp[i].q_no != q-1) {
			printf("%d blocked and moved down\n", i);
			listp[i].q_no++;
			insert(listp[i].q_no, i);
			delete(listp[i].q_no-1, i); 
		}
	}

	int j;
	for (j=0; j<q; j++) {
		printf("Queue %d: ",j+1);
		display(j);
		printf("\n");
	}

	pthread_cond_broadcast(&c);
} 

void * create_thread () {
	pthread_mutex_lock (&lock);
	int i = pthread_self() - 1;
	while (listp[i].time_complete > 0) {
		main_thread();
	}
	pthread_mutex_unlock (&lock);
}

void insert(int q_id, int p_id)															//Insertion at beginning
{
	t = (struct node *) malloc (sizeof(struct node));
	t->id = listp[p_id].id;
	t->next = listq[q_id].head;
	listq[q_id].head = t;
}

void delete(int q_id, int p_id)
{
	if(listq[q_id].head == NULL)
		return;
	else
	{
		struct node *k = listq[q_id].head;
		if (k->id == listp[p_id].id) {
			listq[q_id].head = k->next;
			return;
		}
		while (k->next != NULL) {
			if (k->next->id == listp[p_id].id) {
				k->next = k->next->next;
				return;
			}
			k = k->next;
		}
	}
}

void display(int i)
{
	if (listq[i].head != NULL)
	{
		t = listq[i].head;
		while(t != NULL)
		{
			printf("%d ", t->id);
			t = t->next;
		}
	}
}

int length(int id)
{
	int c = 0;
	struct node *k = listq[id].head;
	while(k != NULL)
	{
		c++;
		k = k->next;
	}
	return c;
}

bool all_queues_empty() {
	int i, c = 0;
	for (i=0; i<q; i++) {
		if (length(i) == 0) {
			c++;
		}
	}
	return (c == q ? true : false);
}

void add_new_process_to_queue() {
	int i;
	for (i=0; i<n; i++) {
		if (listp[i].time_arrival <= time && listp[i].registered == false) {
			insert(0, i);
			listp[i].registered = true;
			printf("%d registered\n", i);
		}
	}
}

bool upper_queues_empty(int q_id) {
	int i;
	for (i=0; i<q_id; i++) {
		if (length(i) > 0) {
			return false;
		}
	}
	return true;
}

void boost() {
	int i;
	for (i=1; i<q; i++) {
		struct node *k = listq[i].head;
		if (k == NULL) {
			continue;
		}
		while (k != NULL) {
			insert (0, k->id);
			listp[k->id].q_no = 0;		
			k = k->next;
		}
		listq[i].head = NULL;
	}
}