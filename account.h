#include <stdlib.h>
#include <string>
#include <mutex>


typedef enum result_ {PASS_ERR, BALANCE_ERR , SUCCESS} result;

using namespace std;

class account
{
private:
	const int account_id_;
	int balance_;
	const int password_;
	unsigned int readers_counter;
	mutex write_mutex;
	mutex read_counter_mutex;
public:
	account(int id, int pass, int init_balance) : account_id_(id), password_(pass), balance_(init_balance), readers_counter(0){};
	int getPassword() const;
	int getID() const;
	int updateBalance(int amount);
	int getBalance();

};