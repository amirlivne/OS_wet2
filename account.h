#include <stdlib.h>
#include <string>

enum result {PASS_ERR, BALANCE_ERR , SUCCESS};

using namespace std;

class account {
private:
	int account_id;
	double balance;
	string password;
public:
	account(int id, string pass, double init_balance) : account_id(id), password(pass), balance(init_balance) {}
	~account();
	string getPassword();
	double deposite(double amount);
	double withdrawal(double amount);
	double getBalance();
	
	
}