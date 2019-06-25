/*******************************************************************************
+
+  _rm3_flow_dire_graph.c
+
+  This file is part of GDToolkit. It can be 
+  used free of charge in academic research and teaching. 
+  Any direct or indirect commercial use of this software is illegal 
+  without a written authorization of
+  the Dipartimento di Informatica e Automazione
+
+  Universita' di Roma Tre, Roma, ITALIA
+
+
*******************************************************************************/


#include <GDT/rm3_global.h>
#include <GDT/rm3_flow_dire_graph.h>
#include <GDT/gdt_error.h>

using std::cout;
using std::endl;
using std::flush;
using std::ostream;


//-----------------------------------------------------------------------------
// flow_dire_graph:
// 	base class for network flow
//
// W.Didimo, A.Leonforte (1998)
//-----------------------------------------------------------------------------

// --------------------------
// Constructors and Operators
// --------------------------

	struct_flow_dire_node_info::
struct_flow_dire_node_info
	()
	{
	balance = 0;
	}


	struct_flow_dire_edge_info::
struct_flow_dire_edge_info
	()
	{
	upper_cap = INFINITE;
	lower_cap = 0;
	cost 	  = 0;
	flow 	  = 0;
	}



	flow_dire_graph::
flow_dire_graph
	()
	{
	//cout << "\n flow_dire_graph () creator" << flush;
	local_new();
	}



	flow_dire_graph::
~flow_dire_graph
	()
	{
	local_del();
	}



	flow_dire_graph::
flow_dire_graph 
	(const undi_graph& ug)
	{
	//cout << "\n flow_dire_graph (undi_graph) creator" << flush;
	dire_graph::operator=(ug);
	local_new ();
	local_init();
	}


	
	flow_dire_graph::
flow_dire_graph 
	(const dire_graph& dg)
	{
	//cout << "\n flow_dire_graph (dire_graph) creator" << flush;
	dire_graph::operator=(dg);
	local_new ();
	local_init();
	}



	flow_dire_graph::
flow_dire_graph 
	(const flow_dire_graph& fdg)
	{
	//cout << "\n flow_dire_graph (flow_dire_graph) creator" << flush;
	local_new();
	operator=(fdg);
	}



	flow_dire_graph&
	flow_dire_graph::
operator=
	(const undi_graph& ug)
	{
	//cout << "\n undi_graph -(copy)-> flow_dire_graph" << flush;
	dire_graph::operator=(ug);
	local_renew();
	local_init ();
	return *this;
	}
	
	
	
	flow_dire_graph&
	flow_dire_graph::
operator=
	(const dire_graph& dg)
	{
	//cout << "\n dire_graph -(copy)-> flow_dire_graph" << flush;
	dire_graph::operator=(dg);
	local_renew();
	local_init ();
	return *this;
	}
	
	
	
	flow_dire_graph&
	flow_dire_graph::
operator=
	(const flow_dire_graph& fdg)
	{
	//cout << "\n dire_graph -(copy)-> flow_dire_graph" << flush;
	gdtnode v,vc;
	gdtedge e,ec;
	dire_graph::operator=(fdg);
	
	forall_nodes (v,fdg)
		{
		vc = find_node (fdg.id(v));
		set_balance (vc,fdg.get_balance(v));
		}
	
	forall_edges (e,fdg)
		{
		ec = find_edge (fdg.id(e));
		set_upper_capacity (ec,fdg.get_upper_capacity(e));
		set_lower_capacity (ec,fdg.get_lower_capacity(e));
		set_cost	   (ec,fdg.get_cost(e));
		set_flow	   (ec,fdg.get_flow(e));
		}
	return *this;
	}
	
	

	// -----------------
	// Access operations
	// -----------------
	
	void
	flow_dire_graph::
local_get
	(gdt::gdtnode_map<struct_flow_dire_node_info>*& p1, gdt::gdtedge_map<struct_flow_dire_edge_info>*& p2)
	{
	p1 = node_info;
	p2 = edge_info;
	}
	
	
	
	int 
	flow_dire_graph::
get_balance        
	(gdtnode v) const	
	{
	return (*node_info)[v].balance;
	}
	

	
	int 
	flow_dire_graph::
get_upper_capacity 
	(gdtedge e) const
	{
	return (*edge_info)[e].upper_cap;
	}



	int 
	flow_dire_graph::
get_lower_capacity 
	(gdtedge e) const
	{
	return (*edge_info)[e].lower_cap;
	}	

	
	
	int 
	flow_dire_graph::
get_flow
	(gdtedge e) const
	{
	return (*edge_info)[e].flow;
	}



	int 
	flow_dire_graph::
get_cost
	(gdtedge e) const
	{
	return (*edge_info)[e].cost;
	}



	int 
	flow_dire_graph::
flow_cost
	() const
	{
	gdtedge e;
	int  fc = 0;
	forall_edges(e,*this)
		fc += get_cost(e)*get_flow(e);
	return fc;
	}



	int 
	flow_dire_graph::
in_flow
	(gdtnode v) 
	{
	gdtedge e;
	int  f = 0;
	forall_edges_entering_node (e,v,*this)
		f += get_flow(e);
	return f; 
	}



	int 
	flow_dire_graph::
out_flow
	(gdtnode v) 
	{
	gdtedge e;
	int  f = 0;
	forall_edges_leaving_node (e,v,*this)
		f += get_flow(e);
	return f; 
	}



	int
	flow_dire_graph::
diff_flow
	(gdtnode v) 
	{
	int f = out_flow(v) - in_flow(v);
	return f;
	}



	bool 
	flow_dire_graph::
balance_is_consistent	
	() const
	{
	gdtnode v;
	int  b = 0;
	forall_nodes (v,*this)
		b += get_balance(v);
	if (b) 
		{
		//cout << "balance=" << b << endl;
		return false;
		}
	return true;
	}



	bool 
	flow_dire_graph::
flow_is_consistent		
	(gdtedge e) const
	{
	if (get_flow(e) < get_lower_capacity(e)  ||
	    get_flow(e) > get_upper_capacity(e)) return false;
	return true;
	}



	bool 
	flow_dire_graph::
flow_is_consistent		
	(gdtnode v) 
	{
	if (diff_flow(v) != get_balance(v)) return false;
	return true;
	}
	


	bool 
	flow_dire_graph::
flow_is_consistent		
	() 
	{
	gdtnode v;
	gdtedge e;
	forall_edges (e,*this)
		if (!flow_is_consistent(e)) return false;
	forall_nodes (v,*this)
		if (!flow_is_consistent(v)) return false; 
	return true;
	}



	bool 
	flow_dire_graph::
is_consistent     		
	()	
	{
	if (!balance_is_consistent() || 
	    !flow_is_consistent()) return false;
	return false;
	}



	//------------------
	// Update operations
	//------------------
	

	void
	flow_dire_graph::	
clear 
	()
	{
	dire_graph::clear();
	local_renew ();
	}
	
	

	void 
	flow_dire_graph::
set_balance
	(gdtnode v, int b)
	{
	(*node_info)[v].balance = b;
	}	
	
	
	
	void
	flow_dire_graph::
set_upper_capacity 
	(gdtedge e, int uc)
	{
	if (uc < get_lower_capacity(e))
		gdt_error("upper/lower cap. not consistent");
	(*edge_info)[e].upper_cap = uc;
	}
	
	
	
	void
	flow_dire_graph::
set_lower_capacity 
	(gdtedge e, int lc)
	{
	if (lc > get_upper_capacity(e) || lc < 0)
		gdt_error("upper/lower cap. not consistent");
	(*edge_info)[e].lower_cap = lc;
	}	
	
	
	
	void
	flow_dire_graph::
set_cost
	(gdtedge e, int c)
	{
	(*edge_info)[e].cost = c;
	}
	
	
	
	void
	flow_dire_graph::
set_flow
	(gdtedge e, int f)
	{
	if (f < 0) 		
		gdt_error("flow must be non negative");
	(*edge_info)[e].flow = f;
	}	
	

	
	void
	flow_dire_graph::
set_edge_info
	(gdtedge e, int uc, int lc, int c, int f)
	{
	if (f < 0)
		gdt_error("flow must be non negative");
	if (lc < 0 || lc > uc)
		gdt_error("upper/lower cap. not consistent");
	(*edge_info)[e].upper_cap = uc;
	(*edge_info)[e].lower_cap = lc;
	(*edge_info)[e].cost	  = c ;
	(*edge_info)[e].flow	  = f ;		
	}
	
	
	
	void
	flow_dire_graph:: 
steal_from 
	(dire_graph& dg)
	{
	//cout << "\n dire_graph -(move)-> flow_dire_graph" << flush;
	clear();
	//
	// Copy all the pointers of dg
	// that is, the dire_graph part of this class
	// 
	dire_graph::mirror (dg);
	//
	// Create a new empty local_environment for dg
	// in order to avoid side_effects with this
	//
	dg.forget ();
	//
	// Inizialize the extra part (i.e. the records for capacity,
	// cost and flow) of this, according to the dire_graph part 
	// stolen from dg
	//
	local_renew();
	local_init();
	}
	
	
	
	void
	flow_dire_graph::
mirror
	(flow_dire_graph& fdg)
	{
	gdt::gdtnode_map<struct_flow_dire_node_info>* p1;
	gdt::gdtedge_map<struct_flow_dire_edge_info>* p2;
	dire_graph::mirror (fdg);
	fdg.local_get(p1,p2);
	local_set(p1,p2);
	}	



	void
	flow_dire_graph::
forget
	()
	{
	dire_graph::forget();
	local_new();
	}
	
	

	bool 
	flow_dire_graph::
min_cost_flow  
	()
	{
	// ---------------------------------------
	// disable the maintenance of the ordering
	// in the directed edges lists.
	// This allows us to save computation time
	// ---------------------------------------
	
	disable_keep_ordering_of_directed_edges();
	
	// --------------------------------------------------------------------------------
	// This is an implementation of the basic successive augmenting paths algorithm 
	// due to Gowman and Busaker. A complete description of this algorithm can be found
	// in "Network Flow", R.K.Ahuja, T.L.Magnanti and J.B.Orlin, 1995.
	// Currently such implementation does not support negative costs. We plan to 
	// extend it in the next future. Conversly, it works with lower capacity on edges.
	// With respect to the original formulation of the algorithm, this implementation
	// contains some variants, in order to have a faster version, tailored for 
	// Tamassia's network and similar.
	// --------------------------------------------------------------------------------
	
	// ONLY FOR CHECKING
	// -----------------
	   double max_pi = 0;
	   int    max_red_cost = 0;
	// -----------------
	
	bool exit = false;
	bool feasibility;
	gdtnode v;
	gdtedge e;
	int  total_flow;	 // total augmented flow
	gdt::gdtlist<gdtedge> dummy_edges;	 // list of dummy edges added to garantuee a non-infinite distance between two any nodes
	
	// ------------------------
	// Check balances and costs
	// ------------------------
	
	forall_edges (e,*this)
		if (get_cost(e)<0) 
			gdt_error("found a negative cost");
			
	if (!balance_is_consistent())
		gdt_error("balance is not consistent");
		
	// ------------------------
				
	while (!exit)
		{
		// --------------------------------
		// 		  0
		// --------------------------------
		// 	INITIALIZE VARIABLES
		// --------------------------------
		
		gdt::gdtnode_array<int> inb (nodes_and_edges(),0);		// inbalance for each gdtnode
		dummy_edges.clear();
		total_flow = 0;
	
		// --------------------------------------------------
		// Determine the maximal cost of an gdtedge and possibly
		// add a suitable number of dummy edges to guarantee
		// a non-infinite shortest path between two any nodes.
		// Also, the flow is initially set to 0 on each gdtedge, 
		// and the inbalance of a gdtnode is set equal to the 
		// balance value.
		// --------------------------------------------------
	
		int C = 1;	// maximal cost (different from INFINITE) of an gdtedge * number of edges

		forall_edges (e,*this)
			{
			int c = get_cost(e);
			if (c != INFINITE) C = gdt_maximum(C,c);
			set_flow(e,0);
			}
			
		C = C * number_of_edges();
		
		//cout << "\nADDING ARTIFICIAL EDGES WITH COST " << C << flush;
			
		gdtnode v0 = first_node();		// chose any gdtnode; 
			
		forall_nodes (v,*this)
			{
			if (v != v0)
				{
				gdtedge e1 = new_edge (v0,v),
				        e2 = new_edge (v,v0);
				set_edge_info(e1,INFINITE,0,C,0);
				set_edge_info(e2,INFINITE,0,C,0);
				dummy_edges.append(e1);
				dummy_edges.append(e2);
				}
			inb[v] = get_balance(v);	
			}
		// ---------------------------------------------------------------------------------
		
		// MOVED ABOVE	
		// -----------------------------------------------
		// Initially, the flow is set to 0 on each gdtedge, 
		// and the inbalance of a gdtnode is set equal to the 
		// balance value.
		// -----------------------------------------------
		//
		//forall_nodes (v,*this) inb[v] = get_balance(v);
		//forall_edges (e,*this) set_flow(e,0);
		//
		// END MOVED ABOVE
		
		
		
		// -------------------------------
		//		  1
		// -------------------------------
		// REMOVE NONZERO LOWER CAPACITIES
		// -------------------------------
	
		// -----------------------------------------------
		// The problem is translated to a similar one, in 
		// which the new flow variable is the following:
		// flow'(u,v) = flow(u,v) - lower_cap(u,v).
		// In this way, for each gdtedge, we have a new lower
		// capacity equal to 0, and a new upper capacity 
		// cap = upper_cap - lower_cap. Also, it is needed 
		// to update the inbalance value for each gdtnode.
		// -----------------------------------------------
	
		gdt::gdtedge_array<int> cap (nodes_and_edges(),0);	// new upper capacity of gdtedge
		
		forall_edges (e,*this)
			{
			int uc = get_upper_capacity(e);
			int lc = get_lower_capacity(e);
			if (lc > 0)
				{
				gdtnode u = start(e);
				gdtnode v = stop (e);
				inb[u] -= lc;		// inbalance of u is decreased
				inb[v] += lc;		// inbalance of v is increased
				
				if (uc == INFINITE) cap[e] = INFINITE;
				else		    cap[e] = uc - lc; 
			
				//cout << "\nReduced Capacity of the gdtedge (" << id(u) << "," << id(v) << ") to " << cap[e] << flush; 
				}
			else cap[e] = uc;
			}
	
	
		// -------------------------------
		// 		  2
		// -------------------------------
		//  COMPUTES A MIN-COST-FLOW WITH 
		//    1 - balance   = inbalance; 
		//    2 - lower_cap = 0; 
		//    3 - upper_cap = cap.
		// -------------------------------
	
		// -------------------------------- 2.1 ------------------------------------
		// Build the residual network 'rn' associated with the original network (fn)
		// -------------------------------------------------------------------------
	
		const bool FORWARD  = false;
		const bool BACKWARD = true;
	
		gdtnode v1, w, w1;
		gdtedge e1, e1inv;
		
		dire_graph rn;							  	// residual network
		gdt::gdtedge_map<gdtedge>   edge_corr_in_fn 	   (rn.nodes_and_edges(), NULL_EDGE);  	// rn-gdtedge --> fn-gdtedge
		gdt::gdtedge_map<gdtedge>	 inv_edge		   (rn.nodes_and_edges(), NULL_EDGE);	// rn-gdtedge --> rn-inv_edge
		gdt::gdtedge_map<bool>   direction_edge_corr_in_fn (rn.nodes_and_edges(), false);  	// rn-gdtedge --> dir. fn-gdtedge
		gdt::gdtedge_map<int>	 res_cost		   (rn.nodes_and_edges(), 0);	// cost of the edges in the residual network
		
		rn.disable_keep_ordering_of_directed_edges();
		
		forall_nodes (v,*this) v1 = rn.new_node(id(v));
		forall_edges (e,*this)
			if (cap[e] > 0) // make a forward gdtedge in rn
				{
				// --------------------
				// create a new gdtedge in 
				// the residual network
				// --------------------
			
				v 	= start(e);
				w 	= stop(e);
				v1	= rn.find_node(id(v));
				w1	= rn.find_node(id(w));
				e1    	= rn.new_edge(v1,w1) ;	// gdtedge forward  associated with e
				e1inv 	= NULL_EDGE ;		// gdtedge backward associated with e (initially NULL, becouse flow is 0 on e)
				inv_edge[e1] = e1inv;
			
				// --------------------
				// update gdtedge costs in 
				// the residual network
				// --------------------
			
				edge_corr_in_fn[e1] = e;
				direction_edge_corr_in_fn[e1] = FORWARD;
				res_cost[e1] = get_cost(e);
				}
	 	
		// -------------------------------- 2.2 ---------------------------------
		// Partition the gdtnode set into two lists: 
		// 	- E = nodes with positive inbalance (excess)
		// 	- D = nodes with negative inbalance (deficit)
		// ----------------------------------------------------------------------
	 		
		gdt::gdtlist<gdtnode> E;		// nodes with positive inbalance;
		gdt::gdtlist<gdtnode> D;		// nodes with negative inbalance;
		
		int balance_check = 0;	// quantity for balance check consistency (is not really needed)
	
		forall_nodes (v,*this)
			if 	(inb[v]>0) {E.append(v); balance_check += inb[v];}
			else if	(inb[v]<0) {D.append(v); balance_check += inb[v];}
		if (balance_check) 
			gdt_error("balance check error");
	
		// -------------------------------- 2.3 ----------------------------------
		// Compute a sequence of minimum cost augmenting paths as shortest paths
		// in the residual network, and using reduced costs as distances.
		// -----------------------------------------------------------------------
			
		gdt::gdtnode_array<gdtedge>   edge_pred   (rn.nodes_and_edges(), NULL_EDGE);	// father gdtedge
		gdt::gdtnode_array<int>    distance    (rn.nodes_and_edges(), INFINITE);	// distances
		gdt::gdtnode_array<double> pi 	       (rn.nodes_and_edges(), 0);		// gdtnode-potential
		int 		   max_augmenting_flow;					// max flow that can be augmented on a shortest path
		
		// experimental technique
		// for obtaining better distributions
		//
		//E.permute();
		//D.permute();
		//
					
		while (!E.empty())	// sequence of successive shortest paths
			{			
			// -------------------------------------------------------
			// scaling of the gdtedge costs in rn, in order to have only
			// positive costs (reduced_costs): res_cost --> red_cost 
			// -------------------------------------------------------

			gdt::gdtedge_array<int> red_cost (rn.nodes_and_edges(), 0);
		
			if (total_flow == 0) forall_edges (e1,rn) red_cost[e1] = res_cost[e1]; 
			else
				{
				// ----------------------
				// update gdtnode potentials
				// ----------------------
			
				forall_nodes (v1,rn)  
					{
					pi[v1] -= distance[v1];
					//cout << "\npi[" << rn.id(v1) << "]=" << pi[v1] << flush;
					
					// ONLY FOR CHECKING
					// -----------------
					   double p = pi[v1];
					   if (p < 0) p = -p;
					   if (max_pi < p) max_pi = p; 
					// -----------------
					}
				
				// ----------------------	
				// compute reduced costs
				// ----------------------
						
				forall_edges (e1,rn)  
					{
					v1 = rn.start(e1);
					w1 = rn.stop (e1);
					
					if (res_cost[e1] == INFINITE) red_cost[e1] = INFINITE;
					else			      red_cost[e1] = res_cost[e1] + int(pi[w1] - pi[v1]);
					
					// ONLY FOR CHECKING
					// -----------------
					   max_red_cost = gdt_maximum(max_red_cost,red_cost[e1]);
					// -----------------
										
					// ERROR CHECKING 
					// --------------
					if (red_cost[e1]<0)
						{
						// CHECKING
						// --------
						cout.precision(20);
						cout << "\nCOST " << rn.id(v1) << "->" << rn.id(w1) << ":"
						     << "\nres_cost = " << res_cost[e1] 
						     << "\npi[" << rn.id(v1) << "]    = " << pi[v1] 
						     << "\npi[" << rn.id(w1) << "]    = " << pi[w1] 
						     << "\ndiff. potentials = " << int(- pi[v1] + pi[w1])
						     << "\nred_cost = " << red_cost[e1] << flush; 
						// -------     			
						
						cout << "\nFAILED ALGORITHM!.." << flush;
						gdt_error("negative reduced cost");
						break;
						}
					} 
				}
			
			
			// ------------------------------------------------------------
			// Select an excess inbalance gdtnode s and a deficit inbalance 
			// gdtnode t, and compute a shorthest path from s to t in the 
			// residual network with reduced costs. Then determine the 
			// maximum flow that can be augmented on this path. 
			// ------------------------------------------------------------	
		
			gdtnode s = E.head();	// source gdtnode
			gdtnode t = D.head();	// sink gdtnode
		
			gdtnode s1 = rn.corresponding_node (s,*this);			
			gdtnode t1 = rn.corresponding_node (t,*this); 					
				
			rn.Dijkstra (s1, red_cost, distance, edge_pred);	// shortest path
			
			if (distance[t1] == INFINITE) 
				gdt_error("found an infinite distance");
		
		        v1 = t1;
			max_augmenting_flow = INFINITE;
	
			//cout << "\n PATH: ";
			
			// ------------------------------
			// compute the maximum augmenting 
			// flow on shortest path
			// ------------------------------
			 
			while (v1 != s1)
				{
				e1 = edge_pred[v1]; 
				e  = edge_corr_in_fn[e1];
				int c = cap[e];
				int x = get_flow(e);
				bool direction = direction_edge_corr_in_fn[e1];
			
				if      (direction == FORWARD  && max_augmenting_flow > c - x) max_augmenting_flow = c - x;
				else if (direction == BACKWARD && max_augmenting_flow > x)     max_augmenting_flow = x;
					
				//cout << id(find_node(rn.id(v1))); 
				//if (direction == FORWARD)  cout << "<-";
				//else			     cout << "->";
			
				v1 = rn.start(e1);
				}
		
			max_augmenting_flow = gdt_minimum(max_augmenting_flow,  inb[s]);
			max_augmenting_flow = gdt_minimum(max_augmenting_flow, -inb[t]);

			//cout << id(find_node(rn.id(v1))); 
			//cout << "\naugmenting flow = " << max_augmenting_flow << "\n";
			
			if (max_augmenting_flow == 0) 
				gdt_error("max augmenting flow is 0");
				
				
			// -------------------------------
			
	    		// -------------------------------------------------------------
			// augment the flow along all edges of the path in fn and update
		  	// the costs of the associated edges in the residual network rn
			// -------------------------------------------------------------			
			
			v1 = t1;
			gdtedge ex1;
			while (v1 != s1)
				{
				// ------------
				// augment flow
				// ------------
				
				ex1 = edge_pred[v1];
				e   = edge_corr_in_fn[ex1];
				bool direction = direction_edge_corr_in_fn[ex1];
				if (direction == FORWARD) set_flow (e, get_flow(e) + max_augmenting_flow);
				else 		 	  set_flow (e, get_flow(e) - max_augmenting_flow);
					
				v1 = rn.start(ex1);
				
				// -----------------------
				// update residual network
				// -----------------------
				
				// --------------------------
				// remove old rn edges 
				// associated with the gdtedge e
				// --------------------------
				
				gdtedge ex1_inv = inv_edge[ex1];
				rn.del_edge(ex1);
				if (ex1_inv) rn.del_edge(ex1_inv);
				
				// -----------------------------
				// add one or two new rn edges
				// associated with the gdtedge e
				// -----------------------------
								   
				gdtnode u  = start(e),
				     w  = stop(e),
				     u1 = rn.find_node(id(u)),
				     w1 = rn.find_node(id(w));
				
				int c = cap[e],
				    x = get_flow(e);
				
				gdtedge e1     = NULL_EDGE,
				     e1_inv = NULL_EDGE;
				     
				if (x < c) 
					{
					e1 = rn.new_edge(u1,w1);
					edge_corr_in_fn[e1] = e;
					direction_edge_corr_in_fn[e1] = FORWARD;
					res_cost[e1] = get_cost(e);
					}
				if (x > 0)
					{
					e1_inv = rn.new_edge(w1,u1);
					edge_corr_in_fn[e1_inv] = e;
					direction_edge_corr_in_fn[e1_inv] = BACKWARD;
					res_cost[e1_inv] = - get_cost(e);
					}
				
				if (e1)     inv_edge[e1] = e1_inv;
				if (e1_inv) inv_edge[e1_inv] = e1;	
				}
		
			// -------------------------------------------------------------
			// update total flow and inbalance for gdtnode s and t
			// -------------------------------------------------------------
		
			inb[s] -= max_augmenting_flow;
			inb[t] += max_augmenting_flow;
			if (!inb[s]) E.pop();		// delete balanced source
			if (!inb[t]) D.pop();		// delete balanced sink
			total_flow += max_augmenting_flow;
			}
			
		exit = true;
		}
		
	// -------------------------------
	//		  3
	// -------------------------------
	// ADJUST THE ORIGINAL FLOW VALUES
	// -------------------------------
	
	// -----------------------------------------------
	// All lower capacities are added to the obtained
	// flow, in order to adjust the original flow
	// requirements.
	// -----------------------------------------------
	
	forall_edges (e,*this)
		{
		int x = get_flow(e) + get_lower_capacity(e);
		set_flow(e,x);
		total_flow += x;
		//cout << "\nassigned flow " << x << " to gdtedge " << id(e) << flush;			
		}
	
	// -------------------------------
	//		  4
	// -------------------------------
	//   CHECK FLOW FEASIBILITY AND
	//  REMOVE ALL DUMMY NODES ADDED
	// -------------------------------
	
	feasibility = true;
	
	if (!flow_is_consistent())
		gdt_error("obtained a non-flow");
		
		
	forall (e,dummy_edges)
		{
		if (get_flow(e)) 
			{
			//cout << "\nFlow is not feasible!" << flush;
			feasibility = false;
			}
		del_edge (e);
		//Added by Marcandalli 8 2001
		edge_info->undefine(e);
		//		
		}
			
	//cout << "\nFLOW COMPUTATION TERMINATED" << flush;
	//cout << "\nMAX POTENTIAL = " << max_pi << flush;
	//cout << "\nMAX RED. COST = " << max_red_cost << flush;
	//cout << "\nTOTAL FLOW    = " << total_flow << flush;
	
	// re-enable the maintenance of the ordering of the directed edges lists
	// 
	enable_keep_ordering_of_directed_edges();
	
	return feasibility;
	}


//	bool 
//	flow_dire_graph::
//max_flow  
//	(gdtnode s, gdtnode t)
//	{
//	// STILL TO BE CODED
//	return false;
//	}

/* LEDA flow

	bool 
	flow_dire_graph::
min_cost_flow 
	()
	{
	gdtnode v;
	gdtedge e;
	graph G;
	gdt::gdtnode_map<gdtnode> corr_LEDA_node (nodes_and_edges(),NULL_NODE);
	gdt::gdtedge_map<gdtedge> corr_LEDA_edge (nodes_and_edges(),NULL_EDGE);
	 
	forall_nodes (v,*this) 
		{
		gdtnode u_leda = G.new_node ();
		corr_LEDA_node[v] = u_leda;
		}
		
	forall_edges (e,*this)
		{
		gdtnode u_leda = corr_LEDA_node[start(e)];
		gdtnode v_leda = corr_LEDA_node[stop(e)];
		gdtedge e_leda = G.new_edge (u_leda,v_leda);
		corr_LEDA_edge[e] = e_leda;
		}
	
	gdt::gdtedge_array<int> lcap_leda (G,0);
	gdt::gdtedge_array<int> ucap_leda (G,0);
	gdt::gdtedge_array<int> cost_leda (G,0);
	gdt::gdtedge_array<int> flow_leda (G,0);
	gdt::gdtnode_array<int> supply_leda (G,0);
	
	forall_nodes (v,*this)
		{
		gdtnode u_leda = corr_LEDA_node[v];
		supply_leda[u_leda] = get_balance(v);
		}
	forall_edges (e,*this)
		{
		gdtedge e_leda = corr_LEDA_edge[e];
		lcap_leda[e_leda] = get_lower_capacity(e);
		ucap_leda[e_leda] = get_upper_capacity(e);
		cost_leda[e_leda] = get_cost(e);
		flow_leda[e_leda] = 0;
		}
	
	if (!MIN_COST_FLOW (G, lcap_leda, ucap_leda, cost_leda, supply_leda, flow_leda))
		cout << "\n FLOW NOT FEASIBLE!!!" << flush;
	
	
	forall_edges (e,*this)
		{
		gdtedge e_leda = corr_LEDA_edge[e];
		set_flow(e,flow_leda[e_leda]);
		}
	
	// checking (temporary)
	//
	if (!flow_is_consistent())
		gdt_error("obtained a non-flow");
	
	return true;
	}
------------------ */ 

//--------------------------------------------------------
// PRIVATE
//--------------------------------------------------------

	void
	flow_dire_graph::
local_new
	()
	{
	//cout << "\n flow_dire_graph::local_new" << flush;
	node_info = new(gdt::gdtnode_map<struct_flow_dire_node_info>);
	edge_info = new(gdt::gdtedge_map<struct_flow_dire_edge_info>);
	node_info -> init (nodes_and_edges());
	edge_info -> init (nodes_and_edges());
	}


	void
	flow_dire_graph::
local_del	
	()
	{
	//cout << "\n flow_dire_graph::local_del" << flush;
	safe_delete (node_info);
	safe_delete (edge_info);
	}



	void
	flow_dire_graph::
local_renew
	()
	{
	local_del();
	local_new();
	}
	
	
	void
	flow_dire_graph::
local_init
	()
	{
	}
	
	

	void
	flow_dire_graph::
local_set
	(gdt::gdtnode_map<struct_flow_dire_node_info>* p1, gdt::gdtedge_map<struct_flow_dire_edge_info>* p2)
	{
	node_info = p1;
	edge_info = p2;
	}

