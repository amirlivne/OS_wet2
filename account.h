#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <stdlib.h>
#include <string>
#include <pthread.h>
#include <semaphore.h>


using namespace std;

class account
{
private:
	const int account_id_;
	int balance_;
	const string password_;
	unsigned int readers_counter_;
	sem_t write_mutex_;
	sem_t read_counter_mutex_;
public:
	account(int id=0, string pass=0000, int init_balance=0);
	account(const account &src);
	~account();
	string getPassword() const;
	int getID() const;
	int updateBalance(int amount);
	int getBalance();
	int payCommision(int com_rate);
	void lockAccountWrite();
	void unlockAccountWrite();
	void accountReaderEnter();
	void accountReaderLeave();
	//void semInit();
		
};

#endif //ACCOUNT_H
