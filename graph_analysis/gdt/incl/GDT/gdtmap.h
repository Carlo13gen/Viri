/*! \file */

#ifndef __gdtmap__
#define __gdtmap__

#include <iostream>
#include <assert.h>
#include <math.h>

#include "ptr.h"

#define MIN_TABLE_SIZE 2
#define UPPER_THRESHOLD 1.28

//#ifdef WIN32
#define HASH_POINTER_SHIFT 4
//#endif


namespace gdt {

#ifdef __x86_64__
inline int Hash(void* p){return int((long long)(p)>> HASH_POINTER_SHIFT);};
#else

#ifdef	GDTWRAPPER_EXPORTS
inline int Hash(void* p){return int((long long)(p)>> HASH_POINTER_SHIFT);};
#else 
inline int Hash(void* p){return int((long)(p)>> HASH_POINTER_SHIFT);};

#endif
#endif

inline int Hash(int p){return p;};

int gdtceil(double f);

template<class Key, class Value>
class gdtmap {

	private:
		
		struct Rec {
			bool used;
			Key key;
			Ptr ptr;
			Rec *next;		
		};

		//Valore di default per value
		Value xdef;

		//Dimensione attuale della tabella
		unsigned long table_size;
		unsigned long table_size_1;

		//Dimensione della tabella per il chaining
		unsigned long extra_size;

		//Numero di entries
		unsigned long number_of_entries;

		//Massimo numero di entries
		//per la table_size attuale
		unsigned long max_number_of_entries;

		//Table
		Rec* table;
		Rec* extra;
		Rec* end;		

		//Eseguita log2(n) volte
		void double_table();

		//Ritorna la posizione libera e sposta il 
		//puntatore		
		inline Rec* get_free_slot();

		inline bool no_more_slots() const;

		//Eseguita almeno n volte
		//Questa funzione �utilizzata direttamente solo da doubling
		//e quindi non �necessario che controlli che vi sia spazio nella 
		//extra table.
		//Quando viene utilizzata da add_entry il controllo �gi�stato fatto
		inline Rec* simple_add_entry(const Key& key, const Value& value, Rec *rec);

		inline Rec* reinsert_entry(const Key& key, Ptr& ptr, Rec *rec);
		
		inline Rec* add_entry(Rec * prec, const Key& key, const Value& value);

		inline Rec* find_key(Rec *first, const Key& key) const;

		//Eseguita almeno n volte
		inline Rec* HASH(Key key) const;

		inline double load_factor() const;

		void copy(const gdtmap<Key,Value>& M);

	protected:

		void init_table(unsigned long size = MIN_TABLE_SIZE);

		void delete_table();

		void set_default(Value x);

	public:

		bool consistency_check();

		typedef void* item;

		void show(std::ostream& os) const;

		//******************************
		//*                            *
		//* Constructors & destructors *
		//*                            *
		//******************************

		gdtmap();

		gdtmap(Value x);

		gdtmap(Value x, unsigned long table_sz);

		gdtmap<Key,Value>& operator = (const gdtmap<Key,Value>& M);

		gdtmap(const gdtmap<Key,Value>& M);

		~gdtmap();

		//*****************************
		//*                           *
		//* Access and update methods *
		//*                           *
        	//*****************************

		inline const Value& operator[](const Key& key) const;
  
		inline Value& operator[](const Key& key);
 
		bool defined(const Key& key) const;
		
		void undefine(const Key& key);
		
		void clear();
		
		void clear(Value x);

		void statistics() const;

		//*********************
		//*                   *
		//* Iteration methods *
		//*                   *
		//*********************

		item first_item() const;

		item next_item(item _it) const;

		Key key(item _it) const;

		inline Value& inf(item _it) const;
};

template <class Key, class Value>
void gdtmap<Key,Value>::double_table() {

	assert(table_size > 0 && extra_size > 0);			
			
	//Salvataggio vecchi valori
	Rec* old_table = table;
	Rec* old_end = end;
	unsigned long old_table_size = table_size;
	unsigned long old_extra_size = extra_size;						
	
	//Allocazione nuova tabella
	table = new Rec[ old_table_size * 2 + old_extra_size * 2];			
			
	//Inizializzazione 
	table_size = old_table_size * 2;
	table_size_1 = table_size - 1;
	extra_size = old_extra_size * 2;
	max_number_of_entries = (unsigned long)floor(table_size * UPPER_THRESHOLD);
			
	extra = table + table_size;
	end = extra + extra_size;

	//Inizializzazione della nuova tabella
	Rec *it;
	for (it = table; it < end; ++it) {
		it->used = false;				
	}			

	//Reinserimento dalla vecchia tabella			
	for (it = old_table; it < old_end; ++it) {
		if (it->used)
			reinsert_entry(it->key, it->ptr, HASH(it->key));
	}

	//Rilascio della vecchia tabella
	delete [] old_table;

	assert(consistency_check());
}

template <class Key, class Value>
inline bool gdtmap<Key,Value>::no_more_slots() const {
	return extra == end;
}

//Ritorna la posizione libera e sposta il
//puntatore
template <class Key, class Value>
inline typename gdtmap<Key,Value>::Rec* gdtmap<Key,Value>::get_free_slot() {
	assert(extra != end);
	return extra++;
}



//Eseguita almeno n volte
//Questa funzione �utilizzata direttamente solo da doubling
//e quindi non �necessario che controlli che vi sia spazio nella
//extra table.
//Quando viene utilizzata da add_entry il controllo �gi�stato fatto
template <class Key, class Value>
inline typename gdtmap<Key,Value>::Rec* gdtmap<Key,Value>::simple_add_entry(const Key& key, const Value& value, Rec *rec) {

	if (rec->used) {
		Rec * old_second = rec->next;
		Rec * new_rec = get_free_slot();
		rec->next = new_rec;
		new_rec->used = true;
		new_rec->key = key;
		new_rec->ptr = ptr_copy(value);
		new_rec->next = old_second;
		return new_rec;
	}
	else {
		rec->used = true;
		rec->key = key;												
		rec->ptr = ptr_copy(value);
		rec->next = NULL;
		return rec;
	}			

	// assert(consistency_check()); titto 13/08/2006 to avoid warning
}

template <class Key, class Value> 
inline typename gdtmap<Key,Value>::Rec* gdtmap<Key,Value>::reinsert_entry(const Key& key, Ptr& ptr, Rec *rec) {

	if (rec->used) {
		Rec * old_second = rec->next;
		Rec * new_rec = get_free_slot();
		rec->next = new_rec;
		new_rec->used = true;
		new_rec->key = key;
		new_rec->ptr = ptr;
		new_rec->next = old_second;
		return new_rec;
	}
	else {
		rec->used = true;
		rec->key = key;
		rec->ptr = ptr;
		rec->next = NULL;
		return rec;
	}

 	// assert(consistency_check()); titto 13/08/2006 to avoid warning 
}

template <class Key, class Value>
inline typename gdtmap<Key,Value>::Rec* gdtmap<Key,Value>::add_entry(Rec * prec, const Key& key, const Value& value) {

	Rec* res;

	if (number_of_entries > max_number_of_entries) {
		double_table();
		res = simple_add_entry(key, value, HASH(key));
	}
	else {
		if (prec->used && no_more_slots()) {
			double_table();
			res = simple_add_entry(key, value, HASH(key));
		}
		else {
			res = simple_add_entry(key, value, prec);
		}
	}
	++number_of_entries;

	assert(consistency_check());
	return res;
}

template <class Key, class Value>
inline typename gdtmap<Key,Value>::Rec* gdtmap<Key,Value>::find_key(Rec *first, const Key& key) const {
	assert(first && first >= table && first < (table + table_size));
	if (first->used) {
		Rec *it = first;
		while (it && it->key != key) {
			assert(it >= table && it < end);
			it = it->next;
		}
		assert((it && it->key == key) || !it);
		return it;
	}
	else
		return NULL;
}

//Eseguita almeno n volte
template <class Key, class Value>
inline typename gdtmap<Key,Value>::Rec* gdtmap<Key,Value>::HASH(Key key) const {
	Rec* res = table + (((unsigned long) Hash(key)) & table_size_1);
	assert(res < table + table_size);
	return res;
}

template <class Key, class Value>
inline double gdtmap<Key,Value>::load_factor() const {
	return (double) (number_of_entries + 1) / table_size;
}

template <class Key, class Value>
void gdtmap<Key,Value>::copy(const gdtmap<Key,Value>& M) {

	assert(table);
	//cout << endl<< "Chiamata di Copy" << endl;
	//std::cout << "table_size: " << table_size << std::endl;
	//std::cout << "number of entries: " << number_of_entries << std::endl;
	//std::cout << "M table_size: " << M.table_size << std::endl;
	//std::cout << "M.number of entries: " << M.number_of_entries << std::endl;
	//Copia a basso livello
	Rec* itM = M.table;
	//std::cout << itM << ' ' << M.table << std::endl;
	Rec* it = table;
	for (; it < end; ++it, ++itM) {
		assert(itM < M.end);
		if (itM->used) {
			++number_of_entries;
			//std::cout << "Now number of entries=" << number_of_entries << endl;
			it->used = true;
			it->key = itM->key;
			it->ptr = ptr_copy(PTR_ACCESS(Value,itM->ptr));
			it->next = itM->next ? (itM->next - M.table) + table : NULL;
		}
	}

	//Aggiorna
	extra = (M.extra - M.table) + table;
	xdef = M.xdef;

	assert(consistency_check());
}

template <class Key, class Value>
void gdtmap<Key,Value>::init_table(unsigned long size) { // = MIN_TABLE_SIZE) {
	
	
	size = size > MIN_TABLE_SIZE ? size : MIN_TABLE_SIZE;
	
	///////////////////////////////////////////////////////////////////
	//std::cout << "posizioni da shiftare (senza ceil)=" << log10(double (size))/log10(double(2)) << std::endl;
	//std::cout << "posizioni da shiftare =" << std::ceil(log10(double (size))/log10(double(2))) << std::endl;
	//////////////////////////////////////////////////////////////////
	//double tempf=log10(double(size))/log10(2.);
	size = 1 << (int) (gdtceil(log10(double(size))/log10(2.)));
	//size = 1 << (int) (std::ceil(tempf));
	//std::cout << "size=" << size << std::endl;
	table_size = size;
	table_size_1 = table_size - 1;
	extra_size = size / 2;

	table = new Rec[table_size + extra_size];
	assert(table);

	number_of_entries = 0;
	max_number_of_entries = (unsigned long) floor((double) size * UPPER_THRESHOLD);

	unsigned long all_size = table_size + extra_size;
	for (unsigned long i = 0; i < all_size; ++i) {
		table[i].used = false;
	}

	//Inizio della parte extra
	extra = table + table_size;

	//Posizione successiva alla fine dalla tabella
	end = extra + extra_size;

	assert(consistency_check());

}

template <class Key, class Value>
void gdtmap<Key,Value>::delete_table() {
	if (table) {
		for (Rec* rec = table; rec < end; ++rec) {
			if (rec->used) PTR_DELETE(Value, rec->ptr);
		}
		delete [] table;
		table = NULL;
	}
}

template <class Key, class Value>
void gdtmap<Key,Value>::set_default(Value x) {
	xdef = x;
}

template <class Key, class Value>
bool gdtmap<Key,Value>::consistency_check() {
	assert(table);
	if (table_size < 0)
		std::cout << "table_size < 0" << std::endl;
	assert(table_size_1 == table_size - 1);
	assert(extra_size > 0);
	assert(table + table_size + extra_size == end);
	return true;
}

template <class Key, class Value>
void gdtmap<Key,Value>::show(std::ostream& os) const {
	std::cout << "Table size " << table_size << std::endl;
	std::cout << "Entries    " << number_of_entries << std::endl;
	std::cout << "Load factor " << load_factor() << std::endl;
	for (unsigned long i = 0; i < table_size; ++i) {
		Rec* it = table + i;
		os << "Index " << i << " ";
		if (it->used) {
			while (it) {
				os << "(" << it->key << "," << it << "," << it->ptr << ") ";
				it = it->next;
			}

		}
		os << std::endl;
	}
			os << "---" << std::endl;
}

//******************************
//*                            *
//* Constructors & destructors *
//*                            *
//******************************

template <class Key, class Value>
gdtmap<Key,Value>::gdtmap() {
	table = NULL;
	init_table();
	assert(consistency_check());
}

template <class Key, class Value>
gdtmap<Key,Value>::gdtmap(Value x) : xdef(x) {
	table = NULL;
	init_table();
	assert(consistency_check());
}

template <class Key, class Value>
gdtmap<Key,Value>::gdtmap(Value x, unsigned long table_sz) : xdef(x) {
	table =NULL;
	init_table(table_sz);
	assert(consistency_check());
}

template <class Key, class Value>
gdtmap<Key,Value>& gdtmap<Key,Value>::operator = (const gdtmap<Key,Value>& M) {
	//Pulisce la tabella corrente
	delete_table();
	//
	init_table(M.table_size);
	copy(M);
	
	assert(consistency_check());
	return *this;
}

template <class Key, class Value>
gdtmap<Key,Value>::gdtmap(const gdtmap<Key,Value>& M) {
	init_table(M.table_size);
	//cout << "Ciao Ciao!!!!!" << endl;
	copy(M);
	assert(consistency_check());
}

template <class Key, class Value>
gdtmap<Key,Value>::~gdtmap() {
	gdtmap<Key,Value>::delete_table();			
}

//*****************************
//*                           *
//* Access and update methods *
//*                           *
//*****************************

template <class Key, class Value>
const Value& gdtmap<Key,Value>::operator[](const Key& key) const {			
	Rec* it = find_key(HASH(key), key);			
	return it ? PTR_CONST_ACCESS(Value, it->ptr) : xdef;
}
  
template <class Key, class Value> 
Value& gdtmap<Key,Value>::operator[](const Key& key) {
	Rec* first = HASH(key);
	Rec* it = find_key(first, key);
	assert(it ? (it->key == key) : true);
	return PTR_ACCESS(Value, it ?  it->ptr : add_entry(first, key, xdef)->ptr);
}

template <class Key, class Value>
bool gdtmap<Key,Value>::defined(const Key& key) const {			
	return find_key(HASH(key), key) != NULL;
}

template <class Key, class Value>		
void gdtmap<Key,Value>::undefine(const Key& key) {
	Rec* first = HASH(key);	
	if (first->used) {
		Rec *it = first;
		//Rec *pred = first;
		while (it && it->key != key) {					
			it = it->next;
		}				
		if (it) { //La chiave esiste
			PTR_DELETE(Value, it->ptr);
			PTR_DEFAULT_INIT(Value,it->ptr);
		}
		//else do nothing
	}
}

template <class Key, class Value>		
void gdtmap<Key,Value>::clear() {
	delete_table();			
	init_table();
	assert(consistency_check());
}

template <class Key, class Value>
void gdtmap<Key,Value>::clear(Value x) {			
	xdef = x;
	clear();			
	assert(consistency_check());
}

template <class Key, class Value>
void gdtmap<Key,Value>::statistics() const {
	std::cout << "table_size: " << table_size << std::endl;
	std::cout << "number of entries: " << number_of_entries << std::endl;

	unsigned long used_slots = 0;
	for (unsigned long i = 0; i < table_size; ++i) {
		if (table[i].used) ++used_slots;
	}

	std::cout << "fraction of entries in first position: " << ((double) used_slots / number_of_entries) << std::endl;
	std::cout << "fraction of empty lists: " << ((double) (table_size - used_slots)/number_of_entries) << std::endl<< std::flush;
}

//*********************
//*                   *
//* Iteration methods *
//*                   *
//*********************

template <class Key, class Value>
typename gdtmap<Key,Value>::item gdtmap<Key,Value>::first_item() const {
	if (table == NULL) return NULL;
	Rec *it = table;
	for (; it < end && !it->used; ++it);
	return (void*) it != end ? it : NULL;
}

template <class Key, class Value>
typename gdtmap<Key,Value>::item gdtmap<Key,Value>::next_item(item _it) const {
	Rec* it = (Rec*) _it;
	if (it && it < end) {
		++it;
		for (; it < end && !it->used; ++it);
			return it < end ? it : NULL;
	}
	else 
		return NULL;			
}

template <class Key, class Value>
Key gdtmap<Key,Value>::key(item _it) const {
	Rec* it = (Rec*) _it;
	assert(it && it->used);
	return it->key;			
}

template <class Key, class Value>
Value& gdtmap<Key,Value>::inf(item _it) const { 
	Rec* it = (Rec*) _it;
	assert(it && it->used);
	return PTR_ACCESS(Value, it->ptr);
}	

}

#endif
