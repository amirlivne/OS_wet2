#include <pthread.h>
#include <mutex>
#include <iostream>
#include <stdlib.h>
#include <fstream>


using namespace std;


typedef struct ATM_ {
	int id;
	char* input_file; 
} ATM, *pATM;

void* activateATM(void* pATM)
{
	return;
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

	//needs to create threads that will activate ATM's function. //bug
	int rv;
	pthread_t* ATM_threads = new pthread_t[ATM_num]; //initilize N threads variables.
	for (int i = 0; i < ATM_num; i++)
	{
		rv = pthread_create(&ATM_threads[i], NULL, activateATM, argv[i + 2]);
	}
	
	// create thread to get commision for the bank each 3 sec.
	// create thread to print the bank status each 0.5 sec.

}