#include "ExpressionEvaluator.h"
#include <cmath>

vector<Operator> ExpressionEvaluator::operators;

ExpressionEvaluator::ExpressionEvaluator(void)
: rootNode(0)
{
	init();
}

//ExpressionEvaluator* ExpressionEvaluator::getDeepCopy()
//{
//    ExpressionEvaluator* e = new ExpressionEvaluator(*this);
//    e->setExpression(this->getExpression());
//
//    return e;
//}

ExpressionEvaluator::~ExpressionEvaluator(void)
{
	if(rootNode != NULL) delete rootNode;
}

void ExpressionEvaluator::init()
{
	if(operators.empty())
		initializeOperators();
	if(variables.empty())
		initializeVariables();
}

string ExpressionEvaluator::prepareExpressionString(string s)
{
		// Usunięcie zbędnych znaków
	string illegalChars = " \t\n\r";
	size_t found;
	while((found = s.find_first_of(illegalChars)) != string::npos)
	{
		s.erase(found, 1);
	}

	return s;
}

void ExpressionEvaluator::initializeOperators()
{
    operators.push_back(Operator("+"	, Operator::BINARY, 0));
    operators.push_back(Operator("-"	, Operator::BINARY, 0));
    operators.push_back(Operator("*"	, Operator::BINARY, 10));
    operators.push_back(Operator("/"	, Operator::BINARY, 10));
    operators.push_back(Operator("^"	, Operator::BINARY, 11));

    operators.push_back(Operator("sin" 	, Operator::UNARY,  20));
    operators.push_back(Operator("cos" 	, Operator::UNARY,  20));
    operators.push_back(Operator("tan" 	, Operator::UNARY,  20));
    operators.push_back(Operator("ctan" , Operator::UNARY,  20));
    operators.push_back(Operator("hsin" , Operator::UNARY,  20));
    operators.push_back(Operator("hcos" , Operator::UNARY,  20));
    operators.push_back(Operator("htan" , Operator::UNARY,  20));
    operators.push_back(Operator("hctan", Operator::UNARY,  20));
    operators.push_back(Operator("asin"	, Operator::UNARY,  20));
    operators.push_back(Operator("acos"	, Operator::UNARY,  20));
    operators.push_back(Operator("atan"	, Operator::UNARY,  20));
    operators.push_back(Operator("actan", Operator::UNARY,  20));
    operators.push_back(Operator("sqrt"	, Operator::UNARY,  20));
    operators.push_back(Operator("ln" 	, Operator::UNARY,  20));
    operators.push_back(Operator("log" 	, Operator::UNARY,  20));
    operators.push_back(Operator("exp" 	, Operator::UNARY,  20));
//    operators.push_back(Operator("floor", Operator::UNARY,  20));
//    operators.push_back(Operator("ceil" , Operator::UNARY,  20));
//    operators.push_back(Operator("abs"  , Operator::UNARY,  20));
}

void ExpressionEvaluator::initializeVariables()
{
	variables["e"] = 2.71828183;
	variables["pi"] = 3.14159265;
}

void ExpressionEvaluator::setExpression(string s)
{
	expression = s;
	if(rootNode != NULL) { delete rootNode; rootNode = NULL; };
        rootNode = new Node(prepareExpressionString(s), this);
}

double ExpressionEvaluator::evaluate(Node* n)
{
	if(n->getOperator() != NULL && (n->getLeft() != NULL || n->getRight() != NULL))
	{
		if(n->getOperator()->getType() == Operator::BINARY)
			n->setNodeValue(evaluateOper(n->getOperator(), evaluate(n->getLeft()), evaluate(n->getRight())));
		if(n->getOperator()->getType() == Operator::UNARY)
			n->setNodeValue(evaluateOper(n->getOperator(), evaluate(n->getLeft())));
	}

	return n->getNodeValue();
}

double ExpressionEvaluator::evaluateOper(Operator* o, double v1, double v2)
{
	string op = o->getSymbol();

	// Binary ops
	if(op == "+")    return v1 + v2;
	if(op == "-")    return v1 - v2;
	if(op == "*")    return v1 * v2;
	if(op == "/")    return v1 / v2;
	if(op == "^")    return pow(v1, v2);

	// Unary ops
        if(op == "sin")     return sin(v1);
        if(op == "cos")     return cos(v1);
        if(op == "tan")     return tan(v1);
        if(op == "ctan")    return 1.0/tan(v1);
        if(op == "hsin")    return sinh(v1);
        if(op == "hcos")    return cosh(v1);
        if(op == "htan")    return tanh(v1);
        if(op == "hctan")   return 1.0/tanh(v1);
        if(op == "asin")    return asin(v1);
        if(op == "acos")    return acos(v1);
        if(op == "atan")    return atan(v1);
        if(op == "actan")   return variables["pi"]/2-atan(v1);
        if(op == "sqrt")    return sqrt(v1);
        if(op == "ln")      return log(v1);
        if(op == "log")     return log10(v1);
        if(op == "exp")     return exp(v1);
//        if(op == "floor")   return floor(v1);
//        if(op == "ceil")    return ceil(v1);
        if(op == "abs")     return abs(v1);

        return sqrt(-1.0); //NaN
}

double ExpressionEvaluator::getValue()
{
	return evaluate(rootNode);
}

void ExpressionEvaluator::setVariable(string s, double d)
{
	ExpressionEvaluator::variables[s] = d;
}

string ExpressionEvaluator::getExpression()
{
	return expression;
}

void ExpressionEvaluator::trace()
{
	if(rootNode != NULL)
		rootNode->trace();
}

Node* ExpressionEvaluator::getRootNode()
{
    return rootNode;
}
