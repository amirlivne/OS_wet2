#include "account.h"


using namespace std;

int account::getPassword() const {
	return password_;
}

int account::getID() const {
	return account_id_;
}

int account::updateBalance(int amount) {
	write_mutex.lock();
	balance_ += amount;
	int result = balance_;
	write_mutex.unlock();
	return result;
}

int account::getBalance() {
	read_counter_mutex.lock();
	if (!readers_counter)
	{
		write_mutex.lock();
	}
	readers_counter++;
	read_counter_mutex.unlock();
	int result = balance_;
	read_counter_mutex.lock();
	if (readers_counter)
	{
		write_mutex.unlock();
	}
	readers_counter--;
	read_counter_mutex.unlock();
	return result;
}
