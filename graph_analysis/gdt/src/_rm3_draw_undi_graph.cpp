/*******************************************************************************
+
+  _rm3_draw_undi_graph.cpp
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

#include <fstream>
#include <sstream>
#include <math.h>
#include <assert.h>
#include <float.h>
#include <GDT/rm3_undi_graph.h>
#include <GDT/rm3_draw_undi_graph.h>
#include <GDT/gdt_error.h>

using namespace std;
#define DEBUG

//-----------------------------------------------------------------------------
// draw_undi_graph: base class for all directed and undirected embedded graphs
//                  with basic draw-oriented information
//
// W.Didimo e A.Leonforte (1996)
//-----------------------------------------------------------------------------

# define min(a,b) ((a)>(b) ? (b) : (a))
# define max(a,b) ((a)>(b) ? (a) : (b))


	struct_node_defaults::
struct_node_defaults
	()
	{
	altitude         = 0.5;
	width            = 3;
	height           = 3;
	body_color       = color(yellow);
	label_color      = black;
	border_color     = black;
	shape            = RECTANGLE;
	}


	struct_edge_defaults::
struct_edge_defaults
	()
	{
	anchor		 = gdt::gdtpoint(0,0);
	body_color       = black;
	label_color      = black;
	shape            = NORMAL;
	}


	struct_draw_info::
struct_draw_info
	()
	{
	scale                   = 10;
	bend_radius             = 0.5;
	edge_sensivity          = 1;
	shadow_color            = grey1; 
	shadows_are_visible     = false;
	node_labels_are_visible = true;
	edge_labels_are_visible = true;
	}
	

// ------------------------------------------------------------------------------
// 				    draw_undi_graph
// ------------------------------------------------------------------------------



// -----------------------
// PUBLIC
// -----------------------


	draw_undi_graph::
draw_undi_graph
	()
	{
	//cout << "\n draw_undi_graph () creator \n" << flush;
	local_new ();
	}



	draw_undi_graph::
~draw_undi_graph
	()
	{
	//cout << "\n draw_undi_graph () destructor \n" << flush;
	local_del ();
	}


	
	draw_undi_graph::
draw_undi_graph
	(
	const undi_graph& ug, 
	bool remove_crosses
	)
	{
	//cout << "\n draw_undi_graph (undi_graph) creator \n" << flush;
	undi_graph::operator=(ug);
	local_new ();
	local_init(ug,remove_crosses);
	}



	draw_undi_graph::
draw_undi_graph
	(
	const plan_undi_graph& pug, 
	algorithm_type alg,
	face ef, 
	bool remove_crosses, 
	gdt::gdtedge_array<int>* cp,
	int min_switches
	)
	{
	//cout << "\n draw_undi_graph (plan_undi_graph) creator \n" << flush;
	undi_graph::operator=(pug);
	local_new ();
	local_init(pug,alg,ef,remove_crosses,cp,min_switches);
	}




	draw_undi_graph::
draw_undi_graph
	(
	const orth_plan_undi_graph& opug,
	algorithm_type alg, 
	bool remove_crosses
	)
	{
	heading_type dir = undefined_heading;
	construct_draw_undi_graph (opug, dir, alg, remove_crosses);
	}
	
	

	draw_undi_graph::
draw_undi_graph
	(
	const orth_plan_undi_graph& opug,
	heading_type dir, 
	algorithm_type alg, 
	bool remove_crosses
	)
	{
	//cout << "\n draw_undi_graph (orth_plan_undi_graph) creator \n" << flush;
	construct_draw_undi_graph (opug, dir, alg, remove_crosses);
	}
	

	void
	draw_undi_graph::
construct_draw_undi_graph
	(
	const orth_plan_undi_graph& opug,
	heading_type dir,
	algorithm_type alg, 
	bool remove_crosses
	)
	{
	if (alg == DEFAULT_ALGORITHM) alg = SLOW_REGULAR_COMPACTION_2_REFINED;
	dime_orth_plan_undi_graph dopug (opug,alg,dir);
	dopug.remove_all_dummy_nodes_and_edges();

	// Linearize dopug
	// ---------------
	gdtnode v;
	gdt::gdtlist<gdtnode> dummy_N;
	gdt::gdtlist<gdtedge> dummy_E;
	dopug.replace_bends_with_nodes(dummy_N,dummy_E);
	forall (v,dummy_N) dopug.mark (v,RM3_REPLACES_A_BEND);
	// --------------
	
	// ---------------------------------------
	// check if there is at least a constraint 
	// on the dimensions of the nodes and in
	// case construct the list of all these  
	// kind of constraints
	// ---------------------------------------
	
	bool constr_node_dimensions = false;
	 
	gdt::gdtlist<constraint> dim_of_nodes;
	constraint c;
	constraint_type ct;
	
	forall_constraints (c,opug)
		{
		ct = opug.type_of_constraint (c);
		if (ct == NODE_WIDTH || ct == NODE_HEIGHT) dim_of_nodes.append(c);
		}
	if (!dim_of_nodes.empty()) constr_node_dimensions = true;

	// ---------------------------------------
	// CASE OF NO NODE-DIMENSIONS CONSTRAINTS:
	// ---------------------------------------
	// all nodes will have zero size
	// ---------------------------------------
	
	if (!constr_node_dimensions)
		{
		undi_graph::operator=(dopug);
		local_new ();
		bool refine_compaction = false;
		if (alg == FAST_COMPACTION_REFINED 	     ||
		    alg == SLOW_COMPACTION_REFINED 	     ||
		    alg == FAST_REGULAR_COMPACTION_1_REFINED ||
		    alg == SLOW_REGULAR_COMPACTION_2_REFINED)	refine_compaction = true;
		
		// visual debugging
		// ----------------
		//   draw_undi_graph dug1 (0,dopug);
		//   int x_min = 0;
		//   int x_max = 200;
		//   int y_min = 0;
		//   window W1 (1000,1000,"expanded dime");
		//   W1.init(x_min,x_max,y_min);
		//   W1.open();
		//   dug1.draw(W1);
		//   int i;
		//   cin >> i;
		// ----------------		
		
		local_init (dopug, refine_compaction, true, remove_crosses);
		}
	
	// ------------------------------------
	// CASE OF NODE_DIMENSIONS CONSTRAINTS:
	// ------------------------------------
	// each gdtnode will have a size according
	// to its dimensions constraints.
	// ------------------------------------
		
	else
		{
		// graph has been linearized, but it has not dummy nodes and edges
		// due to rectangularization or regularization methods
		//
		gdt::gdtnode_array<int> w (dopug,0);
		gdt::gdtnode_array<int> h (dopug,0);
		
		forall (c,dim_of_nodes)
			{
			ct = opug.type_of_constraint (c);
			if (ct == NODE_WIDTH) 
				{
				constraint_node_width cw = (constraint_node_width)c;
				gdtnode v = cw -> get_constrained_node();
				gdtnode u = dopug.corresponding_node(v,opug);
				w[u] = (int)(cw -> get_value());
				}
			else	
				{
				constraint_node_height ch = (constraint_node_height)c;
				gdtnode v = ch -> get_constrained_node();
				gdtnode u = dopug.corresponding_node(v,opug);
				h[u] = (int)(ch -> get_value());
				}
			}

		dime_orth_plan_undi_graph dopug1;
		gdt::gdtnode_map<gdtnode> super_node(dopug1,NULL_NODE);

		bool one_edge_attachment_wrt_previous_corner_exists = false;
		forall_constraints(c,opug)
			if (opug.type_of_constraint(c)==EDGE_ATTACHMENT_WRT_PREVIOUS_CORNER)
				one_edge_attachment_wrt_previous_corner_exists = true;


		// -----------------------------------------------------------------------------------------
		// the function to call depends on the existence of edge_attachment_wrt_bottom_left_constraints
		// -----------------------------------------------------------------------------------------
		
		if( one_edge_attachment_wrt_previous_corner_exists )
			dopug.compute_dime_with_expanded_nodes_and_pins (w,h,dopug1,super_node);
		else
			dopug.compute_dime_with_expanded_nodes (w,h,dopug1,super_node);

		undi_graph::operator=(dopug1);
		local_new();
		//super_node.show(cout);
		local_init_for_dimensioned_nodes (dopug,dopug1,super_node,w,h,true);
		}	
	}



	draw_undi_graph::
draw_undi_graph 
		(
		const gdt::gdtnode_array<int>& w0,
		const gdt::gdtnode_array<int>& h0,
		const dime_orth_plan_undi_graph& dopug0,
		bool edges_centered
		)
		{
		//cout << "\n draw_undi_graph (dime_orth_plan_undi_graph) creator \n" << flush;
		dime_orth_plan_undi_graph dopug (dopug0);
		dopug.remove_all_dummy_nodes_and_edges();
		
		// Linearize
		//
		gdtnode v;
		gdt::gdtlist<gdtnode> dummy_N;
		gdt::gdtlist<gdtedge> dummy_E;
		dopug.replace_bends_with_nodes(dummy_N,dummy_E);	
		forall (v,dummy_N) dopug.mark (v,RM3_REPLACES_A_BEND);
		//
		
		// copy the gdtnode's dimensions
		//
		gdt::gdtnode_array<int> w (dopug,0);
		gdt::gdtnode_array<int> h (dopug,0);
		forall_nodes (v,dopug)
			{
			gdtnode v0 = dopug0.corresponding_node(v,dopug);
			if (v0) {w[v] = w0[v0]; h[v]=h0[v0];}
			else	  {w[v] = 0; h[v]=0;}
			}		

		// debug
		//
		//cout << "\nwidths and heights after:" << flush;
		//forall_nodes (v,dopug)
		//	{
		//	cout << "\nnode " << dopug.id(v) << flush;
		//	cout << "\nw = " << w[v] << flush;
		//	cout << "\nh = " << h[v] << flush;
		//	}
		//

		// Construct a dime with expanded nodes
		//	
		dime_orth_plan_undi_graph dopug1;
		gdt::gdtnode_map<gdtnode> super_node(dopug1,NULL_NODE);

		if (edges_centered) dopug.compute_dime_with_expanded_nodes_and_edges_centered (w,h,dopug1,super_node);
		else		    dopug.compute_dime_with_expanded_nodes (w,h,dopug1,super_node);		

		// Construct the draw
		//
		undi_graph::operator=(dopug1);
		local_new();
		local_init_for_dimensioned_nodes (dopug,dopug1,super_node,w,h,true);
		}						



	draw_undi_graph::
draw_undi_graph
	(
	const dime_orth_plan_undi_graph& dopug0, 
	bool refine_compaction, 
	bool remove_crosses
	)
	{
	//cout << "\n draw_undi_graph (dime_orth_plan_undi_graph) creator \n" << flush;
	dime_orth_plan_undi_graph dopug (dopug0);
	dopug.remove_all_dummy_nodes_and_edges();
	
	// Added by W.D. on April 7
	// Linearize dopug
	// 
	gdtnode v;
	gdt::gdtlist<gdtnode> dummy_N;
	gdt::gdtlist<gdtedge> dummy_E;
	dopug.replace_bends_with_nodes(dummy_N,dummy_E);	
	forall (v,dummy_N) dopug.mark (v,RM3_REPLACES_A_BEND);
	//

	undi_graph::operator=(dopug);
	local_new ();
	local_init(dopug,refine_compaction,true,remove_crosses);
	}


	draw_undi_graph::
draw_undi_graph
	(
	int details,
	const dime_orth_plan_undi_graph& dopug0
	)
	{
	//cout << "\n draw_undi_graph (dime_orth_plan_undi_graph) creator \n" << flush;
	dime_orth_plan_undi_graph dopug (dopug0);
	
	if (details == 1) dopug.remove_all_dummy_nodes_and_edges();
	
	undi_graph::operator=(dopug);
	local_new ();
	local_init (details,dopug);
	}



	draw_undi_graph::
draw_undi_graph
	(
	const tree& tr, 
	algorithm_type alg, 
	bool remove_crosses
	)
	{
	//cout << "\n draw_undi_graph (tree) creator \n" << flush;
	undi_graph::operator=(tr);
	local_new ();
	local_init(tr,alg,remove_crosses);
	}



	draw_undi_graph::
draw_undi_graph
	(
	const upwa_plan_undi_graph& upug, 
	algorithm_type alg,
	bool remove_crosses
	)
	{
	//cout << "\n draw_undi_graph (upwa_plan_undi_graph) creator \n" << flush;
	undi_graph::operator=(upug);
	local_new ();
	local_init(upug, alg, remove_crosses);
	}


	draw_undi_graph::
draw_undi_graph
	(
	rel_coord_orth& rco,
	bool replace_dummy_nodes
	)
	{
	rco.redistribute_coords(10.0);  // we want grid lines 10 units apart

	if( replace_dummy_nodes )
		{
		undi_graph ug = rco.get_underlying_undi_graph();
		local_new();
		
		gdtnode ug_n;
		forall_nodes(ug_n,ug)
			{
			int source_id = rco.aux_id(ug.id(ug_n));
			if ( source_id == -1 ) continue;       // skip the bends now
			                                       // since you don't know which id
							       // it is safe to give them
			
			gdtnode dug_n = find_node(source_id);  
			if( dug_n != NULL_NODE )
				{
				// a node corresponding to n was already inserted and is dug_n
				// verify that the size and position of dug_n is correct
				//
				gdt::gdtpoint c = center(dug_n);
				double cx = c.xcoord();
				double cy = c.ycoord();
				double w2  = width(dug_n)/2.0;
				double h2 = height(dug_n)/2.0;
				
				double X = cx + w2;
				double x = cx - w2;
				double Y = cy + h2;
				double y = cy - h2;
				
				double nx = rco.xcoord(ug.id(ug_n));
				double ny = rco.ycoord(ug.id(ug_n));

				if( nx > X ) X = nx + scale()/4;   // scale()/4 is half the default width
				if( nx < x ) x = nx - scale()/4;
				if( ny > Y ) Y = ny + scale()/4;   // scale()/4 is half the default height
				if( ny < y ) y = ny - scale()/4;
				
				set_center(dug_n,gdt::gdtpoint((X+x)/2.0,(Y+y)/2.0));
				set_width(dug_n,X-x);
				set_height(dug_n,Y-y);
				}
			else
				{
				// a node corresponding to n was not inserted: insert it
				//
				double x = rco.xcoord(ug.id(ug_n));
				double y = rco.ycoord(ug.id(ug_n));
				dug_n = new_node(gdt::gdtpoint(x,y),source_id);
				}
			}
		
		// insert bends
		//
		gdt::gdtmap< gdtnode, gdtnode > ug_bend_2_dug_node;	
		forall_nodes(ug_n,ug)
			{
			int source_id = rco.aux_id(ug.id(ug_n));
			if ( source_id != -1 ) continue;  // we want bends only
			
			double x = rco.xcoord(ug.id(ug_n));
			double y = rco.ycoord(ug.id(ug_n));
			gdtnode dug_n = new_node(gdt::gdtpoint(x,y));
			ug_bend_2_dug_node[ug_n] = dug_n;
			}
			
		// insert edges
		//
		gdtedge ug_e;
		forall_edges(ug_e,ug)
			{
			gdtnode ug_source = ug.source(ug_e);
			gdtnode ug_target = ug.target(ug_e);
			int ug_id_source = ug.id(ug_source);
			int ug_id_target = ug.id(ug_target);
			int dug_id_source = rco.aux_id(ug_id_source);  // may be -1 if bend
			int dug_id_target = rco.aux_id(ug_id_target);  // may be -1 if bend
			
			gdtnode dug_source;
			gdtnode dug_target;
			if( dug_id_source == -1 ) 	dug_source = ug_bend_2_dug_node[ug_source];
			else 				dug_source = find_node(dug_id_source);

			if( dug_id_target == -1 ) 	dug_target = ug_bend_2_dug_node[ug_target];
			else 				dug_target = find_node(dug_id_target);
			
			if( dug_source == dug_target ) continue;      // do not add edges that form the box 
			
			gdtedge dug_e = new_edge(dug_source,dug_target,gdt::gdtlist<gdt::gdtpoint>());
			
			set_anchor_point(dug_e,dug_source,rco.center(ug_id_source));
			set_anchor_point(dug_e,dug_target,rco.center(ug_id_target));
			}

		// now replace dummy vertices with bends
		//		
		forall_nodes(ug_n,ug) 
			{
			if( ug_bend_2_dug_node.defined(ug_n) ) 
				{
				remove_two_degree_node(ug_bend_2_dug_node[ug_n]);
				}
			}
		}
	else
		{
		undi_graph ug = rco.get_underlying_undi_graph();
		undi_graph::operator=(ug);
		local_new();
		
		gdtnode n;
		forall_nodes(n,*this) 
			{
			apply_defaults(n);
			double x = rco.xcoord(id(n));
			double y = rco.ycoord(id(n));
			set_center(n,gdt::gdtpoint(x,y));
			}

		// work on edges
		//
		gdtedge e;
		forall_edges(e,*this)
			{
			set_bends (e,source(e),gdt::gdtlist<gdt::gdtpoint>());
 			apply_defaults(e);
			}
		}
		
	}



	draw_undi_graph::
draw_undi_graph
	(
	const draw_undi_graph& dug,
	bool remove_crosses
	)
	{
	//cout << "\n draw_undi_graph (draw_undi_graph) creator \n" << flush;
	undi_graph::operator=(dug);
	local_new();
	local_init(dug, remove_crosses);
	}
	
	
	
	
	draw_undi_graph&
	draw_undi_graph::
operator=
	(const undi_graph& ug)
	{
	//cout << "\nundi_graph -(copy)-> draw_undi_graph \n" << flush;
	undi_graph::operator=(ug);
	local_renew();
	local_init (ug);
	return *this; 
	}
	

	
	draw_undi_graph&
	draw_undi_graph::
operator=
	(const plan_undi_graph& pug)
	{
	//cout << "\nplan_undi_graph -(copy)-> draw_undi_graph \n " << flush;
	undi_graph::operator=(pug);
	local_renew();
	local_init (pug);
	return *this;
	}



	draw_undi_graph&
	draw_undi_graph::
operator=
	(const upwa_plan_undi_graph& upug)
	{
	//cout << "\nupwa_plan_undi_graph -(copy)-> draw_undi_graph \n " << flush;
	undi_graph::operator=(upug);
	local_renew();
	local_init (upug);
	return *this; 
	}



	draw_undi_graph&
	draw_undi_graph::
operator=
	(const orth_plan_undi_graph& opug)
	{
	//cout << "\north_plan_undi_graph -(copy)-> draw_undi_graph \n " << flush;
	dime_orth_plan_undi_graph dopug (opug);
	dopug.remove_all_dummy_nodes_and_edges();

	// Added by W.D. on April 7
	// Linearize dopug
	// 
	gdtnode v;
	gdt::gdtlist<gdtnode> dummy_N;
	gdt::gdtlist<gdtedge> dummy_E;
	dopug.replace_bends_with_nodes(dummy_N,dummy_E);	
	forall (v,dummy_N) dopug.mark (v,RM3_REPLACES_A_BEND);
	//
	
	undi_graph::operator=(dopug);
	local_renew();
	local_init (dopug);
	return *this; 
	}


	draw_undi_graph&
	draw_undi_graph::
operator=
	(const dime_orth_plan_undi_graph& dopug0)
	{
	//cout << "\noperator dopug -(copy)-> draw_undi_graph \n" << flush;
	
	dime_orth_plan_undi_graph dopug(dopug0);
	dopug.remove_all_dummy_nodes_and_edges();

	// Added by W.D. on April 7
	// Linearize dopug
	// 
	gdtnode v;
	gdt::gdtlist<gdtnode> dummy_N;
	gdt::gdtlist<gdtedge> dummy_E;
	dopug.replace_bends_with_nodes(dummy_N,dummy_E);	
	forall (v,dummy_N) dopug.mark (v,RM3_REPLACES_A_BEND);
	//
	 
	undi_graph::operator=(dopug);
	local_renew();
	local_init (dopug);
	return *this;
	}


	draw_undi_graph&
	draw_undi_graph::
operator=
	(const tree& tr)
	{
	//cout << "\ntree -(copy)-> draw_undi_graph \n " << flush;
	undi_graph::operator=(tr);
	local_renew();
	local_init (tr);
	return *this;
	}
	
	
	draw_undi_graph&
	draw_undi_graph::
operator=
	(const draw_undi_graph& dug)
	{
	//cout << "\ntree -(copy)-> draw_undi_graph \n " << flush;
	undi_graph::operator=(dug);
	local_renew();
	local_init (dug);
	return *this; 
	}
	


	// -----------------
	// Access operations
	// -----------------


	// Dimensions


	double
	draw_undi_graph::
scale
	() const
	{
	return draw_curr->scale;
	}



	double
	draw_undi_graph::
width
	() const
	{
	double x_min, x_max, dummy;
	calc_bounding_box (x_min, x_max, dummy, dummy);
	return (x_max - x_min);
	}



	double
	draw_undi_graph::
width
	(gdtnode v) const
	{
	return (*node_info)[v].width;
	}


	double
	draw_undi_graph::
height
	() const
	{
	double y_min, y_max, dummy;
	calc_bounding_box (dummy, dummy, y_min, y_max);
	return (y_max - y_min);
	}


	double
	draw_undi_graph::
height
	(gdtnode v) const
	{
	return (*node_info)[v].height;
	}


	double
	draw_undi_graph::
area
	() const
	{
	double x_min, x_max, y_min, y_max;  
	calc_bounding_box (x_min,x_max,y_min,y_max);
	return ((x_max - x_min) * (y_max - y_min));
	}


	double
	draw_undi_graph::
area
	(gdtnode v) const
	{
	return width(v)*height(v);
	}



	double
	draw_undi_graph::
length
	(gdtedge e) const
	{
	gdtnode v1 = source(e); gdt::gdtpoint p1 = center (v1);
	gdtnode v2 = target(e); gdt::gdtpoint p2 = center (v2);
	gdt::gdtlist<gdt::gdtpoint> bp = bends (e,v1);
	gdt::gdtpoint p,q;
	double len = 0;
	
	p = p1;
	
	forall (q,bp)
		{
		len += p.distance(q);
		p = q;
		}
	len += p.distance(p2);

	return len;
	}
	
	
	// Positions

	gdt::gdtpoint
	draw_undi_graph::
center
	() const
	{
	double x_min, x_max, y_min, y_max;
	calc_bounding_box (x_min, x_max, y_min, y_max);
	return gdt::gdtpoint((x_min+x_max)/2,(y_min+y_max)/2);
	}


	gdt::gdtpoint
	draw_undi_graph::
center
	(gdtnode v) const
	{
	return (*node_info)[v].center;
	}


	gdt::gdtpoint
	draw_undi_graph::
anchor_source	   
	(gdtedge e) const
	{
	return (*edge_info)[e].anchor_source;
	}
		

	gdt::gdtpoint
	draw_undi_graph::   
anchor_target
	(gdtedge e) const
	{
	return (*edge_info)[e].anchor_target;
	}
	

	gdt::gdtpoint
	draw_undi_graph::	    
anchor_source_point
	(gdtedge e) const
	{
	gdtnode  v  = source(e);
	gdt::gdtpoint c  = center(v);
	gdt::gdtpoint p  = anchor_source(e);
	gdt::gdtpoint ap = gdt::gdtpoint (c.xcoord()+p.xcoord(), c.ycoord()+p.ycoord());
	return ap;
	}
		

	gdt::gdtpoint
	draw_undi_graph::	    
anchor_target_point
	(gdtedge e) const
	{
	gdtnode  v  = target(e);
	gdt::gdtpoint c  = center(v);
	gdt::gdtpoint p  = anchor_target(e);
	gdt::gdtpoint ap = gdt::gdtpoint (c.xcoord()+p.xcoord(), c.ycoord()+p.ycoord());
	return ap;
	}


	gdt::gdtpoint
	draw_undi_graph::
anchor
	(gdtedge e, gdtnode v) const
	{
	gdt::gdtpoint dp;
	if (v == source(e)) 	 dp = anchor_source(e);
	else if (v == target(e)) dp = anchor_target(e);
	else gdt_error("invalid pair gdtedge,gdtnode");
	return dp;
	}
	
	
	gdt::gdtpoint
	draw_undi_graph::
anchor_point
	(gdtedge e, gdtnode v) const
	{
	gdt::gdtpoint p;
	if (v == source(e)) 	 return p = anchor_source_point(e);
	else if (v == target(e)) return p = anchor_target_point(e);
	else gdt_error("invalid pair gdtedge,gdtnode");
	return p;
	}


	gdt::gdtpoint
	draw_undi_graph::
label_position
	(gdtnode v) const
	{
	return (*node_info)[v].label_pos;
	}
	
	
	gdt::gdtpoint
	draw_undi_graph::
label_position
	(gdtedge e) const
	{
	return (*edge_info)[e].label_pos;
	}


	gdt::gdtpoint
	draw_undi_graph::
intersection_edge_node
	(gdtedge e, gdtnode v0) const
	{
	//cout << "\ndraw_undi_graph::intersection_edge_node " << id(e) << " " << id(v0) << flush;
	
	gdtnode  v1 = opposite(v0,e);	
	
	gdt::gdtpoint p0; 			// center of gdtnode v0
	gdt::gdtpoint p1; 			// center of gdtnode v1
	gdt::gdtpoint ap0; 			// anchor-point of e with respect to v0
	gdt::gdtpoint ap1;			// anchor-point of e with respect to v1
	gdt::gdtpoint q1;			// start of segment edge_segment
	gdt::gdtpoint q2;			// stop  of segment edge_segment
	gdt::gdtpoint ip;       		// intersection point to be calculated
	gdt::gdtpoint p;
	
	double x0,y0;   		// coordinates of p0
	double x1,y1;   		// coordinates of p1
	double dx,dy;   		// width and height of gdtnode v0

	node_shape_type  ns;            // node_shape
	gdt::gdtsegment          edge_segment;  // segment along the intersecting gdtedge
	gdt::gdtlist<gdt::gdtpoint>      bend_list;     // bends along the intersecting gdtedge

	bool shape_is_polygonal;

	p0 = center(v0);
	x0 = p0.xcoord();
	y0 = p0.ycoord();
	
	p1 = center(v1);
	x1 = p1.xcoord();
	y1 = p1.ycoord();

	ap0 = anchor_point (e,v0);
	ap1 = anchor_point (e,v1);
	
	// -------------------------------------------------
	// Calculate the segment along the intersecting gdtedge
	// -------------------------------------------------
	//
	
	bend_list = bends(e); 
	if (bend_list.empty())  p = ap1;
	else if (v0==source(e)) p = bend_list.head();
	else                    p = bend_list.tail();

	if (node_contains_point (v0,ap0)) {q1 = ap0; q2 = p;}
	else 			 	  {
					  //cout << "\nANCHOR POINT OUT OF NODE" << flush;
					  //cout << "node:"<<  id(v0) << "\tpoint:" << ap0 << endl; 
					  q1 = p0 ; 
					  q2 = ap0;
					  }
	

	// ------------------------------
	// avoid error for gdtnode-collision
	// ------------------------------
	//
	
	if (node_contains_point (v0,q2))
		{
		cout << "\nCOLLISION DETECTED" << flush;
		ip = gdt::gdtpoint((q1.xcoord() + q2.xcoord())/2, (q1.ycoord() + q2.ycoord())/2);
		return ip;
		}
	else edge_segment = gdt::gdtsegment (q2,q1);

	//cout << "\n  gdtedge segment: (" << p1.xcoord() << "," << p1.ycoord() << ")-->(" << p2.xcoord() << "," << p2.ycoord() << ")" << flush;

	ns = shape  (v0);
	dx = width  (v0);
	dy = height (v0);
	
	double  dx2 = dx/2,
		dy2 = dy/2;
	 
	shape_is_polygonal = (ns==TRIANGLE) || (ns==RECTANGLE);

	if (shape_is_polygonal)
		{
		gdt::gdtlist<gdt::gdtpoint> vertex_list;
		switch (ns)
			{
			case TRIANGLE:
				// The following code is a work-around required
				// LEDA341 bug on segment-segment intersection

				/*
				//  p4-p3
				//   / \ 
				//  /   \
				// p1---p2
				*/
				//vertex_list.append(gdt::gdtpoint (x0-dx2,  y0-dy2));
				//vertex_list.append(gdt::gdtpoint (x0+dx2,  y0-dy2));
				//vertex_list.append(gdt::gdtpoint (x0+dx/16, y0+dy2));
				//vertex_list.append(gdt::gdtpoint (x0-dx/16, y0+dy2));

				// This is the code you would write
				// when LEDA had not the bug in the
				// segment-segment intersection
				//

				/*
				//   p3
				//   / \ 
				//  /   \ 
				// p1---p2
				*/
				vertex_list.append(gdt::gdtpoint (x0-dx2, y0-dy2));
				vertex_list.append(gdt::gdtpoint (x0+dx2, y0-dy2));
				vertex_list.append(gdt::gdtpoint (x0,     y0+dy2));
				break;

			case RECTANGLE:
				{
				vertex_list.append(gdt::gdtpoint (x0-dx2, y0-dy2)); // p4---p3
				vertex_list.append(gdt::gdtpoint (x0+dx2, y0-dy2)); // |     |
				vertex_list.append(gdt::gdtpoint (x0+dx2, y0+dy2)); // |     |
				vertex_list.append(gdt::gdtpoint (x0-dx2, y0+dy2)); // p1---p2

				
				double x_diff = (q1.xcoord() - q2.xcoord());
				if (x_diff < 0) 	x_diff = - x_diff;
				if (x_diff < 0.01) // vertical segment (it is needed to avoid a bug)
					{
					double ipx = q2.xcoord();
					double ipy;
					if (q2.ycoord() < p0.ycoord()) ipy = y0-dy2;
					else			       ipy = y0+dy2;
					ip = gdt::gdtpoint(ipx,ipy);
					//cout << "\nFunction intersection edge-node returns " << ip << " (vertical segment)" << endl  << flush;
					return ip;
					}
				}

				break;

			default:
				gdt_error("no code for gdtnode shape");
				break;
			}

		
	
		gdt::gdtpolygon border(vertex_list);
		//
		gdt::gdtlist<gdt::gdtpoint> lip = border.intersection(edge_segment);
		if (!lip.empty()) 
			{
			ip = lip.head();
			//cout << "intersection is not empty" << endl << flush;
			};
		//cout << "\nFunction intersection edge-node returns " << ip << endl  << flush;
		return ip;
		}

	else switch (ns)
		{
		case ELLIPSE:
			if (dx == dy)
				{
				// case circle
				//
				gdt::gdtcircle C(p0,dx/2);
				gdt::gdtlist<gdt::gdtpoint> L = C.intersection(edge_segment);
				if (!L.empty()) ip = L.head();
				return ip;
				}
			else
				{
				// case general ellipse
				//
				double xa,xb,ya,yb;		// intersection-solutions
				double qx1 = q1.xcoord();
				double qx2 = q2.xcoord();
				double qy1 = q1.ycoord();
				double qy2 = q2.ycoord();
				
				double Dqx = (qx2-qx1);
				double Dqy = (qy2-qy1);
				
				double rx = dx/2;   
				double ry = dy/2;
				
				double m;
				double q;
				double A,B,C;

				if (Dqx != 0)	// no vertical line 
					{
					m  = Dqy/Dqx;
					q  = qy2 - m*qx2;
					A  = 1/(rx*rx) + (m/ry)*(m/ry);
					B  = -2*x0/(rx*rx) + (2*m*(q-y0)/(ry*ry));
					C  = (x0*x0)/(rx*rx) + (q-y0)*(q-y0)/(ry*ry) - 1;
					
					// solution 1
					//
					xa = (-B + sqrt(B*B - 4*A*C))/(2*A);		
					ya = (m*xa+q);

					// solution 2
					//
					xb = (-B - sqrt(B*B - 4*A*C))/(2*A);
					yb = (m*xb+q);
					}
				else		// vertical line
					{
					A = 1/(ry*ry);
					B = (-2*y0)/(ry*ry);
					C = (y0*y0)/(ry*ry) + (qx1-x0)/(rx*rx) - 1;
					
					// solution 1
					//
					ya = (-B + sqrt(B*B - 4*A*C))/(2*A);		
					xa = qx1;

					// solution 2
					//
					yb = (-B - sqrt(B*B - 4*A*C))/(2*A);
					xb = qx1;					
					}
				

				if ((qx2 >= x0) && (qy2 <= y0))
					{
					// -------------------------
					// p into I° sector of plane
					// -------------------------
					if ((xa >= x0) && (ya <= y0)) ip = gdt::gdtpoint(xa,ya);
					else			      ip = gdt::gdtpoint(xb,yb);
					}
				else if ((qx2 >= x0) && (qy2 >= y0))
					{
					// --------------------------
					// p into II° sector of plane
					// --------------------------
					if ((xa >= x0) && (ya >= y0)) ip = gdt::gdtpoint(xa,ya);
					else			      ip = gdt::gdtpoint(xb,yb);
					}
				else if ((qx2 <= x0) && (qy2 >= y0))
			     		{
					// ---------------------------
					// p into III° sector of plane
					// ---------------------------
					if ((xa <= x0) && (ya >= y0)) ip = gdt::gdtpoint(xa,ya);
					else			      ip = gdt::gdtpoint(xb,yb);
					}
				else if ((qx2 <= x0) && (qy2 <= y0))
				   	{
					// --------------------------
					// p into IV° sector of plane
					// --------------------------
					if ((xa <= x0) && (ya <= y0)) ip = gdt::gdtpoint(xa,ya);
					else			      ip = gdt::gdtpoint(xb,yb);
				   	}

				return ip;
				}
			break;
		default:
			gdt_error("no code for gdtnode shape");
			break;
		}	 
	return ip;
	}
		
	
	
	gdt::gdtlist<gdt::gdtpoint>
	draw_undi_graph::
intersection_edge_edge
	(gdtedge e1, gdtedge e2) const
	{
	//cout << "\nintersection gdtedge-edge started" << flush;
	
	gdt::gdtlist<gdt::gdtpoint>   crosses;	// crosses between e1 and e2
	gdt::gdtlist<gdt::gdtpoint>   bl;	// bends list
	gdt::gdtpoint         ps,pt;
	gdt::gdtpoint	      cu1,cu2,
		      cv1,cv2;
	gdt::list_item     ps_it,pt_it;
	gdt::gdtsegment       s1,s2;
	gdt::gdtlist<gdt::gdtsegment> L1,	// segments of the gdtedge e1
		      L2;	// segments of the gdtedge e2
	
	gdtnode u1,v1,		// extremal nodes of the egde e1
	     u2,v2;		// extremal nodes of the gdtedge e2
	
	
	// ---------- 1 ------------
	// detect all segments of e1
	// and store them in L1 list
	// -------------------------
	
	u1 = source(e1);
	v1 = target(e1);
	cu1= center(u1);
	cv1= center(v1);
	
	bl = bends (e1,u1);
	bl.push  (cu1); //add u1-center point into the bend list
	bl.append(cv1); //add v1-center point into the bend list
	
	ps_it = bl.first();
	while (ps_it)
		{
		pt_it = bl.succ(ps_it);
		ps    = bl.inf (ps_it);
		if (pt_it)
			{
			pt = bl.inf (pt_it);
			gdt::gdtsegment s(ps,pt);
			L1.append (s);
			}
		ps_it = pt_it;
		}
		
	// ---------- 2 ------------
	// detect all segments of e2
	// and store them in L2 list
	// -------------------------

	u2 = source(e2);
	v2 = target(e2);
	cu2= center(u2);
	cv2= center(v2);
	
	bl = bends (e2,u2);
	bl.push  (cu2); //add u2-center point into the bend list
	bl.append(cv2); //add v2-center point into the bend list

	
	ps_it = bl.first();
	while (ps_it)
		{
		pt_it = bl.succ(ps_it);
		ps    = bl.inf (ps_it);
		if (pt_it)
			{
			pt = bl.inf (pt_it);
			gdt::gdtsegment s(ps,pt);
			L2.append (s);
			}
		ps_it = pt_it;
		}
	
	// ------------- 3 --------------
	// detect all the crosses between 
	// the L1 and L2 segments list
	// and return them
	// ------------------------------

	forall (s1,L1)
		forall (s2,L2)
			{
			gdt::gdtpoint p;
			if (s1.intersection(s2,p))
				if (!(p==cu1 || p==cv1 || p==cu2 || p==cv2))
					{
					crosses.append(p);
					// CHECK OF CONSISTENCY
					//
					//if (!s1.contains(p)) cout << "\ns1 CONSISTENCY ERROR!" << flush;
					//if (!s2.contains(p)) cout << "\ns2 CONSISTENCY ERROR!" << flush;
					//cout << "\ndetected cross in point (" << p.xcoord() << "," << p.ycoord() << ")" << flush;
					}
			}
	return crosses;
	}
	
	
		
	gdt::gdtlist<gdt::gdtpoint>
	draw_undi_graph::
bends
	(gdtedge e) const
	{
	return bends(e,source(e));
	}



	gdt::gdtlist<gdt::gdtpoint>
	draw_undi_graph::
bends
	(gdtedge e, gdtnode v) const
	{
	gdt::gdtlist<gdt::gdtpoint> L = (*edge_info)[e].bends;
	if (v == target(e)) 
		L.reverse();
		//for (int i=1; i<L.size(); i++) L.move_to_front(L.get_item(i));  
	return L;
	}

	

	gdt::gdtlist<gdt::gdtpoint>
	draw_undi_graph::
crosses
	() const
	{
	double round = 0.0001;
	gdtedge e1,e2;
	gdt::gdtlist<gdt::gdtpoint> C; // crosses
	gdt::gdtlist<gdtedge> L1 = all_edges();
	gdt::gdtlist<gdtedge> L2 = all_edges();
	gdt::gdtedge_array<bool> edge_already_tried (*this,false);
	
	forall (e1,L1)
		{
		edge_already_tried[e1] = true;
		forall (e2,L2)
			if (!edge_already_tried[e2])
				{
				gdt::gdtlist<gdt::gdtpoint> cross_set = intersection_edge_edge (e1,e2);
				C.conc(cross_set);
				}
		}
	
	// ---------------------------------------
	// delete crosses occurred more than once.
	// This case is possible if an gdtedge 
	// intersects another one on a bends
	// ---------------------------------------
	
	gdt::gdtlist<gdt::gdtpoint> double_cross;
	gdt::gdtpoint     p1,p2;
	gdt::list_item p1_pos,
		  p2_pos;

	p1_pos = C.first ();
	while (p1_pos)
		{
		p1 = C.inf(p1_pos);
		p2_pos = C.succ(p1_pos);
		while (p2_pos)
			{
			p2 = C.inf(p2_pos);
			//cout << "\ncomparing p1=(" << p1.xcoord() << "," << p1.ycoord() << ")" << flush;
			//cout << " with p2=(" << p2.xcoord() << "," << p2.ycoord() << ")" << flush;
			
			if (p1.distance(p2) < round)
				{
				gdt::list_item p2_pos_temp = p2_pos;
				p2_pos = C.succ(p2_pos);
				C.del_item (p2_pos_temp);
				//cout << "\nremoved point (" << p2.xcoord() << "," << p2.ycoord() << ")" << flush;
				}
			else p2_pos = C.succ(p2_pos);
			}
		p1_pos = C.succ(p1_pos);
		}	
		
	return C;	
	}



	double
	draw_undi_graph::
altitude
	(gdtnode v) const
	{
	return (*node_info)[v].altitude;
	}


	bool
	draw_undi_graph::
node_contains_point
	(gdtnode v, gdt::gdtpoint p) const
	{
	 node_shape_type st = shape (v);

	 double dx = width  (v);
	 double dy = height (v);
	 gdt::gdtpoint  p0 = center (v);
	 double x0 = p0.xcoord(); 
	 double y0 = p0.ycoord();
	 double x  = p.xcoord();  
	 double y  = p.ycoord();

	 switch (st)
		 {
		 case RECTANGLE:
			 {
			 //cout << "\nRECTANGLE";
			 double x1 = x0 - dx/2; double y1 = y0 - dy/2;
			 double x2 = x0 + dx/2; double y2 = y0 + dy/2;
			 if ((x1 <= x) && (x <= x2) && (y1 <= y) && (y <= y2)) {return true;}
			 break;
			 }
		 case TRIANGLE:
			 {
			 //cout << "\nTRIANGLE";
			 double x1 = x0 - dx/2; double y1 = y0 - dy/2;
			 double x2 = x0 + dx/2; double y2 = y0 + dy/2;
			 if ((y >= y1) && (y <= y1+(x-x1)*(y2-y1)/(x0-x1)) && (y <= y2+(x-x0)*(y1-y2)/(x2-x0))) return true;
			 break;
			 }
		 case ELLIPSE:
			 {
			 //cout << "\nELLIPSE";
			 double rx = dx/2;
			 double ry = dy/2;
			 //if (pow((x-x0)/rx,2) + pow((y-y0)/ry,2) <= 1) return true;
			 double X = ((x-x0)/rx) * ((x-x0)/rx);
			 double Y = ((y-y0)/ry) * ((y-y0)/ry);
			 if ( X + Y <= 1) return true;
			 break;
			 }
		 }
	return false;	
	}



	bool
	draw_undi_graph::
edge_contains_point
	(gdtedge e, gdt::gdtpoint p) const
	{
	gdt::gdtpoint ps,pt;
	double round = 0.01;
	gdt::gdtpoint cu = center(source(e));
	gdt::gdtpoint cv = center(target(e));
	
	gdt::gdtlist<gdt::gdtpoint> lb = bends(e,source(e));
	ps = cu;
	
	forall (pt,lb)
		{
		gdt::gdtsegment s(ps,pt);
		if (s.distance(p) < round) return true;
		else ps = pt;
		}
	pt = cv;
	gdt::gdtsegment s(ps,pt);
	if (s.distance(p) < round) return true;
	else 			   return false;
	}



	// Recommendations


	double
	draw_undi_graph::
recommended_scale () const
	{
	gdtnode v;
	double max_width  = 0;
	double max_height = 0;
	// compute the scale for length unit
	forall_nodes (v,*this)
		{
		max_width  = max(max_width, width(v));
		max_height = max(max_height, height(v));
		}
	double M = max(max_width, max_height);
	double k = M*2;
	return k;
	}

	
	gdt::gdtpoint
	draw_undi_graph::
recommended_label_position
	(gdtnode v) const
	{
	return center(v);
	}


	gdt::gdtpoint
	draw_undi_graph::
recommended_label_position
	(gdtedge e) const
	{
	gdtnode  v1,v2;
	gdt::gdtpoint p1,p2;
	double slope;

	if (edge_is_directed(e)) v1 = start(e);
 	else			 v1 = source(e);
	v2 = opposite(v1,e);
	
	gdt::gdtlist<gdt::gdtpoint> bl = bends (e,v1);
	
	if (bl.size() > 1)	
		{
		int bend_start = bl.size()/2 - 1; 
		p1 = bl.inf(bl.get_item(bend_start));
		p2 = bl.inf(bl.get_item(bend_start+1));
		}
	else 
		{
		p1 = anchor_point(e,v1);
		if (bl.size() == 1) p2 = bl.head();
		else		    p2 = anchor_point(e,v2);
		}
			
	double x1 = p1.xcoord(); double y1 = p1.ycoord();
	double x2 = p2.xcoord(); double y2 = p2.ycoord();
	double x  = (x1+x2)/2  ; double y  = (y1+y2)/2  ;
	gdt::gdtline l(p1,p2);
	slope = l.direction(); if (slope < 0)  slope = - slope;
	if (slope < 0)  slope = - slope;
	//if ((slope > PI/4) && (slope < (PI/4)*3)) return gdt::gdtpoint(x+1.5,y);
	//else		  		      	  return gdt::gdtpoint(x,y+1.5);
	if ((slope > PI/4) && (slope < (PI/4)*3)) return gdt::gdtpoint(x+0.15*scale(),y);
	else		  		      	  return gdt::gdtpoint(x,y+0.15*scale());
	}


	// Colors


	color
	draw_undi_graph::
body_color
	(gdtnode v) const
	{
	return (*node_info)[v].internal_col;
	}


	color
	draw_undi_graph::
body_color
	(gdtedge e) const
	{
	return (*edge_info)[e].col;
	}
	

	color
	draw_undi_graph::
label_color
	(gdtnode v) const
	{
	return (*node_info)[v].label_col;
	}

	
	color
	draw_undi_graph::
label_color
	(gdtedge e) const
	{
	return (*edge_info)[e].label_col;
	}




	color
	draw_undi_graph::
border_color
	(gdtnode v) const
	{
	return (*node_info)[v].border_col;
	}


	// Shapes


	node_shape_type
	draw_undi_graph::
shape
	(gdtnode v) const
	{
	return (*node_info)[v].shape;
	}


	edge_shape_type
	draw_undi_graph::
shape
	(gdtedge e) const
	{
	return (*edge_info)[e].shape;
	}


	// Labels


	std::string
	draw_undi_graph::
label
	(gdtnode v) const
	{
	return (*node_info)[v].label;
	}

	
	std::string
	draw_undi_graph::
label
	(gdtedge e) const
	{
	return (*edge_info)[e].label;
	}

	
	// Predicates


	bool
	draw_undi_graph::
shadows_are_visible
	() const
	{
	return draw_curr->shadows_are_visible;
	}


	bool
	draw_undi_graph::
node_labels_are_visible
	() const
	{
	return draw_curr->node_labels_are_visible;
	}
	

	bool
	draw_undi_graph::
edge_labels_are_visible
	() const
	{
	return draw_curr->edge_labels_are_visible;
	}
	

	// Click detection


	gdtnode
	draw_undi_graph::
find_node
	(int id) const
	{
	return undi_graph::find_node(id);
	}
		

	gdtedge
	draw_undi_graph::
find_edge
	(int id) const
	{
	return undi_graph::find_edge(id);
	}


	gdtnode
	draw_undi_graph::
find_node
	(gdt::gdtpoint p) const
	{
	gdtnode v;
	forall_nodes (v,*this) if (node_contains_point (v,p)) return v;
	return NULL_NODE;
	}
	

	gdtedge
	draw_undi_graph::
find_edge
	(gdt::gdtpoint p) const
	{
	gdtedge   e;	// current gdtedge
	gdt::gdtlist<gdt::gdtpoint> pl;	// bends of the current gdtedge
	gdt::gdtpoint p1,p2;	// end-points of the current segment
	double d;	// distance of p from the current segment.

	double d_min = DBL_MAX; // minimal point-segment distance from p
	gdtedge   e_min = nil;       // gdtedge containg the minimal-distance segment from p

	double x,y;   // coords of p, the point clicked by the user
	double x1,y1; // coords of p1, the first  end-point of the current segment
	double x2,y2; // coords of p2, the second end-point of the current segment
	double xk,yk; // coords of pk, the intersection between the LA and LB lines

	double ma,qa; // parameters of the LA line equation y=ma*x+qa
	double mb,qb; // parameters of the LB line equation y=mb*x+qb
	gdt::gdtpoint  pk;		// intersection between lines LA and LB
	gdtnode s,t;
	gdt::list_item pos;

	forall_edges(e,*this)
		{
		s = source(e);
		t = target(e);

		pl = bends (e,s);

		//pl.push   (center(s));
		//pl.append (center(t));		
		pl.push( anchor_point(e,s) );
		pl.append( anchor_point(e,t) );
		//pl.set_iterator (pl.first());
		//pl.current_element (p1);
		//while(pl.next_element (p2))
		
		pos = pl.first();
		p1 = pl.inf(pos);
		pos = pl.succ(pos);
		while(pos)
			{
			// p1,p2 are the end-point of the current segment of gdtedge e
			// p is the point clicked by the user

			p2 = pl.inf(pos);
			x  = p.xcoord();
			y  = p.ycoord();
			x1 = p1.xcoord();
			y1 = p1.ycoord();
			x2 = p2.xcoord();
			y2 = p2.ycoord();

			// Horizontal gdtedge
			//
			if (y1==y2)
				if (x1 < x2)
					if      (x < x1) d =  p.distance (p1);
					else if (x > x2) d =  p.distance (p2);
					else             d =  fabs(y-y1);
				else
					if      (x < x2) d =  p.distance (p2);
					else if (x > x1) d =  p.distance (p1);
					else             d =  fabs(y-y1);


			// Vertical gdtedge
			//
			else if (x1==x2)
				if (y1 < y2) 
					if      (y < y1) d =  p.distance (p1);
					else if (y > y2) d =  p.distance (p2);
					else             d =  fabs(x-x1);
				else
					if      (y < y2) d =  p.distance (p2);
					else if (y > y1) d =  p.distance (p1);
					else             d =  fabs(x-x1);

			// Generic gdtedge
			//
			else
				{

				/*
				//              p
				//  p1         / 
				//   \        /
				//    \      /LB 
				//     \ 90 /
				//      \  /
				//       pk
				//        \ 90
				//         \
				//          \
				//         LA\
				//            \
				//            p2
				*/

				// We want to determine the line LA from p1 to p2,
				// and the line LB passing through p, orthogonal with respect to LA.

				// For each line we calculate m and q,
				// in order to express each line
				// with an equation in the form y=mx+q.
				//
				ma = (y2 - y1)/(x2 - x1);	// ---------------------
				qa = y1 - ma * x1;		// LA equation y=ma*x+qa
				mb = -1/ma;			// LB equation y=mb*x+qb
				qb = y - mb * x;		// ---------------------

				// Now we calculate the intersection point pk
				// between LA and its orthogonal LB.
				//
				xk = (qb - qa) / (ma - mb);
				yk = ((qb * ma) - (qa * mb)) / (ma - mb);
				pk = gdt::gdtpoint(xk,yk);

				// Now we calculate wether pk is within the p1-p2 segment.
				// Since pk belongs to line LA, we just compare the coordinates
				// along either the x or the y axes.
				//
				if (x1<x2)
					if      (xk < x1) d =  p.distance(p1);
					else if (xk > x2) d =  p.distance(p2);
					else              d =  p.distance(pk);
				else
					if      (xk < x2) d =  p.distance(p2);
					else if (xk > x1) d =  p.distance(p1);
					else              d =  p.distance(pk);
				}

			// Now d contains the distance from p
			// of the current segment. Check it,
			// and update d_min when needed.
			//
			if (d < d_min)
				{
				e_min = e;
				d_min = d;
				}

			// Examine next segment of gdtedge e
			// 
			p1=p2;
			pos = pl.succ(pos);
			}
		}
		
	// All edges have been examined
	// e_min contains now the candidate gdtedge,
	// and d_min is its minimal distance from p.

	if (d_min < 0.25 * draw_curr->edge_sensivity) return e_min;
	else return NULL_EDGE;
	}

		
	gdt::list_item
	draw_undi_graph::
find_bend
	(gdt::gdtpoint p, gdtedge& e) const
	{
	gdtnode s,t; 
	gdt::gdtpoint px;
	gdt::gdtlist<gdt::gdtpoint> pl;
	gdt::list_item pos_px;
	forall_edges(e,*this)
		{
		s = source(e);
		t = target(e);
		pl = bends (e,s);
		/*----------------------------------------------------------------------------
		pl.set_iterator(pl.first());
		while(pl.current_element(px))
			if (px.distance(p) < draw_curr->bend_radius) return pl.get_iterator();
			else pl.move_iterator(forward);
		-----------------------------------------------------------------------------*/
		pos_px = pl.first();
		while(pos_px)
			{
			px = pl.inf(pos_px);
			if (px.distance(p) < draw_curr->bend_radius) return pos_px; 
			else pos_px = pl.succ(pos_px);
			}
		}

	return nil;
	}


	// Statistics


	int
	draw_undi_graph::
number_of_bends
	() const
	{
	gdtedge e;
	int  bn = 0;
	forall_edges (e,*this) bn += number_of_bends(e);
	return bn;	
	}
	

	int
	draw_undi_graph::
number_of_bends 
	(gdtedge e) const
	{
	return (*edge_info)[e].bends.size ();
	}
	

	int
	draw_undi_graph::
number_of_bends
	(gdt::gdtlist<gdtedge> edge_list) const
	{
	gdtedge e;
	int  bn = 0;
	forall (e,edge_list) bn += number_of_bends(e);
	return bn;	
	}
	

	int
	draw_undi_graph::
number_of_bends_max
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
	draw_undi_graph::
number_of_bends_avg
	() const
	{
	return (double)number_of_bends() / (double)number_of_edges();
	}
	

	double
	draw_undi_graph::
number_of_bends_sdv
	() const
	{
	double sd; // Standard Deviation
	double sum = 0;
	double avg = number_of_bends_avg();
	gdtedge e;
	
	forall_edges (e,*this) 
		{
		double sde = ((double)number_of_bends(e) - avg);
		sde = sde*sde;
		sum += sde;
		}
	sd = sqrt(sum / (double)number_of_edges());
	return sd;	
	}


	double
	draw_undi_graph::
edge_length_sum
	() const
	{
	double len = 0;
	gdtedge e;
	forall_edges (e,*this) len += length (e);
	return len;
	}


	double
	draw_undi_graph::
edge_length_max
	() const
	{
	double mawidth = 0;
	gdtedge e;
	forall_edges (e,*this) mawidth = max(mawidth,length(e));
	return mawidth;
	}
	
	
	double
	draw_undi_graph::
edge_length_avg
	() const
	{
	return edge_length_sum()/number_of_edges();
	}
	

	double
	draw_undi_graph::
edge_length_sdv
	() const
	{
	double lsd;
	double sum = 0;
	double mean = edge_length_avg ();
	gdtedge e;
	
	forall_edges (e,*this)
		{
		double el = length(e);
		sum += (el-mean)*(el-mean);
		}
	lsd = sqrt(sum / number_of_edges());
	return lsd;
	}
	
	
	double
	draw_undi_graph::
screen_ratio_sdv
	() const
	{
	double ssd1, ssd2;
	double x_min,x_max,
	       y_min,y_max;
	double dx, dy, ratio, inv_ratio;
	
	calc_bounding_box (x_min,x_max,y_min,y_max);

	dx     = (x_max - x_min);
	dy     = (y_max - y_min);
	ratio     = dx / dy;
	inv_ratio = dy / dx;
	
	if ((ratio - 4/3) < 0) ssd1 = - (ratio - 4/3);
	else                   ssd1 = + (ratio - 4/3);

	if ((inv_ratio - 3/4) < 0) ssd2 = - (inv_ratio - 3/4);	
	else                       ssd2 = + (inv_ratio - 3/4);
	return min(ssd1,ssd2);
	}


	// Defaults

	struct_draw_info
	draw_undi_graph::
draw_defaults
	() const
	{
	return *draw_defs;
	}

	struct_node_defaults
	draw_undi_graph::
node_defaults
	() const
	{
	return *node_defs;
	}


	struct_edge_defaults
	draw_undi_graph::
edge_defaults
	() const
	{
	return *edge_defs;
	}


	// Miscellaneous


	struct_draw_info
	draw_undi_graph::
draw_status
	() const
	{
	return *draw_curr;
	}


	struct_draw_node_info
	draw_undi_graph::
node_status
	(gdtnode v) const
	{
	return (*node_info)[v];
	}


	struct_draw_edge_info
	draw_undi_graph::
edge_status
	(gdtedge e) const
	{
	return (*edge_info)[e];
	}


	void
	draw_undi_graph::
calc_bounding_box
	(double& x_min, double& x_max, double& y_min, double& y_max) const
	{
	gdtnode v;
	gdtedge e;
	gdt::gdtpoint p;
	
	v = first_node();
	
	if (v) 
		{
		// initialize bounding box
		//
		p = center (v);
	
		x_min = p.xcoord() - width(v)/2.0; 
		y_min = p.ycoord() - height(v)/2.0;
		x_max = p.xcoord() + width(v)/2.0;
		y_max = p.ycoord() + height(v)/2.0;		
		//

		forall_nodes (v,*this)
			{
			p = center (v);
			x_min = min(x_min ,p.xcoord() - width(v)/2.0);
			y_min = min(y_min, p.ycoord() - height(v)/2.0);
			x_max = max(x_max,p.xcoord() + width(v)/2.0);
			y_max = max(y_max,p.ycoord() + height(v)/2.0);
			}

		forall_edges (e,*this)
			{
			gdt::gdtlist<gdt::gdtpoint> bp = bends (e,source(e));
			forall (p,bp)
				{
				x_min = min(x_min,p.xcoord());
				y_min = min(y_min,p.ycoord());
				x_max = max(x_max,p.xcoord());
				y_max = max(y_max,p.ycoord());
				}
			}
		}
	else {x_min = 0; x_max = 0; y_min = 0; y_max = 0;}
	}
	


	void
	draw_undi_graph::
detect_external_node_and_edge
	(
	gdtnode& sn, 
	gdtedge& se
	)
	{
	gdtnode v, u;
	gdtedge e;
	gdt::gdtpoint p(0,0),
	      p_temp(0,0);
	gdt::gdtlist<gdt::gdtpoint> bl;
		
	// Makes p the point (gdtnode or bend) with the greatest ycoord
	//
	u = NULL_NODE;
	forall_nodes (v,*this)
		{
		p_temp = center (v);
		if (p_temp.ycoord() > p.ycoord()) 
			{
			p = p_temp;
			u = v;
			}
		}
		
	forall_edges (e,*this)
		{
		bl = bends(e);
		forall (p_temp,bl)
			if (p_temp.ycoord() > p.ycoord()) 
				{
				p = p_temp;
				se = e;
				u  = NULL_NODE;
				}
		}
		
	// Detect start gdtnode and start gdtedge
	//
	if (u) 	// Case p is a gdtnode
		{
		//cout << "\nHIGHEST NODE " << id(u) << flush;
		bool ae;
		gdt::gdtpoint pt(p.xcoord(),p.ycoord() + 1);
		p_temp = pt;
		se = find_prev_adj_edge (u,p_temp,ae);
		sn = opposite (u,se);
		// cout << "\nPREV EDGE: " << id(se) << flush;
		}

	else	// Case p is a bend
		{
		//cout << "\nHEIGEST BEND IN EDGE " << id(se) << flush;

		gdtnode v1  = source (se);
		gdtnode v2  = target (se);
		
		// Make p1 and p2 the object (gdtnode or bend) nearest to p on gdtedge e,
		// starting from v1 and v2 respectively
		//
		gdt::gdtpoint p1 = center (v1);
		gdt::gdtpoint p2 = center (v2);
		gdt::gdtlist<gdt::gdtpoint> bl_v1_v2 = bends (se,v1);
		gdt::gdtlist<gdt::gdtpoint> bl_v2_v1 = bends (se,v2);
		forall (p_temp,bl_v1_v2)
			if (p_temp.xcoord() == p.xcoord() && p_temp.ycoord() == p.ycoord()) break;
			else p1 = p_temp;
		forall (p_temp,bl_v2_v1)
			if (p_temp.xcoord() == p.xcoord() && p_temp.ycoord() == p.ycoord()) break;
			else p2 = p_temp;

		// Create the oriented segments that are needed for the angles comparation
		//
		gdt::gdtpoint p3 (p.xcoord() + 1, p.ycoord());
		gdt::gdtsegment s1 (p,p1);
		gdt::gdtsegment s2 (p,p2);
		gdt::gdtsegment s3 (p,p3);
		
		// Detect the minimum angle between the oriented segments s3,s1 and s3,s2
		// and return the corresponding gdtnode
		//
		double angle_s3_s1 = s3.angle(s1);
		double angle_s3_s2 = s3.angle(s2);
		if (angle_s3_s1 > 0) angle_s3_s1 -= 2*PI;
		if (angle_s3_s2 > 0) angle_s3_s2 -= 2*PI;
		if (angle_s3_s1 > angle_s3_s2) sn = v1;
		else			       sn = v2;		
		}
	}
	

	
	// --------------
	// Update methods
	// --------------

	void
	draw_undi_graph::
set_edge_sensivity
	(double k)
	{
	draw_curr->edge_sensivity = k;
	}



	// Dimensions


	void
	draw_undi_graph::
set_scale
	(double k)
	{
	draw_curr->scale = k;
	}



	void
	draw_undi_graph::
set_height	
	(gdtnode v, double l)
	{
	(*node_info)[v].height = l;
	}

	
	
	void
	draw_undi_graph::
set_width	
	(gdtnode v, double l)
	{
	(*node_info)[v].width = l;
	}



	void
	draw_undi_graph::
set_bend_radius
	(double br)
	{
	draw_curr->bend_radius = br;
	}



	// Positions


	void
	draw_undi_graph::
set_center
	(gdtnode v, gdt::gdtpoint p)
	{
	(*node_info)[v].center = p;
	}	
	

	void
	draw_undi_graph::	
set_altitude   
	(gdtnode v, double d)
	{
	(*node_info)[v].altitude = d;
	}


	void
	draw_undi_graph::
set_label_position
	(gdtnode v, gdt::gdtpoint p)
	{
	(*node_info)[v].label_pos = p;
	}


	void
	draw_undi_graph::
set_label_position
	(gdtedge e, gdt::gdtpoint p)
	{
	(*edge_info)[e].label_pos = p;
	}

	
	void
	draw_undi_graph::
set_anchor_source
	(gdtedge e, gdt::gdtpoint p)
	{
	(*edge_info)[e].anchor_source = p;
	}

	
	void
	draw_undi_graph::
set_anchor_target
	(gdtedge e, gdt::gdtpoint p)
	{
	(*edge_info)[e].anchor_target = p;
	}
	

	void
	draw_undi_graph::
set_anchor_point
	(gdtedge e, gdtnode v, gdt::gdtpoint p)
	{
	gdt::gdtpoint  cv = center(v);
	double dx = p.xcoord() - cv.xcoord();
	double dy = p.ycoord() - cv.ycoord();
	if      (v == source(e)) set_anchor_source (e,gdt::gdtpoint(dx,dy));
	else if (v == target(e)) set_anchor_target (e,gdt::gdtpoint(dx,dy));
	else 
		gdt_error("invalid gdtedge,gdtnode pair");
	}


	void
	draw_undi_graph::
set_bends
	(gdtedge e, gdt::gdtlist<gdt::gdtpoint> bl)
	{
	(*edge_info)[e].bends = bl;
	}


	void
	draw_undi_graph::
set_bends
	(gdtedge e, gdtnode v, gdt::gdtlist<gdt::gdtpoint> bl)
	{
	(*edge_info)[e].bends.clear();
	if (v == source(e)) (*edge_info)[e].bends = bl;
	else
		{
		//for (int i=1; i < bl.size(); i++) bl.move_to_front(bl.get_item(i));
		bl.reverse();
		(*edge_info)[e].bends = bl;
		}
	}




	int
	draw_undi_graph::
remove_avoidable_bends
	(
	gdtedge e
	)
	{
	int count_alligned = 0;
	int count_overlapping = 0;
	double epsilon = 0.0001; 
        //int e_id= id(e);
	gdtnode v = source(e);
        //int v_id= id(v);
	gdtnode w = target(e);
        //int w_id= id(w);
	gdt::gdtlist<gdt::gdtpoint> b_v_w = bends(e,v);
	gdt::gdtlist<gdt::gdtpoint> b_v_w_tmp;
	gdt::gdtlist<gdt::gdtpoint> b_v_w_new;

        // overlaping bends
	gdt::list_item p_item=b_v_w.first();
        int i;
	for (i=1; i<= b_v_w.size()-1; i++ ) // scan all the bends but the last one
          {
          gdt::gdtpoint p= b_v_w.inf(p_item);
          gdt::gdtpoint p_next= b_v_w.inf(b_v_w.succ(p_item));

          if (p.distance(p_next)<epsilon)
              count_overlapping++;
          else
              b_v_w_tmp.append(p);

          p_item= b_v_w.succ(p_item);  // the next one
          }
        if (b_v_w.last())
            b_v_w_tmp.append(b_v_w.tail());

        // alligned segments
	gdt::gdtpoint p1 = anchor_point(e,v);
	forall_items (p_item, b_v_w_tmp)
		{
		gdt::gdtpoint p = b_v_w_tmp.inf(p_item);

                gdt::gdtpoint p2;
                    { // init p2
                    gdt::list_item p_item_succ = b_v_w_tmp.succ(p_item);
                    if (!p_item_succ)
                        p2 = anchor_point(e,w);
                    else
                        p2 = b_v_w_tmp.inf(p_item_succ);
                    };

                gdt::gdtsegment s1 (p1,p), s2 (p,p2);
                double angle = fabs( s1.direction()-s2.direction() );

                if (angle > epsilon)
                    b_v_w_new.append(p);
                else
                    count_alligned++;
                p1 = p;
                }

	if (count_alligned+count_overlapping > 0)
            set_bends (e,v,b_v_w_new);

	return count_alligned+count_overlapping;
	}


	int
	draw_undi_graph::
remove_avoidable_bends
	(
	)
	{
	int count = 0;
	gdtedge e;
	forall_edges (e,*this)
		count += remove_avoidable_bends(e);
	return count;
	}


	gdt::gdtlist<gdtnode>
	draw_undi_graph::
replace_long_edge_with_chain
	(
	gdtedge e
	)
	{
	gdt::gdtlist<gdtnode> dummy_nodes;

       	gdtnode start_v = source(e); 
       	gdtnode end_v = target(e);

	//std::cout << "processing edge from " << id(start_v) << " to " << id(end_v) << "\n";

	gdt::gdtpoint start_p = center(start_v);
	gdt::gdtpoint end_p = center(end_v);

        //std::cout << start_p << " " << end_p << "\n";

	double start_x = start_p.xcoord();
	double start_y = start_p.ycoord();
	double end_x = end_p.xcoord();
	double end_y = end_p.ycoord();

	double delta_x = end_x - start_x;
	double delta_y = end_y - start_y;

	if( delta_x > 0 ) delta_x =  10.0;
	if( delta_x < 0 ) delta_x = -10.0;
	if( delta_y > 0 ) delta_y =  10.0;
	if( delta_y < 0 ) delta_y = -10.0;
 	
        //std::cout << "delta: " << delta_x << " " << delta_y << "\n";
	
        double current_x = start_x + delta_x;
        double current_y = start_y + delta_y;
       	gdtnode previous_v = start_v;

       	while ( current_x != end_x || current_y != end_y ) 
		{
		//std::cout << "[" << current_x << "," << current_y << "]";
		gdtnode this_node = new_node(gdt::gdtpoint(current_x,current_y));
		dummy_nodes.append(this_node);
		new_edge(previous_v, this_node, gdt::gdtlist<gdt::gdtpoint>());
		current_x = current_x + delta_x;
		current_y = current_y + delta_y;	
		previous_v = this_node;	
		}

	if ( previous_v != start_v ) 
		{  
		new_edge(previous_v, end_v, gdt::gdtlist<gdt::gdtpoint>());
		del_edge(e);
		}
        //std::cout << "\n";        

	return dummy_nodes;
	}


	gdt::gdtlist<gdtnode>
	draw_undi_graph::
replace_long_edges_with_chains
	(
	)
	{
	gdt::gdtlist<gdtnode> dummy_nodes;
	gdt::gdtlist<gdtedge> all_edges;  // copy of the list of the edges of the graph that will be modified
	gdtedge e;

	forall_edges(e,*this) 
		{
		all_edges.append(e);
  		}

	forall(e,all_edges) 
		{
		gdt::gdtlist<gdtnode> tmp_nodes = replace_long_edge_with_chain(e);
		dummy_nodes.conc(tmp_nodes, AFTER);
		}

	return dummy_nodes;
	}


	
	void
	draw_undi_graph::
remove_zig_zag_along_edge
	(
	gdtedge e
	)
	{
	gdtnode u = source(e);
	gdtnode v = target(e);

	//debug
	// 
	//std::cout << "\n\tDetecting zig-zag along edge " << id(e) << std::flush;	
	//std::cout << "starting from node " << id(u) << std::flush;	
	//
	
	gdt::gdtstack<gdt::gdtsegment> S; // stack that stores all segments of edge e
	gdt::gdtlist<gdt::gdtpoint> P = bends(e,u); // list that stores all points of edge e
	P.push(anchor_point (e,u));	
	P.append(anchor_point (e,v));
	gdt::gdtlist<gdt::gdtsegment> Sout; // list that stores the final segments of the edge

	// construct the segments of e and stores them in the stack
	// 		
	gdt::list_item it= P.first();	
	gdt::gdtpoint p1 = P.inf(it);
	gdt::gdtpoint p2 = P.inf(P.succ(it));
	S.push(gdt::gdtsegment(p1,p2));
	it = P.succ(it);	
	while (it!=P.last())
	  {
	  p1 = P.inf(it);
	  p2 = P.inf(P.succ(it));
	  S.push(gdt::gdtsegment(p1,p2));
	  it = P.succ(it);	  
	  }
	
	// simplify the number of segments if possible
	//	
	while (S.size() > 3)
	  {
	  gdt::gdtsegment s3 = S.pop();
	  gdt::gdtsegment s2 = S.pop();
	  gdt::gdtsegment s1 = S.pop();
	  
	  zig_zag_type zzt = is_zig_zag (s1,s2,s3);
	  switch (zzt)
	    {
	    case HORIZONTAL_UP_DOWN:
	    case HORIZONTAL_DOWN_UP:
	      {
	      //debug
	      // 
	      //std::cout << "\n\tFound zig-zag h-v-h " << std::flush;
	      //
	      
	      //         s1
	      //   p1 o-----o p2s  o p 
	      //            |
	      //            |s2
	      //            |
	      //   q o      o------o p3  
	      //           p2e  s3
	      // 
	      gdt::gdtpoint p1  = s1.start();
	      gdt::gdtpoint p2s = s2.start();
	      gdt::gdtpoint p2e = s2.end();
	      gdt::gdtpoint p3  = s3.end();
	      gdt::gdtpoint p (p3.xcoord(),p1.ycoord());
	      gdt::gdtpoint q (p1.xcoord(),p3.ycoord());
	      
	      gdt::gdtsegment sp2sp(p2s,p);
	      gdt::gdtsegment spp3(p,p3);
	      
	      gdt::gdtsegment sp1q(p1,q);
	      gdt::gdtsegment sqp2e(q,p2e);
	      
	      if ( p3 != P.tail() &&
	           !segment_intersects_nodes(sp2sp) &&
		   !segment_intersects_nodes(spp3) && 
	           !segment_overlaps_edges(sp2sp,&p2s) &&
		   !segment_overlaps_edges(spp3,&p3) ) 
	         {
	         //debug
	         // 
	         std::cout << "\n\tRemoving zig-zag h-v-h " << std::flush;
	         //
		 
		 //            sp1p
		 //   p1 o------------o p 
		 //                   |
		 //                   |spp3 
		 //                   |
		 //                   o p3  
		 //           
		 // 		 
		 gdt::gdtsegment sp1p(p1,p);
		 gdt::gdtsegment spp3(p,p3);
		 S.push(sp1p);
		 S.push(spp3);		 
	         }
	      else if ( p1 != P.head() && 
	                !segment_intersects_nodes(sp1q) &&
		        !segment_intersects_nodes(sqp2e) && 
		        !segment_overlaps_edges(sp1q,&p1) &&
		        !segment_overlaps_edges(sqp2e,&p2e) )
	         {
	         //debug
	         // 
	         //std::cout << "\n\tRemoving zig-zag h-v-h " << std::flush;
	         //		 
		 
		 //            
		 //   p1 o
		 //      |             
		 //      |sp1q            
		 //      |             
		 //    q o------------o p3   
		 //           sqp3
		 // 		 		 
		 gdt::gdtsegment sp1q(p1,q);
		 gdt::gdtsegment sqp3(q,p3);
		 S.push(sp1q);
		 S.push(sqp3);
		 }
	      else
	         {
	         //debug
	         // 
	         //std::cout << "\n\tzig-zag h-v-h not removable " << std::flush;
	         //
		 
		 S.push(s1);
	      	 S.push(s2);
		 Sout.push(s3);
		 }	        
	      }
	    break;

	    case VERTICAL_LEFT_RIGHT:
	    case VERTICAL_RIGHT_LEFT:
	      {
	      //debug
	      // 
	      //std::cout << "\n\tFound zig-zag v-h-v " << std::flush;
	      //
	      
	      //   p o      o p3
	      //            |
	      //            | s3
	      //       s2   |
	      // p2s o------o p2e         
	      //     |
	      //     | s1     
	      //     |
	      //     o p1   o q    
	      // 
	      gdt::gdtpoint p1  = s1.start();
	      gdt::gdtpoint p2s = s2.start();
	      gdt::gdtpoint p2e = s2.end();
	      gdt::gdtpoint p3  = s3.end();
	      gdt::gdtpoint p (p1.xcoord(),p3.ycoord());
	      gdt::gdtpoint q (p3.xcoord(),p1.ycoord());
	      
	      gdt::gdtsegment sp2sp(p2s,p);
	      gdt::gdtsegment spp3(p,p3);
	      
	      gdt::gdtsegment sp1q(p1,q);
	      gdt::gdtsegment sqp2e(q,p2e);
	      
	      if ( p3 != P.tail() &&
	           !segment_intersects_nodes(sp2sp) &&
		   !segment_intersects_nodes(spp3) && 
	           !segment_overlaps_edges(sp2sp,&p2s) &&
		   !segment_overlaps_edges(spp3,&p3) ) 
	         {
	         //debug
	         // 
	         std::cout << "\n\tRemoving zig-zag h-v-h " << std::flush;
	         //
		 
	         //   p o------o p3
	         //     | spp3   
	         //     |      
	         //     |      
	         //     | sp1p       
	         //     |
	         //     |      
	         //     |
	         //     o p1       
	         // 
		 gdt::gdtsegment sp1p(p1,p);
		 gdt::gdtsegment spp3(p,p3);
		 S.push(sp1p);
		 S.push(spp3);		 
	         }
	      else if ( p1 != P.head() && 
	                !segment_intersects_nodes(sp1q) &&
		        !segment_intersects_nodes(sqp2e) && 
		        !segment_overlaps_edges(sp1q,&p1) &&
		        !segment_overlaps_edges(sqp2e,&p2e) )
	         {
	         //debug
	         // 
	         std::cout << "\n\tRemoving zig-zag h-v-h " << std::flush;
	         //
		 		 
	         //            o p3
	         //            |
	         //            | 
	         //            |
	         //            | sqp3       
	         //            |
	         //            |
	         //       sp1q |
	         //   p1 o-----o q    
	         // 
		 gdt::gdtsegment sp1q(p1,q);
		 gdt::gdtsegment sqp3(q,p3);
		 S.push(sp1q);
		 S.push(sqp3);
		 }
	      else
	         {
	         //debug
	         // 
	         //std::cout << "\n\tzig-zag h-v-h not removable" << std::flush;
	         //
		 
		 S.push(s1);
	      	 S.push(s2);
		 Sout.push(s3);
		 }	        
	      }	      
	    break;
	    
	    case (NO_ZIG_ZAG):
	      {
	      S.push(s1);
	      S.push(s2);
	      Sout.push(s3);
	      }
	    break;	    
	    } //end switch
	  }// end while
	  
	  while (!S.empty())
	    Sout.push(S.pop());
	
	  // reconstruct the list of bends of e based on 
	  // Sout list of segments
	
	  gdt::gdtlist<gdt::gdtpoint> B;
	  gdt::gdtsegment s;
	  forall (s,Sout)
	    B.append(s.start());
	  B.del_item(B.first()); // it is an anchor point
	  set_bends (e,u,B);	
	}


	zig_zag_type
	draw_undi_graph::
is_zig_zag
	(
	gdt::gdtsegment s1,
	gdt::gdtsegment s2,
	gdt::gdtsegment s3
	)
	{
	zig_zag_type zzt = NO_ZIG_ZAG;
	if (s1.is_almost_horizontal() && s2.is_almost_vertical() && s3.is_almost_horizontal())
	  {
	  gdt::gdtpoint p1 = s1.start();
	  gdt::gdtpoint p2 = s2.start();
	  gdt::gdtpoint p3 = s3.end();
	  double x1 = p1.xcoord();
	  double x2 = p2.xcoord();
	  double x3 = p3.xcoord();
	  if ( (x1 > x2 && x2 > x3) || 
	       (x1 < x2 && x2 < x3)
	     )
	     {
	     double y1 = p1.ycoord();
	     double y3 = p3.ycoord();
	     if (y1 > y3)
	       zzt = HORIZONTAL_UP_DOWN;
	     else
	       zzt = HORIZONTAL_DOWN_UP; 
	     }	  
	  }
	else if (s1.is_almost_vertical() && s2.is_almost_horizontal() && s3.is_almost_vertical())
	  {
	  gdt::gdtpoint p1 = s1.start();
	  gdt::gdtpoint p2 = s2.start();
	  gdt::gdtpoint p3 = s3.end();
	  double y1 = p1.ycoord();
	  double y2 = p2.ycoord();
	  double y3 = p3.ycoord();
	  if ( (y1 > y2 && y2 > y3) ||
	       (y1 < y2 && y2 < y3)
	     )   
	     {
	     double x1 = p1.xcoord();
	     double x3 = p3.xcoord();
	     if (x1 > x3)
	       zzt = VERTICAL_RIGHT_LEFT;
	     else
	       zzt = VERTICAL_LEFT_RIGHT;
	     }
	  }
	  
	return zzt;	
	}


	bool
	draw_undi_graph::
segment_intersects_node(gdt::gdtsegment s, gdtnode v)
	{
	bool flag = false;		
	gdt::gdtpoint p  = center(v);	 
	double x   = p.xcoord();
	double y   = p.ycoord();
	double dx2 = width(v)/2;
	double dy2 = height(v)/2;	
	gdt::gdtlist<gdt::gdtpoint> vertex_list;
	vertex_list.append(gdt::gdtpoint (x-dx2, y-dy2)); // p4---p3
	vertex_list.append(gdt::gdtpoint (x+dx2, y-dy2)); // |     |
	vertex_list.append(gdt::gdtpoint (x+dx2, y+dy2)); // |     |
	vertex_list.append(gdt::gdtpoint (x-dx2, y+dy2)); // p1---p2
	gdt::gdtpolygon border(vertex_list);	
	gdt::gdtlist<gdt::gdtpoint> lip = border.intersection(s);
	if (!lip.empty())
	  flag = true;
	return flag;
	}


	bool
	draw_undi_graph::
segment_intersects_nodes(gdt::gdtsegment s)
	{
	bool flag = false;
	gdtnode v;
	forall_nodes (v,*this)
	  if (segment_intersects_node (s,v))
	  	{
		flag = true;
		break;
	  	}
	// debug
	// 
	//if (flag)
	//  std::cout << "\t\t\n segment intersects nodes " << std::flush;
	//
	return flag;
	}
	
	
	
	bool
	draw_undi_graph::
segment_overlaps_edge 
	(
	gdt::gdtsegment s,
	gdtedge e,
	gdt::gdtpoint* p
	)
	{
	bool flag = false;
	gdt::gdtpoint q1 = s.start();
	gdt::gdtpoint q2 = s.end();
	gdtnode u = source(e);
	gdtnode v = target(e);	
	gdt::gdtlist<gdt::gdtpoint> P = bends(e,u); // list that stores all points of edge e
	P.push(anchor_point (e,u));	
	P.append(anchor_point (e,v));	

	// construct the segments of e and stores them in the stack
	// 		
	gdt::gdtlist<gdt::gdtsegment> S; // list that stores all segments of edge e
	gdt::list_item it= P.first();	
	gdt::gdtpoint p1 = P.inf(it);
	gdt::gdtpoint p2 = P.inf(P.succ(it));
	S.append(gdt::gdtsegment(p1,p2));
	it = P.succ(it);	
	while (it!=P.last())
	  {
	  p1 = P.inf(it);
	  p2 = P.inf(P.succ(it));
	  S.append(gdt::gdtsegment(p1,p2));
	  it = P.succ(it);  
	  }
	
	gdt::gdtsegment se;
	forall (se,S)	
		{
		gdt::gdtpoint p1 = se.start();
		gdt::gdtpoint p2 = se.end();
		if ( ((p != NULL) && (q1 != *p) && se.contains(q1)) ||
		     ((p != NULL) && (q2 != *p) && se.contains(q2)) ||
		     ((p != NULL) && (p1 != *p) && s.contains(p1)) ||
		     ((p != NULL) && (p2 != *p) && s.contains(p2))
		   )
		   {
		   flag = true;
		   break;
		   }
		}				
	return flag;
	}
	

	bool
	draw_undi_graph::
segment_overlaps_edges
	(
	gdt::gdtsegment s,
	gdt::gdtpoint* p
	)
	{
	bool flag = false;
	gdtedge e;
	forall_edges (e,*this)
	  if (segment_overlaps_edge (s,e,p))
	  	{
		flag = true;
		break;
	  	}
	// debug
	// 
	//if (flag)
	//  std::cout << "\t\t\n segment overlaps edges " << std::flush;
	//

	return flag;
	}

	
	/* the following struct is used by space_overlapping_edges */

	struct 
incident_edge 
	{
	gdtedge e;
	gdt::gdtpoint b;   // the first bend or opposite node center
	double leg;        // the first bend distance from the node
	bool straight;     // true if the edge has no bend
	bool left;         // true if the edge turns left
	};




	void
	draw_undi_graph::
space_overlapping_edges
	(
	)
	{
	gdtnode v;
	forall_nodes(v,*this) 
		{
		gdt::gdtlist<incident_edge> top;    // edges leaving upwards
		gdt::gdtlist<incident_edge> bottom; // edges leaving backwards
		gdt::gdtlist<incident_edge> left;   // edges leaving towards left
		gdt::gdtlist<incident_edge> right;  // edges leaving towards right
				
		gdt::gdtpoint p_v = center(v);
			
		gdtedge e;
		forall_edges_adjacent_node(e,v,*this)
			{
			incident_edge ie;
			ie.e = e;
			gdt::gdtpoint next_b;
			gdt::gdtlist<gdt::gdtpoint> Lb = bends(e,v); // from v moving out
			ie.straight = (Lb.size() == 0);
			if( ie.straight )
				{   
				ie.b = center(opposite(v,e));
				}
			else	
				{ 
				ie.b = Lb.pop_front();
				if ( Lb.size() > 0 ) 	next_b = Lb.pop_front();
				else 			next_b = center(opposite(v,e));
				}

			double bx = ie.b.xcoord();
			double by = ie.b.ycoord();
			double cx = center(v).xcoord();
			double cy = center(v).ycoord();
					
				
			bool vertical; 
			if( cx == bx ) 	vertical = true;
			else 		vertical = false;

			//std::cout << "vertical = " << vertical << std::endl;

					
			double delta;
			if( vertical ) 	delta = by-cy;
			else 		delta = bx-cx;
					
			ie.leg = delta > 0.0 ? delta : -delta;
			
			if( !ie.straight ) 
				{
				if( vertical ) 
					{
					if( delta > 0 )
						ie.left = next_b.xcoord() < cx;	
					else
						ie.left = next_b.xcoord() > cx;
					}
				else  // horizontal
					{
					if( delta > 0 ) 
						ie.left = next_b.ycoord() > cy;
					else
						ie.left = next_b.ycoord() < cy;
					}
				}
			
			// insert ie into the top list
			//
			if( vertical && delta > 0 ) // top
				{
				gdt::list_item li;
				if( top.size() == 0 ) 
					top.push_front(ie);
				else
					{
					li = top.first();
					if( top.inf(li).leg < ie.leg ) top.push_front(ie);
					else
						{
						while( top.inf(li).leg > ie.leg )
							{
							if( li == top.last() ) break;
							else li = top.succ(li);
							}
						if( top.inf(li).leg > ie.leg ) 	top.push_back(ie);
						else				top.insert(ie, li, BEFORE);		
						}
					}
				
				}
				
			// insert ie into the bottom list
			//
			if( vertical && delta < 0 ) // bottom
				{
				gdt::list_item li;
				if( bottom.size() == 0 ) 
					bottom.push_front(ie);
				else
					{
					li = bottom.first();
					if( bottom.inf(li).leg < ie.leg ) bottom.push_front(ie);
					else
						{
						while( bottom.inf(li).leg > ie.leg )
							{
							if( li == bottom.last() ) break;
							else li = bottom.succ(li);
							}
						if( bottom.inf(li).leg > ie.leg ) 	bottom.push_back(ie);
						else					bottom.insert(ie, li, BEFORE);		
						}
					}
					
				}	
				
			// insert ie into the right list
			//
			if( !vertical && delta > 0 ) // right
				{
				gdt::list_item li;
				if( right.size() == 0 ) 
					right.push_front(ie);
				else
					{
					li = right.first();
					if( right.inf(li).leg < ie.leg ) right.push_front(ie);
					else
						{
						while( right.inf(li).leg > ie.leg )
							{
							if( li == right.last() ) break;
							else li = right.succ(li);
							}
						if( right.inf(li).leg > ie.leg ) right.push_back(ie);
						else			 	 right.insert(ie, li, BEFORE);		
						}
					}
				}
				
			// insert ie into the left list
			//
			if( !vertical && delta < 0 ) // left
				{
				if( left.size() == 0 ) left.push_front(ie);
				else
					{
					gdt::list_item li = left.first();
					if( left.inf(li).leg < ie.leg ) left.push_front(ie);
					else
						{
						while( left.inf(li).leg > ie.leg )
							{
							if( li == left.last() ) break;
							else li = left.succ(li);
							}
						if( left.inf(li).leg > ie.leg ) left.push_back(ie);
						else			        left.insert(ie, li, BEFORE);		
						}
					}
				
				}
			}
	
		incident_edge ie;
		double y = center(v).ycoord();
		double x = center(v).xcoord();
		double dd = 1.0/((double)(left.size()));
		int counter = 1;         // number of already positioned edges
		bool prev_left = false;  // it is the same
		bool first = true;       // first edge to position
		forall(ie,left) 
			{
			if( first )    // first edge stays where it is
				{
				first = false;
				}
			else           // other edges must be redrawn
				{
				if( ie.left )
					if( prev_left ) 	y -= dd;
					else 			y -= dd*counter;
				else
					if( !prev_left ) 	y += dd;
					else 			y += dd*counter; 
				set_anchor_point(ie.e,v, gdt::gdtpoint(x,y));
				gdt::gdtlist<gdt::gdtpoint> Lb = bends(ie.e,v);
				if( Lb.size() > 0 )
					{
					gdt::gdtpoint first_b = Lb.pop_front();
					Lb.push_front(gdt::gdtpoint(first_b.xcoord(),y));
					set_bends(ie.e, v, Lb);
					}
				prev_left = ie.left;
				counter++;
				}
			}
			
		y = center(v).ycoord();
		x = center(v).xcoord();
		dd = 1.0/((double)(right.size()));
		counter = 1;         // number of already positioned edges
		prev_left = false;  // it is the same
		first = true;       // first edge to position
		forall(ie,right) 
			{
			if( first )    // first edge stays where it is
				{
				first = false;
				}
			else           // other edges must be redrawn
				{
				if( ie.left )
					if( prev_left ) 	y += dd;
					else 			y += dd*counter;
				else
					if( !prev_left ) 	y -= dd;
					else 			y -= dd*counter; 
				set_anchor_point(ie.e,v, gdt::gdtpoint(x,y));
				gdt::gdtlist<gdt::gdtpoint> Lb = bends(ie.e,v);
				if( Lb.size() > 0 )
					{
					gdt::gdtpoint first_b = Lb.pop_front();
					Lb.push_front(gdt::gdtpoint(first_b.xcoord(),y));
					set_bends(ie.e, v, Lb);
					}
				prev_left = ie.left;
				counter++;
				}
			}
			
		y = center(v).ycoord();
		x = center(v).xcoord();
		dd = 1.0/((double)(top.size()));
		counter = 1;         // number of already positioned edges
		prev_left = false;  // it is the same
		first = true;       // first edge to position
		forall(ie,top) 
			{
			if( first )    // first edge stays where it is
				{
				first = false;
				}
			else           // other edges must be redrawn
				{
				if( ie.left )
					if( prev_left ) 	x += dd;
					else 			x += dd*counter;
				else
					if( !prev_left ) 	x -= dd;
					else 			x -= dd*counter; 
				set_anchor_point(ie.e,v, gdt::gdtpoint(x,y));
				gdt::gdtlist<gdt::gdtpoint> Lb = bends(ie.e,v);
				if( Lb.size() > 0 )
					{
					gdt::gdtpoint first_b = Lb.pop_front();
					Lb.push_front(gdt::gdtpoint(first_b.xcoord(),y));
					set_bends(ie.e, v, Lb);
					}
				prev_left = ie.left;
				counter++;
				}
			}
			
		y = center(v).ycoord();
		x = center(v).xcoord();
		dd = 1.0/((double)(bottom.size()));
		counter = 1;         // number of already positioned edges
		prev_left = false;  // it is the same
		first = true;       // first edge to position
		forall(ie,bottom) 
			{
			if( first )    // first edge stays where it is
				{
				first = false;
				}
			else           // other edges must be redrawn
				{
				if( ie.left )
					if( prev_left ) 	x -= dd;
					else 			x -= dd*counter;
				else
					if( !prev_left ) 	x += dd;
					else 			x += dd*counter; 
				set_anchor_point(ie.e,v, gdt::gdtpoint(x,y));
				gdt::gdtlist<gdt::gdtpoint> Lb = bends(ie.e,v);
				if( Lb.size() > 0 )
					{
					gdt::gdtpoint first_b = Lb.pop_front();
					Lb.push_front(gdt::gdtpoint(first_b.xcoord(),y));
					set_bends(ie.e, v, Lb);
					}
				prev_left = ie.left;
				counter++;
				}
			}
			
		}
	}

	void
	draw_undi_graph::
undo_space_overlapping_edges
	(
	gdtedge e
	)
	{
	if( number_of_bends(e) == 0 ) return;

	gdtnode start_n = source(e);
	gdt::gdtpoint start = center(start_n);
        gdt::gdtpoint end = center(target(e));

	gdt::gdtlist<gdt::gdtpoint> P = bends(e,start_n);
	gdt::gdtlist<gdt::gdtpoint> temp;
	gdt::gdtlist<gdt::gdtpoint> result;

	gdt::gdtpoint p;
	double start_x = start.xcoord();
	double start_y = start.ycoord();
	double end_x   = end.xcoord();
	double end_y   = end.ycoord();
	bool first = true;  

	forall(p,P) {
		double this_x = p.xcoord();
		double this_y = p.ycoord();

		/* one between this_x or this_y must be equal to previous_x or previous_y */
		if( first ) {

			double delta_x = this_x - start_x;
			double delta_y = this_y - start_y;
			delta_x = (delta_x > 0.0 ? delta_x : -delta_x);  // take the absolute value of the difference
			delta_y = (delta_y > 0.0 ? delta_y : -delta_y);  // same

			if ( delta_x > delta_y ) {  // there is a significant difference in x coord => y coord should be equal
				this_y = start_y;
			} else if ( delta_y > delta_x ) {  // there is a significant difference in y coord => x coord should be equal
				this_x = start_x;
			}
			first = false;
		}
		temp.push_front(gdt::gdtpoint(this_x,this_y));
	}

        // now in temp you have the list reversed

        //std::cout << end << " ";

	first = true;
	forall(p,temp) {
		double this_x = p.xcoord();
		double this_y = p.ycoord();

		/* one between this_x or this_y must be equal to previous_x or previous_y */
		if( first ) {

			double delta_x = this_x - end_x;
			double delta_y = this_y - end_y;
			delta_x = (delta_x > 0.0 ? delta_x : -delta_x);  // take the absolute value of the difference
			delta_y = (delta_y > 0.0 ? delta_y : -delta_y);  // same

			if ( delta_x > delta_y ) {  // there is a significant difference in x coord => y coord should be equal
				this_y = end_y;
			} else if ( delta_y > delta_x ) {  // there is a significant difference in y coord => x coord should be equal
				this_x = end_x;
			}
			first = false;
		}
		result.push_front(gdt::gdtpoint(this_x,this_y));
                //std::cout << gdt::gdtpoint(this_x,this_y) << " ";
	}
        //std::cout << start << " ";
        //std::cout << "\n";

	set_bends(e, start_n, result);
	}


	void
	draw_undi_graph::
undo_space_overlapping_edges
	(
	)
	{
	gdtedge e; 
	forall_edges(e,*this) 
		{
		undo_space_overlapping_edges(e);
		}
	}


	gdt::gdtlist<gdtnode>
	draw_undi_graph::
replace_bends_with_nodes
	(
	gdtedge e
	)
	{
	gdt::gdtlist<gdtnode> dummy_nodes;

	if( number_of_bends(e) == 0 ) return dummy_nodes;

        gdtnode start_v = source(e); 
        gdtnode end_v = target(e);
	gdt::gdtlist<gdt::gdtpoint> all_bends = bends(e,start_v);

	gdtnode previous_node = start_v;
	gdt::gdtpoint p;
	forall(p,all_bends) {
		//std::cout << p << "\t";
		gdtnode this_node = new_node(p);
		dummy_nodes.append(this_node);
		new_edge(previous_node, this_node, gdt::gdtlist<gdt::gdtpoint>());
		previous_node = this_node;
	}
	new_edge(previous_node, end_v, gdt::gdtlist<gdt::gdtpoint>());
	del_edge(e);

	return dummy_nodes;
	}


	gdt::gdtlist<gdtnode>
	draw_undi_graph::
replace_bends_with_nodes
	(
	)
	{
	gdt::gdtlist<gdtnode> dummy_nodes;
	gdt::gdtlist<gdtedge> all_edges;  // copy of the list of the edges of the graph that will be modified
	gdtedge e;

	forall_edges(e,*this) 
		{
		all_edges.append(e);
  		}

	forall(e,all_edges) 
		{
		gdt::gdtlist<gdtnode> tmp_nodes = replace_bends_with_nodes(e);
		dummy_nodes.conc(tmp_nodes, AFTER);
		}

	return dummy_nodes;
	}


	// Colors

	void
	draw_undi_graph::
set_body_color
	(gdtnode v, color c)
	{
	(*node_info)[v].internal_col = c;
	}


	void
	draw_undi_graph::
set_body_color
	(gdtedge e, color c)
	{
	(*edge_info)[e].col = c;
	}


	void
	draw_undi_graph::
set_label_color
	(gdtnode v, color c)
	{
	(*node_info)[v].label_col = c;
	}


	void
	draw_undi_graph::
set_label_color
	(gdtedge e, color c)
	{
	(*edge_info)[e].label_col = c;
	}


	void
	draw_undi_graph::
set_border_color
	(gdtnode v, color c)
	{
	(*node_info)[v].border_col = c;
	}


	// Shapes


	void
	draw_undi_graph::
set_shape
	(gdtnode v, node_shape_type st)
	{
	(*node_info)[v].shape = st;
	}



	void
	draw_undi_graph::
set_shape
	(gdtedge e, edge_shape_type st)
	{
	(*edge_info)[e].shape = st;
	}


	// Labels


	void
	draw_undi_graph::
set_label
	(gdtnode v, std::string s)
	{
	(*node_info)[v].label = s;
	}


	void
	draw_undi_graph::
set_label
	(gdtedge e, std::string s)
	{
	(*edge_info)[e].label = s;
	}


	// Switches


	void
	draw_undi_graph::
show_shadows
	()
	{
	draw_curr->shadows_are_visible=true;
	}

	void
	draw_undi_graph::
show_node_labels
	()
	{
	draw_curr->node_labels_are_visible=true;
	}


	void
	draw_undi_graph::
show_edge_labels
	()
	{
	draw_curr->edge_labels_are_visible=true;
	}


	void
	draw_undi_graph::
hide_shadows
	()
	{
	draw_curr->shadows_are_visible=false;
	}


	void
	draw_undi_graph::
hide_node_labels
	()
	{
	draw_curr->node_labels_are_visible=false;
	}


	void
	draw_undi_graph::
hide_edge_labels
	()
	{
	draw_curr->edge_labels_are_visible=false;
	}


	// Initializers	


	void 
	draw_undi_graph::
apply_defaults
	()
	{
	gdtnode v;
	gdtedge e;
	forall_nodes(v,*this) apply_defaults(v);
	forall_edges(e,*this) apply_defaults(e);
	}


	void 
	draw_undi_graph::
apply_defaults
	(gdtnode v)
	{
	ostringstream x;
	x<< id(v); // << ends;

	std::string s(x.str());
	set_label(v,s);
	//
	set_width	   (v,node_defs->width);
	set_height	   (v,node_defs->height);
	set_body_color     (v,node_defs->body_color);
	set_border_color   (v,node_defs->border_color);
	set_label_color    (v,node_defs->label_color);
	set_altitude       (v,node_defs->altitude);
	set_shape	   (v,node_defs->shape);
	set_label_position (v,recommended_label_position(v));
	
	// Added by Walter Didimo
	// --------------------------------------------
	// this is equivalent to assume that, with k=1,
	// each gdtnode has dimension 0.5 x 0.5, and hence 
	// that the distance between two any nodes is 
	// always 0.5 in a unit-grid
	// --------------------------------------------

	set_width  (v,scale()/2);
	set_height (v,scale()/2);
	}


	void 
	draw_undi_graph::
apply_defaults
	(gdtedge e)
	{
	ostringstream x;
	x << id(e); //<< ends;
	std::string s(x.str());
	set_label(e,s);
	//
	set_anchor_source  (e,edge_defs->anchor);
	set_anchor_target  (e,edge_defs->anchor);
	set_shape	   (e,edge_defs->shape);
	set_body_color     (e,edge_defs->body_color);
	set_label_color    (e,edge_defs->label_color);
	set_label_position (e,recommended_label_position(e));
	}


	// Drawing control


	void
	draw_undi_graph::
shift 
	(double sh, direction_type dir)
	{
	gdtnode v;
	gdtedge e;
	
	forall_nodes (v,*this)  
		{
		gdt::gdtpoint p = center(v);
		gdt::gdtpoint pl= label_position(v);

		switch (dir)
			{
			case RIGHT: 
				{
				gdt::gdtpoint p1 (p.xcoord() +sh, p.ycoord());
				gdt::gdtpoint pl1(pl.xcoord()+sh,pl.ycoord());
				set_center (v, p1); 
				set_label_position (v, pl1); 
				break;
				}
				
			case LEFT:
				{  
				gdt::gdtpoint p1 (p.xcoord() -sh, p.ycoord());
				gdt::gdtpoint pl1(pl.xcoord()-sh,pl.ycoord());
				set_center (v, p1); 
				set_label_position (v, pl1); 
				break;
				}
				
			case UP:
				{
				gdt::gdtpoint p1 (p.xcoord() ,p.ycoord() +sh);
				gdt::gdtpoint pl1(pl.xcoord(),pl.ycoord()+sh);
				set_center (v, p1);
				set_label_position (v,pl1);
				break;
				}
				
			case DOWN:
				{  
				gdt::gdtpoint p1 (p.xcoord() ,p.ycoord() -sh);
				gdt::gdtpoint pl1(pl.xcoord(),pl.ycoord()-sh);
				set_center (v, p1);
				set_label_position (v,pl1);
				break;
				}
			}

		}
		
	forall_edges (e,*this)
		{
		gdt::gdtpoint p;
		gdt::gdtlist<gdt::gdtpoint> L = bends (e,source(e));
		gdt::gdtlist<gdt::gdtpoint> L1;
		forall (p,L)
			switch (dir)
				{
				case RIGHT: {gdt::gdtpoint p1(p.xcoord()+sh,p.ycoord());L1.append(p1);} break;
				case LEFT:  {gdt::gdtpoint p1(p.xcoord()-sh,p.ycoord());L1.append(p1);} break;
				case UP:    {gdt::gdtpoint p1(p.xcoord(),p.ycoord()+sh);L1.append(p1);} break;
				case DOWN:  {gdt::gdtpoint p1(p.xcoord(),p.ycoord()-sh);L1.append(p1);} break;
				}
		set_bends (e,L1);
		p = label_position (e);
		switch (dir)
			{
			case RIGHT: p = gdt::gdtpoint(p.xcoord()+sh,p.ycoord()); break;
			case LEFT:  p = gdt::gdtpoint(p.xcoord()-sh,p.ycoord()); break;
			case UP:    p = gdt::gdtpoint(p.xcoord(),p.ycoord()+sh); break;
			case DOWN:  p = gdt::gdtpoint(p.xcoord(),p.ycoord()-sh); break;
			}
		set_label_position (e,p);
		}
	}
	

	void
	draw_undi_graph::
rotate 
	(angle_type anglet, gdt::gdtpoint ref)
	{
	double angle;
	gdtnode v;
	gdtedge e;
	switch (anglet)
		{
		case _000: angle=0; break;
		case _090: angle=M_PI/2; break;
		case _180: angle=M_PI; break;
		case _270: angle=3*M_PI/2; break;
		case _360: break;
		}
	forall_nodes (v,*this)
		{
		gdt::gdtpoint p = center(v);
		gdt::gdtpoint p1 = p.rotate(ref,angle);
		set_center (v,p1);
		p = label_position (v);
		p1 = p.rotate(ref,angle);
		set_label_position (v,p1);
		if (anglet==_090 || anglet== _270)
			{
			double temp_dimension=width(v);
			set_width(v,height(v));
			set_height(v,temp_dimension);
			}
		}
		
	forall_edges (e,*this)
		{
		gdt::gdtpoint p;
		gdt::gdtlist<gdt::gdtpoint> L = bends (e,source(e));
		gdt::gdtlist<gdt::gdtpoint> L1;
		forall (p,L)
			{
			gdt::gdtpoint p1 = p.rotate(ref,angle);	
			L1.append(p1);
			}
		set_bends (e,L1);
		p = label_position (e);
		gdt::gdtpoint p1 = p.rotate(ref,angle);
		set_label_position (e,p1);
		gdt::gdtpoint as1=anchor_source(e);
		gdt::gdtpoint at1=anchor_target(e);	
		gdt::gdtpoint z(0,0);	
		//cout << "as1=" << as1 << endl;
		//cout << "at1=" << at1 << endl;
		set_anchor_source(e,as1.rotate(z,angle));
		
		set_anchor_target(e,at1.rotate(z,angle));
		}	
	}
	
	
	void
	draw_undi_graph::
flip_horizontally 
	(gdt::gdtpoint fulcrum)
	{
	gdtnode v;
	gdtedge e;
	forall_nodes (v,*this)  
		{
		gdt::gdtpoint p = center(v);
		gdt::gdtpoint p1= label_position(v);
		double dx = p.xcoord() - fulcrum.xcoord();
		p = gdt::gdtpoint(fulcrum.xcoord()-dx,p.ycoord());
		p1= gdt::gdtpoint(fulcrum.xcoord()-dx,p1.ycoord());
		set_center (v,p);
		set_label_position (v,p1);
		}
		
	forall_edges (e,*this)
		{
		gdt::gdtpoint p;
		gdt::gdtlist<gdt::gdtpoint> L = bends (e,source(e));
		gdt::gdtlist<gdt::gdtpoint> L1;
		forall (p,L)
			{
			double dx = p.xcoord() - fulcrum.xcoord();
			gdt::gdtpoint p1(fulcrum.xcoord()-dx,p.ycoord());
			L1.append(p1);
			}
		set_bends (e,L1);
		p = anchor_source(e);
		set_anchor_source (e,gdt::gdtpoint(-p.xcoord(),p.ycoord()));
		p = anchor_target(e);
		set_anchor_target (e,gdt::gdtpoint(-p.xcoord(),p.ycoord()));
		p = label_position (e);
		double dx = p.xcoord() - fulcrum.xcoord();
		gdt::gdtpoint p1(fulcrum.xcoord()-dx,p.ycoord());
		set_label_position (e,p1);
		}	
	}


	void
	draw_undi_graph::
flip_vertically 
	(gdt::gdtpoint fulcrum)
	{
	gdtnode v;
	gdtedge e;
	forall_nodes (v,*this)  
		{
		gdt::gdtpoint p = center(v);
		gdt::gdtpoint p1= label_position(v);
		double dy = p.ycoord() - fulcrum.ycoord();
		p = gdt::gdtpoint(p.xcoord(),fulcrum.ycoord()-dy);
		p1= gdt::gdtpoint(p1.xcoord(),fulcrum.ycoord()-dy);
		set_center (v,p);
		set_label_position (v,p1);
		}

	forall_edges (e,*this)
		{
		gdt::gdtpoint p;
		gdt::gdtlist<gdt::gdtpoint> L = bends (e);
		gdt::gdtlist<gdt::gdtpoint> L1;
		forall (p,L)
			{
			double dy = p.ycoord() - fulcrum.ycoord();
			//gdt::gdtpoint p1(fulcrum.xcoord(),p.ycoord()-dy);
			gdt::gdtpoint p1(p.xcoord(),fulcrum.ycoord()-dy);
			L1.append(p1);
			}
		set_bends (e,L1);
		p = anchor_source(e);
		set_anchor_source (e,gdt::gdtpoint(p.xcoord(),-p.ycoord()));
		p = anchor_target(e);
		set_anchor_target (e,gdt::gdtpoint(p.xcoord(),-p.ycoord()));
		p = label_position (e);
		double dy = p.ycoord() - fulcrum.ycoord();
		gdt::gdtpoint p1(p.xcoord(),fulcrum.ycoord()-dy);
		set_label_position (e,p1);
		}
	}


	void
	draw_undi_graph::
rescale (double k)
	{
	denormalize_coordinates();
	if (!k) k = recommended_scale();
	set_scale(k);
	normalize_coordinates();
	}


	void
	draw_undi_graph::
normalize_coordinates
	()
	{
	gdtnode v;
	gdtedge e;
	
	double k = scale();
	
	forall_nodes (v,*this)
		{
		gdt::gdtpoint p = center(v);
		set_center (v,gdt::gdtpoint(p.xcoord()*k+k,p.ycoord()*k+k));
		p = label_position (v);
		set_label_position (v,gdt::gdtpoint(p.xcoord()*k+k,p.ycoord()*k+k));
		}
		
	forall_edges (e,*this)
		{
		gdt::gdtpoint p;
		gdt::gdtlist<gdt::gdtpoint> L = bends(e,source(e));
		gdt::gdtlist<gdt::gdtpoint> LN;
		forall (p,L) LN.append (gdt::gdtpoint(p.xcoord()*k+k,p.ycoord()*k+k));
		set_bends (e,LN);
		p = anchor_source(e);
		set_anchor_source(e,gdt::gdtpoint(p.xcoord()*k,p.ycoord()*k));
		p = anchor_target(e);
		set_anchor_target(e,gdt::gdtpoint(p.xcoord()*k,p.ycoord()*k));
		p = label_position (e);
		set_label_position (e,gdt::gdtpoint(p.xcoord()*k+k,p.ycoord()*k+k));
		}

	}
	
	
	void
	draw_undi_graph::
denormalize_coordinates
	()
	{
	gdtnode v;
	gdtedge e;
	
	double k = scale();
	
	forall_nodes (v,*this)
		{
		gdt::gdtpoint p = center(v);
		set_center (v,gdt::gdtpoint((p.xcoord()-k)/k,(p.ycoord()-k)/k));
		p = label_position (v);
		set_label_position (v,gdt::gdtpoint((p.xcoord()-k)/k,(p.ycoord()-k)/k));
		}
		
	forall_edges (e,*this)
		{
		gdt::gdtpoint p;
		gdt::gdtlist<gdt::gdtpoint> L = bends(e,source(e));
		gdt::gdtlist<gdt::gdtpoint> LN;
		forall (p,L) LN.append (gdt::gdtpoint((p.xcoord()-k)/k,(p.ycoord()-k)/k));
		set_bends (e,LN);
		p = anchor_source(e);
		set_anchor_source(e,gdt::gdtpoint(p.xcoord()/k,p.ycoord()/k));
		p = anchor_target(e);
		set_anchor_target(e,gdt::gdtpoint(p.xcoord()/k,p.ycoord()/k));
		p = label_position (e);
		set_label_position (e,gdt::gdtpoint((p.xcoord()-k)/k,(p.ycoord()-k)/k));
		}
	
	}
	

	// Defaults

	void
	draw_undi_graph::
set_draw_defaults
	(struct_draw_info dd)
	{
	*draw_defs = dd;
	}


	void
	draw_undi_graph::
set_node_defaults
	(struct_node_defaults nd)
	{
	*node_defs = nd;
	}


	void
	draw_undi_graph::
set_edge_defaults
	(struct_edge_defaults ed)
	{
	*edge_defs = ed;
	}


	// Miscellaneous


	void
	draw_undi_graph::
set_draw_status
	(struct_draw_info ds)
	{
	*draw_curr = ds;
	}


	void
	draw_undi_graph::
set_node_status
	(gdtnode v, struct_draw_node_info ni)
	{
	(*node_info)[v]=ni;
	}


	void
	draw_undi_graph::
set_edge_status
	(gdtedge e, struct_draw_edge_info ei)
	{
	(*edge_info)[e]=ei;
	}


	void
	draw_undi_graph::
set_node_status_preserving_positions
	(gdtnode v, struct_draw_node_info ni)
	{
	gdt::gdtpoint tmp1 = (*node_info)[v].center;
	gdt::gdtpoint tmp2 = (*node_info)[v].label_pos;
	(*node_info)[v]=ni;
	(*node_info)[v].center    = tmp1;
	(*node_info)[v].label_pos = tmp2;
	}


	void
	draw_undi_graph::
set_edge_status_preserving_positions
	(gdtedge e, struct_draw_edge_info ei)
	{
	gdt::gdtlist<gdt::gdtpoint> tmp1 = (*edge_info)[e].bends;
	gdt::gdtpoint       tmp2 = (*edge_info)[e].label_pos;
	(*edge_info)[e]=ei;
	(*edge_info)[e].bends     = tmp1;
	(*edge_info)[e].label_pos = tmp2;
	}


	void
	draw_undi_graph::
update_label_positions_after_movement_of_node
	(gdtnode v)
	{
	gdtedge e;
	set_label_position (v,recommended_label_position(v));
	//Modified by A.M.
	forall_edges_adjacent_node(e,v,*this)
		set_label_position (e,recommended_label_position(e));
	}


	void
	draw_undi_graph::
update_label_position_after_movement_of_bend_along_edge
	(gdtedge e)
	{
	set_label_position (e,recommended_label_position(e));
	}


	void
	draw_undi_graph::
update_node_and_bend_positions_according_to
	(dime_orth_plan_undi_graph& dopug)
	{
	local_init(dopug,false,false);
	}


	void
	draw_undi_graph::
update_node_and_bend_positions_according_to
	(draw_undi_graph& dug)
	{
	gdtedge e,d;
	gdtnode v1,v2,w1,w2;
	forall_edges (e,*this)
		{
		v1 = source(e);
		v2 = target(e);
		
		d  = dug.corresponding_edge (e, *this);
		w1 = dug.corresponding_node (v1,*this);
		w2 = dug.corresponding_node (v2,*this);
		set_center (v1,dug.center(w1));
		set_center (v2,dug.center(w2));
		set_label_position (v1,recommended_label_position(v1));
		set_label_position (v2,recommended_label_position(v2));

		gdt::gdtlist<gdt::gdtpoint> dug_bends = dug.bends(d,w1);
		set_bends (e,v1,dug_bends);
		
		if (w1 == dug.source(d)) 
			{
			set_anchor_source (e,dug.anchor_source(d));
			set_anchor_target (e,dug.anchor_target(d));
			}
		else
			{
			set_anchor_source (e,dug.anchor_target(d));
			set_anchor_target (e,dug.anchor_source(d));
			}	
		
		set_label_position (e,recommended_label_position(e));
		}
	}

	// Added by Sandra Follaro (October 1997)
	//
	void
	draw_undi_graph::
set_dummy_nodes_and_edges_status 
	()
	{
	gdtedge e;
	gdtnode v;
	gdt::gdtlist<gdtedge> dummy_edges;
	gdt::gdtlist<gdtnode> dummy_nodes;
	gdt::gdtlist<gdtnode> bend_nodes;
	
	// Find and set the status of all edges and nodes added
	// by rectangularization
	//
        dummy_edges = find_edges_marked_as(RM3_ADDED_BY_RECTANGULARIZATION);
	dummy_nodes = find_nodes_marked_as(RM3_ADDED_BY_RECTANGULARIZATION);
	bend_nodes  = find_nodes_marked_as(RM3_REPLACES_A_BEND);

	forall (e, dummy_edges) 
		{
		apply_defaults(e);
		set_body_color(e, red);
		}
	//forall (e, dummy_edges) set_body_color(e, green);
	forall (v, dummy_nodes)
		{
		apply_defaults(v);
		set_shape(v, TRIANGLE);
		}
	forall (v, bend_nodes)
		{
		apply_defaults(v);
		set_shape(v, RECTANGLE);
		}

	}	

	
	// Topology


	void
	draw_undi_graph::
clear 
	()
	{
	undi_graph::clear();
	local_renew ();
	}

	
	gdtnode
	draw_undi_graph:: 	
new_node 
	(gdt::gdtpoint p, int new_id)
	{
	gdtnode v = undi_graph::new_node(new_id);
	set_center (v,p);
	apply_defaults (v);
	return v;
	}

	
	gdtedge
	draw_undi_graph::
new_edge
	(gdtnode v1, gdtnode v2, gdt::gdtlist<gdt::gdtpoint> bends, int new_id)
	{
	gdtedge e = undi_graph::new_edge(v1,v2,new_id);
	set_bends (e,v1,bends);
	apply_defaults (e);
	return e;
	}

	
	gdtedge
	draw_undi_graph::
new_edge
	(gdtnode v1, gdtedge e1, gdtnode v2, gdt::gdtlist<gdt::gdtpoint> bends, int new_id, int dir)
	{
	gdtedge e = undi_graph::new_edge(v1,e1,v2,new_id,dir);
	set_bends (e,v1,bends);
	apply_defaults (e);
	return e;
	}
	
	
	gdtedge
	draw_undi_graph::
new_edge
	(gdtnode v1, gdtedge e1, gdtnode v2, gdtedge e2, gdt::gdtlist<gdt::gdtpoint> bends, int new_id, int dir)
	{
	gdtedge e = undi_graph::new_edge(v1,e1,v2,e2,new_id,dir);
	set_bends (e,v1,bends);
	apply_defaults (e);
	return e;
	}
	
	
	void
	draw_undi_graph::
new_bend
	(gdtedge e, gdtnode v, int i, gdt::gdtpoint p)
	{
	struct_draw_edge_info& ei=(*edge_info)[e];
	gdt::gdtpoint q;
	int n;
	
	n = ei.bends.length();
	if (n == 0) 
		{
		ei.bends.append(p);
		//cout << "\nappended bend on point (" << p.xcoord() << "," << p.ycoord() << ")" << flush;
		}
	else	
		{
		if (i>n) i=n;
		if (i<0) i=0;
		if (v == target(e)) i=n-i;
		else if (v != source(e)) gdt_error("illegal gdtnode or gdtedge");
		
		if   (i==0) ei.bends.push(p);
		else 
			{
			gdt::list_item prev = ei.bends.get_item(i-1);
			ei.bends.insert(p,prev);
			//cout << "\ninserted bend on point (" << p.xcoord() << "," << p.ycoord() << ")" << flush;
			}
		}
	//cout << "\nbends list is now the following: " << flush;
	//ei.bends.print(); 
	}

	

	gdtnode
	draw_undi_graph::
replace_cross_with_node (gdtedge e1, gdtedge e2, gdt::gdtpoint p)
	{
	// -------------------------------------
	//              O u1
	//         u2   |
	//         O    |    e1_u1    	 O v2
	//         |    o-------o	 |
	//         |            |	 | e2_v2
	//         o----------- X -------o
	//             e2_u2    | p 
	//                      |
	//                      o----------O v1
	//                          e1_v1     
	//
	// -------------------------------------
	
	double round = 0.01;

	gdtnode vp;	// gdtnode to return
	gdtedge e1_u1,
	     e1_v1,
	     e2_u2,
	     e2_v2;
	
	int old_e1_id;
	int old_e2_id;
	bool point_found = false;


	gdt::gdtpoint ps,pt;
	gdt::list_item ps_it, pt_it;

	gdtnode u1 = source (e1);
	gdtnode v1 = target (e1);
	gdtnode u2 = source (e2);
	gdtnode v2 = target (e2);
	
	gdt::gdtpoint cu1 = center (u1);
	gdt::gdtpoint cv1 = center (v1);
	gdt::gdtpoint cu2 = center (u2);
	gdt::gdtpoint cv2 = center (v2);
	
	gdt::gdtlist<gdt::gdtpoint> bl_u1;	// bend list u1 --> p
	gdt::gdtlist<gdt::gdtpoint> bl_v1;	// bend list p  --> v1
	gdt::gdtlist<gdt::gdtpoint> bl_u2;	// bend list u2 --> p
	gdt::gdtlist<gdt::gdtpoint> bl_v2;	// bend list p  --> v2
	gdt::gdtlist<gdt::gdtpoint> bl;
	
	// --------------------- 1 -----------------------
	// build the bend lists bl_u1, bl_v1, bl_u2, bl_v2 
	// -----------------------------------------------
	
	bl = bends (e1,u1);
	
	//cout << "\nLIST OF e1 BENDS: " << flush;
	//forall (ps,bl) cout << " (" << ps.xcoord() << "," << ps.ycoord() << ")" << flush;


	ps    = cu1;
	pt_it = bl.first (); 
	
	while (pt_it && !point_found)
		{
		pt = bl.inf(pt_it);
		gdt::gdtsegment s(ps,pt);
		//cout << "\nDISTANCE SEGMENT POINT = " << s.distance(p) << flush;
		 
		if (s.distance(p) < round) //point detected
			{ 
			// split bl at item pt_it
			//
			bl.split (pt_it,bl_u1,bl_v1);
			//cout << "\nSPLITTED EDGE e1" << flush;
			point_found = true;
			}
		else
			{
			ps = pt;
			pt_it = bl.succ (pt_it);
			}
		}  
	
	if (!point_found) //last gdtedge-segment
		{
		pt = cv1;
		gdt::gdtsegment s(ps,pt);
		//cout << "\nDISTANCE SEGMENT POINT = " << s.distance(p) << flush;

		if (s.distance(p) < round)
			{
			bl_u1=bl;
			bl_v1.clear();
			//cout << "\nSPLITTED EDGE e1" << flush;
			}
	 	}
	 
	ps_it = bl_v1.first();
	if (ps_it) 
		{
		ps = bl_v1.inf(ps_it);
		if (ps.xcoord() == p.xcoord() && ps.ycoord() == p.ycoord()) 
			{
			//case p on bend
			//cout << "\nremoved bend in e1" << flush;
			bl_v1.del_item(ps_it); 
			}
		}	 	 
	 	
	 	
	//cout << "\nLIST bl_u1 BENDS: " << flush;
	//forall (ps,bl_u1) cout << " (" << ps.xcoord() << "," << ps.ycoord() << ")" << flush;
 	//cout << "\nLIST bl_v1 BENDS: " << flush;
 	//forall (ps,bl_v1) cout << " (" << ps.xcoord() << "," << ps.ycoord() << ")" << flush;


	point_found = false;
	bl.clear();
	bl = bends (e2,u2);
	
	//cout << "\nLIST OF e2 BENDS: " << flush;
	//forall (ps,bl) cout << " (" << ps.xcoord() << "," << ps.ycoord() << ")" << flush;
	
	ps    = cu2;
	pt_it = bl.first (); 
	
	while (pt_it && !point_found)
		{
		pt = bl.inf(pt_it);
		gdt::gdtsegment s(ps,pt);
		//cout << "\nDISTANCE SEGMENT POINT = " << s.distance(p) << flush;

		if (s.distance(p) < round) // point detected
			{
			// split bl at item pt_it
			//
			bl.split (pt_it,bl_u2,bl_v2);
			//cout << "\nSPLITTED EDGE e2" << flush;
			point_found = true;
			}
		else
			{
			ps = pt;
			pt_it = bl.succ (pt_it);
			}
		} 
		
		 
	
	if (!point_found) //last gdtedge-segment
		{
		pt = cv2;
		gdt::gdtsegment s(ps,pt);
		//cout << "\nDISTANCE SEGMENT POINT = " << s.distance(p) << flush;
		
		if (s.distance(p) < round)
			{
			bl_u2=bl;
			bl_v2.clear();
			//cout << "\nSPLITTED EDGE e2" << flush;
			}
	 	}	
	

	ps_it = bl_v2.first();
	if (ps_it)  
		{
		ps = bl_v2.inf(ps_it);
		if (ps.xcoord() == p.xcoord() && ps.ycoord() == p.ycoord()) 
			{
			//case p on bend
			//cout << "\nremove bend on e2" << flush;
			bl_v2.del_item(ps_it); 
			}
		}
			 	 

	//cout << "\nLIST bl_u2 BENDS: " << flush;
	//forall (ps,bl_u2) cout << " (" << ps.xcoord() << "," << ps.ycoord() << ")" << flush;
 	//cout << "\nLIST bl_v2 BENDS: " << flush;
 	//forall (ps,bl_v2) cout << " (" << ps.xcoord() << "," << ps.ycoord() << ")" << flush;
 	
 	
	// --------------------- 2 -----------------------
	// create a new gdtnode, by splitting the edges e1,e2
	// and update directions, markers and constraints
	// -----------------------------------------------
	
	old_e1_id = id(e1);
	old_e2_id = id(e2);
	
	vp    = new_node (p);
	e1_u1 = new_edge (u1,vp,bl_u1); mark(e1_u1, markers(e1));  
	e2_u2 = new_edge (u2,vp,bl_u2); mark(e2_u2, markers(e2));
	e1_v1 = new_edge (vp,v1,bl_v1); mark(e1_v1, markers(e1)); 
	e2_v2 = new_edge (vp,v2,bl_v2); mark(e2_v2, markers(e2)); 
	
	update_constraints_after_split_edge (e1,e1_u1,e1_v1);
	update_constraints_after_split_edge (e2,e2_u2,e2_v2);
	
	if      (start(e1) == u1) {make_directed (e1_u1, u1); make_directed (e1_v1, vp);}
	else if (start(e1) == v1) {make_directed (e1_u1, vp); make_directed (e1_v1, v1);}
	if      (start(e2) == u2) {make_directed (e2_u2, u2); make_directed (e2_v2, vp);}
	else if (start(e2) == v2) {make_directed (e2_u2, vp); make_directed (e2_v2, v2);}
	
	apply_defaults (e1_u1);
	apply_defaults (e2_u2);

	set_anchor_source (e1_u1, anchor_source(e1));
	set_anchor_target (e1_v1, anchor_target(e1));
	set_anchor_source (e2_u2, anchor_source(e2));
	set_anchor_target (e2_v2, anchor_target(e2));

	del_edge (e1);	
	del_edge (e2);
	//Added by Marcandalli 8 2001
	edge_info->undefine(e1);
	edge_info->undefine(e2);
	//
	assign_id (e1_u1, old_e1_id);
	assign_id (e2_u2, old_e2_id);

	update_embedding_after_node_move (vp); // accord the embedding to the drawing informations 	
	mark (vp,RM3_CROSS_ON_REAL_EDGE);      // mark the gdtnode vp as a dummy cross-gdtnode
	
	return vp;
	}
	


	gdt::gdtlist<gdtnode>
	draw_undi_graph::
replace_crosses_with_nodes
	()
	{
	// this is a first non efficient version of function.
	// Its cost is O(|E|^2 + c*|E|) where c = number of crosses
	//
	//cout << "\nStart replacing crosses with nodes" << flush;
	
	gdt::gdtlist<gdtnode> dummy_nodes;
	gdtedge e1,e2;
	gdtedge e = NULL_EDGE;
	gdt::gdtpoint p;
	gdt::gdtlist<gdt::gdtpoint> C; //all crosses of the graph
	
	// -------------- 1 --------------
	// detect all crosses in the graph
	// -------------------------------

	C = crosses();
	
	//cout << "\nCROSSES LIST : " << flush;
	//forall (p,C) cout << " (" << p.xcoord() << "," << p.ycoord() << ")" << flush;
	

	// --------------- 2 --------------
	// Scan all points and replace them
	// with dummy nodes
	// --------------------------------
	
	forall (p,C)
		{
		//cout << "\nreplacing cross (" << p.xcoord() << "," << p.ycoord() << ")" << flush;

		// ------------- 2.1 -------------
		// look for two edges containing p
		// -------------------------------

		e1 = NULL_EDGE;
		e2 = NULL_EDGE;
		
		forall (e, all_edges())
			if (edge_contains_point (e,p))
				if (!e1) e1 = e;
				else    {e2 = e; break;}
		
		//cout << "\nfound edges e1 = " << id(e1) << " and e2 = " << id(e2) << flush;
		
		// ------------- 2.2 -------------
		// replace p with a dummy gdtnode
		// -------------------------------

		gdtnode vp = replace_cross_with_node (e1,e2,p);
		dummy_nodes.append(vp);
		}

	return dummy_nodes;
	}	
	

	
	void
	draw_undi_graph::
remove_cross_node
	(gdtnode v)
	{
	if (!is_marked(v,RM3_CROSS_ON_REAL_EDGE))
		gdt_error("invalid cross-gdtnode");

	double round = 0.01;
	double angle,diff;
	gdt::gdtpoint p1,p2,p3,p4;
	gdt::gdtpoint p = center(v);

	
	gdtedge e1 = first_adj_edge  (v);     gdtnode v1 = opposite (v,e1);
	gdtedge e3 = cyclic_adj_succ (e1,v);  gdtnode v3 = opposite (v,e3);
	gdtedge e2 = cyclic_adj_succ (e3,v);  gdtnode v2 = opposite (v,e2);
	gdtedge e4 = cyclic_adj_succ (e2,v);  gdtnode v4 = opposite (v,e4);
	
	gdt::gdtlist<gdt::gdtpoint> b_v1_v = bends (e1,v1);
	gdt::gdtlist<gdt::gdtpoint> b_v_v2 = bends (e2,v) ;
	gdt::gdtlist<gdt::gdtpoint> b_v3_v = bends (e3,v3);
	gdt::gdtlist<gdt::gdtpoint> b_v_v4 = bends (e4,v) ;
	
	// ------------------------------------------
	// check the slope of the segments incident v
	// in order to decide if adding a bend
	// ------------------------------------------
	//
	if (b_v1_v.empty()) p1 = anchor_point(e1,v1);
	else		    p1 = b_v1_v.tail();
	if (b_v_v2.empty()) p2 = anchor_point(e2,v2);
	else		    p2 = b_v_v2.head();
	if (b_v3_v.empty()) p3 = anchor_point(e3,v3);
	else		    p3 = b_v3_v.tail();
	if (b_v_v4.empty()) p4 = anchor_point(e4,v4);
	else		    p4 = b_v_v4.head();
	
	gdt::gdtpoint p1v = anchor_point(e1,v),
	      p2v = anchor_point(e2,v),
	      p3v = anchor_point(e3,v),
	      p4v = anchor_point(e4,v);

	gdt::gdtsegment s1 (p,p1),
		s2 (p,p2),
		s3 (p,p3),
		s4 (p,p4);
	angle = max(s1.direction()-s2.direction(), s2.direction()-s1.direction());
	diff  = max(angle-PI, PI-angle);
	if (p.distance(p1v)) b_v1_v.append (p1v);
	if (p.distance(p2v)) b_v_v2.push   (p2v);
	if (diff > round)    b_v1_v.append (p);

	angle = max(s3.direction()-s4.direction(), s4.direction()-s3.direction());
	diff  = max(angle-PI, PI-angle);
	if (p.distance(p3v)) b_v3_v.append (p3v);
	if (p.distance(p4v)) b_v_v4.push   (p4v);
	if (diff > round)    b_v3_v.append (p);

	// ------------------------------------------- 

	b_v1_v.conc (b_v_v2);
	b_v3_v.conc (b_v_v4);
	
	gdt::gdtlist<gdt::gdtpoint> b_v1_v2 = b_v1_v;
	gdt::gdtlist<gdt::gdtpoint> b_v3_v4 = b_v3_v;
	
	int old_id_12 = min(id(e1),id(e2));
	int old_id_34 = min(id(e3),id(e4));
	
	gdtedge e12 = new_edge (v1,e1,v2,e2,b_v1_v2);  // v1 will be the source of e12
	gdtedge e34 = new_edge (v3,e3,v4,e4,b_v3_v4);  // v3 will be the target of e34 
	
	apply_defaults (e12);
	apply_defaults (e34);

	if (v1 == source(e1)) set_anchor_source (e12,anchor_source(e1));
	else		      set_anchor_source (e12,anchor_target(e1));
	if (v2 == target(e2)) set_anchor_target (e12,anchor_target(e2));
	else		      set_anchor_target (e12,anchor_source(e2));
	if (v3 == source(e3)) set_anchor_source (e34,anchor_source(e3));
	else		      set_anchor_source (e34,anchor_target(e3));
	if (v4 == target(e4)) set_anchor_target (e34,anchor_target(e4));
	else		      set_anchor_target (e34,anchor_source(e4));
	
	// --------------------------------------------------------
	// update directions, markers and constraints for new edges
	// --------------------------------------------------------
	
	mark(e12, markers (e1));
	mark(e34, markers (e3));
	
	update_constraints_after_merge_edges (e1,e2,e12);
	update_constraints_after_merge_edges (e3,e4,e34);
	
	if      (start(e1) == v1) make_directed (e12,v1);
	else if (start(e1) == v ) make_directed (e12,v2);
	if      (start(e3) == v3) make_directed (e34,v3);
	else if (start(e3) == v ) make_directed (e34,v4);
	
	del_node (v);

	assign_id (e12,old_id_12);
	assign_id (e34,old_id_34);
	}
	

	
	void
	draw_undi_graph::
remove_cross_nodes
	(gdt::gdtlist<gdtnode> nl)
	{
	gdtnode v;
	forall (v,nl) remove_cross_node (v);
	}


	std::set<gdt::gdtpoint,ltpt>
	draw_undi_graph::
remove_cross_nodes
	()
	{
	gdt::gdtlist<gdtnode> nl = find_nodes_marked_as (RM3_CROSS_ON_REAL_EDGE);
	remove_cross_nodes (nl);
	gdtnode v;
	std::set<gdt::gdtpoint,ltpt> P;
	forall (v, nl)
		{
		gdt::gdtpoint p = center(v);
		P.insert(p);
		}
	return P;
	}


	gdtedge
	draw_undi_graph::
remove_two_degree_node
	(gdtnode v, gdt::gdtlist<marker_type>* Lp)
	{
	if (degree(v) != 2)
		gdt_error("invalid gdtnode");
	
	double round = 0.01;
	double angle,diff;
	gdt::gdtpoint p1,p2;
	gdt::gdtpoint p = center(v);
	
	gdtedge e1 = first_adj_edge (v);
	gdtedge e2 = last_adj_edge  (v);
	gdtnode v1 = opposite (v,e1);
	gdtnode v2 = opposite (v,e2);
		
	gdt::gdtlist<gdt::gdtpoint> b_v1_v = bends (e1,v1);
	gdt::gdtlist<gdt::gdtpoint> b_v_v2 = bends (e2,v) ;
	
	// ------------------------------------------
	// check the slope of the segments incident v
	// in order to decide if adding a bend
	// ------------------------------------------
	//
	if (b_v1_v.empty()) p1 = anchor_point(e1,v1);
	else		    p1 = b_v1_v.tail();
	if (b_v_v2.empty()) p2 = anchor_point(e2,v2);
	else		    p2 = b_v_v2.head();

	gdt::gdtpoint p1v = anchor_point(e1,v),
	      p2v = anchor_point(e2,v);
	
	gdt::gdtsegment s1 (p,p1),
		s2 (p,p2); 

	angle = max(s1.direction()-s2.direction(), s2.direction()-s1.direction());
	diff  = max(angle-PI, PI-angle);
	if (p.distance(p1v)) b_v1_v.append (p1v);
	if (p.distance(p2v)) b_v_v2.push   (p2v);
	if (diff > round)    b_v1_v.append (p);
	
	// ------------------------------------------- 	

	b_v1_v.conc (b_v_v2);
	gdt::gdtlist<gdt::gdtpoint> b_v1_v2 = b_v1_v;
	int old_id_12 = min(id(e1),id(e2));

	gdtedge e12 = new_edge (v1,e1,v2,e2,b_v1_v2);   //v1 will be the source of e12

	apply_defaults (e12);
	
	if (v1 == source(e1)) set_anchor_source (e12,anchor_source(e1));
	else		      set_anchor_source (e12,anchor_target(e1));
	if (v2 == target(e2)) set_anchor_target (e12,anchor_target(e2));
	else		      set_anchor_target (e12,anchor_source(e2));		
	
	// ----------------------------------------------------------
	// update all directions, markers and constraints on new gdtedge
	// ----------------------------------------------------------
	
	mark(e12, markers (e1));
	update_constraints_after_merge_edges (e1,e2,e12);
	
	// ----------------------------------------------------
	// direction of e12 is assigned as follows:
	// if either v1 or v2 has not any marker in *Lp, 
	// choose the same direction as e1 or e2, respectively.
	// Else, choose the e1 or e2 direction if there is any. 
	// OBSERVE that direction can be ->, <- or --
	// ----------------------------------------------------

	marker_type m;
	bool  v1_is_marked = false;
	bool e12_directed = false;
	
	if (Lp)
    	   forall (m,*Lp) if (is_marked(v1,m)) { v1_is_marked = true; break; } 

	if (!v1_is_marked) // choose for e12 the e1 direction
	    	{
	    	if      (start(e1) == v1) { make_directed (e12,v1); e12_directed = true; }
	    	else if (stop (e1) == v1) { make_directed (e12,v2); e12_directed = true; }
	    	}
		
	if (!e12_directed) // choose for e12 the e2 direction
	    	{
	    	if 	(start(e2) == v2) make_directed (e12,v2);
	    	else if (stop (e2) == v2) make_directed (e12,v1);
	    	} 

	del_node (v);
	assign_id (e12,old_id_12);
	
	return e12;
	}



	// ----------------------
	// Input / output methods
	// ----------------------


	// Draw
    // Removed by Pier Francesco Cortese   Jan 2004

	/*
	void
	draw_undi_graph::
draw
	(window& W, bool curve, int ref1, double ref2)
	{
	gdtnode v;
	gdtedge e;
	W.clear();
	if (draw_curr->shadows_are_visible)
		{
		forall_edges (e,*this) draw_shadow (e,W,curve,ref1,ref2);
		forall_nodes (v,*this) draw_shadow (v,W);
		}
	forall_nodes (v,*this) draw (v,W);
	forall_edges (e,*this) draw (e,W,curve,ref1,ref2);
	}


	void
	draw_undi_graph::
draw
	(gdtnode v, window& W)
	{
	node_shape_type st = shape (v);
	gdt::gdtpoint	   c  = center (v);
	double     dx = width (v);
	double	   dy = height (v);
	color      ic = body_color (v);
	color	   bc = border_color (v);
	color      lc = label_color (v);
	std::string     l  = label (v);
	gdt::gdtpoint	   lp = label_position (v);

	switch(st)
		{
		case ELLIPSE:
			{
			W.draw_filled_ellipse (c,dx/2,dy/2,ic);
			W.draw_ellipse 	      (c,dx/2,dy/2,bc);
			break;
			}

		case RECTANGLE:
			{
			double x0 = c.xcoord();
			double y0 = c.ycoord();
			gdt::gdtpoint  p1 (x0-dx/2,y0-dy/2);	// vertex lower-left
			gdt::gdtpoint  p2 (x0+dx/2,y0+dy/2);	// vertex upper-rigth
			gdt::gdtpoint  p3 (x0-dx/2,y0+dy/2);
			gdt::gdtpoint  p4 (x0+dx/2,y0-dy/2);
			gdt::gdtlist<gdt::gdtpoint> P;
			P.append(p1);
			P.append(p3);
			P.append(p2);
			P.append(p4);

			//old code (to be restored)
			//W.draw_filled_polygon (P,ic);

			//new code (to be removed)
			list<gdt::gdtpoint> P_leda;
			P.copy_to_leda_list(P_leda);
			W.draw_filled_polygon (P_leda,ic);
			//
			W.draw_rectangle (p1,p2,bc);
			break;
			}

		case TRIANGLE:
			{
			double x0 = c.xcoord();
			double y0 = c.ycoord();
			gdt::gdtpoint  p1 (x0-dx/2,y0-dy/2);
			gdt::gdtpoint  p2 (x0+dx/2,y0-dy/2);
			gdt::gdtpoint  p3 (x0,y0+dy/2);
			W.draw_filled_triangle (p1,p2,p3,ic);
			W.draw_triangle        (p1,p2,p3,bc);
			break;
			}
		}
	if (draw_curr->node_labels_are_visible)
		W.draw_ctext (lp,l,lc);
	}


	void
	draw_undi_graph::
draw
	(gdtedge e, window& W, bool curve, int ref1, double ref2)
	{
	gdtnode v1;
	gdtnode v2;
	if (edge_is_directed(e)) v1 = start(e);
 	else			 v1 = source(e);
	v2 = opposite(v1,e);

	gdt::gdtlist<gdt::gdtpoint> bl  = bends (e,v1);
	color	    ec  = body_color (e);
	std::string	    le  = label (e);
	gdt::gdtpoint	    lp  = label_position (e);
	gdt::gdtpoint	    ap1 = anchor_point (e,v1),
		    ap2 = anchor_point (e,v2);
	color	    lc  = label_color (e);

	if (!node_contains_point(v1,ap1)) bl.push(ap1);
	if (!node_contains_point(v2,ap2)) bl.append(ap2);

	gdt::gdtpoint p;
	gdt::gdtpoint p1 = intersection_edge_node (e,v1);
	gdt::gdtpoint p2 = intersection_edge_node (e,v2);

	if (!curve || bl.empty())		// Polyline
		{
		forall (p,bl)
	 		{
			W.draw_segment (p1,p,ec);
			draw_bend (W,p,ec);
			p1 = p;
			}

		if (edge_is_directed(e)) W.draw_arrow   (p1,p2,ec);
		else			 W.draw_segment (p1,p2,ec);
		}
	else					// Curves
		{
		gdt::gdtlist<gdt::gdtpoint> fragmentation;
		bl.push(p1);
		bl.append(p2);
		fragmentation = fragment_polyline (bl,scale()*ref2);

		//old code (to be restored)
		//if (edge_is_directed(e)) W.draw_bezier_arrow   (fragmentation,ref1,ec);
		//else			 W.draw_bezier 	       (fragmentation,ref1,ec);

		//new code (to be removed)
		list<gdt::gdtpoint> F;
		fragmentation.copy_to_leda_list(F);
		if (edge_is_directed(e)) W.draw_bezier_arrow   (F,ref1,ec);
		else			 W.draw_bezier 	       (F,ref1,ec);
		//

		}

	if (draw_curr->edge_labels_are_visible) W.draw_ctext (lp,le,lc);
	}


	void
	draw_undi_graph::
draw_around
	(gdtnode v, window& W)
	{
	gdtedge e;
	//Modified by A.M.
	forall_edges_adjacent_node(e,v,*this) draw(e,W);
	draw (v,W);
	}

*/

	bool
	draw_undi_graph::
draw
	(std::string file_name)
	{
	#ifdef NSAVE
	 cout << "\nTHIS IS A DEMO VERSION OF GDT."
	      << "\nYOU CANNOT SAVE DATA!"
	      << "\nTO SAVE DATA YOU NEED A COMMERCIAL LICENSE" << flush;
	 exit (1);
	#endif

	gdtnode v;
	gdtedge e;

	// -------------------------------------------------
	// open a text file for output with name "file_name"
	// -------------------------------------------------

	ofstream out(file_name.c_str());
	if (!out) return false;

	// ---------------------------------------------------------------
	// set all gdtnode shapes as RECTANGLE, storing the previous shapes
	// in a mapping. Also, each gdtnode is printed on file
	// ---------------------------------------------------------------

	gdt::gdtnode_map<node_shape_type> old_shape (*this);

	out << "<NODELIST>\n";

	forall_nodes (v,*this)
		{
		old_shape[v] = shape(v);
		set_shape(v,RECTANGLE);

		int 	idv = id(v);
		gdt::gdtpoint   cv  = center(v);
		double  wv  = width(v),
			hv  = height(v);

		out << "\t<NODE>  ";
		out << idv << "\t" << cv << "\t\t" << wv << "\t\t" << hv << "\t\t";
		out << "</NODE>\n";
		}

	out << "</NODELIST>\n\n";


	// ----------------------------
	// Each gdtedge is printed on file
	// ----------------------------

	out << "<EDGELIST>\n";

	forall_edges (e,*this)
		{
		int ide = id(e);
		out << "\t<EDGE>  ";
		out << ide << "\t";

		gdtnode v1 = source(e);
		gdtnode v2 = target(e);

		out << "<DIRECTION> ";
		if 	(v1 == start(e)) out << "->   ";
		else if (v2 == start(e)) out << "<-   ";
		else			 out << "--   ";

		gdt::gdtlist<gdt::gdtpoint> bl  = bends (e,v1);
		gdt::gdtpoint	    ap1 = anchor_point (e,v1),
			    ap2 = anchor_point (e,v2);

		if (!node_contains_point(v1,ap1)) bl.push  (ap1);
		if (!node_contains_point(v2,ap2)) bl.append(ap2);

		gdt::gdtpoint p1 = intersection_edge_node (e,v1); bl.push  (p1);
		gdt::gdtpoint p2 = intersection_edge_node (e,v2); bl.append(p2);

		gdt::gdtpoint p;

		forall(p,bl) out << "<BEND> " << p << "   ";

		out << "\n\t</EDGE>\n";
		}

	out << "</EDGELIST>";

	// ---------------------------------------------------
	// close the file and restore the original gdtnode shapes
	// ---------------------------------------------------

	out.close();
	forall_nodes (v,*this) set_shape(v,old_shape[v]);
	return true;
	}


        

	// File


	bool
	draw_undi_graph::
write
	(std::string file_name)
	{
	// open a text file for output with name "file_name"
	//

	ofstream out (file_name.c_str());
	if (!out) return false;

	// write the undi_graph part, i.e. the adjacent lists
	// forall nodes and direction of their edges

	undi_graph::append_section_to_fstream (out);  // write the undi_graph part
	append_section_to_fstream (out);  	      // write the draw_undi_graph part
	out.close();
	return true;
	}



	bool
	draw_undi_graph::
read
	(std::string file_name)
	{
	// open a text file for input with name "file_name"
	ifstream in (file_name.c_str());
	if (!in) return false;

	undi_graph::read_section_from_fstream (in); // read and build the undi_graph part
	//cout << "Sessione undi_graph letta" << endl;
	read_section_from_fstream (in);	   	    // read and build the draw_undi_graph part
	//cout << "Sessione draw_undi_graph letta" << endl;
	in.close();
	return true;
	}



	void
	draw_undi_graph::
append_section_to_fstream
	(ofstream& out)
	{
	// write the draw_undi_graph section, i.e. the nodes and edges
	// draw informations
	//
	gdtnode v;
	gdtedge e;
	gdt::gdtpoint _point;

	out << "\n<DRAWSECTION>";
	out << "\n\t<NODELIST>";

	forall_nodes (v,*this)
		{
		out << "\n\t\t<NODE>";
		out << "\n\t\t\t";
		//
		out << id(v)                  << " ";
		out << center(v)              << " ";
		out << (unsigned int)shape(v) << " ";
		out << width(v)               << " ";
		out << height(v)              << " ";
		out << altitude(v)            << " ";
		out << (int)body_color(v)          << " ";
		out << (int)border_color(v)        << " ";
		out << (int)label_color(v)         << " ";
        std::string lab_v=replace_all(label(v)," ","&nbsp;");
		out << lab_v << " "; // cout << lab_v << endl;

		///////////////////////////////////////////////////////////////////////////////////////////////////
		//cout << "label_color=" << label_color(v) << endl;

		//cout << "label_position=" << label_position(v) << endl;


		///////////////////////////////////////////////////////////////////////////////////////////////////

		out << label_position(v)      << " ";
		//
		out << "\n\t\t</NODE>";
		}

	out << "\n\t</NODELIST>";
	out << "\n\t<EDGELIST>";

	forall_edges (e,*this)
		{
		out << "\n\t\t<EDGE>";
		out << "\n\t\t\t";
		//
		out << id(e)                       << " ";
		out << id(source(e))               << " ";
		out << (int)body_color(e)               << " ";
		out << (int)label_color(e)              << " ";

		std::string lab_e = replace_all(label(e)," ", "&nbsp;");
		out << lab_e << " ";

		out << label_position(e)           << " ";
		out << (unsigned int)shape(e)      << " ";
		out << anchor_source(e)            << " ";
		out << anchor_target(e)            << " ";
		gdt::gdtlist<gdt::gdtpoint> lb = bends(e);
		forall (_point,lb) out << "\n\t\t\t<BEND> " << _point << " ";
		//
		out << "\n\t\t</EDGE>";
		}
	out << "\n\t</EDGELIST>";
	out << "\n</DRAWSECTION>";
	}



	void
	draw_undi_graph::
read_section_from_fstream
	(ifstream& in)
	{
	// Read the draw_undi_graph section, i.e. the nodes and edges
	// draw informations and build the corresponding *this part.

	gdtnode v;
	gdtedge e;

	int             _int;
	std::string          _tag;
	std::string          _string;
	int             _color;
	double          _double;
	gdt::gdtpoint           _point;
	gdt::gdtlist<gdt::gdtpoint>     _pointlist;
        int             _node_shape;
        int             _edge_shape;
        gdt::gdtpoint		_anch_source,
        		_anch_target;

	local_renew ();

	in >> _tag; if (_tag !="<DRAWSECTION>") gdt_error("DRAWSECTION not found");
	in >> _tag; if (_tag !="<NODELIST>") gdt_error("NODELIST not found");
	in >> _tag; // <NODE> || </NODELIST>
	while (_tag == "<NODE>")
		{
		in >> _int;        v = find_node      (_int);                            //cout << "\n" << _int;
		in >> _point; 	   set_center         (v,_point);                        //cout << "\n" << _point;
		in >> _node_shape; set_shape          (v,(node_shape_type)_node_shape);  //cout << "\n" << _node_shape;
		in >> _double;     set_width          (v,_double);                      // cout << "\n" << _double;
		in >> _double;     set_height         (v,_double);                      // cout << "\n" << _double;
		in >> _double;     set_altitude       (v,_double);                      // cout << "\n" << _double;
		in >> _color;	   set_body_color     (v,(color)_color);              //   cout << "\n" << _color;
		in >> _color;	   set_border_color   (v,(color)_color);              //   cout << "\n" << _color;
		in >> _color;	   set_label_color    (v,(color)_color);               //  cout << "\n" << _color;
		in >> _string;	   set_label          (v,replace_all (_string,"&nbsp;", " "));       // cout << "\n" << _string;
		in >> _point;	   set_label_position (v,_point);                      //  cout << "\n" << _point;
		in >> _tag;                                                              //cout << "\n" << _tag; // </NODE>
		in >> _tag;                                                              //cout << "\n" << _tag; // <NODE> || </NODELIST>
		}
	in >> _tag; if (_tag !="<EDGELIST>") gdt_error("EDGELIST not found");
	in >> _tag;     //  <EDGE> || </EDGELIST>
	while (_tag == "<EDGE>")
		{
		in >> _int;        e = find_edge      (_int);                           // cout << "\n" << _int;
		in >> _int;        v = find_node      (_int);                           // cout << "\n" << _int; // source(e)
		in >> _color;      set_body_color     (e,(color)_color);         //        cout << "\n" << _color;
		in >> _color;      set_label_color    (e,(color)_color);          //       cout << "\n" << _color;
		in >> _string;     set_label          (e,replace_all (_string,"&nbsp;", " "));      //  cout << "\n" << _string;
		in >> _point;      set_label_position (e,_point);                     //   cout << "\n" << _point;
		in >> _edge_shape; set_shape          (e,(edge_shape_type)_edge_shape);  //cout << "\n" << _node_shape;
		in >> _anch_source;
		in >> _anch_target;

		if (source(e) == v) {set_anchor_source  (e,_anch_source); set_anchor_target (e,_anch_target);}
		else		    {set_anchor_source  (e,_anch_target); set_anchor_target (e,_anch_source);}

		in >> _tag;                                                        //      cout << "\n" << _tag; // <BEND> || </EDGE>
		_pointlist.clear();
		while (_tag =="<BEND>")
			{
			in >> _point; _pointlist.append(_point);                 //        cout << "\n" << _point;
			in >> _tag;                                                      //cout << "\n" << _tag; // <BEND> || </EDGE>
			}
		if (_pointlist.size()>0) set_bends (e,v,_pointlist);
		in >> _tag;                                                           //   cout << "\n" << _tag; // <EDGE> || </EDGELIST>
		}
	in >> _tag;                                                                 //     cout << "\n" << _tag << flush; // </DRAWSECTION>
	}



	void
	draw_undi_graph::
export_to_fig_file_format
 	(
	bool spline_edge,
	double refinement,
	double scale_factor,
	ostream& out
	)
	{
	#ifdef NSAVE
	 cout << "\nTHIS IS A DEMO VERSION OF GDT."
	      << "\nYOU CANNOT SAVE DATA!"
	      << "\nTO SAVE DATA YOU NEED A COMMERCIAL LICENSE" << flush;
	 exit (1);
	#endif

	gdtnode  v;
	gdtedge  e;
	gdt::gdtpoint p;
	
	out << "#FIG 3.2";
	out << "\nLandscape";
	out << "\nCenter";
	out << "\nInches";
	out << "\nLetter";
	out << "\n100.00";
	out << "\nSingle";
	out << "\n-2";
	out << "\n1200 2";
	
	// ----------------------------------------------------------------------
	// The xfig's coordinates system has the origin at the upper-left corner.
	// Due to this, we need the 'y_max' coordinates of the drawing, in order 
	// to correctly convert all y-coordinates.
	// ----------------------------------------------------------------------

	double x_min, x_max, 
	       y_min, y_max;
	calc_bounding_box (x_min, x_max, y_min, y_max); 
	
	int y_up = (int) (y_max) + 10;

	// --------------------------------------------
	// this parameter is needed to get a rescale
	// of the drawing in xfig's coordinates-system.
	// --------------------------------------------

	if (scale_factor < 0) scale_factor = 0.00;
	
	int xfig_resc = (int)(100 * scale_factor);
	
	// export the vertices
	//
	forall_nodes (v,*this)
		{
		out << "\n";
		switch (shape(v))
			{
			case ELLIPSE:
				{
				int xfig_resc_dim = (int)(50 * scale_factor);
				
			 	out << "1 "; 				// type of object (ELLIPSE)
			 	out << "3 ";				// ellipse defined by radius 	
			 	out << "0 "; 				// line style
			 	out << "1 "; 				// thickness (1--80 inches)	
			 	out << (int)border_color(v) - 1 << " "; // pen-color
			 	out << (int)body_color(v) + 1 << " ";	// fill-color
			 	out << "100 ";				// depth
			 	out << "0 ";				// pen-style (not used by xfig)
			 	out << "20 ";				// area-fill (-1=not filled 20=100% intensity)
			 	out << "0.000 ";			// style-val
			 	out << "1 ";				// direction (always 1)
			 	out << "0.000 ";			// angle (randians, the angle of the x-asis)
			 
			 	int center_x = (int)(center(v).xcoord() * xfig_resc);
			 	int center_y = (int)((y_up - center(v).ycoord()) * xfig_resc);
			 	int radius_x = (int)(width(v) * xfig_resc_dim);
			 	int radius_y = (int)(height(v) * xfig_resc_dim);
			 	int start_x  = center_x;
			 	int start_y  = center_y;
				int end_x    = start_x + radius_x - 2*xfig_resc_dim;
				int end_y    = start_y + radius_y - 2*xfig_resc_dim;

				out << center_x << " ";
				out << center_y << " ";
				out << radius_x << " ";
				out << radius_y << " ";
				out << start_x << " ";
				out << start_y << " ";
				out << end_x << " ";
				out << end_y << " ";
				}
			break;
			
			case RECTANGLE:
			case TRIANGLE:
				{
				int xfig_resc_dim = (int)(100 * scale_factor);
				
				out << "2 "; 				// type of object (POLYLINE)
				out << "2 ";				// sub-type of object (BOX)
				out << "0 "; 				// line style
				out << "1 "; 				// thickness (1--80 inches)
				out << (int)border_color(v) - 1 << " "; // pen-color
			 	out << (int)body_color(v) + 1 << " ";	// fill-color
			 	out << "100 ";				// depth
			 	out << "0 ";				// pen-style (not used by xfig)
			 	out << "20 ";				// area-fill (-1=not filled 20=100% intensity)					
			 	out << "0.000 ";			// style-val
				out << "0 ";                            // join-style (enumeration type)
				out << "0 "; 				// cap_style  (enumeration type, only used for POLYLINE)
				out << "-1 ";				// radius (1/80 inch, radius of arc-boxes)
				out << "0 ";                            // forward_arrow  (0: off, 1: on)
				out << "0 ";				// backward_arrow (0: off, 1: on)
				
				int center_x = (int)(center(v).xcoord() * xfig_resc);
				int center_y = (int)((y_up - center(v).ycoord()) * xfig_resc);
				int half_width  = (int)(width(v)/2 * xfig_resc_dim);
				int half_height = (int)(height(v)/2 * xfig_resc_dim);
				
				if (shape(v) == RECTANGLE)
					{
					int x1 = center_x - half_width;		// upper-left corner
					int y1 = center_y - half_height;
					int x2 = x1;				// bottom-left corner				
					int y2 = center_y + half_height;
					int x3 = center_x + half_width;		// bottom-right corner
					int y3 = y2;
					int x4 = x3;				// upper-right corner
					int y4 = center_y - half_height;
					int x5 = x1; 
					int y5 = y1;
					out << "5 "; // number of points in line for rectangle
					out << "\n\t " << x1 << " " << y1 << " " << x2 << " " << y2 << " "
				   	               << x3 << " " << y3 << " " << x4 << " " << y4 << " " << x5 << " " << y5;
					}
				else 	
					{
					int x1 = center_x;			// upper corner
					int y1 = center_y - half_height;
					int x2 = center_x - half_width;	        // bottom-left corner				
					int y2 = center_y + half_height;
					int x3 = center_x + half_width;		// bottom-right corner
					int y3 = y2;
					int x4 = x1;				
					int y4 = y1;	 
					out << "4 "; // number of points in line for rectangle
					out << "\n\t " << x1 << " " << y1 << " " << x2 << " " << y2 << " "
				   	               << x3 << " " << y3 << " " << x4 << " " << y4;
					}
					
				}
			break;
			}
		out << flush;
		}
		
	forall_edges (e,*this)
		{
		out << "\n";
		if (!spline_edge)
			{
			out << "2 "; 		// type of object (POLYLINE)
			out << "1 ";		// sub-type of object (POLYLINE)
			}
		else 	
			{
			out << "3 ";		// type of object (SPLINE)
			out << "0 ";		// sub-type of object (opened approximated spline)
			}
		
		out << "0 "; 				// line style
		out << "1 "; 				// thickness (1--80 inches)
		out << (int)body_color(e) - 1 << " ";   // pen-color
		out << (int)body_color(e) - 1 << " ";	// fill-color
		out << "110 ";				// depth
		out << "0 ";				// pen-style (not used by xfig)
		out << "-1 ";				// area-fill (-1=not filled 20=100% intensity)					
		out << "0.000 ";			// style-val
		if (!spline_edge) out << "0 ";          // join-style (enumeration type)
		out << "0 "; 				// cap_style  (enumeration type, only used for POLYLINE)
		if (!spline_edge) out << "-1 ";		// radius (1/80 inch, radius of arc-boxes)

		bool arrow = false;
		
		if (start(e) == source(e)) 
			{
		        out << "1 "; // forward_arrow  (0: off, 1: on)
			out << "0 "; // backward_arrow (0: off, 1: on)
			arrow = true;					
			}
		else if (start(e) == target(e))
			{
		        out << "0 "; // forward_arrow  (0: off, 1: on)
			out << "1 "; // backward_arrow (0: off, 1: on)
			arrow = true;	
			}
		else
			{
		        out << "0 "; // forward_arrow  (0: off, 1: on)
			out << "0 "; // backward_arrow (0: off, 1: on)		
			}
					
		gdt::gdtlist<gdt::gdtpoint> bl = bends(e);

		gdtnode vs = source(e);
		gdtnode vt = target(e);
		
		gdt::gdtpoint aps = anchor_source_point(e);
		gdt::gdtpoint apt = anchor_target_point(e);
		
		if (!node_contains_point(vs,aps)) bl.push  (aps);
		if (!node_contains_point(vt,apt)) bl.append(apt);
		
		gdt::gdtpoint ps = intersection_edge_node (e,vs); bl.push  (ps);
		gdt::gdtpoint pt = intersection_edge_node (e,vt); bl.append(pt);
		
		gdt::gdtlist<gdt::gdtpoint> fragmentation;

		if (!spline_edge) fragmentation = bl;
		else              fragmentation = fragment_polyline (bl,scale()*refinement);
		
		out << fragmentation.size() << " "; // number of points of the line
 		
	        if (arrow) // define the arrow parameters
			{
			out << "\n\t";
			out << "2 ";  			// arrow_type  (enumeration type)
			out << "1 ";			// arrow_style (enumeration type)
			out << "1.00 ";	 		// arrow_thickness (1/80 inch)
			out << "90.00 "; 		// arrow_width (Fig units)
			out << "135.00 ";		// arrow_height(Fig units)
			}
		
		out << "\n\t ";
		gdt::gdtpoint p;
		forall (p,fragmentation) 
			{
			out << (int)(p.xcoord() * xfig_resc) << " ";
			out << (int)((y_up - p.ycoord()) * xfig_resc) << " ";
			}
		
		if (spline_edge)
			{
			// add the shape factor (-1 -- 1) for each control point
			// -1 = interpolated;
			// +1 = approximated;
			//  0 = angular;
			out << "\n\t";
			out << "0.000 ";
			for (int i = 2; i <= fragmentation.size()-1; i++) out << "1 ";
			out << "0.000 ";	
			}
		}
	
	// put labels
	//
	int fs = (int)(1.6*scale()*scale_factor)+2; //W.D. 2 May 2008			// font-size
	
	if (node_labels_are_visible())  
	    forall_nodes (v,*this)
		{
		gdt::gdtpoint label_pos = label_position(v);
		
		out << "\n";
		out << "4 "; 								// type of object (TEXT)
		out << "1 ";								// sub_type of object (0: Left justified
											//     		       1: Center justified
											//     		       2: Right justified)
					
		out << (int)label_color(v) - 1 << " ";					// color (enumeration type)
		out << "95 ";								// depth
		out << "0 ";								// pen-style (enumeration not-used)
		out << "0 ";								// font (enumeration type: 0=default)
		out << fs << " ";							// font size in points (16 <--> scale=10)
		out << "0.0000 ";							// angle (radians, the angle of the text)
		out << "4 "; 								// font_flags (bit vector)
		out << "100 ";								// height (fig units -- fig automatically recompute it)
		out << "100 ";								// length (fig units -- fig automatically recompute it)

		out << (int)(label_position(v).xcoord() * xfig_resc) << " ";
		out << (int)((y_up - label_position(v).ycoord() + (float)fs/160 * scale()) * xfig_resc) << " "; 	// (fig units, coordinate of the origin
					 									//  of the string.  If sub_type = 0, it is
					 									//  the lower left corner of the string.
					 									//  If sub_type = 1, it is the lower
														//  center.  Otherwise it is the lower
														//  right corner of the string).

		out << label(v) << "\\001";									// end of the string
		}
	
	if (edge_labels_are_visible())
	    forall_edges (e,*this)
		{
		gdt::gdtpoint label_pos = label_position(e);
		
		out << "\n";
		out << "4 "; 								// type of object (TEXT)
		out << "1 ";								// sub_type of object (0: Left justified
											//     		       1: Center justified
											//     		       2: Right justified)

		out << (int)label_color(e) - 1 << " ";					// color (enumeration type)
		out << "95 ";								// depth
		out << "0 ";								// pen-style (enumeration not-used)
		out << "0 ";								// font (enumeration type: 0=default)
		out << fs << " ";							// font size in points (16 <--> scale=10)
		out << "0.0000 ";							// angle (radians, the angle of the text)
		out << "4 "; 								// font_flags (bit vector)
		out << "100 ";								// height (fig units -- fig automatically recompute it)
		out << "100 ";								// length (fig units -- fig automatically recompute it)
		
		out << (int)(label_position(e).xcoord() * xfig_resc) << " ";
		out << (int)((y_up - label_position(e).ycoord() + (float)fs/160 * scale()) * xfig_resc) << " "; 	// (fig units, coordinate of the origin
					 									//  of the string.  If sub_type = 0, it is
					 									//  the lower left corner of the string.
					 									//  If sub_type = 1, it is the lower
														//  center.  Otherwise it is the lower
														//  right corner of the string).
											
		out << label(e) << "\\001";						// end of the string
		}
		
	out << flush;
	}



	void
	draw_undi_graph::
export_to_fig_file_format
 	(
	ostream& out
	)
	{
	export_to_fig_file_format (false, 0.4, 1, out);
	}



	void
	draw_undi_graph::
export_to_fig_file_format
 	(
	std::string file_name,
	bool spline_edge,
	float refinement,
	float  scale_factor
	)
	{
	ofstream out (file_name.c_str());
	export_to_fig_file_format (spline_edge, refinement, scale_factor, out);
	out.close();
	}



	void
	draw_undi_graph::
export_to_xml_file_format
	(
	ostream& out
	)
	{
	#ifdef NSAVE
	 cout << "\nTHIS IS A DEMO VERSION OF GDT."
	      << "\nYOU CANNOT SAVE DATA!"
	      << "\nTO SAVE DATA YOU NEED A COMMERCIAL LICENSE" << flush;
	 exit (1);
	#endif

	// ---------------------------------
	// write the DTD of the xml document
	// ---------------------------------
	
	out << "<?xml version=\"1.0\"  encoding=\"UTF-8\" standalone=\"yes\" ?> \n\n";
        out << "<!DOCTYPE Exported_Graph_Drawing \n";
	out << "  [ \n" << flush; 
	out << "  <!ELEMENT Exported_Graph_Drawing (NODELIST, EDGELIST)  > \n";
	out << "  <!ELEMENT NODELIST (NODE+) > \n";
	out << "  <!ELEMENT EDGELIST (EDGE*) > \n";
	out << "  <!ELEMENT NODE EMPTY> \n";
	out << "  <!ELEMENT EDGE (BEND, BEND*, BEND) > \n";
	out << "  <!ELEMENT BEND EMPTY> \n";
   	out << "\n" << flush;
	out << "  <!ATTLIST NODE  id  		CDATA #REQUIRED \n";
 	out << "                  center        CDATA #REQUIRED \n";
	out << "		  width	        CDATA #REQUIRED \n";
	out << "	   	  height	CDATA #REQUIRED > \n";
	out << "  <!ATTLIST EDGE  id  		CDATA 		   	   #REQUIRED \n";
	out << "        	  dir   	(FORWARD|BACKWARD|NOTHING) #REQUIRED > \n";
	out << "  <!ATTLIST BEND  center        CDATA #REQUIRED > \n";
 	out << "  ]\n" << flush;
        out << ">\n\n" << flush;

	// --------------
	// write the data
	// --------------
	
	
	out << "<Exported_Graph_Drawing> \n\n" << flush;
	
	gdtnode v;
	gdtedge e;

	// ---------------------------------------------------------------	
	// set all gdtnode shapes as RECTANGLE, storing the previous shapes
	// in a mapping. Also, each gdtnode is printed on file
	// ---------------------------------------------------------------
	
	gdt::gdtnode_map<node_shape_type> old_shape (*this);
	
	out << "<NODELIST>\n";
	
	forall_nodes (v,*this)
		{
		old_shape[v] = shape(v);
		set_shape(v,RECTANGLE);
		
		int 	idv = id(v);
		gdt::gdtpoint   cv  = center(v);
		double  wv  = width(v),
			hv  = height(v);
		
		out << "\t<NODE";
		out << " id=" << "\"" << idv << "\"";
		out << " center=" << "\"" << cv << "\"";
		out << " width="  << "\"" << wv << "\""; 
		out << " height=" << "\"" << hv << "\"";
		out << " />\n";
		}

	out << "</NODELIST>\n\n";


	// ----------------------------
	// Each gdtedge is printed on file
	// ----------------------------
	
	out << "<EDGELIST>\n"; 
	
	forall_edges (e,*this)
		{
		int ide = id(e);
		out << "\t<EDGE";
		out << " id=" << "\"" << ide << "\"";
		
		gdtnode v1 = source(e);
		gdtnode v2 = target(e);
	
		out << " dir=";
		if 	(v1 == start(e)) out << "\"" << "FORWARD" << "\"";
		else if (v2 == start(e)) out << "\"" << "BACKWARD" << "\"";
		else			 out << "\"" << "NOTHING" << "\"";
	
		out << " >\n" << flush;
		
		gdt::gdtlist<gdt::gdtpoint> bl  = bends (e,v1);
		gdt::gdtpoint	    ap1 = anchor_point (e,v1),
			    ap2 = anchor_point (e,v2);

		if (!node_contains_point(v1,ap1)) bl.push  (ap1);
		if (!node_contains_point(v2,ap2)) bl.append(ap2);
	
		gdt::gdtpoint p1 = intersection_edge_node (e,v1); bl.push  (p1);
		gdt::gdtpoint p2 = intersection_edge_node (e,v2); bl.append(p2);
		
		gdt::gdtpoint p;
		
		forall(p,bl)
			{
			out << "\t\t<BEND";
		 	out << " center=" << "\"" << p << "\" /> \n";
			}
			
		out << "\t</EDGE>\n";
		}
		
	out << "</EDGELIST> \n\n";
	
	out << "</Exported_Graph_Drawing> \n" << flush;
	
	forall_nodes (v,*this) set_shape(v,old_shape[v]);
	}


	void
	draw_undi_graph::
export_to_xml_file_format 
	(
	std::string file_name
	)
	{
	ofstream out (file_name.c_str());
	export_to_xml_file_format (out);
	out.close();
	}

	
	
// Funzioni introdotte da Gabriel Sassone 15/06/2005
// Conversione per il formato di JGraph Builder

	bool
	draw_undi_graph::
export_to_graphml_file_format
	(std::string file_name)
	{
	// export draw_undi_graph to the graphml format
	//

	ofstream out (file_name.c_str());
	if (!out) return false;
	
	gdtnode v;
	gdtedge e;
	gdt::gdtpoint _point;
	double x,y;
	unsigned int s;
	
	
	//    DEFINITION OF XML HEADER
	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	//out << "\n<!-- This file was written by JGraphBuilder 3.1 -->";
	out << "\n<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">";

	//    DEFINITION OF JBUILDER IDENTIFICATOR
	out << "\n\t<key id=\"d0\" for=\"all\" attr.name=\"label\" attr.type=\"string\"/>";

	out << "\n\t<key id=\"d1\" for=\"node\" attr.name=\"node.color\" attr.type=\"double,double,double\">";
	out << "\n\t\t<desc>Default color for nodes, given in R,G,B</desc>";
	out << "\n\t\t<default>204,204,255</default>";
	out << "\n\t</key>";

	out << "\n\t<key id=\"d2\" for=\"edge\" attr.name=\"edge.color\" attr.type=\"double,double,double\">";
	out << "\n\t\t<desc>Default color for edges, given in R,G,B</desc>";
	out << "\n\t\t<default>0,102,153</default>";
	out << "\n\t</key>";

	out << "\n\t<key id=\"d3\" for=\"edge\" attr.name=\"arrow.color\" attr.type=\"double,double,double\">";
	out << "\n\t\t<desc>Default color for arrows, given in R,G,B</desc>";
	out << "\n\t\t<default>0,71,107</default>";
	out << "\n\t</key>";

	out << "\n\t<key id=\"d4\" for=\"node\" attr.name=\"line.color\" attr.type=\"double,double,double\">";
	out << "\n\t\t<desc>Default color for nodes' labels, given in R,G,B</desc>";
	out << "\n\t\t<default>0,0,0</default>";
	out << "\n\t</key>";

	out << "\n\t<key id=\"d5\" for=\"all\" attr.name=\"point\" attr.type=\"double,double\">";
	out << "\n\t\t<desc>Point, given in X,Y</desc>";
	out << "\n\t\t<default>0,0</default>";
	out << "\n\t</key>";

	out << "\n\t<key id=\"d6\" for=\"edge\" attr.name=\"anch1.point\" attr.type=\"double,double\">";
	out << "\n\t\t<desc>Edges' first anchor point, given in X,Y (relative to node's center)</desc>";
	out << "\n\t\t<default>0,0</default>";
	out << "\n\t</key>";

	out << "\n\t<key id=\"d7\" for=\"edge\" attr.name=\"anch2.point\" attr.type=\"double,double\">";
	out << "\n\t\t<desc>Edges' second anchor point, given in X,Y (relative to node's center)</desc>";
	out << "\n\t\t<default>0,0</default>";
	out << "\n\t</key>";

	out << "\n\t<key id=\"d8\" for=\"node\" attr.name=\"size\" attr.type=\"double,double\">";
	out << "\n\t\t<desc>Nodes' default size, given in W,H</desc>";
	out << "\n\t\t<default>2,2</default>";
	out << "\n\t</key>";

	out << "\n\t<key id=\"d9\" for=\"node\" attr.name=\"image\" attr.type=\"string\">";
	out << "\n\t\t<desc>Default nodes' image, this is the path of the image</desc>";
	out << "\n\t</key>";

	out << "\n\t<key id=\"d10\" for=\"node\" attr.name=\"shape\" attr.type=\"string\">";
	out << "\n\t\t<desc>Default nodes' shape, can be: rect2d, rect3d, oval2d, star2d</desc>";
	out << "\n\t\t<default>rect3d</default>";
	out << "\n\t</key>";

	out << "\n\t<key id=\"d11\" for=\"edge\" attr.name=\"weight\" attr.type=\"double\">";
	out << "\n\t\t<default>0</default>";
	out << "\n\t</key>";

	out << "\n\t<key id=\"d12\" for=\"all\" attr.name=\"label.point\" attr.type=\"double,double\">";
	out << "\n\t\t<desc>Label position, given in X,Y</desc>";
	out << "\n\t\t<default>0,0</default>";
	out << "\n\t</key>";
	
	out << "\n\t<graph type=\"2D\" edgedefault=\"directed\">";
	out << "\n\t\t<data key=\"d0\">New Graph</data>";

	forall_nodes (v,*this)
		{
			out << "\n\t\t<node id=\"n" << id(v) << "\" parse.indegree=\"" << degree_in(v) << "\" parse.outdegree=\"" << degree_out(v) << "\">";
			std::string lab_v = label(v);
			if (lab_v != ""){
				out << "\n\t\t\t<data key=\"d0\">" << lab_v << "</data>";
			}
		
		out << "\n\t\t\t<data key=\"d5\">" << center(v).xcoord() << "," << center(v).ycoord() << "</data>";
	    if ((height(v) != 2) || (width(v)!=2))
			out << "\n\t\t\t<data key=\"d8\">" << width(v) << "," << height(v) << "</data>";
		s = (unsigned int)shape(v);
		if (s == 0)
			out << "\n\t\t\t<data key=\"d10\">OVAL2D</data>";
		x = label_position(v).xcoord()-center(v).xcoord();
		y = label_position(v).ycoord()-center(v).ycoord();
		if ((x != 0) && ((y != 0)))
			out << "\n\t\t\t<data key=\"d12\">" << x << "," << y << "</data>";		
		out << "\n\t\t</node>";
		}
	x = 0;
	y = 0;
	forall_edges(e,*this)
		{
			if (!edge_is_directed(e))
				out << "\n\t\t<edge id=\"e" << id(e) << "\" directed=\"false\" source=\"n" << id(source(e)) << "\" target=\"n" << id(target(e)) << "\">";
			else
				out << "\n\t\t<edge id=\"e" << id(e) << "\" source=\"n" << id(source(e)) << "\" target=\"n" << id(target(e)) << "\">";
			std::string lab_e = label(e);
			if (lab_e != ""){
				out << "\n\t\t\t<data key=\"d0\">" << lab_e << "</data>";
			}
			gdt::gdtlist<gdt::gdtpoint> lb = bends(e);
			forall (_point,lb) out << "\n\t\t\t<data key=\"d5\">" << _point.xcoord() << "," << _point.ycoord() << "</data>";
			x = anchor_source(e).xcoord();
			y = anchor_source(e).ycoord();
			if ((x!=0) && (y!=0))
				out << "\n\t\t\t<data key=\"d6\">" << x << "," << y << "</data>";
			x = anchor_target(e).xcoord();
			y = anchor_target(e).ycoord();
			if ((x!=0) && (y!=0))
				out << "\n\t\t\t<data key=\"d7\">" << x << "," << y << "</data>";
			_point = recommended_label_position(e);
			x = label_position(e).xcoord()-_point.xcoord();
			y = label_position(e).ycoord()-_point.ycoord();
			if ((x != 0) && ((y != 0)))			
				out << "\n\t\t\t<data key=\"d12\">"<< x << "," << y << "</data>";
			out << "\n\t\t</edge>";
		}
	out << "\n\t</graph>";
	out << "\n</graphml>";
	out.close();
	return true;
	}
	
	
	bool
	draw_undi_graph::
import_from_graphml_file_format
	(std::string file_name)
	{
	// Control the existence of the file
	ifstream in (file_name.c_str());
	if (!in) {
		cout << "errore nell'apertura del file" << endl;
		return false;
	}
	
	gdtnode v  = NULL_NODE;
	gdtnode v2 = NULL_NODE;
	gdtedge e;
	
	//int  _int;
	int  _id;
	std::string  _tag,_t;
	std::string  _label;
	std::string _key;
	//int  _color;
	//double  _double;
	gdt::gdtpoint  _point;
	gdt::gdtlist<gdt::gdtpoint>  _pointlist;
    	//int    _node_shape;
    	//int    _edge_shape;
    	gdt::gdtpoint _anch_source,_anch_target;
	
	//int  node_count = 0;
	double 	xpoint,ypoint;
	string::size_type location,location2,location3;
	double 	width,height;
	bool directed = false;

	
	local_renew ();
	
	// Begin the control of the ifstream
	in >> _tag;
	while(_tag != "<graph"){	
		in >> _tag;
	}
	// Controls that the <graph tag exists, if negative exit this function
	if (_tag != "<graph"){
		cout << "errore nel trovare il grafo" << endl;
		return false;
	}
	// Continue to control the stream, seeking the part of the nodes
	while(_tag != "<node"){
		in >> _tag;
	}
	
	// At this point the string _tag contains <node for the first time. 
	while (_tag == "<node"){
		_label = "";
		_t = "";
		in >> _tag;
		location = _tag.find("n",0);
		location2 = _tag.size()-location-2;
		_t = _tag.substr(location+1,location2);
		_id=atoi(_t.c_str());		
		in >> _tag;
		in >> _tag;
		in >> _tag;
		in >> _tag;	
		location = _tag.find( ">",0 );
		_key = _tag.substr(0,location);
		// At this point we have a string that represent the key read from the file.
		// The initial key is d0, that can also not be assigned.
		if (_key == "key=\"d0\""){
			location2 = _tag.find("<",0);
			if (location2 == string::npos){
				_label = _tag.substr((location+1),_tag.size()-1);
				while ( location2 == string::npos ){
					in >> _tag;
					_label = _label + " " + _tag;
					location2 = _tag.find("<",0);
				}
				location2 = _label.find("<",0);
				_label = _label.substr(0,location2);
			}
			else{
				_label = _tag.substr((location+1),(location2-location-1));
			}
			
			// Proceed to the next tag
			in >> _tag;
			in >> _tag;
			location = _tag.find( ">",0 );
			_key = _tag.substr(0,location);
		}else{
			_label = _t;
		}
		/* This part is dedicated to the managment of the color, but it needs
		   the format RGB in GDT
		if (_key == "key=\"d1\""){
			cout << "allora c'e' un problema!" << endl;					
		}
		if (_key == "key=\"d4\""){
			cout << "allora c'e' un problema!" << endl;				
		}
		*/
		// Here we take care of the key d5, that is always present in the document
		// because it handles the position of the node
		if (_key == "key=\"d5\""){
			location2 = _tag.find("<",0);
			location3 = _tag.find(",",0);
			xpoint = atof(_tag.substr(location+1,location3-location-1).c_str());
			ypoint = atof(_tag.substr(location3+1,location2-location3-1).c_str());
			_point = gdt::gdtpoint(xpoint,ypoint);
			// After searching for the coordinates we can create the node.
			v = new_node(_point,_id);
			set_label(v,_label);
		}
		// Continue to the next key
		in >> _tag;
		in >> _tag;
		location = _tag.find( ">",0 );
		_key = _tag.substr(0,location);
		// Controls if the key is d8, the dimension of the node
		if (_key == "key=\"d8\""){
			location2 = _tag.find("<",0);
			location3 = _tag.find(",",0);
			width = atof(_tag.substr(location+1,location3-location-1).c_str());
			height = atof(_tag.substr(location3+1,location2-location3-1).c_str());
			set_width(v,width);
			set_height(v,height);
			// After finding the dimension, it proceeds to the next tag
			in >> _tag;
			in >> _tag;
			location = _tag.find( ">",0 );
			_key = _tag.substr(0,location);
		}else{ // Set the dimension default
			set_width(v,2);
			set_height(v,2);
		}
		// Controls the key d10, that handles the shape of the node
		if (_key == "key=\"d10\""){
			location2 = _tag.find(">",0);
			location3 = _tag.find("<",0);
			_t = _tag.substr(location2+1,(location3-location2-1));;
			if (_t == "OVAL2D")
				set_shape(v,ELLIPSE);
			else
				set_shape(v,RECTANGLE);
			in >> _tag;
			in >> _tag;
			location = _tag.find( ">",0 );
			_key = _tag.substr(0,location);
			
		}else { // Else set the default shape for this node
			set_shape(v,RECTANGLE);
		}
		// Controls the key d12, that handles the position of the label
		if (_key == "key=\"d12\""){
			location2 = _tag.find("<",0);
			location3 = _tag.find(",",0);
			xpoint = atof(_tag.substr(location+1,location3-location-1).c_str());
			ypoint = atof(_tag.substr(location3+1,location2-location3-1).c_str());
			_point = gdt::gdtpoint(xpoint,ypoint);
			set_label_position(v,_point);
			// Proceed to the next tag
			in >> _tag;
			in >> _tag;
		}else{
			_point = recommended_label_position(v);
			set_label_position(v,_point);
		}	
		set_body_color(v,grey1);
		set_label_color(v,black);
		set_border_color(v,grey2);
			
	}
	// At the end of the earlier cycle, the _tag contain <edge, that is the tag
	// of the beginning of the part dedicated to the edges.
	
	while (_tag=="<edge"){
		in >> _tag;
		location = _tag.find("e",0);
		location2 = _tag.size()-location-2;
		_t = _tag.substr(location+1,location2);
		_id=atoi(_t.c_str());
		in >> _tag;
		if (_tag == "directed=\"false\"")
			in >> _tag;
		else
			directed = true;
		location = _tag.find("n",0);
		location2 = _tag.size()-location-2;
		v = find_node(atoi(_tag.substr(location+1,location2).c_str()));
		in >> _tag;
		location = _tag.find("n",0);
		location2 = _tag.size()-location-2;
		v2 = find_node(atoi(_tag.substr(location+1,location2).c_str()));
		e = undi_graph::new_edge(v,v2,_id);
		if (directed)
			undi_graph::make_directed(e,v);
		if (_tag.find("/",0) == string::npos){
			in >> _tag;
			in >> _tag;
			location = _tag.find( ">",0 );
			_key = _tag.substr(0,location);
			if (_key == "key=\"d0\""){
				location2 = _tag.find("<",0);
				_label = _tag.substr((location+1),(location2-location-1));
				set_label(e,replace_all (_label,"&nbsp;", " "));
				// Proceed to the next tag
				in >> _tag;
				in >> _tag;
				location = _tag.find( ">",0 );
				_key = _tag.substr(0,location);
			}else{
				set_label(e,replace_all (_t,"&nbsp;", " "));
			}
			if (_key == "key=\"d5\""){
				_pointlist.clear();
				while(_key == "key=\"d5\""){
					location2 = _tag.find("<",0);
					location3 = _tag.find(",",0);
					xpoint = atof(_tag.substr(location+1,location3-location-1).c_str());
					ypoint = atof(_tag.substr(location3+1,location2-location3-1).c_str());
					_point = gdt::gdtpoint(xpoint,ypoint);
					_pointlist.append(_point);
					in >> _tag;
					in >> _tag;
					location = _tag.find( ">",0 );
					_key = _tag.substr(0,location);
				}
				if (_pointlist.size() > 0) set_bends(e,v,_pointlist);
			}
			if (_key == "key=\"d6\""){
				location2 = _tag.find("<",0);
				location3 = _tag.find(",",0);
				xpoint = atof(_tag.substr(location+1,location3-location-1).c_str());
				ypoint = atof(_tag.substr(location3+1,location2-location3-1).c_str());
				_anch_source = gdt::gdtpoint(xpoint,ypoint);
				set_anchor_source(e,_anch_source);
				in >> _tag;
				in >> _tag;
				location = _tag.find( ">",0 );
				_key = _tag.substr(0,location);				
			}
			if (_key == "key=\"d7\""){
				location2 = _tag.find("<",0);
				location3 = _tag.find(",",0);
				xpoint = atof(_tag.substr(location+1,location3-location-1).c_str());
				ypoint = atof(_tag.substr(location3+1,location2-location3-1).c_str());
				_anch_target = gdt::gdtpoint(xpoint,ypoint);
				set_anchor_target(e,_anch_target);	
				in >> _tag;
				in >> _tag;
				location = _tag.find( ">",0 );
				_key = _tag.substr(0,location);				
			}
			if (_key == "key=\"d12\""){
				location2 = _tag.find("<",0);
				location3 = _tag.find(",",0);
				xpoint = atof(_tag.substr(location+1,location3-location-1).c_str());
				ypoint = atof(_tag.substr(location3+1,location2-location3-1).c_str());
				_point = gdt::gdtpoint(xpoint,ypoint);
				set_label_position(e,_point);
				// Proceed to the next tag
				in >> _tag;
				in >> _tag;
			}
			else{
				_point=recommended_label_position(e);
				set_label_position(e,_point);
			}
		}
		else { 
			in >> _tag;
			set_label(e,replace_all (_t,"&nbsp;", " "));
		}
		set_shape(e,edge_defs->shape);
		set_body_color(e,blue);
		set_label_color(e,black);
		directed = false;
	}
	
	
	in.close();
	return true;	
	}
	




	
	
	
	

// -----------------------
// PRIVATE
// -----------------------


	void
	draw_undi_graph::
local_new
	()
	{
	draw_curr = new (struct_draw_info);
	draw_defs = new (struct_draw_info);
	node_defs = new (struct_node_defaults);
	edge_defs = new (struct_edge_defaults);
	node_info = new (gdt::gdtnode_map<struct_draw_node_info>);
	edge_info = new (gdt::gdtedge_map<struct_draw_edge_info>);
	//
	node_info -> init (*this);
	edge_info -> init (*this);
	}
	
	
	
	void
	draw_undi_graph::
local_del
	()
	{
	safe_delete (draw_curr);
	safe_delete (draw_defs);
	safe_delete (node_defs);
	safe_delete (edge_defs);
	safe_delete (node_info);
	safe_delete (edge_info);
	}

	
	
	void
	draw_undi_graph::
local_renew
	()
	{
	local_del ();
	local_new ();
	}



	void
	draw_undi_graph::
local_init
	(const undi_graph& ug, bool remove_crosses)
	{
	set_scale (draw_curr->scale);
	//cout << "\ndraw_undi_graph::local_init (undi_graph)";
	
	plan_undi_graph pug(ug);
	if (pug.is_biconnected())
		local_init(pug, POLYLINE_COMPACTED, NULL_FACE, remove_crosses);
	else
		local_init(pug, PLAN_UPWARD_EMBEDDING, NULL_FACE, remove_crosses);
	}




	void
	draw_undi_graph::
local_init
	(
	const plan_undi_graph& pug0,
	algorithm_type alg,
	face ef,
	bool remove_crosses,
	gdt::gdtedge_array<int>* cp0,
	int min_switches
	)
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

	set_scale (draw_curr->scale);
	//cout << "\ndraw_undi_graph::local_init (plan_undi_graph)";

	gdtnode s,t;
	gdtedge e0,e;
	face f;
	if (alg == DEFAULT_ALGORITHM)
		{
		if (is_biconnected()) alg = POLYLINE_COMPACTED;
		else alg = PLAN_UPWARD_EMBEDDING;
		}

	switch (alg)
		{
		case VISIBILITY:
		case VISIBILITY_COMPACTED:
		case VISIBILITY_REGULAR:		// dummy (not considered)
		case VISIBILITY_REGULAR_COMPACTED:	// dummy (not considered)
			{
			if (!ef) ef=pug0.first_face();	 // select an external face

			plan_undi_graph pug(pug0);       // make pug as a copy of pug0; it is needed
			ef = pug.find_face(pug0.id(ef)); // because compute_visibility_representation
						         // is not a constant method.


			// ------------------------------------------
			// compute a visibility representation of pug
			// automatically checking if the graph is
			// st-orientable, compatibility with the
			// chosed external face.
			// ------------------------------------------
			//
			gdt::gdtnode_array<horizontal_segment> seg_node (pug);
			gdt::gdtedge_array<vertical_segment>   seg_edge (pug);

			// -----------------------------
			// make an gdtedge e_st if pug is
			// an st-digraph with compatible
			// external face
			// -----------------------------

			gdtedge e_st = NULL_EDGE;

			if (pug.is_st_digraph(s,t))
				{
				face st_face=NULL_FACE;

				forall_faces (f,pug)
					if (pug.node_belongs_to_face(s,f) && pug.node_belongs_to_face(t,f))
						if (f == ef)  {st_face = f; break;}
						else	       st_face = f;

				if (st_face)
					{
					e_st = pug.new_edge(s,t,st_face);
					pug.make_directed (e_st,s);
					seg_edge.init (pug);
					}
				}

			// --------------
			// set gdtedge costs
			// --------------

			gdt::gdtedge_array<int>* cp = NULL;
			if (cp0)
				{
				cp = new gdt::gdtedge_array<int>(pug, 0);
				forall_edges (e0,pug0)
					{
					e = pug.corresponding_edge(e0,pug0);
					(*cp)[e] = (*cp0)[e0];
					}
				}

			// -----------------------------------
			// compute a visibility representation
			// -----------------------------------

			bool comp = false;
			if (alg == VISIBILITY_COMPACTED ||
			    alg == VISIBILITY_REGULAR_COMPACTED) comp = true;

			if (e_st)
				{
				if (cp) (*cp)[e_st] = 0;
				pug.compute_visibility_representation (seg_node,seg_edge,e_st,comp,cp);
				pug.del_edge(e_st);
				}
			else pug.compute_visibility_representation (seg_node,seg_edge,ef,comp,cp);


			//cout << "\nvisibility representation of pug executed" << flush;
			double k = scale();

			// --------------------------------------------------------------
			// compute and set shape, center and width for each gdtnode of *this
			// --------------------------------------------------------------
			//
			gdtnode v;
			forall_nodes (v,*this)
				{
				apply_defaults(v);
				gdtnode w = pug.corresponding_node(v,*this);
				double xmin = seg_node[w].x_min;
				double xmax = seg_node[w].x_max;
				gdt::gdtpoint p((xmin + xmax)/2*k+k,seg_node[w].y*k+k);
				set_center(v,p);
				set_shape  (v,RECTANGLE);
				set_height (v,0.5*k);
				set_width  (v,(xmax - xmin)*k+0.25*k);
				set_label_position(v,recommended_label_position(v));
				}

			// -----------------------------------------------
			// compute and set the bends on each gdtedge of *this
			// -----------------------------------------------
			//
			gdtedge e;
			forall_edges (e,*this)
				{
				apply_defaults(e);
				gdtedge  d  = pug.corresponding_edge(e,*this);
				gdtnode  v1 = source(e);
				gdtnode  v2 = target(e);
				gdt::gdtpoint c1 = center(v1);
				gdt::gdtpoint c2 = center(v2);
				gdt::gdtpoint p1((c1.xcoord()-k)/k,(c1.ycoord()-k)/k);
				gdt::gdtpoint p2((c2.xcoord()-k)/k,(c2.ycoord()-k)/k);
				double w1 = height(v1)/2;
				double w2 = height(v2)/2;

				if (c1.ycoord() < c2.ycoord())  w2 = -w2;
				else				w1 = -w1;

				if (seg_edge[d].x != p1.xcoord())
					{
					//gdt::gdtpoint p(seg_edge[d].x*k+k,p1.ycoord()*k+k);				// new anchor_source point
					gdt::gdtpoint p(seg_edge[d].x*k+k,c1.ycoord() + w1);
					gdt::gdtpoint dp = gdt::gdtpoint (p.xcoord()-c1.xcoord(),p.ycoord()-c1.ycoord());	// new anchor_source
					set_anchor_source (e,dp);
					//new_bend (e,v1,0,p);
					//cout << "\nadded a new bend" << flush;
					}
				if (seg_edge[d].x != p2.xcoord())
					{
					//gdt::gdtpoint p(seg_edge[d].x*k+k,p2.ycoord()*k+k);				// new anchor_target point
					gdt::gdtpoint p(seg_edge[d].x*k+k,c2.ycoord() + w2);
					gdt::gdtpoint dp = gdt::gdtpoint (p.xcoord()-c2.xcoord(),p.ycoord()-c2.ycoord());	// new anchor_target
					set_anchor_target (e,dp);
					//new_bend (e,v2,0,p);
					//cout << "\nadded a new bend" << flush;
					}
				set_label_position(e,recommended_label_position(e));
				}
			 if (cp) {delete (cp); cp=NULL;}	
			}
			break;

		case POLYLINE:
		case POLYLINE_COMPACTED:
		case POLYLINE_REGULAR:			//dummy (not considered)
		case POLYLINE_REGULAR_COMPACTED:	//dummy (not considered)
			{
			if (!ef) ef=pug0.first_face();
			// -------------------------------------------
			//
			double round = 0.001;
			int  ef_lsnode;
			int  ef_rsnode;
			int  ef_sedge;

			// ----------------------------------------------
			// store a reference gdtedge and two reference nodes
			// in order to refind the external face
			// after the function make_simple
			// ----------------------------------------------
			//
			gdtedge e   = pug0.first_adj_edge (ef);
			ef_sedge = pug0.id(e);

			gdtnode vl,vr;
			gdtnode v = pug0.source(e);
			if (pug0.right_face_moving_along_edge(e,v) == ef) {vr = v; vl = pug0.target(e);}
			else						  {vl = v; vr = pug0.target(e);}
			ef_lsnode = pug0.id(vl);
			ef_rsnode = pug0.id(vr);

			// -----------------------------------------------
			// make *this simple (i.e. without multiple edges)
			// and pug as an its copy
			// -----------------------------------------------
			//
			make_simple();			// make *this simple

			undi_graph ug_temp (*this);
			ug_temp.del_all_constraints();
			plan_undi_graph pug(ug_temp);
			// -----------------------------------
			// refind the new external face in pug
			// -----------------------------------

			gdtedge e_start = pug.find_edge (ef_sedge);
			gdtnode v1      = pug.source(e_start);
			gdtnode v2      = pug.target(e_start);
			int  idv1    = pug.id(v1);
			int  idv2    = pug.id(v2);
			if      (idv1 == ef_lsnode) ef = pug.left_face_moving_along_edge  (e_start,v1);
			else if (idv1 == ef_rsnode) ef = pug.right_face_moving_along_edge (e_start,v1);
			else if (idv2 == ef_lsnode) ef = pug.left_face_moving_along_edge  (e_start,v2);
			else if (idv2 == ef_rsnode) ef = pug.right_face_moving_along_edge (e_start,v2);

			//cout << "\nEXTERNAL FACE " << pug.id(ef) << flush;


			// ------------------------------------------
			// compute a visibility representation of pug
			// automatically checking if the graph is
			// st-orientable, compatibility with the
			// chosen external face.
			// ------------------------------------------
			//

			gdt::gdtnode_array<horizontal_segment> seg_node(pug);
			gdt::gdtedge_array<vertical_segment>   seg_edge(pug);

			// -----------------------------
			// make an gdtedge e_st if pug is
			// an st-digraph with compatible
			// external face
			// -----------------------------

			gdtedge e_st = NULL_EDGE;

			if (pug.is_st_digraph(s,t))
				{
				//cout << "\nTHE GRAPH IS AN ST-DIGRAPH" << flush;
				face st_face = NULL_FACE;
				forall_faces (f,pug)
					if (pug.node_belongs_to_face(s,f) && pug.node_belongs_to_face(t,f))
						if (f == ef) {st_face = f; break;}
						else	      st_face = f;

				if (st_face)
					{
					e_st = pug.new_edge(s,t,st_face);
					pug.make_directed(e_st,s);
					seg_edge.init(pug);
					}
				}
			// --------------
			// set gdtedge costs
			// --------------

			gdt::gdtedge_array<int>* cp = NULL;
			if (cp0)
				{
				cp = new gdt::gdtedge_array<int>(pug,0);
				gdtedge e,e0;
				forall_edges (e0,pug0)
					{
					e = pug.corresponding_edge(e0,pug0);
					(*cp)[e] = (*cp0)[e0];
					}
				}

			// -----------------------------------
			// compute a visibility representation
			// -----------------------------------

			bool comp = false;
			if (alg == POLYLINE_COMPACTED ||
			    alg == POLYLINE_REGULAR_COMPACTED) comp = true;
			if (e_st)
				{
				if (cp) (*cp)[e_st] = 0;
				pug.compute_visibility_representation (seg_node,seg_edge,e_st,comp,cp);
				pug.del_edge(e_st);
				}
			else {pug.compute_visibility_representation (seg_node,seg_edge,ef,comp,cp);
				//cout << endl << "valore di cp dopo  compute_visibility_representation=" << cp << endl;
			}
			//cout << "\nvisibility representation of pug executed" << flush;
			
			double k = scale();
						
			// ------------------ 1 -------------------
			// compute the center of each gdtnode of *this
			// ----------------------------------------
			//
			forall_nodes (v,*this)
				{
				apply_defaults(v);
				gdtnode w = pug.corresponding_node(v,*this);
				double xmin = seg_node[w].x_min;
				double xmax = seg_node[w].x_max;
				gdt::gdtpoint p((xmin + xmax)/2*k+k,seg_node[w].y*k+k);
				set_center(v,p);
				set_label_position(v,recommended_label_position(v));
				}
			
			// -------------------- 2 --------------------
			// compute the gdtedge shape and bend coordinates
			// -------------------------------------------
			//
			//node_matrix<bool> E (pug,false);
			gdtedge e1;
			
			forall_edges (e1,*this)
				{
				gdtnode u,v;    // endpoints of e (of pug graph)
				gdtnode u1,v1;  // endpoints of e1 (of *this)
				gdtedge e = pug.corresponding_edge(e1,*this);
				gdt::gdtlist<gdt::gdtpoint> bends_u_v;

				if (seg_node[pug.source(e)].y < seg_node[pug.target(e)].y) u = pug.source(e);
				else							   u = pug.target(e); 
				v  = pug.opposite(u,e);	
				u1 = corresponding_node(u,pug);
				v1 = corresponding_node(v,pug);			
				
				// u (u1) is below v (v1)
														
				// if the distance between u and v is 1, then no bend is needed
				// otherwise you have to insert a bend in the appropriate point
				//
				if (seg_node[v].y - seg_node[u].y > 1)
					{
					double zu= (center(u1).xcoord()-k)/k;  // that is: z_u = (x_u / k) - 1
					double diff = max (seg_edge[e].x-zu, zu-seg_edge[e].x);	
					// that is: diff = | x_e - z_u |	
					if (diff > round) // insert a bend above u in correspondence with seg_edge[e]
						{
						gdt::gdtpoint p1(seg_edge[e].x*k+k,(seg_node[u].y+1)*k+k);
						bends_u_v.append(p1);
						}
					
					if (seg_node[v].y - seg_node[u].y > 2 || bends_u_v.empty())
						{
						double zv= (center(v1).xcoord()-k)/k;
						diff = max (seg_edge[e].x-zv, zv-seg_edge[e].x);
						if (diff > round)
							{
							gdt::gdtpoint p2(seg_edge[e].x*k+k,(seg_node[v].y-1)*k+k);
							bends_u_v.append(p2);
							}
						}
					}
								  
				set_bends (e1,u1,bends_u_v);
				set_label_position(e1,recommended_label_position(e1));
				}
				
			// remove all dummy nodes added by make_simple
			//
			gdt::gdtlist<gdtnode> Liter(all_nodes());
			forall (v,Liter) if (is_marked (v,RM3_ADDED_BY_MAKE_SIMPLE)) remove_two_degree_node (v);
			forall_edges (e1,*this) apply_defaults(e1);
			if (cp) {delete (cp); cp=NULL;}
			}
			break;


		case ORTH_FROM_VISIBILITY:
		case ORTH_FROM_VISIBILITY_COMPACTED:
			{
			if (!ef) ef=pug0.first_face();
			// -------------------------------------------
			//
			double round = 0.001;
			int  ef_lsnode;
			int  ef_rsnode;
			int  ef_sedge;

			// ----------------------------------------------
			// store a reference gdtedge and two reference nodes
			// in order to find again the external face
			// after the function make_simple
			// ----------------------------------------------
			//
			gdtedge e   = pug0.first_adj_edge (ef);
			ef_sedge = pug0.id(e);

			gdtnode vl,vr;
			gdtnode v = pug0.source(e);
			if (pug0.right_face_moving_along_edge(e,v) == ef) {vr = v; vl = pug0.target(e);}
			else						  {vl = v; vr = pug0.target(e);}
			ef_lsnode = pug0.id(vl);
			ef_rsnode = pug0.id(vr);

			// -----------------------------------------------
			// make *this simple (i.e. without multiple edges)
			// and copy it into pug
			// -----------------------------------------------
			//
			make_simple();			// make *this simple

			undi_graph ug_temp (*this);
			ug_temp.del_all_constraints();
			plan_undi_graph pug(ug_temp);
			
			
			// -----------------------------------
			// refind the new external face in pug
			// -----------------------------------

			gdtedge e_start = pug.find_edge (ef_sedge);
			gdtnode v1      = pug.source(e_start);
			gdtnode v2      = pug.target(e_start);
			int  idv1    = pug.id(v1);
			int  idv2    = pug.id(v2);
			if      (idv1 == ef_lsnode) ef = pug.left_face_moving_along_edge  (e_start,v1);
			else if (idv1 == ef_rsnode) ef = pug.right_face_moving_along_edge (e_start,v1);
			else if (idv2 == ef_lsnode) ef = pug.left_face_moving_along_edge  (e_start,v2);
			else if (idv2 == ef_rsnode) ef = pug.right_face_moving_along_edge (e_start,v2);

			//cout << "\nEXTERNAL FACE " << pug.id(ef) << flush;


			// ------------------------------------------
			// compute a visibility representation of pug
			// automatically checking if the graph is
			// st-orientable, compatibility with the
			// chosen external face.
			// ------------------------------------------
			//

			gdt::gdtnode_array<horizontal_segment> seg_node(pug);
			gdt::gdtedge_array<vertical_segment>   seg_edge(pug);

			// -----------------------------
			// make an gdtedge e_st if pug is
			// an st-digraph with compatible
			// external face
			// -----------------------------

			gdtedge e_st = NULL_EDGE;

			if (pug.is_st_digraph(s,t))
				{
				//cout << "\nTHE GRAPH IS AN ST-DIGRAPH" << flush;
				face st_face = NULL_FACE;
				forall_faces (f,pug)
					if (pug.node_belongs_to_face(s,f) && pug.node_belongs_to_face(t,f))
						if (f == ef) {st_face = f; break;}
						else	      st_face = f;

				if (st_face)
					{
					e_st = pug.new_edge(s,t,st_face);
					pug.make_directed(e_st,s);
					seg_edge.init(pug);
					}
				}
			// --------------
			// set gdtedge costs
			// --------------

			gdt::gdtedge_array<int>* cp = NULL;
			if (cp0)
				{
				cp = new gdt::gdtedge_array<int>(pug,0);
				gdtedge e,e0;
				forall_edges (e0,pug0)
					{
					e = pug.corresponding_edge(e0,pug0);
					(*cp)[e] = (*cp0)[e0];
					}
				}

			// -----------------------------------
			// compute a visibility representation
			// -----------------------------------

			bool comp = false;
			if (alg == ORTH_FROM_VISIBILITY_COMPACTED) comp = true;
			if (e_st)
				{
				if (cp) (*cp)[e_st] = 0;
				pug.compute_visibility_representation (seg_node,seg_edge,e_st,comp,cp);
				pug.del_edge(e_st);
				}
			else pug.compute_visibility_representation (seg_node,seg_edge,ef,comp,cp);

			//cout << "\nvisibility representation of pug executed" << flush;
			
			double k = scale();
						
			// ------------------ 1 -------------------
			// compute the x-coordinate 
			// ----------------------------------------
			//
			forall_nodes (v,*this)
				{
				apply_defaults(v);
				gdtnode w = pug.corresponding_node(v,*this);
				double xmed = (seg_node[w].x_min + seg_node[w].x_max)/2.0;
				
				// chose an x that is near to the middle point of the segment
				// and corresponding to an edge_segment
				//
				gdtedge e;
				double chosen_x = 0.0;
				double min_dist = INFINITE;
				forall_edges_adjacent_node(e,v,*this)
					{
					double e_x = seg_edge[pug.corresponding_edge(e,*this)].x;
					double dist = max( e_x - xmed, xmed - e_x);
					if ( dist <= min_dist )
						{
						min_dist = dist;
						chosen_x = e_x;
						}
					}
					
				gdt::gdtpoint p(chosen_x*k+k,(seg_node[w].y*k+k));
				set_center(v,p);
				set_label_position(v,recommended_label_position(v));
				}
			
			// -------------------- 2 --------------------
			// compute the gdtedge shape and bend coordinates
			// -------------------------------------------
			//
			gdtedge e1;
			forall_edges (e1,*this)
				{
				gdtnode u,v;    // endpoints of e (of pug graph)
				gdtnode u1,v1;  // endpoints of e1 (of *this)
				gdtedge e = pug.corresponding_edge(e1,*this);
				gdt::gdtlist<gdt::gdtpoint> bends_u_v;

				if (seg_node[pug.source(e)].y < seg_node[pug.target(e)].y) u = pug.source(e);
				else							   u = pug.target(e); 
				v  = pug.opposite(u,e);	
				u1 = corresponding_node(u,pug);
				v1 = corresponding_node(v,pug);			
				
				// u (u1) is below v (v1)
														
				double zu= (center(u1).xcoord()-k)/k; 
				double diff = max (seg_edge[e].x-zu, zu-seg_edge[e].x);	
				// that is: diff = | x_e - z_u |	
				if (diff > round) // insert a bend above u in correspondence with seg_edge[e]
					{
					gdt::gdtpoint p1(seg_edge[e].x*k+k,(seg_node[u].y*k+k));
					bends_u_v.append(p1);
					}
				
				//if (seg_node[v].y - seg_node[u].y > 2 || bends_u_v.empty())
					{
					double zv= (center(v1).xcoord()-k)/k;
					diff = max (seg_edge[e].x-zv, zv-seg_edge[e].x);
					if (diff > round)
						{
						gdt::gdtpoint p2(seg_edge[e].x*k+k,(seg_node[v].y*k+k));
						bends_u_v.append(p2);
						}
					}
								  
				set_bends (e1,u1,bends_u_v);
				set_label_position(e1,recommended_label_position(e1));
				}
				
			// remove all dummy nodes added by make_simple
			//
			gdt::gdtlist<gdtnode> Liter(all_nodes());
			forall (v,Liter) if (is_marked (v,RM3_ADDED_BY_MAKE_SIMPLE)) remove_two_degree_node (v);
			forall_edges (e1,*this) apply_defaults(e1);
			if (cp) {delete (cp); cp=NULL;}
			}
			space_overlapping_edges();			
			break;

		
		case PLAN_UPWARD_EMBEDDING:
		case PLAN_UPWARD_EMBEDDING_REGULAR:
			{
			if (!ef) ef = pug0.first_face();
			
			plan_undi_graph pug (pug0);				// make a copy of pug0
			face ext_face = pug.corresponding_face (ef,pug0);	// take the external face
			gdt::gdtnode_array<gdtedge> fe (pug, NULL_EDGE);		// first gdtedge of the upward embedding
			
			pug.make_upward_embedding (ext_face, fe, min_switches);	 // determine an upward planar embedding with minimum sources and sinks
			upwa_plan_undi_graph upug (pug,fe);			 // construct the upward representation
			
			if (alg == PLAN_UPWARD_EMBEDDING_REGULAR)
				*this = draw_undi_graph (upug,POLYLINE_REGULAR_COMPACTED,remove_crosses);
			else	*this = draw_undi_graph (upug,POLYLINE_COMPACTED,remove_crosses); // construct a drawing of the upward representation
			
			// restore the initial 
			// directions of the edges
			//
			gdtedge e;
			forall_edges (e,*this)
				{
				gdtedge e0 = pug0.corresponding_edge (e,*this);
				gdtnode v0 = pug0.start(e0);
				if (!v0) make_undirected (e);
				else make_directed (e,corresponding_node (v0,pug0));
				}
			}
			break;
		default:
			break;

		}
	if (remove_crosses) remove_cross_nodes();
	}	
	


	void
	draw_undi_graph::
local_init
	(const upwa_plan_undi_graph& upug, algorithm_type alg, bool remove_crosses)
	{
	gdt::gdtlist<gdtedge> dummy_edges;
	gdtedge e;
	gdtnode v,s,t;
	
	if (alg == DEFAULT_ALGORITHM) alg = POLYLINE_COMPACTED;
	
	switch (alg)
		{
		case POLYLINE:
		case POLYLINE_COMPACTED:
		case VISIBILITY:
		case VISIBILITY_COMPACTED:
		case POLYLINE_REGULAR:
		case POLYLINE_REGULAR_COMPACTED:
		case VISIBILITY_REGULAR:
		case VISIBILITY_REGULAR_COMPACTED:		
			{
			// ----------------- 1 -----------------
			// make a plan as a copy of the upug and 
			// transform it into an st-digraph.
			// -------------------------------------
			
			upwa_plan_undi_graph upug1 (upug);
			
			if (alg == POLYLINE_REGULAR || alg == VISIBILITY_REGULAR ||
			    alg == POLYLINE_REGULAR_COMPACTED || alg == VISIBILITY_REGULAR_COMPACTED)
				dummy_edges = upug1.make_st_with_regularity (s,t);	
			else 	dummy_edges = upug1.make_st (s,t);		
										
			plan_undi_graph pug (upug1);
						
			//std::cout << "\nUPWARD COMPUTED!" << std::flush;
						
			// ----------------- 2 -----------------
			// make a new dug by the constructor 
			// plan --> draw.
			// -------------------------------------

			gdt::gdtedge_array<int>* cp = NULL;
			if (alg == POLYLINE_COMPACTED || alg == VISIBILITY_COMPACTED ||
			    alg == POLYLINE_REGULAR_COMPACTED || alg == VISIBILITY_REGULAR_COMPACTED)
				{
				cp = new gdt::gdtedge_array<int>(pug,1);
				forall (e,dummy_edges)
					{
					gdtedge e_pug = pug.corresponding_edge(e,upug1);
					(*cp)[e_pug] = 0; 
					}
				}
			
			draw_undi_graph dug (pug,alg,NULL_FACE,false,cp);

			// ----------------- 3 -----------------
			// remove all dummy edges and nodes and
			// make *this as the resulting graph.
			// -------------------------------------
		
			forall (e,dummy_edges)
				{
				gdtedge e_dug = dug.corresponding_edge(e,upug1);
				dug.del_edge (e_dug);
				}
			
			dug.del_node (dug.corresponding_node(s,upug1));
			dug.del_node (dug.corresponding_node(t,upug1));
		
			*this = dug;
				
			// ----------------- 4 -----------------
			//      remove all marked nodes
			// -------------------------------------
			
			//gdt::gdtnode_array<bool> deleted (*this,false);
			gdt::gdtlist<gdtnode>       dummy_nodes = find_nodes_marked_as (RM3_ADDED_BY_MAKE_UPWARD);
			gdt::gdtlist<marker_type> L;
			L.append(RM3_ADDED_BY_MAKE_UPWARD);
			forall (v,dummy_nodes) remove_two_degree_node (v,&L);
			
			forall_edges (e,*this) 
				{
				ostringstream x;
				x << id(e); // << ends;
				set_label (e,std::string(x.str()));
				set_label_position(e,recommended_label_position(e)); 
				}
			if (cp) {delete (cp); cp=NULL;}
			}			
			break;
		default:
			gdt_error("invalid algorithm");
			break;
		}
	if (remove_crosses) remove_cross_nodes();
	}



	// Modified by W.D. on April 7
	//
	void
	draw_undi_graph::
local_init
	(
	const dime_orth_plan_undi_graph& dopug0, 
	bool refine_compaction,
	bool init_draw_info, 
	bool remove_crosses
	)
	{
	// graph dopug0 and the current draw_undi_graph 
	// do not contain bends
	//
	
	set_scale (draw_curr->scale);
	dime_orth_plan_undi_graph dopug(dopug0);
	
	// new code
	// ---------------------------------------------------
	// recursively apply the one dimensional compaction
	// algorithm in order to improve, as much as possible,
	// the total gdtedge length of the drawing
	// ---------------------------------------------------

	
	if (refine_compaction)
		   {
		   gdtedge e;
		   bool exit = false;
		   gdt::gdtmap<int,int> _min(0);
		   gdt::gdtmap<int,int> _max(0);
		   gdt::gdtmap<int,int> cost(0);
		   int total_edge_length_before = 0;
		   int total_edge_length_after  = 0;

		   // set costs, lower, and upper capacities
		   //
		   forall_edges (e,dopug)
			   {
			   int ide = dopug.id(e);
			   _min[ide] = 1;
			   _max[ide] = INFINITE;
			   if (dopug.is_marked(e,RM3_ADDED_BY_RECTANGULARIZATION)) cost[ide] = 0;
			   else							   cost[ide] = 1;
			   total_edge_length_before += dopug.length_of_edge(e);
			   }

		   // compact recursively
		   //
		   //cout << "\nINIT POST-COMPACTION" << flush;
		   int i = 0;
		   while (!exit)
			   {
			   i++;
			   //cout << "\ncycle number " << i << flush;

			   dopug.one_dimensional_tieing (_min, _max, cost, horizontal);
			   dopug.one_dimensional_tieing (_min, _max, cost, vertical);

			   total_edge_length_after = 0;

			   forall_edges (e,dopug) total_edge_length_after += dopug.length_of_edge(e);

			   //cout << "\ntotal gdtedge length before = " <<  total_edge_length_before << flush;
			   //cout << "\ntotal gdtedge length after = " <<  total_edge_length_after << flush;

			   if (total_edge_length_after < total_edge_length_before)
						   total_edge_length_before = total_edge_length_after;
			   else    exit = true;
			   }
		   }
	// ------------------ end new code ------------------------
	
	
	gdtnode  u;		// dopug gdtnode
	gdtnode  v;		// this gdtnode
	gdtedge  e;
	gdt::gdtpoint p;

	// ---------------------------------------
	// copy the dopug gdtnode-coordinates in this
	// ---------------------------------------

	forall_nodes(v,*this)
		{
		u = dopug.corresponding_node (v,*this);
		p = dopug.position_of_node(u);
		p = gdt::gdtpoint (p.xcoord(),p.ycoord());
		set_center(v,p);
		if (init_draw_info) apply_defaults(v);
		}

	remove_bend_nodes(dopug);				// uncompress thick edges and remove bend nodes
	std::set<gdt::gdtpoint,ltpt> CP;					// List of cross points
	if (remove_crosses) CP = remove_cross_nodes();		// remove all crosses
	if (refine_compaction) refine_orth_compaction(CP);	// apply a post-processing to refine compaction

	// -------------------------------------------------------
	// normalize coordinates and apply some default parameters
	// -------------------------------------------------------

	normalize_coordinates();				// multiply all gdtnode and bend coordinates for the scale
	forall_edges (e,*this)					// set the label position for the edges 
			{
			if (init_draw_info)
				{
				gdt::gdtpoint as = anchor_source(e);
				gdt::gdtpoint at = anchor_target(e);
				apply_defaults(e);
				set_anchor_source (e,as);
				set_anchor_target (e,at);
				}
			gdtnode v1 = source(e);
			gdtnode v2 = target(e);
			set_label_position(e,recommended_label_position(e));
			set_label_position (v1,recommended_label_position(v1));
			set_label_position (v2,recommended_label_position(v2));
			}
	}
	


	void
	draw_undi_graph::	
local_init_for_dimensioned_nodes 
	(
	const dime_orth_plan_undi_graph& dopug,
	const dime_orth_plan_undi_graph& dopug1,
	const gdt::gdtnode_map<gdtnode>&            super_node,
	const gdt::gdtnode_array<int>&           w,
	const gdt::gdtnode_array<int>&           h,
	bool  				 remove_crosses)
	{
	double k = scale(); 
	
	// ----------------------------
	// contract all the super nodes
	// into single nodes
	// ----------------------------
	
	gdtedge e;
	gdtedge e1;
	gdtnode v;
	  
	//print (COMPLETE);

	forall_edges (e1,dopug1)
		{
		gdtnode u1 = dopug1.source(e1);
		gdtnode u2 = dopug1.target(e1);
		
		//cout << "\nexaming gdtedge " << dopug1.id(e1)
		//    << "=(" << dopug1.id(u1) << "," << dopug1.id(u2) << ")" << flush;

		gdtnode U1 = super_node[u1];
		gdtnode U2 = super_node[u2];
		
		if (U1) //cout << " super gdtnode of " << dopug1.id(u1) << " = " << dopug.id(U1) << flush;
		if (U2) //cout << " super gdtnode of " << dopug1.id(u2) << " = " << dopug.id(U2) << flush;
				
		if ( 
		   (U1) &&
		   (U2) && 
		   (U1 == U2) 
		   ) 
		   {
		   e = corresponding_edge (e1,dopug1);
		   if (e)
		   	{
			gdtnode v1 = source(e);
			gdtnode v2 = target(e);
			int old_id;
			if (id(v1) < id(v2)) old_id = id(v1);
			else		     old_id = id(v2);
			v = contract(e);
			assign_id(v,old_id);
			}
		   }	
		}
	
	
	// --------------------------------
	// update gdtnode and gdtedge coordinates
	// --------------------------------
		
	forall_nodes (v,*this)
		{
		gdtnode u = dopug1.corresponding_node(v,*this);
		gdtnode z = dopug.corresponding_node(v,*this);

		if (u) 
			{
			mark(v,dopug1.markers(u));
			apply_defaults(v);
			set_shape (v,RECTANGLE);
			double width  = 0;
			double height = 0;
			
			if (z)
				{
				width  = w[z];
				height = h[z];
				}
				
			gdt::gdtpoint  pu     = dopug1.position_of_node(u);
			gdt::gdtpoint  pv     = gdt::gdtpoint(pu.xcoord() + width/2.0, pu.ycoord() + height/2.0);
			set_center (v,pv);
			set_width  (v,(width+0.5)*k);
			set_height (v,(height+0.5)*k);

			/*if (width==0) set_width  (v,(width+0.5)*k);
			else set_width  (v, width);
			if (height==0) set_height (v,(height+0.5)*k);
			else set_height (v, height);*/
			}
		
		}

	gdtedge d;
	//super_node.show(cout);
	forall_edges (d,dopug1)
		{
		e = corresponding_edge (d,dopug1);
		
		if (e)
			{
			gdtnode u1 = dopug1.source(d);
			gdtnode u2 = dopug1.target(d);
			gdtnode U1 = super_node[u1];
			gdtnode U2 = super_node[u2];
			
			//cout << "\nexamining gdtnode u1 = " << dopug1.id(u1) << flush;
			//cout << "\nexamining gdtnode u2 = " << dopug1.id(u2) << flush;
			
			if (U1)
				{
				//cout << "\nsuper gdtnode of u1 = " << dopug.id(U1) << flush;
				gdtnode  v1  = corresponding_node (U1,dopug);
				gdt::gdtpoint pu1 = dopug1.position_of_node (u1);
				set_anchor_point (e,v1,pu1);
				}
			if (U2)
				{
				//cout << "\nsuper gdtnode of u2 = " << dopug.id(U2) << flush;
				gdtnode  v2  = corresponding_node (U2,dopug);
				gdt::gdtpoint pu2 = dopug1.position_of_node (u2);
				set_anchor_point (e,v2,pu2);
				}
			}
		}
	
	remove_bend_nodes_for_dimensioned_nodes (dopug, dopug1, w, h, super_node);
	
	if (remove_crosses) remove_cross_nodes(); // remove all crosses
	
	// -------------------------------------------------------
	// normalize coordinates and apply some default parameters
	// -------------------------------------------------------
	
	normalize_coordinates();		// multiply all gdtnode- and bend-coordinates for the scale
	forall_edges (e,*this)			// set the label position for the edges 
		{
		gdt::gdtpoint as = anchor_source(e);
		gdt::gdtpoint at = anchor_target(e);
		apply_defaults(e);
		set_anchor_source (e,as);
		set_anchor_target (e,at);
		set_label_position (e, recommended_label_position(e));
		gdtnode v1 = source(e);
		gdtnode v2 = target(e);
		//set_shape (v1,RECTANGLE);
		//set_shape (v2,RECTANGLE);
		set_label_position (v1,recommended_label_position(v1));
		set_label_position (v2,recommended_label_position(v2));
		
		// temporary
		//
		/*
		if (is_marked(v1,RM3_BEND_NODE))
			{
			set_body_color (v1,cyan);
			set_shape      (v1,ELLIPSE);
			set_width      (v1,2);
			set_height     (v1,2);
			}
		if (is_marked(v2,RM3_BEND_NODE))
			{
			set_body_color (v2,cyan);
			set_shape      (v2,ELLIPSE);
			set_width      (v2,2);
			set_height     (v2,2);
			}
		*/
		}
	}



	void 
	draw_undi_graph::
local_init
	(
	int details,
	const dime_orth_plan_undi_graph& dopug
	)
	{
	gdtnode  v;	// gdtnode of *this
	gdtedge  e;	// gdtedge of *this
	gdt::gdtpoint p;
	
	double k = scale();
	
	forall_nodes (v,*this)
		{
		gdtnode v1;
		v1 = dopug.corresponding_node(v,*this);
		p  = dopug.position_of_node(v1); 
		p  = gdt::gdtpoint (p.xcoord()*k+k,p.ycoord()*k+k);
		set_center(v,p);
		apply_defaults(v);
		set_label_position(v,recommended_label_position(v));
		if (is_marked(v,RM3_REPLACES_A_BEND)) 		  
			{
			set_body_color (v,green);
			set_shape (v,ELLIPSE);
			set_width (v,3);
			set_height (v,3);
			}
		if (is_marked(v,RM3_ADDED_BY_RECTANGULARIZATION)) 
			{
			set_body_color(v,green);
			set_shape(v,RECTANGLE);
			set_width (v,3);
			set_height (v,3);
			}
		if (is_marked(v,RM3_BEND_NODE)) 		  
			{
			set_body_color(v,cyan);
			set_shape (v,ELLIPSE);
			set_width (v,3);
			set_height (v,3);
			}
		if (is_marked(v,RM3_CROSS_ON_REAL_EDGE))
			{
			set_body_color(v,red);
			set_shape (v,TRIANGLE);
			set_width (v,3);
			set_height (v,3);
			}
		if (is_marked(v,RM3_NODE_IN_SUPER_NODE))
			{
			set_body_color(v,blue);
			set_shape (v,RECTANGLE);
			set_label_color(v,yellow);
			}
		}

	forall_edges(e,*this)
		{
		gdtnode v1;
		gdtedge e1;
		v   = source(e);
		v1  = dopug.corresponding_node(v,*this);
		e1  = dopug.corresponding_edge(e,*this);
		gdt::gdtlist<gdt::gdtpoint> pl1 = dopug.position_of_bends_along_edge(e1,v1);
		gdt::gdtlist<gdt::gdtpoint> pl2;
		forall (p,pl1) pl2.append (gdt::gdtpoint (p.xcoord()*k+k,p.ycoord()*k+k));
		set_bends(e,pl2);
		apply_defaults(e);
		set_label_position(e,recommended_label_position(e));
		if (details == 0)
			{
			if (is_marked(e,RM3_ADDED_BY_RECTANGULARIZATION)) set_body_color(e,green);
			// thickness??	
			}
		}
	}	
	
	

	
	void
	draw_undi_graph::
local_init
	(const tree& tr, algorithm_type alg, bool remove_crosses)
	{
	#ifdef BOUNDEDNODES	
	 if (count_not_dummy_nodes() > BOUND_ON_NODES)
	 	{
	 	cout << "\nTHIS IS A DEMO VERSION OF GDT."
	      	     << "\nYOU CANNOT RUN A DRAWING ALGORITHM ON GRAPHS WITH MORE THAN "
	             << BOUND_ON_NODES << " NODES."  
		     << "\nTO DO THAT YOU NEED TO GET A COMMERCIAL LICENSE\n\n" << flush;
         	exit(1);
		}
	#endif	
	
	gdtnode v,w;	// Nodes iterator
	gdtedge e;		// Edges iterator
	
	set_scale (draw_curr->scale);
	if (alg == DEFAULT_ALGORITHM) alg = TREE_DRAW_CENTERSONS;
		
	switch (alg)
		{
		//case DEFAULT_ALGORITHM: 
		case TREE_DRAW_CENTERSONS: 
			{			
			gdt::gdtnode_array<int>    lev (tr, 0);
			gdt::gdtnode_array<int>    width_of_subtree (tr, 0);
			gdt::gdtnode_array<double> x (tr, 0.0),
					   y (tr, 0.0);
			int depth;
			gdt::gdtlist<gdtnode> post_order;
			gdt::gdtlist<gdtnode> BFS_order;
			
			// compute width of subtree forall nodes
			//
			forall_nodes (v,tr) width_of_subtree[v] = 0;
			depth = tr.post_order_of_subtree (lev,post_order);
			forall (v,post_order)
				{
				if (tr.degree(v)-1 == 0 && v != tr.root_of_tree())
					width_of_subtree[v] = 1;
				w = tr.father_node(v);
				if (w) width_of_subtree[w] += width_of_subtree[v];
				}
			
			// compute the integer coordinates x and y forall nodes
			//
			tr.BFS_of_subtree (lev,BFS_order);
			forall (v,BFS_order)
				{
				if (v == tr.root_of_tree()) x[v] = 0;
				else 
					{
					w  = tr.father_node (v);
					if (v == tr.first_son_node(w)) x[v] = x[w];
					else 
						{
						gdtnode v1 = tr.son_pred(v,w);
						x[v] = x[v1] + width_of_subtree[v1];  
						}
					}
				y[v] = depth - lev[v];
				}

			// center gdtnode with respect its sons
			//
			forall (v,post_order) 
				{
				gdtnode v1 = tr.first_son_node (v);
				gdtnode v2 = tr.last_son_node (v);
				if (v1) x[v] = x[v1] + (x[v2] - x[v1])/2;
				}

			double k = scale();
			
			forall_nodes (v,tr)
				{
				gdtnode v1 = corresponding_node (v,tr);
				gdt::gdtpoint p (x[v],y[v]);
				gdt::gdtpoint pn(p.xcoord()*k+k,p.ycoord()*k+k);
				set_center (v1,pn);
				}
			break;			
			}
			
		case TREE_DRAW_CENTERSUBTREE:
			{			
			gdt::gdtnode_array<int> lev (tr, 0);
			gdt::gdtnode_array<int> width_of_subtree (tr, 0);
			gdt::gdtnode_array<int> x (tr, 0),
					y (tr, 0);
			
			gdt::gdtnode_array<int> h (tr, 0);
			
			
			// compute width of subtree forall nodes
			//
				{
				gdt::gdtlist<gdtnode> order;
				
				forall_nodes (v,tr) width_of_subtree[v] = 0;
				tr.post_order_of_subtree (lev,order);
				forall (v,order)
					if (!tr.first_son_node(v))
						width_of_subtree[v] = 1;
					else
						{
						gdtnode w;
						int  number_sons = 0;
						forall_son_nodes (w,v,tr)
							{
							h[v] = max(h[v],width_of_subtree[w]);
							number_sons ++;
							}
						width_of_subtree[v] = (h[v]*number_sons);
						if (!(width_of_subtree[v]%2)) width_of_subtree[v]++; 	
						}
						
				}
			
			// compute the integer coordinates x and y forall nodes
			//
				{
				gdt::gdtlist<gdtnode> order;
				int depth; 
				gdtnode w,v1;
				int n;
				depth = tr.BFS_of_subtree (lev,order);

				forall (v,order)
					{
					gdt::gdtlist<gdtnode> L1,L2;
					gdt::list_item  pos;
					
					y[v] = depth - lev[v]; //cout << "\ny[" << tr.id(v) << "]=" << y[v] << flush;
					
					if (v == tr.root_of_tree())
						{
						x[v] = width_of_subtree[v]/2;
						//cout << "\nx[" << tr.id(v) << "]=" << x[v] << flush;
						}
				 	
					n = tr.number_of_sons (v); 
					
					if (n != 0)
						{
						w = tr.first_son_node (v);
						if (n == 1) x[w] = x[v];
						else
							{
							int i;
							for (i=1; i<=n/2; i++) 
								{ L1.push(w); w = tr.son_succ (w,v); }
							if (n%2)
								{
								// central son gdtnode 
								x[w] = x[v];
								w = tr.son_succ (w,v);  
								}
							for (i=1; i<=n/2; i++)
								{ L2.append(w); w = tr.son_succ (w,v); }

							pos= L1.first();
							w  = L1.inf(pos);
							if (n%2) {x[w] = x[v] - h[v]; }
							else	 {x[w] = x[v] - (h[v]+1)/2; }
							pos = L1.succ(pos);
							while (pos) {v1 = w; w = L1.inf(pos); x[w] = x[v1]-h[v]; pos = L1.succ(pos);}

							pos= L2.first();
							w  = L2.inf(pos);
							if (n%2) {x[w] = x[v] + h[v]; }
							else	 {x[w] = x[v] + (h[v]+1)/2; }
							pos = L2.succ(pos);
							while (pos) {v1 = w; w = L2.inf(pos); x[w] = x[v1]+h[v]; pos = L2.succ(pos);}
							}
						}
					}
				}
			
			//set_scale(recommended_scale());
			//cout << "\nSCALE SETTED\n" << flush;
			double k = scale();
			
			forall_nodes (v,tr)
				{
				gdtnode v1 = corresponding_node (v,tr);
				gdt::gdtpoint p (x[v],y[v]);
				gdt::gdtpoint pn(p.xcoord()*k+k,p.ycoord()*k+k);
				set_center (v1,pn);
				}
			break;			
			}
		default:
			break;
		}
		
	forall_nodes (v,*this) apply_defaults (v);
	forall_edges (e,*this) 
		{
		apply_defaults (e);
		set_label_position(e,recommended_label_position(e));
		}
		
	if (remove_crosses) remove_cross_nodes();
	}
	

	void
	draw_undi_graph::
local_init 
	(const draw_undi_graph& dug, bool remove_crosses)
	{
	gdtnode v,v1;
	gdtedge e,e1;
	*draw_curr = dug.draw_status  ();
	*draw_defs = dug.draw_defaults();
	*node_defs = dug.node_defaults();
	*edge_defs = dug.edge_defaults();
	
	forall_nodes (v,dug)
		{
		v1 = corresponding_node (v,dug);
		set_center 		(v1,dug.center(v));
		set_shape  		(v1,dug.shape(v));
		set_width 		(v1,dug.width(v));
		set_height 		(v1,dug.height(v));
		set_altitude 	        (v1,dug.altitude(v));
		set_body_color          (v1,dug.body_color(v));
		set_border_color 	(v1,dug.border_color(v));
		set_label_color 	(v1,dug.label_color(v));
		set_label 		(v1,dug.label(v));
		set_label_position 	(v1,dug.label_position(v));
		}
	forall_edges (e,dug)
		{
		e1 = corresponding_edge (e,dug);
		gdt::gdtlist<gdt::gdtpoint> bp = dug.bends (e,dug.source(e));
		
		//set_anchor_source   (e1,dug.anchor_source(e));
		//set_anchor_target   (e1,dug.anchor_target(e));		
		
		if (id(source(e1)) == dug.id(dug.source(e)))
			{
			set_anchor_source   (e1,dug.anchor_source(e));
			set_anchor_target   (e1,dug.anchor_target(e));
			}
		else
			{
			set_anchor_source   (e1,dug.anchor_target(e));
			set_anchor_target   (e1,dug.anchor_source(e));
			}
		
		set_bends           (e1,bp);
		set_body_color      (e1,dug.body_color(e));
		set_label           (e1,dug.label(e));
		set_label_color     (e1,dug.label_color(e));
		set_label_position  (e1,dug.label_position(e));
		set_shape           (e1,dug.shape(e));
		}
	if (remove_crosses) remove_cross_nodes();
	}

	

	void
	draw_undi_graph::	
remove_bend_nodes_for_dimensioned_nodes 
	(
	const dime_orth_plan_undi_graph& dopug, 
	const dime_orth_plan_undi_graph& dopug1,
	const gdt::gdtnode_array<int>&		 W,
	const gdt::gdtnode_array<int>&           H,		
	const gdt::gdtnode_map<gdtnode>&            super_node
	)
	{
	double  round = 0.05,
		dev   = 0, 
		devx  = 0, 
		devy  = 0;

	// ----------------------- 1 --------------------------
	// create a list L of initial border steps to expand
	// ----------------------------------------------------
	//
	border_step s;
	gdt::gdtlist<border_step> L; // contains border steps of dopug1
	gdtedge e;
	
	forall_edges (e,dopug1)
		{
		border_step s1 = dopug1.border_step_moving_along_edge (e,dopug1.source(e)),
			    s2 = dopug1.border_step_moving_along_edge (e,dopug1.target(e));
		
		int	    t1 = dopug1.thickness_of_border_step(s1),
			    t2 = dopug1.thickness_of_border_step(s2);
		
		gdtnode 	    v11 = dopug1.start_of_border_step(s1),
			    v12 = dopug1.stop_of_border_step (s1),
			    v21 = dopug1.start_of_border_step(s2),
			    v22 = dopug1.stop_of_border_step (s2);
			    
		if ((t1 > 1) && 
		    ((dopug1.is_marked(v11,RM3_BEND_NODE) && !dopug1.is_marked(v12,RM3_BEND_NODE)) ||
		     (dopug1.is_marked(v12,RM3_BEND_NODE) && !dopug1.is_marked(v11,RM3_BEND_NODE))))
		     L.append (s1);
		     
		if ((t2 > 1) &&		    
		    ((dopug1.is_marked(v21,RM3_BEND_NODE) && !dopug1.is_marked(v22,RM3_BEND_NODE)) ||
		     (dopug1.is_marked(v22,RM3_BEND_NODE) && !dopug1.is_marked(v21,RM3_BEND_NODE))))
		     L.append (s2);
		}
	
		
	// ----------------------- 2 --------------------------
	// remove the bend nodes, following the ordering of L
	// ----------------------------------------------------
	//
	//   |---|		|---|
	//   |   |              |   |
	//   |---|              |---|
	//     |                  |
	//     |d                 |d    
	//     O------b           O------b
	//     |c        ==>      |c
	//  ue O------a           Oo-----a
	//     |  en              ||
	//     |e                e||
	//   |---|              |---|
	//   |ve |              |   |
	//   |---|              |---|
	
	forall (s,L)
		{
		int  ts  = dopug1.thickness_of_border_step (s);
		
		gdtedge es  = dopug1.edge_of_border_step(s);
		gdtedge  e  = corresponding_edge (es,dopug1);

		gdtnode v1s = dopug1.start_of_border_step (s);
		gdtnode v2s = dopug1.stop_of_border_step  (s);
		gdtnode V1s = super_node[v1s];		
		gdtnode v1  = corresponding_node (V1s,dopug);
		
		gdtnode ves = NULL_NODE;
		gdtnode ues = NULL_NODE;   // real gdtnode 'ves' and bend gdtnode 'ues' to expand in dopug.
		gdtnode Ves;               // super node of dopug of 'ves'
		gdtnode ve;                // node corresponding to 'Ves' in this.
		gdtnode ue;                // node corresponding to 'Ues' in this. 
		         
		
		if      (dopug1.is_marked(v1s,RM3_BEND_NODE)) {ues = v1s; ves = v2s;}
		else if (dopug1.is_marked(v2s,RM3_BEND_NODE)) {ues = v2s; ves = v1s;}
		else gdt_error("no gdtedge consistency");
		
		Ves = super_node[ves];
		ve  = corresponding_node (Ves,dopug);
		ue = corresponding_node (ues,dopug1);
		
		dev = 1;	
		bool horizontal_shift = true;
		
		gdt::gdtpoint ave = anchor_point(e,ve);
		gdt::gdtpoint aue = anchor_point(e,ue);
		
		gdt::gdtpoint  bend;		// bend that will replace the bend-gdtnode u on the new gdtedge
		gdt::gdtpoint  anch;
		
		double ve_x = (center(ve)).xcoord();
		double ve_y = (center(ve)).ycoord();
		double width  = W[Ves];
		double height = H[Ves];

		
		double ddx; 	// distance between two edges with zero angle in the dx direction
		double ddy;	// distance between two edges with zero angle in the dy direction
		
		double dx = 0;
		double dy = 0;


		/* BEFORE Titto changed 04/05/2004		
        
		if (width == 0)  ddx = (double)1/(4*((double)(ts)));
		else	 	 ddx = (double)5/(8*(double)(ts));
		
		if (height == 0) ddy = (double)1/(4*((double)(ts)));
		else		 ddy = (double)5/(8*(double)(ts));
		*/

		if (width == 0)  ddx = 0.25/((double)(ts));  // when the side is small
		else	 	 ddx = 0.625/((double)(ts)); // otherwise
		
		if (height == 0) ddy = 0.25/((double)(ts));
		else		 ddy = 0.625/((double)(ts));


		double adx = max (ave.xcoord() - aue.xcoord(), aue.xcoord() - ave.xcoord());
		double ady = max (ave.ycoord() - aue.ycoord(), aue.ycoord() - ave.ycoord());
				
		// --------------------- 2.1 ------------------
		// Compute the distance and direction to expand 
		// --------------------------------------------
		//
		// ------------------------------------------------
		// CASE 1 (ve and ue have the same x coordinates)
		// ------------------------------------------------    
		
		if (adx < round) //move x
			{
			dy   = 0;
			devx = 0;
			horizontal_shift = true;
			
			if (v1 == ve) // move x to right of border step
				if (aue.ycoord() > ave.ycoord()) {dx = +ddx; devy = +dev;} 
				else				 {dx = -ddx; devy = -dev;}
				
			else 	      // move x to left of border step
				if (aue.ycoord() > ave.ycoord()) {dx = -ddx; devy = +dev;}
				else				 {dx = +ddx; devy = -dev;}
			}
			
		// ------------------------------------------------
		// CASE 2 (ve and ue have the same y coordinates)
		// ------------------------------------------------
		
		else
		if (ady < round) //move y
			{
			dx   = 0;
			devy = 0;
			horizontal_shift = false;
			
			if (v1 == ve) // move y to right
				if (aue.xcoord() > ave.xcoord()) {dy = -ddy; devx = +dev;} 
				else				 {dy = +ddy; devx = -dev;}
					
			else	     // move y to left 
				if (aue.xcoord() > ave.xcoord()) {dy = +ddy; devx = +dev;} 
				else				 {dy = -ddy; devx = -dev;}	
			}
		
		
		//cout << "\ndx = " << dx << flush;
		//cout << "\ndy = " << dy << flush;
		
		// ---------------------- 2.2 ---------------------
		// expand all the border steps starting from s
		// ------------------------------------------------
			
		bool forward = true;
		if (ves == dopug1.start_of_border_step(s)) forward = true;
		else				           forward = false;
		
		while (ts > 1)
			{
			// -----------------------------
			// examine current border step s
			// -----------------------------
			//
			gdtedge ens, en, e_ve_w;
			gdtnode w;
			
			//-----------
			//
			double x,y;
			x = aue.xcoord() + dx*(ts-1);
			y = aue.ycoord() + dy*(ts-1); 
			
			gdt::gdtpoint bend = gdt::gdtpoint (x,y);
			
			gdt::gdtpoint p    = gdt::gdtpoint(ve_x + (width/2.0 + 0.25) * devx , ve_y + (height/2.0 + 0.25) * devy);
			gdt::gdtpoint anch;
			if (horizontal_shift) anch = gdt::gdtpoint (x, p.ycoord());
			else 		      anch = gdt::gdtpoint (p.xcoord(), y);
			
			//cout << "\nnode     " << id (ve) << ":" << flush;
			//cout << "\nwidth  = " << width  << flush;
			//cout << "\nheight = " << height << flush;
			
			//
			// ------------
			
			gdt::gdtlist<gdt::gdtpoint> b_ve_w;
			
			angle_type alpha;
			if (forward) alpha = dopug1.angle_of_border_step(s);
			else	
				{
				border_step s_pred = dopug1.cyclic_pred_border_step(s);
				alpha = dopug1.angle_of_border_step(s_pred);
				}
			
			if (alpha == _090)
				{
				if (forward)		// ues is the stop of s
					{ 
					ens    = dopug1.cyclic_adj_succ (es,ues);
					en     = corresponding_edge (ens,dopug1);
					b_ve_w = bends (en,ue);
					b_ve_w.push(bend);
					w      = opposite(ue,en);
					e_ve_w = new_edge (ve,e,w,en,b_ve_w,AUTO_ID,gdt::before);
					}
				else			// ues is the start of s
					{
					ens    = dopug1.cyclic_adj_pred (es,ues);
					en     = corresponding_edge (ens,dopug1);
					b_ve_w = bends (en,ue);
					b_ve_w.push(bend); 
					w      = opposite(ue,en);
					e_ve_w = new_edge (ve,e,w,en,b_ve_w,AUTO_ID,gdt::after);
					}
			
				int   iden = id(en); 
			
				set_anchor_point (e_ve_w,ve,anch);
				set_anchor_target (e_ve_w,anchor(en,w));
			
				//cout << "\nedge " << id(en) << ",  ts = " << ts << flush;
				//cout << "\ncue  = " << "(" << cue.xcoord() << "," << cue.ycoord() << ")" << flush;
				//cout << "\nbend = " << "(" << bend.xcoord() << "," << bend.ycoord() << ")" << flush;
				//cout << "\nanch = " << "(" << anch.xcoord() << "," << anch.ycoord() << ")" << flush;

				gdtnode sen = start(en);
				if (sen)
					if (sen == ue) make_directed(e_ve_w,ve);
					else	       make_directed(e_ve_w,w);
			
				// -----------------------------------------------------------
				// upadte all directions, markers and constraints on new gdtedge
				// -----------------------------------------------------------

				mark(e_ve_w,markers(en));
				update_constraints_after_edge_translation (en,ue,w,e_ve_w,ve,w);
				
				del_edge  (en);
				//Added by Marcandalli 8 2001
				edge_info->undefine(en);
				//
				assign_id (e_ve_w,iden);
			
				// ---------------------------------------
				// choose the next border step s to expand
				// ---------------------------------------
				//
				if (forward) 
					{
					es = dopug1.cyclic_adj_succ (ens,ues); 
					s  = dopug1.border_step_moving_along_edge (es,ues);
					}
				else		
					{      
					es = dopug1.cyclic_adj_pred (ens,ues);
					s  = dopug1.border_step_moving_along_edge (es,dopug1.opposite(ues,es));
					}
				//
				// update and check the thickness
				//
				int ts_old = ts;
				ts  = dopug1.thickness_of_border_step (s);
				if (ts_old <= ts)
					gdt_error("thickness inconsistency");
				//
				if (ts > 1)
					{
					ues = dopug1.opposite(ues,es);
					v1s = dopug1.start_of_border_step (s);
					v2s = dopug1.stop_of_border_step  (s);
				
					//Ues = super_node[ues];
					//ue  = corresponding_node (Ues,dopug);

					ue = corresponding_node(ues,dopug1);
				
					gdtedge e_temp = corresponding_edge (es,dopug1); 
		      			aue = anchor_point (e_temp,ue);
					}
				}
				
			else if (alpha == _180) // skip border step
				{
				if (forward) s = dopug1.cyclic_succ_border_step (s);
				else	     s = dopug1.cyclic_pred_border_step (s);				
				es  = dopug1.edge_of_border_step(s);
				//
				// update and check the thickness
				//
				int ts_old = ts;
				ts  = dopug1.thickness_of_border_step (s);
				if (ts_old != ts) 
					gdt_error("thickness inconsistency");
				//
				if (ts > 1)
					{			
					ues = dopug1.opposite(ues,es);
					v1s = dopug1.start_of_border_step (s);
					v2s = dopug1.stop_of_border_step  (s);
				
					//Ues = super_node[ues];
					//ue  = corresponding_node (Ues,dopug);
					
					ue = corresponding_node(ues,dopug1);
				
					gdtedge e_temp = corresponding_edge (es,dopug1);
		      			aue = anchor_point (e_temp,ue);
					}
				}

			else gdt_error("angle inconsistency");
			}
		}
	
		
	// ---------------------- 3 -----------------------
	// remove all bend nodes that, now, have degree two 
	// ------------------------------------------------
	
	gdtnode u;        
	gdt::gdtlist<gdtnode> Ln = all_nodes(); 
	forall(u,Ln)
		if (is_marked(u,RM3_BEND_NODE) || 
		    is_marked(u,RM3_REPLACES_A_BEND))
			remove_two_degree_node (u);
	}	
	
	
	void
	draw_undi_graph::
remove_bend_nodes
	(dime_orth_plan_undi_graph& dopug)
	{
	// debugging
	//
	//	{
	//	draw_undi_graph dug1 (0,dopug);
	//	dug1.write("dopug.gdt");	
	//	write("dug.gdt");
	//	
	//	cout << "\n DOPUG : " << flush;
	//	dopug.print(COMPLETE);
	//	
	//	cout << "\n DUG : " << flush;
	//	print(BY_EDGES);
	//	}
	//

	double  round = 0.05,
		dev  = 0, 
		devx = 0, 
		devy = 0;
	
	// ----------------------- 1 --------------------------
	// create a list L of initial border steps to expand
	// ----------------------------------------------------
	//
	border_step s;
	gdt::gdtlist<border_step> L;
	gdtedge e;
	
	forall_edges (e,dopug)
		{
		border_step s1 = dopug.border_step_moving_along_edge (e,dopug.source(e)),
			    s2 = dopug.border_step_moving_along_edge (e,dopug.target(e));
		
		int	    t1 = dopug.thickness_of_border_step(s1),
			    t2 = dopug.thickness_of_border_step(s2);
		
		gdtnode 	    v11 = dopug.start_of_border_step(s1),
			    v12 = dopug.stop_of_border_step (s1),
			    v21 = dopug.start_of_border_step(s2),
			    v22 = dopug.stop_of_border_step (s2);
			    
		//cout << "\nDime gdtedge e " << dopug.id(e) << "(" << dopug.id(source(e)) << "," << dopug.id(target(e)) << ")" << flush;
		//cout << "\n  thickness of its border step from source = " << t1 << flush;
		//cout << "\n  thickness of its border step from target = " << t2 << flush;
		
		if ((t1 > 1) &&
		    ((dopug.is_marked(v11,RM3_BEND_NODE) && !dopug.is_marked(v12,RM3_BEND_NODE)) ||
		     (dopug.is_marked(v12,RM3_BEND_NODE) && !dopug.is_marked(v11,RM3_BEND_NODE))))
		     {
		     L.append (s1);
		     //cout << "\n    added s from source in list of initial border step" << flush;
		     }

		if ((t2 > 1) &&		    
		    ((dopug.is_marked(v21,RM3_BEND_NODE) && !dopug.is_marked(v22,RM3_BEND_NODE)) ||
		     (dopug.is_marked(v22,RM3_BEND_NODE) && !dopug.is_marked(v21,RM3_BEND_NODE))))
		     {
		     L.append (s2);
		     //cout << "\n    added s from target in list of initial border step" << flush;
		     }
		}
	
		
	// ----------------------- 2 --------------------------
	// remove the bend nodes, following the ordering of L
	// ----------------------------------------------------
	//
	//   |---|		|---|
	//   |   |              |   |
	//   |---|              |---|
	//     |                  |
	//     |d                 |d    
	//     O------b           O------b
	//     |c        ==>      |c
	//  ue O------a           Oo-----a
	//     |  en              ||
	//     |e                e||
	//   |---|              |---|
	//   |ve |              |   |
	//   |---|              |---|

	forall (s,L)
		{
		int  ts  = dopug.thickness_of_border_step (s);
		
		gdtedge es  = dopug.edge_of_border_step(s) ,	 
		     e   = corresponding_edge (es,dopug);
		     
		gdtnode v1s = dopug.start_of_border_step (s),
		     v2s = dopug.stop_of_border_step  (s),
		     v1  = corresponding_node (v1s,dopug);
		
		gdtnode ves = NULL_NODE,
		     ues = NULL_NODE,	// real gdtnode 'ves' and bend gdtnode 'ues' to expand in dopug
		     ve ,ue ;   	// nodes corresponding to 'ves' and 'ues' in this     
		
		if      (dopug.is_marked(v1s,RM3_BEND_NODE)) {ues = v1s; ves = v2s;}
		else if (dopug.is_marked(v2s,RM3_BEND_NODE)) {ues = v2s; ves = v1s;}
		else gdt_error("no gdtedge consistency");
		
		ve = corresponding_node (ves,dopug);
		ue = corresponding_node (ues,dopug);     
		
		dev = 0.25;	
		
		//gdt::gdtpoint cve = center(ve),
		//      cue = center(ue);
		
		// debugging
		//
		//cout << "\n examining dime gdtedge " << dopug.id(es) << flush;
		//cout << "\n thickness = " << ts << flush;
		//cout << "\n exmaining draw gdtedge " << id(e) << flush;
		//cout << "\n gdtnode ve = " << id(ve) << flush;
		//cout << "\n gdtnode ue = " << id(ue) << flush;
		//
				
		gdt::gdtpoint ave = anchor_point (e,ve);
		gdt::gdtpoint aue = anchor_point (e,ue);

		gdt::gdtpoint  bend;	            // bend that will replace the bend-gdtnode u on the new gdtedge
		double d = 0.25/((double)(ts));	    // distance between two edges with zero angle
		double dx = 0;
		double dy = 0;
		gdt::gdtpoint  anch;
		
		//cout << "\nts = " << ts << ",  d = " << d << flush;

		//double cdx = max (cve.xcoord() - cue.xcoord(), cue.xcoord() - cve.xcoord()),
		//       cdy = max (cve.ycoord() - cue.ycoord(), cue.ycoord() - cve.ycoord());
		
		double adx = max (ave.xcoord() - aue.xcoord(), aue.xcoord() - ave.xcoord()),
		       ady = max (ave.ycoord() - aue.ycoord(), aue.ycoord() - ave.ycoord());
				
		// --------------------- 2.1 ------------------
		// Compute the distance and direction to expand 
		// --------------------------------------------
		//
		// ------------------------------------------------
		// CASE 1 (ve and ue have the same x coordinates)
		// ------------------------------------------------    
		
		if (adx < round) //move x
			{
			dy   = 0;
			devx = 0;
			
			if (v1 == ve) // move x to right of border step
				if (aue.ycoord() > ave.ycoord()) {dx = +d; devy = +dev;} 
				else				 {dx = -d; devy = -dev;}
				
			else 	      // move x to left of border step
				if (aue.ycoord() > ave.ycoord()) {dx = -d; devy = +dev;}
				else				 {dx = +d; devy = -dev;}
			}

		// ------------------------------------------------
		// CASE 2 (ve and ue have the same y coordinates)
		// ------------------------------------------------
		
		else
		if (ady < round) //move y
			{
			dx   = 0;
			devy = 0;

			if (v1 == ve) // move y to right
				if (aue.xcoord() > ave.xcoord()) {dy = -d; devx = +dev;} 
				else				 {dy = +d; devx = -dev;}
					
			else	     // move y to left 
				if (aue.xcoord() > ave.xcoord()) {dy = +d; devx = +dev;} 
				else				 {dy = -d; devx = -dev;}	
			}
		
		
		//cout << "\ndx = " << dx << flush;
		//cout << "\ndy = " << dy << flush;
		
		
		// ---------------------- 2.2 ---------------------
		// expand all the border steps starting from s
		// ------------------------------------------------
		
		// -----------------------------------------------------
		// this portion of code must be deleted
		// when the case of straight thick edges will be removed
		// ------------------from here-----------------------------
		// to skip the case of straight thick edges
		//
		//bool forward = true;
		//if (ves == dopug.start_of_border_step(s)) forward = true;
		//else					  forward = false;
		//
		//
		//
		//bool exit = false;
		//if (!dopug.is_marked(v1s,RM3_BEND_NODE) && 
		//   !dopug.is_marked(v2s,RM3_BEND_NODE) ) exit = true;
		//
		//while (ts > 1 && !exit)
		// --------------------to here--------------------------
		
		bool forward = true;
		if (ves == dopug.start_of_border_step(s)) forward = true;
		else					  forward = false;
		
		
		while (ts > 1)
			{
			// -----------------------------
			// examine current border step s
			// -----------------------------
			//
			gdtedge ens, en, e_ve_w;
			gdtnode w;
			
			gdt::gdtpoint bend = gdt::gdtpoint (aue.xcoord() + dx*(ts-1), aue.ycoord() + dy*(ts-1));
			gdt::gdtpoint anch = gdt::gdtpoint (dx*(ts-1)+devx, dy*(ts-1)+devy);
			
			gdt::gdtlist<gdt::gdtpoint> b_ve_w;
			
			angle_type alpha;
			if (forward) alpha = dopug.angle_of_border_step(s);
			else	
				{
				border_step s_pred = dopug.cyclic_pred_border_step(s);
				alpha = dopug.angle_of_border_step(s_pred);
				}
			
			if (alpha == _090)	// expand border step
				{
				if (ues == v2s)		// ues is the stop of s
					{ 
					ens    = dopug.cyclic_adj_succ (es,ues);
					en     = corresponding_edge (ens,dopug);
					b_ve_w = bends (en,ue);
					b_ve_w.push(bend); 
					w      = opposite(ue,en);
					e_ve_w = new_edge (ve,e,w,en,b_ve_w,AUTO_ID,gdt::before);
					}
				else			// ues is the start of s
					{
					ens    = dopug.cyclic_adj_pred (es,ues);
					en     = corresponding_edge (ens,dopug);
					b_ve_w = bends (en,ue);
					b_ve_w.push(bend); 
					w      = opposite(ue,en);
					e_ve_w = new_edge (ve,e,w,en,b_ve_w,AUTO_ID,gdt::after);
					}
			
				int   iden = id(en); 
			
				set_anchor_source (e_ve_w,anch);
				set_anchor_target (e_ve_w,anchor(en,w));
			
				//cout << "\nedge " << id(en) << ",  ts = " << ts << flush;
				//cout << "\ncue  = " << "(" << cue.xcoord() << "," << cue.ycoord() << ")" << flush;
				//cout << "\nbend = " << "(" << bend.xcoord() << "," << bend.ycoord() << ")" << flush;
				//cout << "\nanch = " << "(" << anch.xcoord() << "," << anch.ycoord() << ")" << flush;
			
				gdtnode sen = start(en);
				if (sen)
					if (sen == ue) make_directed(e_ve_w,ve);
					else	       make_directed(e_ve_w,w);
			
				// -----------------------------------------------------------
				// upadte all directions, markers and constraints on new gdtedge
				// -----------------------------------------------------------
			
				mark(e_ve_w,markers(en));
				update_constraints_after_edge_translation (en,ue,w,e_ve_w,ve,w);
				
				del_edge  (en);
				//Added by Marcandalli 8 2001
				edge_info->undefine(en);
				//
				assign_id (e_ve_w,iden);
			
				// ---------------------------------------
				// choose the next border step s to expand
				// ---------------------------------------
				//
				if (forward) 
					{
					es = dopug.cyclic_adj_succ (ens,ues); 
					s  = dopug.border_step_moving_along_edge (es,ues);
					}
				else		
					{      
					es = dopug.cyclic_adj_pred (ens,ues);
					s  = dopug.border_step_moving_along_edge (es,dopug.opposite(ues,es));
					}
				//
				// update and check the thickness
				//
				int ts_old = ts;
				ts  = dopug.thickness_of_border_step (s);
				if (ts_old <= ts)
					{
					cout << "\nBorder step of gdtedge " << dopug.id(dopug.edge_of_border_step(s))
					     << " from gdtnode "            << dopug.id(dopug.start_of_border_step(s))
					     << " to gdtnode "              << dopug.id(dopug.stop_of_border_step(s))
					     << " has thickness "        << dopug.thickness_of_border_step(s)
					     << flush;
					gdt_error("thickness inconsistency");
					}
				//
				if (ts > 1)	// to avoid inconsistencies on the reference gdtedge (straight)
					{
					ues = dopug.opposite(ues,es);
					v1s = dopug.start_of_border_step (s);
					v2s = dopug.stop_of_border_step  (s);
					
					ue  = corresponding_node (ues,dopug);
					gdtedge e_temp = corresponding_edge (es,dopug);
				
		      			//cue = center(ue);
					aue = anchor_point(e_temp,ue);
					}
				}
				
			else if (alpha == _180) // skip border step
				{
				if (forward) s = dopug.cyclic_succ_border_step (s);
				else	     s = dopug.cyclic_pred_border_step (s);				
				es  = dopug.edge_of_border_step(s);
				//
				// update and check the thickness
				//
				int ts_old = ts;
				ts  = dopug.thickness_of_border_step (s);
				if (ts_old != ts) 
					{ 
					cout << "\n Border step of gdtedge "<< dopug.id(dopug.edge_of_border_step(s))
					     << " from gdtnode "            << dopug.id(dopug.start_of_border_step(s))
					     << " to gdtnode "              << dopug.id(dopug.stop_of_border_step(s))
					     << " has thickness "        << dopug.thickness_of_border_step(s)
					     << flush;
					gdt_error("thickness inconsistency");
					}
				//
				if (ts > 1)	// to avoid inconsistencies on the reference gdtedge (straight)
					{
					ues = dopug.opposite(ues,es);
					v1s = dopug.start_of_border_step (s);
					v2s = dopug.stop_of_border_step  (s);
					ue  = corresponding_node (ues,dopug);
		      			//cue = center(ue);
					
					gdtedge e_temp = corresponding_edge (es,dopug);
					aue = anchor_point(e_temp,ue);
					}
				}
				
			else gdt_error("angle inconsistency");
			}
		}

		
	// ---------------------- 3 -----------------------
	// remove all bend nodes that, now, have degree two 
	// ------------------------------------------------
	
	// Modified on April 7
	//

        gdt::gdtlist<gdtnode> Liter(all_nodes());

	gdtnode u;
	forall (u,Liter)
		if (is_marked(u,RM3_BEND_NODE) ||
		    is_marked(u,RM3_REPLACES_A_BEND))
			remove_two_degree_node (u);
	
	
	// Commented on April 7	
	// ---------------------- 4 -----------------------
	// normalize the coordinates of the drawing 
	// ------------------------------------------------
	//
	//normalize_coordinates();
	}


	// ----------------------------------- 
	// Methods for refined orth-compaction 
	// -----------------------------------
	

	direction_type
	draw_undi_graph::
direction_of_edge
	(gdtedge e, gdtnode v)
	{
	gdt::gdtpoint sp = anchor_point(e, v);	// the start point of the gdtedge e from the gdtnode v
	gdt::gdtpoint dp;
	if(number_of_bends(e) == 0)
		dp = anchor_point(e, opposite(v, e));
	else
		{
		gdt::gdtlist<gdt::gdtpoint> lp = bends(e, v);
		// Case of more than one bends
		//
		// Force the first bend of the list to be the nearest to gdtnode v
		//
		if (!(lp.head().xdist(center(v)) < 0.25 || lp.head().ydist(center(v)) < 0.25))
			{
			lp.reverse_items();
			//cout << "   Reverse list of bends in direction_of_edge\n";
			}
		// Now the first segment is the distance between the first bend and the center of gdtnode n
		//
		dp = lp.head();
		}
	double dx = dp.xcoord() - sp.xcoord();
	double dy = dp.ycoord() - sp.ycoord();
	if ((int)floor(dy + 0.5) == 0)
		{
		if (dx < 0)
			return LEFT;
		else
			return RIGHT;
		}
	else
		{
		if (dy > 0)
			return UP;
		else
			return DOWN;
		}
	}



	gdt::gdtlist<gdtedge>
	draw_undi_graph::
edges_in_direction
	(gdtnode v, direction_type d)
	{
	// The circular embedding is considered
	// A group of edges on a certain side of gdtnode v has adjacent position in the list of edges
	// except for the case in which the group is part on the end and part on the begin of list
	//
	// This method groups all the edges on side d from the begin to the end of the natural order,
	// removing the eventual multiple parts in the list of edges
	//
	bool unique = true;	// unique = true means the group has certainly adjacent position
	gdt::gdtlist<gdtedge> l;
	//
	// For connected graph the list adj_edge is never empty
	//
	bool find = false;	// find = true means at least one gdtedge has been found
	if(direction_of_edge(adj_edges(v).head(), v) == d)
		{
		unique = false;
		find = true;
		l.append(adj_edges(v).head());
		}
	int i = 1;
	int len = adj_edges(v).length();
	bool cont = true;
	while(i < len && cont)
		{
		gdt::gdtlist<gdtedge> adj = adj_edges(v);
		if (unique) {			
			if (direction_of_edge(adj.inf(adj.get_item(i)), v) == d)
				{
				find = true;
				l.append(adj.inf(adj.get_item(i)));
				}
			else
				{
				if (find)
					cont = false;
				}
		}
		else 
			// find is certainly at true
			if (direction_of_edge(adj.inf(adj.get_item(i)), v) == d)
				l.append(adj.inf(adj.get_item(i)));
			else
				cont = false;		
		++i;
		}
	if (!unique && i < len)
		{
		int j = len - 1;
		cont = true;
		while(cont)
			{
			if (direction_of_edge(adj_edges(v).inf(adj_edges(v).get_item(j)), v) == d)
				l.push(adj_edges(v).inf(adj_edges(v).get_item(j)));
			else
				cont = false;
			--j;
			}
		}
	return l;
	}



	direction_type
	draw_undi_graph::
direction_of_second_segment_of_edge
	(gdtedge e, gdtnode v)
	{
	gdt::gdtlist<gdt::gdtpoint> B = bends(e, v);	// list of bends of gdtedge e
	if (B.empty())
		{
		cerr << "GDT error    in draw_undi_graph::direction_of_second_segment_of_edge"
		     << "The gdtedge considered have one segment (no bends)" << endl << flush;
		B.clear();
		exit(EXIT_FAILURE);
		//
		//				!!!!!!!!!!!!!!!
		//
		// If in the enum direction_type is present a NULL_DIRECTION,
		// instead of report an error and exit the program,
		// it can handle this error in a better way
		//
		//				!!!!!!!!!!!!!!!
		//
		}
	else
		{
		gdt::gdtpoint sp = B.head();	// sp is the start point of the second segment
		gdt::gdtpoint dp;		// dp is the end point of second segment, the direction point
		// The dp may be another bend or the anchor_point of the opposite gdtnode of v
		//
		if (B.length() > 1)
			// Case of dp is a bend
			//
			dp = B.inf(B.get_item(1));
		else
			// Case of dp is the anchor_point of opposite gdtnode
			dp = anchor_point(e, opposite(v, e));
		B.clear();
		double dx = dp.xcoord() - sp.xcoord();
		double dy = dp.ycoord() - sp.ycoord();
		if ((int)floor(dy + 0.5) == 0)
			{
			if (dx < 0)
				return LEFT;
			else
				return RIGHT;
			}
		else
			{
			if (dy > 0)
				return UP;
			else
				return DOWN;
			}
		}
        gdt_error("unhandled case");
	return UP;	// to avoid warnings
	}



	gdtnode
	draw_undi_graph::
node_with_center(gdt::gdtpoint p)
	{
	gdtnode v;
	forall_nodes (v,*this) if (p.distance(center(v)) <= 0.01) return v;
	return NULL_NODE;
	}
	
	
	
	gdtedge
	draw_undi_graph::
find_orthogonal_edge
	(gdt::gdtpoint p) const
	{
	gdtedge   e;	// current gdtedge
	gdt::gdtlist<gdt::gdtpoint> pl;	// bends of the current gdtedge
	gdt::gdtpoint p1,p2;	// end-points of the current segment
	double d = 0;	// distance of p from the current segment.

	double d_min = DBL_MAX; // minimal point-segment distance from p
	gdtedge   e_min = nil;       // gdtedge containg the minimal-distance segment from p

	double x,y;   // coords of p, the point clicked by the user
	double x1,y1; // coords of p1, the first  end-point of the current segment
	double x2,y2; // coords of p2, the second end-point of the current segment
	gdt::gdtpoint  pk;		// intersection between lines LA and LB
	gdtnode s,t;
	gdt::list_item pos;

	forall_edges(e,*this)
		{
		s = source(e);
		t = target(e);

		pl = bends (e,s);
		pl.push   (center(s));
		pl.append (center(t));		
		
		pos = pl.first();
		p1 = pl.inf(pos);
		pos = pl.succ(pos);
		while(pos)
			{
			// p1,p2 are the end-point of the current segment of gdtedge e
			// p is the point clicked by the user

			p2 = pl.inf(pos);
			x  = p.xcoord();
			y  = p.ycoord();
			x1 = p1.xcoord();
			y1 = p1.ycoord();
			x2 = p2.xcoord();
			y2 = p2.ycoord();

			// Horizontal gdtedge
			//
			if (y1==y2)
				if (x1 < x2)
					if      (x < x1) d =  p.distance (p1);
					else if (x > x2) d =  p.distance (p2);
					else             d =  fabs(y-y1);
				else
					if      (x < x2) d =  p.distance (p2);
					else if (x > x1) d =  p.distance (p1);
					else             d =  fabs(y-y1);

			// Vertical gdtedge
			//
			else if (x1==x2)
				if (y1 < y2) 
					if      (y < y1) d =  p.distance (p1);
					else if (y > y2) d =  p.distance (p2);
					else             d =  fabs(x-x1);
				else
					if      (y < y2) d =  p.distance (p2);
					else if (y > y1) d =  p.distance (p1);
					else             d =  fabs(x-x1);

			// Generic gdtedge
			//
			else
				{
				}
			// Now d contains the distance from p
			// of the current segment. Check it,
			// and update d_min when needed.
			//
			if (d < d_min)
				{
				e_min = e;
				d_min = d;
				}

			// Examine next segment of gdtedge e
			// 
			p1=p2;
			pos = pl.succ(pos);
			}
		}
		
	// All edges have been examined
	// e_min contains now the candidate gdtedge,
	// and d_min is its minimal distance from p.

	if (d_min < 0.25) return e_min;
	else return NULL_EDGE;
	}

		
	bool
	draw_undi_graph::
position_is_compatible(gdt::gdtpoint d, gdtnode n, gdtedge e, direction_type d2, std::set<gdt::gdtpoint,ltpt>& C, int k, int j, int& j_start)
	{
	// The gdtnode in the translated position must not be over others nodes or edges
	// The second segment of gdtedge must not cross others gdtnode
	// cout << "\t position_is_compatible\n";
	// cout << "\t position to be controlled is " << d.xcoord() << " " << d.ycoord() << "\n";
	int error = 0;
	//
	// error = 0  means NO error, real contraction is possible
	// error = 1  means incompatibility due to an gdtedge
	// error = 2  means incompatibiltiy due to a gdtnode
	// error = 3  means no more convenience in the research respect to the original position
	//
	gdtnode victim = node_with_center(d);
	if (victim == NULL_NODE)
		{
		// cout << "\t In position " << d.xcoord() << " " << d.ycoord() << " node_with_center() have found a NULL_NODE\n";
		int l;
		gdt::gdtpoint aux = d;
		for(l = k; l >= 1; --l)
			{
			//cout << "\t in the most internal cicle\n";
			//cout << "\t k = " << k << "  l = " << l << "\n";
			//cout << "\t Verify for no gdtedge\n";
			gdtedge vict = find_orthogonal_edge(aux);
			if (vict != NULL_EDGE)
				{
				//cout << "\t Found the gdtedge " << id(vict) << "\n";
				if (vict == e)
					{
					//cout << "\t  Case of vict == e\n";
					//if (C.member(aux))
					if (C.find(aux) != C.end())
						// Forbidden position
						// cout << "\t  Case of another gdtedge besides e\n";
						error = 1;
						break;
					}
				else
					// Forbidden position
					{
					//cout << "\t  Case of foreign gdtedge\n";
					error = 1;
					break;
					}
				}	
			int dx = 0, dy = 0;
			if (d2 == UP)
				dy = -1;
			else if (d2 == DOWN)
				dy = 1;
			else if (d2 == LEFT)
				dx = 1;
			else
				dx = -1;
			//cout << "\t Old position controlled is " << aux.xcoord() << " " << aux.ycoord() << "\n";
			aux = aux.translate(dx, dy);
			//cout << "\t New position controlled is " << aux.xcoord() << " " << aux.ycoord() << "\n";
			}
		//cout << "\t End of most internal cicle with error = " << error << "\n";
		if (error == 0)
			// It's possible to contract the gdtedge e
			//
			{
			j_start = j + 1;
			//cout << "\t j_start = " << j_start << "\n";
			//cout << "\t Error number " << error << "\n";
			return true;
			}
		else
			{
			//cout << "\t Error number " << error << "\n";
			return false;
			}
		}
	else if (victim == n) /// else if (id(victim) == id (n))
		// Any other position of gdtnode n is of less convenience;
		// is not possible to contract this gdtedge!
		{
		//cout << "\t In position " << d.xcoord() << " " << d.ycoord() << " node_with_center() have found the gdtnode " << id(victim) << "\n";
		error = 3;
		j_start = j + 1;
		//cout << "\t j_start = " << j_start << "\n";
		//cout << "\t Error number " << error << "\n";
		return false;
		}
	else
		// Forbidden position
		{
		//cout << "\t In position " << d.xcoord() << " " << d.ycoord() << " node_with_center() have found the gdtnode " << id(victim) << "\n";
		//cout << "\t Found preexisting gdtnode " << id(victim) << " in position " << center(victim).xcoord() << " " << center(victim).ycoord() << "\n";
		//cout << "\t the gdtnode with degree > 1 is gdtnode " << id(opposite(n, e)) << " in position " << center(opposite(n, e)).xcoord() << " " << center(opposite(n, e)).ycoord() << "\n";
		//cout << "\t the gdtnode with degree = 1 is gdtnode " << id(n) << " in position " << center(n).xcoord() << " " << center(n).ycoord() << "\n";
		error = 2;
		j_start = j + 1;
		//cout << "\t j_start = " << j_start << "\n";
		//cout << "\t Error number " << error << "\n";
		return false;
		}
	}
	


	bool
	draw_undi_graph::
contract_bridge_with_one_bend
	(gdtedge e, gdtnode n, gdt::gdtpoint p, direction_type d1, direction_type d2, std::set<gdt::gdtpoint,ltpt>& C, int& j_start)
	{
	// The purpose is the contraction of the length of the gdtedge
	// with the constraint to mantein inalterate the shape of draw;
	// it means that is neither possible to remove bends even if useless
	// nor rotate even single segment of the gdtedge
	//
	//cout << " contract_bridge_with_one_bend with gdtnode " << id(n) << " and gdtedge " << id(e) << "\n";
	int len = (int)floor(length(e) + 0.5);
	//cout << "  Length of the gdtedge = " << len << "\n";
	//cout << "  first value for variable j = " << j_start << "\n";
	int dx, dy;
	int i,j,k;
	gdt::gdtpoint c = p;
	for (i = 2; i <= len; ++i)
		for (j = j_start; j < i; ++j)
			{
			//cout << "  In the most internal for cicle\n";
			k = i - j;
			// The possibles cases are 8:
			//   d1 = R   d2 = D
			//   d1 = R   d2 = U
			//   d1 = L   d2 = D
			//   d1 = L   d2 = U
			//   d1 = D   d2 = R
			//   d1 = U   d2 = R
			//   d1 = D   d2 = L
			//   d1 = U   d2 = L
			//
			gdt::gdtpoint d = c;
			if (d1 == RIGHT)
				{
				dx = 1;
				if (d2 == DOWN)
					dy = -1;
				else
					dy = 1;
				//cout << "\tOld position of gdtnode with degree = 1 is " << d.xcoord() << " " << d.ycoord() << "\n";
				d = d.translate(j*dx, k*dy);
				//cout << "\tTranslation respect to gdtnode with degree > 1:\n"
				//     << "\t\tdelta_x = " << j*dx << "\n\t\tdelta_y = " << k*dy << "\n";
				//cout << "\tNew position of gdtnode with degree = 1 is " << d.xcoord() << " " << d.ycoord() << "\n";
				//cout << "\tdirection of gdtedge = RIGHT " << (dy == -1 ? "DOWN\n" : "UP\n");
				if (position_is_compatible(d, n, e, d2, C, k, j, j_start))
					{
					set_center(n, d);
					//cout << "\tThe gdtedge " << id(e) << " has been contracted!\n";
					//cout << "\tThe gdtnode " << id(n) << " has been placed in position " << d.xcoord() << " " << d.ycoord() << "\n";
					return true;
					}				
				}
			else if (d1 == LEFT)
				{
				dx = -1;
				if (d2 == DOWN)
					dy = -1;
				else
					dy = 1;
				//cout << "\tOld position of gdtnode with degree = 1 is " << d.xcoord() << " " << d.ycoord() << "\n";
				d = d.translate(j*dx, k*dy);
				//cout << "\tTranslation respect to gdtnode with degree > 1:\n"
				//     << "\t\tdelta_x = " << j*dx << "\n\t\tdelta_y = " << k*dy << "\n";
				//cout << "\tNew position of gdtnode with degree = 1 is " << d.xcoord() << " " << d.ycoord() << "\n";
				//cout << "\tdirection of gdtedge = LEFT " << (dy == -1 ? "DOWN\n" : "UP\n");
				if (position_is_compatible(d, n, e, d2, C, k, j, j_start))
					{
					set_center(n, d);
					//cout << "\tThe gdtedge " << id(e) << " has been contracted!\n";
					//cout << "\tThe gdtnode " << id(n) << " has been placed in position " << d.xcoord() << " " << d.ycoord() << "\n";
					return true;
					}
				}
			else if (d2 == RIGHT)
				{
				dy = 1;
				if (d1 == DOWN)
					dx = -1;
				else
					dx = 1;
				//cout << "\tOld position of gdtnode with degree = 1 is " << d.xcoord() << " " << d.ycoord() << "\n";
				d = d.translate(k*dy, j*dx);
				//cout << "\tTranslation respect to gdtnode with degree > 1:\n"
				//     << "\t\tdelta_x = " << j*dx << "\n\t\tdelta_y = " << k*dy << "\n";
				//cout << "\tNew position of gdtnode with degree = 1 is " << d.xcoord() << " " << d.ycoord() << "\n";
				//cout << "\tdirection of gdtedge = " << (dx == -1 ? "DOWN" : "UP") << " RIGHT\n";
				if (position_is_compatible(d, n, e, d2, C, k, j, j_start))
					{
					set_center(n, d);
					//cout << "\tThe gdtedge " << id(e) << " has been contracted!\n";
					//cout << "\tThe gdtnode " << id(n) << " has been placed in position " << d.xcoord() << " " << d.ycoord() << "\n";
					return true;
					}
				}
			else if (d2 == LEFT)
				{
				dy = -1;
				if (d1 == DOWN)
					dx = -1;
				else
					dx = 1;
				//cout << "\tOld position of gdtnode with degree = 1 is " << d.xcoord() << " " << d.ycoord() << "\n";
				d = d.translate(k*dy, j*dx);
				//cout << "\tTranslation respect to gdtnode with degree > 1:\n"
				//     << "\t\tdelta_x = " << j*dx << "\n\t\tdelta_y = " << k*dy << "\n";
				//cout << "\tNew position of gdtnode with degree = 1 is " << d.xcoord() << " " << d.ycoord() << "\n";
				//cout << "\tdirection of gdtedge = " << (dx == -1 ? "DOWN" : "UP") << " LEFT\n";
				if (position_is_compatible(d, n, e, d2, C, k, j, j_start))
					{
					set_center(n, d);
					//cout << "\tThe gdtedge " << id(e) << " has been contracted!\n";
					//cout << "\tThe gdtnode " << id(n) << " has been placed in position " << d.xcoord() << " " << d.ycoord() << "\n";
					return true;
					}
				}
			}
	return false;
	}



	bool
	draw_undi_graph::
contract_bridge_with_no_bends
	(gdtnode n, gdt::gdtpoint p, direction_type s, int mlfs, std::set<gdt::gdtpoint,ltpt>& C)
	{	
	gdt::gdtpoint old_point = center (n);
	
	//cout << " contract_bridge_with_no_bends\n";
	// A possible shortcut for this method is:
	//   the gdtnode opposite(n) have several edges on side s;
	//   this gdtedge e is the last processed;
	//   if is known the maximal length of the first segment mlfs of all others edges of this side
	//   the gdtnode n in position mlfs + 1 from the gdtnode opposite(n) is surely compatible or nearly
	//
	// Currently mlfs is not computed;
	//   note that for it is computed it must be evaluated not only the length of contracted gdtedge,
	//   but also the length of non contracted gdtedge and the length of gdtedge with degree > 1
	//   totally neglected up to now!!!

	double dx = 0, dy = 0;
	switch (s)
		{
		case LEFT:  dx = -1; break;
		case RIGHT: dx =  1; break;
		case UP:    dy =  1; break;
		case DOWN:  dy = -1; break;
		}
		
	p = p.translate((mlfs + 1) * dx, (mlfs + 1) * dy);
	
	//cout << " mlfs = " << mlfs << "\n position is " << p.xcoord() << " " << p.ycoord() << "\n";
	// The possible incompatibility is an gdtedge ie different from e
	//
	// If present, ie can't have the same orientation of gdtedge e, but must cross it;
	// otherwise an error is present on previous algorithms!
	//
	//while (C.member(p))
	while (C.find(p) != C.end()) 
		{
		p = p.translate(dx, dy);
		}
	set_center(n, p);
	
	if (p.distance(old_point) < 0.25) return false;
	return true;
	}



	double
	draw_undi_graph::
refine_orth_compaction
	(std::set<gdt::gdtpoint,ltpt>& C)
	//
	// The list of point C is the list of crosses between edges
	// This list is the trace of the operations of remove_cross_nodes
	//
	{
	// ------------------------------------------------------------
	// Currently this method improves only the length of edges that
	// have an extremal gdtnode with degree 1
	// ------------------------------------------------------------
	
	
	double improvement = 0;
	double initial_length = edge_length_sum();
	
	
	// ----------------- 0 ------------------
	// Denormalize coordinates and executes a
	// transormation to integer coordinates
	// for cross-points
	// --------------------------------------
	
	// Commented on April 7
	//
	//denormalize_coordinates();
	//double k = scale();
	//gdt::gdtpoint p_aux;
	//forall (p_aux, C)
	//	{
	//	double x = p_aux.xcoord();
	//	double y = p_aux.ycoord();
	//	p_aux = gdt::gdtpoint(floor((x - k)/k + 0.5), floor((y - k)/k + 0.5));
	//	}
	
	// ----------------- 1 -------------------
	// Create the list of nodes with degree  1
	// ---------------------------------------
	
	gdtnode 	       v;
	gdt::gdtlist<gdtnode>     L;
	gdt::gdtnode_map<bool> BN (*this,false);	// marker for nodes with degree 1
	
	//gdt::gdtlist<bool> BN;
	
	//cout << "Nodes with deg.1 :\n" << flush;
	
	forall_nodes(v,*this) //if (degree(v) == 1) 
		if (degree(v)==1 && number_of_bends(first_adj_edge(v)) == 1)
		   {
		   L.append(v); 
		   //BN.append(true); 
		   //cout << "\tnode" << id(v) << "\n" << flush;
		   }
	
	bool contracted_once = true;
	
	while (contracted_once)
		{
		contracted_once = false;
		
		forall (v,L) BN[v] = true;
		forall(v,L)
			{		
			// -------------------------------------------------------
			// find all edges belong to the same side of the only gdtedge 
			// of v on the opposite gdtnode of v 
			// -------------------------------------------------------
			//if(BN.inf(BN[L.rank(v) - 1]))

			if (BN[v])
	        		{
				//BN.assign(BN[L.rank(v) - 1], false);
				BN[v] = false;

				//cout << "Node " << id(v) << " is in position " << center(v).xcoord() << " " << center(v).ycoord() << "\n";

				gdtedge 	       e = first_adj_edge     (v);
				gdtnode 	       n = opposite 	      (v, e);
				direction_type s = direction_of_edge  (e, n);
				gdt::gdtlist<gdtedge>     E = edges_in_direction (n, s);

				double dx = 0, dy = 0;
				switch (s)
					{
					case LEFT:  dx = -1; break;
					case RIGHT: dx =  1; break;
					case UP:    dy =  1; break;
					case DOWN:  dy = -1; break;
					}
				gdt::gdtpoint p = center(n);

				int length = E.length();

				// --------------------------------------------------
				// CASE 1: there exists only one gdtedge e on the side s
				// --------------------------------------------------
				// Try to contract the gdtedge e as long as possible.
				// It is considered only the case of e without bends;
				// this is sufficient for optimal orthogonal drawings
				// --------------------------------------------------

				if (length == 1)
					{
					//cout << "Node with degree 1 on side " << s << " of gdtnode " << id(n) << " and with only one segment:\n";
					gdt::gdtlist<gdt::gdtpoint> P = bends(e);
					if (P.empty())
						{
						// The only case of possible conflict
						// is a cross along the gdtedge e
						//
						// cout << "\tnodo " << id(v) << endl;
						gdt::gdtpoint old_point = center(v);
						p = p.translate(dx, dy);
						//while (C.member(p)) p = p.translate(dx, dy);
						while (C.find(p) != C.end()) p = p.translate(dx, dy);
						if (p.distance(old_point) > 0.25)
							{
							set_center(v, p);
							contracted_once = true;
							}
						}
					else
						{
						//cout << "\n\nCase of single gdtedge " << id(e) << " on ";
						//switch (s)
						//	{
						//	case LEFT:  cout << "left";  break;
						//	case RIGHT: cout << "right"; break;
						//	case UP:    cout << "up";    break;
						//	case DOWN:  cout << "down";  break;
						//	}
						//cout << " side of gdtnode " << id(n) << " with more than one segment\n";
						//cout << "\tThis case is not possible in optimal orthogonal drawings!\n";
						}
					}

				// --------------------------------------------------
				// CASE 2: there are two or more edges on the side s
				// --------------------------------------------------
				// Global considerations about all edges of the side
				// are used to improve the drawing
				// --------------------------------------------------

				else
					{
					// Start the contraction from the gdtedge which segment nearest the gdtnode n
					// is the shortest. This allows to prepare the plane for greater contraction
					//
					// The strategy for contraction is the following:
					//
					// 
					//
					//       n---|---|---|---|	n is the position of the gdtnode with degree > 1
					//       |   |   |   |   |	
					//       |---1---3---6---10	1 to 10 is the order of the positions in which
					//       |   |   |   |   |		the contractor attempts to place the
					//       |---2---5---9---|		first gdtnode; for the second gdtnode the
					//       |   |   |   |   |		position rejected from the first gdtnode
					//       |---4---8---|---|		is equally to reject; for this reason 
					//       |   |   |   |   |		the contractor start from the successive
					//       |---7---|---|---|		position with respect to the placed gdtnode
					//
					//

					// The gdtedge with the shortest first segment is either the first or the last of the list E
					//
					// In the most general case on a single side of gdtnode n it is possible to find
					// edges with rigth bends, at most one gdtedge with no bends and edges with left bends;
					// in every case the edges, if present, are in the above order;
					// therefore it will be examined first the edges with bend in one direction,
					// after the edges with opposite bend and in the end the gdtedge with no bends.
					//
					//
					//
					//	                   O  O
					//	                   |  |
					//	|-------|         1|  |2
					//	|       |----------o  |
					//	|       |-------------o      5
					//	|   n   |-------------------------O
					//	|       |----------------------o
					//	|       |-------------------o  |
					//	|-------|                  3|  |4
					//	                            |  |
					//	                            O  O
					//
					//
					//		1 to 5 is the order in which the edges are examinated
					//

					// Often on the same side land edges of nodes of degree > 1;
					// temporarily these are neglected
					//

					// Select as first gdtedge to examine one that has the shortest first segment length
					// It may be either the first or the last of the list of edges on side s of gdtnode n
					//

					bool start;	// start = true means the gdtedge with the shortest first segment
							// is the first of the list E

					gdt::gdtlist<gdt::gdtpoint> P1 = bends(E.head());
					gdt::gdtlist<gdt::gdtpoint> P2 = bends(E.back());
					if      (P1.empty()) start = false;
					else if (P2.empty()) start = true;
					else
						{
						if (P1.head().distance(p) > P2.head().distance(p))
							start = false;
						else    start = true;
						}
					P1.clear();
					P2.clear();

					// If the first gdtedge to examine is the last of the list of edges
					// revert the list of edges; the first to examine must be in first position
					//
					int step = 1;
					if (!start)
						E.reverse_items();

					direction_type d1, d2;
					gdtedge ei = E.head();

					int separator = length - 1;	// is the position in the list E in which the edges have no more the same second segment direction
					int i 	      = 0;
					int changes   = 2;		// Number of possibly changes of the way of scan the list of edges
					int first     = 1;		// First position of the attempts of contraction;
									// it is a shortcut: allows to not examine position surely forbidden
					int mlfs      = 0;		// Maximal Length of First Segment of all the edges on side s
									// except for the gdtnode with degree = 1 and no bends;

					d1 = direction_of_second_segment_of_edge(ei, n); // it is a shortcut for the method contract_bridge_with_no_bends

					while(changes && i*step <= separator*step)
						{
						gdtedge ei = E.inf(E.get_item(i));
						gdtnode w = opposite(n,ei);	
						//cout << "Examining the gdtnode " << id(w) << "\n";
						//
						// ei is the current gdtedge
						// It is considered only if it belongs to a gdtnode with degree = 1;
						//

						if (degree(w) == 1)
							{
							// Check if ei has at least one bend
							//
							//BN.assign(BN[L.rank(w) - 1], false);
							BN[w] = false;

							if (!number_of_bends(ei))
								{
								//cout << "Case of gdtedge with no bends not only on the opposite side\n";
								//cout << "i = " << i << " step = " << step << " separator = " << separator << " changes = " << changes << "\n";
								// Assign to d1 the opposite direction wrt the actual
								// Revert the way to scan the list of edges
								//
								switch (d1)
									{
									case LEFT:  d1 = RIGHT; break;
									case RIGHT: d1 = LEFT;  break;
									case UP:    d1 = DOWN;  break;
									case DOWN:  d1 = UP;    break;
									}
								separator = E.rank(ei) - 1;
								i = length;
								step = -step;
								changes--;
								//cout << "i = " << i << " step = " << step << " separator = " << separator << " changes = " << changes << "\n";
								if (!changes)
									// try to contract no bend gdtedge
									//
									{
									if (contract_bridge_with_no_bends(w, p, s, mlfs, C)) contracted_once = true;
									}
								}
							else
								{
								d2 = direction_of_second_segment_of_edge(ei, n);
								if (d2 == d1)
									{
									//cout << "scanning gdtedge  = " << id(ei) << " for contraction of the two its segments" << endl;
									gdt::gdtpoint new_bend;
									gdt::gdtlist<gdt::gdtpoint> OBL = bends(ei);	// Old Bend List
									if (contract_bridge_with_one_bend(ei, w, p, s, d1, C, first))
										// Bend must be updated
										//
										{
										gdt::gdtlist<gdt::gdtpoint> NBL;	// New Bend List
										switch (s)
											{
											case LEFT:
											case RIGHT: {new_bend = gdt::gdtpoint(center(w).xcoord(), OBL.head().ycoord());} break;
											case UP:
											case DOWN:  {new_bend = gdt::gdtpoint(OBL.head().xcoord(), center(w).ycoord());} break;
											}
										NBL.append(new_bend);
										set_bends(ei, NBL);
										contracted_once = true;
										}
									else
										{
										new_bend = OBL.head();
										}
									// mlfs must be updated
									//
									OBL.clear();
									//cout << " new_bend = " << new_bend.xcoord() << " " << new_bend.ycoord() << "\n";
									//cout << " center   = " << center(n).xcoord() << " " << center(n).ycoord() << "\n";
									int lfs = (int)floor(new_bend.distance(center(n)) + 0.5);
									//cout << " old mlfs = " << mlfs << "\n";
									//cout << " lfs      = " << lfs << "\n";
									if (mlfs < lfs)
										mlfs = lfs;
									//cout << " mlfs     = " << mlfs << "\n";
									}
								else
									{
									// Assign to d1 the opposite direction wrt the actual
									// Revert the way to scan the list of edges
									//
									switch (d1)
										{
										case LEFT:  d1 = RIGHT; break;
										case RIGHT: d1 = LEFT;  break;
										case UP:    d1 = DOWN;  break;
										case DOWN:  d1 = UP;    break;
										}
									separator = E.rank(ei) - 1;
									i = length;
									step = -step;
									changes--;
									}
								}
							}
						else
							// Update the mlfs
							//
							{
							//cout << " Updating the mlfs in case of gdtnode with degree > 1\n";
							gdt::gdtlist<gdt::gdtpoint> lp = bends(ei);
							int lfs;
							if (lp.empty())
								{
								// Case of no bends: the first segment is the distance between the centers of the two nodes
								//
								lfs = (int)floor(p.distance(center(opposite(n, ei))) + 0.5);
								}
							else if (lp.length() == 1)
								{
								// Case of only one bend: the first segment is the distance between bend and center of gdtnode n
								//
								lfs = (int)floor(p.distance(lp.head()) + 0.5);
								//cout << "   lfs = " << lfs << "\n";
								}
							else
								{
								// Case of more than one bends
								//
								// Force the first bend of the list to be the nearest to gdtnode n
								//
								if (!(lp.head().xdist(center(n)) < 0.25 || lp.head().ydist(center(n)) < 0.25))
									lp.reverse_items();
								// Now the first segment is the distance between the first bend and the center of gdtnode n
								//
								lfs = (int)floor(p.distance(lp.head()) + 0.5);
								}
							//
							//
							// with the following line this method avoid to generate new crosses
							// among edges of the same side of the same gdtnode
							first = lfs;
							// without the above line this method, at cost of introduce new crosses
							// perform a jump of the possible obstacles on its way
							//
							//
							if (mlfs < lfs)
								mlfs = lfs;
							//cout << " mlfs = " << mlfs << "\n";
							}
						i += step;
						//cout << "mlfs = " << mlfs << "\n";
						}
					}
				}
			}
		}
	// OTHER CASES FOR NOT BRIDGES HAVE TO BE CODED 
	
	// Commented by W.D. on April 7
	// normalize_coordinates();
	
	double terminal_length = edge_length_sum();
	improvement = (initial_length - terminal_length) / initial_length;
	//cout << "\nREFINED COMPACTION IMPROVEMENT  = " << improvement << "\n" << flush;
	return improvement;
	}

// -------------------------------------------------------------------------------------------



	
// no properly methods
// -------------------
	
	gdt::gdtlist<gdt::gdtpoint>
	draw_undi_graph::
fragment_polyline
	(const gdt::gdtlist<gdt::gdtpoint>& bl, double c)
	{
	gdt::gdtlist<gdt::gdtpoint> fragmentation;
	gdt::gdtpoint p1,p2;
	gdt::list_item  ip1,ip2;
	
	if (bl.size()>1)
		{
		ip1 = bl.first();
		ip2 = bl.succ(ip1);
		while (ip2)
			{
			p1 = bl.inf(ip1);
			p2 = bl.inf(ip2);
			gdt::gdtlist<gdt::gdtpoint> frag = fragment_segment(p1,p2,c);
			fragmentation.conc(frag);
			fragmentation.pop_back();
			ip1 = ip2;
			ip2 = bl.succ(ip1);
			}
		fragmentation.append(bl.tail());
		}
	
	return fragmentation;
	}
	

	gdt::gdtlist<gdt::gdtpoint>
	draw_undi_graph::
fragment_segment
	(gdt::gdtpoint p1, gdt::gdtpoint p2, double c)
	{
	gdt::gdtpoint p;
	int i;
	gdt::gdtlist<gdt::gdtpoint> frag;
	double x1 = p1.xcoord(), y1 = p1.ycoord(),
	       x2 = p2.xcoord(), y2 = p2.ycoord();
	double d  = p1.distance(p2);
	double n  = d/c;  	
	double t  = (double)1/n;
	n = (int)n;
	
	frag.append(p1);
	for (i=1; i<(n-1); i++)
		{
		double x  = x1 + i*t*(x2-x1),
		       y  = y1 + i*t*(y2-y1);
		gdt::gdtpoint  p(x,y);
		frag.append(p);
		}
	frag.append(p2);
	return frag;
	}
// ---------

/*
	// -------------------------
	// EDITOR
	// -------------------------

	void
	draw_undi_graph::
edit
	(window& W, bool& quit_flag, bool redraw)
	{
	int save_but[8];

	bool edge_directed  = false;
	bool shadow_status;

	drawing_mode save_mode = W.get_mode();
	W.std_buttons(save_but);

  	W.set_mode(xor_mode);
  	W.set_show_coordinates(true);

	update_max_node_id();
	update_max_edge_id();

	shadow_status = shadows_are_visible();
	hide_shadows ();

	if (redraw)
		{
		draw (W);
		}

 	if (edge_directed) W.message ("gdtedge directed");
  	else		   W.message ("gdtedge undirected");

	while (!quit_flag)
		{
		int k;			// mouse button code
 		gdt::gdtpoint p,q;		// mouse current point
		gdt::gdtpoint p0,q0;		// mouse original point
		double x,y;
		double dx,dy;
		window* wp;
		unsigned long msec;
		int val;		// dummy int used for read_event


		gdtnode u,v;
		gdtedge bend_edge;
		gdt::list_item bend_pos;

		k = read_mouse(wp,x,y);
		p = gdt::gdtpoint(x,y);

		if (wp != &W) continue;

		msec = W.button_press_time();

		if (W.ctrl_key_down())    {ctrl_editor_functions (W,k,p); continue;}
		//if (W.shift_key_down()) {shift_editor_functions (W,k,p); continue;}

		if (W.shift_key_down())
			{
			edge_directed = !edge_directed;
			W.del_messages();
			if (edge_directed) W.message ("gdtedge directed..");
			else		   W.message ("gdtedge undirected..");
			continue;
			}


		switch (k)
			{
			case MOUSE_BUTTON(1):
				{
				bend_pos = find_bend(p,bend_edge);
				if (bend_pos)
				        {
				        gdt::gdtlist<gdt::gdtpoint>& bend_list = (*edge_info)[bend_edge].bends;

				        p0 = bend_list.contents(bend_pos);
					dx = p0.xcoord() - x;
					dy = p0.ycoord() - y;

					while ( W.read_event(val,x,y) != button_release_event)
						{
						q  = gdt::gdtpoint(x+dx,y+dy);
						q0 = bend_list.contents(bend_pos);
						if (q != q0)
							{
							draw (bend_edge,W);              					// Hide gdtedge
							bend_pos = bend_list.search(q0); 					// Detect bend list-item
							bend_list.assign(bend_pos,q);    					// Move bend point
							update_label_position_after_movement_of_bend_along_edge (bend_edge);
							draw (bend_edge,W);              					// Redraw gdtedge
							}
						}
					if      (bend_pos == bend_list.first()) update_embedding_after_node_move (source(bend_edge));
					else if (bend_pos == bend_list.last ()) update_embedding_after_node_move (target(bend_edge));
					break;
				        }


				v = find_node(p);
 				if (v)
					{
					// move gdtnode or new gdtedge
					//
					p0 = center(v); 	    // store original position of gdtnode v
					dx = p0.xcoord() - x;       // user might drag the gdtnode from one side (i.e. not from the center)
					dy = p0.ycoord() - y;	    // user might drag the gdtnode from one side
					while ( W.read_event(val,x,y) != button_release_event)
						{
						q = gdt::gdtpoint(x+dx,y+dy);      // q is the new candidate position for gdtnode v
						p = center(v); 		   // p is the current position of the gdtnode
						if (q != p)                // if the user is dragging
							{
							draw_around(v,W);  // erase gdtnode v from the previous prosition
							set_center(v,q);   // move gdtnode v to the new position
							update_label_positions_after_movement_of_node (v);
							draw_around(v,W);  // draw gdtnode v at the new position
							}
						}

					// Now the user has released the mouse button

					//cout << "\nUPDATING EMBEDDING AFTER NODE MOVE" << flush;
					update_embedding_after_node_move (v);
					msec = W.button_release_time() - msec;

					p = center (v);     // get current (new) position of gdtnode v
					set_center (v,p0);  // move gdtnode v back to its original position (temporarily)
					u = find_node (p);  // check for another gdtnode at candidate point p;
					set_center (v,p);   // move gdtnode back to the new candidate position

					if (msec > 200)
						{
						// The user performed a drag,
						// a simple click would have been quicker
						//
						if (!u || u==v)
							{
							//No other gdtnode is at the new candidate position p,
							//therefore that position can be assigned to gdtnode v
							//cout << "\nPOSITION NOT ALREADY TAKEN" << flush;
							}
						else
							{
							// Collision detected
							// Drag operation is rejected and a rollback is performed
							//cout << "\nPOSITION ALREADY TAKEN BY NODE" << id(u) << flush;
							draw_around (v,W);  // Erase the gdtnode from the new candidate position
							set_center  (v,p0); // Move back gdtnode to its original position
							update_label_positions_after_movement_of_node (v);
							draw_around (v,W);  // Draw the gdtnode at its original position
							}

						// Quit the gdtnode-drag processing
						// Get ready to process next event
						//
						break;
						}
					else
						{
						// The user clicked on the gdtnode
						// which means he wants to add a new gdtedge.
						// Make the gdtedge starting from the center of gdtnode v,
						// then continue with the BUTTON(2) case.
						p = p0;
						}
					}

				else
					{
					gdtedge e = find_edge(p);
					if (e)
						{
						// make gdtedge directed
						//
						gdtnode v1 = source(e);
						gdtnode v2 = target(e);
						gdt::gdtpoint p1 = center (v1);
						gdt::gdtpoint p2 = center (v2);
						double d1 = p.distance(p1);
						double d2 = p.distance(p2);

						//print (BY_EDGES);
						cout << flush;
						draw(e,W);

						if (d1 > d2)
							{
							if (start(e) == v1)
								{
								make_undirected (e);
								//cout << "\nedge made undirected" << flush;
								}
							else
								{
								make_directed (e,v1);
								//cout << "\nedge made directed" << flush;
								}
							}

						else if (d1 < d2)
							{
							if (start(e) == v2)
								{
								make_undirected (e);
								//cout << "\nedge made undirected" << flush;
								}
							else
								{
								make_directed (e,v2);
								//cout << "\nedge made directed" << flush;
								}
							}

						//print (BY_EDGES);
						//cout << flush;
						draw (e,W);
						break;
						}
					else
				        	{
						// new gdtnode
						//
						v  =  add_node_to_embedding (p);
						draw (v,W);
						break;
						}
					}
				}

			case MOUSE_BUTTON(2):
				{
				// new gdtedge
				gdtnode v = find_node(p);	// start gdtnode
				gdtnode w = NULL_NODE;			// stop gdtnode
				W.del_messages();
				W.message("new gdtedge");
				if (v != nil)
					{
					p = center(v);

					int k1;
					gdt::gdtpoint p1,p2;
					gdt::gdtlist<gdt::gdtpoint> bend_list;
					bool edge_aborted=false;
					bool edge_completed=false;
					bool node_double_clicked=false;

					p1=p;
					while (!(edge_completed || edge_aborted || node_double_clicked))
						{
						k1=W.read_mouse_seg(p1.xcoord(),p1.ycoord(),x,y) ;
						switch (k1)
							{
							case MOUSE_BUTTON(2):
								p2 = gdt::gdtpoint(x,y);
								w = find_node(p2);
								if (w!=nil)
									if (w!=v) edge_completed=true;
									else      edge_aborted=true; // self-loop rejected
								else
									{
									w  = add_node_to_embedding(p2);
									draw (w,W);
									edge_completed=true;
									}
								break;
							case MOUSE_BUTTON(1):
								p2 = gdt::gdtpoint(x,y);
								w = find_node (p2);
								if (w!=nil)
									if (w!=v) edge_completed=true;
									else      edge_aborted=true; // self-loop rejected
								else
									{
									bend_list.append(p2);			    // bend added
									W.draw_segment(p1,p2,edge_defs->body_color); // draw temporary gdtedge
									draw_bend (W,p2,edge_defs->body_color);      // draw temporary bend
									p1=p2;
									}
								break;
							case MOUSE_BUTTON(3):
								// abort
								edge_aborted=true;
								break;
							}
						}

					if (!bend_list.empty())
						W.draw_segment(center(v),bend_list.head(),edge_defs->body_color);

					draw_polyline(W, bend_list,edge_defs->body_color); // erase temporary gdtedge

					if (edge_completed)
						{
						gdtedge e;
						e = add_edge_to_embedding (v,w,bend_list);
						if (e) { if (edge_directed) make_directed(e,v); draw (e,W); }
						else W.acknowledge ("Ambiguous embedding, gdtedge rejected");
						}
					else
						{
						bend_list.clear();
						}
					}
				W.del_messages();
				if (edge_directed) W.message ("gdtedge directed..");
				else		   W.message ("gdtedge undirected..");
				break;
				}

			case MOUSE_BUTTON(3):
				{
				// delete gdtnode or gdtedge
				gdtnode v = find_node(p);
				if (v != nil)
					{
					draw_around (v,W);
					del_node(v);
					update_max_node_id();
					update_max_edge_id();
					break;
					}
				gdtedge e = find_edge(p);
				if (e != nil)
					{
					draw (e,W); // Erase the gdtedge
					del_edge(e);
					//Added by Marcandalli 8 2001
					edge_info->undefine(e);
					//
					update_max_edge_id();
					}
				break;
				}

			} // switch

		} // while !quit_flag
		
		if (shadow_status) show_shadows ();
		W.set_redraw();
		W.set_mode(save_mode);
		W.set_buttons(save_but);
	}


	void
	draw_undi_graph::
ctrl_editor_functions
	(window& W, int k, gdt::gdtpoint p)
	{
	gdtnode v;
	gdtedge e;
	//int    _int;    // Dummy
	//double _double; // Dummy

	v = find_node(p);
	e = find_edge(p);

	// Wait unti mouse button has been released
	//while ( W.read_event(_int,_double,_double) != button_release_event);

	switch (k)
		{
		case MOUSE_BUTTON(1):
			 if (v)
				{
				// Change the shape of gdtnode v
				//
				draw_around(v,W);  // temporarily erase gdtnode v
				switch (shape(v))
					{
					case ELLIPSE:   set_shape (v,TRIANGLE);  break;
					case TRIANGLE:  set_shape (v,RECTANGLE); break;
					case RECTANGLE: set_shape (v,ELLIPSE);   break;
					}
				draw_around(v,W);  // redraw gdtnode v with the new shape
				}
			else if (e)
				{
				// Change the shape of gdtedge e
				//
				draw (e,W);  // temporarily erase gdtedge e
				switch (shape(e))
					{
					case NORMAL: set_shape (e,DASHED); break;
					case DASHED: set_shape (e,DOTTED); break;
					case DOTTED: set_shape (e,NORMAL); break;
					}
				draw (e,W);  // redraw gdtedge with the new shape
				break;
				}
			break;
		case MOUSE_BUTTON(2):
			 if (v)
				{
				cout << "\nCTRL-MIDDLE on gdtnode " << label(v);
				cout << " (no action currently assigned)" << flush;
				}				

			else if (e)
				{
				cout << "\nCTRL-MIDDLE on gdtedge " << label(e);
				cout << " (no action currently assigned)" << flush;
				}
			break;
		case MOUSE_BUTTON(3):
			 if (v)
				{
				cout << "\nCTRL-RIGHT on gdtnode " << label(v);
				cout << " (no action currently assigned)" << flush;
				}				

			else if (e)
				{
				cout << "\nCTRL-RIGHT on gdtedge " << label(e);
				cout << " (no action currently assigned)";
				}
			break;
		}
	}


	void
	draw_undi_graph::
shift_editor_functions
	(int k, gdt::gdtpoint p)
	{
	gdtnode v;
	gdtedge e;
	//int    _int;    // Dummy
	//double _double; // Dummy

	v = find_node(p);
	e = find_edge(p);

	// Wait unti mouse button has been released
	//while ( W.read_event(_int,_double,_double) != button_release_event);

	switch (k)
		{
		case MOUSE_BUTTON(1):
			 if (v)
				{
				cout << "\nSHIFT-LEFT on gdtnode " << label(v);
				cout << " (no action currently assigned)" << flush;
				}				

			else if (e)
				{
				cout << "\nSHIFT-LEFT on gdtedge " << label(e);
				cout << " (no action currently assigned)" << flush;
				}
			break;
		case MOUSE_BUTTON(2):
			 if (v)
				{
				cout << "\nSHIFT-MIDDLE on gdtnode " << label(v);
				cout << " (no action currently assigned)" << flush;
				}				

			else if (e)
				{
				cout << "\nSHIFT-MIDDLE on gdtedge " << label(e);
				cout << " (no action currently assigned)" << flush;
				}
			break;
		case MOUSE_BUTTON(3):
			 if (v)
				{
				cout << "\nSHIFT-RIGHT on gdtnode " << label(v);
				cout << " (no action currently assigned)" << flush;
				}				

			else if (e)
				{
				cout << "\nSHIFT-RIGHT on gdtedge " << label(e);
				cout << " (no action currently assigned)" << flush;
				}
			break;
		}
	}


	void
	draw_undi_graph::
draw_bend
	(window& W, gdt::gdtpoint p, color col)
	{
	W.draw_disc (p,draw_curr->bend_radius,col);
	}
	

	void
	draw_undi_graph::
draw_polyline
	(window& W, gdt::gdtlist<gdt::gdtpoint>& pl, color col)
	{
	gdt::list_item pos;
	if (!pl.empty())
		{
		gdt::gdtpoint p1,p2;
		pos = pl.first();
		p1 = pl.inf(pos);
		draw_bend(W,p1,col);
		pos = pl.succ(pos);		
		while(pos)
			{
			p2 = pl.inf(pos);
			W.draw_segment(p1,p2,col);
			draw_bend(W,p2,col);
			p1=p2;
			pos = pl.succ(pos);
			}
		}
	}


	void 
	draw_undi_graph::
draw_shadow
	(gdtnode v, window& W)
	{
	node_shape_type st = shape (v);
	gdt::gdtpoint	   c       = center (v);
	double     dx      = width (v);
	double	   dy      = height (v);
	double     depth   = altitude(v);
		
	switch(st)
		{
		case ELLIPSE:
			{
			gdt::gdtpoint cs (c.xcoord()-depth, c.ycoord()-depth);
			W.draw_filled_ellipse (cs,dx/2,dy/2,draw_curr->shadow_color);
			break;
			}
			
		case RECTANGLE:
			{
			double x0 = c.xcoord();
			double y0 = c.ycoord();
			gdt::gdtpoint  p1 (x0-dx/2,y0-dy/2);	// vertex low-left
			gdt::gdtpoint  p2 (x0+dx/2,y0+dy/2);	// vertex high-rigth
			gdt::gdtpoint p1s (p1.xcoord()-depth, p1.ycoord()-depth);
			gdt::gdtpoint p2s (p2.xcoord()-depth, p2.ycoord()-depth);
			W.draw_box (p1s,p2s,draw_curr->shadow_color);
			break;				
			}
			
		case TRIANGLE:
			{
			double x0 = c.xcoord();
			double y0 = c.ycoord();
			gdt::gdtpoint  p1 (x0-dx/2,y0-dy/2);
			gdt::gdtpoint  p2 (x0+dx/2,y0-dy/2);
			gdt::gdtpoint  p3 (x0,y0+dy/2);
			gdt::gdtpoint p1s (p1.xcoord()-depth,p1.ycoord()-depth);
			gdt::gdtpoint p2s (p2.xcoord()-depth,p2.ycoord()-depth);
			gdt::gdtpoint p3s (p3.xcoord()-depth,p3.ycoord()-depth);
			W.draw_filled_triangle (p1s,p2s,p3s,draw_curr->shadow_color);
			break;
			}
		}
	}
	

	void
	draw_undi_graph::
draw_shadow
	(gdtedge e, window& W, bool curve, int ref1, double ref2)
	{
	gdtnode v1;
	gdtnode v2;
	if (edge_is_directed(e)) v1 = start(e);
	else			 v1 = source(e);
	v2 = opposite (v1,e);
	
	gdt::gdtlist<gdt::gdtpoint>  bl     = bends (e,v1);
	int          nb     = bl.size();
	double       depth1 = altitude (v1);
	double	     depth2 = altitude (v2);
	double	     depth;
	double	     step   = (depth1 - depth2)/(nb+1);
	color	     col    = draw_curr->shadow_color;
 	
	gdt::gdtpoint p1 = anchor_point (e,v1);
	gdt::gdtpoint p2 = anchor_point (e,v2);
	gdt::gdtpoint p;
	 	
	gdt::gdtpoint p1d (p1.xcoord() - depth1, p1.ycoord() - depth1);
	gdt::gdtpoint p2d (p2.xcoord() - depth2, p2.ycoord() - depth2);
	gdt::gdtpoint pd = p1d;
	
	depth = depth1;
	
	if (!curve || bl.empty())
		{	
		forall (p,bl)
		 	{
		 	depth -= step;
		 	gdt::gdtpoint p1d_t = pd;
		 	gdt::gdtpoint p2d_t (p.xcoord() - depth, p.ycoord() - depth);
			W.draw_segment (p1d_t, p2d_t, col);
			draw_bend (W,p2d_t,col);
		 	pd = p2d_t;
		 	}
	 	
		if (edge_is_directed(e)) W.draw_arrow   (pd,p2d,col);
		else			 W.draw_segment (pd,p2d,col);
		}
	else
		{
		gdt::gdtlist<gdt::gdtpoint> bld;
		forall (p,bl) 
			{
			depth -= step;
			gdt::gdtpoint pd (p.xcoord() - depth, p.ycoord() - depth);
			bld.append(pd);
			}
		bld.push (p1d);
		bld.append(p2d);
		gdt::gdtlist<gdt::gdtpoint> fragmentation = fragment_polyline (bld,scale()*ref2);
		
		//old code (to be restored)
		//if (edge_is_directed(e)) W.draw_bezier_arrow   (fragmentation,ref1,col);
		//else			 W.draw_bezier 	       (fragmentation,ref1,col);
		
		//new code (to be removed)
		list<gdt::gdtpoint> F;
		fragmentation.copy_to_leda_list(F);
		if (edge_is_directed(e)) W.draw_bezier_arrow   (F,ref1,col);
		else			 W.draw_bezier 	       (F,ref1,col);
		//
		}	 	
	}
	
*/
	// EMBEDDING METHODS
	// --------------------------------------------------------
        // The following methods are intended to handle the 
	// embedding, and to maintain it aligned with the graphical
	// representation.
	// --------------------------------------------------------


	void
	draw_undi_graph::
update_embedding_after_node_move
	(gdtnode u)
	{
	gdtedge e;
	gdtnode v = NULL_NODE,
	     w = NULL_NODE,
	     s = NULL_NODE;
	int  eid = 0;
	struct_draw_edge_info ei;
	gdt::gdtlist<int>		    eids;
	gdt::gdtlist<gdtnode>                  sources;
	gdt::gdtlist<gdtnode>                  targets;
	gdt::gdtlist<struct_draw_edge_info> infos;
	gdt::gdtlist<gdtnode> 		    starts;

	// Removes all edges around gdtnode u
	// saving info into temporary lists

	while (degree(u)>0)
		{
		e  = first_adj_edge(u);

		// extract gdtedge info
		//
		v  = source(e);
		w  = target(e);
		ei = (*edge_info)[e];

		// save info in the temporary lists
		//
		sources.append(v);
		targets.append(w);
		  infos.append(ei);
		   eids.append(id(e));
		 starts.append(start(e));

		// temporarily removes the gdtedge
		//
		del_edge(e);
		}

	// Ordinately reinsert edges around gdtnode u
	// restoring info saved into the temporary lists
	while (!sources.empty())
		{
		// extract gdtedge info from temporary lists
		//
		v   = sources.pop();
		if (!targets.empty())  w   = targets.pop();
		if (!infos.empty())    ei  = infos.pop();
		if (!eids.empty())     eid = eids.pop();
		if (!starts.empty())   s   = starts.pop();
		
		// restore the gdtedge previously removed
		//
		e = add_edge_to_embedding(v,w,ei,s);
		assign_id(e,eid);
		}

	update_max_node_id();
	update_max_edge_id();
	
	// Test printout
	//cout << "\nAdjacent nodes around gdtnode " << label(u) << " : ";
	//Modified by A.M.
	//forall_edges_adjacent_node(e,u,*this) cout << label(opposite(u,e)) << ",";
	//cout << "\n" << flush;
	}
	



	gdtedge
	draw_undi_graph::
add_edge_to_embedding
	(gdtnode v, gdtnode w, struct_draw_edge_info ei, gdtnode s)
	{
	gdt::gdtpoint pv,pw,apv,dpv,apw,dpw,cv,cw;
	gdtedge  e,ev,ew;
		
	bool aev; // ambiguous embedding around gdtnode v;
	bool aew; // ambiguous embedding around gdtnode w;
	
	cv = center(v);
	cw = center(w);
	
	dpv = ei.anchor_source;							// anchor-source distance of gdtedge	
	dpw = ei.anchor_target;							// anchor-target distance of gdtedge
	apv = gdt::gdtpoint(cv.xcoord()+dpv.xcoord(), cv.ycoord()+dpv.ycoord());	// anchor-source point of gdtedge
	apw = gdt::gdtpoint(cw.xcoord()+dpw.xcoord(), cw.ycoord()+dpw.ycoord());	// anchor-target point of gdtedge
		
	if (ei.bends.empty())
		{
		if (apv == cv) pv = apw;
		else	       pv = apv;
		if (apw == cw) pw = apv;
		else	       pw = apw;
		}
	else
		{
		if (apv == cv) pv = ei.bends.head();
		else	       pv = apv;
		if (apw == cw) pw = ei.bends.tail();
		else	       pw = apw;
		}
	
	ev = find_prev_adj_edge(v,pv,aev);
	ew = find_prev_adj_edge(w,pw,aew);
	
	if (aev || aew) return NULL_EDGE;
	
	if (ev)
		if (ew)
		        {
		        e=new_edge(v,ev,w,ew, ei);
		        //cout << "\nAdded gdtedge " << label(e) << " at gdtnode " << label(v) << " after gdtedge to " << label(opposite(v,ev)) << flush;
		        //cout << "\nAdded gdtedge " << label(e) << " at gdtnode " << label(w) << " after gdtedge to " << label(opposite(w,ew)) << flush;
		        }
		else
		        {
		        e=new_edge(v,ev,w, ei);
		        //cout << "\nAdded gdtedge " << label(e) << " at gdtnode " << label(v) << " after gdtedge to " << label(opposite(v,ev)) << flush;
		        //cout << "\nAdded gdtedge " << label(e) << " at gdtnode " << label(w) << flush;
		        }
	else
		if (ew)
		        {
		        // Since new_edge(v,w,ew,ei) is NOT available
		        // we have to use new_edge(w,ew,v, ei) and then
		        // revert the bend list order.
		        //
		        //for (int k=1; k <= (ei.bends.length()-1); k++)
		        //	ei.bends.move_to_front(ei.bends.get_item(k));

		        e = new_edge(w,ew,v, ei);
		        set_source(e,v);
		        // cout << "\nAdded gdtedge " << label(e) << " at gdtnode " << label(v) << flush;
		        // cout << "\nAdded gdtedge " << label(e) << " at gdtnode " << label(w) << " after gdtedge to " << label(opposite(w,ew)) << flush;
		        }
		else
		        {
		        e= new_edge(v,w, ei);
		        // cout << "\nAdded gdtedge " << label(e) << " at gdtnode " << label(v) << flush;
		        // cout << "\nAdded gdtedge " << label(e) << " at gdtnode " << label(w) << flush;
		        }
	
	// check the gdtedge direction
	//	        
	if (s) make_directed (e,s);	
	return e;
	}
	


	gdtedge
	draw_undi_graph::
add_edge_to_embedding
	(gdtnode v, gdtnode w, gdt::gdtlist<gdt::gdtpoint>& bend_points)
	{
	gdt::gdtpoint p(0,0);
	gdtedge e;
	struct_draw_edge_info ei;
	ei.bends         = bend_points;
	ei.anchor_source = p;
	ei.anchor_target = p;
	
	//cout << "\nENTERING IN add_edge_to_embedding" << flush;
	e = add_edge_to_embedding(v,w,ei,NULL_NODE);
	if (e) apply_defaults(e);
	return e;
	}
	

	
	gdtnode
	draw_undi_graph::
add_node_to_embedding
	(gdt::gdtpoint p)
	{
	gdtnode v;
	struct_draw_node_info ni;
	ni.center=p;
	v = add_node_to_embedding (ni);
	apply_defaults (v);
	return v;
	}


	gdtnode
	draw_undi_graph::
add_node_to_embedding
	(struct_draw_node_info ni)
	{
	return new_node(ni);
	}



	gdtedge
	draw_undi_graph::
find_prev_adj_edge
	(gdtnode u, gdt::gdtpoint p, bool& ambiguous_embedding) const
	{
	int	indeg;
	gdtedge	prev_adj_edge=NULL_EDGE;

	ambiguous_embedding=false;
	
	indeg = degree(u);
	switch(indeg)
		{
		case 0:
			prev_adj_edge=NULL_EDGE;
			break;
		case 1:
			prev_adj_edge=first_adj_edge(u);
			break;
		default:
			{
			gdtnode v;			
			gdt::gdtpoint	p1,p2;
			gdt::gdtsegment	cur_segm, ref_segm;
			gdtedge	cur_edge;
			double	cur_angle, min_angle;
			
			min_angle = 2*PI+1;
			
			p1=center(u);
			p2=p;
			ref_segm = gdt::gdtsegment(p1,p2);
			
			//cout << "\nScanning edges around gdtnode " << label(u);
			
			//Modified by A.M.
			forall_edges_adjacent_node(cur_edge,u,*this)
				{
				v = opposite(u,cur_edge);				
				gdt::gdtlist<gdt::gdtpoint> bend_list = bends(cur_edge,u);
				gdt::gdtpoint apu = anchor_point (cur_edge,u);
				gdt::gdtpoint apv = anchor_point (cur_edge,v);				
				if (bend_list.empty()) 
					{
					if (apu != p1) p2 = apu;
					else           p2 = center(v);
					}
				else    
					{
					if (apu != p1) p2 = apu;
					else	       p2 = bend_list.head();
					}

				cur_segm  = gdt::gdtsegment(p1,p2);
				cur_angle = cur_segm.angle(ref_segm);
				if (cur_angle > 0) cur_angle -= 2*PI;
				//cout << "\nAngle at gdtnode " << label(v) << " is " << cur_angle << flush;
				
				if (cur_angle < min_angle)
					{
					min_angle     = cur_angle;
					prev_adj_edge = cur_edge;
					}
				}
			if (min_angle==0) ambiguous_embedding=true;
			}
		}
	//if (prev_adj_edge) cout << "\nEdge " << label(prev_adj_edge) << " has been selected" << flush;
	//else cout << "\nNo gdtedge selected";
	return prev_adj_edge;
	}


	// RESTORE METHODS
	// --------------------------------------------------------
        // The following new methods are intended
	// to be used when restoring previously deleted gdtnode/edges
	// whose associated info have been saved and are now passed
	// as the ni/ei formal parameters.
	// --------------------------------------------------------


	gdtnode
	draw_undi_graph::
new_node
	(struct_draw_node_info ni)
	{
	gdtnode v = undi_graph::new_node();
	(*node_info)[v]=ni;
	return v;
	}


	gdtedge
	draw_undi_graph::
new_edge
	(gdtnode v1, gdtnode v2, struct_draw_edge_info ei)
	{
	gdtedge e = undi_graph::new_edge(v1,v2);
	(*edge_info)[e]=ei;
	return e;
	}


	gdtedge
	draw_undi_graph::
new_edge
	(gdtnode v1, gdtedge e1, gdtnode v2, struct_draw_edge_info ei, int dir) 
	{
	gdtedge e = undi_graph::new_edge(v1,e1,v2, AUTO_ID, dir);
	(*edge_info)[e]=ei;
	return e;
	}

	
	gdtedge
	draw_undi_graph::
new_edge
	(gdtnode v1, gdtedge e1, gdtnode v2, gdtedge e2, struct_draw_edge_info ei, int dir) 
	{
	gdtedge e = undi_graph::new_edge(v1,e1,v2,e2, AUTO_ID, dir);
	(*edge_info)[e]=ei;
	return e;
	}
	
	
