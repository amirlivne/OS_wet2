#include "bank.h"


using namespace std;

bank::bank()
{
	bank_money_ = 0;
	db_readers_counter = 0;
}

void bank::bank_commision()
{

}

bool bank::Password(int acount_id, int password)
{
	if (accounts_.find(acount_id) == accounts_.end())   // checks if the account exist
	{
		return false;
	}
	return ((*accounts_.find(acount_id)).second.getPassword() == password);	 // checks if the passowrd for the account is correct
}

void bank::print_bank()
{
	cout << "Current Bank Status" << endl;
	for (map<int, account>::iterator it = accounts_.begin(); it != accounts_.end(); ++it)
	{
			cout << "Account " << (*it).second.getID() << ": Balance - " << (*it).second.getBalance() << " $ , Account Password - " << (*it).second.getPassword() << endl;
	}
	cout << "The Bank has " << bank_money_ << " $" << endl;
}

void bank::Open_Account(int acount_id, int password, int init_balance, int ATM_ID)
{
	mutex_accountsDB_write.lock();
	if (accounts_.find(acount_id) == accounts_.end())	// the account do not exist
	{
		// add addind - fix
		cout << ATM_ID << ": New account id is " << acount_id << " with password " << password << " and initial balance " << init_balance << endl;
	}
	else   // the account already exist
	{
		cout << "Error " << ATM_ID << ": Your transaction failed – account with the same id exists" << endl;
	}
	mutex_accountsDB_write.unlock();
}

void bank::Deposit_Account(int acount_id, int password, int amount, int ATM_ID)   // ?????????????
{
	db_read_counter_mutex.lock();
	if (!db_readers_counter++)
	{
		mutex_accountsDB_write.lock();
	}
	db_read_counter_mutex.unlock();
	// do something
	db_read_counter_mutex.lock();
	if (!db_readers_counter)
	{
		mutex_accountsDB_write.unlock();
	}
	mutex_accountsDB_write.unlock();
	
}

void bank::Withdraw_Account(int acount_id, int password, int amount, int ATM_ID)
{

}

void bank::Get_Balance_Account(int acount_id, int password, int ATM_ID)
{

}

void bank::Quit_Account(int acount_id, int password, int ATM_ID)
{
	mutex_accountsDB_write.lock();
	if (accounts_.find(acount_id) == accounts_.end())	// the account do not exist
	{
		cout << "Error " << ATM_ID << ": Your transaction failed – account id " << acount_id << " does not exist" << endl;
	}
	else if (!Password(acount_id, password))	// the account exist and the password is incorrect
	{
		cout << "Error " << ATM_ID << ": Your transaction failed – password for account id " << acount_id << " is incorrect" << endl;
	}
	else   // the account exist and the password is correct
	{
		cout << ATM_ID << ": Account id " << acount_id << " is now closed. Balance was " << (*accounts_.find(acount_id)).second.getBalance() << endl;
		// add closing - fix
	}
	mutex_accountsDB_write.unlock();

}

void bank::Transfer_Account(int acount_id_source, int password, int acount_id_target, int amount, int ATM_ID)
{

}




