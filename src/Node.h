#ifndef NODE_H
#define NODE_H

#include <string>

class ExpressionEvaluator;
class Operator;

using namespace std;

class Node
{
private:
        ExpressionEvaluator* evaluator;
	string nString;
	Operator* nOperator;
	Node* nLeft;
	Node* nRight;
	Node* nParent;
	int nLevel;
	double nValue;
	bool hasDoubleValue;

public:
        Node(string s, ExpressionEvaluator* e);
        Node(Node* parent, string s, ExpressionEvaluator* e, int level);
	~Node(void);
	double getNodeValue();
	void setNodeValue(double d);
	Operator* getOperator();
	Node* getLeft();
	Node* getRight();
        string& getNString();
        bool hasConstant();
	void trace();

private:
	void init(Node* parent, string s, int level);
	bool bracketsOk(const string& s);
	void prepareNodeString(string& s);
	void getDouble(const string& s);
	Operator* getOperator(string s, int start);
	string getNextWord(string s);

};

class ParsingException : public exception {};
class WrongNumberOfBracketsException : public ParsingException {};

#endif
