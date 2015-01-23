#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include "Operator.h"
#include "Node.h"
#include <vector>
#include <map>

class ExpressionEvaluator
{
	friend class Node;

private:
	static vector<Operator> operators;
        map<string, double> variables;
	Node* rootNode;
	string expression;

public:
        ExpressionEvaluator(void);
	~ExpressionEvaluator(void);

	void setVariable(string s, double d);
	void setExpression(string s);
	string getExpression();
	double getValue();
        Node* getRootNode();
//        ExpressionEvaluator* getDeepCopy();

        void trace();

private:
	void init();
	void initializeOperators();
	void initializeVariables();
	string prepareExpressionString(string s);
	double evaluate(Node* n);
	double evaluateOper(Operator* o, double v1, double v2 = 0.0);
};

#endif
