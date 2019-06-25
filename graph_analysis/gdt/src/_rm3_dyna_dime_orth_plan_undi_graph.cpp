#include <GDT/rm3_dime_orth_plan_undi_graph.h>
#include <GDT/rm3_draw_undi_graph.h>
#include <assert.h>
#include <GDT/gdt_error.h>

//-----------------------------------------------------------------------------
// DYNAMIC DRAWING
//-----------------------------------------------------------------------------

/*
	typedef struct
	{
	gdtnode                    vt1;                 // source terminal dime-gdtnode of the dime-gdtedge to be created
	gdtnode                    vt2;                 // sink   terminal dime-gdtnode of the dime-gdtedge to be created
	int			bend_cost;	     // bend cost
	int                     cross_cost;          // cross cost
	int 			length_unit_cost;    // length unit cost
	heading_type            initial_heading;     // heading of the first gdtedge of the minimal-cost path
	dire_graph              map;                 // map	
	edge_map<gdtedge>          edge_of_map_edge;    // map-gdtedge  -> real or dummy dime-gdtedge
	edge_map<gdtnode>          node_of_map_edge;    // map_edge  -> dummy dime-gdtnode
    	edge_map<face>          face_of_map_edge;    // map-gdtedge  -> face crossed by the map-gdtedge
	edge_map<string>        bends_of_map_edge;   // map_edge  -> sequence of bends (for the use of this, see the DELTA CONSIDERATIONS documentation at the end of this file)
	edge_map<map_node_set>  map_nodes_of_edge;   // dime-gdtedge -> set(map-gdtnode)
	node_map<map_node_set>  map_nodes_of_node;   // dime-gdtnode -> set(map-gdtnode)
	edge_map<int>           map_edge_cost;       // map gdtedge  -> cost
	gdt::gdtlist<gdtedge>	        map_edge_path;       // list of map-edges along the shortest path from mnt1 to mnt2
	gdt::gdtlist<gdtedge>	        edge_path;           // sequence of the new dime-edges created between vt1 and vt2
	gdtnode             	mnt1;                // map-gdtnode centered on vt1
	gdtnode             	mnt2;                // map-gdtnode centered on vt2
	bool     is_a_terminal_node          (gdtnode n)  {return (n == vt1 || n == vt2);}
	bool     is_the_source_terminal_node (gdtnode n)  {return (n == vt1);}
	bool     is_the_sink_terminal_node   (gdtnode n)  {return (n == vt2);}
	gdtnode     source_terminal             ()        {return vt1;}
	gdtnode     sink_terminal               ()        {return vt2;}
	bool     map_node_belongs_to_node    (gdtnode mn, gdtnode n)
		{
		map_node_set mns = DD.map_nodes_of_node[n];
		// A gdtnode may not has the map gdtnode set if this gdtnode has been created from the build_dime_edge
		if (!mns.empty())
			if (mns.north_node == mn || mns.east_node == mn || mns.south_node == mn || mns.west_node == mn) return true;
		return false;
		}
	}
DD_struct;
*/











































//-----------------------------------------------------------------------------
// DELETE EDGE
//-----------------------------------------------------------------------------
	
	bool
	dime_orth_plan_undi_graph::
the_graph_will_be_still_connected_after_deletion_of_edge
	(
	gdtedge e
	)
	{
	cout << "\nStart of method:\n\tdime_orth_plan_undi_graph::the_graph_will_be_still_connected_after_deletion_of_edge";
	dime_orth_plan_undi_graph dime(*this);
	dime.remove_all_dummy_nodes_and_edges();
	undi_graph undi(dime);
	gdtedge ue = undi.corresponding_edge(e, dime);
	undi.del_edge(ue);
	gdtnode zdn; // Zero degree gdtnode
	forall_nodes(zdn, undi) if (degree(zdn)==0) undi.del_node(zdn);
	cout << "\nEnd   of method:\n\tdime_orth_plan_undi_graph::the_graph_will_be_still_connected_after_deletion_of_edge";
	return(undi.is_connected());
	}
	
	
	
	gdt::gdtlist<gdtedge>
	dime_orth_plan_undi_graph::
find_edges_path_corresponding_to_unit_thick_real_edge
	(
	gdtedge e
	)
	{
	//-----------------
	// Fault situations
	//-----------------
	if (!e)
		gdt_error("gdtedge does not exist");
	if (!edge_is_real(e))
		gdt_error("gdtedge is not a real edge");
	else if (thickness_of_edge(e) != 1)
		gdt_error("gdtedge has thickness greater than one");
	
	//------------------
	// Begin of the list
	//------------------
	gdt::gdtlist<gdtedge> el;
	el.append(e);
	gdtedge ei;
	
	//------------------------------------------------
	// Completation of list from source side of gdtedge e
	//------------------------------------------------
	gdtnode n1 = source(e);
	gdt::gdtlist<gdtedge> els; // Edge List from Source gdtnode of gdtedge e
	els = complete_path_from_node_of_edge(n1, e);
	forall(ei, els)
		el.push(ei);
	
	//--------------------------------------------
	// Find the terminal gdtnode of this side of list
	//--------------------------------------------
	gdtnode stn; // Source side Terminal Node
	stn = source(el.head());
	if (!node_is_real(stn)) stn = target(el.head());
	
	//------------------------------------------------
	// Completation of list from target side of gdtedge e
	//------------------------------------------------
	gdtnode n2 = target(e);
	gdt::gdtlist<gdtedge> elt; // Edge List from Target gdtnode of gdtedge e
	elt = complete_path_from_node_of_edge(n2, e);
	forall(ei, elt)
		el.append(ei);
	
	//--------------------------------------------
	// Find the terminal gdtnode of this side of list
	//--------------------------------------------
	gdtnode ttn; // Target side Terminal Node
	ttn = source(el.tail());
	if (!node_is_real(ttn)) ttn = target(el.tail());
	
	//------------------------------------------------------------
	// To preserve from bad initialization of angle of border step
	//------------------------------------------------------------
	if (degree(stn) > degree(ttn)) el.reverse();
	
	// Debugging
	//
	// Show the result
	//cout << "\n\t\tEdges path corresponding to unit gdtedge " << id(e) << flush;
	//forall(ei, el)
	//	cout << "\n\t\t\tedge " << id(ei) << flush;
	//cout << "\n\t\tEnd edges path corresponding to unit gdtedge " << id(e) << flush;
	//
	return el;
	}
	
	
	
	gdt::gdtlist<gdtedge>
	dime_orth_plan_undi_graph::
complete_path_from_node_of_edge
	(
	gdtnode n,
	gdtedge e
	)
	{
	// The possible markers of gdtnode are:
	//  RM3_ADDED_BY_RECTANGULARIZATION,
	//  RM3_CROSS_ON_REAL_EDGE,
	//  RM3_BEND_NODE,
	//  RM3_REPLACES_A_BEND.
	// Walking only on real edges are possible the following situation:
	//  for RM3_ADDED_BY_RECTANGULARIZATION and RM3_CROSS_ON_REAL_EDGE it is sufficient to walk in the same direction;
	//  for RM3_BEND_NODE it is sufficient to walk on the most thick gdtedge; this is sufficient because for construction in the dime the sequences of bend nodes are possibles only starting from a real gdtnode;
	//  for RM3_REPLACES_A_BEND it is sufficient to walk on the only other real gdtedge.
	//
	// PRECONDITION:
	//  the gdtedge e must be real
	//  the gdtnode n must belong to gdtedge e
	//
	gdt::gdtlist<gdtedge> el;
	heading_type h = heading_after_rotation(initial_heading_of_border_step(border_step_moving_along_edge(e, n)), _180);
	while (!node_is_real(n))
		{
		if (is_marked(n, RM3_ADDED_BY_RECTANGULARIZATION) || is_marked(n, RM3_CROSS_ON_REAL_EDGE))
			//{
			e = find_edge_leaving_node_with_heading(n, h);
			//}
		else if (is_marked(n, RM3_BEND_NODE))
			{
			h = DD_heading_of_edge_with_max_thickness_around_bend_node(n);
			e = find_edge_leaving_node_with_heading(n, h);
			}
		else if (is_marked(n, RM3_REPLACES_A_BEND))
			{
			heading_type new_h = heading_after_rotation(h, _090);
			e = find_edge_leaving_node_with_heading(n, new_h);
			if (!e || edge_is_dummy(e))
				{
				new_h = heading_after_rotation(h, _270);
				e = find_edge_leaving_node_with_heading(n, new_h);
				}
			h = new_h;
			}
		else
			gdt_error("gdtnode has a not knew marker");
		el.append(e);
		n = opposite(n, e);
		}
	return el;
	}
	
	
	
	
	// DD_delete_edge subroutine
	// Decrease the thickness of the specified dime gdtedge under specified condition
	//
	void
	dime_orth_plan_undi_graph::
DD_decrease_thickness_of_border_step
	(
	border_step bs
	)
	{
	set_thickness_of_border_step(bs, thickness_of_border_step(bs)-1);
	}
	
	
	
	face
	dime_orth_plan_undi_graph::
DD_delete_edge
	(
	gdtedge e
	)
	{
	if (!the_graph_will_be_still_connected_after_deletion_of_edge(e)) return NULL_FACE;
	
	// debugging
	//
	//	{
	//	draw_undi_graph dug (0,*this);
	//	dug.write("dime_before_de.gdt");
	//	}
	//
	
	face f;
	f = DD_delete_edge_without_rectangularization(e);
	
	//------------------------------------------------------
	// A gdtnode ADDED_BY_RECTANGULARIZATION have sense no more
	// when no dummy edges are connected to it
	//------------------------------------------------------
	{
	gdtnode n;
	forall_nodes(n, *this)
		if (n && is_marked(n, RM3_ADDED_BY_RECTANGULARIZATION) && degree(n)==2 && DD_number_of_real_edges_around_node(n)==2)
			weld_edges_at_node(n);
	}
	
	//cout << "\n\t\tRerectangularization" << flush;
	local_init();
	//cout << "\n\t\tEnd rerectangularization" << flush;
	
	// debugging
	//
	//	{
	//	//print(COMPLETE);
	//	assert(is_orthogonal());
	//	draw_undi_graph dug (0,*this);
	//	dug.write("dime_de.gdt");
	//	}
	//
	
	return f;
	}
	
	
	
	face
	dime_orth_plan_undi_graph::
DD_delete_edge_without_rectangularization
	(
	gdtedge e
	)
	{
	gdt::gdtlist<gdtedge> el = find_edges_path_corresponding_to_unit_thick_real_edge(e);
	
	gdtedge ei = el.head();
	face f;
	
	//-------------------
	// List with one gdtedge
	//-------------------
	if (el.length() == 1)
		{
		//cout << "\n\t\tDeleting list of only one gdtedge " << id(ei) << flush;
		f = del_edge(ei);
		return f;
		}
	
	//----------------------------
	// List with two or more edges
	//----------------------------
	//cout << "\n\t\tDeleting dime edges:" << flush;
	
	gdtnode n  = source(ei);     // Current source gdtnode
	if (!node_is_real(n)) n = target(ei);
	
	string bendType = "";
	
	heading_type h   = initial_heading_of_border_step(border_step_moving_along_edge(ei, n));
	heading_type ph;          // Heading of the precedent gdtedge
	
	int t  = thickness_of_edge(ei);
	int pt;
	
	bool thk_are_decreasing = (t > 1);
	gdt::gdtlist<border_step> bsl;   // List of border step to be decreased in thickness
	
	gdt::list_item li;
	forall_items (li, el)
		{
		ei = el.inf(li);  // Current gdtedge
		
		ph = h;
		h  = initial_heading_of_border_step(border_step_moving_along_edge(ei, n));
		
		pt = t;
		t  = thickness_of_edge(ei);
		
		//--------------------------
		// Handling the current gdtnode
		//--------------------------
		//cout << "\n\t\t\thandling the gdtnode " << id(n) << flush;
		
		//--------------------------------------------
		// All the dummy edges around gdtnode are deleted
		//--------------------------------------------
		delete_all_dummy_paths_around_node(n);
		
		if (is_marked(n, RM3_BEND_NODE))
			{
			//cout << "\n\t\t\t\tthe gdtnode " << id(n) << " is a BEND NODE" << flush;
			if (thk_are_decreasing)
				{
				if (h != ph)
					{
					angle_type alpha = angle_required_to_change_heading(ph, h);
					border_step bs;
					
					//---------------
					// Case of L bend
					//---------------
					if (alpha == _270)
						{
						forall(bs, bsl)
							DD_decrease_thickness_of_border_step(opposite_border_step(bs));
						}
					
					//---------------
					// Case of R bend
					//---------------
					else if (alpha == _090)
						{
						forall(bs, bsl)
							DD_decrease_thickness_of_border_step(bs);
						}
					
					bsl.clear();
					thk_are_decreasing = false;
					
					if (from_node_direction_is_free(n, heading_after_rotation(h, _180)))
						{
						unmark(n, RM3_BEND_NODE);
						mark  (n, RM3_ADDED_BY_RECTANGULARIZATION);
						}
					}
				else // h == ph
					{
					if (t == 1)
						{
						heading_type Lh = heading_after_rotation(h, _270);
						heading_type Rh = heading_after_rotation(h, _090);
						if (from_node_direction_is_free(n, Rh)
						||  from_node_direction_is_free(n, Lh))
							{
							unmark(n, RM3_BEND_NODE);
							mark  (n, RM3_REPLACES_A_BEND);
							}
						else // two side edges on the same bend gdtnode
							{
							// A bend must be added to the list of bend of one of the two side edges and the initial heading setted to h
							//---------------------------------------------
							// Arbitrary choose of one of the two edges (R)
							//---------------------------------------------
							gdtedge to_be_promoted = find_edge_leaving_node_with_heading(n, Rh);
							
							//------------------------------------------------------------
							// Extracting the outing border steps of the edges around gdtnode
							//------------------------------------------------------------
							border_step tbs = opposite_border_step(border_step_moving_along_edge(el.inf(el.pred(li)), n)); // thick border step
							border_step bs  = opposite_border_step(border_step_moving_along_edge(to_be_promoted, n));      // border step of the promoted gdtedge
							
							//----------------------------------------------------------------------
							// Adding of a bend in the list of bends of the two border steps of gdtedge
							//----------------------------------------------------------------------
							gdt::gdtlist<bend_type> btl;
							btl = bends_of_border_step(bs);
							btl.push(left);
							set_bends_of_border_step(bs, btl);
							border_step obs = opposite_border_step(bs);
							btl = bends_of_border_step(obs);
							btl.append(right);
							set_bends_of_border_step(obs, btl);
							set_initial_heading_of_border_step_pair(bs, h);
							
							//-----------------------------------------------------------------------
							// Setting of the angles among the modified border step and the other one
							//-----------------------------------------------------------------------
							set_angle_of_border_step(bs , _000);
							set_angle_of_border_step(tbs, _180);
							}
						}
					}
				}
			else // thk are increasing
				{
				if (h != ph)
					{
					angle_type alpha = angle_required_to_change_heading(ph, h);
					
					//---------------
					// Case of L bend
					//---------------
					if (alpha == _270)
						{
						bendType = "L";
						}
					
					//---------------
					// Case of R bend
					//---------------
					else if (alpha == _090)
						{
						bendType = "R";
						}
					
					if (from_node_direction_is_free(n, ph))
						{
						unmark(n, RM3_BEND_NODE);
						mark  (n, RM3_ADDED_BY_RECTANGULARIZATION);
						}
					}
				else // h == ph
					{
					if (pt == 1)
						{
						heading_type Lh = heading_after_rotation(h, _270);
						heading_type Rh = heading_after_rotation(h, _090);
						if (from_node_direction_is_free(n, Rh)
						||  from_node_direction_is_free(n, Lh))
							{
							unmark(n, RM3_BEND_NODE);
							mark  (n, RM3_REPLACES_A_BEND);
							}
						else // two side edges on the same bend gdtnode
							{
							/*
							// A bend must be added to the list of bend of one of the two side edges and the initial heading setted to h
							gdtedge to_be_promoted = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _270)); // Arbitrary choose
							gdt::gdtlist<bend_type> btl;
							border_step bs = border_step_moving_along_edge(to_be_promoted, n);
							btl = bends_of_border_step(bs);
							btl.push(right);
							set_bends_of_border_step(bs, btl);
							border_step obs = opposite_border_step(bs);
							btl = bends_of_border_step(obs);
							btl.append(left);
							set_bends_of_border_step(obs, btl);
							set_initial_heading_of_border_step_pair(bs, h);
							*/
							//---------------------------------------------
							// Arbitrary choose of one of the two edges (L)
							//---------------------------------------------
							gdtedge to_be_promoted = find_edge_leaving_node_with_heading(n, Lh);
							
							//------------------------------------------------------------
							// Extracting the outing border steps of the edges around gdtnode
							//------------------------------------------------------------
							border_step tbs = opposite_border_step(border_step_moving_along_edge(ei, n));             // thick border step
							border_step bs  = opposite_border_step(border_step_moving_along_edge(to_be_promoted, n)); // border step of the promoted gdtedge
							
							//----------------------------------------------------------------------
							// Adding of a bend in the list of bends of the two border steps of gdtedge
							//----------------------------------------------------------------------
							gdt::gdtlist<bend_type> btl;
							btl = bends_of_border_step(bs);
							btl.push(left);
							set_bends_of_border_step(bs, btl);
							border_step obs = opposite_border_step(bs);
							btl = bends_of_border_step(obs);
							btl.append(right);
							set_bends_of_border_step(obs, btl);
							set_initial_heading_of_border_step_pair(bs, h);
							
							//-----------------------------------------------------------------------
							// Setting of the angles among the modified border step and the other one
							//-----------------------------------------------------------------------
							/*
							angle_type a_bs;
							gdtedge dummy = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _180));
							if (dummy && edge_is_dummy(dummy))
								a_bs = _000;
							else
								a_bs = _090;
								
							set_angle_of_border_step(tbs, _180);
							set_angle_of_border_step(bs , a_bs);
							*/
							
							set_angle_of_border_step(tbs, _180);
							set_angle_of_border_step(bs , _090);
							}
						}
					}
				}
			}
		else if (is_marked(n, RM3_CROSS_ON_REAL_EDGE))
			{
			unmark(n, RM3_CROSS_ON_REAL_EDGE);
			//cout << "\n\t\t\t\tnode " << id(n) << " is no more a cross" << flush;
			mark  (n, RM3_ADDED_BY_RECTANGULARIZATION);
			}
		else if (is_marked(n, RM3_REPLACES_A_BEND))
			{
			unmark(n, RM3_REPLACES_A_BEND);
			//cout << "\n\t\t\t\tnode " << id(n) << " is no more a bend" << flush;
			mark  (n, RM3_ADDED_BY_RECTANGULARIZATION);
			
			angle_type alpha = angle_required_to_change_heading(ph, h);
			if (thk_are_decreasing)
				{
				//-------------------------------------------------
				// The border steps in list are correctly decreased
				// the list cleared and
				// the decreasing condition is terminated
				//-------------------------------------------------
				border_step bs;
				if (alpha == _270)
					forall(bs, bsl)
						DD_decrease_thickness_of_border_step(opposite_border_step(bs));
				else if (alpha == _090)
					forall(bs, bsl)
						DD_decrease_thickness_of_border_step(bs);
				bsl.clear();
				thk_are_decreasing = false;
				}
			else // thk are increasing
				{
				//-----------------------
				// The bendType is setted
				//-----------------------
				if (alpha == _270)
					bendType = "L";
				else if (alpha == _090)
					bendType = "R";
				}
			}
		else // Other markers than BEND_NODE, CROSS_ON_REAL_EDGE, REPLACES_A_BEND
			{
			// Nothing to do
			}
		
		//--------------------------
		// Handling the current gdtedge
		//--------------------------
		//cout << "\n\t\t\thandling the gdtedge " << id(ei) << flush;
			
		//-------------------------------------------
		// Case of gdtedge with thickness greater than 1
		//-------------------------------------------
		if (t > 1)
			{
			border_step bs = border_step_moving_along_edge(ei, n); // R border_step
			
			//---------------------------
			// Decreasing thickness state
			//---------------------------
			if (thk_are_decreasing)
				// Arbitrarily put in list the border steps as they are of a R bend
				bsl.append(bs);
			
			//---------------------------
			// Increasing thickness state
			//---------------------------
			else // thk are increasing
				{
				//---------------------------------------
				// The gdtnode handling assign the bend type
				// before the gdtedge is handled, and then
				// when in increasing thickness state
				// an gdtedge with thickness greater than 1
				// has a bend type L or R
				// and no other possibility
				//---------------------------------------
				if (bendType == "R")
					{
					DD_decrease_thickness_of_border_step(bs);
					}
				else // bendType == L
					{
					DD_decrease_thickness_of_border_step(opposite_border_step(bs));
					}
				}
			}
			
		//------------------------------------
		// Case of gdtedge with thickness equal 1
		//------------------------------------
		else
			{
			f = del_edge(ei);
			}
		
		n = opposite(n, ei);
		}
	
	//-----------------------------------------------------
	// All the dummy edges around terminal gdtnode are deleted
	//-----------------------------------------------------
	if (n && node_is_real(n))
		delete_all_dummy_paths_and_nodes_around_node(n);
	else
		gdt_error("path ends with a not real gdtnode");
	
	//cout << "\n\t\tEnd deleting dime edges path" << flush;
	
	// debugging
	//
	//	{
	//	print(COMPLETE);
	//	draw_undi_graph dug (0,*this);
	//	dug.write("dime_during_de.gdt");
	//	cout << "\nCheck the orthogonality" << flush;
	//	assert(is_orthogonal());
	//	char c;
	//	cin >> c;
	//	cout << flush;
	//	}
	//
	
	return f;
	}
	
	
	
	void
	dime_orth_plan_undi_graph::
delete_all_dummy_paths_around_node
	(
	gdtnode n
	)
	{
	//cout << "\n\t\t\t\tBegin delete_all_dummy_paths_around_node from gdtnode " << id(n) << flush;
	gdtedge de; // dummy gdtedge
	int i;
	for (i=0; i<4; ++i)
		{
		de = find_edge_leaving_node_with_heading(n, heading_type(i));
		if (de && edge_is_dummy(de))
			{
			gdtnode dn = n; // dummy gdtnode
			dn = opposite(dn, de);
			if (is_marked(dn, RM3_ADDED_BY_RECTANGULARIZATION))
				{
				//cout << "\n\t\t\t\t\tdeleting the dummy gdtedge " << id(de) << flush;
				del_edge(de);
				if (dn) delete_all_dummy_paths_around_node(dn);
				}
			else
				//{
				//cout << "\n\t\t\t\t\tdeleting the dummy gdtedge " << id(de) << flush;
				del_edge(de);
				//}
			}
		}
	
	//cout << "\n\t\t\t\tEnd   delete_all_dummy_paths_around_node" << flush;
	}
	
	
	
	void
	dime_orth_plan_undi_graph::
delete_all_dummy_paths_and_nodes_around_node
	(
	gdtnode n
	)
	{
	//cout << "\n\t\t\t\tBegin delete_all_dummy_paths_and_nodes_around_node from gdtnode " << id(n) << flush;
	
	gdtedge de; // dummy gdtedge
	int i;
	for (i=0; i<4; ++i)
		{
		de = find_edge_leaving_node_with_heading(n, heading_type(i));
		if (de && edge_is_dummy(de))
			{
			gdtnode dn = n; // dummy gdtnode
			dn = opposite(dn, de);
			if (is_marked(dn, RM3_ADDED_BY_RECTANGULARIZATION))
				{
				//cout << "\n\t\t\t\t\tdeleting the dummy gdtedge " << id(de) << flush;
				del_edge(de);
				if (dn) delete_all_dummy_paths_and_nodes_around_node(dn);
				}
			else
				//{
				//cout << "\n\t\t\t\t\tdeleting the dummy gdtedge " << id(de) << flush;
				del_edge(de);
				//}
			}
		}
	
	//------------------------------------------------------
	// A gdtnode ADDED_BY_RECTANGULARIZATION have sense no more
	// when no dummy edges are connected to it
	//------------------------------------------------------
	if (n && is_marked(n, RM3_ADDED_BY_RECTANGULARIZATION) && degree(n)==2 && DD_number_of_real_edges_around_node(n)==2)
		//{
		//cout << "\n\t\t\t\t\twelding edges around gdtnode " << id(n) << flush;
		weld_edges_at_node(n);
		//}
	
	//cout << "\n\t\t\t\tEnd   delete_all_dummy_paths_and_nodes_around_node" << flush;
	}











































//-----------------------------------------------------------------------------
// DELETE NODE
//-----------------------------------------------------------------------------
	
//	bool
//	dime_orth_plan_undi_graph::
//the_graph_will_be_still_connected_after_deletion_of_node
	gdtnode
	dime_orth_plan_undi_graph::
get_connectivity_assurer_node_when_deleting_node
	(
	gdtnode n
	)
	{
	cout << "\nStart of method:\n\tdime_orth_plan_undi_graph::get_connectivity_assurer_node_when_deleting_node";
	// A corresponding undi is created;
	// on this the gdtnode is deleted and all the zero degree nodes;
	// after deletion the connectivity is tested;
	// 
	dime_orth_plan_undi_graph dime(*this);
	dime.remove_all_dummy_nodes_and_edges();
	undi_graph undi(dime);
	gdtnode un = undi.corresponding_node(n, dime);
	gdt::gdtlist<gdtnode> adj_n = undi.adj_nodes(un);
	undi.del_node(un);
	gdtnode zdn; // Zero degree gdtnode
	forall_nodes(zdn, undi) if (degree(zdn)==0) undi.del_node(zdn);
	
	
	// The gdtnode n before deletion has a set of adjacent nodes (adj_n);
	// after deletion only a part of these survive;
	// a gdtnode of these who survive is the connectivity assurer gdtnode that is:
	// if during deletion this gdtnode and the gdtedge from this and n is not deleted
	// then the graph will never be disconnected;
	//
	gdtnode connectivity_assurer_node = NULL_NODE;
	if(undi.is_connected())
		{
		gdtnode i;
		forall(i, adj_n)
			if (undi.find_node(id(i)))
				{
				connectivity_assurer_node = dime.corresponding_node(i, undi);
				break;
				}
		}
	
	if (connectivity_assurer_node) cout << "\n\tassurer gdtnode = " << id(connectivity_assurer_node);
	else                           cout << "\nassurer gdtnode not found";
	cout << "\nEnd   of method:\n\tdime_orth_plan_undi_graph::get_connectivity_assurer_node_when_deleting_node";
	return(connectivity_assurer_node);
	}
	
	
	
	face
	dime_orth_plan_undi_graph::
DD_delete_eagle_starting_from_node_along_edge
	(
	gdtnode n,
	gdtedge e,
	gdtnode can
	)
	{
	//cout << "\n\tDeleting eagle starting from gdtnode " << id(n) << " along gdtedge " << id(e) << flush;
	face f;
	gdtnode aux_n = n;
	gdtedge aux_e = e;
	heading_type h  = initial_heading_of_border_step(border_step_moving_along_edge(e, n));
	heading_type Lh = heading_after_rotation(h, _270); // Left  respect to h
	heading_type Rh = heading_after_rotation(h, _090); // Rigth respect to h
	bool found_can = false;
	bool is_can    = false;
	while (thickness_of_edge(aux_e) > 1)
		{
		aux_n   = opposite(aux_n, aux_e); // it is surely a bend_node
		gdtedge Le = find_edge_leaving_node_with_heading(aux_n, Lh);
		is_can  = opposite(aux_n, Le) == can;
		if (is_can) found_can = true;
		if (Le && !is_can)
			if (edge_is_dummy(Le))
				{
				cout << "\n\t\tDeleting dummy gdtedge " << id(Le) << flush;
				del_edge(Le);
				}
			else
				f = DD_delete_edge_without_rectangularization(Le);
		is_can = false;
		gdtedge Re = find_edge_leaving_node_with_heading(aux_n, Rh);
		is_can  = opposite(aux_n, Re) == can;
		if (is_can) found_can = true;
		if (Re)
			if (edge_is_dummy(Re))
				{
				cout << "\n\t\tDeleting dummy gdtedge " << id(Re) << flush;
				del_edge(Re);
				}
			else
				f = DD_delete_edge_without_rectangularization(Re);
		is_can = false;
		aux_e = find_edge_leaving_node_with_heading(aux_n, h);
		}
	// The cycle exit when thickness of gdtedge aux_e is == 1
	is_can = opposite(aux_n, aux_e) == can;
	if (is_can) found_can = true;
	if (!found_can) f = DD_delete_edge_without_rectangularization(aux_e);
	//cout << "\n\tEnd deleting eagle" << flush;
	return f;
	}
	
	
	
	face
	dime_orth_plan_undi_graph::
DD_delete_node
	(
	gdtnode n
	)
	{
//	if (!the_graph_will_be_still_connected_after_deletion_of_node(n)) return NULL_FACE;
	gdtnode can = get_connectivity_assurer_node_when_deleting_node(n);
	if (!can) return NULL_FACE;
	// debugging
	//
	//	{
	//	draw_undi_graph dug (0,*this);
	//	dug.write("dime_before_dn.gdt");
	//	}
	//
	
	gdt::gdtlist<gdtedge> le = adj_edges (n);
	gdtedge e;
	face f;
	forall(e, le)
		{
		if (e && edge_is_real(e))
			{
			if (thickness_of_edge(e) == 1 && opposite(n, e) != can)
				f = DD_delete_edge_without_rectangularization(e);
			else
				f = DD_delete_eagle_starting_from_node_along_edge(n, e, can);
			}
		// debugging
		//
		//	{
		//	draw_undi_graph dug (0,*this);
		//	dug.write("dime_during_dn.gdt");
		//	}
		//
		}
	delete_connectivity_assurer_eagle_around_node(can, n);
	if (n) delete_all_dummy_paths_and_nodes_around_node(n);
	
	//------------------------------------------------------
	// A gdtnode ADDED_BY_RECTANGULARIZATION have sense no more
	// when no dummy edges are connected to it
	//------------------------------------------------------
	{
	gdtnode n;
	forall_nodes(n, *this)
		if (n && is_marked(n, RM3_ADDED_BY_RECTANGULARIZATION) && degree(n)==2 && DD_number_of_real_edges_around_node(n)==2)
			weld_edges_at_node(n);
	}
	
	local_init();
	
	// debugging
	//
	//	{
	//	draw_undi_graph dug (0,*this);
	//	dug.write("dime_dn.gdt");
	//	}
	//
	
	return f;
	}
	
	
	
	void
	dime_orth_plan_undi_graph::
delete_connectivity_assurer_eagle_around_node
	(
	gdtnode can,
	gdtnode n
	)	
	{
	gdt::gdtlist<gdtedge> le = adj_edges (n);
	gdtedge e;
	forall(e, le)
		{
		if (e && edge_is_real(e))
			{
			if (thickness_of_edge(e) == 1)
				{
				DD_delete_edge_without_rectangularization(e);
				}
			else
				{
				heading_type h  = initial_heading_of_border_step(border_step_moving_along_edge(e, n));
				heading_type Lh = heading_after_rotation(h, _270); // Left  respect to h
				heading_type Rh = heading_after_rotation(h, _090); // Rigth respect to h
				gdtnode bend = opposite(n, e);
				
				gdtedge to_be_deleted = find_edge_leaving_node_with_heading(bend, h);
				DD_delete_edge_without_rectangularization(to_be_deleted);
				
				to_be_deleted = find_edge_leaving_node_with_heading(bend, Lh);
				if (to_be_deleted == NULL_EDGE)
					to_be_deleted = find_edge_leaving_node_with_heading(bend, Rh);
				DD_delete_edge_without_rectangularization(to_be_deleted);
				}
			break;
			}
		}
	}











































//-----------------------------------------------------------------------------
// ATTACH VERTEX
//-----------------------------------------------------------------------------
	
	// PRECONDITION:
	//  The gdtnode n must have two and only edges and
	//  these edges must have the opposite nodes respect to n
	//  on the same horizontal or vertical line.
	//  This is the tipical situation of an gdtedge splitted by a gdtnode n
	//  with orth::new_node (the orth class not know the headings)
	//
	void
	dime_orth_plan_undi_graph::
set_initial_heading_of_edges_splitted_by_node
	(
	gdtnode split
	)
	{
	gdtedge    e1 = first_adj_edge(split);
	gdtedge    e2 = last_adj_edge (split);
	gdtnode   oe1 = opposite(split, e1);
	gdtnode   oe2 = opposite(split, e2);
	point poe1 = position_of_node(oe1);
	point poe2 = position_of_node(oe2);
	if (poe1.ycoord() == poe2.ycoord())
		{
		if (poe1.xcoord() < poe2.xcoord())
			{
			set_initial_heading_of_border_step_pair(border_step_moving_along_edge(e1, split), west);
			set_initial_heading_of_border_step_pair(border_step_moving_along_edge(e2, split), east);
			}
		else
			{
			set_initial_heading_of_border_step_pair(border_step_moving_along_edge(e1, split), east);
			set_initial_heading_of_border_step_pair(border_step_moving_along_edge(e2, split), west);
			}
		}
	else
		{
		if (poe1.ycoord() < poe2.ycoord())
			{
			set_initial_heading_of_border_step_pair(border_step_moving_along_edge(e1, split), south);
			set_initial_heading_of_border_step_pair(border_step_moving_along_edge(e2, split), north);
			}
		else
			{
			set_initial_heading_of_border_step_pair(border_step_moving_along_edge(e1, split), north);
			set_initial_heading_of_border_step_pair(border_step_moving_along_edge(e2, split), south);
			}
		}
	}
	
	
	
	bool
	dime_orth_plan_undi_graph::
from_node_direction_is_free
	(
	gdtnode n,
	heading_type h
	)
	{
	gdtedge e = find_edge_leaving_node_with_heading(n, h);
	return (!e || edge_is_dummy(e));
	}
	
	
	
	point
	dime_orth_plan_undi_graph::
next_position_along_heading
	(
	point p_n,
	heading_type h
	)
	{
	point p = p_n;
	switch (h)
		{
		case north: p = point(p.xcoord(),   p.ycoord()+1); break;
		case south: p = point(p.xcoord(),   p.ycoord()-1); break;
		case east:  p = point(p.xcoord()+1, p.ycoord());   break;
		case west:  p = point(p.xcoord()-1, p.ycoord());   break;
		default: gdt_error("illegal slope"); break;
		}
	return p;
	}
	
	
	
	bool
	dime_orth_plan_undi_graph::
the_next_position_along_heading_is_free_from_real_node_and_orthogonal_edge
	(
	point p_n,
	heading_type h,
	gdtnode &n,
	gdtedge &e
	)
	{
	//cout << "\n\tBegin the_next_position..." << flush;
	n = NULL_NODE;
	e = NULL_EDGE;
	
	//----------------------------------------
	// Determination of the position to verify
	//----------------------------------------
	point p = next_position_along_heading(p_n, h);
	
	//-------
	// Verify
	//-------
	gdtnode  cn; //current gdtnode
	point cp; //current point
	gdtnode	upper = NULL_NODE,
		lower = NULL_NODE;
	forall_nodes(cn, *this)
		{
		cp = position_of_node(cn);
		//-----------------------------------------------
		// Case of gdtnode exactly in the position to verify
		//-----------------------------------------------
		if (cp == p)
			{
			n = cn;
			if (is_marked(cn, RM3_ADDED_BY_RECTANGULARIZATION))
				{
				//cout << "\n\t\tfound a ADDED BY RECT gdtnode in position -> true" << flush;
				if (from_node_direction_is_free(cn, heading_after_rotation(h, _270)))
					return true;
				else
				    	{
				    	e = find_edge_leaving_node_with_heading(cn, heading_after_rotation(h, _270));
				    	n = NULL_NODE;
				    	return false;
				    	}
				}
			else
				//{
				//cout << "\n\t\tfound a gdtnode in position -> false" << flush;
				return false;
				//}
			}
		//----------------------------------------------------------------------------------------------
		// Case of gdtnode with coordinates on the orthogonal of the heading h from the position to verify:
		//  in this case between two of this nodes may be an gdtedge in the position to verify
		//----------------------------------------------------------------------------------------------
		else if ((h == north || h == south) && cp.ycoord() == p.ycoord())
			{
			//------------------
			// Updating of upper
			//------------------
			if (cp.xcoord() > p.xcoord())
				{
				if (!upper)
					upper = cn;
				else if (cp.xcoord() < position_of_node(upper).xcoord())
					upper = cn;
				//-------------------------------------
				// Verify if exists an gdtedge in position
				//-------------------------------------
				if (lower)
					{
					e = find_edge(upper, lower);
					if (e)
						{
						if (edge_is_real(e))
							return false;
						else
							return true;
						}
					// eventuali scorciatoie:
					// pippo2: se esistono sia upper che lower, quindi dentro questa if,
					//         se entrambi sono a distanza 1 e non c'e' l'arco tra i due, non c'e' piu' possibilita' di averlo;
					//         si potrebbe velocizzare facendo questo controllo;
					// pippo3: ancora meglio se, una volta trovato un nodo, upper o lower che sia, si guarda se da li', verso la posizione cercata, esce un arco;
					//         se l'arco c'e', allora si guarda il nodo opposto:
					//          se coincide con la posizione cercata allora la posizione e' occupata da un nodo e si interrompe tutto;
					//          se questo e' ancora in zona upper si va ancora all'opposto ad oltranza;
					//          se e' in zona lower, allora la posizione e' occupata da un arco e si interrompe tutto;
					//         se invece l'arco non c'e' allora si deve proseguire con la ricerca di altri lower e upper normalmente.
					} 
				}
			//------------------
			// Updating of lower
			//------------------
			else if (cp.xcoord() < p.xcoord())
				{
				if (!lower)
					lower = cn;
				else if (cp.xcoord() > position_of_node(lower).xcoord())
					lower = cn;
				//-------------------------------------
				// Verify if exists an gdtedge in position
				//-------------------------------------
				if (upper)
					{
					e = find_edge(upper, lower);
					if (e)
						{
						if (edge_is_real(e))
							return false;
						else
							return true;
						}
					} 
				}
			}
		else if ((h == east || h == west) && cp.xcoord() == p.xcoord())
			{
			//------------------
			// Updating of upper
			//------------------
			if (cp.ycoord() > p.ycoord())
				{
				if (!upper)
					upper = cn;
				else if (cp.ycoord() < position_of_node(upper).ycoord())
					upper = cn;
				//-------------------------------------
				// Verify if exists an gdtedge in position
				//-------------------------------------
				if (lower)
					{
					e = find_edge(upper, lower);
					if (e)
						{
						if (edge_is_real(e))
							return false;
						else
							return true;
						}
					} 
				}
			//------------------
			// Updating of lower
			//------------------
			else if (cp.ycoord() < p.ycoord())
				{
				if (!lower)
					lower = cn;
				else if (cp.ycoord() > position_of_node(lower).ycoord())
					lower = cn;
				//-------------------------------------
				// Verify if exists an gdtedge in position
				//-------------------------------------
				if (upper)
					{
					e = find_edge(upper, lower);
					if (e)
						{
						if (edge_is_real(e))
							return false;
						else
							return true;
						}
					} 
				}
			}
		else
			; // Nothing to do
		}
	
	//cout << "\n\tEnd   the_next_position..." << flush;
	return true;
	}
	
	
	
	
	
/*	
	struct
_solution
	{
	int          score;
	heading_type eagle_heading;
	int          number_of_steps;
	string       bend_type;
	//bool         node_have_space;
	//gdtnode         no_space_because_node;
	//gdtedge         no_space_because_edge;
	
	_solution()
		{
		score                 = INFINITE;
		eagle_heading         = undefined_heading;
		number_of_steps       = INFINITE;
		bend_type             = "X";
		//node_have_space       = false;
		//no_space_because_node = NULL_NODE;
		//no_space_because_edge = NULL_EDGE;
		}
		
	_solution(const _solution& s)
		{
		*this = s;
		}
		
		_solution&
	operator=(const _solution& s)
		{
		score                 = s.score;
		eagle_heading         = s.eagle_heading;
		number_of_steps       = s.number_of_steps;
		bend_type             = s.bend_type;
		//node_have_space       = s.node_have_space;
		//no_space_because_node = s.no_space_because_node;
		//no_space_because_edge = s.no_space_because_edge;
		}
	}
_solution;
*/	
	
	
	
	
	gdtnode
	dime_orth_plan_undi_graph::
DD_dynamic_attach_vertex
	(
	gdtnode n
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
	
	//cout << "\nBegin attach_vertex on gdtnode " << id(n) << flush;
	
	// debugging
	//
	//	{
	//	draw_undi_graph dug (0,*this);
	//	dug.write("dime_before_nn.gdt");
	//	}
	//
	
	_solution * tmp = new (_solution);
	_solution * opt = new (_solution);
	
	static int offset = 0;	// walter's code
	int i;
	for (i = 0; i<4; ++i)
		{
		//---------------------------------------------------
		// In case of equivalent solution,
		// it must to prefer the solution along east heading.
		//
		// Therefore the exploration start from east and
		// only if a strictly better solution is found
		// the chosen heading will be different from east
		//---------------------------------------------------
		
		// 1 = east direction
		//
		// DD_explore_eagle(*tmp, n, heading_type((i+1)%4)); // frank's code
		//
		DD_explore_eagle(*tmp, n, heading_type((i+offset)%4)); 
		if (tmp->score < opt->score) *opt = *tmp;
		}
	offset ++;			// walter's code
	if (offset == 4) offset = 0;	// walter's code
	
	// debugging
	//
	//	{
	//	draw_undi_graph dug (0,*this);
	//	dug.write("dime_during_nn.gdt");
	//	}
	//
	
	gdtnode final = DD_build_solution(*opt, n);
	
	// debugging
	//
	//	{
	//	draw_undi_graph dug (0,*this);
	//	dug.write("dime_nn.gdt");
	//	}
	//
	
	//cout << "\nEnd   attach_vertex" << flush;
	return final;
	}
	
	
	
	// DD_dynamic_attach_vertex utility function
	// 
	void
	dime_orth_plan_undi_graph::
DD_explore_eagle
	(
	_solution &tmp,
	gdtnode n,
	heading_type h
	)
	{
	//cout << "\n\tBegin explore_eagle on gdtnode " << id(n) << " along heading " << h << flush;
	
	//---------------------------------
	// Cost of the different situations
	//---------------------------------
	int	straigth_cost                           =  0,
		split_cost                              = 10,
		turn_cost                               = 20,
		
		straigth_with_space                     = straigth_cost,
		straigth_without_space                  = straigth_cost + split_cost,
		turn_on_node_with_space                 = turn_cost,
		turn_on_node_without_space              = turn_cost     + split_cost,
		turn_on_unitary_thick_with_space        = turn_cost                  + 15,
		turn_on_unitary_thick_without_space     = turn_cost     + split_cost + 15,
	      /*turn_on_not_unitary_thick_with_space    = turn_cost                  + 35,
		turn_on_not_unitary_thick_without_space = turn_cost     + split_cost + 35,
	      */turn_on_eagle_basement                  = turn_cost     + split_cost + 55;
	
	point p = position_of_node(n);
	tmp.eagle_heading = h;
	gdtedge e = find_edge_leaving_node_with_heading(n, h);
	
	//-----------------
	// There is no gdtedge
	//-----------------
	if (!e)
		{
		//cout << "\n\t\tedge not exists" << flush;
		
		tmp.bend_type = "";
		tmp.number_of_steps = 0;
		gdtnode nsn = NULL_NODE; //no space gdtnode
		gdtedge nse = NULL_EDGE; //no space gdtedge
		bool fp  = the_next_position_along_heading_is_free_from_real_node_and_orthogonal_edge(p, h, nsn, nse); // free position
		if (fp)
			//{
			tmp.score = straigth_with_space;
			//tmp.node_have_space = true;
			//}
		else
			//{
			tmp.score = straigth_without_space;
			//tmp.node_have_space = false;
			//tmp.no_space_because_node = nsn;
			//tmp.no_space_because_edge = nse;
			//}
		}
	//----------------------
	// There is a dummy gdtedge
	//----------------------
	else if (edge_is_dummy(e))
		{
		//cout << "\n\t\tedge " << id(e) << " is dummy" << flush;
		
		tmp.bend_type = "";
		tmp.number_of_steps = 0;
		gdtnode opp = opposite(n, e);
		if (length_of_edge(e)>1 || (is_marked(opp, RM3_ADDED_BY_RECTANGULARIZATION) && from_node_direction_is_free(opp, heading_after_rotation(h, _270))))
			//{
			tmp.score = straigth_with_space;
			//tmp.node_have_space = true;
			//}
		else
			//{
			tmp.score = straigth_without_space;
			//tmp.node_have_space = false;
			//}
		}
	//----------------------------------------
	// There is a real gdtedge with thickness = 1
	//----------------------------------------
	else if (thickness_of_edge(e)==1)
		{
		//cout << "\n\t\tedge " << id(e) << " is real with thick = 1" << flush;
		
		tmp.number_of_steps = 0;
		if (length_of_edge(e)>1 || is_marked(opposite(n,e), RM3_ADDED_BY_RECTANGULARIZATION))
			{
			gdtnode nsn = NULL_NODE;
			gdtedge nse = NULL_EDGE;
			//----------------------------------------
			// Determination of the position to verify
			//----------------------------------------
			point poe = next_position_along_heading(p, h); //point on gdtedge
				
			if (the_next_position_along_heading_is_free_from_real_node_and_orthogonal_edge(poe, heading_after_rotation(h, _270), nsn, nse))
				{
				tmp.score = turn_on_unitary_thick_with_space;
				//tmp.node_have_space = true;
				tmp.bend_type = "L";
				}
			else if (the_next_position_along_heading_is_free_from_real_node_and_orthogonal_edge(poe, heading_after_rotation(h, _090), nsn, nse))
				{
				tmp.score = turn_on_unitary_thick_with_space;
				//tmp.node_have_space = true;
				tmp.bend_type = "R";
				}
			else
				{
				tmp.score = turn_on_unitary_thick_without_space;
				//tmp.node_have_space = the_next_position_along_heading_is_free_from_real_node_and_orthogonal_edge(poe, heading_after_rotation(h, _090), nsn, nse); //Arbitrary choose
				//tmp.no_space_because_node = nsn; //Side effect of the next_position_along ...
				//tmp.no_space_because_edge = nse; //Side effect of the next_position_along ...
				tmp.bend_type = "R"; //Arbitrary choose
				}
			}
		else
			{
			tmp.score = turn_on_unitary_thick_without_space;
			//tmp.node_have_space = true;
			tmp.bend_type = "R"; //Arbitrary choose
			}
		}
		
	//------------------------------
	// Case of sequence of bend gdtnode
	//------------------------------
	else
		{
		//cout << "\n\t\tedge " << id(e) << " is real with thick > 1" << flush;
		gdtnode sn = n; //succ n
		gdtedge se = e; //succ e
		bool found_a_free_place = false;
		tmp.number_of_steps = 0;
		int number_of_steps_tmp = 0;
		heading_type hL = heading_after_rotation(h, _270); //heading after a left turn
		heading_type hR = heading_after_rotation(h, _090); //heading after a right turn
		while (se && thickness_of_edge(se)>1 && !found_a_free_place)
			{
			tmp.number_of_steps += 1;
			sn = opposite(sn, se);
			p  = position_of_node(sn);
			gdtnode dn; //dummy n, created only to invoke the next_position_along ...
			gdtedge de; //dummy e, created only to invoke the next_position_along ...
			if (from_node_direction_is_free(sn, hL) && the_next_position_along_heading_is_free_from_real_node_and_orthogonal_edge(p, hL, dn, de))
				{
				tmp.score = turn_on_node_with_space;
				//tmp.node_have_space = true;
				tmp.bend_type = "L";
				found_a_free_place = true;
				}
			else if (from_node_direction_is_free(sn, hR) && the_next_position_along_heading_is_free_from_real_node_and_orthogonal_edge(p, hR, dn, de))
				{
				tmp.score = turn_on_node_with_space;
				//tmp.node_have_space = true;
				tmp.bend_type = "R";
				found_a_free_place = true;
				}
			/*
			else if (length_of_edge(se)>1 && the_next_position_along_heading_is_free_from_real_node_and_orthogonal_edge(next_position_along_heading(p, h), hL, dn, de))
				{
				tmp.score = turn_on_not_unitary_thick_with_space;
				tmp.bend_type = "L";
				number_of_steps_tmp = tmp.number_of_steps;
				}
			else if (length_of_edge(se)>1 && the_next_position_along_heading_is_free_from_real_node_and_orthogonal_edge(next_position_along_heading(p, h), hR, dn, de))
				{
				tmp.score = turn_on_not_unitary_thick_with_space;
				tmp.bend_type = "R";
				number_of_steps_tmp = tmp.number_of_steps;
				}
			*/
			else
				{
				tmp.score = turn_on_node_without_space;
				//tmp.node_have_space = false;
				tmp.bend_type = "R"; //Arbitrary choose
				}
			se = find_edge_leaving_node_with_heading(sn, h);
			}
			
		//gdtnode nsn = NULL_NODE;
		//gdtedge nse = NULL_EDGE;
		if (!found_a_free_place)
			{
			if (number_of_steps_tmp != 0)
				tmp.number_of_steps = number_of_steps_tmp;
			else
				{
				tmp.score = turn_on_eagle_basement;
				tmp.number_of_steps = 0;
				//tmp.node_have_space = the_next_position_along_heading_is_free_from_real_node_and_orthogonal_edge(p, heading_after_rotation(h, _090), nsn, nse); //Arbitrary choose
				//tmp.no_space_because_node = nsn; //Side effect of the next_position_along ...
				//tmp.no_space_because_edge = nse; //Side effect of the next_position_along ...
				tmp.bend_type = "R"; //Arbitrary choose
				}
			}
		}
	//cout << "\n\t\tscore = " << tmp.score << flush;
	//cout << "\n\tEnd   explore_eagle" << flush;
	}
	
	
	
	// DD_dynamic_attach_vertex utility function
	// 
	// Precondition:
	//  from gdtnode n direction h is free, it means there is no gdtedge or a dummy gdtedge
	// 
	gdtnode
	dime_orth_plan_undi_graph::
DD_attach_straigth_vertex_from_free_direction
	(
	gdtnode n,
	heading_type h
	)
	{
	//cout << "\n\t\tBegin attach_straigth from gdtnode " << id(n) << " toward heading " << h << flush;
	
	if (!n) gdt_error("the gdtnode not exists");
	gdtnode nn; // new gdtnode, contains the result
	
	gdtedge e = find_edge_leaving_node_with_heading(n, h);
	
	//----------------------------
	// Case of existent dummy gdtedge
	//----------------------------
	if (e)
		{
		gdtnode opp = opposite(n,e);
		if (is_marked(opp, RM3_ADDED_BY_RECTANGULARIZATION) && length_of_edge(e)==1 && from_node_direction_is_free(opp, heading_after_rotation(h, _270)))
			{
			//--------------------------------------------------------
			// There is no need to split the gdtedge;
			// it is sufficent to change in real the gdtedge and the gdtnode
			//--------------------------------------------------------
			unmark(opp, RM3_ADDED_BY_RECTANGULARIZATION);
			unmark(e  , RM3_ADDED_BY_RECTANGULARIZATION);
			nn = opp;
			}
		else
			{
			int old_id = id(e);
			
			//---------------------
			// The gdtedge is splitted
			//---------------------
			gdtnode split = orth_plan_undi_graph::new_node(e);
			
			//---------------------------------
			// Restoring the old id for an gdtedge
			//---------------------------------
			gdtedge e1 = first_adj_edge(split);
			gdtedge e2 = last_adj_edge (split);
			if (id(e1)>id(e2)) assign_id(e1,old_id);
			else               assign_id(e2,old_id);
			
			//------------------------
			// Updating of the heading
			//------------------------
			set_initial_heading_of_edges_splitted_by_node(split);
			
			//------------------------------------
			// The split gdtnode receive the position
			//------------------------------------
			set_position_of_node(split, next_position_along_heading(position_of_node(n), h));
			
			//-------------------------------------
			// An half of the gdtedge must become real
			//-------------------------------------
			unmark(find_edge(n,split), RM3_ADDED_BY_RECTANGULARIZATION);
			
			// debugging
			//
			//	{
			//	draw_undi_graph dug (0,*this);
			//	dug.write("dime_during.gdt");
			//	}
			//
			
			nn = split;
			}
		}
	
	//-------------------------
	// Case of no existant gdtedge
	//-------------------------
	else
		{
		gdtnode dn = NULL_NODE; // dummy gdtnode
		gdtedge de = NULL_EDGE; // dummy gdtedge
		bool fp = the_next_position_along_heading_is_free_from_real_node_and_orthogonal_edge(position_of_node(n), h, dn, de);
		if (fp && (dn || de))
			{
			//-----------------------------------------
			// Case of presence of a ADDED BY RECT gdtnode
			//-----------------------------------------
			if (dn)
				{
				unmark(dn, RM3_ADDED_BY_RECTANGULARIZATION);
				new_straight_edge(n, dn);
				nn = dn;
				}
			//--------------------------------------------
			// Case of presence of a trasversal dummy gdtedge
			//--------------------------------------------
			else 
				{
				//------------------------------------------------------------------------
				// Split of gdtedge:
				//  with respect to a orth split a dime split have some more informations:
				//   for the edges it must fixed the headings of the border steps;
				//   for the gdtnode it must fixed the position.
				//
				// Make the dime split manually preserve to invoke the local_init()
				//------------------------------------------------------------------------
				int old_id = id(de);
				
				//-----------
				// Orth split
				//-----------
				gdtnode split = orth_plan_undi_graph::new_node(de);
				
				//---------------------------------
				// Restoring the old id for an gdtedge
				//---------------------------------
				gdtedge e1 = first_adj_edge(split);
				gdtedge e2 = last_adj_edge (split);
				if (id(e1)>id(e2)) assign_id(e1,old_id);
				else               assign_id(e2,old_id);
				
				//------------------------
				// Updating of the heading
				//------------------------
				set_initial_heading_of_edges_splitted_by_node(split);
				
				//---------------------------------
				// Updating of the position of gdtnode
				//---------------------------------
				set_position_of_node(split, next_position_along_heading(position_of_node(n), h));
				
				new_straight_edge(n, split);
				
				// debugging
				//
				//	{
				//	draw_undi_graph dug (0,*this);
				//	dug.write("dime_during.gdt");
				//	}
				//
				
				nn = split;
				}
			}
		else
			{
			//-------------------------------------
			// Creation of new gdtnode with a new gdtedge
			//-------------------------------------
			heading_type d_e = h;
			gdtedge         fe  = e;
			while (fe == NULL_EDGE) 
				{
				d_e = heading_after_rotation (d_e, _090);
				fe  = find_edge_leaving_node_with_heading (n, d_e);
				//cout << "\n\t\t\tfrom mode " << id(n) << " along heading " << d_e << " the gdtedge "; if(fe) cout << "is " << id(fe) << flush; else cout << "not exists" << flush;
				}
			angle_type alpha = angle_required_to_change_heading(h, d_e);
			nn = orth_plan_undi_graph::new_node(n, fe, alpha);
			
			//------------------------
			// Updating of the heading
			//------------------------
			set_initial_heading_of_border_step_pair(border_step_moving_along_edge(find_edge(nn, n), n), h);
			
			//---------------------------------
			// Updating of the position of gdtnode
			//---------------------------------
			if (fp)
				set_position_of_node(nn, next_position_along_heading(position_of_node(n), h));
			else
				local_init();
			
			// debugging
			//
			//	{
			//	draw_undi_graph dug (0,*this);
			//	dug.write("dime_during.gdt");
			//	}
			//
			}
		}
	//cout << "\n\t\tEnd   attach_straigth" << flush;
	return nn;
	}
	
	
	
	// DD_dynamic_attach_vertex utility function
	// 
	gdtnode
	dime_orth_plan_undi_graph::
DD_build_solution
	(
	const _solution &opt,
	gdtnode n
	)
	{
	//cout << "\n\n\tBegin build_solution" << flush;
	//cout << "\n\t\tOptimal score   = " << opt.score           << flush;
	//cout << "\n\t\tOptimal heading = " << opt.eagle_heading   << flush;
	//cout << "\n\t\tOptimal steps   = " << opt.number_of_steps << flush;
	//cout << "\n\t\tOptimal bend    = " << opt.bend_type       << flush;
	
	gdtnode attached;
	
	heading_type h = opt.eagle_heading;
	
	//---------------------
	// Case of free heading
	//---------------------
	if (opt.bend_type == "")
		{
		attached = DD_attach_straigth_vertex_from_free_direction(n, h);
		}
		
	//------------------------------------------------------
	// Case of requested split of the base gdtedge of the eagle
	//------------------------------------------------------
	else if (opt.number_of_steps == 0)
		{
		gdtedge e = find_edge_leaving_node_with_heading(n, h);
		int loe = length_of_edge(e);
		int old_id = id(e);
		
		//--------------------------
		// The real gdtedge is splitted
		//--------------------------
		gdtnode split = orth_plan_undi_graph::new_node(e);
		
		//---------------------------------
		// Restoring the old id for an gdtedge
		//---------------------------------
		gdtedge e1 = first_adj_edge(split);
		gdtedge e2 = last_adj_edge (split);
		if (id(e1)>id(e2)) assign_id(e1,old_id);
		else               assign_id(e2,old_id);
		
		//------------------------
		// Updating of the heading
		//------------------------
		set_initial_heading_of_edges_splitted_by_node(split);
		
		//----------------------------------------
		// The position of split gdtnode is regulated
		//----------------------------------------
		if (loe > 1)
			set_position_of_node(split, next_position_along_heading(position_of_node(n), h));
		else
			local_init();
		
		//---------------------------------------
		// The split gdtnode nust become a bend_node
		//---------------------------------------
		mark (split, RM3_BEND_NODE);
		
		//------------------------------------------------------
		// The half real gdtedge more near to n must thickned
		// A straigth vertex is attached in the chosen direction
		//------------------------------------------------------
		if (opt.bend_type == "L")
			{
			DD_increase_thickness_of_border_step(border_step_moving_along_edge(find_edge(n,split), split));
			attached = DD_attach_straigth_vertex_from_free_direction(split, heading_after_rotation(h, _270));
			}
		else
			{
			DD_increase_thickness_of_border_step(border_step_moving_along_edge(find_edge(n,split), n));
			attached = DD_attach_straigth_vertex_from_free_direction(split, heading_after_rotation(h, _090));
			}
		}
		
	//---------------------------------
	// Case of completation of an eagle
	//---------------------------------
	else
		{
		gdtnode n_aux = n;
		gdtedge e_aux;
		
		//-------------------------------------------------------------------
		// All the real edges from n to the chosen bend_node must be thickned
		//-------------------------------------------------------------------
		int i;
		if (opt.bend_type == "L")
			{
			for (i=0; i<opt.number_of_steps; ++i)
				{
				e_aux = find_edge_leaving_node_with_heading(n_aux, h);
				n_aux = opposite(n_aux, e_aux);
				DD_increase_thickness_of_border_step(border_step_moving_along_edge(find_edge(n_aux, opposite(n_aux, e_aux)), n_aux));
				}
			}
		else
			{
			for (i=0; i<opt.number_of_steps; ++i)
				{
				e_aux = find_edge_leaving_node_with_heading(n_aux, h);
				DD_increase_thickness_of_border_step(border_step_moving_along_edge(find_edge(n_aux, opposite(n_aux, e_aux)), n_aux));
				n_aux = opposite(n_aux, e_aux);
				}
			}
		
		//----------------------------------
		// The gdtnode must be marked BEND_NODE
		//----------------------------------
		if (is_marked (n_aux, RM3_ADDED_BY_RECTANGULARIZATION))
			{
			unmark(n_aux, RM3_ADDED_BY_RECTANGULARIZATION);
			mark  (n_aux, RM3_BEND_NODE);
			}
		
		//------------------------------------------------------
		// A straigth vertex is attached in the chosen direction
		//------------------------------------------------------
		if (opt.bend_type == "L")
			attached = DD_attach_straigth_vertex_from_free_direction(n_aux, heading_after_rotation(h, _270));
		else
			attached = DD_attach_straigth_vertex_from_free_direction(n_aux, heading_after_rotation(h, _090));
		}
		
		// debugging
		//
		//	{
		//	//print(COMPLETE);
		//	draw_undi_graph dug (0,*this);
		//	dug.write("dime_during.gdt");
		//	}
		//
	
	local_init();
	//cout << "\n\tEnd   build_solution" << flush;
	return attached;
	}











































//-----------------------------------------------------------------------------
// NEW EDGE
//-----------------------------------------------------------------------------
	
	
	
	// Create a new gdtedge-path ep=(v1,v2), preserving the current shape 
	// if no crosses are needed ep is an gdtedge, otherwise it is a list of
	// edges along a cross sequence.
	// cross_cost is the cost for crossing an gdtedge of the graph
	// bend_cost is the cost for introducing a bend
	// length_unit_cost is the cost for length of the new gdtedge
	// 
	void 
	dime_orth_plan_undi_graph::
DD_dynamic_new_edge
	(
	gdtnode vt1,               // terminal dime-gdtnode of the dime-gdtedge to be created
	gdtnode vt2,               // terminal dime-gdtnode of the dime-gdtedge to be created
	int  bend_cost,	        // bend cost
	int  cross_cost,	// cross cost
	int  length_unit_cost   // length unit cost
	)
	{
	//cout << "\ndime_orth_plan_undi_graph::DD_dynamic_new_edge" << flush;
	
	// debugging
	//
	//	{
	//	draw_undi_graph dug (0,*this);
	//	dug.write("dime_before_ne.gdt");
	//	}
	//
	
	DD_struct DD;
	
	DD.vt1              = vt1;
	DD.vt2              = vt2;
	DD.bend_cost        = bend_cost;
	DD.cross_cost       = cross_cost;
	DD.length_unit_cost = length_unit_cost;
       
	DD_init_mappings                      (DD);
	DD_create_map                         (DD);	// note: costs are enforced??
	
	DD_find_shortest_path_through_the_map (DD);
	
	DD_print_map                          (DD);	// note: debugging (remove!)
	
	
	// debugging
	//
	//	{
	//	draw_undi_graph dug (0,*this);
	//	dug.write("dime_during_ne.gdt");
	//	}
	//
	
	DD_build_dime_edge_path               (DD);
	
	// debugging
	//
		{
		draw_undi_graph dug (0,*this);
	//	dug.write("dime_ne.gdt");
		//this->print(COMPLETE);
		}
	//
	
        //return DD.edge_path;
	}
	
	
	// DD_dynamic_new_edge utility function
	// 
	bool
	dime_orth_plan_undi_graph::
DD_node_can_be_crossed
	(gdtnode v)
	{
	// A gdtnode can be crossed (by map-edges) if it's dummy, and if it has
	// either no real adjacent edges or two opposite real adjacent edges.
	//
	// NOTE: a dummy gdtnode is introduced in the dime during the rectangularization
	// either to replace a bend or to split a real gdtedge. As a consequence
	// only 0 or 2 real edges can be adjacent a dummy gdtnode.
	
	/*
	// There is three possible cases of not real nodes in this situation:
	//   the nodes added by rectangularization as split of a straight gdtedge   marked as RM3_ADDED_BY_RECTANGULARIZATION
	//   the nodes added by rectangularization as split of an gdtedge with bend marked as RM3_REPLACES_A_BEND
	//   the nodes added by planarization as a replacement of a cross        marked as RM3_CROSS_ON_REAL_EDGE
	// therefore a real gdtnode is a gdtnode without these markers!!!
	//
	// Only the case RM3_ADDED_BY_RECTANGULARIZATION has a lot of possibility to result crossable and then
	// in all other cases there is no need to examine the gdtnode, it is not crossable!!!
	//
	//
	// In the problem >4 there is an other situation in which a dummy gdtnode are persent;
	// when more than one edges walk parallely from one only gdtnode, and
	// gradually each gdtedge change direction (see the picture)
	//
	//  Draw representation                                          Dime representation
	//
	//                 ( )       ( )                                                ( )       ( )
	//                  |         |                                                  |         |
	//    /----\        |         |               /----\               /----\        |         |               /----\
	//    |    |--------|         |               |    |               |    |       /-\       /-\              |    |
	//    |    |------------------|               |    |               |    |-------| |-------| |--------------|    |
	//    |    |----------------------------------|    |               |    |       \-/       \-/              |    |
	//    \----/                                  \----/               \----/                                  \----/
	//
	//       0          1         2                  3                    0          1         2                  3
	//
	// In corrispondence of the 0 there is the gdtnode with some parallel edges;
	// in the points 1 and 2 there is two dummy nodes marked as RM3_BEND_NODE that split the multiple gdtedge in some parts:
	// the part from 0 to 1 has thickness 3, the one from 1 to 2 has thickness 2 and the one from 2 to 3 has thickness 1!
	//
	// This case too is a case of not crossable gdtnode and for this the criterio to use to verify the crossability
	// is the same of the other
	//
	//
	// Only the RM3_ADDED_BY_RECTANGULARIZATION gdtnode can be crossed, but not all these gdtnode!
	// There is the case of one of this gdtnode with two real edgse at 180 degree among them ----O----
	// with the two real edges with thickness greater than 1;
	// for the "no thick cross" problem this gdtnode cannot be crossed!
	//
	// All the theorical possible cases of dummy gdtnode with no edges, dummy edges or real edges
	// are displayed in the following picture;
	// at any case in the picture is associated a word; this word is the possible markers of the gdtnode or is the consideration of not existance of the case in GDT
	//
	//  O = dummy gdtnode    --- = real gdtedge    - - = dummy gdtedge
	//
	//---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------
	//         |         |         |         |         |         |         |         |         |         |         
	// 1       | 2       | 3       | 4       | 5       | 6       | 7       | 8       | 9       | 10      | 11
	//         |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |   
	//         |    |    |    |    |    |    |    |    |    |    |         |         |         |         |        
	//    O    |    O    |    O--- |    O    |    O--- | ---O--- |    O    |    O--- |    O    |    O--- | ---O---
	//         |         |         |    |    |    |    |    |    |         |         |    |    |    |    |        
	//         |         |         |    |    |    |    |    |    |         |         |    |    |    |    |        
	// NO      | NO      | REPLACE | ADDED   | BEND    | CROSS   | NO      | NO      | NO      | REPLACE | ADDED
	//         |         | A BEND  | BY RECT | NODE    |  OR     |         |         |         | A BEND  | BY RECT
	//         |         |         |         |         | BEND    |         |         |         |         |
	//         |         |         |         |         | NODE    |         |         |         |         |
	//         |         |         |         |         |         |         |         |         |         |         
	//---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------
	//         |         |         |         |         |         |         |         |         |         |
	// 12      | 13      | 14      | 15      | 16      | 17      | 18      | 19      | 20      | 21      |
	//    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
	//         |         |         |         |         |         |         |         |         |         |
	// ---O--- |    O- - |    O    |    O- - |    O--- | ---O- - | ---O--- |    O- - | ---O- - | - -O- - |
	//    |    |         |         |    |    |         |    |    |         |         |         |         |
	//    |    |         |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
	// BEND    | NO      | ADDED   | NO      | NO      | REPLACE | ADDED   | ADDED   | NO      | ADDED   |
	// NODE    |         | BY RECT |         |         | A BEND  | BY RECT | BY RECT |         | BY RECT |
	//         |         |         |         |         |         |         |         |         |         |
	//---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+
	//
	// It's interesting to note that the cases of RM3_ADDED_BY_RECTANGULARIZATION gdtnode may have
	// only two real edges or no real edges and if there is two real edges they are at 180 degree among them;
	// the thickness of the two real edges is the same because there is a split of a single real gdtedge performed by rectangularization;
	// therefore for the "no thick cross" problem it must be checked only the two first edges around gdtnode;
	// if among these there is no real gdtedge, there is no real edges around the gdtnode and the gdtnode is crossable;
	// if there is a real gdtedge, if its thickness is > 1, the gdtnode is not crossable, else is crossable!
	*/
	
	if (!is_marked(v, RM3_ADDED_BY_RECTANGULARIZATION)) return false;
	else
		{
		gdtedge ae[4];	// array of dime-edges adjacent current dime-gdtnode v (0-north, 1-east, 2-south, 3-west)
		int i;
		gdtedge e;
		for(i=0;i<4;i++) ae[i] = NULL_EDGE;                                                                // reset adjacent edges array
		forall_edges_adjacent_node (e,v,*this) ae[(short)position_of_edge_with_respect_to_node(e,v)] = e;  // init adjacent edges array
		
		//              north
		//
		//              ae[0]
		//     	         |
		//  west  ae[3]-----ae[1]  east
		//     	         |
		//              ae[2]
		//
		//              south
		
		i = 0;
		while(i <= 1)
			{
			e = ae[i];
			if (e && edge_is_real(e) && thickness_of_edge(e)>1)
				return false;
			++i;
			}
		}
	return true;
	}
	
	
	
	bool
	dime_orth_plan_undi_graph::
DD_from_this_terminal_the_other_one_is_along_heading
	(
	gdtnode this_terminal,
	DD_struct& DD,
	heading_type h
	)
	{
	gdtnode other_one;
	if (DD.is_the_source_terminal_node(this_terminal))
		other_one = DD.sink_terminal();
	else
		other_one = DD.source_terminal();
	return (position_of_node_with_respect_to_node(other_one, this_terminal) == h);
	}
	
	
	
	// DD_dynamic_new_edge utility function
	//
	int
	dime_orth_plan_undi_graph::
DD_number_of_real_edges_around_node
	(gdtnode v)
	{
	return find_adj_edges_not_marked_as(v,RM3_ADDED_BY_RECTANGULARIZATION).size();
	}
	
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 1
	//
	void
	dime_orth_plan_undi_graph::
DD_verify_preliminary_conditions
	(DD_struct& DD)
	{
	int d1 = degree(DD.source_terminal());
	int d2 = degree(DD.sink_terminal());
	int d3 = find_adj_edges_marked_as(DD.source_terminal(), RM3_ADDED_BY_RECTANGULARIZATION).size();
	int d4 = find_adj_edges_marked_as(DD.sink_terminal(),   RM3_ADDED_BY_RECTANGULARIZATION).size();
	if (((d1 - d3) == 4) || ((d2 - d4) == 4) )
		gdt_error("terminal gdtnode has degree > 4");
	}
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 2
	//
	void
	dime_orth_plan_undi_graph::
DD_init_mappings
	(DD_struct& DD)
	{
	gdtnode v;
	gdtedge e, me;
	
	DD.map_nodes_of_edge.init   (nodes_and_edges());
	DD.map_nodes_of_node.init   (nodes_and_edges());
	
	DD.edge_of_map_edge.init    (DD.map.nodes_and_edges(), NULL_EDGE);
	DD.node_of_map_edge.init    (DD.map.nodes_and_edges(), NULL_NODE);
	DD.face_of_map_edge.init    (DD.map.nodes_and_edges(), NULL_FACE);
	DD.bends_of_map_edge.init   (DD.map.nodes_and_edges(), "");
	DD.map_edge_cost.init       (DD.map.nodes_and_edges(), -1);
	
	forall_edges(e, *this)  DD_init_map_node_set (DD.map_nodes_of_edge[e], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
	forall_nodes(v, *this)  DD_init_map_node_set (DD.map_nodes_of_node[v], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
	
	forall_edges(me, DD.map) DD.bends_of_map_edge[me] = "";
	}
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3 - Create the map for the min-cost-flow algorithm
	//
	void
	dime_orth_plan_undi_graph::
DD_create_map
	(DD_struct& DD)
	{
	//cout << "\nstart creation of the map" << flush;
	DD_create_map_subnet_for_each_edge                          (DD);
	DD_create_map_subnet_for_each_crossable_or_terminal_node    (DD);
	DD_complete_map_subnet_with_particularity_of_terminal_nodes (DD);	// Added for expansion to >4plan
	DD_connect_map_subnets                                      (DD);
	//cout << "\nend creation of the map" << flush;
	}
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.1 - Create the map subnet for each dime-gdtedge
	//
	void
	dime_orth_plan_undi_graph::
DD_create_map_subnet_for_each_edge
	(DD_struct& DD)
	{
	//cout << "\ndime_orth_plan_undi_graph::DD_create_map_subnet_for_each_edge" << flush;
	
	gdtnode sn, tn, free_node;
	gdtedge e;
	
	gdtnode mn1, mn2, mn3, mn4;
	gdtedge me12, me21, me13, me31, me14, me41, me23, me32, me24, me42, me34, me43; 
	
	forall_edges(e, *this)
		{
		sn = source(e);
		tn = target(e);
		
		int walk_along_dummy_cost      = DD.length_unit_cost*2*length_of_edge(e),
		    dummy_split_cost           = DD.length_unit_cost*2,
		    real_split_cost            = DD.length_unit_cost*2*thickness_of_edge(e),
		    turn_on_dummy_edge_cost    = DD.bend_cost*2  + dummy_split_cost + walk_along_dummy_cost/2,
		    cross_on_dummy_edge        = dummy_split_cost,
		    cross_on_long_dummy_edge   = 0,
		    cross_on_real_edge         = DD.cross_cost*2 + real_split_cost,
		    cross_on_long_real_edge    = DD.cross_cost*2;
		
		if (edge_is_dummy(e))
			{
			// e is a dummy dime-gdtedge 
			//
			bool sn_free = (DD.is_a_terminal_node(sn) || (DD_node_can_be_crossed(sn)));
			bool tn_free = (DD.is_a_terminal_node(tn) || (DD_node_can_be_crossed(tn)));
			
			if(!sn_free && !tn_free)
				{
				// sn and tn are both real and non-terminal 
				//      
				//           mn1
				//            | 
				// (sn)===============(tn)
				//            | 
				//           mn2
				
				mn1 = DD.map.new_node();
				mn2 = DD.map.new_node();
				
				me12 = DD.map.new_edge(mn1,mn2);
				me21 = DD.map.new_edge(mn2,mn1);
				
				if (length_of_edge(e)>1)
					{
					DD.map_edge_cost[me12] = cross_on_long_dummy_edge;
					DD.map_edge_cost[me21] = cross_on_long_dummy_edge;
					}
				else
					{
					DD.map_edge_cost[me12] = cross_on_dummy_edge;
					DD.map_edge_cost[me21] = cross_on_dummy_edge;
					}
				
				DD.edge_of_map_edge[me12] = e;
				DD.edge_of_map_edge[me21] = e;
				
				
				DD.map.mark(me12,RM3_DD_CROSS_ON_DUMMY_EDGE);
				DD.map.mark(me21,RM3_DD_CROSS_ON_DUMMY_EDGE);
				
				DD_init_map_node_set
					(
					DD.map_nodes_of_edge[e],
					NULL_NODE, mn1, NULL_NODE, mn2,
					position_of_node_with_respect_to_edge(sn,e)
					);
				}
				
			else if ((sn_free && !tn_free) || (!sn_free && tn_free))
				{					
				// sn is real and non-terminal (i.e. non-free)
				// tn is not real or terminal  (i.e. free)
				// or viceversa
				//
				//                 mn1
				//                / | 
				// (free)======mn3==========(non-free)
				//                \ | 
				//                 mn2
				
				if(sn_free) free_node = sn; 
				else        free_node = tn;
				
				mn1 = DD.map.new_node();
				mn2 = DD.map.new_node();
				mn3 = DD.map.new_node();
				me12 = DD.map.new_edge(mn1,mn2);                     me21 = DD.map.new_edge(mn2,mn1);
				me13 = DD.map.new_edge(mn1,mn3);                     me31 = DD.map.new_edge(mn3,mn1);
				me23 = DD.map.new_edge(mn2,mn3);                     me32 = DD.map.new_edge(mn3,mn2);
				DD.map_edge_cost[me13] = turn_on_dummy_edge_cost;    DD.map_edge_cost[me31] = turn_on_dummy_edge_cost;
				DD.map_edge_cost[me23] = turn_on_dummy_edge_cost;    DD.map_edge_cost[me32] = turn_on_dummy_edge_cost;
				if (length_of_edge(e)>1)
					{
					DD.map_edge_cost[me12] = cross_on_long_dummy_edge;
					DD.map_edge_cost[me21] = cross_on_long_dummy_edge;
					}
				else
					{
					DD.map_edge_cost[me12] = cross_on_dummy_edge;
					DD.map_edge_cost[me21] = cross_on_dummy_edge;
					}
				
				DD.bends_of_map_edge[me13]="R";                    DD.bends_of_map_edge[me31]="L";
				DD.bends_of_map_edge[me23]="L";                    DD.bends_of_map_edge[me32]="R";
				
				
				DD.edge_of_map_edge[me12] = e;
				DD.edge_of_map_edge[me21] = e;
				
				DD.map.mark(me12,RM3_DD_CROSS_ON_DUMMY_EDGE);
				DD.map.mark(me21,RM3_DD_CROSS_ON_DUMMY_EDGE);
				
				// Added by F.M. on June 17 1999
				DD.edge_of_map_edge[me13] = e;
				DD.edge_of_map_edge[me31] = e;
				DD.edge_of_map_edge[me23] = e;
				DD.edge_of_map_edge[me32] = e;
				DD.map.mark(me13,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_DUMMY_EDGE);
				DD.map.mark(me23,RM3_DD_TURN_LEFT_AND_WALK_ALONG_DUMMY_EDGE);
				DD.map.mark(me31,RM3_DD_WALK_AND_TURN_LEFT_ALONG_DUMMY_EDGE);
				DD.map.mark(me32,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_DUMMY_EDGE);
				
				DD_init_map_node_set
					(
					DD.map_nodes_of_edge[e],
					mn3, mn1, NULL_NODE, mn2,
					position_of_node_with_respect_to_edge(free_node,e)
					);
				}
				
			else
				{
				/*
				// sn and tn are both not real or terminal
				//
				//               mn1
				//              / | \
				// (sn)======mn3-----mn4======(tn)
				//              \ | /
				//               mn2
				*/
				
				mn1 = DD.map.new_node();
				mn2 = DD.map.new_node();
				mn3 = DD.map.new_node();
				mn4 = DD.map.new_node();
				me12 = DD.map.new_edge(mn1,mn2);                     me21 = DD.map.new_edge(mn2,mn1);
				me13 = DD.map.new_edge(mn1,mn3);                     me31 = DD.map.new_edge(mn3,mn1);
				me14 = DD.map.new_edge(mn1,mn4);                     me41 = DD.map.new_edge(mn4,mn1);
				me23 = DD.map.new_edge(mn2,mn3);                     me32 = DD.map.new_edge(mn3,mn2);
				me24 = DD.map.new_edge(mn2,mn4);                     me42 = DD.map.new_edge(mn4,mn2);
				me34 = DD.map.new_edge(mn3,mn4);                     me43 = DD.map.new_edge(mn4,mn3);
				DD.map_edge_cost[me34] = walk_along_dummy_cost;      DD.map_edge_cost[me43] = walk_along_dummy_cost;
				if (length_of_edge(e)>1)
					{
					DD.map_edge_cost[me12] = cross_on_long_dummy_edge;
					DD.map_edge_cost[me21] = cross_on_long_dummy_edge;
					}
				else
					{
					DD.map_edge_cost[me12] = cross_on_dummy_edge;
					DD.map_edge_cost[me21] = cross_on_dummy_edge;
					}
				
				DD.map_edge_cost[me13] = turn_on_dummy_edge_cost;    DD.map_edge_cost[me31] = turn_on_dummy_edge_cost;
				DD.map_edge_cost[me14] = turn_on_dummy_edge_cost;    DD.map_edge_cost[me41] = turn_on_dummy_edge_cost;
				DD.map_edge_cost[me23] = turn_on_dummy_edge_cost;    DD.map_edge_cost[me32] = turn_on_dummy_edge_cost;
				DD.map_edge_cost[me24] = turn_on_dummy_edge_cost;    DD.map_edge_cost[me42] = turn_on_dummy_edge_cost;
				
				DD.bends_of_map_edge[me13] = "R";                  DD.bends_of_map_edge[me31] = "L";
				DD.bends_of_map_edge[me14] = "L";                  DD.bends_of_map_edge[me41] = "R";
				DD.bends_of_map_edge[me23] = "L";                  DD.bends_of_map_edge[me32] = "R";
				DD.bends_of_map_edge[me24] = "R";                  DD.bends_of_map_edge[me42] = "L";
				
				DD.edge_of_map_edge[me12] = e;
				DD.edge_of_map_edge[me21] = e;
				DD.edge_of_map_edge[me34] = e;
				DD.edge_of_map_edge[me43] = e;
				
				
				DD.map.mark(me12,RM3_DD_CROSS_ON_DUMMY_EDGE);
				DD.map.mark(me21,RM3_DD_CROSS_ON_DUMMY_EDGE);
				
				DD.map.mark(me34,RM3_DD_WALK_ALONG_DUMMY_EDGE);
				DD.map.mark(me43,RM3_DD_WALK_ALONG_DUMMY_EDGE);
				
				// Added by F.M. on June 17 1999
				DD.edge_of_map_edge[me13] = e;
				DD.edge_of_map_edge[me31] = e;
				DD.edge_of_map_edge[me14] = e;
				DD.edge_of_map_edge[me41] = e;
				DD.edge_of_map_edge[me23] = e;
				DD.edge_of_map_edge[me32] = e;
				DD.edge_of_map_edge[me24] = e;
				DD.edge_of_map_edge[me42] = e;
				DD.map.mark(me13,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_DUMMY_EDGE);
				DD.map.mark(me24,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_DUMMY_EDGE);
				DD.map.mark(me23,RM3_DD_TURN_LEFT_AND_WALK_ALONG_DUMMY_EDGE);
				DD.map.mark(me14,RM3_DD_TURN_LEFT_AND_WALK_ALONG_DUMMY_EDGE);
				DD.map.mark(me31,RM3_DD_WALK_AND_TURN_LEFT_ALONG_DUMMY_EDGE);
				DD.map.mark(me42,RM3_DD_WALK_AND_TURN_LEFT_ALONG_DUMMY_EDGE);
				DD.map.mark(me32,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_DUMMY_EDGE);
				DD.map.mark(me41,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_DUMMY_EDGE);
				
				DD_init_map_node_set
					(
					DD.map_nodes_of_edge[e],
					mn3, mn1, mn4, mn2,
					position_of_node_with_respect_to_edge(sn,e) // tn would also do 
					);
				}
			}
			
		else
			{
			// e is a real dime-gdtedge 
			//
			//     f1
			//           mn1
			//            |
			// (sn)===============(tn)
			//            |
			//           mn2
			//                  f2
			
			//-------------------------------------------------------------
			// The nodes mn1 and mn2 are always created, but it is possible
			//  that no gdtedge will be created between them.
			// However it is useful for the successive constructions
			//  (DD_complete_map_subnet_of_edge)
			//-------------------------------------------------------------
			mn1 = DD.map.new_node();
			mn2 = DD.map.new_node();
			
			
			//------------------------------------
			// For "no thick cross" problem
			// a cross on a real gdtedge is allowed
			// only if thickness of gdtedge e is == 1
			//------------------------------------
			if (thickness_of_edge(e) == 1)
				{
				me12 = DD.map.new_edge(mn1,mn2);
				me21 = DD.map.new_edge(mn2,mn1);
				
				if (length_of_edge(e)>1)
					{
					DD.map_edge_cost[me12] = cross_on_long_real_edge;
					DD.map_edge_cost[me21] = cross_on_long_real_edge;
					}
				else
					{
					DD.map_edge_cost[me12] = cross_on_real_edge;
					DD.map_edge_cost[me21] = cross_on_real_edge;
					}
				
				DD.edge_of_map_edge[me12] = e;
				DD.edge_of_map_edge[me21] = e;
				
				DD.map.mark(me12, RM3_DD_CROSS_ON_REAL_EDGE);
				DD.map.mark(me21, RM3_DD_CROSS_ON_REAL_EDGE);
				}
				
			DD_init_map_node_set
				(
				DD.map_nodes_of_edge[e],
				NULL_NODE, mn1, NULL_NODE, mn2,
				position_of_node_with_respect_to_edge(sn,e)
				);
			}
		}
	}
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.2 - Create the map-subnet for each crossable or terminal dime-gdtnode
	//
	void
	dime_orth_plan_undi_graph::
DD_create_map_subnet_for_each_crossable_or_terminal_node
	(DD_struct& DD)
	{
	DD_create_map_nodes_around_each_crossable_or_terminal_node(DD);
	DD_complete_map_subnet_for_each_crossable_node(DD);
	DD_complete_map_subnet_for_both_terminal_nodes(DD);
	}
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.2.1 - Start creating the external map-nodes of the subnet
	//
	void
	dime_orth_plan_undi_graph::
DD_create_map_nodes_around_each_crossable_or_terminal_node
	(DD_struct& DD)
	{
	int  i;            // cycle counter
	gdtnode v;            // current dime-gdtnode
	gdtedge e;            // current dime-gdtedge
	heading_type h;    // current heading (cycles according to i)
	gdtnode mn1;          // current map-gdtnode
	gdtedge ae[4];        // array of dime-edges adjacent current dime-gdtnode v (0-north, 1-east, 2-south,3-west)
	
	forall_nodes(v, *this)
		if (DD_node_can_be_crossed(v) || DD.is_a_terminal_node(v))
			{
			for(i=0;i<4;i++) ae[i] = NULL_EDGE;                                                                // reset adjacent edges array
			forall_edges_adjacent_node (e,v,*this) ae[(short)position_of_edge_with_respect_to_node(e,v)] = e;  // init adjacent edges array
			
			//              north
			//
			//              ae[0]
			//               |
			//  west  ae[3]-----ae[1]  east
			//               |
			//              ae[2]
			//
			//              south
			
			
			for(i=0;i<4;i++)
				{
				e = ae[i];
				h = (heading_type)i;
				if(!e || edge_is_dummy(e))
					{
					// Create a new map-gdtnode mn1 at heading [h] of v,
					mn1 = DD.map.new_node();
					DD_node_of_map_node_set_with_heading(DD.map_nodes_of_node[v], h) = mn1;
					}				    
				}
			}
		
	}
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.2.2 - Complete the map-gdtnode subnet for each crossable dime-gdtnode
	//
	void
	dime_orth_plan_undi_graph::
DD_complete_map_subnet_for_each_crossable_node
	(DD_struct& DD)
	{
	gdtnode v;            // current dime-gdtnode
	gdtedge me12, me21;   // current map-edges
	
	int turn_on_dummy_node_cost        = DD.bend_cost*2,
	    across_dummy_node_cost         = 1,
	    across_dummy_node_between_real = DD.cross_cost*2 + 1;
	
	forall_nodes(v, *this)
		if (DD_node_can_be_crossed(v))
			{						
			// create map-gdtedge between map-nodes of the map_node_set
			// associated to the current dummy dime-gdtnode
			
			gdtnode mnN = DD.map_nodes_of_node[v].north_node;
			gdtnode mnE = DD.map_nodes_of_node[v].east_node;
			gdtnode mnS = DD.map_nodes_of_node[v].south_node;
			gdtnode mnW = DD.map_nodes_of_node[v].west_node;
			
			if(mnN && mnE)
				{
				gdtedge me12 = DD.map.new_edge(mnN,mnE);
				gdtedge me21 = DD.map.new_edge(mnE,mnN);
				DD.map_edge_cost[me12] = turn_on_dummy_node_cost;
				DD.map_edge_cost[me21] = turn_on_dummy_node_cost;
				DD.bends_of_map_edge[me12] = "L";
				DD.bends_of_map_edge[me21] = "R";
				}
			if(mnN && mnW)
				{
				gdtedge me12 = DD.map.new_edge(mnN,mnW);
				gdtedge me21 = DD.map.new_edge(mnW,mnN);
				DD.map_edge_cost[me12] = turn_on_dummy_node_cost;
				DD.map_edge_cost[me21] = turn_on_dummy_node_cost;
				DD.bends_of_map_edge[me12] = "R";
				DD.bends_of_map_edge[me21] = "L";
				}
			if(mnE && mnS)
				{
				gdtedge me12 = DD.map.new_edge(mnE,mnS);
				gdtedge me21 = DD.map.new_edge(mnS,mnE);
				DD.map_edge_cost[me12] = turn_on_dummy_node_cost;
				DD.map_edge_cost[me21] = turn_on_dummy_node_cost;
				DD.bends_of_map_edge[me12] = "L";
				DD.bends_of_map_edge[me21] = "R";
				}
			if(mnS && mnW)
				{
				gdtedge me12 = DD.map.new_edge(mnS,mnW);
				gdtedge me21 = DD.map.new_edge(mnW,mnS);
				DD.map_edge_cost[me12] = turn_on_dummy_node_cost;
				DD.map_edge_cost[me21] = turn_on_dummy_node_cost;
				DD.bends_of_map_edge[me12] = "L";
				DD.bends_of_map_edge[me21] = "R";
				}
				
			if ((mnN && mnS) || (mnE && mnW))
				{
				int cost = 0;
				marker_type marker = RM3_UNDEFINED_MARKER; 
				switch (DD_number_of_real_edges_around_node(v))
					{
					case 2: {
						marker = RM3_DD_CROSS_ON_DUMMY_NODE_BETWEEN_REAL_EDGES;
						// Modified by F.M. on June 21 1999
						//
						// before:
						// cost = DD.cross_cost;
						//
						// See below for the reasons.
						// This is necessary when the user choose the cross_cost = 0
						cost = across_dummy_node_between_real;
						}
 					break;
 					case 0: {
					        marker = RM3_DD_CROSS_ON_DUMMY_NODE_BETWEEN_DUMMY_EDGES;
						// Modified by F.M. on June 21 1999
						//
						// before:
						// cost = 0;
						//
						// This was modified to avoid the following problem:
						// locally the map subnet of gdtnode has two edges (the cross edges) with 0 cost;
						// in the case of bend on this gdtnode,
						// the minimal cost path may choose indifferently the path of one bend or
						// one cross, a bend and another cross
						//
						// 1)         | 2)     \3     |
						//            |        |\     |
						//    1\      |      1-+-\2   |
						//      \     |        |      |
						//       \2   |        4      |
						//
						// Giving at the cross a small cost, this problem is avoided locally,
						// and any optimal solution that involve a bend on this gdtnode
						// will choose the first solution!
						cost = across_dummy_node_cost;
						}
					break;
					default: gdt_error("default non handled in switch");
					}
				if (mnN && mnS)
					{
					me12 = DD.map.new_edge(mnN,mnS);
					me21 = DD.map.new_edge(mnS,mnN);
					DD.node_of_map_edge[me12] = v;
					DD.node_of_map_edge[me21] = v;
					DD.map_edge_cost[me12] = cost;
					DD.map_edge_cost[me21] = cost;
					DD.map.mark(me12,marker);
					DD.map.mark(me21,marker);
					}
				if (mnE && mnW)
					{
					me12 = DD.map.new_edge(mnE,mnW);
					me21 = DD.map.new_edge(mnW,mnE);
					DD.node_of_map_edge[me12] = v;
					DD.node_of_map_edge[me21] = v;
					DD.map_edge_cost[me12] = cost;
					DD.map_edge_cost[me21] = cost;
					DD.map.mark(me12,marker);
					DD.map.mark(me21,marker);
					}
				}	
				
			}
	}
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.2.3 - Create the map-subnet for both the terminal dime-nodes
	//
	void
	dime_orth_plan_undi_graph::
DD_complete_map_subnet_for_both_terminal_nodes
	(DD_struct& DD)
	{
	gdtnode source_terminal = DD.source_terminal();
	gdtnode sink_terminal   = DD.sink_terminal();
	
	map_node_set mns1 = DD.map_nodes_of_node[source_terminal];
	map_node_set mns2 = DD.map_nodes_of_node[sink_terminal];
	
	gdtnode mn1N = mns1.north_node;
	gdtnode mn1E = mns1.east_node;
	gdtnode mn1S = mns1.south_node;
	gdtnode mn1W = mns1.west_node;
	
	gdtnode mn2N = mns2.north_node;
	gdtnode mn2E = mns2.east_node;
	gdtnode mn2S = mns2.south_node;
	gdtnode mn2W = mns2.west_node;
	
	DD.mnt1 = DD.map.new_node();
	DD.mnt2 = DD.map.new_node();
	
	gdtedge me;
	if(mn1N) {me = DD.map.new_edge(DD.mnt1,mn1N); DD.map_edge_cost[me] = 0; DD.node_of_map_edge[me] = source_terminal;}
	if(mn1E) {me = DD.map.new_edge(DD.mnt1,mn1E); DD.map_edge_cost[me] = 0; DD.node_of_map_edge[me] = source_terminal;}
	if(mn1S) {me = DD.map.new_edge(DD.mnt1,mn1S); DD.map_edge_cost[me] = 0; DD.node_of_map_edge[me] = source_terminal;}
	if(mn1W) {me = DD.map.new_edge(DD.mnt1,mn1W); DD.map_edge_cost[me] = 0; DD.node_of_map_edge[me] = source_terminal;}
	
	if(mn2N) {me = DD.map.new_edge(mn2N,DD.mnt2); DD.map_edge_cost[me] = 0; DD.node_of_map_edge[me] = sink_terminal;}
	if(mn2E) {me = DD.map.new_edge(mn2E,DD.mnt2); DD.map_edge_cost[me] = 0; DD.node_of_map_edge[me] = sink_terminal;}
	if(mn2S) {me = DD.map.new_edge(mn2S,DD.mnt2); DD.map_edge_cost[me] = 0; DD.node_of_map_edge[me] = sink_terminal;}
	if(mn2W) {me = DD.map.new_edge(mn2W,DD.mnt2); DD.map_edge_cost[me] = 0; DD.node_of_map_edge[me] = sink_terminal;}
	}
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.3 - Connect all the map-subnets to get the final map
	//
	void
	dime_orth_plan_undi_graph::
DD_connect_map_subnets
	(DD_struct& DD)
	{
	DD_connect_map_subnets_for_nodes_among_them (DD);
	DD_connect_map_subnets_for_edges_among_them (DD);
	DD_connect_map_subnets_for_nodes_with_map_subnets_for_edges (DD);
	}
		
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.3.1 - Connect among them the map-subnets
	//              previously created for each dime-gdtedge
	//  
	void
	dime_orth_plan_undi_graph::
DD_connect_map_subnets_for_edges_among_them
	(DD_struct& DD)
	{
	face f;
	border_step bs1;
	border_step bs2;
	
	// Create all the map-edges
	// crossing each face betweeen two dime-edges
	
	forall_faces(f,*this)
		{
		bs1 = first_border_step(f);
		while (bs1)
			{
			bs2 = succ_border_step(bs1);
			while (bs2)
				{
				DD_create_map_edges_between_border_steps (bs1, bs2, DD);
				bs2 = succ_border_step(bs2);
				}
			bs1 = succ_border_step(bs1);
			}
		}
	}
	
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.3.2 - Connect among them the map-subnets
	//              previously created for each crossable or terminal dime-gdtnode
	//
	void
	dime_orth_plan_undi_graph::
DD_connect_map_subnets_for_nodes_among_them
	(DD_struct& DD)
	{
	face f;
	border_step bs1;
	border_step bs2;
	gdt::gdtlist<border_step> bsl;
	gdt::gdtlist<gdtnode> border_nodes;
	gdt::gdtlist<gdtnode> dummy_border_nodes_1;
	gdt::gdtlist<gdtnode> dummy_border_nodes_2;
	
	// Create all the map-edges
	// crossing each face betweeen two dime-edges
	
	
	forall_faces(f,*this)
		{
		bs1 = first_border_step(f);
		while (bs1)
			{
			// Create all the map-edges
			// crossing each face betweeen two dime-nodes
			// NOTE(1) : only crossable and terminal dime-nodes are considered
			// NOTE(2) : dime-nodes on face corners are also skipped
					
			gdtnode       v1  = start_of_border_step(bs1); 
			angle_type av1 = angle_on_node (v1,f);
			
			if ((DD_node_can_be_crossed(v1) || DD.is_a_terminal_node(v1)) && (av1!=_090))
				{
				bs2 = succ_border_step(bs1);
				while (bs2)
					{
					gdtnode       v2  = start_of_border_step(bs2);
					angle_type av2 = angle_on_node (v2,f);
					
					if ((DD_node_can_be_crossed(v2) || DD.is_a_terminal_node(v2)) && (av2!=_090))
						{
						heading_type h1 = initial_heading_of_border_step(bs1);
						heading_type h2 = initial_heading_of_border_step(bs2);
						
						// Modified by F.M. on August 30 2000
						//
						// before:
						//
						DD_create_map_edges_between_start_node_of_border_steps
							(
							bs1, heading_after_rotation(h1,_090),
							bs2, heading_after_rotation(h2,_090),
							DD
							);
							
						if (av1==_270) DD_create_map_edges_between_start_node_of_border_steps
							(
							bs1, heading_after_rotation(h1,_180),
							bs2, heading_after_rotation(h2,_090),
							DD
							);
							
						if (av2==_270) DD_create_map_edges_between_start_node_of_border_steps
							(
							bs1, heading_after_rotation(h1,_090),
							bs2, heading_after_rotation(h2,_180),
							DD
							);
							
						if ((av1==_270)&&(av2==_270)) DD_create_map_edges_between_start_node_of_border_steps
							(
							bs1, heading_after_rotation(h1,_180),
							bs2, heading_after_rotation(h2,_180),
							DD
							);
						//
						// after
						/*
						//   because the precondition of NOTE(2) above (dime-nodes on face corners are skipped)
						//   and because the loop is performed on a face (with no pending edges)
						//   the connection must be performed only between pair of nodes with these angle (av1, av2):
						//   (180 180), (180 360), (180 270), (360 180), (270 180), (360 360), (270 270)
						//   
						//   because the counterclock rotation of border steps in an internal face
						//   and because the counterclock rotation of angle type
						//   the only connetcions to perform to grant all the possible cases for a face (external or internal)
						//   are summarized in the following table:
						//   a single row is a single couplet of dime nodes with certain angle of gdtnode (av1, av2);
						//   a single column is what the headings h1, h2 must be rotated to find the map nodes of v1 and v2 that will be connected
						//             ||  0 0 | 0 90 | 90 0 | 90 90 | 90 270 | 270 90 | 270 270
						//   ----------++------+------+------+-------+--------+--------+--------
						//   180  180  ||      |      |      |   X   |        |        |        
						//   180  270  ||      |      |   X  |   X   |        |        |        
						//   180  360  ||      |      |      |   X   |        |        |        
						//   270  180  ||      |  X   |      |   X   |        |        |        
						//   270  270  ||      |  X   |      |       |        |        |        
						//   360  180  ||      |      |      |   X   |        |        |        
						//   360  360  ||   X  |      |      |       |   X    |    X   |        
						//   
						if ((av1==_180) && (av2==_180))
							{
							DD_create_map_edges_between_start_node_of_border_steps
								(
								bs1, heading_after_rotation(h1,_090),
								bs2, heading_after_rotation(h2,_090),
								DD
								);
							}
							
						else if ((av1==_180) && (av2==_270))
							{
							DD_create_map_edges_between_start_node_of_border_steps
								(
								bs1, heading_after_rotation(h1,_090),
								bs2, h2,
								DD
								);
							DD_create_map_edges_between_start_node_of_border_steps
								(
								bs1, heading_after_rotation(h1,_090),
								bs2, heading_after_rotation(h2,_090),
								DD
								);
							}
							
						else if ((av1==_180) && (av2==_360))
							{
							DD_create_map_edges_between_start_node_of_border_steps
								(
								bs1, heading_after_rotation(h1,_090),
								bs2, heading_after_rotation(h2,_090),
								DD
								);
							}
							
						else if ((av1==_270) && (av2==_180))
							{
							DD_create_map_edges_between_start_node_of_border_steps
								(
								bs1, h1,
								bs2, heading_after_rotation(h2,_090),
								DD
								);
							DD_create_map_edges_between_start_node_of_border_steps
								(
								bs1, heading_after_rotation(h1,_090),
								bs2, heading_after_rotation(h2,_090),
								DD
								);
							}
							
						else if ((av1==_270) && (av2==_270))
							{
							DD_create_map_edges_between_start_node_of_border_steps
								(
								bs1, h1,
								bs2, heading_after_rotation(h2,_090),
								DD
								);
							}
							
						else if ((av1==_360) && (av2==_180))
							{
							DD_create_map_edges_between_start_node_of_border_steps
								(
								bs1, heading_after_rotation(h1,_090),
								bs2, heading_after_rotation(h2,_090),
								DD
								);
							}
							
						else if ((av1==_360) && (av2==_360))
							{
							DD_create_map_edges_between_start_node_of_border_steps
								(
								bs1, h1,
								bs2, h2,
								DD
								);
							DD_create_map_edges_between_start_node_of_border_steps
								(
								bs1, heading_after_rotation(h1,_090),
								bs2, heading_after_rotation(h2,_270),
								DD
								);
							DD_create_map_edges_between_start_node_of_border_steps
								(
								bs1, heading_after_rotation(h1,_270),
								bs2, heading_after_rotation(h2,_090),
								DD
								);
							}
						//else nothing to do
						*/
						// end after
						}
					bs2 = succ_border_step(bs2);
					}
				}
			bs1 = succ_border_step(bs1);
			}
		}
	}
	
	
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.3.3 - Connect the map-subnets previously created for each crossable or terminal dime-gdtnode
	//            with the map-subnets previously created for each dime-gdtedge
	//
	void
	dime_orth_plan_undi_graph::
DD_connect_map_subnets_for_nodes_with_map_subnets_for_edges
	(DD_struct& DD)
	{
	int  i;            // cycle counter
	gdtnode v;            // current dime-gdtnode
	gdtedge e;            // current dime-gdtedge
	face f;            // current dime-face
	heading_type h;    // current heading (cycles according to i)
	
	gdtnode mn1, mn2;     // current map-nodes
	gdtedge me12, me21;   // current map-edges
	
	short bends;       // number of bends  (for map-gdtedge cost calculation)
	int   length;      // total length     (for map-gdtedge cost calculation)
	int   length_full; // length units
	int   length_half; // length half-units
	bool  ordered;     // map-gdtedge from the current dummy-gdtnode to the current dime-gdtedge corresponds to the face rotation versus 
	
	gdtedge  ae[4];       // array of dime-edges adjacent current dime-gdtnode v (0-north, 1-east, 2-south,3-west)
	
	forall_nodes(v, *this)
		// Modified by F.M. on June 17 1999
		//
		// before:
		//if (DD_node_can_be_crossed(v) || DD.is_a_terminal_node(v))
		if (!DD.map_nodes_of_node[v].empty())
			{						
			for(i=0;i<4;i++) ae[i] = NULL_EDGE;                                                                // reset adjacent edges array
			forall_edges_adjacent_node (e,v,*this) ae[(short)position_of_edge_with_respect_to_node(e,v)] = e;  // init adjacent edges array
			
			//              north
			//
			//              ae[0]
			//               |
			//  west  ae[3]-----ae[1]  east
			//               |
			//              ae[2]
			//
			//              south
			
			for(i=0;i<4;i++)
			    {
			    e = ae[i];
			    h = (heading_type)i;
			    
			    if(!e)
				    {
				    // Find the map-gdtnode mn1 placed at heading h with respect to gdtnode v,
				    // find the face f where mn1 is placed
				    // then connect mn1 with the map-nodes along the edges of f
				    //
				    mn1 = DD_node_of_map_node_set_with_heading(DD.map_nodes_of_node[v], h);
				    f   = face_visible_from_node_looking_with_heading (v,h); // e = NULL_EDGE -> f != NULL_FACE
				    
				    border_step bs;
				    forall_face_border_steps (bs,f)
					    {
					    gdtedge         bse  = edge_of_border_step    (bs);          // gdtedge of the current border step
					    heading_type bsh  = initial_heading_of_border_step (bs);  // heading of the current border_step
					    
 					    int          idis;    // inline distance between v and bse
					    int          tdis;    // traversal distance between v and bse	
					    
					    mn2 = DD_node_of_map_node_set_with_heading (DD.map_nodes_of_edge[bse], heading_after_rotation(bsh,_090));
					    
					    
					    // Added by F.M. on June 17 1999
					    //
					    // In 4pl case these two gdtnode always exist.
					    // In >4pl case not only the crossable nodes have the map subnet and then
					    // there be nodes with subnet to allow to across the gdtnode;
					    // therefore these nodes can have a dummy gdtedge around without map gdtnode;
					    // for these gdtnode along that heading there be not any map gdtnode and then
					    // no map gdtedge from there must exist.
					    if (mn1 && mn2)
						{
						me12 = DD.map.new_edge(mn1,mn2);
						me21 = DD.map.new_edge(mn2,mn1);
						
						if(bsh == heading_after_rotation(h,_270))
 							{
							// -----------------------------------------
							// v and bse are on the SAME SIDE of face f
							// ----------------------------------------
							// NOTE: length and bends number calculation
							// is the same for external and internal faces
							//
							//
							//          ******************
							//     f    *                *
							//          *                *    <--bs
							// o--------v--------o=======bse=======o
							// |                                   | h   = north
							// |                                   | bsh = west
							// |                                   |
							// |                                   |
							// |                                   |
							// o-----------------------------------o
							// 
							// o-----------------------------------o
							// |                                   | h   = north
							// |   f                               | bsh = west
							// |        ******************         |
							// |        *                *         |
							// |        *                *   <--bs |
							// o--------v--------o=======bse=======o
							//
							bends       = 2;
							length_full = min_border_distance(v,bse,f,ordered) + 2;
							length_half = length_of_edge(bse);
							DD.bends_of_map_edge[me12]="LL";
							DD.bends_of_map_edge[me21]="RR";
							}
						
						else if(bsh == heading_after_rotation(h,_090))
							{
							// -----------------------------------------
							// v and bse are on OPPOSITE SIDES of face f
							// -----------------------------------------
							//
							if(f == external_face())
								{
								//               **************************
								//      f        *                        *
								//               *                        *
								//  o------------v-------------------o    *    h   = north
								//  |                                |    *    bsh = east
								//  |                                |    *
								//  |                                |    *
								//  o-----o======bse======o----------o    *
								//         bs-->  *                       *
								//                *                       *
								//                *************************
								//
								bends       = 4;
								length_full = min_border_distance(v,bse,f,ordered) + 6;
								length_half = length_of_edge(bse);
								DD.bends_of_map_edge[me12]="RRRR";
								DD.bends_of_map_edge[me21]="LLLL";
								}
							else
								{
								idis = inline_distance(v,bse);
								tdis = traversal_distance(v,bse);
								if(idis>=0)
									{
									// The projection of v on the side of bse falls outside bse
									// therefore 2 bends might be required.
									// However, some gdtedge stretching is possible during the rectangularization
									// placing the projection of v within e. Therefore, we prefer here
									// NOT to assign any bend-cost.
									//
									//  o-----o=====bse======o-----------o
									//  |            *  bs-->            |  h   = north
									//  |   f        *                   |  bsh = east
									//  |            ?************?      |
									//  |                         *      |
									//  |                         *      |
									//  o-------------------------v------o
									//
									bends       = 0;
									length_full = idis + tdis;
									length_half = length_of_edge(bse);
									DD.bends_of_map_edge[me12]="LR";
									DD.bends_of_map_edge[me21]="LR";
									}
								else
									{ 
									// The projection of v on the side of bse
									// falls inside bse (no bends required)
									//
									//  o-----o=====bse======o-----------o
									//  |            *  bs-->            |  h   = north
									//  |            *                   |  bsh = east
									//  |            *                   |
									//  |            *                   |
									//  |            *                   |
									//  o------------v-------------------o
									//
									bends       = 0;
									length_full = tdis;
									length_half = 0;
									DD.bends_of_map_edge[me12]="";
									DD.bends_of_map_edge[me21]="";
									}
								}
							}
						else
							// -----------------------------------------
							// v and bse are on ADJACENT SIDES of face f
							// -----------------------------------------
							//
							{
							if(f == external_face())
								{
								//    f     *************
								//          *           *
								// o--------v-------o   *
								// |                |   *   h   = north
								// |                o   *   bsh = north
								// |                #   *
								// |                #   *
								// |            bse #****
								// |                #  +
								// |                #  |
								// |                o
								// |                |
								// o----------------o
								//
								// **************      f
								// *            *
								// *   o--------v-------o
								// *   |                |   h   = north
								// *   o                |   bsh = south
								// *   #                |
								// *   #                |
								// ****#  bse           |
								//     #                |
								//  |  #                |
								//  +  o                |
								//     |                |
								//     o----------------o
								//
								bends       = 3;
								length_full = min_border_distance(v,bse,f,ordered)+4;
								length_half = length_of_edge(bse);
								if (bsh == h)
								    {
								    DD.bends_of_map_edge[me12]="RRR";
								    DD.bends_of_map_edge[me21]="LLL";
								    }
								else
								    {
								    DD.bends_of_map_edge[me12]="LLL";
								    DD.bends_of_map_edge[me21]="RRR";
								    }
								}
							else
								{
								// o----------------o
								// |                |   h   = north
								// |       f        o   bsh = south
								// |                #
								// |                #
								// |        ********# bse
								// |        *     | #
								// |        *     + #
								// |        *       o
								// |        *       |
								// o--------v-------o
								//
								// o----------------o
								// |                |   h   = north
								// o       f        |   bsh = north
								// #                |
								// #                |
								// #********        |
								// # +     *        |
								// # |     *        |
								// o       *        |
								// |       *        |
								// o-------v--------o
								//
								bends       = 1;
								length_full = min_border_distance(v,bse,f,ordered);
								length_half = length_of_edge(bse);
								if (bsh == h)
								    {
								    DD.bends_of_map_edge[me12]="L";
								    DD.bends_of_map_edge[me21]="R";
								    }
								else
								    {
								    DD.bends_of_map_edge[me12]="R";
								    DD.bends_of_map_edge[me21]="L";
								    }
								}
							}
						
						// All lengths are doubled everywere in the map
						// because the min-cost-flow costs must be integer
						// and it is not possible di specify a 1/2 cost
						//
						length = length_full*2 + length_half;
						
						DD.map_edge_cost[me12] = DD.length_unit_cost*length + DD.bend_cost*bends*2;
						DD.map_edge_cost[me21] = DD.length_unit_cost*length + DD.bend_cost*bends*2;
						
						DD.face_of_map_edge[me12] = f;
						DD.face_of_map_edge[me21] = f;
						
						DD.map.mark(me12,RM3_DD_CROSS_ON_FACE);
						DD.map.mark(me21,RM3_DD_CROSS_ON_FACE);
						}
					    }
				    } // forall_face_border_steps
				    
			    else if(edge_is_dummy(e))
				    {
				    // Find the map-gdtnode mn1 placed at heading h with respect to gdtnode v,
				    // find the map_node mn2 on the dime-gdtedge adjacent v at heading [h]
				    // then connect mn1 and mn2 with a new map_edge
				    
				    length_full = 0;
				    length_half = length_of_edge(e);
				    
				    mn1 = DD_node_of_map_node_set_with_heading(DD.map_nodes_of_node[v], h);
				    mn2 = DD_node_of_map_node_set_with_heading(DD.map_nodes_of_edge[e], heading_after_rotation(h,_180));
				    
				    // Added by F.M. on June 17 1999
				    //
				    // In 4pl case these two gdtnode always exist.
				    // In >4pl case not only the crossable nodes have the map subnet and then
				    // there be nodes with subnet to allow to across the gdtnode;
				    // therefore these nodes can have a dummy gdtedge around without map gdtnode;
				    // for these gdtnode along that heading there be not any map gdtnode and then
				    // no map gdtedge from there must exist.
				    if (mn1 && mn2)
				    	{
					// mn1 belongs to the map_node_set associated to the dummy dime-gdtnode v
					// mn2 belongs to the map_node_set associated to the dummy dime-gdtedge adjacent v
					
					me12 = DD.map.new_edge(mn1,mn2);
					me21 = DD.map.new_edge(mn2,mn1);
					
					DD.bends_of_map_edge[me12]="";
					DD.bends_of_map_edge[me21]="";
					
					// All lengths are doubled everywere in the map
					// because the min-cost-flow costs must be integer
					// and it is not possible di specify a 1/2 cost.
					//
					//
					length = length_full*2 + length_half;
					
					DD.map_edge_cost[me12] = DD.length_unit_cost*length;
					DD.map_edge_cost[me21] = DD.length_unit_cost*length;
					
					DD.edge_of_map_edge[me12] = e;
					DD.edge_of_map_edge[me21] = e;
					
					//DD.map.mark(me12,RM3_DD_WALK_ALONG_DUMMY_EDGE);
					//DD.map.mark(me21,RM3_DD_WALK_ALONG_DUMMY_EDGE);
				    	}
				    }
			    else
				    {
				    // Edge e adjacent dummy-gdtnode v is real,
				    // therefore no map-gdtnode exists to be connected.
				    //
				    }
			} // for each heading h around v
		
		} // if v is crossable or terminal
	}
	
	
	
	
	//===========================================================================================
	// OPERATIONS ADDED TO EXPAND THE DYNAMIC DRAWING TO PROBLEMS WITH NODES OF DEGREE > 4
	//===========================================================================================
	
	
	
	gdtnode
	dime_orth_plan_undi_graph::
find_node_after_edge_along_heading
	(
	gdtedge e,
	heading_type h
	)
	{
	gdtnode n;
	n = source(e);
	if (find_edge_leaving_node_with_heading(n, h) == e) return opposite(n, e);
	n = target(e);
	if (find_edge_leaving_node_with_heading(n, h) == e) return opposite(n, e);
	n = NULL_NODE;
	//if (n) cout << "\nnode after gdtedge " << id(e) << " along heading " << int(h) << " = " << id(n) << flush;
	return n;
	}
	
	
	
	heading_type
	dime_orth_plan_undi_graph::
DD_heading_of_edge_with_max_thickness_around_bend_node
	(
	gdtnode n
	)
	{
	int          tmax; // maximum thickness of the edges around n
	heading_type hmax; // heading of gdtedge with maximum thickness around n
	int i;
	heading_type hcur; // current heading
	gdtedge         ecur; // current gdtedge
	int          tcur; // thickness of current gdtedge
	
	tmax = 0;
	hmax = undefined_heading;
	for (i = 0; i < 4; ++i)
		{
		hcur = (heading_type)i;
		ecur = find_edge_leaving_node_with_heading(n, hcur);
		if (ecur)
			{
			tcur = thickness_of_edge(ecur);
			if (tcur > tmax)
				{
				tmax = tcur;
				hmax = hcur;
				}
			}
		}
	//cout << "\nheading of gdtedge with max thickness around bend gdtnode " << id(n) << " is " << int(hmax) << flush;
	return hmax;
	}
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.4 - Complete the map-subnets with the particularity that
	//            every terminal dime-gdtnode introduces
	//            Added for expansion to >4plan problems
	//
	void
	dime_orth_plan_undi_graph::
DD_complete_map_subnet_with_particularity_of_terminal_nodes
	(DD_struct& DD)
	{
	DD_complete_map_subnet_for_terminal_node (DD, DD.vt1, source_terminal);
	DD_complete_map_subnet_for_terminal_node (DD, DD.vt2, sink_terminal);
	}
	
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.4.1 - Complete the map-subnet with the particularity that
	//              the terminal dime-gdtnode (source or sink) introduces.
	//              Added for expansion to >4plan problems
	//
	void
	dime_orth_plan_undi_graph::
DD_complete_map_subnet_for_terminal_node
	(
	DD_struct& DD,
	gdtnode tn,
	DD_terminal_node_type tnt
	)
	{
	map_node_set mnst = DD.map_nodes_of_node[tn];
	gdtnode mnt[4];
	DD_get_map_node_set
		(
		mnst,
		mnt[0], mnt[1], mnt[2], mnt[3],
		heading_type(0)
		);
	
	//------------------------------------------------------------------------------------
	// For each heading
	//------------------------------------------------------------------------------------
	int j;
	//cout << "\nFor each heading:" << flush;
	for(j=0; j<4; j++)
		{
		//---------------------------------------------------
		// Completation of subnet of the terminal gdtnode itself
		// in the current direction
		//---------------------------------------------------
		if (!mnt[j])
			{
			mnt[j] = DD.map.new_node();
			gdtedge me;
			if (tnt == source_terminal)
				me = DD.map.new_edge(DD.mnt1, mnt[j]);
			else
				me = DD.map.new_edge(mnt[j], DD.mnt2);
			DD.map_edge_cost   [me] = 0;
			DD.edge_of_map_edge[me] = find_edge_leaving_node_with_heading(tn, heading_type(j));
			DD.node_of_map_edge[me] = tn;
			//-------------------------------------------------------
			// Updating of the map-gdtnode set with the completed values
			//-------------------------------------------------------
			DD_init_map_node_set
				(
				DD.map_nodes_of_node[tn],
				mnt[0], mnt[1], mnt[2], mnt[3],
				heading_type(0)
				);
			}
		else
			{
			gdtedge me;
			if (tnt == source_terminal)
				//{
				me = DD.map.find_directed_edge(DD.mnt1, mnt[j]);
				//if (!me)
				//	{
				//	cout << "\ncase of terminal gdtnode: gdtnode exists but gdtedge not found" << flush;
				//	}
				//}
			else
				//{
				me = DD.map.find_directed_edge(mnt[j], DD.mnt2);
				//if (!me)
				//	{
				//	cout << "\ncase of terminal gdtnode: gdtnode exists but gdtedge not found" << flush;
				//	}
				//}
			DD.node_of_map_edge[me] = tn;
			}
		
		//-------------------------------------------------------------------------------
		// A terminal line is a straight line which starts from terminal gdtnode
		// and walk always in the same heading
		// until the condition of terminal line stop are verified.
		// These condition are grouped in the method DD_edge_sequence_along_terminal_line
		//-------------------------------------------------------------------------------
		
		//------------------------------------------------------------------------------------
		// Creation of list of gdtedge to modify on the terminal line along the current direction
		//------------------------------------------------------------------------------------
		//cout << "\n\tHeading " << j << flush;
		//cout << "\n\t\tCreation of list of gdtedge to modify on the terminal line along the current heading" << flush;
		heading_type h = (heading_type)j;
		gdt::gdtlist<gdtedge> el;	//list of edges to modify on the terminal line along the current direction
		gdtedge eagle_boundary = NULL_EDGE;
		bool cross_last_node;
		el = DD_edge_sequence_along_terminal_line(DD, tn, h, eagle_boundary, cross_last_node);
		//if (eagle_boundary) cout << "\n\tcomplete_for_terminal::eagle boundary gdtedge is " << id(eagle_boundary) << "\n" << flush;
		gdtnode pred = tn;
		gdtnode succ;
		
		//------------------------------------------------------------------------------------------------------------------
		// For each terminal line, particular attention must have the following situation:
		// in the picture to the left there is a simple situation that in the following will be named "eagle".
		// There are more complex cases, like the central picture, that are legal and the Dynamic draw must allow these.
		// But the Dynamic must not allow cases like the right picture: in this case infact, at the height 3, there are
		//                                                                 the gdtedge from 0 to 3 and the one from 5 to 3
		//                       |-----------|          |-------------|    that turn on the same height and this is
		//         O             |           |          |             |    forbidden in the GDT philosophy.
		//         |             |-----------|     5    |-------------|    In consequence of that, it can be assert that
		//         |               | | | | |                  | | | |      between two opposite eagles it must be present
		// O-----| | |-----O   O---| | | | |---O   4  O-------| | | |---O  a segment of thickness = 1 (in the center picture
		//       | | |               | | |                      | |        between height 2 and 3) called eagle boundary.
		//       | | |         O-----| | |-----O   3  O-------| | |-----O  Therefore each eagle area is independent from
		// O---| | | | |---O           |                      | |          the other.
		//     | | | | |       O-----| | |-----O   2  O-----| | | |-----O  The map constructor for the gdtedge must allow the
		//     | | | | |             | | |                  | | | |        diagonal edges outgoing from south for the level
		//   |-----------|     O---| | | | |---O   1  O---| | | | | |---O  0-1, 1-2, all the four diagonals edges for the
		//   |           |         | | | | |              | | | | | |      level 2-3 and no diagonals (or if 5 is a terminal
		//   |           |       |-----------|     0    |-------------|    gdtnode only the entering diagonals towards north)
		//   |-----------|       |           |          |             |    for the level 3-4 and 4-5.
		//                       |-----------|          |-------------|
		//   SIMPLE EAGLE
		//                    LEGAL DOUBLE EAGLE     ILLEGAL DOUBLE EAGLE
		//
		//------------------------------------------------------------------------------------------------------------------
		
		DD_edge_completament_type ect;
		//------------------------------------------------------------------------------------
		// For each gdtedge
		//------------------------------------------------------------------------------------
		gdtedge e;
		gdt::list_item li;
		forall_items (li, el)
			{
			e = el.inf(li);
			//cout << "\n\tcurrent gdtedge of sequence of terminal line is " << id(e) << flush;
			
			//-------------------------------------------------------
			// Choose of the type of completation of the current gdtedge
			//-------------------------------------------------------
			//int et = thickness_of_edge(e);
			if (e == eagle_boundary)
				//{
				//cout << "\n\tedge " << id(e) << " is an eagle boundary gdtedge" << flush;
				ect = ONLY_SINK_OR_SOURCE_DIAGONALS_FOR_EAGLE_BOUNDARY_EDGE;
				//}
			else if (li == el.last() && !cross_last_node)
				ect = ONLY_SINK_OR_SOURCE_DIAGONALS;
			else
				ect = WALK_ALONG_AND_SINK_OR_SOURCE_DIAGONALS;
			//------------------------------------------------------------------------------------
			// Completation of subnet of the gdtedge and his connections with the subnet of pred gdtnode
			//------------------------------------------------------------------------------------
			DD_complete_map_subnet_of_edge (DD, e, h, tnt, ect);
			DD_connect_map_subnets_of_node_and_edge (DD, pred, e, h, tnt);
			succ = opposite(pred, e);
			
			//------------------------------------------------------------------------------------
			// Completation of subnet of the succ gdtnode and his connections with the subnet of gdtedge
			// Even the gdtnode that ends a terminal line must be crossed
			//------------------------------------------------------------------------------------
			if (!(li == el.last() && !cross_last_node))
				{
				DD_complete_map_subnet_of_dime_node (DD, succ, h, tnt);
				DD_connect_map_subnets_of_node_and_edge (DD, succ, e, h, tnt);
				pred = succ;
				}
			}
		}
	}
	
	
	
	
	
	// DD_dynamic_new_edge subroutine
	//            Under the condition imposed by particularity of terminal gdtnode
	//            says if a certain gdtnode belongs to a certain terminal line
	//            is a terminator of the terminal line along a certain heading
	//            Added for expansion to >4plan problems
	//
	bool
	dime_orth_plan_undi_graph::
DD_node_ends_a_terminal_line
	(
	gdtnode n,
	heading_type h
	)
	{
	
	// One of the problems of the GDT library is that it doesn't support crosses between edges with thickness > 1;
	// in the following this problem'll be indicated with "no thick cross" problem
	
	//----------------------------------------------------------------
	// A gdtnode is a terminator of the terminal line if:
	// o	is a real gdtnode                                          or
	// o	is a cross gdtnode                     ("no thick cross")  or
	// o	is a trasversal_bend_node           ("no thick cross")  or
	// o	is a trasversal_real_thick_node     ("no thick cross")  or
	// o	is a after_eagle_boundary_bend_node ("opposite eagle")
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	// A gdtnode is a real gdtnode when there is not any marker among these:
	//	RM3_BEND_NODE
	//	RM3_ADDED_BY_RECTANGULARIZATION
	//	RM3_REPLACES_A_BEND
	//	RM3_CROSS_ON_REAL_EDGE
	//----------------------------------------------------------------
	bool n_is_real_or_cross = node_is_real(n) || is_marked(n, RM3_CROSS_ON_REAL_EDGE);
	if (n_is_real_or_cross)
		return true;
	
	//----------------------------------------------------------------------
	// A gdtnode is a after_eagle_boundary_bend_node when
	//	for the 4 dime edges (edges with thickness) around gdtnode
	//	the maximum thickness is in the current heading:
	//	it means an opposite eagle is found
	// A gdtnode is a trasversal bend gdtnode when
	//	for the 4 dime edges (edges with thickness) around gdtnode
	//	the maximum thickness is not in the slope of the current heading
	//----------------------------------------------------------------------
	if (is_marked(n, RM3_BEND_NODE))
		{
		heading_type h_max = DD_heading_of_edge_with_max_thickness_around_bend_node(n);
		if (h_max == h)		// opposite eagle
			return true;
		slope_type s1 = slope_along_heading(h_max);
		slope_type s2 = slope_along_heading(h);
		if (s1 != s2)		// trasversal bend gdtnode
			return true;
		}
	
	//-----------------------------------------------------------------------------------------
	// A gdtnode is a trasversal_real_thick_node when
	//	trasversally respect to the current heading there is a real gdtedge with thickness > 1
	//-----------------------------------------------------------------------------------------
	if (is_marked(n, RM3_ADDED_BY_RECTANGULARIZATION))
		{
		gdtedge e090 = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _090));
		if (e090 && edge_is_real(e090))
			{
			int thk090 = thickness_of_edge(e090);
			gdtedge e270 = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _270));
			if (e270 && edge_is_real(e270))
				{
				int thk270 = thickness_of_edge(e270);
				if (thk090 != thk270) cerr << "ERROR: Possibile colpa del planarizzatore:\n\tspessori differenti di semiarchi opposti in un nodo fittizio cross!!!" << flush;
				if (thk090 > 1 || thk270 > 1)
					return true;
				}
			}
		}
	
	return false;
	}
	
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.4.1.1 - Find the sequence of edges along terminal line with heading h
	//                that satisfies the particularity of terminal gdtnode.
	//                Added for expansion to >4plan problems
	//
	gdt::gdtlist<gdtedge>
	dime_orth_plan_undi_graph::
DD_edge_sequence_along_terminal_line
	(
	DD_struct& DD,
	gdtnode tn,
	heading_type h,
	gdtedge& eb,
	bool& cross_last_node
	)
	{
	//--------------------------------------------------------------
	// A terminal line always starts from the terminal gdtnode.
	// It may contain 0 to n edges.
	// The sequence of gdtedge belong to the terminal line ends if:
	// o	the gdtedge e not exists (not real nor dummy)
	// o	the gdtedge e is an eagle boundary gdtedge
	// o	the gdtedge e come after a sequence of dummy edges (1 to n)
	// o	the succ gdtnode ends a terminal line
	//--------------------------------------------------------------
	gdt::gdtlist<gdtedge> l_e;
	gdtnode pred = tn;
	gdtnode succ;
	gdtedge eagle_boundary = NULL_EDGE;
	bool eagle_boundary_not_searched = true;
	bool dummy_yet_encountered       = false;
	cross_last_node = false;
	
	gdtedge e = find_edge_leaving_node_with_heading(pred, h);
	while (e)
		{
		//---------------------------------------------
		// The gdtedge e is put in the gdtedge list
		// Even the eagle boundary gdtedge must be in list
		//---------------------------------------------
//		l_e.append(e);
		
		//--------------------------------------------------------
		// If in the terminal line there is at least a dummy gdtedge,
		// there is no an eagle boundary gdtedge
		//--------------------------------------------------------
		if (edge_is_dummy(e))
			{
			eagle_boundary_not_searched = false;
			dummy_yet_encountered = true;
			}
		
		//-----------------------------------------------------------------------------------------------------------------
		// If the current gdtedge is real, with thickness = 1 and the two terminal nodes are on the same current terminal line
		// then the eagle boundary gdtedge may exists;
		// if this gdtedge is not yet detected nor yet searched, now is the right moment to do it
		//-----------------------------------------------------------------------------------------------------------------
		int tck = thickness_of_edge(e);
		if (
		       eagle_boundary_not_searched
		    && !eagle_boundary
		    && tck == 1
		    && edge_is_real(e)
		    && DD_from_this_terminal_the_other_one_is_along_heading(tn, DD, h)
		   )
			{
			eagle_boundary_not_searched = false;
			eagle_boundary = DD_detect_the_eagle_boundary_edge(DD, pred, e);
			eb = eagle_boundary;
			//if (eb) cout << "\n\tedge_sequence::eagle boundary gdtedge is " << id(eb) << flush;
			}
		
		//-----------------------------------------------
		// If the current gdtedge is the eagle boundary gdtedge
		// go on with the sequence have sense no more
		//-----------------------------------------------
		if (e == eagle_boundary)
			{
			l_e.append(e);
			break;
			}
		
		//--------------------------------------------------------------------------------------
		// The sequence of dummy edges must be in the gdtedge sequence,
		// but go on with the sequence beyond have sense no more.
		// Therefore the first no dummy edges after the sequence of dummy is the break criteria;
		// the case of no gdtedge is yet a break criteria;
		// the first no dummy is real and the 
		// If the current gdtedge is a dummy gdtedge
		// go on with the sequence have sense no more
		//--------------------------------------------------------------------------------------
		if (edge_is_real(e) && dummy_yet_encountered)
			break;
		
		//---------------------
		// Verify the succ gdtnode
		//---------------------
		succ = opposite(pred, e);
		if (DD_node_ends_a_terminal_line(succ, h))
			{
			l_e.append(e);
			break;
			}
		
		//-------------------------------------------
		// Takes the succ gdtedge along the same heading
		//-------------------------------------------
		l_e.append(e);
		pred = succ;
		e = find_edge_leaving_node_with_heading(pred, h);
		}
	
	//--------------------------------------------------------------------------------------------------
	// The only case in which the gdtnode after the last gdtedge of the sequence (last gdtnode) can be crossed is
	// when the sequence ends because the gdtedge not exists;
	// only in this case the corresponding flag is setted to true.
	//--------------------------------------------------------------------------------------------------
	if (!e) cross_last_node = true;
	
	//gdtedge aux;
	// cout << "\n\tList of edges on the terminal line:" << flush;
	//forall(aux, l_e) {cout << "\n\t" << id(aux) << flush;}
	//cout << "\n\tEnd of list" << flush;
	
	return l_e;
	}
	
	
	gdtedge
	dime_orth_plan_undi_graph::
DD_detect_the_eagle_boundary_edge
	(
	DD_struct& DD,
	gdtnode n,
	gdtedge e
	)
	{
	//-------------------------------------------------------------------------------------------------------
	// This method finds the eagle boundary gdtedge;
	// this gdtedge exist only under particular condition (see the method DD_edge_sequence_along_terminal_line);
	// for this method these conditions are preconditions;
	// the eagle boundary must be choose among the consecutive real edges with thickness = 1
	// with the first gdtedge e and with the heading from n to e.
	//-------------------------------------------------------------------------------------------------------
	int tck = thickness_of_edge(e);
	if (tck != 1) gdt_error("gdtedge has not thickness = 1");
	heading_type h = position_of_edge_with_respect_to_node(e, n);
	gdtnode succ_n = opposite(n, e);
	
	if (DD.is_a_terminal_node(succ_n)) return e;	// by walter didimo on frank's request
	
	gdtedge succ_e = find_edge_leaving_node_with_heading(succ_n, h);
	int i = 1;
	while (succ_e && tck == 1 && edge_is_real(succ_e))
		{
		tck = thickness_of_edge(succ_e);
		if (DD_node_ends_a_terminal_line(succ_n, h))
			{
			if (DD.is_a_terminal_node(succ_n))
				tck = 100;
			else if (is_marked(succ_n, RM3_BEND_NODE) && tck > 1)
				tck = 100;
			else	// cases of end of terminal line without a eagle boundary gdtedge
				tck = 0;
			break;
			}
		succ_n = opposite(succ_n, succ_e);
		succ_e = find_edge_leaving_node_with_heading(succ_n, h);
		++i;
		}
	//cout << "\n\tdetect_eagle_boundary::number of consecutive real gdtedge with thickness = 1 is " << i << flush;
	
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// If the stop condition of the cicle is:
	//    the not existence of succ_e,     it means the terminal line do not connect the two terminal gdtnode with a yet existent real path and therefore not an eagle boundary gdtedge exists;
	//    the gdtedge is dummy,               it means the terminal line do not connect the two terminal gdtnode with a yet existent real path and therefore not an eagle boundary gdtedge exists;
	//    the thickness is greater than 1, it means there is an opposite eagle and therefore the eagle boundary exists.
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	if (tck > 1)
		{
		//----------------------------------------------------------------------------------------------
		// The choose of the eagle boundary gdtedge in this condition is arbitrary;
		// because the eagle boundary is a borderline for the two opposite eagles
		// and only the diagonals in the source or sink heading are allowed for each eagle,
		// then it will chosed the central gdtedge of the sequence.
		// It is hopeful the central gdtedge is the same both for the sourc and for the sink terminal case;
		// to do it possible, the quantization of the central must be different for the two cases.
		//----------------------------------------------------------------------------------------------
		succ_n = n;
		succ_e = e;
		int shifts;
		if (position_of_node_with_respect_to_node(DD.sink_terminal(), DD.source_terminal()) == h)
			shifts = (i-1)/2;
		else
			shifts = i/2;
		//cout << "\n\t\ti = " << i << "\tshifts = " << shifts << flush;
		int j;
		for (j = 0; j < shifts; ++j)
			{
			//cout << "\n\t\tshift numero " << j+1 << flush;
			succ_n = opposite(succ_n, succ_e);
			succ_e = find_edge_leaving_node_with_heading(succ_n, h);
			}
		//cout << "\n\tdetect_eagle_boundary::eagle boundary is gdtedge " << id(succ_e) << flush;
		return succ_e;
		}
	else
		return NULL_EDGE;
	}
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.4.1.2 - completes the map subnet of the gdtedge
	//                considering the gdtedge as part of a terminal line
	//                walking with a certain heading and
	//                belongs to a terminal gdtnode of a certain type.
	//                Added for expansion to >4plan problems
	//
	void
	dime_orth_plan_undi_graph::
DD_complete_map_subnet_of_edge
	(
	DD_struct& DD,
	gdtedge e,
	heading_type h,
	DD_terminal_node_type tnt,
	DD_edge_completament_type ect
	)
	{
	/*
	//    Current heading
	//	---------->
	//
	//       Source terminal case                          Sink terminal case
	//
	//                         -o 2                                           o 2
	//                         /|                                            /
	//                        /                                             /
	//                     1 /       3                                  1 |/       3
	//       ---------------o------>o---------------       ---------------o<------o---------------
	//                       \                                            |\
	//                        \                                             \
	//                         \|                                            \
	//                         -o 4                                           o 4
	//
	//
	// Both in the case of real and dummy gdtedge there is a common subnet:
	// in every case, for the precedent construction
	// every gdtedge have almost the two trasversal nodes 2 and 4;
	// therefore no verifies on this are performed
	//
	//    The subnet in the picture must be present in every case
	//    This subnet must be connected with the most near map-gdtnode of the pred dime gdtnode
	*/
	
	//cout << "\n\t\tCompletion of the subnet of gdtedge e" << flush;
	
	//---------------------------------
	// Costs of the different situation
	//---------------------------------
	int walk_along_dummy_cost        = DD.length_unit_cost*2*length_of_edge(e),
	    walk_along_real_cost         = DD.length_unit_cost*2*length_of_edge(e),
	    dummy_split_cost             = DD.length_unit_cost*2,
	    real_split_cost              = DD.length_unit_cost*2*thickness_of_edge(e),
	    turn_on_dummy_edge_cost      = DD.bend_cost*2 + walk_along_dummy_cost/2 + dummy_split_cost,
	    turn_on_long_dummy_edge_cost = DD.bend_cost*2 + walk_along_dummy_cost/2,
	    turn_on_real_edge_cost       = DD.bend_cost*2 + walk_along_real_cost/2  + real_split_cost + 1,
	    turn_on_long_real_edge_cost  = DD.bend_cost*2 + walk_along_real_cost/2  + 1,
	    eagle_boundary_cost          = DD.bend_cost*2 + turn_on_real_edge_cost  + 3*DD.length_unit_cost + 1;
	
	//------------------------------------------------------------------------
	// The corresponding map-nodes are named as shown in the precedent picture
	//------------------------------------------------------------------------
	//cout << "\n\t\t\trename" << flush;
	map_node_set mns = DD.map_nodes_of_edge[e];
	gdtnode mn1, mn2, mn3, mn4;
	DD_get_map_node_set
		(
		mns,
		mn3, mn4, mn1, mn2,
		h
		);
	
	//--------------------------------------------------------------------------
	// Completation of the map gdtnode 1:
	//   if gdtnode 1 not exists, the connections with nodes 2 and 4 not exist too
	//      and therefore the monodirectional edges must be created;
	//   if gdtnode 1 yet exists, then the connections with nodes 2 and 4 yet exist
	//      except for the case of same terminal-line terminals;
	//      even in this case these edges must be created.
	// The edges from map gdtnode 1 to map nodes 2 and 4 must exist in every case!
	//   These edges must be marked RM3_DD_WALK_AND_TURN_ALONG_REAL_EDGE or
	//   RM3_DD_TURN_AND_WALK_ALONG_REAL_EDGE and must have the bend_cost
	//--------------------------------------------------------------------------
	//cout << "\n\t\t\tcompletation of map gdtnode 1" << flush;
	gdtedge me12 = NULL_EDGE,
	     me14 = NULL_EDGE;
	
	
	//----------------------------
	// Case of not existent gdtnode 1
	//----------------------------
	if (!mn1)
		{
		//-------------------
		// Creation of gdtnode 1
		//-------------------
		mn1 = DD.map.new_node();
		
		if (ect == ONLY_SINK_OR_SOURCE_DIAGONALS || ect == ONLY_SINK_OR_SOURCE_DIAGONALS_FOR_EAGLE_BOUNDARY_EDGE || ect == WALK_ALONG_AND_SINK_OR_SOURCE_DIAGONALS)
			{
			//--------------------------------------------------
			// Creation of new monodirectional edges 1-2 and 1-4
			//--------------------------------------------------
			if (tnt == source_terminal)
				{
				//-----------------------------
				// Case of terminal source gdtnode
				//-----------------------------
				me12 = DD.map.new_edge(mn1,mn2);
				me14 = DD.map.new_edge(mn1,mn4);
				DD.bends_of_map_edge[me12]="L";
				DD.bends_of_map_edge[me14]="R";
				if (edge_is_real(e))
					{
					if (ect == ONLY_SINK_OR_SOURCE_DIAGONALS_FOR_EAGLE_BOUNDARY_EDGE)
						{
						//cout << "\n\t\tDiagonal source marked WAT on EAGLE BOUNDARY of dime-gdtedge " << id(e) << flush;
						DD.map.mark(me12,RM3_DD_WALK_AND_TURN_LEFT_ALONG_EAGLE_BOUNDARY_EDGE);
						DD.map.mark(me14,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_EAGLE_BOUNDARY_EDGE);
						DD.map_edge_cost[me12] = eagle_boundary_cost;
						DD.map_edge_cost[me14] = eagle_boundary_cost;
						}
					else
						{
						DD.map.mark(me12,RM3_DD_WALK_AND_TURN_LEFT_ALONG_REAL_EDGE);
						DD.map.mark(me14,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_REAL_EDGE);
						if (length_of_edge(e)>1)
							{
							DD.map_edge_cost[me12] = turn_on_long_real_edge_cost;
							DD.map_edge_cost[me14] = turn_on_long_real_edge_cost;
							}
						else
							{
							DD.map_edge_cost[me12] = turn_on_real_edge_cost;
							DD.map_edge_cost[me14] = turn_on_real_edge_cost;
							}
						}
					}
				else
					{
					DD.map.mark(me12,RM3_DD_WALK_AND_TURN_LEFT_ALONG_DUMMY_EDGE);
					DD.map.mark(me14,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_DUMMY_EDGE);
					if (length_of_edge(e)>1)
						{
						DD.map_edge_cost[me12] = turn_on_long_dummy_edge_cost;
						DD.map_edge_cost[me14] = turn_on_long_dummy_edge_cost;
						}
					else
						{
						DD.map_edge_cost[me12] = turn_on_dummy_edge_cost;
						DD.map_edge_cost[me14] = turn_on_dummy_edge_cost;
						}
					}
				}
			else
				{
				//---------------------------
				// Case of terminal sink gdtnode
				//---------------------------
				me12 = DD.map.new_edge(mn2,mn1);
				me14 = DD.map.new_edge(mn4,mn1);
				DD.bends_of_map_edge[me12]="R";
				DD.bends_of_map_edge[me14]="L";
				if (edge_is_real(e))
					{
					if (ect == ONLY_SINK_OR_SOURCE_DIAGONALS_FOR_EAGLE_BOUNDARY_EDGE)
						{
						//cout << "\n\t\tDiagonal sink   marked WAT on EAGLE BOUNDARY of dime-gdtedge " << id(e) << flush;
						DD.map.mark(me12,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE);
						DD.map.mark(me14,RM3_DD_TURN_LEFT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE);
						DD.map_edge_cost[me12] = eagle_boundary_cost;
						DD.map_edge_cost[me14] = eagle_boundary_cost;
						}
					else
						{
						DD.map.mark(me12,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_REAL_EDGE);
						DD.map.mark(me14,RM3_DD_TURN_LEFT_AND_WALK_ALONG_REAL_EDGE);
						if (length_of_edge(e)>1)
							{
							DD.map_edge_cost[me12] = turn_on_long_real_edge_cost;
							DD.map_edge_cost[me14] = turn_on_long_real_edge_cost;
							}
						else
							{
							DD.map_edge_cost[me12] = turn_on_real_edge_cost;
							DD.map_edge_cost[me14] = turn_on_real_edge_cost;
							}
						}
					}
				else
					{
					DD.map.mark(me12,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_DUMMY_EDGE);
					DD.map.mark(me14,RM3_DD_TURN_LEFT_AND_WALK_ALONG_DUMMY_EDGE);
					if (length_of_edge(e)>1)
						{
						DD.map_edge_cost[me12] = turn_on_long_dummy_edge_cost;
						DD.map_edge_cost[me14] = turn_on_long_dummy_edge_cost;
						}
					else
						{
						DD.map_edge_cost[me12] = turn_on_dummy_edge_cost;
						DD.map_edge_cost[me14] = turn_on_dummy_edge_cost;
						}
					}
				}
			}
		}
	
	//----------------------------
	// Case of yet existent gdtnode 1
	//----------------------------
	else
		{
		//-----------------------------------------------------------
		// The edges from map gdtnode 1 to map nodes 2 and 4 must exist!
		//  if not present they must be created!
		// The nodes 1, 2 and 4 necessarily yet exist:
		//  the gdtnode 1 for the present if condition;
		//  the nodes 2 and 4 for the precedent construction
		//      (DD_create_map_subnet_for_each_edge)
		//-----------------------------------------------------------
		if (ect == ONLY_SINK_OR_SOURCE_DIAGONALS || ect == ONLY_SINK_OR_SOURCE_DIAGONALS_FOR_EAGLE_BOUNDARY_EDGE || ect == WALK_ALONG_AND_SINK_OR_SOURCE_DIAGONALS)
			{
			if (tnt == source_terminal)
				{
				//-----------------------------
				// Case of terminal source gdtnode
				//-----------------------------
				me12 = DD.map.find_directed_edge(mn1, mn2);
				me14 = DD.map.find_directed_edge(mn1, mn4);
				//------------------------------------------------------- ?!????
				// Case of terminal dime nodes on the same terminal line:
				//   the gdtedge 1-2 and/or 1-4 may not exist;
				//-------------------------------------------------------
				if (!me12)
					{
					//cout << "\nCase of same line for the two terminal nodes" << flush;
					me12 = DD.map.new_edge(mn1,mn2);
					//if (!me12) cout << "\narco verso nodino 2 che doveva esistere non c'e'" << flush;
					DD.bends_of_map_edge[me12]="L";
					if (edge_is_dummy(e))
						{
						//cout << "\nWATLD" << flush;
						DD.map.mark(me12,RM3_DD_WALK_AND_TURN_LEFT_ALONG_DUMMY_EDGE);
						if (length_of_edge(e)>1)
							DD.map_edge_cost[me12] = turn_on_long_dummy_edge_cost;
						else
							DD.map_edge_cost[me12] = turn_on_dummy_edge_cost;
						}
					else
						{
						if (ect == ONLY_SINK_OR_SOURCE_DIAGONALS_FOR_EAGLE_BOUNDARY_EDGE)
							{
							//cout << "\n\t\tDiagonal source marked WAT on EAGLE BOUNDARY of dime-gdtedge " << id(e) << flush;
							DD.map.mark(me12,RM3_DD_WALK_AND_TURN_LEFT_ALONG_EAGLE_BOUNDARY_EDGE);
							DD.map_edge_cost[me12] = eagle_boundary_cost;
							}
						else
							{
							DD.map.mark(me12,RM3_DD_WALK_AND_TURN_LEFT_ALONG_REAL_EDGE);
							if (length_of_edge(e)>1)
								DD.map_edge_cost[me12] = turn_on_long_real_edge_cost;
							else
								DD.map_edge_cost[me12] = turn_on_real_edge_cost;
							}
						}
					}
				if (!me14)
					{
					//cout << "\nCase of same line for the two terminal nodes" << flush;
					me14 = DD.map.new_edge(mn1,mn4);	//?!?????
					//if (!me14) cout << "\narco verso nodino 4 che doveva esistere non c'e'" << flush;
					DD.bends_of_map_edge[me14]="R";
					if (edge_is_dummy(e))
						{
						//cout << "\nWATRD" << flush;
						DD.map.mark(me14,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_DUMMY_EDGE);	//?!???
						if (length_of_edge(e)>1)
							DD.map_edge_cost[me14] = turn_on_long_dummy_edge_cost;
						else
							DD.map_edge_cost[me14] = turn_on_dummy_edge_cost;
						}
					else
						{
						if (ect == ONLY_SINK_OR_SOURCE_DIAGONALS_FOR_EAGLE_BOUNDARY_EDGE)
							{
							DD.map.mark(me14,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_EAGLE_BOUNDARY_EDGE);	//?!???
							DD.map_edge_cost[me14] = eagle_boundary_cost;
							}
						else
							{
							DD.map.mark(me14,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_REAL_EDGE);	//?!???
							if (length_of_edge(e)>1)
								DD.map_edge_cost[me14] = turn_on_long_real_edge_cost;
							else
								DD.map_edge_cost[me14] = turn_on_real_edge_cost;
							}
						}
					}
				}
			else
				{
				//---------------------------
				// Case of terminal sink gdtnode
				//---------------------------
				me12 = DD.map.find_directed_edge(mn2, mn1);
				me14 = DD.map.find_directed_edge(mn4, mn1);
				//------------------------------------------------------- 
				// Case of terminal dime nodes on the same terminal line:
				//   the gdtedge 1-2 and/or 1-4 may not exist;
				//-------------------------------------------------------
				if (!me12)
					{
					//cout << "\nCase of same line for the two terminal nodes" << flush;
					me12 = DD.map.new_edge(mn2,mn1);
					//if (!me12) cout << "\narco verso nodino 2 che doveva esistere non c'e'" << flush;
					DD.bends_of_map_edge[me12]="R";
					if (edge_is_dummy(e))
						{
						//cout << "\nTRAWD" << flush;
						DD.map.mark(me12,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_DUMMY_EDGE);
						if (length_of_edge(e)>1)
							DD.map_edge_cost[me12] = turn_on_long_dummy_edge_cost;
						else
							DD.map_edge_cost[me12] = turn_on_dummy_edge_cost;
						}
					else
						{
						if (ect == ONLY_SINK_OR_SOURCE_DIAGONALS_FOR_EAGLE_BOUNDARY_EDGE)
							{
							//cout << "\n\t\tDiagonal sink   marked WAT on EAGLE BOUNDARY of dime-gdtedge " << id(e) << flush;
							DD.map.mark(me12,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE);
							DD.map_edge_cost[me12] = eagle_boundary_cost;
							}
						else
							{
							DD.map.mark(me12,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_REAL_EDGE);
							if (length_of_edge(e)>1)
								DD.map_edge_cost[me12] = turn_on_long_real_edge_cost;
							else
								DD.map_edge_cost[me12] = turn_on_real_edge_cost;
							}
						}
					}
				if (!me14)
					{
					//cout << "\nCase of same line for the two terminal nodes" << flush;
					me14 = DD.map.new_edge(mn4,mn1);
					//if (!me14) cout << "\narco verso nodino 4 che doveva esistere non c'e'" << flush;
					DD.bends_of_map_edge[me14]="L";
					if (edge_is_dummy(e))
						{
						//cout << "\nTLAWD" << flush;
						DD.map.mark(me14,RM3_DD_TURN_LEFT_AND_WALK_ALONG_DUMMY_EDGE);
						if (length_of_edge(e)>1)
							DD.map_edge_cost[me14] = turn_on_long_dummy_edge_cost;
						else
							DD.map_edge_cost[me14] = turn_on_dummy_edge_cost;
						}
					else
						{
						if (ect == ONLY_SINK_OR_SOURCE_DIAGONALS_FOR_EAGLE_BOUNDARY_EDGE)
							{
							DD.map.mark(me14,RM3_DD_TURN_LEFT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE);
							DD.map_edge_cost[me14] = eagle_boundary_cost;
							}
						else
							{
							DD.map.mark(me14,RM3_DD_TURN_LEFT_AND_WALK_ALONG_REAL_EDGE);
							if (length_of_edge(e)>1)
								DD.map_edge_cost[me14] = turn_on_long_real_edge_cost;
							else
								DD.map_edge_cost[me14] = turn_on_real_edge_cost;
							}
						}
					}
				}
			}
		}
	
	//---------------------------------------------------------------
	// Assignment of corresponding dime gdtedge to map edges 1-2 and 1-4
	//---------------------------------------------------------------
	if (me12) DD.edge_of_map_edge[me12] = e;
	if (me14) DD.edge_of_map_edge[me14] = e;
	
	
	//------------------------------------------------------------
	// Creation, if not exists, of the gdtnode 3 of the map of gdtedge e
	//------------------------------------------------------------
	////cout << "\n\t\t\t\tcompletation of map gdtnode 3" << flush;
	if (!mn3) mn3 = DD.map.new_node();
	
	
	//-------------------------------------------------------
	// Updating of the map-gdtnode set with the completed values
	//-------------------------------------------------------
	DD_init_map_node_set
		(
		DD.map_nodes_of_edge[e],
		mn3, mn4, mn1, mn2,
		h
		);
	
	if (ect == WALK_ALONG_AND_SINK_OR_SOURCE_DIAGONALS)
		{
		//-------------------------------------------------------------------
		// Creation, if not exists, of the connection among gdtnode 1 and gdtnode 3
		//-------------------------------------------------------------------
		//cout << "\n\t\t\t\tcompletation of connection between map gdtnode 1 and map gdtnode 3" << flush;
		gdtedge me13 = NULL_EDGE;
		if (tnt == source_terminal)
			{
			//-----------------------------
			// Case of terminal source gdtnode
			//-----------------------------
			me13 = DD.map.find_directed_edge(mn1, mn3);
			if (!me13)
				//{
				//cout << "\n\t\t\t\tsource case" << flush;
				me13 = DD.map.new_edge(mn1, mn3);
				//}
			}
		else
			{
			//---------------------------
			// Case of terminal sink gdtnode
			//---------------------------
			me13 = DD.map.find_directed_edge(mn3, mn1);
			if (!me13)
				//{
				//cout << "\n\t\t\t\tsink case" << flush;
				me13 = DD.map.new_edge(mn3, mn1);
				//}
			}
		
		//-------------------------------------------
		// Assegnation of cost and marker to gdtedge 1-3
		//-------------------------------------------
		DD.edge_of_map_edge[me13] = e;
		if (edge_is_real(e))
			{
			DD.map.mark(me13,RM3_DD_WALK_ALONG_REAL_EDGE);
			DD.map_edge_cost[me13] = walk_along_real_cost;
			}
		else
			{
			DD.map.mark(me13,RM3_DD_WALK_ALONG_DUMMY_EDGE);
			DD.map_edge_cost[me13] = walk_along_dummy_cost;
			}
		}
	}
	
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.4.1.3 - completes the map subnet of the gdtnode
	//                considering the gdtnode as part of a terminal line
	//                walking with a certain heading and
	//                belongs to a terminal gdtnode of a certain type.
	//                Added for expansion to >4plan problems
	//
	void
	dime_orth_plan_undi_graph::
DD_complete_map_subnet_of_dime_node
	(
	DD_struct& DD,
	gdtnode n,
	heading_type h,
	DD_terminal_node_type tnt
	)
	{
	/*
	//    Current heading
	//	---------->
	//
	//       Source terminal case                          Sink terminal case
	//
	//                         -o 2                                           o 2
	//                         /|                                            /
	//                        /                                             /
	//                     1 /       3                                  1 |/       3
	//       ---------------o------>o---------------       ---------------o<------o---------------
	//                       \                                            |\
	//                        \                                             \
	//                         \|                                            \
	//                         -o 4                                           o 4
	//
	//
	// There is no warranty of existence for the four map-gdtnode
	// from the precedent construction
	//
	//    It must have at least the gdtnode 1, the gdtnode 3 and the gdtedge among 1 and 3.
	//    For the nodes 2 and 4:
	//       the precedent construction creates gdtnode 2 (or 4) only if the gdtedge on that heading is not real;
	//       therefore if gdtnode 2 (or 4) not exists, it mustn't be created.
	//       The case of gdtnode 2 (or 4) exists, it is no warranty of existance of connection with gdtnode 1
	//       (for example: dime gdtedge corresponding to map gdtnode 1 and 3 are real, and the others are dummy;
	//       the gdtnode 2 and 4 are created, but not the same for the nodes 1 and 3,
	//       therefore no connection are possible between 1 and another map-gdtnode, 1 not exists);
	//       in every case the connection must be verified and, if not exists, created
	*/
	
	//cout << "\n\t\tcompletation of subnet of succ gdtnode:" << flush;
	
	//---------------------------------
	// Costs of the different situation
	//---------------------------------
	int turn_on_dummy_node_cost        = DD.bend_cost*2,
	    across_dummy_node_cost         = 1,
	    across_dummy_node_between_real = DD.cross_cost*2 + 1;
	
	//------------------------------------------------------------------------
	// The corresponding map-nodes are named as shown in the precedent picture
	//------------------------------------------------------------------------
	map_node_set mns = DD.map_nodes_of_node[n];
	gdtnode mn1, mn2, mn3, mn4;
	//cout << "\n\t\t\tDD_get_map_node_set???" << flush;
	DD_get_map_node_set
		(
		mns,
		mn3, mn4, mn1, mn2,
		h
		);
	
	//-----------------------------------------------------------------------
	// Creation, if not exists, of the part of subnet that must to be present
	//-----------------------------------------------------------------------
	//cout << "\n\t\t\t\tcompletation of necessary part" << flush;
	
		//---------------------------------------
		// Creation, if not exists, of the gdtnode 1
		//---------------------------------------
		if (!mn1) mn1 = DD.map.new_node();
	
		//---------------------------------------
		// Creation, if not exists, of the gdtnode 3
		//---------------------------------------
		if (!mn3) mn3 = DD.map.new_node();
	
		//-------------------------------------------------------------------
		// Creation, if not exists, of the connection among gdtnode 1 and gdtnode 3
		//-------------------------------------------------------------------
		gdtedge me13;
		heading_type h_aux = h;
		if (tnt == source_terminal)
			{
			//-----------------------------
			// Case of terminal source gdtnode
			//-----------------------------
			me13 = DD.map.find_directed_edge(mn1, mn3);
			if (!me13)
				{
				me13 = DD.map.new_edge(mn1, mn3);
				DD.node_of_map_edge[me13] = n;
				DD.map_edge_cost   [me13] = across_dummy_node_cost;
				}
			}
		else
			{
			//---------------------------
			// Case of terminal sink gdtnode
			//---------------------------
			me13 = DD.map.find_directed_edge(mn3, mn1);
			if (!me13)
				{
				me13 = DD.map.new_edge(mn3, mn1);
				DD.node_of_map_edge[me13] = n;
				DD.map_edge_cost   [me13] = across_dummy_node_cost;
				}
			h_aux = heading_after_rotation(h_aux, _180);
			}
		
		//-----------------------
		// The gdtedge is now marked
		//-----------------------
		if (is_marked(n, RM3_REPLACES_A_BEND))
			{
			//cout << "\n\t\twalking from gdtnode " << id(n) << " with heading = " << h_aux << flush;
			gdtedge aux = find_edge_leaving_node_with_heading(n, h_aux);
			if (aux && edge_is_real(aux))
				{
				//cout << "\n\t\tFrom gdtnode " << id(n) << " with rotated heading = " << heading_after_rotation(h_aux, _090) << flush;
				aux = find_edge_leaving_node_with_heading(n, heading_after_rotation(h_aux, _090));
				if (aux && edge_is_real(aux))
					{
					//cout << "\n\t\t\tFRONT RIGHT" << flush;
					DD.map.mark(me13,RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_FRONT_AND_RIGHT_REAL_EDGES);
					}
				else
					{
					//cout << "\n\t\t\tFRONT LEFT" << flush;
					DD.map.mark(me13,RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_FRONT_AND_LEFT_REAL_EDGES);
					}
				}
			else
				{
				//cout << "\n\t\tFrom gdtnode " << id(n) << " with rotated heading = " << heading_after_rotation(h_aux, _090) << flush;
				aux = find_edge_leaving_node_with_heading(n, heading_after_rotation(h_aux, _090));
				if (aux && edge_is_real(aux))
					{
					//cout << "\n\t\t\tBACK RIGHT" << flush;
					DD.map.mark(me13,RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_BACK_AND_RIGHT_REAL_EDGES);
					}
				else
					{
					//cout << "\n\t\t\tBACK LEFT" << flush;
					DD.map.mark(me13,RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_BACK_AND_LEFT_REAL_EDGES);
					}
				}
			}
		else
			DD.map.mark(me13,RM3_DD_ACROSS_A_NODE);
		
		//-----------------------------------------------------------------
		// Detection of the case of real trasversal gdtedge with thickness = 1
		//-----------------------------------------------------------------
		if (is_marked(n, RM3_ADDED_BY_RECTANGULARIZATION))
			{
			gdtedge e090 = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _090));
			if (e090 && edge_is_real(e090))
				{
				int thk090 = thickness_of_edge(e090);
				gdtedge e270 = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _270));
				if (e270 && edge_is_real(e270))
					{
					int thk270 = thickness_of_edge(e270);
					if (thk090 == 1 && thk270 == 1)
						{
						DD.map_edge_cost[me13] = across_dummy_node_between_real;
						// The reasons of + 1 are showed in comments in method DD_complete_map_subnet_for_each_crossable_node
						DD.map.mark(me13, RM3_DD_CROSS_ON_DUMMY_NODE_BETWEEN_REAL_EDGES);
						}
					}
				}
			}
	
	//---------------------------------------------------------------------------
	// Creation of the map nodes that the 4pl has no created,
	// but are necessary for the >4pl
	//
	// If the gdtnode is a BEND_NODE and there is no real gdtedge in the side direction
	// then a map gdtnode must exist to allow the path go out from that bend_node
	//---------------------------------------------------------------------------
	if (is_marked(n, RM3_BEND_NODE))
		{
		gdtedge side = NULL_EDGE;
		side = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _090));
		if (!side || edge_is_dummy(side))
			if (!mn4)
				mn4 = DD.map.new_node();
		side = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _270));
		if (!side || edge_is_dummy(side))
			if (!mn2)
				mn2 = DD.map.new_node();
		}
	
	//-------------------------------------------------------
	// Updating of the map-gdtnode set with the completed values
	//-------------------------------------------------------
	DD_init_map_node_set
		(
		DD.map_nodes_of_node[n],
		mn3, mn4, mn1, mn2,
		h
		);
		
	//----------------------------------------------------------------------------------------
	// Creation, if not exists, of the part of subnet that must to be present under conditions
	//----------------------------------------------------------------------------------------
	//cout << "\n\t\t\t\tcompletation of under condition part" << flush;
	
	//---------------------
	// If the gdtnode 2 exists
	//---------------------
	if (mn2)
		{
		//-------------------------------------------------------------------
		// Creation, if not exists, of the connection among gdtnode 1 and gdtnode 2
		// if the gdtnode is not a REPLACES_A_BEND gdtnode.
		//                (no walk on REPLACES_A_BEND problem)
		//-------------------------------------------------------------------
		//cout << "\n\t\t\tcompletation of connession between gdtnode 1 and gdtnode 2" << flush;
		gdtedge me12 = NULL_EDGE;
		if (tnt == source_terminal)
			{
			//-----------------------------
			// Case of terminal source gdtnode
			//-----------------------------
			me12 = DD.map.find_directed_edge(mn1, mn2);
			if (!me12)
				{
				//-----------------------------------					
				// no walk on REPLACES_A_BEND problem
				//-----------------------------------
				if (!is_marked(n, RM3_REPLACES_A_BEND))
					if (is_marked(n, RM3_BEND_NODE))
						{
						//---------------------------------------------------------------------------------
						// no new gdtedge on bend_node if they walk in the opposite sense of the current eagle
						//---------------------------------------------------------------------------------
						heading_type h_max = DD_heading_of_edge_with_max_thickness_around_bend_node(n);
						if (heading_after_rotation(h_max, _180) == h)
							{
							me12 = DD.map.new_edge(mn1, mn2);
							DD.bends_of_map_edge[me12]="L";
							}
						}
					else
						{
						me12 = DD.map.new_edge(mn1, mn2);
						DD.bends_of_map_edge[me12]="L";
						}
				}
			////The bend are renamed
			//else?!?
			//	//DD.bends_of_map_edge[me12]="L";
			
			//-------------------
			// Marker attribution
			//-------------------
			if(!is_marked(n, RM3_REPLACES_A_BEND) && me12)
				{
				//----------------------------------------------------------------
				//			NEW_DUMMY problem
				//----------------------------------------------------------------
				gdtedge from = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _180));
				if (from && edge_is_real(from))
					DD.map.mark(me12,RM3_DD_TURN_LEFT_ON_NODE_FROM_REAL_TO_DUMMY);
				else
					{
					gdtedge to = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _270));
					if (to && edge_is_real(to))
						DD.map.mark(me12,RM3_DD_TURN_LEFT_ON_NODE_FROM_DUMMY_TO_REAL);
					else
						DD.map.mark(me12,RM3_DD_TURN_LEFT_ON_NODE);
					}
				}
			else if (me12)
				gdt_error("There is a gdtedge turning on a replaces a bend gdtnode");
			//	DD.map.mark(me12,RM3_DD_TURN_LEFT_ON_A_REPLACES_A_BEND_NODE); //?!???
			}
		else
			{
			//---------------------------
			// Case of terminal sink gdtnode
			//---------------------------
			me12 = DD.map.find_directed_edge(mn2, mn1);
			if (!me12)
				{
				//-----------------------------------					
				// no walk on REPLACES_A_BEND problem
				//-----------------------------------
				if (!is_marked(n, RM3_REPLACES_A_BEND))
					if (is_marked(n, RM3_BEND_NODE))
						{
						//---------------------------------------------------------------------------------
						// no new gdtedge on bend_node if they walk in the opposite sense of the current eagle
						//---------------------------------------------------------------------------------
						heading_type h_max = DD_heading_of_edge_with_max_thickness_around_bend_node(n);
						if (heading_after_rotation(h_max, _180) == h)
							{
							me12 = DD.map.new_edge(mn2, mn1);
							DD.bends_of_map_edge[me12]="R";
							}
						}
					else
						{
						me12 = DD.map.new_edge(mn2, mn1);
						DD.bends_of_map_edge[me12]="R";
						}
				}
			////The bend are renamed
			//else?!?
			//	//DD.bends_of_map_edge[me12]="R";
			
			//-------------------
			// Marker attribution
			//-------------------
			if(!is_marked(n, RM3_REPLACES_A_BEND) && me12)
				{
				//----------------------------------------------------------------
				//			NEW_DUMMY problem
				//----------------------------------------------------------------
				gdtedge from = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _270));
				if (from && edge_is_real(from))
					DD.map.mark(me12,RM3_DD_TURN_RIGHT_ON_NODE_FROM_REAL_TO_DUMMY);
				else
					{
					gdtedge to = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _180));
					if (to && edge_is_real(to))
						DD.map.mark(me12,RM3_DD_TURN_RIGHT_ON_NODE_FROM_DUMMY_TO_REAL);
					else
						DD.map.mark(me12,RM3_DD_TURN_RIGHT_ON_NODE); //?!???
					}
				}
			else if (me12)
				gdt_error("There is an gdtedge turning on a replaces a bend gdtnode");
			//	DD.map.mark(me12,RM3_DD_TURN_RIGHT_ON_A_REPLACES_A_BEND_NODE); //?!???
			}
		
		//-----------------
		// Cost attribution
		//-----------------
		if (me12)
			{
			DD.node_of_map_edge[me12] = n;
			if (is_marked(n, RM3_BEND_NODE) && !find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _270)))
				DD.map_edge_cost [me12] = turn_on_dummy_node_cost;
			else
				DD.map_edge_cost [me12] = turn_on_dummy_node_cost + 1;
			}
		//if(is_marked(n, RM3_REPLACES_A_BEND))
		//	DD.map.mark(me12,RM3_DD_TURN_ON_A_REPLACES_A_BEND_NODE); //?!???
		//else
		//	DD.map.mark(me12,RM3_DD_TURN_ON_NODE); //?!???
		}
	
	//---------------------
	// If the gdtnode 4 exists
	//---------------------
	if (mn4)
		{
		//-------------------------------------------------------------------
		// Creation, if not exists, of the connection among gdtnode 1 and gdtnode 4
		//-------------------------------------------------------------------
		//cout << "\n\t\t\tcompletation of connession between gdtnode 1 and gdtnode 4" << flush;
		gdtedge me14;
		if (tnt == source_terminal)
			{
			//-----------------------------
			// Case of terminal source gdtnode
			//-----------------------------
			me14 = DD.map.find_directed_edge(mn1, mn4);
			if (!me14)
				{
				//-----------------------------------					
				// no walk on REPLACES_A_BEND problem
				//-----------------------------------
				if (!is_marked(n, RM3_REPLACES_A_BEND))
					if (is_marked(n, RM3_BEND_NODE))
						{
						//---------------------------------------------------------------------------------
						// no new gdtedge on bend_node if they walk in the opposite sense of the current eagle
						//---------------------------------------------------------------------------------
						heading_type h_max = DD_heading_of_edge_with_max_thickness_around_bend_node(n);
						if (heading_after_rotation(h_max, _180) == h)
							{
							me14 = DD.map.new_edge(mn1, mn4);
							DD.bends_of_map_edge[me14]="R";
							}
						}
					else
						{
						me14 = DD.map.new_edge(mn1, mn4);
						DD.bends_of_map_edge[me14]="R";
						}
				}
			////The bend are renamed
			//else?!?
			//	//DD.bends_of_map_edge[me14]="R";
			
			//-------------------
			// Marker attribution
			//-------------------
			if(!is_marked(n, RM3_REPLACES_A_BEND) && me14)
				{
				//----------------------------------------------------------------
				//			NEW_DUMMY problem
				//----------------------------------------------------------------
				gdtedge from = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _180));
				if (from && edge_is_real(from))
					DD.map.mark(me14,RM3_DD_TURN_RIGHT_ON_NODE_FROM_REAL_TO_DUMMY);
				else
					{
					gdtedge to = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _090));
					if (to && edge_is_real(to))
						DD.map.mark(me14,RM3_DD_TURN_RIGHT_ON_NODE_FROM_DUMMY_TO_REAL);
					else
						DD.map.mark(me14,RM3_DD_TURN_RIGHT_ON_NODE); //?!???
					}
				}
			else if (me14)
				gdt_error("There is an gdtedge turning on a replaces a bend gdtnode");
			//	DD.map.mark(me14,RM3_DD_TURN_RIGHT_ON_A_REPLACES_A_BEND_NODE); //?!???
			}
		else
			{
			//---------------------------
			// Case of terminal sink gdtnode
			//---------------------------
			me14 = DD.map.find_directed_edge(mn4, mn1);
			if (!me14)
				{
				//-----------------------------------					
				// no walk on REPLACES_A_BEND problem
				//-----------------------------------
				if (!is_marked(n, RM3_REPLACES_A_BEND))
					if (is_marked(n, RM3_BEND_NODE))
						{
						//---------------------------------------------------------------------------------
						// no new gdtedge on bend_node if they walk in the opposite sense of the current eagle
						//---------------------------------------------------------------------------------
						heading_type h_max = DD_heading_of_edge_with_max_thickness_around_bend_node(n);
						if (heading_after_rotation(h_max, _180) == h)
							{
							me14 = DD.map.new_edge(mn4, mn1);
							DD.bends_of_map_edge[me14]="L";
							}
						}
					else
						{
						me14 = DD.map.new_edge(mn4, mn1);
						DD.bends_of_map_edge[me14]="L";
						}
				}
			////The bend are renamed
			//else?!?
			//	//DD.bends_of_map_edge[me14]="L";
			
			//-------------------
			// Marker attribution
			//-------------------
			if(!is_marked(n, RM3_REPLACES_A_BEND) && me14)
				{
				//----------------------------------------------------------------
				//			NEW_DUMMY problem
				//----------------------------------------------------------------
				gdtedge from = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _090));
				if (from && edge_is_real(from))
					DD.map.mark(me14,RM3_DD_TURN_LEFT_ON_NODE_FROM_REAL_TO_DUMMY);
				else
					{
					gdtedge to = find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _180));
					if (to && edge_is_real(to))
						DD.map.mark(me14,RM3_DD_TURN_LEFT_ON_NODE_FROM_DUMMY_TO_REAL);
					else
						DD.map.mark(me14,RM3_DD_TURN_LEFT_ON_NODE);
					}
				}
			else if (me14)
				gdt_error("There is an gdtedge turning on a replaces a bend gdtnode");
			//	DD.map.mark(me14,RM3_DD_TURN_LEFT_ON_A_REPLACES_A_BEND_NODE); //?!???
			}
		
		//-----------------
		// Cost attribution
		//-----------------
		if (me14)
			{
			DD.node_of_map_edge[me14] = n;
			if (is_marked(n, RM3_BEND_NODE) && !find_edge_leaving_node_with_heading(n, heading_after_rotation(h, _090)))
				DD.map_edge_cost [me14] = turn_on_dummy_node_cost;
			else
				DD.map_edge_cost [me14] = turn_on_dummy_node_cost + 1;
			}
		//if(is_marked(n, RM3_REPLACES_A_BEND))
		//	DD.map.mark(me14,RM3_DD_TURN_ON_A_REPLACES_A_BEND_NODE); //?!???
		//else
		//	DD.map.mark(me14,RM3_DD_TURN_ON_NODE); //?!???
		}
	}
	
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 3.4.1.4 - completes the connections between map subnets of a gdtnode and an gdtedge
	//                considering the gdtnode and the gdtedge as part of a terminal line
	//                walking with a certain heading and
	//                belongs to a terminal gdtnode of a certain type.
	//                Added for expansion to >4plan problems
	//
	void
	dime_orth_plan_undi_graph::
DD_connect_map_subnets_of_node_and_edge
	(
	DD_struct& DD,
	gdtnode n,
	gdtedge e,
	heading_type h,
	DD_terminal_node_type tnt
	)
	{
	/*
	//    Current heading
	//	---------->
	//
	//       Source terminal case                          Sink terminal case
	//
	//                 -o 2            -o 2                           o 2             o 2         
	//                 /|              /|                            /               /
	//                /               /                             /               /             
	//             1 /       3     1 /       3                   1|/       3     1|/       3      
	//       -------o------>o------>o------>o-------       -------o<------o<------o<------o-------
	//               \               \                            |\              |\              
	//                \               \                             \               \             
	//                 \|              \|                            \               \
	//                 -o 4            -o 4                           o 4             o 4
	//
	//
	// The gdtedge to be created is the one from the first map_node_set to the second
	// walking along the current heading
	*/
	
	//cout << "\n\t\tRelation among subnets of gdtnode n and gdtedge e" << flush;
	
	gdtedge map_e;
	
	//------------------------------------------------------------------------
	// The corresponding map-nodes are named as shown in the precedent picture
	//------------------------------------------------------------------------
	map_node_set mnse = DD.map_nodes_of_edge[e];	// mnse means Map Node Set of Edge e
	gdtnode mne1, mne2, mne3, mne4;			// mnex means Map Node of Edge e subnet
	DD_get_map_node_set
		(
		mnse,
		mne3, mne4, mne1, mne2,
		h
		);
	
	//------------------------------------------------------------------------
	// The corresponding map-nodes are named as shown in the precedent picture
	//------------------------------------------------------------------------
	map_node_set mnsn = DD.map_nodes_of_node[n];	// mnsn means Map Node Set of Node n
	gdtnode mnn1, mnn2, mnn3, mnn4;			// mnnx means Map Node of Node n subnet
	DD_get_map_node_set
		(
		mnsn,
		mnn3, mnn4, mnn1, mnn2,
		h
		);
	
	//-----------------------------------------------------
	// Case of sequence gdtnode-gdtedge along the current heading
	//-----------------------------------------------------
	//cout << "\n\t\t\tRicerca arco che parte da nodo con una certa direzione" << flush;
	gdtedge aux_e;
	aux_e = find_edge_leaving_node_with_heading(n, h);
	if (aux_e && aux_e == e)
		{
		//----------------------------------------------------
		// Creation, if not exists, of the connection between
		// the subnet of the dime-gdtnode and the one of the gdtedge
		//----------------------------------------------------
		//cout << "\n\t\t\tCaso along" << flush;
		if (tnt == source_terminal)
			{
			//-----------------------------
			// Case of terminal source gdtnode
			//-----------------------------
			//cout << "\n\t\t\tCase source gdtnode" << flush;
			map_e = DD.map.find_directed_edge(mnn3, mne1);
			if (!map_e)
				//{
				//cout << "\n\t\t\tcreating a new gdtedge" << flush;
				map_e = DD.map.new_edge(mnn3, mne1);
				//}
			}
		else
			{
			//---------------------------
			// Case of terminal sink gdtnode
			//---------------------------
			//cout << "Case sink gdtnode" << flush;
			map_e = DD.map.find_directed_edge(mne1, mnn3);
			if (!map_e)
				//{
				//cout << "\n\t\t\tCreating a new gdtedge" << flush;
				map_e = DD.map.new_edge(mne1, mnn3);
				//}
			}
		}
	//-----------------------------------------------------
	// Case of sequence gdtedge-gdtnode along the current heading
	//-----------------------------------------------------
	else
		{
		//----------------------------------------------------
		// Creation, if not exists, of the connection between
		// the subnet of the gdtedge and the one of the dime-gdtnode
		//----------------------------------------------------
		//cout << "\n\t\t\tCase not along" << flush;
		if (tnt == source_terminal)
			{
			//-----------------------------
			// Case of terminal source gdtnode
			//-----------------------------
			map_e = DD.map.find_directed_edge(mne3, mnn1);
			if (!map_e)
				//{
				//cout << "\n\t\t\tCreating a new gdtedge" << flush;
				map_e = DD.map.new_edge(mne3, mnn1);
				//}
			}
		else
			{
			//---------------------------
			// Case of terminal sink gdtnode
			//---------------------------
			map_e = DD.map.find_directed_edge(mnn1, mne3);
			if (!map_e)
				//{
				//cout << "\n\t\t\tCreating a new gdtedge" << flush;
				map_e = DD.map.new_edge(mnn1, mne3);
				//}
			}
		}
	//cout << "\n\t\t\tEnd of cases along/not along" << flush;
	DD.edge_of_map_edge[map_e] = e;
	DD.map_edge_cost   [map_e] = 0;
	//cout << "\n\t\tEnd of relation" << flush;
	}
	
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 4
	//
	void 
	dime_orth_plan_undi_graph::
DD_find_shortest_path_through_the_map
	(DD_struct& DD)
	{
	//cout << "\nStarting the computation of a minimum cost path" << flush;
	gdtnode mn;
	gdtedge me;
	gdtedge me_prev;
	
	bool bounce_detected;
	
	// Apply the min-cost-flow algorithm 
	
	do
		{
		gdt::gdtedge_array<int> length;
		gdt::gdtnode_array<int> distance;
		gdt::gdtnode_array<gdtedge> pred_edge;
		
		length.init    (DD.map.nodes_and_edges());
		distance.init  (DD.map.nodes_and_edges());
		pred_edge.init (DD.map.nodes_and_edges());
		
		forall_edges(me, DD.map) length[me] = DD.map_edge_cost[me];
		DD.map.find_shortest_path (DD.mnt1, length, distance, pred_edge);
		
		// Collect map-edges along the shortest path
		//
		DD.map_edge_path.clear();	
		mn = DD.mnt2;
		me = pred_edge[mn];
		
		// Load map_edge_path
		// by reversely scanning pred_edge[]
		
		while (me)
			{
			//cout << "\n\tPushing "<< DD.map.id(me) << " into map_edge_path" << flush;
			DD.map_edge_path.push(me);
			mn = DD.map.source(me);
			me_prev = me;
			me = pred_edge[mn];
			}
		
		// Now scan forward map_edge_path
		// to detect and remove any bounce
		// within a face
		
		//cout << "\nStarting scan of map_edge_path";
		
		me_prev=NULL_EDGE;
		bounce_detected=false;
		forall (me,DD.map_edge_path) 
			{
			//cout << "\nScanning "<< DD.map.id(me) << " within map_edge_path" << flush;
			
			if (me_prev)
				if (DD.map.is_marked(me,RM3_DD_CROSS_ON_FACE) && DD.map.is_marked(me_prev,RM3_DD_CROSS_ON_FACE))
					if (DD.face_of_map_edge[me]==DD.face_of_map_edge[me_prev])
						{
						bounce_detected=true;  //cout << "\nBOUNCE DETECTED" << flush;
						DD.map.del_edge(me_prev);
						break;
						}
			me_prev=me;
			}
		
		//cout << "\nScanning of map_edge_path terminated";
		
		// Apply again the min-cost-flow
		// after the removal of the first map-gdtedge
		// causing the first bounce
		
		} while (bounce_detected);		
	//cout << "\nEnd   computation of a minimum cost path" << flush;
	
	// initialization of curr_start_heading
	gdtedge first_edge = DD.map_edge_path.inf(DD.map_edge_path.first());
	gdtnode first_terminal_node = DD.vt1;
	gdtnode first_terminal_center_map_node = DD.mnt1;
	map_node_set mns1 = DD.map_nodes_of_node[first_terminal_node];
	if      (DD.map.find_directed_edge(first_terminal_center_map_node, mns1.north_node) == first_edge) DD.initial_heading = north;
	else if (DD.map.find_directed_edge(first_terminal_center_map_node, mns1.east_node)  == first_edge) DD.initial_heading = east;
	else if (DD.map.find_directed_edge(first_terminal_center_map_node, mns1.south_node) == first_edge) DD.initial_heading = south;
	else if (DD.map.find_directed_edge(first_terminal_center_map_node, mns1.west_node)  == first_edge) DD.initial_heading = west;
	else gdt_error("DD.initial_heading initialization strategy failed");
	}
	
	
	
	
	
	// DD_dynamic_new_edge subroutine
	// STEP 5 - Build the new dime-gdtedge as a sequence of dime-edges according to map_edge_path
	// 
	void
	dime_orth_plan_undi_graph::
DD_build_dime_edge_path
	(DD_struct& DD)
	{
	//cout << "\nBegin build_dime_edge_path" << flush;
	gdtedge   e;    // created once here instead of create a new one at every cycle
	gdtedge   me;   // current map gdtedge
	face   fx;   // face to be splitted
	gdtedge   ev;   // after this gdtedge will start  the new part of dime gdtedge
	gdtedge   ew;   // after this gdtedge will finish the new part of dime gdtedge
	gdtnode   vx1;
	gdtnode   vx2;
	gdtnode   aux_n;		// gdtnode at the start of current map gdtedge at every step
	gdtedge   curr_e;		// current dime gdtedge at the end of each single step
	gdtnode   crossed_node;
	string bend_type = "";	// bend_type flag
	gdt::gdtlist<gdtedge>        edges_to_be_removed;
//	gdt::gdtlist<gdtnode>        nodes_to_be_removed;
	gdt::gdtlist<border_step> border_steps_to_be_thickned;
	border_step       bs;
	heading_type      curr_start_heading = DD.initial_heading;
	
	bool frozen_layout_backup = frozen_layout;
	frozen_layout = true;
	
	vx1   = DD.source_terminal();
	aux_n = DD.source_terminal();
	vx2   = NULL_NODE;
	fx    = NULL_FACE;
	ev    = NULL_EDGE;
	ew    = NULL_EDGE;
	
	                     ev = find_edge_leaving_node_with_heading(vx1, heading_after_rotation(curr_start_heading, _090));
	if (ev == NULL_EDGE) ev = find_edge_leaving_node_with_heading(vx1, heading_after_rotation(curr_start_heading, _180));
	if (ev == NULL_EDGE) ev = find_edge_leaving_node_with_heading(vx1, heading_after_rotation(curr_start_heading, _270));
	if (ev == NULL_EDGE) ev = find_edge_leaving_node_with_heading(vx1, curr_start_heading);
	//int cost = 0;
	gdt::list_item li;
	forall_items (li, DD.map_edge_path)
		{
		me = DD.map_edge_path.inf(li);
		//cout << "\n\tmap-gdtedge along the shortest path " << DD.map.id(me) << flush;
		curr_e = DD.edge_of_map_edge[me];	// This operation may return NULL_EDGE
		//if (curr_e) cout << "\n\t\tcurrent gdtedge = " << id(curr_e) << flush;
		//cout << "\n\t\tcurrent gdtnode = " << id(aux_n) << flush;
		
		//cost += DD.map_edge_cost[me];
		
		if (DD.map.is_marked(me,RM3_DD_CROSS_ON_FACE))
			{
			//cout << "\n\t\tCROSS_ON_FACE" << flush;
			fx = DD.face_of_map_edge[me];
			}
		
		else if (DD.map.is_marked(me,RM3_DD_CROSS_ON_DUMMY_EDGE) ||
			 DD.map.is_marked(me,RM3_DD_WALK_ALONG_DUMMY_EDGE))
			{
			//cout << "\n\t\tRM3_DD_CROSS_ON_DUMMY_EDGE or RM3_DD_WALK_ALONG_DUMMY_EDGE" << flush;
			//curr_e = DD.edge_of_map_edge[me];
			if (edges_to_be_removed.search(curr_e)==NULL) edges_to_be_removed.append(curr_e);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_CROSS_ON_REAL_EDGE))
			{
			//cout << "\n\t\tRM3_DD_CROSS_ON_REAL_EDGE" << flush;
			//curr_e = DD.edge_of_map_edge[me];
			
			int old_id = id(curr_e);
			
			// The dime::new_node is called (in place of the ortho one)
			// because the dime::new_node(gdtedge) sets the initial heading
			// of the new generated border steps
			//
			vx2 = new_node(curr_e);
			
			mark (vx2,RM3_CROSS_ON_REAL_EDGE);
			DD_init_map_node_set (DD.map_nodes_of_node[vx2], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
			
			// the current map gdtedge now belongs to a gdtnode
			DD.node_of_map_edge[me] = vx2;
			
			gdtedge e1 = first_adj_edge(vx2);
			gdtedge e2 = last_adj_edge(vx2);
			if (id(e1)>id(e2)) assign_id(e1,old_id);
			else               assign_id(e2,old_id);
			
			forall (e, edges_to_be_removed) fx = del_edge(e);
			edges_to_be_removed.clear();
//			gdtnode n;
//			forall (n, nodes_to_be_removed) del_node(n);
//			nodes_to_be_removed.clear();
			
			// ev is redetermined from vx1 and curr_start_heading because
			// to have vx2 an gdtedge was splitted and deleted and
			// if ev was exactly that gdtedge an arror may occur
			if (!edge_belongs_to_node(ev,vx1))
				{
				if (edge_belongs_to_node(e1,vx1)) ev = e1;
				else if (edge_belongs_to_node(e2,vx1)) ev = e2;
				else  gdt_error("gdtedge no more existent, but not because of split of gdtedge by a new_node(gdtedge)");
				}
			
			curr_start_heading = heading_after_rotation(curr_start_heading, DD_angle_of_bends_of_map_edges_before_list_item_starting_from_node(li, vx1, DD));
			ew = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _270));
			
			// The ortho::new_edge is called (in place of the dime one)
			// because the dime::new_edge attempts to preserve headings,
			// which is useless now, since a local_init is required anyway
			// after the rectangularization
			//
			gdtedge aux = orth_plan_undi_graph::new_edge (vx1,ev,vx2,ew);
			DD.edge_path.append(aux);
			//cout << "\n\t\tIn DD.edge_path appended gdtedge " << id(aux) << flush;
			DD_init_map_node_set (DD.map_nodes_of_edge[aux], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
			//cout << "\n\t\tused gdtedge = " << id(aux) << flush;
			//cout << "\n\t\tused gdtnode = " << id(vx2) << flush;
			ev    = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _090));
			fx    = NULL_FACE;
			vx1   = vx2;
			aux_n = vx2;
			}
			
		else if (DD.map.is_marked(me,RM3_DD_CROSS_ON_DUMMY_NODE_BETWEEN_DUMMY_EDGES))
			{
			//cout << "\n\t\tRM3_DD_CROSS_ON_DUMMY_NODE_BETWEEN_DUMMY_EDGES" << flush;
			crossed_node = DD.node_of_map_edge[me];
			forall_edges_adjacent_node (e,crossed_node,*this)
				if (edges_to_be_removed.search(e)==NULL) edges_to_be_removed.append(e);
//			nodes_to_be_removed.append(crossed_node);
			}
			
		else if (DD.map.is_marked(me,RM3_DD_CROSS_ON_DUMMY_NODE_BETWEEN_REAL_EDGES))
			{
			//cout << "\n\t\tRM3_DD_CROSS_ON_DUMMY_NODE_BETWEEN_REAL_EDGES" << flush;
			crossed_node = DD.node_of_map_edge[me];
			unmark(crossed_node,RM3_ADDED_BY_RECTANGULARIZATION);
			
			vx2 = crossed_node;
			mark(vx2,RM3_CROSS_ON_REAL_EDGE);
			
			forall (e, edges_to_be_removed) fx = del_edge(e);
			edges_to_be_removed.clear();
//			gdtnode n;
//			forall (n, nodes_to_be_removed) del_node(n);
//			nodes_to_be_removed.clear();
			
			curr_start_heading = heading_after_rotation(curr_start_heading, DD_angle_of_bends_of_map_edges_before_list_item_starting_from_node(li, vx1, DD));
			ew = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _270));
			
			// The ortho::new_edge is called (in place of the dime one)
			// because the dime::new_edge attempts to preserve headings,
			// which is useless now, since a local_init is required anyway
			// after the rectangularization
			//
			gdtedge aux = orth_plan_undi_graph::new_edge (vx1,ev,vx2,ew);
			DD.edge_path.append(aux);
			//cout << "\n\t\tIn DD.edge_path appended gdtedge " << id(aux) << flush;
			DD_init_map_node_set (DD.map_nodes_of_edge[aux], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
			//cout << "\n\t\tused gdtedge = " << id(aux) << flush;
			//cout << "\n\t\tused gdtnode = " << id(crossed_node) << flush;
			ev    = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _090));
			fx    = NULL_FACE;
			vx1   = vx2;
			aux_n = vx2;
			}
		
		else if (DD.map.is_marked(me,RM3_DD_WALK_AND_TURN_LEFT_ALONG_DUMMY_EDGE))
			{
			//cout << "\n\t\tRM3_DD_WALK_AND_TURN_LEFT_ALONG_DUMMY_EDGE" << flush;
			//-------------------------------------------
			// Put in list to be removed the current gdtedge
			//-------------------------------------------
			if (edges_to_be_removed.search(curr_e)==NULL)
				//{
				edges_to_be_removed.append(curr_e);
				//cout << "\n\t\tEdge to remove " << id(curr_e) << " in lista" << flush;
				//}
			
			bend_type = "";
//			curr_start_heading = heading_after_rotation(curr_start_heading, _270);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_DUMMY_EDGE))
			{
			//cout << "\n\t\tRM3_DD_WALK_AND_TURN_RIGHT_ALONG_DUMMY_EDGE" << flush;
			//-------------------------------------------
			// Put in list to be removed the current gdtedge
			//-------------------------------------------
			if (edges_to_be_removed.search(curr_e)==NULL)
				//{
				edges_to_be_removed.append(curr_e);
				//cout << "\n\t\tEdge to remove " << id(curr_e) << " in lista" << flush;
				//}
			
			bend_type = "";
//			curr_start_heading = heading_after_rotation(curr_start_heading, _090);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_TURN_LEFT_AND_WALK_ALONG_DUMMY_EDGE))
			{
			//cout << "\n\t\tRM3_DD_TURN_LEFT_AND_WALK_ALONG_DUMMY_EDGE" << flush;
			
			//-------------------------------------------
			// Put in list to be removed the current gdtedge
			//-------------------------------------------
			if (edges_to_be_removed.search(curr_e)==NULL)
				//{
				edges_to_be_removed.append(curr_e);
				//cout << "\n\t\tEdge to remove " << id(curr_e) << " in lista" << flush;
				//}
			
//			curr_start_heading = heading_after_rotation(curr_start_heading, _270);
			bend_type = "L";
			}
		
		else if (DD.map.is_marked(me,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_DUMMY_EDGE))
			{
			//cout << "\n\t\tRM3_DD_TURN_RIGHT_AND_WALK_ALONG_DUMMY_EDGE" << flush;
			
			//-------------------------------------------
			// Put in list to be removed the current gdtedge
			//-------------------------------------------
			if (edges_to_be_removed.search(curr_e)==NULL)
				//{
				edges_to_be_removed.append(curr_e);
				//cout << "\n\t\tEdge to remove " << id(curr_e) << " in lista" << flush;
				//}
			
//			curr_start_heading = heading_after_rotation(curr_start_heading, _090);
			bend_type = "R";
			}
		
		else if (DD.map.is_marked(me,RM3_DD_WALK_AND_TURN_LEFT_ALONG_EAGLE_BOUNDARY_EDGE))
			{
			//cout << "\n\t\tRM3_DD_WALK_AND_TURN_LEFT_ALONG_EAGLE_BOUNDARY_EDGE" << flush;
			
			//--------------------------------------------------------------------
			// Splitting of the dime gdtedge with a gdtnode that will be a RM3_BEND_NODE
			//--------------------------------------------------------------------
			map_node_set mnse = DD.map_nodes_of_edge[DD.edge_of_map_edge[me]];
			vx2 = new_node(DD.edge_of_map_edge[me]);
			mark (vx2,RM3_BEND_NODE);
			DD.map_nodes_of_node[vx2] = mnse;
			DD_init_map_node_set (DD.map_nodes_of_edge[DD.edge_of_map_edge[me]], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
			
			// the current map gdtedge now belongs to a gdtnode
			DD.node_of_map_edge[me] = vx2;
			
			curr_e = find_edge(aux_n, vx2);
			DD.edge_of_map_edge[me] = curr_e;
			
			//---------------------------------------------------------------------------------------------------
			// If the next gdtedge in the minimal cost path is a RM3_DD_TURN_AND_WALK_LEFT_ALONG_EAGLE_BOUNDARY_EDGE
			// the dime gdtedge of this map gdtedge is the gdtedge before the split that exists no more now.
			// This correspondence is now resetted
			//---------------------------------------------------------------------------------------------------
			gdtedge succ_me = DD.map_edge_path.inf(DD.map_edge_path.succ(li));
			if (DD.map.is_marked(succ_me, RM3_DD_TURN_LEFT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE))
				{
				//cout << "\n\t\tChangement of correspondence for the TAW of the eagle boundary gdtedge" << flush;
				gdtedge new_corresponding = first_adj_edge(vx2);
				if (opposite(vx2, new_corresponding) == aux_n)
					new_corresponding = last_adj_edge(vx2);
				DD.edge_of_map_edge[succ_me] = new_corresponding;
				DD.bends_of_map_edge[succ_me] = "RRL";
				//cout << "\n\t\tthe post split dime-gdtedge associated with WAT is " << id(curr_e) << " and with TAW is " << id(new_corresponding) << flush;
				}
			
			//----------------------------------------
			// The border_step is put in the list as L
			//----------------------------------------
			//cout << "\n\t\tbend type is '" << bend_type << "'" << flush;
			border_steps_to_be_thickned.append(border_step_moving_along_edge(curr_e, aux_n));
			
			//-----------------------------------------------------
			// With the correct bend_type flag
			//  the arbitrary choose of WALK_ALONG is corrected and
			//  the correct border_step is thickned
			//-----------------------------------------------------
			forall (bs, border_steps_to_be_thickned) DD_increase_thickness_of_border_step(opposite_border_step(bs));
			border_steps_to_be_thickned.clear();
			bend_type = "";
			
			//----------------------------------
			// Append the gdtedge in edge_path list
			//----------------------------------
			if (DD.edge_path.search(curr_e)==NULL) DD.edge_path.append(curr_e);
			
			//cout << "\n\t\tAppended in DD.edge_path the gdtedge " << id(curr_e) << flush;
			//cout << "\n\t\tused gdtedge = " << id(curr_e) << flush;
			//cout << "\n\t\tused gdtnode = " << id(aux_n) << flush;
			
//			curr_start_heading = heading_after_rotation(curr_start_heading, _270);
			fx     = NULL_FACE;
			ev     = find_edge_leaving_node_with_heading(vx2, curr_start_heading);
			vx1    = vx2;
			aux_n  = vx2;
			}
		
		else if (DD.map.is_marked(me,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_EAGLE_BOUNDARY_EDGE))
			{
			//cout << "\n\t\tRM3_DD_WALK_AND_TURN_RIGHT_ALONG_EAGLE_BOUNDARY_EDGE" << flush;
			
			//--------------------------------------------------------------------
			// Splitting of the dime gdtedge with a gdtnode that will be a RM3_BEND_NODE
			//--------------------------------------------------------------------
			map_node_set mnse = DD.map_nodes_of_edge[DD.edge_of_map_edge[me]];
			vx2 = new_node(DD.edge_of_map_edge[me]);
			mark (vx2,RM3_BEND_NODE);
			DD.map_nodes_of_node[vx2] = mnse;
			DD_init_map_node_set (DD.map_nodes_of_edge[DD.edge_of_map_edge[me]], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
			
			// the current map gdtedge now belongs to a gdtnode
			DD.node_of_map_edge[me] = vx2;
			
			curr_e = find_edge(aux_n, vx2);
			DD.edge_of_map_edge[me] = curr_e;
			
			//---------------------------------------------------------------------------------------------------
			// If the next gdtedge in the minimal cost path is a RM3_DD_TURN_AND_WALK_RIGHT_ALONG_EAGLE_BOUNDARY_EDGE
			// the dime gdtedge of this map gdtedge is the gdtedge before the split that exists no more now.
			// This correspondence is now resetted
			//---------------------------------------------------------------------------------------------------
			gdtedge succ_me = DD.map_edge_path.inf(DD.map_edge_path.succ(li));
			if (DD.map.is_marked(succ_me, RM3_DD_TURN_RIGHT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE))
				{
				//cout << "\n\t\tChangment of correspondence for the TAW of the eagle boundary gdtedge" << flush;
				gdtedge new_corresponding = first_adj_edge(vx2);
				if (opposite(vx2, new_corresponding) == aux_n)
					new_corresponding = last_adj_edge(vx2);
				DD.edge_of_map_edge[succ_me] = new_corresponding;
				DD.bends_of_map_edge[succ_me] = "LLR";
				//cout << "\n\t\tthe post split dime-gdtedge associated with WAT is " << id(curr_e) << " and with TAW is " << id(new_corresponding) << flush;
				}
			
			//----------------------------------------
			// The border_step is put in the list as R
			//----------------------------------------
			//cout << "\n\t\tbend type is '" << bend_type << "'" << flush;
			border_steps_to_be_thickned.append(border_step_moving_along_edge(curr_e, aux_n));
			
			//-----------------------------------------------------
			// With the correct bend_type flag
			//  the arbitrary choose of WALK_ALONG is corrected and
			//  the correct border_step is thickned
			//-----------------------------------------------------
			forall (bs, border_steps_to_be_thickned) DD_increase_thickness_of_border_step(bs);
			border_steps_to_be_thickned.clear();
			bend_type = "";
			
			//----------------------------------
			// Append the gdtedge in edge_path list
			//----------------------------------
			if (DD.edge_path.search(curr_e)==NULL) DD.edge_path.append(curr_e);
			
			//cout << "\n\t\tAppended in DD.edge_path the gdtedge " << id(curr_e) << flush;
			//cout << "\n\t\tused gdtedge = " << id(curr_e) << flush;
			//cout << "\n\t\tused gdtnode = " << id(aux_n) << flush;
			
//			curr_start_heading = heading_after_rotation(curr_start_heading, _090);
			fx     = NULL_FACE;
			ev     = curr_e;
			vx1    = vx2;
			aux_n  = vx2;
			}
		
		else if (DD.map.is_marked(me,RM3_DD_WALK_AND_TURN_LEFT_ALONG_REAL_EDGE))
			{
			//cout << "\n\t\tRM3_DD_WALK_AND_TURN_LEFT_ALONG_REAL_EDGE" << flush;
			
			//--------------------------------------------------------------------
			// Splitting of the dime gdtedge with a gdtnode that will be a RM3_BEND_NODE
			//--------------------------------------------------------------------
			map_node_set mnse = DD.map_nodes_of_edge[DD.edge_of_map_edge[me]];
			vx2 = new_node(DD.edge_of_map_edge[me]);
			mark (vx2,RM3_BEND_NODE);
			DD.map_nodes_of_node[vx2] = mnse;
			DD_init_map_node_set (DD.map_nodes_of_edge[DD.edge_of_map_edge[me]], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
			
			// the current map gdtedge now belongs to a gdtnode
			DD.node_of_map_edge[me] = vx2;
			
			curr_e = find_edge(aux_n, vx2);
			DD.edge_of_map_edge[me] = curr_e;
			
			//----------------------------------------
			// The border_step is put in the list as R
			//----------------------------------------
			//cout << "\n\t\tbend type is '" << bend_type << "'" << flush;
			border_steps_to_be_thickned.append(border_step_moving_along_edge(curr_e, aux_n));
			
			//-----------------------------------------------------
			// With the correct bend_type flag
			//  the arbitrary choose of WALK_ALONG is corrected and
			//  the correct border_step is thickned
			//-----------------------------------------------------
			forall (bs, border_steps_to_be_thickned) DD_increase_thickness_of_border_step(opposite_border_step(bs));
			border_steps_to_be_thickned.clear();
			
			bend_type = "";
			
			//----------------------------------
			// Append the gdtedge in edge_path list
			//----------------------------------
			if (DD.edge_path.search(curr_e)==NULL) DD.edge_path.append(curr_e);
			
			//cout << "\n\t\tAppended in DD.edge_path the gdtedge " << id(curr_e) << flush;
			//cout << "\n\t\tused gdtedge = " << id(curr_e) << flush;
			//cout << "\n\t\tused gdtnode = " << id(aux_n) << flush;
			
//			curr_start_heading = heading_after_rotation(curr_start_heading, _270);
			fx     = NULL_FACE;
			ev     = find_edge_leaving_node_with_heading(vx2, curr_start_heading);
			vx1    = vx2;
			aux_n  = vx2;
			}
		
		else if (DD.map.is_marked(me,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_REAL_EDGE))
			{
			//cout << "\n\t\tRM3_DD_WALK_AND_TURN_RIGHT_ALONG_REAL_EDGE" << flush;
			
			//--------------------------------------------------------------------
			// Splitting of the dime gdtedge with a gdtnode that will be a RM3_BEND_NODE
			//--------------------------------------------------------------------
			map_node_set mnse = DD.map_nodes_of_edge[DD.edge_of_map_edge[me]];
			vx2 = new_node(DD.edge_of_map_edge[me]);
			mark (vx2,RM3_BEND_NODE);
			DD.map_nodes_of_node[vx2] = mnse;
			DD_init_map_node_set (DD.map_nodes_of_edge[DD.edge_of_map_edge[me]], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
			
			// the current map gdtedge now belongs to a gdtnode
			DD.node_of_map_edge[me] = vx2;
			
			curr_e = find_edge(aux_n, vx2);
			DD.edge_of_map_edge[me] = curr_e;
			
			//----------------------------------------
			// The border_step is put in the list as R
			//----------------------------------------
			//cout << "\n\t\tbend type is '" << bend_type << "'" << flush;
			border_steps_to_be_thickned.append(border_step_moving_along_edge(curr_e, aux_n));
			
			//-----------------------------------------------------
			// With the correct bend_type flag
			//  the arbitrary choose of WALK_ALONG is corrected and
			//  the correct border_step is thickned
			//-----------------------------------------------------
			forall (bs, border_steps_to_be_thickned) DD_increase_thickness_of_border_step(bs);
			border_steps_to_be_thickned.clear();
			
			bend_type = "";
			
			//----------------------------------
			// Append the gdtedge in edge_path list
			//----------------------------------
			if (DD.edge_path.search(curr_e)==NULL) DD.edge_path.append(curr_e);
			
			//cout << "\n\t\tAppended in DD.edge_path the gdtedge " << id(curr_e) << flush;
			//cout << "\n\t\tused gdtedge = " << id(curr_e) << flush;
			//cout << "\n\t\tused gdtnode = " << id(aux_n) << flush;
			
//			curr_start_heading = heading_after_rotation(curr_start_heading, _090);
			fx     = NULL_FACE;
			ev     = curr_e;
			vx1    = vx2;
			aux_n  = vx2;
			}
		
		else if (DD.map.is_marked(me,RM3_DD_TURN_LEFT_AND_WALK_ALONG_REAL_EDGE) ||
		         DD.map.is_marked(me,RM3_DD_TURN_LEFT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE))
			{
			//if (DD.map.is_marked(me,RM3_DD_TURN_LEFT_AND_WALK_ALONG_REAL_EDGE))
			//	cout << "\n\t\tRM3_DD_TURN_LEFT_AND_WALK_ALONG_REAL_EDGE" << flush;
			//else
			//	cout << "\n\t\tRM3_DD_TURN_LEFT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE" << flush;
			
			int old_id = id(curr_e);
			
			//--------------------------------------------------------------------
			// Splitting of the dime gdtedge with a gdtnode that will be a RM3_BEND_NODE
			//--------------------------------------------------------------------
			map_node_set mnse = DD.map_nodes_of_edge[DD.edge_of_map_edge[me]];
			vx2 = new_node(DD.edge_of_map_edge[me]);
			mark (vx2,RM3_BEND_NODE);
			//cout << "\n\t\tvx2 = " << id(vx2) << flush;
			
			// the current map gdtedge now belongs to a gdtnode
			DD.node_of_map_edge[me] = vx2;

			gdtedge e1 = first_adj_edge(vx2);
			gdtedge e2 = last_adj_edge(vx2);
			if (id(e1)>id(e2)) assign_id(e1,old_id);
			else               assign_id(e2,old_id);
			
			//-----------------------------
			// The bend_type flag is setted
			//-----------------------------
			bend_type = "L";
			//cout << "\n\t\tbend type is '" << bend_type << "'" << flush;
						
			//-----------------------------------------------
			// The correct border step is thickned:
			//  the gdtedge starts from the vx2 gdtnode and
			//  the direction belongs from the bend_type flag
			//-----------------------------------------------
			gdtedge succ_succ_me = DD.map_edge_path.inf(DD.map_edge_path.succ(DD.map_edge_path.succ(li)));
			gdtnode succ_n       = DD.node_of_map_edge[succ_succ_me];
			gdtedge eee          = find_edge(vx2, succ_n);
			DD_increase_thickness_of_border_step(border_step_moving_along_edge(eee, succ_n));
			
			//ew = eee;
			
			// --------------------
			// Creation of new gdtedge
			// --------------------
			forall (e, edges_to_be_removed) fx = del_edge(e);
			edges_to_be_removed.clear();
//			gdtnode n;
//			forall (n, nodes_to_be_removed) del_node(n);
//			nodes_to_be_removed.clear();
			// fx may have a null value only in case of EAGLE_BOUNDARY gdtedge
			if (fx == NULL_FACE) fx = left_face_moving_along_edge(DD.edge_of_map_edge[me],vx1);
			
			// ev is redetermined from vx1 and curr_start_heading because
			// to have vx2 an gdtedge was splitted and deleted and
			// if ev was exactly that gdtedge an arror may occur
			if (!edge_belongs_to_node(ev,vx1))
				{
				if (edge_belongs_to_node(e1,vx1)) ev = e1;
				else if (edge_belongs_to_node(e2,vx1)) ev = e2;
				else  gdt_error("gdtedge no more existent, but not because of split of gdtedge by a new_node(gdtedge)");
				}
			
//		cout << "\nTLAWA(R/EB)E:\tev = " << id(ev) << flush;
//		cout << "\nTLAWA(R/EB)E:\tcurr_start_heading before = " << curr_start_heading << flush;
			curr_start_heading = heading_after_rotation(curr_start_heading, DD_angle_of_bends_of_map_edges_before_list_item_starting_from_node(li, vx1, DD));
//		cout << "\nTLAWA(R/EB)E:\tcurr_start_heading after  = " << curr_start_heading << flush;
			ew = find_edge_leaving_node_with_heading(vx2, curr_start_heading);
//		cout << "\nTLAWA(R/EB)E:\tew = " << id(ew) << flush;
			
			// The ortho::new_edge is called (in place of the dime one)
			// because the dime::new_edge attempts to preserve headings,
			// which is useless now, since a local_init is required anyway
			// after the rectangularization
			//
			gdtedge aux = orth_plan_undi_graph::new_edge (vx1,ev,vx2,ew);
//		cout << "\nTLAWA(R/EB)E:\tcreated gdtedge aux = " << id(aux) << flush;
			
			//cout << "\n\t\tcreated new gdtedge " << id(aux) << " from " << id(vx1) << " to " << id(vx2) << flush;
			DD.edge_path.append(aux);
			//cout << "\n\t\tAppended in DD.edge_path the gdtedge " << id(aux) << flush;
			DD_init_map_node_set (DD.map_nodes_of_edge[aux], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
			DD.map_nodes_of_node[vx2] = mnse;
			DD_init_map_node_set (DD.map_nodes_of_edge[DD.edge_of_map_edge[me]], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
			curr_e = find_edge(aux_n, vx2);
			DD.edge_of_map_edge[me] = curr_e;
			
			//---------------------------------
			// The gdtedge is put in the edge_path
			//---------------------------------
			if (DD.edge_path.search(eee)==NULL) DD.edge_path.append(eee);
			
			//cout << "\n\t\tAppended in DD.edge_path the gdtedge " << id(eee) << flush;
			//cout << "\n\t\tused gdtedge = " << id(eee) << flush;
			//cout << "\n\t\tused gdtnode = " << id(vx2) << flush;
			
//			curr_start_heading = heading_after_rotation(curr_start_heading, _270);
			ev    = NULL_EDGE;
			fx    = NULL_FACE;
			vx1   = vx2;
			aux_n = vx2;
			}
		
		else if (DD.map.is_marked(me,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_REAL_EDGE) ||
		         DD.map.is_marked(me,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE))
			{
			//if (DD.map.is_marked(me,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_REAL_EDGE))
			//	cout << "\n\t\tRM3_DD_TURN_RIGHT_AND_WALK_ALONG_REAL_EDGE" << flush;
			//else
			//	cout << "\n\t\tRM3_DD_TURN_RIGHT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE" << flush;
			
			int old_id = id(curr_e);
			
			//--------------------------------------------------------------------
			// Splitting of the dime gdtedge with a gdtnode that will be a RM3_BEND_NODE
			//--------------------------------------------------------------------
			map_node_set mnse = DD.map_nodes_of_edge[DD.edge_of_map_edge[me]];
			vx2 = new_node(DD.edge_of_map_edge[me]);
			mark (vx2,RM3_BEND_NODE);
			//cout << "\n\t\tvx2 = " << id(vx2) << flush;
			
			// the current map gdtedge now belongs to a gdtnode
			DD.node_of_map_edge[me] = vx2;

			gdtedge e1 = first_adj_edge(vx2);
			gdtedge e2 = last_adj_edge(vx2);
			if (id(e1)>id(e2)) assign_id(e1,old_id);
			else               assign_id(e2,old_id);
			
			//-----------------------------
			// The bend_type flag is setted
			//-----------------------------
			bend_type = "R";
			//cout << "\n\t\tbend type is '" << bend_type << "'" << flush;
			
			//-----------------------------------------------
			// The correct border step is thickned:
			//  the gdtedge starts from the vx2 gdtnode and
			//  the direction belongs from the bend_type flag
			//-----------------------------------------------
			gdtedge succ_succ_me = DD.map_edge_path.inf(DD.map_edge_path.succ(DD.map_edge_path.succ(li)));
			gdtnode succ_n       = DD.node_of_map_edge[succ_succ_me];
			gdtedge eee          = find_edge(vx2, succ_n);
			DD_increase_thickness_of_border_step(border_step_moving_along_edge(eee, vx2));
			
			//ew = cyclic_adj_pred(eee, vx2);
			
			// --------------------
			// Creation of new gdtedge
			// --------------------
			forall (e, edges_to_be_removed) fx = del_edge(e);
			edges_to_be_removed.clear();
//			gdtnode n;
//			forall (n, nodes_to_be_removed) del_node(n);
//			nodes_to_be_removed.clear();
			// fx may have a null value only in case of EAGLE_BOUNDARY gdtedge
			if (fx == NULL_FACE) fx = right_face_moving_along_edge(DD.edge_of_map_edge[me],vx1);
			
			// ev is redetermined from vx1 and curr_start_heading because
			// to have vx2 an gdtedge was splitted and deleted and
			// if ev was exactly that gdtedge an arror may occur
			if (!edge_belongs_to_node(ev,vx1))
				{
				if (edge_belongs_to_node(e1,vx1)) ev = e1;
				else if (edge_belongs_to_node(e2,vx1)) ev = e2;
				else  gdt_error("gdtedge no more existent, but not because of split of gdtedge by a new_node(gdtedge)");
				}
			
//		cout << "\nTRAWA(R/EB)E:\tev = " << id(ev) << flush;
//		cout << "\nTRAWA(R/EB)E:\tcurr_start_heading before = " << curr_start_heading << flush;
			curr_start_heading = heading_after_rotation(curr_start_heading, DD_angle_of_bends_of_map_edges_before_list_item_starting_from_node(li, vx1, DD));
//		cout << "\nTRAWA(R/EB)E:\tcurr_start_heading after  = " << curr_start_heading << flush;
			ew = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _180));
//		cout << "\nTRAWA(R/EB)E:\tew = " << id(ew) << flush;
			
			// The ortho::new_edge is called (in place of the dime one)
			// because the dime::new_edge attempts to preserve headings,
			// which is useless now, since a local_init is required anyway
			// after the rectangularization
			//
			gdtedge aux = orth_plan_undi_graph::new_edge (vx1,ev,vx2,ew);
					
			//cout << "\n\t\tcreated a new gdtedge " << id(aux) << " from " << id(vx1) << " to " << id(vx2) << flush;
			DD.edge_path.append(aux);
			//cout << "\n\t\tAppended in DD.edge_path the gdtedge " << id(aux) << flush;
			DD_init_map_node_set (DD.map_nodes_of_edge[aux], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
			DD.map_nodes_of_node[vx2] = mnse;
			DD_init_map_node_set (DD.map_nodes_of_edge[DD.edge_of_map_edge[me]], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
			curr_e = find_edge(aux_n, vx2);
			DD.edge_of_map_edge[me] = curr_e;
			
			//---------------------------------
			// The gdtedge is put in the edge_path
			//---------------------------------
			if (DD.edge_path.search(eee)==NULL) DD.edge_path.append(eee);
			
			//cout << "\n\t\tAppended in DD.edge_path the gdtedge " << id(eee) << flush;
			//cout << "\n\t\tused gdtedge = " << id(eee) << flush;
			//cout << "\n\t\tused gdtnode = " << id(vx2) << flush;
			
//			curr_start_heading = heading_after_rotation(curr_start_heading, _090);
			ev    = NULL_EDGE;
			fx    = NULL_FACE;
			vx1   = vx2;
			aux_n = vx2;
			}
		
		else if (DD.map.is_marked(me,RM3_DD_WALK_ALONG_REAL_EDGE))
			{
			//cout << "\n\t\tRM3_DD_WALK_ALONG_REAL_EDGE" << flush;
			//------------------------------------------
			// There is to discrimine between two cases:
			//  a WALK_ALONG before a WALK_AND_TURN and
			//  a WALK_ALONG after  a TURN_AND_WALK
			//------------------------------------------
			gdtedge eee = DD.edge_of_map_edge[me];
			//------------------------------------------
			// Case of WALK_ALONG before a WALK_AND_TURN
			//------------------------------------------
			//cout << "\n\t\tbend type is '" << bend_type << "'" << flush;
			if      (bend_type == "")
				{
				//-----------------------------------------------------
				// A border step must be put in the list, but
				//  it is not known the direction of border_step.
				//  Therefore the rigth direction is arbitrarly choosen
				//  and WALK_AND_TURN case will correct it
				//-----------------------------------------------------
				bs = border_step_moving_along_edge(eee, aux_n);
				//cout << "\n\t\tDirect border step is chosen from gdtnode " << id(start_of_border_step(bs)) <<
				//	" along gdtedge " << id(edge_of_border_step(bs)) << flush;
				border_steps_to_be_thickned.append(bs);
				}
			//-----------------------------------------
			// Case of WALK_ALONG after a TURN_AND_WALK
			//-----------------------------------------
				//--------------------------------------------------------
				// After the TURN_AND_WALK the correct direction is known.
				//  Therefore the border step is directly thickned,
				//  without appending it in list
				//--------------------------------------------------------
			else if (bend_type == "R")
				{
				bs = border_step_moving_along_edge(eee, aux_n);
				//cout << "\n\t\tDirect border step is chosen from gdtnode " << id(start_of_border_step(bs)) <<
				//	" along gdtedge " << id(edge_of_border_step(bs)) << flush;
				DD_increase_thickness_of_border_step(bs);
				}
			else if (bend_type == "L")
				{
				bs = border_step_moving_along_edge(eee, opposite(aux_n, eee));
				//cout << "\n\t\tOpposite border step is chosen from gdtnode " << id(start_of_border_step(bs)) <<
				//	" along gdtedge " << id(edge_of_border_step(bs)) << flush;
				DD_increase_thickness_of_border_step(bs);
				}
			else
				gdt_error("anomalous bend on gdtedge");
			DD.edge_path.append(eee);
			
			//cout << "\n\t\tAppended in DD.edge_path the gdtedge " << id(eee) << flush;
			//cout << "\n\t\tused gdtedge = " << id(eee) << flush;
			//cout << "\n\t\tused gdtnode = " << id(aux_n) << flush;
			}
		
		else if (DD.map.is_marked(me,RM3_DD_ACROSS_A_NODE))
			{
			//cout << "\n\t\tRM3_DD_ACROSS_A_NODE" << flush;
			aux_n = DD.node_of_map_edge[me];
			}
			
		else if (DD.map.is_marked(me,RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_FRONT_AND_LEFT_REAL_EDGES))
			{
			//cout << "\n\t\tRM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_FRONT_AND_LEFT_REAL_EDGES" << flush;
			bend_type = "L";
			vx2 = DD.node_of_map_edge[me];
			unmark(vx2,RM3_REPLACES_A_BEND);
			mark(vx2,RM3_BEND_NODE);
			
			forall (e, edges_to_be_removed) fx = del_edge(e);
			edges_to_be_removed.clear();
//			gdtnode n;
//			forall (n, nodes_to_be_removed) del_node(n);
//			nodes_to_be_removed.clear();
			
			curr_start_heading = heading_after_rotation(curr_start_heading, DD_angle_of_bends_of_map_edges_before_list_item_starting_from_node(li, vx1, DD));
			ew = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _270));
			
			// The ortho::new_edge is called (in place of the dime one)
			// because the dime::new_edge attempts to preserve headings,
			// which is useless now, since a local_init is required anyway
			// after the rectangularization
			//
			//cout << "\n\t\tstart gdtnode = " << id(vx1) << flush;
			//cout << "\n\t\tstop  gdtnode = " << id(vx2) << flush;
			gdtedge aux = orth_plan_undi_graph::new_edge (vx1,ev,vx2,ew);
			
			DD.edge_path.append(aux);
			//cout << "\n\t\tAppended in DD.edge_path the gdtedge " << id(aux) << flush;
			DD_init_map_node_set (DD.map_nodes_of_edge[aux], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
			//cout << "\n\t\tused gdtedge = " << id(aux) << flush;
			//cout << "\n\t\tused gdtnode = " << id(vx2) << flush;
			ev    = NULL_EDGE;
			fx    = NULL_FACE;
			vx1   = vx2;
			aux_n = vx2;
			}
			
		else if (DD.map.is_marked(me,RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_FRONT_AND_RIGHT_REAL_EDGES))
			{
			//cout << "\n\t\tRM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_FRONT_AND_RIGHT_REAL_EDGES" << flush;
			bend_type = "R";
			vx2 = DD.node_of_map_edge[me];
			unmark(vx2,RM3_REPLACES_A_BEND);
			mark(vx2,RM3_BEND_NODE);
			
			//cout << "\nprint of edges to be removed" << flush;
			forall (e, edges_to_be_removed)
				{
				//cout << "\nEdge to remove = " << id(e) << flush;
				fx = del_edge(e);
				}
			edges_to_be_removed.clear();
//			gdtnode n;
//			forall (n, nodes_to_be_removed) del_node(n);
//			nodes_to_be_removed.clear();
			
			curr_start_heading = heading_after_rotation(curr_start_heading, DD_angle_of_bends_of_map_edges_before_list_item_starting_from_node(li, vx1, DD));
			ew = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _270));
			if (ew == NULL_EDGE) ew = find_edge_leaving_node_with_heading(vx2, curr_start_heading);
			
			// The ortho::new_edge is called (in place of the dime one)
			// because the dime::new_edge attempts to preserve headings,
			// which is useless now, since a local_init is required anyway
			// after the rectangularization
			//
			//cout << "\n\t\tstart gdtnode = " << id(vx1) << flush;
			//cout << "\n\t\tstop  gdtnode = " << id(vx2) << flush;
			gdtedge aux = orth_plan_undi_graph::new_edge (vx1,ev,vx2,ew);
			
			DD.edge_path.append(aux);
			//cout << "\n\t\tAppended in DD.edge_path the gdtedge " << id(aux) << flush;
			DD_init_map_node_set (DD.map_nodes_of_edge[aux], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);
			//cout << "\n\t\tused gdtedge = " << id(aux) << flush;
			//cout << "\n\t\tused gdtnode = " << id(vx2) << flush;
			ev    = NULL_EDGE;
			fx    = NULL_FACE;
			vx1   = vx2;
			aux_n = vx2;
			}
			
		else if (DD.map.is_marked(me,RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_BACK_AND_LEFT_REAL_EDGES))
			{
			//cout << "\n\t\tRM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_BACK_AND_LEFT_REAL_EDGES" << flush;
			vx2   = DD.node_of_map_edge[me];
			unmark(vx2,RM3_REPLACES_A_BEND);
			mark(vx2,RM3_BEND_NODE);
			
			//----------------------------------------------------
			// The border_step in list border_steps_to_be_thickned
			// now are thickned in the correct way
			//----------------------------------------------------
			forall(bs, border_steps_to_be_thickned)
				DD_increase_thickness_of_border_step(opposite_border_step(bs));
			border_steps_to_be_thickned.clear();
			
			bend_type = "";
			
			vx1   = vx2;
			aux_n = vx2;
			ev = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _090));
			if (ev == NULL_EDGE) ev = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _180));
			}
			
		else if (DD.map.is_marked(me,RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_BACK_AND_RIGHT_REAL_EDGES))
			{
			//cout << "\n\t\tRM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_BACK_AND_RIGHT_REAL_EDGES" << flush;
			vx2   = DD.node_of_map_edge[me];
			unmark(vx2,RM3_REPLACES_A_BEND);
			mark(vx2,RM3_BEND_NODE);
			
			//----------------------------------------------------
			// The border_step in list border_steps_to_be_thickned
			// now are thickned in the correct way
			//----------------------------------------------------
			forall(bs, border_steps_to_be_thickned)
				DD_increase_thickness_of_border_step(bs);
			border_steps_to_be_thickned.clear();
			
			bend_type = "";
			
			vx1   = vx2;
			aux_n = vx2;
			ev = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _090));
			}
			
		else if (DD.map.is_marked(me,RM3_DD_TURN_LEFT_ON_NODE))
			{
			//cout << "\n\t\tRM3_DD_TURN_LEFT_ON_NODE" << flush;
						
			// The bend type mustn't be setted
			// nor anyone gdtedge must be thickned!
			bend_type = "L";
			aux_n = DD.node_of_map_edge[me];
//			curr_start_heading = heading_after_rotation(curr_start_heading, _270);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_TURN_RIGHT_ON_NODE))
			{
			//cout << "\n\t\tRM3_DD_TURN_RIGHT_ON_NODE" << flush;
						
			// The bend type mustn't be setted
			// nor anyone gdtedge must be thickned!
			bend_type = "R";
			aux_n = DD.node_of_map_edge[me];
//			curr_start_heading = heading_after_rotation(curr_start_heading, _090);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_TURN_LEFT_ON_NODE_FROM_DUMMY_TO_REAL))
			{
			//cout << "\n\t\tRM3_DD_TURN_LEFT_ON_NODE_FROM_DUMMY_TO_REAL" << flush;
			
			bend_type = "L";
			
			//------------------------
			// New marker for the gdtnode
			//------------------------
			vx2   = DD.node_of_map_edge[me];
			if (is_marked(vx2,RM3_ADDED_BY_RECTANGULARIZATION))
				unmark(vx2,RM3_ADDED_BY_RECTANGULARIZATION);
			//----------------------------------------------------------------
			//			NEW_DUMMY problem
			// This else is to prevent future dangerous situation:
			// if someone adds a marker for a dummy dime gdtedge,
			// in the precedent line it is not considered,
			// but it must be considered!
			// The following lines cause a gdt_error
			// in order to force the correction of code with new dummy marker!
			//----------------------------------------------------------------
			else if (!is_marked(vx2,RM3_BEND_NODE))
				//{
				// Debugging
				//marker_type mt;
				//forall(mt, markers(vx2))
				//	cout << "\n\tnode " << id(vx2) << " is marked " << mt << flush;
				//
				gdt_error("not considered new dummy dime gdtedge marker");
				//}
			mark(vx2,RM3_BEND_NODE);
			
			//-----------------------------------------
			// Creation of dime gdtedge for the dummy path
			//-----------------------------------------
			forall (e, edges_to_be_removed) fx = del_edge(e);
			edges_to_be_removed.clear();
//			gdtnode n;
//			forall (n, nodes_to_be_removed) del_node(n);
//			nodes_to_be_removed.clear();
			
			curr_start_heading = heading_after_rotation(curr_start_heading, DD_angle_of_bends_of_map_edges_before_list_item_starting_from_node(li, vx1, DD));
			ew = find_edge_leaving_node_with_heading(vx2, curr_start_heading);
			
			// The ortho::new_edge is called (in place of the dime one)
			// because the dime::new_edge attempts to preserve headings,
			// which is useless now, since a local_init is required anyway
			// after the rectangularization
			//
			gdtedge aux = orth_plan_undi_graph::new_edge (vx1,ev,vx2,ew);
			
			DD.edge_path.append(aux);
			//cout << "\n\t\tAppended in DD.edge_path the gdtedge " << id(aux) << flush;
			
			ev    = NULL_EDGE;
			fx    = NULL_FACE;
			vx1   = DD.node_of_map_edge[me];
			aux_n = DD.node_of_map_edge[me];
			}
		
		else if (DD.map.is_marked(me,RM3_DD_TURN_RIGHT_ON_NODE_FROM_DUMMY_TO_REAL))
			{
			//cout << "\n\t\tRM3_DD_TURN_RIGHT_ON_NODE_FROM_DUMMY_TO_REAL" << flush;
						
			bend_type = "R";
			
			//------------------------
			// New marker for the gdtnode
			//------------------------
			vx2   = DD.node_of_map_edge[me];
			if (is_marked(vx2,RM3_ADDED_BY_RECTANGULARIZATION))
				unmark(vx2,RM3_ADDED_BY_RECTANGULARIZATION);
			//----------------------------------------------------------------
			//			NEW_DUMMY problem
			// This else is to prevent future dangerous situation:
			// if someone adds a marker for a dummy dime gdtedge,
			// in the precedent line it is not considered,
			// but it must be considered!
			// The following lines cause a gdt_error
			// in order to force the correction of code with new dummy marker!
			//----------------------------------------------------------------
			else if (!is_marked(vx2,RM3_BEND_NODE))
				//{
				// Debugging
				//marker_type mt;
				//forall(mt, markers(vx2))
				//	cout << "\n\tnode " << id(vx2) << " is marked " << mt << flush;
				//
				gdt_error("not considered new dummy dime gdtedge marker");
				//}
			mark(vx2,RM3_BEND_NODE);
			
			//-----------------------------------------
			// Creation of dime gdtedge for the dummy path
			//-----------------------------------------
			forall (e, edges_to_be_removed) fx = del_edge(e);
			edges_to_be_removed.clear();
//			gdtnode n;
//			forall (n, nodes_to_be_removed) del_node(n);
//			nodes_to_be_removed.clear();
			
			curr_start_heading = heading_after_rotation(curr_start_heading, DD_angle_of_bends_of_map_edges_before_list_item_starting_from_node(li, vx1, DD));
			ew = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _180));
			if (ew == NULL_EDGE) ew = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _270));
			if (ew == NULL_EDGE) ew = find_edge_leaving_node_with_heading(vx2, curr_start_heading);
			
			// The ortho::new_edge is called (in place of the dime one)
			// because the dime::new_edge attempts to preserve headings,
			// which is useless now, since a local_init is required anyway
			// after the rectangularization
			//
			gdtedge aux = orth_plan_undi_graph::new_edge (vx1,ev,vx2,ew);
			
			DD.edge_path.append(aux);
			//cout << "\n\t\tAppended in DD.edge_path the gdtedge " << id(aux) << flush;
			
			ev    = NULL_EDGE;
			fx    = NULL_FACE;
			vx1   = DD.node_of_map_edge[me];
			aux_n = DD.node_of_map_edge[me];
			}
		
		else if (DD.map.is_marked(me,RM3_DD_TURN_LEFT_ON_NODE_FROM_REAL_TO_DUMMY))
			{
			//cout << "\n\t\tRM3_DD_TURN_LEFT_ON_NODE_FROM_REAL_TO_DUMMY" << flush;
			
			//-----------------------------------
			// Thicks the border_step yet in list
			//-----------------------------------
			forall (bs, border_steps_to_be_thickned) DD_increase_thickness_of_border_step(opposite_border_step(bs));
			border_steps_to_be_thickned.clear();
			
			bend_type = "";
			
			//------------------------
			// New marker for the gdtnode
			//------------------------
			vx2   = DD.node_of_map_edge[me];
			if (is_marked(vx2,RM3_ADDED_BY_RECTANGULARIZATION))
				unmark(vx2,RM3_ADDED_BY_RECTANGULARIZATION);
			//----------------------------------------------------------------
			//			NEW_DUMMY problem
			// This else is to prevent future dangerous situation:
			// if someone adds a marker for a dummy dime gdtedge,
			// in the precedent line it is not considered,
			// but it must be considered!
			// The following lines cause a gdt_error
			// in order to force the correction of code with new dummy marker!
			//----------------------------------------------------------------
			else if (!is_marked(vx2,RM3_BEND_NODE))
				//{
				// Debugging
				//marker_type mt;
				//forall(mt, markers(vx2))
				//	cout << "\n\tnode " << id(vx2) << " is marked " << mt << flush;
				//
				gdt_error("not considered new dummy dime gdtedge marker");
				//}
			mark(vx2,RM3_BEND_NODE);
			
	                ev = find_edge_leaving_node_with_heading(vx2, curr_start_heading); // the opposite gdtedge of the currently walking gdtedge
			if (ev == NULL_EDGE) ev = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _090));
			if (ev == NULL_EDGE) ev = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _180)); // the currently walking gdtedge
//			curr_start_heading = heading_after_rotation(curr_start_heading, _270);
			vx1   = DD.node_of_map_edge[me];
			aux_n = DD.node_of_map_edge[me];
			}
		
		else if (DD.map.is_marked(me,RM3_DD_TURN_RIGHT_ON_NODE_FROM_REAL_TO_DUMMY))
			{
			//cout << "\n\t\tRM3_DD_TURN_RIGHT_ON_NODE_FROM_REAL_TO_DUMMY" << flush;
			
			//-----------------------------------
			// Thicks the border_step yet in list
			//-----------------------------------
			forall (bs, border_steps_to_be_thickned) DD_increase_thickness_of_border_step(bs);
			border_steps_to_be_thickned.clear();
			
			bend_type = "";
			
			//------------------------
			// New marker for the gdtnode
			//------------------------
			vx2   = DD.node_of_map_edge[me];
			if (is_marked(vx2,RM3_ADDED_BY_RECTANGULARIZATION))
				unmark(vx2,RM3_ADDED_BY_RECTANGULARIZATION);
			//----------------------------------------------------------------
			//			NEW_DUMMY problem
			// This else is to prevent future dangerous situation:
			// if someone adds a marker for a dummy dime gdtedge,
			// in the precedent line it is not considered,
			// but it must be considered!
			// The following lines cause a gdt_error
			// in order to force the correction of code with new dummy marker!
			//----------------------------------------------------------------
			else if (!is_marked(vx2,RM3_BEND_NODE))
				//{
				// Debugging
				//marker_type mt;
				//forall(mt, markers(vx2))
				//	cout << "\n\tnode " << id(vx2) << " is marked " << mt << flush;
				//
				gdt_error("not considered new dummy dime gdtedge marker");
				//}
			mark(vx2,RM3_BEND_NODE);
			
			ev    = DD.edge_of_map_edge[DD.map_edge_path.inf(DD.map_edge_path.pred(li))];
//			curr_start_heading = heading_after_rotation(curr_start_heading, _090);
			vx1   = DD.node_of_map_edge[me];
			aux_n = DD.node_of_map_edge[me];
			}
		
		//else cout << "\n\t\tNO marker" << flush;
		
		// At this point the cases of curr_e == NULL_EDGE are only the cases
		// of not marked map gdtedge and gdtedge corresponding to just born dime gdtedge;
		//    for the first case no action must be performed and no new value to gdtnode aux_n is to be assigned;
		//    for the second case curr_e is set to NULL_EDGE just to avoid this assignment
		//        and to assign it according the needs of the specified marker
		}
	forall (e, edges_to_be_removed) fx = del_edge(e);
	edges_to_be_removed.clear();
//	gdtnode n;
//	forall (n, nodes_to_be_removed) del_node(n);
//	nodes_to_be_removed.clear();
	
	// -------------------
	// create the new gdtedge
	// -------------------
	
	if (ev != NULL_EDGE)
		{
		gdtedge aux = NULL_EDGE;
		vx2 = DD.sink_terminal();

//		cout << "\nLAST:\tv1 = " << id(vx1) << flush;
//		cout << "\nLAST:\tev = " << id(ev) << flush;
//		cout << "\nLAST:\tcurr_start_heading before = " << curr_start_heading << flush;
		curr_start_heading = heading_after_rotation(curr_start_heading, DD_angle_of_bends_of_map_edges_before_list_item_starting_from_node(DD.map_edge_path.last(), vx1, DD));
//		cout << "\nLAST:\tcurr_start_heading after  = " << curr_start_heading << flush;
		                     ew = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _270));
		if (ew == NULL_EDGE) ew = find_edge_leaving_node_with_heading(vx2, curr_start_heading);
		if (ew == NULL_EDGE) ew = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _090)); 
		if (ew == NULL_EDGE) ew = find_edge_leaving_node_with_heading(vx2, heading_after_rotation(curr_start_heading, _180));
//		cout << "\nLAST:\tv2 = " << id(vx2) << flush;
//		cout << "\nLAST:\tew = " << id(ew) << flush;
				
		// The ortho::new_edge is called (in place of the dime one)
		// because the dime::new_edge attempts to preserve headings,
		// which is useless now, since a local_init is required anyway
		// after the rectangularization
		//
		aux = orth_plan_undi_graph::new_edge (vx1,ev,vx2,ew);
		
//		if (fx) aux = new_edge(vx1,DD.vt2,fx);	
		DD.edge_path.append(aux);
		//cout << "\n\t\tAppended in DD.edge_path the gdtedge " << id(aux) << flush;
			
		DD_init_map_node_set (DD.map_nodes_of_edge[aux], NULL_NODE, NULL_NODE, NULL_NODE, NULL_NODE, north);	//?!????
		}
	//--------------------------------------------------------------------------------
	// If the list of border_step to be thickned is not empty,
	//  it means that some WALK_ALONG are encountered, but no one WALK_AND_TURN.
	//  Therefore this is the case of a walk along on real followed by a walk along on dummy.
	//  If a bend was crossed then the type of thickness is stored but not applied.
	//  Now the thickness are applied.
	//--------------------------------------------------------------------------------
	forall (bs, border_steps_to_be_thickned)
		if (bend_type == "R")
		       	DD_increase_thickness_of_border_step(bs);
	       	else if (bend_type == "L")
		       	DD_increase_thickness_of_border_step(opposite_border_step(bs));
	       	else	// arbitrary choice
		       	DD_increase_thickness_of_border_step(opposite_border_step(bs));
       	border_steps_to_be_thickned.clear();
	
	frozen_layout = frozen_layout_backup;
	if (!frozen_layout) local_init(SLOW_COMPACTION);
	
	//cout << "\nEnd   build_dime_edge_path" << flush;
	
	//cout << "\nStart print of build_dime_edge_path" << flush;
	//forall(e, DD.edge_path)
	//	cout << "\n\tEdge " << id(e) << flush <<
	//	        " from gdtnode " << id(source(e)) << flush <<
	//	        " to gdtnode "  << id(target(e)) << flush <<
	//	        //" with r thick " << right_thickness_of_edge(e, source(e)) << flush <<
	//	        //" with l thick " << right_thickness_of_edge(e, target(e)) <<
	//		flush;
	//cout << "\nEnd   print of build_dime_edge_path" << flush;
	
	//cout << "\n\tcost of new gdtedge = " << cost << flush;
	}
	
	
	
	
	// DD_dynamic_new_edge subroutine
	// Increase the thickness of the specified dime gdtedge under specified condition
	//
	void
	dime_orth_plan_undi_graph::
DD_increase_thickness_of_border_step
	(
	border_step bs
	)
	{
	// The thickness is a property of border_step, not of gdtedge;
	// therefore it must be chosen the border_step to increase.
	// While constructing the map, for the bending edges
	//   is stored in a string the direction (L or R) of the bend
	//   according the direction of minimal cost path (from source terminal to sink);
	//   therefore it is sufficient read this value and choose the correct border step along e	
	
	//cout << "\n\t\tStart of DD_increase_thickness_of_border_step from gdtnode " << id(start_of_border_step(bs)) <<
	//	" along gdtedge " << id(edge_of_border_step(bs)) << flush;
	set_thickness_of_border_step(bs, thickness_of_border_step(bs)+1);
	//cout << "\n\t\tEnd   of DD_increase_thickness_of_border_step" << flush;
	}
	
	
	
	// DD_dynamic_new_edge subroutine
	// Create a pair of map-gdtedge between two given border_step of the same face
	//
	void
	dime_orth_plan_undi_graph::
DD_create_map_edges_between_border_steps
	(
	border_step  bs1,
	border_step  bs2,
	DD_struct&   DD
	)
	{
	short bends;       // number of bends  (for map-gdtedge cost calculation)
	int   length;      // total length     (for map-gdtedge cost calculation)
	int   length_full; // length units
	int   length_half; // length half-units
 	int   idis;        // inline    distance between e1 and e2
	int   tdis;        // traversal distance between e1 and e2	
	bool  ordered;     // map-gdtedge from the current dummy-gdtnode to the current dime-gdtedge corresponds to the face rotation versus 
	
	face f  = face_of_border_step(bs1); // NOTE: it is assumed that bs1 and bs2 belong to the same face f
	gdtedge e1 = edge_of_border_step(bs1);
	gdtedge e2 = edge_of_border_step(bs2);
	
	heading_type h1 = initial_heading_of_border_step(bs1);
	heading_type h2 = initial_heading_of_border_step(bs2);
	
	gdtnode mn1 = DD_node_of_map_node_set_with_heading (DD.map_nodes_of_edge[e1], heading_after_rotation (h1,_090));
	gdtnode mn2 = DD_node_of_map_node_set_with_heading (DD.map_nodes_of_edge[e2], heading_after_rotation (h2,_090));
	
	gdtedge me12 = DD.map.new_edge(mn1,mn2);
	gdtedge me21 = DD.map.new_edge(mn2,mn1);
	
	if(h1 == h2)
		{
		// -----------------------------------------
		// e1 and e2 are on the SAME SIDE of face f
		// ----------------------------------------
		// NOTE: length and bends number calculation
		// is the same for external and internal faces
		//
		//
		//          ******************
		//     f    *                *
		//          *                *    <--bs
		// o----o===e1===o------o===e2===o-----o
		// |                                   | h1 = west
		// |                                   | h2 = west
		// |                                   |
		// |                                   |
		// |                                   |
		// o-----------------------------------o
		//
		// o-----------------------------------o
		// |                                   | h1 = west
		// |   f                               | h2 = west
		// |        ******************         |
		// |        *                *         |
		// |        *                *   <--bs |
		// o----o===e1===o------o===e2===o-----o
		//
		bends       = 2;
		length_full = min_border_distance(e1,e2,f,ordered) + 2;
		length_half = length_of_edge(e1)+length_of_edge(e2);
		DD.bends_of_map_edge[me12]="RR";
		DD.bends_of_map_edge[me21]="LL";
		}
	
	else if (h1 == heading_after_rotation(h2,_180))
		{
		// -----------------------------------------
		// e1 and e2 are on OPPOSITE SIDES of face f
		// -----------------------------------------
		//
		if(f == external_face())
			{
			//               **************************
			//      f        *                        *
			//         <--bs *                        *
			//  o--------o===e1===o--------------o    *    h1 = west
			//  |                                |    *    h2 = est
			//  |                                |    *
			//  |                                |    *
			//  o---------o===e2===o-------------o    *
			//         bs-->  *                       *
			//                *                       *
			//                *************************
			//
			bends       = 4;
			length_full = min_border_distance(e1,e2,f,ordered) + 6;
			length_half = length_of_edge(e1)+length_of_edge(e2);
			DD.bends_of_map_edge[me12]="RRRR";
			DD.bends_of_map_edge[me21]="LLLL";
			}
		else
			{
			idis = inline_distance(e1,e2);
			tdis = traversal_distance(e1,e2);
			if(idis>=0)
				{
				// The projection of e1 on the side of e2 falls outside e2, therefore 2 bends might be required.
				// However, some gdtedge stretching is possible during the rectangularization
				// aligning e1 and e2 in order to avoid any bend. Therefore, we prefer here
				// NOT to assign any bend-cost.
				//
				//  o--------o===e2===o--------------o
				//  |            *                   |  h1 = west
				//  |   f        *                   |  h2 = east
				//  |            ?************?      |
				//  |                         *      |
				//  |                   <--bs *      |
				//  o--------------------o===e1===o--o
				//
				bends       = 0;
				length_full = idis + tdis;
				length_half = length_of_edge(e1)+length_of_edge(e2);
				DD.bends_of_map_edge[me12]="LR";
				DD.bends_of_map_edge[me21]="LR";
				}
			else
				{ 
				// The projection of e1 on the side of e2
				// falls inside e2 (no bends required)
				//
				//  o--------o===e2===o--------------o
				//  |            *  bs-->            |  h1 = west
				//  |            *                   |  h2 = east
				//  |            *                   |
				//  |            *                   |
				//  |      <--bs *                   |
				//  o--------o===e1===o--------------o
				//
				bends       = 0;
				length_full = tdis;
				length_half = 0;
				DD.bends_of_map_edge[me12]="";
				DD.bends_of_map_edge[me21]="";
				}
			}
		}
	else
		{
		// -----------------------------------------
		// e1 and e2 are on ADJACENT SIDES of face f
		// -----------------------------------------
		//
		if(f == external_face())
			{
			//    f     *************
			//          *           *
			// o---o===e1===o---o   *
			// |                |   *   h1 = west
			// |                o   *   h2 = north
			// |                #   *
			// |                #   *
			// |             e2 #****
			// |                #  +
			// |                #  |
			// |                o
			// |                |
			// o----------------o
			//
			// **************      f
			// *            *
			// *   o---o===e1===o---o
			// *   |                |   h1 = west
			// *   o                |   h2 = south
			// *   #                |
			// *   #                |
			// ****# e2             |
			//     #                |
			//  |  #                |
			//  +  o                |
			//     |                |
			//     o----------------o
			//
			bends       = 3;
			length_full = min_border_distance(e1,e2,f,ordered)+4;
			length_half = length_of_edge(e1)+length_of_edge(e2);
			if (h1 == heading_after_rotation (h2,_090))
			   {
			   DD.bends_of_map_edge[me12]="LLL";
			   DD.bends_of_map_edge[me21]="RRR";
			   }
			else
			   {
			   DD.bends_of_map_edge[me12]="RRR";
			   DD.bends_of_map_edge[me21]="LLL";
			   }
			}
		else
			{
			// o----------------o
			// |                |   h1 = west
			// |       f        o   h2 = south
			// |                #
			// |                #
			// |        ********# e2
			// |        *     | #
			// |        *     + #
			// |        *       o
			// |        *       |
			// o---o===e1===o---o
			//
			// o----------------o
			// |                |   h1 = west
			// o       f        |   h2 = north
			// # e2             |
			// #                |
			// #********        |
			// # +     *        |
			// # |     *        |
			// o       *        |
			// |       *        |
			// o---o===e1===o---o
			//
			bends       = 1;
			length_full = min_border_distance(e1,e2,f,ordered);
			length_half = length_of_edge(e1)+length_of_edge(e2);
			if (h1 == heading_after_rotation (h2,_090))
			   {
			   DD.bends_of_map_edge[me12]="R";
			   DD.bends_of_map_edge[me21]="L";
			   }
			else
			   {
			   DD.bends_of_map_edge[me12]="L";
			   DD.bends_of_map_edge[me21]="R";
			   }
			}
		}
	
	// All lengths are doubled everywere in the map
	// because the min-cost-flow costs must be integer
	// and it is not possible di specify a 1/2 cost
	//
	length = length_full*2 + length_half;
	
	DD.map_edge_cost[me12] = DD.length_unit_cost*length + DD.bend_cost*2*bends;
	DD.map_edge_cost[me21] = DD.length_unit_cost*length + DD.bend_cost*2*bends;
	
	DD.face_of_map_edge[me12] = f;
	DD.face_of_map_edge[me21] = f;
	
	DD.map.mark(me12,RM3_DD_CROSS_ON_FACE);
	DD.map.mark(me21,RM3_DD_CROSS_ON_FACE);
	}
	
	
	
	
	// DD_dynamic_new_edge subroutine
	// Create a pair of map-gdtedge between two dummy-nodes along the sides of a face
	//
	void
	dime_orth_plan_undi_graph::
DD_create_map_edges_between_start_node_of_border_steps
	(
	border_step     bs1,               // border_step whose dime start-gdtnode v1 owns the first map_node to be connected 
 	heading_type    hmn1,              // heading determining which map-gdtnode must be connected, among the map-nodes associated to v1
	border_step     bs2,               // border_step whose dime start-gdtnode v2 owns the second map_node to be connected
	heading_type    hmn2,              // heading determining which map-gdtnode must be connected, among the map-nodes associated to v2
	DD_struct&	DD
	)
	{
	short bends;       // number of bends  (for map-gdtedge cost calculation)
	int   length;      // total length     (for map-gdtedge cost calculation)
	int   length_full; // length units
	int   length_half; // length half-units
 	int   idis;        // inline    distance between v1 and v2
	int   tdis;        // traversal distance between v1 and v2
	bool  ordered;     // map-gdtedge from the current dummy-gdtnode to the current dime-gdtedge corresponds to the face rotation versus 
	
	face f  = face_of_border_step (bs1); // NOTE: it is assumed that bs1 and bs2 belong to the same face f
	gdtnode v1 = start_of_border_step(bs1); // NOTE: it is assumed v1 is NOT a corner of face f
	gdtnode v2 = start_of_border_step(bs2); // NOTE: it is assumed v2 is NOT a corner of face f
	
	//cout << "\nCreating map-gdtedge betwen gdtnode " << id(v1) << "(h" << hmn1 << ") and gdtnode " << id(v2)  << "(h" << hmn2 << ")" << flush;
	
	gdtedge e1 = edge_of_border_step(bs1);
	//gdtedge e2 = edge_of_border_step(bs2);
	
	map_node_set mns1 = DD.map_nodes_of_node[v1];
	map_node_set mns2 = DD.map_nodes_of_node[v2];
	
	gdtnode mn1 = DD_node_of_map_node_set_with_heading (mns1, hmn1);
	gdtnode mn2 = DD_node_of_map_node_set_with_heading (mns2, hmn2);
	
	// Added by F.M. on June 17 1999
	//
	// In 4pl case these two gdtnode always exist.
	// In >4pl case not only the crossable nodes have the map subnet and then
	// there be nodes with subnet to allow to across the gdtnode;
	// therefore these nodes can have a dummy gdtedge around without map gdtnode;
	// for these gdtnode along that heading there be not any map gdtnode and then
	// no map gdtedge from there must exist.
	if (mn1 && mn2)
		{
		gdtedge me12 = DD.map.new_edge(mn1,mn2);
		gdtedge me21 = DD.map.new_edge(mn2,mn1);
		
		if(hmn1 == hmn2)
			{
			// ----------------------------------------
			// v1 and v2 are on the SAME SIDE of face f
			// ----------------------------------------
			// NOTE: length and bends number calculation
			// is the same for external and internal faces
			//
			//
			//          ******************
			//     f    *                *
			//          *                *    <--bs
			// o--------v1-------o-------v2--------o
			// |                                   | hmn1 = north
			// |                                   | hmn2 = north
			// |                                   |
			// |                                   |
			// |                                   |
			// o--------o-----------------o--------o
			//
			// o-----------------o-----------------o
			// |                                   | hmn1 = north
			// |   f                               | hmn2 = north
			// |        ******************         |
			// |        *                *         |
			// |        *                *   <--bs |
			// o--------v1-------o-------v2--------o
			//
			bends       = 2;
			length_full = min_border_distance(v1,v2,f,ordered) + 2;
			length_half = 0;
			DD.bends_of_map_edge[me12]="RR";
			DD.bends_of_map_edge[me21]="LL";
			}
		
		else if(hmn1 == heading_after_rotation(hmn2,_180))
			{
			// -----------------------------------------
			// v1 and v2 are on OPPOSITE SIDES of face f
			// -----------------------------------------
			//
			if(f == external_face())
				{
				//               **************************
				//      f        *                        *
				//               *                        *
				//  o-----o------v1--------o---------o    *    hmn1 = north
				//  |                                |    *    hmn2 = south
				//  |                                |    *
				//  |                                |    *
				//  o------------v2--------o---------o    *
				//                *                       *
				//                *                       *
				//                *************************
				//
				bends       = 4;
				length_full = min_border_distance(v1,v2,f,ordered) + 6;
				length_half = 0;
				DD.bends_of_map_edge[me12]="RRRR";
				DD.bends_of_map_edge[me21]="LLLL";
				}
			
			else
				{
				slope_type slope;
				slope = slope_of_edge(e1);
				idis  = inline_distance(v1,v2,slope);
				tdis  = traversal_distance(v1,v2,slope);
				if(idis>0)
					{
					// v1 and v2 are NOT aligned, therefore 2 bends might be required.
					// However, some gdtedge stretching is possible during the rectangularization
					// aligning v1 and v2 in order to avoid any bend. Therefore, we prefer here
					// NOT to assign any bend-cost.
					//
					//  o------------v2------------------o
					//  |            *  bs-->            |  hmn1 = north
					//  |   f        *                   |  hmn2 = south
					//  |            ?************?      |
					//  |                         *      |
					//  |                         *      |
					//  o------------------------v1------o
					//
					bends       = 0;
					length_full = idis + tdis;
					length_half = 0;
					DD.bends_of_map_edge[me12]="LR";
					DD.bends_of_map_edge[me21]="LR";
					}
				else
					{ 
					// v1 and v2 are aligned
					// therefore no bends are required
					//
					//  o------------v2------------------o
					//  |            *  bs-->            |  hmn1 = north
					//  |            *                   |  hmn2 = south
					//  |            *                   |
					//  |            *                   |
					//  |            *                   |
					//  o------------v1------------------o
					//
					bends       = 0;
					length_full = tdis;
					length_half = 0;
					DD.bends_of_map_edge[me12]="";
					DD.bends_of_map_edge[me21]="";
					}
				}
			}
		
		else if(hmn1 == heading_after_rotation(hmn2,_090))
			{
			// -----------------------------------------
			// v1 and v2 are on ADJACENT SIDES of face f
			// -----------------------------------------
			//
			if(f == external_face())
				{
				//
				// **************      f
				// *            *
				// *   o-------v1-------o
				// *   |                |   hmn1 = north
				// *   |                |   hmn2 = west
				// *   |                |
				// *   |                |
				// ***v2                |
				//     |                |
				//     |                |
				//     |                |
				//     |                |
				//     o----------------o
				//
				bends       = 3;
				length_full = min_border_distance(v1,v2,f,ordered)+4;
				length_half = 0;
				DD.bends_of_map_edge[me12]="LLL";
				DD.bends_of_map_edge[me21]="RRR";
				}
			else
				{
				// o----------------o
				// |                |   hmn1 = north
				// |       f        |   hmn2 = east
				// |                |
				// |                |
				// v2*******        |
				// |       *        |
				// |       *        |
				// |       *        |
				// |       *        |
				// o------v1--------o
				//
				bends       = 1;
				length_full = min_border_distance(v1,v2,f,ordered);
				length_half = 0;
				DD.bends_of_map_edge[me12]="L";
				DD.bends_of_map_edge[me21]="R";
				}
			}
		
		else
			{
			// -----------------------------------------
			// v1 and v2 are on ADJACENT SIDES of face f
			// -----------------------------------------
			//
			if(f == external_face())
				{
				//    f     *************
				//          *           *
				// o-------v1-------o   *
				// |                |   *   hmn1 = north
				// |                |   *   hmn2 = east
				// |                |   *
				// |                |   *
				// |               v2****
				// |                |
				// |                |
				// |                |
				// |                |
				// o----------------o
				//
				//
				bends       = 3;
				length_full = min_border_distance(v1,v2,f,ordered)+4;
				length_half = 0;
				DD.bends_of_map_edge[me12]="RRR";
				DD.bends_of_map_edge[me21]="LLL";
				}
			else
				{
				// o----------------o
				// |                |   hmn1 = north
				// |       f        |   hmn2 = west
				// |                |
				// |        *******v2
				// |        *       |
				// |        *       |
				// |        *       |
				// |        *       |
				// o-------v1-------o
				//
				bends       = 1;
				length_full = min_border_distance(v1,v2,f,ordered);
				length_half = 0;
				DD.bends_of_map_edge[me12]="R";
				DD.bends_of_map_edge[me21]="L";
				}
			}
		
		// All lengths are doubled everywere in the map
		// because the min-cost-flow costs must be integer
		// and it is not possible di specify a 1/2 cost
		//
		length = length_full*2 + length_half;
		
		DD.map_edge_cost[me12] = DD.length_unit_cost*length + DD.bend_cost*2*bends;
		DD.map_edge_cost[me21] = DD.length_unit_cost*length + DD.bend_cost*2*bends;
		
		DD.face_of_map_edge[me12] = f;
		DD.face_of_map_edge[me21] = f;
		
		DD.map.mark(me12,RM3_DD_CROSS_ON_FACE);
		DD.map.mark(me21,RM3_DD_CROSS_ON_FACE);
		}
	}
	
	
	
	
	// DD_dynamic_new_edge subroutine
	// Initializes the nodes of map_node_set with the values of mnx nodes
	//
	void
	dime_orth_plan_undi_graph::
DD_init_map_node_set
	(
	map_node_set& mns,
	gdtnode mn1,
	gdtnode mn2,
	gdtnode mn3,
	gdtnode mn4,
	heading_type hmn1
	)
	{
	switch (hmn1)
		{
		case north:
			mns.north_node = mn1;
			mns.east_node  = mn2;
			mns.south_node = mn3;
			mns.west_node  = mn4;
			break;
		case east:
			mns.east_node  = mn1;
			mns.south_node = mn2;
			mns.west_node  = mn3;
			mns.north_node = mn4;
			break;
		case south:
			mns.south_node = mn1;
			mns.west_node  = mn2;
			mns.north_node = mn3;
			mns.east_node  = mn4;
			break;
		case west:
			mns.west_node  = mn1;
			mns.north_node = mn2;
			mns.east_node  = mn3;
			mns.south_node = mn4;
			break;
		default:
			gdt_error("undefined heading");
			break;
		}
	}
	
	
	
	
	// DD_dynamic_new_edge subroutine
	// Initializes the values of mnx nodes with the nodes of map_node_set
	// Renames the gdtnode of map_node_set with the mnx gdtnode names
	// Added by Francesco Matera
	// as utility for completation of subnet in order to
	// expand the Dynamic Drawing to >4plan problems
	//
	void
	dime_orth_plan_undi_graph::
DD_get_map_node_set
	(
	map_node_set mns,
	gdtnode& mn1,
	gdtnode& mn2,
	gdtnode& mn3,
	gdtnode& mn4,
	heading_type hmn1
	)
	{
	switch (hmn1)
		{
		case north:
			mn1 = mns.north_node;
			mn2 = mns.east_node;
			mn3 = mns.south_node;
			mn4 = mns.west_node;
			break;
		case east:
			mn1 = mns.east_node;
			mn2 = mns.south_node;
			mn3 = mns.west_node;
			mn4 = mns.north_node;
			break;
		case south:
			mn1 = mns.south_node;
			mn2 = mns.west_node;
			mn3 = mns.north_node;
			mn4 = mns.east_node;
			break;
		case west:
			mn1 = mns.west_node;
			mn2 = mns.north_node;
			mn3 = mns.east_node;
			mn4 = mns.south_node;
			break;
		default:
			gdt_error("undefined heading");
			break;
		}
	}
	
	
	
	// DD_dynamic_new_edge utility function
	//
	gdtnode&
	dime_orth_plan_undi_graph::
DD_node_of_map_node_set_with_heading
	(
	map_node_set& mns,
	heading_type hmn
	)
	{
	switch (hmn)
		{
 		case north: return mns.north_node; break;
 		case east : return mns.east_node;  break;
		case south: return mns.south_node; break;
		case west : return mns.west_node;  break;
		default   : gdt_error("invalid heading"); break;
		}
	return mns.north_node; //Just to avoid compiler warnings
	}
	
	
	
	angle_type
	dime_orth_plan_undi_graph::
DD_angle_of_bends_of_map_edge
	(
	gdtedge me, // map gdtedge
	DD_struct& DD
	)
	{
	angle_type delta = _000;
	int number_of_left_bends  = 0;
	int number_of_right_bends = 0;
	string bends = DD.bends_of_map_edge[me];
	int i;
	for (i = 0; i < bends.length(); ++i)
		{
		if (bends[i] == 'L') ++number_of_left_bends;
		else if (bends[i] == 'R') ++number_of_right_bends;
		else gdt_error("invalid bend char");
		}
	number_of_right_bends -= number_of_left_bends;
	while (number_of_right_bends < 0) number_of_right_bends += 4;
	number_of_right_bends = number_of_right_bends % 4;
	switch (number_of_right_bends)
		{
		case 1: return _090;
		case 2: return _180;
		case 3: return _270;
		default: return delta;
		}
	}
	
	
	
	angle_type
	dime_orth_plan_undi_graph::
DD_angle_of_bends_of_map_edges_before_list_item_starting_from_node
	(
	gdt::list_item start_li, // map gdtedge
	gdtnode n, // dime gdtnode
	DD_struct& DD
	)
	{
	angle_type delta;
	int number_of_right_bends = 0;
	gdt::list_item li;
	forall_items (li, DD.map_edge_path)
		{
		if (li == start_li)
			{
			gdt::list_item aux_li = li;
			gdtedge me;
			do
				{
				me = DD.map_edge_path.inf(aux_li);
				delta = DD_angle_of_bends_of_map_edge(me,DD);
				switch (delta)
					{
					case _090: number_of_right_bends += 1; break;
					case _180: number_of_right_bends += 2; break;
					case _270: number_of_right_bends += 3; break;
					default: ;//do nothing
					}
				aux_li = DD.map_edge_path.pred(aux_li);
				}
			while(DD.node_of_map_edge[me] != n);
			break;
			}
		}
	number_of_right_bends = number_of_right_bends % 4;
	switch (number_of_right_bends)
		{
		case 0: return _360;
		case 1: return _090;
		case 2: return _180;
		case 3: return _270;
		}
	return _360; //only to avoid warning. Not reachable statement (in the switch ever one case happens)
	}
	
	
	
	// DD_dynamic_new_edge utility function
	//
	void
	dime_orth_plan_undi_graph::
DD_print_map
	(DD_struct& DD,
	string str)
	{
	gdtnode v;
	gdtedge e;
	gdtedge me;
	map_node_set mns;
	
	ofstream out (str);
	
	cout << "\n" << flush;
	//cout << "\nforall_nodes (v,*this)" << flush;
	forall_nodes (v,*this)
		//if (is_marked(v,RM3_ADDED_BY_RECTANGULARIZATION) || DD.is_a_terminal_node(v))
		{
		//cout << "\n\tnode " << id(v) << flush;
		mns = DD.map_nodes_of_node[v];
		out << "\nmap-nodes on gdtnode " << id(v) << " located at " << position_of_node(v) << flush;
		// A gdtnode may not has the map gdtnode set if this gdtnode has been created from the build_dime_edge
		if (!mns.empty())
			{
			if (mns.north_node)                    out << "\n.north " << DD.map.id(mns.north_node) << flush;
			if (mns.east_node )                    out << "\n.east  " << DD.map.id(mns.east_node ) << flush;
			if (mns.south_node)                    out << "\n.south " << DD.map.id(mns.south_node) << flush;
			if (mns.west_node )                    out << "\n.west  " << DD.map.id(mns.west_node ) << flush;
			if (DD.is_the_source_terminal_node(v)) out << "\ncenter " << DD.map.id(DD.mnt1)        << flush;
			if (DD.is_the_sink_terminal_node  (v)) out << "\ncenter " << DD.map.id(DD.mnt2)        << flush;
			if (DD.is_the_source_terminal_node(v))
				{
				out << "\n\tedge to n " << DD.map.id(DD.map.find_directed_edge(DD.mnt1, mns.north_node)) << flush;
				out << "\n\tedge to e " << DD.map.id(DD.map.find_directed_edge(DD.mnt1, mns.east_node )) << flush;
				out << "\n\tedge to s " << DD.map.id(DD.map.find_directed_edge(DD.mnt1, mns.south_node)) << flush;
				out << "\n\tedge to w " << DD.map.id(DD.map.find_directed_edge(DD.mnt1, mns.west_node )) << flush;
				}
			else if (DD.is_the_sink_terminal_node(v))
				{
				out << "\n\tedge from n " << DD.map.id(DD.map.find_directed_edge(mns.north_node, DD.mnt2)) << flush;
				out << "\n\tedge from e " << DD.map.id(DD.map.find_directed_edge(mns.east_node , DD.mnt2)) << flush;
				out << "\n\tedge from s " << DD.map.id(DD.map.find_directed_edge(mns.south_node, DD.mnt2)) << flush;
				out << "\n\tedge from w " << DD.map.id(DD.map.find_directed_edge(mns.west_node , DD.mnt2)) << flush;
				}
			else
				{
				if (mns.north_node && mns.east_node && DD.map.find_directed_edge(mns.north_node, mns.east_node)) out << "\n\tedge from n to e " << DD.map.id(DD.map.find_directed_edge(mns.north_node, mns.east_node)) << flush;
				if (mns.east_node && mns.north_node && DD.map.find_directed_edge(mns.east_node, mns.north_node)) out << "\n\tedge from e to n " << DD.map.id(DD.map.find_directed_edge(mns.east_node, mns.north_node)) << flush;
				if (mns.east_node && mns.south_node && DD.map.find_directed_edge(mns.east_node, mns.south_node)) out << "\n\tedge from e to s " << DD.map.id(DD.map.find_directed_edge(mns.east_node, mns.south_node)) << flush;
				if (mns.south_node && mns.east_node && DD.map.find_directed_edge(mns.south_node, mns.east_node)) out << "\n\tedge from s to e " << DD.map.id(DD.map.find_directed_edge(mns.south_node, mns.east_node)) << flush;
				if (mns.south_node && mns.west_node && DD.map.find_directed_edge(mns.south_node, mns.west_node)) out << "\n\tedge from s to w " << DD.map.id(DD.map.find_directed_edge(mns.south_node, mns.west_node)) << flush;
				if (mns.west_node && mns.south_node && DD.map.find_directed_edge(mns.west_node, mns.south_node)) out << "\n\tedge from w to s " << DD.map.id(DD.map.find_directed_edge(mns.west_node, mns.south_node)) << flush;
				if (mns.west_node && mns.north_node && DD.map.find_directed_edge(mns.west_node, mns.north_node)) out << "\n\tedge from w to n " << DD.map.id(DD.map.find_directed_edge(mns.west_node, mns.north_node)) << flush;
				if (mns.north_node && mns.west_node && DD.map.find_directed_edge(mns.north_node, mns.west_node)) out << "\n\tedge from n to w " << DD.map.id(DD.map.find_directed_edge(mns.north_node, mns.west_node)) << flush;
				if (mns.north_node && mns.south_node&& DD.map.find_directed_edge(mns.north_node,mns.south_node)) out << "\n\tedge from n to s " << DD.map.id(DD.map.find_directed_edge(mns.north_node, mns.south_node))<< flush;
				if (mns.south_node && mns.north_node&& DD.map.find_directed_edge(mns.south_node,mns.north_node)) out << "\n\tedge from s to n " << DD.map.id(DD.map.find_directed_edge(mns.south_node, mns.north_node))<< flush;
				if (mns.east_node && mns.west_node  && DD.map.find_directed_edge(mns.east_node,  mns.west_node)) out << "\n\tedge from e to w " << DD.map.id(DD.map.find_directed_edge(mns.east_node, mns.west_node))  << flush;
				if (mns.west_node && mns.east_node  && DD.map.find_directed_edge(mns.west_node,  mns.east_node)) out << "\n\tedge from w to e " << DD.map.id(DD.map.find_directed_edge(mns.west_node, mns.east_node))  << flush;
				}
			}
		else
			out << "\nEMPTY MAP NODE SET" << flush;
		}
	
	//cout << "\nforall_edges (e,*this)" << flush;
	forall_edges (e,*this)
		{
		mns = DD.map_nodes_of_edge[e];
		out << "\nmap-nodes on gdtedge " << id(e) << " between nodes " << id(source(e)) << " and " << id(target(e)) \
		    << " with thickness " << thickness_of_edge(e) << flush;
		// An gdtedge may not has the map gdtnode set if this gdtnode has been created from the build_dime_edge
		if (!mns.empty())
			{
			if (mns.north_node) out << "\n.north " << DD.map.id(mns.north_node) << flush;
			if (mns.east_node ) out << "\n.east  " << DD.map.id(mns.east_node ) << flush;
			if (mns.south_node) out << "\n.south " << DD.map.id(mns.south_node) << flush;
			if (mns.west_node ) out << "\n.west  " << DD.map.id(mns.west_node ) << flush;
			
			if (mns.north_node && mns.east_node && DD.map.find_directed_edge(mns.north_node, mns.east_node)) out << "\n\tedge from n to e " << DD.map.id(DD.map.find_directed_edge(mns.north_node, mns.east_node)) << flush;
			if (mns.east_node && mns.north_node && DD.map.find_directed_edge(mns.east_node, mns.north_node)) out << "\n\tedge from e to n " << DD.map.id(DD.map.find_directed_edge(mns.east_node, mns.north_node)) << flush;
			if (mns.east_node && mns.south_node && DD.map.find_directed_edge(mns.east_node, mns.south_node)) out << "\n\tedge from e to s " << DD.map.id(DD.map.find_directed_edge(mns.east_node, mns.south_node)) << flush;
			if (mns.south_node && mns.east_node && DD.map.find_directed_edge(mns.south_node, mns.east_node)) out << "\n\tedge from s to e " << DD.map.id(DD.map.find_directed_edge(mns.south_node, mns.east_node)) << flush;
			if (mns.south_node && mns.west_node && DD.map.find_directed_edge(mns.south_node, mns.west_node)) out << "\n\tedge from s to w " << DD.map.id(DD.map.find_directed_edge(mns.south_node, mns.west_node)) << flush;
			if (mns.west_node && mns.south_node && DD.map.find_directed_edge(mns.west_node, mns.south_node)) out << "\n\tedge from w to s " << DD.map.id(DD.map.find_directed_edge(mns.west_node, mns.south_node)) << flush;
			if (mns.west_node && mns.north_node && DD.map.find_directed_edge(mns.west_node, mns.north_node)) out << "\n\tedge from w to n " << DD.map.id(DD.map.find_directed_edge(mns.west_node, mns.north_node)) << flush;
			if (mns.north_node && mns.west_node && DD.map.find_directed_edge(mns.north_node, mns.west_node)) out << "\n\tedge from n to w " << DD.map.id(DD.map.find_directed_edge(mns.north_node, mns.west_node)) << flush;
			if (mns.north_node && mns.south_node&& DD.map.find_directed_edge(mns.north_node,mns.south_node)) out << "\n\tedge from n to s " << DD.map.id(DD.map.find_directed_edge(mns.north_node, mns.south_node))<< flush;
			if (mns.south_node && mns.north_node&& DD.map.find_directed_edge(mns.south_node,mns.north_node)) out << "\n\tedge from s to n " << DD.map.id(DD.map.find_directed_edge(mns.south_node, mns.north_node))<< flush;
			if (mns.east_node && mns.west_node  && DD.map.find_directed_edge(mns.east_node,  mns.west_node)) out << "\n\tedge from e to w " << DD.map.id(DD.map.find_directed_edge(mns.east_node, mns.west_node))  << flush;
			if (mns.west_node && mns.east_node  && DD.map.find_directed_edge(mns.west_node,  mns.east_node)) out << "\n\tedge from w to e " << DD.map.id(DD.map.find_directed_edge(mns.west_node, mns.east_node))  << flush;
			}
		else
			out << "\nEMPTY MAP NODE SET" << flush;
		}
	
	//cout << "\nforall_edges (me, DD.map)" << flush;
	forall_edges (me, DD.map)
	out << "\nmap-gdtedge " << DD.map.id(me)  << \
		" (" << DD.map.id(DD.map.start(me)) << ")->(" << DD.map.id(DD.map.stop(me))  << \
		") costs " << DD.map_edge_cost[me] << flush;
	
	out << "\nmap-gdtedge shortest path (between dime-nodes " << id(DD.source_terminal()) << " and " << id(DD.sink_terminal()) << ")" << flush;
	forall (me, DD.map_edge_path)
		{
		out << "\nmap-gdtedge " << DD.map.id(me)  << " (" << DD.map.id(DD.map.start(me)) << ")->(" << DD.map.id(DD.map.stop(me))  << ")";
		if (DD.map.is_marked(me,RM3_DD_CROSS_ON_FACE))
			{
			out << "\nCROSS_ON_FACE";
			print(DD.face_of_map_edge[me]);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_CROSS_ON_DUMMY_EDGE))
			{
			out << "\nCROSS_ON_DUMMY_EDGE ";
			print(DD.edge_of_map_edge[me],true);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_WALK_ALONG_DUMMY_EDGE))
			{
			out << "\nWALK_ALONG_DUMMY_EDGE";
			print(DD.edge_of_map_edge[me],true);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_CROSS_ON_REAL_EDGE))
			{
			out << "\nCROSS_ON_REAL_EDGE";
			print(DD.edge_of_map_edge[me],true);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_CROSS_ON_DUMMY_NODE_BETWEEN_DUMMY_EDGES))
			{
			out << "\nCROSS_ON_DUMMY_NODE_BETWEEN_DUMMY_EDGES";
			print(DD.node_of_map_edge[me]);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_CROSS_ON_DUMMY_NODE_BETWEEN_REAL_EDGES))
			{
			out << "\nCROSS_ON_DUMMY_NODE_BETWEEN_REAL_EDGES";
			print(DD.node_of_map_edge[me]);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_WALK_ALONG_REAL_EDGE))
			{
			out << "\nWALK_ALONG_REAL_EDGE";
			print(DD.edge_of_map_edge[me],true);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_WALK_AND_TURN_LEFT_ALONG_DUMMY_EDGE))
			{
			out << "\nWALK_AND_TURN_LEFT_ALONG_DUMMY_EDGE";
			print(DD.edge_of_map_edge[me],true);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_DUMMY_EDGE))
			{
			out << "\nWALK_AND_TURN_RIGHT_ALONG_DUMMY_EDGE";
			print(DD.edge_of_map_edge[me],true);
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_TURN_LEFT_AND_WALK_ALONG_DUMMY_EDGE))
			{
			out << "\nTURN_LEFT_AND_WALK_ALONG_DUMMY_EDGE";
			print(DD.edge_of_map_edge[me],true);
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_DUMMY_EDGE))
			{
			out << "\nTURN_RIGHT_AND_WALK_ALONG_DUMMY_EDGE";
			print(DD.edge_of_map_edge[me],true);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_WALK_AND_TURN_LEFT_ALONG_REAL_EDGE))
			{
			out << "\nWALK_AND_TURN_LEFT_ALONG_REAL_EDGE";
			print(DD.edge_of_map_edge[me],true);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_REAL_EDGE))
			{
			out << "\nWALK_AND_TURN_RIGHT_ALONG_REAL_EDGE";
			print(DD.edge_of_map_edge[me],true);
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_TURN_LEFT_AND_WALK_ALONG_REAL_EDGE))
			{
			out << "\nTURN_LEFT_AND_WALK_ALONG_REAL_EDGE";
			print(DD.edge_of_map_edge[me],true);
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_REAL_EDGE))
			{
			out << "\nTURN_RIGHT_AND_WALK_ALONG_REAL_EDGE";
			print(DD.edge_of_map_edge[me],true);
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_ACROSS_A_NODE))
			{
			out << "\nACROSS_A_NODE";
			print(DD.node_of_map_edge[me]);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_FRONT_AND_LEFT_REAL_EDGES))
			{
			out << "\nRM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_FRONT_AND_LEFT_REAL_EDGES";
			print(DD.node_of_map_edge[me]);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_FRONT_AND_RIGHT_REAL_EDGES))
			{
			out << "\nRM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_FRONT_AND_RIGHT_REAL_EDGES";
			print(DD.node_of_map_edge[me]);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_BACK_AND_LEFT_REAL_EDGES))
			{
			out << "\nRM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_BACK_AND_LEFT_REAL_EDGES";
			print(DD.node_of_map_edge[me]);
			}
		
		else if (DD.map.is_marked(me,RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_BACK_AND_RIGHT_REAL_EDGES))
			{
			out << "\nRM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_BACK_AND_RIGHT_REAL_EDGES";
			print(DD.node_of_map_edge[me]);
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_TURN_LEFT_ON_NODE))
			{
			out << "\nTURN_LEFT_ON_NODE";
			print(DD.node_of_map_edge[me]);
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_TURN_RIGHT_ON_NODE))
			{
			out << "\nTURN_RIGHT_ON_NODE";
			print(DD.node_of_map_edge[me]);
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_TURN_LEFT_ON_NODE_FROM_DUMMY_TO_REAL))
			{
			out << "\nTURN_LEFT_ON_NODE_FROM_DUMMY_TO_REAL";
			print(DD.node_of_map_edge[me]);
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_TURN_RIGHT_ON_NODE_FROM_DUMMY_TO_REAL))
			{
			out << "\nTURN_RIGHT_ON_NODE_FROM_DUMMY_TO_REAL";
			print(DD.node_of_map_edge[me]);
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_TURN_LEFT_ON_NODE_FROM_REAL_TO_DUMMY))
			{
			out << "\nTURN_LEFT_ON_NODE_FROM_REAL_TO_DUMMY";
			print(DD.node_of_map_edge[me]);
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_TURN_RIGHT_ON_NODE_FROM_REAL_TO_DUMMY))
			{
			out << "\nTURN_RIGHT_ON_NODE_FROM_REAL_TO_DUMMY";
			print(DD.node_of_map_edge[me]);
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_WALK_AND_TURN_LEFT_ALONG_EAGLE_BOUNDARY_EDGE))
			{
			out << "\nWALK_AND_TURN_LEFT_ALONG_EAGLE_BOUNDARY_EDGE";
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_WALK_AND_TURN_RIGHT_ALONG_EAGLE_BOUNDARY_EDGE))
			{
			out << "\nWALK_AND_TURN_RIGHT_ALONG_EAGLE_BOUNDARY_EDGE";
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_TURN_LEFT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE))
			{
			out << "\nTURN_LEFT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE";
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_TURN_RIGHT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE))
			{
			out << "\nTURN_RIGHT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE";
			}
		
		/*
					//-----------------------------------
					// no walk on REPLACES_A_BEND problem
					//-----------------------------------
		else if	(DD.map.is_marked(me,RM3_DD_TURN_LEFT_ON_A_REPLACES_A_BEND_NODE))
			{
			out << "\nTURN_LEFT_ON_A_REPLACES_A_BEND_NODE";
			print(DD.node_of_map_edge[me]);
			}
		
		else if	(DD.map.is_marked(me,RM3_DD_TURN_RIGHT_ON_A_REPLACES_A_BEND_NODE))
			{
			out << "\nTURN_RIGHT_ON_A_REPLACES_A_BEND_NODE";
			print(DD.node_of_map_edge[me]);
			}
		*/
		
		
		else
			{
			out << "\nUNMARKED";
			}
		out << "\nCost = " << DD.map_edge_cost[me] << "\n\n" << flush;
		}
	
	out.close();
	}
	
	
	
	bool
	dime_orth_plan_undi_graph::
edge_belongs_to_node
	(gdtedge e,
	gdtnode v)
	{
	gdt::gdtlist<gdtedge> el;
	el = adj_edges(v);
	gdtedge ei;
	forall(ei, el) if (ei==e) return true;
	return false;
	}


/*
	//==================================================================================================================================
	//                                                      DELTA CONSIDERATIONS
	//==================================================================================================================================
	//
	// All the following are based on a rectangular face with its gdtedge numbered from 1 to 4 from north to west.
	// All the theorical possible cases of tho nodes on the edges of the face are described by th matrix A:
	//
	//     11 12 13 14                               11 12 13 14
	// A = 21 22 23 24                           B =    22 23 24
	//     31 32 33 34                                     33 34
	//     41 42 43 44                                        44
	//
	// The symmetry of this matrix suggests that the under diagonal triangolar part represents duplicated cases.
	// Only the matrix B have to be considered.
	// Suppose that the cases obtained rotating a case are equivalent among themselves.
	// From the ten cases of the matrix B:
	//     11 22 33 44  are equivalent
	//     12 23 34 14  are equivalent
	//     13 24        are equivalent
	// only three case have to be considered 11, 12, 13
	//
	// The two nodes on the face have a name: the first is N1, the second N2
	// The three cases above are insufficient to describe all the possible situations.
	// For each case there are two possible configurations different only for the order of the two nodes:
	//
	// case 11:   +-N1--N2-+   +-N2--N1-+     case 12:   +---N1---+   +---N2---+     case 13:   +---N1---+   +---N2---+
	//            |        |   |        |                |        N2  |        N1               |        |   |        |
	//            +--------+   +--------+                +--------+   +--------+                +---N2---+   +---N1---+
	//
	// For the cases related to 13, rotating the first configuration the second one is obtained
	// and then only five cases are to be considered.
	//
	// The noe N1 is the start gdtnode of a new gdtedge e, the gdtnode N2 is the stop gdtnode of the same new gdtedge e.
	// Of this new gdtedge is of interest to know the angular clockwise difference delta from the starting heading of e and the ending one.
	// Call every bend of the gdtedge to the right R and to the left L.
	// To know delta is irrelevant every sequence of bends with result 0 (i.e. LR, RL, LLLL, RRRR, ....).
	// The sequences obtained adding and/or removing 0-sequences are equivalent among themselves.
	// The sequences obtained changing the order of its bends are all equivalent among themselves.
	// In particular the sequences:
	//    LL and RR are delta-equivalent (LL = LL + LR + LR = LLLRLR = LLLLRR = LLLL + RR = RR)
	//    R and LLL are delta-equivalent (R = R + LLLL = RLLLL = RL + LLL = LLL)
	// These allows to consider only edges with a minimal bends to connect N1 to N2 to know delta.
	// For the case 13 in every situation delta = 0 
	// For the case 11 in every situation delta = 180 (LL o RR)
	// For the case 12:
	//   for the first configuration delta = 270 (L o RRR)
	//   for the second configuration delta = 90 (R o LLL)
	//
	// In conclusion only four cases have to be considered to know delta
	//
	//   delta = 0         delta = 90        delta = 180       delta = 270
	//
	//   +--------+        +---N2---+        +-N1--N2-+        +---N1---+
	//   |        |        |   |    |        | |   |  |        |   |    |
	//   N1=======N2       |   +====N1       | +===+  |        |   +====N2
	//   |        |        |        |        |        |        |        |
	//   +--------+        +--------+        +--------+        +--------+
	//
	//==================================================================================================================================
*/
