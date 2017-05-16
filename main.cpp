#include <pthread.h>
#include <mutex>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>


using namespace std;


typedef struct ATM_ {
	int id;
	char* input_file; 
} ATM, *pATM;

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

	//needs to create threads that will activate ATM's function.
	int rv;
	pthread_t ATM_threads = new pthread_t[ATM_num];
	for (int i = 0; i < ATM_num; i++)
	{
		rv = pthread_create(ATM_threads[i], NULL, FUNCTION, argv[i + 2]);
	}



}