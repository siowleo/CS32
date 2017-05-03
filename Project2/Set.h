#ifndef SET_INCLUDED
#define SET_INCLUDED
#include<string>
// Later in the course, we'll see that templates provide a much nicer
// way of enabling us to have Sets of different types.  For now,
// we'll use a typedef.

//typedef unsigned long ItemType;
typedef std::string ItemType;

const int DEFAULT_MAX_ITEMS = 200;

class Set
{
public:
	Set();
	Set::Set(const Set& other);
	Set& Set::operator=(const Set& rhs);
	~Set();



	bool empty() const;  // Return true if the set is empty, otherwise false.
	int size() const;    // Return the number of items in the set.

	bool insert(const ItemType& value);
	// Insert value into the set if it is not already present.  Return
	// true if the value was actually inserted.  Leave the set unchanged
	// and return false if the value was not inserted (perhaps because it
	// is already in the set or because the set has a fixed capacity and
	// is full).

	bool erase(const ItemType& value);
	// Remove the value from the set if present.  Return true if the
	// value was removed; otherwise, leave the set unchanged and
	// return false.

	bool contains(const ItemType& value) const;
	// Return true if the value is in the set, otherwise false.

	bool get(int i, ItemType& value) const;
	// If 0 <= i < size(), copy into value the item in the set that is
	// greater than exactly i items in the set and return true.  Otherwise,
	// leave value unchanged and return false.

	void swap(Set& other);
	// Exchange the contents of this set with the other one.

private:

	struct Node
	{
		ItemType value;
		Node* next;
		Node* prev;
	};
	Node* head;
	Node* tail;

	ItemType m_data[DEFAULT_MAX_ITEMS];  // the items in the set
	int      m_size;                     // number of items in the set

										 // At any time, the elements of m_data indexed from 0 to m_size-1
										 // are in use and are stored in increasing order.

	//int findFirstAtLeast(const ItemType& value) const;
	// Return the position of the smallest item in m_data that is >= value,
	// or m_size if there are no such items.
};


// Inline implementations

inline
int Set::size() const
{
	return m_size;
}

inline
bool Set::empty() const
{
	return size() == 0;
}

/*
inline
bool Set::contains(const ItemType& value) const
{
	int pos = findFirstAtLeast(value);
	return pos < m_size  &&  m_data[pos] == value;
}
*/


void unite(const Set& s1, const Set& s2, Set& result);

void subtract(const Set& s1, const Set& s2, Set& result);

#endif // SET_INCLUDED
