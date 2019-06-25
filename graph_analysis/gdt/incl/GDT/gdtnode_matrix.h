/*! \file */

#ifndef __gdtnode_matrix__
#define __gdtnode_matrix__

#include <GDT/rm3_undi_graph.h>
#include <GDT/gdt_graph_array.h>

namespace gdt {

template<class E>

class gdtnode_matrix {

	gdtnode_array< gdtnode_array<E> > M;

public:

gdtnode_matrix() {}

gdtnode_matrix(const undi_graph& G)  { init(G);   }

gdtnode_matrix(const undi_graph& G, E x) { init(G,x); }

gdtnode_matrix(const gdtnode_matrix<E>& mat) : M(mat.M) {}

gdtnode_matrix<E>& operator=(const gdtnode_matrix<E>& mat) 
{ M = mat.M; return *this; }

~gdtnode_matrix() {}

void  init(const undi_graph& G)
{ 
	M.init(G); 
	gdtnode v;
	forall_nodes(v,G) M[v].init(G);
}

void  init(const undi_graph& G, E x)
{ 
	M.init(G); 
	gdtnode v;
	forall_nodes(v,G) M[v].init(G,x);
}

const gdtnode_array<E>& operator[](gdtnode v) const { return M[v]; }
      gdtnode_array<E>& operator[](gdtnode v)       { return M[v]; }

const E&  operator()(gdtnode v, gdtnode w) const { return M[v][w]; }
      E&  operator()(gdtnode v, gdtnode w)       { return M[v][w]; }

};

}

#endif
