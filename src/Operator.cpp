#include "Operator.h"

Operator::~Operator(void) {}

Operator::Operator(string symbol, int type, int priority)
: symbol(symbol), type(type), priority(priority) {}

string Operator::getSymbol()
{
	return symbol;
}

int Operator::getPriority()
{
	return priority;
}

int Operator::getType()
{
	return type;
}