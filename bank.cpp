#include "bank.h"


using namespace std;


bank::bank()
{
	bank_money_ = 0;
	db_readers_counter = 0;

	//initilizing the readers-writers mutexes:
	pthread_mutex_init(&mutex_accountsDB_write, NULL);
	pthread_mutex_init(&db_read_counter_mutex, NULL);
}

bank::~bank()
{
	//destroying the readers-writers mutexes:
	pthread_mutex_destroy(&mutex_accountsDB_write);
	pthread_mutex_destroy(&db_read_counter_mutex);
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
	pthread_mutex_lock(&mutex_accountsDB_write);
	for (map<int, account>::iterator it = accounts_.begin(); it != accounts_.end(); ++it)
	{
		cout << "Account " << (*it).second.getID() << ": Balance - " << (*it).second.getBalance() << " $ , Account Password - " << (*it).second.getPassword() << endl;
	}
	cout << "The Bank has " << bank_money_ << " $" << endl;
	pthread_mutex_unlock(&mutex_accountsDB_write);
}

void bank::Open_Account(int acount_id, int password, int init_balance, int ATM_ID)
{
	
	pthread_mutex_lock(&mutex_accountsDB_write);
	if (accounts_.find(acount_id) == accounts_.end())	// the account do not exist
	{
		// add addind - fix
		cout << ATM_ID << ": New account id is " << acount_id << " with password " << password << " and initial balance " << init_balance << endl;
	}
	else   // the account already exist
	{
		cout << "Error " << ATM_ID << ": Your transaction failed – account with the same id exists" << endl;
	}
	pthread_mutex_unlock(&mutex_accountsDB_write);
}

void bank::Deposit_Account(int acount_id, int password, int amount, int ATM_ID)   // ?????????????
{
	pthread_mutex_lock(&db_read_counter_mutex);
	if (!db_readers_counter++)
	{
		pthread_mutex_lock(&mutex_accountsDB_write);
	}
	pthread_mutex_unlock(&db_read_counter_mutex);
	// do something
	pthread_mutex_lock(&db_read_counter_mutex);
	if (!db_readers_counter)
	{
		pthread_mutex_unlock(&mutex_accountsDB_write);
	}
	pthread_mutex_unlock(&mutex_accountsDB_write);

}

void bank::Withdraw_Account(int acount_id, int password, int amount, int ATM_ID)
{

}

void bank::Get_Balance_Account(int acount_id, int password, int ATM_ID)
{

}

void bank::Quit_Account(int acount_id, int password, int ATM_ID)
{
	pthread_mutex_lock(&mutex_accountsDB_write);
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
	pthread_mutex_unlock(&mutex_accountsDB_write);

}

void bank::Transfer_Account(int acount_id_source, int password, int acount_id_target, int amount, int ATM_ID)
{

}




