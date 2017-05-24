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
	const int password_;
	unsigned int readers_counter_;
	sem_t write_mutex_;
	sem_t read_counter_mutex_;
public:
	account(int id, int pass, int init_balance);
	account(const account &src);
	~account();
	int getPassword() const;
	int getID() const;
	int updateBalance(int amount);
	int moneyTransfer(int amount);
	int getBalance();
	int payCommision(int com_rate);
	void lockAccount();
	void unlockAccount();
	//void semInit();
		
};

#endif //ACCOUNT_H
