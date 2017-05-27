//#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include "account.h"
#include "bank.h"


using namespace std;

ofstream Log_file;
sem_t log_file_mutex;
sem_t com_flag_mutex;
sem_t print_flag_mutex;
//sem_t get_line_mutex;
bank best_bank;

typedef struct ATM_ {
	int id;
	char* input_file;
} ATM, *pATM;


//********************************************
// function name: activateATM
// Description: a function to run the ATMs (thread) actions from the relevant file
// Parameters: patm - a pointer to an ATM struct that hold the ATM's ID and file name
// Returns: NONE
//***********************************************
void* activateATM(void* patm)
{
	pATM curr_ATM = pATM(patm);
	char cmd;
	ifstream file;
	file.open(curr_ATM->input_file);
	int accountID, amount;
	string password;
	while (file >> cmd)
	{
		usleep(100000); //sleep for 100,000 micro sec == 100 milisec == 0.1 sec
		//sem_wait(&get_line_mutex);
		//	sem_post(&get_line_mutex);
		file >> accountID;
		file >> password;
		switch (cmd)
		{
		case 'O':
			int initial_balance;
			file >> initial_balance;
			best_bank.Open_Account(accountID, password, initial_balance, curr_ATM->id);
			break;
		case 'D':
			file >> amount;
			best_bank.Deposit_Account(accountID, password, amount, curr_ATM->id);
			break;
		case 'W':
			file >> amount;
			best_bank.Withdraw_Account(accountID, password, amount, curr_ATM->id);
			break;
		case 'B':
			best_bank.Get_Balance_Account(accountID, password, curr_ATM->id);
			break;
		case 'Q':
			best_bank.Quit_Account(accountID, password, curr_ATM->id);
			break;
		case 'T':
			int target;
			file >> target;
			file >> amount;
			best_bank.Transfer_Account(accountID, password, target, amount, curr_ATM->id);
			break;
		default: //if the actions is not recognized;
			sem_wait(&log_file_mutex);
			Log_file << "Error " << curr_ATM->id << ": illigeal action" << endl;
			sem_post(&log_file_mutex);
			break;
		}
	}
	file.close();
	pthread_exit(NULL);
	void* res = 0; //debug
	return res; //debug
	}

//********************************************
// function name: commision_func
// Description: a function to run the commision thread
// Parameters: ATMs_active_flag - a pointer to a flag that says when does the ATM active.
// Returns: NONE
//***********************************************
void* commission_func(void* ATMs_active_flag)
{
	while (true)
	{
		sem_wait(&com_flag_mutex);
		bool ATMs_active = *((bool*)(ATMs_active_flag)); //read the current value of the flag
		sem_post(&com_flag_mutex);
		if (!ATMs_active) break; //if the flag is off - exit the loop (and the thread)
		sleep(3); //sleep for 3 sec
		int com_rate = rand() % 3 + 2; //random int between 2-4
		best_bank.Bank_Commission(com_rate);
	}
	pthread_exit(NULL);
	void* res = 0; //debug
	return res; //debug
}


void* print_bank_func(void* prog_running_flag)
{
	while (true)
	{
		sem_wait(&print_flag_mutex);
		bool prog_running = *((bool*)prog_running_flag); //read the current value of the flag
		sem_post(&print_flag_mutex);
		if (!prog_running) break; //if the flag is off - exit the loop (and the thread);
		usleep(500000); //sleep for 500,000 micro sec == 0.5 sec
		best_bank.Print_Bank();
	}
	pthread_exit(NULL);
	void* res = NULL;
	return res;
}

int main(int argc, char *argv[]) {
	//initilizing log file
	Log_file.open("log.txt");
	//initilizing log file mutex
	sem_init(&log_file_mutex, 0, 1);
	sem_init(&com_flag_mutex, 0, 1);
	sem_init(&print_flag_mutex, 0, 1);
	//sem_init(&get_line_mutex,0,1);
	/*Arguments Checks:*/
	if (argc <= 2) //at least 3 args needed (prog, ATM num, and at list 1 atm file)
	{
		cerr << "illegal arguments" << endl;
		return(-1);
	}
	int ATM_num = atoi(argv[1]); //initilize the ATM amount.
	if (argc != ATM_num + 2) //check that there are enough input files for all of the ATMs
	{
		cerr << "illegal arguments" << endl;
		return(-1);
	}
	for (int i = 0; i < ATM_num; ++i) //check that all of the input files exists
	{
		ifstream f(argv[i + 2]);
		if (!f.good())
		{
			cerr << "illegal arguments" << endl;
			return(-1);
		}
	}

	int rv;

	//initilizing N ATMs
	pATM ATMs = new ATM[ATM_num];
	for (int i = 0; i < ATM_num; i++)
	{
		ATMs[i].id = i + 1;
		ATMs[i].input_file = argv[i + 2];
	}

	//initilizing commission thread;
	pthread_t commission_thread;
	bool ATMs_active_flag = true; //a flag to tell the commision when to stop running;
	rv = pthread_create(&commission_thread, NULL, commission_func, &ATMs_active_flag);
	if (rv)
	{
		cerr << "Error in creating Commision thread" << endl;
	}

	//initilizng print thread;
	pthread_t print_bank_thread;
	bool prog_running_flag = true; //a flag to the the print thread when to stop running
	rv = pthread_create(&print_bank_thread, NULL, print_bank_func, &prog_running_flag);
	if (rv)
	{
		cerr << "Error in creating Print Bank thread" << endl;
	}

	//initilize N threads variables
	pthread_t* ATM_threads = new pthread_t[ATM_num];

	for (int i = 0; i < ATM_num; i++) //activating the N ATMs
	{
		rv = pthread_create(&ATM_threads[i], NULL, activateATM, (void*)&ATMs[i]);
		if (rv)
		{
			cerr << "Error <" << i + 1 << ">: error in creating ATM thread" << endl;
		}
	}

	//joining ATMs threads
	for (int i = 0; i < ATM_num; i++)
	{
		rv = pthread_join(ATM_threads[i], NULL);
		if (rv)
		{
			cerr << "Error <" << i+1 << ">: error in joining ATM thread" << endl;
		}
	}
	//joining commision thread
	sem_wait(&com_flag_mutex);
	ATMs_active_flag = false;
	sem_post(&com_flag_mutex);
	rv = pthread_join(commission_thread, NULL);
	if (rv)
	{
		cerr << "Error in joining Commision thread" << endl;
	}

	//joining print bank thread
	sem_wait(&print_flag_mutex);
	prog_running_flag = false;
	sem_post(&print_flag_mutex);
	rv = pthread_join(print_bank_thread, NULL);
	if (rv)
	{
		cerr << "Error in joining Print Bank thread" << endl;
	}

	//destroying log file mutex and closing the log file
	Log_file.close();
	sem_destroy(&log_file_mutex);
	sem_destroy(&com_flag_mutex);
	sem_destroy(&print_flag_mutex);
	//sem_destroy(&get_line_mutex);

	return 1;
}