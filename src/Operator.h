#ifndef OPERATOR_H
#define OPERATOR_H

#include <string>
using namespace std;

class Operator
{
private:
	string symbol;
	int type;
	int priority;

public:
	Operator(string symbol, int type, int priority);
	~Operator(void);

	string getSymbol();
	void setSymbol(string symbol);
	int getType();
	int getPriority();

	enum {UNARY = 1, BINARY = 2};
};

#endif