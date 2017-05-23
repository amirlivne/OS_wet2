#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <mutex>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>


using namespace std;


typedef struct ATM_ {
	int id;
	string input_file; 
} ATM, *pATM;

void* activateATM(void* patm)
{
	pATM curr_ATM = pATM(patm);
	string line;
	fstream file(curr_ATM->input_file);
	while (getline(file, line))
	{
		istringstream iss(line);
		char cmd;
		int account, password;
		iss >> cmd;
		iss >> account;
		iss >> password;
		switch (cmd)
		{
			case 'O':
				cout << "perform add acount" << endl;
				break;
			case 'D':
				cout << "perform deposit acount" << endl;
				break;
			case 'W':
				cout << "perform withrawl acount" << endl;
				break;
			case 'B':
				cout << "perform balance enquiry acount" << endl;
				break;
			case 'Q':
				cout << "perform erase acount" << endl;
				break;
			case 'T':
				cout << "perform teransit acount" << endl;
				break;
		}

		
	}
	void* res = 0;
	return res;
}

int main(int argc, char *argv[]) {

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
			ifstream f(argv[i+2]);
			if (!f.good())
			{
				cerr << "illegal arguments" << endl;
				return(-1);
			}
	}
	
	//need to add a log file // bug 

	pATM ATMs = new ATM[ATM_num];
	for (int i = 0; i < ATM_num; i++)
	{
		ATMs[i].id = i + 1;
		ATMs[i].input_file = argv[i + 2];
	}

	//needs to create threads that will activate ATM's function. //bug
	int rv;
	pthread_t* ATM_threads = new pthread_t[ATM_num]; //initilize N threads variables.
	for (int i = 0; i < ATM_num; i++)
	{
		rv = pthread_create(&ATM_threads[i], NULL, activateATM, &ATMs[i]);
	}
	
	// create thread to get commision for the bank each 3 sec.
	// create thread to print the bank status each 0.5 sec.

}