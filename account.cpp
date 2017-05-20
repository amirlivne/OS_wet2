#include "account.h"


using namespace std;


//********************************************
// function name: account
// Description: account class constractor
// Parameters: 3 ints - id, password and balance
// Returns: NONE
//***********************************************
account::account(int id, int pass, int init_balance) : account_id_(id), password_(pass), balance_(init_balance), readers_counter_(0)
{
	//initilizing the readers-writers mutexes:
	pthread_mutex_init(&write_mutex_, NULL);
	pthread_mutex_init(&read_counter_mutex_, NULL);
}

account::~account()
{
	//destroying the readers-writers mutexes:
	pthread_mutex_destroy(&write_mutex_);
	pthread_mutex_destroy(&read_counter_mutex_);
}

//********************************************
// function name: getPassword
// Description: returns the current password of an account
// Parameters: NONE
// Returns: an integer equeles to the current password. 
//***********************************************
int account::getPassword() const {
	return password_;
}

//********************************************
// function name: getBalance
// Description: returns the current balance of an account
// Parameters: NONE
// Returns: an integer equeles to the current balance. 
//***********************************************
int account::getID() const {
	return account_id_;
}

//********************************************
// function name: updateBalance
// Description: updates the current balance of an account
// Parameters: the amount to add to the current balance (could be negative)
// Returns: an integer - amount of money to add
//***********************************************
int account::updateBalance(int amount) {
	pthread_mutex_lock(&write_mutex_);
	sleep(1);
	balance_ += amount;
	int result = balance_;
	pthread_mutex_unlock(&write_mutex_);
	return result;
}

//********************************************
// function name: getBalance
// Description: returns the current balance of an account
// Parameters: NONE
// Returns: an integer equeles to the current balance. 
//***********************************************
int account::getBalance() {
		//lock the reader mutex
	pthread_mutex_lock(&read_counter_mutex_);
	if (!readers_counter_++) //if this is the first reader, lock the writing mutex
	{
		pthread_mutex_lock(&write_mutex_);
	}
	pthread_mutex_unlock(&read_counter_mutex_); //unlock reader counter to allow multiple readers
	//READ!
	int result = balance_; 
	sleep(1);
		//lock the reader mutex
	pthread_mutex_lock(&read_counter_mutex_);
	if (readers_counter_--) //if this is the last reader = unlock the writing mutex
	{
		pthread_mutex_unlock(&write_mutex_);
	}
	pthread_mutex_unlock(&read_counter_mutex_);
	return result;
}
