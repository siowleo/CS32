#include <iostream>
#include <stack>
#include <string>
#include <cctype>
#include <cassert>
using namespace std;

int evaluate(string infix, string& postfix, bool& result);
void whiteSpaces(string &infix);
bool evaluateInfix(string infix);
int operatorPrecedence(const char other);
bool matchingBraces(string infix);
bool convertCharToBool(char other);
bool operators(const char other);
int helperFunctions(const char other);
const int Operators = 1;

int evaluate(string infix, string& postfix, bool& result)
{//-------------Error check------------------
	postfix = "";
	stack<char> operatorStack;
	whiteSpaces(infix);
	if (!evaluateInfix(infix))
	{
		return 1;
	}
	//---------------------Infix to postfix conversion ----------------------------
	for (unsigned int i = 0; i < infix.size(); i++)
		{
			switch (infix[i])
			{

			case '!':
			case '(':
				operatorStack.push(infix[i]);
				break;
			case ')':
				while (operatorStack.top() != '(')
				{
					postfix += operatorStack.top();
					operatorStack.pop();
				}
				operatorStack.pop();
				break;
			case '&':
			case '|':
				while (!operatorStack.empty() && operatorStack.top() != '(' && operatorPrecedence(infix[i]) >= operatorPrecedence(operatorStack.top()))
				{
					postfix += operatorStack.top();
					operatorStack.pop();
				}
				operatorStack.push(infix[i]);
				break;
			case 'T':
			case 'F':
				postfix += infix[i];
				break;
			}
		}
		while (!operatorStack.empty())
		{
			postfix += operatorStack.top();
			operatorStack.pop();
		}

//-------------------------------Postfix evaluation -------------------------------------
	stack<bool> operandStack;
	for (int k = 0; k < postfix.size(); k++)
	{
		if (postfix[k] == 'T' || postfix[k] == 'F')
		{
			bool x = convertCharToBool(postfix[k]);
			operandStack.push(x);
		}
		else
		{
			bool operand2 = operandStack.top();
			operandStack.pop();
			if (postfix[k] == '|')
			{
				bool operand1 = operandStack.top();
				operandStack.pop();
				operandStack.push(operand2 || operand1);
			}
			if (postfix[k] == '!')
			{
				operandStack.push(!operand2);
			}
			if (postfix[k] == '&')
			{
				bool operand1 = operandStack.top();
				operandStack.pop();
				operandStack.push(operand2 && operand1);
			}
		}
	}
	result = operandStack.top();
	return 0;
}

int helperFunctions(const char other)
{
	if (operators(other))
	{
		return Operators;
	}

}

bool operators(const char other)
{
	return other == '&' || other == '|';
}

int operatorPrecedence(const char other)
{
	// use integers to determine the operator precedence
	if (other == '!')
	{
		return 1;
	}
	else if (other == '&')
	{
		return 2;
	}
	else if (other == '|')
	{
		return 3;
	}
	else
	{
		return -1;
	}
}

void whiteSpaces(string &infix)
{
	//remove white spaces
	string temp = "";
	for (int i = 0; i < infix.size(); i++)
	{
		if (infix[i] != ' ')
		{
			temp += infix[i];
		}
	}
	infix = temp;
}
bool matchingBraces(string infix)
{
	//check if number of left braces is equivalent to the number of right braces
	int leftBraces = 0;
	int rightBraces = 0;
	for (int i = 0; i < infix.size(); i++)
	{
		if (infix[i] == '(')
		{
			leftBraces++;
		}
		if (infix[i] == ')')
		{
			rightBraces++;
		}
	}
	if (leftBraces == rightBraces)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool convertCharToBool(char other)
{
	//conert character to boolean
	if (other == 'F')
		return false;
	else
		return true;
}


bool evaluateInfix(string infix)
{
	whiteSpaces(infix); //error check for white spaces
	if (infix.empty()) //if infix is empty, return false
	{
		return false;
	}
	//error check first character
	if (infix.size() == 1)
	{
		if (infix == "T" || infix == "F")
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	if (infix[0] == '&' || infix[0] == ')' || infix[0] == '|')
	{
		return false;
	}
	int lastCharacter = infix.size() - 1;
	if (infix[lastCharacter] == '&' || infix[lastCharacter] == '|' || infix[lastCharacter] == '!' || infix[lastCharacter] == '(')
	{
		return false;
	}
	for (int i = 0; i < infix.size(); i++)
	{
		if (infix[i] != '!' && infix[i] != '|' && infix[i] != '(' && infix[i] != ')'  && infix[i] != 'T' && infix[i] != 'F' && infix[i] != '&')
			return false;
	}
	//error check middle of body
	for (int k = 0; k < infix.size(); k++)
	{
		if (!matchingBraces(infix))
		{
			return false;
		}
		else if ((infix[0] == 'T' || infix[0] == 'F') && (infix[1] == '!' || infix[1] == 'F' || infix[1] == 'T' ||  infix[1] == ')'))
		{
			return false;
		}
		else if ((infix[0] == '!') && (((infix[1] == '|') || infix[1] == '&') || (infix[1] == ')')))
		{
			return false;
		}
		else if (((infix[1] == '|') || infix[0] == '(') && ((infix[1] == '&') || (infix[1] == ')')))
		{
			return false;
		}
		else if ((infix[k] == '|' || infix[k] == '&') && k != 0)
		{
			if (infix[k - 1] == '&' || infix[k - 1] == '!' ||  infix[k - 1] == '|')
			{
				return false;
			}
			if (infix[k + 1] == ')' || infix[k + 1] == '&' || infix[k + 1] == '|')
			{
				return false;
			}
		}
		else if (infix[k] == '!' && k != 0)
		{
			if (infix[k + 1] == ')' || infix[k + 1] == '&' || infix[k + 1] == '|')
			{
				return false;
			}
			if (infix[k - 1] == ')' || infix[k - 1] == 'T' || infix[k - 1] == 'F')
			{
				return false;
			}
		}
		else if ((infix[k] == 'T' || infix[k] == 'F') && k != 0)
		{
			if (infix[k - 1] == 'T' || infix[k - 1] == 'F' || infix[k - 1] == ')')
			{
				return false;
			}
			if (infix[k + 1] == 'T' || infix[k + 1] == 'F' || infix[k + 1] == '(' || infix[k + 1] == '!')
			{
				return false;
			}
		}
		else if ((k != 0 && infix[k] == '('))
		{
			if (infix[k - 1] == 'T' || infix[k - 1] == 'F' || infix[k - 1] == ')')
			{
				return false;
			}
			if (infix[k + 1] == '&' || infix[k + 1] == ')' || infix[k + 1] == '|')
			{
				return false;
			}
		}
		else if ((infix[k] == ')') && k != 0)
		{
			if (infix[k - 1] == '&' || infix[k - 1] == '(' || infix[k - 1] == '!' ||  infix[k - 1] == '|')
			{
				return false;
			}
			if (infix[k + 1] == 'T' || infix[k + 1] == 'F' || infix[k + 1] == '!' || infix[k + 1] == '(')
			{
				return false;
			}
		}
		else if (k == lastCharacter)
		{
			if (infix[k] == ')' && (infix[k - 1] == '|' || infix[k - 1] == '&' || infix[k - 1] == '(' || infix[k - 1] == '!'))
			{
				return false;
			}
			if ((infix[k] == 'T' || infix[k] == 'F') && (infix[k - 1] == 'T' || infix[k - 1] == 'F' || infix[k - 1] == ')' || infix[k - 1] == '('))
			{
				return false;
			}
		}
		else
		{
		}
	}
	return true;
}
