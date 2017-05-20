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



typedef struct ATM_ {
	int id;
	char* input_file;
} ATM, *pATM;

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
			cout << "accountID is" << accountID << endl;  //debug
			cout << "account password is: " << password << endl; //debug
			cout << "initial_balance is" << initial_balance << endl; //debug
			break;
		case 'D':
			iss >> amount;
			test_account.updateBalance(amount); //debug
			cout << "updated balance is: " << test_account.getBalance() << endl;
			break;
		case 'W':
			iss >> amount;
			amount = amount*(-1);
			test_account.updateBalance(amount); //debug
			cout << "updated balance is: " << test_account.getBalance() << endl;
			break;
		case 'B':
			cout << "account balance is: " << test_account.getBalance() << endl; //debug
			break;
		case 'Q':
			break;
		case 'T':
			int target;
			iss >> target;
			iss >> amount;
			cout << "target is: " << target << endl; //debug
			cout << "amount is: " << amount << endl; //debug
			break;
		default:
			cout << "NOT GOOD" << endl;
			break;
		}
	}
	file.close();
	pthread_exit(NULL);
	void* res = 0; //debug
	return res; //debug
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
		ifstream f(argv[i + 2]);
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
		//  cout << i+1 << endl;
		rv = pthread_create(&ATM_threads[i], NULL, activateATM, (void*)&ATMs[0]);
	}
	for (int i = 0; i < ATM_num; i++)
	{
		rv = pthread_join(ATM_threads[i], NULL);
	}
	// create thread to get commision for the bank each 3 sec.
	// create thread to print the bank status each 0.5 sec.
	return 1;
}