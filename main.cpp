//#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include "account.h"
#include <vector> //debug

using namespace std;

ofstream Log_file;
pthread_mutex_t log_file_mutex;

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
	string sLine;
	ifstream file;
	file.open(curr_ATM->input_file);
	int accountID, password, amount;
	char cmd;
	account test_account(1111, 1234, 1000); //debug
	while (!file.eof())
	{
		getline(file, sLine);
		istringstream iss(sLine);
		iss >> cmd;
		iss >> accountID;
		iss >> password;
		switch (cmd)
		{
		case 'O':
			int initial_balance;
			iss >> initial_balance;
			//add Open functions
			break;
		case 'D':
			iss >> amount;
			//add Deposit functions
			break;
		case 'W':
			iss >> amount;
			amount = amount*(-1);
			//add Withrawl functions
			break;
		case 'B':
			//add Balance enqury functions
			break;
		case 'Q':
			//add Delete account functions
			break;
		case 'T':
			int target;
			iss >> target;
			iss >> amount;
			//add Transfer functions
			break;
		default: //if the actions is not recognized;
			pthread_mutex_lock(&log_file_mutex);
			Log_file << "Error <" << curr_ATM->id << ">: illigeal action" << endl;
			pthread_mutex_unlock(&log_file_mutex);
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
void* commision_func(void* ATMs_active_flag)
{
	bool* ATMs_active = (bool*)(ATMs_active_flag);
	while (*ATMs_active)
	{
		sleep(3); //sleep for 3 sec
		//add commision function;
	}
	pthread_exit(NULL);
	void* res = 0; //debug
	return res; //debug
}


void* print_bank_func(void* prog_running_flag)
{
	bool* prog_running = (bool*)prog_running_flag;
	while (prog_running)
	{
		usleep(500000); //sleep for 500,000 micro sec == 0.5 sec
		//add print functions
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	
	//initilizing log file
	Log_file.open("log.txt");
	//initilizing log file mutex
	pthread_mutex_init(&log_file_mutex, NULL);

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

	//initilizing N ATMs
	pATM ATMs = new ATM[ATM_num];
	for (int i = 0; i < ATM_num; i++)
	{
		ATMs[i].id = i + 1;
		ATMs[i].input_file = argv[i + 2];
	}

	//initilize N threads variables
	pthread_t* ATM_threads = new pthread_t[ATM_num];
	int rv;
	for (int i = 0; i < ATM_num; i++)
	{
		//activating the N ATMs
		rv = pthread_create(&ATM_threads[i], NULL, activateATM, (void*)&ATMs[0]);
		if (rv)
		{
			cout << "Error <" << i + 1 << ">: error in creating ATM thread" << endl;
		}
	}

	//initilizing commision thread;
	pthread_t commision_thread;
	bool ATMs_active_flag = true; //a flag to tell the commision when to stop running;
	rv = pthread_create(&commision_thread, NULL, commision_func, &ATMs_active_flag);
	if (rv)
	{
		cout << "Error in creating Commision thread" << endl;
	}

	//initilizng print thread;
	pthread_t print_bank_thread;
	bool prog_running_flag = true; //a flag to the the print thread when to stop running
	rv = pthread_create(&print_bank_thread, NULL, print_bank_func, NULL);
	if (rv)
	{
		cout << "Error in creating Print Bank thread" << endl;
	}

	//joining ATMs threads
	for (int i = 0; i < ATM_num; i++)
	{
		rv = pthread_join(ATM_threads[i], NULL);
		if (rv)
		{
			cout << "Error <" << i+1 << ">: error in joining ATM thread" << endl;
		}
	}

	//joining commision thread
	ATMs_active_flag = false;
	rv = pthread_join(commision_thread, NULL);
	if (rv)
	{
		cout << "Error in joining Commision thread" << endl;
	}

	//joining print bank thread
	prog_running_flag = false;
	rv = pthread_join(print_bank_thread, NULL);
	if (rv)
	{
		cout << "Error in joining Print Bank thread" << endl;
	}

	//destroying log file mutex and closing the log file
	Log_file.close();
	pthread_mutex_destroy(&log_file_mutex);
	
	return 1;
}