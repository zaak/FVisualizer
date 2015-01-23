#include "Node.h"
#include "ExpressionEvaluator.h"
#include "Operator.h"

#include <cstdlib>
#include <iostream>

Node::Node(string s, ExpressionEvaluator *e)
: nOperator(NULL), nLeft(NULL), nRight(NULL), nParent(NULL), hasDoubleValue(false), evaluator(e)
{
	init(NULL, s, 0);
}

Node::Node(Node* parent, string s, ExpressionEvaluator *e, int level)
: nOperator(NULL), nLeft(NULL), nRight(NULL), nParent(NULL), hasDoubleValue(false), evaluator(e)
{
	init(parent, s, level);
}

Node::~Node(void) {
	if(nLeft != NULL) { delete nLeft; nLeft = NULL; };
	if(nRight != NULL) { delete nRight; nRight = NULL; };
}

// Inicjalizacja węzła
void Node::init(Node* parent, string s, int level)
{
	if(s.empty()) throw ParsingException();
	prepareNodeString(s);
	if(!bracketsOk(s)) throw WrongNumberOfBracketsException();

	nParent = parent;
	nString = s;
	nLevel = level;
	getDouble(s);
	int inBrackets = 0;
	int startOperator = 0;

	for(unsigned int i = 0; i < s.length(); ++i)
	{
		if(s[i] == '(')
			++inBrackets;
		else if(s[i] == ')')
			--inBrackets;
		else if(inBrackets == 0)
		{
			Operator* o = getOperator(nString, i);
			if(o != NULL)
			{
				if(nOperator == NULL || nOperator->getPriority() >= o->getPriority())
				{
					nOperator = o;
					startOperator = i;
				}

				i += o->getSymbol().length()-1;
			}
		}
	}

	if(nOperator != NULL)
	{
		if(startOperator == 0 && nOperator->getType() == Operator::UNARY)
		{
			if(bracketsOk(s.substr(nOperator->getSymbol().length())))
			{
                                nLeft = new Node(this, s.substr(nOperator->getSymbol().length()), evaluator, nLevel+1);
				nRight = NULL;
			} else {
				throw WrongNumberOfBracketsException();
			}

		}
		else if(startOperator > 0 && nOperator->getType() == Operator::BINARY)
		{
                        nLeft = new Node(this, s.substr(0, startOperator), evaluator, nLevel+1);
                        nRight = new Node(this, s.substr(startOperator + nOperator->getSymbol().length()), evaluator, nLevel+1);
		}
	}
}

// Przygotowanie wyrażenia do etapu parsowania
void Node::prepareNodeString(string& s)
{
	// Usunięcie zbędnych nawiasów na początku i końcu wyrażenia
	while(s.length() > 2 &&
		  s.find_first_of('(') == 0 &&
		  s.find_last_of(')') == s.length()-1 &&
		  bracketsOk(s.substr(1, s.length()-2)))
	{
		s.erase(0,1);
		s.erase(s.length()-1,1);
	}

	// Dodaj zera do wyrażeń zaczynających się od "+" lub '-"
	if(s.find_first_of("+-") == 0)
	{
		s.insert(0, "0");
	}
}


/*
 * Czy liczba nawiasów w wyrażeniu jest prawidłowa?
 * Zwraca true jeśli liczba nawiasów jest prawidłowa
 */
bool Node::bracketsOk(const string &s)
{
	int inBrackets = 0;

	for(unsigned int i = 0; i < s.length(); ++i)
	{
		if(s[i] == '(' && inBrackets >= 0)
			++inBrackets;
		else if(s[i] == ')')
			--inBrackets;
	}

	return inBrackets == 0;
}

void Node::getDouble(const string& s)
{
	char* endp;

	double d = strtod(s.c_str(), &endp);

	if(*endp == '\0')
	{
		hasDoubleValue = true;
		nValue = d;
	}
}

Operator* Node::getOperator(string s, int start)
{
	vector<Operator>& operators = ExpressionEvaluator::operators;

	string temp = getNextWord(s.substr(start));

	for(unsigned int i = 0; i < operators.size(); ++i)
    {
		if(temp.find(operators[i].getSymbol()) == 0)
		{
			return &operators[i];
		}
    }

	return NULL;
}

string Node::getNextWord(string s)
{
	for(unsigned int i = 1; i < s.length(); ++i)
    {
    	char c = s[i];
        if ( (c > 'z' || c < 'a') && (c > '9' || c < '0') )
        	return s.substr(0, i);
    }
    return s;
}

void Node::trace()
{
	string op = nOperator == NULL ? " " : nOperator->getSymbol();
	string s;
	for(int i = 0; i < nLevel; ++i) s += "  ";
	cout << s << "|" << op << " : " << nString;
	if(hasDoubleValue)
		cout << "\tnValue[" << nValue << "]" << endl;
	else
		cout << "\tnString(" << nString << ")" << endl;

	if(nLeft != NULL) nLeft->trace();
	if(nRight != NULL) nRight->trace();
}

double Node::getNodeValue()
{
	if(hasDoubleValue) return nValue;

	map<string, double>::iterator it;

        it = evaluator->variables.find(nString);
        if(it != evaluator->variables.end())
		return it->second;
	else
		throw ParsingException();
}

bool Node::hasConstant()
{
        char* endp;

        strtod(nString.c_str(), &endp);

        if(*endp == '\0')
        {
                return true;
        }

        map<string, double>::iterator it;

        it = evaluator->variables.find(nString);
        if(it != evaluator->variables.end())
                if(it->first != "x")
                        return true;

        return false;
}

void Node::setNodeValue(double d)
{
	nValue = d;
	hasDoubleValue = true;
}

Operator* Node::getOperator()
{
	return nOperator;
}

Node* Node::getLeft()
{
	return nLeft;
}

Node* Node::getRight()
{
	return nRight;
}

string& Node::getNString()
{
    return nString;
}
