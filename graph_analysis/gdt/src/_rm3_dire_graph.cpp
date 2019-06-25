/*******************************************************************************
+
+  _rm3_dire_graph.c
+
+  This file is part of GDToolkit. It can be 
+  used free of charge in academic research and teaching. 
+  Any direct or indirect commercial use of this software is illegal 
+  without a written authorization of
+  the Dipartimento di Informatica e Automazione
+  Universita' di Roma Tre, Roma, ITALIA
+  
+  All rights reserved.
+ 
*******************************************************************************/

#include <GDT/gdtstack.h>

#include <GDT/rm3_global.h>
#include <GDT/rm3_dire_graph.h>
#include <GDT/gdt_error.h>

using namespace std;

//-----------------------------------------------------------------------------
// dire_graph:
// 	base class for all directed embedded graphs
//
// W.Didimo, A.Leonforte (1996)
//-----------------------------------------------------------------------------

	dire_graph::
dire_graph
	()
	{
	//cout << "\n dire_graph () creator" << flush;
	local_new();
	}


 

	dire_graph::
dire_graph
	(const undi_graph& ug)
	{
	//cout << "\n dire_graph (undi_graph) creator" << flush;
	if (!ug.all_edges_are_directed()) 
		gdt_error("undirected gdtedge found");
	undi_graph::operator=(ug);
	local_new();
	local_init();
	}



	
	dire_graph&
	dire_graph::
operator=
	(const undi_graph& ug)
	{
	//cout << "\n undi_graph -(copy)-> dire_graph" << flush;
	if (!ug.all_edges_are_directed()) 
		gdt_error("undirected gdtedge found");
	undi_graph::operator=(ug);
	local_renew();
	local_init();
	return *this;
	}
	
	
	
	// -----------------
	// Access operations
	// -----------------
	

	gdtnode
	dire_graph::
find_source
	()
	{
	gdtnode v;
	forall_nodes(v,*this)
		if (in_edges(v).empty()) return v;
	return NULL_NODE;
	}


	gdtnode
	dire_graph::
find_sink
	()
	{
	gdtnode v;
	forall_nodes(v,*this)
		if (out_edges(v).empty()) return v;
	return NULL_NODE;
	}
	
	

	void
	dire_graph::
find_shortest_path
	(
	gdtnode s,
	gdt::gdtedge_array<int>& length,
	gdt::gdtnode_array<int>& distance,
	gdt::gdtnode_array<gdtedge>& pred_edge
	)
	{
	Dijkstra (s,length,distance,pred_edge);
	}
	
	
	
	void
	dire_graph::
find_longest_path
	(
	gdtnode s,
	gdt::gdtedge_array<int>&  length,
	gdt::gdtnode_array<int>&  distance,
	gdt::gdtnode_array<gdtedge>& pred_edge
	) 
	{
	gdtnode v,w;
	gdtedge e;
	gdt::gdtlist<gdtnode> 	 order;
	gdt::gdtnode_array<bool> reached (nodes_and_edges(),false);
	
	// check the preconditions and compute the topological sort of nodes
	//
	if (topological_sort(s,reached,order))
		gdt_error("graph is not acyclic");
	
	//cout << "\nTopological sort completed : "; forall (v,order) print(v); cout << flush;
	
	forall_nodes(v,*this) if (!reached[v]) 
		gdt_error("gdtnode is not reacheble");
	
	// compute the longest path
	//
	forall (v,order)
		{
		distance[v] = 0;
		forall_edges_entering_node(e,v,*this)
			{
			w = start(e);
			if (distance[v] < distance[w] + length[e])
				{
				distance[v]  = distance[w] + length[e];
				pred_edge[v] = e; 
				}
			}
		//cout << "\ndistance of " << id(v) << "=" << distance[v] << flush;
		//if (pred_edge[v]) cout << "\npred_edge of " << id(v) << " = " << id(pred_edge[v]) << flush;
		//else		  cout << "\npred_edge of " << id(v) << " = NULL" << flush;
		}	
 	}



	void
	dire_graph:: 
find_path_with_minimum_switches 
	(gdtnode s, gdt::gdtnode_array<int>& switches, gdt::gdtnode_array<gdtedge>& pred_edge, visit_direction_type dir)
	{
	gdtnode v;
	visit_direction_type curr_dir;
	gdt::gdtnode_array<bool> reached (nodes_and_edges(),false);
	gdt::gdtlist<gdtnode> order_temp_1;
	gdt::gdtlist<gdtnode> order_temp_2;
	gdt::gdtlist<gdtnode> order;
	int switch_count = 0;
	
	
	// -------- 1 ---------
	// inizialize variables
	// --------------------

	forall_nodes (v,*this) 
		{
		switches[v]  = INFINITE;
		pred_edge[v] = NULL_EDGE;
		reached[v]   = false; 
		}	
	
	curr_dir = dir;
	
	// --------------------- 2 ----------------------
	// executes a number of directed visits, changing
	// curr_dir at each new visit.
	// ----------------------------------------------
	
	simple_DFS (s,reached,pred_edge,order,curr_dir);
	order_temp_1 = order;	
	
	while (!order_temp_1.empty())
		{
		order_temp_2.clear();
		
		if (curr_dir == OUTGOING) curr_dir = INCOMING;
		else			  curr_dir = OUTGOING;
		
		forall (v,order_temp_1)
			{
			switches[v]  = switch_count;
			order.clear();
			simple_DFS (v,reached,pred_edge,order,curr_dir);
			order_temp_2.conc(order);
			}
			
		order_temp_1 = order_temp_2;
		switch_count ++;
		}	
	}


	
	void
	dire_graph:: 
find_path_with_minimum_switches 
	(gdtnode s, gdtnode t, gdt::gdtnode_array<int>& switches, gdt::gdtnode_array<gdtedge>& pred_edge)
	{
	gdtnode v;
	gdt::gdtnode_array<int>  switches_out  (nodes_and_edges(), 0);
	gdt::gdtnode_array<int>  switches_in   (nodes_and_edges(), 0);
	gdt::gdtnode_array<gdtedge> pred_edge_out (nodes_and_edges(), NULL_EDGE);
	gdt::gdtnode_array<gdtedge> pred_edge_in  (nodes_and_edges(), NULL_EDGE);
	
	find_path_with_minimum_switches (s, switches_out, pred_edge_out, OUTGOING);
	find_path_with_minimum_switches (s, switches_in , pred_edge_in , INCOMING);
	
	if (switches_out[t] <= switches_in[t]) 
		forall_nodes (v,*this) {switches[v] = switches_out[v]; pred_edge[v] = pred_edge_out[v];}
	else	forall_nodes (v,*this) {switches[v] = switches_in[v] ; pred_edge[v] = pred_edge_in[v] ;}	
	}
	
		
		
	// ---------------------------
	// Access low-level operations
	// ---------------------------

	void 
	dire_graph::
Dijkstra	
	(
	gdtnode s,
	gdt::gdtedge_array<int>& length, 
	gdt::gdtnode_array<int>& distance,
	gdt::gdtnode_array<gdtedge>& pred_edge
	) 	      
	{
	//cout << "\ndire_graph::Dijkstra" << flush;
	//Modified by A.M. 10/2002
	//graph& g = nodes_and_edges();
	undi_graph& g = *this;

	gdt::gdtnode_pq<int> PQ(g);
	int c;
	gdtnode u,v;
	gdtedge e;
	
	// Inizialize
	//
	forall_nodes (v,*this)
		{
		pred_edge[v] = NULL_EDGE;
		distance[v]  = INFINITE;
		PQ.insert(v,distance[v]);
		}
	
	
	// Inizialize and put in PQ randomly
	// experimental
	//
	//gdt::gdtlist<gdtnode> L = all_nodes();
	//L.permute();
	//forall (v,L)
	//	{
	//	pred_edge[v] = NULL_EDGE;
	//	distance[v]  = INFINITE;
	//	PQ.insert(v,distance[v]);
	//	}
	//
	
	distance[s] = 0;
	PQ.decrease_inf(s,0);
	
	// Search distances
	//
	while (!PQ.empty())
		{
		u = PQ.del_min();
		forall_edges_leaving_node(e,u,*this)
			{
			v = stop(e);
			if (distance[u] == INFINITE || length[e] == INFINITE) c = INFINITE;
			else						      c = distance[u] + length[e];
			
			// CHECKING FOR TOO LARGE INTEGER VALUES
			// --------------------------------------------
			if (abs(c) < distance[u] || abs(c) < length[e]) 
				{
				cout << "\nWARNING: Dijkstra is handling too large integer: computation can be invalid!!" << flush; 
				cout << "\ndistance[" << id(u) << "] = " << distance[u] << flush; 
				cout << "\nlength  [" << id(e) << "] = " << length[e] << flush;
				cout << "\nSUM c = " << c << flush;
				}
			// --------------------------------------------
			
			else if (c < distance[v])
				{
				distance[v] = c;
				//cout << "\n distance[" << id(v) << "]=" << c << "\n" <<flush;
				pred_edge[v] = e;
				PQ.decrease_inf(v,c);
				}
			}
		}
	}



	gdt::gdtlist<gdtedge>
	dire_graph::
simple_DFS 
	(gdtnode v, gdt::gdtnode_array<bool>& reached, gdt::gdtnode_array<gdtedge>& father, gdt::gdtlist<gdtnode>& order, visit_direction_type dir)
	{
	//cout << "\nSTART VISIT FROM NODE " << id(v) << "\n" << flush;
	gdt::gdtlist<gdtedge> cyclic;
	gdt::gdtstack<gdtnode> S;
	gdtnode w;
	gdtedge e;
	gdt::gdtnode_array<gdtedge> iterator (nodes_and_edges(),NULL_EDGE);
	gdt::gdtnode_array<bool> extracted(nodes_and_edges(),false);
	
	forall_nodes (w,*this) if (reached[w]) extracted[w] = true;
	
	if (!reached[v])
		{		
		reached[v]  = true;
		father[v]   = NULL_EDGE;
		order.append (v);
		//cout << "\nVISITED NODE " << id(v) << flush;
		}
		
	if (dir == OUTGOING) iterator[v] = first_out_edge(v);
	else		     iterator[v] = first_in_edge(v);
	S.push(v);
		
	while (!S.empty())
		{
		v = S.top();
		e = iterator[v];
		while (e)
			{
			w = opposite(v,e);
			if (!reached[w])
				{
				reached[w]  = true;
				father[w]   = e;
				if (dir == OUTGOING) iterator[w] = first_out_edge(w);
				else		     iterator[w] = first_in_edge(w);
				S.push(w);
				order.append (w);
				//cout << "\nVISITED NODE " << id(w) << flush;
				if (dir == OUTGOING) iterator[v] = out_succ (e,v);
				else		     iterator[v] = in_succ  (e,v);
				v = w;
				e = iterator[v];
				}
			else 	
				{
				if (!extracted[w]) cyclic.append(e);
				if (dir == OUTGOING) iterator[v] = out_succ (e,v);
				else		     iterator[v] = in_succ  (e,v);
				e = iterator[v];
				}
			}
		v = S.pop();
		extracted[v]= true;
		}
	return cyclic;
	}
	
	
	
	bool
	dire_graph::
is_acyclic
	()
	{
	bool acyclic = true; 
	gdt::gdtnode_array<bool> reached (nodes_and_edges(), false);
	gdt::gdtnode_array<gdtedge> father  (nodes_and_edges(), NULL_EDGE);
	gdt::gdtlist<gdtnode>       order;
	gdtnode v;
	forall_nodes (v,*this) reached[v]=false; 
	forall_nodes (v,*this)
		if (!reached[v])
			{
			gdt::gdtlist<gdtedge> cyclic_edges;
			cyclic_edges = simple_DFS (v,reached,father,order); 
			if (cyclic_edges.size() > 0)
				{ 
				acyclic = false;
				break;
				}
			}
	
	//if (acyclic) cout << "\nACYCLIC" << flush;
	//else	     cout << "\nCYCLIC"  << flush;
	return acyclic;
	}
	
	
	gdt::gdtlist<gdtedge>
	dire_graph::
make_acyclic
	()
	{
	gdt::gdtlist<gdtedge> inverted;
	gdt::gdtnode_array<bool> reached (nodes_and_edges(), false);
	gdt::gdtnode_array<gdtedge> father  (nodes_and_edges(), NULL_EDGE);
	gdt::gdtlist<gdtnode>       order;
	gdtnode v;
	forall_nodes (v,*this) reached[v]=false; 
	forall_nodes (v,*this)
		if (!reached[v])
			{
			gdt::gdtlist<gdtedge> cyclic_edges;
			cyclic_edges = simple_DFS (v,reached,father,order); 
			inverted.conc(cyclic_edges);					
			}
	gdtedge e;
	forall (e,inverted)
		reverse(e);
	
	return inverted;
	}

	
	bool
	dire_graph::
topological_sort
	(gdtnode v, gdt::gdtnode_array<bool>& reached, gdt::gdtlist<gdtnode>& order)
	{
	//cout << "\nSTART VISIT FROM NODE " << id(v) << "\n" << flush;
	bool cyclic = false;
	gdt::gdtstack<gdtnode> S;
	gdtnode w;
	gdtedge e;
	gdt::gdtnode_array<gdtedge> iterator (nodes_and_edges(),NULL_EDGE);
	gdt::gdtnode_array<gdtedge> father	  (nodes_and_edges(),NULL_EDGE);
	gdt::gdtnode_array<bool> extracted(nodes_and_edges(),false);

	if (!reached[v])
		{		
		reached[v] = true;
		father[v] = NULL_EDGE;
		iterator[v] = first_out_edge(v);
		S.push(v);
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
				iterator[w] = first_out_edge(w);
				S.push(w);
				iterator[v] = out_succ (e,v);
				v = w;
				e = iterator[v];
				}
			else 	
				{
				if (!extracted[w]) cyclic = true;
				iterator[v] = out_succ (e,v);
				e = iterator[v];
				}
			}
		v = S.pop();
		extracted[v] = true;
		order.push(v);
		//cout << "\nEXTRACTED NODE " << id(v) << "\n" << flush;
		}
	return cyclic;
	}
	


	// -----------------
	// Update operations
	// -----------------

	gdtedge
	dire_graph::
new_edge
	(gdtnode v, gdtnode w, int new_id) 
	{
	gdtedge e = undi_graph::new_edge(v,w,new_id);
	make_directed (e,v);
	return e;
	}
	
	
	
	gdtedge
	dire_graph::
new_edge
	(gdtnode v, gdtedge ev, gdtnode w, int new_id, int dir) 
	{
	gdtedge e = undi_graph::new_edge(v,ev,w,new_id,dir);
	make_directed (e,v);
	return e;
	}
	
	
	

	gdtedge
	dire_graph::
new_edge
	(gdtnode v, gdtnode w, gdtedge ew, int new_id, int dir) 
	{
	gdtedge e = undi_graph::new_edge(w,ew,v,new_id,dir);
	make_directed (e,v);
	return e;
	}
	
	
	

	gdtedge
	dire_graph::
new_edge
	(gdtnode v, gdtedge ev, gdtnode w, gdtedge ew, int new_id, int dir) 
	{
	gdtedge e = undi_graph::new_edge(v,ev,w,ew,new_id,dir);
	make_directed (e,v);
	return e;
	}
	
	

	void
	dire_graph::
clear
	()
	{
	undi_graph::clear();
	local_renew();
	}
	
	
	
	void
	dire_graph::
steal_from
	(undi_graph& ug)
	{
	//cout << "\n undi_graph -(move)-> dire_graph" << flush;
	clear();
	//
	// Copy all the pointers of ug
	// that is, the undi_graph part of this class
	//
	mirror (ug);
	//
	// Create a new empty environment for ug
	// in order to avoid side-effects with this
	//
	ug.forget();
	//
	// No extra part to build, with respect to base class undi_graph,
	// therefore, no local_renew() is required.
	}



//-----------------------------------------------------------------------------
//	PRIVATE
//-----------------------------------------------------------------------------


	void
	dire_graph::
local_new
	()
	{
	//cout << "\n dire_graph::local_new" << flush;
	// No additional attribute to be allocated
	// with respect to the parent class undi_graph

	// Removed A. M.
	//nodes_and_edges().make_directed();
	// Removed by A. M. 10/2002
	//assert(nodes_and_edges().is_directed);
	}
	


	void
	dire_graph::
local_del
	()
	{
	//cout << "\n dire_graph::local_del" << flush;
	// No additional attribute to be deallocated
	// with respect to the parent class undi_graph
	}
	
	
	
	void
	dire_graph::
local_renew
	()
	{
	local_del();
	local_new();
	}



	void
	dire_graph::
local_init
	()
	{
	// Intentionally left blank,
	// nothing to initialize in this class
	}
