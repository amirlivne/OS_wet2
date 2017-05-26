#include "account.h"


using namespace std;

//********************************************
// function name: lockAccountWrite
// Description: lock account from changing the balance
// Parameters: NONE
// Returns: NONE
//***********************************************
void account::lockAccountWrite()
{
	sem_wait(&write_mutex_);
}

//********************************************
// function name: unlockAccountWrite
// Description: unlock account from changing the balance
// Parameters: NONE
// Returns: NONE
//***********************************************
void account::unlockAccountWrite()
{
	sem_post(&write_mutex_);
}
//********************************************
// function name: accountReaderEnter
// Description: safely adds 1 to the readers counter in the account 
// Parameters: NONE
// Returns: NONE
//***********************************************
void account::accountReaderEnter()
{
	sem_wait(&read_counter_mutex_); //lock reader counter in order to update it
	if (!readers_counter_++) //update the counter. if this is the first reader, lock the writing mutex 
	{
		sem_wait(&write_mutex_);
	}
	sem_post(&read_counter_mutex_); //unlock reader counter to allow multiple readers
}

//********************************************
// function name: accountReaderLeave
// Description: safely subs 1 from thr readers counter in the bank
// Parameters: NONE
// Returns: NONE
//***********************************************
void account::accountReaderLeave()
{
	sem_wait(&read_counter_mutex_); //lock reader counter in order to update it
	readers_counter_--; //update the counter
	if (!readers_counter_) //if this is the last reader = unlock the writing mutex
	{
		sem_post(&write_mutex_);
	}
	sem_post(&read_counter_mutex_); //unlock reader counter to allow multiple readers
}


//********************************************
// function name: account
// Description: account class constractor
// Parameters: 3 ints - id, password and balance
// Returns: NONE
//***********************************************
account::account(int id, string pass, int init_balance) : account_id_(id), password_(pass), balance_(init_balance), readers_counter_(0)
{
}

//********************************************
// function name: ~account
// Description: account distractor
// Parameters: NONE
// Returns: NONE
//***********************************************
account::~account()
{
	//destroying the readers-writers mutexes:
	sem_destroy(&write_mutex_);
	sem_destroy(&read_counter_mutex_);
}

//********************************************
// function name: account(const account &src)
// Description: copy constractor of account (ment to initilize mutexes after insertion to DB map)
// Parameters: NONE
// Returns: NONE
//***********************************************
account::account(const account &src): account_id_(src.account_id_), password_(src.password_), balance_(src.balance_)
{
	readers_counter_ = 0;
	//initilizing the readers-writers mutexes:
	sem_init(&write_mutex_, 0, 1);
	sem_init(&read_counter_mutex_, 0, 1);
}

//********************************************
// function name: getPassword
// Description: returns the current password of an account
// Parameters: NONE
// Returns: an integer equeles to the current password. 
//***********************************************
string account::getPassword() const {
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
	if (balance_ + amount < 0) //
	{
		sem_post(&write_mutex_);
		return -1;
	}
	balance_ += amount;
	int result = balance_;
	return result;
}

//********************************************
// function name: getBalance
// Description: returns the current balance of an account
// Parameters: NONE
// Returns: an integer equeles to the current balance. 
//***********************************************
int account::getBalance() {
	accountReaderEnter();
	//read!
	int result = balance_; 
	accountReaderLeave();
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
	double com_rate_precent = (double)com_rate / 100.0;
	int commision_taken = int(balance_*com_rate_precent);
	balance_ -= commision_taken;
	return commision_taken;
}




