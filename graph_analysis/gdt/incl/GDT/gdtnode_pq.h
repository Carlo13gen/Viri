/*! \file */

#ifndef __gdtnode_pq__
#define __gdtnode_pq__

#include "gdtp_queue.h"
#include "gdtmap.h"

namespace gdt {

template <class P>
class gdtnode_pq : public gdtp_queue<P,gdtnode>  {

	const undi_graph* g;	
	typedef gdtp_queue<P,gdtnode> base;
	gdtmap<gdtnode, pq_item> item;

public:
 
	gdtnode_pq(const undi_graph& _g) : g(&_g), item(NULL) {}
	
	~gdtnode_pq() { }

	void insert(gdtnode v, const P& p) {
		pq_item it = base::insert(p,v); 
		item[v] = it;
	}	

	const P& prio(gdtnode v) const	{ return base::prio(v);	}	

	bool member(gdtnode v) const { return (item[v] != NULL); }

	void decrease_p(gdtnode v, const P& p)	{ base::decrease_p(p,v); }
	
	gdtnode find_min() const { return base::inf(base::find_min()); }
	
	void del(gdtnode v) { 
		base::del_item(item[v]); 
		item.undefine(v);
	}	

	gdtnode del_min()   
	{ 
		gdtnode v = find_min(); 
		if (v) { base::del_min(); }
		return v;
	}

	void get_nodes(gdtlist<gdtnode>& l) 
	{ 
		l.clear();
		gdtnode v;
		gdt::list_item it;
		forall_items(it,*this) {
			l.append(base::inf(it));
		}
	}

	void clear() { 
		base::clear();
		item.clear();
	}			

	const P& inf(gdtnode v) const {
		return base::prio(item[v]);		
	}	

	void decrease_inf(gdtnode v, const P& x) { base::decrease_p(item[v], x); }
};
}

#endif
