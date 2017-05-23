#include "account.h"
#include "bank.h"
#define HAVE_STRUCT_TIMESPEC

#define ACCOUNT_NOT_EXISTS "Error " << ATM_ID << ": Your transaction failed – account id " << account_id << " does not exist" << endl
#define DEST_ACCOUNT_NOT_EXISTS "Error " << ATM_ID << ": Your transaction failed – account id " << account_id_target << " does not exist" << endl
#define INCORECT_PASSWORD "Error " << ATM_ID << ": Your transaction failed – password for account id " << account_id << " is incorrect" << endl
#define BALANCE_NOT_SUFFICIENT "Error " << ATM_ID << ": Your transaction failed – account id " << account_id << " balance is lower than " << amount << endl
#define WITHDRAWAL_SUCCEEDED ATM_ID << ": Account " << account_id << " new balance is " << rv << " after " << amount << " $ was withdrew" << endl
#define CURRENT_BALANCE ATM_ID << ": Account " << account_id << " balance is " << curr_balance
#define COMMISSION_TAKEN  "Bank: commision of " << com_rate << " % were charged, the bank gained " << com << " $ from account " << tmp_account.getID() << endl
using namespace std;

extern ofstream Log_file;
extern pthread_mutex_t log_file_mutex;


/* FUNCTIONS FOR INTERNAL USE OF THE BANK */
////////////////////////////////////////////
//********************************************
// function name: logPrint
// Description: a local function to safely print text into Log_file
// Parameters: ostringstream that containts the message relevant for printing
// Returns: NONE
//***********************************************
void logPrint(ostringstream* message)
{
	pthread_mutex_lock(&log_file_mutex); //lock the log file before writing
	Log_file << (*message).str() << endl;
	pthread_mutex_unlock(&log_file_mutex); //unlock the log file
}

//********************************************
// function name: readerEnter
// Description: safely adds 1 to the readers counter in the bank to 
// Parameters: NONE
// Returns: NONE
//***********************************************
void bank::readerEnter()
{
	pthread_mutex_lock(&db_read_counter_mutex);
	if (!db_readers_counter++)
	{
		pthread_mutex_lock(&mutex_accountsDB_write);
	}
	pthread_mutex_unlock(&db_read_counter_mutex);
}

//********************************************
// function name: readerLeave
// Description: safely subs 1 from thr readers counter in the bank
// Parameters: NONE
// Returns: NONE
//***********************************************
void bank::readerLeave()
{
	pthread_mutex_lock(&db_read_counter_mutex);
	if (!--db_readers_counter)
	{
		pthread_mutex_unlock(&mutex_accountsDB_write);
	}
	pthread_mutex_unlock(&mutex_accountsDB_write);
}

/* MEMBER BANK FUNCTIONS */
///////////////////////////
bank::bank()
{
	bank_money_ = 0;
	db_readers_counter = 0;

	//initilizing the readers-writers mutexes:
	pthread_mutex_init(&mutex_accountsDB_write, NULL);
	pthread_mutex_init(&db_read_counter_mutex, NULL);
	pthread_mutex_init(&bank_balance_mutex, NULL);	
}

bank::~bank()
{
	//destroying the readers-writers mutexes:
	pthread_mutex_destroy(&mutex_accountsDB_write);
	pthread_mutex_destroy(&db_read_counter_mutex);
	pthread_mutex_destroy(&bank_balance_mutex);
}


//********************************************
// function name: bank_commision
// Description: takes commision from all of the bank's accounts
// Parameters: an integer (between 2-4) that means the commisions rate.
// Returns: NONE
//***********************************************
void bank::Bank_Commission(int com_rate)
{
	ostringstream print_to_log;
	readerEnter();
	for (map<int, account>::iterator it = accounts_.begin(); it != accounts_.end(); ++it)
	{
		account tmp_account = it->second; //  read the account;
		int com = tmp_account.payCommision(com_rate); //take commision from the account
		pthread_mutex_lock(&bank_balance_mutex); // lock the bank's balance before updating it
		bank_money_ += com;
		pthread_mutex_unlock(&bank_balance_mutex); // unlock the bank's balance
		print_to_log << COMMISSION_TAKEN;
		logPrint(&print_to_log);
	}
	readerLeave();
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

//********************************************
// function name: Withdraw_Account
// Description: Withdraw a certain amount of money from an account and prints it to the log file
// Parameters: 4 int - account_id, account_password,  amount_to_withdraw, and ATM_ID
// Returns: NONE
//***********************************************
void bank::Withdraw_Account(int account_id, int password, int amount, int ATM_ID)
{
	ostringstream print_to_log;
	readerEnter(); // db reader enter
	map<int, account>::iterator it = accounts_.find(account_id);
	if (it == accounts_.end())	// the account does not exist
	{
		print_to_log << ACCOUNT_NOT_EXISTS;
	}
	else if (!Password(it->second.getID(), password))	// the password is incorrect
	{	
		print_to_log << INCORECT_PASSWORD;
	}
	else //perform withdrawal
	{
		int rv = it->second.updateBalance(-1 * amount);
		if (rv == -1) //if there was not enough money in the account to perform the withdrawal
		{
			print_to_log << BALANCE_NOT_SUFFICIENT;
		}
		else //if the withdrawal was succesful
		{
			print_to_log << WITHDRAWAL_SUCCEEDED;
		}
	}
	readerLeave(); // db reader leave
	logPrint(&print_to_log);
}

//********************************************
// function name: Get_Balance_Account
// Description: gets the current balance of an acount and prints it to the log file
// Parameters: 3 int - account_id, account_password, and ATM_ID
// Returns: NONE
//***********************************************
void bank::Get_Balance_Account(int account_id, int password, int ATM_ID)
{
	ostringstream print_to_log;
	readerEnter(); // db reader enter
	map<int, account>::iterator it = accounts_.find(account_id);
	if (it == accounts_.end())	// the account does not exist
	{
		print_to_log << ACCOUNT_NOT_EXISTS;
	}
	else if (!Password(it->second.getID(), password))	// the password is incorrect
	{
		print_to_log << INCORECT_PASSWORD;
	}
	else
	{
		int curr_balance = it->second.getBalance();
		print_to_log << CURRENT_BALANCE;
	}
	readerLeave(); // db reader leave
	logPrint(&print_to_log);
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

void bank::Transfer_Account(int account_id, int password, int account_id_target, int amount, int ATM_ID)
{
	ostringstream print_to_log;
	readerEnter(); // db reader enter
	map<int, account>::iterator it1 = accounts_.find(account_id);
	map<int, account>::iterator it2 = accounts_.find(account_id_target);
	if (it1 == accounts_.end())	// the source account does not exist
	{
		print_to_log << ACCOUNT_NOT_EXISTS;
	}
	else if (it2 == accounts_.end()) //the dest account does not exists
	{
		print_to_log << DEST_ACCOUNT_NOT_EXISTS;
	}
	else if (!Password(it2->second.getID(), password))	// the password is incorrect
	{
		print_to_log << INCORECT_PASSWORD;
	}
	else //transaction is valid
	{

	}
	readerLeave(); // db reader leave
	logPrint(&print_to_log);

}




