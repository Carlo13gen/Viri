/*! \file */

#ifndef __gdt_graph_array__
#define __gdt_graph_array__


#include <GDT/gdtmap.h>
#include <GDT/gdtlist.h>


class undi_graph;
class struct_gdtnode;
class struct_gdtedge;


namespace gdt {

class EDGE_OP {
public:
	static gdtlist<struct_gdtedge*> all(const undi_graph *g); //{return g->all_edges();};
	
	static int number(const undi_graph *g); //{return g->number_of_edges();}; 
};

class NODE_OP {
public:
	static gdtlist<struct_gdtnode*> all(const undi_graph *g); //{return g->all_nodes();}; 
	
	static int number(const undi_graph *g); //{return g->number_of_nodes()};
};

/*
class FACE_OP {
public:
	static gdtlist<face> all(const undi_graph *g);
	static int number(const undi_graph *g);
};
*/

template <class I, class OP, class E>
class gdt_graph_array : public gdtmap<I, E>
{	
	private:

		const undi_graph *g; 

		typedef gdtmap<I, E> base;
		typedef gdt_graph_array<I,OP,E> self;

	protected:		
  
	public:	

		gdt_graph_array() : base(), g(NULL) {}

		gdt_graph_array(const undi_graph& _g) : g(&_g) {		
			assert(g);
			gdtmap<I,E>::delete_table();  // inserted by titto 8/8/2005 to fix memory leak
			gdtmap<I,E>::init_table(OP::number(g)); // added "gdtmap<I,E>::" by titto 20/3/2016
		}
		
		gdt_graph_array(const undi_graph& _g, E x) : base(x), g(&_g) {
			assert(g);
			gdtmap<I,E>::delete_table();  // inserted by titto 8/8/2005 to fix memory leak
			gdtmap<I,E>::init_table(OP::number(g)); // added "gdtmap<I,E>::" by titto 20/3/2016
			I i;
			gdtlist<I> objs = OP::all(g);
			forall(i, objs) {
				this->operator[](i) = x;
			}
		}

		gdt_graph_array(const self& A) : base(A), g(A.g) {
                //assert(g); (pizzo april 7, 2003)
		}

		self& operator=(const self& A) 
		{ 
			assert(A.g);
			g = A.g;		
			base::operator = (A);
			return *this;
		}	

		const E& const_get(I i) const {
			assert(g);
			return base::operator[](i);
		}
		
		E& get(I i) {  
			assert(g);
			return base::operator[](i);
		}
		
		const undi_graph& get_graph() const { return *g; }			

		void init() { 
			gdtmap<I,E>::delete_table();
			g = NULL;
			gdtmap<I,E>::init_table();
		}

		void init(const undi_graph& _g)      
		{ 
			gdtmap<I,E>::delete_table();
			g = &_g;
			gdtmap<I,E>::init_table(OP::number(g));
		}

		void init(const undi_graph& _g, E x) 
		{ 
			gdtmap<I,E>::set_default(x);  // added "gdtmap<I,E>::" by titto 20/3/2016
			init(_g);
		}
		
		void init(const undi_graph& _g, int n, E x) 
		{
			gdtmap<I,E>::set_default(x);  // added "gdtmap<I,E>::" by titto 20/3/2016
			g = &_g;
			gdtmap<I,E>::init_table(n);
		}
};

//#ifndef _DEBUG_NODE_ARRAY

template <class E>
class gdtnode_array : public gdt_graph_array<struct_gdtnode*, NODE_OP, E>
{	
	private:	
		typedef gdt_graph_array<struct_gdtnode*, NODE_OP, E> base;
		typedef gdtnode_array<E> self;
	public:
		gdtnode_array() : base() {}
		gdtnode_array(const undi_graph& g) : base(g) {}
		gdtnode_array(const undi_graph& g, E x) : base(g,x) {}
		gdtnode_array(const self& A) : base(A) {}

		self& operator=(const self& A) { base::operator = (A); return *this; }
		const E&  operator[](struct_gdtnode* i) const { return base::const_get(i);	}		
		E& operator[](struct_gdtnode* i) { return base::get(i); }				
};

/*
#else

template <class E>
class struct_gdtnode*_array
{	
	private:	
		node_array<E> _debug_array;		
	public:
		struct_gdtnode*_array() {}
		struct_gdtnode*_array(const graph& g) : _debug_array(g) {}
		struct_gdtnode*_array(const graph& g, E x) : _debug_array(g,x) {}
		struct_gdtnode*_array(const struct_gdtnode*_array<E>& A) : _debug_array(A._debug_array) {}

		struct_gdtnode*_array<E>& operator=(const struct_gdtnode*_array<E>& A) { _debug_array = A._debug_array; return *this;}
		const E&  operator[](struct_gdtnode* i) const { return _debug_array[i];}		
		E& operator[](struct_gdtnode* i) { return _debug_array[i];}	
		
		void init() { _debug_array.init(); }		
		void init(const graph& g)  { _debug_array.init(g); }		
		void init(const graph& g, E x) { _debug_array.init(g,x); }						
		void init(const graph& g, int n, E x) { _debug_array.init(g,n,x); }		
};

#endif
*/
//#ifndef _DEBUG_EDGE_ARRAY

template <class E>
class gdtedge_array : public gdt_graph_array<struct_gdtedge*, EDGE_OP, E>
{	
	private:	
		typedef gdt_graph_array<struct_gdtedge*, EDGE_OP, E> base;
		typedef gdtedge_array<E> self;
	public:
		gdtedge_array() : base() {}
		gdtedge_array(const undi_graph& g) : base(g) {}
		gdtedge_array(const undi_graph& g, E x) : base(g,x) {}
		gdtedge_array(const self& A) : base(A) {}

		self& operator=(const self& A) { base::operator = (A); return *this;}
		const E&  operator[](struct_gdtedge* i) const { return base::const_get(i);}		
		E& operator[](struct_gdtedge* i) { return base::get(i);}				
};
/*
#else

template <class E>
class struct_gdtedge*_array
{	
	private:	
		edge_array<E> _debug_array;		
	public:
		struct_gdtedge*_array() {}
		struct_gdtedge*_array(const graph& g) : _debug_array(g) {}
		struct_gdtedge*_array(const graph& g, E x) : _debug_array(g,x) {}
		struct_gdtedge*_array(const struct_gdtedge*_array<E>& A) : _debug_array(A._debug_array) {}

		struct_gdtedge*_array<E>& operator=(const struct_gdtedge*_array<E>& A) { _debug_array = A.debug_array; return *this;}
		const E&  operator[](struct_gdtedge* i) const { return _debug_array[i];}		
		E& operator[](struct_gdtedge* i) { return _debug_array[i];}				

		void init() { _debug_array.init(); }		
		void init(const graph& g)  { _debug_array.init(g); }		
		void init(const graph& g, E x) { _debug_array.init(g,x); }						
		void init(const graph& g, int n, E x) { _debug_array.init(g,n,x); }		
};

#endif
*/
//#ifndef _DEBUG_NODE_MAP

template <class E>
class gdtnode_map : public gdtnode_array<E>
{	
	private:	
		typedef gdtnode_array<E> base;
		typedef gdtnode_map<E> self;
	public:
		gdtnode_map() {}
		gdtnode_map(const undi_graph& g) : base(g) {}
		gdtnode_map(const undi_graph& g, E x) : base(g,x) {}
		gdtnode_map(const self& A) : base(A) {}

		self& operator=(const self& A) { base::operator = (A); return *this; }
		const E&  operator[](struct_gdtnode* i) const { return base::const_get(i);	}		
		E& operator[](struct_gdtnode* i) { return base::get(i); }				
};

/*
#else

template <class E>
class struct_gdtnode*_map
{	
	private:	
		node_map<E> _debug_map;
	public:
		struct_gdtnode*_map() {}
		struct_gdtnode*_map(const graph& g) : _debug_map(g) {}
		struct_gdtnode*_map(const graph& g, E x) : _debug_map(g,x) {}
		struct_gdtnode*_map(const struct_gdtnode*_map<E>& A) : _debug_map(A._debug_map) {}

		struct_gdtnode*_map<E>& operator=(const struct_gdtnode*_map<E>& A) { _debug_map = A._debug_map; return *this; }
		const E&  operator[](struct_gdtnode* i) const { return _debug_map[i];	}		
		E& operator[](struct_gdtnode* i) { return _debug_map[i]; }				

		void init() { _debug_map.init(); }		
		void init(const graph& g)  { _debug_map.init(g); }		
		void init(const graph& g, E x) { _debug_map.init(g,x); }						
		void init(const graph& g, int n, E x) { _debug_map.init(g,n,x); }		
};

#endif
*/
//#ifndef _DEBUG_EDGE_MAP

template <class E>
class gdtedge_map : public gdtedge_array<E>
{
	private:	
		typedef gdtedge_array<E> base;
		typedef gdtedge_map<E> self;
	public:
		gdtedge_map() {}
		gdtedge_map(const undi_graph& g) : base(g) {}
		gdtedge_map(const undi_graph& g, E x) : base(g,x) {}
		gdtedge_map(const self& A) : base(A) {}

		self& operator=(const self& A) { base::operator = (A); return *this; }
		const E&  operator[](struct_gdtedge* i) const { return base::const_get(i); }
		E& operator[](struct_gdtedge* i) { return base::get(i); }
};

/*
#else

template <class E>
class struct_gdtedge*_map
{
	private:	
		edge_map<E> _debug_map;		
	public:
		struct_gdtedge*_map() {}
		struct_gdtedge*_map(const graph& g) : _debug_map(g) {}
		struct_gdtedge*_map(const graph& g, E x) : _debug_map(g,x) {}
		struct_gdtedge*_map(const struct_gdtedge*_map<E>& A) : _debug_map(A._debug_map) {}

		struct_gdtedge*_map<E>& operator=(const struct_gdtedge*_map<E>& A) { _debug_map = A._debug_map; return *this; }
		const E&  operator[](struct_gdtedge* i) const { return _debug_map[i]; }
		E& operator[](struct_gdtedge* i) { return _debug_map[i]; }

		void init() { _debug_map.init(); }		
		void init(const graph& g)  { _debug_map.init(g); }		
		void init(const graph& g, E x) { _debug_map.init(g,x); }						
		void init(const graph& g, int n, E x) { _debug_map.init(g,n,x); }		
};

#endif
*/

};

#endif
