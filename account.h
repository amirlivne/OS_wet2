#include <stdlib.h>
#include <string>

typedef enum result_ {PASS_ERR, BALANCE_ERR , SUCCESS} result;

using namespace std;

class account
{
	
	int account_id;
	int balance;
	string password;
public:
	account(int id, string pass, int init_balance) : account_id(id), password(pass), balance(init_balance) {};
	~account();
	string getPassword();
	int deposite(double amount);
	int withdrawal(double amount);
	int getBalance();
	
	
};