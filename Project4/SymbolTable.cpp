#include "SymbolTable.h"
#include <string>
#include <iostream>
#include <cassert>
using namespace std;

const int SIZE = 100;

class hashing //creating linked list for hash table
{
public:
	hashing() //intialize the hash table
	{
		for (int i = 0; i < SIZE; ++i)
		{
			table[i] = nullptr;
		}
	}
	bool declare(string id, int lineNum);
	int search(string id);
private:
	struct hashNode //create node for each hash table node
	{
		string id;
		int lineNum;
		hashNode* next;
		hashNode(string newId, int newLine) //initialize the nodes
		{
			id = newId;
			lineNum = newLine;
			next = nullptr;
		}
	};

	hashNode* table[SIZE];
};

int hashFunc(const string& id) //hash function from string to key
{
	unsigned int value = 0;
	for (int i = 0; i < id.size(); i++)
	{
		value = value + (i + 1) * id[i];
	}
	value = value % SIZE;
	return value;
}

bool hashing::declare(string id, int lineNum) //checking hash table for key values
{
	int key = hashFunc(id);
	hashNode** ptr = &table[key];
	while (*ptr != nullptr)
	{
		if ((*ptr)->id == id)
		{
			return false;
		}
		else
		{
			ptr = &((*ptr)->next);
		}
	}
	*ptr = new hashNode(id, lineNum); //if bucket with value doesn't exist, make a new one
	return true;
}

int hashing::search(string id)
{
	int key = hashFunc(id);
	hashNode* ptr = table[key];
	while (ptr != nullptr)
	{
		if (ptr->id == id)
		{
			int num = ptr->lineNum;
			return num;
		}
		ptr = ptr->next;
	}
	return -1;
}


struct Node
{
	hashing table;
	Node* next;
	Node* previous;
	Node()
	{
		next = nullptr;
		previous = nullptr;
	}
	~Node()
	{
		delete next;
	}
};

// This class does the real work of the implementation.
class SymbolTableImpl //constructor for hash table 
{
public:
	SymbolTableImpl()
	{
		head = new Node;
		current = head;
	}
	void enterScope();
	bool exitScope();
	bool declare(const string& id, int lineNum);
	int find(const string& id) const;
private:

	Node* head;
	Node* current;
};

void SymbolTableImpl::enterScope() //traversing the linked list
{
	current->next = new Node;
	current->next->previous = current;
	current = current->next; //moving on to next node
}

bool SymbolTableImpl::exitScope()
{ //exit scope by going to previous node
	if (current == nullptr)
	{
		return false;
	}
	else if (current->previous == nullptr)
	{
		return false;
	}
	else
	{
		current = current->previous;
		delete current->next;
		current->next = nullptr;
		return true;
	}

	return false;
}

inline bool SymbolTableImpl::declare(const string& id, int lineNum)
{
	bool result; //declare using the hashnode function
	result = current->table.declare(id, lineNum);
	return result;
}

int SymbolTableImpl::find(const string& id) const
{
	if (current == nullptr)
	{
		return false;
	}
	Node* ptr;
	ptr = current;
	int num = ptr->table.search(id);
	while (ptr->previous != nullptr && num == -1)//search for node with existing id
	{
		ptr = ptr->previous; //traverse backwards
		num = ptr->table.search(id);

	}
	return num;
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
