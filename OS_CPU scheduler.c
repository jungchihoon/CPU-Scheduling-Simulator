#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define P_num 5
#define QUE_SIZE (P_num+1)
#define MAX_CPU 10
#define MAX_IO 10
#define MAX_ARRIVAL 6
#define MAX_PRIORITY 6
#define	TIME_QUANTUM 3
#define LIMIT_IO_RAND 20
#define Algorithm_NUM 9
#define QUANTUM 8
#define AGING_TIME 20
typedef struct process {
	int	PID;
	int init_CPU_bt;
	int CPU_bt;
	int IO_start;
	int IO_bt;
	int Arrival_t;
	int Priority;
}process_info;

process_info* Ready_Q[QUE_SIZE];
process_info* Ready_Q2[QUE_SIZE];
process_info* WQue[QUE_SIZE];
process_info* Running_Process = NULL;
process_info process[P_num];
double Terminated_t[P_num] = { 0 };
double Turnaround_t[P_num] = { 0 };
double	Waiting_t[P_num] = { 0 };
double Average_Turnaround_t[Algorithm_NUM] = { 0 };
double Average_Waiting_t[Algorithm_NUM] = { 0 };

int init_IO_bt[P_num]; 
int init_Priority[P_num];
int runnig_time = 0;
int	ReadyQ_front = 0;
int ReadyQ_rear = 0;
int	ReadyQ2_front = 0;
int ReadyQ2_rear = 0;
int WaitingQ_front = 0;
int WaitingQ_rear = 0;
int Comple_p = 0;
int	RR_t = 0;
int Select;
bool RR_change = false;
bool Preemptive = 0;


void Enter_CPU2(process_info* enter_p);

void All_init() {
	RR_t = 0;
	Comple_p = 0; // completed Process
	ReadyQ_front = 0;
	ReadyQ_rear = 0;
	runnig_time = 0;
	WaitingQ_front = 0;
	WaitingQ_rear = 0;
}
void Make_Process() {
	int loop;
	for (loop = 0; loop < P_num; loop++)
	{
		process[loop].PID = loop + 1;
		process[loop].init_CPU_bt = rand() % (MAX_CPU - 1) + 1;
		process[loop].CPU_bt = process[loop].init_CPU_bt;
		init_IO_bt[loop] = (rand() % LIMIT_IO_RAND) + 1;
		if (init_IO_bt[loop] > MAX_IO)
		{
			init_IO_bt[loop] = 0;
		}
		process[loop].IO_bt = init_IO_bt[loop];
		process[loop].IO_start = rand() % process[loop].CPU_bt;
		process[loop].Arrival_t = rand() % MAX_ARRIVAL;
		init_Priority[loop] = rand() % MAX_PRIORITY;
		process[loop].Priority = init_Priority[loop];
	}
}
void Reset_Process() {
	int loop;
	for (loop = 0; loop < P_num; loop++)
	{
		process[loop].CPU_bt = process[loop].init_CPU_bt;
		process[loop].IO_bt = init_IO_bt[loop];
		process[loop].Priority = init_Priority[loop];
	}
}

void Print_Process_INFO() {
	printf("===================processes_info===================\n");
	int loop;
	for (loop = 0; loop < P_num; loop++)
	{
		printf("======%d. process_info======\n", loop + 1);
		printf("== PROCESS_ID     : %d\n", process[loop].PID);
		printf("== CPU BURST TIME : %d\n", process[loop].init_CPU_bt);
		if (process[loop].IO_bt != 0)
		{
			printf("== IO START TIME  : %d\n", process[loop].IO_start);
			printf("== IO BURST TIME  : %d\n", process[loop].IO_bt);
		}
		printf("- Arrival_t   : %d\n", process[loop].Arrival_t);
		printf("- Priority       : %d\n", process[loop].Priority);

	}
}
void Print_Scheduling Algorithm() {
	printf("====================Scheduling Algorithm====================\n");
	printf("1.FCFS\n2.SJF\n3.Preemptive SJF\n4.Priority\n5.Preemptive Priority\n6.RR\n7.LIFO\n0.Exit\n\nSelect: ");
	scanf("%d", &Select);
}

void READY_Queue(int real_time) {
	int loop;
	for (loop = 0; loop < P_num; loop++) {
		if (process[loop].Arrival_t == real_time)
		{
			Ready_Q[ReadyQ_rear] = &process[loop];
			printf("process%d enter ready-que\n", process[loop].PID);
			ReadyQ_rear = (ReadyQ_rear + 1) % QUE_SIZE;
		}
	}
}
void READY_Queue2(process_info* READY_Queue) {
	Ready_Q[ReadyQ_rear] = READY_Queue;
	printf("process%d enter ready-que\n", READY_Queue->PID);
	ReadyQ_rear = (ReadyQ_rear + 1) % QUE_SIZE;
}
void Enter_CPU(process_info* enter_p) {
	Running_Process = enter_p;
	ReadyQ_front = (ReadyQ_front + 1) % QUE_SIZE;
	//printf("process%d is runnig\n", Running_Process->PID);
}
void Enter_CPU_LIFO(process_info* enter_p) {
	Running_Process = enter_p;
	ReadyQ_rear = (ReadyQ_rear - 1) % QUE_SIZE;
}
void swap_front(int PID) {
	process_info* temp = Ready_Q[ReadyQ_front];
	Ready_Q[ReadyQ_front] = Ready_Q[PID];
	Ready_Q[PID] = temp;
}
void swap_front_CPU() {
	process_info* temp = Ready_Q[ReadyQ_front];
	Ready_Q[ReadyQ_front] = Running_Process;
	Running_Process = temp;
}
bool Running_IO()
{
	if (((Running_Process->init_CPU_bt) - (Running_Process->CPU_bt)) == Running_Process->IO_start)
	{
		if (Running_Process->IO_bt > 0)
		{
			if (Running_Process->IO_start == 0)
				printf("process%d enter CPU\n", Running_Process->PID); //CPU들렸다가 wating가는것을 확실히하기 위해
			printf("process%d go to waiting que\n", Running_Process->PID);
			WQue[WaitingQ_rear] = Running_Process;
			WaitingQ_rear = (WaitingQ_rear + 1) % QUE_SIZE;
			RR_change = true;
			Running_Process = NULL;
			return false;
		}
		return true;
	}
	return true;
}
void waiting_IO()
{

	int loop = WaitingQ_front;
	if (WaitingQ_front == WaitingQ_rear) return;
	while (1)
	{
		(WQue[loop]->IO_bt)--;
		if (WQue[loop]->IO_bt == 0)
		{
			if (Select == 7 && (WQue[loop]->init_CPU_bt - WQue[loop]->CPU_bt >= QUANTUM))// multilevel일경우에만
			{
				printf("process%d enter ready-que2\n", WQue[loop]->PID);
				Ready_Q2[ReadyQ2_rear] = WQue[loop];
				ReadyQ2_rear = (ReadyQ2_rear + 1) % QUE_SIZE;
			}
			else
				READY_Queue2(WQue[loop]);
			int i = loop % QUE_SIZE;
			for (i; i%QUE_SIZE != WaitingQ_front; i = (i - 1) % QUE_SIZE) //한칸씩 땡긴다.
			{
				WQue[i] = WQue[i - 1];
			}
			WaitingQ_front = (WaitingQ_front + 1) % QUE_SIZE; //헤더를 앞으로.
		}
		loop = (loop + 1) % QUE_SIZE;
		if (loop == WaitingQ_rear)return;
	}
}
void CPU() {
	printf("process%d is runnig.\n", Running_Process->PID);
	(Running_Process->CPU_bt)--;
	RR_t++;
	if (Running_Process->CPU_bt == 0)
	{
		printf("process%d is finish.\n", Running_Process->PID);
		Terminated_t[(Running_Process->PID) - 1] = runnig_time + 1;
		//printf("process%d is terminated.\n", Running_Process->PID);
		Running_Process = NULL;
		RR_t = 0;
		Comple_p++;
	}
}
void FCFS() {
	if (Running_Process == NULL) // nothing runing
		if (ReadyQ_front != ReadyQ_rear) //not empty ready-que
			Enter_CPU(Ready_Q[ReadyQ_front]);
}
void LIFO() {
	if (Running_Process == NULL) // nothing runing
		if (ReadyQ_front != ReadyQ_rear) //not empty ready-que
			Enter_CPU_LIFO(Ready_Q[ReadyQ_rear - 1]);
}
void SJF(bool preemptive) {
	int min_Burst_time_PID;

	if (!preemptive) // if nonpreemptive
		if (Running_Process != NULL) return;// if anything is runing , return.

	if (ReadyQ_front != ReadyQ_rear) //not empty ready-que
	{
		min_Burst_time_PID = ReadyQ_front;
		int loop;
		for (loop = ReadyQ_front; loop != ReadyQ_rear; loop = (loop + 1) % QUE_SIZE)
		{
			if (Ready_Q[min_Burst_time_PID]->CPU_bt > Ready_Q[loop]->CPU_bt)
				min_Burst_time_PID = loop;
		}
		swap_front(min_Burst_time_PID);
		if (Running_Process == NULL)
			Enter_CPU(Ready_Q[ReadyQ_front]);
		else
			if (Running_Process->CPU_bt > Ready_Q[ReadyQ_front]->CPU_bt)
				swap_front_CPU();
	}
}
void Priority(bool preemptive) {
	int max_Priority_PID;
	if (!preemptive) // if nonpreemptive
		if (Running_Process != NULL) return;// anything is runing
	if (ReadyQ_front != ReadyQ_rear) //not empty ready-que
	{
		max_Priority_PID = ReadyQ_front;
		int loop;
		for (loop = ReadyQ_front; loop != ReadyQ_rear; loop = (loop + 1) % QUE_SIZE)
		{
			if (Ready_Q[max_Priority_PID]->Priority < Ready_Q[loop]->Priority)
				max_Priority_PID = loop;
		}
		swap_front(max_Priority_PID);
		if (Running_Process == NULL)
			Enter_CPU(Ready_Q[ReadyQ_front]);
		else
			if (Running_Process->Priority < Ready_Q[ReadyQ_front]->Priority)
				swap_front_CPU();
	}
}



void RR() {
	if ((RR_t == TIME_QUANTUM) || RR_change)
	{
		RR_t = 0;
		if (!RR_change) //not waiting
		{
			READY_Queue2(Running_Process);
			Running_Process = NULL;
		}
		RR_change = false;
	}
	FCFS();
}
void Enter_CPU2(process_info* enter_p) {
	Running_Process = enter_p;
	ReadyQ2_front = (ReadyQ2_front + 1) % QUE_SIZE;
	//printf("process%d is runnig\n", Running_Process->PID);
}

void aging()
{
	int loop;
	if (Select == 9)
		return;
	for (loop = 0; loop < P_num; loop++)
	{
		if (process[loop].CPU_bt != 0 && (runnig_time - process[loop].Arrival_t) % AGING_TIME == 0 &&
			runnig_time != process[loop].Arrival_t && process[loop].Priority < MAX_PRIORITY - 1) // 프로세스가 도착하고 AGING_TIME 마다 priority를 높여준다
		{
			process[loop].Priority++;
			printf("priority of process%d change to %d\n", process[loop].PID, process[loop].Priority);
		}

	}
}
void Choice_Algorithm(int choice) // 알고리즘 선택 
{
	switch (choice) {
	case 1: FCFS(); break;
	case 2: SJF(false); break;
	case 3: SJF(true); break;
	case 4: Priority(false); break;
	case 5: Priority(true); break;
	case 6: RR(); break;
	case 7: LIFO(); break;
	
	default: printf("오류\n"); break;
	}
}
void Schedule() {
	while (1)
	{
		printf("============== time %d ===============\n", runnig_time);
		if (Comple_p == P_num) break;
		else
		{
			aging();
			waiting_IO();
			READY_Queue(runnig_time); // arrive time = real time ?
			while (1)
			{
				Choice_Algorithm(Select);//스케쥴링

				if (Running_Process == NULL) break;
				else {
					if (Running_IO()) //IO start ??
					{
						break; // -> if not, break.
					}
				}

			}

			if (Running_Process != NULL)
				CPU();


			runnig_time++; // real time

		}
	}
}
void Evaluation(int choice)
{
	printf("=====================Evaluation=====================\n\n");
	double Average_waiting = 0;
	double Average_turnaround = 0;
	int loop;
	for (loop = 0; loop < P_num; loop++)
	{
		Turnaround_t[loop] = Terminated_t[loop] - process[loop].Arrival_t;
		Waiting_t[loop] = Turnaround_t[loop] - process[loop].init_CPU_bt;
		Average_turnaround += Turnaround_t[loop];
		Average_waiting += Waiting_t[loop];
		printf("Process%d", process[loop].PID);
		printf("Turnaround time: %.0f\n", Turnaround_t[loop]);
		printf("Waiting time: %.0f\n\n", Waiting_t[loop]);
		
	}
	printf("=====================Average=====================\n\n");
	printf("Average turnaround time is  %.2f\n", Average_turnaround / P_num);
	printf("Average waiting time is  %.2f\n\n", Average_waiting / P_num);
	Average_Turnaround_t[choice - 1] = Average_turnaround / P_num;
	Average_Waiting_t[choice - 1] = Average_waiting / P_num;
}
void All_Evaluation()
{
	printf("FCFS                 Average turnaround time :  %.2f\n\t\t\tAverage waiting time is  %.2f\n\n", Average_Turnaround_t[0], Average_Waiting_t[0]);
	printf("SJF                  Average turnaround time :  %.2f\n\t\t\tAverage waiting time is  %.2f\n\n", Average_Turnaround_t[1], Average_Waiting_t[1]);
	printf("Preemptive SJF       Average turnaround time :  %.2f\n\t\t\tAverage waiting time is  %.2f\n\n", Average_Turnaround_t[2], Average_Waiting_t[2]);
	printf("Priority             Average turnaround time :  %.2f\n\t\t\tAverage waiting time is  %.2f\n\n", Average_Turnaround_t[3], Average_Waiting_t[3]);
	printf("Preemptive Priority  Average turnaround time :  %.2f\n\t\t\tAverage waiting time is  %.2f\n\n", Average_Turnaround_t[4], Average_Waiting_t[4]);
	printf("RR                   Average turnaround time :  %.2f\n\t\t\tAverage waiting time is  %.2f\n\n", Average_Turnaround_t[5], Average_Waiting_t[5]);
	printf("LIFO                 Average turnaround time :  %.2f\n\t\t\tAverage waiting time is  %.2f\n\n", Average_Turnaround_t[6], Average_Waiting_t[6]);
	
}
int main()
{
	srand(time(NULL));
	while (1)
	{

		All_init();
		Print_Scheduling Algorithm();
		if (Select == 0)break;
		else if (Select >  Algorithm_NUM + 1 || Select < 1)
		{
			printf("******************error: input value.************************\n");
			continue;
		}
		Make_Process();
		Print_Process_INFO();
		if (Select == Algorithm_NUM + 1)
		{
			Select = 1;
			for (Select; Select < Algorithm_NUM + 1; Select++)
			{
				All_init();
				Reset_Process();
				Print_Process_INFO();
				Schedule();
				Evaluation(Select);
			}
			All_Evaluation();
		}
		else {
			Schedule();
			Evaluation(Select);
		}
	}
	return 0;
}