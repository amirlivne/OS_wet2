#include "account.h"
#include "bank.h"
//#define HAVE_STRUCT_TIMESPEC

#define ACCOUNT_NOT_EXISTS "Error " << ATM_ID << ": Your transaction failed – account id " << account_id << " does not exist"
#define DEST_ACCOUNT_NOT_EXISTS "Error " << ATM_ID << ": Your transaction failed – account id " << account_id_target << " does not exist"
#define INCORECT_PASSWORD "Error " << ATM_ID << ": Your transaction failed – password for account id " << account_id << " is incorrect"
#define BALANCE_NOT_SUFFICIENT "Error " << ATM_ID << ": Your transaction failed – account id " << account_id << " balance is lower than " << amount
#define WITHDRAWAL_SUCCEEDED ATM_ID << ": Account " << account_id << " new balance is " << rv << " after " << amount << " $ was withdrew"
#define CURRENT_BALANCE ATM_ID << ": Account " << account_id << " balance is " << curr_balance
#define DEPOSIT_SUCCEEDED ATM_ID << ": Account id " << account_id << " new balance is " << new_balance << " after " << amount << " $ was deposited"
#define ACCOUNT_ALREADY_EXISTS "Error " << ATM_ID << ": Your transaction failed – account with the same id exists"
#define OPEN_ACCOUNT_SUCCEEDED ATM_ID << ": New account id is " << account_id << " with password " << password << " and initial balance " << init_balance
#define COMMISSION_TAKEN  "Bank: commision of " << com_rate << " % were charged, the bank gained " << com << " $ from account " << it->second.getID()

using namespace std;

extern ofstream Log_file;
extern sem_t log_file_mutex;


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
	sem_wait(&log_file_mutex); //lock the log file before writing
	Log_file << (*message).str() << endl;
	sem_post(&log_file_mutex); //unlock the log file
}

//********************************************
// function name: readerEnter
// Description: safely adds 1 to the readers counter in the bank to 
// Parameters: NONE
// Returns: NONE
//***********************************************
void bank::readerEnter()
{
	sem_wait(&db_read_counter_mutex);
	if (!db_readers_counter++)
	{
		sem_wait(&mutex_accountsDB_write);
	}
	sem_post(&db_read_counter_mutex);
}

//********************************************
// function name: readerLeave
// Description: safely subs 1 from thr readers counter in the bank
// Parameters: NONE
// Returns: NONE
//***********************************************
void bank::readerLeave()
{
	sem_wait(&db_read_counter_mutex);
	db_readers_counter--;
	if (!db_readers_counter)
	{
		sem_post(&mutex_accountsDB_write);
	}
	sem_post(&db_read_counter_mutex);
}

/* MEMBER BANK FUNCTIONS */
///////////////////////////


//********************************************
// function name: ~bank
// Description: constructor
// Parameters: NONE
// Returns: NONE
//***********************************************
bank::bank()
{
	bank_money_ = 0;
	db_readers_counter = 0;

	//initilizing the readers-writers mutexes:
	sem_init(&mutex_accountsDB_write, 0, 1);
	sem_init(&db_read_counter_mutex, 0, 1);
	sem_init(&bank_balance_mutex, 0, 1);
}

//********************************************
// function name: ~bank
// Description: destructor
// Parameters: NONE
// Returns: NONE
//***********************************************
bank::~bank()
{
	//destroying the readers-writers mutexes:
	sem_destroy(&mutex_accountsDB_write);
	sem_destroy(&db_read_counter_mutex);
	sem_destroy(&bank_balance_mutex);
}

//********************************************
// function name: bank_commision
// Description: takes commision from all of the bank's accounts
// Parameters: an integer (between 2-4) that means the commisions rate.
// Returns: NONE
//***********************************************
void bank::Bank_Commission(int com_rate)
{
	readerEnter();
	for (map<int, account>::iterator it = accounts_.begin(); it != accounts_.end(); ++it)
	{
		ostringstream print_to_log;
		int com = it->second.payCommision(com_rate); //take commision from the account
		sem_wait(&bank_balance_mutex); // lock the bank's balance before updating it
		bank_money_ += com;
		sem_post(&bank_balance_mutex); // unlock the bank's balance
		print_to_log << COMMISSION_TAKEN;
		logPrint(&print_to_log);
	}
	readerLeave();
}

//********************************************
// function name: Password
// Description: checkes if the password for an account is correct
// Parameters: int - account_id, int - password
// Returns: True - if the password is correct, False - if the account does not exist ar the password is incorrect
//***********************************************
bool bank::Password(int account_id, int password)
{
	if (accounts_.find(account_id) == accounts_.end())   // checks if the account exist
	{
		return false;
	}
	return ((*accounts_.find(account_id)).second.getPassword() == password);	 // checks if the passowrd for the account is correct
}

//********************************************
// function name: Print_Bank
// Description: prints each all accounts status
// Parameters: NONE
// Returns: NONE
//***********************************************
void bank::Print_Bank() 
{
	ostringstream print_to_log;
	cout << "Current Bank Status" << endl;
	readerEnter();
	for (map<int, account>::iterator it = accounts_.begin(); it != accounts_.end(); ++it)
	{
		cout << "Account " << (*it).second.getID() << ": Balance - " << (*it).second.getBalance() << " $ , Account Password - " << (*it).second.getPassword() << endl;
	}
	sem_wait(&bank_balance_mutex);
	cout << "The Bank has " << bank_money_ << " $" << endl;
	sem_post(&bank_balance_mutex);
	readerLeave();
}

//********************************************
// function name: Open_Account
// Description: Open a new Account
// Parameters: int - account_id, int - password, int - init_balance, int - ATM_ID
// Returns: NONE
//***********************************************
void bank::Open_Account(int account_id, int password, int init_balance, int ATM_ID)
{
	ostringstream print_to_log;
	sem_wait(&mutex_accountsDB_write);
	if (accounts_.find(account_id) == accounts_.end())	// the account do not exist
	{
		account new_account(account_id, password, init_balance);
		accounts_.insert(pair<int, account>(account_id, new_account));
		print_to_log << OPEN_ACCOUNT_SUCCEEDED;
	}
	else   // the account already exist
	{
		print_to_log << ACCOUNT_ALREADY_EXISTS;
	}
	sem_post(&mutex_accountsDB_write);
	logPrint(&print_to_log);
}

//********************************************
// function name: Deposit_Account
// Description: Deposit a certain amount of money to an account and prints it to the log file
// Parameters:  int - account_id, int - account_password,  int - amount_to_withdraw, int - ATM_ID
// Returns: NONE
//***********************************************
void bank::Deposit_Account(int account_id, int password, int amount, int ATM_ID)   // ?????????????
{
	ostringstream print_to_log;
	readerEnter(); // db reader enter
	if (accounts_.find(account_id) == accounts_.end())	// the account do not exist
	{
		print_to_log << ACCOUNT_NOT_EXISTS;
	}
	else if (!Password(account_id, password))	// the account exist and the password is incorrect
	{
		print_to_log << INCORECT_PASSWORD;
	}
	else  //perform deposit
	{
		int new_balance = (*accounts_.find(account_id)).second.updateBalance(amount);
		print_to_log << DEPOSIT_SUCCEEDED;
	}
	readerLeave(); // db reader leave
	logPrint(&print_to_log);
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
// Description: gets the current balance of an account and prints it to the log file
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

//********************************************
// function name: Quit_Account
// Description: Close an Account
// Parameters: int - account_id, int - password, int - ATM_ID
// Returns: NONE
//***********************************************
void bank::Quit_Account(int account_id, int password, int ATM_ID)
{
	ostringstream print_to_log;
	sem_wait(&mutex_accountsDB_write);
	if (accounts_.find(account_id) == accounts_.end())	// the account do not exist
	{
		print_to_log << ACCOUNT_NOT_EXISTS;
	}
	else if (!Password(account_id, password))	// the account exist and the password is incorrect
	{
		print_to_log << INCORECT_PASSWORD;
	}
	else   // the account exist and the password is correct
	{
		print_to_log << ATM_ID << ": Account id " << account_id << " is now closed. Balance was " << (*accounts_.find(account_id)).second.getBalance();
		accounts_.erase(account_id);
	}
	sem_post(&mutex_accountsDB_write);
	logPrint(&print_to_log);
}

//********************************************
// function name: Transfer_Account
// Description: transfer amount from source account to target account
// Parameters: int - account_id_source, int - password, int - account_id_target, int - amount, int - ATM_ID
// Returns: NONE
//***********************************************
void bank::Transfer_Account(int account_id, int password, int account_id_target, int amount, int ATM_ID)
{
	ostringstream print_to_log;
	readerEnter(); // db reader enter
	map<int, account>::iterator source = accounts_.find(account_id);
	map<int, account>::iterator target = accounts_.find(account_id_target);
	
	if (source == accounts_.end())	// the source account does not exist
	{
		print_to_log << ACCOUNT_NOT_EXISTS;
	}
	else if (target == accounts_.end())	// the target account does not exist
	{
		print_to_log << "Error " << ATM_ID << ": Your transaction failed – account id " << account_id_target << " does not exist";
	}
	else
	{
		// lock the accouts from being changed
		(*source).second.lockAccount();
		(*target).second.lockAccount();
		if (!Password(account_id, password))	// the source account exist and the password is incorrect
		{
			print_to_log << INCORECT_PASSWORD;
		}
		else
		{
			int source_balance = (*source).second.moneyTransfer(-1 * amount);
			if (source_balance >= 0)
			{
				int target_balance = (*target).second.moneyTransfer(amount);
				print_to_log << ATM_ID << ": Transfer " << amount << " from account " << account_id << " to account " << account_id_target << " new account balance is " << source_balance << " new target account balance is " << target_balance;
			}
			else  // the source account do not have enough money in his account
			{
				print_to_log << BALANCE_NOT_SUFFICIENT;
			}
		}
		(*source).second.unlockAccount();
		(*target).second.unlockAccount();
	}
	readerLeave(); // db reader leave
	logPrint(&print_to_log);
}




