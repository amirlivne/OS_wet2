#include "account.h"


using namespace std;


//********************************************
// function name: account
// Description: account class constractor
// Parameters: 3 ints - id, password and balance
// Returns: NONE
//***********************************************
account::account(int id=0, int pass=0000, int init_balance=0) : account_id_(id), password_(pass), balance_(init_balance), readers_counter_(0)
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
// Returns: an integer - amount of balance after update, or -1 in case of failure
//***********************************************
int account::updateBalance(int amount) {
	pthread_mutex_lock(&write_mutex_);
	if (balance_ + amount < 0) //
	{
		pthread_mutex_unlock(&write_mutex_);
		return -1;
	}
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
	if (!--readers_counter_) //if this is the last reader = unlock the writing mutex
	{
		pthread_mutex_unlock(&write_mutex_);
	}
	pthread_mutex_unlock(&read_counter_mutex_);
	return result;
}

//********************************************
// function name: payCommision
// Description: takes commision from the account and updates it's balance accordinly
// Parameters: the commision rate (integer)
// Returns: an integer equeles to amount of commision taken
//***********************************************
int account::payCommision(int com_rate)
{
	double com_rate_precent = com_rate / 100;
	pthread_mutex_lock(&write_mutex_);
	int commision_taken = int(balance_*com_rate_precent);
	balance_ -= commision_taken;
	pthread_mutex_unlock(&write_mutex_);
	return commision_taken;
}

//********************************************
// function name: lockAccount
// Description: lock account from changing the balance
// Parameters: NONE
// Returns: NONE
//***********************************************
void account::lockAccount()
{
	pthread_mutex_lock(&write_mutex_);
}

//********************************************
// function name: unlockAccount
// Description: unlock account from changing the balance
// Parameters: NONE
// Returns: NONE
//***********************************************
void account::unlockAccount()
{
	pthread_mutex_lock(&write_mutex_);
}

//********************************************
// function name: moneyTransfer
// Description: updates the current balance of an account
// Parameters: the amount to add to the current balance (could be negative)
// Returns: an integer - amount of balance after update, or -1 in case of failure
//***********************************************
int account::moneyTransfer(int amount)
{
	if (balance_ + amount < 0) //
	{
		pthread_mutex_unlock(&write_mutex_);
		return -1;
	}
	sleep(1);
	balance_ += amount;
	int result = balance_;
	return result;
}

