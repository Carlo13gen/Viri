/*! \file */

#ifndef __gdtlist__
#define __gdtlist__

#include <assert.h>
#include <iostream>


#define UNKNOWN_COUNTER -1
#define __DLINKP (dlink*)
#define AFTER 0
#define BEFORE 1


typedef void* list_item;


namespace gdt {

template<class T>
void Swap(T& x, T& y) {
	T aux = x;
	x = y;
	y = aux;
}

enum { after = AFTER, before = BEFORE };

typedef ::list_item list_item;

template <class F>
		class Record {
		public:
			Record<F> *succ;
			Record<F> *pred;
			F inf;
		};



template <class E>
class gdtlist {

	public:

		typedef void* item;

	private:

		//Abbreviazione
		typedef Record<E> Item;

		//Pointer to the first Item
		Item *firstitem;

		//Pointer to the last Item
		Item *lastitem;

		//Number of elements
		int counter;

		//Buffer per l'ultimo elemento cancellato
		//Se non punta a nulla e' sempre a NULL
		Item *buffer;

		//Ripulisce il buffer
		void clear_buffer();

		//Assegna un record al buffer
		void assign_buffer(Item *i) {
			if (buffer) clear_buffer();
			buffer = i;
		}

		//Conta gli elementi della lista
		//o restituisce counter se valido
		int count() const;

		//Incrementa il contatore degli elementi se valido
		void inc_counter();

		//Decrementa il contatore se valido
		void dec_counter();

	
		//Copia una gdtlist
		void copy(const gdtlist<E>& l);

	public:

		/******************************
		 *                            *
		 * Constructors & destructors *
		 *                            *
		 ******************************/

		gdtlist();

		//Copy constructor
		gdtlist(const gdtlist<E>& l);

		//Destructor
		~gdtlist();

		gdtlist<E>& operator = (const gdtlist<E>& l);

		bool operator == (const gdtlist<E>& l2) const;


		/*********************
		 *                   *
		 * Iteration methods *
		 *                   *
         	 *********************/

		inline Item* first_item() const;
		inline Item* last_item() const;
		inline Item* next_item(gdt::list_item p) const;
		inline Item* pred_item(gdt::list_item p) const;


		/******************
		 *                *
		 * Access methods *
		 *                *
         	 ******************/

		int length() const;

		int size() const;

		bool empty() const;

		gdt::list_item first() const;

		gdt::list_item last() const;

		/*
		returns the Item at position i (the first position is 0).
        	precondition: 0<=i<length()
		*/
		gdt::list_item get_item(int i) const;

		inline gdt::list_item succ(gdt::list_item li) const;

		inline gdt::list_item pred(gdt::list_item li) const;

		inline gdt::list_item cyclic_succ(gdt::list_item li) const;

		inline gdt::list_item cyclic_pred(gdt::list_item li) const;

		inline const E& contents(gdt::list_item li) const;

		inline const E& inf(gdt::list_item it) const;

		inline const E& front() const;

		inline const E& head() const;

		inline const E& back() const;

		inline const E& tail() const;

		/*
		Returns the rank of x in L, i.e. its first
	    	position in L as an integer from [1...|L|]
	    	(0 if $x$ is not in $L$).
		*/
		int rank(const E& x) const;

		/******************
		 *                *
		 * Update methods *
		 *                *
		 ******************/

		inline gdt::list_item push(const E& x);

		inline gdt::list_item push_front(const E& x);

		inline gdt::list_item append(const E& x);

		inline gdt::list_item push_back(const E& x);

		gdt::list_item insert(const E& x, gdt::list_item _pos, int dir = AFTER);
		const E& operator[] (list_item it) const;

		E& operator[] (list_item it);

		//Scambia fra loro di posizione due record.
		//In questo modo gli item rimangono validi e puntano a gli stessi elementi
		void swap_items(gdt::list_item it1, gdt::list_item it2);

		inline const E& pop();

		inline const E& pop_front();

		inline const E&  Pop();

		inline const E& pop_back();

		const E&  del_item(gdt::list_item it);

		const E&  del(gdt::list_item it);

		void  erase(gdt::list_item it);

		void remove(const E& x);

		void assign(gdt::list_item li, const E& x);

		void conc(gdtlist<E>& L1, int dir = AFTER);

		void split(gdt::list_item _it, gdtlist<E>& L1, gdtlist<E>& L2);
		
		void reverse_items();

		void reverse();

		/*********************************
		 *                               *
		 * Sorting and searching methods *
		 *                               *
		 *********************************/

		list_item search(const E& x) const;

		/*******************
		 *                 *
		 * Various methods *
		 *                 *
		 *******************/


		void print(std::ostream& os, char space = ' ') const;

		void print(char space = ' ') const;

		void clear();

		bool check_item(gdt::list_item lit) const ;

		bool consistency_check() const;
};

//Ripulisce il buffer
template <class E>
void gdtlist<E>::clear_buffer() {
	//assert(buffer);
	Item *i = buffer;
	delete i;
}

//Conta gli elementi della lista
//o restituisce counter se valido
template <class E>
int gdtlist<E>::count() const {
	if (counter == UNKNOWN_COUNTER) {
		list_item li = first();
		int &c = const_cast< gdtlist<E>* >(this)->counter;
		c = 0;
		while (li) { ++c; li = succ(li); }
	} 
	return counter;
}

//Incrementa il contatore degli elementi se valido
template <class E>
void gdtlist<E>::inc_counter() {
	if (counter != UNKNOWN_COUNTER) ++counter;			
}

//Decrementa il contatore se valido
template <class E>
void gdtlist<E>::dec_counter() {
	if (counter != UNKNOWN_COUNTER) --counter;
}


//Copia una gdtlist
template <class E>
void gdtlist<E>::copy(const gdtlist<E>& l) {
	clear();
	list_item it = l.first();
	while (it) { append(l.inf(it));it = l.succ(it); }
	//assert(operator == (l));
}

/******************************
 *                            *
 * Constructors & destructors *
 *                            *
 ******************************/

template <class E>
gdtlist<E>::gdtlist() : firstitem(NULL), lastitem(NULL), counter(0), buffer(NULL) {}
		

//Copy constructor
template <class E>
gdtlist<E>::gdtlist(const gdtlist<E>& l) : firstitem(NULL), lastitem(NULL), counter(0), buffer(NULL) {
	//message("copy constructor from gdtlist");
	copy(l);
	//assert(consistency_check());
}



//Destructor
template <class E>
gdtlist<E>::~gdtlist() {
	//message("destructor");
	clear();
	if (buffer) clear_buffer();
}


template <class E>
gdtlist<E>& gdtlist<E>::operator = (const gdtlist<E>& l) {
	copy(l);
	//assert(consistency_check());
	return *this;
}

template <class E>
bool gdtlist<E>::operator == (const gdtlist<E>& l2) const {
	list_item i1 = first();
	list_item i2 = l2.first();

	while (i1 && i2 && inf(i1) == l2.inf(i2)) {
		i1 = succ(i1);
		i2 = l2.succ(i2);
	}

	//assert(consistency_check());
	return (i1 == i2);
}


/*********************
 *                   *
 * Iteration methods *
 *                   *
 *********************/

template <class E>
typename gdtlist<E>::Item* gdtlist<E>::first_item() const { return firstitem; }
template <class E>
typename gdtlist<E>::Item* gdtlist<E>::last_item() const { return lastitem; }
template <class E>
typename gdtlist<E>::Item* gdtlist<E>::next_item(gdt::list_item p) const { return (Item*) (p != NULL ? succ(p) : NULL); }
template <class E>
typename gdtlist<E>::Item* gdtlist<E>::pred_item(gdt::list_item p) const { return (Item*) (p != NULL ? pred(p) : NULL); }

/******************
 *                *
 * Access methods *
 *                * 
 ******************/

template <class E>
int gdtlist<E>::length() const { 
	//message("length");
	count(); 
	//assert(consistency_check()); 
	return counter;
}

template <class E>
int gdtlist<E>::size() const { return length(); }

template <class E>
bool gdtlist<E>::empty() const { return firstitem == NULL; }

template <class E>
gdt::list_item gdtlist<E>::first() const { 
	return firstitem;
}			

template <class E>
gdt::list_item gdtlist<E>::last() const { return lastitem; }

template <class E>
gdt::list_item gdtlist<E>::get_item(int i) const {
	//message("get_item");
	//assert(i < length());
	gdt::list_item li = first();
	int j = 0;
	while (j < i) {li = succ(li); ++j;}
			
	//assert(consistency_check());

	return li;
}

template <class E>
gdt::list_item gdtlist<E>::succ(gdt::list_item li) const {			
	//assert(check_item(li));
	Item *i = static_cast<Item *>((void *) li);
	return i->succ;
}
		
template <class E>
gdt::list_item gdtlist<E>::pred(gdt::list_item li) const {			
	//assert(check_item(li));
	Item *i = static_cast<Item *>((void *) li);
	return i->pred;			
}

template <class E>
gdt::list_item gdtlist<E>::cyclic_succ(gdt::list_item li) const {
	//assert(check_item(li));
	return li != last() ? succ(li) : first();			
}

template <class E>
gdt::list_item gdtlist<E>::cyclic_pred(gdt::list_item li) const {
	//assert(check_item(li));
	return li != first() ? pred(li) : last();			
}

template <class E>
const E& gdtlist<E>::contents(gdt::list_item li) const {
	//message("inf");
	//assert(li != NULL);
	//assert(check_item(li));
	Item *i = static_cast<Item *>((void*)li);
	//assert(consistency_check());
	return i->inf;
}

template <class E>
const E& gdtlist<E>::inf(gdt::list_item it) const { 
	//assert(check_item(it));
	return contents(it); 
}

template <class E>
const E& gdtlist<E>::front() const {
	//assert(first() != NULL);
	return contents(first());
}

template <class E>
const E& gdtlist<E>::head() const { return front(); }		

template <class E>
const E& gdtlist<E>::back() const { 
	//assert(first() != NULL);
	return contents(last());
}

template <class E>
const E& gdtlist<E>::tail() const { return back(); }

/* 
Returns the rank of x in L, i.e. its first
position in L as an integer from [1...|L|]
(0 if $x$ is not in $L$).
*/
template <class E>
int gdtlist<E>::rank(const E& x) const   
{
	//message("rank");
	int i = 1;
	gdt::list_item li = first();
	while(li != NULL && contents(li) != x) {li = succ(li); ++i;};

	//assert(consistency_check());

	return li != NULL ? i : 0;			
}
		
/******************
 *                *
 * Update methods *
 *                *
 ******************/

template <class E>
gdt::list_item gdtlist<E>::push(const E& x)   { return insert(x, first(), gdt::before); }

template <class E>
gdt::list_item gdtlist<E>::push_front(const E& x) { return push(x); }

template <class E>
gdt::list_item gdtlist<E>::append(const E& x) { return insert(x, last(), gdt::after); }

template <class E>
gdt::list_item gdtlist<E>::push_back(const E& x)  { return append(x); }

template <class E>
gdt::list_item gdtlist<E>::insert(const E& x, gdt::list_item _pos, int dir) {

	//message("insert");
	//assert((!empty() && check_item(_pos)) || (empty() && _pos == NULL));
	//assert(dir == gdt::after || dir == gdt::before);

	Item* newitem = new Item;
	Item* pos = static_cast<Item*>((void*)_pos);
	newitem->inf = x;
	if (empty()) {
		firstitem = newitem;
		lastitem = newitem;
		newitem->succ = NULL;
		newitem->pred = NULL;
	}
	else {
		//assert(pos != NULL);
		if (dir == gdt::after) {
			//AFTER
			newitem->succ = pos->succ;
			if (pos->succ)
				pos->succ->pred = newitem;
			else
				lastitem = newitem; //E' l'ultimo
			newitem->pred = pos;
			pos->succ = newitem;
		}
		else {
			//BEFORE
			newitem->pred = pos->pred;
			if (pos->pred) pos->pred->succ = newitem;
			else firstitem = newitem;
			newitem->succ = pos;
			pos->pred = newitem;
		}
	}
	inc_counter();

	//assert(consistency_check());
	return newitem;
}

template <class E>
const E& gdtlist<E>::operator[] (list_item it) const {
	return contents(it);
}

template <class E>
E& gdtlist<E>::operator[] (list_item it) {			
	//assert(it && check_item(it));
	Item* i = (Item *) it;
	return i->inf;
}

		//Scambia fra loro di posizione due record.
		//In questo modo gli item rimangono validi e puntano a gli stessi elementi
template <class E>
void gdtlist<E>::swap_items(gdt::list_item it1, gdt::list_item it2) {
	//assert(it1 && it2 && it1!=it2 && check_item(it1) && check_item(it2));
			
	Item* i1 = (Item*) it1;
	Item* i2 = (Item*) it2;
	Item* p1 = i1->pred;
	Item* s1 = i1->succ;
	Item* p2 = i2->pred;
	Item* s2 = i2->succ;

	if (p1 == NULL) firstitem = i2;
	if (p2 == NULL) firstitem = i1;
	if (s1 == NULL) lastitem = i2;
	if (s2 == NULL) lastitem = i1;

	if (s1 == i2) { //Adiacenti
		//std::cout << "Adiacenti i1->i2" << endl;
		//assert(p2 == i1);				
		i2->succ = i1;
		i1->pred = i2;

		i1->succ = s2;
		i2->pred = p1;	
				
		ass_succ(p1, i2);
		ass_pred(s2, i1);
	}
	else if (s2 == i1) {
		//std::cout << "Adiacenti i2->i1" << endl;
		//assert(p1 == i2);
		i1->succ = i2;
		i2->pred = i1;
				
		i2->succ = s1;
		i1->pred = p2;

		ass_succ(p2, i1);
		ass_pred(s1, i2);
	}
	else { //Non adiacenti
		//std::cout << "Non adiacenti" << endl;
		//assert(p2 != i1);
		i2->succ = s1;							
		i1->pred = p2;				
		ass_pred(s1, i2);
		ass_succ(p2, i1);
				
		i1->succ = s2;
		i2->pred = p1;				

		ass_succ(p1, i2);
		ass_pred(s2, i1);
	}	
			
	//assert(consistency_check());
}

template <class E>
const E& gdtlist<E>::pop() { return del_item(first()); } 

template <class E>
const E& gdtlist<E>::pop_front() { return pop(); }

template <class E>
const E&  gdtlist<E>::Pop() { return del_item(last()); }			

template <class E>
const E& gdtlist<E>::pop_back()  { return Pop(); }

template <class E>
const E&  gdtlist<E>::del_item(gdt::list_item it) {			
	//assert(!empty() && it != NULL && check_item(it));

	if (buffer) clear_buffer();
	buffer = static_cast<Item*>((void*)it);
	if (buffer->pred) buffer->pred->succ = buffer->succ;
	else firstitem = buffer->succ;
	if (buffer->succ) buffer->succ->pred = buffer->pred;
	else lastitem = buffer->pred;

	dec_counter();

	//assert(consistency_check());
	return buffer->inf;
}

template <class E>
const E&  gdtlist<E>::del(gdt::list_item it) { return del_item(it); }

template <class E>
void  gdtlist<E>::erase(gdt::list_item it) { del_item(it); }

template <class E>
void gdtlist<E>::remove(const E& x)	{ 
	//message("remove");
	Item *it = firstitem;
	while (it) {
		Item *itsucc = it->succ;
		if (inf(it) == x) del_item(it);
		it = itsucc;
	}

	//assert(consistency_check());
}

template <class E>
void gdtlist<E>::assign(gdt::list_item li, const E& x) {
	//assert(check_item(li));
	//message("assign");
	Item *i = static_cast<Item*>(li);
	i->inf = x;

	//assert(consistency_check());
}

template <class E>
void gdtlist<E>::conc(gdtlist<E>& L1, int dir) {
	//message("conc");
	//assert(&L1 != this);
	//assert(dir == gdt::after || dir == gdt::before);

	if (dir == gdt::after) {
		if (lastitem) lastitem->succ = L1.firstitem;
		else firstitem = L1.firstitem;

		if (L1.firstitem) L1.firstitem->pred = lastitem;

		if (L1.lastitem) lastitem = L1.lastitem;
		//else this->lastitem doesn't change
	}
	else { //dir == gdt::before
		if (firstitem) firstitem->pred = L1.lastitem;
		else lastitem = L1.lastitem;

		if (L1.lastitem) L1.lastitem->succ = firstitem;

		if (L1.firstitem) firstitem = L1.firstitem;
		//else firstitem doesn't change
	}

	if (counter != UNKNOWN_COUNTER && L1.counter != UNKNOWN_COUNTER)
					counter += L1.counter;
		else counter = UNKNOWN_COUNTER;

	//L1 is made empty
	L1.firstitem = NULL;
	L1.lastitem = NULL;
	L1.counter = 0;

	//assert(consistency_check());
}


template <class E>
void gdtlist<E>::split(gdt::list_item _it, gdtlist<E>& L1, gdtlist<E>& L2) {
	//assert(check_item(_it));
	//message("split");
	//assert(!this->empty());
			
	Item* it = (Item*) _it;
	Item* temp_firstitem = firstitem;
	Item* temp_lastitem = lastitem;
	int temp_counter = counter;
			
	if (it) {		
		//L1
		L1.lastitem = it->pred;
		L1.firstitem = temp_firstitem == it ? NULL : temp_firstitem;
		L1.counter = UNKNOWN_COUNTER;

		//The predecessor of it is the tail of L1
		if (it->pred) it->pred->succ = NULL;
				
		//L2
		it->pred = NULL;
		L2.firstitem = it;
		L2.lastitem = temp_lastitem;
		L2.counter = UNKNOWN_COUNTER;				
	}
	else {
		//L1 made empty
		L1.firstitem = NULL;
		L1.lastitem = NULL;
		L1.counter = 0;

		//L2 copy of *this
		L2.firstitem = temp_firstitem;
		L2.lastitem = temp_lastitem;
		L2.counter = temp_counter;
	}

	//L is made empty if is not L1 or L2
	if (&L1 != this && &L2 != this) {
		firstitem = NULL;
		lastitem = NULL;
		counter = 0;
		//assert(L1.length() + L2.length() + this->length() == temp_counter);
	}
	else {	
		//assert(L1.length() + L2.length() == temp_counter);				
	}

	//assert(this->consistency_check());
	//assert(L1.consistency_check());
	//assert(L2.consistency_check());
}

//splits L at item it into lists L1 and L2. 
//More precisely, if it! = nil and L = x1,..., xk - 1, it, xk + 1,..., xn 
//then L1 = x1,..., xk - 1 and L2 = it, xk + 1,..., xn. 
//If it = nil then L1 is made empty and L2 a copy of L. 
//Finally L is made empty if it is not identical to L1 or L2. 
//Precondition it is an item of L or nil.

template <class E>
void gdtlist<E>::reverse_items() {
	//message("reverse_items");
	Swap(firstitem, lastitem);
	Item *it = lastitem;
	while (it) {								
		Swap(it->succ, it->pred);
		it = it->pred;
	}
	//assert(consistency_check());
}

template <class E>
void gdtlist<E>::reverse() { reverse_items(); }

	/*********************************
	*                               *
	* Sorting and searching methods *
	*                               *
	*********************************/

template <class E>
list_item gdtlist<E>::search(const E& x) const {
	//message("search");
	list_item li = first();
	while (li && inf(li) != x) {li = succ(li);}
	return li;
}

	/*******************
	*                 *
	* Various methods *
	*                 *
	*******************/

template <class E>
void gdtlist<E>::print(std::ostream& os, char space) const {
	list_item li;
	os << "{";
	if (!empty()) {
		li = first();
		while (li != last()) {
			os << inf(li) << ",";
			li = succ(li);
		}
		os << back();
	}
	os << "}";
}

template <class E>
void gdtlist<E>::print(char space) const {
	print(std::cout, space);
}



template <class E>
void gdtlist<E>::clear() { while (!empty()) pop(); }

template <class E>
bool gdtlist<E>::check_item(gdt::list_item lit) const {
	Item *it = firstitem;
	while (it && lit != it) it = it->succ;
	return it == lit;
}

template <class E>
bool gdtlist<E>::consistency_check() const {
	Item *it = firstitem;		
	Item *predit = NULL;
	int number_of_items = 0;

	while (it) {
		++number_of_items;
		if (predit != it->pred) 
			return false;
		predit = it;
		it = it->succ;
	}
			
	if (predit != lastitem) 
		return false;
	if (counter != UNKNOWN_COUNTER && number_of_items != counter)
		return false;

	return true;
}

};


template <class OS, class E>
OS& operator << (OS& os, const gdt::gdtlist<E>& l) {
	gdt::list_item li;
	os << "{";
	if (!l.empty()) {
		li = l.first();
		while (li != l.last()) {
			os << l.inf(li) << ",";
			li = l.succ(li);			
		}
		os << l.back();
	}
	os << "}";
	return os;
}

//Dummy functions
template <class IS, class E>
IS& operator >> (IS& is, gdt::gdtlist<E>& l) {
	//assert(false);
	std::cout << "Operatore << non implementato" << std::endl;
	return is;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//   MACRO
///////////////////////////////////////////////////////////////////////////////////////////////////////

/* Be careful!!*
 The following macros are based on the line number for the creation of the macro-local variables.
 If two or more forall are put on the same line, problems may happen with non-standard compilers.
 Notably, for Visual C++ the scope of the variables declared into the FOR statement goes beyond the body of the FOR.
 To avoid any problem, put only ONE forall for each line.

*/                             //((S).size()!=0)?(x=(S).inf((S).first()),0):0



/* this is an experiment

#define LOOP_INIT(x,S,line) gdt::list_item _VAR(line)= (void*)( (((S).size()!=0)?(x=(S).inf((S).first()),0):0) ,   (S).first() )

#define LOOP_INC(x,S,line) _VAR(line)=(void*)(S).succ(_VAR(line)),((_VAR(line)!=0) ? (x=(S).inf(_VAR(line)),0):0)


#define forall(x,S) for( LOOP_INIT(x,S,__LINE__); _VAR(__LINE__); LOOP_INC(x,S,__LINE__) )
*/




/* This works under LINUX but not under Windows

#define _CONCAT(x,y) x ## y
#define _VAR(line) _CONCAT(loop_private_,line)
  
#define forall(x,S) for(gdt::list_item _VAR(__LINE__)= (void*)( (((S).size()!=0)?(x=(S).inf((S).first()),0):0) ,   (S).first() )  ;\
		 _VAR(__LINE__) ;\
 		 _VAR(__LINE__)=(void*)(S).succ(_VAR(__LINE__)),((_VAR(__LINE__)!=0) ? (x=(S).inf(_VAR(__LINE__)),0):0) )
*/


/*
Questo codice sembra più ordinato del precedente, ma ancora non compila  

#define forall(x,S) for( \
	    gdt::list_item _VAR(__LINE__)= (S).first();\
        ( (_VAR(__LINE__)!=0) ? (x=(S).inf(_VAR(__LINE__))):0),  _VAR(__LINE__) ;\
 		 _VAR(__LINE__)=(void*)(S).succ(_VAR(__LINE__)) )
*/



#define forall_items(it,S) \
		for(it=(S).first(); it; it=(S).succ(it))





#if (defined(__GNUC__) && __GNUC_MINOR__ <= 7) || defined(__ELSE_SCOPE_BUG__)
#define __FORALL_PREAMBLE
#define LOOP_CAT(x,y) x ## y 
#define LOOP_VAR(y)  LOOP_CAT(loop_var_,y)
#define LOOP_VAR1(y) LOOP_CAT(loop_var1_,y)
#else
#define __FORALL_PREAMBLE  if (0); else
#define LOOP_VAR(y)  loop_var
#define LOOP_VAR1(y) loop_var1
#endif


#define LOOP_ITEM(p) (typename T::item)p

#if defined(__NO_CAST_TO_LOCAL_TYPE__)
template <class T>
inline T cast_to_item(T,void* p) { return (T)p; }
#undef  LOOP_ITEM
#define LOOP_ITEM(p) cast_to_item(S.first_item(),p)
#endif


template<class T>
inline bool LoopSucc(const T& S, void*& p) 
{ if (p) { p = S.next_item(LOOP_ITEM(p)); return true; }
  else return false;
 }

template<class T>
inline bool LoopPred(const T& S, void*& p)
{ if (p) { p = S.pred_item(LOOP_ITEM(p)); return true; }
  else return false;
 }

template<class T, class var_type>
inline void LoopInf(const T& S, var_type& x, void* p) 
{ if (p) x = S.inf(LOOP_ITEM(p)); }

template<class T, class var_type>
inline void LoopKey(const T& S, var_type& x, void* p) 
{ if (p) x = S.key(LOOP_ITEM(p)); }


#define forall(x,S)\
__FORALL_PREAMBLE \
for(void* LOOP_VAR(__LINE__) = (S).first_item();\
LoopInf(S,x,LOOP_VAR(__LINE__)), LoopSucc(S,LOOP_VAR(__LINE__)); )


#endif


