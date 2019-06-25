/*******************************************************************************
+
+  _rm3_upwa_plan_undi_graph.cpp
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


#include <GDT/gdtstack.h>
#include <GDT/gdt_error.h>
#include <GDT/rm3_global.h>
#include <GDT/rm3_flow_dire_graph.h>
#include <GDT/rm3_upwa_plan_undi_graph.h>
#include <GDT/rm3_SPQR_tree.h>

using namespace std;

//-----------------------------------------------------------------------------
// upwa_plan_undi_graph: base class for handling upward representations of 
// 			 directed planar graphs.
//
// W.Didimo, A.Leonforte (1997)
//-----------------------------------------------------------------------------

// ---------------------------------
// struct_upwa_node_info constructor
// ---------------------------------

	struct_upwa_node_info::
struct_upwa_node_info
	()
	{
	//cout << "\nstruct_upwa_node_info () creator" << flush;
	first_in_cand  = NULL_EDGE;
	first_out_cand = NULL_EDGE;
	in_cand_edges_are_ordered  = false;
	out_cand_edges_are_ordered = false;
	}
	
	
// ----------------------------------
// 	  upwa_plan_undi_graph
// ----------------------------------
	
	
	upwa_plan_undi_graph::
~upwa_plan_undi_graph 
	()
	{
	local_del();
	}
	
	
	
	upwa_plan_undi_graph::
upwa_plan_undi_graph
	()
	{
	//cout << "\nupwa_plan_undi_graph () creator" << flush;
	gdtedge e = plan_undi_graph::new_edge(first_node(),last_node(),first_face());
	plan_undi_graph::make_directed(e,source(e));
	local_new ();
	face ef = first_face();
	local_init(ef,PLAN_UPWARD_OPTIMAL,true);
	}
	
	
	upwa_plan_undi_graph::	
upwa_plan_undi_graph  
	(const undi_graph& ug, algorithm_type alg, bool err_mess)
	{
	//cout << "\nupwa_plan_undi_graph (undi_graph) creator" << flush;
	plan_undi_graph::operator=(ug);
	local_new();
	face ef = first_face();
	local_init(ef,alg,err_mess);
	}	
	

	
	upwa_plan_undi_graph::
upwa_plan_undi_graph 
	(const plan_undi_graph& pug, algorithm_type alg, bool err_mess)
	{
	//cout << "\nupwa_plan_undi_graph (plan_undi_graph) creator" << flush;
	plan_undi_graph::operator=(pug);
	local_new();
	face ef = first_face();
	local_init(ef,alg,err_mess);
	}
		
	
	upwa_plan_undi_graph::
upwa_plan_undi_graph
	(const plan_undi_graph& pug, face ef, algorithm_type alg, bool err_mess) 
	{
	//cout << "\n upwa_plan_undi_graph (plan_undi_graph) creator" << flush;
	
	plan_undi_graph::operator=(pug);
	local_new();
	local_init(corresponding_face(ef,pug),alg,err_mess);
	}
	
	
	
	upwa_plan_undi_graph::
upwa_plan_undi_graph
	(const plan_undi_graph& pug, gdt::gdtnode_array<gdtedge>& fce)
	{
	//cout << "\n upwa_plan_undi_graph (plan_undi_graph, gdt::gdtnode_array<gdtedge>) creator" << flush;
	
	plan_undi_graph::operator=(pug);
	local_new();
	
	if (!all_edges_are_directed())
  		gdt_error("not a directed graph");
		
	if (!is_candidate())
  		gdt_error("not a candidate embedding");
	
	
	// set candidate lists (upward shape)
	//
	gdtnode v,u;
	gdtedge ev,eu;
	forall_nodes (v,*this)
		if (is_extremal(v))
			{
			u  = pug.corresponding_node(v,*this);
			eu = fce[u];
			ev = corresponding_edge(eu,pug);
			
			if (is_source(v)) set_first_out_cand(v,ev);
			else		  set_first_in_cand (v,ev);
			}
	order_cand ();
	
	//cout << "\nDETECTING EXTERNAL FACE.." << flush;
	
	// find and set the correct external face
	//
	face f;
	forall_faces (f,*this)
		{
		//cout << "\nSCANNING FACE " << id(f) << " : " << flush; print(f); cout << flush;
		
		int Bf = big_angles(f).size();
		
		//cout << "\nnumber of big angles = " << Bf << flush;
		//cout << "\nnumber of extremals  = " << number_of_extremals(f) << flush;
		
		if (Bf == number_of_extremals(f)/2 + 1) 
			{
			//cout << "\nfound external face " << id(f) << flush;
			set_external_face(f);
			break;
			}
		}
		
	if (!is_upward())
  		gdt_error("no upward shape obtained");
		
	}
	

	
	upwa_plan_undi_graph::	
upwa_plan_undi_graph 
	(const upwa_plan_undi_graph& upug)
	{
	//cout << "\nupwa_plan_undi_graph (upwa_plan_undi_graph) creator" << flush;
	local_new();    // required because operator= calls renew()
	operator=(upug);
	}
	
	
	
	upwa_plan_undi_graph&	
	upwa_plan_undi_graph::		
operator= 
 	(const undi_graph& ug)
 	{
 	//cout << "\n undi_graph -(copy)-> upwa_plan_undi_graph" << flush;
	plan_undi_graph::operator=(ug);
	local_renew();
	face ef = first_face();
	local_init(ef,PLAN_UPWARD_OPTIMAL,true); 
	return *this;	
 	}
 	
 	
	
	upwa_plan_undi_graph& 
	upwa_plan_undi_graph::
operator= 
	(const plan_undi_graph& pug)
	{
 	//cout << "\n undi_graph -(copy)-> upwa_plan_undi_graph" << flush;
	plan_undi_graph::operator=(pug);
	local_renew();
	face ef = first_face();
	local_init(ef,PLAN_UPWARD_OPTIMAL,true); 
	return *this;	
	}
	
	
		
	upwa_plan_undi_graph& 
	upwa_plan_undi_graph::
operator= 
	(const upwa_plan_undi_graph& upug)
	{
 	//cout << "\n plan_undi_graph -(copy)-> upwa_plan_undi_graph" << flush;
 	
	plan_undi_graph::operator=(upug);
	local_renew();
	
	gdtnode v;
	gdtedge e;
	forall_nodes (v,upug)
		{
		gdtnode w     = find_node (upug.id(v));
		
		gdtedge e_in  = upug.get_first_in_cand (v);
		if (e_in) e = find_edge (upug.id(e_in));
		else	  e = NULL_EDGE;
		set_first_in_cand (w,e);
		set_in_cand_edges_are_ordered (w,upug.get_in_cand_edges_are_ordered(v));
		
		gdtedge e_out = upug.get_first_out_cand(v);
		if (e_out) e = find_edge (upug.id(e_out));
		else	   e = NULL_EDGE;
		set_first_out_cand (w,e);
		set_out_cand_edges_are_ordered (w,upug.get_out_cand_edges_are_ordered(v));	
		}
		
	face ef = upug.external_face();
	face f  = find_face (upug.id(ef));
	set_external_face (f);
	//
	// Linear Solution
	//
	gdt::gdtedge_map<gdtedge> edge_corr_in_upug (nodes_and_edges(),NULL_EDGE);
	build_mapping_edge_to_edge_with_undi_graph (upug, edge_corr_in_upug);
	forall_edges (e,*this) 
		constraint_on_bends_of_edge[e] = upug.get_constraint_on_bends_of_edge (edge_corr_in_upug[e]);
		
	set_layout_algorithm (upug.get_layout_algorithm());
	last_cost = upug.cost_of_last_algorithm();
	
	return *this;
	}
	
	
	
		
	// -----------------
	// Access operations
	// -----------------

	void 
	upwa_plan_undi_graph::
local_get
	(
	face& p1, 
	algorithm_type& p2, 
	gdt::gdtedge_map<bend_constraint>& p3, 
	gdt::gdtnode_map<struct_upwa_node_info>*& p4,
	int& p5)
	{
	p1 = ext_face;
	p2 = layout_algorithm;
	p3 = constraint_on_bends_of_edge;
	p4 = node_info;
	p5 = last_cost;
	}
	

	
	
	bool 
	upwa_plan_undi_graph::
get_in_cand_edges_are_ordered  
	(gdtnode v) const
	{
	return (*node_info)[v].in_cand_edges_are_ordered;
	}
	
	
	
	bool 
	upwa_plan_undi_graph::
get_out_cand_edges_are_ordered  
	(gdtnode v) const
	{
	return (*node_info)[v].out_cand_edges_are_ordered;
	}
	
		
		
	gdtedge 
	upwa_plan_undi_graph::
get_first_in_cand 
	(gdtnode v) const
	{
	return (*node_info)[v].first_in_cand;
	}
		
	
	
	gdtedge 
	upwa_plan_undi_graph::
get_first_out_cand
	(gdtnode v) const
	{
	return (*node_info)[v].first_out_cand;
	}	
			
		
		
	gdtedge 
	upwa_plan_undi_graph::
first_in_cand_edge  
	(gdtnode v)
	{
	order_in_cand (v);
	gdtedge e = get_first_in_cand (v);
	return e;
	}
	
	
	
	gdtedge 
	upwa_plan_undi_graph::
first_out_cand_edge  
	(gdtnode v) 
	{
	order_out_cand (v);
	gdtedge e = get_first_out_cand (v);
	return e;
	}
	
	
	
	gdtedge
	upwa_plan_undi_graph::
cand_edge_succ
	(gdtedge e, gdtnode v)
	{
	order_in_cand (v);
	bool entering = (v == stop(e));
	gdtedge e1;
	
	if (entering) 
		{
		e1 = cyclic_in_succ  (e,v);
		if (e1 == first_in_cand_edge (v)) e1 = NULL_EDGE;
		}
	else	
		{      
		e1 = cyclic_out_pred (e,v);
		if (e1 == first_out_cand_edge (v)) e1 = NULL_EDGE;
		}
	
	return e1;
	}
	
	
	
	gdtedge
	upwa_plan_undi_graph::
cand_edge_pred
	(gdtedge e, gdtnode v)
	{
	order_in_cand (v);
	bool entering = (v == stop(e));
	gdtedge e1 = NULL_EDGE;
	
	if (entering) 
		if (e1 == first_in_cand_edge (v)) e1 = NULL_EDGE;
		else e1 = cyclic_in_pred  (e,v);
	else	
		if (e1 == first_out_cand_edge(v)) e1 = NULL_EDGE;      
		else e1 = cyclic_out_succ (e,v);
		
	return e1;
	}
	
		
	
	face
	upwa_plan_undi_graph::
external_face
	() const
	{
	return ext_face;
	}
		


	algorithm_type
	upwa_plan_undi_graph::
get_layout_algorithm
	() const
	{
	return layout_algorithm;
	}
		
	
		
	bend_constraint	
	upwa_plan_undi_graph::
get_constraint_on_bends_of_edge 
	(gdtedge e)  const
	{
	return constraint_on_bends_of_edge[e]; 
	}
	
		
	
	bool
	upwa_plan_undi_graph:: 
is_source		 
	(gdtnode v) const
	{
	return undi_graph::is_source(v);
	}
		
		
	
	bool 
	upwa_plan_undi_graph::
is_target		 
	(gdtnode v) const
	{
	return undi_graph::is_target(v);
	}
	
	
	
	bool 
	upwa_plan_undi_graph::
is_extremal	 
	(gdtnode v) const
	{
	return undi_graph::is_extremal(v);
	}
	
	
	
	bool 
	upwa_plan_undi_graph::
is_internal	 
	(gdtnode v) const
	{
	return undi_graph::is_internal(v);
	}
	
	
	
	bool 
	upwa_plan_undi_graph::
is_extremal	 
	(gdtnode v, gdtedge e) const
	{
	return undi_graph::is_extremal(v,e);
	}
		
		
	
	bool 
	upwa_plan_undi_graph::
is_internal	 
	(gdtnode v, gdtedge e) const
	{
	return undi_graph::is_internal(v,e);
	}
	
	
	
	bool
	upwa_plan_undi_graph:: 
is_source
	(gdtnode v, face f) const
	{
	bool all_leaving = true;
	gdtedge e;
	gdt::gdtlist<gdtedge> le = adj_edges (f,v);
	forall (e,le)
		if (start(e)!=v) {all_leaving = false; break;}
	return all_leaving;
	}
	
	
	bool
	upwa_plan_undi_graph:: 
is_target
	(gdtnode v, face f) const
	{
	bool all_entering = true;
	gdtedge e;
	gdt::gdtlist<gdtedge> le = adj_edges (f,v);
	forall (e,le)
		if (stop(e)!=v) {all_entering = false; break;}
	return all_entering;
	}
	
	
	
	bool
	upwa_plan_undi_graph:: 
is_extremal
	(gdtnode v, face f) const
	{
	bool all_same_direction = true;
	gdtedge e, e1;
	gdt::list_item e_pos, e1_pos;
	gdt::gdtlist<gdtedge> le = adj_edges (f,v);
	e_pos  = le.first();
	e      = le.inf (e_pos);
	e1_pos = le.succ(e_pos);
	
	while ((e1_pos) && (all_same_direction)) 
		{
		e1     = le.inf(e1_pos);
		if ((start(e)==v && stop(e1)==v) || 
		    (stop(e)==v && start(e1)==v))  all_same_direction = false;
		e_pos  = e1_pos;
		e      = e1;
		e1_pos = le.succ(e_pos);
		}
	return all_same_direction;
	}

	
	
	bool
	upwa_plan_undi_graph:: 
is_internal
	(gdtnode v, face f) const
	{
	return (!is_extremal(v,f));
	}
	


	bool
	upwa_plan_undi_graph::
is_big_angle
	(gdtnode v, face f)
	{
	if (is_internal(v)) return false;		
		
	gdtedge e  = first_adj_edge (f);
	gdtedge e1 = NULL_EDGE;
	gdtnode w = NULL_NODE;
	
	if (degree(f)==2) e1 = last_adj_edge (f);
	else
		while (w != v)
			{
			e1 = e;
			e  = cyclic_adj_succ (e1,f);
			w  = node_between_adj_edges (e1,e);
			}
		
	if ((is_source(v) && e1==first_out_cand_edge(v)) ||
	    (is_target(v) && e ==first_in_cand_edge (v))) return true;
	    	
	return false;
	}



	bool
	upwa_plan_undi_graph::
is_big_angle
	(gdtnode v, gdtedge e1)
	{
	if (is_internal(v)) return false;
	if (degree(v)==1)   return true;
	
	gdtedge e2 = cyclic_adj_succ(e1,v);
	
	if ((is_target(v) && e2==first_in_cand_edge (v))) return true;
	if ((is_source(v) && e1==first_out_cand_edge (v))) return true;
	
	return false;
	}



	gdt::gdtlist<gdtedge>
	upwa_plan_undi_graph::
in_cand_edges
	(gdtnode v)
	{
	order_in_cand (v);
	gdt::gdtlist<gdtedge> in_cand_list;
	gdtedge e;
	forall_candidate_edges_entering_node (e,v,*this)
		in_cand_list.append(e);
	return in_cand_list;
	}


	
	gdt::gdtlist<gdtedge>
	upwa_plan_undi_graph::
out_cand_edges
	(gdtnode v)
	{
	order_out_cand (v);
	gdt::gdtlist<gdtedge> out_cand_list;
	gdtedge e;
	forall_candidate_edges_leaving_node (e,v,*this)
		out_cand_list.append(e);
	return out_cand_list;
	}
	
	
	
	int
	upwa_plan_undi_graph::
number_of_extremals
	() const
	{
	return undi_graph::number_of_extremals();
	}
	

	int
	upwa_plan_undi_graph:: 
number_of_extremals
	(face f) const
	{
	return extremals(f).length();
	}
	
	
	
	int
	upwa_plan_undi_graph::
number_of_bends
	() const
	{
	gdtnode v;
	int total = 0;
	forall_nodes (v,*this)
		if (is_marked(v,RM3_ADDED_BY_MAKE_UPWARD)) total++;
	return total;	
	}
	
	
	
	int
	upwa_plan_undi_graph::
max_bends_on_edge
	() const
	{
	int mnbe = 0;
	gdt::gdtnode_array<bool> counted(nodes_and_edges(),false);
	gdtnode v,v1;
	gdtedge e,e1;
	
	forall_nodes (v,*this)
		if (!counted[v] && is_marked(v,RM3_ADDED_BY_MAKE_UPWARD))
			{
			int nbe = 1;
			counted[v] = true;
			
			// count all the previous bends with respect to v
			// ----------------------------------------------
			e = first_adj_edge (v);
			e1 = e;
			v1 = v;
			
			do
				{
				if (v1 != v) {counted[v1] = true; nbe ++;}
				v1 = opposite(v1,e1);
				e1 = cyclic_adj_pred (e1,v1);
				}
			while (is_marked(v1,RM3_ADDED_BY_MAKE_UPWARD));
			
			
			// count all the successive bends with respect to v
			// ------------------------------------------------
			e = last_adj_edge (v);
			e1 = e;
			v1 = v;
			
			do
				{
				if (v1 != v) {counted[v1] = true; nbe ++;}
				v1 = opposite(v1,e1);
				e1 = cyclic_adj_succ (e1,v1);
				}
			while (is_marked(v1,RM3_ADDED_BY_MAKE_UPWARD));
			
			mnbe = gdt_maximum(mnbe,nbe);
			}
	return mnbe;
	}
	
			
	int
	upwa_plan_undi_graph:: 
capacity
	(face f) const
	{
	int cap = 0;
	if (f == external_face()) cap = number_of_extremals(f)/2 + 1;
	else			  cap = number_of_extremals(f)/2 - 1;
	return cap;
	}
		


	gdt::gdtlist<gdtnode>
	upwa_plan_undi_graph::
extremals
	(face f) const
	{
	gdt::gdtlist<gdtnode> ext;
	gdtnode v;
	gdtedge e;
	border_step bs;
	//forall_face_edges (e,f)
	//	{
	//	e1= cyclic_adj_succ (e,f);
	//	v = node_between_adj_edges(e,e1);
	
	forall_face_border_steps (bs,f)
		{
		v   = stop_of_border_step (bs);
		e   = edge_of_border_step (bs);
		if (is_extremal(v,e)) ext.append(v);
		}
	return ext;
	}


	void
	upwa_plan_undi_graph::
extremals
	(face f, gdt::gdtlist<gdtnode>& ext, gdt::gdtmap<gdt::list_item,bool>& is_big) 
	{
	gdt::list_item pos;
	gdtnode v;
	gdtedge e;
	border_step bs;
	
	//forall_face_edges (e,f)
	//	{
	//	e1= cyclic_adj_succ (e,f);
	//	v = node_between_adj_edges(e,e1);
	
	ext.clear();
	forall_face_border_steps (bs,f)
		{
		v   = stop_of_border_step (bs);
		e   = edge_of_border_step (bs);
		if (is_extremal(v,e)) 
			{
			pos = ext.append(v);
			if (is_big_angle(v,e))  is_big[pos] = true;
			else    		is_big[pos] = false;
			}
		}
		
	//cout << "\n   ANGLE LIST:";
	//pos = ext.first();
	//while (pos) { cout << " " << is_big[pos] << flush; pos = ext.succ(pos); }
	}
	
	
	
	gdt::gdtlist<gdtnode>
	upwa_plan_undi_graph::
big_angles
	(face f)
	{
	gdtnode v;
	gdtedge e,e1;
	border_step bs,bs1;
	gdt::gdtlist<gdtnode> big;
	
	//forall_face_edges (e,f)
	//	{
	//	e1 = cyclic_adj_succ (e,f);
	//	v  = node_between_adj_edges (e,e1);
	
	forall_face_border_steps (bs,f)
		{
		v   = stop_of_border_step (bs);
		bs1 = cyclic_succ_border_step (bs);
		e   = edge_of_border_step (bs);
		e1  = edge_of_border_step (bs1);
		if ((is_source(v) && e == first_out_cand_edge (v)) ||
		    (is_target(v) && e1== first_in_cand_edge (v))) big.append(v);
		}
	return big;
	}	
	
	
	int
	upwa_plan_undi_graph::
cost_of_last_algorithm
	() const
	{
	return last_cost;
	}



	bool
	upwa_plan_undi_graph::
face_is_regular
	(
	face f, 
	border_step& r1, 
	border_step& r2
	) 
	{
	//cout << "\nEXAMINING REGULARITY OF FACE " << id(f) << flush;
	
	r1 = NULL;	
	r2 = NULL;	
	
	// ------------------------------------------------------
	// build the list of border steps that form a large angle
	// (reflex)
	// ------------------------------------------------------
	
	//cout << "\nREFLEX LIST: " << flush;
	
	gdt::gdtlist<border_step>  reflex;		 // reflex border steps list
	border_step bs,bst;	
	gdt::list_item lbs, lbs1, lbst;
	gdt::gdtmap<border_step,gdt::list_item> pos_of_reflex (NULL); // first occurence of the border step in the reflex list
	
	// -----------------------------
	// create the list of all
	// reflex border steps of the 
	// face in the circular ordering
	// -----------------------------
	
	reflex.clear();
	forall_face_border_steps (bs,f)
		{
		angle_type a = angle_of_border_step(bs);
		if (a == _270 || a == _360)
			{
			pos_of_reflex[bs] = reflex.append(bs);
			//cout << " " << id(stop_of_border_step(bs)) << flush;
			}
		}
	
	if (reflex.size() > 1)
		{
		// -------------------------------------------------------
		// build a vector of delta distances between two 
		// consecutive reflex border steps. Distances are computed
		// starting with 0 at the first reflex, and counting +1 
		// on a large angle and -1 on a small angle.
		// Also, the the angle of the first reflex is included 
		// while the angle of the sencond reflex is excluded
		// -------------------------------------------------------
								
		int count = 0;
		gdt::gdtmap<gdt::list_item,int> delta(0);	// delta-distance from the previous reflex 
						// border step to the current reflex border step 
		lbs1 = reflex.first();
		lbs  = lbs1;
		bs   = reflex.inf(lbs);
		
		do
			{
			do 
				{
				angle_type a = angle_of_border_step (bs);
				switch (a)
					{	
					case _090: count ++;  break;
					case _270: 
					case _360: count --; break;
					default: break;
					}
				bs  = cyclic_succ_border_step(bs);
				}
			while (angle_of_border_step(bs) != _270 &&
			       angle_of_border_step(bs) != _360);
			
			lbs = pos_of_reflex[bs];
			delta[lbs] = count;
			//cout << "\ndelta[" << id(stop_of_border_step(bs)) << "] = " << delta[lbs]; 
			count = 0;
			}
		while (lbs != lbs1);
		
		
		// ------------------------------------------------------
		// build a vector that for each reflex gives its absolute
		// distance from the initial reflex; also build a second 
		// bucket vector, that informs if there exists a reflex 
		// with a given integer distance.
		// NOTE: the first reflex border step has distance
		//       +/-2 from itself
		// ------------------------------------------------------
		
		gdt::gdtmap<gdt::list_item,int> distance (0);		// distance from the initial reflex 
		gdt::gdtmap<int,gdt::list_item> lbs_with_distance (NULL);	// border step with distance int from the initial reflex
		
		count = 0;
		
		forall_items (lbs,reflex)
			if (lbs != lbs1)
				{
				count += delta[lbs];
				distance[lbs] = count;
				if (!lbs_with_distance[count]) lbs_with_distance[count] = lbs;
				//bs = reflex.inf(lbs); cout << "\ndistance[" << id(stop_of_border_step(bs)) << "] = " << distance[lbs]; 
				}
			
		count += delta[lbs1];
		distance[lbs1] = count;
		if (!lbs_with_distance[count]) lbs_with_distance[count] = lbs1;
		//bs1 = reflex.inf(lbs1); cout << "\ndistance[" << id(stop_of_border_step(bs1)) << "] = " << distance[lbs1] << flush; 
		
		// -----------------
		// CONSISTENCY CHECK
		// -----------------
		   {
		   int balance;
		   if (f == external_face()) balance = -2;
		   else			     balance = +2;
		   if (distance[lbs1] != balance)
			   gdt_error("balance internal error");
		   }
		
		// --------------------------------------------------------------
		// use the vectors above to detect a pair of kitty corners if any
		// --------------------------------------------------------------
		
		gdt::gdtmap<gdt::list_item,bool> visited (false); // previous reflex with respect to the current one
		int d1,d2;
		
		forall_items (lbs,reflex)
			if (lbs != lbs1)
				{
				//bs = reflex.inf(lbs); cout << "\nEXAMINING BORDER STEP ON REFLEX " << id(stop_of_border_step(bs)) << flush;
				d1 = distance[lbs];
				d2 = d1 - 1;
				// if (d2 == 0) lbst = lbs1;
				lbst = lbs_with_distance[d2];
				if (d2 == 0 && !visited[lbst]) lbst = lbs1;
								
				if ((lbst && visited[lbst]) || (lbst==lbs1 && d2==0))
					{
					// found a pair of kitty corners
					//
					bs  = reflex.inf(lbs);
					bst = reflex.inf(lbst);
					r1 = bs;
					r2 = bst;
					
					//gdtnode v1 = stop_of_border_step(bs) ;
					//gdtnode v2 = stop_of_border_step(bst);
					//cout << "\nIRREGULAR FACE " << id(f) << flush;
					//print (f); cout << flush;
					//cout << "\nFOUND PAIR OF KITTY CORNERS (" << id(v2) << "," << id(v1) << ")" << flush;
					
					break;
					}
				else if (f == external_face())
					{
					d2 = d1 + 3;
					// if (d2 == 0) lbst = lbs1;
					lbst = lbs_with_distance[d2];
					if (d2 == 0 && !visited[lbst]) lbst = lbs1;

					if ((lbst && visited[lbst]) || (lbst==lbs1 && d2==0))
						{
						// found a pair of kitty corners
						//
						bs  = reflex.inf(lbs);
						bst = reflex.inf(lbst);
						r1 = bs;
						r2 = bst;

						//gdtnode v1 = stop_of_border_step(bs) ;
						//gdtnode v2 = stop_of_border_step(bst);
						//cout << "\nIRREGULAR FACE " << id(f) << flush;
						//print (f); cout << flush;
						//cout << "\nFOUND PAIR OF KITTY CORNERS (" << id(v2) << "," << id(v1) << ")" << flush;

						break;
						}	
					}
				lbs_with_distance[d1] = lbs;
				visited[lbs] = true;	
				lbst = NULL;
				}
		
		lbs = lbs1;
		d1 = distance[lbs];	// already +/- 2
		d2 = d1 - 1;		// already +1 or -3
		lbst = lbs_with_distance[d2];
		
		if (lbst && visited[lbst])	// visited is already true (can be omitted)
			{
			bs  = reflex.inf(lbs);
			bst = reflex.inf(lbst);
			r1 = bs;
			r2 = bst;
			
			//gdtnode v1 = stop_of_border_step(bs) ;
			//gdtnode v2 = stop_of_border_step(bst);
			//cout << "\nIRREGULAR FACE " << id(f) << flush;
			//print (f); cout << flush;
			//cout << "\nFOUND PAIR OF KITTY CORNERS (" << id(v2) << "," << id(v1) << ")" << flush;
			}
		}
		
	if (r1) return false;
	else	return true;	
	}	



	angle_type
	upwa_plan_undi_graph::
angle_of_border_step
	(
	border_step bs 
	) 
	{
	angle_type alfa;
	gdtnode v = stop_of_border_step(bs);
	gdtedge e = edge_of_border_step(bs);
	if (is_internal(v,e)) alfa = _180; // flat angle
	else if (!is_big_angle(v,e)) alfa = _090; // small angle
	else if (degree(v) > 1) alfa = _270; // large angle and not bridge
	else alfa = _360; // large angle and bridge
	return alfa;
	}


	face
	upwa_plan_undi_graph::
find_external_face
	()
	{
	face f;
	forall_faces (f,*this)
		{
		int sa = 0,
		    ba = 0,
		    diff;
		border_step bs;
		forall_face_border_steps (bs,f)
			{
			angle_type alfa = angle_of_border_step(bs);
			if (alfa == _090) sa ++;
			else if (alfa == _270 || alfa == _360) ba++;
			}    
		    
		diff = sa-ba;
		if (diff==-2) return f;
		}
	return NULL_FACE;
	}





// -----------------
// Update operations
// -----------------
              	
              
 		
	void
	upwa_plan_undi_graph:: 
clear
	()
	{
	plan_undi_graph::clear();
	local_renew();
	}
	
	
	
	void
	upwa_plan_undi_graph::
steal_from 
	(undi_graph& ug, algorithm_type alg, bool err_mess) 
	{
	plan_undi_graph pug;
	pug.steal_from(ug);
	steal_from (pug,pug.first_face(),alg,err_mess);
	}
	


	void
	upwa_plan_undi_graph::
steal_from 
	(plan_undi_graph& pug, algorithm_type alg, bool err_mess) 
	{
	steal_from (pug,pug.first_face(),alg,err_mess);
	}
			
			
	void
	upwa_plan_undi_graph:: 
steal_from 
	(plan_undi_graph& pug, face ef, algorithm_type alg, bool err_mess)
	{
	//cout << "\n plan_undi_graph -(move)-> upwa_plan_undi_graph" << flush;
	clear();
	//
	// Copy all the pointers of ug
	// that is, the undi_graph part of this class
	//
	plan_undi_graph::mirror (pug);
	//
	// Build the extra part (i.e. the faces) of this,
	// according to the plan_undi part stolen from ug.
	// 
	local_renew();
	if (ef) ef = corresponding_face(ef,pug);
	else 	ef = first_face();
	local_init (ef,alg,err_mess);
		
	//
	// Create a new empty local_environment for pug
	// in order to avoid side-effects with this
	//
	pug.forget();
	}
	
	
	
	void 
	upwa_plan_undi_graph::
mirror 	
	(upwa_plan_undi_graph& upug)
	{
	face 				 p1;
	algorithm_type		 	 p2;
	gdt::gdtedge_map<bend_constraint>	 p3;
	gdt::gdtnode_map<struct_upwa_node_info>* p4;
	int				 p5;
	
	plan_undi_graph::mirror (upug);
	upug.local_get (p1,p2,p3,p4,p5);
	local_set (p1,p2,p3,p4,p5);
	}
	
	
	
	void 
	upwa_plan_undi_graph::
forget 	
	()
	{
	plan_undi_graph::forget();
	local_new ();
	}
	
	
	
	void
	upwa_plan_undi_graph:: 
set_external_face   
	(face ef)
	{
	ext_face = ef;
	}
	
	
	
        void
        upwa_plan_undi_graph::
set_layout_algorithm
	(algorithm_type alg)
	{
	layout_algorithm = alg;
	}
              	
              	
  	
  	void
  	upwa_plan_undi_graph::
update_constraint_internal_structures 
	()
	{
	gdtedge e;
	// set the edge_mapping of bend constraints
	//
	forall_edges (e,*this) set_bend_constraints (e,MINIMAL);	// initialization
	
	constraint c;
	constraint_number_of_bends_on_edge nbc;
	
	forall_constraints (c,*this)	
	       {
	       if (type_of_constraint(c) == NUMBER_OF_BENDS_ON_EDGE)
		       {
		       nbc = (constraint_number_of_bends_on_edge)c; // casting 
		       gdtedge 		e     = nbc -> get_edge_with_nbc();			
		       bend_constraint ct     = nbc -> get_nbc();
		       bend_constraint ct_old = get_constraint_on_bends_of_edge(e);

		       if (ct_old != MINIMAL && ct_old != ct) // collision case
		       		gdt_error("constraint collision on gdtedge");
		       else set_bend_constraints (e,ct);
		       }
		}		
	}
  	
  	
  	
	void
	upwa_plan_undi_graph::
set_bend_constraints
	(gdtedge e, bend_constraint bc)
	{
	constraint_on_bends_of_edge[e] = bc;
	}



	void
	upwa_plan_undi_graph::
set_bend_constraints
	(gdt::gdtedge_array<bend_constraint> bc)
	{
	gdtedge e;
	forall_edges (e,*this) set_bend_constraints (e,bc[e]);
	}	
	
	
	gdt::gdtlist<gdtedge>
	upwa_plan_undi_graph::
make_st_with_regularity
	(gdtnode& s, gdtnode& t)
	{
	// -------------------------------
	// TEMPORARY CHECK (TO BE REMOVED)
	// -------------------------------
	/*
		{
		print(BY_FACES);
		face f;
		forall_faces (f,*this)
			{
			border_step r1,r2;
			face_is_regular(f,r1,r2);
			}	
		}
	*/
	// -------------------------------
	gdt::gdtlist<gdtedge> dum;			// total dummy edges added
	regularize_all_faces(dum);
	gdt::gdtlist<gdtedge> E = make_st (s,t);
	dum.conc(E);
	return dum;
	}
	
	
	gdt::gdtlist<gdtedge>
	upwa_plan_undi_graph::
make_st
	(gdtnode& s, gdtnode& t)
	{
	gdtnode v,v1,v2;
	gdtedge e,e1,e2;
	border_step bs,bs1;
	gdt::gdtlist<gdtedge> dum;			// total dummy edges added
	gdt::gdtlist<gdtnode> sources;		// all sources on the external face
	gdt::gdtlist<gdtnode> targets;		// all targets on the external face
	face ef; 			// external face
	
	// ---------------------- 1 ---------------------------
	// decompose all the faces of the upward representation
	// and  store  the remaining sources and targets on the 
	// external face into the list sources and targets
	// ----------------------------------------------------
	
	dum = decompose();
	
	//cout << "\nDECOMPOSED GRAPH: " << flush;
	//print (COMPLETE);
	
	//cout << "\n\nSTART CREATION OF DUMMY s AND t\n" << flush;
	
	ef  = external_face();
	bs1 = first_border_step (ef);
	v1  = stop_of_border_step(bs1);
	e1  = edge_of_border_step(bs1);
	
	while (is_internal(v1) || !is_big_angle(v1,e1))
		{
		bs1 = cyclic_succ_border_step (bs1);
		v1  = stop_of_border_step(bs1);
		e1  = edge_of_border_step(bs1);
		}
				  
	
	if (is_source(v1)) 	   
				   {
				   //cout << "\ntype SOURCE" << flush;
				   do	// look for the first target big-angle
					{
					bs1 = cyclic_succ_border_step (bs1);
					v1  = stop_of_border_step(bs1);
					e1  = edge_of_border_step(bs1);
					}
				   while (!(is_target(v1) && is_big_angle(v1,e1)));
				   //cout << "\nfound first target " << id(v1) << flush;
				   }
	else 
		if (is_target(v1))
				   {
				   //cout << "\ntype TARGET" << flush;		 
		    		   do	// look for the first source big_angle
			   		{
					bs1 = cyclic_succ_border_step (bs1);
					v1  = stop_of_border_step(bs1);
					e1  = edge_of_border_step(bs1);
			   		}
			   	   while (!(is_source(v1) && is_big_angle(v1,e1)));
			   	   //cout << "\nfound first source " << id(v1) << flush;	
			   	   }
	
	v  = v1;
	bs = bs1;
	
	do
		{
		e = edge_of_border_step(bs);
		//cout << "\nEXAMINING EDGE " << id(e) << " AND NODE " << id(v) << flush;
		if (is_big_angle(v,e))
			if (is_source(v)) sources.append(v);
			else		  targets.append(v);  
		bs = cyclic_succ_border_step(bs);
		v  = stop_of_border_step(bs);
		}
	while (bs != bs1);
	
	/*--------------------------------------------------
			
		        t3         t2     t1	    
		O-------O----------O------O---------O
	       /                                     \
	      /                                       \
	     /                                         \
	    O   EXTREMALS POS. AFTER DECOMPOSITION      O
	     \                                         /
	      \                                       /
	       \                                     /
	        O-------O----------O------O---------O
	               s1=v  -s->  s2     s3 
	    
	-----------------------------------------------------*/    
	
	//cout << "\nGRAPH: " << flush;
	//print(COMPLETE,true);
	
	//cout << "\n  - sources: " << flush;
	//forall (v,sources) cout << id(v) << " " << flush;
	//cout << "\n  - targets: " << flush;
	//forall (v,targets) cout << id(v) << " " << flush;
	
	// ---------------------- 2 ---------------------------
	// add a first dummy gdtedge between a source and a target 
	// of the external  face  and  then split it adding the 
	// dummy source s and target t
	// ----------------------------------------------------
	
	v1 = sources.head();	// first source s1
	v2 = targets.tail();	// last  target t3
		
	e  = new_edge (v2,v1,ef);
	ef = right_face_moving_along_edge (e,v2);
	
	s  = new_node (e); 
	mark(s,RM3_ADDED_BY_MAKE_ST);
	e  = first_adj_edge(s);
	if (opposite(s,e) == v1) e = last_adj_edge(s);
	t  = new_node (e);
	mark(t,RM3_ADDED_BY_MAKE_ST);
	e  = first_adj_edge(t);
	if (opposite(t,e) == v2) e = last_adj_edge(t);
	e1 = cyclic_adj_succ(e,s);
	e2 = cyclic_adj_succ(e,t);
	reverse(e);
	dum.append(e);
	dum.append(e1);
	dum.append(e2);
	
	//cout << "\n  - Added gdtedge: s -"  << id(e1) << "->" << id(v1) << flush;
	//cout << "\n  - Added gdtedge: t <-" << id(e2) << "-"  << id(v2) << flush;
	 
	//cout << "\n  -new external face: " << flush;
	//print(ef); cout << "\n" << flush;
	
	// ---------------------- 3 ---------------------------
	// connect all the remaining sources with s and all the
	// remaining targets to t.
	// ----------------------------------------------------
	
	forall (v,sources)
		if (v != v1) 
			{
			//cout << "\n  - examining gdtnode " << id(v) << flush;
			e  = new_edge (s,v,ef);
			ef = right_face_moving_along_edge(e,s);
			dum.append(e);
			//cout << "\n  - Added gdtedge: s -"  << id(e) << "->" << id(v) << flush;
			}
		
	gdt::gdtlist<gdtnode> rev_targets;
	forall (v,targets) rev_targets.push(v); 		
	forall (v,rev_targets)
		if (v != v2)
			{
			//cout << "\n  - examining gdtnode" << id(v) << flush;
			e  = new_edge (v,t,ef);
			ef = left_face_moving_along_edge(e,t);
			dum.append(e);
			//cout << "\n  - Added gdtedge: t <-" << id(e) << "-"  << id(v) << flush;
			}
	//cout << "\n\nST COMPLETED";
	return dum;
	}
	
	
	
	int
	upwa_plan_undi_graph::
apply_layout_algorithm
	(bool err_mess)
	{
	int cost = 0;
	update_constraint_internal_structures();
	switch (layout_algorithm)
			{
			case PLAN_UPWARD_OPTIMAL : cost = optimal_upward_for_fixed_embedding_with_constraints(); break;
			case PLAN_UPWARD_SLOW    : cost = slow_upward (); break;
			default: break;	
			}
	if ((cost == INFINITE) && (err_mess)) 
		gdt_error("no flow feasibility");
		
	last_cost = cost;
	return cost; 
	}
	

	bool 
	upwa_plan_undi_graph::
regularize_face
	(
	face f, 
	gdt::gdtlist<gdtedge>& dummy_edges
	)
	{
	//cout << "\nREGULARIZING FACE " << id(f) << flush;
	bool irregular = false;
	
	//gdtnode /*v,*/v1,v2;
	gdtnode v1,v2;
	gdtedge e,e1,e2;
	gdt::gdtstack<face> Q;	// faces to be regularized
	
	Q.push(f);
		
	while (!Q.empty())
		{
		border_step r1;
		border_step r2;
		f = Q.pop();
		if (!face_is_regular (f,r1,r2))
			{
			irregular = true;
			bool update_external = false;
			if (external_face() == f) update_external = true;
			
			// ------------- General Strategy --------------
			// decompose f into smaller faces f1,f2, ... ,fk
			// Q.push(f1),...,Q.push(fk)
			// ---------------------------------------------
			
			v1 = stop_of_border_step (r1);
			v2 = stop_of_border_step (r2);
			
			e1 = edge_of_border_step (r1);			
			e2 = edge_of_border_step (r2);
			
			gdtnode vs = NULL_NODE, // the source switch (either v1 or v2)
			        vt = NULL_NODE; // the sink swicth (either v1 or v2)
			
			gdtedge es = NULL_EDGE, 
			        et = NULL_EDGE;
			
			if (stop(e1) == v1 && start(e2) == v2) {vt = v1; vs = v2; et = e1; es = e2;}
			else if (start(e1) == v1 && stop(e2) == v2) {vt = v2; vs = v1; et = e2; es = e1;}
			else gdt_error("not valid pair of source and sink switches");			
			
			// add a new gdtedge directed from vt to vs
			//
			e = new_edge (vt,et,vs,es);
			make_directed (e,vt);
			dummy_edges.append(e);
			
			set_out_cand_edges_are_ordered (vs,true);
			set_in_cand_edges_are_ordered  (vt,true);
			//
			
			face f1 = right_face_moving_along_edge (e,vs);
			face f2 = right_face_moving_along_edge (e,vt);
			
			Q.push(f1);
			Q.push(f2);
			
			//cout << "\nADDED EDGE BETWEEN " << id(v1) << " AND " << id(v2) << flush;
			//cout << "\nFACES OBTAINED : " << flush;
			//print(f1); cout << "\n" << flush;
			//print(f2); cout << "\n" << flush;
			//cout << "\nUPWARD OBTAINED : " << flush;
			//print(COMPLETE); cout << flush;
			
			if (update_external)
				{
				face ef = find_external_face();
				if (ef) ext_face = ef;
				else gdt_error("external face not found");
				}		
			
			}
		}
			
	forall (e,dummy_edges) mark (e,RM3_ADDED_BY_REGULARIZATION);
	return irregular;	
	}


	int
	upwa_plan_undi_graph::
regularize_all_faces
	(gdt::gdtlist<gdtedge>& dummy_edges)
	{
	int num_irr_faces = 0;
	face f;
	gdt::gdtlist<face> faces = all_faces();
	forall (f,faces) 
		if (regularize_face (f, dummy_edges)) num_irr_faces++;	
	
	//cout << "\nUPWARD AFTER REGULARIZATION : " << flush;
	//print(COMPLETE); cout << flush;
	
	// CONSITENCY CHECK 
	// ----------------
	//
	/*
	if (!is_upward())
		gdt_error("no upward obtained");
	*/
	return num_irr_faces;
	}


	
// -------------------------
// Input / output operations
// -------------------------
		
	void
	upwa_plan_undi_graph:: 
print 
	(print_mode mode, bool candidate, ostream& os) 
	{
	os << "\n---- PRINT PLAN UNDI GRAPH TOPOLOGY ----\n" << flush;
	plan_undi_graph::print (mode,os);
	if (candidate)
		{
		gdtnode v;
		os << "\n---- PRINT CANDIDATE EDGE LISTS ----\n" << flush;
		forall_nodes (v,*this) print(v,true);
		}
	}
	
	
	
	void
	upwa_plan_undi_graph::
print 
	(gdtnode v, bool candidate, ostream& os)
	{
	if (!candidate) plan_undi_graph::print (v,os);
	else
		{
		gdtedge e;
		os << "\nin  (" << id(v) << "): " << flush;
		forall_candidate_edges_entering_node (e,v,*this)
			os << "<-" << id(e) << "- (" << id(opposite(v,e)) << ")  " << flush;  
		os << "\nout (" << id(v) << "): " << flush;
		forall_candidate_edges_leaving_node (e,v,*this)
			os << "-" << id(e) << "-> (" << id(opposite(v,e)) << ")  " << flush;  
		}	
	}
	
	
	
	void
	upwa_plan_undi_graph::
print 
	(gdtedge e, ostream& os) const
	{
	plan_undi_graph::print (e,os);
	}	



	void
	upwa_plan_undi_graph::
print 
	(face f, ostream& os) const
	{
	plan_undi_graph::print (f,os);
	}	



	void
	upwa_plan_undi_graph::
print 
	(separation_pair sp, ostream& os) const
	{
	plan_undi_graph::print (sp,os);
	}	

	
	
	void 
	upwa_plan_undi_graph::
print 
	(constraint c, ostream& os) const	
	{
	plan_undi_graph::print (c,os);
	}

	
	
//-----------------------------------------------------------------------------
//	PRIVATE
//-----------------------------------------------------------------------------
		
		
		
	void
	upwa_plan_undi_graph:: 
local_new()   
	{
	//cout << "\n upwa_plan_undi_graph::local_new" << flush;
	node_info =  new (gdt::gdtnode_map<struct_upwa_node_info>);
	node_info -> init(nodes_and_edges());
	constraint_on_bends_of_edge.init (nodes_and_edges(),MINIMAL);
	}
	
	
	
	void 
	upwa_plan_undi_graph::
local_del()
	{
	//cout << "\n upwa_plan_undi_graph::local_del" << flush;
	safe_delete (node_info);
	}
	
	
	
	void 
	upwa_plan_undi_graph::
local_renew() 
	{
	local_del();
	local_new();
	}
	
	
	
	void
	upwa_plan_undi_graph:: 
local_init 
	(face ef , algorithm_type alg, bool err_mess)
	{
	// check if all edges are directed
	//
	if (!all_edges_are_directed()) 
		gdt_error("graph is not directed");
	gdtnode v;

	set_external_face    (ef);
	set_layout_algorithm (alg);
			
	forall_nodes (v,*this)
		{
		set_first_in_cand  	      (v,first_in_edge (v));
		set_in_cand_edges_are_ordered (v,false);
		set_first_out_cand 	      (v,first_out_edge(v));
		set_out_cand_edges_are_ordered(v,false);
		}
	apply_layout_algorithm(err_mess);
	}
	
	
	
	void 
	upwa_plan_undi_graph::
local_set 
	(
	face p1, 
	algorithm_type p2, 
	gdt::gdtedge_map<bend_constraint> p3, 
	gdt::gdtnode_map<struct_upwa_node_info>* p4,
	int p5
	)
	{
	ext_face	 	    = p1;
	layout_algorithm 	    = p2; 
	constraint_on_bends_of_edge = p3;
	node_info 	 	    = p4;
	last_cost		    = p5;
	}	
		
		
			
	void 
	upwa_plan_undi_graph::
set_first_in_cand   
	(gdtnode v,gdtedge e)
	{
	(*node_info)[v].first_in_cand = e;
	}
	
	
	
	void 
	upwa_plan_undi_graph::
set_first_out_cand   
	(gdtnode v,gdtedge e)
	{
	(*node_info)[v].first_out_cand = e;
	}
	
	
	
	void 
	upwa_plan_undi_graph::
set_in_cand_edges_are_ordered  
	(gdtnode v, bool flag)
	{
	(*node_info)[v].in_cand_edges_are_ordered = flag;
	}
	
	

	void 
	upwa_plan_undi_graph::
set_out_cand_edges_are_ordered  
	(gdtnode v, bool flag)
	{
	(*node_info)[v].out_cand_edges_are_ordered = flag;
	}
	


	void
	upwa_plan_undi_graph:: 
order_in_cand  
	(gdtnode v)
	{
	if (!get_in_cand_edges_are_ordered(v))
		{
		if (!is_candidate(v)) 
			gdt_error("gdtnode is not candidate");
	
		if (is_internal(v)) 
				// is internal
				{
				gdtedge e = first_out_edge(v);
				while (stop(e) != v) e = cyclic_adj_succ (e,v);
				set_first_in_cand (v,e);
				}
		else if (is_target(v) && !get_first_in_cand(v)) set_first_in_cand (v,first_in_edge(v));
				
		set_in_cand_edges_are_ordered (v,true);
		}
	}	
	
	
	
	void
	upwa_plan_undi_graph:: 
order_out_cand  
	(gdtnode v)
	{
	if (!get_out_cand_edges_are_ordered(v))
		{
		if (!is_candidate(v)) 
			gdt_error("gdtnode is not candidate");
	
		if (is_internal(v)) 
			{
			gdtedge e = first_in_edge(v);
			while (start(e) != v) e = cyclic_adj_pred (e,v);
			set_first_out_cand (v,e);
			}
		else if (is_source(v) && !get_first_out_cand(v)) set_first_out_cand (v,first_out_edge(v));
			
		set_out_cand_edges_are_ordered (v,true);
		}
	}	
	
		
		
	void 
	upwa_plan_undi_graph::
order_cand	    
	(gdtnode v)
	{
	order_in_cand  (v);
	order_out_cand (v);
	}
	
	
	
	void
	upwa_plan_undi_graph::
order_cand
	()
	{
	gdtnode v;
	forall_nodes (v,*this) order_cand (v);
	}	
		
	
	
	gdt::list_item
	upwa_plan_undi_graph::
find_BSS
	(gdt::gdtlist<gdtnode>& ext, gdt::gdtmap<gdt::list_item,bool>& is_big)
	{
	gdt::list_item init_pos = ext.first();
	gdt::list_item pos1,pos2,pos3;
	//gdtnode	  v1,v2,v3;
	
	pos1 = init_pos;
	pos2 = ext.cyclic_succ(pos1);
	pos3 = ext.cyclic_succ(pos2);
	
	do
		{
		//v1 = ext.inf(pos1);
		//v2 = ext.inf(pos2);
		//v3 = ext.inf(pos3);
		//if (is_big[v1] && !is_big[v2] && !is_big[v3]) return pos1;
		if (is_big[pos1] && !is_big[pos2] && !is_big[pos3]) return pos1;
		pos1 = pos2;
		pos2 = pos3;
		pos3 = ext.cyclic_succ(pos3);	
		}
	while (pos1 != init_pos);
	
	return NULL;
	}


// ------------------------------------------------------------------
// These declarations are here becouse Borland C++ cannot instantiate 
// templates over locally defined types.
// ------------------------------------------------------------------
	typedef enum
	{
	NODE = 0,
	FACE = 1,
	NIL  = 2
	}
fnode_type;

	struct
struct_fnode_info  
	{
	fnode_type type;
	gdtnode	   node_corr;
	face	   face_corr;
	void set (fnode_type o, gdtnode n, face f)
		{
		type      = o;
		node_corr = n;
		face_corr = f;
		}
	};

// ------------------------------------------------------------------


	int
	upwa_plan_undi_graph::  
make_upward_with_constraints 
	()
	{
	//cout << "\nSTART MAKE UPWARD REPRESENTATION\n" << flush;
	
	// ------------------------------------------------------------
	// test if the embedding of the graph is candidate 
	// ------------------------------------------------------------
	
	if (!is_candidate())
  		gdt_error("no candidate embedding");
	
	
	// ---------------------------- 0 ------------------------------
	// build a bend-direction array that means a spirality direction
	// for each gdtedge (from left-face to right-face of gdtedge starting 
	// from gdtnode). This array is builded according to the set of 
	// constraints on this graph
	// -------------------------------------------------------------
	
	gdt::gdtedge_map<gdtnode> bend_dir_node (nodes_and_edges(),NULL_NODE);	// bend-direction (spirality)
	constraint c;
	constraint_bend_direction_on_edge bdc; 
	forall_constraints (c,*this)
		if (type_of_constraint(c) == BEND_DIRECTION_ON_EDGE)
			{
			bdc = (constraint_bend_direction_on_edge)c; // casting 
			gdtedge e      = bdc -> get_edge_with_bdc();			
			gdtnode sn     = bdc -> get_start_node();
			gdtnode sn_old = bend_dir_node[e];

			if (sn_old != NULL_NODE && sn_old != sn) // collision case
				 gdt_error("constraint collision on gdtedge");
			else bend_dir_node[e] = sn;				 
			}
	
	// --------------------------- 1 ------------------------------
 	// build the flow_network fn associated to the planar embedding
 	// of this, and external face ef. 
 	// ------------------------------------------------------------ 
 	
	gdtedge e;	
 	gdtnode v;
	face f,g;
	border_step s;
 	
 	gdtnode vfn;     // flow_net nodes corresponding to nodes v and w
 	gdtnode ffn,gfn; // flow_net nodes corresponding to faces f and g 
	gdtedge efn;     // flow_net gdtedge
	
	int total_cost = INFINITE;
	
 	flow_dire_graph fn;  // flow-network
	
	// --------------------------------------------------
	// links "graph nodes or faces" ---> "flow-net nodes"
	// --------------------------------------------------  
	//
 	gdt::gdtnode_array<gdtnode> fnode_corr_to_node (nodes_and_edges(), NULL_NODE);
 	gdt::gdtmap<face,gdtnode>   fnode_corr_to_face(NULL_NODE);
 	
 	// --------------------------------------------------
 	// links "flow-net nodes" ---> "graph nodes or faces"
 	// --------------------------------------------------
 	 	
 	gdt::gdtnode_map<struct_fnode_info> fnode_info;
 	fnode_info.init (fn.nodes_and_edges());
 	
 	
 	// --------------------- 1.1 ---------------------
 	// Create the nodes of the flow_net having nonzero 
 	// supply or demand 
 	// -----------------------------------------------
 	
  	forall_nodes (v,*this)
  		if (is_extremal(v))
 			{
 			vfn = fn.new_node();
 			fnode_info[vfn].set (NODE, v, NULL_FACE);
 			fnode_corr_to_node[v] = vfn;
 			fn.set_balance(vfn,1);
 			}

 	forall_faces (f,*this)
 		{
 	 	ffn = fn.new_node();
  		fnode_info[ffn].set (FACE, NULL_NODE, f);
  		fnode_corr_to_face[f] = ffn;
  		int cap = capacity(f);	
 		fn.set_balance(ffn,-cap);
  		} 
 		
 		
  	// ---------------------- 1.2 --------------------
 	// Create flow_net edges (vfn->ffn) and (ffn->gfn) 
 	// -----------------------------------------------
  	
  	forall_faces (f,*this)
 		{
 		gdt::gdtnode_array<bool> mark_node (nodes_and_edges(),false);
 		gdt::gdtmap<face,bool>   mark_face (false);
 		
 		ffn = fnode_corr_to_face[f];
 		
		forall_face_border_steps (s,f)
			{
			e = edge_of_border_step(s);
			v = start_of_border_step(s);
			if (!mark_node[v] && is_extremal(v))
				{
				vfn = fnode_corr_to_node[v];
				efn = fn.new_edge(vfn,ffn);
				fn.set_edge_info(efn,INFINITE,0,0,0);	// modify in order to handle angle-contraints
				mark_node[v] = true;
				}
			g = left_face_moving_along_edge (e,v);
			gfn = fnode_corr_to_face[g];
			if (
			   (constraint_on_bends_of_edge[e] != NONE) &&
			   (bend_dir_node[e] == NULL_NODE || bend_dir_node[e] == v)
			   )
				{
				if (!mark_face[g]) 
					{
					efn = fn.new_edge(ffn,gfn);
					if (constraint_on_bends_of_edge[e] == ANY)
						fn.set_edge_info (efn,INFINITE,0,0,0);
					else	fn.set_edge_info (efn,INFINITE,0,2,0);
					mark_face[g] = true;
					}
				else 
					{
					efn = fn.find_directed_edge (ffn,gfn);
					if ((constraint_on_bends_of_edge[e] == ANY) && (fn.get_cost(efn) > 0))
						fn.set_edge_info (efn,INFINITE,0,0,0);
					}
				}
			}
 		} 
 	
 	 // ------------------------------------------------------------
 	 //      PRINT THE FLOW_NET fn (THIS PART IS TEMPORARY)
 	 // ------------------------------------------------------------ 	 

   	 // int i; 
 	 // cout << "\nFLOW NETWORK : " << "\n" << flush;
 	 
 	 // gdt::gdtnode_array<string> flabel(fn.nodes_and_edges());
 	 // char temp[255];
	  
	 // forall_nodes(vfn,fn)
	 // 	{
	 // 	struct_fnode_info& fni = fnode_info[vfn];
	 //	if (fni.type == NODE) sprintf(temp, "v%d", id(fni.node_corr));
	 //	else
	 //		if (fni.type == FACE) sprintf(temp, "f%d", id(fni.face_corr));
	 //		else                  sprintf(temp, "x");
	 // 	flabel[vfn] = temp;
	 // 	}
	 // forall_nodes(vfn,fn)
	 // 	{
	 // 	int b  = fn.get_balance(vfn);
	 // 	cout << "\n" << flabel[vfn] << "[" << fn.id(vfn) << "] " << "<" << b << ">:";
	 // 	forall_edges_leaving_node(efn,vfn,fn)
	 // 		{
	 // 		int uc = fn.get_upper_capacity(efn);
	 // 		int lc = fn.get_lower_capacity(efn);
	 // 		int c  = fn.get_cost(efn);
	 // 		cout << flabel[fn.opposite(vfn,efn)] << "[" << lc << "," << uc << "," << c << "] ";
	 // 		}
	 // 	}
	 // 
	 //cin >> i;
 		  	
	  	
  	// --------------------------- 2 ------------------------------
 	// find a minimum cost flow of maximum value in fn.
	// ------------------------------------------------------------
	
	if (!fn.min_cost_flow ()) return total_cost;
		//gdt_error("no flow feasibility");
		
	total_cost = fn.flow_cost();
	
	// ------------------------------------------------------------
	// PRINT THE FLOW ALONG ALL EDGES (THIS PART IS TEMPORARY)
	// ------------------------------------------------------------
	 
	// forall_edges(efn,fn)
	// 	{
	// 	vfn = fn.start(efn);
	// 	wfn = fn.stop (efn);
	// 	cout << "\nx[" << flabel[vfn] << "," << flabel[wfn] << "] = " << fn.get_flow(efn);
	// 	}	
	// cout << "\n";
	 
	// ---------------------------- 3 -------------------------------
	// create the upward representation using the flow on the network
	// --------------------------------------------------------------
	
	forall_edges (efn,fn)
		{
		gdtnode       v1fn    = fn.start(efn);
		gdtnode       v2fn    = fn.stop(efn);
		int        x_v1_v2 = fn.get_flow(efn);
		fnode_type v1_type = fnode_info[v1fn].type;
		fnode_type v2_type = fnode_info[v2fn].type;
		
		if (x_v1_v2)
			{
			// ----------------------
			// case of gdtedge (v --> f)
			// ----------------------
			//
			if (v1_type == NODE && v2_type == FACE)
				{
				v = fnode_info[v1fn].node_corr;
				f = fnode_info[v2fn].face_corr;
				make_big_angle (v,f);
				//cout << "\nmade big angle on gdtnode " << id(v) << " into the face " << id(f) << flush;
				}
			
			// ----------------------
			// case of gdtedge (f --> g)
			// ----------------------
			//
			else if (v1_type == FACE && v2_type == FACE)
				{
				f = fnode_info[v1fn].face_corr;
				g = fnode_info[v2fn].face_corr;
				gdt::gdtlist<gdtedge> Rfg = edges_shared_by_faces(f,g);
				gdt::list_item it;
				
			        // remove from Rfg all rigid edges in this direction
				//
				forall_items (it,Rfg)
				     	{
				     	e = Rfg.inf(it);  
				     	//if (is_marked(e,RM3_ADDED_BY_MAKE_UPWARD)) Rfg.del_item(it);
				     	//else 
				     	v = bend_dir_node[e];
				     	if (
				     	   (constraint_on_bends_of_edge[e] == NONE) ||
				     	   (v != NULL_NODE && right_face_moving_along_edge(e,v) != f)
				     	   ) Rfg.del_item(it);
				     	}	
				
				if (!fn.get_cost(efn))
					{ 
					// 
					// choose an gdtedge having ANY bend constraint
					//
					forall (e,Rfg) if (constraint_on_bends_of_edge[e] == ANY) break;
					}
					
				else
					{
					//
					// choose an gdtedge having MINIMAL bend constraint
					//
					forall (e,Rfg) if (constraint_on_bends_of_edge[e] == MINIMAL) break;
					}
				
				//cout << "\nSplitting gdtedge " << id(e) << " ..." << flush;
				int i; 
				for (i=1; i<=x_v1_v2; i++)
					{
					// create two new dummy nodes (bends)
					//
					gdtnode old_node_start = bend_dir_node[e];
					int old_ide = id(e);
					gdtnode w1 = new_node (e);
					e = first_adj_edge(w1);
					assign_id(last_adj_edge(w1),old_ide);
					gdtnode w2 = new_node (e);
					
					// reverse the dummy gdtedge to make w1 and w2 as extremals 
					//
					e = first_adj_edge(w1);
					if (opposite(w1,e) != w2) e = last_adj_edge(w1);
					reverse (e);
					//cout << "..Added (" << id(w1) << ")-" << id(e) << "-(" << id(w2) << ")" << flush;          
					
					gdtedge e1 = cyclic_adj_succ(e,w1);
					gdtedge e2 = cyclic_adj_succ(e,w2);
					gdtnode u1 = opposite(w1,e1);
					gdtnode u2 = opposite(w2,e2);
					if (old_node_start == NULL_NODE) 
						{
						bend_dir_node[e1] = NULL_NODE;
						bend_dir_node[e]  = NULL_NODE;
						bend_dir_node[e2] = NULL_NODE;
						}
					else if (old_node_start == u1)
						{
						bend_dir_node[e1] = u1;
						bend_dir_node[e]  = w1;
						bend_dir_node[e2] = w2; 
						}
					else if (old_node_start == u2)
						{
						bend_dir_node[e1] = w1;
						bend_dir_node[e]  = w2;
						bend_dir_node[e2] = u2; 
						}
					else
						gdt_error("bend_dir_node inconsistency");
					
					// mark w1,w2 and e as dummy objects
					//
					mark (w1,RM3_ADDED_BY_MAKE_UPWARD);
					mark (w2,RM3_ADDED_BY_MAKE_UPWARD);
					mark (e ,RM3_ADDED_BY_MAKE_UPWARD);
				
					// make a big angle on w1 and w2 into the face g;
					//
					make_big_angle (w1,g);
					make_big_angle (w2,g); 
					}
				}
			}
		
		}
	
	
	// --------- 4 ---------
	// return the total-cost
	// ---------------------
		
	return total_cost;		
	}	
	
	
	
	
	int
	upwa_plan_undi_graph:: 
optimal_upward_for_fixed_embedding_with_constraints
	()
	{
	#ifdef BOUNDEDNODES	
	 if (count_not_dummy_nodes() > BOUND_ON_NODES)
	 	{
	 	cout << "\nTHIS IS A DEMO VERSION OF GDT."
	      	     << "\nYOU CANNOT RUN A DRAWING ALGORITHM ON GRAPHS WITH MORE THAN "
	             << BOUND_ON_NODES << " NODES."  
		     << "\nTO DO THAT YOU NEED TO GET A COMMERCIAL LICENSE" << flush;
         	exit(1);
		}
	#endif
	return make_upward_with_constraints ();
	}
	
	
	
	int
	upwa_plan_undi_graph::
slow_upward
	()
	{
	#ifdef BOUNDEDNODES	
	 if (count_not_dummy_nodes() > BOUND_ON_NODES)
	 	{
	 	cout << "\nTHIS IS A DEMO VERSION OF GDT."
	      	     << "\nYOU CANNOT RUN A DRAWING ALGORITHM ON GRAPHS WITH MORE THAN "
	             << BOUND_ON_NODES << " NODES." 
		     << "\nTO DO THAT YOU NEED TO GET A COMMERCIAL LICENSE" << flush;
		     
		print (COMPLETE);
		int i; cin >> i;     
         	exit(1);
		}
	#endif	
	
	int cost;
	BB_options Op = STANDARD_BB_OPTIONS;

	// ----------------------- 1 --------------------------
	// make ug as an undi_graph copy of *this, in order to 
	// expand it and look for a candidate planar embedding.
	// The expansion is important to generate candidate
	// planar embeddings only.
	// ----------------------------------------------------
	
	//cout << "\nSTEP 1 STARTED: expanding and looking for candidate planar embedding..." << flush;
	
	undi_graph ug (*this);
	ug.make_embedding_cand_expanded();
	if (!ug.make_embedding_planar ())
		gdt_error("no found a candidate planar embedding");
	
	// NEW CODE
	// -------------------------------------------
	// new bend-constraints are added on the dummy 
	// edges introduced by the gdtnode expansions
	// -------------------------------------------
		{
		gdtedge e;
		forall_edges (e,ug)
			if (ug.is_marked(e,RM3_ADDED_BY_EXPAND)) 
				ug.new_constraint_number_of_bends_on_edge (e,NONE);
		}
	// ------------------------------------------------------------------------
	
	//cout << "\nSTEP 1 END: ok!" << flush;
	
	// ----------------------- 2 --------------------------
	// search the best planar embedding for ug, in order to
	// reduce the cost returned by the algorithm 
	// optimal_upward_for_fixed_embedding, and assign it to
	// graph ug. Finally ug is contracted.
	// ----------------------------------------------------
	
	//cout << "\nSTEP 2 STARTED: searching the best planar embedding..." << flush;
	//cout << "\n  STEP 2.1 STARTED: decomposing..." << flush;
	
	plan_undi_graph pug(ug);
	SPQR_tree T (pug,NULL_EDGE,true);	// all constraints will be automatically 
						// updated on all skeletons of T
	
	//cout << "\n  STEP 2.1 END: ok!" << flush;
	//cout << "\n  STEP 2.2 STARTED: branch and bound computing..." << flush;
	
	plan_undi_graph bpe;	// best planar embedding
	face ef;		// external face 	
		
	if (T.find_best_embedding_with_external_face (bpe, ef, Op, PLAN_UPWARD_SLOW) == INFINITE) return INFINITE;

	//cout << "\n  STEP 2.2 END: ok!" << flush;	
		
	gdtedge e;
	gdtnode v;
	int re_id;         // reference gdtedge id to refind the external face after contraction
	int rv_id;         // reference vertex id to refind the external face after contraction
	bool right = true; // reference direction
	gdtedge e_pug = NULL_EDGE;
	gdtnode v_pug = NULL_NODE;
	
	forall_face_edges (e,ef) 
		{
		e_pug = pug.corresponding_edge (e,bpe);
		if (!pug.is_marked(e_pug,RM3_ADDED_BY_EXPAND)) 
			{
			gdtnode v1 = pug.source(e_pug);
			gdtnode v2 = pug.target(e_pug);
			if      (!pug.is_marked(v1,RM3_ADDED_BY_EXPAND)) {v_pug = v1; break;}
			else if (!pug.is_marked(v2,RM3_ADDED_BY_EXPAND)) {v_pug = v2; break;}
			}
		}
	
	re_id = bpe.id(e);
	v = bpe.corresponding_node (v_pug,pug);
	rv_id = bpe.id(v);
	
	if      (bpe.right_face_moving_along_edge (e,v) == ef) right = true;
	else if (bpe.left_face_moving_along_edge  (e,v) == ef) right = false;						   
	else gdt_error("invalid gdtedge or gdtnode for face");
	
	//cout << "\n  STEP 2.3 STARTED: contracting..." << flush;
	
	ug.make_embedding_as (bpe);
	ug.contract();
	
	//cout << "\n  STEP 2.3 END: ok!" << flush;
	//cout << "\nSTEP 2 END: ok!" << flush;
	
	// ----------------------- 3 ----------------------------
	// make for *this the same embedding of ug, and compute
	// on it the optimal_upward_for_fixed_embedding algorithm 
	// ------------------------------------------------------
	
	//cout << "\nSTEP 3 STARTED: applying layout algorithm..." << flush;
	
	make_embedding_as (ug);
	gdtedge re = find_edge (re_id);
	gdtnode rv = find_node (rv_id);
	if (right) ef = right_face_moving_along_edge (re,rv);
	else 	   ef = left_face_moving_along_edge  (re,rv);
	set_external_face (ef);
	
	cost = optimal_upward_for_fixed_embedding_with_constraints ();
	//cost = optimal_upward_for_fixed_embedding ();
	
	//cout << "\nSTEP 3 END: ok!" << flush;
	return cost;
	} 
	
	
        gdtnode
        upwa_plan_undi_graph::
new_node
	(gdtedge e, int new_id)
	{
	gdtnode v1 = start(e);
	gdtnode v2 = stop (e);
	bool flag1 = (get_first_out_cand(v1) == e);
	bool flag2 = (get_first_in_cand (v2) == e);
	
	//Added by Marcandalli 10 2001	
	constraint_on_bends_of_edge.undefine(e);
	//
	
	gdtnode v  = plan_undi_graph::new_node(e,new_id);
	gdtedge e1 = first_in_edge (v);
	gdtedge e2 = first_out_edge(v);
	
	set_first_in_cand (v,e1);
	set_first_out_cand(v,e2);
	set_in_cand_edges_are_ordered (v,true);
	set_out_cand_edges_are_ordered(v,true);
	
	if (flag1) set_first_out_cand(v1,e1);
	if (flag2) set_first_in_cand (v2,e2);

	
	
	return v; 
	}      	
              		
	
		
	gdtedge 
	upwa_plan_undi_graph::
new_edge 
	(gdtnode v1, gdtnode v2, face f, int new_id)
	{
	gdtedge e = plan_undi_graph::new_edge (v1,v2,f,new_id);
	undi_graph::make_directed (e,v1);
	set_in_cand_edges_are_ordered (v2,false);
	set_out_cand_edges_are_ordered(v1,false);
	return e;
	}
	
	
	
	gdtedge 
	upwa_plan_undi_graph::
new_edge 
	(gdtnode v1, gdtedge e1, gdtnode v2, gdtedge e2, int new_id)
	{
	gdtedge e = plan_undi_graph::new_edge (v1,e1,v2,e2,new_id);
	undi_graph::make_directed (e,v1);
	set_in_cand_edges_are_ordered (v2,false);
	set_out_cand_edges_are_ordered(v1,false);
	return e;
	}
	
	
	
	void
	upwa_plan_undi_graph:: 
del_edge 
	(gdtedge e) // called by del_node
	{
	bool change_ext_face = false;
	gdtnode v1 = start(e); 
	gdtnode v2 = stop (e);
	if (get_first_out_cand(v1) == e) 
		{
		set_first_out_cand (v1,NULL_EDGE);
		set_out_cand_edges_are_ordered (v1,false);
		}
		
	if (get_first_in_cand (v2) == e) 
		{
		set_first_in_cand  (v2,NULL_EDGE);
		set_in_cand_edges_are_ordered  (v2,false);
		}
		
	if (edge_belongs_to_face (e,external_face())) change_ext_face=true;	

	//Added by Marcandalli 10 2001
	if (degree(source(e)) == 1) 
		node_info->undefine(source(e));
	if (degree(target(e)) == 1) 
		node_info->undefine(target(e));
	constraint_on_bends_of_edge.undefine(e);
	//
	face f = plan_undi_graph::del_edge(e);	

	if (change_ext_face) set_external_face(f);
	}
	
	
	
	void
	upwa_plan_undi_graph::
reverse
	(gdtedge e)
	{
	gdtnode v1 = start(e);
	gdtnode v2 = stop (e);
	undi_graph::reverse(e);
	if (get_first_out_cand(v1)== e) set_first_out_cand(v1,first_out_edge (v1));
	if (get_first_in_cand (v2)== e) set_first_in_cand (v2,first_in_edge(v2));
	set_in_cand_edges_are_ordered  (v1,false);
	set_out_cand_edges_are_ordered (v1,false);
	set_in_cand_edges_are_ordered  (v2,false);
	set_out_cand_edges_are_ordered (v2,false);
	}
	
	
	
	void
	upwa_plan_undi_graph::
make_directed
	(gdtedge e, gdtnode v)
	{
	if (v == stop(e)) reverse(e);
	}	
	
	
	void
	upwa_plan_undi_graph::
make_big_angle
	(gdtnode v, face f)
	{
	if (is_internal(v))		
		gdt_error("gdtnode is not extremal");
	
	if (degree(v) != 1)
		{
		//gdtedge e  = first_adj_edge (f);
		//gdtedge e1;
		//gdtnode w = NULL_NODE;
		
		// set e1 as an gdtedge entering in v walking on boundary of f 
		// set e2 as an gdtedge leaving from v walking on baoundary of f
		//
		gdtedge e1 = NULL_EDGE;
		gdtedge e2 = NULL_EDGE;
		gdtedge e;
		
		//Modified by A.M.
		forall_edges_adjacent_node(e,v,*this)
			{
			if (left_face_moving_along_edge  (e,v) == f) e1 = e;
			if (right_face_moving_along_edge (e,v) == f) e2 = e;
			if (e1 && e2) break;
			}
		//		
		

		//if (degree(f)==2) e1 = succ_adj_edge (e,f);
		//else		
		//	while (w != v)
		//		{
		//		e1 = e;
		//		e  = cyclic_adj_succ (e1,f);
		//		w  = node_between_adj_edges (e1,e);
		//		}
			
		if (is_source(v)) set_first_out_cand (v,e1);
	
	
			  // --------------------------------------
			  //		v source 
			  // --------------------------------------
			  //
			  //        +                \ | /
			  //    e1  |  e2           e1\|/ e2
			  //  +-----o-----+  --->      o  
			  //	    v                  v
			  //        
			  //        f                  f
			  //
			  // ---------------------------------------
			 	
	
		else		  set_first_in_cand  (v,e2);
	
	
			  // --------------------------------------
			  //		v target 
			  // --------------------------------------
		          /*
			            |                     f
			        e1  +  e2              v
			      -----+o+-----  --->      o  
			    	    v               e2/|\e1
			                             / | \
			            f                  
			  */  
			  // ---------------------------------------
		}
			
	}
	
	
	
	gdt::gdtlist<gdtedge>
	upwa_plan_undi_graph::
decompose
	(face& f)
	{
	bool ext_face_changed;
	
	if (f==external_face()) ext_face_changed = true;
	else			ext_face_changed = false; 
	
	//cout << "\nSTART DECOMPOSITION OF FACE " << id(f) << flush; 
	
	gdtedge e;
	gdt::gdtmap<gdt::list_item,bool> is_big(false);
	gdt::gdtlist<gdtedge> 	    dum;		
	gdt::gdtlist<gdtnode> 	    ext; 
	
	extremals(f,ext,is_big);
	
	gdt::list_item  	    pos1,pos2,pos3;	
	
	//cout << "\n  - extremals sequence =" << flush;
	pos1 = ext.first();
	while (pos1) 
		{
		//char angle;
		//if (is_big[pos1]) angle = 'B';
		//else		  angle = 'S';		
		//cout << " " << id(ext.inf(pos1)) << " (" << is_big[pos1] << ")"  << flush;
		pos1 = ext.succ(pos1);
		}
	//cout << "\n" << flush;
		 
	pos1 = find_BSS(ext,is_big);
	while (pos1)
		{
		pos2 = ext.cyclic_succ(pos1);
		pos3 = ext.cyclic_succ(pos2);
		
		gdtnode v1 = ext.inf(pos1);
		//gdtnode v2 = ext.inf(pos2);
		gdtnode v3 = ext.inf(pos3);
		
		//cout << "\n  - found sequence BSS: " << flush;
		//cout << " " << id(v1) << flush;
		//cout << " " << id(v2) << flush;
		//cout << " " << id(v3) << flush;
		
		// ------------------- 1 ----------------------
		// add a new dummy gdtedge to remove the big-angle
		// --------------------------------------------
		
		e = new_edge (v1,v3,f);
		
		if (is_source(v1)) 
			{
			make_directed (e,v3);
			//cout << "\n  - added new gdtedge: (" << id(v3) << ")" << flush;
			//cout << id (e) << "->(" << id(v1) << ")" << flush;
			}
		else	
			{	   
			//cout << "\n  - added new gdtedge: (" << id(v1) << ")-" << flush;
			//cout << id (e) << "->(" << id(v3) << ")" << flush;
			}
		
		
		// ------------------- 2 -------------------------------
		// update new sub-face to decompose and the sequence ext
		// -----------------------------------------------------
		
		f = right_face_moving_along_edge (e,v1);
		ext.del_item (pos1);
		ext.del_item (pos2);
		dum.append(e);		
		
		// ----------------------------- 3 ------------------------------------
		// avoid succ. reorderings of cand-edges lists due to add of a new gdtedge
		// --------------------------------------------------------------------
			
		set_out_cand_edges_are_ordered (v1,true);
		set_in_cand_edges_are_ordered  (v3,true);

		pos1 = find_BSS(ext,is_big);
		}
		
	//cout << "\n  - no more BSS-sequences present" << flush;
	//cout << "\nEND DECOMPOSITION OF FACE\n" << flush;
	
	if (ext_face_changed) set_external_face(f);
	
	//cout << "\n      CANDIDATE LISTS: " << flush;
	//print(BY_FACES, true); cout << flush;
	
	return dum;
	}
	
	
	
	gdt::gdtlist<gdtedge>
	upwa_plan_undi_graph::
decompose
	()
	{
	//int i;
	face f;
	gdt::gdtlist<gdtedge> dum,dum1;
	gdt::gdtlist<face> faces;
	forall_faces (f,*this) faces.append(f);
	
	forall (f,faces) 
		if (external_face() != f)
			{		
			dum1 = decompose(f);
			//cout << "\n\nDIGIT A NUMBER AND PRESS ENTER TO CONTINUE" << flush;
			//cin >> i; 
			dum.conc(dum1);
			}
	f = external_face();
	dum1 = decompose(f);
	dum.conc(dum1);
	return dum;
	}	
	
	
	
	bool
	upwa_plan_undi_graph::
is_upward
	()
	{
	face f;
	
	forall_faces (f,*this)
		{
		int Bf = big_angles(f).size();
		if (Bf != capacity(f)) return false;
		}
		
	return true;
	}
		
		
		
		
		
		
