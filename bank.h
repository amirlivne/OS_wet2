#ifndef BANK_H
#define BANK_H


#include "account.h"
#include <string>
#include <map>
#include <utility>
#include <iostream>
#include <mutex>


using namespace std;


class bank {
private:
	mutex mutex_accountsDB_write;
	unsigned int db_readers_counter;
	mutex db_read_counter_mutex;
	int bank_money_;
	map<int, account> accounts_;

public:
	bank();
	void bank_commision();
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
