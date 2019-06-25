/*! \file */

#ifndef __gdtarray__
#define __gdtarray__

//Qualche header fa undefine di NULL
//Probabilmente quando sara' finito il distacco non servira'
#ifndef NULL 
#define NULL 0
#endif
//

#include <assert.h>

namespace gdt {

#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)<(b) ? (a) : (b))

template<class E>
class gdtarray {

private:

	E* table;

	int _low;
	int _size;

	void alloc_table(E*& table, int size) {
		table = new E[size];
		assert(table);
	}

	void dealloc_table(E*& table, int size) {
		assert(table != NULL);
		delete [] table;
		table = NULL;
	}	

	void copy(const gdtarray<E>& a) {
		init(a.low(), a.high());		
		for(int i = low(); i <= high(); ++i) {
			table[i - low()] = a[i];
		}
	}

public:

	typedef void* item;

	/****************
	 * Constructors * 
	 ****************/

	gdtarray(int l, int h) : table(NULL) { init(l,h); }
	gdtarray(int n) : table(NULL) {	init(0, n-1); }
	gdtarray() : table(NULL) { }

	gdtarray(const gdtarray<E>& a) : table(NULL) { copy(a); }

	~gdtarray()  { if (table) dealloc_table(table, _size);	}

	gdtarray<E>& operator = (const gdtarray<E>& a) {
		if (table) dealloc_table(table, _size);
		copy(a);
		return *this;
	}

	void init(int l, int h) {
		assert(table == NULL);
		_low = l;
		_size = h - l + 1;
		alloc_table(table, _size);
	}

	const E& operator[](int x) const { 
		assert(x >= low() && x <= high());
		return table[x - low()];
	}

	E& operator[](int x) {
		assert(x >= low() && x <= high());
		return table[x - low()];
	}

	void resize(int a, int b) {		
		E* table2;
		alloc_table(table2, b - a + 1);		
		if (table) {			
			int l1 = MAX(low(), a);
			int h1 = MIN(high(), b);		
			for (int i = l1; i <= h1; ++i) {
				table2[i-a] = table[i-_low];
			}	
			dealloc_table(table, size());
		}
		_low = a;
		_size = b - a + 1;
		table = table2;
	}	

	void resize(int n) { 
		resize(0, n - 1);		
	}

	int low()  const { return _low; }
	int high() const { return _low + _size - 1; }
	int size() const { return _size; }	

	void swap(int i, int j) {
		assert(false);
		E x = table[i-low];
		table[i-low] = table[j-low];
		table[j-low] = x;
	}	

	//*****************
	//Iteration methods
	//*****************

	const E& inf(item it) const {
		return *(E*)it; 
	}
};

}

#endif
