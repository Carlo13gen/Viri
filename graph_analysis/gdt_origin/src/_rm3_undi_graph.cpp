/*******************************************************************************
+  _rm3_undi_graph.cpp
+
+  This file is part of GDToolkit. It can be
+  used free of charge in academic research and teaching.
+  Any direct or indirect commercial use of this software is illegal
+  without a written authorization of
+  the Dipartimento di Informatica e Automazione
+  Universita' di Roma Tre, Roma, ITALIA
+
+
*******************************************************************************/

#include <assert.h>
#include <sstream>
#include <memory>



#include <GDT/gdt_error.h>
#include <GDT/gdtarray.h>
#include <GDT/gdtstack.h>
#include <GDT/gdtqueue.h>
#include <GDT/rm3_global.h>
#include <GDT/rm3_undi_graph.h>
#include <GDT/rm3_dire_graph.h>
#include <GDT/rm3_plan_undi_graph.h>
#include <GDT/rm3_simple_graph.h>

#ifdef COUNTER
#include <GDT/stopwatch.h>
#endif

using namespace std;
//#define DEBUG


//#include<GDT/rm3_draw_undi_graph.h>

const gdtnode 	 NULL_NODE 	   = (gdtnode)NULL;		// NULL gdtnode
const gdtedge 	 NULL_EDGE 	   = (gdtedge)NULL;		// NULL gdtedge
const constraint NULL_CONSTRAINT   = (constraint)NULL;		// NULL constraint


void* debug_pointer1;
void* debug_pointer2;
void* debug_pointer3;

class dire_graph; // forward declaration


//-----------------------------------------------------------------------------
// undi_graph: base class for all directed and undirected embedded graphs
//
// W.Didimo e A.Leonforte (1996)
//-----------------------------------------------------------------------------



	struct_undi_edge_info::
struct_undi_edge_info
	()
	{
	//cout << "\n struct_undi_edge_info () creator \n" << flush;
	id = NULL_ID;
	source = NULL_NODE;
	target = NULL_NODE;
	start = NULL_NODE;
	pos_into_source_adj_edges = NULL;
	pos_into_target_adj_edges = NULL;
	}


	struct_undi_node_info::
struct_undi_node_info
	()
	{
	//cout << "\n struct_undi_edge_info () creator \n" << flush;
	id = NULL_ID;
	in_edges.clear ();
	out_edges.clear();
	adj_edges.clear();
	in_edges_are_ordered  = false;
	out_edges_are_ordered = false;
	}

// -----------------------------------------------------------------------------
// 	PUBLIC
// -----------------------------------------------------------------------------

	int
	undi_graph::
unweighted_unoriented_shortest_path
	(
		gdtnode	    start_node,
		gdtnode	    end_node,
		gdt::gdtlist<gdtnode>& nodes,
		gdt::gdtlist<gdtedge>& edges
	) const
	{
	gdt::gdtnode_array<bool> reached(nodes_and_edges(),false);
	gdt::gdtnode_array<gdtedge> father(nodes_and_edges(),NULL_EDGE);
	gdt::gdtnode_array<int>  dist(nodes_and_edges(),0);
	gdt::gdtlist<gdtnode>       order;

	simple_BFS(start_node, reached, father, dist, order);

	if( !reached[end_node] ) return INFINITE;
		//gdt_erro("end_node not reached!");

	gdtnode actual_node = end_node;

	while( actual_node != start_node )				// 	s--->o--->o....o--->a
		{
		gdtedge actual_edge = father[actual_node];			// 	s--->o--->o....o-a->a
		nodes.push(actual_node);
		edges.push(actual_edge);
		actual_node = opposite( actual_node, actual_edge );	// 	s--->o--->o....a
		}
	nodes.push( start_node);
	return dist[end_node];		// number of edges in between
	}



	undi_graph::
undi_graph
	()
	{
	if (!check_date())  gdt_error("\n\nTHIS COPY OF GDTOOLKIT IS EXPIRED!!!\n\n");
	//cout << "\n undi_graph () creator \n" << flush;
	local_new();
	local_init();
	}



	undi_graph::
~undi_graph
	()
	{
	local_del();
	}


/*
	undi_graph::
undi_graph
	(const graph& g)
	{
	//cout << "\n undi_graph (graph) creator \n" << flush;
	local_new();  // required because operator= calls renew()
	operator=(g);
	}
*/


	undi_graph::
undi_graph
	(const undi_graph& ug)
	{
	//cout << "\n undi_graph (undi_graph) creator \n" << flush;
	local_new();  // required because operator= calls renew()
	operator=(ug);
	}


	undi_graph&
	undi_graph::
operator=
	(const undi_graph& ug)
	{
	// the graph-part of *this is built according
	// to the corresponding graph-part of ug.
	
	gdtnode       u;
	gdtnode       v,w;
	gdtedge       e,ev,ew;
	constraint c;

	//cout << "\nundi_graph -(copy)-> undi_graph" << flush;

	gdt::gdtnode_array<gdtnode> node_corr(ug.nodes_and_edges(),NULL_NODE);
	gdt::gdtedge_array<gdtedge> edge_corr(ug.nodes_and_edges(),NULL_EDGE);
	gdt::gdtedge_array<bool> edge_copied(ug.nodes_and_edges(),false);

	// ----------------------------------------------------------------
	// local_renew(); commented by W.D. on October-20-1999 and replaced
	//                with the following code
	//
	// clear and restore the internal data structures
	// ----------------------------------------------------------------

	//g->clear();

	//Added by A.M. 2/2002
	
	
	gdt::gdtstack<constraint> S;
	forall_constraints(c,*this)
		S.push(c);
	while (!S.empty())
		del_constraint(S.pop());
	
		
	while (!nodes->empty()) delete(nodes->pop_front());
	while (!edges->empty()) delete(edges->pop_front());
	delete(nodes);
	delete(edges);
	//

	delete(node_info);
	delete(edge_info);
	
	delete(constraints);

	delete(node_with_id);
	delete(edge_with_id);
	delete(constraint_with_id);

	//Added by A.M. 02/2002
	nodes = new gdt::gdtlist<gdtnode>;
	edges = new gdt::gdtlist<gdtedge>;
	//

	node_info = new (gdt::gdtnode_map<struct_undi_node_info>);
	edge_info = new (gdt::gdtedge_map<struct_undi_edge_info>);
	node_info->init(nodes_and_edges());
	edge_info->init(nodes_and_edges());

	constraints = new gdt::gdtlist<constraint>;

	node_with_id       = new gdt::gdtmap<int,gdtnode>(NULL_NODE);
	edge_with_id 	   = new gdt::gdtmap<int,gdtedge>(NULL_EDGE);
	constraint_with_id = new gdt::gdtmap<int,constraint>(NULL_CONSTRAINT);

	max_node_id	  = NULL_ID;
	max_edge_id	  = NULL_ID;
	max_constraint_id = NULL_ID;
	keep_ordering_of_directed_edges = ug.keep_ordering_of_directed_edges;

	// ----------------------------------

	forall_nodes(u,ug)
		{
		node_corr[u]=new_node(ug.id(u));
		(*node_info)[node_corr[u]].markers=ug.markers(u);
		}
	//cout << "ug->number_of_edges()=" << ug.number_of_edges() << endl;
	forall_edges(e,ug)
		{
		//cout << "Iteration with edge " << ug.id(e) << endl;
		v=ug.source(e);
		w=ug.target(e);
		if (degree(node_corr[v])>1)
			if (degree(node_corr[w])>1)
				{
				ev=ug.cyclic_adj_pred(e,v);
				while (!edge_copied[ev]) ev=ug.cyclic_adj_pred(ev,v);
				ew=ug.cyclic_adj_pred(e,w);
				while (!edge_copied[ew]) ew=ug.cyclic_adj_pred(ew,w);
				edge_corr[e]=new_edge(node_corr[v],edge_corr[ev],node_corr[w],edge_corr[ew],ug.id(e));
				}
			else
				{
				ev=ug.cyclic_adj_pred(e,v);
				while (!edge_copied[ev]) ev=ug.cyclic_adj_pred(ev,v);
				edge_corr[e]=new_edge(node_corr[v],edge_corr[ev],node_corr[w],ug.id(e));
				}
		else
			if (degree(node_corr[w])>1)
				{
				ew=ug.cyclic_adj_pred(e,w);
				while (!edge_copied[ew]) ew=ug.cyclic_adj_pred(ew,w);
				edge_corr[e]=new_edge(node_corr[w],edge_corr[ew],node_corr[v],ug.id(e));
				set_source(edge_corr[e],node_corr[v]);
				}
			else
				{
				edge_corr[e]=new_edge(node_corr[v],node_corr[w],ug.id(e));
				}
		if (ug.edge_is_directed(e))  make_directed(edge_corr[e],node_corr[ug.start(e)]);
		(*edge_info)[edge_corr[e]].markers=ug.markers(e);
		edge_copied[e]=true;

		}
	forall_constraints(c,ug) 
		c->copy(*this);
	return *this;
	}



	// -----------------
	// Access operations
	// -----------------



	void
	undi_graph::
local_get
	(
	gdt::gdtlist<gdtnode>*& p,
	gdt::gdtlist<gdtedge>*& p0,
	//graph*&                           p1,
	gdt::gdtnode_map<struct_undi_node_info>*& p2,
	gdt::gdtedge_map<struct_undi_edge_info>*& p3,
	gdt::gdtlist<constraint>*&		  p4,
	gdt::gdtmap<int,gdtnode>*&			  p5,
	gdt::gdtmap<int,gdtedge>*&			  p6,
	gdt::gdtmap<int,constraint>*&             p7,
	int&                              p8,
	int&                              p9,
	int&                              p10,
	bool&				  p11
	)
	{
	p = nodes;
	p0 = edges;
	//p1		= g;
	p2		= node_info;
	p3		= edge_info;
	p4		= constraints;
	p5		= node_with_id;
	p6		= edge_with_id;
	p7		= constraint_with_id;
	p8		= max_node_id;
	p9		= max_edge_id;
	p10		= max_constraint_id;
	p11		= keep_ordering_of_directed_edges;
	}



	const undi_graph&
	undi_graph::
nodes_and_edges
	() const
	{
	return *this;
	}




	int
	undi_graph::
generate_node_id
	()
	{
	return ++max_node_id;
	}



	int
	undi_graph::
generate_edge_id
	()
	{
	return ++max_edge_id;
	}



	int
	undi_graph::
generate_constraint_id
	()
	{
	return ++max_constraint_id;
	}




	int 
	undi_graph::
id
	(gdtnode v) const
	{
	return (*node_info)[v].id;
	}
	
	
	

	int 
	undi_graph::
id
	(gdtedge e) const
	{
	return (*edge_info)[e].id;
	}




	int
	undi_graph::
id
	(constraint c) const
	{
	return c->get_id();
	}




	int
	undi_graph::
get_max_node_id
	() const
	{
	return max_node_id;
	}

	


	int
	undi_graph::
get_max_edge_id  
	() const
	{
	return max_edge_id;
	}




	int
	undi_graph::
get_max_constraint_id  
	() const
	{
	return max_constraint_id;
	}


	

	constraint_type
	undi_graph::
type_of_constraint
	(constraint c) const
	{
	return c->get_type();
	}




	int
	undi_graph::
degree
	(gdtnode v) const
	{	
	gdt::gdtlist<gdtedge>& adj_edges = (*node_info)[v].adj_edges;
	return adj_edges.size();
	}
	

	

	int
	undi_graph::
degree_in
	(gdtnode v) const
	{
	gdt::gdtlist<gdtedge>& in_edges = (*node_info)[v].in_edges;
	return in_edges.size();
	}
	
	
	
	
	int
	undi_graph::
degree_out
	(gdtnode v) const
	{
	gdt::gdtlist<gdtedge>& out_edges = (*node_info)[v].out_edges;
	return out_edges.size();
	}
	


	int
	undi_graph::
number_of_nodes
	() const
	{
	//assert(nodes->size() == g->number_of_nodes());
	return nodes->size();
	}
	
	
	

	int
	undi_graph::
number_of_edges
	() const
	{
	//assert(edges->size() == g->number_of_edges());
	return edges->size();
	}
	


	
	int
	undi_graph::
number_of_constraints
	() const
	{
	return constraints->size();
	}


	
	int
	undi_graph::
max_degree
	() const
	{
	gdtnode v;
	int deg, maxdeg;
	maxdeg = degree(first_node());
	forall_nodes (v,*this)
		{
		deg = degree(v);
		if (deg > maxdeg) maxdeg = deg;
		}
	return maxdeg;
	}
	

	
	int
	undi_graph::
min_degree
	() const
	{
	gdtnode v;
	int deg, mindeg;
	mindeg = degree(first_node());
	forall_nodes (v,*this)
		{
		deg = degree(v);
		if (deg < mindeg) mindeg = deg;
		}
	return mindeg;
	}

	

	int
	undi_graph::
connected_components 
	(gdt::gdtnode_array<int>& component) const
	{
	gdt::gdtnode_array<bool> reached (nodes_and_edges(), false);
	gdt::gdtnode_array<gdtedge> father  (nodes_and_edges(), NULL_EDGE);
	gdt::gdtlist<gdtnode> 	 order;
	gdtnode v = first_node();
	gdtnode w;
	int i = 0;
	
	// initialize the visit markers
	//
	forall_nodes (v,*this) reached[v] = false;
	
	// research all the connected components
	//
	forall_nodes (v,*this)
		if (!reached[v]) 
			{
			i ++;
			order.clear();
			simple_DFS (v,reached,father,order);
			forall (w,order) component[w] = i;
			}
	return i;
	}
	
	
	
	int
	undi_graph::  
number_of_extremals
	() const
	{
	int extremals = 0;
	gdtnode v;
	forall_nodes (v,*this)
		if (is_extremal(v)) extremals++;
	return extremals;
	}



	
	bool
	undi_graph::
node_belongs_to_edge 
	(gdtnode v, gdtedge e) const
	{
	return ((v==source(e)) || (v==target(e)));
	}


	bool
	undi_graph::
edge_is_directed 
	(gdtedge e) const
	{
	return (start(e)!=NULL_NODE);
	}


	
	
	bool
	undi_graph::
all_edges_are_directed 
	() const
	{
	gdtedge e;
	forall_edges (e,*this) if (!edge_is_directed(e)) return false;
	return true;
	}

	
	
	bool
	undi_graph::
is_st_digraph
	(gdtnode& s, gdtnode& t) 
	{
	//cout << "\nSTART CHECK st-digraph" << flush;
	gdtnode v;
	
	s = NULL_NODE;
	t = NULL_NODE;
	
	// check if the graph is directed
	//
	if (!all_edges_are_directed()) 
		{
		//cout << "\nno all edges are directed" << flush;
		return false;
		}
	
	// check if the directed graph is acyclic
	//
	dire_graph dg (*this);
	
	//cout << "\nDIRE-GRAPH RESULTING: " << flush;
	//dg.print (COMPLETE); 
	
	if (!dg.is_acyclic()) 
		{
		//cout << "\nno acyclic" << flush;
		return false;
		}
	
	// if an only source and an only sink exist, 
	// put them into s and t respectively;
	//
	forall_nodes (v,nodes_and_edges())
		{
		if (degree_in(v) == 0)	// v is a source
			if (s) return false;
			else   s = v;
		else
		if (degree_out(v) == 0)	// v is a sink
			if (t) return false;
			else   t = v;	
		} 

	if (s && t) return true;
	else	    return false;
	}
	
	
	
	bool
	undi_graph::
is_connected
	() const
	{
	gdt::gdtnode_array<int> component (nodes_and_edges(), 0);
	if (connected_components (component) == 1)  return true;
	else return false;
	}
	
		
	
	bool
	undi_graph::
is_biconnected
	() const
	{
	gdtnode w;
	gdtnode v = first_node ();
	gdtedge e = first_adj_edge (v);
	gdt::gdtnode_array<bool> reached (nodes_and_edges(), false);
	gdt::gdtnode_array<gdtedge> father  (nodes_and_edges(), NULL_EDGE);
	gdt::gdtlist<gdtnode> order;
	gdt::gdtnode_array<gdtnode> lowpt1   (nodes_and_edges(), NULL_NODE),
			 lowpt2   (nodes_and_edges(), NULL_NODE);
	gdt::gdtnode_array<int>	 num_disc (nodes_and_edges(), 0);
	
	bool root_is_articulated;

	if ((number_of_nodes() == 2) && (number_of_edges() > 0)) return true;
	if (!is_connected()) return false;
	
	forall_nodes (w,*this) reached[w]=false; //Added by LEONFORTE 18JAN97
	complex_DFS (v,e,reached,father,order,lowpt1,lowpt2,num_disc,root_is_articulated);
	
	if (root_is_articulated) return false;

	forall_nodes (w,*this)
		if (
		   (w != v) && 
		   (((lowpt1[w] == opposite(w,father[w])) && (opposite(w,father[w]) != v)) || (lowpt1[w] == w))
		   ) 
		return false;
	return true;	
	}
	

		
	bool
	undi_graph::
is_acyclic
	() const
	{
	gdt::gdtnode_array<bool> reached (nodes_and_edges(), false);
	gdt::gdtnode_array<gdtedge> father  (nodes_and_edges(), NULL_EDGE);
	gdt::gdtlist<gdtnode>       order;
	gdtnode v = first_node();
	gdtnode w;
	forall_nodes (w,*this) reached[w]=false; //Added by LEONFORTE 18JAN97
	if (simple_DFS (v,reached,father,order)) return false;
	else	    				 return true;	
	}
	


	bool
	undi_graph::
is_candidate
	(gdtnode v) const
	{
	if  ((degree(v) < 4) || (is_extremal(v))) return true;
	else
		{
		int invertions = 0;
		gdtedge e_init = first_adj_edge(v);
		gdtedge e1 = e_init;
		gdtedge e2 = NULL_EDGE;
		
		while ((e2 != e_init) && (invertions <= 2))
			{
			e2 = cyclic_adj_succ (e1,v);
			if ((start(e1)==v && stop(e2)==v) || 
			    (stop(e1)==v && start(e2)==v))   invertions++;
			e1 = e2;
			}
		if (invertions > 2) 
			{
			//gdtedge e;
			//cout << "\nnode " << id(v) << " is not candidate: " << flush;
			//forall_edges_adjacent_node (e,v,*this)
			//	if (v == start(e))     cout << "-"  << id(e) << "->" << flush;
			//	else if (v == stop(e)) cout << "<-" << id(e) << "- " << flush;
			//	else cout << "fatal error in is candidate" << flush;
			return false;
			}
		else    return true;
		}
	}
	

	
	bool
	undi_graph::
is_candidate
	() const
	{
	bool candidate = true;
	gdtnode v;
	
	forall_nodes (v,*this)
		if (!is_candidate(v))  
			{
			candidate = false;
			break;
			}
	return candidate;	
	}



	
	bool
	undi_graph:: 
is_source   
	(gdtnode v) const
	{
	if (!degree_in(v)) return true;
	else		   return false;
	}	
	
	
	
	
	bool
	undi_graph:: 
is_target
	(gdtnode v) const
	{
	if (!degree_out(v)) return true;
	else		     return false;
	}



	
	bool
	undi_graph:: 
is_extremal 
	(gdtnode v) const
	{
	if (is_source(v) || is_target(v)) return true;
	else				  return false;
	}



	
	bool
	undi_graph::
is_extremal
	(gdtnode v, gdtedge e) const
	{
	gdtedge e1 = cyclic_adj_succ(e,v);
	if ((start(e)==v && stop(e1)==v) || 
	    (stop(e)==v && start(e1)==v))  return false;
	
	return true;
	}

	
	

	bool
	undi_graph:: 
is_internal 
	(gdtnode v) const
	{
	if (!is_source(v) && !is_target(v)) return true;
	else				    return false;
	}	
	
	
	
	
	bool
	undi_graph:: 
is_internal
	(gdtnode v, gdtedge e) const
	{
	return (!is_extremal(v,e));
	}


	
	
	bool
	undi_graph::
is_multiple
	(gdtedge e) const
	{
	gdtnode v1 = source(e);
	gdtnode v2 = target(e);
	if (edges_with_extremal_nodes(v1,v2).length() > 1) return true;
	else						 return false;
	}
	
	
	
	
	bool
	undi_graph::
is_marked
	(gdtnode v, marker_type m) const
	{
	gdt::list_item mp;
	gdt::gdtlist<marker_type>& L = (*node_info)[v].markers;
	forall_items(mp, L)
		if (L.contents(mp) == m)		
			return true;	
	return false;
	}



	
	bool
	undi_graph::
is_marked
	(gdtedge e, marker_type m) const
	{
	gdt::list_item mp;
	gdt::gdtlist<marker_type>& L = (*edge_info)[e].markers;
	forall_items(mp, L)
		if (L.contents(mp) == m)		
			return true;	
	return false;
	}


	
	gdtnode
	undi_graph::
find_first_node_marked_as 
	(marker_type m)  const
	{
	gdtnode v,v1=NULL_NODE;
	forall_nodes (v,nodes_and_edges())
		if (is_marked(v, m))
			{
			v1=v;
			break;
			}
	return v1;
	}



	gdtedge 
	undi_graph::
find_first_edge_marked_as 
	(marker_type m)  const
	{
	gdtedge e,e1=NULL_EDGE;
	forall_edges (e,nodes_and_edges())
		if (is_marked(e, m))
			{
			e1=e;
			break;
			}
	return e1;	
	}
		
		
		
	gdt::gdtlist<gdtnode> 
	undi_graph::
find_nodes_marked_as 
	(marker_type m) const
	{
	gdtnode v;
	gdt::gdtlist<gdtnode> marked_nodes;
	forall_nodes (v,nodes_and_edges())
		if (is_marked(v, m))
			marked_nodes.append(v);
	return (marked_nodes);
	}



	gdt::gdtlist<gdtedge> 
	undi_graph::
find_edges_marked_as
	(marker_type m) const
	{
	gdtedge e;
	gdt::gdtlist<gdtedge> marked_edges;
	forall_edges (e,nodes_and_edges())
		if (is_marked(e, m))
			marked_edges.append(e);
	return (marked_edges);	
	}



  	gdt::gdtlist<gdtedge> 
	undi_graph::
find_adj_edges_marked_as
	(gdtnode v,marker_type m) const // return the list of adj edges of v marked m
	{
	gdtedge e;
	gdt::gdtlist<gdtedge> real_edges;
	gdt::gdtlist<gdtedge>& adj_edges = (*node_info)[v].adj_edges;
	forall(e,adj_edges)
		if(is_marked(e,m)) real_edges.append(e);
	return real_edges;		
	}
	
	
	
  	gdt::gdtlist<gdtedge> 
	undi_graph::
find_adj_edges_not_marked_as 
	(gdtnode v,marker_type m) const // return the list of adj edges of v not marked m
	{
	gdtedge e;
	gdt::gdtlist<gdtedge> real_edges;
	gdt::gdtlist<gdtedge>& adj_edges = (*node_info)[v].adj_edges;
	forall(e,adj_edges)
		if(!is_marked(e,m)) real_edges.append(e);
	return real_edges;
	}
	
	
	
	gdt::gdtlist<gdtedge> 
	undi_graph::
find_in_edges_marked_as 
	(gdtnode v,marker_type m) const  // return the list of in edges of v marked m
	{
	gdtedge e;
	gdt::gdtlist<gdtedge> real_edges;
	gdt::gdtlist<gdtedge>& in_edges = (*node_info)[v].in_edges;
	forall(e,in_edges)
		if(is_marked(e,m)) real_edges.append(e);
	return real_edges;
	}
	
	
	gdt::gdtlist<gdtedge>
	undi_graph::	
find_out_edges_marked_as 
	(gdtnode v,marker_type m) const // return the list of out edges of v marked m
	{
	gdtedge e;
	gdt::gdtlist<gdtedge> real_edges;
	gdt::gdtlist<gdtedge>& out_edges = (*node_info)[v].out_edges;
	forall(e,out_edges)
		if(is_marked(e,m)) real_edges.append(e);
	return real_edges;	
	}


	gdt::gdtlist<gdtedge>
	undi_graph::
edges_with_extremal_nodes
	(gdtnode v1, gdtnode v2) const
	{
	gdt::gdtlist<gdtedge> multiples;
	
	gdtedge e;
	
	//Modified by A.M.
	forall_edges_adjacent_node(e,v1,*this)
		if (opposite(v1,e) == v2) multiples.append(e);
		
	return multiples;
	}


	gdtnode 
	undi_graph::
node_between_adj_edges
	(gdtedge e1, gdtedge e2)   const
	{
	gdtnode v;

	if (((v = source(e1)) == source(e2)) || (v == target(e2)))
		return(v);
	else if (((v = target(e1)) == source(e2)) || (v == target(e2)))
			return(v);
	else return(NULL_NODE);
	}

	
	
	bool
	undi_graph::
simple_DFS 
	(gdtnode v, gdt::gdtnode_array<bool>& reached, gdt::gdtnode_array<gdtedge>& father, gdt::gdtlist<gdtnode>& order) const
	{
	//cout << "\nSTART DFS VISIT FROM NODE " << id(v) << "\n" << flush;
	bool cyclic = false;
	gdt::gdtstack<gdtnode> S;
	gdtnode w;
	gdtedge e;
	gdt::gdtnode_array<gdtedge> iterator (nodes_and_edges(), NULL_EDGE);
	
	if (!reached[v])
		{
		reached[v] = true;
		father[v] = NULL_EDGE;
		iterator[v] = first_adj_edge(v);
		S.push(v);
		order.append (v);
		//cout << "\nVISITED NODE " << id(v) << flush;
		}
		
	while (!S.empty())
		{
		v = S.top();
		e = iterator[v];
		while (e != NULL_EDGE)
			{
			w = opposite(v,e);
			if (!reached[w])
				{
				reached[w]  = true;
				father[w]   = e;
				iterator[w] = first_adj_edge(w);
				S.push(w);
				order.append (w);
				//cout << "\nVISITED NODE " << id(w) << flush;
				iterator[v] = adj_succ (e,v);
				v = w;
				e = iterator[v];
				}
			else
				{
				if (e != father[v]) cyclic = true;
				iterator[v] = adj_succ (e,v);
				e = iterator[v];
				}
			}
		S.pop();
		}
	return cyclic;
	}
	
	
	
	bool
	undi_graph::
simple_BFS
	(gdtnode v, gdt::gdtnode_array<bool>& reached, gdt::gdtnode_array<gdtedge>& father, gdt::gdtnode_array<int>& dist, gdt::gdtlist<gdtnode>& order) const
	{
	//cout << "\nSTART DFS VISIT FROM NODE " << id(v) << "\n" << flush;
	bool cyclic = false;
	gdt::gdtqueue<gdtnode> Q;
	gdtnode w;
	gdtedge e;
	
	if (!reached[v])
		{
		reached[v] = true;
		father[v]  = NULL_EDGE;
		dist[v]	   = 0;
		Q.append(v);
		order.append (v);
		//cout << "\nVISITED NODE " << id(v) << flush;
		}
		
	while (!Q.empty())
		{
		v = Q.pop();
		forall_edges_adjacent_node (e,v,*this)
			{
			w = opposite(v,e);
			if (!reached[w])
				{
				reached[w] = true;
				father[w]  = e;
				dist[w] = dist[v] + 1;
				Q.append(w);
				order.append(w);
				//cout << "\nVISITED NODE " << id(w) << flush;
				}
			else if (e != father[v]) cyclic = true;
			}
		}
	return cyclic;
	}
	

	
	bool
	undi_graph::
complex_DFS 
	(
	gdtnode v, 
	gdtedge e,
	gdt::gdtnode_array<bool>& reached,
	gdt::gdtnode_array<gdtedge>& father,
	gdt::gdtlist<gdtnode>& order,
	gdt::gdtnode_array<gdtnode>& lowpt1,
	gdt::gdtnode_array<gdtnode>& lowpt2,
	gdt::gdtnode_array<int>& num_disc,
	bool& root_is_articulated
	) const
	{
	//cout << "\nSTART VISIT FROM NODE " << id(v) << "\n" << flush;
	
	bool cyclic = false;
	gdt::gdtnode_array<int>  num_visit  (nodes_and_edges(), 0);
	gdt::gdtnode_array<gdtedge> start_edge (nodes_and_edges(), NULL_EDGE);
	gdt::gdtnode_array<gdtedge> iterator   (nodes_and_edges(), NULL_EDGE);
	gdt::gdtstack<gdtnode> S;
	gdtnode w = NULL_NODE, 
	     root = NULL_NODE;
	int count = 1;
    	int root_visited = 0;
	
	// Initialization
	//
	forall_nodes (w, nodes_and_edges())
		{
		if (w == v) start_edge[w] = e;
		else	    start_edge[w] = first_adj_edge(w);
		iterator [w] = start_edge[w];
		num_disc [w] = 1;
		lowpt1   [w] = w;
		lowpt2   [w] = w;
		}
		
	if (!reached[v])
		{
        	root = v;
		reached[v] = true;
		father [v] = NULL_EDGE;
		S.push(v);
		order.append(v);
		num_visit[v] = count;
		count ++;
		//cout << "\nVISITED NODE " << id(v) << flush;
		}
		
	while (!S.empty())
		{
		v = S.top();
		e = iterator[v];
		while (e != NULL_EDGE)
			{
			w = opposite(v,e);
			if (!reached[w])
				{
				reached[w] = true;
				father [w] = e;
				S.push(w);
				order.append(w);
				num_visit[w] = count;
				count ++;
				//cout << "\nVISITED NODE " << id(w) << flush;
				iterator[v] = cyclic_adj_succ (e,v);
				if (iterator[v] == start_edge[v]) iterator[v] = NULL_EDGE;
				v = w;
				e = iterator[v];
				}
			else
				{
				if (e != father[v])  // backward gdtedge
					{
					cyclic = true;
					if (w != lowpt1[v])
						{
						if (w == compare (w,lowpt1[v],num_visit,MIN))
							{
							lowpt2[v] = lowpt1[v];
							lowpt1[v] = w;
							}
						else if (w == compare (w,lowpt1[v],num_visit,MAX))
							lowpt2[v] = compare (lowpt2[v],w,num_visit,MIN);
						}
					}
				iterator[v] = cyclic_adj_succ (e,v);
				if (iterator[v] == start_edge[v]) iterator[v] = NULL_EDGE;
				e = iterator[v];
				}
			}
		w = S.pop();
		if (num_visit[w] != 1)
			{
			v = opposite(w,father[w]); // father gdtnode of w
			if (lowpt1[w] == lowpt1[v])
				lowpt2[v] = compare (lowpt2[v],lowpt2[w],num_visit,MIN);
			else
			if (lowpt1[w] == compare (lowpt1[w],lowpt1[v],num_visit,MIN))
				{
				lowpt2[v] = compare (lowpt1[v],lowpt2[w],num_visit,MIN);
				lowpt1[v] = lowpt1[w];
				}
			else	lowpt2[v] = compare (lowpt2[v],lowpt1[w],num_visit,MIN);
			num_disc[v] += num_disc[w];
            		if(v == root) ++root_visited; // number of times backtrak to root
			}
		}
	root_is_articulated = (root_visited!=1);
	return cyclic;
	}



	bool
	undi_graph::
spanning_tree
	(gdt::gdtlist<gdtedge>& spanned_edges, gdt::gdtlist<gdtedge>& unspanned_edges, gdtnode start_node) const
	{
	if( start_node == NULL_NODE ) start_node = first_node();   
	if( start_node == NULL_NODE ) return false;
	if( !spanned_edges.empty() ) 
		gdt_error("spanned_edges list not empty");
	if( !unspanned_edges.empty() ) 
		gdt_error("unspanned_edges list not empty");

	gdt::gdtnode_array<bool> reached (nodes_and_edges(), false);
	gdt::gdtnode_array<gdtedge> father  (nodes_and_edges(), NULL_EDGE);
	gdt::gdtlist<gdtnode> 	 order;

	simple_DFS( start_node, reached, father, order );
	
	gdt::gdtedge_array<bool> spanned_edges_array(nodes_and_edges(), false);

	gdtnode v;
	forall_nodes( v, nodes_and_edges() ) 
		if (father[v])
			spanned_edges_array[father[v]] = true;

	gdtedge e;
	forall_edges( e, nodes_and_edges() )
		if ( spanned_edges_array[e] )	spanned_edges.append( e );
		else				unspanned_edges.append( e );

	return true;
	}



	void
	undi_graph:: 
visit_from_node_through_not_marked_nodes 
	(gdtnode v, gdt::gdtnode_array<bool>& marked_node, gdt::gdtlist<gdtedge>& visited_edges) const
	{
	gdtnode w;
	gdtedge e;
	gdt::gdtqueue<gdtnode> Q;
	gdt::gdtedge_array<bool> edge_already_insert (nodes_and_edges(),false);
	
	Q.append (v);
	while (!Q.empty())
		{
		v = Q.pop();
		marked_node[v] = true;
		forall_edges_adjacent_node (e,v,*this)
			{
			if (!edge_already_insert[e])
				{
				visited_edges.append(e);
				edge_already_insert[e] = true;
				}
			w = opposite (v,e);
			if (!marked_node[w]) Q.append(w);
			}
		}
	}




	gdtnode 
	undi_graph::
compare
	(gdtnode v, gdtnode w, gdt::gdtnode_array<int>& f, compare_option co) const 
	{
	if (co == MIN)
		if (f[v] < f[w]) return v;
		else		 return w;
	else
		if (f[v] > f[w]) return v;
		else		 return w;
	}




	gdtnode
	undi_graph::
find_node
	(int ref_id) const
	{
	return (*node_with_id)[ref_id];
	}



	gdtnode 
	undi_graph::
first_node
	() const
	{
	/*
	gdt::list_item it = nodes->first();
	gdtnode n = it ? nodes->inf(it) : NULL;
	assert(g->first_node() == (n ? n->lnode : NULL));
	return g->first_node();
	*/
	gdt::list_item it = nodes->first();
	gdtnode n = it ? nodes->inf(it) : NULL;
	//Modified by A.M. 10/2002
	return n;
	//return n ? n->lnode : NULL;
	}
		
	gdtnode
	undi_graph::
last_node
	() const
	{
	//
	//gdt::list_item it = nodes->last();
	//gdtnode n = it ? nodes->inf(it) : NULL;
	//assert(g->last_node() == (n ? n->lnode : NULL));
	//
	//return g->last_node();
	gdt::list_item it = nodes->last();
	gdtnode n = it ? nodes->inf(it) : NULL;
	//Modified by A.M. 10/2002
	return n;
	//return n ? n->lnode : NULL;
	}
		
	gdtnode
	undi_graph::
succ_node
	(gdtnode v) const
	{
	//Modified by A.M. 2/2002	
	gdtnode sv = NULL;
	if (v) {
		//gdtnode n = (*node_info)[v].gnode;
		gdt::list_item it = v->pos_in_nodes;
		gdt::list_item itsucc = nodes->succ(it);
		if (itsucc) {
			sv = nodes->inf(itsucc);
			//u = nu->lnode;	
		}
 	}
	return sv;
	  
	/*	
	if (v) {
		gdtnode n = (*node_info)[v].gnode;
		assert(n);
		gdt::list_item it = n->pos_in_nodes;
		assert(it);
		if (it != nodes->last()) {			
			n = nodes->inf(nodes->succ(it));
			assert(g->succ_node(v) == n->lnode);
			
		}	
		else return NULL;		
	}
	else 
		return NULL;
	//
	return g->succ_node(v);
	*/
	}
	


	
	gdtnode
	undi_graph::
pred_node
	(gdtnode v) const
	{
	//Modified by A.M. 10/2002
	gdtnode pv = NULL;
	if (v) {
		//gdtnode n = (*node_info)[v].gnode;
		gdt::list_item it = v->pos_in_nodes;
		gdt::list_item itpred = nodes->pred(it);
		if (itpred) {
			pv = nodes->inf(itpred);
		}
 	}
	return pv;
	
	/*
	gdtnode n = (*node_info)[v].gnode;
	gdt::list_item it = (n ? n->pos_in_nodes : NULL);
	assert(it);
	if (it != nodes->first()) {
		n = nodes->inf(nodes->pred(it));
		assert(g->pred_node(v) == n->lnode);
	}
	//
	return g->pred_node(v);
	*/
	}



/* MOVED INLINE
// ------------------	
	gdtnode
	undi_graph::
source
	(gdtedge e) const
	{
	return (*edge_info)[e].source;
	}
	
	
	
	
	gdtnode
	undi_graph::
target
	(gdtedge e) const
	{
	return (*edge_info)[e].target;
	}
	
	
	

	gdtnode
	undi_graph::
opposite
	(gdtnode v, gdtedge e) const
	{
	return g->opposite(v,e);
	}



	
	gdtnode
	undi_graph::
start
	(gdtedge e) const
	{
	return (*edge_info)[e].start;
	}
--------------------------------------*/
	
	
	
	gdtnode
	undi_graph::
stop
	(gdtedge e) const
	{
	gdtnode v;
	if (edge_is_directed(e)) v = opposite(start(e),e);
	else v = NULL_NODE;
	return v;
	}
	
	
	
	
	gdtedge
	undi_graph::
find_edge
	(int ref_id) const
	{
	return (*edge_with_id)[ref_id];
	}
	

	
	
	gdtedge
	undi_graph::
find_edge
	(gdtnode v1, gdtnode v2) const
	{
 	gdtedge e;
	gdtnode n1;
	gdtnode n2;
	if (degree(v1)<degree(v2))
			{
			n1=v1;
			n2=v2;
			}
	else
		{
		n1=v2;
		n2=v1;
		//cout << "scambio" << endl;
		}
  forall_edges_adjacent_node (e,n1,*this)
  	if (opposite(n1,e)==n2)
  		return e;
  return NULL_EDGE;

	}




	gdtedge
	undi_graph::
first_edge
	() const
	{
	//
	gdt::list_item it = edges->first();
	gdtedge e = it ? edges->inf(it) : NULL;
	//Modified by A.M. 10/2002
	return e;
	//return (e ? e->ledge : NULL);
	//return g->first_edge();	
	}
	
	
	
	
	gdtedge
	undi_graph::
last_edge
	() const
	{
	//
	gdt::list_item it = edges->last();
	gdtedge e = it ? edges->inf(it) : NULL;
	//Modified by A.M. 10/2002
	return e;
	//return (e ? e->ledge : NULL);
	//return g->last_edge();
	}
	
	


	gdtedge 
	undi_graph::
succ_edge
	(gdtedge e) const
	{
	//Added by A.M. 10/2002
	//Nuova versione
	gdtedge se = NULL;
	if (e) {
		gdt::list_item it = e->pos_in_edges;
		gdt::list_item itsucc = edges->succ(it);
		if (itsucc) {
			se = edges->inf(itsucc);
		}
 	}
	return se;
	
	//Added A.M. 2/2002
	//Versione preliminare al distacco
	/*
	gdtedge f = NULL;
	if (e) {
		gdtedge d = (*edge_info)[e].gedge;
		gdt::list_item it = d->pos_in_edges;
		gdt::list_item itsucc = edges->succ(it);
		if (itsucc) {
			gdtedge df = edges->inf(itsucc);
			f = df->ledge;	
		}
 	}
	return f;
	*/
	
	
	
	//Vecchia versione
	/*
	if (e) {
		gdtedge n = (*edge_info)[e].gedge;
		assert(n);
		gdt::list_item it = n->pos_in_edges;
		assert(it);
		if (it != edges->last()) {
			n = edges->inf(edges->succ(it));
			assert(g->succ_edge(e) == n->ledge);
		}
	}
	return g->succ_edge(e);
	*/
	}
	
	
	
	
	gdtedge 
	undi_graph::
pred_edge
	(gdtedge e) const
	{
	//Modified by A.M. 2/2002
	gdtedge pe = NULL;
	if (e) {
		//gdtedge d = (*edge_info)[e].gedge;
		gdt::list_item it = e->pos_in_edges;
		gdt::list_item itpred = edges->pred(it);
		if (itpred) {
			pe = edges->inf(itpred);
			//f = df->ledge;	
		}
 	}
	return pe;
	/*
	gdtedge ge = (*edge_info)[e].gedge;
	gdt::list_item it = (ge ? ge->pos_in_edges : NULL);
	assert(it);
	ge = edges->inf(edges->pred(it));
	assert(g->pred_edge(e) == ge->ledge);
	//
	return g->pred_edge(e);
	*/
	}
	
	
	
	
	gdtedge
	undi_graph::
first_in_edge
	(gdtnode v) 
	{
	order_in_edges (v);
	gdt::gdtlist<gdtedge>& in_edges = (*node_info)[v].in_edges;
	if (!in_edges.empty()) return in_edges.head();
	else		       return NULL_EDGE;
	}	

	


	gdtedge
	undi_graph::
first_out_edge
	(gdtnode v)
	{
	order_out_edges (v);
	gdt::gdtlist<gdtedge>& out_edges = (*node_info)[v].out_edges;
	if (!out_edges.empty()) return out_edges.head();
	else		        return NULL_EDGE;
	}	



	
	gdtedge 
	undi_graph::
first_adj_edge
	(gdtnode v) const
	{
	if ((*node_info)[v].adj_edges.empty()) return NULL_EDGE;
	else 				       return (*node_info)[v].adj_edges.head();
	}
	
	
	
	
	gdtedge
	undi_graph::
last_in_edge
	(gdtnode v)
	{
	order_in_edges (v);
	gdt::gdtlist<gdtedge>& in_edges = (*node_info)[v].in_edges;
	if (in_edges.empty()) return NULL_EDGE;
	else                  return in_edges.tail();
	}	


	
	
	gdtedge
	undi_graph::
last_out_edge
	(gdtnode v)
	{
	order_out_edges (v);
	gdt::gdtlist<gdtedge>& out_edges = (*node_info)[v].out_edges;
	if (out_edges.empty()) return NULL_EDGE;
	else                   return out_edges.tail();
	}	
	
	
	
	
	gdtedge 
	undi_graph::
last_adj_edge
	(gdtnode v) const
	{
	if ((*node_info)[v].adj_edges.size() == 0) return NULL_EDGE;
	return (*node_info)[v].adj_edges.tail();
	}


	
	
	gdtedge
	undi_graph::
in_succ
	(gdtedge e, gdtnode v) 
	{
	order_in_edges (v);
	gdt::gdtlist<gdtedge>& in_edges = (*node_info)[v].in_edges;
	gdt::list_item pos = (*edge_info)[e].pos_into_in_edges;
	pos = in_edges.succ(pos);
	if (pos) return in_edges.contents(pos);
	else return NULL_EDGE;
	}


	
	
	gdtedge
	undi_graph::
out_succ
	(gdtedge e, gdtnode v)
	{
	order_out_edges (v);
	gdt::gdtlist<gdtedge>& out_edges = (*node_info)[v].out_edges;
	gdt::list_item pos = (*edge_info)[e].pos_into_out_edges;
	pos = out_edges.succ(pos);
	if (pos) return out_edges.contents(pos);
	else return NULL_EDGE;
	}


	
	
	gdtedge 
	undi_graph::
adj_succ
	(gdtedge e, gdtnode v) const
	{
	gdt::gdtlist<gdtedge>& adj_edges =(*node_info)[v].adj_edges;
	gdt::list_item pos = pos_of_edge_in_adj_edges_of_node (e,v);
	pos = adj_edges.succ(pos);
	if (pos) return adj_edges.contents(pos);
	else return NULL_EDGE;
	}
	
	
	

	gdtedge
	undi_graph::
in_pred
	(gdtedge e, gdtnode v) 
	{
	order_in_edges (v);
	gdt::gdtlist<gdtedge>& in_edges = (*node_info)[v].in_edges;
	gdt::list_item pos = (*edge_info)[e].pos_into_in_edges;
	pos = in_edges.pred(pos);
	if (pos) return in_edges.contents(pos);
	else return NULL_EDGE;
	}


	
		
	gdtedge
	undi_graph::
out_pred
	(gdtedge e, gdtnode v)
	{
	order_out_edges (v);
	gdt::gdtlist<gdtedge>& out_edges = (*node_info)[v].out_edges;
	gdt::list_item pos = (*edge_info)[e].pos_into_out_edges;
	pos = out_edges.pred(pos);
	if (pos) return out_edges.contents(pos);
	else return NULL_EDGE;
	}

	
	
	
	gdtedge 
	undi_graph::
adj_pred
	(gdtedge e, gdtnode v) const
	{
	gdt::gdtlist<gdtedge>& adj_edges =(*node_info)[v].adj_edges;
	gdt::list_item pos = pos_of_edge_in_adj_edges_of_node (e,v);
	pos = adj_edges.pred(pos);
	if (pos) return adj_edges.contents(pos);
	else return NULL_EDGE;
	}



		
	gdtedge
	undi_graph::
cyclic_in_pred
	(gdtedge e, gdtnode v) 
	{
	order_in_edges (v);
	if (!e) return last_in_edge(v);
	gdt::gdtlist<gdtedge>& in_edges = (*node_info)[v].in_edges;
	gdt::list_item pos = (*edge_info)[e].pos_into_in_edges;
	return in_edges.contents(in_edges.cyclic_pred(pos));
	}

	
	
		
	gdtedge
	undi_graph::
cyclic_out_pred
	(gdtedge e, gdtnode v) 
	{
	order_out_edges (v);
	if (!e) return last_out_edge(v);
	gdt::gdtlist<gdtedge>& out_edges = (*node_info)[v].out_edges;
	gdt::list_item pos = (*edge_info)[e].pos_into_out_edges;
	return out_edges.contents(out_edges.cyclic_pred(pos));
	}




	gdtedge 
	undi_graph::
cyclic_adj_pred
	(gdtedge e, gdtnode v) const
	{
	if (!e) return last_adj_edge(v);
	gdt::gdtlist<gdtedge>& L =(*node_info)[v].adj_edges;
	gdt::list_item p = pos_of_edge_in_adj_edges_of_node (e,v);
	return L.contents(L.cyclic_pred(p));
	}
	
	
	
	
	gdtedge
	undi_graph::
cyclic_in_succ
	(gdtedge e, gdtnode v) 
	{
	order_in_edges (v);
	if (!e) return first_in_edge(v);
	gdt::gdtlist<gdtedge>& in_edges = (*node_info)[v].in_edges;
	gdt::list_item pos = (*edge_info)[e].pos_into_in_edges;
	return in_edges.contents(in_edges.cyclic_succ(pos));
	}

	
	
			
	gdtedge
	undi_graph::
cyclic_out_succ
	(gdtedge e, gdtnode v) 
	{
	order_out_edges (v);
	if (!e) return first_out_edge(v);
	gdt::gdtlist<gdtedge>& out_edges = (*node_info)[v].out_edges;
	gdt::list_item pos = (*edge_info)[e].pos_into_out_edges;
	return out_edges.contents(out_edges.cyclic_succ(pos));
	}


	
		
	gdtedge 
	undi_graph::
cyclic_adj_succ
	(gdtedge e, gdtnode v) const
	{
	if (!e) return first_adj_edge(v);
	gdt::gdtlist<gdtedge>& L =(*node_info)[v].adj_edges;
	gdt::list_item p = pos_of_edge_in_adj_edges_of_node (e,v);
	return L.contents(L.cyclic_succ(p));
	}



	
	gdtedge
	undi_graph::
find_directed_edge 
	(gdtnode v, gdtnode w) 
	{
	// return an gdtedge e such that start(e)=v and stop(e)=w;
	// if e not existes it is retuned null. 
	//
	gdtedge e;
	forall_edges_entering_node (e,w,*this)
		if (start(e) == v)  return e;
	return NULL_EDGE;
	}


	
	gdtedge
	undi_graph::
reverse_of_edge
	(gdtedge e) 
	{
	// return an gdtedge einv such that start(einv)=stop(e) and stop(einv)=start(e)
	// if it existes, else return null.
	//
	gdtedge einv = NULL_EDGE;
	forall_edges_entering_node (einv,start(e),*this)
		if (start(einv) == stop(e)) return einv;
	return einv;
	}


	
	
	gdt::gdtlist<gdtedge> 
	undi_graph::
in_edges
	(gdtnode v)
	{
	order_in_edges (v);
	gdt::gdtlist<gdtedge>& in_edges = (*node_info)[v].in_edges;
	return in_edges;
	}


	
	
	gdt::gdtlist<gdtedge> 
	undi_graph::
out_edges
	(gdtnode v)
	{
	order_out_edges (v);
	gdt::gdtlist<gdtedge>& out_edges = (*node_info)[v].out_edges;
	return out_edges;
	}


	
	gdt::gdtlist<gdtedge> 
	undi_graph::
adj_edges
	(gdtnode v) const
	{
	return (*node_info)[v].adj_edges;
	}
	
	
	
	
	gdt::gdtlist<gdtnode> 
	undi_graph::
adj_nodes
	(gdtnode v) const
	{
	gdtedge e;
	gdt::gdtlist<gdtnode> L;
	forall_edges_adjacent_node (e,v,*this) L.append(opposite(v,e));
	return L;
	}


	const gdt::gdtlist<gdtedge>& 
	undi_graph::
all_edges
	() const
	{
	//Added by A.M. 10/2002
	return *edges;
	//Old code
	//return g->all_edges();
	}
	

	const gdt::gdtlist<gdtnode>&
	undi_graph::
all_nodes
	() const
	{
	//Added by A.M. 10/2002
	return *nodes;
	//Old code
	//return g->all_nodes();
	}
			
	gdt::list_item
	undi_graph::
pos_of_edge_in_adj_edges_of_node
	(gdtedge e, gdtnode v) const
	{
	gdt::list_item li = NULL;	
	if      (v==source(e)) 
		li = (*edge_info)[e].pos_into_source_adj_edges;
	else if (v==target(e))
		li = (*edge_info)[e].pos_into_target_adj_edges;
	else 
		gdt_error("invalid (gdtnode,gdtedge) pair");

	return li; 
	}
	


		
	constraint 
	undi_graph::
find_constraint 
	(int id) const
	{
	return (*constraint_with_id)[id];
	}



	constraint 
	undi_graph::
first_constraint
	() const
	{
	if(constraints->empty()) return NULL_CONSTRAINT;
	else 			 return constraints->head();
	}


	constraint 
	undi_graph::
last_constraint
	() const
	{
	if(constraints->empty()) return NULL_CONSTRAINT;
	else 			 return constraints->tail();
	}



	constraint 
	undi_graph::
succ_constraint
	(constraint c) const
	{
	gdt::list_item li 	  = c->get_pos_in_owner_graph_list();
	gdt::list_item next_li = constraints->succ(li);
	
	if (next_li)	return constraints->inf(next_li);
	else		return NULL_CONSTRAINT;
	}


	constraint 
	undi_graph::
pred_constraint
	(constraint c) const
	{
	gdt::list_item li 	  = c->get_pos_in_owner_graph_list();
	gdt::list_item pred_li = constraints->pred(li);
	
	if (pred_li)	return constraints->inf(pred_li);
	else		return NULL_CONSTRAINT;
	}
	


	const gdt::gdtlist<constraint>& 
	undi_graph::
all_constraints
	() const
	{
	return *constraints;
	}



	gdt::gdtlist<constraint>
	undi_graph:: 
constraints_on_edge 
	(gdtedge e) 
	{
	gdt::gdtlist<constraint> Le = all_constraints(e); // execute a copy
	return Le;
	}
	
	
		
	gdt::gdtlist<constraint> 
	undi_graph::
constraints_on_node
	(gdtnode v) 
	{
	gdt::gdtlist<constraint> Lv = all_constraints(v); // execute a copy
	return Lv;
	} 


	
	gdt::gdtlist<marker_type>
	undi_graph::
markers
	(gdtnode v) const
	{
	return (*node_info)[v].markers;
	}

	
	gdt::gdtlist<marker_type>
	undi_graph::
markers
	(gdtedge e) const
	{
	return (*edge_info)[e].markers;
	}
	


 	// --------------------
	// Translate operations
	// --------------------
	
	
	gdtnode
	undi_graph::
corresponding_node
	(gdtnode v, const undi_graph& ug) const
	{
	return (v) ? find_node(ug.id(v)):NULL_NODE;
	}


	
	gdtedge
	undi_graph::
corresponding_edge
	(gdtedge e, const undi_graph& ug) const
	{
	return (e) ? find_edge(ug.id(e)):NULL_EDGE;
	}


	
	constraint
	undi_graph::
corresponding_constraint
	(constraint c, const undi_graph& ug) const
	{
	return (c) ? find_constraint(ug.id(c)):NULL_CONSTRAINT;
	}


	void
	undi_graph::
build_mapping_node_to_node_with_undi_graph
	(const undi_graph& ug, gdt::gdtnode_map<gdtnode>& node_corr_in_ug) const
	{
	int dim;
	int i; 

	if (get_max_node_id() > ug.get_max_node_id()) dim = get_max_node_id() + 1;
	else					      dim = ug.get_max_node_id() + 1;
	
	gdt::gdtarray<gdtnode> a1 (dim);
	gdt::gdtarray<gdtnode> a2 (dim);
	gdtnode v1,v2;
	
	for (i = 0; i <= a1.high(); i++) a1[i] = NULL_NODE;
	for (i = 0; i <= a2.high(); i++) a2[i] = NULL_NODE;

	forall_nodes (v1,*this) a1[id(v1)] = v1;
	forall_nodes (v2,ug) 	a2[ug.id(v2)] = v2;
	for (i = 0; i <= a1.high(); i++) 
		if (a1[i]) node_corr_in_ug[a1[i]] = a2[i];
	}
	
	
	
	void
	undi_graph::
build_mapping_edge_to_edge_with_undi_graph
	(const undi_graph& ug, gdt::gdtedge_map<gdtedge>& edge_corr_in_ug) const
	{
	int dim;
	int i;
	if (get_max_edge_id() > ug.get_max_edge_id()) dim = get_max_edge_id() + 1;
	else dim = ug.get_max_edge_id() + 1;
	
	gdt::gdtarray<gdtedge> a1 (dim);
	gdt::gdtarray<gdtedge> a2 (dim);
	gdtedge e1,e2;
	
	for (i = 0; i <= a1.high(); i++) a1[i] = NULL_EDGE;
	for (i = 0; i <= a2.high(); i++) a2[i] = NULL_EDGE;
	
	forall_edges (e1,*this) a1[id(e1)] = e1;
	forall_edges (e2,ug) 	a2[ug.id(e2)] = e2;
	for (i = 0; i <= a1.high(); i++) 
		if (a1[i]) edge_corr_in_ug[a1[i]] = a2[i];
	}
		
		


	// -----------------
	// Update operations
	// -----------------

	gdtnode
	undi_graph::
new_node
	(int new_id) 
	{
	  /*
	gdtnode v = g->new_node();
	assign_id (v, new_id);
	(*node_info)[v].markers.clear();
	  */

	//Modified by A.M. 10/2002
	gdtnode gv = new struct_gdtnode;
	assign_id (gv, new_id);
	(*node_info)[gv].markers.clear();
	gv->pos_in_nodes = nodes->append(gv);
	//(*node_info)[gv].gnode = gv;
	return gv;
	}



	gdtnode
	undi_graph::
new_node 
	(gdt::gdtlist<gdtnode> node_list, int new_id)
	{
	gdtnode n = new_node(new_id);          		// a new unconnected gdtnode
	gdtedge e = new_edge(n, node_list.pop());       	// first link
	
	while( !node_list.empty() )	e = new_edge(n, e, node_list.pop());
	
	/*
	
	    n1     n2
	     o-----o
	    / \   / \
	   /  ^\ /^  \
	  o   | n |   o n3
	  |   |   |   |
	  .  gdtedge |   .
	  .       |
	  .    new_edge
	
	*/
	
	return n;
	}



	gdtedge
	undi_graph::
new_edge
	(gdtnode v1, gdtnode v2, int new_id)
	{
	//cout << "\nundi_graph::new_edge(gdtnode,node) begin "  << flush;
	
	//Modified by A.M. 10/2002
	gdtedge ge = new struct_gdtedge;
	ge->pos_in_edges = edges->append(ge);	
	assign_id (ge, new_id);
	ge->plan_id=++max_edge_plan_id;
	struct_undi_edge_info& ei = (*edge_info)[ge];
	ei.source = v1;
	ei.target = v2;
	ei.start  = NULL_NODE;
	ei.pos_into_source_adj_edges = (*node_info)[v1].adj_edges.append(ge);
	if (v1 != v2) 
		ei.pos_into_target_adj_edges = (*node_info)[v2].adj_edges.append(ge);
	else	      
		ei.pos_into_target_adj_edges = (*edge_info)[ge].pos_into_source_adj_edges;
	(*edge_info)[ge].markers.clear();
	
	return ge;
	}
	
	
	
	gdtedge
	undi_graph::
new_edge
	(gdtnode v1, gdtedge e1, gdtnode v2, int new_id, int dir)
	{
	//cout << "\nundi_graph::new_edge(gdtnode,gdtedge,gdtnode) begin "  << endl << flush;	
	gdt::list_item pos_e1 = pos_of_edge_in_adj_edges_of_node(e1,v1);
	
	if (v1 == v2) gdt_error("v1 = v2 is not ready yet");	
	
	//Old code
	//if (g->is_directed())  e = g->new_edge(v1,v2);
	//else		       e = g->new_edge(v1,e1,v2,dir);	

	//Modified by A.M. 10/2002
	gdtedge ge = new struct_gdtedge;
	ge->pos_in_edges = edges->append(ge);
	
	assign_id (ge, new_id);
		
	struct_undi_edge_info& ei = (*edge_info)[ge];
	ei.source = v1;
	ei.target = v2;
	ei.start  = NULL_NODE;
	ei.pos_into_source_adj_edges = (*node_info)[v1].adj_edges.insert(ge,pos_e1,dir);
	ei.pos_into_target_adj_edges = (*node_info)[v2].adj_edges.append(ge);
	//cout << "\nundi_graph::new_edge(gdtnode,gdtedge,gdtnode,gdtedge) end " << flush;
	(*edge_info)[ge].markers.clear();	
	
	return ge;
	}
	


	gdtedge
	undi_graph::
new_edge
	(gdtnode v1, gdtedge e1, gdtnode v2, gdtedge e2, int new_id, int dir) 
	{
	//cout << "\nundi_graph::new_edge(gdtnode,gdtedge,gdtnode,gdtedge) begin "  << flush;
	//cout << "\nv1 = " << id(v1) << " e1 = " << id(e1) << " v2 = " << id(v2) << " e2 = " << id(e2) << flush;

	//Modified by A.M. 10/2002
	gdtedge ge = new struct_gdtedge;	
	ge->pos_in_edges = edges->append(ge);

	gdt::list_item pos_e1 = pos_of_edge_in_adj_edges_of_node(e1,v1);
	gdt::list_item pos_e2 = pos_of_edge_in_adj_edges_of_node(e2,v2);
	
	if (v1 == v2) gdt_error("v1 = v2 is not ready yet");
	
	//Old code
	//if (g->is_directed())  e = g->new_edge(v1,v2);
	//else		       e = g->new_edge(v1,e1,v2,e2,dir);

	assign_id (ge, new_id);

	struct_undi_edge_info& ei = (*edge_info)[ge];
	ei.source = v1;
	ei.target = v2;
	ei.start  = NULL_NODE;
	ei.pos_into_source_adj_edges = (*node_info)[v1].adj_edges.insert(ge,pos_e1,dir);
	ei.pos_into_target_adj_edges = (*node_info)[v2].adj_edges.insert(ge,pos_e2,dir);
	(*edge_info)[ge].markers.clear();
	
	
	return ge;
	//cout << "\nundi_graph::new_edge(gdtnode,gdtedge,gdtnode,gdtedge) end "  << flush;
	}



	constraint
	undi_graph::
new_constraint
	(constraint c)
	{
	return c->copy(*this);
	}

	
	
	constraint
	undi_graph::
new_constraint_uncrossable_edge
	(gdtedge e, int new_id)
	{
	constraint c = new struct_constraint_uncrossable_edge(e, *this);
	assign_id (c, new_id);
	return c;
	}
	
	
	
	constraint
	undi_graph::
new_constraint_number_of_bends_on_edge
	(gdtedge e, bend_constraint b, int new_id)
	{
	if (b == MINIMAL)
		gdt_error("MINIMAL is not a valid value"); 
	
	constraint c = new struct_constraint_number_of_bends_on_edge(e, b, *this);
	assign_id (c, new_id);
	return c;
	}

	
	constraint
	undi_graph::
new_constraint_bend_direction_on_edge
	(gdtedge e, gdtnode v, int new_id)
	{
	if (v != source(e) && v != target(e))
		gdt_error("invalid pair gdtedge, gdtnode"); 
		
	constraint c = new struct_constraint_bend_direction_on_edge(e, v, *this);
	assign_id (c, new_id);
	return c;
	}


	constraint
	undi_graph::
new_constraint_same_face_node
	(gdtnode n, int face_label, int new_id)
	{
	constraint c = new struct_constraint_same_face_node(n, *this, face_label);
	assign_id (c, new_id);
	return c;
	}



	gdt::gdtlist<constraint> 
	undi_graph::
new_constraint_same_face_node
	(gdt::gdtlist<gdtnode> Ln, int face_label)
	{
	gdt::gdtlist<constraint> Lc;
	constraint c;
	gdtnode n;
	forall(n,Ln)
		{
		c = new_constraint_same_face_node (n, face_label);
		Lc.append(c);
		}
	return Lc;
	}



	constraint
	undi_graph::
new_constraint_same_face_ordered_nodes
	(
	gdt::gdtlist<gdtnode>	Ln, 
	int 		new_id
	)
	{
	constraint c = new struct_constraint_same_face_ordered_nodes(Ln, *this);
	assign_id (c, new_id);
	return c;
	}


	constraint
	undi_graph::
new_constraint_node_width
	(gdtnode n, double x, int new_id)
	{
	constraint c;
	gdt::gdtlist<constraint> Lc = constraints_on_node(n);
	forall(c,Lc)
		{
		if (c->get_type() == NODE_WIDTH )
			{
			((constraint_node_width)c)->set_value(x);
			assign_id (c, new_id); 
			return c;
			}
		}
	c = new struct_constraint_node_width(n, x, *this);
	assign_id (c, new_id);
	//cout << "***********************" << endl;
	//cout << "DEBUG:" << endl;
	//cout << "c=" << c << endl;
	//cout << "***********************" << endl;
	return c;
	}


	constraint
	undi_graph::
new_constraint_node_height
	(gdtnode n, double x, int new_id)
	{
	constraint c;
	gdt::gdtlist<constraint> Lc = constraints_on_node(n);
	forall(c,Lc)
		{
		if (c->get_type() == NODE_HEIGHT )
			{
			((constraint_node_height)c)->set_value(x);
			assign_id (c, new_id); 
			return c;
			}
		}
	c = new struct_constraint_node_height(n, x, *this);
	assign_id (c, new_id);
	return c;
	}


	constraint
	undi_graph::
new_constraint_angle_sweep
	(gdtnode v, gdtedge e, angle_type alpha, int new_id)
	{
	if (v != source(e) && v != target(e))
		gdt_error("invalid pair gdtnode,gdtedge"); 
		
	constraint c = new struct_constraint_angle_sweep (v,e,alpha,*this);
	assign_id (c, new_id);
	return c;
	}
	

	constraint
	undi_graph::
new_constraint_edge_attachment_wrt_previous_corner
	(gdtnode v, gdtedge e, int value, int new_id)
	{
	if (v != source(e) && v != target(e))
		gdt_error("invalid pair gdtnode,gdtedge"); 
		
	constraint c = new struct_constraint_edge_attachment_wrt_previous_corner (v,e,value,*this);
	assign_id (c, new_id);
	return c;
	}
	
	constraint
	undi_graph::
new_constraint_min_tieing
	( int value, int new_id )
	{
	constraint c = new struct_constraint_min_tieing (value,*this);
	assign_id (c, new_id);
	return c;
	}
	
	void
	undi_graph::
del_node
	(gdtnode v)
	{
	update_constraints_after_del_node (v);	// update all constraints involving gdtnode v
	gdtedge e;
	gdt::gdtlist<gdtedge> L = adj_edges(v); 
	
	//forall (e,L) 
	//	del_edge(e);
	
	gdt::gdtstack<gdtedge> S;
	forall (e,L) 
		S.push(e);
	while (!S.empty())
		del_edge(S.pop());	
	
	//Modified by A.M. 18/10/2002	
	//gdtnode gn = (*node_info)[v].gnode;
	//nodes->del_item(gn->pos_in_nodes);
	//delete gn;
	nodes->del_item(v->pos_in_nodes);
	//
	
	(*node_with_id)[(*node_info)[v].id]=NULL_NODE; //Modificare in undefine
	node_info->undefine(v);
	//g->del_node(v);
	//Added by A.M. 18/10/2002
	delete v;
	}
	
	
	
	void
	undi_graph::
del_edge
	(gdtedge e)
	{
	update_constraints_after_del_edge (e);	// update all constraints involving gdtedge e
	remove_from_adj_edges(e);
	if (edge_is_directed(e))
		{
		remove_from_in_edges (e);
		remove_from_out_edges(e);
		mark_in_edges_as_not_ordered  (start(e));
		mark_out_edges_as_not_ordered (stop(e));
		}

	//Modified by A.M. 18/10/2002
	//gdtedge ge = (*edge_info)[e].gedge;
	//edges->del_item(ge->pos_in_edges);
	//delete ge;
	edges->del_item(e->pos_in_edges);
	//

	(*edge_with_id)[(*edge_info)[e].id] = NULL_EDGE;
	edge_info->undefine(e);
	//g->del_edge(e);

	//Added by A.M. 18/10/2002
	delete e;
	//
	}



	void
	undi_graph::
del_constraint
	(constraint c)
	{
	(*constraint_with_id)[id(c)] = NULL_CONSTRAINT;
	safe_delete(c);	// c destructor provides to remove c from constraint list of undi_graph
	}



	void
	undi_graph::
del_constraints_type
	(constraint_type ct)
	{
	constraint c;
	gdt::gdtlist<constraint> Lc = all_constraints(); 
	forall (c,Lc)
		if (type_of_constraint(c) == ct) del_constraint(c);
	}



	void
	undi_graph::
del_all_constraints
	()
	{
	constraint c;
	gdt::gdtlist<constraint> Lc = all_constraints(); 
	forall (c,Lc) del_constraint(c);
	}



	void
	undi_graph::
del_all_constraints_involving_edge 
	(gdtedge e)
	{
	constraint c;
	gdt::gdtlist<constraint> Le = constraints_on_edge(e);
	forall (c,Le) del_constraint(c);
	}
	
	
	
	void
	undi_graph::
del_all_constraints_involving_node
	(gdtnode v)
	{
	constraint c;
	gdt::gdtlist<constraint> Lv = constraints_on_node(v);
	forall (c,Lv) del_constraint(c);
	}



	void
	undi_graph::
del_constraints_type_involving_edge
	(constraint_type ct, gdtedge e)
	{
	constraint c;
	gdt::gdtlist<constraint> Le = constraints_on_edge(e);
	forall (c,Le)
		if (type_of_constraint(c) == ct) del_constraint(c);
	}

	
	
	void
	undi_graph::
del_constraints_type_involving_node
	(constraint_type ct, gdtnode v)
	{
	constraint c;
	gdt::gdtlist<constraint> Lv = constraints_on_node(v);
	forall (c,Lv)
		if (type_of_constraint(c) == ct) del_constraint(c);
	}



	void
	undi_graph::
clear
	()
	{
	local_renew();
	}



	void 
	undi_graph::
mirror 
	(undi_graph& ug)
	{
	gdt::gdtlist<gdtnode>* p;
	gdt::gdtlist<gdtedge>* p0;
	//graph*                            p1;
	gdt::gdtnode_map<struct_undi_node_info>*  p2;
	gdt::gdtedge_map<struct_undi_edge_info>*  p3;
	gdt::gdtlist<constraint>*		  p4;
	gdt::gdtmap<int,gdtnode>*			  p5;
	gdt::gdtmap<int,gdtedge>*			  p6;
	gdt::gdtmap<int,constraint>*              p7;
	int                               p8;
	int                               p9;
	int                               p10;
	bool				  p11;
	//
	//ug.local_get (p,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11);
	ug.local_get (p,p0,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11);
	//local_set    (p,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11);
	local_set    (p,p0,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11);
	
	// Now the owner graph of all
	// constraints is changed to *this
	
	constraint c;
	forall_constraints (c,*this)
		c -> set_owner_graph(*this);
	}
	


	void
	undi_graph::
forget 
	()
	{
	local_new ();
	}
	
		
	
	void
	undi_graph::
assign_id
	(gdtnode v, int new_id)
	{
	int old_id = (*node_info)[v].id;
	if (new_id <  NULL_ID)  gdt_error("illegal id."); 
	if (old_id != NULL_ID) (*node_with_id)[old_id]=NULL_NODE;  // In case v is already appended to the gdtnode list
	if (new_id == AUTO_ID) new_id = generate_node_id();	
	else if (new_id > max_node_id) max_node_id = new_id;
	else if (find_node(new_id)) gdt_error("duplicate id."); 
	(*node_with_id)[new_id]=v;
	(*node_info)[v].id = new_id;
	}



	void
	undi_graph::
assign_id
	(gdtedge e, int new_id)
	{	
	int old_id = (*edge_info)[e].id;	
	if (new_id <  NULL_ID)  
		gdt_error("illegal id."); 
	if (old_id != NULL_ID) {
		(*edge_with_id)[old_id]=NULL_EDGE;  // In case e is already appended to the gdtedge list
	}
	if (new_id == AUTO_ID) new_id = generate_edge_id();
	else if (new_id > max_edge_id) max_edge_id = new_id;
	else if (find_edge(new_id)) gdt_error("duplicate id."); 	
	(*edge_with_id)[new_id]=e;
	(*edge_info)[e].id = new_id;	
	}



	void
	undi_graph::
assign_id
	(constraint c, int new_id)
	{
	int old_id = c->get_id();
	if (new_id <  NULL_ID)  gdt_error("illegal id."); 
	if (old_id != NULL_ID) (*constraint_with_id)[old_id]=NULL_CONSTRAINT;  // In case c is already appended to the constraint list
	if (new_id == AUTO_ID) new_id = generate_constraint_id();
	else if (new_id > max_constraint_id) max_constraint_id = new_id;
	else if (find_constraint(new_id)) gdt_error("duplicate id."); 
	(*constraint_with_id)[new_id]=c;
	c->set_id(new_id);
	}



	void
	undi_graph::
mark  
	(gdtnode v, marker_type m)
	{
	(*node_info)[v].markers.append(m);
	}
	
	
	
	void
	undi_graph::
mark
	(gdtedge e, marker_type m)
	{
	(*edge_info)[e].markers.append(m);
	}
	
	
	
	void
	undi_graph::
mark
	(gdtnode v, gdt::gdtlist<marker_type> ml)
	{
	marker_type m;
	forall (m,ml)	mark(v,m);
	}
	
	
	
	void
	undi_graph::
mark
	(gdtedge e, gdt::gdtlist<marker_type> ml)
	{
	marker_type m;
	forall (m,ml)	mark(e,m);
	}
	
	
	
	void
	undi_graph::
unmark
	(gdtnode v, marker_type m)
	{
	gdt::list_item mp;
	gdt::gdtlist<marker_type>& L = (*node_info)[v].markers;
	forall_items(mp, L)
		if (L.contents(mp) == m) break;		
	L.del_item(mp);
	}
	
	
	
	void
	undi_graph::
unmark
	(gdtedge e, marker_type m)
	{
	gdt::list_item mp;
	gdt::gdtlist<marker_type>& L = (*edge_info)[e].markers;
	forall_items(mp, L)
		if (L.contents(mp) == m) break;		
	L.del_item(mp);
	}



	void
	undi_graph::
unmark_all
	(gdtnode v)
	{
	gdt::gdtlist<marker_type>& L = (*node_info)[v].markers;
	L.clear();
	}



	void
	undi_graph::
unmark_all
	(gdtedge e)
	{
	gdt::gdtlist<marker_type>& L = (*edge_info)[e].markers;
	L.clear();
	}



	void
	undi_graph::
make_embedding_as
	(const undi_graph& ug)
	{
	//cout << "\nundi_graph::make_embedding_as" << flush;
	gdtnode v, ug_v;
	gdtedge e, ug_e;
	gdt::gdtlist<gdtedge> ug_v_adj;
	gdt::list_item  pos, ug_pos;
	gdt::gdtnode_map<gdtnode> ug_node_corr (nodes_and_edges(), NULL_NODE);
	gdt::gdtedge_map<gdtedge> edge_corr (ug.nodes_and_edges(), NULL_EDGE);


	// build the gdtnode- and gdtedge-mappings from *this to ug
	//
	build_mapping_node_to_node_with_undi_graph    (ug,ug_node_corr);
	ug.build_mapping_edge_to_edge_with_undi_graph (*this,edge_corr);

	forall_nodes (v,*this)
		{
		struct_undi_node_info& ni = (*node_info)[v];
		ug_v = ug_node_corr[v];
		if (!ug_v) gdt_error("corresponding gdtnode not found");
		ug_v_adj = ug.adj_edges(ug_v);

		//cout << "\n" << id(v) << ":";

		// clear the adjacent-list of the gdtnode v
		//
		ni.adj_edges.clear();

		// order the v-adj_edges as ug_v-adj_edges;
		//
		ug_pos = ug_v_adj.first();
		//cout << "\n" << flush;
		while (ug_pos)
			{
			ug_e = ug_v_adj.inf(ug_pos);
			e    = edge_corr[ug_e];
			assert(e != NULL);
			if (!ug_v) gdt_error("corresponding gdtedge not found");
			pos = ni.adj_edges.append(e);
			//cout << id(e) << " ";
			if      (v == source(e)) (*edge_info)[e].pos_into_source_adj_edges = pos;
			else if (v == target(e)) (*edge_info)[e].pos_into_target_adj_edges = pos;
			else {
				//cout << "e = " << e << " " << id(e) << endl;
				//cout << "source(e) = " << source(e) << " " << id(source(e)) << endl;
				//cout << "target(e) = " << target(e) << " " << id(target(e)) << endl;
				//cout << "ug_e = " << ug_e << " " << ug.id(ug_e) << endl;
				//cout << "source(ug_e) = " << ug.source(ug_e) << " " << ug.id(ug.source(ug_e)) << endl;
				//cout << "target(ug_e) = " << ug.target(ug_e) << " " << ug.id(ug.target(ug_e)) << endl;
				//cout << "v = " << v << " " << id(v) << endl;
				//cout << "ug_v = " << ug_v << " " << ug.id(ug_v) << endl;
				gdt_error("no source or target gdtnode of gdtedge");
			}
			ug_pos = ug_v_adj.succ(ug_pos);
			}
		// set as false the boolean variable in/out_edges_are_ordered,
		// these lists will be updated in the next access operation
		//
		ni.in_edges_are_ordered  = false;
		ni.out_edges_are_ordered = false;
		}
	//cout << "\nNOW THE UNDI_GRAPH_EMBEDDING IS THE FOLLOWING: " << flush;
	//print(COMPLETE);
	//cout << flush;
	}



	void
	undi_graph::
make_embedding_cand
	(gdtnode v)
	{
	if (!is_candidate(v))
		{
		//cout << "\n** make embedding candidate for the gdtnode " << id(v) << "(" << v << ")" << flush;

		struct_undi_node_info& vi = (*node_info)[v];
		gdtedge e;
		gdt::list_item pos;
		gdt::gdtlist<gdtedge> in,	 // directed in  edges of v
			   out,  // directed out edges of v
		   	und,  // undirected edges of v
		   	cand; // candidate adj-edges list of v

		//Modified by A.M.
		forall_edges_adjacent_node(e,v,*this)
			{
			if (!start(e)) und.append(e);
			else
				if (start(e) == v) out.append(e);
				else		    in.append(e);
			}

		cand.conc(in);
		cand.conc(out);
		cand.conc(und);

		//cout << "\n** cand list of gdtnode" << id(v) << " will be: " << flush;
		//forall (e,cand) cout << id(e) << " " << flush;

		vi.adj_edges.clear();

		forall (e,cand)
			{
			pos = vi.adj_edges.append(e);
			//cout << "\n** appended gdtedge " << id(e) << "(" << e << ")" << " in new pos." << pos << flush;
			if      (v == source(e)) 
				{
				(*edge_info)[e].pos_into_source_adj_edges = pos;
				//cout << "\ncase source, new pos." << pos << flush;
				}
			else if (v == target(e)) 
				{
				(*edge_info)[e].pos_into_target_adj_edges = pos; 
				//cout << "\ncase target, new pos." << pos << flush;
				}
			else gdt_error("no source or target gdtnode of gdtedge");
			}
		// set as false the boolean variable in/out_edges_are_ordered,
		// these lists will be updated in the next access operation
		//
		vi.in_edges_are_ordered  = false;
		vi.out_edges_are_ordered = false;	

		//cout << "\n" << id(v) << ": " << flush;
		//forall_edges_adjacent_node (e,v,*this) cout << id(e) << " " << flush;
		//cout << "\n" << flush;
		}
	}
	


	bool
	undi_graph::
make_embedding_planar()
	{
	#ifdef DEBUG
	//cout << "Start make_embedding_planar\t" << "nodes: "<< number_of_nodes() << "\tedges: " <<  number_of_edges() << endl;
	#endif
	undi_graph ug (*this);  // making a copy of *this because we don't want to change
	                        // the circular order around nodes of edges if the graph
	                        // is not actually a plane graph.

	gdt::gdtlist<gdtnode> dummy_nodes = ug.make_simple ();  // to handle multiple edges we introduce
	                                             // dummy nodes (we will remove them after
	                                             // the test)

	if (!ug.make_embedding_planar_boyer()) return false;  // embedding of *this is unchanged

	gdtedge e1,e2,e;
	gdtnode v1,v2,v;
	int old_id;
	
	forall (v,dummy_nodes)     // removing all dummy nodes, now
		{
		e1 = ug.first_adj_edge (v);
		e2 = ug.last_adj_edge  (v);
		v1 = ug.opposite(v,e1);
		v2 = ug.opposite(v,e2);
		old_id = ug.id(e1);
		if (ug.id(e2) < old_id) old_id = ug.id(e2);
		e  = ug.new_edge(v1,e1,v2,e2);
		if (ug.start(e1) == v)       ug.make_directed (e,v2);
		else if (ug.start(e1) == v1) ug.make_directed (e,v1);
		ug.del_node (v);
		ug.assign_id (e,old_id);
		}
	
	make_embedding_as (ug);   // actually it IS planar, so we mutuate the planar embedding 
	                          // imposed on 'ug' by the make_embedding_planar_LEDA() procedure.
	return true;	
	}
	


	bool
	undi_graph::
make_embedding_cand_planar
	()
	{
	int old_id;
	gdtnode v;
	gdtedge e;
	gdt::gdtlist<gdtedge> dummy_edges;
	
	// ------------------ 0 -------------------
	// make a copy of *this in order to  modify
	// it if and only if the cand_planar embed-
	// ding exists, and in order to no destroy
	// the basic data structures of *this
	// ----------------------------------------
	
	undi_graph ug (*this);
	
	// ------------------ 1 -------------------
	// make a candidate embedding for each gdtnode
	// and an expansion for each gdtnode having 
	// at least 2 in-edges and 2 out-edges
	// ----------------------------------------
	
	dummy_edges = ug.make_embedding_cand_expanded();
	
	// ------------------ 2 --------------------
	// make embedding planar if it is possible
	// -----------------------------------------
	
	if (!ug.make_embedding_planar()) return false;
	
	//cout << "\n END MAKE EMBEDDING PLANAR" << flush;
	//cout << "\n CURRENT GRAPH: " << flush;
	//ug.print(COMPLETE);
	
	// ------------------ 3 --------------------
	// now the embedding of ug is planar and 
	// candidated. Then contractions are made to
	// reobtain the original graph.
	// -----------------------------------------
	
	forall (e,dummy_edges) 
		{
		gdtnode v1 = ug.source(e);
		gdtnode v2 = ug.target(e);
		if (ug.id(v1) < ug.id(v2)) old_id = ug.id(v1);
		else			   old_id = ug.id(v2);
		v = ug.contract(e);
		ug.assign_id(v,old_id);
		//cout << "\nnode " << ug.id(v) << " AFTER CONTRACTION:" << flush;
		//gdtedge et;
		//forall_edges_adjacent_node (et,v,ug) cout << " " << ug.id(et) << flush;
		}
		
	//cout << "\nEND NODE CONTRACTIONS" << flush;
		
	// ------------------ 5 --------------------
	// finally, for *this, an embedding equal to 
	// ug embedding is made
	// ----------------------------------------- 
	
	make_embedding_as (ug);
	return true;
	}

	
	void
	undi_graph::
make_embedding_cand
	()
	{
	gdtnode v;
	forall_nodes (v,nodes_and_edges())  make_embedding_cand (v);
	}


	gdt::gdtlist<gdtedge> 
	undi_graph::
make_embedding_cand_expanded 
	()
	{
	int old_id;
	gdtnode v;
	gdtedge e;
	gdt::gdtlist<gdtedge> dummy_edges;
	

	// ------------------ 1 -------------------
	// make a candidate embedding for each gdtnode
	// ----------------------------------------

	make_embedding_cand ();
	
	// CHECK
	// -----
	//cout << "\nCURRENT CANDIDATE EMBEDDING:\n";
	//print(COMPLETE); cout << flush;
	//forall_nodes (v,ug)
	//	{
	//	cout << "\n" << id(v) << ": " << flush;
	//	forall_edges_adjacent_node (e,v,*this) cout << id(e) << " " << flush;
	//	cout << "\n" << flush;
	//	}
		
	// ------------------ 2 -------------------
	// make an expansion for each gdtnode with 
	// at least 2 in-edges and 2 out-edges
	// ----------------------------------------
	
	gdt::gdtlist<gdtnode> nodes = all_nodes();
	
	forall (v,nodes)
		if (degree_in(v) >= 2 && degree_out(v) >= 2)
			{
			gdtedge e_init = first_in_edge(v);
			while (start(cyclic_adj_pred(e_init,v)) != v) 
				e_init = cyclic_adj_pred (e_init,v);
			gdtedge e_end = last_in_edge(v);
			while (start(cyclic_adj_succ(e_end,v)) != v)
				e_end  = cyclic_adj_succ (e_end,v);
			
			old_id = id(v);
			e = expand(v,e_init,e_end);
			gdtnode v1 = source(e);
			assign_id (v1,old_id);
			dummy_edges.append(e);
			}

	return dummy_edges;
	}



	gdt::gdtlist<gdtnode>
	undi_graph::
make_simple
	()
	{
	//cout << "Make simple activated" << endl;
	gdtnode v;
	gdtedge e;
	gdt::gdtlist<gdtnode> dummy_nodes;
	gdt::gdtlist<gdtnode> N;

	forall_nodes (v,*this) N.append(v);

	forall (v,N)
		{
		gdt::gdtnode_map<bool> multiple (nodes_and_edges(),false);
		gdt::gdtlist<gdtedge> E = adj_edges(v);
		forall (e,E)
			{
			gdtnode w = opposite(v,e);
			if (!multiple[w]) multiple[w] = true;
			else
				{
				// ----------
				// split gdtedge
				// ----------

				int old_id = id(e);
				gdtnode u   = new_node();

				gdtedge evu = new_edge(v,e,u);
				gdtedge ewu = new_edge(w,e,u);

				// -----------------------------
				// update markers and directions
				// -----------------------------

				(*edge_info)[evu].markers = (*edge_info)[e].markers;
				(*edge_info)[ewu].markers = (*edge_info)[e].markers;

				if (start(e) == v)
					{
					make_directed (evu,v);
					make_directed (ewu,u);
					}
				else if (start(e) == w)
					{
					make_directed (evu,u);
					make_directed (ewu,w);
					}

				// ----------------------------------
				// update constraints after splitting
				// ----------------------------------
				
				update_constraints_after_split_edge (e,evu,ewu); 
				
				// ---------------------
				// del the original gdtedge
				// and mark the new gdtnode
				// ---------------------
				
				del_edge(e);
				assign_id(evu,old_id); //in order to correctly rebuild old gdtedge
				dummy_nodes.append(u);
				mark (u,RM3_ADDED_BY_MAKE_SIMPLE);
				} 
			}
		}
	return dummy_nodes;
	}
	


	gdt::gdtlist<gdtedge>
	undi_graph::
make_connected
	()
	{
	gdtnode v;
	gdtedge e;
	gdt::gdtlist<gdtedge> dummy_edges;
	gdt::gdtnode_array<int> component(nodes_and_edges(), 0);

	connected_components(component);

        gdt::gdtmap<int,bool> attached(false);

	gdtnode prev = NULL_NODE;
	forall_nodes (v,*this) {
          int my_comp = component[v];
	  if( !attached[my_comp] ) {
	    if( prev != NULL_NODE ) { // you attach from the second on 
	      e = new_edge(v,prev);
	      dummy_edges.append(e);
	      mark(e,RM3_ADDED_BY_MAKE_CONNECTED);
	    }
	    attached[my_comp] = true;
	    prev = v;
	  }
	}
	return dummy_edges;
	}
	


	int
	undi_graph::
make_max_degree 
	(int d)
	{
	gdtnode v = NULL_NODE,
	     w = NULL_NODE;
	gdtedge e;
	int deleted_edges = 0;
	
	//Modified by A.M. 10/2002
	//gdt::gdtnode_pq<int> PQ(nodes_and_edges());
	gdt::gdtnode_pq<int> PQ(*this);
	//
	
	forall_nodes (v,nodes_and_edges())
		if (degree(v) > d) PQ.insert (v, -degree(v));
	
	while (!PQ.empty()) //extract the gdtnode with maximum degree
		{
		v = PQ.del_min();
		//cout << "\nEXTRACTED NODE " << id(v) << flush;
		int  max   = 0;
		gdtedge e_max = NULL_EDGE;
		//Modified by A.M.
		forall_edges_adjacent_node(e,v,*this) // search the adj gdtnode with maximum degree
			{
			w = opposite(v,e);
			//cout << "\nSCANNING ADJ-NODE " << id(w) << flush;
			int deg_w = degree(w);
			if (max < deg_w) {max=deg_w; e_max = e;}
			}
			
		// delete the gdtedge e=(v,w) and update PQ
		 
		del_edge (e_max);
		deleted_edges ++;
		if (PQ.member(w))  PQ.del(w); 
		if (degree(v) > d) PQ.insert (v, -degree(v));
		if (degree(w) > d) PQ.insert (w, -degree(w));	
		}
	return deleted_edges;
	}
	


	void
	undi_graph::
make_directed
	(gdtedge e, gdtnode v)
	{
	if (node_belongs_to_edge(v,e)) 
		{
		gdtnode w = opposite(v,e);
		struct_undi_edge_info& ei =(*edge_info)[e];
		struct_undi_node_info& vi =(*node_info)[v];
		struct_undi_node_info& wi =(*node_info)[w];
		
		if (edge_is_directed(e))
			{
			remove_from_in_edges (e);
			remove_from_out_edges(e);
			}
			
		ei.start = v;
		ei.pos_into_out_edges = vi.out_edges.append(e);
		ei.pos_into_in_edges  = wi.in_edges.append(e);
		
		mark_in_edges_as_not_ordered (start(e));
		mark_out_edges_as_not_ordered (stop(e));
		}
	else gdt_error("gdtnode does not belong to gdtedge");
	}
	
	
	
	void
	undi_graph::
make_directed
	(bool randomly)
	{
	gdtedge e;
	forall_edges (e,*this)
		if (!randomly) make_directed (e,source(e));
		else
			{
			int k = random (0,1);
			if      (k == 0) make_directed (e,source(e));
			else if (k == 1) make_directed (e,target(e));
			}
	}
	
	
	
	void
	undi_graph::
make_directed
	(gdtnode s, gdtnode t)
	{
	//cout << "\nmake st-directed" << flush;
	gdtedge e_st;
	gdtedge e;
	gdtnode v;
	bool flag = false;
	gdt::gdtnode_array<int> st_num (nodes_and_edges(), 0);
	
	forall_edges_adjacent_node (e_st,s,*this)
		if (opposite(s,e_st) == t) 
			{
			flag = true;
			break;
			}
	if (!flag)
		{ 
		e_st = new_edge (s,t);
		//cout << "\nADDED TEMPORARY EDGE (s,t)\n" << flush;
		}
	
	st_number (e_st,s,st_num);
	forall_edges (e,*this)
		{
		v = compare (source(e),target(e),st_num);
		make_directed (e,v);
		}
		
	if (!flag)
		{
		del_edge (e_st);
		//cout << "\nDELETED TEMPORARY EDGE (s,t)\n" << flush;
		}
	}
	
	
	
	void
	undi_graph::
make_undirected
	(gdtedge e)
	{
	if (edge_is_directed(e))
		{
		struct_undi_edge_info& ei = (*edge_info)[e];
		
		remove_from_in_edges (e);
		remove_from_out_edges(e);

		ei.start = NULL_NODE;
		ei.pos_into_in_edges = NULL;
		ei.pos_into_out_edges = NULL;
		}
	}
	


	void
	undi_graph::
make_undirected
	()
	{
	gdtedge e;
	forall_edges(e,*this) make_undirected(e);
	}
	
	
	
	void
	undi_graph::
make_source
	(gdtnode v)
	{
	gdtedge e;
	forall_edges_adjacent_node(e,v,*this) make_directed(e,v);
	}
	
	

	void
	undi_graph::
make_sink
	(gdtnode v)
	{
	gdtedge e;
	forall_edges_adjacent_node(e,v,*this) make_directed(e,opposite(v,e));
	}
	
	
	
	void
	undi_graph:: 
reverse 
	(gdtedge e)
	{
	make_directed (e,stop(e));
	}
	
	
	
	void
	undi_graph::
st_number 
	(gdtedge e_st, gdtnode s, gdt::gdtnode_array<int>& st_num)
	{
	undi_graph ug(*this);	// graph copy
	
	gdtnode v, w, t = opposite(s,e_st);
	gdtedge e;
	gdt::gdtstack<gdtnode> S;
	gdt::gdtlist<gdtedge>  L;
	gdt::list_item   pos_in_L;
	int  count = 0;
	bool root_is_articulated,
	     exist_path1 = true,
	     exist_path2 = true,
	     exist_path3 = true;
	
	gdt::gdtnode_array<bool> marked_node (ug.nodes_and_edges(), false),
			 reached     (ug.nodes_and_edges());
	gdt::gdtnode_array<gdtedge> father      (ug.nodes_and_edges(), NULL_EDGE);
	gdt::gdtnode_array<int>  num_visit   (ug.nodes_and_edges(), 0),
			 num_disc    (ug.nodes_and_edges(), 0),
			 st_num_ug   (ug.nodes_and_edges(), 0);
	gdt::gdtnode_array<gdtnode> lowpt1      (ug.nodes_and_edges(), NULL_NODE),
			 lowpt2      (ug.nodes_and_edges(), NULL_NODE);
	gdt::gdtlist<gdtnode> order;
	
	// test if graph is biconnected
	//
	if (!is_biconnected())	
		gdt_error("graph is not biconnected."); 
 
	
	// search the nodes corresponding to s,t and e_st in ug.
	//
	s    = ug.find_node (id(s));
	t    = ug.find_node (id(t));
	e_st = ug.find_edge (id(e_st));
	
	// compute a DFS of ug, starting from gdtnode t and gdtedge e_st.
	// Thus t and s will be the first and the second nodes visited.
	//	
	
	forall_nodes (v,ug.nodes_and_edges()) // Add by Walter Didimo on 22/07/97
		{
		reached[v] = false;
		father[v]  = NULL_EDGE;
		}

	ug.complex_DFS (t,e_st,reached,father,order,lowpt1,lowpt2,num_disc, root_is_articulated);
	
	forall (v,order)
		{
		count ++;
		num_visit[v] = count;
		}
	count = 0;
	
	
	// Initialization
	//
	S.push(t); //cout << "\nIN STACK NODE " << ug.id(t) << flush;
	S.push(s); //cout << "\nIN STACK NODE " << ug.id(s) << flush;
	ug.del_edge(e_st);
	forall_nodes (v,ug.nodes_and_edges())
		if ((v == s) || (v == t))  marked_node[v] = true;
		else			   marked_node[v] = false;
		
	// compute an st_numbering of ug
	//
	while (!S.empty())
		{
		v = S.pop(); //cout << "\nOUT STACK NODE " << ug.id(v) << flush;
		if (v != t)
			{
			// research a path v -> v1 -> v2 ->... -> vn -> w
			//
			L = ug.find_path (v,exist_path1,exist_path2,exist_path3,father,num_visit,lowpt1,marked_node);
			//cout << "\nRETURNED PATH " << flush;
			if (!L.empty())
				{
				// mark and put into stack all path's nodes 
				// vn,..., v2, v1, v  in this order.
				// Moreover delete all the path's edges.
				// 
				pos_in_L = L.last();
				marked_node[v] = false;
				while (pos_in_L)
					{
					e = L.contents(pos_in_L);
					if (marked_node[ug.source(e)]) w = ug.target(e);
					else			       w = ug.source(e);
					S.push(w); //cout << "\nIN STACK NODE " << ug.id(w) << flush;
					marked_node[w] = true;
					ug.del_edge(e);
					pos_in_L = L.pred (pos_in_L);
					}
				//cout << "\nPATH'S EDGES DELETED" << flush;
				} 
			else
				{
				count ++;
				st_num_ug[v] = count;
				//cout << "\nNUMERATED NODE " << ug.id(v) << flush;
				exist_path1  = true;
				exist_path2  = true;
				exist_path3  = true;
				}
			}
		else
			{
			count ++;
			st_num_ug[t] = count;
			//cout << "\nNUMERATED SINK NODE " << flush;
			}
		}
		
	// copy the ug st_numbering in *this st_numbering
	//
	//cout << "\nCOPYNG ST " << flush;
	v = first_node ();
	w = ug.first_node ();
	while (v)
		{
		//cout << "\nCOPY OF NODE " << ug.id(w) << "IN NODE "  << id(v) << flush;
		st_num[v] = st_num_ug[w];
		v = succ_node(v);
		w = ug.succ_node(w);
		}
	}

	
	void
	undi_graph::
reverse_adj_edges_lists
	(	
	)
	{
	gdtnode v;
	forall_nodes (v,*this)
		{
		((*node_info)[v].adj_edges).reverse();
		((*node_info)[v].in_edges).reverse();
		((*node_info)[v].out_edges).reverse();
		}
	}	
	
	
	/*void
	undi_graph::
calculate_level_of_all_nodes
	(gdt::gdtnode_array<int>& levels_buffer)	
	{
	gdtnode v;
	levels_buffer.init(nodes_and_edges(),-1);



	forall_nodes (v,*this) calculate_level_of_node (v,levels_buffer);
	}
	*/

	void undi_graph::calculate_level_of_all_nodes
		(gdt::gdtnode_array<int>& level) {
		
		gdtnode n_dg;	
		dire_graph dg(*this);
		gdt::gdtnode_array<int> level_dg(dg.nodes_and_edges(), 0);				
		gdt::gdtnode_array<bool> reached_dg(dg.nodes_and_edges(), false);
		gdt::gdtlist<gdtnode> order_dg;

		//gdtnode s_dg = dg.find_source();
		dg.topological_sort(dg.find_source(), reached_dg, order_dg);
		forall(n_dg, order_dg) {
			gdtedge e_dg;
			int max = -1;
			forall_edges_entering_node(e_dg, n_dg, dg) {
				max = gdt_maximum(max, level_dg[dg.opposite(n_dg,e_dg)]);
			}
			level_dg[n_dg] = max + 1;
			level[corresponding_node(n_dg,dg)] = max + 1;
		}
	}

/*
	int
	undi_graph::
calculate_level_of_node
	(gdtnode v, gdt::gdtnode_array<int>& levels_buffer)
	{
	if (levels_buffer[v]<0)
		{
		gdtedge e;	
		levels_buffer[v] = 0;
		forall_edges_entering_node (e,v,*this)
			levels_buffer[v] = gdt_maximum
				(
				levels_buffer[v],
				calculate_level_of_node(opposite(v,e),levels_buffer) + 1
				);
		}
	return levels_buffer[v];
	}
*/


	gdtedge
	undi_graph::
weld_edges_at_node
	(gdtnode v)
	{
	int i1,i2;
	gdtnode v1,v2;
	bool d12,d21;
	gdtedge e,e1,e2;
	//gdtedge e1p,e2p;
	
	if (degree(v)!=2)
		gdt_error("no two degree gdtnode"); 

	e1 = first_adj_edge(v); i1 = id(e1);
	e2 = last_adj_edge(v);  i2 = id(e2);
	
	v1 = opposite(v,e1);
	v2 = opposite(v,e2);
	
	d12=false; 
	d21=false;
	
	//if (edge_is_directed(e1) && edge_is_directed(e2))
	//	if      (stop(e1)==start(e2)) d12=true;
	//	else if (stop(e2)==start(e1)) d21=true;
	
	if (edge_is_directed(e1)) 
		if (stop(e1) == v) d12 = true;
		else		   d21 = true;
	else if (edge_is_directed(e2))
		if (start(e2) == v) d12 = true;
		else		    d21 = true;
	
	// --------
	// new code
	// --------
	
		// --------------------------------------
		// make a new gdtedge and update directions, 
		// markers and constraints
		// --------------------------------------
	
		e = new_edge (v1,e1,v2,e2); 
		if      (d12) make_directed (e,v1);
		else if (d21) make_directed (e,v2);
		mark(e,markers(e1));
		update_constraints_after_merge_edges (e1,e2,e);
	
		// ----------------------------------
		// delete gdtnode v and edges e1,e2, and 
		// reassign id to new gdtedge
		// ----------------------------------
		
		del_node(v);
		assign_id (e,gdt_minimum(i1,i2));
	
	return e;
	}



	gdtedge
	undi_graph::
expand
	(gdtnode v, gdtedge e_init, gdtedge e_end)
	{
	
	//cout << "\n** expanding gdtnode " << id (v) << " with e_init=" << id(e_init) << " and e_end=" << id(e_end) << flush;
	
	// -------------------------------------------------
	// 
	//        e6\    |e5  /e4            e6\   |e5 /e4
	//           \   |   /                  \  |  /
	//            \  |  /                    \ | /
	//             \ | /                      \|/
	//              \|/                        o v2
	//               o v       =>              |
	//              /|\                   e_dum|
	//             / | \                       |
	/*            /  |  \                      o v1      */
	/*           /   |   \                    /|\        */
	/* e_init=e1/  e2|    \e3=e_end          / | \	     */
	/*                                      /  |  \	     */
	//                                   e1/ e2|   \e3
	//
	// -------------------------------------------------
	
	int  old_id;
	gdtnode w;
	gdtedge ev1,ev2,ev,e_end_succ;
	gdtedge e_dum;	
	
	// --------------------------------------------
	// make two new nodes with same properties as v
	// --------------------------------------------
	
	gdtnode v1 = new_node();
	(*node_info)[v1].markers = (*node_info)[v].markers;
	(*node_info)[v1].in_edges_are_ordered  = (*node_info)[v].in_edges_are_ordered;
	(*node_info)[v1].out_edges_are_ordered = (*node_info)[v].out_edges_are_ordered;
	
	gdtnode v2 = new_node();
	(*node_info)[v2].markers = (*node_info)[v].markers;
	(*node_info)[v2].in_edges_are_ordered  = (*node_info)[v].in_edges_are_ordered;
	(*node_info)[v2].out_edges_are_ordered = (*node_info)[v].out_edges_are_ordered;
	
	// ------------------------------------
	// make all edges adjacent v1, updating
	// directions, markers and constraints
	// ------------------------------------
	
	e_end_succ = cyclic_adj_succ(e_end,v);
	ev  = e_init;
	
	while (ev != e_end_succ)
		{
		//cout << "\nexaming gdtedge " << id(ev) << flush;
		w      = opposite(v,ev);
		ev1    = new_edge (w,ev,v1);		
		
		old_id = id(ev);
		(*edge_info)[ev1].markers = (*edge_info)[ev].markers;
		if (source(ev) == v) set_source(ev1,v1);
		else 		     set_source(ev1,w);
			
		if (start(ev)) // gdtedge is directed
			if (start(ev) == v) make_directed(ev1,v1);
			else		    make_directed(ev1,w);
								
		gdtedge e = ev;
		ev = cyclic_adj_succ(ev,v);
		
		update_constraints_after_edge_translation (e,v,w,ev1,v1,w);
		
		del_edge (e);
		assign_id (ev1,old_id);
		}
	
	//cout << "\n     adj. edges of new gdtnode v1 " << id(v1) << ": " << flush;
	//forall_edges_adjacent_node (e,v1,*this) cout << id(e) << " " << flush;
	//cout << "\n"; 
	
	// ------------------------------------
	// make all edges adjacent v2, updating
	// directions, markers and constraints
	// ------------------------------------

	while (ev)
		{
		//cout << "\nexaming gdtedge " << id(ev) << flush;
		w = opposite(v,ev);
		ev2    = new_edge (w,ev,v2);
		old_id = id(ev);
		(*edge_info)[ev2].markers = (*edge_info)[ev].markers;
		if (source(ev) == v) set_source(ev2,v2);
		else 		     set_source(ev2,w);
					
		if (start(ev)) // gdtedge is directed
			if (start(ev) == v) make_directed(ev2,v2);
			else		    make_directed(ev2,w);
			
		gdtedge e = ev;
		if (degree(v) > 1) ev = cyclic_adj_succ(ev,v);
		else ev = NULL_EDGE;
		
		update_constraints_after_edge_translation (e,v,w,ev2,v2,w);
		
		del_edge (e);
		assign_id (ev2,old_id);	
		}
		
	//cout << "\n     adj. edges of new gdtnode v2 " << id(v2) << ": " << flush;
	//forall_edges_adjacent_node (e,v2,*this) cout << id(e) << " " << flush;
	//cout << "\n"; 
	
	// ------------------------------------
	// update constraints still involving v
	// and delete such a gdtnode
	// ------------------------------------
	
	update_constraints_after_split_node (v,v1,v2);
	del_node (v);
	
	// --------------------------------------
	// make a new directed dummy gdtedge (v1,v2) 
	// and return it
	// --------------------------------------
	
	e_dum = new_edge (v1,v2);
	make_directed (e_dum,v1);
	mark (e_dum,RM3_ADDED_BY_EXPAND);
	mark (v2,RM3_ADDED_BY_EXPAND);
	return e_dum;
	}
	


	gdtnode
	undi_graph::
contract
	(gdtedge ec)
	{
	// reverse of expand operation	
	int  old_id;
	gdtnode v1 = source(ec);
	gdtnode v2 = target(ec);
	gdtnode v,w;
	gdtedge ev,ev1,ev2;
	
	//cout << "contracting gdtedge " << id(ec) << "=(" << id(v1) << "," << id(v2) << ")" << endl;
	
	// ----------------------------
	// make a new gdtnode v with the
	// same properties as v1 and v2 
	// (when possibile)
	// ----------------------------
	
	v  = new_node();
	
	if (is_marked(v2,RM3_ADDED_BY_EXPAND)) (*node_info)[v].markers = (*node_info)[v1].markers;
	else 				       (*node_info)[v].markers = (*node_info)[v2].markers;
	
	(*node_info)[v].in_edges_are_ordered   = ((*node_info)[v1].in_edges_are_ordered  && (*node_info)[v2].in_edges_are_ordered );
	(*node_info)[v1].out_edges_are_ordered = ((*node_info)[v1].out_edges_are_ordered && (*node_info)[v2].out_edges_are_ordered);

	// --------------------------------
	// add to gdtnode v all adjacent 
	// edges of gdtnode v1, and update
	// markers, directions, constraints
	// --------------------------------
	
	//cout << "Adjacent edges of " << id(v1) << endl;
	ev1 = cyclic_adj_succ(ec,v1);
	while (ev1 != ec)
		{		
		
		w = opposite(v1,ev1);		
		if (w != v2)
			{ 			
			ev     = new_edge (w,ev1,v);						
			old_id = id(ev1);
			(*edge_info)[ev].markers = (*edge_info)[ev1].markers;	
			if (source(ev1) == v1) set_source (ev,v); 
			else		       set_source (ev,w);
						
			if (start(ev1)) // gdtedge is directed
				if (start(ev1) == v1) make_directed(ev,v);
				else		      make_directed(ev,w);
			
			gdtedge e = ev1;
			ev1 = cyclic_adj_succ(ev1,v1);
			
			update_constraints_after_edge_translation (e,v1,w,ev,v,w);
						
			del_edge (e);			
			assign_id (ev,old_id);				
			}
		else 
			{
			gdtedge e = ev1;
			ev1 = cyclic_adj_succ(ev1,v1);
			del_edge (e);
			}
		}	
		
	// --------------------------------
	// add to gdtnode v all adjacent 
	// edges of gdtnode v2, and update
	// markers, directions, constraints
	// --------------------------------
	
	ev2 = cyclic_adj_succ(ec,v2);
	while (ev2 != ec)
		{
		w = opposite(v2,ev2);
		if (w != v1)
			{
			ev    = new_edge (w,ev2,v);
			old_id = id(ev2);
			(*edge_info)[ev].markers = (*edge_info)[ev2].markers;
			if (source(ev2) == v2) set_source(ev,v);
			else 		       set_source(ev,w);
				
			if (start(ev2)) // gdtedge is directed
				if (start(ev2) == v2) make_directed(ev,v);
				else		      make_directed(ev,w);
			
			gdtedge e = ev2;
			ev2 = cyclic_adj_succ(ev2,v2);
			
			update_constraints_after_edge_translation (e,v2,w,ev,v,w);
			
			del_edge (e);
			assign_id (ev,old_id);				
			}
		else 
			{
			gdtedge e = ev2;
			ev2 = cyclic_adj_succ(ev2,v2);
			del_edge (e);
			}
		}
	
	// ---------------------------------
	// delete the vertices v1 and v2 and
	// update constraints involving them
	// ---------------------------------
	
	update_constraints_after_merge_nodes (v1,v2,v);
	del_node (v1);
	del_node (v2);
		
	return v;
	}
	
	
	
	gdt::gdtlist<gdtnode>
	undi_graph::
contract
	()
	{
	int old_id;
	gdtnode v;
	gdtedge e;
        int ide;
	gdt::gdtlist<gdtnode> N;
        gdt::gdtlist<int> edges_id;
	forall (e, all_edges() ) 
          if (  is_marked (e, RM3_ADDED_BY_EXPAND) ) edges_id.append(id(e));

	forall (ide,edges_id)
			{
                        e = find_edge(ide);
			gdtnode v1 = source(e);
			gdtnode v2 = target(e);
			if (id(v1) < id(v2)) old_id = id(v1);
			else		     old_id = id(v2);
			v = contract(e);
			assign_id(v,old_id); 
			N.append(v);
			}
	return N;
	}
	

		
	void
	undi_graph::
update_max_node_id
	()
	{
	gdtnode v;
	max_node_id = NULL_ID;
	forall_nodes (v,*this)
		if (max_node_id < id(v)) max_node_id = id(v);
	}
	
	
	
	void
	undi_graph::
update_max_edge_id
	()
	{
	gdtedge e;
	max_edge_id = NULL_ID;
	forall_edges (e,*this)
		if (max_edge_id < id(e)) max_edge_id = id(e);
	}



	void
	undi_graph::
update_max_constraint_id
	()
	{
	constraint c;
	max_constraint_id = NULL_ID;
	forall_constraints (c,*this)
		if (max_constraint_id < id(c)) max_constraint_id = id(c);
	}


	
	void
	undi_graph::
rise_max_node_id
	(int new_max_node_id)
	{
	if( new_max_node_id >= max_node_id )
		max_node_id = new_max_node_id;
	else
		gdt_error("new_max_node_id too low");
	}



	void
	undi_graph::
rise_max_edge_id
	(int new_max_edge_id)
	{
	if( new_max_edge_id >= max_edge_id )
		max_edge_id = new_max_edge_id;
	else
		gdt_error("new_max_edge_id too low");
	}



	void
	undi_graph::
rise_max_constraint_id
	(int new_max_constraint_id)
	{
	if( new_max_constraint_id >= max_constraint_id )
		max_constraint_id = new_max_constraint_id;
	else
		gdt_error("new_max_constraint_id too low");
	}



// ============= CLASSES AND METHODS FOR SUPPORTING 'planarize' method ====================

 
	void 
move_components_outside_wheel
	(
	undi_graph& final,
	int hubid
	);



	class
crossing_manager
	{
	private:
			struct
		extremal_edges
			{
			extremal_edges(): source_edge(NULL_EDGE), target_edge(NULL_EDGE) {};
			gdtedge source_edge;  // gdtedge of T
			gdtedge target_edge;  // gdtedge of T (these are the extremal edges of the path corresponding to a single gdtedge in S)
			};

		const undi_graph *		S;                  // 'S' stands for "source graph"
		plan_undi_graph *		T;                  // 'T' stands for "target graph"
		//h_array<gdtedge, gdtedge>		fromT2S;
		gdt::gdtmap<gdtedge, gdtedge> fromT2S;
		gdt::gdtedge_array<extremal_edges>	fromS2T;
		gdt::gdtnode_array<gdtnode>		nodes_fromS2T;

	public:
		crossing_manager(const undi_graph& S, plan_undi_graph& T);  // T is meant to be a planarization of S
		                                                      // initially all edges of S have no corresponding gdtedge in T
		                                                      // S is assumed to remain constant during the life of the object.
		~crossing_manager() {}

		// -----------------
		// Access operations
		// -----------------

		bool is_split(gdtedge e) const                   // e must belong to undi_graph S
			{
			return fromS2T[e].source_edge != fromS2T[e].target_edge;
			}


		bool has_corresponding_in_T(gdtedge e) const
			{
			return fromS2T[e].source_edge != NULL_EDGE;
			}

		gdtedge get_extremal_edge_in_T(gdtedge e, gdtnode n) const
							 // e (incident to n) must belong to S
		                                         // returns the gdtedge of the path in T corresponding to 'e' and
		                                         // incident to the gdtnode in T corresponding to 'n'
			{
			if( n == S->source(e) )
				return fromS2T[e].source_edge;
			else if( n == S->target(e) )
				return fromS2T[e].target_edge;
			else
				gdt_error("n is not incident to e" );
			return NULL_EDGE;  // TO AVOID WARNING
			}


		gdtedge get_corresponding_in_S(gdtedge e) const   // e must belong to T
			{
			if ( fromT2S.defined(e) )
				return fromT2S[e];
			else
				return NULL_EDGE;
			}



		void get_corresponding_face_and_ref_edge_id(gdtedge S_edge, gdtnode S_node, 
		                                            face& f_in_T, int& T_ref_edge_id) const;

							 // S_edge (incident to S_node) must belong to S
							 // returns:
		                                         //    f_in_T - the face of T in which must be inserted the 
		                                         //             gdtedge corresponding to 'S_edge'
		                                         //    T_ref_edge_id - the gdtedge after which the new gdtedge must 
		                                         //                    be inserted in the incidency list of 
		                                         //                    the gdtnode corresponding to 'S_node'
		                                         // This method is intended to work with gdtedge 'S_edge' 
		                                         // which has no correspondance in T

					
		void dump_status(std::ostream& out) const;                // for debugging purposes


		// -----------------
		// Update operations
		// -----------------
		
		void set_corresponding(gdtedge S_e, gdtedge T_e)   // asserts the correspondence between 'S_e' (belonging to S)
		                                             // and 'T_e' (belonging to T). You have to do this
		                                             // each time you create in T an gdtedge corresponding to S
			{
			fromS2T[S_e].source_edge = T_e;
			fromS2T[S_e].target_edge = T_e;
			fromT2S[T_e]= S_e;
			}

		void set_corresponding(gdtedge S_e, const gdt::gdtlist<gdtedge>& edge_list)    // asserts the correspondence between 'S_e' (belonging to S)
		                                                                 // and the path 'edge_list'
			{
			switch( edge_list.size() )
				{
				case 0: 
					gdt_error("empty list");
					break;
				case 1:
					set_corresponding(S_e, edge_list.head());
					break;
				
				default:
					gdtedge e;
					forall( e , edge_list ) fromT2S[e] = S_e;
					
					gdtnode source_node_of_the_chain = nodes_fromS2T[S->source(S_e)];
					if( T->node_belongs_to_edge(source_node_of_the_chain, edge_list.head()) )
						{
						fromS2T[S_e].source_edge = edge_list.head();
						fromS2T[S_e].target_edge = edge_list.tail();
						}
					else
						{
						fromS2T[S_e].source_edge = edge_list.tail();
						fromS2T[S_e].target_edge = edge_list.head();
						}
					break;
				}
			}

		void set_corresponding_node_in_T(gdtnode S_n, gdtnode T_n) { nodes_fromS2T[S_n]= T_n; }

		                                             
                void split_edge(gdtedge T_e, gdtnode& dummy_node_in_T, 
                                face& f1_in_T, gdtedge& e1_in_T, 
                                face& f2_in_T, gdtedge& e2_in_T);
		                                             
		                                             // splits gdtedge 'T_e' of T inserting a dummy gdtnode in the middle,
		                                             // the two resulting edges will have the same 'father gdtedge' as 'T_e'
		                                             // 'T_e' is removed from array fromT2S
		                                             // fromS2T is updated accordingly
							     //
							     // For each split, one of the two generated gdtedge has the old ID,
							     // the other has a generated one.
		
	};


	crossing_manager::
crossing_manager(const undi_graph& S1, plan_undi_graph& T1):
	S(&S1), 
	T(&T1),
	fromS2T( S->nodes_and_edges(), extremal_edges() ),
	nodes_fromS2T(S->nodes_and_edges(), NULL_NODE)
	{}

	void
	crossing_manager::
split_edge(gdtedge T_e, gdtnode& dummy_node_in_T, face& f1_in_T, gdtedge& e1_in_T, 
                                            face& f2_in_T, gdtedge& e2_in_T)
	{
	
	//                  side_1                       side_1      w1
	//           ---------O----------      	 --------------------O--------------------
	//          |         |          |      |                    |                    |
	//          |         |          |      |        e1_in_T --> |     f2_in_T        |
	//          |         |          |      |                    |                    |
	//          |         | T_e      |  ->  |                    O dummy_node_in_T    |
	//          |         |          |      |                    |                    |
	//          |         |          |      |        f1_in_T     | <-- e2_in_T        |
	//          |         |          |      |                    |                    |
	//           ---------O----------        --------------------O--------------------
	//                  side_2                       side_2      w2
	//
	int T_e_id= T->id(T_e);
 
        gdtedge father_edge_in_S = get_corresponding_in_S(T_e);
        gdtnode source_node_in_S = S->source(father_edge_in_S);
        gdtnode target_node_in_S = S->target(father_edge_in_S);
        gdtnode source_node_in_T = nodes_fromS2T[source_node_in_S];
        gdtnode target_node_in_T = nodes_fromS2T[target_node_in_S];
                  	
        // performing the split operation

		//cout << "\n               T_e = " << T_e << " with id " << T->id(T_e) << flush;
		//cout << "\n father_edge_in_S  = " << father_edge_in_S << " with id " << S->id(father_edge_in_S) << flush;

		dummy_node_in_T = T->new_node(T_e);
		T->mark(dummy_node_in_T, RM3_CROSS_ON_REAL_EDGE);
		
		
		
		e1_in_T = T->first_adj_edge(dummy_node_in_T);
		e2_in_T = T->last_adj_edge (dummy_node_in_T);

		
		gdtnode w1 = T->opposite(dummy_node_in_T,e1_in_T);
		//gdtnode w2 = T->opposite(dummy_node_in_T,e2_in_T);


		//cout << "\ndummy gdtnode inserted = " << dummy_node_in_T << " with id " << T->id(dummy_node_in_T) << flush;
		//cout << "\n            e1_in_T = " << e1_in_T << " with id " << T->id(e1_in_T) << flush;
		//cout << "\n            e2_in_T = " << e2_in_T << " with id " << T->id(e2_in_T) << flush;
		//cout << "\nw1 = opposite(dummy, e1_in_T) = " << w1 << " with id " << T->id(w1) << flush;
		//cout << "\nw2 = opposite(dummy, e2_in_T) = " << w2 << " with id " << T->id(w2) << flush;
		//cout << "\nopposite(w1, e1_in_T) = " << T->opposite(w1, e1_in_T) << " with id " << T->id(T->opposite(w1, e1_in_T)) << flush;
		//cout << "\nopposite(w2, e2_in_T) = " << T->opposite(w2, e2_in_T) << " with id " << T->id(T->opposite(w2, e2_in_T)) << flush;

		// one of two has original ID
		//
		if (T->is_marked(w1,RM3_CROSS_ON_REAL_EDGE)) T->assign_id (e2_in_T, T_e_id);
		else    				     T->assign_id (e1_in_T, T_e_id);
		
		//cout << "\n punt gdtedge with id 5 = " << T->find_edge(5) << flush;
		               
		        
	// updating fromS2T
		{
		if( fromS2T[father_edge_in_S].source_edge == T_e )
			{
		        if( T->opposite(dummy_node_in_T, e1_in_T) == source_node_in_T )
		        	fromS2T[father_edge_in_S].source_edge =  e1_in_T;
		        else
		        	fromS2T[father_edge_in_S].source_edge =  e2_in_T;
		        }

		if( fromS2T[father_edge_in_S].target_edge == T_e )
		        {
			if( T->opposite(dummy_node_in_T, e1_in_T) == target_node_in_T )
				fromS2T[father_edge_in_S].target_edge =  e1_in_T;
			else
				fromS2T[father_edge_in_S].target_edge =  e2_in_T;
			}
		}
				
	// updating fromT2S
		{
		fromT2S.undefine(T_e);
		fromT2S[e1_in_T] = father_edge_in_S;
		fromT2S[e2_in_T] = father_edge_in_S;
		}

	// producing output
		{
		f1_in_T = T->left_face_moving_along_edge(e1_in_T, dummy_node_in_T);
		f2_in_T = T->left_face_moving_along_edge(e2_in_T, dummy_node_in_T);
		}

		//cout << "\n(at the end) e1_in_T = " << e1_in_T << " with id " << T->id(e1_in_T) << flush;

	}


	void 
	crossing_manager::
get_corresponding_face_and_ref_edge_id(gdtedge S_edge, gdtnode S_node, 
                                       face& f_in_T, int& T_ref_edge_id) const
	{
	gdtedge S_new_ref_edge = S->cyclic_adj_pred(S_edge, S_node);
	while( !has_corresponding_in_T(S_new_ref_edge) ) 
		S_new_ref_edge = S->cyclic_adj_pred(S_new_ref_edge, S_node);	// this loop terminates because each gdtnode
		    		                     	           		// has degree at least one. (T is connected)

	gdtedge T_ref_edge = get_extremal_edge_in_T(S_new_ref_edge, S_node);
	f_in_T = T->left_face_moving_along_edge(T_ref_edge, nodes_fromS2T[S_node]);
	T_ref_edge_id = T->id(T_ref_edge);
	}


	
	void
	crossing_manager::
dump_status(std::ostream& out) const                // for debugging purposes
	{
	out << " ------------ crossing manager dump ------------" << endl;
	
	out << "---S to T:" << endl;
	gdtedge e;
	forall_edges( e, S->nodes_and_edges() )
		{
		int id_src= ( fromS2T[e].source_edge!=NULL_EDGE ) ? T->id(fromS2T[e].source_edge) : NULL_ID;
		int id_trg= ( fromS2T[e].target_edge!=NULL_EDGE ) ? T->id(fromS2T[e].target_edge) : NULL_ID;
		out << S->id(e) << " src: " << id_src << " trg: " << id_trg << endl;
		}
	
	out << "---T to S:" << endl;
	forall_edges( e, T->nodes_and_edges() )
		{
		if (fromT2S.defined(e))
			out << T->id(e) << "   " << S->id(fromT2S[e]) << endl;
		else
			out << T->id(e) << "   no gdtedge in S" << endl;
		}
		
	out << "---nodes from S to T:" << endl;
	gdtnode n;
	forall_nodes( n, T->nodes_and_edges() )
		{
		if (nodes_fromS2T[n]!= NULL_NODE)
			out << S->id(n) << "   " << T->id(nodes_fromS2T[n]) << endl;
		else
			out << S->id(n) << "   no gdtedge in T" << endl;
		}
		
	}

//==========================================================================================

	int 
	undi_graph::
planarize
	(planarize_options po)
	{
	
	#ifdef DEBUG
	std::cout << "Planarize. This=" << this << std::endl;
	#endif
	if(number_of_edges()<1)
		gdt_error("number_of_edges < 1 not allowed");

	switch(po)
		{
		case DO_NOT_PRESERVE_EMBEDDING:
		case PRESERVE_EMBEDDING:
			break;
		default:
			gdt_error("unknown planarization option");
			break;
		}

	gdt::gdtlist<gdtedge> uncrossable_edges;
	gdt::gdtlist<gdtedge> edge_to_remove_after_max_planar;

	undi_graph ug(*this); 		// make a copy of *this to leave it unchanged

	// MODIFY ug TO ACCOUNT FOR MARKERS AND CONSTRAINTS (this order is important!)
	//

	// MARKERS

	// search for RM3_CENTER_OF_WHEEL markers, and set as uncrossable
	// all the edges of the wheel: PRECONDITION: the wheels are consistent
	// and do not generate collisions!! 
	// (see below for the structure of a wheel))
	//
	//      o-----o		
	/*     / \   / \       */                                                                  
	/*    /   \ /   \      */	
	//   o-----*-----o 
	//    \   / \   /	
	//     \ /   \ /	
	//      o-----o
		
		{
		gdtnode v;
		gdtedge e;
		gdt::gdtmap<gdtnode,gdtnode> wheel_of_node(NULL_NODE);
		forall_nodes (v,ug)
			if (ug.is_marked(v,RM3_CENTER_OF_WHEEL))
				{		
				//Modified by A.M.
				forall_edges_adjacent_node(e,v,ug)		
					{
					ug.new_constraint_uncrossable_edge (e);
					gdtnode w = ug.opposite(v,e);
					wheel_of_node[w] = v;
					}
				}
		forall_edges (e,ug)
			{
			gdtnode v1 = ug.source(e);
			gdtnode v2 = ug.target(e);
			if (wheel_of_node[v1] && 
			    wheel_of_node[v1] == wheel_of_node[v2]) 
				ug.new_constraint_uncrossable_edge (e);
				
			}
		}
	

	// CONTRAINTS
	//
	// the following variables are for SAME_FACE_NODES constraints
	//
	gdt::gdtmap<int,gdt::gdtlist<gdtnode>*>	from_face_labels_to_node_lists(NULL);
	gdt::gdtlist<int>		used_face_labels;

	constraint c;
	forall_constraints(c, ug)
		{
		switch( c->get_type() )
			{
			case UNCROSSABLE_EDGES:
				{
				gdtedge e = ((constraint_uncrossable_edge)c)->get_uncrossable_edge();
				uncrossable_edges.append(e);
				}
				break;
			case SAME_FACE_NODES:
				{
				#ifdef DEBUG
				std::cout << "Constraint SAME_FACE_NODES" << endl;				
				#endif				
				gdtnode n = ((constraint_same_face_node)c)->get_same_face_node();
				int fl = ((constraint_same_face_node)c)->get_face_label();
				if( !from_face_labels_to_node_lists.defined(fl) )
					{
					from_face_labels_to_node_lists[fl] = new gdt::gdtlist<gdtnode>;
					used_face_labels.append(fl);
					}
				from_face_labels_to_node_lists[fl]->append(n);
				}
				break;
			case SAME_FACE_ORDERED_NODES:
				{
				/*			GADGET FOR SAME_FACE_ORDERED_NODES CONSTRAINT
				//       
				//	o-----o		all edges are uncrossable (marked RM3_PLANARIZATION_GADGET_EDGE)
				//     / \   / \                                                                    
				//    /   \ /   \	central vertex is dummy (marked RM3_PLANARIZATION_GADGET_NODE)
				//   o-----*-----o
				//    \   / \   /	external edges will be removed after the maximal planar will
				//     \ /   \ /	be found (they risk to fence part of the graph)
				//      o-----o
				*/
				
				gdt::gdtlist<gdtnode> Ln = ((constraint_same_face_ordered_nodes)c)->get_node_list();
				gdtnode n = ug.new_node(Ln);
				ug.mark(n,RM3_PLANARIZATION_GADGET_NODE);
				gdt::gdtlist<gdtedge> Le = ug.adj_edges(n);
				gdtedge e;
				forall (e,Le)
					{
					ug.mark(e,RM3_PLANARIZATION_GADGET_EDGE);
					uncrossable_edges.append(e);
					}
				
				// 			SITUATION
				//
				//	o     o	
				//       \   / 
				//        \ /   
				//   o-----*-----o
				/*        / \         */
				/*       /   \        */
				//      o     o


				gdtnode pn = Ln.tail();	// previous gdtnode
				gdtnode cn;		// current gdtnode
				//std::cout << "\nbuilding planarization gadget for nodes : ";
				forall(cn,Ln)
					{
					//std::cout << ug.id(cn) << " ";
					gdtedge ne = ug.new_edge(pn,cn);
					ug.mark(ne,RM3_PLANARIZATION_GADGET_EDGE);
					uncrossable_edges.append(ne);
					edge_to_remove_after_max_planar.append(ne);
					pn = cn;
					}
					std::cout << "\n";
				}
				break;
			default:
				break;
			}
		}

	ug.del_all_constraints();	// we do not need them anymore: all constraints have been translated in
					// some gadget added to the graph, plus the uncrossable_edges list.


	int face_label;
	forall(face_label, used_face_labels)
		{
		
		//           GADGET FOR SAME_FACE_NODE CONSTRAINT
		// 
		//           o   o   o      <--- nodes to be put in the same face
		//            \  |  /
		//             \ | /        <--- new uncrossable edges introduced (marked RM3_PLANARIZATION_GADGET_EDGE)
		//              \|/
		//               *          <--- new dummy gdtnode introduced (marked RM3_PLANARIZATION_GADGET_NODE)
		
		
		gdt::gdtlist<gdtnode>& Ln = *(from_face_labels_to_node_lists[face_label]);
		gdtnode n = ug.new_node(Ln);
		ug.mark(n,RM3_PLANARIZATION_GADGET_NODE);
		gdt::gdtlist<gdtedge> Le = ug.adj_edges(n);
		gdtedge e;
		forall (e,Le)
			{
			ug.mark(e,RM3_PLANARIZATION_GADGET_EDGE);
			uncrossable_edges.append(e);
			}
		}
	
	if (!ug.planarize_kernel(uncrossable_edges, edge_to_remove_after_max_planar, po))
		return INFINITE;

	// REMOVE GADGETS INTRODUCED IN ug TO ACCOUNT FOR MARKERS AND CONSTRAINTS
	//
	// modified by W.D. on 24 April 2008
	//
	gdtedge gadget_edge; 
	gdt::gdtlist<gdtedge> edges_copy = ug.all_edges();
	forall (gadget_edge, edges_copy)
		{
		if (ug.is_marked(gadget_edge,RM3_PLANARIZATION_GADGET_EDGE))
			ug.del_edge(gadget_edge);
		}
	//
	// modified by W.D. on 24 April 2008
	//
	gdtnode gadget_node;
	gdt::gdtlist<gdtnode> nodes_copy = ug.all_nodes();
	forall (gadget_node, nodes_copy)
		{
		if (ug.is_marked(gadget_node,RM3_PLANARIZATION_GADGET_NODE))
			ug.del_node(gadget_node);
		}
	
	// REINSERT ALL CONSTRAINTS FROM *this: 
	//
	//	first  - try to copy each constraint from *this with the new_constraint() method
	//	second - remove the constraints on edges of ug belonging to paths corresponding to single edges of *this
	//               (this is because we want to rule out constraints' duplications)
	// 	third  - update constraints on paths of ug corresponding to single edges of *this
	
	forall_constraints (c,*this)
		ug.new_constraint(c);	// try to copy it from *this

	gdtedge e;
	gdtedge e_1;
	gdt::gdtlist<gdtedge> path_in_ug;
	
	forall_edges (e,*this)
		{
		path_in_ug = ug.path_corresponding_to_edge (e,*this);
		if (path_in_ug.size() > 1)
			{
			forall (e_1,path_in_ug)	// remove all constraints of path_in_ug
				{
		                gdt::gdtlist<constraint> Lc = ug.all_constraints(e_1);  // make a copy of gdtedge constraint-list
				constraint c_1;
	        	        forall (c_1,Lc)		ug.del_constraint (c_1);	
				}
			ug.update_constraints_on_path_replacing_edge (e, *this, path_in_ug);
			}
		}
	
	// ----------------------------------
	// count the number of added vertices
	// ----------------------------------
	//
	int number_of_cross_nodes = 0;
		{
		gdtnode v;
		forall_nodes (v,ug)
			if (!corresponding_node(v,ug)) number_of_cross_nodes++;
		}
		
	*this = ug;
	
	return number_of_cross_nodes;	
	}	
	


	bool 
	undi_graph::
planarize_kernel
	(
	gdt::gdtlist<gdtedge>& C_uncrossable_edges, 
	gdt::gdtlist<gdtedge>& C_edge_to_remove_after_max_planar,
	planarize_options po)
	{
	#ifdef DEBUG
    std::cout << "Starting planarize_kernel..." << endl;
	std::cout << "This= " << this << "\tnodes= " << this->number_of_nodes() << "\tedges= " << this->number_of_edges() << endl;
	#endif

	// NOTE: *this is without constraints: all constraints have been translated
	//       in gadgets attached to the graph plus a list of uncrossable edges.
	//
	// ------------------------------------
	//   GENERAL SCHEME OF THE ALGORITHMS:
	// ------------------------------------
	//
	// 1) BUILD A MAXIMAL PLANAR GRAPH
	// 
	// 1.A) if no uncrossable_edges defined
	//      
	//      start from a spanning tree of original graph.
	//
	//      1.A.1) if DO_NOT_PRESERVE_EMBEDDING
	//
	//	 	add the first 3 edges (you don't lose
	//	  	planarity if you add 3 edges to a tree)
	//
	// 	1.A.2) if PRESERVE_EMBEDDING
	//             
	//             nothing to do
	//
	// 1.B) if uncrossable_edges provided
	//
	//      1.B.1) if DO_NOT_PRESERVE_EMBEDDING
	//
	//		start from the set of uncrossable edges 
	//   		and all nodes.
	//
	// 	1.B.2) if PRESERVE_EMBEDDING
	//             
	//             WE DO NOT ALLOW THIS CASE !!!
	// 1.A) if no uncrossable	//
	// 
	// 2) ADD REMAINING EDGES
	//
	//      2.A) if DO_NOT_PRESERVE_EMBEDDING
	//
	//		2.A.1) try inserting remaining edges testing each time
	//		       if planarity is preserved, and saving edges
	//	  	       that can't be inserted this way (you end up 
	//	     	       with a maximal planar embedding). This step
	//	               alters original embedding.
	//	 	2.A.2) make dual graph and remove edges corresponding
	//                     to uncrossable_edges.
	//                     Insert the saved edges with a 
	//	     	       shortest_path_on_dual_graph strategy, inserting 
	//	               dummy nodes when needed.
	//
	// 	2.B) if PRESERVE_EMBEDDING
	//
	// 	   	2.B.1) make dual graph and remove edges corresponding
	//                     to uncrossable_edges.
	//                     reinsert the remaining edges with a 
	// 	               shortest-path-in-dual-graph strategy inserting dummy 
	//                     nodes when needed.
	//
	// ------------------------------------

	// In the following:
	// 
	// EVERY 'SWITCH' STATEMENT ENCLOSE DIFFERENTIATING PART BETWEEN ALGORITHMS
	//
	// 	- every variable that begins with the "C_" prefix indicates an object pertaining 
	// 	  to the (*this) undi_graph from which we want a constrained planarization.
	// 
	//	- every variable that begins with the "U_" prefix indicates an object pertaining
	// 	  to 'ug', a growing undi graph used as a temporary graph.
	//
	//	- every variable that begins with the "P_" prefix indicates an object pertaining
	//	  to 'pug', a growing planar undi graph, that will be the desired output.
	//
	//	- every variable that begins with the "D_" prefix indicates an object pertaining
	//  	  to 'dual', the dual graph of 'pug' created anew each time an gdtedge is inserted.
	

	// gdtedge id assignment, example:
	//
	//    S->get_max_edge_id() == 24
	//     
	//     v1   |  5  |  v2
	//     O----+-----+--O                    // in S
	//          |     |
	//
	//
	//     v1  28   |   26   |   5    v2
	//     O--------O--------O--------O       // situation in T after two split
	//              |        |
	//
	//  note that each dummy gdtnode introduces two split (two new gdtedge one of which 
	//  take the same ID of the deleted gdtedge), the "crossing remove" step of 
	//  'draw_undi_graph' constructor restores the ID to the smallest one, so
	//  reaching back the original gdtedge id.
	
		
	undi_graph ug(*this);  // we are making a copy of *this  
	#ifdef DEBUG
    std::cout << "Planarize_kernel creation of ug" << endl;
	std::cout << "ug= " << &ug << "\tnodes= " << ug.number_of_nodes() << "\tedges= " << ug.number_of_edges() << endl;
	#endif

	int unused_edge_id = get_max_edge_id();
	bool uncrossable_edges_provided = (C_uncrossable_edges.size() > 0);
	gdt::gdtlist<gdtedge> C_spanned_edges;
	gdt::gdtlist<gdtedge> C_unspanned_edges;

        switch( uncrossable_edges_provided )
        	{
        	case true:
        		if( po == PRESERVE_EMBEDDING )
				//gdt_error ("PRESERVE_EMBEDDING and UNCROSSABLE_EDGE not handled");
				return false;
			else
				{
				// ug contains the whole *this: 
				// we have to delete edges not in C_uncrossable_edges
				//
				gdtedge e;
				gdt::gdtedge_array<bool> edges_to_save (ug.nodes_and_edges(),false);
				forall(e, C_uncrossable_edges)	edges_to_save[ug.corresponding_edge(e,*this)] = true;
				gdt::gdtlist<gdtedge> all_my_edges = ug.all_edges();
				forall(e, all_my_edges ) 
					if( edges_to_save[e] == false ) 
						{
						C_unspanned_edges.append(corresponding_edge(e,ug));
						
						// debugging W.D.
						//cout << "\ndeleting unspanned gdtedge " << ug.id(e) << flush;
						//
						
						ug.del_edge(e);
						}
				}
        		break;

        	case false:
        		{
        		// create a spanning tree: 
        		// 	1) make a copy of the original graph (with right IDs)
        		//	2) deletes edges unspanned by spanning tree
			//
			gdtedge e;
			spanning_tree( C_spanned_edges, C_unspanned_edges );
			forall( e, C_unspanned_edges )	ug.del_edge( ug.corresponding_edge(e, *this));
			}
        		break;
        	}

	#ifdef DEBUG
    std::cout << "Planarize_kernel after the first switch" << endl;
	std::cout << "ug= " << &ug << "\tnodes= " << ug.number_of_nodes() << "\tedges= " << ug.number_of_edges() << endl;
	#endif

	// the following 'switch' statement computes the set of edges that have to be inserted
	// in the planar graph through a minimum spanning tree on the dual graph.

	gdt::gdtlist<gdtedge> C_critical_edges;
	switch( po )
		{
		case PRESERVE_EMBEDDING:
			C_critical_edges = C_unspanned_edges;  	// all unspanned need to be inserted,
								// possibly provoking crossings.
			break;
		case DO_NOT_PRESERVE_EMBEDDING:

			if ( !uncrossable_edges_provided )
				{
				// we can insert three edges in a tree without losing planarity
				//
				for( int k = 1 ; (k < 4) && (!C_unspanned_edges.empty()) ; k++ )
					{
					gdtedge C_popped_out_edge = C_unspanned_edges.pop();
					gdtnode U_source = ug.corresponding_node(source(C_popped_out_edge), *this);
					gdtnode U_target = ug.corresponding_node(target(C_popped_out_edge), *this);
					gdtnode U_start =  ug.corresponding_node( start(C_popped_out_edge), *this);  // may be NULL_NODE

					gdtedge U_new_edge = ug.new_edge( U_source, U_target, id(C_popped_out_edge) );
					if( U_start != NULL_NODE ) 	ug.make_directed(U_new_edge, U_start);
					ug.mark( U_new_edge, markers(C_popped_out_edge) );
					}
				}

			// inserting edges while planarity is assured

			if( !ug.make_embedding_planar() ) return false;

			while( !C_unspanned_edges.empty() )
				{
				gdtedge C_popped_out_edge = C_unspanned_edges.pop();
				gdtnode U_source = ug.corresponding_node(source(C_popped_out_edge), *this);
				gdtnode U_target = ug.corresponding_node(target(C_popped_out_edge), *this);
				gdtnode U_start =  ug.corresponding_node( start(C_popped_out_edge), *this);  // may be NULL_NODE
				gdtedge U_just_inserted_edge = ug.new_edge(U_source, U_target, id(C_popped_out_edge) );

				// debugging W.D.
				//cout << "\ntrying to reinsert gdtedge " << (*this).id(C_popped_out_edge) << flush;
				//

				if( !ug.make_embedding_planar() )
					{
					ug.del_edge(U_just_inserted_edge);
					C_critical_edges.append(C_popped_out_edge);
					// debugging W.D.
					//cout << "\nedge not inserted" << flush;
					//
					}
				else
					{
					if( U_start != NULL_NODE ) 	ug.make_directed(U_just_inserted_edge, U_start);
					ug.mark( U_just_inserted_edge, markers(C_popped_out_edge) );
					// debugging W.D.
					//cout << "\nedge inserted" << flush;
					//
					}
				}
			// debugging W.D.
			//cout << "\nmax planar subgraph completed.. " << flush;
			//
			// now I have a maximal planar subgraph in ug (and a planar embedding too)
			break;
		}

	gdtedge edge_to_remove;
	forall (edge_to_remove,C_edge_to_remove_after_max_planar)
		{
		ug.del_edge(ug.corresponding_edge(edge_to_remove, *this));
		}

	// move components outside the wheels if there are any.
	//
		{
		gdtnode v;
		forall_nodes (v,ug)
			if (ug.is_marked(v,RM3_CENTER_OF_WHEEL))
				move_components_outside_wheel (ug,ug.id(v));
		}
	//
	#ifdef DEBUG
    std::cout << "Now plan_undi_graph will be invocated inside planarize_kernel" << endl;
	std::cout << "ug.number_of_nodes=" << ug.number_of_nodes() << "\tug.number_of_edges=" << ug.number_of_edges() << endl;
	#endif
	plan_undi_graph pug(ug);	// ug is planar and connected so I can translate it in a plan_undi_graph

	crossing_manager cm(*this, pug);  // from now on we need splits & crossing management
	gdtedge e;
	forall_edges( e, pug.nodes_and_edges() )
		cm.set_corresponding( corresponding_edge(e, pug), e);
	gdtnode v;
	forall_nodes( v, nodes_and_edges() )
		cm.set_corresponding_node_in_T( v, pug.corresponding_node(v, *this));

	pug.rise_max_edge_id( unused_edge_id );  // we don't want to use gdtedge IDs already
	                                         // used by *this in the split-generated edges


	//cout << "\nTHIS IS pug BEFORE CRITICAL EDGE INSERTION\n" << endl;
	//pug.print(COMPLETE);

	gdtedge C_edge_to_insert;
	forall( C_edge_to_insert, C_critical_edges )
		{
		gdtnode C_starting_node = source(C_edge_to_insert);
		gdtnode C_ending_node   = target(C_edge_to_insert);
		gdtnode P_starting_node = pug.corresponding_node(C_starting_node, *this);
		gdtnode P_ending_node   = pug.corresponding_node(C_ending_node,   *this);
		bool C_edge_to_insert_forward_passed  = (C_starting_node == start(C_edge_to_insert));
		bool C_edge_to_insert_backward_passed = (C_ending_node   == start(C_edge_to_insert));


		//cout << "I want to insert "<< id(C_edge_to_insert)
		//     << " (of SrcGraph) between " << pug.id(P_starting_node) 
		//     << " and " << pug.id(P_ending_node) << endl;

		// -----------------------
		// creating the dual graph
		// -----------------------
		// cout << "creating the dual graph" << endl;

		plan_undi_graph dual;
		gdt::gdtmap<face,gdtnode> primal_face_2_dual_node(NULL_NODE);
		gdt::gdtmap<gdtedge,gdtedge> primal_edge_2_dual_edge(NULL_EDGE);
	 	gdt::gdtmap<gdtnode,face> dual_node_2_primal_face(NULL_FACE);
		gdt::gdtmap<gdtedge,gdtedge> dual_edge_2_primal_edge(NULL_EDGE);
		
		// debugging W.D.
		//cout << "\nPRIMAL GRAPH" << flush;
		//pug.print(COMPLETE);
		//pug.write("primale.gdt");
		//
		
        	pug.make_dual(	dual,
        			primal_face_2_dual_node, 
        			dual_node_2_primal_face,
        			 
     				primal_edge_2_dual_edge, 
     				dual_edge_2_primal_edge	);

                // we will carry on the computation of shortest path on an undi_graph dual
                
		undi_graph dual_undi = dual;  				// making a copy (from plan_undi_graph to undi_graph)
                dual_undi.rise_max_edge_id( unused_edge_id );

		// modify the dual graph so to avoid the usage of uncrossable_edges: 
		// delete dual_undi edges corresponding to uncrossable_edges of *this
		//
		if( uncrossable_edges_provided )
			{
			gdtedge e;
			forall(e, C_uncrossable_edges)
				{
				gdtedge edge_to_delete = dual_undi.corresponding_edge(e,*this);
				if (edge_to_delete) dual_undi.del_edge(edge_to_delete);
				}
			}
		
		// modify the dual graph to fit the right algorithm (PRESERVE_EMBEDDING or DO_NOT_PRESERVE_EMBEDDING)
		// set the starting and ending faces for the shortest path in the dual graph
		// set the right edge_ip for the first gdtedge of the chain
		//
		gdtnode D_starting_node = NULL_NODE;
		gdtnode D_ending_node = NULL_NODE;
		int  P_from_node_edge_ref_id;	// this is the gdtedge after which (counter-clockwise) we will insert new gdtedge
		int  P_to_node_edge_ref_id;	// the same as above for the other end of new gdtedge
		
		gdt::gdtlist<face> P_adj_faces_of_start;
		gdt::gdtlist<face> P_adj_faces_of_end;
		gdt::gdtlist<gdtnode> DU_adj_nodes_of_start;
		gdt::gdtlist<gdtnode> DU_adj_nodes_of_end;
		face P_face_cursor;
		
		switch(po)
			{
			case PRESERVE_EMBEDDING:

				face P_faces[2];     // starting and ending faces

				cm.get_corresponding_face_and_ref_edge_id(	C_edge_to_insert, 
										C_starting_node, 
										P_faces[0], 		      // output
										P_from_node_edge_ref_id );    // output
				cm.get_corresponding_face_and_ref_edge_id(	C_edge_to_insert, 
										C_ending_node, 
										P_faces[1], 		// output
										P_to_node_edge_ref_id );         // output

				D_starting_node = dual_undi.new_node();
				D_ending_node   = dual_undi.new_node();

				dual_undi.new_edge( 	dual_undi.corresponding_node( primal_face_2_dual_node[P_faces[0]], dual ), 
				                    	D_starting_node 	);
				                    
				dual_undi.new_edge( 	dual_undi.corresponding_node( primal_face_2_dual_node[P_faces[1]], dual ), 
				                    	D_ending_node 		);
				                    
				break;			

			case DO_NOT_PRESERVE_EMBEDDING:
				
				P_adj_faces_of_start = pug.adj_faces( P_starting_node );
				P_adj_faces_of_end   = pug.adj_faces( P_ending_node );

                                forall( P_face_cursor, P_adj_faces_of_start )
                                	{
					DU_adj_nodes_of_start.append(
						dual_undi.corresponding_node( primal_face_2_dual_node[P_face_cursor], dual));
                                	}

                                forall( P_face_cursor, P_adj_faces_of_end )
                                	{
					DU_adj_nodes_of_end.append(
						dual_undi.corresponding_node( primal_face_2_dual_node[P_face_cursor], dual));
                                	}

				D_starting_node = dual_undi.new_node(DU_adj_nodes_of_start);
				D_ending_node   = dual_undi.new_node(DU_adj_nodes_of_end);

				// value for P_first_ip not available yet
				break;
			}


		gdt::gdtlist<gdtnode> DU_nodes_of_path;  // lists of dual_undi
		gdt::gdtlist<gdtedge> DU_edges_of_path;
		
		// debugging
		//cout << "\nCURRENT DUAL GRAPH" << flush;
		//dual_undi.print(COMPLETE);
		//dual_undi.write("duale.gdt");
		//cout << "unweighted_unoriented_shortest_path from " << dual_undi.id(D_starting_node) <<
		//        " and " << dual_undi.id(D_ending_node)<< " found ";		
		//
		
		// debugging W.D.		
		//cout << "\ntrying to reinsert gdtedge " << (*this).id(C_edge_to_insert) << flush;
		//
		if (
		    dual_undi.unweighted_unoriented_shortest_path
					(   	
					D_starting_node,  	// input
					D_ending_node,    	// input
					DU_nodes_of_path, 	// output
					DU_edges_of_path  	// output
					) == INFINITE
		   ) { std::cout << "\nedge not inserted " << flush; return false; }
		

		//cout << " edges in between:\t";
		//{
		//	gdtedge titto;
		//	forall(titto, DU_edges_of_path)
		//		{
		//		cout << dual_undi.id(titto) << " ";
		//		}
		//		cout << endl;
		//}
		//cout << " nodes in between:\t";
		//{
		//	gdtnode titto;
		//	forall(titto, DU_nodes_of_path)
		//		{
		//		cout << dual_undi.id(titto) << " ";
		//		}
		//		cout << endl;
		//}

		// --------------------------------------------------------------------------			
		// delete the dummy nodes (and edges) inserted to handle the face choosing in
		// the algorithm 
		// --------------------------------------------------------------------------
		
		DU_nodes_of_path.pop_back();	
		DU_edges_of_path.pop_back();
		DU_nodes_of_path.pop_front();	
		DU_edges_of_path.pop_front();

		face P_starting_face = NULL_FACE;
		switch(po)
			{
			case PRESERVE_EMBEDDING:
				break;
			case DO_NOT_PRESERVE_EMBEDDING:
				{
				P_starting_face = dual_node_2_primal_face[ 
								dual.corresponding_node( DU_nodes_of_path.head(), dual_undi)];
				gdtedge P_from_node_edge_ref = pug.edges_entering_node_while_moving_around_face( P_starting_node, P_starting_face ).head();
				P_from_node_edge_ref_id = pug.id(P_from_node_edge_ref);
				}
				break;
			}
	
		// pug.internal_consistency_check();

		//cout << "insert the chain" << endl;

		gdt::gdtlist<gdtedge> P_added_edges; 	// this will be updated during the 'for' execution with the inserted edges
						// so to state (at the end) the correspondence with original gdtedge

		face P_current_face = dual_node_2_primal_face[ 
						dual.corresponding_node( DU_nodes_of_path.head(), dual_undi) ];


		gdtnode P_previous_node = P_starting_node;
		int  P_prev_node_ref_edge_id = P_from_node_edge_ref_id;
		
		for( ; !DU_edges_of_path.empty() ; )
			{

			//cout << "number of gdtedge still to split " << DU_edges_of_path.size() << endl;
			//gdtedge titto;
			//forall(titto, DU_edges_of_path)
			//	{
			//	cout << dual_undi.id(titto) << " ";
			//	}
			//	cout << endl;
			
			if( !dual_edge_2_primal_edge.defined(dual.corresponding_edge( DU_edges_of_path.head(), dual_undi )) )
				{
				std::cout << "no primal gdtedge to split\n";
				exit(1);
				}

		       	gdtedge P_e_to_split = dual_edge_2_primal_edge[
		       				dual.corresponding_edge( DU_edges_of_path.pop(), dual_undi )];

		       	
		       	//cout << "splitting gdtedge " << pug.id(P_e_to_split) << endl;

	
		/*	                                *---- P_current_face
		//	                                |
		//                       *-----*-----*--|--*--------*-----*-----*-----*
		//                       |     |     |  |  |        |     |     |     |
		//                       | *---o-----o     | <-*    |     |     |     |  <-- line of edges to split
		//                       |/    |    /|     |    \   |     |     |     |
		//                       o-----*---/-*\----*-----\--*-----*-----*-----O
		//                       ^        /    \          \                    \
		//	                 |       /      \          \                    *--- P_ending_node
		//	P_starting_node--*      /        \          *---- P_e_to_split
		//	                       /          \
		//      P_previous_node-------*            *---- P_previous_ip (insertion point)
		*/

                        // assert(this->internal_consistency_check());

			gdtnode P_dummy_node;
			face P_f_current, P_f_next;
			gdtedge P_e_current, P_e_next;
                       	cm.split_edge(P_e_to_split, 
                                      P_dummy_node,     // output 
                                      P_f_current, 	// output
	                              P_e_current, 	// output
                                      P_f_next, 	// output
                                      P_e_next);	// output

			int P_e_current_id = pug.id(P_e_current);
			int P_e_next_id    = pug.id(P_e_next);

			if( P_f_current != P_current_face )	// swap 'current' with 'next'
				{
				face temp_face = P_f_current;
				P_f_current = P_f_next;
				P_f_next = temp_face;

				int temp_edge_id = P_e_current_id;
				P_e_current_id = P_e_next_id;
				P_e_next_id = temp_edge_id;

				}

		//                       --------------------O--------------------
		//                      |                    |                    |
		//                      | P_e_current_id --> |     P_f_next       |
		//                      |                    |                    |
		//  P_previous_node --> O                    O <-- P_dummy_node   |
		//                      |                    |                    |
		//  P_previous_ip ----> |    P_f_current     | <-- P_e_next_id    |
		//                      |                    |                    |
		//                       --------------------O--------------------


 			//pug.print(COMPLETE);
                        //cout << "P_previous_node = " << pug.id(P_previous_node) << endl;
                        //cout << "P_prev_node_ref_edge_id = " << P_prev_node_ref_edge_id << endl;
                        //cout << "P_dummy_node = " << pug.id(P_dummy_node) << endl;
                        //cout << "P_e_current_id = " << P_e_current_id << endl;
                        
			// if this is the first insertion you risk this configuration:
			//
			//   ---------------------------O--------------------
			//  |                           |                    |
			//  |     P_e_current_id -----> |     P_f_next       |
			//  |                           |                    |
			//  |         P_f_current       O <-- P_dummy_node   |
			//  |                           |                    |
			//  | P_prev_node_ref_edge_id-> | <-- P_e_next_id    |
			//  |                           |                    |
			//   ---------------------------O--------------------
			//                              ^
			//                              |
			//            P_previous_node --*
			//
			// in which the previous splitting of the gdtedge assigned the 
			// P_e_to_split id to P_e_current_id, so P_prev_node_ref_edge_id
			// is out of date.
			// 
                        if( P_prev_node_ref_edge_id == P_from_node_edge_ref_id )
                        	{
                        	switch(po)
                        		{
					case PRESERVE_EMBEDDING:
						{
	        	                	face P_dummy_face;
						cm.get_corresponding_face_and_ref_edge_id(	C_edge_to_insert, 
												C_starting_node, 
												P_dummy_face,			// dummy output
												P_prev_node_ref_edge_id );	// all to update this
		                        	}
		                        	break;

					case DO_NOT_PRESERVE_EMBEDDING:
						{
						gdtedge P_from_node_edge_ref = pug.edges_entering_node_while_moving_around_face( P_starting_node, P_starting_face ).head();
						P_prev_node_ref_edge_id = pug.id(P_from_node_edge_ref);
						}
						break;
					}
				}
					
			gdtedge P_new_edge = pug.new_edge(	P_previous_node, 
							pug.find_edge(P_prev_node_ref_edge_id), 
							P_dummy_node, 
							pug.find_edge(P_e_current_id) );

			
			pug.mark( P_new_edge, markers(C_edge_to_insert) );
			P_added_edges.push_back(P_new_edge);
			if( C_edge_to_insert_forward_passed )	pug.make_directed(P_new_edge, P_previous_node);
			if( C_edge_to_insert_backward_passed )	pug.make_directed(P_new_edge, P_dummy_node);


			// updating for the next iteration

			P_current_face = P_f_next;
			P_previous_node = P_dummy_node;
			P_prev_node_ref_edge_id = P_e_next_id;
			}

		
		// We are on the last face, now.
		
		face P_ending_face;
		switch(po)
			{
			case PRESERVE_EMBEDDING:
				{
				cm.get_corresponding_face_and_ref_edge_id(	C_edge_to_insert, 
										C_ending_node, 
										P_ending_face,           // dummy output
										P_to_node_edge_ref_id ); // all to update this
				}
				break;
				
			case DO_NOT_PRESERVE_EMBEDDING:
				{
				P_ending_face = P_current_face;
				gdtedge P_to_node_edge_ref = pug.edges_entering_node_while_moving_around_face( P_ending_node, P_ending_face ).head();
				P_to_node_edge_ref_id = pug.id(P_to_node_edge_ref);
				}
				break;
			}


		//pug.print(COMPLETE);
		//cout << "P_previous_node = " << pug.id(P_previous_node) << '\n';
		//cout << "P_prev_node_ref_edge_id = " << P_prev_node_ref_edge_id << '\n';
		//cout << "P_ending_node = " << pug.id(P_ending_node) << '\n';
		//cout << "P_to_node_edge_ref_id = " << P_to_node_edge_ref_id << '\n';


		gdtedge P_new_edge = pug.new_edge(	P_previous_node,
						pug.find_edge(P_prev_node_ref_edge_id), 
						P_ending_node,
						pug.find_edge(P_to_node_edge_ref_id),
						id(C_edge_to_insert) );

		pug.mark( P_new_edge, markers(C_edge_to_insert) );
		if( C_edge_to_insert_forward_passed )	pug.make_directed(P_new_edge, P_previous_node);
		if( C_edge_to_insert_backward_passed )	pug.make_directed(P_new_edge, P_ending_node);

		P_added_edges.push_back( P_new_edge );

		cm.set_corresponding(C_edge_to_insert, P_added_edges);
		pug.update_constraints_on_path_replacing_edge(C_edge_to_insert, *this, P_added_edges); 
		
		
		unused_edge_id = pug.get_max_edge_id();
		}
		
	*this = pug;
	return true;	
	}	
	


	
	
	gdt::gdtlist<gdtedge>
	undi_graph::
replace_edge_with_path
	(gdtedge e, gdt::gdtlist<int>& node_id_path, gdt::gdtlist<int>& edge_id_path)
	{
	gdt::gdtlist<gdtedge> path_edges;
	int id_v1,id_v2;
	int id_w;
	gdtnode v1 = NULL_NODE;
	gdtnode v2 = NULL_NODE;
	gdtnode v_ug,w_ug;
	gdtedge e_ug;
	gdt::list_item pos_edge = edge_id_path.first();
	id_v1 = node_id_path.head();
	id_v2 = node_id_path.tail();
	
	path_edges.clear();
	
	if (id(source(e)) == id_v1)  	 v1 = source(e);
	else if (id(target(e)) == id_v1) v1 = target(e);
		else gdt_error("invalid gdtedge");
		
	v2 = opposite (v1,e);
	
	// ----------------------------
	// add all nodes with path's id 
	// different by id_v1 and id_v2
	// ----------------------------
	//
	//cout << "\nREPLACING EDGE WITH NODES PATH "; node_id_path.print(); cout << flush;
	//cout << "\nAND EDGES PATH "; edge_id_path.print(); cout << "\n" << flush;
	v_ug = v1;
	e_ug = e;
	forall (id_w,node_id_path)
		{ 
		if ((id_w == node_id_path.inf(node_id_path.succ(node_id_path.first()))) && (id_w != id_v2)) 
			{
			w_ug = new_node (id_w);
			e_ug = new_edge (v_ug,e_ug,w_ug,edge_id_path.inf(pos_edge));
			pos_edge = edge_id_path.succ(pos_edge);
			path_edges.append (e_ug);
			v_ug = w_ug;
			}
		else if (id_w == id_v2)
			{ 
			e_ug = new_edge (v_ug,e_ug,v2,e,edge_id_path.inf(pos_edge));
			pos_edge = edge_id_path.succ(pos_edge);
			path_edges.append (e_ug);
			}
		     else if (id_w != id_v1)
			{
			w_ug = new_node (id_w);
			e_ug = new_edge (v_ug,w_ug,edge_id_path.inf(pos_edge));
			pos_edge = edge_id_path.succ(pos_edge);
			path_edges.append (e_ug);
			v_ug = w_ug;
			}
		}
		
	// ---------------------------
	// delete the gdtedge e in this
	// ---------------------------
	//
	del_edge (e);
	
	return path_edges;
	}	
	
	
	
	gdt::gdtlist<gdtedge>
	undi_graph::
replace_edge_with_path_of_graph
	(gdtedge re, gdt::gdtlist<gdtedge>& edge_path, undi_graph& ug)
	{
	gdtnode v1,v2;
	gdtnode v = NULL_NODE,
	     v_ug = NULL_NODE,
	     v1_ug= NULL_NODE,
	     v2_ug= NULL_NODE;
	gdtedge e,e_ug;
	gdt::gdtlist<gdtedge> edges_added;
	
	e_ug = edge_path.head();	
	v1 = source(re);
	if (id(v1) != ug.id(ug.source(e_ug)) && id(v1) != ug.id(ug.target(e_ug))) 
		{
		v1 = target(re);
		if (id(v1) != ug.id(ug.source(e_ug)) && id(v1) != ug.id(ug.target(e_ug))) 
			gdt_error("invalid path or gdtedge");
		}	
		
	v2 = opposite (v1,re);
	
	e_ug = edge_path.tail();
	if (id(v2) != ug.id(ug.source(e_ug)) && id(v2) != ug.id(ug.target(e_ug))) 
		gdt_error("invalid path or gdtedge");	
	
	// ----------------
	// case single gdtedge
	// ----------------
	
	gdtedge e1_ug = edge_path.head();
	gdtedge e2_ug = edge_path.tail();

	if (edge_path.size() == 1)
		{
		e = new_edge (v1,re,v2,re,ug.id(e_ug));
		edges_added.append(e);
		v_ug = ug.start(e_ug);
		if (v_ug) make_directed (e,corresponding_node(v_ug,ug));
		mark(e,ug.markers(e_ug));
		}
		
	// -----------------------
	// case more than one gdtedge
	// -----------------------
		
	else
		forall (e_ug,edge_path)
			{
			if (e_ug == e1_ug)	// first gdtedge of the path
				{
				v1_ug = ug.corresponding_node (v1,*this);
				v2_ug = ug.opposite(v1_ug,e_ug);
				v     = new_node (ug.id(v2_ug));
				int ide = ug.id(e_ug);
				e     = new_edge (v1,re,v,ide);
				}
				
			else if (e_ug != e2_ug) // medium gdtedge of the path
				{
				v2_ug = ug.opposite(v1_ug,e_ug);
				v     = new_node (ug.id(v2_ug));
				e     = new_edge (v1,v,ug.id(e_ug));
				}
				
			else			// last gdtedge of the path
				{
				v2_ug = ug.opposite(v1_ug,e_ug);
				v     = v2;
				e     = new_edge (v,re,v1,ug.id(e_ug));
				}
			
			edges_added.append(e);
			v_ug  = ug.start(e_ug);
			
			if (v_ug)
				{
				gdtnode v_temp = find_node (ug.id(v_ug)); 
				make_directed (e,v_temp);
				}
				
			mark(e,ug.markers(e_ug));
			v1    = v;
			v1_ug = v2_ug;			
			}
	
	// -----------------------------
	// copy all contraints from 
	// edge_path as much as possible
	// and delete the gdtedge re.
	// -----------------------------
	
	copy_constraints_from_subgraph_of_undi_graph (edge_path,ug);
	del_edge(re);
	
	return edges_added;
	}	
	

	
	void 
	undi_graph::
generate_plan_biconnected 
	(
	int num_nodes, 
	double prob_iv, 
	int max_degree, 
	bool multiple
	)
	{
	if (prob_iv <= 0 || prob_iv >= 1)
		gdt_error("invalid value for prob_iv");
	
	//cout << "\nGENERATING GRAPH WITH " << num_nodes << " NODES\n" << flush;
		
	double d;			// density (number of edges/ number of nodes)	
	prob_iv = prob_iv * 100;
	
	//do
		{
		double num_iv = 0;
		double num_ie = 0;
	
		// -----------------------------------------
		// Initialize the undi_graph with a triangle
		// -----------------------------------------
		//
		clear();
		
		bool insert_edge_failed = false;
		gdtnode v1 = new_node ();
		gdtnode v2 = new_node ();
		gdtnode v3 = new_node ();
		new_edge (v1,v2);
		new_edge (v2,v3);
		new_edge (v1,v3);
	
	
		// ----------------------------------
		// Make all the other nodes and edges
		// ----------------------------------
		
		while (num_iv < (num_nodes - 3))
			{
			// -------------------------------
			// Choose a new operation iv or ie
			// -------------------------------
			
			int n;
			
			if (insert_edge_failed) {n = 0; insert_edge_failed = false;}	// execute an insert vertex operation
			else 			 n = random (1,100);			// randomly extract an operation
		
			if (n <= prob_iv) 
				{
				// -----------------
				// Operation type iv
				// -----------------
				//
				//cout << "\nOPERATION TYPE iv" << flush;
				
				int   ide = random (0,number_of_edges()-1);
				gdtedge  e   = find_edge (ide);
				
				if (!e)
					gdt_error("fatal error, invalid gdtedge e");
				
				gdtnode  v1  = source(e);
				gdtnode  v2  = target(e);
				gdtnode  v   = new_node();
				gdtedge  e1  = new_edge(v1,v);
				gdtedge  e2  = new_edge(v2,v);
				del_edge(e);	
				if (id(e1) < id(e2)) assign_id(e2,ide);
				else		     assign_id(e1,ide);
				max_edge_id = gdt_maximum(id(e1),id(e2));
				//update_max_edge_id();
				num_iv ++;
				
				//cout << "\nADDED NEW EDGES: " << id(e1) << " " << id(e2) << flush;
				//cout << "\nADDED NEW NODE " << id(v) << flush;
				}
			else
				{
				// -----------------
				// Operation type ie
				// -----------------
				//
				//cout << "\nOPERATION TYPE ie" << flush;
				
				int k  = number_of_nodes();
				int k2 = k*k;
				int number_of_trials  = 0;
				bool found_nodes = false;
				
				while (number_of_trials<=k2 && !found_nodes)
				//while (!found_nodes)
					{
					int id1,id2;   
					gdtnode v1,v2;
					
					// -----------------
					// Choose a gdtnode v1
					// -----------------
					//
					//cout << "\nEXTRACTING NUMBER FROM 0 TO " << k-1 << flush;
					id1  = random   (0,k-1);
					v1   = find_node(id1);
					if (!v1) 
						gdt_error("fatal error, invalid gdtnode v1");
					
					// ----------------------
					// Choose a gdtnode v2 != v1
					// ----------------------
					//
					v2 = v1;
					while (v2 == v1)
						{
						id2  = random   (0,k-1);
						v2   = find_node(id2);
						if (!v2) 
						gdt_error("fatal error, invalid gdtnode v2");
						}
						
					//cout << "\n  gdtnode chosen: " << pug.id(v1) << " and " << pug.id(v2) << flush;
					
					// -----------------------------------------------------
					// Check if it is possible add an gdtedge between v1 and v2
					// -----------------------------------------------------
					//
					
					if (degree(v1) < max_degree && degree(v2) < max_degree)
						{
						// ----------------------------------------------
						// add a new gdtedge from v1 to v2 if the graph is
						// also planar after this adding
						// ----------------------------------------------
						//
						gdtedge e = new_edge(v1,v2);
						if (!make_embedding_planar())
							{
							del_edge (e);
							max_edge_id --;
							//update_max_edge_id();
							number_of_trials++;
							}
						else 
							{
							num_ie ++;
							found_nodes = true;
							//cout << "\nADDED AN EDGE : " << id(e) << flush;
							}
						}
						
					else number_of_trials++;
					}
				if (number_of_trials >= k2) insert_edge_failed = true;
				}
			}
		
		if (!multiple)
			{
			// -------------------------
			// remove all multiple edges
			// -------------------------

			gdtnode v;
			gdtedge e;
			forall_nodes (v,*this)
				{
				gdt::gdtnode_map<bool> mult (nodes_and_edges(),false);
				gdt::gdtlist<gdtedge> E = adj_edges(v);
				forall (e,E)
					{
					gdtnode w = opposite(v,e);
					if (!mult[w]) mult[w] = true;
					else 
						{
						//cout << "\nREMOVING EDGE " << id(e) << flush;
						del_edge(e);
						}
					}
				}
			}
		
		
		d = ((double)number_of_edges()/((double)number_of_nodes()));
		//cout << "\nOPERATIONS insert gdtnode = " << num_iv << flush;
		//cout << "\nOPERATIONS insert gdtedge = " << num_ie << flush;
		//cout << "\ndensity = " << d << flush;
		}
	}
	


	void 
	undi_graph::
disable_keep_ordering_of_directed_edges ()
	{
	this->keep_ordering_of_directed_edges = false;
	}



	void 
	undi_graph::
enable_keep_ordering_of_directed_edges ()
	{
	gdtnode v;
	this->keep_ordering_of_directed_edges = true;
	forall_nodes (v,*this)
		{
		order_out_edges(v);
		order_in_edges(v);
		}
	}




	// -------------------------
	// Input / output operations
	// -------------------------


	bool
	undi_graph::
write
	(std::string file_name)
	{
	// open a file "file_name" and write the undi_graph in this file.
	// Return true if the operation is correctly executed
	//
	ofstream out (file_name.c_str());
	if (!out) return false;
	append_section_to_fstream (out);
	out.close();
	return true;
	}



	bool
	undi_graph::
read
	(std::string file_name)
	{
	// open a file "file_name" and read the undi_graph from this file.
	// Return true if the operation is correctly executed
	//

	ifstream in (file_name.c_str());
	if (!in) return false;
	read_section_from_fstream (in);
	in.close();
	return true;
	}


	
	void
	undi_graph::
append_section_to_fstream 
	(ofstream& out)
	{
	// write the undi_graph section in out, i.e. the adjacent lists
	// forall nodes and direction of their edges
	
	#ifdef NSAVE
	 std::cout << "\nTHIS IS A DEMO VERSION OF GDT."
	      << "\nYOU CANNOT SAVE DATA!"
	      << "\nTO SAVE DATA YOU NEED A COMMERCIAL LICENSE" << flush;
	 exit(1);
	#endif
		
	gdtedge e;
	gdtnode v,s;
	
	out << "\n<UNDISECTION>";
	out << "\n\t<NODELIST>";	
	forall_nodes (v,*this)
		{
		out << "\n\t\t<NODE>";
		//
		out << "\n\t\t\t" << id(v);
		forall_edges_adjacent_node (e,v,*this) 
			{
			s = start(e);
			out << "\n\t\t\t<EDGE> ";
			out << id(e) << " ";
			if (!s)        out << "--";
			else if (v==s) out << "->";
			else           out << "<-";
			}
		out << "\n\t\t</NODE>";
		}
	out << "\n\t</NODELIST>";
	out << "\n</UNDISECTION>";
	}
	
	
	
	void
	undi_graph::
read_section_from_fstream
	(ifstream& in)
	{
	// read the undi_graph section from in, i.e read the adj. lists
	// forall nodes and build the corresponding ug.
	
	gdtnode v;
	gdtedge e;

	int    nid; // Node Identifier
	int    eid; // Edge Identifier
	std::string dir; // Direction along gdtedge
	std::string tag;

	typedef gdt::gdtlist<int>   int_list;
	int_list            edges_list;
	gdt::gdtmap<gdtnode,int_list>  adj_list;

	gdt::gdtmap<int,gdtnode> 	   V1     (NULL_NODE);	// extreme 1 of gdtedge 
	gdt::gdtmap<int,gdtnode> 	   V2     (NULL_NODE);	// extreme 2 of gdtedge
	gdt::gdtmap<int,gdtnode> 	   SN     (NULL_NODE);	// start gdtnode of gdtedge when gdtedge is directed
	gdt::gdtmap<int,gdt::list_item> V1_pos (NULL);	// pos into V1 adjacent list
	gdt::gdtmap<int,gdt::list_item> V2_pos (NULL);	// pos into V2 adjacent list

	local_renew();	// clear this undi_graph

	in >> tag; if (tag !="<UNDISECTION>") gdt_error("UNDISECTION not found");
	in >> tag; if (tag !="<NODELIST>")    gdt_error("NODELIST not found");
	in >> tag; // <NODE> || </NODELIST>
	while (tag == "<NODE>")
		{
		in >> nid;     //cout << "\n" << nid << flush;
		in >> tag;     //cout << "\n" << tag << flush; // <EDGE> || </NODE>

		v = new_node (nid);

		while (tag == "<EDGE>")
			{
			in >> eid;   //cout << "\n" << eid << flush;
			in >> dir;   //cout << " "  << dir << flush;

			if (dir == "->") SN[eid]=v;
			gdt::list_item pos = adj_list[v].append(eid);
			if (!V1[eid]) { V1[eid]=v; V1_pos[eid] = pos; edges_list.append(eid); }
			else          { V2[eid]=v; V2_pos[eid] = pos; }

			in >> tag;    //cout << "\n" << tag << flush; // <EDGE>||</NODE>
			}
		in >> tag;            //cout << "\n" << tag << flush; // <NODE>||</NODELIST>
		}
	in >> tag;                    //cout << "\n" << tag << flush; // </UNDISECTION>

	gdt::gdtmap<int,gdtedge> corr_edge (NULL_EDGE); 
	
	forall (eid,edges_list)
		{
		gdtnode v1 = V1[eid];
		gdtnode v2 = V2[eid];
		gdt::list_item pos;
		int eid1,eid2;
		
		if (degree(v1) > 1)
			if (degree(v2) > 1)
				{
				pos  = adj_list[v1].cyclic_pred (V1_pos[eid]);
				eid1 = adj_list[v1].inf(pos);
				while (!corr_edge[eid1]) 
					{
					pos  = adj_list[v1].cyclic_pred (pos);
					eid1 = adj_list[v1].inf(pos);
					}
				pos  = adj_list[v2].cyclic_pred (V2_pos[eid]);
				eid2 = adj_list[v2].inf(pos);
				while (!corr_edge[eid2])
					{
					pos  = adj_list[v2].cyclic_pred (pos); 
					eid2 = adj_list[v2].inf(pos);
					}
				e = new_edge (v1,corr_edge[eid1],v2,corr_edge[eid2],eid);
				}
			else
				{
				pos  = adj_list[v1].cyclic_pred (V1_pos[eid]);
				eid1 = adj_list[v1].inf(pos);
				while (!corr_edge[eid1]) 
					{
					pos  = adj_list[v1].cyclic_pred (pos); 
					eid1 = adj_list[v1].inf(pos);
					}
				e = new_edge (v1,corr_edge[eid1],v2,eid);
				}
		else
			if (degree(v2)>1)
				{
				pos  = adj_list[v2].cyclic_pred (V2_pos[eid]);
				eid2 = adj_list[v2].inf(pos);
				while (!corr_edge[eid2])
					{
					pos  = adj_list[v2].cyclic_pred (pos); 
					eid2 = adj_list[v2].inf(pos);
					}
				e = new_edge (v2,corr_edge[eid2],v1,eid);
				}
			else    
				{
				e = new_edge(v1,v2,eid);
				}
		corr_edge[eid] = e;
		if (SN[eid]) make_directed (e,SN[eid]);					
		}
	}
	
	

	void
	undi_graph::
print
	(print_mode mode, std::ostream& os) const
	{
	gdtnode       v,v1;
	gdtedge 	   e;
	switch (mode)
		{
		case BY_NODES:
			{
			os << "\n";
			forall_nodes(v,*this)
				{
				print(v, os);
				os << " : ";
				//Modifed by A.M. 10/2002
				gdtedge f;
				forall_edges_adjacent_node (f,v,*this)
					{
					v1 = opposite(v,f);
					print(v1, os);
					os << " ";
					}
				os << "\n" << flush;
				}
			}
			break;
		case BY_EDGES:
			{
			os << "\n";
			forall_edges(e,*this)
				if (edge_is_directed(e))
					{
					print(start(e), os);
					print(e, os);			
					os << ">";
					print(stop(e), os);
					os << "\n" << flush;
					}
				else
					{
					print(source(e), os);
					print(e, os);
					print(target(e), os);
					os << "\n" << flush;
					}
			}
			break;
		case COMPLETE:
			{
			os << "\n";
			forall_nodes(v,nodes_and_edges())
				{
				print(v, os); os << " : ";
				forall_edges_adjacent_node (e,v,*this)
					{
					if (edge_is_directed(e))
						if (v==start(e))
							{
							print(e, os);
							os << ">";
							print(stop(e), os);
							os << " ";
							}
						else
							{
							os << "<";
							print(e, os);
							print(start(e), os);
							os << " ";
							}
					else
						{
						print(e, os);
						print(opposite(v,e), os);
						os << " ";
						}
					}
				os << "\n" << flush;
				}
				
			os << "\nconstraint list : ";
			print_constraints(os);
			std::cout << "\n" << std::flush;
			}
			break;
		case FOR_DEBUG: //Added by Marcandalli
			{
			gdtnode n;
			os << "Nodes" << endl;			
			forall_nodes(n, *this) {
				os << "(" << n << "," << id(n) << ") ";
			}
			os << endl << "Edges" << endl;
			gdtedge e;
			forall_edges(e, *this) {
				os << "(" << e << "," << id(e) << ", from " << id(source(e)) << " to " << id(target(e)) << ")" << endl;
			}
			}
			break;
		default:
			break;
			
		}
	}
						



	void
	undi_graph::
print
	(gdtnode v, std::ostream& os) const
	{
	os << "(" << id(v) << ")";
	}



	void
	undi_graph::
print
	(gdtedge e, std::ostream& os) const
	{
	os << "-" << id(e) << "-";
	}



	void
	undi_graph::
print
	(constraint c, std::ostream& os) const
	{
	c->print(os);
	}



	void
	undi_graph::
print_constraints_on_node
	(gdtnode v, std::ostream& os)
	{
	gdt::gdtlist<constraint>& Lv = all_constraints(v);
	constraint c;
	forall (c,Lv) print(c,os);
	}



	void
	undi_graph::
print_constraints_on_edge
	(gdtedge e, std::ostream& os)
	{
	gdt::gdtlist<constraint>& Le = all_constraints(e);
	constraint c;
	forall (c,Le) print(c,os);
	}
	
	
	
	void
	undi_graph::
print_constraints
	(std::ostream& os) const
	{
	constraint c;
	forall_constraints (c,*this) print(c,os);
	}



	// --------
	// Checkers
	// --------
				
	bool
	undi_graph:: 
internal_consistency_check
	() const
	{
	// -----------------------------------
	// the current version only checks the
	// consistency of gdtnode/gdtedge adj_list,
	// and the consistency of source/target
	// for each gdtedge
	// ------------------------------------
	
	gdtnode v;
	forall_nodes (v,nodes_and_edges())
		{
		struct_undi_node_info& vi = (*node_info)[v];
		gdt::gdtlist<gdtedge>& Lv = vi.adj_edges;

		gdt::list_item it_v;
		forall_items (it_v,Lv)
			{
			gdtedge e_it = Lv.inf(it_v);
			struct_undi_edge_info& e_iti = (*edge_info)[e_it];

			if ((e_iti.source != v) && (e_iti.target != v))
				{
				std::cout << "\nINTERNAL ERROR: " << flush;
				std::cout << "\n  gdtedge e=" << e_it << " with id=" << e_iti.id << flush;
				std::cout << "\n  should have gdtnode v=" << v << " with id=" << vi.id << " as extremal gdtnode" << flush;
				gdt_error("gdtedge with invalid source or target");
				}

			if (e_iti.source == v)
				{
				gdt::list_item pos_in_source_L = e_iti.pos_into_source_adj_edges;
				if (pos_in_source_L != it_v)
					{
					std::cout << "\nINTERNAL ERROR: " << flush;
					std::cout << "\n  gdtedge e=" << e_it << " with id=" << e_iti.id << flush;
					std::cout << "\n  should have position=" << it_v << " instead of position="
					     << pos_in_source_L << " in its source adj_edge list" << flush;
					gdt_error("gdtedge with invalid position in source list");
					}
				}
			if (e_iti.target == v)
				{
				gdt::list_item pos_in_target_L = e_iti.pos_into_target_adj_edges;
				if (pos_in_target_L != it_v)
					{
					std::cout << "\nINTERNAL ERROR: " << flush;
					std::cout << "\n  gdtedge e=" << e_it << " with id=" << e_iti.id << flush;
					std::cout << "\n  should have position=" << it_v << " instead of position="
					     << pos_in_target_L << " in its target adj_edge list" << flush;
					gdt_error("gdtedge with invalid position in target list");
					}

				}
			}
		}
	gdtedge e;
	forall_edges (e,nodes_and_edges())
		{
		struct_undi_edge_info& ei=(*edge_info)[e];
		gdtnode sn=ei.source;
		gdtnode tn=ei.target;
		int found_s=0;
		int found_t=0;
		gdtnode temp_node;


		forall(temp_node, *nodes) // check if source and target nodes are in graph nodelist
			{
			if (temp_node==sn) found_s++;
			if (temp_node==tn) found_t++;
			}
		if (found_s!=1 || found_t!=1)
			{
			std::cout << "\nINTERNAL ERROR: " << flush;
			std::cout << "\n  gdtedge e=" << e << " with id=" << ei.id << flush;
			std::cout << "\n  has inconsistency in source or target node" << flush;
					gdt_error("gdtedge with invalid source or target node");
			}
		struct_undi_node_info& sni=(*node_info)[sn];
		struct_undi_node_info& tni=(*node_info)[tn];
		gdt::gdtlist<gdtedge> sn_adj_edges=sni.adj_edges;
		gdt::gdtlist<gdtedge> tn_adj_edges=tni.adj_edges;

		gdtedge temp_edge;
		int found_es=0;
		forall(temp_edge,sn_adj_edges)
			{
			if (temp_edge==e) found_es++;
			}
		if (found_es==0)
			{
			std::cout << "\nINTERNAL ERROR: " << flush;
			std::cout << "\n  gdtedge e=" << e << " with id=" << ei.id << flush;
			std::cout << "\n  is not present in source node " << id(sn) << " adj list." << flush;
					gdt_error("gdtedge not present in source node adj list");
			}
		if (found_es>1)
			{
			std::cout << "\nINTERNAL ERROR: " << flush;
			std::cout << "\n  gdtedge e=" << e << " with id=" << ei.id << flush;
			std::cout << "\n  is present more than once in source node " << id(sn) << " adj list." << flush;
					gdt_error("gdtedge present more than once in source node adj list");
			}
		int found_ts=0;
		forall(temp_edge,tn_adj_edges)
			{
			if (temp_edge==e) found_ts++;
			}
		if (found_ts==0)
			{
			std::cout << "\nINTERNAL ERROR: " << flush;
			std::cout << "\n  gdtedge e=" << e << " with id=" << ei.id << flush;
			std::cout << "\n  is not present in target node " << id(tn) << " adj list." << flush;
					gdt_error("gdtedge not present in target node adj list");
			}
		if (found_ts>1)
			{
			std::cout << "\nINTERNAL ERROR: " << flush;
			std::cout << "\n  gdtedge e=" << e << " with id=" << ei.id << flush;
			std::cout << "\n  is present more than once in target node " << id(tn) << " adj list." << flush;
					gdt_error("gdtedge present more than once in target node adj list");
			}
		gdt::list_item pos_in_source_list = ei.pos_into_source_adj_edges;
		gdt::list_item pos_in_target_list = ei.pos_into_target_adj_edges;
		if (sn_adj_edges.inf(pos_in_source_list)!=e)
			{
			std::cout << "\n gdtedge e=" << e << " with id=" << id(e) << flush;
			std::cout << "\n has invalid position in source node adj list" << flush;
			gdt_error("Invalid position in source node adj list");
			}
		if (tn_adj_edges.inf(pos_in_target_list)!=e)
			{
			std::cout << "\n gdtedge e=" << e << " with id=" << id(e) << flush;
			std::cout << "\n has invalid position in target node adj list" << flush;
			gdt_error("Invalid position in target node adj list");
			}
		} // end of forall_edges
	return true;
	}



// -----------------------------------------------------------------------------
// 	PROTECTED
// -----------------------------------------------------------------------------

	void
	undi_graph::
set_source
	(gdtedge e, gdtnode new_source)
	{
	gdt::list_item ps,pt;
	gdtnode old_source;
	gdtnode old_target;
	gdtnode new_target;

	//cout << "\nundi_graph::set_source" << flush;

	old_source = source(e);
	old_target = target(e);

	if (new_source != old_source)
		{
		if (new_source != old_target) gdt_error("invalid gdtnode or gdtedge"); 
		else 
			{
			new_target = old_source;
			(*edge_info)[e].source = new_source;
			(*edge_info)[e].target = new_target;
			ps = (*edge_info)[e].pos_into_source_adj_edges;
			pt = (*edge_info)[e].pos_into_target_adj_edges;
			(*edge_info)[e].pos_into_source_adj_edges = pt;
			(*edge_info)[e].pos_into_target_adj_edges = ps;
			}
		}
	}



	void
	undi_graph::
update_constraints_after_del_edge
	(gdtedge e)
	{
	constraint c;
	gdt::gdtlist<constraint> Le = all_constraints(e); // copy gdtedge constraint-list
	forall (c,Le) 
		if (c -> update_after_del_edge(e)) del_constraint(c);
	}

	
	
	void
	undi_graph::
update_constraints_after_del_node
	(gdtnode v)
	{
	constraint c;
	gdt::gdtlist<constraint> Lv = all_constraints(v); // copy gdtnode constraint-list
	forall (c,Lv) 
		if (c -> update_after_del_node(v)) del_constraint(c);
	}



	void
	undi_graph::
update_constraints_after_split_edge 
	(gdtedge e, gdtedge e1, gdtedge e2)
	{
	constraint c;
	gdt::gdtlist<constraint> Le = all_constraints(e); // copy gdtedge constraint-list
	forall (c,Le) 
		if (c -> update_after_split_edge(e,e1,e2)) del_constraint(c);	
	}
		
	
		
	void 
	undi_graph::
update_constraints_after_split_node 
	(gdtnode v, gdtnode v1, gdtnode v2)
	{
	constraint c;
	gdt::gdtlist<constraint> Lv = all_constraints(v); // copy gdtnode constraint-list
	forall (c,Lv)
		if (c -> update_after_split_node(v,v1,v2)) del_constraint(c);
	}



	void 
	undi_graph::
update_constraints_after_merge_edges 
	(gdtedge e1, gdtedge e2, gdtedge e)
	{
	constraint c;
	gdt::gdtlist<constraint> Le1 = all_constraints(e1); // copy gdtedge constraint-list
	forall (c,Le1) 
		if (c -> update_after_merge_edges(e1,e2,e)) del_constraint(c);
		
	// added on 16 June 1999
	//
	gdt::gdtlist<constraint> Le2 = all_constraints(e2); // copy gdtedge constraint-list
	forall (c,Le2) 
		if (c -> update_after_merge_edges(e2,e1,e)) del_constraint(c);
	}
	

	
	void 
	undi_graph::
update_constraints_after_merge_nodes 
	(gdtnode v1, gdtnode v2, gdtnode v)
	{
	constraint c;
	gdt::gdtlist<constraint> Lv1 = all_constraints(v1); // copy gdtnode constraint-list
	forall (c,Lv1) 
		if (c -> update_after_merge_nodes(v1,v2,v)) del_constraint(c);
		
	// added on 16 June 1999
	//
	gdt::gdtlist<constraint> Lv2 = all_constraints(v2); // copy gdtedge constraint-list
	forall (c,Lv2)
		if (c -> update_after_merge_nodes(v2,v1,v)) del_constraint(c);
	}
 		


	void 
	undi_graph::
update_constraints_after_edge_translation 
	(
	gdtedge e, gdtnode ve1, gdtnode ve2, 
	gdtedge d, gdtnode vd1, gdtnode vd2
	)
	{
	constraint c;
	gdt::gdtlist<constraint> Le = all_constraints(e); // copy gdtedge constraint-list
	forall (c,Le)
		c -> update_after_edge_translation(e,ve1,ve2,d,vd1,vd2);
	}



	void 
	undi_graph::
update_constraints_on_path_replacing_edge
	(
	gdtedge e,
	undi_graph& ug,
	gdt::gdtlist<gdtedge> path
	)
	{
	gdtnode ue   = ug.source(e);				
	gdtnode ve   = ug.target(e);				
	int  idue = ug.id(ue);
	int  idve = ug.id(ve);
	gdtnode ud   = find_node (idue);
	gdtnode vd   = find_node (idve);
	//
	//            ue    e    ve
	//             o---------o
	//          (idue)     (idve)

	gdtnode v1p = NULL_NODE;	// initial extremal gdtnode of path of *this
	gdtnode v2p = NULL_NODE;	// final   extremal gdtnode of path of *this

	int size = path.size();

	if (size==0)	
		gdt_error("void path!");
	else if (size == 1)
		{
		gdtedge e1	= path.head();
		v1p     = source(e1);
		v2p     = target(e1);
		}
	else
		{
		gdtedge e1      = path.head();				// first gdtedge of path
		gdtedge e2      = path.tail();				// last gdtedge of path
		gdtedge e1_succ = path.inf(path.succ(path.first()));	// successive gdtedge of the first one of path
		gdtedge e2_pred = path.inf(path.pred(path.last ()));	// previous gdtedge of the last one of path
		gdtnode v1      = node_between_adj_edges(e1,e1_succ);
		gdtnode v2      = node_between_adj_edges(e2,e2_pred);
		v1p     = opposite(v1,e1);				// initial extremal gdtnode of path
		v2p     = opposite(v2,e2);				// final   extremal gdtnode of path
		//
		//
		//    *this       e1  e1_succ             e2_pred  e2
		//     path:    o----o--------o-- ..... -o-------o----o
		//              ^    ^                           ^    ^
		//              |    |                           |    |
		//
		//             v1p   v1                          v2   v2p
		//
		
		}

	if ((ud == v1p && vd == v2p) ||
	    (ud == v2p && vd == v1p)) 	
		{	
		constraint c;
		gdt::gdtlist<constraint> Le = ug.constraints_on_edge(e); // copy gdtedge constraint-list
		forall (c,Le)
			c -> update_on_path_replacing_edge (e,path,*this);
		}
	else 		
		gdt_error("invalid gdtedge e or path p!");
	}




	void
	undi_graph::
copy_constraints_from_subgraph_of_undi_graph
	(
	gdt::gdtlist<gdtedge>& sub,
	undi_graph& ug
	)
	{
	gdtnode v;
	gdtedge e;
	gdt::gdtlist<gdtedge>& Le = sub;		// edges of subgraph
	gdt::gdtlist<gdtnode>  Lv;			// nodes of subgraph
	
	// ---------------------------------
	// store in Lv all nodes of subgraph
	// ---------------------------------
	
	gdt::gdtnode_array<bool> stored (ug.nodes_and_edges(),false);
	
	forall (e,Le)
		{
		gdtnode v1 = ug.source(e);
		gdtnode v2 = ug.target(e);
		if (!stored[v1]) Lv.append(v1);
		if (!stored[v2]) Lv.append(v2);
		stored[v1] = true;
		stored[v2] = true;
		}
	
	// ------------------------------------
	// copy all constraints involving
	// edges of Le or nodes of Lv, 
	// avoiding duplications of constraints
	// ------------------------------------
	
	constraint c;
	gdt::gdtmap<constraint,bool> copied (false);
	
	forall (e,Le)
		{
		gdt::gdtlist<constraint> Lce = ug.constraints_on_edge (e);
		forall (c,Lce)
			if (!copied[c]) 
				{
				new_constraint(c);
				copied[c] = true;
				}
		}
		
	forall (v,Lv)
		{
		gdt::gdtlist<constraint> Lcv = ug.constraints_on_node (v);
		forall (c,Lcv)
			if (!copied[c])
				{
				new_constraint(c);
				copied[c] = true;
				}
		}
	

	}



	gdt::gdtlist<gdtedge> 
	undi_graph::
path_corresponding_to_edge 
	(
	gdtedge e, 
	undi_graph& ug
	) const
	{
	gdt::gdtlist<gdtedge> output_list;
	int e_id = ug.id(e);
	gdtedge first_edge;

	first_edge = find_edge(e_id);
	if( first_edge )
		{
		output_list.push_back(first_edge);
		gdtedge edge_cur = first_edge;
		gdtnode node_cur;

		node_cur = source(first_edge);
		while ( is_marked (node_cur, RM3_CROSS_ON_REAL_EDGE) )
			{
			edge_cur = cyclic_adj_succ (edge_cur, node_cur);
			edge_cur = cyclic_adj_succ (edge_cur, node_cur);
			output_list.push_back (edge_cur);
			node_cur = opposite(node_cur, edge_cur);
			}
		
		node_cur = target(first_edge);     
		while ( is_marked (node_cur, RM3_CROSS_ON_REAL_EDGE) )
			{
			edge_cur = cyclic_adj_succ (edge_cur, node_cur);
			edge_cur = cyclic_adj_succ (edge_cur, node_cur);
			output_list.push_front (edge_cur);
			node_cur = opposite(node_cur, edge_cur);
			}
		}
	return output_list;
	}


	int
	undi_graph::
count_not_dummy_nodes
	() const
	{
	int i = 0;
	gdtnode v;
	forall_nodes (v,*this)
		if (markers(v).empty()) i++;
		
	return i;
	}



// -----------------------------------------------------------------------------
// 	PRIVATE
// -----------------------------------------------------------------------------

	void
	undi_graph::
local_new
	()
	{
	// WARNING: this function inizializes a new structure
	//          for the undi_graph ug, but it does not delete
	//          the old structure in memory.
	//
	//cout << "\n undi_graph::local_new \n" << flush;
	
	//Modified by A.M. 10/2002
	//g = new (graph);
	
	//Old code
	//g->make_undirected();
	
	//Added by A.M. 2/2002
	nodes = new gdt::gdtlist<gdtnode>;
	edges = new gdt::gdtlist<gdtedge>;
	//

	node_info = new gdt::gdtnode_map<struct_undi_node_info>(*this);
	edge_info = new gdt::gdtedge_map<struct_undi_edge_info>(*this);
	//node_info->init(nodes_and_edges());
	//edge_info->init(nodes_and_edges());
	
	constraints = new gdt::gdtlist<constraint>;
	
	//cout << "********************" << endl;
	//cout << "this=" << this << "\tthis->costraints=" << this->constraints << endl;
	debug_pointer1= constraints;
	debug_pointer2= nodes;
	debug_pointer3= edges;
	
	node_with_id       = new gdt::gdtmap<int,gdtnode>(NULL_NODE);
	edge_with_id 	   = new gdt::gdtmap<int,gdtedge>(NULL_EDGE);
	constraint_with_id = new gdt::gdtmap<int,constraint>(NULL_CONSTRAINT);
	
	max_node_id	  = NULL_ID;
	max_edge_id	  = NULL_ID;
	max_constraint_id = NULL_ID;
	
	keep_ordering_of_directed_edges = true;
	}	
	
		

	void
	undi_graph::
local_del
	()
	{
	//cout << "\n undi_graph::local_del \n" << flush;
	//Modified by A.M. 10/2002
	//safe_delete(g);
	//Added by A. M. 2/2002
	
	//Added by Pizzo & PJ 20/09/2006
	// Constraints should be deleted before nodes, since the destructor of constraints deletes 
	// the reference in ug, too!
	gdt::gdtstack<constraint> S;
	constraint c;
	forall_constraints(c,*this)
		S.push(c);
	while (!S.empty())
		del_constraint(S.pop());
		
	while (!nodes->empty()) delete (nodes->pop_front());
	while (!edges->empty()) delete (edges->pop_front());
	
	
	// Modified by Pier Francesco Cortese
	
	/*if (!internal_consistency_check())
		gdt_error("Errore in local_del, grafo incoerente");
	gdtnode n;
	gdt::gdtstack<gdtnode> S;
	
	forall_nodes(n,*this)
		S.push(n);
	while (!S.empty()) 
	    del_node(S.pop());
	*/	
	//	
	
	delete(nodes);
	delete(edges);
	delete(node_info);
	delete(edge_info);
	
	delete(constraints);
	delete(node_with_id);
	delete(edge_with_id);
	delete(constraint_with_id);
	}
	
	
	
	void
	undi_graph::
local_renew
	()
	{
	local_del();
	local_new();
	}



	void
	undi_graph::
local_init
	()
	{
	// Intentionally left blank,
	// nothing to initialize in this class
	}



	void
	undi_graph::
local_set
	(
	gdt::gdtlist<gdtnode>*	p,
	gdt::gdtlist<gdtedge>*	p0,
	//graph*                           p1,
	gdt::gdtnode_map<struct_undi_node_info>* p2,
	gdt::gdtedge_map<struct_undi_edge_info>* p3,
	gdt::gdtlist<constraint>*		 p4,
	gdt::gdtmap<int,gdtnode>*			 p5,
	gdt::gdtmap<int,gdtedge>*			 p6,
	gdt::gdtmap<int,constraint>*		 p7,
	int                              p8,
	int                              p9,
	int                              p10,
	bool				 p11
	)
	{
	///////////////////////
	// Added by Pier Francesco Cortese 18/09/2006
	delete nodes;
	delete edges;
	delete node_info;
	delete edge_info;
	delete constraints;
	delete node_with_id;
	delete edge_with_id;
	delete constraint_with_id;
	//////////////
	
	nodes = p;
	edges = p0;
	//g			= p1;
	node_info		= p2;
	edge_info		= p3;
	constraints		= p4;
	node_with_id		= p5;
	edge_with_id		= p6;
	constraint_with_id	= p7;
	max_node_id		= p8;
	max_edge_id		= p9;
	max_constraint_id	= p10;
	keep_ordering_of_directed_edges = p11;	
	}

	

	void
	undi_graph::
order_in_edges
	(gdtnode v)
	{
	bool& keep = this->keep_ordering_of_directed_edges;
	bool& mark = (*node_info)[v].in_edges_are_ordered;

	if (keep && !mark)
		{
		gdtedge e;
		gdt::list_item pos;
		gdt::gdtlist<gdtedge>& in_edges = (*node_info)[v].in_edges;
		in_edges.clear();
		forall_edges_adjacent_node (e,v,*this)
			if (v==stop(e))
				{
				pos = in_edges.append(e);
				(*edge_info)[e].pos_into_in_edges = pos;
				}
		mark = true;
		}
	}




	void
	undi_graph::
order_out_edges
	(gdtnode v)
	{
	bool& keep = this->keep_ordering_of_directed_edges;
	bool& mark = (*node_info)[v].out_edges_are_ordered;
		
	if (keep && !mark)
		{
		gdtedge e;
		gdt::list_item pos;
		gdt::gdtlist<gdtedge>& out_edges = (*node_info)[v].out_edges;
		out_edges.clear();
		forall_edges_adjacent_node (e,v,*this)
			if (v==start(e))
				{
				pos = out_edges.append(e);
				(*edge_info)[e].pos_into_out_edges = pos;
				}
		mark = true;
		}
	}



	void
	undi_graph::
mark_in_edges_as_not_ordered
	(gdtnode v)
	{
	(*node_info)[v].in_edges_are_ordered = false;
	}



	void
	undi_graph::
mark_out_edges_as_not_ordered
	(gdtnode v)
	{
	(*node_info)[v].out_edges_are_ordered = false;
	}


	
	void
	undi_graph::
remove_from_in_edges 
	(gdtedge e) 
	{
	gdtnode v = stop(e);
	gdt::gdtlist<gdtedge>& in_edges  = (*node_info)[v].in_edges;
	gdt::list_item pos_in  = (*edge_info)[e].pos_into_in_edges;
	in_edges.del_item(pos_in);
	}
	
	
	
	void
	undi_graph::
remove_from_out_edges
	(gdtedge e)	
	{
	gdtnode v = start(e);
	gdt::gdtlist<gdtedge>& out_edges  = (*node_info)[v].out_edges;
	gdt::list_item pos_out  = (*edge_info)[e].pos_into_out_edges;
	out_edges.del_item(pos_out);
	}



	void
	undi_graph::
remove_from_adj_edges
	(gdtedge e)
	{
	gdt::gdtlist<gdtedge>& source_adj_edges  = (*node_info)[source(e)].adj_edges;
	gdt::gdtlist<gdtedge>& target_adj_edges  = (*node_info)[target(e)].adj_edges;
	gdt::list_item pos_into_source = (*edge_info)[e].pos_into_source_adj_edges;
	gdt::list_item pos_into_target = (*edge_info)[e].pos_into_target_adj_edges;
	source_adj_edges.del_item(pos_into_source);
	target_adj_edges.del_item(pos_into_target);
	}





		bool
		undi_graph::
make_embedding_planar_boyer
		()
		{
        //cout << "make_embeding_planar_boyer" << endl;
		return boyer(first_node());

		}




	gdt::gdtlist<constraint>*
	undi_graph::
get_constraints
	()
	{
	return constraints;
	}



	gdt::gdtlist<constraint>&
	undi_graph::
all_constraints
	(gdtedge e)
	{
	struct_undi_edge_info& ei = (*edge_info)[e];
	return ei.constraints;
	}



	gdt::gdtlist<constraint>&
	undi_graph::
all_constraints
	(gdtnode v)
	{
	struct_undi_node_info& vi = (*node_info)[v];
	return vi.constraints;
	}






	gdt::gdtlist<gdtedge> 
	undi_graph::
find_path 
	(gdtnode v, bool& exist_path1, bool& exist_path2, bool& exist_path3,
	 gdt::gdtnode_array<gdtedge>& father, gdt::gdtnode_array<int>& num_visit,
	 gdt::gdtnode_array<gdtnode>& lowpt1, gdt::gdtnode_array<bool>& marked_node)
	{
	bool flag;
	gdtnode w,u;
	gdtedge e,e1;
	gdt::gdtlist<gdtedge> path;
	
	path.clear();
	
	//cout << "\nSEARCHING PATH WITH START NODE " << id(v) << "\n" << flush;
	
	if (exist_path1)
		{
		// test for existence of a BACK_EDGE e = (v,v1) 
		// (i.e. e is not an gdtedge in the spanning tree and num_visit[v] > num_visit[w])
		// 
		forall_edges_adjacent_node (e,v,*this)
			if (DFS_edge (e,v,father,num_visit) == BACK_EDGE)
				{
				// found BACK_EDGE.
				//
				//cout << "\nFOUND PATH TYPE 1\n" << flush; 
				//
				// compute path1 = (v,v1)
				//
				path.append(e);
				//cout << "\nAPPENDED EDGE " << id(e);
				//
				// end path computation
				//
				break;
				}
		}
	
	if (path.empty()) 	
		{
		// not found path type 1
		//
		exist_path1 = false;
		
		if (exist_path2)
			{
			// test for existence of a forward TREE_EDGE e = (v,v1)
			// (i.e. e is an gdtedge in the spanning tree such that e = father[v1])
			// 
			forall_edges_adjacent_node (e,v,*this)
				if ((DFS_edge (e,v,father,num_visit) == TREE_EDGE) && (e != father[v]))
					{
					// found forward TREE_EDGE.
					//
					//cout << "\nFOUND PATH TYPE 2\n" << flush;
					//
					// compute path2 = (v,v1,v2,...,vn,w)
					// with  (vi,v(i+1)) forward TREE_EDGE, (vn,w) BACK_EDGE 
					// and w = lowpt1(v1).
					//
					path.append(e);
					//cout << "\nAPPENDED EDGE " << id(e);
					w = opposite(v,e);
					u = lowpt1[w];

					while (w != u)
						{
						e = first_adj_edge(w);
						flag = false;
						e1 = NULL_EDGE;
						
						// find the succ. path's gdtedge
						while (!flag)
							{
							if ((DFS_edge (e,w,father,num_visit) == BACK_EDGE) &&
							   (opposite(w,e) == u))
								{
								e1 = e;
								flag = true;
								}
							else if ((DFS_edge (e,w,father,num_visit) == TREE_EDGE) && 
								(e != father[w]) &&
								(lowpt1[opposite(w,e)] == u))
								if (
								   (!e1) || 
								   ((e1) && (compare (opposite(w,e),opposite(w,e1),num_visit,MIN) == opposite(w,e)))
								   )   
								   {
								   e1 = e;
								   }
							e = adj_succ (e,w);
							if (!e) flag = true;							
							}
                                                assert(e1!=NULL_EDGE);	
						//	
						// append the found gdtedge to the path
						//
						path.append (e1);
						//cout << "\nAPPENDED EDGE " << id(e1) << flush;
						w = opposite (w,e1);
						}				
					//
					// end path computation
					//
					break;
					}
			}
				
		if (path.empty())	
			{
			// not found path type 2
			//
			exist_path2 = false;

			if (exist_path3)
				{
				// test for existence of a FORW_EDGE e = (v,v1)
				// (i.e. e is not an gdtedge in the spanningf tree and num_visit[v] < num_visit[v1])
				// 
				forall_edges_adjacent_node (e,v,*this)
					if (DFS_edge (e,v,father,num_visit) == FORW_EDGE)
						{
						// found FORW_EDGE.
						//
						//cout << "\nFOUND PATH TYPE 3\n" << flush;
						//
						// compue path3 = (v,v1,v2,...,vn,w)
						// with (vi,v(i+1)), (vn,w) backward tree gdtedge, 
						// and w marked gdtnode.
						//
						path.append(e);
						//cout << "\nAPPENDED EDGE " << id(e);
						w = opposite(v,e);

						while (!marked_node[w])
							{
							e = father[w];
							path.append(e);
							//cout << "\nAPPENDED EDGE " << id(e);
							w = opposite(w,e);
							}
						// 
						// end path computation
						// 
						break;
						}
				}
				
			if (path.empty())
				{
				// not found path type 3
				// 
				exist_path3 = false;
				}
			}			
		}
	
	return path;	
	}


	
	DFS_edge_type 
	undi_graph::
DFS_edge 
	(gdtedge e, gdtnode v, gdt::gdtnode_array<gdtedge>& father, gdt::gdtnode_array<int>& num_visit) const
	{
	gdtnode w = opposite(v,e);
	
	// test if e is a TREE_EDGE
	//
	if ((e == father[v]) || (e == father[w])) return TREE_EDGE;
	else
		// test if e is a BACK_EDGE 
		// (i.e. e is not into spanning tree and num_visit[w] < num_visit[v])
		// or a FORW_EDGE
		// (i.e. e is not into spanning tree and num_visit[w] > num_visit[v])
		//
		if (num_visit[w] < num_visit[v]) return BACK_EDGE;
		else				return FORW_EDGE;
	}

// ---------------------------------
// A.Marcandalli (5-April-2001)
// this is a function (not a method) 
// declared only in this file
// ---------------------------------

	void 
move_components_outside_wheel
	(
	undi_graph& final, 
	int hubid
	) 
	{
	undi_graph ug(final);

	gdtedge e;
	gdtnode hub = ug.find_node(hubid);
	forall_edges_adjacent_node(e, hub, ug) {
		
		gdtedge e1 = e;
		gdtedge e2 = ug.cyclic_adj_succ(e, hub);
		gdtnode opp1 = ug.opposite(hub, e1);
		gdtnode opp2 = ug.opposite(hub, e2);
		
		// The boundary-vertices of the wheel form a simple cycle 
		//		
		assert(ug.edges_with_extremal_nodes(opp1, opp2).size() == 1);
		gdtedge border = ug.edges_with_extremal_nodes(opp1, opp2).front();

		//Rotazione rispetto ad opp1		
		{
		//Memorizzo tutti gli eventuali archi fra e1 e border che
		//potrebbero far parte di mezzelune o gocce ordinati da 
		//border ad e1
		gdtedge e;
		gdt::gdtlist<gdtedge> tomove;
		for (e = ug.cyclic_adj_pred(e1, opp1); e != border; e = ug.cyclic_adj_pred(e, opp1))
			tomove.push(e);
		
		//Ruoto gli archi posizionandoli prima di border
		forall(e, tomove) {
			gdtedge newedge = ug.new_edge(opp1, border, ug.opposite(opp1, e), e, AUTO_ID, gdt::before);
			int id = ug.id(e);
			ug.del_edge(e);
			ug.assign_id(newedge, id);
		}
		}

		//Rotazione rispetto a opp2
		{
		//Memorizzo tutti gli eventuali archi fra e2 e border che
		//potrebbero far parte di mezzelune o gocce ordinati da 
		//border ad e2
		gdtedge e;
		gdt::gdtlist<gdtedge> tomove;
		for (e = ug.cyclic_adj_succ(e2, opp2); e != border; e = ug.cyclic_adj_succ(e, opp2))
			tomove.push(e);
		
		//Ruoto gli archi posizionandoli prima di border
		forall(e, tomove) {
			gdtedge newedge = ug.new_edge(opp2, border, ug.opposite(opp2, e), e, AUTO_ID, gdt::after);
			int id = ug.id(e);
			ug.del_edge(e);
			ug.assign_id(newedge, id);
		}
		}
	}

	final.make_embedding_as(ug);
}


/********************************************************************************************************************
*********************************************************************************************************************
*********************************************************************************************************************/
/*  Pier Francesco Cortese    Novembre 2003   */

bic_obj_node* bic_obj_node_memory;
void* dfs_node_info_memory;


//bic_obj* bic_obj_actual_pointer;
//bic_obj_node* bic_obj_node_actual_pointer;
dfs_node_info* dfs_node_info_actual_pointer;
bm_node_info* bm_node_info_actual_pointer;
bm_edge_info* bm_edge_info_actual_pointer;




	dfs_node_info::
dfs_node_info
	()
	{
	//cout << "\nCostruttore di dfs_node_info" << endl;
	//counter++;
	}

		dfs_node_info::
~dfs_node_info
	(
	)
	{
	//cout << "Distruttore di dsf_node_info" << endl;
	//counter--;
	}



	dfs_edge_info::
dfs_edge_info
	()
	{
	//counter++;
	}

	dfs_edge_info::
~dfs_edge_info
	()
	{
	//counter--;
	}




/* Funzione per il calcolo delle componenti biconnesse di un grafo; tramite una DFS vengono calcolati gli indici DFI, i valori del
   lowpoint e gli archi vengono marcati come TREE_EDGE o BACK_EDGE.
   Pe ogni nodo sono costruite le liste dei back_edge uscenti ed entranti.
   La lista bic conterrà i nuovi grafi creati, ognuno costituito da una componente biconnessa del grafo originale.
   La lista cut_nodes conterrà l'insieme dei cut vertex individuati
*/
        int
        undi_graph::
biconnected_components
        (
        gdtnode v,
        gdt::gdtnode_map<dfs_node_info*>& vettore_nodi,
        //edge_map<dfs_edge_info*>& vettore_archi,
        gdt::gdtlist<undi_graph*>& bic
        //list<node>& cut_nodes
        ) const
        {
        // cout << "\nSTART DFS VISIT FROM NODE " << id(v) << "\n" << flush;
        int index=0;                     // Valore del DFI dei nodi
        int componenti=0;
        gdt::gdtstack<gdtnode> S1;                  // Stack utilizzato per effettuare la DFS
        gdt::gdtstack<gdtedge> S2;                  // Stack utilizzato per accumulare gli archi visitati:
                                         // Verrà utilizzato per costruire una componente biconnessa dopo aver
                                         // individuato un cut-vertex.
                                         // Ogni componente biconnessa verrà creata prelevando archi da questo stack
                                         // fino a quello che congiunge il cut-vertex al primo nodo della componente

        gdtnode w;
        gdtedge e;
        gdtedge temp;
        undi_graph* gr;
        bool stop;
        gdtnode a;                         // a,b,n1 e n2 sono variabili temporanee che servono per costruire i grafi
        gdtnode b;                         // delle componenti biconnesse
        gdtnode n1;
        gdtnode n2;
        std::string file_name;
        //gdtnode root=v;
        gdt::gdtnode_array<gdtedge> iterator (nodes_and_edges());

        gdt::gdtnode_map<bool> reached(nodes_and_edges());
        gdt::gdtedge_map<bool> e_reached(nodes_and_edges());
        forall_nodes(w,*this)
			{
			reached[w]=false;
			vettore_nodi[w]=new dfs_node_info();
			}
        forall_edges(e,*this)
			{
			e_reached[e]=false;
			//vettore_archi[e]=new dfs_edge_info();
			}
                if (!reached[v])
                {
				reached[v] = true;
                vettore_nodi[v]->parent= NULL_EDGE;
                vettore_nodi[v]->DFI=index;
                vettore_nodi[v]->lowpoint=index;
                //vettore_nodi[v]->cut_vertex=false;
                index++;
                iterator[v] = first_adj_edge(v);
                S1.push(v);
                // cout << "\nVISITED NODE " << id(v) << endl;
                }

        while (!S1.empty())
                {
                v = S1.top();
                e = iterator[v];
                while (e != NULL_EDGE)
                        {
                        if (e_reached[e]) e=(adj_succ(e,v));
                        else
                        {
                        // cout << "Sto lavorando con e= "<< id(e) << endl;
                        w = opposite(v,e);
                        e_reached[e]=true;
                        if (!reached[w])
                                {
                                 reached[w]  = true;
                                 vettore_nodi[w]->parent= e;
                                 //vettore_nodi[v]->children.append(e);
                                 vettore_nodi[w]->DFI=index;
                                 vettore_nodi[w]->lowpoint=vettore_nodi[v]->DFI;
                                 //vettore_nodi[w]->cut_vertex=false;
                                 // cout << "VISITED NODE " << id(w) <<endl;
                                 index++;
                                 //vettore_archi[e]->type=TREE_EDGE;
                                 S2.push(e);
                                 // cout << "Aggiunto TREE_EDGE " <<id(e) << endl;
                                 iterator[w] = first_adj_edge(w);
                                 S1.push(w);
                                 iterator[v] = adj_succ (e,v);
                                 v = w;
                                 e = iterator[v];
                                 }
                        else
                                {
                                if (e != (vettore_nodi[v]->parent))
                                        {//vettore_archi[e]->type=BACK_EDGE;
                                         S2.push(e);
                                         // cout << "Aggiunto BACK_EDGE " << id(e) << endl;
                                         //vettore_nodi[v]->out_back_edges.append(e);
                                         //vettore_nodi[w]->in_back_edges.append(e);
                                         if ((vettore_nodi[w])->DFI<(vettore_nodi[v]->lowpoint))
                                                (vettore_nodi[v]->lowpoint)=(vettore_nodi[w]->DFI);

                                         }
                                iterator[v] = adj_succ (e,v);
                                e = iterator[v];
                                }
                        }
                        }

                if ((S1.size())>1) {
                        S1.pop();
                        w=S1.top();    // w è il nodo nello stack sotto il nodo v
                        if ((vettore_nodi[v]->lowpoint)==(vettore_nodi[w]->DFI))  /* w è un cut_vertex  */
                                        /* E' possibile che un nodo venga individuato pìu volte come cut-vertex,
                                        perchè i cut vertex vengono individuati quando si esaminano i nodi figli,
                                        e uno stesso nodo può avere dei figli su diverse componenti biconesse.
                                        Tutte le componenti vengono create, ma il nodo è inserito una sola volta nella
                                        lista cut_nodes.
                                        Il nodo radice non viene mai inserito nella lista cut_vertex, perchè questo nodo
                                        è sempre considerato come cut_vertex dall'algoritmo.
                                        Solo alla fine si controlla se il nodo radice è effetivamente un cut-vertex */
                                { // cout << "Trovato cut-vertex " << id(w) <<" DFI=" << vettore_nodi[w].DFI << endl;
                                  // cout << "Nodo figlio " << id(v) << endl;
                                  /*if ((!(vettore_nodi[w]->cut_vertex)) && w!=root)
                                          {
                                            vettore_nodi[w]->cut_vertex=true;
                                            //cut_nodes.append(w);
                                            // cout << "Inserito nella lista dei cut vertex il vertice " << id(w) << endl;
                                          }
								  */
                                  /* costruzione della componente biconnessa con il contenuto di S2 */
                                  // cout << "S2 contiene " << S2.size() << " elementi" << endl;
                                  stop=false;
                                  gr=new undi_graph();
                                  while (!S2.empty() && !stop) {
                                        temp=S2.pop();
                                        a=source(temp);
                                        b=target(temp);
                                        // cout << "L'arco prelevato da S2 è " << id(temp) << "  ";
                                        // cout << "a è il nodo " << id(a) << " , b è il nodo " << id(b)<< endl;
                                        if ((((vettore_nodi[a]->DFI)==(vettore_nodi[w]->DFI)) &&
                                        ((vettore_nodi[b]->DFI)==(vettore_nodi[v]->DFI)))||
                                           (((vettore_nodi[a]->DFI)==(vettore_nodi[v]->DFI)) &&
                                           ((vettore_nodi[b]->DFI)==(vettore_nodi[w]->DFI))))
                                                {/* La costruzione finisce quando si trova il primo arco della componente biconnessa
                                                 che era stato inserito nello stack */
                                                 stop=true;
                                                 // cout << "Fine della costruzione di una componente biconnessa " << endl;
                                                 // cout << "L'arco "<< id(temp) << " è l'ultimo arco " << endl;
                                                }
                                        n1=gr->find_node(id(a));
                                        if (n1==NULL_NODE)  n1=gr->new_node(id(a));
                                        n2=gr->find_node(id(b));
                                        if (n2==NULL_NODE)  n2=gr->new_node(id(b));
                                        gr->new_edge(n1,n2,id(temp));
                                        // cout << "aggiunto alla componente l'arco " << id(temp)<< endl;
                                  }
                                bic.append(gr);
                                componenti++;

                                /* Creazione di un file per ogni componente biconnessa
                                file_name="componente_";
                                string numero_componente("%d",componenti);
                                file_name=file_name+numero_componente;
                                gr.write(file_name);
                                cout << "*****************************************" <<endl;
                                cout << "Creato file : " << file_name << endl;
                                cout << "*****************************************" <<endl;

                                */

                                }
                                else
                                        if ((vettore_nodi[w]->lowpoint)>(vettore_nodi[v]->lowpoint))
                                                vettore_nodi[w]->lowpoint=vettore_nodi[v]->lowpoint;
                        S1.push(v);
                }
                S1.pop();
                }
        /*
		if (((vettore_nodi[root]->children).length())>1)
                {//cut_nodes.append(root);
                 vettore_nodi[root]->cut_vertex=true;
                }
		*/
        // cout << "Sono stati individuati " << cut_nodes.length() << " cut vertex" << endl;
        return componenti;
        } ;




/**********************************************************************************************************
***********************************************************************************************************/
                                        /*   BOYER & MIRVOLD         */
/**********************************************************************************************************
***********************************************************************************************************/

//int bm_node_info::counter=0;
//int bm_edge_info::counter=0;

///////////////////////////////////////////////

//TIMERS

//stopwatch* biconnected_components_counter=new stopwatch();
#ifdef COUNTER
stopwatch* preprocessing_counter=new stopwatch();

stopwatch* walk_up_counter=new stopwatch();
stopwatch* walk_down_counter=new stopwatch();

stopwatch* extract_embedding_counter=new stopwatch();

stopwatch* garbage_counter=new stopwatch();

stopwatch* memory_counter=new stopwatch();
stopwatch* post_order_counter=new stopwatch();
#endif
/////////////////////////////////////////////




	bm_node_info* bm_node_info::
my_new_bm_node_info ()
	{
	bm_node_info* temp=bm_node_info_actual_pointer;
	bm_node_info_actual_pointer++;
	return temp;
	};


	bm_edge_info* bm_edge_info::
my_new_bm_edge_info ()
	{
	bm_edge_info* temp=bm_edge_info_actual_pointer;
	bm_edge_info_actual_pointer++;
	return temp;
	}


	bm_node_info::
bm_node_info
	(
	)
	{
    first_back_edge_dfi=INT_MAX;
	//counter++;
	}


	bm_edge_info::
bm_edge_info
	(
	)
	{
	inserted=false;
	//counter++;
	}


	bm_node_info::
~bm_node_info
	(
	)
	{
	//cout << "Distruttore di bm_node_info" << endl;
	//counter--;
	}


	bm_edge_info::
~bm_edge_info
	(
	)
	{
	//counter--;
	//cout << "Distruttore di bm_edge_info" << endl;
	}



/*This function computes a spanning tree and the lowpoint values for each node of the graph.
 if the graph is not biconnected, makes the graph biconnected. */

        bool
        undi_graph::
bm_spanning_tree
        (
		bm_node_info node_vector[],
		gdt::gdtlist<gdtedge>& added_edges,
		bic_obj_node* IMM[],
		gdtnode v,
		bool reached[],
		bool e_reached[],
        	gdtedge iterator[],
		gdtnode graph_nodes[],
		int& nodes_in_component,
		bic_obj*& bic_obj_actual_pointer,
		bic_obj_node*& bic_obj_node_actual_pointer
		)
        {

        int index=1;
        gdt::gdtstack<gdtnode> S1;
        gdtnode w;
        gdtedge e;
        bool success=true;
        //if (v == NULL_NODE) v=first_node();
        //if (v == NULL_NODE) return false;
        gdtnode root=v;
        gdtnode first_child;
	#ifdef OUTPUT
	std::cout << "\nSTART DFS VISIT FROM NODE " << id(v) << "\n" << flush;
	#endif
	
	bic_obj_node* bj_root=bic_obj_node::my_new_bic_obj(root,bic_obj_node_actual_pointer);

    	#ifdef LABEL
	bj_root->label="1";
	#endif
	IMM[root->plan_id]=bj_root;
	bj_root->DFI=index;
	//DFI_vector[index]=root;

   	 if (!reached[v->plan_id])
                {
		reached[v->plan_id] = true;
		//graph_nodes.append(v);
                graph_nodes[index-1]=v;
                node_vector[v->plan_id].parent= NULL_EDGE;
                //node_vector[v]->DFI=index;
		//DFI_vector[id(v)]=index;
		IMM[v->plan_id]->DFI=index;
		//DFI_vector[index]=v;
                node_vector[v->plan_id].lowpoint=index;
                index++;
                iterator[v->plan_id] = first_adj_edge(v);
                first_child=opposite(v,iterator[v->plan_id]);
		S1.push(v);
		#ifdef OUTPUT
                std::cout << "\nVISITED NODE " << id(v) << endl;
		#endif
                }

        while (!S1.empty())
                {
                v = S1.top();
                e = iterator[v->plan_id];

                //cout << "Inizio ciclo:\tv= " <<id(v)<< "\te=";
		//if (e==NULL_EDGE) cout << "NULL_EDGE\n";
		//else cout << id(e) << endl;
		gdtnode a=S1.pop();
		if (!S1.empty()&& e==NULL_EDGE && S1.top()==root && iterator[root->plan_id]!=NULL_EDGE)
			{
			//cout << "First_child=" << id(first_child)<< endl;
			gdtedge it_root=iterator[root->plan_id];
			while(it_root!=NULL_EDGE && reached[opposite(root,it_root)->plan_id])
				it_root=adj_succ(it_root,root);
			iterator[root->plan_id]=it_root;
			gdtnode temp_node;
			if (it_root==NULL_EDGE) temp_node=NULL_NODE;
			else  temp_node=opposite(root,iterator[root->plan_id]);

			if (temp_node!=NULL_NODE)
				{
				gdtedge temp_edge=new_edge(first_child,opposite(root,iterator[root->plan_id]));
				added_edges.append(temp_edge);
				if(this->number_of_edges() > ((3*this->number_of_nodes())-6) )
					return false;
				//cout << "Aggiunto arco tra " << id(first_child) << " e " << id(opposite(root,iterator[root])) << endl;
				first_child=temp_node;
				S1.push(first_child);
				//edge_vector[temp_edge]=new bm_edge_info();
				e_reached[temp_edge->plan_id]=false;
				e=temp_edge;
				gdtedge it_root=adj_succ(iterator[root->plan_id],root);
				while(it_root!=NULL_EDGE && reached[opposite(root,it_root)->plan_id])
					it_root=adj_succ(it_root,root);
				iterator[root->plan_id]=it_root;
				}
			}
			S1.push(a);
			while (e != NULL_EDGE)
                        {
                        if (e_reached[e->plan_id])
				 {
				 //cout << "Visitato arco " << id (e) << " già marcato" << endl;
				 //cout << "v= " << id(v)<< endl;
				 e=(adj_succ(e,v));
				 //if (e==NULL_EDGE) cout << "ora e ==NULL_EDGE" << endl;
				 //else cout << "arco e=" << id(e) << endl;
				 gdtnode a=S1.pop();
				 //cout << "S1.top()=" << id(S1.top()) << endl;
				if (!S1.empty()&& e==NULL_EDGE && S1.top()==root && iterator[root->plan_id]!=NULL_EDGE)
					{
					gdtedge it_root=iterator[root->plan_id];
					while(it_root!=NULL_EDGE && reached[opposite(root,it_root)->plan_id])
						it_root=adj_succ(it_root,root);
					iterator[root->plan_id]=it_root;
					gdtnode temp_node;
					if (iterator[root->plan_id]==NULL_EDGE) temp_node=NULL_NODE;
					else temp_node=opposite(root,iterator[root->plan_id]);
					if (temp_node!=NULL_NODE)
						{
						gdtedge temp_edge=new_edge(first_child,opposite(root,iterator[root->plan_id]));					
						added_edges.append(temp_edge);
						if(this->number_of_edges() > ((3*this->number_of_nodes())-6) )
							return false;
						
						#ifdef OUTPUT
						std::cout << "1 - Added edge between " << id(first_child) << " and " << id(opposite(root,iterator[root->plan_id])) << endl;
						#endif
						first_child=temp_node;
						S1.push(first_child);
						//edge_vector[temp_edge]=new bm_edge_info();
						e_reached[temp_edge->plan_id]=false;
						e=temp_edge;
						gdtedge it_root=adj_succ(iterator[root->plan_id],root);
						while(it_root!=NULL_EDGE && reached[opposite(root,it_root)->plan_id])
							it_root=adj_succ(it_root,root);
						iterator[root->plan_id]=it_root;
						}
					}
					S1.push(a);
				}
                        else                 {
                        //cout << "working with e= "<< id(e) << endl;
                        w = opposite(v,e);
                        e_reached[e->plan_id]=true;
                        if (!reached[w->plan_id])
                                {
                                 reached[w->plan_id]  = true;
                                 node_vector[w->plan_id].parent= e;
                                 node_vector[v->plan_id].children.append(w);
                                 node_vector[w->plan_id].lowpoint=IMM[v->plan_id]->DFI;
				 #ifdef OUTPUT
                                 std::cout << "VISITED NODE " << id(w) <<endl;
				 #endif
				//graph_nodes.append(w);
				 graph_nodes[index-1]=w;
                                 //index++;
                                 //edge_vector[e]->type=TREE_EDGE;
				 #ifdef OUTPUT
                                 std::cout << "Added TREE_EDGE " <<id(e) << endl;
				 #endif
				 //////////////////////////////////////////////////////////////////////////////////////////////////////
				 // Creation of elementary bicomp
				 //////////////////////////////////////////////////////////////////////////////////////////////////////
				 bic_obj_node* P=bic_obj_node::my_new_bic_obj(v,bic_obj_node_actual_pointer);
				 //P->is_node=true;
				 //P->graph_node=v;
		                 P->twin_link=(IMM[v->plan_id]);
		                 P->first_child=w;
				 P->DFI=IMM[v->plan_id]->DFI;
                                 #ifdef OUTPUT
				 std::cout << "set DFI=" << P->DFI << " on node " << endl;
				 #endif
				 P->can_be_flipped=false;
				 P->active_walk_up=false;
				 #ifdef LABEL
                                 {
				 ostringstream x;
				 x<< P->DFI << ends;
				 //x.freeze();
				 P->label=std::string(x.str());
				 }
				 #endif
				 //edge parent_edge=node_vector[v]->parent;
				 //if (parent_edge!=NULL_EDGE) P->graph_parent_node=opposite(v,parent_edge);
				 bic_obj_node* V=bic_obj_node::my_new_bic_obj(w,bic_obj_node_actual_pointer);//new bic_obj(v);
				 //V->is_node=true;
				 //V->graph_node=w;
				 #ifdef LABEL
                                 {
				 ostringstream x;
				 x<< index << ends;
				 //x.freeze();
				 V->label=std::string(x.str());
				 }
       				#endif
				 V->twin_link=V;
				 //V->graph_parent_node=v;
				 V->DFI=index;
                                 #ifdef OUTPUT
				 std::cout << "set DFI=" << V->DFI << endl;
				 #endif
				 //DFI_vector[index]=w;
				 bic_obj* cp=bic_obj::my_new_bic_obj(e,bic_obj_actual_pointer);//new bic_obj(e);
				 bic_obj* cv=bic_obj::my_new_bic_obj(e,bic_obj_actual_pointer);//new bic_obj(e);
   				 #ifdef LABEL
				 {
				 ostringstream x;
				 x<< P->DFI<<','<<V->DFI<< ends;
				 //x.freeze();
				 cp->label=std::string(x.str());
				 }
				 {
				 ostringstream x;
				 x<< V->DFI<<','<<P->DFI<< ends;
				 //x.freeze();
				 cv->label=std::string(x.str());
				 }
        			 #endif
				 cp->twin_link=cv;
				 cv->twin_link=cp;
				 cp->graph_node=v;
				 cv->graph_node=w;
				 P->black=cp;
				 P->white=cp;
				 cp->white=P;
				 cp->black=P;
				 V->black=cv;
				 V->white=cv;
				 cv->white=V;
				 cv->black=V;
				 //bicomp++;
				 IMM[w->plan_id]=V;
				 index++;
				 //cout << "PARENT: " << P->label << "   " << P << "       CHILD: " << V->label << "   " << V << endl;
				 //cout << "Connector 1: " << cp->label << "   " << cp <<  "     Connector 2: " << cv->label << "   " << cv << endl;
				 //cout << "Parent twin link:" << P->twin_link->label << endl;
				 //cout << "Child twin link"  << V->twin_link->label << endl;
				 //if (V->twin_link==V) cout << "Twin_link Ok" << endl;
				 //else cout << "Error in Twin_Link" << endl;
				 //cout << "*************************************************************************" << endl;
				 //////////////////////////////////////////////////////////////////////////////////////////////////////////
				 //////////////////////////////////////////////////////////////////////////////////////////////////////////
                                 iterator[w->plan_id] = first_adj_edge(w);
                                 S1.push(w);
                                 iterator[v->plan_id] = adj_succ (e,v);
                                 v = w;
                                 e = iterator[v->plan_id];
				//cout << "Ora v= " << id(v) << endl;
                                 gdtnode a=S1.pop();
				 if (!S1.empty()&& e==NULL_EDGE && S1.top()==root && iterator[root->plan_id]!=NULL_EDGE)
				{
					gdtedge it_root=iterator[root->plan_id];
					while(it_root!=NULL_EDGE && reached[opposite(root,it_root)->plan_id])
					it_root=adj_succ(it_root,root);
					iterator[root->plan_id]=it_root;
					gdtnode temp_node;
					if (it_root==NULL_EDGE) temp_node=NULL_NODE;
					else temp_node=opposite(root,it_root);
					if( temp_node!=NULL_NODE)
					{
						gdtedge temp_edge=new_edge(first_child,opposite(root,iterator[root->plan_id]));
						added_edges.append(temp_edge);
						if(this->number_of_edges() > ((3*this->number_of_nodes())-6) )
							return false;
												
						#ifdef OUTPUT
						std::cout << "2 - Added edge between  " << id(first_child) << " and " << id(opposite(root,iterator[root->plan_id])) << endl;
						#endif
						first_child=temp_node;
						S1.push(first_child);
						
						//edge_vector[temp_edge]=new bm_edge_info();
						e_reached[temp_edge->plan_id]=false;
						e=temp_edge;
						it_root=adj_succ(it_root,root);
						while(it_root!=NULL_EDGE && reached[opposite(root,it_root)->plan_id])
							it_root=adj_succ(iterator[root->plan_id],root);
						iterator[root->plan_id]=it_root;
					}
				}
			 S1.push(a);
			}
                        else
                                {
                                if (e != (node_vector[v->plan_id].parent))
                                        {
					//edge_vector[e]->type=BACK_EDGE;
					#ifdef OUTPUT
          				std::cout << "Added BACK_EDGE " << id(e) << "\t";
					if (e_reached[e->plan_id])
						std::cout<< "back edge is marked as reached" << endl;
					else
						std::cout << "back edge is NOT marked as reached" << endl;
					#endif
                                        node_vector[w->plan_id].in_back_edges.append(v);
					gdtnode temp_c=opposite(w,e);
					if (node_vector[temp_c->plan_id].first_back_edge_dfi>IMM[w->plan_id]->DFI)
						node_vector[temp_c->plan_id].first_back_edge_dfi=IMM[w->plan_id]->DFI;
                                        if ((IMM[w->plan_id]->DFI)<(node_vector[v->plan_id].lowpoint))
                                                {
						(node_vector[v->plan_id].lowpoint)=IMM[w->plan_id]->DFI;
						//bic_obj* temp_bj=IMM[v];
						}

                                         }
					iterator[v->plan_id] = adj_succ (e,v);
					e = iterator[v->plan_id];
					gdtnode a=S1.pop();
					if (!S1.empty()&& e==NULL_EDGE && S1.top()==root && iterator[root->plan_id]!=NULL_EDGE)
						{
						gdtedge it_root=iterator[root->plan_id];
						while(it_root!=NULL_EDGE && reached[opposite(root,it_root)->plan_id])
							it_root=adj_succ(it_root,root);
						iterator[root->plan_id]=it_root;
						gdtnode temp_node;
						if (iterator[root->plan_id]==NULL_EDGE) temp_node=NULL_NODE;
						else temp_node=opposite(root,iterator[root->plan_id]);
						if (temp_node!=NULL_NODE)
							{
							gdtedge temp_edge=new_edge(first_child,opposite(root,iterator[root->plan_id]));
							added_edges.append(temp_edge);
							if(this->number_of_edges() > ((3*this->number_of_nodes())-6) )
								return false;
							#ifdef OUTPUT
							std::cout << "3 - Added edge between " << id(first_child) << " and " << id(opposite(root,iterator[root->plan_id])) << endl;
							#endif
							first_child=temp_node;
							S1.push(first_child);
							//edge_vector[temp_edge]=new bm_edge_info();
							e_reached[temp_edge->plan_id]=false;
							e=temp_edge;
							//v=temp_node;
							gdtedge it_root;
							it_root=adj_succ(iterator[root->plan_id],root);
							while(it_root!=NULL_EDGE && reached[opposite(root,it_root)->plan_id])
								it_root=adj_succ(it_root,root);
							iterator[root->plan_id]=it_root;
							}
						}
						S1.push(a);
                                }
                        }

		}

                if ((S1.size())>1) {
                        S1.pop();
                        w=S1.top();    // w è il nodo nello stack sotto il nodo v
                        if (((node_vector[v->plan_id].lowpoint)==IMM[w->plan_id]->DFI) && (w!=root))  /* w is cut_vertex  */
                               { //cout << endl << "Cut-vertex found, vertex " << id(w) <<" DFI=" << node_vector[w]->DFI << endl;
				gdtnode temp_node=opposite(w,node_vector[w->plan_id].parent);
				gdtedge temp_edge=new_edge(v,temp_node);
				added_edges.append(temp_edge);
				if(this->number_of_edges() > ((3*this->number_of_nodes())-6) )
					return false;
				#ifdef OUTPUT
				std::cout << "4 - Added edge between " << id(v) << " and " << id(temp_node) << endl;
				#endif
				e_reached[temp_edge->plan_id]=true;
				//edge_vector[temp_edge]=new bm_edge_info();
				//edge_vector[temp_edge]->type=BACK_EDGE;
				node_vector[opposite(w,node_vector[w->plan_id].parent)->plan_id].in_back_edges.append(v);
                                if (node_vector[v->plan_id].first_back_edge_dfi>IMM[temp_node->plan_id]->DFI)
					node_vector[v->plan_id].first_back_edge_dfi=IMM[temp_node->plan_id]->DFI;
				node_vector[v->plan_id].lowpoint=IMM[opposite(w,node_vector[w->plan_id].parent)->plan_id]->DFI;
				//bic_obj* temp_bj=IMM[v];
				//temp_bj->black->twin_link->black->child_lowpoint=node_vector[v]->lowpoint;
				//cout << "Aggiunto arco" << endl;
                                }
                                else
                                	if ((node_vector[w->plan_id].lowpoint)>(node_vector[v->plan_id].lowpoint))
						{
						node_vector[w->plan_id].lowpoint=node_vector[v->plan_id].lowpoint;
						//bic_obj* temp_bj=IMM[w];
						//temp_bj->black->twin_link->black->child_lowpoint=node_vector[w]->lowpoint;
						}
                        S1.push(v);
                }

	//if (!S1.empty()) S1.pop();
	S1.pop();


                }
        if (((node_vector[root->plan_id].children).length())>1)
                {
                 success=false;
                 //std::cout << "Root is cut_vertex " <<endl;
                }
		#ifdef OUTPUT
		std::cout << "Fine funzione bm_spanning_tree" << endl;
		#endif
		nodes_in_component=index-1;
		return success;
        } ;



/* Ordina una lista di nodi secondo il valore del lowpoint calcolato */


	void
bucket_sort_lowpoint
	(
	int max,
	gdtnode graph_nodes[],
	bm_node_info node_vector[],
	gdt::gdtlist<gdtnode>& results
	)
	{
     //gdt::gdtlist<gdtnode> bucket[max];
	 gdt::gdtlist<gdtnode>* bucket= new gdt::gdtlist<gdtnode>[max] ;
	 //std::auto_ptr<gdt::gdtlist<gdtnode> > delbucket(bucket);

	 gdtnode n;
	 //cout << "Function bucket_sort_lowpoint activated "<< endl;
	 //cout << "Number of nodes: " << L.length() << "\tbucket size: " << (max-min+1);
     for (int k=0; k<max; k++)
	 	{
		 n=graph_nodes[k];
		 bucket[(node_vector[n->get_plan_id()].lowpoint)-1].append(n);
		};
	 for (int i=0; i<=max-1; i++)
		if (bucket[i].length()>0) results.conc(bucket[i]);
	//cout << "Max lowpoint value: " << node_vector[(results.inf(results.last()))]->lowpoint << endl;
	delete[] bucket;
	};


	void undi_graph::
create_children_ordered
	(
	bm_node_info node_vector[],
	//int lowpoint[],
	//edge parent[],
	//list<node> children_ordered[],
	//list_item position_in_parent_list[],
	gdtnode graph_nodes[],
	int nodes_in_component
	)
	{
	gdtnode n;
	gdt::list_item l;
	//list<node> graph_nodes=all_nodes();
	gdt::gdtlist<gdtnode> ordered;

	bucket_sort_lowpoint(nodes_in_component,graph_nodes,node_vector,ordered);
	//bucket_sort_lowpoint_counter->pause();

	forall(n,ordered)
		if (node_vector[n->plan_id].parent!=NULL_EDGE)
			{
			gdtedge e=node_vector[n->plan_id].parent;
			gdtnode p=opposite(n,e);
			(node_vector[p->plan_id].children_ordered).append(n);
			l=(node_vector[p->plan_id].children_ordered).last();
			node_vector[n->plan_id].position_in_parent_list=l;

			//cout << "parent=" << id(p)<< " child=" << id(n) << endl;
			//cout << "Edge added: ";
			//e=(node_vector[p]->children_ordered).inf(node_vector[n]->position_in_parent_list);
			}

	};


	bool
    undi_graph::
bm_preprocessing
	(
	bm_node_info node_vector[],
	//int lowpoint[],
	//edge parent[],
	//list<node> children[],
	//list<node> children_ordered[],
	//list<node> in_back_edge[],
	//int first_back_edge_dfi[],
	//list_item position_in_parent_list[],
	//edge_map<bm_edge_info*>& edge_vector,
	gdt::gdtlist<gdtedge>& added_edges,
	gdtnode root,
	bic_obj_node* IMM[],
	bool reached[],
	//edge_map<bool>& e_reached,
	bool e_reached[],
	gdtedge iterator[],
	int& nodes_in_component,
	gdtnode graph_nodes[],
	bic_obj*& bic_obj_actual_pointer,
	bic_obj_node*& bic_obj_node_actual_pointer
	)
    {
    //cout << "---------------------------------------------------------------" << endl;
    //cout << endl << "Start Preprocessing .........." << endl;
    //bm_spanning_tree_counter->start();
	//list<node> graph_nodes;
	bool 	success=bm_spanning_tree(node_vector,added_edges,IMM,root,reached,e_reached,iterator,graph_nodes,nodes_in_component,bic_obj_actual_pointer,bic_obj_node_actual_pointer);
    //bm_spanning_tree_counter->pause();
	if (success)
		{
     	//create_children_ordered_counter->start();
		if (nodes_in_component <=3) return true;

		#ifdef OUTPUT
		std::cout << "Function Create_children_ordered activated" << endl;
		#endif
		create_children_ordered(node_vector,graph_nodes,nodes_in_component);
        #ifdef OUTPUT
		std::cout << "End of create_children_ordered" << endl;
		#endif
		//create_back_edges_ordered_counter->pause();
     	}
  	//cout << endl << "End Preprocessing"  <<endl<< endl;
	#ifdef OUTPUT
	std::cout << "TEST E_REACHED values:" << endl;
	gdtedge e;
	forall_edges (e,*this)
		{
		std::cout << "Edge: " << id(e) << "\te_reached=";
		if (e_reached[e->plan_id]) std::cout << "TRUE" << endl;
		else std::cout << "FALSE" << endl;

		}
	#endif
	//for (int k=0; k<number_of_nodes(); k++)
	//	cout << k << "\tfbed\t" << first_back_edge_dfi[k] << "\tlowpoint" <<lowpoint[k]<<  endl;
	//cout << endl;
	return success;
    };




/***********************************************************************************
				BIC_OBJ
*************************************************************************************/


	bic_obj* bic_obj::
my_new_bic_obj (bic_obj*& bic_obj_actual_pointer)
	{
	bic_obj* temp=bic_obj_actual_pointer;
	bic_obj_actual_pointer++;
	return temp;
	};


	bic_obj_node* bic_obj_node::
my_new_bic_obj (gdtnode n,bic_obj_node*& bic_obj_node_actual_pointer)
	{
	bic_obj_node* temp=bic_obj_node_actual_pointer;
	temp->is_node=true;
	temp->graph_node=n;
	bic_obj_node_actual_pointer++;
	return temp;
	};


	bic_obj* bic_obj::
my_new_bic_obj (gdtedge e,bic_obj*& bic_obj_actual_pointer)
	{
	bic_obj* temp=bic_obj_actual_pointer;
	temp->is_node=false;
	temp->graph_edge=e;
	bic_obj_actual_pointer++;
	return temp;
	};


	bic_obj::
bic_obj()
	{
	 //counter++;
	 //twin_link=NULL;
	 //black=NULL;
	 //white=NULL;
	 marked=INT_MAX;
//	 ordered=false;
//	 graph_node=NULL_NODE;
//	 graph_edge=NULL_EDGE;
	 //active_passages=0;
	};


    bic_obj::
bic_obj(gdtnode n)
	{
	 //counter++;
	 is_node=true;
	 graph_node=n;
	 graph_edge=NULL_EDGE;
	 //ordered=false;
	 //first_child=NULL_NODE;
	 //twin_link=NULL;
	 //black=NULL;
	 //white=NULL;
	 marked=INT_MAX;
	 //active_passages=0;
	};


    bic_obj::
bic_obj(gdtedge e)
	{
	 //counter++;
	 is_node=false;
	 graph_node=NULL_NODE;
	 graph_edge=e;
	 //ordered=false;
	 //first_child=NULL_NODE;
	 //twin_link=NULL;
	 //black=NULL;
	 //white=NULL;
	 marked=INT_MAX;
	 //active_passages=0;
	};


	bic_obj::
~bic_obj()
	{
	//counter--;

	//active_bicomp.clear();
	//not_active_bicomp.clear();
	//cout << "Delete bic_obj" << this->label << endl;
	};


	bic_obj_node::
bic_obj_node()
	{
	 //counter++;
	 //twin_link=NULL;
	 //black=NULL;
	 //white=NULL;
	 //marked=INT_MAX;
	 //ordered=false;
	 //graph_node=NULL_NODE;
	 //graph_edge=NULL_EDGE;
	 active_passages=0;
	};


#ifdef LABEL
     std::string bic_obj::
get_label()
	{
	return label;
	};
#endif


/* destroy_bicomp delete the bicomp structure */
	void undi_graph::
destroy_bicomp
	(
	gdt::gdtlist<bic_obj*>& bic_obj_created
	)
	{
	bic_obj* temp_bj;
	forall (temp_bj,bic_obj_created)
		delete temp_bj;

	};


/* next_node return the successive adiacent node on the external face of the bicomp */
/* return false if the bicomp structure is not correct */

	bic_obj_node*
bic_obj_node::next_node
	(
	bool black
	//bool& success
	)
		{
		// cout << "Function next_node activated on bicomp " << this->label << "    " << this << endl;
		bic_obj* first_edge_connector;
		bic_obj* second_edge_connector;
		bic_obj_node* result;
		/*if ((this->black==NULL) || (this->white==NULL))
			{
			success=false;
			return this;
			}*/
		if (black) first_edge_connector=this->black;
			else first_edge_connector=this->white;

		/*if (first_edge_connector->is_node)
			{
			success=false;
			return this;
			}*/
		/*if (first_edge_connector->twin_link==NULL)
			{
			success=false;
			return this;
			}*/
		// cout << "first_edge_connector=" << first_edge_connector->label << "   " << first_edge_connector << endl;
		second_edge_connector=first_edge_connector->twin_link;
		/*if (second_edge_connector->is_node)
			{
			success=false;
			return this;
			} */
		// cout << "second_edge_connector=" << second_edge_connector->label <<"   "  << second_edge_connector << endl;
		result=(bic_obj_node*)(second_edge_connector->black);
		if (result->is_node)
			{
			//success=true;
			// cout << "result=" << result << endl;
			return result;
			};
		//success=true;
		return (bic_obj_node*)(second_edge_connector->white);

		/*if (result->is_node)
			{
			success=true;
			// cout << "result=" << result << endl;
			return result;
			}
		success=false;
		return this;*/
		};




/* check if a bicomp is correct */


#ifdef LABEL

	bool bic_obj_node::
check_bicomp ()
	{
	bic_obj_node* start=this;
	bool stop=false;
	bool success=true;
	bool elementary=false;
	int visited_nodes=1;
	bic_obj_node* current;
	bic_obj_node* temp;
	bic_obj_node* previous=start;
	std::cout << "Check bicomp - visit of the exernal side" << endl;
	std::cout << "Start=" << start->label << "   marked=" << start->marked << endl;

	if (!start->is_node)
		{
		std::cout << "Check_bicomp must be activate on a node bic_obj!" << endl;
		return false;
		};

	if (start->black==NULL)
		{
		std::cout << "No edge connected to this bic_obj " << endl;
		std::cout << "*******************************" << endl;
		std::cout << "Check OK" << endl;
		std::cout << "*******************************" << endl;
		return true;
		};
	if (start->black==start->white)
		{
		elementary=true;
		std::cout << "Elementary bicomp" << endl;
		}
	current=next_node(true);
	if (!success) return false;
	while (!stop)
		{
		std::cout << "Visited bic_obj "<< current->label;
		std::cout << "\tmarked= " << current->marked << "   ";
		if ((current->twin_link)==current)
			std::cout << ", twin link is self loop" << endl;
		else std::cout << ", twin link is not self loop" << endl;
		visited_nodes++;
		if (current==start && (elementary || visited_nodes>2))
			{
			// cout << current <<"," << start << endl;
			stop=true;
			}
		else
			{
			temp=(current->next_node(true));
			if (!success) return false;
			if (temp==previous) temp=current->next_node(false);
			if (!success) return false;
			previous=current;
			current=temp;
			}

	}

	std::cout << "*******************************" << endl;
	std::cout << "Check OK" << endl;
	std::cout << "********************************" << endl;
	return success;

	};

#endif


/* Test if a node is active */

bool bic_obj::
	is_active
	(
	int target,
	bm_node_info node_vector[]
	//int lowpoint[],
	//int first_back_edge_dfi[],
	//list<node> children_ordered[]
	)
	{
	bool success=true;
	bool first_test;   // check if the node has a back_edge over target;
	bool second_test;  // check if the node has a child bicomp active;

	gdtnode n=this->graph_node;
	#ifdef OUTPUT
	std::cout << "Function is_active activated on node " << this->label << "   target=" << target << endl;
	#endif
	if ((node_vector[n->plan_id].first_back_edge_dfi)<target)	first_test=true;
	else first_test=false;

	// Second Test
	if ((node_vector[n->plan_id].children_ordered.length())==0)
		{
		second_test=false;
		}
	else
		{
		gdt::list_item l=node_vector[n->plan_id].children_ordered.first();
		gdtnode c=node_vector[n->plan_id].children_ordered.inf(l);
		if ((node_vector[c->plan_id].lowpoint) < target) second_test=true;
		else second_test=false;
		}
	success=(first_test || second_test);
	//if (!success) cout << "Ecco un nodo non attivo!!!!!!!!!!!!!!!!!!!!!!!" << endl;
	//else
	//	{
	//	if (first_test) cout << "FIRST TEST=TRUE" << endl;
	//	if (second_test) cout << "SECOND TEST=TRUE" << endl;
	//	}
	return success;
	};





/* Test if a root node is externally active */

	bool bic_obj::
is_root_externally_active
	(
	int target,
	gdtnode child,
	bm_node_info node_vector[]
	//int lowpoint[],
	//int first_back_edge_dfi[],
	//list<node> children_ordered[]
	)
	{
	bool success=true;
	bool first_test;   // check if the node has a back_edge over target;
	bool second_test;  // check if the node has a child bicomp active;

	gdtnode r=this->graph_node;
	//cout << "Function is_root_externally_active activated on node " << node_vector[r]->DFI << "   target=" << node_vector[target]->DFI << endl;


	if ((node_vector[r->plan_id].first_back_edge_dfi)<target)
		{
		first_test=true;
		}
	else first_test=false;
		//}

	// Second Test

    if (node_vector[r->plan_id].children_ordered.length()==0)
		{
        // cout << "No child bicomp - OK -" << endl;
		second_test=false;
		}
	else
		{
        //   Test di consistenza della struttura
		if (node_vector[r->plan_id].children_ordered.length()==1)
			{
            //gdt::list_item l=node_vector[r->plan_id].children_ordered.first();
			//edge e=node_vector[r]->children_ordered.inf(l);
			//node c=opposite(r,e);
			//gdtnode c=node_vector[r->plan_id].children_ordered.inf(l);
			/*if (c!=child)
				{
                cout << "********************************************************************" << endl;
				cout << "lista con un elemento diverso da child!! Errore nella struttura" << endl;
                cout << "********************************************************************" << endl;
                return true;
				}
			*/
			//cout << "Un solo elemento uguale a child. OK" << endl;
			second_test=false;
			}
		else // 2 or more child bicomp
			{
			gdt::list_item l=node_vector[r->plan_id].children_ordered.first();
			//edge e=node_vector[r]->children_ordered.inf(l);
			//node c=opposite(r,e);
			gdtnode c=node_vector[r->plan_id].children_ordered.inf(l);
			if (c==child)
				{
				l=node_vector[r->plan_id].children_ordered.succ(l);
            	//e=node_vector[r]->children_ordered.inf(l);
				//c=opposite(r,e);
				c=node_vector[r->plan_id].children_ordered.inf(l);
				}
			if ((node_vector[c->plan_id].lowpoint) < target)
				{
				//cout << "Active bicomp found - node=" << node_vector[c]->DFI;
				//cout << "  node "<< node_vector[n]->DFI << " is active." << endl;
				second_test=true;
				}
			else second_test=false;
			}
		}

	success=(first_test || second_test);
	//if (success) cout << "Externally active root found" << endl;
	//else cout << "Not active" << endl;
	return success;
	};


	bool bic_obj::
is_internally_active_node
	(
    int target,
	bm_node_info node_vector[]
	//int first_back_edge_dfi[],
	//list<node> children_ordered[]
	)
	{
	return (node_vector[this->graph_node->plan_id].children_ordered.length()!=0 ||
		node_vector[this->graph_node->plan_id].first_back_edge_dfi==target);
	}

#ifdef LABEL

/* Check if a node is linked correctly with its adjacent edges */
/*	bool
bic_obj::check_node()
		{
		std::cout << "Check consistency on node " << this->label<< endl;
		if (this->black==NULL && this->white==NULL) return true;
		std::cout << "black= " << this->black->label << "    white= " << this->white->label << endl;
		bic_obj* temp=this->black;
		bic_obj* previous=this;
		if (!this->is_node) return false;
		while (temp!=this)
			{
			std::cout << "visited element: " << temp->label<< "    black= " << temp->black->label << "  white= " << temp->white->label << endl;
			if (temp->is_node) return false;
			if (temp->black==previous)
				{
				previous=temp;
				temp=temp->white;
				}
			else
				{
				previous=temp;
				temp=temp->black;
				}
			}
		std::cout << endl;
		return true;
		}
*/
#endif



/* WALK UP. Try if an edge can be added to the graph .
It must be invocated on the source node of the back-edge*/


//int inutili;


	bool bic_obj_node::
walk_up
	(
	int target,
	bm_node_info node_vector[],
	gdt::gdtlist<bic_obj_node*>& secondary,
	undi_graph& ug
	//stack<bic_obj*>& embedding_stack
	)
	{

	int w=target;
	bool success=true;

	int index;  	// Determine if we are on one side of the bicomp (odd) or on the other side (even).
	int delta;  	// It's added to index. Determine if there are two sides active (=1) or if there's only one (=2).
	int current; 	// 0 or 1, indicate the current side
	bic_obj* next[2];// connectors to the next node;
	bool twin_link_previous=false;
	bool active_bicomp_previous=false;
	gdt::gdtstack<bic_obj*> dt_stack[2];	// dove-tail stacks.
	bic_obj_node* start_node;		// entry node of the current bicomp
	bool start_connector_black[2];  // true if the connector of start_node on side0 (1) in the dove_tail is black;

	// patch
	bool check_other_side=false;
	bool not_invert_side=false;

	int counter[2];
	// end patch

	bic_obj* temp_bj;
	bic_obj_node* previous[2];

	#ifdef LABEL
	std::cout << "Function Walk_up activated. Target=" << target << " , Source=" << this->label << "\t";
	std::cout << "Start node->active_passages=" << this->active_passages << endl;
	//this->check_bicomp();
	#endif
	/* Initialization */

	bic_obj_node* x=this;
	current=0;
	index=0;
	delta=1;
	counter[0]=1;
	counter[1]=1;

	/* Now we are on the entry node of the first bicomp */

	if (abs(x->marked)==w)
		{
        //cout << "Marcatura del nodo di ingresso=" << x->marked << endl;
		//cout << "**************         WALK UP SUCCESS!!      *****************" << endl;
		return true;
		}

	if (x->is_active(w,node_vector))
		{
		check_other_side=true;
		counter[0]=1;
		counter[1]=1;
		#ifdef OUTPUT
		std::cout << "Dove tail starts from active node" << endl;
		#endif
		}
	if (x->marked==-w)
		x->marked=w;
	else x->marked=-w;
	start_node=x;
	next[0]=x->black;
	next[1]=x->white;
	previous[0]=x;
	previous[1]=x;

	/* Generic Iteration */

	bool stop=false;
	//cout << "Start generic iteration" << endl;
	while (!stop)
		{
		//cout << endl << "new iteration" << endl << endl;
		if (!twin_link_previous)
			{
			previous[current]=x;
			index=index+delta;
			current=(index%2);
			//cout << "current=" <<current << endl;
			bic_obj* tmp=next[current];			// connector to the next node;
			#ifdef OUTPUT
			std::cout << "VISITED BIC_OBJ: " << tmp->label << endl;
			#endif
			//if (tmp->is_node)
			//	{
			//	//cout << "Error in bicomp structure." << endl;
			//	return false;
			//	}
			if (tmp->marked==-w)
				tmp->marked=w;
			else tmp->marked=-w;

			dt_stack[current].push(tmp);
			tmp=tmp->twin_link;
			// cout << "Twin link "<< endl;
			#ifdef OUTPUT
			std::cout << "VISITED BIC_OBJ: " << tmp->label << " black=" << tmp->black->label << " white=" << tmp->white->label << endl;
			if (tmp->is_node)
				{
				std::cout << "Error in bicomp structure." << endl;
				return false;
				}
			#endif
			if (tmp->marked==-w)
				tmp->marked=w;
			else
				tmp->marked=-w;
			dt_stack[current].push(tmp);
			x=(bic_obj_node*)tmp->black;
			if (!(x->is_node)) x=(bic_obj_node*)tmp->white;
			/*if (!(x->is_node))
				{
				cout << "Error in bicomp structure" << endl;
				return false;
				}*/
			dt_stack[current].push(x);
			#ifdef OUTPUT
			std::cout << "VISITED BIC_OBJ: " << x->label << endl;
			#endif
			if (x->twin_link==x)
				{  // x is not root in this bicomp
				#ifdef OUTPUT
				std::cout << "x is not root" << endl;
				#endif
				//if (x->marked) cout << "x is marked" << endl;
				if (x->is_active(w,node_vector))
					{	// x is active
					#ifdef OUTPUT
					std::cout << "x is active" << endl;
					#endif
					if (delta==2)
						{
						success=false;
						stop=true;
						while (!dt_stack[current].empty())
							{
							//temp_bj=dt_stack[current].pop();
							//temp_bj->marked=INT_MAX;
                            (dt_stack[current].pop())->marked=INT_MAX;
							}
						}
					else    // delta==1
						{
						delta=2;
						index++;
                        if (check_other_side) not_invert_side=true;
						check_other_side=false;
						if (abs(x->marked)==w) dt_stack[current].pop();
						while (!dt_stack[current].empty())
							{
							//temp_bj=dt_stack[current].pop();
							//temp_bj->marked=INT_MAX;
							(dt_stack[current].pop())->marked=INT_MAX;
							}
						}   // end of else (delta==1)
					}  // end of if(x->is_active)

				else  // x is not active, is not root, is not entry node
					{
					if (abs(x->marked)==w)
						{
						//walk_up_stack.push(start_node);
						dt_stack[current].clear();
						//while (!dt_stack[current].empty())
							//{
							//temp_bj=dt_stack[current].pop();
                            				//if (temp_bj->twin_link!=temp_bj)
							//	cout << " -1- root node inserted in walk up stack" << endl;
							//walk_up_stack.push(temp_bj);
						//	}
							if (delta==1)
								{
								while (!dt_stack[(current+1)%2].empty())
									{
									//temp_bj=dt_stack[(current+1)%2].pop();
									//temp_bj->marked=INT_MAX;
                                    (dt_stack[(current+1)%2].pop())->marked=INT_MAX;
									}
								}
						return true; //success;
						}
					else // x->marked==0
						{
						x->marked=-w;
						//bool s;
						temp_bj=x->next_node(true);
						if (temp_bj==previous[current])
							next[current]=x->white;
						else next[current]=x->black;
						//cout << "previous[current]= " << previous[current]->label << endl;
						//cout << "next[current]= " << next[current]->label <<endl;

						// patch

						if (check_other_side && (x->is_internally_active_node(w,node_vector)) && delta==1)
							{
							counter[current]++;
							delta=2;
							#ifdef OUTPUT
							std::cout << "Nodo di ingresso x=" << x->label << endl;
							#endif
							while (!dt_stack[(current+1)%2].empty())
								{
								//temp_bj=dt_stack[(current+1)%2].pop();
								//temp_bj->marked=INT_MAX;
								(dt_stack[(current+1)%2].pop())->marked=INT_MAX;
								}
							//if (x->is_entry_node)
							//	if (!x->is_internaly_active_node(target,node_vector))
							//		cout << "Error in internaly active" << endl;
							//cout << "Entry node found. Dove tail continues with only this branch" << endl;
							} // end if (check_other_side && (x->is_entry_node) delta==1)
						// end patch

						} // end of else (x->marked==0)
					} // end of else ( x is not active)
				}  // end of if(x->twin_link==x)
			else 		// (x->twin_linh!=x)      // x is root
				{
				#ifdef OUTPUT
				cout << "x=" << x->label << " root node." << endl;
				#endif
				if (x->DFI==target)
					{
					if (abs(x->marked)>w)
						{
						(x->marked)=-w;
						secondary.append(x);
						}
					//walk_up_stack.push(start_node);
					dt_stack[current].clear();
					//while (!dt_stack[current].empty()) // Copy of dt_stack[current] in walk_up_stack
						//{
						//temp_bj=dt_stack[current].pop();
                        //if (temp_bj->twin_link!=temp_bj)
						//		cout << " -2- root node inserted in walk up stack" << endl;

						//walk_up_stack.push(temp_bj);
						//}

					while (!dt_stack[(current+1)%2].empty())
						{
						//temp_bj=dt_stack[(current+1)%2].pop();
						//temp_bj->marked=INT_MAX;
						(dt_stack[(current+1)%2].pop())->marked=INT_MAX;
						}

					//success=true;
					//cout << "**************         WALK UP SUCCESS!!      *****************" << endl;
					return true; //success;
					}   // end of if(x->graph_node==target)
				else   // x->graph_node!=target
					{
					///////////////////////////////////////
					// PATCH
					///////////////////////////////////////
					//cout << "INIZIO PATCH" << endl;
                    bool invert_current_walk_up=false;
					bool invert_previous_walk_up=true;

                    if (check_other_side || not_invert_side)
						{
                        //cout << "Check other side is true" << endl;
						check_other_side=false;
                        invert_previous_walk_up=false;
						//if ((x->entry_nodes_counter==counter[current]) || counter[current]>1 || not_invert_side)
						if (counter[current]>1 || not_invert_side)
							{
							#ifdef OUTPUT
							cout << "This choice is correct for the entry nodes" << endl;
							#endif
							}
						else
							{
                            if (abs(x->marked)>w)
								{
								x->can_be_flipped=true; // not in this walk up
								#ifdef OUTPUT
								cout << "can be flipped=true on bic_obj " << x->label << endl;
								cout << "Start node=" << start_node->label << endl;
								#endif
                                }
							else // root x is marked
								{
								#ifdef OUTPUT
								cout << "root node is already marked" << endl;
								#endif
								if (x->active_walk_up)
                                    {
									#ifdef OUTPUT
									if (x->active_walk_up) cout << "x->active_walk_up==true" << endl;
									#endif
									x->can_be_flipped=false;
									}
								else
									{
                                    x->can_be_flipped=true;
                                    invert_current_walk_up=true;
									#ifdef OUTPUT
									cout << "Current walk p must be inverted" << endl;
									#endif
									}
								}
                            }

						x->active_walk_up=true;
						not_invert_side=false;
						#ifdef OUTPUT
						cout << "set active_walk_up on bic_obj" << x->label<< endl;
						#endif
						} // end if (check_other_side)


					dt_stack[current].pop();
					if (abs(x->marked)>w)
						{
						twin_link_previous=true;
						//cout << "set twin_link_previous=true"  << endl;
						//cout << "x is not marked" << endl;
						x->marked=-w;
						if (node_vector[x->first_child->plan_id].lowpoint>=w)   // child bicomp not active
							{
							((bic_obj_node*)(x->twin_link))->not_active_bicomp.append(x);
							active_bicomp_previous=false;
							#ifdef OUTPUT
							cout << "Added "<< x->label << " at non-active bicomp list" << endl;
							//cout << "set active_bicomp_previous=false" << endl;
							#endif
							}
						else //  child_lowpoint<target;      child bicomp active
							{
							((bic_obj_node*)(x->twin_link))->active_bicomp.append(x);
							((bic_obj_node*)(x->twin_link))->active_passages++;
							active_bicomp_previous=true;
							#ifdef OUTPUT
							cout << "Aggiunto "<< x->label << " alla lista bicomp attive" << endl;
							#endif
							//cout << "set active_bicomp_previous=true" << endl;
							}  // end of else (child_lowpoint < target)
							// End of dove-tail for this bicomp
						dt_stack[current].clear();
      					while (!dt_stack[(current+1)%2].empty())
							{
							//temp_bj=dt_stack[(current+1)%2].pop();
							//temp_bj->marked=INT_MAX;
							(dt_stack[(current+1)%2].pop())->marked=INT_MAX;
							}
						// x->check_bicomp();
						x=(bic_obj_node*)x->twin_link;
						#ifdef OUTPUT
						cout << "Now x= " << x->label << endl;
						#endif
						}   // end of if(x->marked==0)
					else  // (abs(x->marked)==w)
						{
						if ((node_vector[x->first_child->plan_id].lowpoint)>=w)
							{
							delta=2;
							index++;
							while (!dt_stack[current].empty())
								{
								//temp_bj=dt_stack[current].pop();
								//temp_bj->marked=w+1;
								(dt_stack[current].pop())->marked=INT_MAX;
								}
							x->marked=-w;  // root node has been unmarked because it was in dt_stack[current]
							}  // end of if ((x->child_lowpoint)>=node_vector[target]->DFI)
						else // (x->child_lowpoint)>=node_vector[target]->DFI);    this is active child bicomp
							{
							active_bicomp_previous=true;
							//////////////////////////////////////////////////////
                            //PREVIOUS WALK UP MODIICATION
                            if (x->can_be_flipped && invert_previous_walk_up)
								{
								x->can_be_flipped=false;
                                invert_previous_walk_up=false;
								#ifdef OUTPUT
								cout << "Inversion of previous walk up" << endl;
								cout << "Root=" << x->label << endl;
								cout << "Start node= " << start_node->label;
								//x->check_bicomp();
								if (x->black==x->white) cout << "\nElementary bicomp" << endl;
								#endif
								bool stop_visit=false;
								//bool flip_bicomp=false;
								bic_obj*y=NULL;
								int temp_current=(current+1)%2;
								gdt::gdtstack<bic_obj*> temp_dt_stack;
								while (!stop_visit)
									{
                                    bic_obj* connector1=next[temp_current];
									connector1->marked=-w;
									temp_dt_stack.push(connector1);
									#ifdef OUTPUT
									cout << "VISITED BIC_OBJ: " << connector1->label << endl;
									#endif
									bic_obj* connector2=connector1->twin_link;
									//temp_candidate_last_bicomp_connector=connector2;
									#ifdef OUTPUT
									cout << "VISITED BIC_OBJ: " << connector2->label << endl;
									#endif
									connector2->marked=-w;
									temp_dt_stack.push(connector2);
									y=connector2->black;
									if (!y->is_node) y=connector2->white;
									temp_dt_stack.push(y);
									#ifdef OUTPUT
									cout << "y= " << y->label<< endl;
									if (y->twin_link!=y)
										{
                                        cout << "y root node" << endl;
										}
									#endif
									if (abs(y->marked)==w)
										{
                                        stop_visit=true;
										//bool flip_bicomp=true;
										bool stop_visit2=false;
										if (y->black==connector2)
											next[temp_current]=y->white;
										else next[temp_current]=y->black;
										#ifdef OUTPUT
										cout << "unmark previous side" << endl;
										#endif
										while (!stop_visit2)
											{
											bic_obj* c1=next[temp_current];
											c1->marked=INT_MAX;
											#ifdef OUTPUT
											cout << "unmarked " << c1->label << endl;
											#endif
											bic_obj* c2=c1->twin_link;
											c2->marked=INT_MAX;
											#ifdef OUTPUT
											cout << "unmarked " << c2->label << endl;
											#endif
											bic_obj* y2=c2->black;
											if (!y2->is_node) y2=c2->white;
											if (y2==x) // root node
												stop_visit2=true;
											else
												{
                                                y2->marked=w+1;
												//cout << "smarcato " << y2->label << endl;
												if (y2->black==c2)
													next[temp_current]=y2->white;
												else next[temp_current]=y2->black;
												}
											} // while (!stop_visit2)
										if (delta==2)  //
											{
											bool stop_visit3=false;
                                            if (abs(start_node->black->marked)==w && abs(start_node->white->marked)==w)
												stop_visit3=true;
											#ifdef OUTPUT
                                            cout << "\n\ndelta=2  - recover of failed dove-tail side" << endl;
                                            if (abs(start_node->black->marked)==w && abs(start_node->white->marked)==w)
											cout << "Niente dA MARCARE" << endl;
											#endif
											bic_obj* next;
											if (abs(start_node->black->marked)==w)
												next=start_node->white;
											else
												next=start_node->black;

											while (!stop_visit3)
												{
                                                bic_obj* tbj1=next;
												tbj1->marked=-w;
												#ifdef OUTPUT
												cout << "marked " << tbj1->label << endl;
												#endif
                                                bic_obj* tbj2=tbj1->twin_link;
                                                tbj2->marked=-w;
												#ifdef OUTPUT
												cout << "marked " << tbj2->label<< endl;
												#endif
                                                bic_obj* tbjnode=tbj2->black;
												if (!tbjnode->is_node)
                                                     tbjnode=tbj2->white;
                                                if (abs(tbjnode->marked)==w)
													stop_visit3=true;
												else
													{
													tbjnode->marked=-w;
													#ifdef OUTPUT
													cout << "marked" << tbjnode->label << endl;
													#endif
													if (tbjnode->black==tbj2)
														next=tbjnode->white;
													else
														next=tbjnode->black;
													}
												}
											}
										return true;
										} // if (abs(y->marked)==w)
									else
										{
                                        if (y->is_active(target,node_vector))
											{
											#ifdef OUTPUT
											cout << "Active node found" << endl;
											#endif
											stop_visit=true;
											temp_dt_stack.pop();
											while(!temp_dt_stack.empty())
												{
                                                //bic_obj* tbj=temp_dt_stack.pop();
												//tbj->marked=INT_MAX;
												(temp_dt_stack.pop())->marked=INT_MAX;
												}
											}
										else // y is not active
											{
											#ifdef OUTPUT
											cout << "Not active node" << endl;
											#endif
											if (y->black==connector2)
												next[temp_current]=y->white;
											else
												next[temp_current]=y->black;
											y->marked=-w;
											}
										} //end of else (y is not marked)
									}
								#ifdef OUTPUT
								cout << "\nEnd of walk up modification" << endl;
								//x->check_bicomp();
								#endif
								}

                            ///////////////////////////////////////////////////////7

							if (x->can_be_flipped && invert_current_walk_up)
								{
                                x->can_be_flipped=false;
                                invert_current_walk_up=false;
								#ifdef OUTPUT
								cout << "Inversion of current walk up" << endl;
								cout << "Root=" << x->label << endl;
								cout << "Start node= " << start_node->label;
								//x->check_bicomp();
								if (x->black==x->white) cout << "\nElementary bicomp" << endl;
								#endif
								bool stop_visit=false;
								bic_obj*y=NULL;
								int temp_current=(current+1)%2;
								gdt::gdtstack<bic_obj*> temp_dt_stack;
								while (!stop_visit)
									{
                                    bic_obj* connector1=next[temp_current];
									connector1->marked=-w;
									temp_dt_stack.push(connector1);
									#ifdef OUTPUT
									cout << "VISITED BIC_OBJ: " << connector1->label << endl;
									#endif
									bic_obj* connector2=connector1->twin_link;
									//temp_candidate_last_bicomp_connector=connector2;
									#ifdef OUTPUT
									cout << "VISITED BIC_OBJ: " << connector2->label << endl;
									#endif
									connector2->marked=-w;
									temp_dt_stack.push(connector2);
									y=connector2->black;
									if (!y->is_node) y=connector2->white;
									temp_dt_stack.push(y);
									#ifdef OUTPUT
									cout << "y= " << y->label<< endl;
									if (y->twin_link!=y)
										{
                                        cout << "y root node" << endl;
										}
									#endif
									if (abs(y->marked)==w && !y->is_active(target,node_vector))
										{
                                        stop_visit=true;
										//bool stop_visit2=false;
										#ifdef OUTPUT
										cout << "unmark previous side" << endl;
										#endif
										while (!dt_stack[current].empty())
											{
                                            //bic_obj* tbj=dt_stack[current].pop();
											//tbj->marked=INT_MAX;
											(dt_stack[current].pop())->marked=INT_MAX;
											}
										start_node->marked=-w;
										return true;
										} // if (abs(y->marked)==w)
									else
										{
                                        if (y->is_active(target,node_vector))
											{
											#ifdef OUTPUT
											cout << "Active node found" << endl;
											#endif
											stop_visit=true;
											temp_dt_stack.pop();
											while(!temp_dt_stack.empty())
												{
                                                //bic_obj* tbj=temp_dt_stack.pop();
												//tbj->marked=INT_MAX;
												(temp_dt_stack.pop())->marked=INT_MAX;
												}
											}
										else // y is not active
											{
											#ifdef OUTPUT
                                            cout << "Not active node" << endl;
											#endif
											if (y->black==connector2)
												next[temp_current]=y->white;
											else
												next[temp_current]=y->black;
											y->marked=-w;
											}
										} //end of else (y is not marked)
									}
								#ifdef OUTPUT
								cout << "\nEnd of walk up modification" << endl;
								//x->check_bicomp();
								#endif
								}

							// END OF WALK UP MODIFICATION
							/////////////////////////////////////////////////////
							//cout << "set active_bicomp_previous=true" << endl;
							//walk_up_stack.push(start_node);
							//cout << "x is root, child bicomp active" << endl;
							dt_stack[current].clear();

							while (!dt_stack[(current+1)%2].empty())
								{
								//temp_bj=dt_stack[(current+1)%2].pop();
								//temp_bj->marked=INT_MAX;
								(dt_stack[(current+1)%2].pop())->marked=INT_MAX;
								}

							((bic_obj_node*)(x->twin_link))->active_passages++;
							if (x->is_root_externally_active(w,x->first_child,node_vector))
								{
								#ifdef OUTPUT
                                cout << "Il walk up chiuderà un nodo attivo" << endl;
								#endif
								return false;
								}
							x=(bic_obj_node*)(x->twin_link);
							twin_link_previous=true;
							//cout << "now x=" << x->label << endl;
							}  // end of else (x->child_lowpoint)>=node_vector[target]->DFI);
						}  // end of else (x->marked==0)
					}  // end of else (x->graph_node!=target)
				}   // end of else (x->twin_link!=x)
			}  // end of if(!twin_link_previous)
		else //   twin_link_previus==true; x is entry node
			{
			twin_link_previous=false;
			//cout << "x=" << x->label << " is entry node"  << endl;
			//if (x->twin_link==x) cout << "Twin on entry node OK" << endl;
			#ifdef OUTPUT
			if (x->is_active(w,node_vector)) cout << "Dove tail starts from active node node" << endl;
			//x->check_bicomp();
			#endif

			if ((abs(x->marked)==w)&& !active_bicomp_previous)
				{
				//stop=true;
				//success=true;
				//cout << "x is marked and active_bicomp_previous=false" << endl;
				//cout << "**************         WALK UP SUCCESS!!      *****************" << endl;
				return true;
				}
			if ((abs(x->marked)==w) && active_bicomp_previous)
				{
				//cout << "x is marked and active_bicomp previous==true" << endl;
				if ((x->black!=x->white) && ((x->black)->marked==-w) && ((x->white)->marked==-w))
					{   //
					//stop=true;
					//success=false;
					//cout << "Walk Up fail because of entry node with 2 connectors marked and active_bicomp_previous=true" << endl;
					return false;
					}
				if ((x->black==x->white) && ((x->black)->marked==w))
					{
					#ifdef OUTPUT
					cout << "Walk Up fail because of an entry node in elementary bicomp with connectors marked twice and active_bicomp_previous=true" << endl;
					#endif
					return false;
					}
				// Here x->black->marked==1  or x->white->marked==1
				if ((((x->black)->marked==-w)  || ((x->white)->marked==-w)) && (x->active_passages==1))
					{
					// Accept previous marked
					//if (x->black==x->white) last_bicomp_connector->walk_down_hint_black=false;
					//else last_bicomp_connector->walk_down_hint_black=(abs((x->black)->marked)==w);
					//cout << "Set walk_down_hint_black=";
									//cout << "**************         WALK UP SUCCESS!!      *****************" << endl;
					return true;
					}

				#ifdef OUTPUT
				cout <<  "Dove tail start with only 1 branch\tactive_passages=" << x->active_passages << endl;
				#endif
				delta=2;
				current=0;
				//cout << "set delta=2" << endl;
				if (x->black==x->white)
					{
					index=0;      // la prima volta che visito una bicomp elementare index=0, delta=1,
         						  // quindi vado sicuramente dal lato 1 con index=index+delta
								  // adesso devo salire dal lato 0
					next[0]=x->black;
					}
				else
					{
					if (x->black->marked==-w)
						{
						index=1;
						next[1]=x->white;
						}
					if (x->white->marked==-w)
						{
						index=0;
						next[0]=x->black;
						}
					}
				previous[0]=x;
				previous[1]=x;
				start_connector_black[0]=true;
				start_connector_black[1]=false;
				start_node=x;
				not_invert_side=true;

				//cout << "next[current]=" << next[current]->label << endl;

				}   // end of if ((x->marked>0) && active_bicomp_previous)
			if (abs(x->marked)>w)
				{
				//cout << "x is not marked" << endl;
				active_bicomp_previous=false;
				//cout << "set active_bicomp_previous=false" << endl;
				x->marked=-w;
				#ifdef OUTPUT
				cout << "Start new dove tail" << endl;
				#endif
				if (x->is_active(w,node_vector))   // patch
					{
					check_other_side=true;
					counter[0]=1;
					counter[1]=1;
					//cout << "Dove tail starts from active node. " << endl;
					}
				// x->check_bicomp();
				start_node=x;
				delta=1;
				index=0;
				current=0;
				next[0]=x->black;
				next[1]=x->white;
				//cout << "next[0]=" << next[0]->label << endl;
				//cout << "next[1]=" << next[1]->label << endl;
				start_connector_black[0]=true;
				start_connector_black[1]=false;
				previous[0]=x;
				previous[1]=x;
				}
			}  // end of else (twin_link_previous=true)
		} // End of while


	return success;

	};





/* Walk Down. Add back-edges to the bicomp structure.*/


bool
	bic_obj_node::
	walk_down
	(
	bm_node_info node_vector[],
	//list_item position_in_parent_list[],
	//list<node> children_ordered[],
	gdt::gdtlist<bic_obj_node*>& secondary,
	undi_graph* ug,
	//stack<bic_obj*>& embedding_stack,
	bool edge_to_be_inserted[],
	int flip[],
	bic_obj*& bic_obj_actual_pointer
	)
	{
	#ifdef OUTPUT
	cout << "Function Walk_down activated."  << endl;
	#endif
	#ifdef OUTPUT
	cout << "TEST EDGE_TO_BE_INSERTED values:" << endl;
	gdtedge e_test;
	forall_edges (e_test,*ug)
		{
		cout << "Edge: " << ug->id(e_test) << "\tedge_to_be_inserted=";
		if (edge_to_be_inserted[e_test->plan_id]) cout << "TRUE" << endl;
		else cout << "FALSE" << endl;

		}
	#endif
	gdt::gdtstack<bic_obj_node*> walk_down_stack;
	//gdt::gdtstack<bic_obj_node*> previous_nodes_stack;

	bic_obj* actual_connector = (bic_obj*)NULL;
	while (!secondary.empty())
		{
		bic_obj_node* y=secondary.pop();
		#ifdef OUTPUT
		cout << "y=" << y->label << endl;
		#endif
		//cout << "y->black=" << y->black->label << "    y->white=" << y->white->label << endl;
        	int w=y->DFI;//  DFI_vector[ug->id(y->graph_node)];
		//cout << "w= " << w << endl;
		int connectors_marked=0;
		bool y_is_elementary = false;  // TITTO: initialized to false (03/06/04)
		//cout << "y->black->marked=" << y->black->marked << "\ty->white->marked=" << y->white->marked << endl;
		if ((y->black->marked==w) || ((y->black->marked==-w) && (y->white->marked==-w) && (y->black!=y->white) ))
			{
			connectors_marked=2;
			#ifdef OUTPUT
			cout << "OK, connectors_marked=" << connectors_marked << endl;
			#endif
			actual_connector=y->black;
			// a tree edge is added to embedding stack
			//embedding_stack.push(actual_connector);
			}
		 else
			{
			if ((y->black->marked==-w) || (y->white->marked==-w))
				{
				connectors_marked=1;
				#ifdef OUTPUT
				cout << "OK, connectors_marked=" << connectors_marked << endl;
				#endif
				if ((y->black)->marked==-w) actual_connector=y->black;
				else actual_connector=y->white;
				}
			};
		while (connectors_marked>0)
			{
			#ifdef OUTPUT
			cout << "start iteration with y=" << y->label << ", actual connector=" << actual_connector->label << endl;
			#endif
			if (y->black==y->white) y_is_elementary=true;
			else y_is_elementary=false;
			bic_obj* temp_bj1=actual_connector->twin_link;
			bic_obj_node* temp_bj;
			bic_obj_node* previous_node=y;
			//bool now_add_back_edges=false;

			// bic_obj* temp_bj_connector=temp_bj;
			if (y->DFI==1)
				flip[actual_connector->graph_edge->plan_id]=1;
			if ((temp_bj1->black)->is_node) temp_bj=(bic_obj_node*)temp_bj1->black;
			else temp_bj=(bic_obj_node*)temp_bj1->white;
			/*if (!temp_bj->is_node)
				{
				cout << "Error in bicomp structure" << endl;
				return false;
				};
                	*/
			if (connectors_marked==1) temp_bj->actual_previous_connector=true;
			if (connectors_marked==2) temp_bj->actual_previous_connector=false;
			walk_down_stack.push(temp_bj);
			//previous_nodes_stack.push(y);
			bool adj_to_y=true;
			bic_obj* previous_connector;   // Connettore con cui sono giunto al nodo corrente
			bic_obj* previous_connector2;   // Connettore con cui sono giunto al nodo precedente
			//bic_obj_node* last_node=y;
			//bic_obj_node* last_node2;
			//bic_obj_node* pre_x2;
			while (!walk_down_stack.empty())
				{
				bic_obj_node* x=walk_down_stack.pop();  // current node;
				//bic_obj_node* pre_x=previous_nodes_stack.pop();  // this node placed x in walk_down_stack

				//x->is_entry_node=false;   // unmark all entry nodes
				#ifdef OUTPUT
				cout << "VISITED BIC_OBJ: " << x->label << endl;
				#endif
				//cout << "pre_x= ";
				/*if (pre_x!=NULL) cout << pre_x->label << endl;
				else cout << "deleted node" << endl;*/
				//cout << "Last_node= ";
				//if (last_node!=NULL) cout << last_node->label << endl;
				//else cout << "deleted node" << endl;


				if (x->twin_link!=x)   // x is root
					{

					#ifdef OUTPUT
					cout << "x is root node " << x->label << endl;
					cout << "x->black->marked=" << x->black->marked << "\tx->white->marked=" << x->white->marked << endl;
					#endif
					//if (pre_x!=NULL && last_node!=NULL && pre_x!=last_node)
					//	{
					//	last_node2=last_node;
					//	pre_x2=pre_x;
					//	now_add_back_edges=true;
					//	}
					if (x->twin_link->actual_previous_connector)
						previous_connector2=x->twin_link->black;
					else previous_connector2=x->twin_link->white;
					//cout << "Previous connector2= " << previous_connector2 ->label << endl;
					previous_node=x->twin_link;
					//cout << "Previous_node= " << previous_node->label << endl;
					//cout << "previous_node->black= " << previous_node->black->label << "     ";
					//cout << "previous_node->white= " << previous_node->white->label << endl;
					bic_obj* next_connector1=NULL;
					bic_obj* next_connector2=NULL;
                    #ifdef OUTPUT
					if ( (abs(x->black->marked)>w) && (abs(x->white->marked)>w) )
						{
						cout << "No connectors marked. Error in structure" << endl;
						return false;
						}
					#endif
					if (((x->black->marked==-w) && (x->white->marked==-w) && (x->black!=x->white)) || (x->black->marked==w))
						{
						#ifdef OUTPUT
						cout << "2 connectors marked" << endl;
						#endif
						next_connector1=x->black;
      /*
						bool test1=x->black->walk_down_hint_black;
						bool test2=x->white->walk_down_hint_black;
						if (((test1) && (previous_node->actual_previous_connector))
							||
						   ((!test1) && (!previous_node->actual_previous_connector)))
								next_connector1=x->black;
						else
							{
							if (((test2) && (previous_node->actual_previous_connector))
								||
						   	((!test2) && (!previous_node->actual_previous_connector)))
									next_connector1=x->white;
							}
						if (next_connector1==NULL)
							{
							//cout << "Error in walk down hint black. -1-" << endl;
							return false;
							}
                         */
						} // end of if (((x->black->marked==1) && (x->white->marked==1)) || (x-black->marked==2))
					else	// Only one connector marked
						{
						#ifdef OUTPUT
						cout << "1 connector marked" << endl;
						#endif
						if ((x->black->marked)==-w) next_connector1=x->black;
						else next_connector1=x->white;
						} // end of else Only one connector marked
					//cout << "Next connector1=" << next_connector1->label << endl;
					next_connector2=next_connector1->twin_link;

					//if (next_connector2->is_node)
					//	{
					//	cout << "Error in bicomp structure." << endl;
					//	return false;
					//	}
					bic_obj_node* next_node=(bic_obj_node*)NULL;
					if (next_connector2->black->is_node) next_node=(bic_obj_node*)next_connector2->black;
					else next_node=(bic_obj_node*)next_connector2->white;
					//if (!next_node->is_node)
					//	{
					//	cout << "Error in bicomp structure" << endl;
					//	return false;
					//	}
					if (x->black==x->white)
						{
						if (x->black->marked==w)
							next_node->actual_previous_connector=false; // elementary bicomp with 2 connectors marked
						if (x->black->marked==-w)
							next_node->actual_previous_connector=true; // elementary bicomp with 1 connector marked
						}
					else next_node->actual_previous_connector=(next_node->black==next_connector2);
					walk_down_stack.push(next_node);
   					  //previous_nodes_stack.push(NULL);
					//cout << "Put Next node=" << next_node->label << " in walk_down_stack" << endl;
					//cout << "next_node->black= " << next_node->black->label << next_node->black->marked;
					//cout << "   next_node->white= " << next_node->white->label << next_node->white->marked <<endl;
					//cout << "next_node->actual_previous_connector= ";
					//if (next_node->actual_previous_connector) cout << "true" << endl;
					//else cout << "false" << endl;

					//////////////////////////////////////
					// fusion of two bicomp
					//////////////////////////////////////

					gdtnode child=x->first_child;
					//if (child==NULL_NODE) cout << "Null Node" << endl;
					gdt::list_item l=node_vector[child->plan_id].position_in_parent_list;
					node_vector[x->graph_node->plan_id].children_ordered.erase(l);
     				//cout << "Delete bic_obj " << node_vector[child]->DFI << " from parent list" << endl;

					bic_obj* other_connector;

					if ((x->black)==next_connector1) other_connector=x->white;
					else other_connector=x->black;
					///////////////////////////////////////////////////////////
					//SETTING FLIP ON TREE EDGE
					///////////////////////////////////////////////////////////
					gdtedge fc_edge=ug->find_edge(x->graph_node,x->first_child);
					#ifdef OUTPUT
					if (fc_edge==NULL_EDGE)
						{
						cout << "Setting flip value on NULL_EDGE!" << endl;
						return false;
						}
					#endif
  					if (previous_node->black==previous_node->white) // parent is elementary
						if (x->black==x->white) // parent and child are elementary
							{
                        	#ifdef OUTPUT
							cout << "Parent bicomp is elementary" << endl;
							cout << "Child bicomp is elementary" << endl;
							cout <<" x->black=" << x->black->label <<"\tx->white=" << x->white->label << endl;
							cout << "previous_node->black=" << previous_node->black->label << "\tprevious_node->white=";
							cout << previous_node->white->label << endl;
							cout << "x->black->marked=" << x->black->marked << "\tx->white->marked=" << x->white->marked;
							cout << endl;

							cout << "previous_node->black->marked=" << previous_node->black->marked << "\tprevious_node->white->marked=" << previous_node->white->marked;
							cout << " **\n" << endl;
							cout << "TEST EDGE_TO_BE_INSERTED values before setting flip:" << endl;
							//gdtedge e_test;
							forall_edges (e_test,*ug)
								{
								cout << "Edge: " << ug->id(e_test) << "\tedge_to_be_inserted= ";
								if (edge_to_be_inserted[e_test->plan_id]) cout << "TRUE" << endl;
								else cout << "FALSE" << endl;
								}


							#endif
							if (previous_node->black->marked==x->black->marked)
								flip[fc_edge->plan_id]=1;
							//previous_node->black->marked=-w;
							else
								flip[fc_edge->plan_id]=-1;
							//x->black->marked=-w;


							#ifdef OUTPUT
							cout << "TEST EDGE_TO_BE_INSERTED values after setting flip:" << endl;
							gdtedge e_test;
							forall_edges (e_test,*ug)
								{
								cout << "Edge: " << ug->id(e_test) << "\tedge_to_be_inserted= ";
								if (edge_to_be_inserted[e_test->plan_id]) cout << "TRUE" << endl;
								else cout << "FALSE" << endl;
								}
							#endif
							}
						else  // parent is elementary, child is not elementary
							{
                            #ifdef OUTPUT
							cout << "Parent elementary - child non-elementary" << endl;
							#endif
                            if ( (previous_node->black->marked==-w) && ((x->black->marked)==-w) ||
                                ((previous_node->black->marked==w) && ((x->white->marked)==-w) && ((x->black->marked)!=-w)) )
								flip[fc_edge->plan_id]=-1;
							else
								flip[fc_edge->plan_id]=1;
							}
                    else // parent is not elementary
						if (x->black==x->white)
							{
                          	#ifdef OUTPUT
							cout << "Parent non-elementary - child elementary" << endl;
							cout << "previous_node->black->marked=" << previous_node->black->marked;
							cout << "\tprevious_node->white->marked=" << previous_node->white->marked << endl;
							#endif
                            if ((previous_node->actual_previous_connector && (x->black->marked==-w)) ||
								(!previous_node->actual_previous_connector && (x->black->marked==w)) )
								flip[fc_edge->plan_id]=1;
							else
								flip[fc_edge->plan_id]=-1;
							//x->black->marked=-w;
							}
						else // General case: parent and child are non-elementary
							{
							#ifdef OUTPUT
                            cout << "General case:"<< endl;
							if (previous_node->actual_previous_connector)
								cout << "Previous_node->actual_prev_connector=true"<< endl;
							else
								cout << "Previous_node->actual_prev_connector=false" <<endl;
							#endif
							if ((previous_node->actual_previous_connector && (x->black==next_connector1))
								|| (!(previous_node->actual_previous_connector) && (x->white==next_connector1)))
								{
                        		// La bicomp viene flippata
								flip[fc_edge->plan_id]=-1;
								#ifdef OUTPUT

								cout <<" x->black=" << x->black->label <<"\tx->white=" << x->white->label << endl;
								cout << "previous_node->black=" << previous_node->black->label << "\tprevious_node->white=";
								cout << previous_node->white->label << endl;
								cout << "x->black->marked=" << x->black->marked << "\tx->white->marked=" << x->white->marked;
								cout << endl;
                                cout << "previous_node->black->marked=" << previous_node->black->marked << "\tprevious_node->white->marked=" << previous_node->white->marked;
								cout << " **\n" << endl;
								#endif
								}
							else
								{
                        		// La bicomp NON viene flippata
                        		flip[fc_edge->plan_id]=1;
								#ifdef OUTPUT
								cout <<" x->black=" << x->black->label <<"\tx->white=" << x->white->label << endl;
								cout << "previous_node->black=" << previous_node->black->label << "\tprevious_node->white=";
								cout << previous_node->white->label << endl;
								cout << "x->black->marked=" << x->black->marked << "\tx->white->marked=" << x->white->marked;
								cout << endl;
                                cout << "previous_node->black->marked=" << previous_node->black->marked << "\tprevious_node->white->marked=" << previous_node->white->marked;
								cout << " **\n" << endl;
								#endif
								}
							}
					#ifdef OUTPUT
					cout << "\n**\nSet flip=" << flip[fc_edge->plan_id]<< "   on edge " << ug->id(fc_edge)  << " ";
					cout << " **\n" << endl;
					cout << "TEST EDGE_TO_BE_INSERTED values:" << endl;
					gdtedge e_test;
					forall_edges (e_test,*ug)
						{
						cout << "Edge: " << ug->id(e_test) << "\tedge_to_be_inserted=";
						if (edge_to_be_inserted[e_test->plan_id]) cout << "TRUE" << endl;
						else cout << "FALSE" << endl;
						}
					#endif

                    ///////////////////////////////////////////////////////////
					// END SETTING FLIP
					///////////////////////////////////////////////////////////
					if(previous_connector2->black==previous_node)
						previous_connector2->black=next_connector1;
					else previous_connector2->white=next_connector1;

					if (next_connector1->black==x)
						next_connector1->black=previous_connector2;
					else next_connector1->white=previous_connector2;


					if (previous_node->actual_previous_connector)
						{
                        #ifdef OUTPUT
                        cout << "Black pointer on previous node will be modified" << endl;
						#endif
						previous_node->black=other_connector;
						}
					else
						{
                        #ifdef OUTPUT
                        cout << "white pointer on previous node will be modified" << endl;
						#endif

						previous_node->white=other_connector;
						}

					if (other_connector->black==x)
						other_connector->black=previous_node;
					else other_connector->white=previous_node;



					//cout << "Delete bicomp" << x->label << endl;
					//cout << "previous_node->black= " << previous_node->black->label << endl;
					//cout << "previous_node->white= " << previous_node->white->label << endl;

					//bic_obj_created.erase(x->position_in_bic_obj_created);
					//delete x;
					//last_node=NULL;
					//walk_down_fusion->pause();
					} // end of  if (x->twin_link==x)

				else	//   x-twin_link==x
					{
					#ifdef OUTPUT
					cout << "x is not root in this bicomp" << endl;
					cout << "TEST EDGE_TO_BE_INSERTED values:  " << endl;
							gdtedge e_test1;
							forall_edges (e_test1,*ug)
								{
								cout << "Edge: " << ug->id(e_test1) << "\tplan id=" << e_test1->plan_id<< "\t";
								if (edge_to_be_inserted[e_test1->plan_id]) cout << "TRUE" << endl;
								else cout << "FALSE" << endl;
								}
					#endif
					//walk_down_critical->start();
					//bool success_next_node=true;
					bool exist_next_node=false;
					if (x->actual_previous_connector)
						previous_connector=x->black;
					else previous_connector=x->white;

					

					//cout << "Previous node=" << previous_node->label << endl;
					//cout << "Previous connector=" << previous_connector->label << endl;
					bic_obj_node* next_node=(bic_obj_node*)NULL;
					bic_obj* temp_previous_connector = (bic_obj*)NULL;
					
					if ((abs(x->black->marked)>w) || (abs(x->white->marked)>w))
						exist_next_node=false;
					else  // 2 marked connectors
						{
						#ifdef OUTPUT
						cout << "x->black= " << x->black->label << "    x->white= " << x->white->label << endl;
						#endif
						if (x->actual_previous_connector)
							{
							next_node=x->next_node(false);
							temp_previous_connector=x->white;
							}
						else
							{
							next_node=x->next_node(true);
							temp_previous_connector=x->black;
							}
						
						
						
						if (next_node==previous_node) exist_next_node=false;   // elementary bicomp
							else exist_next_node=true;
						} // end of else // 2 marked connectors

					//if (exist_next_node) cout << "Next node= " << next_node->label << endl;
					////////////////////////////////////////////////////////////
					// Insertion of elements in walk down stack
					////////////////////////////////////////////////////////////
					
					
					
					
					
					if (exist_next_node) {
					if ((x->active_passages==0) && (abs(next_node->marked)==w))
						{
						next_node->actual_previous_connector=(next_node->black==temp_previous_connector->twin_link);
						walk_down_stack.push(next_node);
						#ifdef OUTPUT
						cout << "Put next_node=" << next_node->label << " in walk_down_stack" << endl;
						#endif
						} 
					}
					
					if (x->active_passages>0)
						{
						bic_obj_node* temp_active_root;

						gdt::list_item temp_li=x->active_bicomp.first();
						temp_active_root=x->active_bicomp.inf(temp_li);
						walk_down_stack.push(temp_active_root);
						#ifdef OUTPUT
                        cout << "Put " << temp_active_root->label << " in Walk Down Stack" << endl;
						cout << "black->marked=" << temp_active_root->black->marked << "\twhite->marked=";
						cout << temp_active_root->white->marked << endl;
						#endif
						//previous_nodes_stack.push(x);
						//if (temp_active_root->black==temp_active_root->white)
						//	{
						//	temp_active_root->black->marked=-w;
      					//	}
						x->active_bicomp.erase(temp_li);
						if (x->active_bicomp.length()==0)
							x->active_passages=0;
						else x->active_passages=1;
						}

					
					if (x->not_active_bicomp.length()>0)
						{
						bic_obj_node* temp_not_active_root;
						forall (temp_not_active_root, x->not_active_bicomp)
							{
							#ifdef OUTPUT
							cout << "Put not active bicomp " << temp_not_active_root->label << " in walk down stack" << endl;
							#endif
							walk_down_stack.push(temp_not_active_root);
							//previous_nodes_stack.push(x);
							}
						x->not_active_bicomp.clear();
						}
					//walk_down_critical->pause();
					//////////////////////////////////////
					//  Insertion of back edge
					//////////////////////////////////////
					
					if (adj_to_y)
						{
						adj_to_y=false;
						//walk_down_critical->pause();
						}
					else
						{

						gdtedge back_edge=ug->find_edge(x->graph_node, y->graph_node);

						bic_obj* e1;
						bic_obj* e2;
						#ifdef OUTPUT
						if (back_edge==NULL_EDGE)
							cout << "There is no back edge between " << x->label << " and " << y->label << endl;
						else
							{
							cout << "Back edge found between " << x->label << " and " << y->label << endl;
							cout << "Back edge id= " << ug->id(back_edge) << "\tplan_id= " << back_edge->plan_id << endl;
							if (edge_to_be_inserted[back_edge->plan_id])
								cout << "The back edge must be inserted" << endl;
							else
								cout << "The back edge is already inserted." << endl;
							cout << "TEST EDGE_TO_BE_INSERTED values:" << endl;
							gdtedge e_test1;
							forall_edges (e_test1,*ug)
								{
								cout << "Edge: " << ug->id(e_test1) << "\tplan id=" << e_test1->plan_id<< "\t";
								if (edge_to_be_inserted[e_test1->plan_id]) cout << "TRUE" << endl;
								else cout << "FALSE" << endl;
								}
							}
						#endif
						if (back_edge!=NULL_EDGE)
							if (edge_to_be_inserted[back_edge->plan_id])
							{
							//walk_down_add_back->start();
							#ifdef OUTPUT
							cout << "Insertion of back edge between " << x->label << " and " << y->label << endl;
							#endif

							e1=bic_obj::my_new_bic_obj(back_edge,bic_obj_actual_pointer);//new bic_obj(back_edge);
							e2=bic_obj::my_new_bic_obj(back_edge,bic_obj_actual_pointer);//new bic_obj(back_edge);
							edge_to_be_inserted[back_edge->plan_id]=false;

                        #ifdef LABEL
						e1->label=y->label+","+x->label;
						e2->label=x->label+","+y->label;
                        #endif
						e1->graph_node=y->graph_node;
						e2->graph_node=x->graph_node;
						e1->twin_link=e2;
						e2->twin_link=e1;
						bic_obj* e3=actual_connector;
						bic_obj* e4=previous_connector;

						if (y_is_elementary)
							{
							if (connectors_marked==2) y->black=e1;
							else y->white=e1;
							y_is_elementary=false;
							}
						else
							if (y->black==e3) y->black=e1;
							else y->white=e1;

						//cout << "y->black= " << y->black->label << "   y->white= " << y->white->label << endl;
						e1->black=y;
						if (e3->black==y) e3->black=e1;
							else e3->white=e1;
						e1->white=e3;
						e2->black=e4;

						if (x->actual_previous_connector) x->black=e2;
						else x->white=e2;

						e2->white=x;
						if (e4->black==x) e4->black=e2;
							else e4->white=e2;

						actual_connector=e1;
						}  // Modifica per aggiungere solo gli archi reali
						//cout << "Created bic_obj " << e1->label << endl;
						//cout << "Created bic_obj " << e2->label << endl;
						//embedding_stack.push(e1);
						}
					previous_node=x;
					//last_node=x;
					//walk_down_add_back->pause();
					//walk_down_triangulate1->pause();
					}	// end of else (x-twin_link==x)
				}  // end of while (!walk_down_stack.empty())
			connectors_marked--;
			if (connectors_marked==1) actual_connector=y->white;
			}	// end of while(connector_marked>0)
		} // end of while !secondary.empty()

	//cout << endl << "WALK DOWN END SUCCESSFULLY" << endl;
	return true;

	};



    	bool
	bic_obj_node::embedding_reporting
        (
		bm_node_info node_vector[],
		//bool ordered_first_black[],
		//list<node> children[],
		//edge parent[],
		undi_graph& ug,
		int flip[],
		bic_obj_node* IMM[]
		)
		{
		#ifdef OUTPUT
		cout << "Function Embedding Reporting activated" << endl;
		bool test_integrity=false;
		test_integrity=ug.internal_consistency_check();
		if (test_integrity) cout << "Internal check is OK"<<endl;
		else cout << "Internal check fail" << endl;
		cout << "Check flip values:" << endl;
		gdtedge e;
		forall_edges(e,ug)
			cout << "edge " << ug.id(e) << " flip=" << flip[e->plan_id] << endl;
		#endif
        //flip[0]=1;///////////////////////////attenzione!!
		gdtnode root=this->graph_node;
		bic_obj_node* start=this;
		node_vector[root->plan_id].ordered_first_black=true;
		bic_obj* edge_connector=start->black;
		bic_obj* previous_edge_connector=start;
		bool stop=false;
		
		struct_undi_node_info& ni=(*ug.node_info)[start->graph_node];
		
		#ifdef OUTPUT
        cout << "\nCURRENT NODE= " << start->label << "\tdegree=" << ug.degree(start->graph_node)<< endl;
       // start->check_node();
		#endif

		gdt::list_item  pos;
		ni.adj_edges.clear();
		ni.in_edges_are_ordered  = false;
  		ni.out_edges_are_ordered = false;
		while (!stop) // order the start node
			{
			if (edge_connector->is_node)
				{
				//cout << "--------------------------------------------" << endl;
				stop=true;
				}
			else
				{
				//edge_connector->ordered=true;
				/////////////////////////////////////
				//order the node
                //if (edge_connector->graph_edge!=NULL_EDGE) // serve per togliere gli archi fittizi
				//	{
				gdtedge e=edge_connector->graph_edge;
				pos = ni.adj_edges.append(e);
                #ifdef OUTPUT
				cout << "ADDED EDGE " << edge_connector->label << " to the embedding,edge-id= :" << ug.id (e) << endl;
				#endif
				if   (root == ug.source(e))
					(*ug.edge_info)[e].pos_into_source_adj_edges = pos;
      			else
					if (root == ug.target(e)) (*ug.edge_info)[e].pos_into_target_adj_edges = pos;
     				else cout << "bic_obj::read_node_embedding : no source or target node of edge" << endl;
				//}
				//////////////////////////////////////////////////////////////////////////////////////////////

				//cout << "\tedge " << edge_connector->label;
				//if (edge_connector->graph_edge==NULL_EDGE) cout << "    (ficticious)";
				//cout << endl;
                bic_obj* temp=edge_connector;
				if (edge_connector->black==previous_edge_connector)
					edge_connector=edge_connector->white;
				else
					edge_connector=edge_connector->black;
				previous_edge_connector=temp;


				} // end of else (edge_connector is not a node)
			}  // end of while
		#ifdef OUTPUT
		cout << "Check on root node..." << endl;
		if (!ug.internal_consistency_check())
			{
			cout << "Check integrity fails on start node" << endl;
			gdt_error("Inconsistency after the embedding of the start node");
			}
		#endif
		gdt::gdtlist<gdtnode> next_nodes;
        gdtnode n;
		forall(n,node_vector[root->plan_id].children)
			next_nodes.append(n);
		while (next_nodes.length()!=0)
			{
            // visita in ampiezza dello spanning tree
			gdtnode current=next_nodes.Pop();
			bic_obj_node* current_bj=IMM[current->plan_id];
			gdtedge parent_edge=node_vector[current->plan_id].parent;
			gdtnode p_node=ug.opposite(current,parent_edge);

			previous_edge_connector=current_bj;
			#ifdef OUTPUT
			cout << "\nFlip parent edge= " << flip[parent_edge->plan_id] << " on current node " << current_bj->label;
			cout << "\t" << IMM[(ug.source(parent_edge))->plan_id]->DFI << "," << IMM[(ug.target(parent_edge))->plan_id]->DFI;
			cout << "\tEdge id=" << ug.id(parent_edge) << endl;
			if(flip[parent_edge->plan_id]==0)
				cout << "CAUTION ------ FLIP NOT SET ON EDGE: " << ug.id(parent_edge)<< endl;
            #endif
			if ((node_vector[p_node->plan_id].ordered_first_black && flip[parent_edge->plan_id]==1)
				|| (!node_vector[p_node->plan_id].ordered_first_black && flip[parent_edge->plan_id]==-1))
				{
				edge_connector=current_bj->black;
                node_vector[current->plan_id].ordered_first_black=true;
				}
			else
                {
				edge_connector=current_bj->white;
				node_vector[current->plan_id].ordered_first_black=false;
				}

			stop=false;
			struct_undi_node_info& ni=(*ug.node_info)[current];
			#ifdef OUTPUT
        	cout << "\nCURRENT NODE=" << current_bj->label << "\tdegree=" << ug.degree(current)<< endl;
            //current_bj->check_node();
			#endif

			gdt::list_item  pos;
			ni.adj_edges.clear();
			ni.in_edges_are_ordered  = false;
  			ni.out_edges_are_ordered = false;
			while (!stop) // order the start node
				{
				if (edge_connector->is_node)
					{
					//cout << "--------------------------------------------" << endl;
					stop=true;
					}
				else
					{
					//edge_connector->ordered=true;
					/////////////////////////////////////
					//order the node
              	  	//if (edge_connector->graph_edge!=NULL_EDGE) // serve per togliere gli archi fittizi
						//{
					gdtedge e=edge_connector->graph_edge;
					pos = ni.adj_edges.append(e);
                    #ifdef OUTPUT
					cout << "ADDED EDGE " << edge_connector->label << " to the embedding,edge-id= :" << ug.id (e) <<endl;
					#endif
					if  (current == ug.source(e))
						(*ug.edge_info)[e].pos_into_source_adj_edges = pos;
      				else
						(*ug.edge_info)[e].pos_into_target_adj_edges = pos;
						//if (current == ug.target(e)) (*ug.edge_info)[e].pos_into_target_adj_edges = pos;
     				//else error_handler(1,"bic_obj::read_node_embedding : no source or target node of edge");
						//}
					//////////////////////////////////////////////////////////////////////////////////////////////
                    bic_obj* temp=edge_connector;
					#ifdef OUTPUT
					cout << "Edge connector=" << edge_connector->label << "\tblack=" << edge_connector->black->label << "\twhite=" << edge_connector->white->label << endl;
                    #endif
					if (edge_connector->black==previous_edge_connector)
						edge_connector=edge_connector->white;
					else
						edge_connector=edge_connector->black;
                    previous_edge_connector=temp;

					} // end of else (edge_connector is not a node)
				}  // end of while
			forall(n,node_vector[current->plan_id].children)
				next_nodes.append(n);
			}
		#ifdef OUTPUT
		test_integrity=ug.internal_consistency_check();
		if (test_integrity) cout << "Internal check is OK at end of embedding reporting"<<endl;
		else cout << "Internal check fail at end of embedidng reporting" << endl;
		#endif
		return true;
		}




	/* Extract_embedding.
	   Extract the planar embedding from the bicomp structure */
/*

		bool
	bic_obj_node::extract_embedding
		(
		stack<bic_obj*>& embedding_stack,
		undi_graph& ug
		)
		{
		bic_obj_node* start=this;
		bic_obj* edge_connector;
		bic_obj* previous_edge_connector;

		//cout <<"ciao" <<endl << "Function Extract Embedding activated on node " << start->label << endl;
		//cout << "Embedding of the external side:" << endl;
		start->ordered=true;
		start->first_black=true;
		edge_connector=start->black;
		previous_edge_connector=start;
		bool stop=false;
		//cout << "\nNode " << start->label << endl;
		struct_undi_node_info& ni=(*ug.node_info)[start->graph_node];
		#ifdef OUTPUT
        cout << "\nCURRENT NODE=" << ug.id(start->graph_node) << "\tdegree=" << ug.degree(start->graph_node)<< endl;
		#endif
        //embedded_nodes[ug.id(start->graph_node)]=true;
		list_item  pos;
		ni.adj_edges.clear();
		ni.in_edges_are_ordered  = false;
  		ni.out_edges_are_ordered = false;
		while (!stop) // order the start node
			{
			if (edge_connector->is_node)
				{
				//cout << "--------------------------------------------" << endl;
				stop=true;
				}
			else
				{
				edge_connector->ordered=true;
				/////////////////////////////////////
				//order the node
                if (edge_connector->graph_edge!=NULL_EDGE)
					{
					edge e=edge_connector->graph_edge;
					pos = ni.adj_edges.append(e);
                    #ifdef OUTPUT
					cout << "ADDED EDGE " << ug.id(ug.source(e)) << "," << ug.id(ug.target(e))<< endl;
					#endif
					if   (start->graph_node == ug.source(e))
						(*ug.edge_info)[e].pos_into_source_adj_edges = pos;
      				else
						if (start->graph_node == ug.target(e)) (*ug.edge_info)[e].pos_into_target_adj_edges = pos;
     					else error_handler(1,"bic_obj::read_node_embedding : no source or target node of edge");
					}
				//////////////////////////////////////////////////////////////////////////////////////////////

				//cout << "\tedge " << edge_connector->label;
				//if (edge_connector->graph_edge==NULL_EDGE) cout << "    (ficticious)";
				//cout << endl;
				if (edge_connector->black==previous_edge_connector)
					{
					edge_connector->first_black=false;
					previous_edge_connector=edge_connector;
					edge_connector=edge_connector->white;
					}
				else
					{
					edge_connector->first_black=true;
					previous_edge_connector=edge_connector;
					edge_connector=edge_connector->black;
					}
				} // end of else (edge_connector is not a node)
			}  // end of while
		// Now order the other nodes on the external side
		bool s;
		bic_obj_node* x=start->next_node(true);
		bic_obj_node* previous_node=start;
		while (x!=start)
			{
			x->ordered=true;
			//embedded_nodes[ug.id(x->graph_node)]=true;
            #ifdef OUTPUT
        	cout << "\nCURRENT NODE=" << ug.id(x->graph_node) <<"\tdegree=" << ug.degree(x->graph_node)<< endl;
			#endif

			struct_undi_node_info& ni=(*ug.node_info)[x->graph_node];
			ni.adj_edges.clear();
			ni.in_edges_are_ordered  = false;
  			ni.out_edges_are_ordered = false;
			bic_obj* previous_connector;
			if (previous_node->first_black)
				previous_connector=previous_node->black->twin_link;
			else previous_connector=previous_node->white->twin_link;
			//cout << "Previous_node= " << previous_node->label << "      previous_connector= " << previous_connector->label << endl;
			if (x->black==previous_connector)
				{
				x->first_black=false;
				edge_connector=x->white;
				}
			else
				{
				x->first_black=true;
				edge_connector=x->black;
				}
			previous_edge_connector=x;
			bool stop=false;
			//cout << "\nNode " << x->label << endl;
			while (!stop) // order a node on the external side
				{
				if (edge_connector->is_node)
					{
					//cout << "--------------------------------------------" << endl;
					stop=true;
					}
				else
					{
					edge_connector->ordered=true;
					//cout << "\tedge " << edge_connector->label;
					if (edge_connector->graph_edge!=NULL_EDGE)
						{
						edge e=edge_connector->graph_edge;
                        #ifdef OUTPUT
						cout << "ADDED EDGE " << ug.id(ug.source(e)) << "," << ug.id(ug.target(e))<< endl;
						#endif
						pos = ni.adj_edges.append(e);
						if   (x->graph_node == ug.source(e))
							(*ug.edge_info)[e].pos_into_source_adj_edges = pos;
      					else
							if (x->graph_node == ug.target(e)) (*ug.edge_info)[e].pos_into_target_adj_edges = pos;
     						else error_handler(1,"bic_obj::read_node_embedding : no source or target node of edge");
						}
					//cout << endl;
					if (edge_connector->black==previous_edge_connector)
						{
						edge_connector->first_black=false;
						previous_edge_connector=edge_connector;
						edge_connector=edge_connector->white;
						}
					else
						{
						edge_connector->first_black=true;
						previous_edge_connector=edge_connector;
						edge_connector=edge_connector->black;
						}
					} // end of else (edge_connector is not a node)
				}  // end of while (!stop) // order a node on the external side
			previous_node=x;
			//cout << "x->black=" << x->black->label <<  "       x->white=" << x->white->label << endl;
			if (x->first_black)
				x=x->next_node(true);
			else
				x=x->next_node(false);
			} // end while (x!=start)
		//cout << "\nExternal side complete. \nStart embedding the internal nodes\n" << endl;
		while (!embedding_stack.empty())
			{
			bic_obj* c1;
			bic_obj* c2;
			bic_obj* c3;
			bic_obj* c4;
			bic_obj* c5;
			bic_obj* c6;
			bic_obj* c7;
			bic_obj* c8;
			bic_obj* c9;
			bic_obj* c10;
			bic_obj* temp_bj=embedding_stack.pop();
			//cout << "Bic_obj " << temp_bj->label << "  extracted from embedding_stack" << endl;
			bic_obj* temp_bj2=temp_bj->twin_link;
			//if ((!temp_bj->ordered) && (!temp_bj2->ordered))
			//	{
				//cout << "This edge has not adjacent node ordered" << endl;
				//cout << "Error in embedding extraction" << endl;
				//return false;
			//	}
			if ((!temp_bj->ordered) || (!temp_bj2->ordered)) // one node is not ordered
				{
				if (temp_bj->ordered)
					c1=temp_bj;
				else
					c1=temp_bj2;  // c1 is ordered
				c2=c1->twin_link; // c2 is not ordered
				//cout << "c1= " << c1->label << "\t\tc2= " << c2->label << endl;
				if (c1->first_black)
					{
					c3=c1->white;
					c4=c1->black;
					}
				else // c1->first_black is false
					{
					c3=c1->black;
					c4=c1->white;
					}
				if (c3->is_node)
						{
						if (c3->black==c1)
							c3=c3->white;
						else c3=c3->black;
						}
				if (c4->is_node)
						{
						if (c4->black==c1)
							c4=c4->white;
						else c4=c4->black;
						}
				// c3 precede c1
				// c4 segue c1
				//cout << "c3= " << c3->label << "\t\tc4= " << c4->label << endl;
				c5=c3->twin_link;
				c6=c4->twin_link;
				//cout << "c5= " << c5->label << "\t\tc6= " << c6->label << endl;

				//if ((!c5->ordered) && (!c6->ordered))
				//	{
				//	cout << "No connectors ordered" << endl;
				//	cout << "Error in embedding extraction" << endl;
				//	return false;
				//	}
				if (c5->ordered)
					{
					//cout << "c5 is ordered" << endl;
					if (c5->first_black)
						{
						c7=c5->white;
						}
					else // c5->first_black is false
						{
						c7=c5->black;
						}
					if (c7->is_node)
						{
						if (c7->black==c5)
							c7=c7->white;
						else c7=c7->black;
						}
					c9=c7->twin_link;
					//cout << "c7= " << c7->label << "\t\tc9= " << c9->label << endl;

					// c2 is ordered

					c2->ordered=true;
					node current_node=c2->graph_node;
					//embedded_nodes[ug.id(current_node)]=true;
					#ifdef OUTPUT
        			cout << "\nCURRENT NODE=" << ug.id(current_node) <<"\tdegree=" << ug.degree(current_node)<< endl;
					#endif

					//cout << "c2 ordered" << endl;
					bic_obj* temp_black=c2->black;
					bic_obj* temp_white=c2->white;
     				if (temp_black->is_node)
						{
						if (temp_black->black==c2)
							temp_black=temp_black->white;
						else temp_black=temp_black->black;
						}
					if (temp_white->is_node)
						{
						if (temp_white->black==c2)
							temp_white=temp_white->white;
						else temp_white=temp_white->black;
						}
					//cout << "temp_black= " << temp_black->label << "\t\ttemp_white= " << temp_white->label << endl;
					//if ((temp_black!=c9) && (temp_white!=c9))
					//	{
					//	cout << "Error in structure" << endl;
					//	return false;
					//	}

					struct_undi_node_info& ni=(*ug.node_info)[current_node];
					ni.adj_edges.clear();
					ni.in_edges_are_ordered  = false;
  					ni.out_edges_are_ordered = false;
                    if (c2->graph_edge!=NULL_EDGE)
						{
						edge e=c2->graph_edge;

						#ifdef OUTPUT
						cout << "ADDED EDGE " << ug.id(ug.source(e)) << "," << ug.id(ug.target(e))<< endl;
						#endif
						pos = ni.adj_edges.append(e);
						if   (current_node == ug.source(e))
							(*ug.edge_info)[e].pos_into_source_adj_edges = pos;
      					else
							if (current_node== ug.target(e)) (*ug.edge_info)[e].pos_into_target_adj_edges = pos;
     						else error_handler(1,"bic_obj::read_node_embedding : no source or target node of edge");
                    	}
					c2->first_black=(temp_black==c9);
					stop=false;
					previous_edge_connector=c2;

					if (c2->first_black)
						edge_connector=c2->black;
					else
						edge_connector=c2->white;
					while (!stop) // order the elements in c2 list
						{
						if (edge_connector==c2)
							{
							//cout << "--------------------------------------------" << endl;
							stop=true;
							}
						else
							{
							edge_connector->ordered=true;
							//cout << "ordered element " << edge_connector->label;
							if (edge_connector->graph_edge!=NULL_EDGE)
								{
                                edge e=edge_connector->graph_edge;
								#ifdef OUTPUT
								cout << "ADDED EDGE " << ug.id(ug.source(e)) << "," << ug.id(ug.target(e))<< endl;
								#endif
								pos = ni.adj_edges.append(e);
								if   (current_node == ug.source(e))
									(*ug.edge_info)[e].pos_into_source_adj_edges = pos;
      							else
									if (current_node== ug.target(e)) (*ug.edge_info)[e].pos_into_target_adj_edges = pos;
     								else error_handler(1,"bic_obj::read_node_embedding : no source or target node of edge");

								}
							//cout << endl;
							if (edge_connector->black==previous_edge_connector)
								{
								edge_connector->first_black=false;
								previous_edge_connector=edge_connector;
								edge_connector=edge_connector->white;
								}
							else
								{
								edge_connector->first_black=true;
								previous_edge_connector=edge_connector;
								edge_connector=edge_connector->black;
								}
							} // end of else (edge_connector is not a node)
						}  // end of while (!stop)

					} // end if (c5->orderd)

				else // c6 is ordered
					{
					//cout << "c6 is ordered" << endl;
					if (c6->first_black)
						{
						c8=c6->black;
						}
					else // c6->first_black is false
						{
						c8=c6->white;
						}
					if (c8->is_node)
						{
						if (c8->black==c6)
							c8=c8->white;
						else c8=c8->black;
						}
					c10=c8->twin_link;
					//cout << "c8= " << c8->label << "\t\tc10= " << c10->label << endl;

					// c2 is ordered

					c2->ordered=true;
					//cout << "--------------------------------------------" << endl;
					//cout << "c2 ordered" << endl;
					node current_node=c2->graph_node;

					#ifdef OUTPUT
        			cout << "\nCURRENT NODE=" << ug.id(current_node) << "\tdegree=" << ug.degree(current_node)<<endl;
					#endif
                    //embedded_nodes[ug.id(current_node)]=true;
					struct_undi_node_info& ni=(*ug.node_info)[current_node];
					ni.adj_edges.clear();
					ni.in_edges_are_ordered  = false;
  					ni.out_edges_are_ordered = false;
					if (c2->graph_edge!=NULL_EDGE)
						{
						edge e=c2->graph_edge;
						#ifdef OUTPUT
						cout << "ADDED EDGE " << ug.id(ug.source(e)) << "," << ug.id(ug.target(e))<< endl;
						#endif
						pos = ni.adj_edges.append(e);
						if   (current_node == ug.source(e))
							(*ug.edge_info)[e].pos_into_source_adj_edges = pos;
      					else
							if (current_node== ug.target(e)) (*ug.edge_info)[e].pos_into_target_adj_edges = pos;
     						else error_handler(1,"bic_obj::read_node_embedding : no source or target node of edge");
                    	}

					bic_obj* temp_black=c2->black;
					bic_obj* temp_white=c2->white;
     				//cout << "temp_black= " << temp_black->label << "\t\ttemp_white= " << temp_white->label << endl;
					if (temp_black->is_node)
						{
						if (temp_black->black==c2)
							temp_black=temp_black->white;
						else temp_black=temp_black->black;
						}
					if (temp_white->is_node)
						{
						if (temp_white->black==c2)
							temp_white=temp_white->white;
						else temp_white=temp_white->black;
						}
					//cout << "temp_black= " << temp_black->label << "\t\ttemp_white= " << temp_white->label << endl;
					//if ((temp_black!=c10) && (temp_white!=c10))
					//	{
					//	cout << "Error in structure" << endl;
					//	return false;
					//	}
					c2->first_black=!(temp_black==c10);
					stop=false;
					previous_edge_connector=c2;
					if (c2->first_black)
						edge_connector=c2->black;
					else
						edge_connector=c2->white;
										while (!stop) // order the elements in c2 list
						{
						if (edge_connector==c2)
							{
							//cout << "--------------------------------------------" << endl;
							stop=true;
							}
						else
							{
							edge_connector->ordered=true;
                            if (edge_connector->graph_edge!=NULL_EDGE)
								{
                                edge e=edge_connector->graph_edge;
								#ifdef OUTPUT
								cout << "ADDED EDGE " << ug.id(source(e)) << "," << ug.id(target(e))<< endl;
								#endif
								pos = ni.adj_edges.append(e);
								if   (current_node == ug.source(e))
									(*ug.edge_info)[e].pos_into_source_adj_edges = pos;
      							else
									if (current_node== ug.target(e)) (*ug.edge_info)[e].pos_into_target_adj_edges = pos;
     								else error_handler(1,"bic_obj::read_node_embedding : no source or target node of edge");

								}
							//cout << "ordered element " << edge_connector->label;
							//if (edge_connector->graph_edge==NULL_EDGE && edge_connector->graph_node==NULL_NODE)
							//	{
							//	cout << "    (ficticious)" ;
							//	}
							//cout << endl;
							if (edge_connector->black==previous_edge_connector)
								{
								edge_connector->first_black=false;
								previous_edge_connector=edge_connector;
								edge_connector=edge_connector->white;
								}
							else
								{
								edge_connector->first_black=true;
								previous_edge_connector=edge_connector;
								edge_connector=edge_connector->black;
								}
							} // end of else (edge_connector is not a node)
						}  // end of while (!stop)

					} // end of else c6 is ordered
				} // end if ((!temp_bj->ordered) || (!temp_bj2->ordered))
			}
		return true;
		}

               */

/*    Creation of the planar embedding by adding the back edges    */


		bool undi_graph::
	make_planar_embedding
		(
		bm_node_info node_vector[],
		bic_obj_node* IMM[],
		gdtnode root,
		bool edge_to_be_inserted[],
		int flip[],
		bic_obj*& bic_obj_actual_pointer
		)
		{
        #ifdef COUNTER
		post_order_counter->start();
        #endif
		 bool success=true;
		 gdt::gdtstack<gdtnode> S;	   		// Stack for the post-order visit
		 gdt::gdtstack<bic_obj*> embedding_stack;


		 gdtnode n;
		 //gdtedge e;
		 bic_obj_node* root_bic_obj = (bic_obj_node*)NULL; // secondary image of the root node
		// node_map<bool> expanded(nodes_and_edges(),false);
		 bool* expanded=new bool[number_of_nodes()];
		 for (int k=0; k<number_of_nodes(); k++) expanded[k]=false;
		 S.push(root);
         #ifdef OUTPUT
		 cout << endl << "Start POST-ORDER visit with root=" << IMM[root->plan_id]->DFI << "\tID=" << id(root) <<endl << endl;
		 #endif
		 //for (int actual_node=number_of_nodes(); actual_node>0; actual_node--)
		 while (!S.empty())
			{
            n=S.top();
			//actual_node=IMM[n]->DFI;
			#ifdef OUTPUT
			//cout << "actual node=" << actual_node << endl;
			#endif

			//n=DFI_vector[actual_node];

			if (((node_vector[n->plan_id].children).length()==0) || expanded[n->plan_id])
				{
				/* Action */
				#ifdef OUTPUT
				cout << "VISITED NODE: "<< IMM[n->plan_id]->DFI << endl;
				#endif
				if ((node_vector[n->plan_id].in_back_edges.length())>0)
					{
					#ifdef OUTPUT
					cout << "node " << id(n) << "\taggiunta back edge" << endl;
					#endif
					gdtnode source;
					gdt::gdtlist<bic_obj_node*> secondary;
					bic_obj_node* bj_source = (bic_obj_node*)NULL;
					forall(source,node_vector[n->plan_id].in_back_edges)
						{
						//cout << "Insertion of back_edge " << id(be) << "      ";
						//node source=opposite(n,be);
						bj_source=IMM[source->plan_id];
						//cout << "source node=" << bj_source->label << "  " << bj_source << endl;
                        #ifdef COUNTER
						walk_up_counter->start();
						#endif
                        #ifdef OUTPUT
						cout << "Walk Up Start" << endl;
                        #endif
						success=bj_source->walk_up(IMM[n->plan_id]->DFI,node_vector,secondary,*this);
                        #ifdef OUTPUT
						cout << "Walk Up End" << endl;
                        #endif
                        #ifdef COUNTER
						walk_up_counter->pause();
						#endif
						if (!success)
							{
							//cout << "Walk Up Fail on edge " << id(be) << endl;
							//cout << endl << "Graph is not planar."  << endl;
							delete[] expanded;
							return false;
							}
						}

					gdt::list_item temp_li=secondary.first();

					root_bic_obj=secondary.inf(temp_li);
                    #ifdef COUNTER
					walk_down_counter->start();
					#endif
                    #ifdef OUTPUT
					cout << "Walk Down start" << endl;
                    #endif
					success=bj_source->walk_down(node_vector,secondary,this,edge_to_be_inserted,flip,bic_obj_actual_pointer);
                    #ifdef OUTPUT
					cout << "Walk Down end" << endl;
                    #endif
					#ifdef COUNTER
					walk_down_counter->pause();
					#endif
					#ifdef OUTPUT
					if (!success)
						{
						cout << endl << "Error in Walk Down on node " << IMM[n->plan_id]->DFI << endl;
						delete[] expanded;
						return false;
						}
					#endif
			        //bic_obj* temp_check_bj;
					//forall(temp_check_bj,secondary2)
					//	temp_check_bj->check_bicomp();

					}

				S.pop();
				}
			else
				{
				/* Put children in S */
				expanded[n->plan_id]=true;
				gdtnode n1;
				forall(n1,node_vector[n->plan_id].children)
				//S.push(opposite(n,e));
				S.push(n1);
				#ifdef OUTPUT
				cout << "Ok aggiunta nodi nello stack" << endl;
				#endif
			    }

		 }   // End of while(!S.empty())
		#ifdef OUTPUT
		cout << "End of the POST-ORDER visit" << endl;
		#endif
		#ifdef COUNTER
		post_order_counter->pause();
		#endif
        #ifdef OUTPUT
		cout << "All back edges inserted. Graph is planar. " << endl;
		//cout << "\n\n TEST STRUCTURE \n\n\n" << endl;
		#endif
		//gdtnode a;
		//bool structure_ok=true;

		IMM[root->plan_id]=root_bic_obj;

		bool success_embedding_extraction=true;
        #ifdef COUNTER
		extract_embedding_counter->start();
		#endif
		#ifdef OUTPUT
		cout << "Extract embedding start" << endl;
		#endif
		//success_embedding_extraction=root_bic_obj->extract_embedding(embedding_stack,*this);
        success_embedding_extraction=root_bic_obj->embedding_reporting(node_vector,*this,flip,IMM);
		#ifdef OUTPUT
		cout << "Extract embedding end" << endl;
		#endif
        #ifdef COUNTER
		extract_embedding_counter->pause();
        #endif
		if (!success_embedding_extraction)
			{
			#ifdef OUTPUT
			cout << "Error in embedding extraction" << endl;
			#endif
			delete[] expanded;
			return false;
			}

		/*list<node> lista_nodi=all_nodes();


		forall(a,lista_nodi)
			{
			cout << "--------------------------------------------------------------------" << endl;

			if (a==root)
				structure_ok=root_bic_obj->read_node_embedding(*this);
			else
			//read_node_embedding_counter->start();
			structure_ok=IMM(a)->read_node_embedding(*this);
			//read_node_embedding_counter->pause();
			if (!structure_ok)
				{
				cout << "Error in final structure " << endl;
				return false;
				}
			}*/
		/*cout << "\nTest embedding:" << endl;
		forall_nodes(n,*this)
			{
			cout << "Node " << id(n) << endl;
			cout << "Adjacent edges:" << endl;
			list<node> adj=this->adj_nodes(n);
			node n1;
			forall(n1,adj)
				cout << "edge to " << id(n1) << endl;
			cout << "--------------------------------------" << endl;
			}
		*/
		//cout << "\n(make planar embedding) bic_obj counter=" << bic_obj::counter << endl;
		//destroy_bicomp_counter->start();
		//destroy_bicomp(bic_obj_created);
		//destroy_bicomp_counter->pause();
		//bic_obj_created.clear();
		//lista_nodi.clear();
		//cout << "\n\n**********************************************************************************" << endl;
		//cout << "************************** EMBEDDING EXTRACTION SUCCESS **************************" << endl;
		//cout << "**********************************************************************************" << endl;
		//this->write("embedded_graph");
		#ifdef OUTPUT
		cout << "End of make_planar_embedding function." << endl;
		#endif
		delete[]expanded;
		return true;
		};


/* merge planar biconnected component */


		bool undi_graph::
	merge_biconnected
		(
		gdt::gdtlist<undi_graph*>& bic
		)
	{
	//cout << "\nFunction merge_biconnected activated " << endl;

	gdtnode n;
	gdtedge e;
	gdtnode n_i;
	gdtedge e_i;
	undi_graph* ug_i;
	gdt::list_item  pos;
	int number_of_components=0;

	forall_nodes(n,*this)
		{
		struct_undi_node_info& n_info=(*(this->node_info))[n];
		n_info.adj_edges.clear();
		n_info.in_edges_are_ordered  = false;
  		n_info.out_edges_are_ordered = false;
		}

	forall(ug_i,bic)
		{
		number_of_components++;
		forall_nodes(n_i,*ug_i)
			{
			n=corresponding_node(n_i,*ug_i);
			//cout << "visited node " << ug_i->id(n_i) << " in component " << number_of_components ;
			//cout <<"\tcorresponding node= " << id(n) << endl;
			struct_undi_node_info& n_info=(*(this->node_info))[n];
			struct_undi_node_info& n_info_i=(*(ug_i->node_info))[n_i];
			gdt::gdtlist<gdtedge> adj=ug_i->adj_edges(n_i);
			// ug_pos=adj.first();
			//while (ug_pos)
			forall(e_i,n_info_i.adj_edges)
				{
				//e_i=adj.inf(ug_pos);
				e=corresponding_edge(e_i,*ug_i);
				pos=n_info.adj_edges.append(e);
				//cout << "\t visited edge to" << id(opposite(n,e)) << endl;
				if   (n == source(e))
						(*(this->edge_info))[e].pos_into_source_adj_edges = pos;
      			else
					if (n == target(e)) (*(this->edge_info))[e].pos_into_target_adj_edges = pos;
     				else cout << "bic_obj::read_node_embedding : no source or target node of edge" << endl;
				//ug_pos=adj.succ(ug_pos);
				}
			}
		}
	/*cout << "\nTest embedding:" << endl;
	forall_nodes(n,*this)
		{
		cout << "Node " << id(n) << endl;
		cout << "Adjacent edges:" << endl;
		list<node> adj=this->adj_nodes(n);
		node n1;
		forall(n1,adj)
			cout << "edge to " << id(n1) << endl;
		cout << "--------------------------------------" << endl;
		}
	*/
	return true;
	};




	/* Apply the planarity testing algorithm.  */




	bool undi_graph::
boyer
	(
	gdtnode root
	)
	{
	
	if (number_of_edges()==0) return true;
	gdtedge e_self;
	forall_edges(e_self,*this)
		{
		if (source(e_self) == target(e_self) )
			gdt_error("The graph contains self-loops");
		} 	
	if (number_of_nodes()<4) return true;	
	if (this->number_of_edges() > (3*this->number_of_nodes() - 4))
		return false;
	int z=0;
	gdtnode m;
	forall_nodes(m,*this)
		m->plan_id=z++;
	z=0;
	gdtedge ed;
	forall_edges(ed,*this)
		ed->plan_id=z++;
	this->max_edge_plan_id=number_of_edges()-1;
	gdt::gdtlist<gdtedge> added_edges;

	bool test=false;
	bool success=false;
	#ifdef DEBUG
    	cout << "Function boyer()" << endl;
	cout << "Root node= " << id(root) << endl;
	cout << "Nodes=" << number_of_nodes() << "\tedges=" << number_of_edges() << endl << endl;
	#endif
	if (number_of_edges()<=3)  // graph with  1, 2, or 3 nodes
		{
		#ifdef OUTPUT		
		cout << "Graph with 2 nodes. Nothing to do" << endl;
		#endif
		success=true;
		}
	else
		{

		#ifdef OUTPUT
		cout << "\nStart Boyer............" << endl;
		#endif

		#ifdef COUNTER
		memory_counter->start();
		#endif
		//bm_node_info_memory=new bm_node_info[number_of_nodes()];//calloc(number_of_nodes(),sizeof(bm_node_info));
		bm_node_info* node_vector=new bm_node_info[number_of_nodes()];

		bic_obj* bic_obj_memory=new bic_obj[6*number_of_nodes()];// EDGES
		bic_obj* bic_obj_actual_pointer=bic_obj_memory;
		bic_obj_node_memory=new bic_obj_node[2*number_of_nodes()]; // NODES
		bic_obj_node* bic_obj_node_actual_pointer=bic_obj_node_memory;


        bic_obj_node** IMM=new bic_obj_node*[number_of_nodes()];

		//gdtnode graph_nodes [number_of_nodes()];
		gdtnode* graph_nodes=new gdtnode[number_of_nodes()];

		//gdtedge iterator[number_of_nodes()];
		gdtedge* iterator=new gdtedge[number_of_nodes()];


		//bool reached[number_of_nodes()];
		bool* reached=new bool[number_of_nodes()];

		//bool e_reached[number_of_nodes()*3];
		bool* e_reached=new bool[number_of_nodes()*3];

		//int flip[number_of_nodes()*3];
		int* flip=new int[number_of_nodes()*3];

		for (int z1=0; z1<(3*number_of_nodes()); z1++) e_reached[z1]=false;
		for (int z2=0; z2<(number_of_nodes()); z2++) reached[z2]=false;


		//gdtnode w;
		gdtedge e;

		#ifdef COUNTER
		memory_counter->pause();
		#endif

		int number=max_node_id;


		#ifdef OUTPUT
		cout << "Memory allocated" << endl;
		#endif

		for (int j=0; j<=number; j++)
            {
            gdtnode bic_root=find_node(j);
			if (bic_root!=NULL_NODE)
                {
                if (!reached[find_node(j)->plan_id])  // this node belongs to a new connected component
                    {
                    if (degree(bic_root)!=0)
						{
						//cout << "\nPreprocessing start on node " << id(bic_root)<< endl;
						int nodes_in_connected_component;
                        #ifdef COUNTER
						preprocessing_counter->start();
						#endif
						test=bm_preprocessing(node_vector,added_edges,bic_root,IMM,reached,e_reached,iterator,nodes_in_connected_component,graph_nodes,bic_obj_actual_pointer,bic_obj_node_actual_pointer);
                        //cout << "Ciao" << endl;
						#ifdef COUNTER
						preprocessing_counter->pause();
						#endif
                        #ifdef OUTPUT
						cout << "Preprocessing end" << endl;
						cout << "Nodi nella componente connessa " << nodes_in_connected_component << endl;

						#endif

						if (test && nodes_in_connected_component>3)
							{
							#ifdef OUTPUT
							cout << "make_planar_embedding will be activated" << endl;
							#endif
							success=make_planar_embedding(node_vector,IMM,bic_root,e_reached,flip,bic_obj_actual_pointer);
							if (!success)
								{
								delete [] node_vector;
								delete [] bic_obj_memory;
								delete [] bic_obj_node_memory;
								delete [] IMM;
								delete [] graph_nodes;
								delete [] iterator;
								delete [] reached;
								delete [] e_reached;
								delete [] flip;

								#ifdef COUNTER
								garbage_counter->start();
								#endif
								forall(e,added_edges)
									{
									//archi++;
									del_edge(e);
									//cout << "\narchi cancellati : "<< archi << endl;
									}
								#ifdef COUNTER
								garbage_counter->pause();
								#endif
								return false;
								}
							}
						else
                            {
							if (!test)
								{	//cout << "Error in connected component " << j << endl;
								#ifdef COUNTER
								garbage_counter->start();
								#endif
								forall(e,added_edges)
									{
									//archi++;
									del_edge(e);
									//cout << "\narchi cancellati : "<< archi << endl;
									}
								#ifdef COUNTER
								garbage_counter->pause();
								#endif
								delete [] node_vector;
								delete [] bic_obj_memory;
								delete [] bic_obj_node_memory;
								delete [] IMM;
								delete [] graph_nodes;
								delete [] iterator;
								delete [] reached;
								delete [] e_reached;
								delete [] flip;

								return false;
								}
                            success=true;
							}
						}
					} // bic_root is not NULL_NODE
    			}
			 }
		#ifdef OUTPUT
		cout << endl <<"Now free memory..." << endl;
		#endif
		delete [] node_vector;
		delete [] bic_obj_memory;
		delete [] bic_obj_node_memory;
		delete [] IMM;
		delete [] graph_nodes;
		delete [] iterator;
		delete [] reached;
		delete [] e_reached;
		delete [] flip;

		#ifdef OUTPUT
		cout <<"End of free memory" << endl;
		#endif
		#ifdef COUNTER
		garbage_counter->start();
		#endif
		#ifdef OUTPUT
		cout << endl <<"Now deleting added edges..." << endl;
		#endif
		
		forall(e,added_edges)
			{
			#ifdef OUTPUT
   			cout << "Deleting added edge " << id(e) << "\t" << e << endl;
			#endif
			del_edge(e);
			#ifdef OUTPUT
			if (internal_consistency_check()) cout << "Check OK" << endl;
			#endif
			//cout << "\narchi cancellati : "<< archi << endl;
			}

		#ifdef COUNTER
		garbage_counter->pause();
		#endif
		#ifdef OUTPUT
		cout << endl <<"Added edges deleted" << endl;
		#endif
		} // end of else (graph has more than 3 nodes)

		if (success)
			{
			#ifdef DEBUG
			cout << "Function boyer returns TRUE" << endl;
			#endif
			return true;

			}
		else
			{
			#ifdef DEBUG
			cout << "Function boyer returns FALSE" << endl;
			#endif
			return false;
			}


	}




			bool
	undi_graph::is_planar()
			{
			//cout << "funzione is planar" << endl;
			//this->write("test.gdt");
			//cout << "grafo test.gdt creato" << endl;
			return boyer(first_node());
			}



			bool
undi_graph::quick_minimum_spanning_tree
			(
			gdt::gdtedge_map<int> weights,
			gdt::gdtlist<gdtedge>& span_edges)
			{
				plan_undi_graph pug(*this);
				if (this->number_of_nodes() !=pug.number_of_nodes())
				{
					gdt_error("The graph is not planar!\n");
				}
				cout << "OK, the graph is planar" << endl;
				//cout << "Number of faces in dual graph: " << pug.number_of_faces() << endl; 			
				
				gdt::gdtlist<int> list_prim; // final result;

				simple_graph prim(pug);
				simple_graph dual;
				
				gdt::gdtmap<face,node> faces;
				gdt::gdtmap<edge,edge> prim_dual;
				gdt::gdtmap<edge,edge> dual_prim;
				gdt::gdtmap<node,bool> inserted(false);
				
				// Dual graph costruction;
				face f;
				forall_faces(f,pug)
					{
					node n=dual.new_node();
					faces[f]=n;	
					}		
				edge e_prim;
				forall(e_prim,prim._edges)
					{
					gdtedge e_pug=pug.find_edge(e_prim->index);
					gdtnode sn=pug.source(e_pug);
					face f1=pug.right_face_moving_along_edge(e_pug,sn);
					face f2=pug.left_face_moving_along_edge(e_pug,sn);
					node n1=faces[f1];
					node n2=faces[f2];
					edge e_dual=dual.new_edge(n1,n2,e_prim->index);
					prim_dual[e_prim]=e_dual;
					dual_prim[e_dual]=e_prim;
					e_prim->set_weight(weights[find_edge(e_prim->index)]);
					e_dual->set_weight(e_prim->get_weight());
					}
				cout <<"Dual graph:\tnodes: " << dual.number_of_nodes();
				cout <<"\tedges: " << dual.number_of_edges() << endl << endl;
				gdt::gdtlist<node> candidate_nodes;
				gdt::gdtmap<node, simple_graph* > corresponding_graph; 
				
				node n;
				forall(n,prim._nodes)
				{
					corresponding_graph[n]=&prim;
					if (n->incident_edges.length()<=4)
					{
						candidate_nodes.append(n);
						inserted[n]=true;
					}
					else
						inserted[n]=false;
				}
				forall(n,dual._nodes)
				{
					corresponding_graph[n]=&dual;
					if (n->incident_edges.length()<4)
					{
						candidate_nodes.append(n);
						inserted[n]=true;
					}
					else
						inserted[n]=false;
				}

				//////////////////////////////////////////////////////////////
				//The algorithm starts here
	
				while ( (!prim.is_empty()) && (!dual.is_empty()) )
				{
					//bool test1=prim.check();
					//bool test2=dual.check();
					/*if (test1)
						cout << "Primal Graph OK" << endl;
					else
						cout << "Error on Primal Graph" << endl;
					if (test2)
						cout << "Dual Graph OK" << endl;
					else
						cout << "Error on Dual Graph" << endl;
					*/
					
					node v;
					
					if (!candidate_nodes.empty())
						{
						
						//debug
						/*
						cout << "* lista di candidate nodes * " << endl;
						node a;
						forall(a,candidate_nodes)
						{
							cout << a << "\t";
							if (corresponding_graph[a]==&prim) cout << "Primale" << endl;
							if (corresponding_graph[a]==&dual) cout << "Duale" << endl;
							if (inserted[a]!=true)
								gdt_error("errore nella marcatura in candidate_nodes");
						}
						*/
						// fine debug
						v=candidate_nodes.pop_front();
						inserted[v]=false;
						}
					else
						gdt_error("Error in quick spanning tree");
					
					//cout << "Current node=" << v << "\t";
					//if (corresponding_graph[v]==&prim)
					//	cout << "Primal Graph";
					//else
					//	cout << "Dual Graph";
					//cout << endl;
					
					//cout << prim << endl;
					//cout << dual << endl;
					
					if (v->incident_edges.length()==0) // isolated vertex
						{
						//cout << "Vertice isolato" << endl;
						
						simple_graph* temp_graph=corresponding_graph[v];
						//temp_graph->check();
						//if (temp_graph==&prim) cout << "Sono nel primale" << endl;
						//if (temp_graph==&dual) cout << "Sono nel duale" << endl;
						//cout << "Ora verrà cancellato il nodo " << v << endl;
						temp_graph->del_node(v);
						}
					else
					{
						edge self_loop=NULL;
						edge e;
						forall(e,v->incident_edges)
							if (e->_start == e->_end) self_loop=e;
						if (self_loop)
						{
							//cout << "Self loop" << endl;
							edge e_p,e_d;
							if ( (corresponding_graph[v]) == &prim)
							{
								e_p=self_loop; 
								e_d=prim_dual[e_p];
								if (!inserted[v]) 
								{
									candidate_nodes.append(v);
									inserted[v]=true;
									//cout << "** 1-Inserito nodo " << v << endl;
								}
								node n1,n2;
								n1=e_d->start();
								n2=e_d->end();
								if ((n1->degree() <=4) && !inserted[n1]) 
								{
									candidate_nodes.append(n1);
									inserted[n1]=true;
									//cout << "** 2-Inserito nodo " << n1 << endl;
								}
								if (n1!=n2)	
									if ((n2->degree() <=4) && !inserted[n2]) 
									{
										candidate_nodes.append(n2);
										inserted[n2]=true;
										//cout << "** 3-Inserito nodo " << n2 << endl;
									}
							}
							else
							{
								e_d=self_loop; 
								e_p=dual_prim[e_d];
								if (!inserted[v])
								{
									candidate_nodes.append(v);
									inserted[v]=true;
									//cout << "** 4-Inserito nodo " << v << endl;
								}
								node n1,n2;
								n1=e_p->start();
								n2=e_p->end();
								if ((n1->degree() <=4) && !inserted[n1]) 
								{
									candidate_nodes.append(n1);
									inserted[n1]=true;
									//cout << "** 5-Inserito nodo " << n1 << endl;
								}
								if (n1!=n2)	
									if ((n2->degree() <=4) && !inserted[n2]) 
									{
										candidate_nodes.append(n2);
										inserted[n2]=true;
										//cout << "** 6-Inserito nodo " << n2 << endl;
									}
								list_prim.append(e_p->index);
							}
							
							prim.del_edge(e_p);
							dual.del_edge(e_d);			
						} // end of self_loop
						else // there are no self-loops around v
						{
							//cout << "No self loops" << endl;
							//if (!prim.check()) gdt_error("errore nel primale");
							//if (!dual.check()) gdt_error("errore nel duale");
							if ((corresponding_graph[v]) == &prim)
							{
								edge e_p,e_d;
								e_p=v->incident_edges.front();
								forall(e,v->incident_edges)
									if (e->get_weight() < e_p->get_weight()) e_p=e;
								e_d=prim_dual[e_p];
								list_prim.append(e_p->index);
								node opposite_v;
								if (e_p->_start== v)
									opposite_v=e_p->_end;
								else
									opposite_v=e_p->_start;
								prim.contract(e_p,opposite_v);
								node n1,n2;
								n1=e_d->start();
								n2=e_d->end();
								dual.del_edge(e_d);
								
								if ((n1->degree()<4 && !inserted[n1]))
								{
									candidate_nodes.append(n1);
									inserted[n1]=true;
									//cout << "** 7-Inserito nodo " << n1 << endl;
								}
								if (n1!=n2)
									if ((n2->degree()<4) && !inserted[n2])
									{
										candidate_nodes.append(n2);
										inserted[n2]=true;
										//cout << "** 8-Inserito nodo " << n2 << endl;
									}
								if ((opposite_v->degree()<4) && !inserted[opposite_v])
								{
									candidate_nodes.append(opposite_v);
									inserted[opposite_v]=true;
									//cout << "** 9-Inserito nodo " << opposite_v << endl;
								}
								
								//prim.check();
								//dual.check();
							}
							else
							{
								edge e_p,e_d;
								e_d=v->incident_edges.front();
								forall(e,v->incident_edges)
									if (e->get_weight()> e_d->get_weight()) e_d=e;
								e_p=dual_prim[e_d];
								node opposite_v;
								if (e_d->_start== v)
									opposite_v=e_d->_end;
								else
									opposite_v=e_d->_start;
								//prim.check();
								//dual.check();
								dual.contract(e_d,opposite_v);
								node n1,n2;
								n1=e_p->start();
								n2=e_p->end();
								prim.del_edge(e_p);
								
								if ((n1->degree()<4) && !inserted[n1])
								{
									candidate_nodes.append(n1);
									inserted[n1]=true;
									//cout << "** 10-Inserito nodo " << n1 << endl;
								}
								if (n1!=n2)
									if ((n2->degree()<4) && !inserted[n2])
									{
										candidate_nodes.append(n2);
										inserted[n2]=true;
										//cout << "** 11-Inserito nodo " << n2 << endl;
									}
								if ((opposite_v->degree()<4) && !inserted[opposite_v])
								{ 
									candidate_nodes.append(opposite_v);
									inserted[opposite_v]=true;
									//cout << "** 12-Inserito nodo " << opposite_v << endl;
								}
							}
						}
					}
					
							
				}//end while
							
				int ind;
				forall(ind,list_prim)
				{
					gdtedge e_ris=this->find_edge(ind);
					span_edges.append(e_ris);
				}		
				return true;
			}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			int
		undi_graph::compare_embedding(undi_graph& ug)
			 {
			int swap=0;
			if ((number_of_nodes()!=ug.number_of_nodes()) || (number_of_edges()!=ug.number_of_edges()) )
				gdt_error("Error in compare_embedding: the graphs differ (different number of nodes or edges.");
			gdtnode n=NULL_NODE;
			gdtedge e=NULL_EDGE; 				
			forall_edges(e,*this)
				{
				int id_e=id(e);
				gdtedge e_ug=ug.find_edge(id_e);
				if (!e_ug) gdt_error("Error in compare_embedding: the graphs differ (edge not found.");
				gdtnode n1=source(e);
				gdtnode n2=target(e);
				gdtnode n1_ug=ug.source(e_ug);
				gdtnode n2_ug=ug.target(e_ug);
				if (( (id(n1)!=ug.id(n1_ug) || (id(n2)!=ug.id(n2_ug)) )) && 
					( (id(n1)!=ug.id(n2_ug) || (id(n2)!=ug.id(n1_ug)) )) )
					{
					//cerr << "e=" << id(e) << "\te_ug=" << ug.id(e_ug) << endl;
					//cerr <<"n1=" << id(n1) << "\tn1_ug=" << ug.id(n1_ug) << endl;
					//cerr <<"n2=" << id(n2) << "\tn2_ug=" << ug.id(n2_ug) << endl;
					gdt_error("Error in compare_embedding: the graphs differ (node not found.");
					}	
				}
			
			forall_nodes(n,*this)
				{
				gdtnode n_ug=ug.find_node(id(n));	
				//cout <<"esamino nodo " << id(n) << endl;
				
				gdt::gdtlist<gdtedge> edges1=adj_edges(n);
				gdtedge first_edge=edges1.front();
				gdtedge first_edge_ug=ug.find_edge(id(first_edge));
				gdt::gdtlist<gdtedge> edges2;
				edges2.append(first_edge_ug);				
				gdtedge et=ug.cyclic_adj_succ(first_edge_ug,ug.find_node(id(n)));
				while(et!=first_edge_ug)
					{
					edges2.append(et);					
					et=ug.cyclic_adj_succ(et,ug.find_node(id(n)));
					}
				
				int l=edges1.length();
				int i;
				int node_swaps=INT_MAX;
				for(i=0; i<l; i++)
					{
					#ifdef DEBUG					
					cout << "edges1= ";
					forall(e,edges1) 
						cout << id(e) << " ";
					cout << endl;					

					cout << "edges2= ";
					forall(e,edges2) 
						cout << ug.id(e) << " ";
					cout << endl;					
					#endif

					gdt::gdtedge_map<int> position1(nodes_and_edges());
					gdt::gdtedge_map<int> position2(ug.nodes_and_edges());
					i=0;
					forall(e,edges1)
						position1[e]=i++;
					forall(e,edges2)
						position2[e]=position1[find_edge(ug.id(e))];
					
					#ifdef DEBUG
					cout << "position2:" << endl;
					forall(e,edges2)
						cout << position2[e]<< " ";
					cout << endl;
					#endif

					//Compute the number of inversions for each permutation			
					int permutation_swaps=0;					
					int* inversion_vector;
					inversion_vector=new int[l];
					for (i=0; i<l; i++)
						{
						inversion_vector[i]=0;
						}
					for (i=0; i<l; i++)
						{
						int z=0;
						list_item li=edges2.first();
						while(z<i)
							{
							gdtedge e_temp=edges2.inf(li);
							if (position2[e]>i) inversion_vector[i]++;
							li=edges2.succ(li);
							z++;
							}
						}
					for (i=0; i<l; i++)
						{
						permutation_swaps+=inversion_vector[i];
						}
				
					if (permutation_swaps<node_swaps) node_swaps=permutation_swaps;
					//cout << "swap in questa permutazione=" << permutation_swaps << endl;
					/////////////////////////////////////////////				
					//new permutation					
					gdtedge temp_edge1=edges1.pop_front();
					gdtedge temp_edge2=edges2.pop_front();
					edges1.append(temp_edge1);
					edges2.append(temp_edge2);
					}
	
				swap+=node_swaps;	
				}
			return swap;
			}





			void
		undi_graph::visible_subgraph
			(
			gdtnode n,
			int k,
			undi_graph& vg			
			)
		{
		gdtnode w=NULL_NODE;		
		gdt::gdtnode_map<int> distance(nodes_and_edges(),INT_MAX);
		gdt::gdtnode_map<bool> visited(nodes_and_edges(),false);
		distance[n]=0;
		
		gdt::gdtlist<gdtnode> nodes;
		nodes.append(n);
		while(!nodes.empty())
			{
			gdtnode v=nodes.pop_front();	
			//cout << "visito il nodo " << id(v) << "distanza=" << distance[v] <<endl;			
			if (distance[v]>k) break;			
			if (!visited[v])
				{
				visited[v]=true;			
				vg.new_node(id(v));	
					
				gdt::gdtlist<gdtnode> temp_nodes=adj_nodes(v);			
				forall(w,temp_nodes)
					{
					if (!visited[w])
						{
						if (distance[w]>distance[v]+1) distance[w]=distance[v]+1;
						nodes.append(w);
						//cout << "appendo alla lista il nodo " << id(w) << endl;
						}
					}
				}			
			}

		cout << "nodi di vg=" << vg.number_of_nodes() << endl;
		gdt::gdtedge_map<bool> e_visited(*this, false);
		forall_nodes(w,vg)
			{
			gdt::gdtlist<gdtedge> edges=adj_edges(find_node(vg.id(w)));
			gdtedge e;
			forall(e,edges)
				{
				//cout << "considero l'arco" << id(e) << endl;
				gdtnode z=opposite(find_node(vg.id(w)),e);
				if (vg.find_node(id(z)) != NULL_NODE && !e_visited[e])
					{					
					vg.new_edge(w,vg.find_node(id(z)),id(e));
					//cout << "aggiunto arco " << vg.id(e1) << " fra " << vg.id(w)<< " e " << id(z) << endl;
					e_visited[e]=true;
					}
				}
			}
		return;
		}

		

			int
		undi_graph::kplanarity
			(gdtnode n)
		{
		int current_kplanarity=0;
		bool update=false;
		int previous_nodes=1;
		do 
			{
			current_kplanarity++;			
			undi_graph vug;
			visible_subgraph(n,current_kplanarity,vug);
			if (vug.is_planar() && vug.number_of_nodes()>previous_nodes)
				{ 
				update=true;
				previous_nodes=vug.number_of_nodes();				
				}
			else update=false;
			} while (update);	
		--current_kplanarity;
		cout << "la funzione k planarity sul nodo " << id(n) << " ritorna " << current_kplanarity << endl;	
		return current_kplanarity;
		}


			int
		undi_graph::extended_kplanarity
			(gdtnode n)
		{
		int current_kplanarity=0;
		bool update=false;
		int previous_nodes=1;
		do 
			{
			current_kplanarity++;			
			undi_graph vug;
			visible_subgraph(n,current_kplanarity,vug);
			
			gdtnode infinite=vug.new_node();
			gdtnode v;
			forall_nodes(v,vug)
				 {
				 if (((vug.adj_nodes(v)).length() != adj_nodes(find_node(vug.id(v))).length()) &&
					v!=infinite )
					{
					cout << "nodo esterno " << vug.id(v) << endl;
					vug.new_edge(v,infinite);
					}
				 }
			if (vug.is_planar() && vug.number_of_nodes()>previous_nodes)
				{ 
				update=true;
				previous_nodes=vug.number_of_nodes();				
				}
			else update=false;
			} while (update);	
		--current_kplanarity;
		cout << "la funzione k planarity sul nodo " << id(n) << " ritorna " << current_kplanarity << endl;	
		return current_kplanarity;
		}




			void
		undi_graph::kplanarity
			(gdt::gdtnode_map<int>& kplan)
		{
		gdtnode n;
		forall_nodes(n,*this)
			kplan[n]=kplanarity(n);	
		return;
		}


			void
		undi_graph::extended_kplanarity
			(gdt::gdtnode_map<int>& kplan)
		{
		gdtnode n;
		forall_nodes(n,*this)
			kplan[n]=extended_kplanarity(n);	
		return;
		}







//////////////////////////////////////////////////////////////////////////////////77

        gdt::gdtlist<gdtedge>
        undi_graph::
make_biconnected ()
        {
	//cout << "Method make_biconnected...." << endl;	
	gdt::gdtlist<gdtedge> added_edges;	
	int index=1;
        gdt::gdtstack<gdtnode> S1;
        gdtnode w;
        gdtedge e;
        gdtnode v=first_node();
        if (v == NULL_NODE) gdt_error("empty graph");
	//cout << "Starting from node:" << id(v) << endl;
	
	gdt::gdtnode_map<bool> reached(*this,false);
	gdt::gdtedge_map<bool> e_reached(*this,false);
	gdt::gdtnode_map<bm_node_info> node_vector(*this);
	gdt::gdtnode_map<gdtedge> iterator(*this);
	gdt::gdtnode_map<int> DFI(*this);
		
	gdtnode root=v;
        gdtnode first_child;
	
   	if (!reached[v])
                {
		reached[v] = true;
		DFI[v]=index++;
		//cout << "first - setting on node: " << id(v)<< "    DFI=" << DFI[v] << endl;
		node_vector[v].parent= NULL_EDGE;
                node_vector[v].lowpoint=index;
                iterator[v] = first_adj_edge(v);
                first_child=opposite(v,iterator[v]);
		S1.push(v);
                }

        while (!S1.empty())
                {
                v = S1.top();
                e = iterator[v];
		
                gdtnode a=S1.pop();
		if (!S1.empty()&& e==NULL_EDGE && S1.top()==root && iterator[root]!=NULL_EDGE)
			{
			gdtedge it_root=iterator[root];
			while(it_root!=NULL_EDGE && reached[opposite(root,it_root)])
				it_root=adj_succ(it_root,root);
			iterator[root]=it_root;
			gdtnode temp_node;
			if (it_root==NULL_EDGE) temp_node=NULL_NODE;
			else  temp_node=opposite(root,iterator[root]);
			
			if (temp_node!=NULL_NODE)
				{
				gdtedge temp_edge=new_edge(first_child,opposite(root,iterator[root]));
				added_edges.append(temp_edge);
				//cout << "Added edge between " << id(first_child) << " and " << id(opposite(root,iterator[root])) << endl;
				first_child=temp_node;
				S1.push(first_child);
				e_reached[temp_edge]=false;
				e=temp_edge;
				gdtedge it_root=adj_succ(iterator[root],root);
				while(it_root!=NULL_EDGE && reached[opposite(root,it_root)])
					it_root=adj_succ(it_root,root);
				iterator[root]=it_root;
				}
			}
		S1.push(a);
		while (e != NULL_EDGE)
                        {
                        if (e_reached[e])
				 {
				 e=(adj_succ(e,v));
				 gdtnode a=S1.pop();
				if (!S1.empty()&& e==NULL_EDGE && S1.top()==root && iterator[root]!=NULL_EDGE)
					{
					gdtedge it_root=iterator[root];
					while(it_root!=NULL_EDGE && reached[opposite(root,it_root)])
						it_root=adj_succ(it_root,root);
					iterator[root]=it_root;
					gdtnode temp_node;
					if (iterator[root]==NULL_EDGE) temp_node=NULL_NODE;
					else temp_node=opposite(root,iterator[root]);
					if (temp_node!=NULL_NODE)
						{
						gdtedge temp_edge=new_edge(first_child,opposite(root,iterator[root]));
						added_edges.append(temp_edge);
						//std::cout << "1 - Added edge between " << id(first_child) << " and " << id(opposite(root,iterator[root])) << endl;
						first_child=temp_node;
						S1.push(first_child);
						
						e_reached[temp_edge]=false;
						e=temp_edge;
						gdtedge it_root=adj_succ(iterator[root],root);
						while(it_root!=NULL_EDGE && reached[opposite(root,it_root)])
							it_root=adj_succ(it_root,root);
						iterator[root]=it_root;
						}
					}
					S1.push(a);
				}
                        else                 {
                        //cout << "working with e= "<< id(e) << endl;
                        w = opposite(v,e);
                        e_reached[e]=true;
                        if (!reached[w])
                                {
                                 reached[w]  = true;
				 DFI[w]=index++;
		                 //cout << "setting on node: " << id(w)<< "    DFI=" << DFI[w] << endl;
                                 node_vector[w].parent= e;
                                 node_vector[v].children.append(w);
                                 node_vector[w].lowpoint=DFI[v];
				
                                 iterator[w] = first_adj_edge(w);
                                 S1.push(w);
                                 iterator[v] = adj_succ (e,v);
                                 v = w;
                                 e = iterator[v];
                                 gdtnode a=S1.pop();
				 if (!S1.empty()&& e==NULL_EDGE && S1.top()==root && iterator[root]!=NULL_EDGE)
				{
					gdtedge it_root=iterator[root];
					while(it_root!=NULL_EDGE && reached[opposite(root,it_root)])
					it_root=adj_succ(it_root,root);
					iterator[root]=it_root;
					gdtnode temp_node;
					if (it_root==NULL_EDGE) temp_node=NULL_NODE;
					else temp_node=opposite(root,it_root);
					if( temp_node!=NULL_NODE)
					{
						gdtedge temp_edge=new_edge(first_child,opposite(root,iterator[root]));
						
						//std::cout << "2 - Added edge between  " << id(first_child) << " and " << id(opposite(root,iterator[root])) << endl;
			
						first_child=temp_node;
						S1.push(first_child);
						added_edges.append(temp_edge);
						e_reached[temp_edge]=false;
						e=temp_edge;
						it_root=adj_succ(it_root,root);
						while(it_root!=NULL_EDGE && reached[opposite(root,it_root)])
							it_root=adj_succ(iterator[root],root);
						iterator[root]=it_root;
					}
				}
			 S1.push(a);
			}
                        else
                                {
                                if (e != (node_vector[v].parent))
                                        {
					
                                        node_vector[w].in_back_edges.append(v);
					gdtnode temp_c=opposite(w,e);
					if (node_vector[temp_c].first_back_edge_dfi>DFI[w])
						node_vector[temp_c].first_back_edge_dfi=DFI[w];
                                        if ((DFI[w])<(node_vector[v].lowpoint))
                                                {
						(node_vector[v].lowpoint)=DFI[w];
						}

                                         }
					iterator[v] = adj_succ (e,v);
					e = iterator[v];
					gdtnode a=S1.pop();
					if (!S1.empty()&& e==NULL_EDGE && S1.top()==root && iterator[root]!=NULL_EDGE)
						{
						gdtedge it_root=iterator[root];
						while(it_root!=NULL_EDGE && reached[opposite(root,it_root)])
							it_root=adj_succ(it_root,root);
						iterator[root]=it_root;
						gdtnode temp_node;
						if (iterator[root]==NULL_EDGE) temp_node=NULL_NODE;
						else temp_node=opposite(root,iterator[root]);
						if (temp_node!=NULL_NODE)
							{
							gdtedge temp_edge=new_edge(first_child,opposite(root,iterator[root]));
							added_edges.append(temp_edge);
						
							//std::cout << "3 - Added edge between " << id(first_child) << " and " << id(opposite(root,iterator[root])) << endl;
					
							first_child=temp_node;
							S1.push(first_child);
							//edge_vector[temp_edge]=new bm_edge_info();
							e_reached[temp_edge]=false;
							e=temp_edge;
							//v=temp_node;
							gdtedge it_root;
							it_root=adj_succ(iterator[root],root);
							while(it_root!=NULL_EDGE && reached[opposite(root,it_root)])
								it_root=adj_succ(it_root,root);
							iterator[root]=it_root;
							}
						}
						S1.push(a);
                                }
                        }

		}

                if ((S1.size())>1) {
                        S1.pop();
                        w=S1.top();    // w è il nodo nello stack sotto il nodo v
                        if (((node_vector[v].lowpoint)==DFI[w]) && (w!=root))  /* w is cut_vertex  */
                               {// cout << endl << "Cut-vertex found, vertex " << id(w) <<" DFI=" << DFI[w] << endl;
				gdtnode temp_node=opposite(w,node_vector[w].parent);
				gdtedge temp_edge=new_edge(v,temp_node);
				added_edges.append(temp_edge);
				
				//std::cout << "4 - Added edge between " << id(v) << " and " << id(temp_node) << endl;
				
				e_reached[temp_edge]=true;
				
				node_vector[opposite(w,node_vector[w].parent)].in_back_edges.append(v);
                                if (node_vector[v].first_back_edge_dfi>DFI[temp_node])
					node_vector[v].first_back_edge_dfi=DFI[temp_node];
				node_vector[v].lowpoint=DFI[opposite(w,node_vector[w].parent)];
                                }
                                else
                                	if ((node_vector[w].lowpoint)>(node_vector[v].lowpoint))
						{
						node_vector[w].lowpoint=node_vector[v].lowpoint;
						}
                        S1.push(v);
                }

	S1.pop();

        }
        	
	//std::cout << "End of method make_biconnected" << endl;	
	return added_edges;
} ;










