#include "account.h"


using namespace std;

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
	write_mutex.lock();
	balance_ += amount;
	int result = balance_;
	write_mutex.unlock();
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
	read_counter_mutex.lock();
	if (!readers_counter) //if this is the first reader, lock the writing mutex
	{
		write_mutex.lock();
	}
	readers_counter++;
	read_counter_mutex.unlock(); //free the reading mutex
		//READ!
	int result = balance_; 
		//lock the reader mutex
	read_counter_mutex.lock(); 
	if (readers_counter) //if this is the last reader = unlock the writing mutex
	{
		write_mutex.unlock();
	}
	readers_counter--;
	read_counter_mutex.unlock(); //free the reading mutex
	return result;
}
