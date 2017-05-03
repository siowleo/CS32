#include "Set.h"
#include <iostream>

Set::Set()
{
	m_size = 0;
	head = nullptr;
	tail = head; //circular linked list
}

Set::~Set()
{
	Node* p = head; 

	while (p != nullptr)
	{
		Node* j = p->next; //moves pointer to next node before the current one gets deleted, prevents dangling pointers
		delete p; 
		p = j;
	}

}


Set::Set(const Set& other)
{
	m_size = other.m_size;
	head = other.head;
	tail = other.tail;

	//Node* p = head;
	//Node* j = other.head;

	for (Node* p = head; p != nullptr; p = p->next)
	{
		for (Node* j = head; j != nullptr; j = j->next)
		{
			p->value = j->value; //copies values from the other list into our current list
		}
	}
}

Set& Set::operator=(const Set& rhs)
{
	//if (&rhs == this)
	if(this == &rhs)
	{
		return *this;
	}

	Set n = rhs; 
	swap(n); //swap values
	return *this;
}

bool Set::insert(const ItemType& value)
{
	if (empty()) //checks for empty linked list
	{
		Node* p = new Node;

		p->next = head;
		p->value = value;
		p->prev = nullptr; //signals the end of the linkedlist, makes the whole linked list circular
		head = p;
		tail = head; //makes this linked list circular
		m_size++; //increase size of linked list 
		return true;
	}

	else
	{
		Node* p = head;
		while (p->next != nullptr) //traverse the node until it reaches the current tail
		{
			if (p->value == value) //return false if value already exists inside the linked list
			{
				return false;
			}
			p = p->next;
		}
		Node* j = new Node; //double linked list
		j->value = value;
		j->next = nullptr;
		tail = j;
		tail->prev = p;
		p->next = tail;
		m_size++;



		return true;
	}
}

bool Set::erase(const ItemType& value)
{
	if (empty()) //checks for empty list
	{
		return false;
	}
	else
	{
		Node* p = head;
		while (p != nullptr)
		{
			if (head->value == value) //removal from the front
			{
				Node * j = head;
				head = j->next; //sets pointer to next node
				delete j;
				m_size--; //decrement after removal
				return true;
			}
			if (p->next->value == value && p->next != nullptr) //removal from mid and rear
			{
				break;
			}
			p = p->next;
		}
		Node* j = p->next;
		p->next = j->next;
		if (j->next != nullptr)
		{
			j->next->prev = p; //circular linked list
		}
		delete j;
		m_size--;
		return true;
	}


	return false;
}

bool Set::contains(const ItemType& value) const
{
	Node * p = head;
	while( p != nullptr)
	{
		if (p->value == value) //checks to see if value of node matches ItemType value
			return true;
		p = p->next;
	}
	return false;
}

bool Set::get(int i, ItemType& value) const
{

	if (i < 0 || i >= m_size)
		return false;

	int j = 0;

	for (Node* p = head; p != nullptr; p = p->next)
	{
		for (Node* k = head; k != nullptr; k = k->next)
		{
			if (p->value > k->value)
			{
				j++; //coutns for number of greater elements in list
			}
		}
		if (j == i)
		{
			value = p->value;
			return true;
		}
		j = 0; //reset counter
	}

	/*
	while (p != nullptr)
	{
		while (k != nullptr)
		{
			if (p->value > k->value)
			{
				j++;
			}
			k = k->next;
		}
		if (j == i)
		{
			value = p->value;
			return true;
		}
		j = 0;
		p = p->next;

	}
	*/
	return false;
}



void Set::swap(Set& other)
{
	//swap head nodes
	Node* p = other.head;
	other.head = head;
	head = p;

	//swap tail nodes
	Node* j = other.tail;
	other.tail = tail;
	tail = j;

	// Swap sizes
	int tempSize = m_size;
	m_size = other.m_size;
	other.m_size = tempSize;
}


void unite(const Set& s1, const Set& s2, Set& result)
{
	int totalSize = s1.size() + s2.size(); //combine sizes
	ItemType x, y;
	Set j;

	for (int k = 0; k < totalSize; k++)
	{
		if (k < s1.size()) //checks elements from 0 to size of s1
		{
			s1.get(k, x);
			j.insert(x);
		}

		else
		{
			s2.get(k - s1.size(), y); //checks elements starting from after s1 to end of s2
				if (!j.contains(y))//check for duplicates
				{
					j.insert(y);
				}
		}
	}
	result.swap(j);
}




void subtract(const Set& s1, const Set& s2, Set& result)
{
	unite(s1, s2, result);
	ItemType x;
	int size = result.size();
	for (int k = 0; k < result.size(); k++)
	{
		result.get(k, x);
		if (!s2.contains(x)) //if list doesn't contain x, keep looping
		{
			continue;
		}
		else
		{
			result.erase(x);
			size--;
			k--;
		}
	}
}



