/*! \file */

#ifndef __gdtp_queue__
#define __gdtp_queue__

#include "gdtbinary_heap2.h"

namespace gdt {

typedef heap_item pq_item;

template <class T, class L> 
struct LessToGreater { 
	bool operator () (const T& x, const T& y) {
		return L()(y,x);
	}
};

template<class P, class I, class less = stdless<P> > 
class gdtp_queue : protected gdtbinary_heap2<P, I, LessToGreater<P, less> > 
{
	typedef gdtbinary_heap2<P, I, LessToGreater<P, less> > base;	

public:	

	const P& prio(pq_item it) const 
	{ 
		return base::prio(it);
	}

	const I& inf(pq_item it) const {	
		return base::inf(it);
	}

	pq_item insert(const P& p, const I& i) {
		return base::insert(p,i);
	}

	pq_item find_min() const { 
		return base::find_max();
	}

	void decrease_p(pq_item it, const P& x) { 
		base::increase_key(it,x); 
	}	

	P del_min() 
	{ 
		assert(base::size()>0);
		P p = base::prio(base::find_max());
		base::extract_max();
		return p;
	}

	void del_item(pq_item it) {
		base::remove(it);
	}

	bool empty() const {
		return base::size() == 0;
	}

	typedef heap_item item;
	heap_item first_item() const { return base::data.first(); }
	heap_item last_item() const { return base::data.last(); }
	heap_item next_item(heap_item p) const { return base::data.next_item(p); }
	heap_item pred_item(heap_item p) const { return base::data.pred_item(p); }
};


}



#endif

