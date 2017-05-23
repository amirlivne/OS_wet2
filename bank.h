#ifndef BANK_H
#define BANK_H


#include "account.h"
#include <string>
#include <map>
#include <utility>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>

using namespace std;


class bank {
private:
	int bank_money_;
	unsigned int db_readers_counter;
	pthread_mutex_t mutex_accountsDB_write;
	pthread_mutex_t db_read_counter_mutex;
	pthread_mutex_t bank_balance_mutex;
	int bank_money_;
	map<int, account> accounts_;
	void readerEnter();
	void readerLeave();

public:
	bank();
	~bank();
	void bank_commision(int commision_rate);
	void print_bank();
	void Open_Account(int acount_id, int password, int init_balance, int ATM_ID);
	void Deposit_Account(int acount_id, int password, int amount, int ATM_ID);
	void Withdraw_Account(int acount_id, int password, int amount, int ATM_ID);
	void Get_Balance_Account(int acount_id, int password, int ATM_ID);
	void Quit_Account(int acount_id, int password, int ATM_ID);
	void Transfer_Account(int acount_id_source, int password, int acount_id_target, int amount, int ATM_ID);
	bool Password(int acount_id, int password);


};


#endif //BANK_H
