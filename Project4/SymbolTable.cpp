#include "SymbolTable.h"
#include <string>
#include <iostream>
#include <cassert>
#include <vector>
using namespace std;

const int SIZE = 20000;

struct hashNode //create node for each bucket of hash table
{
	int lineNum;
	int scope;
	hashNode(int newLine, int newScope) 
	{ 
		lineNum = newLine; 
		scope = newScope;
	}
};


struct Node
{
	string id;
	vector<hashNode> Scoping; //vector of hash nodes used for scopes
	Node* left;
	Node* right;
	Node** parent;
	Node(const string& stringid, int line, int scope) //linked list in binary tree format
	{
		Scoping.push_back(hashNode(line, scope));
		id = stringid;
		left = nullptr;
		right = nullptr;
		parent = nullptr;
	}
	~Node()
	{
		delete left;
		delete right;
	}
};

class hashTable
{
public:
	hashTable() //constructor
	{
		for (int i = 0; i < SIZE; ++i)
		{
			bucket[i] = nullptr;
		}
	}
	~hashTable() //destructor
	{
		for (int i = 0; i < SIZE; ++i)
		{
			delete bucket[i];
		}
	}

	Node* declare(const string& id, const int& lineNum, int scopeNum);
	int search(const string& id) const;
private:
	Node* bucket[SIZE]; //create hash table of SIZE
};

// This class does the real work of the implementation.
class SymbolTableImpl
{
public:
	SymbolTableImpl() { idV.push_back(vector<Node*>()); }

	void enterScope();
	bool exitScope();
	bool declare(const string& id, int lineNum);
	int find(const string& id) const;
private:
	vector<vector<Node*> > idV;
	hashTable tableHash;
};

// function implementation
///////////////////////////////////////////////////////////

int hashFunc(const string& key) //string to key hash function
{
	unsigned long long value = 0;
	for (int i = 0; i < key.size(); ++i)
	{
		value = value * 101 + key[i] + 1;
	}
	value =  value % SIZE;
	return value;
}

 Node* hashTable::declare(const string& id, const int& lineNum, int num)
{
	 int key = hashFunc(id);
	Node** ptr = &bucket[key];  
	while (*ptr != nullptr)
	{
		if ((*ptr)->id == id)  
		{
			if ((*ptr)->Scoping.empty() || (*ptr)->Scoping.back().scope != num)
			{
				(*ptr)->Scoping.push_back(hashNode(lineNum, num)); //push node fir the last value of vector
				return *ptr;
			}
			else
			{
				return nullptr;
			}
		}
		ptr = (id < (*ptr)->id) ? &((*ptr)->left) : &((*ptr)->right);
	}
	*ptr = new Node(id, lineNum, num);
	(*ptr)->parent = ptr;
	return *ptr;
}

int hashTable::search(const string& id) const
{
	int key = hashFunc(id);
	Node* ptr = bucket[key];
	while (ptr != nullptr)
	{
		if (ptr->id == id)
		{
			return (ptr->Scoping.empty()) ? -1 : ptr->Scoping.back().lineNum;
		}
		ptr = (id < ptr->id) ? ptr->left : ptr->right; //decide which direction of linked list to go
	}
	return -1;
}

 void SymbolTableImpl::enterScope()
{
	idV.push_back(vector<Node*>()); //when enterin scope, push vector value with vector of nodes
}

bool SymbolTableImpl::exitScope()
{ //binary tree
	if (idV.size() <= 1)
	{
		return false;
	}
		vector<Node*>& current = idV.back(); //set current node to back of vector
		int len = current.size();
		for (int i = 0; i < len; ++i)
		{
			current[i]->Scoping.pop_back();
			if (current[i]->Scoping.empty()) //if empty
			{
				Node* currNode = current[i]->left;
				if (currNode == nullptr)
				{
					currNode = current[i];
					if (currNode->right != nullptr)
					{
						currNode->right->parent = currNode->parent;
					}
					*(current[i]->parent) = current[i]->right;
				}
				else
				{
					while (currNode->right != nullptr)
					{
						currNode = currNode->right;
					}
					if (currNode->left != nullptr)
					{
						currNode->left->parent = currNode->parent;
					}
					*(currNode->parent) = currNode->left; 
					*(current[i]->parent) = currNode;
					currNode->left = current[i]->left; currNode->right = current[i]->right;
					if (current[i]->left != nullptr)
					{
						current[i]->left->parent = &(currNode->left);
					}
					if (current[i]->right != nullptr)
					{
						current[i]->right->parent = &(currNode->right);
					}
				}
				current[i]->left = current[i]->right = nullptr;
				*(current[i]->parent) = nullptr;
				delete current[i];
			}
		}
		idV.pop_back();
		return true;
	return false;
}

bool SymbolTableImpl::declare(const string& id, int lineNum)
{
	//use hashTable functions 
	Node* final = tableHash.declare(id, lineNum, idV.size());
	if (final == nullptr) //cannot be point to nullptr
	{
		return false;
	}
	else 
	{
		idV.back().push_back(final);
		return true;
	}
	return false;
}
int SymbolTableImpl::find(const string& id) const
{
	//search function for hash table
	int find =  tableHash.search(id);
	return find;
}
//*********** SymbolTable functions **************

// For the most part, these functions simply delegate to SymbolTableImpl's
// functions.

SymbolTable::SymbolTable()
{
	m_impl = new SymbolTableImpl;
}

SymbolTable::~SymbolTable()
{
	delete m_impl;
}

void SymbolTable::enterScope()
{
	m_impl->enterScope();
}

bool SymbolTable::exitScope()
{
	return m_impl->exitScope();
}

bool SymbolTable::declare(const string& id, int lineNum)
{
	return m_impl->declare(id, lineNum);
}

int SymbolTable::find(const string& id) const
{
	return m_impl->find(id);
}