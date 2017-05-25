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
	sem_t mutex_accountsDB_write;
	sem_t db_read_counter_mutex;
	sem_t bank_balance_mutex;
	map<int, account> accounts_;
	void readerEnter();
	void readerLeave();

public:
	bank();
	~bank();
	void Bank_Commission(int commision_rate);
	void Print_Bank();
	void Open_Account(int account_id, string password, int init_balance, int ATM_ID);
	void Deposit_Account(int account_id, string password, int amount, int ATM_ID);
	void Withdraw_Account(int account_id, string password, int amount, int ATM_ID);
	void Get_Balance_Account(int account_id, string password, int ATM_ID);
	void Quit_Account(int account_id, string password, int ATM_ID);
	void Transfer_Account(int account_id_source, string password, int account_id_target, int amount, int ATM_ID);
	bool Password(int account_id, string password);


};


#endif //BANK_H
