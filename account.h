#include <stdlib.h>
#include <string>
#include <pthread.h>


typedef enum result_ {PASS_ERR, BALANCE_ERR , SUCCESS} result;

using namespace std;

class account
{
private:
	const int account_id_;
	int balance_;
	const int password_;
	unsigned int readers_counter_;
	pthread_mutex_t write_mutex_;
	pthread_mutex_t read_counter_mutex_;
public:
	account(int id, int pass, int init_balance);
	~account();
	int getPassword() const;
	int getID() const;
	int updateBalance(int amount);
	int getBalance();

};