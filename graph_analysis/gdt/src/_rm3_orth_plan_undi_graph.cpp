/*******************************************************************************
+
+  _rm3_orth_plan_undi_graph.c
+
+  This file is part of GDToolkit. It can be
+  used free of charge in academic research and teaching.
+  Any direct or indirect commercial use of this software is illegal
+  without a written authorization of
+  the Dipartimento di Informatica e Automazione
+  Universita' di Roma Tre, Roma, ITALIA
+
+
************************************************************************/

#include <assert.h>
#include <string>
#include <GDT/gdtarray.h>
#include <GDT/gdtstack.h>
#include <GDT/gdtp_queue.h>
#include <GDT/gdt_graph_array.h>
#include <GDT/rm3_global.h>
#include <GDT/rm3_flow_dire_graph.h>
#include <GDT/rm3_orth_plan_undi_graph.h>
#include <GDT/rm3_SPQR_tree.h>
#include <GDT/rm3_upwa_plan_undi_graph.h>
#include <GDT/gdt_error.h>


using std::cout;
using std::endl;
using std::flush;
using std::ostream;



//-----------------------------------------------------------------------------
// ortho_plan_undi_graph:
// 	base class for all planar undirected graphs
// 	with planar embedding and orthogonal shape
//
// W.Didimo, A.Leonforte (1996)
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// struct_orth_border_step_info
//----------------------------------------------------------------------------- 

	struct_orth_border_step_info::
struct_orth_border_step_info
	()
	{
	bends.clear();
	angle      = _180; 
	thickness  = 1;
	pin_number = UNDEFINED_PIN_NUMBER;
	}


//-----------------------------------------------------------------------------
// orth_plan_undi_graph 
//-----------------------------------------------------------------------------
	
	orth_plan_undi_graph::
~orth_plan_undi_graph
	()
	{
	local_del();
	}
	
	

	orth_plan_undi_graph::
orth_plan_undi_graph
	()
	{
	//cout << "\n orth_plan_undi_graph () creator" << flush;
	plan_undi_graph::new_edge(first_node(),last_node(),first_face());
	local_new();
	face ef = first_face();
	local_init(ef,PLAN_ORTH_OPTIMAL,true);
	}
	


	orth_plan_undi_graph::
orth_plan_undi_graph
	(const undi_graph& ug, algorithm_type alg, bool err_mess) 
	{
	//cout << "\n orth_plan_undi_graph (undi_graph) creator" << flush;
	plan_undi_graph::operator=(ug);
	local_new();
	face ef = first_face();
	local_init(ef,alg,err_mess);
	}



	orth_plan_undi_graph::
orth_plan_undi_graph
	(const plan_undi_graph& pug, algorithm_type alg, bool err_mess) 
	{
	//cout << "\n orth_plan_undi_graph (plan_undi_graph) creator" << flush;
	enable_diagnostic_printouts();
	plan_undi_graph::operator=(pug);
	local_new();
	face ef = first_face();
	local_init(ef,alg,err_mess);
	}



	orth_plan_undi_graph::
orth_plan_undi_graph
	(const plan_undi_graph& pug, face ef, algorithm_type alg, bool err_mess) 
	{
	//cout << "\n orth_plan_undi_graph (plan_undi_graph) creator" << flush;
	
	plan_undi_graph::operator=(pug);
	local_new();
	local_init(corresponding_face(ef,pug),alg,err_mess);
	}



	orth_plan_undi_graph::
orth_plan_undi_graph 
	(
	const upwa_plan_undi_graph& upug,
	algorithm_type alg
	)
	{
	plan_undi_graph::operator=(upug);
	local_new();
	local_init(upug,alg);
	}
	
	
	
	orth_plan_undi_graph::
orth_plan_undi_graph
	(const orth_plan_undi_graph& opug)
	{
	//cout << "\n orth_plan_undi_graph (orth_plan_undi_graph) creator" << flush;
	local_new(); 	// required because operator= calls renew()
	operator=(opug);
	}
		
	

	orth_plan_undi_graph&
	orth_plan_undi_graph::
operator=
	(const undi_graph& ug)
	{
	//cout << "\n undi_graph -(copy)-> orth_plan_undi_graph" << flush;
	plan_undi_graph::operator=(ug);
	local_renew();
	face ef = first_face();
	local_init(ef,PLAN_ORTH_OPTIMAL,true);
	return *this;
	}



	orth_plan_undi_graph& 
	orth_plan_undi_graph::
operator= 
	(const plan_undi_graph& pug)
	{
	//cout << "\n plan_undi_graph -(copy)-> orth_plan_undi_graph" << flush;
	plan_undi_graph::operator=(pug);
	local_renew();
	face ef = first_face();
	local_init(ef,PLAN_ORTH_OPTIMAL,true);
	return *this;
	}



	orth_plan_undi_graph& 
	orth_plan_undi_graph::
operator= 
	(const upwa_plan_undi_graph& upug)
	{
	//cout << "\n upwa_plan_undi_graph -(copy)-> orth_plan_undi_graph" << flush;
	plan_undi_graph::operator=(upug);
	local_renew();
	local_init(upug,PLAN_ORTH_UPWA_STRAIGHT_MIDDLE);
	return *this;
	}



	orth_plan_undi_graph&
	orth_plan_undi_graph::
operator=
	(const orth_plan_undi_graph& opug)
	{
	face f;
	border_step s,sc;
	
	//cout << "\n orth_plan_undi_graph -(copy)-> orth_plan_undi_graph" << flush;
	
	plan_undi_graph::operator=(opug);
	local_renew();
	
	// the orth_plan_undi_graph-part of *this is built
	// according to the corresponding part of opug.
	//
	ext_face = corresponding_face (opug.external_face(),opug);
	forall_faces (f,opug)
		forall_face_border_steps (s,f)
			{
			sc = corresponding_border_step (s,opug);
			set_bends_of_border_step      (sc,opug.bends_of_border_step(s));
			set_angle_of_border_step      (sc,opug.angle_of_border_step(s));
			set_thickness_of_border_step  (sc,opug.thickness_of_border_step(s));
			set_pin_number_of_border_step (sc,opug.pin_number_of_border_step(s));
			}
	
	gdtedge e;
	//
	// Linear Solution
	//
	gdt::gdtedge_map<gdtedge> edge_corr_in_opug (nodes_and_edges(),NULL_EDGE);
	build_mapping_edge_to_edge_with_undi_graph (opug, edge_corr_in_opug);
	forall_edges (e,*this) 
		constraint_on_bends_of_edge[e] = opug.get_constraint_on_bends_of_edge (edge_corr_in_opug[e]);
		
	suspend_local_consistency();
	set_layout_algorithm (opug.get_layout_algorithm());
	local_consistency_is_suspended   = opug.get_local_consistency_is_suspended(); 
	diagnostic_printouts_are_enabled = opug.get_diagnostic_printouts_are_enabled();
	ref_border_step = corresponding_border_step (opug.get_ref_border_step(),opug);
	last_cost = opug.cost_of_last_algorithm();
	
	return *this;
	}
	



	// -----------------
	// Access operations
	// -----------------
	
	void
	orth_plan_undi_graph::
local_get
	(
	face&                                            p1,
	algorithm_type&                                  p2,
	gdt::gdtedge_map<bend_constraint>&                       p3,
	gdt::gdtmap<border_step,struct_orth_border_step_info>*&  p4,
	bool&                                            p5,
	bool&                                            p6,
	border_step&					 p7,
	int						 p8
	)
	{
	p1 = ext_face;
	p2 = layout_algorithm;
	p3 = constraint_on_bends_of_edge; 
	p4 = border_step_info;
	p5 = local_consistency_is_suspended;
	p6 = diagnostic_printouts_are_enabled;
	p7 = ref_border_step;
	p8 = last_cost;
	} 	


	
	gdt::gdtlist<bend_type>
	orth_plan_undi_graph:: 
bends_of_border_step
	(border_step s) const
	{
	return (*border_step_info)[s].bends;
	}
	
	
	
	angle_type
	orth_plan_undi_graph::	
angle_of_border_step
	(border_step s) const
	{
	return (*border_step_info)[s].angle;
	}
	
	
	
	int
	orth_plan_undi_graph::	
thickness_of_border_step
	(border_step s) const
	{
	return (*border_step_info)[s].thickness;
	}
	

	int
	orth_plan_undi_graph::	
pin_number_of_border_step
	(border_step s) const
	{
	return (*border_step_info)[s].pin_number;
	}

	
	int
	orth_plan_undi_graph::	
thickness_of_edge
	(gdtedge e) const
	{
	int sum=0;
	border_step s1 = border_step_moving_along_edge (e,source(e));
	border_step s2 = border_step_moving_along_edge (e,target(e));
	sum = thickness_of_border_step(s1) + thickness_of_border_step(s2) - 1;
	return sum;
	}
	
	
	int
	orth_plan_undi_graph::
right_thickness_of_edge
	(gdtedge e, gdtnode v) const
	{
	border_step s = border_step_moving_along_edge (e,v);
	return thickness_of_border_step(s);
	}
	
	
	int
	orth_plan_undi_graph::
left_thickness_of_edge
	(gdtedge e, gdtnode v) const
	{
	border_step s = border_step_moving_along_edge (e,opposite(v,e));
	return thickness_of_border_step(s);
	}
	
	
	int
	orth_plan_undi_graph::
number_of_bends
	(border_step s) const
	{
	return bends_of_border_step(s).size();
	}
	
	
	
	int
	orth_plan_undi_graph::
number_of_bends
	(gdtedge e) const
	{
	border_step s;
	s = border_step_moving_along_edge (e,source(e));
	return number_of_bends(s);
	}
	
	
	
	int
	orth_plan_undi_graph::
number_of_bends
	() const
	{
	gdtedge e;
	int total = 0;
	forall_edges (e,*this)
		total += number_of_bends (e);
	return total;
	}
	
	
	
	int
	orth_plan_undi_graph::
number_of_bends 
	(gdt::gdtlist<gdtedge> ls_edge) const
	{
	gdtedge e;
	int total =0;
	forall (e,ls_edge)
		total += number_of_bends (e);
	return total;
	}
	
	
	int
	orth_plan_undi_graph::
number_of_left_bends
	(border_step s) const
	{
	bend_type b;
	int counter = 0;
	gdt::gdtlist<bend_type> B = bends_of_border_step(s);
	forall (b,B) if (b==left) counter+=1;
	return counter;
	}
	


	int
	orth_plan_undi_graph::
number_of_right_bends
	(border_step s) const
	{
	bend_type b;
	int counter = 0;
	gdt::gdtlist<bend_type> B = bends_of_border_step(s);
	forall (b,B) if (b==right) counter+=1;
	return counter;
	}
	
	
		
	int
	orth_plan_undi_graph::
number_of_left_turns_around_face
	(face f) const
	{
	//cout << "\n orth_plan_undi_graph::number_of_left_turns_around_face " << flush;
	border_step s;
	int counter = 0;
	forall_face_border_steps(s,f)
		{
		//cout << "\nEdge " << id(edge_of_border_step(s)) << " angle " << angle_of_border_step(s) << flush;
		counter += number_of_left_bends(s);
		switch (angle_of_border_step(s))
			{
			case _270: counter+=1; break;
			case _360: counter+=2; break;
			default: break;
			}
		}
	return counter;
	}
	
	
	
	int
	orth_plan_undi_graph::
number_of_right_turns_around_face
	(face f) const
	{
	//cout << "\n orth_plan_undi_graph::number_of_right_turns_around_face " << flush;
	border_step s;
	int counter = 0;
	forall_face_border_steps(s,f)
		{
		//cout << "\nEdge " << id(edge_of_border_step(s)) << " angle " << angle_of_border_step(s) << flush;
		counter += number_of_right_bends(s);
		if (angle_of_border_step(s) == _090)      counter+=1;
		else if (angle_of_border_step(s) == _000) counter+=2;
		}
	return counter;
	}
	


	int
	orth_plan_undi_graph::
number_of_left_turns_along_border
	(border_step sv, border_step sw) const
	{
	//cout << "\n orth_plan_undi_graph::number_of_left_turns_along_border " << flush;
	border_step s;
	int counter=0;
	gdt::gdtlist<border_step> seq;
	seq = border_step_sequence (sv,sw);
	forall (s,seq)
		{
		//cout << "\nEdge " << id(edge_of_border_step(s)) << flush;
		counter += number_of_left_bends(s);
		if (s!=seq.tail())		
			switch (angle_of_border_step(s))
				{
				case _270: counter+=1; break;
				case _360: counter+=2; break;
				default  : break;
				}
		}
	return counter;
	}



	int
	orth_plan_undi_graph::
number_of_right_turns_along_border
	(border_step sv, border_step sw) const
	{
	border_step s;
	int counter=0;
	gdt::gdtlist<border_step> seq;
	seq = border_step_sequence (sv,sw);
	forall (s,seq)
		{
		counter += number_of_right_bends(s);
		if (s!=seq.tail())
			if (angle_of_border_step(s)==_090) 	 counter+=1;
			else if (angle_of_border_step(s)==_000 ) counter+=2;
		}
	return counter;
	}
	
	
	
	int
	orth_plan_undi_graph::
max_number_of_bends_on_edge 
	() const
	{
	int max = 0;
	gdtedge e;
	forall_edges (e,*this)
		{
		int nbe = number_of_bends (e);
		if (max < nbe) max = nbe;
		}
	return max;
	}	
	
	
	
	double 
	orth_plan_undi_graph::
mean_number_of_bends_on_edge 
	() const
	{
	double mean = (double)number_of_bends() / (double)number_of_edges();
	return mean;
	}
	
	
	
	double
	orth_plan_undi_graph::
bend_standard_deviation
	() const
	{
	double sd;
	double sum = 0;
	double mean = mean_number_of_bends_on_edge();
	gdtedge e;
	
	forall_edges (e,*this) 
		{
		double sde = ((double)number_of_bends(e) - mean);
		sum += (sde*sde);
		}
	sd = sqrt (sum / (double)number_of_edges());
	return sd;
	}
	
	
	
	face
	orth_plan_undi_graph::
external_face 
	() const
	{
	return ext_face;
	} 
	


	bend_constraint
	orth_plan_undi_graph::
get_constraint_on_bends_of_edge
	(gdtedge e) const
	{
	return constraint_on_bends_of_edge[e];
	}
	
		
	
	algorithm_type
	orth_plan_undi_graph::
get_layout_algorithm
	() const
	{
	return layout_algorithm;
	}



	bool 
	orth_plan_undi_graph::
get_local_consistency_is_suspended
	() const
	{
	return local_consistency_is_suspended;
	}
	
	
	
	bool
	orth_plan_undi_graph::
get_diagnostic_printouts_are_enabled
	() const
	{
	return diagnostic_printouts_are_enabled;
	}



	bool
	orth_plan_undi_graph::
border_is_closed
	(face f) const
	{
	//cout << "\north_plan_undi_graph::border_is_closed for face " << id(f) << flush;
	int lt,rt,dt;
	rt = number_of_right_turns_around_face (f);
	lt = number_of_left_turns_around_face  (f);
	dt = rt-lt;
	return (dt==+4 && f!=ext_face) ||
	       (dt==-4 && f==ext_face);
	}



	bool
	orth_plan_undi_graph::
border_step_turns_left
	(border_step s) const
	{
	border_step ps = cyclic_pred_border_step(s);
	return (angle_of_border_step(ps) == _270);
	}
	

	
	bool
	orth_plan_undi_graph::
border_step_turns_right
	(border_step s) const
	{
	border_step ps = cyclic_pred_border_step(s);
	return (angle_of_border_step(ps) == _090);
	}



	bool
	orth_plan_undi_graph::
border_step_turns_back 
	(border_step s) const
	{
	border_step ps = cyclic_pred_border_step(s);
	return (angle_of_border_step(ps) == _360 || angle_of_border_step(ps) == 0);
	}



	bool
	orth_plan_undi_graph::
border_step_goes_straight
	(border_step s) const
	{
	border_step ps = cyclic_pred_border_step(s);
	return (angle_of_border_step(ps) == _180);
	}


 	
	gdt::list_item
	orth_plan_undi_graph::
find_border_step_turning_left
	(gdt::gdtlist<border_step>& bsl, gdt::list_item start_pos) const
	{
	gdt::list_item pos;
	if (start_pos) pos = start_pos;
	else           pos = bsl.first();
	while (pos && !border_step_turns_left(bsl.inf(pos))) pos = bsl.succ(pos);
	return pos;
	}
	
	
	
	gdt::list_item
	orth_plan_undi_graph::
find_border_step_turning_right
	(gdt::gdtlist<border_step>& bsl, gdt::list_item start_pos) const
	{
	gdt::list_item pos;
	if (start_pos) pos = start_pos;
	else           pos = bsl.first();
	while (pos && !border_step_turns_right(bsl.inf(pos))) pos = bsl.succ(pos);
	return pos;
	}
	
	
	
	gdt::list_item
	orth_plan_undi_graph::
find_border_step_turning_left_or_turning_back
	(gdt::gdtlist<border_step>& bsl, gdt::list_item start_pos) const
	{
	gdt::list_item pos;
	if (start_pos) pos = start_pos;
	else           pos = bsl.first();
	while (pos && !(border_step_turns_left(bsl.inf(pos)) || border_step_turns_back(bsl.inf(pos)))) pos = bsl.succ(pos);
	return pos;
	}
	
	
	
	border_step
	orth_plan_undi_graph::
get_ref_border_step
	() const
	{
	return ref_border_step;
	}


	
	int
	orth_plan_undi_graph::
cost_of_last_algorithm
	() const
	{
	return last_cost;
	}

	
	
	gdt::gdtlist<border_step>
	orth_plan_undi_graph::
turning_border_steps_of_face
	(face f) const
	{
	border_step s;
	gdt::gdtlist<border_step> bsl;
	forall_face_border_steps (s,f)
		if (!border_step_goes_straight(s))
			bsl.append(s);
	return bsl;
	}

	
	
	void
	orth_plan_undi_graph::
edges_on_each_side_of_node 
	(gdtnode v, gdt::gdtlist<gdtedge>& L1, gdt::gdtlist<gdtedge>& L2, gdt::gdtlist<gdtedge>& L3, gdt::gdtlist<gdtedge>& L4) const
	{
	gdtnode 		w; 
	gdtedge 		e;
	border_step     s;
	
	// --------------------------------------------------- 
	//			L2
	//		     	|||
	//		     	|||
	//		    ---|---|---
	//		 L3 ---| v |--- L1
	//		    ---|---|---
	//			|||
	//			|||
	//	                 L4
	// ---------------------------------------------------
	//
	// ----------------------- 0 -------------------------
	// init all the four lists of edges L1, L2, L3 and L4
	// ---------------------------------------------------
	//
	L1.clear();
	L2.clear();
	L3.clear();
	L4.clear();
	
	// ------------------------ 1 ------------------------
	// look for the end of the edges on a side
	// ---------------------------------------------------
	//
	e = first_adj_edge (v);
	w = opposite (v,e);
	s = border_step_moving_along_edge (e,w);
	while (angle_of_border_step(s) == _000)
		{
		e = cyclic_adj_succ (e,v);
		w = opposite (v,e);
		s = border_step_moving_along_edge (e,w);
		}
	
	// ------------------------ 2 ------------------------
	// store the edges per side in the corresponding list
	// ---------------------------------------------------
	//
	int i=1;
	while (i<=4)
		{
		do
			{
			e = cyclic_adj_succ (e,v);
			switch (i)
				{
				case 1: L1.append(e); break;
				case 2: L2.append(e); break;
				case 3: L3.append(e); break;
				case 4: L4.append(e); break;
				}
			w = opposite (v,e);
			s = border_step_moving_along_edge (e,w);
			}
		while (angle_of_border_step(s) == _000);
		
		
		// choose the next side
		//
		switch (angle_of_border_step(s))
			{
			case _090: i++ ; break;
			case _180: i+=2; break;
			case _270: i+=3; break;
			case _360: i+=4; break;
			default:	 break;
			}	
		}
	}



	bool
	orth_plan_undi_graph::
face_is_regular
	(face f, border_step& r1, border_step& r2) const 
	{
	//cout << "\nEXAMINING REGULARITY OF FACE " << id(f) << flush;
	
	r1 = NULL;
	r2 = NULL;	
	
	// -------------------------------------------------------
	// build the list of border steps that form a reflex angle
	// -------------------------------------------------------
	
	//cout << "\nREFLEX LIST: " << flush;
	
	gdt::gdtlist<border_step>  reflex;		 // reflex list
	border_step bs,bst;
	//border_step bs1;
	
	gdt::list_item lbs, lbs1, lbst;
	gdt::gdtmap<border_step,gdt::list_item> pos_of_reflex (NULL); // first occurence of the border step in the reflex list
	
	reflex.clear();
	forall_face_border_steps (bs,f)
		if (angle_of_border_step(bs) == _270)
			{
			pos_of_reflex[bs] = reflex.append(bs);
			//cout << " " << id(stop_of_border_step(bs)) << flush;
			}
		else if (angle_of_border_step(bs) == _360)
		    	{
		    	pos_of_reflex[bs] = reflex.append(bs);
		    	reflex.append(bs);
		    	//cout << " " << id(stop_of_border_step(bs)) << flush;
		    	//cout << " " << id(stop_of_border_step(bs)) << flush;
		    	}
	
	if (reflex.size() > 1)
		{
		// -------------------------------------------------------
		// build a vector of delta distances between two 
		// consecutive reflex border steps. Distances are computed
		// starting with 0 at the first reflex, and counting +1 
		// on a right-bend and -1 on a left-bend.
		// -------------------------------------------------------
	
		//map<border_step,int> delta(0);	
							
		int count = 0;
		gdt::gdtmap<gdt::list_item,int> delta(0);			// delta-distance from the previous reflex border step 
								// to this reflex border step 
										
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
					case _000: count +=2; break;	
					case _090: count ++;  break;
					case _270: 
					case _360: count --; break;
					default: break;
					}
				if (a == _360) 
					{
					lbs = reflex.cyclic_succ(lbs);
					delta[lbs] = count;
					//cout << "\ndelta[" << id(stop_of_border_step(bs)) << "] = " << delta[lbs];
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
		
		
		// -------------------------------------------------------
		// build a vector such that for each reflex there is
		// an associated absolute distance from the initial reflex; 
		// also build a second bucket vector, that 
		// informs if there exists a reflex with a given 
		// integer distance
		// -------------------------------------------------------
		
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
		   if (f == external_face()) balance = -4;
		   else			     balance = +4;
		   if (distance[lbs1] != balance)
			   gdt_error("balance internal error");
		   }
		
		// -------------------------------------------------------
		// using the vectors above, detect a reflex pair if any
		// -------------------------------------------------------
		
		gdt::gdtmap<gdt::list_item,bool> visited (false);
		int d1,d2;
		
		forall_items (lbs,reflex)
			{
			//bs = reflex.inf(lbs); cout << "\nEXAMINING BORDER STEP ON REFLEX " << id(stop_of_border_step(bs)) << flush;
			if (lbs != lbs1)
				{
				d1 = distance[lbs];
				d2 = d1 - 2;
				
				if (d2 == 0) lbst = lbs1; 
				else 	     lbst = lbs_with_distance[d2];
				
				if ((lbst && visited[lbst]) || (lbst == lbs1 && !d2))
					{
					// found a pair of kitty corners
					bs  = reflex.inf(lbs);
					bst = reflex.inf(lbst);
					r1 = bs;
					r2 = bst;
					
					//gdtnode v1 = stop_of_border_step(bs) ;
					//gdtnode v2 = stop_of_border_step(bst);
					//cout << "\nIRREGULAR FACE " << id(f) << flush;
					//print (f); cout << flush;
					//cout << "\nFOUND A PAIR OF KITTY CORNERS (" << id(v2) << "," << id(v1) << ")" << flush;
					break;
					}
				else if (f == external_face())
					{					
					d2 = d1 + 6;
					lbst = lbs_with_distance[d2];
					if (d2 == 0) lbst = lbs1; 
					else 	     lbst = lbs_with_distance[d2];
				
					if ((lbst && visited[lbst]) || (lbst == lbs1 && !d2))
						{
						// found a pair of kitty corners
						bs  = reflex.inf(lbs);
						bst = reflex.inf(lbst);
						r1 = bs;
						r2 = bst;

						//gdtnode v1 = stop_of_border_step(bs) ;
						//gdtnode v2 = stop_of_border_step(bst);
						//cout << "\nIRREGULAR FACE " << id(f) << flush;
						//print (f); cout << flush;
						//cout << "\nFOUND A PAIR OF KITTY CORNERS (" << id(v2) << "," << id(v1) << ")" << flush;
						break;
						}
					}	
					
				lbs_with_distance[d1] = lbs;
				visited[lbs] = true;	
				lbst = NULL;
				}
			}
		
		lbs = lbs1;
		d1 = distance[lbs];
		d2 = d1 - 2;
		lbst = lbs_with_distance[d2];
		
		if (lbst && visited[lbst])
			{
			bs  = reflex.inf(lbs);
			bst = reflex.inf(lbst);
			r1 = bs;
			r2 = bst;
			
			//gdtnode v1 = stop_of_border_step(bs) ;
			//gdtnode v2 = stop_of_border_step(bst);
			//cout << "\nIRREGULAR FACE " << id(f) << flush;
			//print (f); cout << flush;
			//cout << "\nFOUND KITTY CORNERS (" << id(v2) << "," << id(v1) << ")" << flush;
			}
		}
		
	if (r1) return false;
	else	return true;
	}



	bool
	orth_plan_undi_graph::
node_is_flat	
	(
	gdtnode v
	) const
	{
	bool flat = false;
	if (degree(v) == 2)
		{
		gdtedge e = first_adj_edge(v);
		border_step s = border_step_moving_along_edge(e,opposite(v,e));
		if (angle_of_border_step(s) == _180) flat = true;
		}
	return flat;
	}



	// -----------------
	// Update operations
	// -----------------
	
	// Description: split gdtedge 'e' by creating a new gdtnode on it.
	// Value: the gdtnode created.
	// Preconditions: gdtedge 'e' must have no bends.
	// Notes: -
	
	gdtnode
	orth_plan_undi_graph::
new_node
	(gdtedge e, int new_id) 
	{
	// split the gdtedge e with a new gdtnode v if e has not bends.
	//
	gdtedge e1, e2;
	gdtnode v, v1, v2;
	angle_type a1, a2;
	border_step s1, s2;
	int t1, t2;		// thickness
	int pn1, pn2;		// pin_number
	
	if (number_of_bends(e) > 0) 
		gdt_error("the gdtedge has some bends."); 

	v1  = source(e);
	v2  = target(e);
	s1  = border_step_moving_along_edge (e,v1);
	s2  = border_step_moving_along_edge (e,v2);
	a1  = angle_of_border_step (s1);
	a2  = angle_of_border_step (s2);
	t1  = thickness_of_border_step (s1);
	t2  = thickness_of_border_step (s2);
	pn1 = pin_number_of_border_step (s1);
        pn2 = pin_number_of_border_step (s2);
		
	bool change_rbs_from_v1 = false;	// "rbs" = "reference border step"
	bool change_rbs_from_v2 = false;
	if (s1 == ref_border_step)      change_rbs_from_v1 = true;
	else if (s2 == ref_border_step) change_rbs_from_v2 = true;
	
	
	/* 
	//   \              
	//    \  a2                   
	//     \     <-s2-          
	//    (v1)----e-----(v2)
	//          -s1->     \
	//                 a1  \
	//                      \
	*/
	
	//cout << "\ndeleting gdtedge e = " << id(e) << flush;
	
	v  = plan_undi_graph::new_node(e,new_id);	// apply the plan_new_edge
	
	// Added by A.M. 10 2001
	border_step_info -> undefine (s1); 
	border_step_info -> undefine (s2);
	//


	e1 = first_adj_edge(v);
	e2 = last_adj_edge (v);
	if (opposite(v,e1) != v1)  {gdtedge ex=e1; e1=e2; e2=ex;} // Swap e1 and e2

	/* 
	//   \              
	//    \  a2                   
	//     \
	//    (v1)----e1-----(v)-----e2-----(v2)
	//                                    \
	//                                 a1  \
	//                                      \
	*/

	set_edge_shape (e1, v1, "",  a2 , _180);
	set_edge_shape (e2, v,  "", _180,  a1 );
	
	border_step s_v1_v = border_step_moving_along_edge (e1,v1);
	border_step s_v_v1 = border_step_moving_along_edge (e1,v );
	border_step s_v2_v = border_step_moving_along_edge (e2,v2);
	border_step s_v_v2 = border_step_moving_along_edge (e2,v );
	
	if (change_rbs_from_v1)      ref_border_step = s_v1_v;
	else if (change_rbs_from_v2) ref_border_step = s_v2_v;
	
	set_thickness_of_border_step (s_v1_v,t1);
	set_thickness_of_border_step (s_v_v2,t1);
	set_thickness_of_border_step (s_v2_v,t2);
	set_thickness_of_border_step (s_v_v1,t2);	
	
	set_pin_number_of_border_step (s_v1_v,pn1);
	set_pin_number_of_border_step (s_v_v2,pn1);
	set_pin_number_of_border_step (s_v2_v,pn2);
	set_pin_number_of_border_step (s_v_v1,pn2);	

	return v; 
	}




	gdtnode 
	orth_plan_undi_graph::
new_node 
	(
	gdtedge e,
	gdtnode v,
	int seg_num, 
	int new_id
	)
	{
	// debugging
	//
	//cout << "\nsplitting gdtedge " << id(e) << " on segment " << seg_num << " from gdtnode " << id(v) << flush;
	//	
	gdtnode u = NULL_NODE;		// gdtnode to be returned
	gdtnode w = opposite (v,e);
	border_step sv = border_step_moving_along_edge (e,v);
	border_step sw = border_step_moving_along_edge (e,w);	
	gdt::gdtlist<bend_type> bends = bends_of_border_step(sv);

	/* 
	//   \              
	//    \  aw                   
	//     \     <-sw-          
	//     (v)----e-----(w)
	//          -sv->     \
	//                 av  \
	//                      \
	*/
	
	// ----------------- 1 ------------------
	// check the feasibility of the operation
	// --------------------------------------
	
	if (bends.size() < seg_num-1)
		gdt_error("invalid seg_num.");
	
	bool main_edge_on_v = is_main_edge_on_node_side (e,v);	
	bool main_edge_on_w = is_main_edge_on_node_side (e,w);	
	
	if ( (!main_edge_on_v && seg_num == 1) ||
	     (!main_edge_on_w && seg_num == bends.size()+1) )	     
		gdt_error("invalid seg_num.");
	
	// ------------------ 2 ------------------
	// save all information about gdtedge e
	// ---------------------------------------
	
	angle_type av  = angle_of_border_step (sv);
	angle_type aw  = angle_of_border_step (sw);
	int tv  = thickness_of_border_step (sv);
	int tw  = thickness_of_border_step (sw);
	int pnv = pin_number_of_border_step (sv);
        int pnw = pin_number_of_border_step (sw);
	
	gdt::gdtlist<bend_type> bends_v_u,
			bends_u_w,
			bends_w_u,
			bends_u_v;
	
	gdt::list_item li = bends.first();
	
	int i;
	for (i=1; i<=seg_num-1; i++)
		{
		bend_type bend=bends.inf(li); 
		bends_v_u.append(bend);
		if (bend == left) bend = right;
		else if (bend == right) bend = left;
		bends_u_v.push(bend);
		li=bends.succ(li);
		}
	
	for (i=seg_num; i <= bends.size(); i++)
		{
		bend_type bend=bends.inf(li); 
		bends_u_w.append(bend);
		if (bend == left) bend = right;
		else if (bend == right) bend = left;
		bends_w_u.push(bend);		
		li=bends.succ(li);
		}
		
	bool change_rbs_from_v = false;	// "rbs" = "reference border step"
	bool change_rbs_from_w = false;
	if (sv == ref_border_step)      change_rbs_from_v = true;
	else if (sw == ref_border_step) change_rbs_from_w = true;

	// ------------------- 3 --------------------
	// split the gdtedge by adding the new gdtnode, and
	// set the shape of the two
	// new edges according to the previous shape
	// ------------------------------------------

	u  = plan_undi_graph::new_node(e,new_id);	// apply the plan_new_edge		

	// Added by A.M. 10 2001
	border_step_info -> undefine (sv);
	border_step_info -> undefine (sw);
	//

	gdtedge ev = first_adj_edge(u);
	gdtedge ew = last_adj_edge (u);
	if (opposite(u,ev) != v)  {gdtedge ex=ev; ev=ew; ew=ex;} // Swap e1 and e2

	/* 
	//   \              
	//    \ aw                
	//     \    <-sev_u--  <-sew_w-- 
	//    (v)----ev-----(u)-----ew------(w)
	//         --sev_v->  --sew_u->       \
	//                                  av \
	//                                      \
	*/

	border_step sev_v = border_step_moving_along_edge (ev,v);
	border_step sev_u = border_step_moving_along_edge (ev,u);
	border_step sew_u = border_step_moving_along_edge (ew,u);
	border_step sew_w = border_step_moving_along_edge (ew,w);
		
	set_thickness_of_border_step (sev_v,tv);
	set_thickness_of_border_step (sev_u,tw);
	set_thickness_of_border_step (sew_u,tv);
	set_thickness_of_border_step (sew_w,tw);	
	
	set_pin_number_of_border_step (sev_v,pnv);
	set_pin_number_of_border_step (sev_u,pnw);
	set_pin_number_of_border_step (sew_u,pnv);
	set_pin_number_of_border_step (sew_w,pnw);	
		
	set_angle_of_border_step (sev_v, _180);
	set_angle_of_border_step (sev_u, aw);
	set_angle_of_border_step (sew_u, av);
	set_angle_of_border_step (sew_w, _180);		
	
	set_bends_of_border_step (sev_v, bends_v_u);
	set_bends_of_border_step (sev_u, bends_u_v);
	set_bends_of_border_step (sew_u, bends_u_w);
	set_bends_of_border_step (sew_w, bends_w_u);
	
	if (change_rbs_from_v)      ref_border_step = sev_v;
	else if (change_rbs_from_w) ref_border_step = sew_w;
		
	return u;
	}          	  



	gdtnode 
	orth_plan_undi_graph::
new_node 
	(
	gdtnode v,
	gdtedge e,		// you will insert the new gdtedge after this (counterclockwise)
	angle_type alpha, 
	int new_node_id, 
	int new_edge_id
	)
	{
	// attach vertex
	//
	//		      s_e_v
	//            beta    <--- 
	//     --------O----------O
	//             v   e
	//
	// 
	
	int a,b;
	border_step  s_e_v = border_step_moving_along_edge (e,opposite(v,e)); 
	angle_type   beta  = angle_of_border_step(s_e_v);
	
	a = angle_to_int (alpha);
	if (a == 0) 
		gdt_error("invalid angle.");
	b = angle_to_int (beta);
	
	int ba_diff = (b-a);
	if (ba_diff < 90) 
		gdt_error("no angular space.");
	
	//             u
	//             O + s_v_u
	//           | | |
	//     s_u_v | | |     s_e_v
	//           + | alpha <--- 
	//     --------O----------O
	//             v   e
	//
	// 	
	
	gdtnode u   = plan_undi_graph::new_node (v,e,new_node_id,new_edge_id);	// plan attach vertex
	gdtedge evu = first_adj_edge (u);
	
	border_step s_u_v = border_step_moving_along_edge (evu,u);
	border_step s_v_u = opposite_border_step (s_u_v);
	
	reset_border_step_info(s_u_v);
	reset_border_step_info(s_v_u);
	
	angle_type beta_alfa_diff = int_to_angle (ba_diff); 
	set_angle_of_border_step (s_u_v, beta_alfa_diff); 
	set_angle_of_border_step (s_v_u, _360);
	
	set_angle_of_border_step (s_e_v, alpha);
	
	set_thickness_of_border_step (s_u_v,1);
	set_thickness_of_border_step (s_v_u,1);

	set_pin_number_of_border_step (s_u_v,UNDEFINED_PIN_NUMBER);
	set_pin_number_of_border_step (s_v_u,UNDEFINED_PIN_NUMBER);
	
	return u;	
	}   
	      


	// Description  : Split face 'f' by adding a new gdtedge between 'v' and 'w'.
	// Precondition : ('v' and 'w' are vertices of the border of 'f') and ('v' != 'w')
	// Notes        : When 'v' (or 'w') is the start-gdtnode of a bridge, the new gdtedge is inserted on the right of that bridge, moving on the bridge from 'v' ('w') to its opposite.
	gdtedge
	orth_plan_undi_graph:: 
new_edge 
	(gdtnode v, gdtnode w, face f, int new_id) 
	{
	//cout << "\north_plan_undi_graph:: new_edge" << flush;
	gdtedge ev,ew;
	gdt::gdtlist<gdtedge> le_vf = edges_entering_node_while_moving_around_face (v,f);
	gdt::gdtlist<gdtedge> le_wf = edges_entering_node_while_moving_around_face (w,f);
	
	if (!le_vf.empty()) ev = le_vf.head(); else ev = NULL_EDGE;
	if (!le_wf.empty()) ew = le_wf.head(); else ew = NULL_EDGE;
		
	return new_edge (v,ev, w,ew, new_id);
	}

	//----- Siamo arrivati qui ------

	// Description  : Split face 'f' by adding a new gdtedge between 'v' and 'w'. The new gdtedge is placed around v and w, respectively after ev and ew.
	// Precondition : ('v' and 'w' are vertices of the border of 'f') and ('v' != 'w')
	// Notes        : When 'v' (or 'w') is the start-gdtnode of a bridge, the new gdtedge is inserted on the right of that bridge, moving on the bridge from 'v' ('w') to its opposite.
	gdtedge
	orth_plan_undi_graph:: 
new_edge
	(gdtnode v, gdtedge ev, gdtnode w, gdtedge ew, int new_id) 
	{
	gdtedge e;
	face fv,fw;
	border_step sv1,sw1;
	//
	sv1 = border_step_moving_along_edge (ev,opposite (v,ev));
	sw1 = border_step_moving_along_edge (ew,opposite (w,ew));
	//
	fv = face_of_border_step (sv1);
	fw = face_of_border_step (sw1);
	if (fv!=fw) gdt_error("planarity violation."); 

	if (fv==ext_face) e = split_external_face    (v,ev, w,ew, new_id);
	else              e = split_internal_face    (v,ev, w,ew, new_id);
	
	return e;
	}

		
	
	face
	orth_plan_undi_graph::
del_node
	(gdtnode v)
	{
	// delete the gdtnode v and all its adjacent edges.
	//
	gdtedge e;
	face f = NULL_FACE;
	gdt::gdtlist<gdtedge> edges_list;
	
	edges_list = adj_edges (v);
	forall (e,edges_list) f=del_edge (e);
	return f;
	}



	face
	orth_plan_undi_graph:: 
del_edge
	(gdtedge e)
	{
	face f;
	gdtnode v = source(e);
	gdtnode w = target(e);
	
	//cout << "\nDELETING EDGE " << id(e) << " from " << id(v) << " to " << id(w) << flush;
	
	border_step  sv2 = border_step_moving_along_edge(e,v);
	border_step  sw2 = border_step_moving_along_edge(e,w);
	border_step  sv1 = cyclic_pred_border_step(sv2);
	border_step  sw1 = cyclic_pred_border_step(sw2);
	
	angle_type   av1 = angle_of_border_step (sv1);
	angle_type   av2 = angle_of_border_step (sv2);
	angle_type   aw1 = angle_of_border_step (sw1);
	angle_type   aw2 = angle_of_border_step (sw2);
	
	face fv = face_of_border_step (sv1);
	face fw = face_of_border_step (sw1);
	face ef = external_face();

	if (sv2 == get_ref_border_step() || 
	    sw2 == get_ref_border_step())  change_ref_border_step(); 

	/*		  
	//  |               |  |
	//  |               |  |
	//  |       fw     sw1 |
	//  |               |  |
	//  |               +  |
	//  |                  |
	//  |aw2  <--sw2--  aw1|    
	// (v)----------------(w)
	//  |av1  --sv2-->  av2|
	//  |                  |
	//  |  +               |
	//  |  |               |
	//  | sv1    fv        |
	//  |  |               |
	//  |  |               |
	*/

	//cout << "\nBefore removing gdtedge on gdtedge " << id(edge_of_border_step(sv1)) << " angle of sv1 " << av1 << " on face " << id(fv) << flush;
	set_angle_of_border_step (sv1,(angle_type)(av1+aw2));
	set_angle_of_border_step (sw1,(angle_type)(aw1+av2));
	
	//Added by Marcandalli 16 10 2001
	undefine(e);
	//
	//f = plan_undi_graph::del_edge(e);

	//cout << "\nAfter removing gdtedge angle of sv1 " << angle_of_border_step (sv1) << flush;

	// If either fv or fw were the external face,
	// the new face (replacing both fv,fw)
	// is new external face.
	//
	

	if (fv==ef || fw==ef)
		ext_face = face_of_border_step(sv1);
			
	f = plan_undi_graph::del_edge(e);	
	return f;
	}


	
	gdtedge
	orth_plan_undi_graph::
weld_edges_at_node
	(gdtnode v)
	{
	if (degree(v)!=2) 
		gdt_error("gdtnode has not degree 2");
	
	gdtedge euv = first_adj_edge(v);
	gdtedge ewv = last_adj_edge(v);
	gdtnode u   = opposite(v,euv);
	gdtnode w   = opposite(v,ewv);
	
	border_step swv = border_step_moving_along_edge (ewv,w);
	border_step svu = border_step_moving_along_edge (euv,v);
	border_step suv = border_step_moving_along_edge (euv,u);
	border_step svw = border_step_moving_along_edge (ewv,v);
	
	gdt::gdtlist<bend_type> b_swv = bends_of_border_step (swv);
	gdt::gdtlist<bend_type> b_svu = bends_of_border_step (svu);
	gdt::gdtlist<bend_type> b_suv = bends_of_border_step (suv);
	gdt::gdtlist<bend_type> b_svw = bends_of_border_step (svw);
	  
	//angle_type a_swv = angle_of_border_step (swv);
	angle_type a_svu = angle_of_border_step (svu);  
	angle_type a_suv = angle_of_border_step (suv);
	angle_type a_svw = angle_of_border_step (svw); 
	
	int t_swv = thickness_of_border_step (swv); 
	int t_svu = thickness_of_border_step (svu);
	int t_suv = thickness_of_border_step (suv);
	int t_svw = thickness_of_border_step (svw);
	
	if ((t_swv != t_svu) || (t_suv != t_svw))
		//{
		//cout << "\ndeleting gdtnode " << id(v) << flush;
		//cout << "\nthickness of gdtedge " << id(euv) << " from gdtnode " << id(u) << " = " << t_suv << flush;
		//cout << "\nthickness of gdtedge " << id(euv) << " from gdtnode " << id(v) << " = " << t_svu << flush;
		//cout << "\nthickness of gdtedge " << id(ewv) << " from gdtnode " << id(w) << " = " << t_swv << flush;
		//cout << "\nthickness of gdtedge " << id(ewv) << " from gdtnode " << id(v) << " = " << t_svw << flush;
		gdt_error("invalid thickness");
		//}

        // ### Begin import from GDT fork of W. Didimo on June, 18 2004
	//int pn_swv = pin_number_of_border_step (swv); 
	int pn_svu = pin_number_of_border_step (svu);
	//int pn_suv = pin_number_of_border_step (suv);
	int pn_svw = pin_number_of_border_step (svw);

        // removed by W.D. on October 2002 
	//
	//if ((pn_swv != pn_svu) || (pn_suv != pn_svw))
	//  	{
	//	cout << "\nwelding operation of gdtnode" 
	//	     << id(v) << "failed: inconsistent pin numbers (" 
	//	     << pn_swv << " != " 
	//	     << pn_svu << " || " 
	//	     <<  pn_suv << " != " 
	//	     << pn_svw <<")" << endl;
	//	gdt_error("invalid pin_number");
	//	}
        // ### End import

	bool change_ref_from_u = false;
	bool change_ref_from_w = false;
	border_step rbs = get_ref_border_step(); 
	
	if (rbs == swv || rbs == svu) 	   change_ref_from_w = true;
	else if (rbs == suv || rbs == svw) change_ref_from_u = true; 

	//Added by Marcandalli 16 10 2001	
	undefine(euv);
	undefine(ewv);
	//
	gdtedge e = plan_undi_graph::weld_edges_at_node(v);	// create the new gdtedge
	
	border_step suw = border_step_moving_along_edge (e,u);
	border_step swu = border_step_moving_along_edge (e,w);
	
	
	// update ref border step if it is needed
	// 
	if (change_ref_from_w) 	    ref_border_step = swu;
	else if (change_ref_from_u) ref_border_step = suw;
	
	
	// Calculate angles and bends of the border_steps
	// along the new gdtedge.
	//
	gdt::gdtlist<bend_type> b_suw = b_suv; 
	gdt::gdtlist<bend_type> b_swu = b_swv;
	switch (a_suv)
		{
		case _090: b_suw.append(right); b_swu.append(left ); break;
		case _270: b_suw.append(left ); b_swu.append(right); break;
		case _360: gdt_error("invalid _360 angle at gdtnode with degree 2");
		default: break;
		}
	b_suw.conc(b_svw);
	b_swu.conc(b_svu);
	
	//	
	angle_type a_suw = a_svw;
	angle_type a_swu = a_svu;
	

	// Apply the calculated angles and bends
	// to the new gdtedge		
	//
	set_edge_shape
		(
		e,u,
		b_swu, b_suw,
		a_swu, a_suw
		);
	
	// set thickness
	//
	set_thickness_of_border_step (suw,t_suv);
	set_thickness_of_border_step (swu,t_swv);

	// set pin_number
	//
        // ### Begin import from GDT fork of W. Didimo on June, 18 2004
	set_pin_number_of_border_step (suw,pn_svw); // modified by W.D. October 2002
	set_pin_number_of_border_step (swu,pn_svu); // modified by W.D. October 2002
        // ### End import
	
	// check to remove
	//
	//assert(is_orthogonal ());
	
	return e;   
	}



	// -----------------------------------------------------
	// THE BODY OF THIS METHOD HAS BEEN REPLACED WITH THE
	// weld_edges_at_node ONE, WHICH IS MUCH FASTER 
	// WARNING: PIN_NUMBERs ARE NOT HANDLED - UPDATE THEM IF YOU 
	//          RESUME THIS CODE!!
	// -----------------------------------------------------
	/*
	// Description: join the two edges adjacent to  'v' and make 'v' a bend of the new gdtedge resulting from such join. When a _180 angle is at gdtnode 'v', no bend is placed on the new gdtedge.
	// Precondition: the degree of 'v' is 2.
	// Notes: -
	//
	gdtedge
	orth_plan_undi_graph:: 
replace_node_with_bend
	(gdtnode v)
	{
	//cout << "\n orth_plan_undi_graph::replace_node_with_bend" << id(v) << flush;
	int old_id;
	gdtnode v1,v2,rv;
	gdtedge e1,e2,e3,e;
	face fx,f1,f2,ef;
	angle_type a_v1_v2;
	angle_type a_v2_v1;
	angle_type a_v1_v, a_v_v2;
	angle_type a_v2_v, a_v_v1;
	border_step sv1,sv2;
	border_step s_v1_v, s_v_v2;
	border_step s_v2_v, s_v_v1;
	gdt::gdtlist<bend_type> b_v1_v2;
	gdt::gdtlist<bend_type> b_v2_v1;
	gdt::gdtlist<bend_type> b_v1_v, b_v_v2;
	gdt::gdtlist<bend_type> b_v2_v, b_v_v1;
	
	if (degree(v)!=2) gdt_error("degree of gdtnode is not 2");
	e1 = first_adj_edge (v);
	e2 = last_adj_edge  (v);
	
	if (id(e1) < id(e2)) old_id = id(e1);
	else		     old_id = id(e2);
	
	v1 = opposite(v,e1); 
	v2 = opposite(v,e2);
	e3 = cyclic_adj_pred(e2,v2);
	
	s_v1_v = border_step_moving_along_edge(e1,v1);
	s_v_v2 = border_step_moving_along_edge(e2,v );
	s_v2_v = border_step_moving_along_edge(e2,v2);
	s_v_v1 = border_step_moving_along_edge(e1,v );
	
	rv = NULL_NODE;
	if 	(s_v1_v == ref_border_step || s_v_v2 == ref_border_step) rv = v1;
	else if (s_v_v1 == ref_border_step || s_v2_v == ref_border_step) rv = v2;

		
	//  |  +              f1                   
	//  |  |                                   |
	//  | sv1                                  |
	// e0  |                                  e3
	//  |                                      |
	//  |  a_v_v1        a_v2_v                |
	//  |    <--s_v_v1--        <--s_v2_v--    |   
	//(v1)-------e1-------(v)-------e2--------(v2)
	//       --s_v1_v-->        --s_v_v2-->    |
	//                   a_v1_v        a_v_v2  |
	//                                         |
	//                                      |  |
	//                                     sv2 |
	//                                      |  |
	//                    f2                +  |
	

	// Faces f1,f2 will be first joined into
	// a new face fx, then fx will be splitted
	// into faces f3,f4 by the new gdtedge e
	// which will replace (i.e. join) edges e1,e2.

	// Save border_steps sv1,sv2 in order to
	// - retrieve the new face fx resulting from
	//   the join of faces f1,f2
	// - retrieve the faces (corresponding to f1,f2)
	//   resulting from the split of face fx
	//
	sv1 = cyclic_succ_border_step (s_v_v1);
	sv2 = cyclic_succ_border_step (s_v_v2);
	
	// Since both f1,f2 could be the external face,
	// save them now to properly update the external face
	// later (when necessary).
	//
	f1 = face_of_border_step (sv1);
	f2 = face_of_border_step (sv2);
	ef = external_face();
	
	int idf1 = id(f1);	// new code
	int idf2 = id(f2);	// new code
	
	// Save angles and bends of the border_steps
	// along the edges to be joined
	//
	a_v1_v = angle_of_border_step (s_v1_v);
	a_v_v2 = angle_of_border_step (s_v_v2);
	a_v2_v = angle_of_border_step (s_v2_v);
	a_v_v1 = angle_of_border_step (s_v_v1);
	b_v1_v = bends_of_border_step (s_v1_v);
	b_v_v2 = bends_of_border_step (s_v_v2);
	b_v2_v = bends_of_border_step (s_v2_v);
	b_v_v1 = bends_of_border_step (s_v_v1);
	
	int t_v1_v = thickness_of_border_step (s_v1_v),
	    t_v_v2 = thickness_of_border_step (s_v_v2),
	    t_v2_v = thickness_of_border_step (s_v2_v),
	    t_v_v1 = thickness_of_border_step (s_v_v1);
	    
	if ((t_v1_v != t_v_v2) || (t_v2_v != t_v_v1))
		gdt_error("invalid thickness");
	    
	
	// Delete the gdtnode and its adjacent edges.
	// As a result, the two faces previously separated
	// by edges e1,e2, are joined as a new face fx,
	// which can be retrieved from border_step sv1 (or sv2).
	// Face fx is required because plan_undi_graphs
	// only creates an gdtedge by splitting a face.
	//
	
	// -----------------------------------
	// make the new gdtedge e and update
	// directions, markers and constraints
	// -----------------------------------
		
	fx = face_of_border_step (sv1);
	e  = plan_undi_graph::new_edge(v1,e1,v2,e3);	// apply the plan new gdtedge
		
	if (rv) ref_border_step = border_step_moving_along_edge (e,rv);
	
	if      (v == start(e1) && v != start(e2)) make_directed (e,v2);
	else if (v == start(e2) && v != start(e1)) make_directed (e,v1);
	mark(e,markers(e1));
	update_constraints_after_merge_edges (e1,e2,e);
	
	// ----------------------------------
	// delete the original gdtnode and edges
	// ----------------------------------
	
	plan_undi_graph::del_node(v);
	assign_id(e,old_id);
	
	// ----------------------
	// update shape of gdtedge e
	// ----------------------
	
	// Calculate angles and bends of the border_steps
	// along the new gdtedge.
	//
	b_v1_v2 = b_v1_v;
	b_v2_v1 = b_v2_v;
	switch (a_v1_v)
		{
		case _090: b_v1_v2.append(right); b_v2_v1.append(left ); break;
		case _270: b_v1_v2.append(left ); b_v2_v1.append(right); break;
		case _360: gdt_error("invalid _360 angle at gdtnode with degree 2");
		default: break;
		}
	b_v1_v2.conc(b_v_v2);
	b_v2_v1.conc(b_v_v1);
	//	
	a_v1_v2 = a_v_v2;
	a_v2_v1 = a_v_v1;
	
	
	// Apply the calculated angles and bends
	// to the new gdtedge		
	//
	set_edge_shape
		(
		e,v1,
		b_v2_v1, b_v1_v2,
		a_v2_v1, a_v1_v2
		);
	
	border_step s_v1_v2 = border_step_moving_along_edge (e,v1);
	border_step s_v2_v1 = border_step_moving_along_edge (e,v2);
	
	set_thickness_of_border_step (s_v1_v2,t_v1_v);
	set_thickness_of_border_step (s_v2_v1,t_v_v1);
	
	// Both the old faces f1,f2 have been replaced
	// by new faces. If one of them was the external face,
	// ext_face now points to an unexisting face
	// and must be updated.
	//
	face nf1 = face_of_border_step (sv1);
	face nf2 = face_of_border_step (sv2);
	
	if      (f1==ef) ext_face = nf1;
	else if (f2==ef) ext_face = nf2;
	
	if (idf1 != id(nf1)) assign_id(nf1,idf1);	// new code
	if (idf2 != id(nf2)) assign_id(nf2,idf2);	// new code

	return e;
	}
	*/
	
	
	
	gdtedge
	orth_plan_undi_graph:: 
replace_node_with_bend
	(gdtnode v)
	{
	return weld_edges_at_node(v);
	}	
	


	// Description: detect the 'bend_number'th bend along gdtedge 'e' in the direction starting from gdtnode 'v1', then replace that bend with a new gdtnode and return it.
	// Precondition: 'v1' is an end-gdtnode of gdtedge 'e'.
	// Notes: -
	//
	void 
	orth_plan_undi_graph::
replace_bend_with_node
	(gdtedge e, gdtnode v1, int bend_number, gdt::gdtlist<gdtnode>& dummy_nodes, gdt::gdtlist<gdtedge>& dummy_edges)
	{
	//cout << "\nReplacing bend " << bend_number << " along gdtedge " << id(e) << "\n" << flush;

	gdtnode v,v2,rv; 
	gdtedge e1,e2;
	
	int bnum, old_id;
	gdt::list_item bpos;
	bend_type btype = straight;
	
	angle_type angle_e_v1;
	angle_type angle_e_v2;
	
	border_step step_v_v1;
	border_step step_v_v2;
	border_step step_v1_v;
	border_step step_v2_v;
	border_step step_v1_v2;
	border_step step_v2_v1;
	
	gdt::gdtlist<bend_type> bends_v_v1;
	gdt::gdtlist<bend_type> bends_v_v2;
	gdt::gdtlist<bend_type> bends_v1_v;
	gdt::gdtlist<bend_type> bends_v2_v;
	gdt::gdtlist<bend_type> bends_v1_v2;
	gdt::gdtlist<bend_type> bends_v2_v1;
	
	v2 = opposite(v1,e);
	
	step_v1_v2  = border_step_moving_along_edge (e,v1);
	step_v2_v1  = border_step_moving_along_edge (e,v2);
	
	// to correctly update the 
	// reference border step
	//	
	rv = NULL_NODE;						
	if      (step_v1_v2 == ref_border_step) rv = v1;
	else if (step_v2_v1 == ref_border_step) rv = v2;

	bends_v1_v2 = bends_of_border_step (step_v1_v2);
	bends_v2_v1 = bends_of_border_step (step_v2_v1);
	
	angle_e_v1  = angle_of_border_step (step_v2_v1);
	angle_e_v2  = angle_of_border_step (step_v1_v2);
	
	int thickness_v1_v2 = thickness_of_border_step (step_v1_v2);
	int thickness_v2_v1 = thickness_of_border_step (step_v2_v1); 
	int pin_number_v1_v2 = pin_number_of_border_step (step_v1_v2);
	int pin_number_v2_v1 = pin_number_of_border_step (step_v2_v1); 
	
	// Split bends_v1_v2
	// into bends_v1_v and bends_v_v2
	// on the bend at position bend_number
	//
	bnum = bend_number - 1;
	bpos = bends_v1_v2.get_item (bnum);
	bends_v1_v2.split (bpos,bends_v1_v, bends_v_v2);
	
	// Split bends_v2_v1
	// into bends_v2_v and bends_v_v1
	// on the bend at position bend_number
	//
	bnum = bends_v2_v1.length() - bend_number;
	bpos = bends_v2_v1.get_item (bnum);
	bends_v2_v1.split (bpos,bends_v2_v, bends_v_v1);

	// Remove bend at position bend_number
	// from the splitted sub-lists
	// and save its type
	// 
	if (!bends_v_v2.empty()) btype = bends_v_v2.pop();
	if (!bends_v_v1.empty())         bends_v_v1.pop();
	
	//cout << "\nEdge " << id(e) << " (" << id(v1) << "," << id(v2) << ")" << flush;
	
	// Split gdtedge e by creating a new gdtnode v on it.
	// Two new edges e1,e2 are created.
	// The gdtedge e1 will be labeled as e, in order to remember this label.
	//
	
	// ----------------------------------------------------------
	// make two new edges e1, e2 by splitting gdtedge e
	// and update directions, markers and constraints
	// (updating will be automatically done by the plan new_node) 
	// ----------------------------------------------------------
	
	old_id = id(e);

	//Added by Marcandalli 16 10 2001
	undefine(e);	
	//
	v  = plan_undi_graph::new_node(e);	// apply the plan new gdtnode

	//cout << " has been splitted" << flush;
	
	// Find e1 = (v1,v)
	// Find e2 = (v,v2)
	//
	e1 = first_adj_edge(v);
	e2 = last_adj_edge(v);
	if (opposite(v,e1)!=v1)
		{
		//
		// swap e1 and e2
		//
		gdtedge ex;
		ex = e1;
		e1 = e2;
		e2 = ex;
		}

	if (id(e1)>id(e2)) assign_id(e1,old_id);
	else               assign_id(e2,old_id);

	//cout << "\n into edges " << id(e1) << "," << id(e2) << flush;
	
	// -----------------------------
	// update shape of two new edges
	// -----------------------------

	// Retrieve the new border_steps 
	// along edges e1,e2.
	//
	step_v_v1 = border_step_moving_along_edge (e1,v);
	step_v_v2 = border_step_moving_along_edge (e2,v);
	step_v1_v = border_step_moving_along_edge (e1,v1);
	step_v2_v = border_step_moving_along_edge (e2,v2);
	
	if      (rv == v1) ref_border_step = step_v1_v;
	else if (rv == v2) ref_border_step = step_v2_v;
	
	// Set the bends along the 
	// border steps of  e1,e2.
	//
	set_bends_of_border_step (step_v_v1, bends_v_v1);
	set_bends_of_border_step (step_v_v2, bends_v_v2);
	set_bends_of_border_step (step_v1_v, bends_v1_v);
	set_bends_of_border_step (step_v2_v, bends_v2_v);
	
	// Set the angles of the new
	// border steps along edges e1,e2.
	//
	set_angle_of_border_step (step_v_v1, angle_e_v1);
	set_angle_of_border_step (step_v_v2, angle_e_v2);
	//	
	if (btype == right)
		{
		set_angle_of_border_step (step_v1_v,_090);
		set_angle_of_border_step (step_v2_v,_270);
		}
	else
		{
		set_angle_of_border_step (step_v1_v,_270);
		set_angle_of_border_step (step_v2_v,_090);
		}
		
	// Set the thickness of the new
	// border steps along edges e1,e2.
	//
	set_thickness_of_border_step (step_v1_v, thickness_v1_v2);
	set_thickness_of_border_step (step_v_v2, thickness_v1_v2);	
	set_thickness_of_border_step (step_v2_v, thickness_v2_v1);
	set_thickness_of_border_step (step_v_v1, thickness_v2_v1);
		
	// Set the pin_number of the new
	// border steps along edges e1,e2.
	//
	set_pin_number_of_border_step (step_v1_v, pin_number_v1_v2);
	set_pin_number_of_border_step (step_v_v2, pin_number_v1_v2);	
	set_pin_number_of_border_step (step_v2_v, pin_number_v2_v1);
	set_pin_number_of_border_step (step_v_v1, pin_number_v2_v1);
		
	// Save dummy nodes and edges
	// 
	dummy_nodes.append(v);
	dummy_edges.append(e1);
	dummy_edges.append(e2);
	
	// In case the initial gdtedge 'e' was a dummy itself
	//
	gdt::list_item ep = dummy_edges.search(e);
	if (ep) dummy_edges.del_item(ep);
	}
	
	
	

	void
	orth_plan_undi_graph::
replace_bends_with_nodes
	(gdtedge e, gdt::gdtlist<gdtnode>& dummy_nodes, gdt::gdtlist<gdtedge>& dummy_edges)
	{
	//cout << "\nReplacing bends along gdtedge " << id(e) << "\n" << flush;

	gdtedge e2;
	gdtnode v,v1,v2;
	gdt::gdtlist<bend_type> bends;

	v1 = source(e);
	
	while (number_of_bends(e)>0)
		{
		v2 = opposite(v1,e);
		
		// replace the first bend
		// found along gdtedge e=(v1,v2) while walking from v1 to v2
		// with a new gdtnode v,
		// then save v into the nodes_added list.
		// 
		replace_bend_with_node (e,v1,1,dummy_nodes,dummy_edges);
				
		// Find the two edges e1=(v1,v) and e2=(v,v2)
		// which have replaced gdtedge e=(v1,v2).
		//
		v = dummy_nodes.tail();
		e2 = last_adj_edge(v);
		if (opposite(v,e2) != v2) e2 = first_adj_edge(v);
		
		//cout << "\nThe number of bends on gdtedge " << id(e2) << " is now " << number_of_bends(e2) << "\n" << flush;
		
		// Prepare for iteration
		//
		e  = e2;
		v1 = v;
		}
	}
	
	
	
	void
	orth_plan_undi_graph::
replace_bends_with_nodes
	(face f, gdt::gdtlist<gdtnode>& dummy_nodes, gdt::gdtlist<gdtedge>& dummy_edges)
	{
	gdtedge e;
	gdt::gdtlist<gdtedge> border = adj_edges(f);
	forall (e, border) replace_bends_with_nodes (e,dummy_nodes,dummy_edges);
	}
	
	
	
	void
	orth_plan_undi_graph::
replace_bends_with_nodes
	(gdt::gdtlist<gdtnode>& dummy_nodes, gdt::gdtlist<gdtedge>& dummy_edges)
	{
	gdtedge e;
	gdt::gdtlist<gdtedge> initial_edges = all_edges();
	forall (e,initial_edges)
		replace_bends_with_nodes(e,dummy_nodes,dummy_edges);
	}	
	


	// Description: adds dummy edges and nodes in order to make all the faces rectangular.
	// Preconditions : -
	// Notes: -
	//	
	void
	orth_plan_undi_graph::
decompose_all_faces_into_smaller_rectangular_faces 
	(gdt::gdtlist<gdtnode>& dummy_nodes, gdt::gdtlist<gdtedge>& dummy_edges)
	{
	face f;
	gdt::gdtlist<face> faces = all_faces();
	replace_bends_with_nodes(dummy_nodes, dummy_edges);
	forall (f,faces) remove_LRR_sequences_from_border_of_face (f, dummy_nodes, dummy_edges); 
	remove_LRL_sequences_from_border_of_external_face(dummy_nodes, dummy_edges);
	}



	int
	orth_plan_undi_graph::
regularize_all_faces
	(gdt::gdtlist<gdtnode>& dummy_nodes, gdt::gdtlist<gdtedge>& dummy_edges, algorithm_type alg)
	{
	int num_irr_faces = 0;
	face f;
	gdt::gdtlist<face> faces = all_faces();
	replace_bends_with_nodes(dummy_nodes, dummy_edges);
	forall (f,faces) if (regularize_face (f, dummy_nodes, dummy_edges, alg)) num_irr_faces++;	
	
	//cout << "\nORHTOGONAL AFTER REGULARIZATION : " << flush;
	//print(COMPLETE); cout << flush;
	
	// CHECKING 
	// --------
	//
	//if (!is_orthogonal())
	//	gdt_error("no orthogonal obtained");
	//
	
	return num_irr_faces;
	}



	void
	orth_plan_undi_graph::	
clear
	()
	{
	plan_undi_graph::clear();
	local_renew();
	}


			
	void
	orth_plan_undi_graph::
steal_from 
	(undi_graph& ug, algorithm_type alg, bool err_mess) 
	{
	plan_undi_graph pug;
	pug.steal_from(ug);
	steal_from (pug,pug.first_face(),alg,err_mess);
	}
	


	void
	orth_plan_undi_graph::
steal_from 
	(plan_undi_graph& pug, algorithm_type alg, bool err_mess)
	{
	steal_from (pug,pug.first_face(),alg,err_mess);
	}
	
	
	
	void
	orth_plan_undi_graph::
steal_from 
	(plan_undi_graph& pug, face ef, algorithm_type alg, bool err_mess) 
	{
	//cout << "\n plan_undi_graph -(move)-> orth_plan_undi_graph" << flush;
	clear();
	//
	// Copy all the pointers of pug
	// that is, the plan_undi_graph part of this class
	//
	plan_undi_graph::mirror (pug);
	//
	// Build the extra part (i.e. the orthogonal lists) of this,
	// according to the plan_undi part stolen from pug.
	// 
	local_renew();
	if (ef) ef = corresponding_face(ef,pug);
	else 	ef = first_face();
	local_init (ef,alg,err_mess);
	
	//
	// Create a new empty local_environment for pug
	// in order to avoid side-effects with this
	//
	pug.forget ();
	}
	
		
	
	void	
	orth_plan_undi_graph::
mirror	
	(orth_plan_undi_graph& opug)
	{
	face                                            p1;
	algorithm_type                                  p2;
	gdt::gdtedge_map<bend_constraint>                       p3;
	gdt::gdtmap<border_step,struct_orth_border_step_info>*  p4;
	bool                                            p5;
	bool                                            p6;
	border_step					p7;
	int						p8 = 0;
	
	plan_undi_graph::mirror (opug);
	opug.local_get(p1,p2,p3,p4,p5,p6,p7,p8);
	local_set(p1,p2,p3,p4,p5,p6,p7,p8);
	}
	
	
	
	void
	orth_plan_undi_graph::
forget
	()
	{
	plan_undi_graph::forget();
	local_new();
	}



	void
	orth_plan_undi_graph::
set_external_face
	(face ef)
	{	
	if (ef != ext_face)
		{
		if (ef) ext_face=ef;
		else    ext_face=first_face();
		apply_layout_algorithm();
		}
	}
	
	
	
	void
	orth_plan_undi_graph::
set_reference_border_step
	(
	border_step s
	)
	{
	ref_border_step  = s;
	}
	


	void
	orth_plan_undi_graph::
set_reference_border_step
	(
	gdtnode v,
	gdtedge e
	)
	{
	border_step s = border_step_moving_along_edge (e,v);
	ref_border_step = s;
	}

	
	
	void
	orth_plan_undi_graph::
set_layout_algorithm
	(algorithm_type la)
	{
	if (la != layout_algorithm)
		{
		layout_algorithm = la;
		apply_layout_algorithm();
		}
	}
	


	int
	orth_plan_undi_graph::
apply_layout_algorithm
	(bool err_mess)
	{
	int cost = 0;
	if (!local_consistency_is_suspended)
		{
		update_constraint_internal_structures();
		switch (layout_algorithm)
			{
			case PLAN_ORTH_QUICK   : quick_orth_for_fixed_embedding()  			   ; break;
			case PLAN_ORTH_OPTIMAL : cost = optimal_orth_for_fixed_embedding_with_constraints(); break;
			case PLAN_ORTH_SLOW    : cost = slow_orth ()					   ; break;
			default: break;
			}			
					
		if (err_mess)	// return a message error for no flow feasibility
			{
			if (cost == INFINITE) 
				gdt_error("no flow feasibility");
			else if (!is_orthogonal())
				{
				cout << "ORTH-PLAN-UNDI-GRAPH:" << flush;
				print(COMPLETE);
				gdt_error("not orthogonal obtained");
				}
				
			}
			
		else 		// do not return a message error for no flow feasibility
			{
			if ((cost < INFINITE) && (!is_orthogonal()))
				gdt_error("not correct orthogonal obtained");
			}
		}
	last_cost = cost;
	return cost;
	}
	
	
	
	void
	orth_plan_undi_graph::
suspend_local_consistency
	()
	{
	local_consistency_is_suspended = true;
	}



	void
	orth_plan_undi_graph::
restore_local_consistency
	()
	{
	local_consistency_is_suspended = false;
	apply_layout_algorithm();
	}


	void
	orth_plan_undi_graph::
enable_diagnostic_printouts
	()
	{
	diagnostic_printouts_are_enabled = true;
	}


	void
	orth_plan_undi_graph::
disable_diagnostic_printouts
	()
	{
	diagnostic_printouts_are_enabled = false;
	}	
	 	

	// -------------------------
	// Input / output operations
	// -------------------------

	void
	orth_plan_undi_graph::
print
	(print_mode mode, ostream& os) const 
	{
	switch (mode)
		{
		case BY_NODES:			
		case BY_EDGES:
			plan_undi_graph::print(mode,os);
			break;
		case BY_FACES:
			{
			face f;
			forall_faces(f,(*this))
				{
				os << "\n" << flush;
				print (f,os);
				}
			}
			break;
		case COMPLETE:
			{
			plan_undi_graph::print(BY_NODES,os);
			plan_undi_graph::print(BY_EDGES,os);
			os << "\nconstraint list : ";
			print_constraints();
			print(BY_FACES,os);
			}
			break;
		default:
			break;
		}
		
	}



	void
	orth_plan_undi_graph::
print
	(gdtnode v, ostream& os) const 
	{
	plan_undi_graph::print(v,os);
	}
		
	
		
	void
	orth_plan_undi_graph::
print
	(gdtedge e, ostream& os) const
	{
	plan_undi_graph::print(e,os);
	}
 
 
 
	void
	orth_plan_undi_graph::
print
	(face f, ostream& os) const
	{
	bend_type b;
	border_step s,ps;
	std::string bends, angle;

	os << "<" << id(f) << ">";
	if (f==ext_face) os << " ext : ";
	else             os << " int : ";

	forall_face_border_steps(s,f)
		{
		// Init the bends string
		//
		bends = "";
		
		gdt::gdtlist<bend_type> B=bends_of_border_step(s);
		
		forall (b,B)
			{
			if   (b == left) bends += "L";
			else   		 bends += "R";
			}
		
		if (bends == "") bends = "-";

		// Init the angle string
		//
		ps = cyclic_pred_border_step(s);
		switch (angle_of_border_step(ps))
			{
			case _000: angle = "S"; break;
			case _090: angle = "R"; break;
			case _180: angle = "-"; break;
			case _270: angle = "L"; break;
			case _360: angle = "B"; break;
			}
		
		print(start_of_border_step(s));
		os << angle  << " -"<< bends << "- ";
		}
	}
 
 
 
 	
 	void
 	orth_plan_undi_graph::
print 
 	(constraint c, ostream& os) const
 	{
 	plan_undi_graph::print(c,os);
 	}
 	
 
 
 
	void
	orth_plan_undi_graph::
print 
	(separation_pair sp, ostream& os) const
	{
	plan_undi_graph::print (sp,os);
	}	

 
 
 
//-----------------------------------------------------------------------------
//	PROTECTED
//-----------------------------------------------------------------------------

 	// Description: Splits an internal/external face with a new gdtedge with a given shape
	// Note       : FUNCTION FOR 4-PLANAR ONLY
	gdtedge
	orth_plan_undi_graph:: 
split_face_with_shaped_edge
	(
	gdtnode v, gdtedge ev, angle_type av,
	gdtnode w, gdtedge ew, angle_type aw,
	std::string bends_sequence,
	int new_id
	)
	{
	//cout << "\north_plan_undi_graph::split_face_with_shaped_edge" << flush;
	gdtedge e;
	face f, fv, fw;
	bool f_was_external;
	angle_type avi, av1, av2;
	angle_type awi, aw1, aw2;
	border_step sv1, sv2, sv;
	border_step sw1, sw2, sw;
	gdt::gdtlist<border_step> seqv, seqw;

	sv1  = border_step_moving_along_edge (ev,opposite (v,ev));
	sw1  = border_step_moving_along_edge (ew,opposite (w,ew));
	sv2  = cyclic_succ_border_step (sv1);
	sw2  = cyclic_succ_border_step (sw1);

	fv = face_of_border_step (sv1);
	fw = face_of_border_step (sw1);
	
	//Debug Marcandalli
	//cout << "Print prima dell'errore " << endl;
	//cout << "Facce incriminate " << id(fv) << " " << fv << " " << id(fw) << " " << fw << endl;
	//print(COMPLETE);
	//

	if (fv!=fw) gdt_error("invalid border_step pair");

	sv2  = cyclic_succ_border_step (sv1);
	sw2  = cyclic_succ_border_step (sw1);
	seqv = border_step_sequence (sv2,sw1);
	seqw = border_step_sequence (sw2,sv1);

	f    = fv;
	avi  = angle_of_border_step (sv1); // "i" stand for "initial" (i.e. before face splitting)
	awi  = angle_of_border_step (sw1); // "i" stand for "initial" (i.e. before face splitting)

	/*
	//    o-----ev----(v)-----------o
	//   /   --sv1->       --sv2->   \       
	//  /             av              \          
	//
        // seqw            f             seqv  
	//                                          
	//  \             aw              /           
	//   \   <-sw2--       <-sw1--   /          
	//    o-----------(w)----ew-----o
	*/

	//print (f);
	//cout << "\n. v     " << id(v);
	//cout << "\n. w     " << id(w);
	//cout << "\n. ev    " << id(ev);
	//cout << "\n. ew    " << id(ew);
	//cout << "\n. av    " << av;
	//cout << "\n. aw    " << aw;
	//cout << "\n. bends " << bends_sequence;
	//cout << "\n\n\n" << flush;


	////////////////////////////////////////////////
	// WARNING: check angular space more carefully 
	////////////////////////////////////////////////


	// Verify the angular space required at each end_node
	// in order to preserve the orthogonal embedding
	//
	if (avi==_090 || awi==_090)
		gdt_error("no angular space");
	    	
	//cout << "\nSplitting face " << flush;
	//print(f);
	//cout << flush;

	f_was_external = (f==ext_face);
	
	// Split face f
	// using a method of the parent class.
	//
	// ev = edge_of_border_step  (sv1);
	// ew = edge_of_border_step  (sw1);
	e  = plan_undi_graph::new_edge(v,ev, w,ew, new_id);
	
	// Retrieve the two faces
	// resulting from the split.
	//
	sv = border_step_moving_along_edge (e,v);
	sw = border_step_moving_along_edge (e,w);
	set_thickness_of_border_step (sv,1);
	set_thickness_of_border_step (sw,1);
	set_pin_number_of_border_step (sv,UNDEFINED_PIN_NUMBER);
	set_pin_number_of_border_step (sw,UNDEFINED_PIN_NUMBER);
	
	fv = face_of_border_step (sv);
	fw = face_of_border_step (sw);

	/*
	//    o-----------(v)-----------o
	//   /   --sv1->   |   --sv2->   \       
	//  /          av1 |  av2         \          
	//                 |                   
        // seqw   fv       e      fw     seqv
	//                 |            
	//  \          aw2 |  aw1         /           
	//   \   <-sw2--   |   <-sw1--   /          
	//    o-----------(w)-----------o
	*/


	// av1 and aw2 are given as parameters
	// av2 and aw1 derive as follows.
	//
	av1 = av;
	aw1 = aw;
	av2 = (angle_type)(avi-av1);
	aw2 = (angle_type)(awi-aw1);

	//cout << "\n Angle av  is " << av;
	//cout << "\n Angle aw  is " << aw;
	//cout << "\n Angle av1 is " << av1;
	//cout << "\n Angle av2 is " << av2;
	//cout << "\n Angle aw1 is " << aw1;
	//cout << "\n Angle aw2 is " << aw2;
		
	set_edge_shape (e,v,bends_sequence,av2,aw2);

	// Adjust the angles along the border_steps
	// of the faces fv and fw entering nodes v and w.
	//
	set_angle_of_border_step (sv1, av1);
	set_angle_of_border_step (sw1, aw1);
	
	if (f_was_external) 
		{
		int rt = number_of_right_turns_around_face (fv);
		int lt = number_of_left_turns_around_face  (fv);
		int dt = rt-lt;
		if (dt==-4) ext_face = fv;
		else	    ext_face = fw;
		}
	
	
		//{
		//int nl = number_of_left_bends  (sv);
		//int nr = number_of_right_bends (sv);
		//if (nr>nl)
			
			//                 o-----------------------o                   
			//                 |        --sv->         |
			//             av2 | av1                   |
			//    o-----------(v)-----------o          |
			//   /   <-sv2--       <-sv1--   \    fv   |  
			//  /                             \        |   
			//                                         |
			// seqv                          seqw      e
			//                                         | 
			//  \                             /        |    
			//   \   --sw1->       --sw2->   /         | 
			//    o-----------(w)-----------o          |   
			//             aw1 |  aw2                  |
			//                 |                       |
			//         fw      o-----------------------o                   
			//                          --sw->           
			
			//ext_face = fw;
		//else
			
			//                                  
			// o-----------------------o                   
			// |         <--sv--       |                 
			// |                   av2 | av1                   
			// |          o-----------(v)-----------o          
			// |         /   <-sv2--       <-sv1--   \    fv     
			// |        /             av              \           
			// |                                        
			// e     seqv                            seqw      
			// |                                         
			// |        \                             /           
			// |         \   --sw1->       --sw2->   /          
			// |          o-----------(w)-----------o             
			// |  fw               aw1 |  aw2                  
			// |                       |                       
			// o-----------------------o                                 
			//          <-sw--
  			
		//ext_face = fv;
		//}

	//print(fv);
	//print(fw);

	// Now check the number of bends around face f
	//
	//if (!(border_is_closed(fv) && border_is_closed (fw)))
	//	gdt_error("malformed face");
	
	
	// CHECK TO REMOVE
	//
	if (!(border_is_closed(fv)))
		gdt_error("malformed face fv");

	if (!(border_is_closed(fw)))
		gdt_error("malformed face fw");

	return e;
	}



	void
	orth_plan_undi_graph:: 
split_list_of_edges_on_side 
	(gdt::gdtlist<gdtedge>& L, gdtnode v, gdt::gdtlist<gdtedge>& L_left, gdtedge& ec, gdt::gdtlist<gdtedge>& L_right)
	{
	gdtedge e;
	border_step s;
	gdt::gdtlist<bend_type> bs;
	bend_type b;
	
	L_left.clear();
	L_right.clear();
	ec = NULL_EDGE;
	
	forall (e,L)
		{
		s  = border_step_moving_along_edge (e,v);
		bs = bends_of_border_step (s);
		if (!bs.empty()) 
			{
			// the gdtedge-center is the first gdtedge turning to left if any, 
			// or the last gdtedge of L otherwise.
			//
			b = bs.head();
			if (b == right && e != L.tail()) L_right.push(e);
			else if (!ec) ec = e;					
		     	     else if (b == left) L_left.append(e);
			  	  else  gdt_error("invalid list");
			}
		else ec = e;
		}
	}
	
	
	void
	orth_plan_undi_graph::
merge_edges_on_same_side
	(gdtnode v)
	{
	//cout << "\nSTARTING COMPRESSION EDGES OF NODE " << id(v) << flush;
	
	// --------------------------------
	//       1                 1
	//       |                 | 
	// 2 --o | o-- 3       2 --O-- 3
	//     | | |               |
	//     | | |               |
	//    -------  	        -------
	//    |     |   --->    |     |
	//    |  v  |           |  v  |
	//    |     |           |     |
	//    -------           -------
	// --------------------------------
	
	gdt::gdtlist<gdtnode> bend_nodes;		// list of the bend-nodes added
	gdt::gdtlist<gdtedge> L1,L2,L3,L4;		// lists of the edges on each side
	int        left_thickness,	
		   right_thickness;
	
	edges_on_each_side_of_node (v,L1,L2,L3,L4);
	int i;
	gdt::gdtlist<gdtedge> L;
	for (i=1; i<=4; i++)
		{
		left_thickness  = 1;
		right_thickness = 1;
		
		switch (i)
			{
			case 1: L = L1; break;
			case 2: L = L2; break;
			case 3: L = L3; break;
			case 4: L = L4; break; 
			}
			
		if (L.size() > 1)
			{ 	
			gdt::gdtlist<gdtedge> L_right, 
			           L_left;
			gdtedge       e;
			gdtedge	   ec;	// central (straight) gdtedge
		
			split_list_of_edges_on_side (L,v,L_left,ec,L_right);
			
			// CHECK (TO REMOVE)
			// -----------------
			//cout << "\n  SIDE " << i << flush;
			//cout << "\n    L_left = " << flush; forall (e,L_left)  cout << " " << id(e) << flush;
			//cout << "\n    Center-Edge = " << id(ec) << flush;
			//cout << "\n    L_right= " << flush; forall (e,L_right) cout << " " << id(e) << flush;
			// -----------------
			
			// ----------------------------------------
			// Merge all the edges on the right of ec 
			// and an equal number of edges on the left
			// of ec, if any.
			// ----------------------------------------
		
			e = ec;
	
			while (!L_right.empty())
				{

				// ---------------------------------------------
				// merge the two edges e and er, by introducing 
				// a bend gdtnode u
				// ---------------------------------------------

				// ---------------------------------------------
				//
				//       <--s_e_inv                  
				//    o------------ v_e            o------- v_e
				//    | s_e-->                     |e2  
				//    |      <--s_er_inv           |     e3
				//   e| o--------------- v_er    u O------- v_er
				//    | |er  s_er-->               |e1  
				//  |-----|			|-----|
				//  |  v  |			|  v  |
				//  |-----|	      ==>	|-----|
				//
				// ---------------------------------------------

				int 	     id_e     = id(e); 
				gdtnode 	     v_e      = opposite(v,e); 
				border_step  s_e      = border_step_moving_along_edge(e,v);
				border_step  s_e_inv  = border_step_moving_along_edge(e,v_e);			
				gdtedge   er             = L_right.pop_front();
				int id_er             = id(er);
				gdtnode v_er 	      = opposite(v,er);
				border_step  s_er     = border_step_moving_along_edge(er,v);
				border_step  s_er_inv = border_step_moving_along_edge(er,v_er);
			
				// -----------------------------------------------------
				// store the bend-sequence and angles of the edges e, el
				// -----------------------------------------------------
			
				gdt::gdtlist<bend_type> b_s_e      = bends_of_border_step (s_e),
			 			b_s_e_inv  = bends_of_border_step (s_e_inv),
			 			b_s_er	   = bends_of_border_step (s_er),
			 			b_s_er_inv = bends_of_border_step (s_er_inv);
			 
				angle_type      a_s_e      = angle_of_border_step (s_e),
						a_s_e_inv  = angle_of_border_step (s_e_inv),
						a_s_er     = angle_of_border_step (s_er);
				

				// -----------------------------------------------------
				// store the pin_number information for gdtedge e 
				// -----------------------------------------------------

			
				int e_left_pin_number   = pin_number_of_border_step(s_e_inv);
				int e_right_pin_number  = pin_number_of_border_step(s_e);
				int er_left_pin_number  = pin_number_of_border_step(s_er_inv);
				int er_right_pin_number = pin_number_of_border_step(s_er);

			
				// -----------------------------------------------------
				// check if one of the border step of gdtedge e or er is the
				// reference one to make the correct update operation afterward
				// -----------------------------------------------------

				bool change_ref_e_from_v     = false;
				bool change_ref_e_from_v_e   = false;
				bool change_ref_er_from_v    = false;
				bool change_ref_er_from_v_er = false;
				
				if (ref_border_step == s_e) 	     change_ref_e_from_v   = true;
				else if (ref_border_step == s_e_inv) change_ref_e_from_v_e = true;
				
				if (ref_border_step == s_er) 	      change_ref_er_from_v    = true;
				else if (ref_border_step == s_er_inv) change_ref_er_from_v_er = true;

				// -----------------------------------------------------
				// now beginning the constructive part
				// -----------------------------------------------------
			
				//Added by Marcandalli 16 10 2001
				undefine(e);				
				//
				gdtnode u  = plan_undi_graph::new_node(e);  // bend gdtnode
				bend_nodes.append(u);
				
				gdtedge e1 = first_adj_edge (u);
				if (opposite(u,e1) != v) e1 = last_adj_edge (u);
				gdtedge e2 = cyclic_adj_succ (e1,u);
				
				// -----------------------------------------
				// make a new gdtedge e3 and update directions,
				// markers and constraints. Also update
				// shape of such gdtedge. 
				// -----------------------------------------
				
				gdtedge e3 = plan_undi_graph::new_edge (u,e1,v_er,cyclic_adj_pred(er,v_er));
				
				mark(e3,markers(er));
				update_constraints_after_edge_translation (er,v,v_er,e3,u,v_er);
				
				// ---------------------------------------------
				//
				//                 v_e          
				//       
				//                  |
				//                  |^   
				// e2      s2_inv  |||s2
				//                 V|      <--s3_inv      
				//                  |
				//             u    o------e3--------- v_er 
				//                  |
				//                  |^     s3-->
				// e1      s1_inv  |||s1
				//                 V|
				//                  |
				//               |-----|			
				//               |  v  |			
				//               |-----|
				//
				// ---------------------------------------------

				border_step s1     = border_step_moving_along_edge (e1,v),
				    	    s1_inv = border_step_moving_along_edge (e1,u),
				    	    s2     = border_step_moving_along_edge (e2,u),
			            	    s2_inv = border_step_moving_along_edge (e2,v_e),
				    	    s3     = border_step_moving_along_edge (e3,u),
				    	    s3_inv = border_step_moving_along_edge (e3,v_er);				
				
				
				// update the ref_border_step if needed
				//
				if      (change_ref_e_from_v) 		ref_border_step = s1;
				else if (change_ref_e_from_v_e)		ref_border_step = s2_inv;
				else if (change_ref_er_from_v)		ref_border_step = s1;
				else if (change_ref_er_from_v_er)	ref_border_step = s3_inv;
				//
									
				reset_border_step_info (s1);
				reset_border_step_info (s2);
				reset_border_step_info (s3);
				reset_border_step_info (s1_inv);
				reset_border_step_info (s2_inv);
				reset_border_step_info (s3_inv);
	
				set_angle_of_border_step (s1    , _090)     ;
				set_angle_of_border_step (s1_inv, a_s_e_inv);
				set_angle_of_border_step (s2    , a_s_e)    ;
				set_angle_of_border_step (s2_inv, _180)     ;
				set_angle_of_border_step (s3    , a_s_er)   ;
				set_angle_of_border_step (s3_inv, _090)     ;
				
				// the first bend will be replaced with a bend-gdtnode
				//
				b_s_er.pop_front();
				b_s_er_inv.pop_back();
				
				set_bends_of_border_step (s2    , b_s_e)     ;
				set_bends_of_border_step (s2_inv, b_s_e_inv) ;
				set_bends_of_border_step (s3    , b_s_er)    ;
				set_bends_of_border_step (s3_inv, b_s_er_inv);
				
				set_thickness_of_border_step (s2    ,right_thickness);
				set_thickness_of_border_step (s2_inv,left_thickness);
				right_thickness++;
				
				set_pin_number_of_border_step (s2    , e_right_pin_number) ;
				set_pin_number_of_border_step (s2_inv, e_left_pin_number)  ;
				set_pin_number_of_border_step (s1    , er_left_pin_number) ;
				set_pin_number_of_border_step (s1_inv, e_left_pin_number)  ;
				set_pin_number_of_border_step (s3    , er_right_pin_number);
				set_pin_number_of_border_step (s3_inv, er_left_pin_number) ;

				gdtnode v_s = start(er);
				if (v_s) if (v_s == v) make_directed(e3,u);
					 else	       make_directed(e3,v_er);
				
				//Added by Marcandalli 8 2001
				undefine(er);
				//
				plan_undi_graph::del_edge (er);	   	// the angles and bends on border steps 
									// incident v_er will be the same
								
				assign_id (e2,id_e);
				assign_id (e3,id_er);
				
				// --------------------------------------------- 
				// look for a left gdtedge to attach to bend gdtnode u
				// ---------------------------------------------
				
				if (!L_left.empty())	// we are in the case 1
					{
					// ----------------------------------------------
					//          |e2                    |e2
					//    el    |                     u|
					// O----o  uO----e3           e4---O---e3
					// v_el |   |                      |
					//      |   |e1       --->         |e1
					//     |-----|                 |-----|
					//     |  v  |                 |  v  |
					//     |-----|                 |-----|
					//
					// ----------------------------------------------
					
					gdtedge 		el          = L_left.pop_front();
					int  		id_el       = id(el);
					gdtnode            v_el        = opposite(v,el);
					border_step     s_el        = border_step_moving_along_edge (el,v);
					border_step	s_el_inv    = border_step_moving_along_edge (el,v_el);
			
					gdt::gdtlist<bend_type> b_s_el      = bends_of_border_step (s_el),
			 				b_s_el_inv  = bends_of_border_step (s_el_inv);
			 
					angle_type      a_s_el      = angle_of_border_step (s_el),
							a_s_el_inv  = angle_of_border_step (s_el_inv);
					
						
					int el_right_pin_number = pin_number_of_border_step (s_el);
					int el_left_pin_number  = pin_number_of_border_step (s_el_inv); 

					//
					bool change_ref_el_from_v    = false;
					bool change_ref_el_from_v_el = false;
	
					if (ref_border_step == s_el) 	      change_ref_el_from_v    = true;
					else if (ref_border_step == s_el_inv) change_ref_el_from_v_el = true;
					//
					
					// -----------------------------------------
					// make a new gdtedge e4 and update directions,
					// markers and constraints. Also update the
					// shape of such gdtedge. 
					// -----------------------------------------
					
					//face fu = left_face_moving_along_edge (e2,u);
					
					gdtedge e4 = plan_undi_graph::new_edge (u,e2,v_el,el);
					mark (e4,markers(el));
					update_constraints_after_edge_translation (el,v,v_el,e4,u,v_el);
					
					//if (fu == external_face()) ext_face = right_face_moving_along_edge(e4,u);
					
					border_step s4     = border_step_moving_along_edge (e4,u);
					border_step s4_inv = border_step_moving_along_edge (e4,v_el);
					
					// update the ref_border_step if it is needed
					//
					if (change_ref_el_from_v) 	  ref_border_step = s1;
					else if (change_ref_el_from_v_el) ref_border_step = s4_inv;
					//
					
					reset_border_step_info (s4);
					reset_border_step_info (s4_inv);
					
					set_angle_of_border_step (s1_inv, a_s_el_inv);
					set_angle_of_border_step (s2_inv, _090);
					set_angle_of_border_step (s4    , a_s_el);
					set_angle_of_border_step (s4_inv, _090);
					
					// a bend will be replaced with a bend gdtnode
					//
					b_s_el.pop_front();
					b_s_el_inv.pop_back();
					
					set_bends_of_border_step (s4    ,b_s_el);
					set_bends_of_border_step (s4_inv,b_s_el_inv);
					
					left_thickness ++;
					
					set_pin_number_of_border_step(s1_inv, el_left_pin_number);
					set_pin_number_of_border_step(s4    , el_right_pin_number);
					set_pin_number_of_border_step(s4_inv, el_left_pin_number);

					gdtnode v_s = start(el);
					if (v_s) if (v_s == v) make_directed(e4,u);
					else	       	       make_directed(e4,v_el);

					//Added by Marcandalli 16 10 2001
					undefine(el);					
					//
					plan_undi_graph::del_edge (el); 	   // the angles and bends on border steps incident v_er will be the same
					
				
					assign_id (e4,id_el);
					}
					
				e  = e1;
				}
			
			// -----------------------------------------------
			// Merge all the edges remaining on the left of ec
			// -----------------------------------------------
	
			while (!L_left.empty())
				{
				int 	     id_e     = id(e); 
				gdtnode 	     v_e      = opposite(v,e); 
				border_step  s_e      = border_step_moving_along_edge(e,v);
				border_step  s_e_inv  = border_step_moving_along_edge(e,v_e);			
				gdtedge   el             = L_left.pop_front();
				int id_el             = id(el);
				gdtnode v_el 	      = opposite(v,el);
				border_step  s_el     = border_step_moving_along_edge(el,v);
				border_step  s_el_inv = border_step_moving_along_edge(el,v_el);
			
				// ------------------------------------------------
				// store the bend-sequence and angles of the gdtedge e 
				// ------------------------------------------------
			
				gdt::gdtlist<bend_type> b_s_e      = bends_of_border_step (s_e),
			 			b_s_e_inv  = bends_of_border_step (s_e_inv),
			 			b_s_el	   = bends_of_border_step (s_el),
			 			b_s_el_inv = bends_of_border_step (s_el_inv);

				angle_type      a_s_e      = angle_of_border_step (s_e),
						//a_s_e_inv  = angle_of_border_step (s_e_inv),
						a_s_el     = angle_of_border_step (s_el),
						a_s_el_inv = angle_of_border_step (s_el_inv);

				// -----------------------------------------------------
				// store the pin_number information for gdtedge e 
				// -----------------------------------------------------
			
				int e_left_pin_number   = pin_number_of_border_step(s_e_inv);
				int e_right_pin_number  = pin_number_of_border_step(s_e);
				int el_left_pin_number  = pin_number_of_border_step(s_el_inv);
				int el_right_pin_number = pin_number_of_border_step(s_el);
		
				
				//
				bool change_ref_e_from_v     = false;
				bool change_ref_e_from_v_e   = false;
				bool change_ref_el_from_v    = false;
				bool change_ref_el_from_v_el = false;
				
				if (ref_border_step == s_e) 	     change_ref_e_from_v   = true;
				else if (ref_border_step == s_e_inv) change_ref_e_from_v_e = true;
				
				if (ref_border_step == s_el) 	      change_ref_el_from_v    = true;
				else if (ref_border_step == s_el_inv) change_ref_el_from_v_el = true;
				//
				
				// ---------------------------------------------
				// merge the two edges e and er, by introducing 
				// a bend gdtnode u (symmetric of case 1).
				// ---------------------------------------------
			
				gdtnode u  = plan_undi_graph::new_node(e);  // bend gdtnode
				bend_nodes.append(u);

				gdtedge e1 = first_adj_edge (u);
				if (opposite(u,e1) != v) e1 = last_adj_edge (u);
				gdtedge e2 = cyclic_adj_succ (e1,u);
				
				// -----------------------------------------
				// make a new gdtedge e3 and update directions,
				// markers and constraints. Also update the
				// shape of such gdtedge. 
				// -----------------------------------------
				
				gdtedge e3 = plan_undi_graph::new_edge (u,e2,v_el,el);
				mark (e3,markers(el));
				update_constraints_after_edge_translation (el,v,v_el,e3,u,v_el);
				
				border_step s1     = border_step_moving_along_edge (e1,v),
				    	    s1_inv = border_step_moving_along_edge (e1,u),
				    	    s2     = border_step_moving_along_edge (e2,u),
			            	    s2_inv = border_step_moving_along_edge (e2,v_e),
				    	    s3     = border_step_moving_along_edge (e3,u),
				    	    s3_inv = border_step_moving_along_edge (e3,v_el);
					    
				// update the ref_border_step if it is needed
				//
				if      (change_ref_e_from_v) 		ref_border_step = s1;
				else if (change_ref_e_from_v_e)		ref_border_step = s2_inv;
				else if (change_ref_el_from_v)		ref_border_step = s1;
				else if (change_ref_el_from_v_el)	ref_border_step = s3_inv;
				//
				
				reset_border_step_info (s1);
				reset_border_step_info (s2);
				reset_border_step_info (s3);
				reset_border_step_info (s1_inv);
				reset_border_step_info (s2_inv);
				reset_border_step_info (s3_inv);
				
				set_angle_of_border_step (s1    , _180)      ;
				set_angle_of_border_step (s1_inv, a_s_el_inv);
				set_angle_of_border_step (s2    , a_s_e)     ;
				set_angle_of_border_step (s2_inv, _090)      ;
				set_angle_of_border_step (s3    , a_s_el)    ;
				set_angle_of_border_step (s3_inv, _090)      ;
				
				// a bend will be replaced with a bend-gdtnode
				//
				b_s_el.pop_front ();
				b_s_el_inv.pop_back();
				
				set_bends_of_border_step (s2    , b_s_e)     ;
				set_bends_of_border_step (s2_inv, b_s_e_inv) ;
				set_bends_of_border_step (s3    , b_s_el)    ;
				set_bends_of_border_step (s3_inv, b_s_el_inv);
				
				set_thickness_of_border_step (s2    ,right_thickness);
				set_thickness_of_border_step (s2_inv,left_thickness);
				left_thickness ++;

				set_pin_number_of_border_step (s2    , e_right_pin_number) ;
				set_pin_number_of_border_step (s2_inv, e_left_pin_number)  ;
				set_pin_number_of_border_step (s1    , e_right_pin_number) ;
				set_pin_number_of_border_step (s1_inv, el_left_pin_number) ;
				set_pin_number_of_border_step (s3    , el_right_pin_number);
				set_pin_number_of_border_step (s3_inv, el_left_pin_number) ;

				gdtnode v_s = start(el);
				if (v_s) if (v_s == v) make_directed(e3,u);
					 else	       make_directed(e3,v_el);
				
				//Added by Marcandalli 16 10 2001
				undefine(el);				
				//
				plan_undi_graph::del_edge (el); 	   // the angles and bends on border steps incident v_er will be the same				
				
				assign_id (e2,id_e);
				assign_id (e3,id_el);
				
				e = e1;
				}
				
			border_step s     = border_step_moving_along_edge (e,v);
			border_step s_inv = border_step_moving_along_edge (e,opposite(v,e));
			set_thickness_of_border_step (s,right_thickness);
			set_thickness_of_border_step (s_inv,left_thickness);
			}
		}
	
	// -------------------------------------------------
	// Mark all bend nodes and set the new external face
	// -------------------------------------------------
	//
	gdtnode u;
	face f;
	forall (u,bend_nodes) mark(u,RM3_BEND_NODE);
	forall_faces (f,*this)
		{
		int tr = number_of_right_turns_around_face(f);
		int tl = number_of_left_turns_around_face (f);
		if ((tl - tr)==4) {ext_face = f; break;}
		}
	
	// CHECK (TO REMOVE)
	// -----------------
	//cout << "\nCOMPRESSION COMPLETED" << flush;
	//print(COMPLETE); cout << "\n" << flush;
	// -----------------
		
	// if the ref_border_step has been lost, during the merging
	// reupdate it and print a WARNING
		{
		bool found_ref_border_step = false;
		face f;
		border_step s;
		forall_faces (f,*this)
			{
			forall_face_border_steps (s,f)
				if (ref_border_step == s) found_ref_border_step = true;
				
			if (found_ref_border_step) break;
			}	
				
		if (!found_ref_border_step) 
			{
			//cout << "\nWARNING. The reference border step has been lost. CHANGED." << flush;
			ref_border_step = first_border_step(first_face());
			}
		}		
	}

	

//-----------------------------------------------------------------------------
//	PRIVATE
//-----------------------------------------------------------------------------

	void
	orth_plan_undi_graph::
local_new
	()
	{
	//cout << "\n orth_plan_undi_graph::local_new" << flush;
	local_consistency_is_suspended 	 = false;
	diagnostic_printouts_are_enabled = false;
	border_step_info 		 = new (gdt::gdtmap<border_step,struct_orth_border_step_info>);
	constraint_on_bends_of_edge.init (nodes_and_edges(),MINIMAL);
	ref_border_step = NULL_BORDER_STEP;
	}
	
	

	void
	orth_plan_undi_graph::
local_del
	()
	{
	//cout << "\n orth_plan_undi_graph::local_del" << flush;
	safe_delete (border_step_info);
	}
	
	
	
	void
	orth_plan_undi_graph::
local_renew
	()
	{
	bool tmp1 = local_consistency_is_suspended;
	bool tmp2 = diagnostic_printouts_are_enabled;
	local_del();
	local_new();
	local_consistency_is_suspended    = tmp1;
	diagnostic_printouts_are_enabled = tmp2;
	}
	
	
	
	void
	orth_plan_undi_graph::
local_init
	(face ef, algorithm_type alg, bool err_mess)
	{
	if (number_of_faces()>0)
		{
		ext_face         = ef;		// set external face
		layout_algorithm = alg;		// set layout algorithm
		apply_layout_algorithm (err_mess);
		}

	ref_border_step = first_border_step (first_face()); // W.D.
	}
	
	
	
	void
	orth_plan_undi_graph::
local_init
	(const upwa_plan_undi_graph& upug0, algorithm_type alg)
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
	
	upwa_plan_undi_graph upug (upug0);
		
	gdtnode v;	// gdtnode of *this
	gdtnode u; // gdtnode of upug
	
	// reset all the border_steps informations
	//
		{
		face f;
		border_step s;
		forall_faces (f,*this)
			forall_face_border_steps (s,f)
				reset_border_step_info(s);	
		}
	//
	
	// set the external face
	//
	ext_face = corresponding_face (upug.external_face(),upug);
	
	// construct the orthogonal shape 
	// according to the selected algorithm
	//
	if (alg == PLAN_ORTH_UPWA_STRAIGHT_MIDDLE)
		{
		forall_nodes(v, *this)
			{
			u = upug.corresponding_node(v,*this);

			if (upug.is_marked(u,RM3_ADDED_BY_MAKE_UPWARD))
				{
				gdtedge e_left  = NULL_EDGE,
				     e_right = NULL_EDGE;

				if (upug.is_source(u)) 
					{
					// ----------------------------
					// 
					// 
					//  ^   ^        ^         ^
					//  |   |        |         |
					//   ---         |   ---   |
					//  | u |   -->  o--| v |--o    
					//  |   |           |   | 
					//   ---             ---     
					//  
					// ----------------------------

					e_left  = upug.first_out_cand_edge (u);

					e_left  = corresponding_edge (e_left,upug);
					e_right = cyclic_adj_pred (e_left,v); 

					border_step s_left 	= border_step_moving_along_edge (e_left,v),
						    s_left_inv  = opposite_border_step (s_left),
				        	    s_right     = border_step_moving_along_edge (e_right,v),
				        	    s_right_inv = opposite_border_step (s_right);

					gdt::gdtlist<bend_type> s_left_bends      = bends_of_border_step (s_left), 
			    	     			s_left_inv_bends  = bends_of_border_step (s_left_inv),
							s_right_bends	  = bends_of_border_step (s_right),
							s_right_inv_bends = bends_of_border_step (s_right_inv);

					s_left_bends.push        (right); //
					s_left_inv_bends.append  (left) ; //

					s_right_bends.push       (left) ;
					s_right_inv_bends.append (right);

  					//set_angle_of_border_step (s_left_inv , _270);
					//set_angle_of_border_step (s_right_inv, _090);

					set_angle_of_border_step (s_left_inv , _180);
					set_angle_of_border_step (s_right_inv, _180);

  					set_bends_of_border_step (s_left     , s_left_bends);
  					set_bends_of_border_step (s_left_inv , s_left_inv_bends);
					set_bends_of_border_step (s_right    , s_right_bends);
					set_bends_of_border_step (s_right_inv, s_right_inv_bends);

					unmark (v,RM3_ADDED_BY_MAKE_UPWARD);
					mark (v,RM3_REPLACES_A_BEND);
					}				

				else
					{
					// ----------------------------
					// 
					//   ---               ---
					//  | u |    -->      | v |    
					//  |   |         o-->|   |<--o
					//   ---          |    ---    |
					//  ^   ^         |           |
 					//  |   |         |           |
					// 
					// ----------------------------	

					e_left  = upug.first_in_cand_edge (u);
					e_left  = corresponding_edge (e_left,upug);
					e_right = cyclic_adj_succ (e_left,v); 

					border_step s_left 	= border_step_moving_along_edge (e_left,opposite(v,e_left)),
						    s_left_inv  = opposite_border_step (s_left),
				        	    s_right     = border_step_moving_along_edge (e_right,opposite(v,e_right)),
				        	    s_right_inv = opposite_border_step (s_right);

					gdt::gdtlist<bend_type> s_left_bends      = bends_of_border_step (s_left), 
			    	     			s_left_inv_bends  = bends_of_border_step (s_left_inv),
							s_right_bends	  = bends_of_border_step (s_right),
							s_right_inv_bends = bends_of_border_step (s_right_inv);

					s_left_bends.append      (right); //
					s_left_inv_bends.push    (left) ; //

					s_right_bends.append     (left) ;
					s_right_inv_bends.push   (right);

  					//set_angle_of_border_step (s_left , _090);
					//set_angle_of_border_step (s_right, _270);

					set_angle_of_border_step (s_left , _180);
					set_angle_of_border_step (s_right, _180);

  					set_bends_of_border_step (s_left     , s_left_bends);
  					set_bends_of_border_step (s_left_inv , s_left_inv_bends);
					set_bends_of_border_step (s_right    , s_right_bends);
					set_bends_of_border_step (s_right_inv, s_right_inv_bends);

					unmark (v,RM3_ADDED_BY_MAKE_UPWARD);
					mark   (v,RM3_REPLACES_A_BEND);
					}

				}

			else if (upug.is_marked(u,RM3_CROSS_ON_REAL_EDGE))
				{
				// -------------------------------
				//  
				//   1 2                 1    2
				//   ^ ^                 ^    ^
				//   | |                 |    |
				//   ---                ---   |
				//  | u |    --->      | v |  |
				//  |   |          o-->|   |--o
				//   ---           |    ---    
				//   ^ ^           |     ^      old configuration (see below)
				//   | |           |     |	o----o
				//   3 4           3     |4          |4
				//
				// -------------------------------

				gdtedge e1 = upug.first_out_cand_edge (u);
				gdtedge e3 = upug.first_in_cand_edge  (u); 

				e1 = corresponding_edge(e1,upug);
				e3 = corresponding_edge(e3,upug);

				gdtedge e2 = cyclic_adj_pred(e1,v);
				gdtedge e4 = cyclic_adj_succ(e3,v);

				border_step s1     = border_step_moving_along_edge(e1,v),
					    s1_inv = opposite_border_step (s1),
					    s2     = border_step_moving_along_edge(e2,v),
					    s2_inv = opposite_border_step (s2),
					    s3     = border_step_moving_along_edge(e3,opposite(v,e3)),
					    s3_inv = opposite_border_step (s3),
					    s4     = border_step_moving_along_edge(e4,opposite(v,e4)),
					    s4_inv = opposite_border_step (s4);

				 gdt::gdtlist<bend_type> s1_bends     = bends_of_border_step (s1),
			 			 s1_inv_bends = bends_of_border_step (s1_inv),
						 s2_bends     = bends_of_border_step (s2),
						 s2_inv_bends = bends_of_border_step (s2_inv),
						 s3_bends     = bends_of_border_step (s3),
						 s3_inv_bends = bends_of_border_step (s3_inv),
						 s4_bends     = bends_of_border_step (s4),
						 s4_inv_bends = bends_of_border_step (s4_inv);

				s2_bends.push	    (left) ;
				s2_inv_bends.append (right);

				s3_bends.append     (right);
				s3_inv_bends.push   (left) ;
				
				// old configuration
				// -----------------
				//s4_bends.append     (left) ;
				//s4_bends.append	    (right);
				//s4_inv_bends.push   (right);
				//s4_inv_bends.push   (left) ;
				// ----------------

				set_angle_of_border_step (s1_inv, _090);
				set_angle_of_border_step (s2_inv, _090);
				set_angle_of_border_step (s3	, _090);
				set_angle_of_border_step (s4	, _090);	

				set_bends_of_border_step (s1    , s1_bends);
  				set_bends_of_border_step (s1_inv, s1_inv_bends);
				set_bends_of_border_step (s2    , s2_bends);
				set_bends_of_border_step (s2_inv, s2_inv_bends);
				set_bends_of_border_step (s3    , s3_bends);
				set_bends_of_border_step (s3_inv, s3_inv_bends);
				set_bends_of_border_step (s4    , s4_bends);
				set_bends_of_border_step (s4_inv, s4_inv_bends);
				}

			else // real gdtnode
				{
				gdt::gdtlist<gdtedge> Le_in_v  = in_edges (v);
				gdt::gdtlist<gdtedge> Le_out_v = out_edges(v);
				int in_size  = Le_in_v.size();
				int out_size = Le_out_v.size();
				int in_middle  = in_size/2 + 1; 
				int out_middle = out_size/2 + 1;

				gdtedge e_in  = upug.first_in_cand_edge (u);
				gdtedge e_out = upug.first_out_cand_edge (u);

				if (e_in)  e_in  = corresponding_edge(e_in ,upug);
				if (e_out) e_out = corresponding_edge(e_out,upug);

				// locally set the shape of the edges entering the gdtnode "v"
				// --------------------------------------------------------
				// 
				//     ---                  ---
				//    | u |                | v |
				//    |   |      --->      |   |      
				//     ---                  ---
				//    ^ ^ ^                ^ ^ ^ 
				//    | | |                | | |
				//    | | |              o-o | o-o
				//                       |   |   |
				// 
				// ---------------------------------------------------------

				int i;
				for ( i=1 ; i <= in_size ; i++)
					{
					border_step s    = border_step_moving_along_edge (e_in,opposite(v,e_in));
					border_step sinv = opposite_border_step (s);

					if (i < in_middle)
						{
						gdt::gdtlist<bend_type> s_bends    = bends_of_border_step(s),
			    	     		        	sinv_bends = bends_of_border_step(sinv);

			    			s_bends.append  (right);
						s_bends.append  (left);
						sinv_bends.push (left);
						sinv_bends.push (right);

  						set_bends_of_border_step (s,s_bends);
  						set_bends_of_border_step (sinv,sinv_bends);
						}

					else if (i == in_middle)
						{
						//nothing to be done
						}

					else // i > in_middle
						{
						gdt::gdtlist<bend_type> s_bends    = bends_of_border_step(s), 
			    	     		        	sinv_bends = bends_of_border_step(sinv);

			    			s_bends.append  (left);
						s_bends.append  (right);
						sinv_bends.push (right);
						sinv_bends.push (left);

  						set_bends_of_border_step (s,s_bends);
  						set_bends_of_border_step (sinv,sinv_bends);					
						}

					if (i != in_size) 	set_angle_of_border_step (s,_000);
					else if (out_size > 0) 	set_angle_of_border_step (s,_180);
					     else	       	set_angle_of_border_step (s,_360);

					e_in = cyclic_adj_succ (e_in,v);
					}

				// locally set the shape of the edges leaving the gdtnode "v"
				// --------------------------------------------------------
				//
				//                       ^   ^   ^
				//    ^ ^ ^              |   |   |
				//    | | |              o-o | o-o
				//    | | |                | | |
				//     ---                  ---
				//    | u |                | v |
				//    |   |      --->      |   |      
				//     ---                  ---
				// 
				// ---------------------------------------------------------


				for ( i=1 ; i <= out_size ; i++)
					{
					border_step s    = border_step_moving_along_edge (e_out,v);
					border_step sinv = opposite_border_step (s);

					if (i < out_middle)
						{
						gdt::gdtlist<bend_type> s_bends    = bends_of_border_step(s), 
			    	     		        	sinv_bends = bends_of_border_step(sinv);

			    			s_bends.push      (right);
						s_bends.push      (left);
						sinv_bends.append (left);
						sinv_bends.append (right);

  						set_bends_of_border_step (s,s_bends);
  						set_bends_of_border_step (sinv,sinv_bends);
						}

					else if (i == out_middle)
						{
						//nothing to be done
						}

					else // i < out_middle
						{					
						gdt::gdtlist<bend_type> s_bends    = bends_of_border_step(s), 
			    	     		        	sinv_bends = bends_of_border_step(sinv);

			    			s_bends.push      (left);
						s_bends.push      (right);
						sinv_bends.append (right);
						sinv_bends.append (left);

  						set_bends_of_border_step (s,s_bends);
  						set_bends_of_border_step (sinv,sinv_bends);
						}

					if (i != 1) 		set_angle_of_border_step(sinv,_000);
					else if (in_size > 0) 	set_angle_of_border_step (sinv,_180);
					     else	      	set_angle_of_border_step (sinv,_360);

					e_out = cyclic_adj_pred (e_out,v);
					}
				}

			}


		// reverse the edges between two any upward-bends
		// (nodes replacing bends)
		// and weld the edges at these nodes.
			{
			gdtedge e;
			forall_edges (e,*this)
				{
				gdtnode v1 = source(e);
				gdtnode v2 = target(e);
				if (is_marked(v1,RM3_REPLACES_A_BEND) &&
				    is_marked(v2,RM3_REPLACES_A_BEND)) reverse(e);
				}

			forall_nodes (v,*this)
				if (is_marked(v,RM3_REPLACES_A_BEND)) weld_edges_at_node(v);
			}


		// remove the alternate sequences of right and left turns
		// that are gdtnode associated with the two extremal bends
		// of an gdtedge


			{
			gdtedge e;
			forall_edges (e,*this)
				{
				border_step s = border_step_moving_along_edge(e,source(e));
				gdt::gdtlist<bend_type> bends = bends_of_border_step (s);
				gdt::list_item curr_pos;
				if (bends.size() > 2) 
					{
					curr_pos = bends.succ(bends.first());
					gdt::gdtstack<gdt::list_item> S;
					while (curr_pos != bends.last())
					      {
					      gdt::list_item succ_pos = bends.succ(curr_pos);
					      if (S.empty()) S.push(curr_pos);
					      else
						   {
						   gdt::list_item top_pos = S.top();
						   bend_type last_bend = bends.inf(top_pos);
						   bend_type curr_bend = bends.inf(curr_pos);
						   if (((last_bend == right) && (curr_bend == left)) ||
						       ((curr_bend == right) && (last_bend == left)))
					    		   {
							   S.pop();
							   bends.del_item(top_pos);
							   bends.del_item(curr_pos);
							   }
						   else S.push(curr_pos);
						   }
					      curr_pos = succ_pos;
					      }
					 }

				border_step s_inv = opposite_border_step(s); 
				gdt::gdtlist<bend_type> bends_inv;
				bend_type 	b;
				forall (b,bends) if (b == left) bends_inv.push(right);
						 else   	bends_inv.push(left);
				set_bends_of_border_step (s,bends); 
				set_bends_of_border_step (s_inv,bends_inv);
				}
			}
		}
	}
	
	

	void
	orth_plan_undi_graph::
local_set
	(
	face                                            p1,
	algorithm_type                                  p2,
	gdt::gdtedge_map<bend_constraint>                       p3,
	gdt::gdtmap<border_step,struct_orth_border_step_info>*  p4,
	bool                                            p5,
	bool                                            p6,
	border_step					p7,
	int						p8
	)
	{
	////////////////////////////////////////
	// Added by Pier Francesco Cortese 29/09/2006
	delete ref_border_step;
	delete ext_face;
	//delete constraint_on_bends_of_edge;
	delete border_step_info;
	////////////////////////////////////////
	ext_face                          = p1;
	layout_algorithm                  = p2;
	constraint_on_bends_of_edge       = p3; 
	border_step_info                  = p4;
	local_consistency_is_suspended    = p5;
	diagnostic_printouts_are_enabled  = p6;
	ref_border_step			  = p7;
	last_cost			  = p8;
	} 	
	


	void
	orth_plan_undi_graph::
update_constraint_internal_structures
	()
	{
	gdtedge e;
	// set the edge_mapping of bend constraints
	//
	forall_edges (e,*this) set_bend_constraints (e,MINIMAL); // initialization
	
	constraint c;
	constraint_number_of_bends_on_edge nbc;
	
	forall_constraints (c,*this)	
		{
		if (type_of_constraint(c) == NUMBER_OF_BENDS_ON_EDGE)
			{
			nbc = (constraint_number_of_bends_on_edge)c; // casting 
			gdtedge 		 e     = nbc -> get_edge_with_nbc();			
			bend_constraint ct     = nbc -> get_nbc();
			bend_constraint ct_old = get_constraint_on_bends_of_edge(e);

			if (ct_old != MINIMAL && ct_old != ct) // collision case
				 gdt_error("constraint collision on gdtedge");
			else set_bend_constraints (e,ct);				 
			}
		}		
	}


	
	void
	orth_plan_undi_graph::
set_bend_constraints
	(gdtedge e, bend_constraint bc)
	{
	constraint_on_bends_of_edge[e] = bc;
	}



	void
	orth_plan_undi_graph::
set_bend_constraints
	(gdt::gdtedge_array<bend_constraint> ebc)
	{
	gdtedge e;
	forall_edges (e,*this) set_bend_constraints (e,ebc[e]);
	}


	
	void
	orth_plan_undi_graph::
reset_bend_constraints
	()
	{
	gdtedge e;
	forall_edges (e,*this) set_bend_constraints (e,MINIMAL);
	}

	
	
	void
	orth_plan_undi_graph::
set_thickness_of_border_step
	(border_step s, int t)
	{
	(*border_step_info)[s].thickness = t;
	}

	
	void
	orth_plan_undi_graph::
set_pin_number_of_border_step
	(border_step s, int p)
	{
	(*border_step_info)[s].pin_number = p;
	}
	
	
	gdt::gdtlist<bend_type>
	orth_plan_undi_graph::
invert_bend_list
	(gdt::gdtlist<bend_type> blist)
	{
	bend_type bt;
	gdt::gdtlist<bend_type> result;
	forall (bt,blist)
		if (bt==left) result.push(right);
		else          result.push(left);
	return result;		
	}


	void
	orth_plan_undi_graph::
reset_edge_info
	(gdtedge e)
	{
	gdtnode v = source(e);
	gdtnode w = target(e);
	border_step s1 = border_step_moving_along_edge (e,v);
	border_step s2 = border_step_moving_along_edge (e,w);
	reset_border_step_info (s1);
	reset_border_step_info (s2);
	}


	void 	
	orth_plan_undi_graph::	
reset_border_step_info
	(border_step s)
	{
	(*border_step_info)[s].angle = _090;  
	(*border_step_info)[s].bends.clear();
	(*border_step_info)[s].thickness = 1;
	(*border_step_info)[s].pin_number = UNDEFINED_PIN_NUMBER;
	} 
	
	
	
	void 	
	orth_plan_undi_graph::	
set_bends_of_border_step
	(border_step s,gdt::gdtlist<bend_type> bends)
	{
	(*border_step_info)[s].bends = bends;
	} 
	
	
	
	void 
	orth_plan_undi_graph::
set_angle_of_border_step
	(border_step s,angle_type angle)
	{
	(*border_step_info)[s].angle = angle;  
	} 

	
	
	int
	orth_plan_undi_graph::
angle_to_int 	           
	(angle_type alpha) const
	{
	int a;
	switch (alpha)
		{
		case _000: a = 0  ; break;
		case _090: a = 90 ; break;
		case _180: a = 180; break;
		case _270: a = 270; break;
		case _360: a = 360; break;
		}
	return a;
	}
	
	
	
	angle_type
	orth_plan_undi_graph::    
int_to_angle	
	(int a) const
	{
	angle_type alpha;
	switch (a)
		{
		case 0  : alpha = _000; break;
		case 90 : alpha = _090; break;
		case 180: alpha = _180; break;
		case 270: alpha = _270; break;
		case 360: alpha = _360; break;
		default: gdt_error("invalid int specification");
		}
	return alpha;
	}
	
	
	
	void
	orth_plan_undi_graph::
make_border_step_turn_left
	(border_step s)
	{
	border_step ps = cyclic_pred_border_step(s);
	set_angle_of_border_step(ps,_270);
	}


 	
	void
	orth_plan_undi_graph::
make_border_step_turn_right
	(border_step s)
	{
	border_step ps = cyclic_pred_border_step(s);
	set_angle_of_border_step(ps,_090);
	}



	void
	orth_plan_undi_graph::
make_border_step_turn_back
	(border_step s)
	{
	border_step ps = cyclic_pred_border_step(s);
	if (edge_of_border_step(ps)==edge_of_border_step(s))
		set_angle_of_border_step(ps,_360);
	else 	set_angle_of_border_step(ps,_000);
	} 
	
	
		
	void
	orth_plan_undi_graph::
make_border_step_go_straight
	(border_step s)
	{
	border_step ps = cyclic_pred_border_step(s);
	set_angle_of_border_step(ps,_180);
	}

	

 	void
	orth_plan_undi_graph::
set_edge_bends
 	(gdtedge e, gdtnode v, int bnum, bend_type btype)
 	{
 	angle_type  av,aw;
 	border_step sv,sw;
 	sv = border_step_moving_along_edge(e,v);
 	sw = opposite_border_step (sv);
 	
 	av = angle_of_border_step (sv);
 	aw = angle_of_border_step (sw);
 	
	set_edge_shape (e,v,bnum,btype,aw,av);
  	}



	// Description: set bends and angles along gdtedge 'e'. A left (right) bend is created for each L (R) char in the 's' string. Bends are ordinately placed along gdtedge 'e' in the direction startin from gdtnode 'v'.
	// Preconditions: 'v' is an end-gdtnode of gdtedge 'e'.
	// Notes: -
	//
 	void
	orth_plan_undi_graph::
set_edge_shape
 	(gdtedge e, gdtnode v, int bnum, bend_type btype, angle_type aw, angle_type av)
 	{
 	int i;
	bend_type symbtype;
 	gdt::gdtlist<bend_type> bv; // bends along sv
 	gdt::gdtlist<bend_type> bw; // bends along sw

	/* 
	//  \              
	//   \  aw                   
	//    \            <-sw-          
	//    (v)--------e--------(w)
	//         -sv->            \
	//                       av  \
	//                            \
	*/

	if (btype==left) symbtype=right;
	else             symbtype=left;
	
 	for (i=0; i<bnum; i++)
 		{
		bv.append (btype);
 		bw.push   (symbtype);
		}

 	set_edge_shape (e,v,bw,bv,aw,av);
  	}




	// Description: set bends and angles along gdtedge 'e'. A left (right) bend is created for each L (R) char in the 's' string. Bends are ordinately placed along gdtedge 'e' in the direction startin from gdtnode 'v'.
	// Preconditions: 'v' is an end-gdtnode of gdtedge 'e'.
	// Notes: -
	//
 	void
	orth_plan_undi_graph::
set_edge_shape
 	(gdtedge e, gdtnode v, std::string s, angle_type aw, angle_type av)
 	{
 	unsigned int i;
 	gdt::gdtlist<bend_type> bv; // bends along sv
 	gdt::gdtlist<bend_type> bw; // bends along sw

	/* 
	//  \              
	//   \  aw                   
	//    \            <-sw-          
	//    (v)--------e--------(w)
	//         -sv->            \
	//                       av  \
	//                            \
	*/

 	// Translate the string s
 	// into bends along the border steps along gdtedge 'e'
 	//
 	for (i=0; i<s.length(); i++)
		switch (s[i])
 			{
 			case 'L':
 				bv.append(left);
 				bw.push(right);
 				break;
 			case 'R':
				bv.append(right);
 				bw.push(left);
 				break;
 			default :
 				gdt_error("invalid shape");
 			}
 	set_edge_shape (e,v,bw,bv,aw,av);
  	}




	// Description: set bends and angles along gdtedge 'e'.
	// Preconditions: 'v' is an end-gdtnode of gdtedge 'e'.
	// Notes: -
	//
 	void
	orth_plan_undi_graph::
set_edge_shape
 	(gdtedge e, gdtnode v, gdt::gdtlist<bend_type> bw, gdt::gdtlist<bend_type> bv, angle_type aw, angle_type av)
 	{
 	gdtnode w;
 	border_step sv,sw;
 	
 	w  = opposite (v,e);
 	sv = border_step_moving_along_edge (e,v);
 	sw = border_step_moving_along_edge (e,w);

	/* 
	//  \              
	//   \  aw                   
	//    \            <-sw-          
	//    (v)--------e--------(w)
	//         -sv->            \
	//                       av  \
	//                            \
	*/

  	set_angle_of_border_step (sv,av);
   	set_angle_of_border_step (sw,aw);
 	
  	set_bends_of_border_step (sv,bv);
 	set_bends_of_border_step (sw,bw);
  	}




	// Description: set bends and angles along gdtedge 'e'. A left (right) bend is created for each L (R) char in the 's' string. Bends are ordinately placed along gdtedge 'e' in the direction startin from gdtnode 'v'.
	// Preconditions: 'v' is an end-gdtnode of gdtedge 'e'.
	// Notes: -
	//
 	void
	orth_plan_undi_graph::
set_edge_shape_near_node
 	(gdtedge e, gdtnode v, std::string s, angle_type a)
 	{
 	unsigned int i;
 	border_step s_in;
 	border_step s_out;	
 	gdt::gdtlist<bend_type> b_in;  // bends along s_in
 	gdt::gdtlist<bend_type> b_out; // bends along s_out

	/* 
	//    --s_in->          
	//    --------e--------(v)
	//             <-s_out-- \
	//                      a \
	//                         \
	*/

	s_out = border_step_moving_along_edge (e,v);
	s_in  = opposite_border_step (s_out);
	
	b_in  = bends_of_border_step (s_in);
	b_out = bends_of_border_step (s_out);

 	// Translate the string s
 	// into bends along the border steps along gdtedge 'e'
 	//
 	for (i=0; i<s.length(); i++)
		switch (s[i])
 			{
 			case 'L':
 				b_in.append(left);
 				b_out.push(right);
 				break;
 			case 'R':
 				b_in.append(right);
 				b_out.push(left);
 				break;
 			default :
 				gdt_error("invalid shape");
 			}
  	set_bends_of_border_step (s_in,  b_in);
  	set_bends_of_border_step (s_out, b_out);
   	set_angle_of_border_step (s_in,  a);
  	}


	
	// Description: split face f into a rectangular and an irregular part, then split the irregular part, and so on until either the face is fully decomposed into rectangles or no more LRR sequences are found.
	// Preconditions : all the edges of the face border must have no bends.
	// Notes: FUNCTION FOR 4-PLANAR ONLY
	//
	void
	orth_plan_undi_graph::
remove_LRR_sequences_from_border_of_face
	(face f, gdt::gdtlist<gdtnode>& dummy_nodes, gdt::gdtlist<gdtedge>& dummy_edges)
	{
	//cout << "\nRemoving LRR sequences from face "; print(f);
	
	int nb;                       // Number of border_steps around the initial face
	//
	border_step s1;               // 1st border_step of an LRR sequence (left)
	border_step s2;               // 2nd border_step of an LRR sequence (right)
	border_step s3;               // 3rd border_step of an LRR sequence (right)
	//
	border_step sd;               // border_step along the dummy_edge from note v1
	border_step s0;               // border_step before s1
	border_step s4;               // border_step after  s3
	//
	gdt::gdtlist<border_step>  ts;        // turning border-steps of face f
	gdt::gdtstack<gdt::list_item>  lsp;        // positions of the left-turning steps in ts
	//
	gdt::list_item s1_pos;             // position (i.e. gdt::list_item) of s1 into ts
	gdt::list_item s2_pos;             // position (i.e. gdt::list_item) of s2 into ts
	gdt::list_item s3_pos;             // position (i.e. gdt::list_item) of s3 into ts
	gdt::list_item s4_pos;             // position (i.e. gdt::list_item) of s4 into ts (Only in case s4 is actually turning)
	gdt::list_item pos;                // utility (e.g. current item during list scan)
	//
	gdtedge ed;                      // dummy gdtedge (v1,v4) splitting face f
	gdtnode v1,v2,v3,v4;             // start nodes of (s1,s2,s3), stop gdtnode of s3
	face nonrect_face_part;       // non-rectangular part of the splitted face
	
	// The nb number helps in avoiding loops
	// when looking for LRR sequences on the external face.
	//
	nb = number_of_left_turns_around_face (f);

	// Make ts contain only the border_steps of face f
	// turning left or right (i.e. ignore steps going straight)
	//
	ts = turning_border_steps_of_face (f);

	//cout << "\nInitially ts is "; forall (s,ts) print(start_of_border_step(s));

	// Scan ts in order to detect LRR step sequences.
	// When removing nested LRR sequences 
	// use the lsp stack in order to remember
	// the positions of the left-turning steps.
	//
	
	s1_pos = find_border_step_turning_left_or_turning_back(ts);
	while ((s1_pos) && (lsp.size()<=nb)) // The second check avoids loop in case f==ext_face
		{
		lsp.push(s1_pos);
		while (!lsp.empty() && lsp.size()<=nb) // The second check avoids loop in case f==ext_face
			{
			//cout << "\nThe size of the lsp stack is now " << lsp.size();
				
			s1_pos = lsp.top();
			s2_pos = ts.cyclic_succ(s1_pos);
			s3_pos = ts.cyclic_succ(s2_pos);

			s1 = ts.inf(s1_pos);
			s2 = ts.inf(s2_pos);
			s3 = ts.inf(s3_pos);
			
			s0 = cyclic_pred_border_step(s1);
			s4 = cyclic_succ_border_step(s3);
						
			v1 = start_of_border_step (s1);
			v2 = start_of_border_step (s2);
			v3 = start_of_border_step (s3);
			v4 = start_of_border_step (s4);
						
			if      (border_step_turns_left(s2) || border_step_turns_back(s2)) lsp.push(s2_pos);
			else if (border_step_turns_left(s3) || border_step_turns_back(s3)) lsp.push(s3_pos);	
			else
				{
				// A left-right-right step sequence has been found
				// A new gdtedge (v1,v4) must be created in order to split face f.
				
				//cout << "\n LRR sequence : ";
				//print (start_of_border_step(s1));	 
				//print (start_of_border_step(s2));	 
				//print (start_of_border_step(s3));
				//cout << "\n";	 

				// This LRR sequence is going to be removed
				// therefore its position must be removed from the lsp stack
				//
				if (!lsp.empty()) lsp.pop();
				switch (angle_of_border_step(s3))
					{
					case _090:
						{
						/*
						// cout << "\nCASE 90\n" << flush;
						//
						//       s2		    s2
						//     o----o		--o----o
						//     |    |		  |    |
						//     |s1  |	      	  |s1  |s3_inv
						//     |    |s3_inv	  |  s3|
						// ----o  s3|		  o    |
						//          | 		       |
						//     <----o              <---o
						//       
						// 's3' is the only step of the face-side it belongs to.
						// It is necessary to split 's3' in order to create a dummy gdtnode 'w'.
						*/

						gdtedge        ex;          // gdtedge to be splitted
						gdtedge        e1,e2;       // edges resulting from ex split
						gdtnode        vx;          // new gdtnode created to split gdtedge ex
						int         old_id;      // id of the ex gdtedge before splitting
						border_step s3_inv;      // opposite border_step of s3
						gdt::list_item   s3_inv_pos;  // position (i.e. gdt::list_item) of s3_inv into ts (NULL if s3_inf is not into ts)
						gdt::list_item   ex_pos;      // position of ex into the dummy_edges list (NULL if ex does is not a summy_edge)

						// Now check if the border_step s3_inv is present into ts list.
						// In this case (s3_inv_pos != NULL) it will be replaced into ts and lsp
						// with a new border_step derived by gdtedge splitting.
						//						
						s3_inv      = opposite_border_step (s3);
						s3_inv_pos  = NULL;
						pos = ts.first(); while (pos) if (ts.inf(pos)==s3_inv) {s3_inv_pos=pos; break;} else pos=ts.succ(pos);

						// Save the id of the gdtedge to be splitted
						// and check wether it is a dummy gdtedge (ex_pos!=NULL)
						//
						ex     = edge_of_border_step(s3);
						ex_pos = dummy_edges.search(ex);
						old_id = id(ex);
						
						/*
						// Split the gdtedge
						// by creating a new gdtnode along it,
						// and retrieve the new edges e1,e2
						// resulting from the split.
						//
						//
						// (v3)      (v3)
						//   |         |
						//   |         e1
						//   |         |
						//  ex   =>  (vx)
						//   |         |
						//   |         e2
						//   |         |
						//  (v4)     (v4)
						*/

						vx = new_node (ex);
						e1 = first_adj_edge(vx);
						e2 = last_adj_edge (vx);
						if (opposite(vx,e1) != v3) {gdtedge temp=e1;e1=e2;e2=temp;}
						
						// Assign the old_id to e1,
						// and include the new gdtnode in the dummy_nodes list.
						//
						assign_id(e1,old_id);
						dummy_nodes.append(vx);
						
						// Since gdtedge ex has been splitted,
						// ex, s3 and s3_inv are now invalid pointers,
						// and must be updated.
						//
						s3     = border_step_moving_along_edge (e1,v3);
						s3_inv = border_step_moving_along_edge (e2,v4);

						// If the old s3_inv was a turning border_step,
						// then the new s3_inv is also a turning one.
						// As a consequence, both the ts list and the lsp stack
						// must be updated.
						//
						if (s3_inv_pos)
							{
							gdt::list_item old_pos;
							gdt::list_item new_pos;

							// Update the ts list
							//
							old_pos = s3_inv_pos;
							new_pos = ts.insert (s3_inv,old_pos,gdt::before);
							ts.del_item (old_pos);

							// Update the lsp stack
							// (a temporary stack is required)
							//
							gdt::gdtstack<gdt::list_item> temp;
							while (!lsp.empty())
								if (lsp.top()!=old_pos) temp.push(lsp.pop());
								else {lsp.pop(); lsp.push(new_pos); break;}
							while (!temp.empty())
								lsp.push(temp.pop());
							}
							
						
						// If gdtedge ex was a dummy gdtedge (ex_pos!=NULL)
						// then both the two new edges e1,e2 are dummy as well.
						//
						if (ex_pos)
							{
							// Edges e1 and e2 are appended
							// the the summy_edges list. Note that
							// the id(e1) is equal to the old_id of ex,
							// therefore it is definitively < id(e2).
							//
							dummy_edges.del_item(ex_pos);
							dummy_edges.append(e1);
							dummy_edges.append(e2);
							}


						// The special _090 case has been reduced
						// to a _180 case, where vx is assumed
						// to be the new gdtnode v4.
						//						
						v4 = vx;
						s4 = border_step_moving_along_edge(e2,vx);
						}

					case _180:
						{
						/* cout << "\nCASE 180\n" << flush;
						//
						//       s2    
						//     o----o   
						//     |    |   
						//   s1|  s3|   
						//     |    |   
						// ->--o    o   
						//          |s4 
						//          |   
						*/

						// Border step s4 is NOT a turning one
						// but it will turn RIGHT once the dummy gdtedge is created.
						// Therefore we insert it into the ts list.
						//
						s4_pos = ts.insert(s4,s3_pos,gdt::after);
						break;
						}

					case _270:
						{
						/* cout << "\nCASE 270\n" << flush;
						//
						//        s2
						//     o----o
						//     |    |
						//   s1|  s3|
						//     |    |
						// ----o    o--->
						//            s4
						*/

						// The border_step s4 IS a turning one,
						// but it will go straight once the dummy gdtedge is created.
						// Therefore we remove it from the ts list.
						//
						s4_pos = ts.cyclic_succ(s3_pos);
						ts.del_item (s4_pos);
						break;
						}

					case _360:
						{
						/* cout << "\nCASE 270,360\n" << flush;
						//
						//        s2
						//     o----o---->
						//     |    |
						//   s1|  s3|s4
						//     |    |
						// ----o    o
						*/

						// The border_step s4 IS a turning one (it actually goes back)
						// and it will still turn LEFT once the dummy gdtedge is created.
						// Therefore the ts list need no update.
						s4_pos = ts.cyclic_succ(s3_pos);
						break;
						}
					default: break;
					}

				// Create the dummy gdtedge (v1,v4).
				// Once face f has been splitted by the dummy gdtedge,
				// the f variable is an invalid pointer.
				//
				ed  = new_edge
					(
					v1,edge_of_border_step(s0),
					v4,edge_of_border_step(s3)
					);

				// Append the new gdtedge
				// to the dummy_edges list
				//
				dummy_edges.append (ed);

				// If s1 was turning back, before the dummy gdtedge was created,
				// then the new border_step s (from v1 along the dummy gdtedge) does
				// not go straight, but actually turns LEFT,
				// and must be added to both the ts list and the lsp stack.
				//
				sd = border_step_moving_along_edge (ed,v1);
				if (border_step_turns_left(sd))
					{
					pos = ts.insert(sd,s1_pos,gdt::before);
					lsp.push(pos);
					}
					
				// If s4 was turning back, before the dummy gdtedge was created,
				// then s4 now turns left, and must be pushed into the lsp stack.
				//
				if (border_step_turns_left(s4))
					lsp.push(s4_pos);
					
				// It is now possible 
				// to remove the old LRR step-sequence
				//
				ts.del_item (s1_pos);
				ts.del_item (s2_pos);
				ts.del_item (s3_pos);
								
				// Face f has been splitted by gdtedge e
				// into a rectangular and a non-rectangular part.
				// Iterate on the non-rectangular part.
				//
				nonrect_face_part = face_of_border_step (sd);
				if (f==ext_face) ext_face = nonrect_face_part;
				f = nonrect_face_part;
				}
			}
		// The lsp stack stores the first (L) position
		// of all the nested LRR sequences in ts.
		// When the lsp stack is empty, it means that all the
		// nested sequences have been ordinately removed.
		// However, another sequence could exist in ts,
		// and must be pushed in the stack.
		//
		s1_pos = find_border_step_turning_left_or_turning_back(ts);
		}
	}	
	


	// Description: split face f into a rectangular and an irregular part, then split the irregular part, and so on until either the face is fully decomposed into rectangles or no more LRL sequences are found.
	// Preconditions : all edges of the face border must have no bends, and no LRR sequences are on the border on the external face.
	// Notes: FUNCTION FOR 4-PLANAR ONLY
	//
	void
	orth_plan_undi_graph::
remove_LRL_sequences_from_border_of_external_face 
	(gdt::gdtlist<gdtnode>& dummy_nodes, gdt::gdtlist<gdtedge>& dummy_edges)
	{
	//cout << "\nRemoving LRL sequences from external face "; print(ext_face);

	border_step s1;               // 1st border_step of an LRL sequence (left)
	border_step s2;               // 2nd border_step of an LRL sequence (right)
	border_step s3;               // 3rd border_step of an LRL sequence (left)
	//

	border_step sv1,sv2;          // border_steps on the dummy gdtedge, after bend replacement
	//
	gdt::gdtlist<border_step>  ts;        // turning border-steps of face f
	gdt::gdtstack<gdt::list_item>  rsp;        // positions of the right-turning steps in ts
	//
	gdt::list_item s1_pos;             // position (i.e. gdt::list_item) of s1 into ts
	gdt::list_item s2_pos;             // position (i.e. gdt::list_item) of s2 into ts
	gdt::list_item s3_pos;             // position (i.e. gdt::list_item) of s3 into ts
	gdt::list_item sv1_pos;            // position (i.e. gdt::list_item) of sv1 into ts
	gdt::list_item sv2_pos;            // position (i.e. gdt::list_item) of sv2 into ts
	//
	gdtedge e;                       // dummy gdtedge
	gdtnode v,w;                     // end-nodes of the dummy gdtedge
	
	// Make ts contain only the border_steps of ext_face
	// turning left or right (i.e. ignore steps going straight)
	//
	ts = turning_border_steps_of_face (ext_face);
	
	//cout << "\nInitially ts is "; forall (s,ts) print(start_of_border_step(s));
	
	// By precondition, we can assume that no LRR sequences are in ts.
	// However, since we are considering the external face,
	// at least four L must be in ts. Therefore, no RR sub sequence
	// can exist in ts, otherwise an LRR would exist too.
	//
	// It is enough to look for R (border_steps turning right) because
	// (given that no LRR sequences exist and the face is external one)
	// each R is the middle one of an LRL sequence.
	
	// Scan ts in order to detect R border_steps.
	//
	s2_pos = find_border_step_turning_right(ts);
	while (s2_pos)
		{
		s1_pos = ts.cyclic_pred(s2_pos);
		s3_pos = ts.cyclic_succ(s2_pos);

		s1 = ts.inf(s1_pos);
		s2 = ts.inf(s2_pos);
		s3 = ts.inf(s3_pos);
		
		// A left-right-left step-sequence has been found
		// (any left turn could actually be a back, but this makes no difference).
		// A new gdtnode and two new edges must be created
		// in order to build a new face filling the concavity of the external face.

		//cout << "\n LRL sequence : ";
		//print (start_of_border_step(s1));	 
		//print (start_of_border_step(s2));	 
		//print (start_of_border_step(s3));
		//cout << "\n";	 

		/*     s3                                     s3
		// o-----(w)                (w)           o-----(w)                 (w)
		// |      |                  |            |      |                   |
		// |      |                s3|            |      |                 s3|
		// |      |s2                |s2          |      |s2                 |s2
		// |      |  s1              |  s1        |      |  s1               |  s1
		// |      o-----(v)    o-----o-----(v)    o      o-----(v)     o-----o-----(v)
		// |             |     |            |     |      |             |     |   
		// |             |     |            |     |      |             |     |
		// |             |     |            |     |      |             |     |
		// |             |     |            |     |      |             |     |
		// o-------------o     o------------o     o------o             o-----o
		*/

		// Create the new dummy_edge
		// Warning: s2 is the first turning border_step before s3.
		// However, generally speaking, s2 != cyclic_pred_border_step(s3);
		//
		v = start_of_border_step (s1);
		w = start_of_border_step (s3);
		e = new_edge
			(
			v,cyclic_adj_pred(edge_of_border_step(s1),v),
		        w,cyclic_adj_pred(edge_of_border_step(s3),w)
		        );

		// WARNING: since the external face has been splitted
		// the f variable is now an invalid pointer

		// The new gdtedge e has a left bend
		// that must be replaced by a gdtnode.
		//
		replace_bends_with_nodes (e,dummy_nodes,dummy_edges);

		// WARNING: since gdtedge e has been splitted,
		// the 'e' variable is now an ivalid pointer.
		// However, sv2 can be obtained from s3.
		//
		sv2 = cyclic_pred_border_step(s3);
		sv1 = cyclic_pred_border_step(sv2);

		/*     s3     sv2                sv2          s3     sv2                 sv2
		// o-----(w)-----o          (w)-----o     o-----(w)-----o           (w)-----o
		// |      |      |           |      |     |      |      |            |      |
		// |      |      |         s3|      |     |      |      |          s3|      |
		// |      |s2    |sv1        |s2    |sv1  |      |s2    |sv1         |s2    |sv1
		// |      |  s1  |           |  s1  |     |      |  s1  |            |  s1  |
		// |      o-----(v)    o-----o-----(v)    o      o-----(v)     o-----o-----(v)
		// |             |     |            |     |      | s0          |     | s0
		// |             |     |            |     |      |             |     |
		// |             |s0   |            |s0   |      |             |     |
		// |             |     |            |     |      |             |     |
		// o-------------o     o------------o     o------o             o-----o
		*/

		// Replace the LRL step-sequence
		// with a single step turning left
		//
		// Note that
		// - sv2 is turning and must be inserted in ts
		// - sv1 could be turning or not. If sv1 is turning, it must be inserted in ts
		// - s3 was in ts but could not be turning anymore: in this case it must be removed from ts
		// - s1 and s2 do not belong anymore to the external face, and must be removed from ts.
		//
		sv2_pos = ts.insert (sv2, s3_pos, gdt::before);
		if (!border_step_goes_straight(sv1)) sv1_pos = ts.insert (sv1,sv2_pos,gdt::before);
		if (border_step_goes_straight(s3)) ts.del_item (s3_pos);
		ts.del_item (s1_pos);
		ts.del_item (s2_pos);
		
		//cout << "\nAfter LRL remove, ts is "; forall (s,ts) print(start_of_border_step(s));

		// The old external face has been splitted
		// into a rectangular and a non-rectangular part.
		// The ext_face class-attribute has been updated
		// by the previous new_edge() call.
		// Iterate on the new ext_face.
		//
		s2_pos = find_border_step_turning_right(ts,sv2_pos);
		}
	}	
	


	bool
	orth_plan_undi_graph::
regularize_face
	(face f, gdt::gdtlist<gdtnode>& dummy_nodes, gdt::gdtlist<gdtedge>& dummy_edges, algorithm_type alg)
	{
	//cout << "\nREGULARIZING FACE " << id(f) << flush;
	bool irregular = false;
	
	gdtnode v,v1,v2;
	gdtedge e;
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
			
			switch (alg)
				{
				case FAST_REGULAR_COMPACTION_1:
				case SLOW_REGULAR_COMPACTION_1:
				case FAST_REGULAR_COMPACTION_1_REFINED:
				case SLOW_REGULAR_COMPACTION_1_REFINED:
					{
					//cout << "\nAPPLYING HEURISTIC 1" << flush;
					// ----------------------------------------------------------------------
					// HEURISTIC 1:
					// face f is rectangularized. Then each new face will be already regular,
					// and stack Q will be empty.
					// ----------------------------------------------------------------------
					
					
					if (f == external_face())
						{
						remove_LRR_sequences_from_border_of_face          (f, dummy_nodes, dummy_edges);
						remove_LRL_sequences_from_border_of_external_face (dummy_nodes, dummy_edges);
						}
					else
						{
						remove_LRR_sequences_from_border_of_face          (f, dummy_nodes, dummy_edges);
						}
					}
					break;
				
				case FAST_REGULAR_COMPACTION_2:
				case SLOW_REGULAR_COMPACTION_2:
				case FAST_REGULAR_COMPACTION_2_REFINED:
				case SLOW_REGULAR_COMPACTION_2_REFINED:
					{
					//cout << "\nAPPLYING HEURISTIC 2" << flush;
					// ----------------------------------------------------------------------
					// HEURISTIC 2:
					// Insert a straight gdtedge between the stop-gdtnode of r1 and the 
					// stop gdtnode of r2.
					// ----------------------------------------------------------------------
			
					v1 = stop_of_border_step (r1);
					v2 = stop_of_border_step (r2);
					gdtedge e1 = edge_of_border_step(r1);
					gdtedge e2 = edge_of_border_step(r2);
			
					e  = plan_undi_graph::new_edge (v1,e1,v2,e2);
			
					dummy_edges.append(e);
					reset_edge_info (e);				// border steps without bends and _090 angles on v1 and v2

					// ------------------------------------------------------
					// set the angles around v1 and v2 in the different cases
					// ------------------------------------------------------
			
					gdt::gdtlist<border_step> B;	// border_steps of bridge edges
			
					angle_type a1 = angle_of_border_step (r1);
					angle_type a2 = angle_of_border_step (r2);
			
					if (a1 == _360) B.append(r1);
					if (a2 == _360) B.append(r2);
			
					int number_of_bridges = B.size();
			
					border_step nr1; 
					border_step nr2;
					gdtnode 	    nv1;
					gdtnode	    nv2;
			
					switch (number_of_bridges)
						{
						case 0:	 // no bridges
							{
							//cout << "\nconnecting no bridges" << flush;
							nr1 = r1;
							nr2 = r2;
							set_angle_of_border_step (nr1,_180);
							set_angle_of_border_step (nr2,_180);
							}
							break;
						case 1:	// one bridge
							{
							//cout << "\nconnecting one bridge" << flush;
							nr1 = B.head();
							if (nr1 == r1) nr2 = r2;
							else	       nr2 = r1;

							nv1 = stop_of_border_step(nr1);
							nv2 = stop_of_border_step(nr2);
							
							border_step ev1 = border_step_moving_along_edge (e,nv1);
							border_step ev2 = border_step_moving_along_edge (e,nv2);
							
							border_step ev2s=cyclic_succ_border_step (ev2);
							
							int spirality = number_of_right_turns_along_border(ev2s,nr2)- 
									number_of_left_turns_along_border(ev2s,nr2);
							
							//cout << "\nspirality = " << spirality << flush;
							
							if (spirality == 3 || spirality == -5)
								{
								set_angle_of_border_step (nr1,_180);
								set_angle_of_border_step (nr2,_090);
								set_angle_of_border_step (ev1,_180);
								set_angle_of_border_step (ev2,_180);
								}
								
							else if (spirality == 4 || spirality == -4)
								{
								set_angle_of_border_step (nr1,_180);
								set_angle_of_border_step (nr2,_180);
								set_angle_of_border_step (ev1,_090);
								set_angle_of_border_step (ev2,_180);
								}
							else gdt_error("spirality error");
							}
							break;
						
						case 2:	// two bridges
							{
							//cout << "\nconnecting two bridges" << flush;
							
							nr1 = r1;
							nr2 = r2;
							nv1 = v1;
							nv2 = v2;
							
							border_step ev1 = border_step_moving_along_edge (e,nv1);
							border_step ev2 = border_step_moving_along_edge (e,nv2);
							
							border_step ev2s=cyclic_succ_border_step (ev2);
							
							int spirality = number_of_right_turns_along_border(ev2s,nr2)- 
									number_of_left_turns_along_border(ev2s,nr2);
							
							//cout << "\nspirality = " << spirality << flush;
							
							if (spirality == 3 || spirality == -5)
								{
								set_angle_of_border_step (nr1,_180);
								set_angle_of_border_step (nr2,_090);
								set_angle_of_border_step (ev1,_270);
								set_angle_of_border_step (ev2,_180);
								}
							else if (spirality == 4 || spirality == -4)
								{
								set_angle_of_border_step (nr1,_180);
								set_angle_of_border_step (nr2,_180);
								set_angle_of_border_step (ev1,_180);
								set_angle_of_border_step (ev2,_180);
								}
							else if (spirality == 5 || spirality == -3)
								{
								set_angle_of_border_step (nr1,_180);
								set_angle_of_border_step (nr2,_270);
								set_angle_of_border_step (ev1,_090);
								set_angle_of_border_step (ev2,_180);
								}
							else gdt_error("spirality error");
							}
					
						}
					
						//
						face f1 = right_face_moving_along_edge (e,v1);
						face f2 = right_face_moving_along_edge (e,v2);
			
						Q.push(f1);
						Q.push(f2);
			
						//cout << "\nADDED EDGE BETWEEN " << id(v1) << " AND " << id(v2) << flush;
						//cout << "\nFACES OBTAINED : " << flush;
						//print(f1); cout << "\n" << flush;
						//print(f2); cout << "\n" << flush;
						//cout << "\nORTHOGONAL OBTAINED : " << flush;
						//print(COMPLETE); cout << flush;
			
						if (update_external)
							{
							face ef = find_external_face();
							if (ef) ext_face = ef;
							else gdt_error("external face not found");
							}
						}
						break;
			
				default: 
					gdt_error("invalid algorithm"); 
					break; 
				}	
			
			}
		
		}
		
	forall (v,dummy_nodes) mark (v,RM3_ADDED_BY_REGULARIZATION);
	forall (e,dummy_edges) mark (e,RM3_ADDED_BY_REGULARIZATION);
	
	//cout << "\nFACE REGULARIZED " << flush;	

	return irregular;
	}
	



	// Description: split face f by adding a new gdtedge (v,w) and return this gdtedge.
	// Notes: FUNCTION FOR 4-PLANAR ONLY
	//
	gdtedge
	orth_plan_undi_graph:: 
split_internal_face
	(gdtnode v, gdtedge ev, gdtnode w, gdtedge ew, int new_id) 
	{
	//cout << "\nSPLITTING INTERNAL FACE " << flush;
	gdtedge e;
	face f, fv, fw;
	int left_bends;
	int right_bends;
	int delta_turns;
	angle_type av, 
		   av1 = _000, 
		   av2;
	angle_type aw, 
		   aw1, 
		   aw2 = _000;
	border_step sv1, sv2, sv;
	border_step sw1, sw2, sw;
	gdt::gdtlist<border_step> seqv, seqw;

	sv1  = border_step_moving_along_edge (ev,opposite (v,ev));
	sw1  = border_step_moving_along_edge (ew,opposite (w,ew));
	sv2  = cyclic_succ_border_step (sv1);
	sw2  = cyclic_succ_border_step (sw1);

	fv = face_of_border_step (sv1);
	fw = face_of_border_step (sw1);
	if (fv!=fw) gdt_error("invalid border_step pair");

	sv2  = cyclic_succ_border_step (sv1);
	sw2  = cyclic_succ_border_step (sw1);
	seqv = border_step_sequence (sv2,sw1);
	seqw = border_step_sequence (sw2,sv1);

	f    = fv;
	av   = angle_of_border_step (sv1);
	aw   = angle_of_border_step (sw1);

	/*
	//    o-----------(v)-----------o
	//   /   --sv1->       --sv2->   \       
	//  /             av              \          
	//                                    
        // seqw            f             seqv  
	//                                          
	//  \             aw              /           
	//   \   <-sw2--       <-sw1--   /          
	//    o-----------(w)-----------o
	*/

	// Verify the angular space required at each end_node
	// in order to preserve the orthogonal embedding
	//
	if (av==_090 || aw==_090)
		gdt_error("no angular space"); 
	    	
	//cout << "\nSplitting face " << flush;
	//print(f);
	//cout << flush;
	
	// Split face f
	// using a method of the parent class.
	//
	ev = edge_of_border_step  (sv1);
	ew = edge_of_border_step  (sw1);
	e  = plan_undi_graph::new_edge(v,ev, w,ew, new_id);

	// since we used a method of the plan_undi_graph,
	// border_step's information must be updated by hand.
	//
	sv = border_step_moving_along_edge (e,v);
	sw = border_step_moving_along_edge (e,w);
	
	set_thickness_of_border_step (sv,1);
	set_thickness_of_border_step (sw,1);
	
	set_pin_number_of_border_step (sv,UNDEFINED_PIN_NUMBER);
	set_pin_number_of_border_step (sw,UNDEFINED_PIN_NUMBER);

	// Retrieve the two faces
	// resulting from the split.
	//	
	fv = right_face_moving_along_edge  (e,v);
	fw = right_face_moving_along_edge  (e,w);

	/*
	//    o-----------(v)-----------o
	//   /   --sv1->   |   --sv2->   \       
	//  /          av1 |  av2         \          
	//                 |                   
        // seqw   fv       e      fw     seqv
	//                 |            
	//  \          aw2 |  aw1         /           
	//   \   <-sw2--   |   <-sw1--   /          
	//    o-----------(w)-----------o
	*/

	// Update the orthogonal embedding on nodes v and w.
	// Face fv is considered (but fw would do as well).
	// When av=aw=_180 there is a unique embedding (av1=av2=aw1=aw2=_090).
	// In the other cases, angles are choosen in order to minimize
	// the number of bends along the new gdtedge.

	delta_turns =
		number_of_right_turns_along_border (sw2,sv1) -
		number_of_left_turns_along_border  (sw2,sv1);
				
	//cout << "\nFace fv is " << id(fv);
	//cout << "\nFace fw is " << id(fw);
	//cout << "\nLeft turns from ";  print(w); cout << " to  "; print(v); cout << " along border of face  fv : " << number_of_left_turns_along_border  (sw2,sv1)<< flush;
	//cout << "\nRight turns from "; print(w); cout << " to  "; print(v); cout << " along border of face  fv : " << number_of_right_turns_along_border (sw2,sv1)<< flush;
				
	left_bends  = 0;
	right_bends = 0;
	
	if (delta_turns<3)
		{
		av1=_090;
		aw2=_090;
		right_bends = 2-delta_turns;
		}
		
	else if (delta_turns==3)
		if (av > _180)      {av1=_180; aw2=_090;}
		else if (aw > _180) {av1=_090; aw2=_180;}
		else                {av1=_090; aw2=_090; left_bends=1;}
		
	else if (delta_turns==4)
		if (av > _270)
			if (aw > _180)      {av1=_180; aw2=_180;}
			else                {av1=_270; aw2=_090;}
		else if (av > _180)
			if (aw > _180)      {av1=_180; aw2=_180;}
			else                {av1=_180; aw2=_090; left_bends=1;}
		else 
			if (aw > _270)      {av1=_090; aw2=_270;}
			else if (aw > _180) {av1=_090; aw2=_180; left_bends=1;}
			else                {av1=_090; aw2=_090; left_bends=2;}
			
	else if (delta_turns==5)
			if (av > _270)
				if (aw > _180)      {av1=_270; aw2=_180;}
				else                {av1=_270; aw2=_090; left_bends=1;}
			else if (av > _180)
				if (aw > _270)      {av1=_180; aw2=_270;}
				else if (aw > _180) {av1=_180; aw2=_180; left_bends=1;}
				else                {av1=_180; aw2=_090; left_bends=2;}
			else 
				if (aw > _270)      {av1=_090; aw2=_270; left_bends=1;}
				else if (aw > _180) {av1=_090; aw2=_180; left_bends=2;}
				else                {av1=_090; aw2=_090; left_bends=3;}
			
	else if (delta_turns>5)
		{
		left_bends = delta_turns-6;
		if (av > _270)
			if (aw > _270)      {av1=_270; aw2=_270;}
			else if (aw > _180) {av1=_270; aw2=_180; left_bends+=1;}
			else                {av1=_270; aw2=_090; left_bends+=2;}
		else if (av > _180)
			if (aw > _270)      {av1=_180; aw2=_270; left_bends+=1;}
			else if (aw > _180) {av1=_180; aw2=_180; left_bends+=2;}
			else                {av1=_180; aw2=_090; left_bends+=3;}
		else 
			if (aw > _270)      {av1=_090; aw2=_270; left_bends+=2;}
			else if (aw > _180) {av1=_090; aw2=_180; left_bends+=3;}
			else                {av1=_090; aw2=_090; left_bends+=4;}
		}

	/*
	//    o-----------(v)-----------o
	//   /   --sv1->   |   --sv2->   \       
	//  /          av1 |  av2         \          
	//                 |                   
        // seqw   fv       e      fw     seqv
	//                 |            
	//  \          aw2 |  aw1         /           
	//   \   <-sw2--   |   <-sw1--   /          
	//    o-----------(w)-----------o
	*/

	// av1 and aw2 have been calculated
	// in order to minimize the number of
	// left_bends or right_bends.
	// av2 and aw1 derive as follows.
	//
	av2 = (angle_type)(av-av1);
	aw1 = (angle_type)(aw-aw2);
	
	//cout << "\n Angle av  is " << av;
	//cout << "\n Angle aw  is " << aw;
	//cout << "\n Angle av1 is " << av1;
	//cout << "\n Angle av2 is " << av2;
	//cout << "\n Angle aw1 is " << aw1;
	//cout << "\n Angle aw2 is " << aw2;
	//cout << "\n Left bends  are " << left_bends;
	//cout << "\n Right_bends are " << right_bends;
		
	// Transform the number of bends
	// into a bend-sequence string
	// and set bends and angle along the new gdtedge e
	//
	std::string bends="";
	while (left_bends>0)  {bends+="L"; left_bends--;}
	while (right_bends>0) {bends+="R"; right_bends--;}
	set_edge_shape (e,v,bends,av2,aw2);

	// Adjust the angles along the border_steps
	// of the faces fv and fw entering nodes v and w.
	//
	set_angle_of_border_step (sv1, av1);
	set_angle_of_border_step (sw1, aw1);
	

	// Now check the number of bends around face f
	//
	if (!(border_is_closed(fv) && border_is_closed (fw)))
		gdt_error("malformed face");

	return e;
	}




	// Description  : Split face 'f' by adding a new gdtedge between 'v' and 'w'.
	// Precondition : ('v' and 'w' are vertices of the border of 'f') and ('v' != 'w')
	// Notes        : When 'v' (or 'w') is the start-gdtnode of a bridge, the new gdtedge is inserted on the right of that bridge,
	//                moving on the bridge from 'v' ('w') to its opposite.
	//		  FUNCTION FOR 4-PLANAR ONLY
	gdtedge
	orth_plan_undi_graph:: 
split_external_face
	(gdtnode v, gdtedge ev, gdtnode w, gdtedge ew, int new_id) 
	{
	//cout << "\nSPLITTING EXTERNAL FACE" << flush;
	gdtedge e;
	face f, fv, fw;
	int left_bends,  left_bends_c1,  left_bends_c2;
	int right_bends, right_bends_c1, right_bends_c2;
	angle_type av, 
		   av1, 
		   av2 = _000, 
		   av1_c1, 
		   av1_c2, 
		   av2_c1, 
		   av2_c2;
	angle_type aw, 
		   aw1 = _000, 
		   aw2, 
		   aw1_c1, 
		   aw1_c2, 
		   aw2_c1, 
		   aw2_c2;
	int delta_turns;
	border_step sv1, sv2, sv;
	border_step sw1, sw2, sw;
	gdt::gdtlist<border_step> seqv, seqw;
	gdtnode start_node;
	angle_type as, at;

	sv1 = border_step_moving_along_edge (ev,opposite (v,ev));
	sw1 = border_step_moving_along_edge (ew,opposite (w,ew));
	sv2  = cyclic_succ_border_step (sv1);
	sw2  = cyclic_succ_border_step (sw1);

	fv = face_of_border_step (sv1);
	fw = face_of_border_step (sw1);
	if (fv!=fw)       gdt_error("invalid border_step pair");
	if (fv!=ext_face) gdt_error("face is not the external one");

	f    = fv;	
	seqv = border_step_sequence (sv2,sw1);
	seqw = border_step_sequence (sw2,sv1);
	
	av   = angle_of_border_step (sv1);
	aw   = angle_of_border_step (sw1);
	
	/*
	//                av
	//                                    
	//    o-----------(v)-----------o
	//   /   <-sv2--       <-sv1--   \    
	//  /                             \          
	//                                    
        // seqv                          seqw  
	//                                          
	//  \                             /           
	//   \   --sw1->       --sw2->   /          
	//    o-----------(w)-----------o
	//
	//        f       aw
	*/

	// Verify the angular space required at each end_node
	// in order to preserve the orthogonal embedding
	//
	if (av==_090 || aw==_090)
		gdt_error("no angular space");
	
	//cout << "\nBefore splitting face number_of_left_turns_along_border  (sv2,sw1)" << flush;
	//number_of_left_turns_along_border  (sv2,sw1);
	//cout << "\nBefore splitting face number_of_left_turns_along_border  (sw2,sv1)" << flush;
	//number_of_left_turns_along_border  (sw2,sv1);
			
	//cout << "\nSplitting face " << flush; print(f);

	e  = plan_undi_graph::new_edge(v,ev, w,ew, new_id);

	// since we used a plan_undi_graph method to create
	// the gdtedge, its border_step's info must be updated by hand
	//	
	sv = border_step_moving_along_edge (e,v);
	sw = border_step_moving_along_edge (e,w);
	
	set_thickness_of_border_step (sv,1);
	set_thickness_of_border_step (sw,1);	

	set_pin_number_of_border_step (sv,UNDEFINED_PIN_NUMBER);
	set_pin_number_of_border_step (sw,UNDEFINED_PIN_NUMBER);	
	
	// Split the face using a method of the parent class.
	// Then retrieve the two faces resulting from the split.
	//
	fv = face_of_border_step (sv);
	fw = face_of_border_step (sw);

	// We have now two possible cases:
	// 1) The new gdtedge goes right (seqv is still external while seqw gets internal)
	// 2) The new gdtedge goes left  (seqw is still external while seqv gets internal)
	//

	/*
	// (case 2)  <---- ? ---->  (case 1)                
	//                 |                   
	//                 |sv
	//                 |                
	//             av2 | av1                   
	//    o-----------(v)-----------o          
	//   /   <-sv2--       <-sv1--   \    fv     
	//  /                             \           
	//                                         
	// seqv                          seqw      
	//                                          
	//  \                             /            
	//   \   --sw1->       --sw2->   /          
	//    o-----------(w)-----------o             
	//             aw1 |  aw2                 
	//                 |                       
	//         fw      |sw                  
	//                 |               
	// (case 2)  <---- ? ---->  (case 1)
	*/

	// For each case angles and bends of the new adge are calculated
        // then the case generating the minimum number of bends is considered.
        
        // ------------------------------ CASE (1) ---------------------------------
	// The new gdtedge goes right (seqv is still external while seqw gets internal)
	// -------------------------------------------------------------------------

	/*
	//                 o-----------------------o                   
	//                 |        <-sw--         |
	//             av2 | av1                   |
	//    o-----------(v)-----------o          |
	//   /   <-sv2--       <-sv1--   \    fv   |  
	//  /                             \        |
	//                                         |
	// seqv                          seqw      e
	//                                         | 
	//  \                             /        |    
	//   \   --sw1->       --sw2->   /         | 
	//    o-----------(w)-----------o          |   
	//             aw1 |  aw2                  |
	//                 |                       |
	//         fw      o-----------------------o                   
	//                          <--sv--           
	*/

	//
	//////////
	        //

		// Calculate the minimum number of left and right bends
		// to be inserted in the new gdtedge, for case (1)
		//
		// When av=aw=_180 there is a unique embedding (av1=av2=aw1=aw2=_090).
		// In the other cases, angles are choosen in order to minimize
		// the number of bends along the new gdtedge.

		delta_turns =
			number_of_left_turns_along_border  (sv2,sw1) -
			number_of_right_turns_along_border (sv2,sw1);
			
		//cout << "\nCase (1) - The old external path from "; print(w); cout << " to "; print(v); cout << " is now internal";
		//cout << "\nThe new internal face is " << id(fv);
		//cout << "\nThe new external face is " << id(fw);
		//cout << "\nLeft turns from ";  print(v); cout << " to  "; print(w); cout << " along the external border : " << number_of_left_turns_along_border  (sv2,sw1);
		//cout << "\nRight turns from "; print(v); cout << " to  "; print(w); cout << " along the external border : " << number_of_right_turns_along_border (sv2,sw1);
					
		left_bends  = 0;
		right_bends = 0;
		
		if (delta_turns<3)
			{
			left_bends = 2-delta_turns;
			if (av > _270)
				if (aw > _270)      {av2=_270; aw1=_270;}
				else if (aw > _180) {av2=_270; aw1=_180; left_bends+=1;}
				else                {av2=_270; aw1=_090; left_bends+=2;}
			else if (av > _180)
				if (aw > _270)      {av2=_180; aw1=_270; left_bends+=1;}
				else if (aw > _180) {av2=_180; aw1=_180; left_bends+=2;}
				else                {av2=_180; aw1=_090; left_bends+=3;}
			else 
				if (aw > _270)      {av2=_090; aw1=_270; left_bends+=2;}
				else if (aw > _180) {av2=_090; aw1=_180; left_bends+=3;}
				else                {av2=_090; aw1=_090; left_bends+=4;}
			}
			
		else if (delta_turns==3)
			if (av > _270)
				if (aw > _180)      {av2=_270; aw1=_180;}
				else                {av2=_270; aw1=_090; left_bends=1;}
			else if (av > _180)
				if (aw > _270)      {av2=_180; aw1=_270;}
				else if (aw > _180) {av2=_180; aw1=_180; left_bends=1;}
				else                {av2=_180; aw1=_090; left_bends=2;}
			else 
				if (aw > _270)      {av2=_090; aw1=_270; left_bends=1;}
				else if (aw > _180) {av2=_090; aw1=_180; left_bends=2;}
				else                {av2=_090; aw1=_090; left_bends=3;}
				
		else if (delta_turns==4)
			if (av > _270)
				if (aw > _180)      {av2=_180; aw1=_180;}
				else                {av2=_270; aw1=_090;}
			else if (av > _180)
				if (aw > _180)      {av2=_180; aw1=_180;}
				else                {av2=_180; aw1=_090; left_bends=1;}
			else 
				if (aw > _270)      {av2=_090; aw1=_270;}
				else if (aw > _180) {av2=_090; aw1=_180; left_bends=1;}
				else                {av2=_090; aw1=_090; left_bends=2;}
					
		else if (delta_turns==5)
			if (av > _180)      {av2=_180; aw1=_090;}
			else if (aw > _180) {av2=_090; aw1=_180;}
			else                {av2=_090; aw1=_090; left_bends=1;}
			
		else if (delta_turns>5)
			{
			av2=_090;
			aw1=_090;
			right_bends = delta_turns-6;
			}
		
		// av2 and aw1 have been calculated
		// in order to minimize the number of
		// left_bends or right_bends.
		// av1 and aw2 derive as follows.
		//
		av1 = (angle_type)(av-av2);
		aw2 = (angle_type)(aw-aw1);

		// Save the results
		// of the analysis on case (1)
		//	
		av1_c1 = av1;
		av2_c1 = av2;
		aw1_c1 = aw1;
		aw2_c1 = aw2;
		left_bends_c1  = left_bends;	
		right_bends_c1 = right_bends;
		
        // ------------------------------ CASE (2) ---------------------------------
	// The new gdtedge goes right (seqv is still external while seqw gets internal)
	// -------------------------------------------------------------------------

	/*
	//          <--sv--                        
	// o-----------------------o                   
	// |                       |                 
	// |                   av2 | av1                   
	// |          o-----------(v)-----------o          
	// |         /   <-sv2--       <-sv1--   \    fv     
	// |        /             av              \           
	// |                                        
	// e     seqv                            seqw      
	// |                                         
	// |        \                             /           
	// |         \   --sw1->       --sw2->   /          
	// |          o-----------(w)-----------o             
	// |  fw               aw1 |  aw2                  
	// |                       |                       
	// o-----------------------o                                 
	//          <-sw--
  	*/
                   
	//
	//////////
	        //

		// Calculate the minimum number of left and right bends
		// to be inserted in the new gdtedge, for case (2)
		//
		// When av=aw=_180 there is a unique embedding (av1=av2=aw1=aw2=_090).
		// In the other cases, angles are choosen in order to minimize
		// the number of bends along the new gdtedge.

		delta_turns =
			number_of_left_turns_along_border   (sw2,sv1) -
			number_of_right_turns_along_border  (sw2,sv1);
			
		//cout << "\nCase (2) - The old external path from "; print(v); cout << " to "; print(w); cout << " is now internal";
		//cout << "\nThe new internal face is " << id(fw);
		//cout << "\nThe new external face is " << id(fv);
		//cout << "\nLeft turns from ";  print(w); cout << " to  "; print(v); cout << " along the external border : " << number_of_left_turns_along_border  (sw2,sv1);
		//cout << "\nRight turns from "; print(w); cout << " to  "; print(v); cout << " along the external border : " << number_of_right_turns_along_border (sw2,sv1);
					
		left_bends  = 0;
		right_bends = 0;
		
		if (delta_turns<3)
			{
			left_bends = 2-delta_turns;
			if (av > _270)
				if (aw > _270)      {av1=_270; aw2=_270;}
				else if (aw > _180) {av1=_270; aw2=_180; left_bends+=1;}
				else                {av1=_270; aw2=_090; left_bends+=2;}
			else if (av > _180)
				if (aw > _270)      {av1=_180; aw2=_270; left_bends+=1;}
				else if (aw > _180) {av1=_180; aw2=_180; left_bends+=2;}
				else                {av1=_180; aw2=_090; left_bends+=3;}
			else 
				if (aw > _270)      {av1=_090; aw2=_270; left_bends+=2;}
				else if (aw > _180) {av1=_090; aw2=_180; left_bends+=3;}
				else                {av1=_090; aw2=_090; left_bends+=4;}
			}
			
		else if (delta_turns==3)
			if (av > _270)
				if (aw > _180)      {av1=_270; aw2=_180;}
				else                {av1=_270; aw2=_090; left_bends=1;}
			else if (av > _180)
				if (aw > _270)      {av1=_180; aw2=_270;}
				else if (aw > _180) {av1=_180; aw2=_180; left_bends=1;}
				else                {av1=_180; aw2=_090; left_bends=2;}
			else 
				if (aw > _270)      {av1=_090; aw2=_270; left_bends=1;}
				else if (aw > _180) {av1=_090; aw2=_180; left_bends=2;}
				else                {av1=_090; aw2=_090; left_bends=3;}
				
		else if (delta_turns==4)
			if (av > _270)
				if (aw > _180)      {av1=_180; aw2=_180;}
				else                {av1=_270; aw2=_090;}
			else if (av > _180)
				if (aw > _180)      {av1=_180; aw2=_180;}
				else                {av1=_180; aw2=_090; left_bends=1;}
			else 
				if (aw > _270)      {av1=_090; aw2=_270;}
				else if (aw > _180) {av1=_090; aw2=_180; left_bends=1;}
				else                {av1=_090; aw2=_090; left_bends=2;}
					
		else if (delta_turns==5)
			if (av > _180)      {av1=_180; aw2=_090;}
			else if (aw > _180) {av1=_090; aw2=_180;}
			else                {av1=_090; aw2=_090; left_bends=1;}
			
		else if (delta_turns>5)
			{
			av1=_090;
			aw2=_090;
			right_bends = delta_turns-6;
			}

		// av1 and aw2 have been calculated
		// in order to minimize the number of
		// left_bends or right_bends.
		// av2 and aw1 derive as follows.
		//
		av2 = (angle_type)(av-av1);
		aw1 = (angle_type)(aw-aw2);
		
		// Save the results
		// of the analysis on case (1)
		//	
		av1_c2 = av1;
		av2_c2 = av2;
		aw1_c2 = aw1;
		aw2_c2 = aw2;
		left_bends_c2  = left_bends;	
		right_bends_c2 = right_bends;
		
        // ------------------------ CHOOSE BEST CASE -------------------------------
	// Select the case generating the minimum number of bends along the new gdtedge
	// -------------------------------------------------------------------------
	//
	if ((left_bends_c1 + right_bends_c1) < (left_bends_c2 + right_bends_c2))
		{
		// Case (1) is chosen
		//
		av1 = av1_c1;
		av2 = av2_c1;
		aw1 = aw1_c1;
		aw2 = aw2_c1;
		left_bends  = left_bends_c1;	
		right_bends = right_bends_c1;
		ext_face    = fw;
		// SF
		start_node  = w;
		as = aw2;
		at = av2;
		}
	else
		{
		// Case (2) is choosen
		//
		av1 = av1_c2;
		av2 = av2_c2;
		aw1 = aw1_c2;
		aw2 = aw2_c2;
		left_bends  = left_bends_c2;	
		right_bends = right_bends_c2;
		ext_face    = fv;
		// SF
		start_node  = v;
		as = av2;
		at = aw2;
		}
	
        // ------------------------ CONTINUE ---------------------------------------
	// Set angles and shapes according to the choosen case
	// -------------------------------------------------------------------------
	
	//cout << "\n Angle av  is " << av;
	//cout << "\n Angle aw  is " << aw;
	//cout << "\n Angle av1 is " << av1;
	//cout << "\n Angle av2 is " << av2;
	//cout << "\n Angle aw1 is " << aw1;
	//cout << "\n Angle aw2 is " << aw2;
	//cout << "\n Left bends  are " << left_bends << flush;
	//cout << "\n Right_bends are " << right_bends << flush;
	//cout << "\n External face is " << id(ext_face) << flush;
	
	// Transform the number of bends
	// into a bend-sequence string
	// and set bends and angle along the new gdtedge e
	//
	std::string bends="";
	while (left_bends>0)  {bends+="L"; left_bends--;}
	while (right_bends>0) {bends+="R"; right_bends--;}

	// SF
	//set_edge_shape (e,v,bends,av2,aw2);
	set_edge_shape (e,start_node,bends,as,at);
	
	// Adjust the angles along the border_steps
	// of the faces fv and fw entering nodes v and w.
	//
	set_angle_of_border_step (sv1, av1);
	set_angle_of_border_step (sw1, aw1);

	// Now check the splitted faces
	//
	if (!(border_is_closed(fv) && border_is_closed (fw)))
		gdt_error("malformed face");
	
	return e;
	}



 	void 
 	orth_plan_undi_graph::
quick_orth_for_fixed_embedding 
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
	
 	if (diagnostic_printouts_are_enabled)
 		{
 		cout << "\n\nApplying the quick-orthogonal layout algorithm ...";
		}

 	if (number_of_edges()==1)
 		{
 		if (diagnostic_printouts_are_enabled) cout << "\n. Applying shortcut for one-gdtedge graph ... ";
 		set_edge_shape (first_edge(),first_node(),"",_360,_360);
		if (diagnostic_printouts_are_enabled) cout << "\n. Algorithm completed\n";
		return;		
 		} 

 	gdtedge e;
 	face f;
 	gdtnode v,vd;
 	border_step s;
 	plan_undi_graph dpug;

 	gdt::gdtnode_array<int> y_of_node;
 	gdt::gdtnode_array<int> xmin_of_node;
 	gdt::gdtnode_array<int> xmax_of_node;
 	
 	gdt::gdtedge_array<int> x_of_edge;
 	gdt::gdtedge_array<int> ymin_of_edge;
 	gdt::gdtedge_array<int> ymax_of_edge;
 	 	
 	gdt::gdtnode_array<int> level_of_node;
 	gdt::gdtnode_array<int> level_of_dual_node;
 	gdt::gdtmap<face,gdtnode>  dual_node_representing_face(NULL_NODE);
 
  	gdt::gdtedge_array<gdtnode> original_start_of_edge;
	 	
	// Init temporary arrays
	//
 	   y_of_node.init (nodes_and_edges(), 0);
 	xmin_of_node.init (nodes_and_edges(), 0);
 	xmax_of_node.init (nodes_and_edges(), 0);

 	   x_of_edge.init (nodes_and_edges(), 0);
 	ymin_of_edge.init (nodes_and_edges(), 0);
 	ymax_of_edge.init (nodes_and_edges(), 0);
 	
 	original_start_of_edge.init (nodes_and_edges(), NULL_NODE);

	// Remove existings bends from edges
	// and save edges initial direction
	
	forall_faces (f,*this)
		forall_face_border_steps (s,f)
			reset_border_step_info(s);	
	
	forall_edges (e,*this)
		original_start_of_edge[e]=start(e);
 	
	s = first_border_step (ext_face);
	e = edge_of_border_step (s);	
 	make_directed  (start_of_border_step(s),stop_of_border_step(s));
 	
 	//cout << "Graph with st-numbering : \n" << flush;  
 	//plan_undi_graph::print (COMPLETE);
 	
  	calculate_dual (dpug, e, dual_node_representing_face);
 	
 	if (diagnostic_printouts_are_enabled)
 		{
 		cout << "done";
 		cout << "\n. Creating visibility representation ... " << flush;
		}

	level_of_node.init      (     nodes_and_edges(), 0);
	level_of_dual_node.init (dpug.nodes_and_edges(), 0);

 	     calculate_level_of_all_nodes (level_of_node);
 	dpug.calculate_level_of_all_nodes (level_of_dual_node);
 	
  	forall_edges (e,*this)
 		{
		f  = left_face_moving_along_edge (e,start(e));
 		vd = dual_node_representing_face [f];
 		
		   x_of_edge[e] = level_of_dual_node [vd];
  		ymin_of_edge[e] = level_of_node [start(e)];
		ymax_of_edge[e] = level_of_node [stop(e)];
 		}

	forall_nodes (v,*this)
 		{
 		int cur_adj_face_level;
 		int min_adj_face_level;
 		int max_adj_face_level;
 		e  = first_adj_edge(v);
		f  = left_face_moving_along_edge (e,start(e));
 		vd = dual_node_representing_face [f];
 		min_adj_face_level = level_of_dual_node [vd];
 		max_adj_face_level = level_of_dual_node [vd];
 		
		//Modified by A.M.
 		forall_edges_adjacent_node(e,v,*this)
 			{
			f  = left_face_moving_along_edge (e,start(e));
 			vd = dual_node_representing_face [f];
 			cur_adj_face_level = level_of_dual_node [vd];
 			min_adj_face_level = gdt_minimum (cur_adj_face_level, min_adj_face_level);
 			max_adj_face_level = gdt_maximum (cur_adj_face_level, max_adj_face_level);
 			}
 				 		
 		   y_of_node[v] = level_of_node [v];
  		xmin_of_node[v] = min_adj_face_level;
		xmax_of_node[v] = max_adj_face_level;
		}

 	if (diagnostic_printouts_are_enabled)
 		{
 		cout << "done";
 		cout << "\n. Translating to orthogonal representation ... " << flush;
		}
		 	
	// A visibility representation is now stored into the coordinate arrays.
	// The following commented code is just a test print-out
	// to check the representation is correct.
	//
	//   forall_nodes (v,*this)
	//    	{
	//   	cout << "\nNode "; print(v);
	//    	cout << xmin_of_node[v] << " ---(" << y_of_node[v] << ")--- " << xmax_of_node[v];
	//    	}
	//   
	//   forall_edges (e,*this)
	//    	{
	//    	cout << "\nEdge "; print(e);
	//    	cout << ymin_of_edge[e] << " ---(" << x_of_edge[e] << ")--- " << ymax_of_edge[e];
	//    	}


	// We don't need anymore the st-orientation
	// forced on our graph in order to calculate gdtnode-levels.
	// It is now safe to restore the original direction on every gdtedge.
	//
	forall_edges (e,*this)
		{
		v = original_start_of_edge[e];
		if (v) make_directed(e,v);
		else make_undirected(e);
		}

	// In order to apply the bend-stretching transformations,
	// we now translate the geometrical information into
	// topological ones. That is, we translate the
	// visibility representation into an orthogonal one.

	forall_nodes (v,*this)
		{
		// The following code place into the arrays of edges et[0..3] and eb[0..3]
		// all the edges adjacent to gdtnode v, ordered from the left to right.
		
		gdtnode w;
		int top_degree = 0;              // Number of edges coming from top
		int bot_degree = 0;              // Number of edges coming from bottom
		gdt::gdtp_queue<int,gdtedge> top_adj_edges; // Queue of edges adjacent from bottom.
		gdt::gdtp_queue<int,gdtedge> bot_adj_edges; // Queue of edges adjacent from bottom.
		gdt::gdtarray<gdtedge> et(4); // Edges adjacent from top,    ordered by x increasing
		gdt::gdtarray<gdtedge> eb(4); // Edges adjacent from bottom, ordered by x increasing
				
		//Modified by A.M.
 		forall_edges_adjacent_node(e,v,*this)
			{
			w = opposite(v,e);
			if (y_of_node[w] > y_of_node[v])
				{
				top_degree +=1;
				top_adj_edges.insert (x_of_edge[e],e);
				}
			else 
				{
				bot_degree +=1;
				bot_adj_edges.insert (x_of_edge[e],e);
				}
			}
		
		int i;
		gdt::pq_item pqi;
		for (i=0; i<top_degree; i++)
			{
			pqi   = top_adj_edges.find_min ();
			et[i] = top_adj_edges.inf      (pqi);
			        top_adj_edges.del_item (pqi);
			}
		for (i=0; i<bot_degree; i++)
			{
			pqi   = bot_adj_edges.find_min ();
			eb[i] = bot_adj_edges.inf      (pqi);
			        bot_adj_edges.del_item (pqi);
			}			


		// cout << "\nNode"; print(v);
		// cout << "\nTotal  degree : " << degree(v);
		// cout << "\nTop    degree : " << top_degree;
		// cout << "\nBottom degree : " << bot_degree;
		// for (i=0; i<top_degree; i++) {cout << "\nTop    gdtedge [" << i << "] is "; print (et[i]);}
		// for (i=0; i<bot_degree; i++) {cout << "\nBottom gdtedge [" << i << "] is "; print (eb[i]);}


		// The following special code treats simmetric cases.
		// When bot_degree > top_degree, just swap top with bottom
		// and invert the edges order along the x-axis.
		//
		// Total  |  Top   | Bottom
		// degree | degree | degree
		// -------|--------|--------
		//    4   |   4    |   0
		//    4   |   3    |   1
		//    4   |   2    |   2
		//    4   |   1    |   3     <== Symmetric case
		//    4   |   0    |   4     <== Symmetric case
		//    3   |   3    |   0
		//    3   |   2    |   1
		//    3   |   1    |   2     <== Symmetric case 
		//    3   |   0    |   3     <== Symmetric case 
		//    2   |   2    |   0      
		//    2   |   1    |   1      
		//    2   |   0    |   2     <== Symmetric case      
		//    1   |   1    |   0       
		//    1   |   0    |   1     <== Symmetric case
		//
		if (bot_degree>top_degree)
			{
			int top_degree_old;
			int bot_degree_old;
			gdt::gdtarray<gdtedge> et_old(4);
			gdt::gdtarray<gdtedge> eb_old(4);
			et_old = et;
			eb_old = eb;
			top_degree_old = top_degree;
			bot_degree_old = bot_degree;
			for (i=0; i<top_degree_old; i++) eb[i] = et_old[top_degree_old-1-i]; bot_degree = top_degree_old;
			for (i=0; i<bot_degree_old; i++) et[i] = eb_old[bot_degree_old-1-i]; top_degree = bot_degree_old;
			}

		// Now use the et[] and eb[] arrays
		// to assign an orthogonal representation
		// for each (top,bottom) degree combination

		switch (bot_degree)
			{
			case 0:
				switch (top_degree)
					{
					case 1:
						//
						// t0        t0
						//  |         |
						//  |   ==>   |
						//  -         v
						set_edge_shape_near_node (et[0],v,"" , _360);
						break;
					case 2:
						//
						// t0 t1        t0 t1
						//  |  |         |  |
						//  |  |   ==>   |  |
						//  ----         o--v
						set_edge_shape_near_node (et[0],v,"L", _270);
						set_edge_shape_near_node (et[1],v,"" , _090);
						break;
					case 3:
						//
						// t0 t1 t2        t0 t1 t2
						//  |  |  |         |  |  |
						//  |  |  |   ==>   |  |  |
						//  -------         o--v--o
						//                     
						set_edge_shape_near_node (et[0],v,"L", _180);
						set_edge_shape_near_node (et[1],v,"" , _090);
						set_edge_shape_near_node (et[2],v,"R", _090);
						break;
					case 4:
						//
						// t0 t1 t2 t3        t0 t1 t2 t3
						//  |  |  |  |         |  |  |  |
						//  |  |  |  |   ==>   |  |  |  |
						//  ----------         |  o--v--o
						//                     |     |
						//                     o-----o
						//
						set_edge_shape_near_node (et[0],v,"LL", _090);
						set_edge_shape_near_node (et[1],v,"L" , _090);
						set_edge_shape_near_node (et[2],v,""  , _090);
						set_edge_shape_near_node (et[3],v,"R" , _090);
						break;
					}
				break;
			case 1:
				switch (top_degree)
					{
					case 1:
						//
						// t0        t0
						//  |         |
						//  |   ==>   |
						//  -         v
						//  |         |
						//  |         |
						//  b0        b0
						//
						set_edge_shape_near_node (et[0],v,"" , _180);
						set_edge_shape_near_node (eb[0],v,"" , _180);
						break;
					case 2:
						//
						// t0 t1        t0 t1
						//  |  |         |  |
						//  |  |   ==>   |  |
						//  ----         o--v
						//    |             |
						//    |             |
						//    b0            b0
						//
						set_edge_shape_near_node (et[0],v,"L" , _090);
						set_edge_shape_near_node (et[1],v,""  , _090);
						set_edge_shape_near_node (eb[0],v,""  , _180);
						break;
					case 3:
						//
						// t0 t1 t2        t0 t1 t2
						//  |  |  |         |  |  |
						//  |  |  |   ==>   |  |  |
						//  -------         o--v--o
						//     |               |
						//     |               |
						//     b0              b0
						//
						set_edge_shape_near_node (et[0],v,"L" , _090);
						set_edge_shape_near_node (et[1],v,""  , _090);
						set_edge_shape_near_node (et[2],v,"R" , _090);
						set_edge_shape_near_node (eb[0],v,""  , _090);
						break;
					}
				break;
			case 2:
				switch (top_degree)
					{
					case 2:
						//
						// t0 t1        t0 t1
						//  |  |         |  |
						//  |  |   ==>   |  |
						//  ----         o--v--o
						//  |  |            |  |
						//  |  |            |  |
						//  b0 b1           b0 b1
						//
						set_edge_shape_near_node (et[0],v,"L" , _090);
						set_edge_shape_near_node (et[1],v,""  , _090);
						set_edge_shape_near_node (eb[0],v,""  , _090);
						set_edge_shape_near_node (eb[1],v,"L"  ,_090);
						break;
					}
				break;
			}
		}

 	if (diagnostic_printouts_are_enabled)
 		{
 		cout << "done";
 		cout << "\n. Stretching bends ... " << flush;
 		}
 		
	// The orthogonal representation is now complete.
	// Some optimization is now performed by applying
	// the bend-stretching trasformations.
	// Do not change the order of the following calls.

	apply_t1_bend_stretching();
	apply_t2_bend_stretching();
	apply_t3_bend_stretching();

 	if (diagnostic_printouts_are_enabled)
 		{
 	  	cout << "\n. Algorithm completed\n" << flush;
 		}
	}
	

// ------------------------------------------------------------------
// These declarations are here because Borland C++ cannot instantiate 
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
	gdtnode	node_corr;
	face	face_corr;
	void set (fnode_type o, gdtnode n, face f)
		{
		type      = o;
		node_corr = n;
		face_corr = f;
		}
	};
// ------------------------------------------------------------------

 
  	int 
 	orth_plan_undi_graph::
optimal_orth_for_fixed_embedding_with_constraints 
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
	
 	if (diagnostic_printouts_are_enabled)
 		{
 		cout << "\n\nApplying the optimal-orthogonal layout algorithm ...";
 		cout << "\n. Creating the flow-network ... " << flush;
		}
	
	// ---------------------------- 0 ------------------------------
	// build a set of arrays to store the constraints on the drawing
	// -------------------------------------------------------------
	
	gdt::gdtedge_array<gdtnode> 	    bend_dir_node (nodes_and_edges(),NULL_NODE);	// bend-direction (spirality)
	gdt::gdtmap<border_step,int> 	    angle_degree  (-1);					// angle on the right of the border_step at the "start" of the border_step  
	
	// init the "angle_degree" map
	//
	   {
	   face f;
	   border_step s;
	   forall_faces (f,*this)
		   forall_face_border_steps (s,f) angle_degree[s] = -1;
	   }
	  
	constraint c;
	constraint_bend_direction_on_edge bdc; 
	constraint_angle_sweep		  asc;
	
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
			
		else if (type_of_constraint(c) == ANGLE_SWEEP)
			{
			asc = (constraint_angle_sweep)c;	// casting
			gdtedge re = asc -> get_ref_edge();
			gdtnode rn = asc -> get_ref_node();
			angle_type alpha = asc -> get_angle_value();
			
			border_step s = border_step_moving_along_edge (re,rn);
			int a = angle_to_int(alpha);
			
			angle_degree[s] = a;
			}
			
	
 	// ------------------------------ 1 --------------------------------
 	// build the flow_network fn associated with the planar embedding
 	// of the current graph, and external face "ef". 
	// The construction takes in account the constraints on the drawing. 
 	// ----------------------------------------------------------------- 
 		
 	gdtnode v;
	gdtedge e;
	face f,g;
	border_step s;

 	flow_dire_graph fn;		// the flow net

 	gdtnode vfn, wfn;     		// flow_net nodes corresponding to nodes v and w
 	gdtnode ffn, gfn; 			// flow_net nodes corresponding to faces f and g 
	gdtedge efn;     			// flow_net gdtedge
	
	int total_cost = INFINITE;	// total bend cost
	
	forall_faces (f,*this) 
		forall_face_border_steps (s,f) reset_border_step_info(s);
	
 	gdt::gdtnode_array<gdtnode> 	    fnode_corr_to_node (nodes_and_edges(), NULL_NODE);	// current graph gdtnode --> flow net gdtnode
 	gdt::gdtmap<face,gdtnode>   	    fnode_corr_to_face(NULL_NODE);				// current graph face --> flow net gdtnode
 	gdt::gdtnode_map<struct_fnode_info> fnode_info (fn.nodes_and_edges());		// flow net gdtnode      --> gdtnode/face current graph

	// ---------------------------------
 	// Create all nodes of the flow_net. 
 	// ---------------------------------		
	
  	forall_nodes (v,*this)	// vertex-nodes
  		{
  		int deg_v = degree(v);
  		vfn = fn.new_node();
  		fnode_info[vfn].set(NODE, v, NULL_FACE);
 		fn.set_balance(vfn,4-deg_v);
 		fnode_corr_to_node[v] = vfn;
 		}

 	forall_faces (f,*this)	// face-nodes
 		{
 		vfn = fn.new_node();
 		fnode_info[vfn].set(FACE, NULL_NODE, f);
 		fnode_corr_to_face[f] = vfn;
 		if (f == ext_face)	fn.set_balance(vfn,-(degree(f,true)+4));		
		else 			fn.set_balance(vfn,4-degree(f,true));
 		}
 		
	
	gdt::gdtedge_map<border_step> zero_angle_b 	(fn.nodes_and_edges(),NULL_BORDER_STEP);
	gdt::gdtedge_map<border_step> non_zero_angle_b	(fn.nodes_and_edges(),NULL_BORDER_STEP);	
	
	
	// --------------------------------------
	// Create all the arcs (vfn,ffn) and
	// (ffn,vfn), according to the angle
	// and to the bend_direction constraints.
	// Further, create all the (ffn,gfn) arcs
	// according to the bend constraints.
	// --------------------------------------
	
	forall_nodes (v,*this)
		{
		int deg_v = degree(v);
		vfn = fnode_corr_to_node[v];
		
		// -------------------------------------------------
		// The variable "A" is the total amount of the flow
		// that must be fixed around gdtnode "v" according
		// to the angle constraints. From this flow and from
		// the degree of "v" it is possible to understand 
		// if some edges around "v" will form a zero degree
		// angle or not.
		// -------------------------------------------------
		
		int A = 0;	// total amount of constrained flow around v
		
		forall_edges_adjacent_node (e,v,*this)
			{
			s = border_step_moving_along_edge (e,v);
			if (angle_degree[s] >= 0) A += (angle_degree[s]/90 - 1);
			}
		int D = (4 - deg_v) - A;  // if (D < 0) some edges around v will form a zero degree angle
		
		//
		
		forall_edges_adjacent_node (e,v,*this)
			{
			s = border_step_moving_along_edge (e,v);
			
			// ------------------
			// add arcs (ffn,gfn)
			// ------------------
			
			f   = face_of_border_step(s);
			g   = left_face_moving_along_edge (e,v);
			ffn = fnode_corr_to_face[f];
			gfn = fnode_corr_to_face[g];
			
			if ( 
			   (constraint_on_bends_of_edge[e] != NONE) &&
			   (bend_dir_node[e] == NULL_NODE || bend_dir_node[e] == v)
			   )
			   {
			   efn = fn.find_directed_edge(ffn,gfn);
			   if (!efn) 
				   {
				   efn = fn.new_edge(ffn,gfn);
				   if (constraint_on_bends_of_edge[edge_of_border_step(s)] == ANY)
					   fn.set_edge_info (efn,INFINITE,0,0,0);
				   else	fn.set_edge_info (efn,INFINITE,0,1,0);
				   zero_angle_b[efn]     = NULL_BORDER_STEP;
				   non_zero_angle_b[efn] = NULL_BORDER_STEP;
				   }
			   else 
				   {
				   if (
				      (constraint_on_bends_of_edge[edge_of_border_step(s)] == ANY) && 
				      (fn.get_cost(efn) > 0)
				      ) fn.set_edge_info (efn,INFINITE,0,0,0);
				   }
			   }			
			
			// --------------------------------			
			// add arcs (vfn,ffn) and (ffn,vfn)
			// --------------------------------
			
			int alpha = angle_degree[s];
			switch (alpha)
				{
				case 90 :
				case 180:
				case 270:
				case 360: 
					{
					f     = face_of_border_step(s);
					ffn   = fnode_corr_to_face[f];
					efn   = fn.new_edge(vfn,ffn);
					int k = (alpha/90 - 1); 
					fn.set_edge_info(efn,k,k,0,0);
					zero_angle_b[efn]     = NULL_BORDER_STEP;
					non_zero_angle_b[efn] = s;
					}   
					break;
				case  0:
					{
					gdtedge e_pred = cyclic_adj_pred(e,v);
					if((constraint_on_bends_of_edge[e_pred] != NONE) &&
					   (bend_dir_node[e_pred] == NULL_NODE || bend_dir_node[e_pred] == v))
					   {
					   s   = border_step_moving_along_edge(e_pred,v);
					   f   = face_of_border_step(s);
					   ffn = fnode_corr_to_face[f];
		   			   efn = fn.new_edge(ffn,vfn);
					   fn.set_edge_info(efn,1,1,1,0);
					   zero_angle_b[efn]     = s;
					   non_zero_angle_b[efn] = NULL_BORDER_STEP;	
					   }
					//else gdt_error("constraint collision");    
					}
					break;

				case -1: // no angle constraint
					{
					if (D >= 0) 
						{
						f   = face_of_border_step(s);
						ffn = fnode_corr_to_face[f];
						efn = fn.new_edge(vfn,ffn);
						fn.set_edge_info(efn,INFINITE,0,0,0);
						zero_angle_b[efn]     = NULL_BORDER_STEP;
						non_zero_angle_b[efn] = s;
						}

					else // D < 0
						{
						gdtedge e_pred = cyclic_adj_pred(e,v);
						if((constraint_on_bends_of_edge[e_pred] != NONE) &&
					   	   (bend_dir_node[e_pred] == NULL_NODE || bend_dir_node[e_pred] == v))
					   	   {
					   	   s   = border_step_moving_along_edge(e_pred,v);
						   f   = face_of_border_step(s);
						   ffn = fnode_corr_to_face[f];
		   				   efn = fn.new_edge(ffn,vfn);
						   fn.set_edge_info(efn,1,0,1,0);
						   zero_angle_b[efn]     = s;
						   non_zero_angle_b[efn] = NULL_BORDER_STEP;
						   }
						}
					}
					break;
				default: 
					gdt_error("invalid angle constraint"); 
					break;
				}
			}  
		}
	

 		
 	if (diagnostic_printouts_are_enabled)
 		{
 		cout << "done";
 		cout << "\n. Calculating the minumum-cost flow ... " << flush;
 		}
 		
 	
 	
 	 //------------------------------------------------------------
 	 //     PRINT THE FLOW_NET fn (THIS PART IS TEMPORARY)
 	 //------------------------------------------------------------
 	 //
	/*
 	 int i; 
 	 cout << "\nFLOW NETWORK : " << "\n" << flush;
 	 
 	 gdt::gdtnode_array<std::string> flabel(fn.nodes_and_edges());
 	 char temp[255];
	 
	 forall_nodes(vfn,fn)
	 	{
	 	struct_fnode_info& fni = fnode_info[vfn];
	 	if (fni.type == NODE) sprintf(temp, "v%d", id(fni.node_corr));
	 	else
	 		if (fni.type == FACE) sprintf(temp, "f%d", id(fni.face_corr));
	 		else                  sprintf(temp, "x");
	 	flabel[vfn] = temp;
	 	}
	 forall_nodes(vfn,fn)
	 	{
	 	int b  = fn.get_balance(vfn);
	 	cout << "\n" << flabel[vfn] << "[" << fn.id(vfn) << "] " << "<" << b << ">:";
	 	forall_edges_leaving_node(efn,vfn,fn)
	 		{
	 		int uc = fn.get_upper_capacity(efn);
	 		int lc = fn.get_lower_capacity(efn);
	 		int c  = fn.get_cost(efn);
	 		cout << flabel[fn.opposite(vfn,efn)] << "[" << lc << "," << uc << "," << c << "] ";
	 		}
	 	}
	 
	 std::cin >> i;
	*/
 	// --------------------------- 2 ------------------------------
 	// find a minimum cost flow of maximum value in fn.
	// ------------------------------------------------------------
	
	
	if (!fn.min_cost_flow()) return total_cost;	// return INFINITE cost
			
	total_cost = fn.flow_cost();
	
 	if (diagnostic_printouts_are_enabled)
 		{
 		cout << "done";
 		cout << "\n. Translating to orthogonal representation ... " << flush;
		}
		 	
	
	//------------------------------------------------------------
	// PRINT THE FLOW ALONG ALL EDGES (THIS PART IS TEMPORARY)
	//------------------------------------------------------------
	//
	//cout << "\nFLOW : \n" << flush;
	//forall_edges(efn,fn)
	// 	{
	// 	vfn = fn.start(efn);
	// 	wfn = fn.stop (efn);
	// 	cout << "\nx[" << flabel[vfn] << "," << flabel[wfn] << "] = " << fn.get_flow(efn);
	// 	}	
	// cout << "\n";
	// cin >> i;
	
 	// ------------------------------ 3 ---------------------------------
 	// build the orthogonal lists by the min_cost_flow computed in step 2
	// ------------------------------------------------------------------
	
	gdt::gdtedge_array<bool> mark(fn.nodes_and_edges(),false);
	forall_edges (efn,fn)
		if (!mark[efn])
			{
			vfn = fn.start(efn);
			wfn = fn.stop(efn);
			if ((fnode_info[vfn].type == NODE)&&
			    (fnode_info[wfn].type == FACE))
				{
				// ---------------------
				// case of gdtedge (v -> f)
				// ---------------------
				//
				int xvf = fn.get_flow(efn);
				angle_type angle = _000;
				switch (xvf)
					{
					case 0: angle = _090; break;
					case 1: angle = _180; break;
					case 2: angle = _270; break;
					case 3: angle = _360; break;
					default: break;
					}
				v = fnode_info[vfn].node_corr;
				f = fnode_info[wfn].face_corr;
				s = non_zero_angle_b[efn];
				s = cyclic_pred_border_step(s);
				set_angle_of_border_step (s,angle);
				mark[efn] = true;
				}
			else    
				if ((fnode_info[vfn].type == FACE)&&
			    	    (fnode_info[wfn].type == NODE))
			    	     {				
			    	     // ---------------------
			    	     // case of gdtedge (f -> v)
			    	     // ---------------------
			    	     //
			    	     if (fn.get_flow(efn))
			    	     	{
			    	     	border_step sinv;
			   	 	s    = zero_angle_b[efn];
			    		sinv = opposite_border_step(s);
			    		
			    	     	gdt::gdtlist<bend_type> s_bends    = bends_of_border_step(s), 
			    	     		                sinv_bends = bends_of_border_step(sinv);			    	    	
					
			    		s_bends.push(right);
					sinv_bends.append(left);
									
  					set_angle_of_border_step (sinv,_000);
  					set_bends_of_border_step (s,s_bends);
  					set_bends_of_border_step (sinv,sinv_bends);
			    		} 
			    	      }   
			else
				if ((fnode_info[vfn].type == FACE) &&
				    (fnode_info[wfn].type == FACE))
				     {
				     // ---------------------
				     // case of gdtedge (f -> g)
				     // ---------------------
				     //
				     f = fnode_info[vfn].face_corr;
				     g = fnode_info[wfn].face_corr;
				     gdtedge efn_inv = fn.reverse_of_edge(efn);
				     if (!efn_inv) 
				     	if (f == g) efn_inv = efn; // case of selfloop (bridge)
				     
				     int xfg = fn.get_flow(efn);
				     int xgf; 
				     if (efn_inv) xgf = fn.get_flow(efn_inv);
				     else	  xgf = 0;
				     
				     border_step sinv;
				     
				     gdt::gdtlist<gdtedge> Rfg = edges_shared_by_faces (f,g);
				     gdt::list_item it;
				     
				     // remove from Rfg all rigid edges in this direction
				     //
				     forall_items (it,Rfg)
				     	{
				     	e = Rfg.inf(it);  
				     	v = bend_dir_node[e];
				     	if (
				     	   (constraint_on_bends_of_edge[e] == NONE) ||
				     	   (v != NULL_NODE && right_face_moving_along_edge(e,v) != f)
				     	   ) Rfg.del_item(it);
				     	}
				    
				     // choose an gdtedge to assign flow
				     //
				     if (fn.get_cost(efn) == 0)
				     	{
				     	// find an gdtedge with constraint_on_bends_of_edge ANY in Rfg
				     	//
				     	forall (e,Rfg)
				     		if (constraint_on_bends_of_edge[e] == ANY) break;
				     	}
				     else
					{ 
					// find an gdtedge with constraint_on_bends_of_edge MINIMAL in Rfg
					//
					forall (e,Rfg)
						if (constraint_on_bends_of_edge[e] == MINIMAL) break;
					}
				     
				     
				     if (right_face_moving_along_edge(e,source(e)) == f)
				     	{
				     	s    = border_step_moving_along_edge (e,source(e));
				     	sinv = border_step_moving_along_edge (e,target(e));
				     	}
				     else
				     	{
				     	s    = border_step_moving_along_edge (e,target(e));
				     	sinv = border_step_moving_along_edge (e,source(e));
				     	}	
				     
				     gdt::gdtlist<bend_type> s_bends,  
				      		             sinv_bends;
				      		     
				     gdt::gdtlist<bend_type> s_zero_angle_bends = bends_of_border_step(s); 
				   
				     
				     int i;
				     
				     if (f != g)	//general case
				     	{
				     	for (i=1; i<=xfg; i++)
				     		{
				     		s_bends.append(right);
				     		sinv_bends.append(left);
				     		}
				     	for (i=1; i<=xgf; i++)
				     		{
				     		s_bends.append(left);
				     		sinv_bends.push(right);
				     		}
				     	}
				     else		//bridge case
				     	{
				     	for (i=1; i<=xfg; i++)  s_bends.append(right);
				     	for (i=1; i<=xgf; i++)  sinv_bends.append(left);
				     	}
				     
				     int zero_angles = s_zero_angle_bends.size();
				     switch (zero_angles)
				     	{
				     	case 0: break;
				     	case 1: 
				     		{
				     		bend_type bend = s_zero_angle_bends.head();
				     		if (bend == right)	//zero angle on sinv
				     			{
				     			s_bends.push(right);
				     			sinv_bends.append(left);
				     			}
				     		else 			//zero angle on s
				     			{
				     			s_bends.append(left);
				     			sinv_bends.push(right);
				     			}
				     		}
				     		break;
				     	case 2: 
				     		{
				     		s_bends.push(right);
				     		s_bends.append(left);
				     		sinv_bends.push(right);
				     		sinv_bends.append(left);
				     		}
				     		break;
				     	}

				     set_bends_of_border_step(s,s_bends);
				     set_bends_of_border_step(sinv,sinv_bends);
				     
				     mark[efn]=true;
				     if (efn_inv) mark[efn_inv]=true;
				     }	
			}
 	if (diagnostic_printouts_are_enabled)
 		{
 		cout << "done";
 		cout << "\n. Algorithm completed\n" << flush;
		}
	
	return total_cost;
	}
  	
 	
	int
	orth_plan_undi_graph::
slow_orth ()
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
	int cost = 0;
	
	bool            printouts_were_enabled;
	plan_undi_graph best_planar_embedding;
	face            external_face;
	
 	if (diagnostic_printouts_are_enabled)
 		{
 		cout << "\n\nApplying the slow-orthogonal layout algorithm ...";
 		cout << "\n. Creating the SPQR tree ... " << flush;
		}
	
	// -------------------------------------------------------------
	// Build an SPQR tree holding the structure of the current graph
	// Only the plan part of *this is considered
	// -------------------------------------------------------------

	SPQR_tree T (*this);	// all constraints will be 
				// automatically updated 
				// on all skeletons of T

 	if (diagnostic_printouts_are_enabled)
 		{
 		cout << "done";
 		cout << "\n. Calculating the best embedding ... " << flush;
		}
	
	// -----------------------------------------------------
	// Find the best embedding, and its external face,
	// with a Branch&Bound algorithm based on the SPQR-trees
	// -----------------------------------------------------
	
	if (T.find_best_embedding_with_external_face (best_planar_embedding,external_face) == INFINITE) return INFINITE;

	printouts_were_enabled = diagnostic_printouts_are_enabled;
 	if (diagnostic_printouts_are_enabled)
 		{
 		cout << "done";
 		cout << "\n. Calculating the optimal orthogonal shape of the best embedding ... " << flush;
		diagnostic_printouts_are_enabled = false;
		}
	
	// -----------------------------------------------------------------------------------------
	// Calculate the orth part (i.e. the orthogonal shape) of the best embedding
	// with the Optimal Orthogonal algorithm (the Tamassia one)
	// -----------------------------------------------------------------------------------------

	make_embedding_as (best_planar_embedding);
	
	face f,ef = NULL_FACE;
	gdt::gdtlist<face> faces = all_faces();
	gdt::list_item  pos   = faces.first();
	bool found_face  = false;
	
	// find face corresponding to the external face 
	//
	while (!found_face)
		{
		if (!pos) gdt_error("slow_orth. No external face detected");
		f = faces.inf(pos);
		if (degree(f) == best_planar_embedding.degree(external_face))
			{
			gdtedge e;
			found_face = true;
			forall_GDT_face_edges (e,f)
				{
				gdtedge bpe_e = best_planar_embedding.corresponding_edge (e,*this);
				if (!best_planar_embedding.edge_belongs_to_face(bpe_e,external_face)) 
					{
					found_face = false;
					break;
					}
				}
			if (found_face) ef = f;
			}
		pos = faces.succ(pos);
		}
		
	//cout << "\nfound external face " << id(ef) << flush;

	ext_face = ef;
	cost = optimal_orth_for_fixed_embedding_with_constraints ();
	
	diagnostic_printouts_are_enabled = printouts_were_enabled;
 	if (diagnostic_printouts_are_enabled)
 		{
 		cout << "done";
 		cout << "\n. Algorithm completed\n" << flush;
		}
	//cout << "\nend algorithm slow orthogonal" << flush;
	return cost;
	}



	int
	orth_plan_undi_graph::
apply_t1_bend_stretching
	()
	{
	gdtnode v;
	gdtedge e;
	int nl,nr;
	border_step s;
	int counter=0;

	forall_edges (e, *this)
		{
		v = source (e);
		s = border_step_moving_along_edge (e,v);
		
		nl = number_of_left_bends (s);
		nr = number_of_right_bends (s);
		
		if (nl>0 && nr>0)
			{
			if      (nl > nr) set_edge_bends (e,v,nl-nr,left);
			else              set_edge_bends (e,v,nr-nl,right);
			counter++;
			}
		}
	if (diagnostic_printouts_are_enabled)
		if (counter) cout << "\n.. Transformation T1 applied " << counter << " times" << flush;
	return counter;
	}
	
	
		
	int
	orth_plan_undi_graph::
apply_t2_bend_stretching
	()
	{
	gdtnode v;
	gdtedge e;
	border_step s;
	int counter_left=0;
	int counter_right=0;
	int nl,nr, minl,minr;

	forall_nodes (v,*this)
		{
		e = first_adj_edge(v);
		s  = border_step_moving_along_edge (e,v);
		minl = number_of_left_bends (s);
		minr = number_of_right_bends (s);
		while (e && (minl>0 || minr>0))
			{
			s  = border_step_moving_along_edge (e,v);
			nl = number_of_left_bends (s);
			nr = number_of_right_bends (s);
			if (nl<minl) minl=nl;			
			if (nr<minr) minr=nr;
			e = adj_succ(e,v);			
			}

		if (minl>0)
			//Modified by A.M.
 			forall_edges_adjacent_node(e,v,*this)
				{
				s  = border_step_moving_along_edge (e,v);
				nl = number_of_left_bends (s);
				set_edge_bends (e,v,nl-minl,left);
				counter_left++;
				}
		else if (minr>0)
			//Modified by A.M.
 			forall_edges_adjacent_node(e,v,*this)
				{
				s  = border_step_moving_along_edge (e,v);
				nr = number_of_right_bends (s);
				set_edge_bends (e,v,nr-minr,right);
				counter_right++;
				}
		}
	
	if (diagnostic_printouts_are_enabled)
		{
		if (counter_left)  cout << "\n.. Transformation T2-left  applied " << counter_left  << " times" << flush;
		if (counter_right) cout << "\n.. Transformation T2-right applied " << counter_right << " times" << flush;
		}
	return counter_left+counter_right;
	}
	
	

	int
	orth_plan_undi_graph::
apply_t3_bend_stretching
	()
	{
	gdtnode v,v1,v2,v3;
	gdtedge e,e1,e2;
	
	int  r23,  l23;
	int  a12,a21,a23,a32;
	border_step s12,s21,s23,s32;
	
	int  m;
	int  r23_, l23_; 
	int  a12_, a32_;

	int counter_left=0;
	int counter_right=0;

	forall_nodes (v,*this)
		{
		//Modified by A.M.
 		forall_edges_adjacent_node(e,v,*this)
			{
			e1 = e;
			e2 = cyclic_adj_succ (e1,v);
			
			v1 = opposite(v,e1);
			v2 = v;
			v3 = opposite(v,e2); 

			s12 = border_step_moving_along_edge (e1,v1);
			s21 = border_step_moving_along_edge (e1,v2);
			s23 = border_step_moving_along_edge (e2,v2);
			s32 = border_step_moving_along_edge (e2,v3);
			
			a12 = (int)angle_of_border_step (s12);
			a21 = (int)angle_of_border_step (s21);
			a23 = (int)angle_of_border_step (s23);
			a32 = (int)angle_of_border_step (s32);
			
			//                   |
			//                   |
			//                   |  a32
			//         <--s21--  |    <--s32--
			//   (v1)-----e1----(v2)----e2----(v3)
			//       --s12-->       --s23-->
			//                  a12
			
			l23 = number_of_left_bends  (s23);
			r23 = number_of_right_bends (s23);
			
			if ((a12 > 1) && (r23 > 0))
				{
				m = gdt_minimum (a12 - 1, r23);

				//// CASE(A) a12=2, r23=1 -> m=1 
				//
				//  o---
				//  |
				//  |
				// (v)    ->  (v)---
				//  |          |
				//  |          |
				//  |          |
				
				//// CASE(B) a12=3, r23=1 -> m=1 
				//
				// |            |
				// |            |
				// |            |
				// o--(v)  ->  (v)
				//     |        |
				//     |        |
				//     |        |

				//// CASE(C) a12=3, r23=2 -> m=2 
				//
				// o----
				// |
				// |
				// o--(v)  ->  (v)---
				//     |        |
				//     |        |
				//     |        |
				
				//// CASE(D) a12=3, r23=3 -> m=2 
				//
				// o------o
				// |      |
				// |      | 
				// o--(v)    ->  (v)---o
				//     |          |    |
				//     |          |    |
				//     |          |

				a12_ = a12 - m;
				r23_ = r23 - m;
				a32_ = a32 + m;
				set_angle_of_border_step (s12,(angle_type)a12_);				
				set_edge_shape (e2,v2,r23_,right,(angle_type)a32_,(angle_type)a23);
				counter_right++;
				}
				
			else if ((a32 > 1) && (l23 > 0))
				{
				m = gdt_minimum (a32 - 1, l23);

				//// CASE(A) a32=2, l23=1 -> m=1 
				//
				// ---o
				//    |
				//    |
				//   (v)  ->  ---(v)
				//    |           |
				//    |           |
				//    |           |
				
				//// CASE(B) a32=3, l23=1 -> m=1 
				//
				//      |       |
				//      |       |
				//      |       |
				// (v)--o  ->  (v)
				//  |           |
				//  |           |
				//  |           |

				//// CASE(C) a32=3, l23=2 -> m=2 
				//
				//  ----o
				//      |
				//      |
				// (v)--o  ->  ---(v)
				//  |              |
				//  |              |
				//  |              |

				//// CASE(D) a32=3, l23=3 -> m=2 
				//
				//  o-------o
				//  |       |
				//  |       | 
				//    (v)---o  ->  o---(v)
				//     |           |    |
				//     |           |    |
				//     |                |

				a12_ = a12 + m;
				l23_ = l23 - m;
				a32_ = a32 - m;
				set_angle_of_border_step (s12,(angle_type)a12_);				
				set_edge_shape (e2,v2,l23_,left,(angle_type)a32_,(angle_type)a23);
				counter_left++;
				}
			}
		}
	if (diagnostic_printouts_are_enabled)
		{
		if (counter_left)  cout << "\n.. Transformation T3-left  applied " << counter_left  << " times" << flush;
		if (counter_right) cout << "\n.. Transformation T3-right applied " << counter_right << " times" << flush;
		}
	return counter_left+counter_right;
	}



	void
	orth_plan_undi_graph::
change_ref_border_step
	()
	{
	// WARNING: the method works correctly only for linearized orthogonal graphs
	// this method must be rewritten to correctly work
	// with not linearized orthogonal graphs
	//
	// Any way, this method is useful to have no rotations of the
	// dime_undi_graph during all the dynamic operations
	
	bool L_direction = false;
	int dir_count = 0;
	border_step s,rs;
	angle_type  as;
	
	rs = get_ref_border_step();
	s  = rs;
	ref_border_step = NULL_BORDER_STEP;
	
	do 
		{
		as = angle_of_border_step(s);
		s  = cyclic_succ_border_step (s);
		switch (as)
			{
			case _180: break;
			case _090: dir_count ++  ; break;
			case _000: dir_count +=2 ; break;
			case _270: dir_count --  ; break;
			case _360: dir_count -=2 ; break;
			}
		L_direction = (abs(dir_count)==1);		 
		}
	while (s != rs && L_direction);
	
	if (s != rs)
		{
		int dir = abs(dir_count);
		switch (dir)
			{
			case 0: ref_border_step = s; break;
			case 2: ref_border_step = opposite_border_step(s); break;
			default: break;
			}
		}
	
	if (!ref_border_step)
		{
		// take a new border step randomly
		//
		gdtedge e;
		gdtedge e_ref = edge_of_border_step(rs);
		forall_edges (e,*this)
			if (e_ref != e) break;
				
		gdtnode v1 = source(e);
		border_step s1 = border_step_moving_along_edge (e,v1);
		ref_border_step = s1;
		}
	}	
	
	

	bool
	orth_plan_undi_graph::
is_orthogonal
	()
	{
	gdtnode v;
	gdtedge e;
	face f;
	int angle_sum;
	
	// check that the sum of the angles around each gdtnode is 360 degree
	//
	//cout << "\nCheck the sum of the angles" << endl;
	forall_nodes (v,*this)
		{
		angle_sum = 0;
		//cout << "Edges " << adj_edges(v).size() << " adiacent to gdtnode " << id(v) << endl;
		forall_edges_adjacent_node(e,v,*this) 
			{
			border_step s  = border_step_moving_along_edge (e,opposite(v,e));
			angle_type  as = angle_of_border_step (s);
			//cout << "\tAngle of gdtedge " << id(e) << " to gdtnode " << id(opposite(v,e)) << " is ";
			switch (as)
				{
				case _000: angle_sum +=   0; break;
				case _090: angle_sum +=  90; break;
				case _180: angle_sum += 180; break;
				case _270: angle_sum += 270; break;
				case _360: angle_sum += 360; break;
				}
			//cout << endl;
			}
		if (angle_sum != 360) return false;
			
		}
	
	
	// check that each face is closed
	//
	//cout << "Checking if faces are closed" << endl; 
	forall_faces (f,*this)
		if (!border_is_closed(f)) return false;
		
	
	// check that the reference border step is correct
	//
	//bool found_ref_border_step = false;
	//forall_faces (f,*this)
	//	{
	//	border_step s;
	//	forall_face_border_steps (s,f)
	//		if (ref_border_step == s) found_ref_border_step = true;
	//	}
	//if (!found_ref_border_step) return false;
	
	//cout << "\nOrthogonal Representation Correct" << flush;
	
	return true;
	}


	face
	orth_plan_undi_graph::
find_external_face
	()
	{
	face f;
	forall_faces (f,*this)
		{
		int lt,rt,dt;
		rt = number_of_right_turns_around_face (f);
		lt = number_of_left_turns_around_face  (f);
		dt = rt-lt;
		if (dt==-4) return f;
		}
	return NULL_FACE;
	}



	bool
	orth_plan_undi_graph::
is_main_edge_on_node_side
	(gdtedge e, gdtnode v)
	{
	// a linear but not optimized implementation
	// (it examines all gdtnode sides, instead 
	//  the only one involing gdtedge e)
	// 
	gdt::gdtlist<gdtedge> L1, L2, L3, L4;
	edges_on_each_side_of_node (v,L1,L2,L3,L4);
	for (int i=1; i<=4; i++)
		{
		gdt::gdtlist<gdtedge> L, L_left, L_right;
		gdtedge ec = NULL_EDGE;
		switch (i)
			{
			case 1: L = L1; break;
			case 2: L = L2; break;
			case 3: L = L3; break;
			case 4: L = L4; break;			
			}
		split_list_of_edges_on_side (L,v,L_left,ec,L_right);
		if (e == ec) return true;
		}
	return false;
	}

//Added by Marcandalli 16 10 2001
void orth_plan_undi_graph::undefine(gdtedge e) {
	border_step b1 = border_step_moving_along_edge(e,source(e));
	border_step b2 = border_step_moving_along_edge(e,target(e));
	border_step_info->undefine(b1);
	border_step_info->undefine(b2);
	constraint_on_bends_of_edge.undefine(e);
}
