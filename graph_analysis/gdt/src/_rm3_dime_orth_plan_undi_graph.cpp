/*******************************************************************************
+
+  _rm3_dime_orth_plan_undi_graph.c
+
+  This file is part of GDToolkit. It can be 
+  used free of charge in academic research and teaching. 
+  Any direct or indirect commercial use of this software is illegal 
+  without a written authorization of the Dipartimento di Informatica e Automazione
+  Universita' di Roma Tre, Roma, ITALIA
+  
+
*******************************************************************************/

#include <assert.h>
//#include <math.h>
#include <GDT/rm3_undi_graph.h>
#include <GDT/rm3_dire_graph.h>
#include <GDT/rm3_plan_undi_graph.h>
#include <GDT/rm3_orth_plan_undi_graph.h>
#include <GDT/rm3_upwa_plan_undi_graph.h>
#include <GDT/rm3_dime_orth_plan_undi_graph.h>
#include <GDT/rm3_draw_undi_graph.h>
#include <GDT/gdt_error.h>

//using namespace std;

// temporary
//#include <GDT/rm3_draw_undi_graph.h>

bool check_find_edge(const undi_graph& ug) {
	gdtedge e;
	bool b = true;
	forall_edges(e, ug) {
		b = b && (ug.find_edge(ug.id(e)) == e);
		if (!b) break;
	}
	return b;
}
//


// The following lines are suggested to compile with Watcom
// but do not compile with GNU.
//
// class edge_info;
// class node_info;
// inline void Print(const edge_info& x, ostream& out) { out << *((int *)((void *)&x)); }
// inline void Print(const node_info& x, ostream& out) { out << *((int *)((void *)&x)); }
// inline void Read(node_info& x, istream& in) { in >> *((int *)((void *)&x)); }
// inline void Read(edge_info& x, istream& in) { in >> *((int *)((void *)&x)); }

//-----------------------------------------------------------------------------
// dime_orth_plan_undi_graph:
// 	base class for all dimensioned planar undirected graphs
// 	with planar embedding, orthogonal shape,
//	orientation (north, south, east, west)
//	and integer length for each gdtedge.
//
// W.Didimo, A.Leonforte (1996)
//-----------------------------------------------------------------------------

# define min(a,b) ((a)>(b) ? (b) : (a))
# define max(a,b) ((a)>(b) ? (a) : (b))



//-----------------------------------------------------------------------------
// struct_dime_border_step_info
//----------------------------------------------------------------------------- 

	struct_dime_border_step_info::
struct_dime_border_step_info
	()
	{
	initial_heading = undefined_heading; 
	}

//-----------------------------------------------------------------------------
// dime_orth_plan_undi_graph 
//-----------------------------------------------------------------------------

	dime_orth_plan_undi_graph::
dime_orth_plan_undi_graph
	()
	{
	//cout << "\n dime_orth_plan_undi_graph () creator" << flush;
	local_new();
	local_init();
	}
	


	dime_orth_plan_undi_graph::
~dime_orth_plan_undi_graph
	()
	{
	local_del();
	}
	
	

	dime_orth_plan_undi_graph::
dime_orth_plan_undi_graph
	(const undi_graph& ug, algorithm_type alg)
	{
	//cout << "\n dime_orth_plan_undi_graph (undi_graph) creator" << flush;
	int num_irr_faces;
	construct_dime_orth_plan_undi_graph (ug,alg,num_irr_faces);
	}


	dime_orth_plan_undi_graph::
dime_orth_plan_undi_graph
	(const undi_graph& ug, algorithm_type alg, int& num_irr_faces)
	{
	//cout << "\n dime_orth_plan_undi_graph (undi_graph) creator" << flush;
	construct_dime_orth_plan_undi_graph (ug,alg,num_irr_faces);
	}


	void
	dime_orth_plan_undi_graph::
construct_dime_orth_plan_undi_graph
	(const undi_graph& ug, algorithm_type alg, int& num_irr_faces)
	{
	//cout << "\n construct_dime_orth_plan_undi_graph (undi_graph) creator" << flush;
	heading_type dir = undefined_heading;
	orth_plan_undi_graph::operator=(ug);
	local_new();
	local_init(alg, dir, num_irr_faces);
	}



	
	dime_orth_plan_undi_graph::
dime_orth_plan_undi_graph
	(const plan_undi_graph& pug, algorithm_type alg)
	{
	//cout << "\n dime_orth_plan_undi_graph (plan_undi_graph) creator" << flush;
	int num_irr_faces;
	construct_dime_orth_plan_undi_graph (pug,alg,num_irr_faces);
	}


	dime_orth_plan_undi_graph::
dime_orth_plan_undi_graph
	(const plan_undi_graph& pug, algorithm_type alg, int& num_irr_faces)
	{
	//cout << "\n dime_orth_plan_undi_graph (plan_undi_graph) creator" << flush;
	construct_dime_orth_plan_undi_graph (pug , alg, num_irr_faces);
	}

	
	void
	dime_orth_plan_undi_graph::
construct_dime_orth_plan_undi_graph
	(const plan_undi_graph& pug, algorithm_type alg, int& num_irr_faces)
	{
	//cout << "\n construct_dime_orth_plan_undi_graph (plan_undi_graph) creator" << flush;
	heading_type dir = undefined_heading;
	orth_plan_undi_graph::operator=(pug);
	local_new();
	local_init(alg, dir, num_irr_faces);
	}


	
	
	dime_orth_plan_undi_graph::
dime_orth_plan_undi_graph
	(const orth_plan_undi_graph& opug, algorithm_type alg)
	{
	//std::cout << "\n dime_orth_plan_undi_graph (orth_plan_undi_graph) creator" << std::endl;
	int num_irr_faces;
	heading_type dir = undefined_heading;
	construct_dime_orth_plan_undi_graph (opug, alg, dir, num_irr_faces);
	}


		
	dime_orth_plan_undi_graph::
dime_orth_plan_undi_graph
	(
	const orth_plan_undi_graph& opug, 
	algorithm_type alg,
	heading_type dir
	)
	{
	//cout << "\n dime_orth_plan_undi_graph (orth_plan_undi_graph) creator" << flush;
	int num_irr_faces;
	construct_dime_orth_plan_undi_graph (opug, alg, dir, num_irr_faces);
	}
	

	dime_orth_plan_undi_graph::
dime_orth_plan_undi_graph
	(
	const orth_plan_undi_graph& opug, 
	algorithm_type alg,
	heading_type dir,
	int& num_irr_faces
	)
	{
	//cout << "\n dime_orth_plan_undi_graph (orth_plan_undi_graph) creator" << flush;
	construct_dime_orth_plan_undi_graph (opug, alg, dir, num_irr_faces);
	}


	void
	dime_orth_plan_undi_graph::
construct_dime_orth_plan_undi_graph
	(const orth_plan_undi_graph& opug, algorithm_type alg, heading_type dir, int& num_irr_faces)
	{
	//cout << "\n construct_dime_orth_plan_undi_graph (orth_plan_undi_graph) creator" << flush;
	orth_plan_undi_graph::operator=(opug);
	local_new();
	local_init(alg, dir, num_irr_faces);
	
	// compare the source and target of all opug edges with this edges
	// 
	gdtedge e;
	forall_edges (e,opug) 
		{
		gdtedge e1 = corresponding_edge(e,opug);
		if (id(source(e1)) != opug.id(opug.source(e))) 	
			{
			//cout << "\nWARNING, SET_SOURCE DIME(ORTH)\n\n" << flush;
			set_source (e1,target(e1)); 
			}
		}
	}

	
	dime_orth_plan_undi_graph::
dime_orth_plan_undi_graph
	(const dime_orth_plan_undi_graph& dopug)
	{
	//cout << "\n dime_orth_plan_undi_graph (dime_orth_plan_undi_graph) creator" << flush;
	local_new(); // required because operator= calls renew()
	operator=(dopug);
	}
		
	

	dime_orth_plan_undi_graph&
	dime_orth_plan_undi_graph::
operator=
	(const undi_graph& ug)
	{
	//cout << "\n undi_graph -(copy)-> dime_orth_plan_undi_graph" << flush;
	orth_plan_undi_graph::operator=(ug);
	local_renew();
	local_init();
	return *this;
	}



	dime_orth_plan_undi_graph& 
	dime_orth_plan_undi_graph::
operator= 
	(const plan_undi_graph& pug)
	{
	//cout << "\n plan_undi_graph -(copy)-> dime_orth_plan_undi_graph" << flush;
	orth_plan_undi_graph::operator=(pug);
	local_renew();
	local_init();
	return *this;
	}



	dime_orth_plan_undi_graph& 
	dime_orth_plan_undi_graph::
operator= 
	(const orth_plan_undi_graph& opug)
	{
	//cout << "\n orth_plan_undi_graph -(copy)-> dime_orth_plan_undi_graph" << flush;
	orth_plan_undi_graph::operator=(opug);
	local_renew();
	local_init();
	
	// compare the source and target of all opug edges with this edges
	// 
	gdtedge e;
	forall_edges (e,opug) 
		{
		gdtedge e1 = corresponding_edge(e,opug);
		if (id(source(e1)) != opug.id(opug.source(e))) 
			{
			//cout << "\nWARNING SET_SOURCE DIME=ORTH\n\n" << flush;
			set_source (e1,target(e1)); 
			}
		}

	return *this;
	}



	dime_orth_plan_undi_graph&
	dime_orth_plan_undi_graph::
operator=
	(const dime_orth_plan_undi_graph& dopug)
	{
	face f;
	gdtnode v,vc;
	gdtedge e,ec;
	border_step s,sc;
	
	//cout << "\n dime_orth_plan_undi_graph -(copy)-> dime_orth_plan_undi_graph" << flush;
	
	orth_plan_undi_graph::operator=(dopug);
	local_renew();
	
	// the dime-part of *this is built
	// according to the corresponding part of dopug.

	frozen_layout 		= dopug.frozen_layout;
	reference_heading 	= dopug.reference_heading;

	forall_nodes (v,dopug)
		{
		vc = corresponding_node (v,dopug);
		set_position_of_node (vc,dopug.position_of_node(v));
		}
	forall_edges (e,dopug)
		{
		v  = dopug.source(e);
		ec = corresponding_edge (e,dopug);
		vc = source(ec);
		set_position_of_bends_along_edge (ec,vc, dopug.position_of_bends_along_edge(e,v));
		}
	forall_faces (f,dopug)
		forall_face_border_steps (s,f)
		{
		sc = corresponding_border_step (s,dopug);
		set_initial_heading_of_border_step (sc, dopug.initial_heading_of_border_step(s));    	     	
		}
	return *this;
	}
	



	// -----------------
	// Access operations
	// -----------------
	
	void
	dime_orth_plan_undi_graph::
local_get
	(
	bool& p1,
	heading_type& p2,
	gdt::gdtnode_map<struct_dime_node_info>*& p3,
	gdt::gdtedge_map<struct_dime_edge_info>*& p4,
	gdt::gdtmap<border_step,struct_dime_border_step_info>*& p5
	)
	{
	p1 = frozen_layout;
	p2 = reference_heading;
	p3 = node_info;
	p4 = edge_info;
	p5 = border_step_info;
	} 	




	heading_type
	dime_orth_plan_undi_graph::
initial_heading_of_border_step
	(border_step s) const
	{
		struct_dime_border_step_info& x = (*border_step_info)[s];
	return x.initial_heading;
	}
	
	
	
	heading_type
	dime_orth_plan_undi_graph::	
heading_after_border_step
	(border_step s) const
	{
	heading_type d;
	d = initial_heading_of_border_step(s);
	d = heading_after_bend_sequence (d, bends_of_border_step(s));
	return heading_after_rotation_around_bend   (d, angle_of_border_step(s));
	}
	
	
	
	
	heading_type
	dime_orth_plan_undi_graph::	
heading_after_inversion
	(heading_type d) const
	{
	return heading_after_rotation_around_bend(d,_360);
	}



	// Heading after a clockwise rotation
	//

	heading_type
	dime_orth_plan_undi_graph::	
heading_after_rotation
	(heading_type d, angle_type a) 
	{
	switch(d)
		{
		case undefined_heading: return undefined_heading;
		case north: switch(a) {case _090:return east;  case _180:return south; case _270:return west;  case _360:case _000: return north; default: break;}
		case east : switch(a) {case _090:return south; case _180:return west;  case _270:return north; case _360:case _000: return east;  default: break;}
		case south: switch(a) {case _090:return west;  case _180:return north; case _270:return east;  case _360:case _000: return south; default: break;}
		case west : switch(a) {case _090:return north; case _180:return east;  case _270:return south; case _360:case _000: return west;  default: break;}
		default: break;
		}
	return undefined_heading;
	}
	
	
	
	// Heading after walking counter-clockwise
	// along a circumference, covering a given angle 
	//
	// d=east            east
	// =======>(---o---)======>
	//           a=180
	//
	heading_type
	dime_orth_plan_undi_graph::	
heading_after_rotation_around_bend
	(heading_type d, angle_type a) const
	{
	switch(d)
		{
		case undefined_heading: return undefined_heading;
		case north: switch(a) {case _090:return east;  case _180:return north; case _270:return west;  case _360:return south; default: break;}
		case east : switch(a) {case _090:return south; case _180:return east;  case _270:return north; case _360:return west;  default: break;}
		case south: switch(a) {case _090:return west;  case _180:return south; case _270:return east;  case _360:return north; default: break;}
		case west : switch(a) {case _090:return north; case _180:return west;  case _270:return south; case _360:return east;  default: break;}
		default: break;
		}
	return undefined_heading;
	}
	
	
	
	heading_type
	dime_orth_plan_undi_graph::	
heading_after_bend
	(heading_type d, bend_type b) const
	{
	switch(d)
		{
		case undefined_heading: return undefined_heading; 
		case north: switch(b) {case left:return west;  case right:return east;  default: break;}
		case east : switch(b) {case left:return north; case right:return south; default: break;}
		case south: switch(b) {case left:return east;  case right:return west;  default: break;}
		case west : switch(b) {case left:return south; case right:return north; default: break;}
		default: break;
		}
	return undefined_heading;
	}
	
	
	
	heading_type
	dime_orth_plan_undi_graph::	
heading_after_bend_sequence
	(heading_type d, gdt::gdtlist<bend_type> bl) const
	{
	bend_type b;
	if (d==undefined_heading) return undefined_heading;
	forall (b,bl) d=heading_after_bend(d,b);
	return d;
	}
	

		
	gdt::gdtlist<bend_type>
	dime_orth_plan_undi_graph::	
minimal_bend_sequence_required_to_change_heading
	(
	heading_type i, // initial heading
	heading_type f  // final heading
	) const
	{
	gdt::gdtlist<bend_type> bl;
        if      (i==north && f==east )  bl.append(right);
	else if (i==north && f==south) {bl.append(right);bl.append(right);} // LL would do as well
	else if (i==north && f==west )  bl.append(left ); 
	else if (i==east  && f==north)  bl.append(left );
	else if (i==east  && f==south)  bl.append(right);
	else if (i==east  && f==west ) {bl.append(right);bl.append(right);} // LL would do as well
	else if (i==south && f==north) {bl.append(right);bl.append(right);} // LL would do as well
	else if (i==south && f==east )  bl.append(left );
	else if (i==south && f==west )  bl.append(right);
	else if (i==west  && f==north)  bl.append(right);
	else if (i==west  && f==east ) {bl.append(right);bl.append(right);} // LL would do as well
	else if (i==west  && f==south)  bl.append(left );
	return bl;
	}

	
/*---------
	// If v is source or target of e then if e is horizontal 
	// the position of v is east or west, if e is vertical 
	// the position of v is north or south
	// If v is not source nor target an error occurs
	//
	heading_type 
	dime_orth_plan_undi_graph::	
position_of_node_with_respect_to_edge(gdtnode v, gdtedge e) 
	{
	gdtnode v1,v2;
	heading_type position;
	int x, x1, x2;
	int y, y1, y2;

	v1 = source(e);
	v2 = target(e);

	if((v != v1) && (v != v2)) gdt_error("invalid gdtnode or gdtedge");
	
	x  = (int)position_of_node(v).xcoord();
	y  = (int)position_of_node(v).ycoord();
	x1 = (int)position_of_node(v1).xcoord();
	y1 = (int)position_of_node(v1).ycoord();
	x2 = (int)position_of_node(v2).xcoord();
	y2 = (int)position_of_node(v2).ycoord();

	switch(slope_of_edge(e))
		{
		case horizontal:
			if(x == gdt_minimum(x1,x2)) position = west;
			else                        position = east;	
			break;
		case vertical:
			if(y == gdt_minimum(y1,y2)) position = south;
			else                        position = north;	
			break;
		default:
			break;
		}
	return position;
	}
	
	
	
	// If v is source or target of e then if e is horizontal 
	// the position of e is east or west, if e is vertical 
	// the position of e is north or south
	// If v is not source nor target an error occurs
	//
	heading_type 
	dime_orth_plan_undi_graph::	
position_of_edge_with_respect_to_node(gdtedge e, gdtnode v) 
	{
	return heading_after_rotation(position_of_node_with_respect_to_edge(v,e),_180);
	}
---------------*/


	// If v is source or target of e then if e is horizontal 
	// the position of v is east or west, if e is vertical 
	// the position of v is north or south
	// If v is not source nor target an error occurs
	//
	heading_type 
	dime_orth_plan_undi_graph::	
position_of_node_with_respect_to_edge(gdtnode v, gdtedge e) const
	{
	return heading_after_rotation(position_of_edge_with_respect_to_node(e,v),_180);
	}
	
	
	// If v is source or target of e then if e is horizontal 
	// the position of e is east or west, if e is vertical 
	// the position of e is north or south
	// If v is not source nor target an error occurs
	//
	heading_type 
	dime_orth_plan_undi_graph::	
position_of_edge_with_respect_to_node(gdtedge e, gdtnode v) const 
	{
	gdtnode v1,v2;

	v1 = source(e);
	v2 = target(e);

	if((v != v1) && (v != v2)) gdt_error("invalid gdtnode or gdtedge");

	border_step  bs = border_step_moving_along_edge (e,v);
	return  initial_heading_of_border_step(bs);
	}


	heading_type
	dime_orth_plan_undi_graph::
position_of_node_with_respect_to_node(gdtnode v1, gdtnode v2)  const
	{
	// The method return a correct value only if the two nodes are aligned
	gdt::gdtpoint p1 = position_of_node(v1);
	gdt::gdtpoint p2 = position_of_node(v2);
	//-------------------
	// Vertical alignment
	//-------------------
	if      (p1.xcoord() == p2.xcoord())
		{
		if (p1.ycoord() > p2.ycoord())
			return north;
		else
			return south;
		}
	//---------------------
	// Horizontal alignment
	//---------------------
	else if (p1.ycoord() == p2.ycoord())
		{
		if (p1.xcoord() > p2.xcoord())
			return east;
		else
			return west;
		}
	//-------------
	// No alignment
	//-------------
	else
		//gdt_error("no aligned nodes");
		return undefined_heading;
	}



	face
	dime_orth_plan_undi_graph::
face_visible_from_node_looking_with_heading
	(gdtnode v, heading_type h) const
	{
	gdtedge e  = NULL_EDGE;
	gdtnode v1 = NULL_NODE;
	heading_type ho1 = heading_after_rotation(h,_090);
	heading_type ho2 = heading_after_rotation(h,_270);
	heading_type ho3 = heading_after_rotation(h,_180);

	if      ((e = find_edge_leaving_node_with_heading(v,h)))   v1 = NULL_NODE;
	else if ((e = find_edge_leaving_node_with_heading(v,ho1))) v1 = opposite(v,e);
	else if ((e = find_edge_leaving_node_with_heading(v,ho2))) v1 = v;
	else if ((e = find_edge_leaving_node_with_heading(v,ho3))) v1 = v;
	else gdt_error("v is isolated");

	if (v1) return right_face_moving_along_edge (e,v1);
	else    return NULL_FACE;
	}



	gdt::gdtpoint
	dime_orth_plan_undi_graph::
position_of_node
	(gdtnode v) const
	{
	return (*node_info)[v].position;
	}



	gdt::gdtlist<gdt::gdtpoint>
	dime_orth_plan_undi_graph::	
position_of_bends_along_edge
	(gdtedge e, gdtnode v) const
	{
	gdt::gdtlist<gdt::gdtpoint> pl = (*edge_info)[e].bends_position;
	if (v == target(e))
		pl.reverse();
		//for (int i=1; i<pl.size(); i++) pl.move_to_front(pl.get_item(i));
	else if (v != source(e)) gdt_error("invalid gdtnode or gdtedge");
	return pl;
	}




	gdtedge
	dime_orth_plan_undi_graph::
find_edge_leaving_node_with_heading
	(gdtnode v, heading_type d) const
	{
	gdtedge e;
	border_step s;
	forall_edges_adjacent_node(e,v,*this)
		{
		s = border_step_moving_along_edge (e,v);
		if (initial_heading_of_border_step(s)==d) return e;
		}
	return NULL_EDGE;
	}



	gdtnode
	dime_orth_plan_undi_graph::
find_node_reachable_moving_from_node_with_heading
	(gdtnode v, heading_type d) const
	{
	gdtedge e = find_edge_leaving_node_with_heading (v,d);
	if (e && number_of_bends(e)==0) return opposite(v,e);
	else return NULL_NODE;
	}



	gdtnode
	dime_orth_plan_undi_graph::
find_node_reachable_moving_from_node_with_heading
	(gdtnode v, heading_type d, gdtedge& e) const
	{
	e = find_edge_leaving_node_with_heading (v,d);
	if (e && number_of_bends(e)==0) return opposite(v,e);
	else return NULL_NODE;
	}



	slope_type
	dime_orth_plan_undi_graph::
opposite_slope
	(slope_type s) const
	{
	switch (s)
		{
		case horizontal:        return vertical;
		case vertical:          return horizontal;
		case undefined_heading: return undefined_slope;
		default: gdt_error("illegal slope"); break;
		}
    	return undefined_slope;
	}



	slope_type
	dime_orth_plan_undi_graph::
slope_along_heading
	(heading_type d) const
	{
	switch (d)
		{
		case north: case south:   return vertical;
		case east:  case west:    return horizontal;
		case undefined_heading:   return undefined_slope;
		default: gdt_error("illegal slope"); break;
		}
    	return undefined_slope;
	}




	slope_type
	dime_orth_plan_undi_graph::
slope_of_border_step
	(border_step s) const
	{
	return slope_along_heading (initial_heading_of_border_step(s));
	}



	slope_type
	dime_orth_plan_undi_graph::
slope_of_edge
	(gdtedge e) const
	{
	return slope_of_border_step (border_step_moving_along_edge(e,source(e)));
	}



	gdt::gdtlist<gdtnode>
	dime_orth_plan_undi_graph::
nodes_reachable_moving_from_node_with_slope
	(gdtnode v, slope_type slope) const
	{
	gdtnode w;
	gdt::gdtlist<gdtnode> chain;
	heading_type back = north,
		     forw = north;
	switch (slope)
		{
		case horizontal:      back = west;  forw = east;  break;
		case vertical:        back = north; forw = south; break;
		case undefined_slope: gdt_error("undefined chain slope");
		}
	w=v; while(w)
		{
		chain.push(w);
		w = find_node_reachable_moving_from_node_with_heading (w,back);
		}
	w=v; while(w)
		{
		w = find_node_reachable_moving_from_node_with_heading (w,forw);
		if (w) chain.append(w);
		}
	return chain;
	}



	gdt::gdtlist<gdtnode>
	dime_orth_plan_undi_graph::
nodes_reachable_moving_from_node_with_slope
	(gdtnode v, slope_type slope, gdt::gdtlist<gdtedge>& edge_chain) const
	{
	gdtnode w;
	gdtedge e;
	gdt::gdtlist<gdtnode> node_chain;
	edge_chain.clear();
	heading_type back = north,
		     forw = north;
	switch (slope)
		{
		case horizontal:      back = west;  forw = east;  break;
		case vertical:        back = north; forw = south; break;
		case undefined_slope: gdt_error("undefined chain slope");
		}
	w=v; while(w)
		{
		node_chain.push(w);
		w = find_node_reachable_moving_from_node_with_heading (w,back,e);
		if (e) edge_chain.push(e);
		}
	w=v; while(w)
		{
		w = find_node_reachable_moving_from_node_with_heading (w,forw,e);
		if (w)
			{
			node_chain.append(w);
			edge_chain.append(e);
			}
		}

	return node_chain;
	}


	int
	dime_orth_plan_undi_graph::
length_of_edge
	(gdtedge e) const
	{
	int len;
	gdtnode s,t;
	gdt::gdtpoint ps,pt,p1,p2;
	gdt::gdtlist<gdt::gdtpoint> bend_points;

	s  = source(e);
	t  = target(e);
	ps = position_of_node (s);
	pt = position_of_node (t);
	bend_points = position_of_bends_along_edge (e,s);

	len = 0;

	if (bend_points.empty())
		len += (int)pt.distance(ps);
	else
		{
		p1 = ps;
		forall (p2,bend_points) {len += (int)p2.distance(p1); p1=p2;}
		len += (int)pt.distance(p2);
		}
		
	return len;
	}
	
	

	// Distance along the border of the face f, 
	// moving clockwise from v1 to v2.
	// Precondition: v1 and v2 lay on the same face f
	int 
	dime_orth_plan_undi_graph::
border_distance 
	(
	gdtnode v1,
	gdtnode v2,
	face f
	) const
	{
	//cout << "\ndime_orth_plan_undi_graph::border_distance gdtnode " << id(v1) << " gdtnode " << id(v2) << flush;
	int len;
	border_step bs;
	gdt::gdtlist<border_step> bsl;

	len = 0;
	if (v1!=v2)
		{
		bsl = border_step_sequence (v1,v2,f);	
 		forall (bs, bsl) len += length_of_edge(edge_of_border_step(bs));
		}
	return len;
	}



	// Distance along the border of the face f, moving
	// clockwise from v to e. The length of e is not included
	// Precondition: v and e lay on the same face f
	int 
	dime_orth_plan_undi_graph::
border_distance 
	(
	gdtnode v,
	gdtedge e,
	face f
	) const
	{
	//cout << "\ndime_orth_plan_undi_graph::border_distance gdtnode " << id(v) << " gdtedge " << id(e) << flush;
	gdtnode vx = first_node_visited_while_moving_on_edge_around_face (e,f);
	return border_distance(v,vx,f);
	}



	// Distance along the border of the face f, moving
	// clockwise from e to v. The length of e is not included
	// Precondition: v and e lay on the same face f
	int 
	dime_orth_plan_undi_graph::
border_distance 
	(
	gdtedge e,
	gdtnode v,
	face f
	) const
	{
	gdtnode vx = second_node_visited_while_moving_on_edge_around_face (e,f);
	return border_distance(vx,v,f);
	}


	
	// Distance along the border of the face f, moving
	// clockwise from e1 to e2. The length of e1 and e2 are not included
	// Precondition: e1 and e2 lay on the same face f
	int 
	dime_orth_plan_undi_graph::
border_distance 
	(
	gdtedge e1,
	gdtedge e2,
	face f
	) const
	{
	//cout << "\ndime_orth_plan_undi_graph::border_distance gdtedge " << id(e1) << " gdtedge " << id(e2) << flush;
	gdtnode vx1 = second_node_visited_while_moving_on_edge_around_face (e1,f);
	gdtnode vx2 = first_node_visited_while_moving_on_edge_around_face (e2,f);
	return border_distance(vx1,vx2,f);
	}


	// Distance along the border of the face f, 
	// moving clockwise or counter-clockwise from v1 to v2.
	// Precondition: v1 and v2 lay on the same face f
	int 
	dime_orth_plan_undi_graph::
min_border_distance 
	(
	gdtnode v1,
	gdtnode v2,
	face f,
	bool& ordered
	) const
	{
	int d1 = border_distance(v1,v2,f);
	int d2 = border_distance(v2,v1,f);
	ordered = (d1<d2);
	return gdt_minimum (d1,d2);
	}

 

	// Distance along the border of the face f, 
	// moving clockwise or counter-clockwise from v along e.
	// Precondition: v1 and v2 lay on the same face f
	int 
	dime_orth_plan_undi_graph::
min_border_distance 
	(
	gdtnode v,
	gdtedge e,
	face f,
	bool& ordered
	) const
	{
	int d1 = border_distance(v,e,f);
	int d2 = border_distance(e,v,f);
	ordered = (d1<d2);
	return gdt_minimum (d1,d2);
	}
		



	// Distance along the border of the face f, 
	// moving clockwise or counter-clockwise from v1 to v2.
	// Precondition: v1 and v2 lay on the same face f
	int 
	dime_orth_plan_undi_graph::
min_border_distance 
	(
	gdtedge e1,
	gdtedge e2,
	face f,
	bool& ordered
	) const
	{
	int d1 = border_distance(e1,e2,f);
	int d2 = border_distance(e2,e1,f);
	ordered = (d1<d2);
	return gdt_minimum (d1,d2);
	}



	int 
	dime_orth_plan_undi_graph::
inline_distance 
	(
	gdtnode v1,
	gdtnode v2,
	slope_type slope
	) const
	{
	int x1 = (int)position_of_node(v1).xcoord();
	int y1 = (int)position_of_node(v1).ycoord();
	int x2 = (int)position_of_node(v2).xcoord();
	int y2 = (int)position_of_node(v2).ycoord();

	int ild = 0;
	switch (slope)
		{
		case horizontal: ild = abs(x1-x2); break;
		case vertical  : ild = abs(y1-y2); break;
		default        : gdt_error("invalid slope"); break;
		}
	return ild;
	}




	int 
	dime_orth_plan_undi_graph::
inline_distance 
	(gdtnode v, gdtedge e) const
	{
	//cout << "\ndime_orth_plan_undi_graph::inline_distance gdtnode " << id(v) << " gdtedge " << id(e) << flush;

	gdtnode v1 = source(e);
	gdtnode v2 = target(e);

	int x  = (int)position_of_node(v).xcoord();
	int y  = (int)position_of_node(v).ycoord();
	int x1 = (int)position_of_node(v1).xcoord();
	int y1 = (int)position_of_node(v1).ycoord();
	int x2 = (int)position_of_node(v2).xcoord();
	int y2 = (int)position_of_node(v2).ycoord();

	int xmin, xmax;
	int ymin, ymax;

	int ild = 0;
	switch (slope_of_edge(e))
		{
		case vertical:
			ymin = gdt_minimum(y1,y2);
			ymax = gdt_maximum(y1,y2);

			if (y>ymin && y<ymax)
				//
				//  Horizontal projection of v falls into e
				//
				//  v1----o------o
				//  |            |
				//  e  ..........v
				//  |            |
				//  v2           |
				//  |            |
				//  |            |
				//  o------------o
				//
				ild = -1;
			
			else
				{
				//  Horizontal projection of v falls outside e
				//
				if (y>=ymax) ild = y-ymax;
				else         ild = ymin-y;
				}
			break;

		case horizontal:
			xmin = gdt_minimum(x1,x2);
			xmax = gdt_maximum(x1,x2);

			if (x>xmin && x<xmax)
				//
				//  Vertical projection of v falls into e
				//  Note: ordered is undefined.
				//
				//  v1--e--v2---o
				//  |   .       |
				//  |   .       |
				//  |   .       |
				//  |   .       |
				//  |   .       |
				//  o---v-------o
				//
				ild = -1;
			
			else
				{
				//  Horizontal projection of v falls outside e
				//
				if (x>=xmax) ild = x-xmax;
				else         ild = xmin-x;
				}
			break;
		default:
			gdt_error("invalid gdtedge slope");
			break;
		}
	return ild;
	}



	int 
	dime_orth_plan_undi_graph::
inline_distance
	(gdtedge e1, gdtedge e2) const
	{
	//cout << "\ndime_orth_plan_undi_graph::inline_distance gdtedge " << id(e1) << " gdtedge " << id(e2) << flush;

    	//  v1----o------o
	//  |            |
	//  e1           |
	//  |            |
    	//  v2           |
	//  |            v3
	//  |            |
	//  |            e2
	//  o            |
	//  |            v4
	//  |            |
	//  o------------o

	if (slope_of_edge(e1) != slope_of_edge(e2)) 
		gdt_error("edges are not parallel");

	gdtnode vs1 = source(e1);
	gdtnode vt1 = target(e1);
	gdtnode vs2 = source(e2);
	gdtnode vt2 = target(e2);

	int ild;
	int d1 = gdt_minimum (inline_distance(vs1,e2), inline_distance(vt1,e2));
	int d2 = gdt_minimum (inline_distance(vs2,e1), inline_distance(vt2,e1));

	if (d1 && d2) ild = gdt_minimum (d1,d2);
	else          ild = -1;

	// inline_distance = -1
	//		
        ////////////////// Edges vertical/horizontal projections totally overlap
			//
			//  o-----o------o
			//  |            |
			//  |            |
			//  v1.......... v4
			//  |            |
			//  e1           e2
			//  |            |
			//  v2...........v3
			//  |            |
			//  |            |
			//  v------------v
			//

	// inline_distance = -1
	//		
	//////////////////
			// Edges vertical/horizontal projections partially overlap 
			//
			//  v1----v------v
			//  |            |
			//  e1           |
			//  |     ...... v3
			//  v2......     |
			//  |            e2
			//  |            |
			//  v            v4
			//  |            |
			//  |            |
			//  v------------v
			//

	// ild = 0
	//		
	//////////////////
			// Edges vertical/horizontal overlap at one (and only one) end-gdtnode
			//
			//  v1----v------v
			//  |            |
			//  e1           |
			//  |            |
			//  v2...........v3
			//  |            |
			//  |            e2
			//  v            |
			//  |            v4
			//  |            |
			//  v------------v
			//

	// ild = n>0
	//		
	//////////////////
			// Edges vertical/horizontal projections do NOT overlap
			//
			//  v1----v------v
			//  |            |
			//  e1           |
			//  |            |
			//  v2......     |
			//  |      ......v3
			//  |            |
			//  |            e2
			//  v            |
			//  |            v4
			//  |            |
			//  v------------v
			//
	return ild;
	}

 
	
	int
	dime_orth_plan_undi_graph::
traversal_distance 
	(
	gdtnode v1,
	gdtnode v2,
	slope_type slope
	) const
	{
	return inline_distance(v1,v2,opposite_slope(slope));
	}



	int 
	dime_orth_plan_undi_graph::
traversal_distance
	(gdtnode v, gdtedge e) const
	{
	//cout << "\ndime_orth_plan_undi_graph::traversal_distance gdtnode " << id(v) << " gdtedge " << id(e) << flush;
	return traversal_distance(v, source(e), slope_of_edge(e));
	}


	int 
	dime_orth_plan_undi_graph::
traversal_distance
	(gdtedge e1, gdtedge e2) const
	{
	//cout << "\ndime_orth_plan_undi_graph::traversal_distance gdtedge " << id(e1) << " gdtedge " << id(e2) << flush;

	if (slope_of_edge(e1) != slope_of_edge(e2)) 
		gdt_error("edges are not parallel");

	return traversal_distance(source(e1),e2);
	}


	// WARNING: this function is not ambiguous for a dime
	// because a dime is rectangular and therefore it has no bridges.
	// It would be ambiguous for an ortho, because ortho might have bridges.
	//
	angle_type
	dime_orth_plan_undi_graph::
angle_on_node
	(gdtnode v, face f) const
	{
	gdtedge e;
	gdt::gdtlist<gdtedge> le_vf = edges_entering_node_while_moving_around_face (v,f);
	if (!le_vf.empty()) e = le_vf.head();
	else		    e = NULL_EDGE;
	border_step bs = border_step_moving_along_edge (e, opposite(v,e));
	return angle_of_border_step(bs);
	}


	angle_type
	dime_orth_plan_undi_graph::
rotation_around_bend_required_to_change_heading
	(
	heading_type ih, // initial heading
	heading_type fh  // final_heading
	) const
	{
	switch (ih)
		{
		case north: switch(fh) {case north:return _180; case east:return _090; case south:return _360; case west:return _270; default: break;}
		case east : switch(fh) {case north:return _270; case east:return _180; case south:return _090; case west:return _360; default: break;}
		case south: switch(fh) {case north:return _360; case east:return _270; case south:return _180; case west:return _090; default: break;}
		case west : switch(fh) {case north:return _090; case east:return _360; case south:return _270; case west:return _180; default: break;}
		default: break;
		}
	gdt_error("invalid heading");
	return _360; //Just to avoid compiler warnings
	}



	angle_type
	dime_orth_plan_undi_graph::
angle_required_to_change_heading
	(
	heading_type ih,
	heading_type fh
	)
	{
	switch (ih)
		{
		case north: switch(fh) {case north:return _360; case east:return _090; case south:return _180; case west:return _270; default: break;}
		case east : switch(fh) {case north:return _270; case east:return _360; case south:return _090; case west:return _180; default: break;}
		case south: switch(fh) {case north:return _180; case east:return _270; case south:return _360; case west:return _090; default: break;}
		case west : switch(fh) {case north:return _090; case east:return _180; case south:return _270; case west:return _360; default: break;}
		default: break;
		}
	gdt_error("invalid heading");
	return _360; //Just to avoid compiler warnings
	}



	void
	dime_orth_plan_undi_graph::
compute_dime_with_expanded_nodes
	(
	gdt::gdtnode_array<int>& w,
	gdt::gdtnode_array<int>& h,
	dime_orth_plan_undi_graph& dopug1,
	gdt::gdtnode_map<gdtnode>& super_node
	) const
	{
	gdtnode v;
	gdtedge e;

	if (number_of_bends() > 0)
		gdt_error("graph must have no bends");

	// --------------------------- 0 ----------------------------
	// copy the current graph in graph dopug
	// ----------------------------------------------------------

	dopug1 = *this;

	// ----------------------------- 1 -----------------------------
	// compute the position of the bottom-left corner of each vertex
	// "v" in order to make free space for expanding "v" into a
	// super-gdtnode with dimensions (w[v] x h[v]).
	// Also, for each gdtnode "v", the structure pie[v] stores the new
	// relative positions of the edges incident with "v" in the
	// four directions (north, south, east, west). The relative
	// positions are expressed with respect to the bottom-left
	// corner of the new super-gdtnode.
	// -------------------------------------------------------------


	gdt::gdtnode_array<gdt::gdtpoint>           pos_bottom_left  (*this);
	gdt::gdtnode_array<pos_incid_edges> pie 	     (*this);
	position_bottom_left_corners_and_incid_edges (w,h,pos_bottom_left,pie);
	

	forall_nodes (v,*this)
		{
		gdtnode v1 = dopug1.corresponding_node (v,*this);
		dopug1.set_position_of_node (v1,pos_bottom_left[v]);
		}
	
	
	// --------------------------- 2 ---------------------------
	// expand nodes of dopug into faces, according to the 
	// dimensions w,h of each gdtnode of the current graph, 
	// and by using the informations stored in the "pie" data 
	// structure to correctly calculate the positions
	// of the edges incident with the new super-nodes in dopug.
	// ---------------------------------------------------------
	
	
	dopug1.expand_nodes_into_super_nodes (*this,w,h,pos_bottom_left,pie,super_node);
		
		// debugging
		//
		//{
		//draw_undi_graph dug (0,dopug1);
		//dug.write("after_expand.gdt");
		//}
		
	dopug1.uncompress_edges (*this,w,h,pie,super_node);
	
		// debugging
		//
		//{
		//draw_undi_graph dug (0,dopug1);
		//dug.write("after_uncompress.gdt");
		//}
		
	// --------
	// new code
	// --------
	
	// -------------------- 3 ---------------------
	// expand all the super nodes that are chains
	// into boxes with one of the two dimensions
	// equal to zero.
	// --------------------------------------------

	gdt::gdtedge_map<bool> is_zero_edge(dopug1,false);	// edges that will have length zero			   
	   
	   {
	   gdt::gdtlist<gdtedge> zero_edges;
	   forall_nodes (v,dopug1)
		{
		gdtnode V = super_node[v];
		if (V && dopug1.id(v) == id(V))
			{
			gdt::gdtlist<gdtnode> Lv;
			if 	(w[V] > 0  && h[V] == 0) Lv = dopug1.expand_chain (v, vertical  , w[V], zero_edges);
			else if (w[V] == 0 && h[V] > 0)  Lv = dopug1.expand_chain (v, horizontal, h[V], zero_edges);
			
			gdtnode u = NULL_NODE;
			forall (u,Lv) super_node[u] = V;
			}
		}
	

	   gdtedge e = NULL_EDGE;	   
	   forall(e, zero_edges) is_zero_edge[e]=true;
	   }
	 
	
	// ----------------------- 4 ----------------------
	// Now, there are no more simple chains.
	// Absorbe all the nodes of "dopug1" that are 
	// in the same super nodes but that are not 
	// corners or that have degree less than three.
	// Also move each gdtedge that is attached to a corner
	// on a new dummy gdtedge close to the corner.
	// ------------------------------------------------
	
	// absorbe nodes
	//
        gdt::gdtlist<gdtnode> dopug1nodes = dopug1.all_nodes(); 
	forall(v,dopug1nodes)
		{
		gdtnode V = super_node[v];
		if (V && id(V) != dopug1.id(v))
			if ( (dopug1.degree(v) == 2) && (dopug1.node_is_flat(v)) )
				dopug1.weld_edges_at_node(v);
		}
	
	//Debug

	 //{
	 //cout << "\nBEFORE MOVE EDGE CLOSE ..." << flush;
	 //gdtedge e1;
	 //forall_edges (e1,dopug1)
	 //	{		
	 //	cout << "\n" << dopug1.id(e1) << " is zero gdtedge = " 
	 //	     << is_zero_edge[e1] << flush;
	 //	}
	 //}
	
	// debugging
	//
	//{
	//draw_undi_graph dug (0,dopug1);
	//dug.write("after_absorbe.gdt");
	//}
	 		
	// move edges at corners
	//	
	gdt::gdtlist<gdtnode> corners = dopug1.corners_of_super_nodes (*this,super_node,w,h);
	forall (v,corners)
		{				
		gdtedge e;
		gdt::gdtlist<gdtedge> Lev;	// edges to move
		//
		forall_edges_adjacent_node (e,v,dopug1)
			{			
			gdtnode v1 = dopug1.opposite(v,e);			
			if (super_node[v1] != super_node[v]) Lev.append(e); // the gdtedge must be moved 
			}
		forall (e,Lev) 
			dopug1.move_edge_close_to_corner_of_super_node(e,v,super_node,is_zero_edge);
		}
		

	//Debug
	
	 //{
	 //cout << "\nAFTER MOVE EDGE CLOSE ..." << flush;
	 //gdtedge e1;
	 //forall_edges (e1,dopug1)
	 //	{		
	 //	cout << "\n" << dopug1.id(e1) << " " << e1 << " is zero gdtedge = " 
	 //	     << is_zero_edge[e1] << flush;
	 //	}
	 //}
	


	// debugging
	//		
	//{
	//draw_undi_graph dug (0,dopug1);
	//dug.write("after_edge_moving.gdt");
	//}

	// --------------------------- 5 ---------------------------
	// execute a sequence of one-dimensional-compactions in
	// order to minimize the total gdtedge length as much as 
	// possible (sub-optimal)
	// ---------------------------------------------------------	 
	
	bool exit = false;
	gdt::gdtmap<int,int> _min(0);	  // lower capacities
	gdt::gdtmap<int,int> _max(0);	  // upper capacities
	gdt::gdtmap<int,int> cost(0);	  // costs
	gdt::gdtmap<int,int> node_width(0);  // width of each gdtnode of the not expanded graph
	gdt::gdtmap<int,int> node_height(0); // height of each gdtnode of the not expanded graph
	
	int total_edge_length_before = 0;	// total gdtedge length before one dimensional compactions
	int total_edge_length_after  = 0;	// total gdtedge length after one dimensinal compactions
	
	
	// set costs, lower, and upper capacities 
	// --------------------------------------
	//

	int minimum_length= 1;
	// look for a MIN_TIEING constraint 
	     {
	       constraint c = NULL_CONSTRAINT;
	       bool found=false;
	       forall( c, dopug1.all_constraints() ) 
		 {
		   if (c->get_type()== MIN_TIEING)
		     {
		       if (found) gdt_error("two MIN_TIEING constraints found");
		       minimum_length= ((constraint_min_tieing)c)->get_value();
		       found=true;
		     }
		 }
	     }

	forall_edges (e,dopug1)
		{
		int ide  = dopug1.id(e);
		if (is_zero_edge[e]) 
			{
			_min[ide]  = 0;
			_max[ide]  = 0;
			cost[ide] = 1;
			}
		else    
			{
			_min[ide]  = minimum_length;
			_max[ide]  = INFINITE;
			cost[ide] = 1;
			}
		total_edge_length_before += dopug1.length_of_edge(e);
		}
	
	
	// fix the lower capacities for the edges adjacent 
	// to the corners of the super-nodes
	// ------------------------------------------------
	//
	forall (v, corners)
		{
		gdtedge e1   = dopug1.first_adj_edge(v);
		gdtedge e2   = dopug1.last_adj_edge(v);
		int ide1  = dopug1.id(e1);
		int ide2  = dopug1.id(e2);
		_min[ide1] = 0;
		_min[ide2] = 0;
		}
	
	
	forall_nodes (v,dopug1)
		{
		gdtnode V = super_node[v];
		int idv = dopug1.id(v);
		
		if (V && id(V) == idv) // v is the bottom left corner of the super-gdtnode
			{
			node_width [idv] = w[V];
			node_height[idv] = h[V];
			}
		else 	{
			node_width[idv]  = 0;
			node_height[idv] = 0;
			}
		}

	
	// Debug
	/*
	  {
	  draw_undi_graph dug (0, dopug1);
	  dug.write ("beforecomp.gdt");
	  cout << "\n++++++++++++++++++++++++++++" << flush;
	  forall_edges (e, dopug1)
	  	{
		int ide1 = dopug1.id(e);
		cout << "\nmin[" << ide1 << "]=" << _min[ide1] << flush;
		cout << "\nmax[" << ide1 << "]=" << _max[ide1] << flush;
		cout << "\ncost[" << ide1 << "]=" << cost[ide1] << flush;
		}
	  }
	*/
	
	//
	
	 		
	// compact recursively
	//
	//cout << "\nINIT POST-COMPACTION" << flush;
	int i = 0;
	while (!exit)
		{
		i++;
		//cout << "\ncycle number " << i << flush;
		dopug1.one_dimensional_tieing_for_expanded_nodes (_min,_max,cost,vertical,node_height, minimum_length);
       		dopug1.one_dimensional_tieing_for_expanded_nodes (_min,_max,cost,horizontal,node_width, minimum_length);
	
		total_edge_length_after = 0;
		forall_edges (e,dopug1) total_edge_length_after += dopug1.length_of_edge(e);
		
		//cout << "\ntotal gdtedge length before = " <<  total_edge_length_before << flush;
		//cout << "\ntotal gdtedge length after = " <<  total_edge_length_after << flush;
		
		if (total_edge_length_after < total_edge_length_before) 
					total_edge_length_before = total_edge_length_after;
		else    exit = true;
		}
      
        //{
        //draw_undi_graph dug(0, dopug1);
        //dug.write( "acomp.gdt" );
        //}	
	}



	void
	dime_orth_plan_undi_graph::
compute_dime_with_expanded_nodes_and_edges_centered
	(
	gdt::gdtnode_array<int>& w,
	gdt::gdtnode_array<int>& h,
	dime_orth_plan_undi_graph& dopug1,
	gdt::gdtnode_map<gdtnode>& super_node
	) const
	{
	gdtnode v;
	gdtedge e;
	
	if (number_of_bends() > 0)
		gdt_error("graph must have no bends");
	
	// --------------------------- 0 ----------------------------
	// copy the current graph in graph dopug
	// ----------------------------------------------------------

	dopug1 = *this;
	
	// ----------------------------- 1 -----------------------------
	// compute the position of the bottom-left corner of each vertex
	// "v" in order to make free space for expanding "v" into a
	// super-gdtnode with dimensions (w[v] x h[v]).
	// Also, for each gdtnode "v", the structure pie[v] stores the new
	// relative positions of the edges incident with "v" in the
	// four directions (north, south, east, west). The relative
	// positions are expressed with respect to the bottom-left
	// corner of the new super-gdtnode.
	// -------------------------------------------------------------


	gdt::gdtnode_array<gdt::gdtpoint>           pos_bottom_left  (*this);
	gdt::gdtnode_array<pos_incid_edges> pie 	     (*this);
	position_bottom_left_corners_and_incid_edges_centered (w,h,pos_bottom_left,pie);
	
	
	forall_nodes (v,*this)
		{
		gdtnode v1 = dopug1.corresponding_node (v,*this);
		dopug1.set_position_of_node (v1,pos_bottom_left[v]);
		}
	
	
	// --------------------------- 2 ---------------------------
	// expand nodes of dopug into faces, according to the
	// dimensions w,h of each gdtnode of the current graph,
	// and by using the informations stored in the "pie" data
	// structure to correctly calculate the positions
	// of the edges incident with the new super-nodes in dopug.
	// ---------------------------------------------------------
	
	
	dopug1.expand_nodes_into_super_nodes (*this,w,h,pos_bottom_left,pie,super_node);
		
		// debugging
		//
		//{
		//draw_undi_graph dug (0,dopug1);
		//dug.write("after_expand.gdt");
		//}

	
	// -------------------- 3 ---------------------
	// expand all the super nodes that are chains
	// into boxes with one of the two dimensions
	// equal to zero.
	// --------------------------------------------

	gdt::gdtedge_map<bool> is_zero_edge(dopug1,false);	// edges that will have length zero
	   {
	   gdt::gdtlist<gdtedge> zero_edges;
	   forall_nodes (v,dopug1)
		{
		gdtnode V = super_node[v];
		if (V && dopug1.id(v) == id(V))
			{
			gdt::gdtlist<gdtnode> Lv;
			if 	(w[V] > 0  && h[V] == 0) Lv = dopug1.expand_chain (v, vertical  , w[V], zero_edges);
			else if (w[V] == 0 && h[V] > 0)  Lv = dopug1.expand_chain (v, horizontal, h[V], zero_edges);
			
			gdtnode u = NULL_NODE;
			forall (u,Lv) super_node[u] = V;
			}
		}
	

	   gdtedge e = NULL_EDGE;
	   forall(e, zero_edges) is_zero_edge[e]=true;
	   }
	
	// ----------------------- 4 ----------------------
	// Now, there are no more simple chains.
	// Absorbe all the nodes of "dopug1" that are
	// in the same super nodes but that are not
	// corners or that have degree more then three.
	// Also move each gdtedge that is attached to a corner
	// on a new dummy gdtedge close to the corner.
	// ------------------------------------------------

		
	// move edges at corners
	//
	gdt::gdtlist<gdtnode> corners = dopug1.corners_of_super_nodes (*this,super_node,w,h);
	forall (v,corners)
		{
		gdtedge e;
		gdt::gdtlist<gdtedge> Lev;	// edges to move
		//
		forall_edges_adjacent_node (e,v,dopug1)
			{
			gdtnode v1 = dopug1.opposite(v,e);
			if (super_node[v1] != super_node[v]) Lev.append(e); // the gdtedge must be moved
			}
		forall (e,Lev)
			dopug1.move_edge_close_to_corner_of_super_node(e,v,super_node,is_zero_edge);
		}

		
	// --------------------------- 5 ---------------------------
	// execute a sequence of one-dimensional-compactions in
	// order to minimize the total gdtedge length as much as
	// possible (sub-optimal)
	// ---------------------------------------------------------	
	
	bool exit = false;
	gdt::gdtmap<int,int> min;	  // lower capacities
	gdt::gdtmap<int,int> max;	  // upper capacities
	gdt::gdtmap<int,int> cost;	  // costs
	gdt::gdtmap<int,int> node_width;  // width of each gdtnode of the not expanded graph
	gdt::gdtmap<int,int> node_height; // height of each gdtnode of the not expanded graph
	
	int total_edge_length_before = 0;	// total gdtedge length before one dimensional compactions
	int total_edge_length_after  = 0;	// total gdtedge length after one dimensinal compactions

	
	// set costs, lower, and upper capacities
	// --------------------------------------
	//

	int minimum_length= 1;
	// look for a MIN_TIEING constraint
	     {
	       constraint c = NULL_CONSTRAINT;
	       bool found=false;
	       forall( c, dopug1.all_constraints() )
		 {
		   if (c->get_type()== MIN_TIEING)
		     {
		       if (found) gdt_error("two MIN_TIEING constraints found");
		       minimum_length= ((constraint_min_tieing)c)->get_value();
		       found=true;
		     }
		 }
	     }

	forall_edges (e,dopug1)
		{
		int ide  = dopug1.id(e);
		
		if (is_zero_edge[e])
			{
			min[ide]  = 0;
			max[ide]  = 0;
			cost[ide] = 1;
			}
		else
			{
			min[ide]  = minimum_length;
			max[ide]  = INFINITE;
			cost[ide] = 1;
			}
		total_edge_length_before += dopug1.length_of_edge(e);
		}
	
	
	
	forall_nodes (v,dopug1)
		{
		gdtnode V = super_node[v];
		int idv = dopug1.id(v);
		
		if (V && id(V) == idv) // v is the bottom left corner of the super-gdtnode
			{
			node_width [idv] = w[V];
			node_height[idv] = h[V];
			}
		else 	{
			node_width[idv]  = 0;
			node_height[idv] = 0;
			}		
		}
		
	 		
	// compact recursively
	//
	//cout << "\nINIT POST-COMPACTION" << flush;
	int i = 0;
	while (!exit)
		{
		i++;
		//cout << "\ncycle number " << i << flush;
		dopug1.one_dimensional_tieing_for_expanded_nodes (min,max,cost,vertical,node_height, minimum_length);
       		dopug1.one_dimensional_tieing_for_expanded_nodes (min,max,cost,horizontal,node_width, minimum_length);

		total_edge_length_after = 0;
		forall_edges (e,dopug1) total_edge_length_after += dopug1.length_of_edge(e);

		//cout << "\ntotal gdtedge length before = " <<  total_edge_length_before << flush;
		//cout << "\ntotal gdtedge length after = " <<  total_edge_length_after << flush;
		
		if (total_edge_length_after < total_edge_length_before)
					total_edge_length_before = total_edge_length_after;
		else    exit = true;
		}

	  // debugging
          //{
          //draw_undi_graph dug(0, dopug1);
          //dug.write( "after_comp.gdt" );
          //}	
	}



	void
	dime_orth_plan_undi_graph::
compute_dime_with_expanded_nodes_and_pins 
	(
	const gdt::gdtnode_array<int>& 		w,
	const gdt::gdtnode_array<int>& 		h,
	dime_orth_plan_undi_graph& 		dopug,
	gdt::gdtnode_map<gdtnode>& 		super_node
	) const
	{
	gdtnode v;
	gdtedge e;
	
	if (number_of_bends() > 0) 
		gdt_error("graph must have no bends");

        // ### Begin import from GDT fork of W. Didimo on June, 18 2004

	// debugging
	//
	//  {
	//  cout << "\ncompute_dime_with_expanded_nodes_and_pins\n" << flush;
	//  cout << "\nDIME:\n" << flush;
	//  print (BY_FACES);	  
	//  gdtedge e;
	//  forall_edges (e,*this)
	//  	{
	//	cout << "\nedge " << id(e) << flush;
	//	border_step s1 = border_step_moving_along_edge (e,source(e));
	//	border_step s2 = opposite_border_step (s1);
	//	int p_s1 = pin_number_of_border_step (s1);
	//	int p_s2 = pin_number_of_border_step (s2);
	//	cout << "\npin number of bs starting from " << id(source(e)) << "=" << p_s1 << flush;
	//	cout << "\npin number of bs starting from " << id(target(e)) << "=" << p_s2 << flush;
	//	}
	//  }
	//

        // ### End import
	
	
	// ----------------------------- 0 -----------------------------
	// compute the position of the bottom-left corner of each vertex
	// "v" in order to make free space for expanding "v" into a
	// super-gdtnode with dimensions (w[v] x h[v]).
	// Also, for each gdtnode "v", the structure pie[v] stores the new
	// relative positions of the edges incident to "v" in the
	// four directions (north, south, east, west). The relative
	// positions are expressed with respect to the bottom-left
	// corner of the new super-gdtnode.
	// -------------------------------------------------------------

	gdt::gdtnode_array<gdt::gdtpoint>   pos_bottom_left  (*this);
	gdt::gdtnode_array<pos_incid_edges> pie 	     (*this);
	position_bottom_left_corners_and_incid_edges_with_pins (w,h,pos_bottom_left,pie);

	// --------------------------- 1 ----------------------------
	// copy the current graph in graph dopug 
	// ----------------------------------------------------------

	dopug = *this;

		// debugging
		//
		//{
		//cout << "\nthis is the dopug in the dime_orth_plan_undi_graph::compute_dime_with_expanded_nodes_and_pins\n";
		//dopug.print(COMPLETE);
		//}
		
	
	forall_nodes (v,*this)
		{
		gdtnode v1 = dopug.corresponding_node (v,*this);
		dopug.set_position_of_node (v1,pos_bottom_left[v]);
		}
	
	
	// --------------------------- 2 ---------------------------
	// expand nodes of dopug into faces, according to the 
	// dimensions w,h of each gdtnode of the current graph, 
	// and by using the informations stored in the "pie" data 
	// structure to correctly calculate the positions
	// of the edges incident with the new super-nodes in dopug.
	// ---------------------------------------------------------

        // ### Begin import from GDT fork of W. Didimo on June, 18 2004
	//dopug.expand_nodes_into_super_nodes (*this,w,h,pos_bottom_left,pie,super_node);
	
	// The three lines of code below replace the line of code above
	// W.D. August 2002
	//
	gdt::gdtnode_map<bool> is_removable (dopug,false); // informs if a dummy node in a super node can be removed before compaction
	gdt::gdtedge_map<bool> removable_edge (dopug,false); // informs if an edge can have zero length after compaction.
	dopug.expand_nodes_into_super_nodes_with_pins (*this,w,h,pos_bottom_left,pie,super_node,is_removable, removable_edge);
        // ### End import

		// debugging
		//
		//{
	        //draw_undi_graph dug(0, dopug);
        	//dug.write("nodi_espansi.gdt"); 
		//}
		//
		
        // ### Begin import from GDT fork of W. Didimo on June, 18 2004
	//dopug.uncompress_edges_with_pins (*this,w,h,pie,super_node);  
	dopug.uncompress_edges_with_pins (*this,w,h,pie,super_node,is_removable,removable_edge);  
        // ### End import


		// debugging
		//
		//{
       		//draw_undi_graph dug(0, dopug);
	        //dug.write("edges_espansi.gdt"); 
		//}
	
	//std::cout << "\nUNCOMPRESS EDGES OK!" << std::flush;
	
	
	// -------------------- 3 ---------------------
	// expand all the super nodes that are chains
	// into boxes with one of the two dimensions
	// equal to zero.
	// --------------------------------------------

	gdt::gdtedge_map<bool> is_zero_edge(dopug,false);	// edges that will have length zero
	   {
	   gdt::gdtlist<gdtedge> zero_edges;
	   forall_nodes (v,dopug)
		{
		gdtnode V = super_node[v];
		if (V && dopug.id(v) == id(V))
			{
			gdt::gdtlist<gdtnode> Lv;
			if 	(w[V] > 0  && h[V] == 0) Lv = dopug.expand_chain (v, vertical  , w[V], zero_edges);
			else if (w[V] == 0 && h[V] > 0)  Lv = dopug.expand_chain (v, horizontal, h[V], zero_edges);
			
			gdtnode u = NULL_NODE;
			forall (u,Lv) super_node[u] = V;
			}
		}
	

	   gdtedge e = NULL_EDGE;
	   forall(e, zero_edges) is_zero_edge[e]=true;
	   }	
	// -----------------------------------------
	   
        // ### Begin import from GDT fork of W. Didimo on June, 18 2004
		// debugging
		//
		//{
       		//draw_undi_graph dug(0, dopug);
	        //dug.write("no-simple-chains.gdt"); 
		//}
        // ### End import
   
   	//std::cout << "\nEXPANSION OF CHAINS OK!" << std::flush;
   
	
        // ### Begin import from GDT fork of W. Didimo on June, 18 2004
	// Added by W.D. on August 2002
	//
	// ----------------------- 4 --------------------------
	// Now, there are no more simple chains.
	// Absorbe all removable nodes of "dopug" such that 
	// (i) they are in the same super node; 
	// (ii) they have degree two (they are unused as pins);
	// (iii) they are not corners.
	// Also move each edge that is attached to a corner
	// and that is not contrained to be incident to that corner 
	// on a new dummy node close to the corner.
	// ----------------------------------------------------
	
	// debugging
	// ---------
	//{
	//gdtnode u;
	//forall_nodes (u,dopug)
	//   if (is_removable[u])
	//   	std::cout << "\n " << dopug.id(u) << " is_removable" << std::flush;
	//
	//forall_edges (e,dopug)
	//   if (removable_edge[e])
	//   	{
	//	int ide = dopug.id(e);
	//	std::cout << "\n removable edge : " << ide << std::flush;
	//	} 
	//	
	//}
	// ---------
	
	// absorbe nodes
	//
	gdt::gdtlist<gdtnode> temp_list;
	forall_nodes (v,dopug)
		temp_list.append(v);
		 
	forall (v,temp_list)
		{		
		gdtnode V = super_node[v];
		if (V && id(V) != dopug.id(v))
		  {
			if ( (dopug.degree(v) == 2) && (dopug.node_is_flat(v)) && (is_removable[v]) )
				{
				gdtedge e1 = dopug.first_adj_edge (v);
				gdtedge e2 = dopug.last_adj_edge (v);
				bool removable = false;
				if (removable_edge[e1] || removable_edge[e2])
					removable = true;				
				gdtedge e = dopug.weld_edges_at_node(v);
				removable_edge[e] = removable;
				}
				
		  }
		}
	
	//std::cout << "\nABSORBE OK!" << std::flush;
	
		//debugging
		//
		//{
		//draw_undi_graph dug(0,dopug);
		//dug.write("after-absorbe-1.gdt");
		//}
	
	// move edges at corners
	//
	gdt::gdtlist<gdtnode> corners = dopug.corners_of_super_nodes (*this,super_node,w,h);
	//list<edge> dummies;
	forall (v,corners)
		{
		gdtedge e;
		gdt::gdtlist<gdtedge> Lev;	// edges to move
		//
		forall_edges_adjacent_node (e,v,dopug)
			{
			gdtnode v1 = dopug.opposite(v,e);
			if (super_node[v1] != super_node[v]) 
				{
				border_step s = dopug.border_step_moving_along_edge (e,dopug.opposite(v,e));
				int pn_s = dopug.pin_number_of_border_step (s);
				if (pn_s == UNDEFINED_PIN_NUMBER)			
					Lev.append(e); // the edge must be moved 					
				}
			}
		
		forall (e,Lev)
			{
			int ide = dopug.id(e);
			gdtedge e_dummy_1 = dopug.move_edge_close_to_corner_of_super_node(e,v,super_node,is_zero_edge);
			//dummies.append(e_dummy);			
			removable_edge[e_dummy_1]=true;			
			gdtnode u = dopug.opposite (v,e_dummy_1);
			// u has degree three
			gdtedge e_dummy_2 = dopug.cyclic_adj_succ (e_dummy_1,u);
			if (dopug.id(e_dummy_2) == ide)
				{
				removable_edge[e_dummy_2]=false;
				e_dummy_2 = dopug.cyclic_adj_succ (e_dummy_2,u); 
				}
			removable_edge[e_dummy_2]=true;	
			}
		}


		// debugging
		// ---------
		//{
		//gdtedge e;
		//forall_edges (e,dopug)
		//	if (removable_edge[e])
		//		cout << "\nedge " << dopug.id(e) << " removable" << flush;
		//	else
		//	        cout << "\nedge " << dopug.id(e) << " is not removable" << flush;		
		//}	
		// ---------


			
		// debugging
		//
		//{
       		//draw_undi_graph dug(0, dopug);
	        //dug.write("before_one_dime_compaction.gdt"); 
		//dopug.print(COMPLETE);
		//cout << flush;
		//}
        // ### End import
	
	//std::cout << "\nMOVE EDGES AT CORNER OK!" << std::flush;
	
	// --------------------------- 5 ---------------------------
	// execute a sequence of one-dimensional-compactions in
	// order to minimize the total gdtedge length as much as 
	// possible (sub-optimal)
	// ---------------------------------------------------------	 

	bool exit = false;
	gdt::gdtmap<int,int> _min(0);	  // lower capacities
	gdt::gdtmap<int,int> _max(0);	  // upper capacities
	gdt::gdtmap<int,int> cost(0);	  // costs
	gdt::gdtmap<int,int> node_width(0);  // width of each gdtnode of the not expanded graph
	gdt::gdtmap<int,int> node_height(0); // height of each gdtnode of the not expanded graph
	
	int total_edge_length_before = 0;	// total gdtedge length before one dimensional compactions
	int total_edge_length_after  = 0;	// total gdtedge length after one dimensinal compactions
	

	// set costs, lower, and upper capacities 
	// --------------------------------------
	//

	int minimum_length= 1;
	// look for a MIN_TIEING constraint 
	     {
	       constraint c = NULL_CONSTRAINT;
	       bool found=false;
	       forall( c, dopug.all_constraints() ) 
		 {
		   if (c->get_type()== MIN_TIEING)
		     {
		       if (found) gdt_error("two MIN_TIEING constraints found");
			   constraint_min_tieing c1 = (constraint_min_tieing)c;
		       minimum_length= c1->get_value();
		       found=true;
		     }
		 }
	     }

	forall_edges (e,dopug)
		{
		int ide  = dopug.id(e);

		_min[ide]  = minimum_length;
		_max[ide]  = INFINITE;
		cost[ide] = 1;
                // ### Begin import from GDT fork of W. Didimo on June, 18 2004

		// TO BE DONE (comment by Walter Didimo 2002)
		// Questo codice è stato messo da Pizzo per eliminare le scalette formate in DBdraw
		// Esso però può modificare la forma e quindi andrebbe in generale messo opzionale.
		// In questo momento è disattivato. Per DBdraw va riattivato manualmente


		// ---------------------------------------------------------------------
		// cout << " minimum set on " << ide << " is " << min[ide] << endl;
		// put zero in min[ide] if we can stretch a pair of bends
		
		//gdtnode e_source= dopug.source(e);
		//gdtnode e_target= dopug.target(e);
		//if ( dopug.is_marked(e_source, RM3_REPLACES_A_BEND) && 
		//     dopug.is_marked(e_target, RM3_REPLACES_A_BEND) )
		//  {
		//  angle_type a1= dopug.angle_of_border_step( dopug.border_step_moving_along_edge( e, e_source ) );
		//  angle_type a2= dopug.angle_of_border_step( dopug.border_step_moving_along_edge( e, e_target ) );
		//  if ( a1==a2 ) _min[ide] =0;
		//  }
                //
		//else 
		// --------------------


                if (is_zero_edge[e]) 
			{
			_min[ide]  = 0;
			_max[ide]  = 0;
			}
                // ### End import

		total_edge_length_before += dopug.length_of_edge(e);
		}


        // ### Begin import from GDT fork of W. Didimo on June, 18 2004
	// Fix to zero the lower capacities for the removable edges.
	// These edges are always adjacent to nodes that are corners
	// of super-nodes.
	// ---------------------------------------------------------
	//
	//forall (e,dummies)
	forall_edges (e,dopug)
		if (removable_edge[e])
			{
			int ide = dopug.id(e);		
			//cout << "\n removable edge : " << ide << flush; 			
			_min[ide] = 0;
			}

        // ### End import


	// the following to be checked again 
	// (perhaps in a non degenerate metrics it could be simplified)
	
	forall_nodes (v,dopug)
		{
		gdtnode V = super_node[v];
		int idv = dopug.id(v);
		
		if (V && id(V) == idv) // v is the bottom left corner of the super-gdtnode
			{
			node_width [idv] = w[V];
			node_height[idv] = h[V];
			}
		else 	{
			node_width[idv]  = 0;
			node_height[idv] = 0;
			}		
		}

	// Debugging
	// ------------------
	//forall_edges (e,dopug)
	//	{
	//	int ide = dopug.id(e);
	//	int idv1 = dopug.id(dopug.source(e));
	//	int idv2 = dopug.id(dopug.target(e));
	//	std::cout << "\nedge " << ide << " (" << idv1 << "," << idv2 << ")" << std::flush;
	//	std::cout << "\n  min = " << _min[ide] << std::flush;
	//	std::cout << "\n  max = " << _max[ide] << std::flush;
	//	std::cout << "\n  cost= " << cost[ide] << std::flush;
	//	}
	// ------------------
	// END Debugging
	 		
	// compact recursively
	//
	//std::cout << "\nINIT POST-COMPACTION" << std::flush;
	int i = 0;
	while (!exit)
		{
		i++;		
		dopug.one_dimensional_tieing_for_expanded_nodes (_min,_max,cost,vertical,node_height,minimum_length);
		dopug.one_dimensional_tieing_for_expanded_nodes (_min,_max,cost,horizontal,node_width,minimum_length);
		total_edge_length_after = 0;
		forall_edges (e,dopug) total_edge_length_after += dopug.length_of_edge(e);
		
		//cout << "\ntotal gdtedge length before = " <<  total_edge_length_before << flush;
		//cout << "\ntotal gdtedge length after = " <<  total_edge_length_after << flush;
		
		if (total_edge_length_after < total_edge_length_before) 
					total_edge_length_before = total_edge_length_after;
		else    exit = true;
		}

	}

	
	
	
	bool
	dime_orth_plan_undi_graph::
edge_is_dummy
	(
	gdtedge e
	) const
	{
	if (!e) gdt_error("null gdtedge");
	return (is_marked(e, RM3_ADDED_BY_RECTANGULARIZATION));
	}
	

	
	
	bool
	dime_orth_plan_undi_graph::
edge_is_real
	(
	gdtedge e
	) const
	{
	if (!e) gdt_error("null gdtedge");
	return (!edge_is_dummy(e));
	}
	
	
	
	
	bool
	dime_orth_plan_undi_graph::
node_is_dummy
	(
	gdtnode n
	) const
	{
	if (!n) gdt_error("null gdtnode");
	return  (
		is_marked(n, RM3_ADDED_BY_RECTANGULARIZATION) ||
		is_marked(n, RM3_CROSS_ON_REAL_EDGE)          ||
		is_marked(n, RM3_REPLACES_A_BEND)             ||
		is_marked(n, RM3_BEND_NODE)
		);
	}
	
	
	
	
	bool
	dime_orth_plan_undi_graph::
node_is_real
	(
	gdtnode n
	) const
	{
	if (!n) gdt_error("null gdtnode");
	return (!node_is_dummy(n));
	}




	// -----------------
	// Update operations
	// -----------------


	// Description: split gdtedge 'e' by creating a new gdtnode on it.
	// Value: the gdtnode created.
	// Preconditions: gdtedge 'e' must have no bends.
	// Notes: -
	gdtnode
	dime_orth_plan_undi_graph::
new_node
	(gdtedge e, int new_id) 
	{
	//cout << "\n dime_orth_plan_undi_graph::new_node";
	gdtedge e1,e2;
	gdtnode v, v1,v2;
	int dist_v1_v2;
	border_step step_v1_v2;
	border_step step_v2_v1;
	border_step step_v1_v, step_v_v2;
	border_step step_v2_v, step_v_v1;
	heading_type head_v1_v2;
	heading_type head_v2_v1;

	v1  = source(e);
	v2  = target(e);
	
	dist_v1_v2  = length_of_edge (e);
	step_v1_v2  = border_step_moving_along_edge (e,v1);
	step_v2_v1  = border_step_moving_along_edge (e,v2);
	head_v1_v2  = initial_heading_of_border_step (step_v1_v2);
	head_v2_v1  = initial_heading_of_border_step (step_v2_v1);

	/* 
	//   \              
	//    \                      
	//     \             
	//    (v1)----e-----(v2)
	//                    \
	//                     \
	//                      \
	*/

	// Split the gdtedge e with a new gdtnode v
	// (an error occurs if e has bends)
	//
	v = orth_plan_undi_graph::new_node(e,new_id);

	//Added by A.M. 10 2001
	{
		border_step_info->undefine(step_v1_v2);
		border_step_info->undefine(step_v2_v1);
		edge_info->undefine(e);
	}
	//
	
	// Find the new edges e1,e2 resulting from split
	//
	e1 = first_adj_edge(v);
	e2 = last_adj_edge (v);
	if (opposite(v,e1) != v1)  {gdtedge ex=e1; e1=e2; e2=ex;} // Swap (e1,e2)

	// Find the new border_steps
	// along thenew edges e1,e2
	//
	step_v1_v = border_step_moving_along_edge (e1,v1);
	step_v_v2 = border_step_moving_along_edge (e2,v );
	step_v2_v = border_step_moving_along_edge (e2,v2);
	step_v_v1 = border_step_moving_along_edge (e1,v );		
			
	/* 
	//   \              
	//    \
	//     \   
	//    (v1)----e1-----(v)-----e2-----(v2)
	//                                    \
	//                                     \
	//                                      \
	*/
	
	// Set headings of the new border_steps
	//
	set_initial_heading_of_border_step (step_v1_v, head_v1_v2);
	set_initial_heading_of_border_step (step_v_v2, heading_after_border_step (step_v1_v));
	set_initial_heading_of_border_step (step_v2_v, head_v2_v1);
	set_initial_heading_of_border_step (step_v_v1, heading_after_border_step (step_v2_v));	

	// Set position of the new_node
	//
	if (!frozen_layout)
		if (dist_v1_v2<2)
			local_init();
		else
			{
			gdt::gdtpoint p1 = position_of_node (v1);
			gdt::gdtpoint p2 = position_of_node (v2);
			gdt::gdtpoint p (
				(int)( (p1.xcoord()+p2.xcoord())/2 ),
				(int)( (p1.ycoord()+p2.ycoord())/2 )
				);
			set_position_of_node (v,p);
			}
	return v; 
	}



	gdtnode
	dime_orth_plan_undi_graph::
new_node 
        (
        gdtedge e,
	gdtnode v,
	int  dist,
        int  new_id
        )
	{
	gdtnode u;
	
	//cout << "\n    adding vertex at distance " << dist << " from gdtnode " << id(v) << flush;   
	
	// ---------------------------
	// check all the preconditions
	// ---------------------------
	 
	if (!node_belongs_to_edge(v,e))
		gdt_error("invalid pair gdtnode,gdtedge");
	if (number_of_bends(e) > 0) 
		gdt_error("gdtedge has some bends");
		
	gdtnode  w  = opposite(v,e);
	gdt::gdtpoint pv = position_of_node (v);
	gdt::gdtpoint pw = position_of_node (w);
	
	border_step  sev = border_step_moving_along_edge  (e,v);
	heading_type dir = initial_heading_of_border_step (sev);
	
	int d = 0;
	switch (dir)
		{
		case north: d = (int)(pw.ycoord()-pv.ycoord()); break;
		case south: d = (int)(pv.ycoord()-pw.ycoord()); break;
		case east : d = (int)(pw.xcoord()-pv.xcoord()); break;
		case west : d = (int)(pv.xcoord()-pw.xcoord()); break;
		default   : break;
		}
		
	if (dist > d)
		gdt_error("no free space for adding gdtnode");
	
	
	// ---------------------------
	// add and return the new gdtnode
	// ---------------------------

	//Added by A.M.	
	undefine(e);	
	//
	u = orth_plan_undi_graph::new_node (e,new_id);	
	
	gdtedge evu, ewu;
	e = first_adj_edge (u);
	if (opposite(u,e) != v) 
		{
		evu = last_adj_edge (u);
		ewu = e;
		}
	else 
		{
		evu = e;
		ewu = last_adj_edge (u);
		}
	
	// set the headings of the new border steps
	//
	border_step svu = border_step_moving_along_edge (evu,v);
	border_step suw = border_step_moving_along_edge (ewu,u);

	set_initial_heading_of_border_step_pair (svu,dir);
	set_initial_heading_of_border_step_pair (suw,dir);
	
	// set the position of the new gdtnode
	//
	gdt::gdtpoint pu;
	switch (dir)	
		{
		case north: pu = gdt::gdtpoint(pv.xcoord(),pv.ycoord()+dist); break;
		case south: pu = gdt::gdtpoint(pv.xcoord(),pv.ycoord()-dist); break;
		case east : pu = gdt::gdtpoint(pv.xcoord()+dist,pv.ycoord()); break;
		case west : pu = gdt::gdtpoint(pv.xcoord()-dist,pv.ycoord()); break;
		default: break;
		}
	set_position_of_node (u,pu);
		
	return u;
	}	



	gdtnode 
	dime_orth_plan_undi_graph::
new_node 
	(
	gdtnode v,
	heading_type dir,
	int dist, 
	int new_node_id,
	int new_edge_id
	)
	{

	  /*
                 s_u_v
            v     <--
	    O-----evu----O u
	    |     -->
     s_e_v |e    s_v_u
	   v|
	    |
	    O

	  */

	gdtedge e = find_edge_leaving_node_with_heading (v,dir);
	if (e) gdt_error("dir is already used by an gdtedge");
	
	// -----------------------------
	// look for the first gdtedge pred. 
	// that will be inserted, and 
	// compute the angle between
	// such gdtedge and the new one.
	// -----------------------------
	
	heading_type d_e = dir;
	while (e == NULL_EDGE) 
		{
		d_e = heading_after_rotation (d_e,_090);
		e   = find_edge_leaving_node_with_heading (v,d_e);
		}
	
	border_step s_e_v = border_step_moving_along_edge (e,v);
	d_e = initial_heading_of_border_step (s_e_v);
	
	angle_type alpha = angle_required_to_change_heading(dir,d_e);
	
	
	// -----------------------------
	// add the new gdtnode and gdtedge and
	// set headings and positions
	// -----------------------------
	
	gdtnode u   = orth_plan_undi_graph::new_node(v,e,alpha,new_node_id,new_edge_id);
	gdtedge evu = first_adj_edge(u);
	
	border_step s_u_v = border_step_moving_along_edge (evu,u);
	border_step s_v_u = opposite_border_step (s_u_v);
	
	set_initial_heading_of_border_step_pair (s_v_u,dir);
	
	gdt::gdtpoint pv = position_of_node(v);
	gdt::gdtpoint pu;
	switch (dir)
		{
		case north: pu = gdt::gdtpoint(pv.xcoord(),pv.ycoord()+dist); break;
		case south: pu = gdt::gdtpoint(pv.xcoord(),pv.ycoord()-dist); break;
		case east : pu = gdt::gdtpoint(pv.xcoord()+dist,pv.ycoord()); break;
		case west : pu = gdt::gdtpoint(pv.xcoord()-dist,pv.ycoord()); break;
		default: gdt_error("invalid direction dir"); break;
		}
	
	set_position_of_node(u,pu);
	
	return u;
	}



	gdtedge
	dime_orth_plan_undi_graph:: 
new_straight_edge
	( gdtnode v, gdtnode w, int new_id )		
        {
	gdt::gdtpoint pv = position_of_node(v);
	gdt::gdtpoint pw = position_of_node(w);
	gdtedge  e_new = NULL_EDGE;
	
	int pv_x = (int)(pv.xcoord());
	int pv_y = (int)(pv.ycoord());
	int pw_x = (int)(pw.xcoord());
	int pw_y = (int)(pw.ycoord());
	
	//Debug	
	//std::cout << "new straight gdtedge between gdtnode " << id(v) << " and " << id(w) << std::endl;
	//std::cout << "pv_x="<<pv_x<< "pv_y="<<pv_y<< "pw_x="<<pw_x<< "pw_y="<<pw_y<<std::endl;
	//
	
	if (pv_x == pw_x)	
	  {
	    gdtnode high = NULL_NODE;
	    gdtnode low  = NULL_NODE;
	    if      (pv_y > pw_y) { high= v; low= w; }
	    else if (pv_y < pw_y) { high= w; low= v; }
	    else
	      { 
	      std::cout << "new straight gdtedge between gdtnode " << id(v) << " and " << id(w) << std::endl;
	      std::cout << "pv_x="<<pv_x<< "pv_y="<<pv_y<< "pw_x="<<pw_x<< "pw_y="<<pw_y<<std::endl;
	      gdt_error("nodes overlap");
	      }

	    gdtedge eh = NULL_EDGE;
	    angle_type ah = _090; // initialized to avoid a compilation warning
	    if ( (eh = find_edge_leaving_node_with_heading ( high, west )) )      ah=_090;
	    else if( (eh = find_edge_leaving_node_with_heading ( high, north )) ) ah=_180;
	    else if( (eh = find_edge_leaving_node_with_heading ( high, east )) )  ah=_270;
	    else gdt_error("highest between v and w, the south direction is busy (or disconnected!)");
	      
	    gdtedge el = NULL_EDGE;
	    angle_type al = _090; // initialized to avoid a compilation warning
	    if ( (el = find_edge_leaving_node_with_heading ( low, east )) )       al=_090;
	    else if( (el = find_edge_leaving_node_with_heading ( low, south )) )  al=_180;
	    else if( (el = find_edge_leaving_node_with_heading ( low, west )) )   al=_270;
	    else gdt_error("lowest between v and w, the north direction is busy (or disconnected!)");
	      

	//Debug 
	//{
	//draw_undi_graph dug(0,*this);
	//dug.write("beforese.gdt");
	//}
	//
	    e_new= split_face_with_shaped_edge ( high, eh, ah,
		                                 low,  el, al,
		                                 "", new_id );
	    border_step s_enew_high = border_step_moving_along_edge (e_new,high);
	    set_initial_heading_of_border_step_pair (s_enew_high,south);
	  }
	else if ( pv_y == pw_y ) 
	  {
	    gdtnode left  = NULL_NODE;
	    gdtnode right = NULL_NODE;
	    if      (pv_x > pw_x){ right= v; left= w; }
	    else if (pv_x < pw_x){ right= w; left= v; }
	    else gdt_error("nodes overlap");	
		
            gdtedge el = NULL_EDGE;   // gdtedge and angle for leftmost vertex
	    angle_type al = _090; // initialized to avoid a compilation warning
	    if ( (el = find_edge_leaving_node_with_heading ( left, south )) )      al=_090;
	    else if( (el = find_edge_leaving_node_with_heading ( left, west )) )   al=_180;
	    else if( (el = find_edge_leaving_node_with_heading ( left, north )) )  al=_270;
	    else gdt_error("leftmost between v and w, the west direction is busy (or disconnected!)");
	      
	    gdtedge er;  // gdtedge and angle for rightmost vertex
	    angle_type ar = _090; // initialized to avoid a compilation warning
	    if ( (er = find_edge_leaving_node_with_heading ( right, north )) )     ar=_090;
	    else if( (er = find_edge_leaving_node_with_heading ( right, east )) )  ar=_180;
	    else if( (er = find_edge_leaving_node_with_heading ( right, south )) ) ar=_270;
	    else gdt_error("rightmost between v and w, the south direction is busy (or disconnected!)");
	
		//Debug 
		//{
		//draw_undi_graph dug(0,*this);
		//dug.write("beforese.gdt");
		//}
		//
	    e_new= split_face_with_shaped_edge ( right, er, ar,
		                                 left,  el, al,
		                                 "", new_id );
	    border_step s_enew_left = border_step_moving_along_edge (e_new,left);
	    set_initial_heading_of_border_step_pair (s_enew_left,east);
	  }
	else
	  gdt_error("v and w should have either the same x coordinte or y coordinate");


	return e_new;
        }



	// Description  : Split face 'f' by adding a new gdtedge between 'v' and 'w'.
	// Precondition : ('v' and 'w' are vertices of the border of 'f') and ('v' != 'w')
	gdtedge
	dime_orth_plan_undi_graph:: 
new_edge 
	(gdtnode v, gdtnode w, face f, int new_id) 
	{
	gdtedge ev = NULL_EDGE,
	     ew = NULL_EDGE;
	gdt::gdtlist<gdtedge> le_vf = edges_entering_node_while_moving_around_face (v,f);
	gdt::gdtlist<gdtedge> le_wf = edges_entering_node_while_moving_around_face (w,f);
	
	if (!le_vf.empty()) ev = le_vf.head();
	if (!le_wf.empty()) ew = le_wf.head();
	return new_edge (v,ev, w,ew, new_id);
	}


	// Description  : Split face 'f' by adding a new gdtedge between 'v' and 'w'.
	// Precondition : ('v' and 'w' are vertices of the border of 'f') and ('v' != 'w')
	gdtedge
	dime_orth_plan_undi_graph:: 
new_edge
	(gdtnode v,gdtedge ev, gdtnode w,gdtedge ew, int new_id) 
	{
	//cout << "\n dime_orth_plan_undi_graph::new_edge";
	int nb;
	gdtedge e;
	border_step sv,svp;
	border_step sw,swp;
	
	e = orth_plan_undi_graph::new_edge(v,ev, w,ew, new_id);
	
	sv = border_step_moving_along_edge (e,v);
	sw = border_step_moving_along_edge (e,w);
	svp = cyclic_pred_border_step (sv);
	swp = cyclic_pred_border_step (sw);
	
	set_initial_heading_of_border_step (sv, heading_after_border_step(svp));
	set_initial_heading_of_border_step (sw, heading_after_border_step(swp));

	nb = number_of_bends(e);
	if (nb)
		{
		// Insert a position point for each bend
		// into the bends_position list of gdtedge e
		//
		int i;
		gdt::gdtpoint p(0,0);
		gdt::gdtlist<gdt::gdtpoint>& bpl = (*edge_info)[e].bends_position;
		bpl.clear(); for (i=1;i<=nb;i++) bpl.append(p);
		
		// Update the position of each bend
		// unless the freeze parameters is true.
		//
		if (!frozen_layout)
			local_init();	        
		}
		
	return e;
	}
	
	face
	dime_orth_plan_undi_graph:: 
del_node
	(gdtnode n)
	{
		//Added by Marcandalli 16 10 2001
		undefine(n);		
		//
		return orth_plan_undi_graph::del_node(n);
	}

	face
	dime_orth_plan_undi_graph:: 
del_edge
	(gdtedge e)
	{
		//Added by Marcandalli 8 2001
		undefine(e);		
		//
		return orth_plan_undi_graph::del_edge(e);
	}



	void 
	dime_orth_plan_undi_graph::
attach_nodes_by_chain
	(gdtnode v1, gdtnode v2, gdt::gdtlist<gdtnode>& N)
	{
	// -----------------------------------
	// check if the two nodes are in the
	// same column or in the same row,
	// and determine the correct direction
	// for attaching them, if possible.
	// -----------------------------------
	
	//
	//print(COMPLETE);
	//cout << "\n      attaching gdtnode " << id(v1) << " at position " << position_of_node(v1) << flush;
	//cout << "\n      with gdtnode      " << id(v2) << " at position " << position_of_node(v2) << flush;
	//
	
	heading_type dir = north; // this assignment is here to avoid a compilation warning
	int          dist = 0;    // this assignment is here to avoid a compilation warning
	
	int col_v1 = (int)(position_of_node(v1)).xcoord();
	int col_v2 = (int)(position_of_node(v2)).xcoord();
	int row_v1 = (int)(position_of_node(v1)).ycoord();
	int row_v2 = (int)(position_of_node(v2)).ycoord();

	if (col_v1 == col_v2)
		{ 
		if (row_v2 > row_v1) 	  {dir = north; dist = row_v2 - row_v1;}
		else if (row_v2 < row_v1) {dir = south; dist = row_v1 - row_v2;}
		else gdt_error("nodes overlap");
		}
	else if (row_v1 == row_v2)
		{
		if (col_v2 > col_v1)	  {dir = east; dist = col_v2 - col_v1;}
		else if (col_v2 < col_v1) {dir = west; dist = col_v1 - col_v2;}
		else gdt_error("nodes overlap");
		}	
	else gdt_error("nodes not aligned");
	
	
	// ----------------------------
	// attach the two nodes with a 
	// chain in the direction "dir"
	// ----------------------------

	int i;
	for (i=1; i<=dist-1; i++) 
		{
		v1 = new_node (v1,dir,1);	// attach gdtnode
		N.append(v1);
		}
	new_straight_edge (v1,v2);	
	}


	
	gdtedge 
	dime_orth_plan_undi_graph::
weld_edges_at_node 
	(gdtnode v)
	{
	gdt::gdtpoint pv;
	gdtnode v1,v2;
	gdtedge e1,e2;
	angle_type a_v1_v;
	border_step s_v1_v2;
	border_step s_v2_v1;
	border_step s_v1_v, s_v_v2;
	border_step s_v2_v, s_v_v1;
	heading_type h_v1_v2;
	heading_type h_v2_v1;
	heading_type h_v1_v;
	heading_type h_v2_v;
	gdt::gdtlist<gdt::gdtpoint> p_v1_v2;
	gdt::gdtlist<gdt::gdtpoint> p_v2_v1;
	gdt::gdtlist<gdt::gdtpoint> p_v1_v, p_v_v2;
	gdt::gdtlist<gdt::gdtpoint> p_v2_v, p_v_v1;
	
	if (degree(v)!=2) gdt_error("degree of gdtnode is not 2");

	e1 = first_adj_edge (v);
	e2 = last_adj_edge  (v);

	v1 = opposite(v,e1);
	v2 = opposite(v,e2);
	
	s_v1_v = border_step_moving_along_edge(e1,v1);
	s_v_v2 = border_step_moving_along_edge(e2,v );
	s_v2_v = border_step_moving_along_edge(e2,v2);
	s_v_v1 = border_step_moving_along_edge(e1,v );

	
	//  |                 
	//  |
	//  |
	//  |
	//  |    <--s_v_v1--        <--s_v2_v--        
	//(v1)--------e1------(v)--------e2-------(v2)
	//       --s_v1_v-->        --s_v_v2-->    |
	//                                         |
	//                  a_v1_v                 |
	//                                         |
	//                                         |
	

	// Save positions and headings along edges e1,e2
	// before they are replaced by the new gdtedge e.
	// 
	pv     = position_of_node (v);
	p_v1_v = position_of_bends_along_edge   (e1,v1);
	p_v_v2 = position_of_bends_along_edge   (e2,v);
	h_v1_v = initial_heading_of_border_step (s_v1_v);
	h_v2_v = initial_heading_of_border_step (s_v2_v);
	a_v1_v = angle_of_border_step (s_v1_v);

	// Join edges e1,e2 by replacing them
	// with a single new gdtedge e (v1,v2).
	//
	
	undefine(v);
	gdtedge e  = orth_plan_undi_graph::weld_edges_at_node(v);

	s_v1_v2 = border_step_moving_along_edge (e,v1);
	s_v2_v1 = border_step_moving_along_edge (e,v2);

	// Calculate the position of the bends
	// and the initial headings of the border_steps
	// along the new gdtedge e.
	//
	p_v1_v2 = p_v1_v;
	if (a_v1_v != _180) p_v1_v2.append (pv);
	p_v1_v2.conc (p_v_v2);
	h_v1_v2 = h_v1_v;
	h_v2_v1 = h_v2_v;
	
	// Apply the calculated positions and headings
	// to the bends and the border_steps
	// along the new gdtedge e.
	//
	set_position_of_bends_along_edge   (e,v1,p_v1_v2);
	set_initial_heading_of_border_step (s_v1_v2, h_v1_v2);
	set_initial_heading_of_border_step (s_v2_v1, h_v2_v1);

	return e;
	}	
	


	// -----------------------------------------------------
	// THE BODY OF THIS METHOD HAS BEEN REPLACED WITH THE
	// weld_edges_at_node ONE, WHICH IS MUCH FASTER   
	// -----------------------------------------------------
	/*	
	gdtedge
	dime_orth_plan_undi_graph:: 
replace_node_with_bend
	(gdtnode v)
	{
	//cout << "\n dime_orth_plan_undi_graph::replace_node_with_bend" << id(v) << flush;
	gdt::gdtpoint pv;
	gdtnode v1,v2;
	gdtedge e1,e2,e;
	angle_type a_v1_v;
	border_step s_v1_v2;
	border_step s_v2_v1;
	border_step s_v1_v, s_v_v2;
	border_step s_v2_v, s_v_v1;
	heading_type h_v1_v2;
	heading_type h_v2_v1;
	heading_type h_v1_v;
	heading_type h_v2_v;
	gdt::gdtlist<gdt::gdtpoint> p_v1_v2;
	gdt::gdtlist<gdt::gdtpoint> p_v2_v1;
	gdt::gdtlist<gdt::gdtpoint> p_v1_v, p_v_v2;
	gdt::gdtlist<gdt::gdtpoint> p_v2_v, p_v_v1;

	if (degree(v)!=2) gdt_error("degree of gdtnode is not 2");

	e1 = first_adj_edge (v);
	e2 = last_adj_edge  (v);

	v1 = opposite(v,e1);
	v2 = opposite(v,e2);
	
	s_v1_v = border_step_moving_along_edge(e1,v1);
	s_v_v2 = border_step_moving_along_edge(e2,v );
	s_v2_v = border_step_moving_along_edge(e2,v2);
	s_v_v1 = border_step_moving_along_edge(e1,v );

	
	//  |                 
	//  |
	//  |
	//  |             
	//  |    <--s_v_v1--        <--s_v2_v--        
	//(v1)----------------(v)-----------------(v2)
	//       --s_v1_v-->        --s_v_v2-->    |
	//                                         |
	//                  a_v1_v                 |
	//                                         |
	//                                         |
	

	// Save positions and headings along edges e1,e2
	// before they are replaced by the new gdtedge e.
	// 
	pv     = position_of_node (v);
	p_v1_v = position_of_bends_along_edge   (e1,v1);
	p_v_v2 = position_of_bends_along_edge   (e2,v);
	h_v1_v = initial_heading_of_border_step (s_v1_v);
	h_v2_v = initial_heading_of_border_step (s_v2_v);
	a_v1_v = angle_of_border_step (s_v1_v);

	// Join edges e1,e2 by replacing them
	// with a single new gdtedge e (v1,v2).
	//
	e = orth_plan_undi_graph::replace_node_with_bend(v);
	s_v1_v2 = border_step_moving_along_edge (e,v1);
	s_v2_v1 = border_step_moving_along_edge (e,v2);

	// Calculate the position of the bends
	// and the initial headings of the border_steps
	// along the new gdtedge e.
	//
	p_v1_v2 = p_v1_v;
	if (a_v1_v != _180) p_v1_v2.append (pv);
	p_v1_v2.conc (p_v_v2);
	h_v1_v2 = h_v1_v;
	h_v2_v1 = h_v2_v;
	
	// Apply the calculated positions and headings
	// to the bends and the border_steps
	// along the new gdtedge e.
	//
	set_position_of_bends_along_edge (e,v1,p_v1_v2);
	set_initial_heading_of_border_step (s_v1_v2, h_v1_v2);
	set_initial_heading_of_border_step (s_v2_v1, h_v2_v1);

	return e;
	}
	*/
	
	
		
	gdtedge
	dime_orth_plan_undi_graph::
replace_node_with_bend
	(gdtnode v)
	{
	return weld_edges_at_node(v);
	}
	
	
	// Added by W.D. on April 7
	//
	void 
	dime_orth_plan_undi_graph::
replace_bend_with_node   
	(	
	gdtedge e, 
	gdtnode v, 
	int b_pos, 
	gdt::gdtlist<gdtnode>& N, 
	gdt::gdtlist<gdtedge>& E
	)
	{
	gdtnode u = opposite (v,e);	// opposite of gdtnode v on gdtedge e
	border_step s_vu;		// border step of gdtedge e starting from v
	border_step s_uv;		// border step of gdtedge e starting from u
	
	s_vu = border_step_moving_along_edge (e,v);
	s_uv = border_step_moving_along_edge (e,u);
	
	/* ====================================
	        situation before replacing
	   ====================================
	
		      b_pos          <---- 
	 	         *----------* s_uv
	 	         |    e     |
	          	 |          |
	   *---------* 	        |
	   |    ---->	        *-------O u
	   |    s_vu
	   O 
	   v
	======================================== */
	
	gdtnode z = NULL_NODE;	        // gdtnode replacing bend
	gdtedge evz = NULL_EDGE;
	gdtedge euz = NULL_EDGE;        // edges derived by the splitting
	border_step s_vz;	        // border step of gdtedge evz starting from v
	border_step s_uz;		// border step of gdtedge euz starting from u 
		    
	/* =====================================
		situation after replacing
	   =====================================	
		
		         z          <---- 
	 	         O----------* s_uz
	 	    s_zv |     euz  |
	       <---- |          |  
       *---------*	 	    |  
   evz |    ---->		    *-------O u
       |    s_vz			---->
	   O				    s_zu
       v		
		
		
	   ===================================== */
	
	// -------------------------------------
	// save position and headings before the 
	// splitting is applied.
	// -------------------------------------
	
	gdt::gdtlist<gdt::gdtpoint>  bends_vu = position_of_bends_along_edge (e,v);	// bend positions from v to u
	gdt::gdtlist<gdt::gdtpoint>  bends_vz,						// bend positions from v to z
		     bends_zu;						// bend positions from z to u

	if (bends_vu.length() < b_pos) gdt_error("invalid bend position");
	//cout << "\nnumber of bends.." << bends_vu.length() << flush;

	gdt::gdtpoint b;							// position of the new gdtnode z
	int   b_num = (b_pos - 1);					// number of bend to replace with z
	
	gdt::gdtpoint p;
	int   pos = 0;
	forall (p,bends_vu)
		{
		if (pos < b_num) 	bends_vz.append(p);
		else if (pos > b_num)	bends_zu.append(p);
		else			b = p;
		pos ++;
		} 
		
	heading_type    hs_vu = initial_heading_of_border_step (s_vu);
	heading_type    hs_uv = initial_heading_of_border_step (s_uv); 
	
	// -------------------------------------
	// apply the splitting by using the
	// orth_plan_undi_graph method.
	// -------------------------------------
	
	// Added by Marcandalli 16 10 2001
	undefine(e); //Controllare
	//
	orth_plan_undi_graph::replace_bend_with_node (e,v,b_pos,N,E);

	z = N.tail();
	
	//cout << "\nadding a new gdtnode replacing bend : " << id(z) << flush;
	 
	gdtedge e1 = first_adj_edge(z);
	gdtedge e2 = last_adj_edge (z);
	if (opposite(z,e1) == v)
		{
		evz = e1;
		euz = e2;
		}
	else if (opposite(z,e2) == v)
		{
		evz = e2;
		euz = e1;
		}
	else gdt_error("invalid gdtedge");
		
	s_vz = border_step_moving_along_edge (evz,v);
	s_uz = border_step_moving_along_edge (euz,u);
		
	// ----------------------------------------
	// update the position of the new gdtnode
	// and the headings of the new border steps
	// ----------------------------------------
	
	set_position_of_node (z,b);
	set_initial_heading_of_border_step_pair (s_vz,hs_vu);
	set_initial_heading_of_border_step_pair (s_uz,hs_uv);
	set_position_of_bends_along_edge (evz,v,bends_vz);
	set_position_of_bends_along_edge (euz,z,bends_zu);
	}



	// Added by W.D. on April 7.
	//
	void
	dime_orth_plan_undi_graph::
replace_bends_with_nodes 
	(
	gdtedge e,
	gdt::gdtlist<gdtnode>& N, 
	gdt::gdtlist<gdtedge>& E
	)
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
		replace_bend_with_node (e,v1,1,N,E);
				
		// Find the two edges e1=(v1,v) and e2=(v,v2)
		// which have replaced gdtedge e=(v1,v2).
		//
		v  = N.tail();
		e2 = last_adj_edge(v);
		if (opposite(v,e2) != v2) e2 = first_adj_edge(v);
		
		//cout << "\nThe number of bends on gdtedge " << id(e2) << " is now " << number_of_bends(e2) << "\n" << flush;
		
		// Prepare for iteration
		//
		e  = e2;
		v1 = v;
		}
	}
	


	// Added by W.D. on April 7
	//
	void
	dime_orth_plan_undi_graph::
replace_bends_with_nodes
	(
	face f,
	gdt::gdtlist<gdtnode>& N,
	gdt::gdtlist<gdtedge>& E
	)
	{
	gdtedge e;
	gdt::gdtlist<gdtedge> border = adj_edges(f);
	forall (e, border) replace_bends_with_nodes (e,N,E);
	}



	// Added by W.D. on April 7
	//
	void
	dime_orth_plan_undi_graph::
replace_bends_with_nodes
	(
	gdt::gdtlist<gdtnode>& N,
	gdt::gdtlist<gdtedge>& E
	)
	{
	//cout << "\nLinearize.." << flush;
	gdtedge e = NULL_EDGE;
	gdt::gdtlist<gdtedge> initial_edges = all_edges();
	forall (e,initial_edges)
		replace_bends_with_nodes(e,N,E);
	}	


	void 
	dime_orth_plan_undi_graph::
update_node_and_bend_positions_according_to
	(
	dime_orth_plan_undi_graph& dopug
	)
	{
	gdtnode v = NULL_NODE, vc = NULL_NODE;
	gdtedge e = NULL_EDGE, ec = NULL_EDGE;
	
	forall_nodes (v,dopug)
		{
		vc = corresponding_node (v,dopug);
		set_position_of_node (vc,dopug.position_of_node(v));
		}
	forall_edges (e,dopug)
		{
		v  = dopug.source(e);
		ec = corresponding_edge (e,dopug);
		vc = corresponding_node (v,dopug);
		set_position_of_bends_along_edge (ec,vc, dopug.position_of_bends_along_edge(e,v));
		}
	}




	void
	dime_orth_plan_undi_graph::	
clear
	()
	{
	plan_undi_graph::clear();
	local_renew();
	}


			
	void
	dime_orth_plan_undi_graph::
steal_from
	(undi_graph& ug)
	{
	orth_plan_undi_graph::steal_from(ug);
	local_renew();
	local_init();
	}
	


	void
	dime_orth_plan_undi_graph::
steal_from 
	(plan_undi_graph& pug)
	{
	orth_plan_undi_graph::steal_from(pug);
	local_renew();
	local_init();
	}
	


	void
	dime_orth_plan_undi_graph::
steal_from 
	(orth_plan_undi_graph& opug)
	{
	//cout << "\n plan_undi_graph -(move)-> dime_orth_plan_undi_graph" << flush;
	clear();
	//
	// Copy all the pointers of opug
	// that is, mirror the orth_plan_undi part of this class
	//
	orth_plan_undi_graph::mirror (opug);
	//
	// Create a new empty local_environment for pug
	// in order to avoid side-effects with this
	//
	opug.forget ();
	//
	// Build the extra part (i.e. the orthogonal lists) of this,
	// according to the plan_undi part stolen from pug.
	// 
	local_renew();
	local_init();
	}
	
		
	
	void	
	dime_orth_plan_undi_graph::
mirror	
	(dime_orth_plan_undi_graph& dopug)
	{
	bool p1;
	heading_type p2;
	gdt::gdtnode_map<struct_dime_node_info>* p3;
	gdt::gdtedge_map<struct_dime_edge_info>* p4;
	gdt::gdtmap<border_step,struct_dime_border_step_info>* p5;
	orth_plan_undi_graph::mirror (dopug);
	dopug.local_get(p1,p2,p3,p4,p5);
	local_set(p1,p2,p3,p4,p5);
	}
	
	
	
	void
	dime_orth_plan_undi_graph::
forget
	()
	{
	orth_plan_undi_graph::forget();
	local_new();
	}
	
	 	

	void
	dime_orth_plan_undi_graph::	
remove_all_dummy_nodes_and_edges 
	()
	{
	gdtedge e;
	gdtnode v;
	gdt::gdtlist<gdtedge> dummy_edges, le;
	gdt::gdtlist<gdtnode> dummy_nodes1, dummy_nodes2, dummy_nodes3;
	gdt::gdtlist<gdtnode> bend_nodes;
	bool all_dummy_edges;

	// Find and remove all edges and nodes added
	// by rectangularization

	dummy_nodes1 = find_nodes_marked_as(RM3_ADDED_BY_RECTANGULARIZATION);
		
	
	forall (v, dummy_nodes1) 
		// if all_adj_edges_marked_as(v, RM3_ADDED_BY_RECTANGULARIZATION) del_node(v); LEONFI
		// else dummy_nodes2.append(v); LEONFI

		// Code replacing the two lines above
		// ----------------------------------
		{
		le = adj_edges(v);
		all_dummy_edges = true;
		forall(e,le)
			{
			if(!all_dummy_edges) break;
			else all_dummy_edges = all_dummy_edges && (is_marked(e,RM3_ADDED_BY_RECTANGULARIZATION));
			}
		if(all_dummy_edges) 
			{
			//cout << "\nDeleting dummy gdtnode " << id(v) << " with all adjacent dummy edges" << flush;
			del_node(v);
			}
		else dummy_nodes2.append(v);
		}
		// -----------------------------------
	
	//dummy_nodes3 = find_nodes_marked_as (RM3_CROSS_ON_DUMMY_EDGE);	
	
    	dummy_edges  = find_edges_marked_as (RM3_ADDED_BY_RECTANGULARIZATION);

	//cout << "\nDummy edges"<< flush;
	//forall(e,dummy_edges)
		//cout << " " << id(e) << flush;
		
	bend_nodes = find_nodes_marked_as(RM3_REPLACES_A_BEND);
	//cout << "\nBend nodes"<< flush;
	//forall(v,bend_nodes)
		//cout << " " << id(v) << flush;	


	forall (e, dummy_edges) 
		{
		//cout << "\nDeleting dummy gdtedge " << id(e) << flush;
		del_edge(e);
		}
	forall (v, dummy_nodes2) 
		{
		if (all_nodes().search(v))
			{
			//cout << "\nReplacing dummy gdtnode " << id(v) << flush; 
			replace_node_with_bend(v);
			}
		}
	forall (v, dummy_nodes3)  
		{
		//cout << "\nReplacing dummy cross gdtnode " << id(v) << flush; 
		replace_node_with_bend(v);
		}

	forall (v, bend_nodes)
		{
		//cout << "\nReplacing bend gdtnode " << id(v) << flush;
		replace_node_with_bend(v);
		}
	}



	// -------------------------
	// Input / output operations
	// -------------------------

	void
	dime_orth_plan_undi_graph::
print
	(print_mode mode, std::ostream& os) const
	{
	gdtedge e;
	gdt::gdtpoint p;
	gdtnode vs,vt;
	gdt::gdtlist<gdt::gdtpoint> bend_list;

	switch (mode)
		{
		case BY_NODES:
		case BY_EDGES:
			orth_plan_undi_graph::print(mode,os);
			break;
		case BY_FACES:
			{
			face f;
			forall_faces(f,(*this))
				{
				os << "\n" << std::flush;
				print (f,os);
				}
			}
			break;
		case BY_PATHS:
			{
			os << "\n";
			forall_edges(e,*this)
				{
				os << "\n";
				print(e);
				os << " : ";
				vs = source(e);
				vt = target(e);
				print(vs,os);
				os << " " << position_of_node (vs) << " - ";
				//forall (p,position_of_bends_along_edge (e,vs)) os << " " << p;
				bend_list = position_of_bends_along_edge (e,vs);
				forall (p,bend_list) os << " " << p;
					// os << " (" << p.xcoord() << "," << p.ycoord() << ")" << flush;
				os << " - " << position_of_node (vt) << " ";
				print(vt,os);
				}
			}
			break;
		case COMPLETE:
			{
			orth_plan_undi_graph::print(BY_NODES,os);
			orth_plan_undi_graph::print(BY_EDGES,os);
			print(BY_FACES,os);
			print(BY_PATHS,os);
			}
			break;
		default: break;
		}
	os << std::flush;
	}



	void
	dime_orth_plan_undi_graph::
print
	(gdtnode v, std::ostream& os) const
	{
	os << "(" << id(v) << ")" << std::flush;
	}



	void
	dime_orth_plan_undi_graph::
print
	(gdtedge e, bool path, std::ostream& os) const
	{
	orth_plan_undi_graph::print(e,os);
	if (path)
		{
		gdt::gdtpoint p;
 	  	gdt::gdtlist<gdt::gdtpoint> L = (*edge_info)[e].bends_position;
		os << id(source(e)) << position_of_node(source(e));
 		forall (p,L)  os << " " << p;
 		os << " " << position_of_node(target(e)) << id(target(e));
		}
	os << std::flush;
	}



	void
	dime_orth_plan_undi_graph::
print
	(face f, std::ostream& os) const
	{
	bend_type b;
	border_step s,ps;
	std::string bends, angle, heading;

	os << "<" << id(f) << ">";
	if (f==external_face()) os << " ext : ";
	else             os << " int : ";

	forall_face_border_steps(s,f)
		{
		// Init the bends string
		//
		bends = "";
		gdt::gdtlist<bend_type> B = bends_of_border_step(s);
		forall (b,B)
			if   (b == left) bends += "L";
			else   		 bends += "R";
		if (bends == "") bends = "-";

		// Init the angle string
		//
		ps = cyclic_pred_border_step(s);
		switch (angle_of_border_step(ps))
			{
			case _090: angle = "R"; break;
			case _180: angle = "-"; break;
			case _270: angle = "L"; break;
			case _360: angle = "B"; break;
			default: break;
			}

		// Init the heading string
		//
		switch (initial_heading_of_border_step(s))
			{
			case undefined_heading: heading = ""; break;
			case north    : heading = "(N)"; break;
			case east     : heading = "(E)"; break;
			case south    : heading = "(S)"; break;
			case west     : heading = "(W)"; break;
			}

		print(start_of_border_step(s));
		os << angle << heading << " -"<< bends << "- ";
		}
	os << std::flush;
	}
 
 
 
//-----------------------------------------------------------------------------
//	PRIVATE
//-----------------------------------------------------------------------------

	void
	dime_orth_plan_undi_graph::
local_new
	()
	{
	//cout << "\n dime_orth_plan_undi_graph::local_new" << flush;
	node_info = new (gdt::gdtnode_map<struct_dime_node_info>);
	edge_info = new (gdt::gdtedge_map<struct_dime_edge_info>);
	border_step_info = new (gdt::gdtmap<border_step,struct_dime_border_step_info>);
	node_info->init(*this);
	edge_info->init(*this);
	
	//
	// Arbitrarily choose
	// a reference border_step
	// and an arbitrary heading for it.
	//
	reference_heading = undefined_heading;
	}

	
	
	void
	dime_orth_plan_undi_graph::
local_del
	()
	{
	//cout << "\n dime_orth_plan_undi_graph::local_del" << flush;
	safe_delete (node_info);
	safe_delete (edge_info);
	safe_delete (border_step_info);
	}
	
	
	
	void
	dime_orth_plan_undi_graph::
local_renew
	()
	{
	local_del();
	local_new();
	}
	
	
	void
	dime_orth_plan_undi_graph::
local_init
	(algorithm_type alg)
	{
	//cout << "\n dime_orth_plan_undi_graph::local_init" << flush;
	int num_irr_faces;
	heading_type dir = undefined_heading;
	local_init (alg, dir, num_irr_faces);
	}
		
					
	void
	dime_orth_plan_undi_graph:: 
local_init
	(algorithm_type alg, heading_type dir, int& num_irr_faces)
	{
	//cout << "\n dime_orth_plan_undi_graph::local_init" << flush;
	gdtnode v;
	gdtedge e;
	gdt::gdtpoint dummy_point(0,0);
	gdt::gdtlist<gdt::gdtpoint> dummy_point_list;

	frozen_layout = false;

	forall_nodes (v,*this) (*node_info)[v].position = dummy_point;
	forall_edges (e,*this) (*edge_info)[e].bends_position = dummy_point_list;
	
	//cout << "\n dime_orth_plan_undi::local init - print(BY_FACES) and print(BY_EDGES) before init_headings_and_positions" << flush;
	//print(BY_FACES);
	//print(BY_EDGES);

	// ---------------------------------------------------------
	// translating EDGE_ATTACHMENT_WRT_PREVIOUS_CORNER constraint to
	// pin_number attributes for each border_step
	// ---------------------------------------------------------

        // ### Begin import from GDT fork of W. Didimo on June, 18 2004
	// Added by W.D. on August 2002
	  {
	  gdtedge e;
	  forall_edges (e,*this)
	  	{
		border_step s1 = border_step_moving_along_edge (e,source(e));
		border_step s2 = opposite_border_step (s1);
		set_pin_number_of_border_step (s1,UNDEFINED_PIN_NUMBER);
		set_pin_number_of_border_step (s2,UNDEFINED_PIN_NUMBER);
		}
	  }
        // ### End import

	constraint c;
	forall_constraints(c, *this)
		{
		if( type_of_constraint(c) == EDGE_ATTACHMENT_WRT_PREVIOUS_CORNER )
			{
			gdtedge e = ((constraint_edge_attachment_wrt_previous_corner)c)->get_ref_edge()  ;
			gdtnode n = ((constraint_edge_attachment_wrt_previous_corner)c)->get_ref_node()  ;
			int  a = ((constraint_edge_attachment_wrt_previous_corner)c)->get_attachment();
			
			border_step s = border_step_moving_along_edge(e,opposite(n,e));
			set_pin_number_of_border_step(s,a);
			}
		}

	del_constraints_type(EDGE_ATTACHMENT_WRT_PREVIOUS_CORNER);

	init_headings_and_positions(alg, dir, num_irr_faces);
	
	//cout << "\n dime_orth_plan_undi::local init - print(BY_FACES) and print(BY_EDGES) after init_headings_and_positions" << flush;
	//print(BY_FACES);
	//print(BY_EDGES);
	}	


	
	void
	dime_orth_plan_undi_graph::
init_headings_and_positions
	(algorithm_type alg, heading_type dir, int& num_irr_faces)
	{
	reference_heading = dir;	// set the direction for the reference border step
	switch (alg)
		{
		case FAST_COMPACTION:
		case FAST_COMPACTION_REFINED:
		case SLOW_COMPACTION:
		case SLOW_COMPACTION_REFINED:
			{
			init_headings_and_positions_with_rectangularization(alg);
			break;
			}
		case FAST_REGULAR_COMPACTION_1:
		case FAST_REGULAR_COMPACTION_2:
		case SLOW_REGULAR_COMPACTION_1:
		case SLOW_REGULAR_COMPACTION_2:
		case FAST_REGULAR_COMPACTION_1_REFINED:
		case FAST_REGULAR_COMPACTION_2_REFINED:
		case SLOW_REGULAR_COMPACTION_1_REFINED:
		case SLOW_REGULAR_COMPACTION_2_REFINED:
			{
			num_irr_faces = init_headings_and_positions_with_regularity(alg);
			break;
			}
		default: 
			gdt_error("invalid algorithm selected");
			break;
		}
	}

	
	
	void
	dime_orth_plan_undi_graph::
init_headings_and_positions_with_rectangularization
	(algorithm_type alg)
	{
	//cout << "\n dime_orth_plan_undi_graph::init_headings_and_positions\n" << flush;
	
	dire_graph DAG;                   // Direct Acyclic Graph holding the horizontal/vertical structure of *this;

	gdtnode v,v1,v2,w;                   // Nodes in *this
	gdtedge e;                           // Edge in *this
	border_step s;                    // Border_step is *this
	gdt::gdtlist<gdt::gdtpoint> ebp;                  // List of bend-points along a *this gdtedge
	gdt::gdtlist<gdtnode> chain;                 // Set of nodes in *this, aligned horizontally/vertically

	gdtnode V,V1,V2;                     // Nodes in DAG (each of them correspond to a chain)
	gdtedge E;                           // Edge in DAG


	//typedef gdt::gdtlist<gdtnode>    node_list;
	gdt::gdtnode_array<gdtnode>        DAG_node;       	 			// Correspondence (nodes in *this) --> (nodes in DAG).
	//gdt::gdtnode_map<node_list>   nodes_in_DAG_node (DAG);	// Correspondence (DAG_node)	   --> (list of nodes in *this)	
	
	gdt::gdtlist<gdtnode> bend_nodes;            // Nodes temporarily replacing a bend in *this
	gdt::gdtlist<gdtedge> splitted_edges;        // Edges splitted in *this when replacing bends with nodes

	gdt::gdtnode_array<int>  X,Y;             // Temporary storage for the coordinates of nodes in *this
	gdt::gdtlist<gdtnode> dummy_nodes;           // Nodes temporarily added to *this during face decomposition
	gdt::gdtlist<gdtedge> dummy_edges;           // Edges temporarily added to *this during face decomposition

	int i;                             		     // Counter for the two (horizontal and vertical) cycles.
	slope_type   node_scan_slope = undefined_slope;      // Horizontal/vertical selector
	slope_type   edge_scan_slope = undefined_slope;      // Horizontal/vertical selector
	heading_type edge_scan_heading = north;    	     // Horizontal/vertical heading for edges scanning

	//cout << "\n dime_orth_plan_undi:: init_headings_and_positions - print(COMPLETE) before replace....." << flush;
	//print(COMPLETE);
	
	
	// ---------------------- 0 ------------------------
	// Merge all edges forming a zero angle on a vertex,
	// in order to make the graph 4-planar
	// -------------------------------------------------
	
	// This code must be replaced by the below one, for
	// handling a model with zero angles on no 4-vertices
	// also.
	// 
	//gdt::gdtlist<gdtnode> high_degree_nodes;
	//forall_nodes (v,*this)
	//	if (degree(v) > 4) high_degree_nodes.append(v);
	//forall (v,high_degree_nodes) merge_edges_on_same_side (v);
	//
	
	// NEW CODE
	// --------
	
	gdt::gdtlist<gdtnode> Lv = all_nodes();
	forall (v,Lv) merge_edges_on_same_side (v);
		
	// -----------------
	
	// CHECKING
	// 
	//if (!is_orthogonal()) 
	//	gdt_error("not orthogonal obtained");
	
	// ---------------------- 1 -----------------------
	// Rectangularize the orthogonal representation and
	// compute the heading for each gdtedge
	// ------------------------------------------------

	
	// --------------------- 1.1 ----------------------
	// Linearize graph, that is replace each bend with 
	// a gdtnode
	// ------------------------------------------------
	
	// Modified by W.D. on April 7
	//
	orth_plan_undi_graph::replace_bends_with_nodes (bend_nodes, splitted_edges);  //cout << "\nREPLACED BENDS WITH NODES..\n" << flush; print(COMPLETE);
	
	// --------------------- 1.2 ----------------------
	// Decompose all faces into rectangular regions 
	// ------------------------------------------------
	
	decompose_all_faces_into_smaller_rectangular_faces (dummy_nodes,dummy_edges); //cout << "\nDECOMPOSED FACES..\n" << flush; print(COMPLETE);
	
	// --------------------- 1.3 ----------------------
	// Assign heading for each border step
	// ------------------------------------------------
	
	init_headings ();							      //cout << "\nORIENTED GRAPH..\n" << flush; //print(COMPLETE);
	
	//
	
	//cout << "\nDUMMY EDGES : "    << "\n" << flush; forall (e,dummy_edges)    cout << id(e) << " ";
	//cout << "\nDUMMY NODES : "    << "\n" << flush; forall (v,dummy_nodes)    cout << id(v) << " ";
	//cout << "\nBEND NODES  : "    << "\n" << flush; forall (v,bend_nodes)     cout << id(v) << " ";
	//cout << "\nSPLITTED_EDGES : " << "\n" << flush; forall (e,splitted_edges) cout << id(e) << " "; print(COMPLETE);
	
	
	// ---------------------- 2 -----------------------
	// Execute a compaction algorithm on the 
	// rectangularized graph obtained from the step 1
	// ------------------------------------------------
	
	X.init        (*this,0);
	Y.init        (*this,0);
	DAG_node.init (*this,NULL_NODE);

	for (i=1; i<=2; i++)
		{
		// Translate cycle counter into slope
		//
		switch(i)
			{
			case 1:
				node_scan_slope = horizontal;
				edge_scan_slope = vertical;
				edge_scan_heading = north;
 				break;
			case 2 :
				node_scan_slope = vertical;
				edge_scan_slope = horizontal;
				edge_scan_heading = east;
				break;
			}
						
		DAG.clear();
		DAG_node.init(*this,NULL_NODE);

		forall_nodes (v,*this)
			if (!DAG_node[v])
				{
				//cout << "\nScanning gdtnode " << id(v) << flush;
				V = DAG.new_node();
				//nodes_in_DAG_node[V].clear();
				chain = nodes_reachable_moving_from_node_with_slope (v,node_scan_slope);
				forall (w,chain) 
					{
					DAG_node[w]=V;
					//nodes_in_DAG_node[V].append(w);
					}
				}
		forall_edges (e,*this)
			if (slope_of_edge(e)==edge_scan_slope)
				{
				s = border_step_moving_along_edge (e,source(e));
				if (initial_heading_of_border_step (s) != edge_scan_heading) s=opposite_border_step(s);
				
				v1 = start_of_border_step(s);
				v2 = stop_of_border_step (s);
				
				//cout << "\nScanning gdtedge " << id(e) << " from " << id(v1) << " to " << id(v2) << flush;

				V1 = DAG_node[v1];
				V2 = DAG_node[v2];
				
				if (V1==V2) gdt_error("malformed graph");
				
				E  = DAG.find_directed_edge (V1,V2);
				if (!E) E=DAG.new_edge(V1,V2,id(e));	//make an gdtedge in DAG with the same id of the gdtedge in *this
					
				}
		
		if (DAG.number_of_nodes()==1)
			switch (edge_scan_slope)
				{
				case horizontal: {forall_nodes (v,*this) X[v]=0;} break;
				case vertical:   {forall_nodes (v,*this) Y[v]=0;} break;
				default: break;
				}
		else
			{
			gdt::gdtedge_array<int>  length     (DAG,1);
			gdt::gdtnode_array<int>  distance   (DAG);
			gdt::gdtnode_array<gdtedge> pred_edge  (DAG);

			V = DAG.find_source();
			DAG.find_longest_path (V, length, distance, pred_edge);

			//cout << "\nDAG : "; DAG.print(COMPLETE); cout << "\n" << flush; 
			//cout << "\nThe source of this DAG is " << DAG.id(V); DAG.print(COMPLETE);
			
			switch (alg)
				{
				case FAST_COMPACTION:
				case FAST_COMPACTION_REFINED:
				      	{
					switch (edge_scan_slope)
						{
						case horizontal: 
							{
							forall_nodes (v,*this) X[v]=distance[DAG_node[v]];
							//cout << "\nEXECUTED HORIZONTAL FAST COMPACTION" << flush;
							} 
							break;
						case vertical  : 
							{
							forall_nodes (v,*this) Y[v]=distance[DAG_node[v]];
							//cout << "\nEXECUTED VERTICAL FAST COMPACTION" << flush;
							} 
							break;
						default: break;
						}		      	
				      	}
				      	break;
				
				case SLOW_COMPACTION:
				case SLOW_COMPACTION_REFINED:
				      	{
				      	// -----------------------------------
				      	// store the id of the edges of the 
				      	// longest path into a list L
				      	// -----------------------------------
	
				      	gdt::gdtlist<gdtedge> L;
				      	V1 = DAG.find_sink(); 
										
					while (V1 != V)
						{
						E = pred_edge[V1];
						L.append(find_edge(DAG.id(E)));
						V1 = DAG.opposite(V1,E);
						}			
					
					// -----------------------------------
					// compute the length of all edges, by
					// using a min-cost-flow algorithm
					// -----------------------------------
					
					flow_dire_graph fn;
					gdt::gdtedge_map<gdtedge>  fn_edge (*this,NULL_EDGE);	// *this-gdtedge --> fn-gdtedge
					build_orthogonal_flow_network (fn, fn_edge, L, edge_scan_slope);
					fn.min_cost_flow ();
										
					// ----------------------------------------
					// assign coordinates using flow values and
					// a topological sorting of the DAG nodes
					// ----------------------------------------
					
					gdt::gdtnode_array<int>  X_DAG (DAG,0),
							 Y_DAG (DAG,0);
					gdt::gdtnode_array<bool> reached (DAG,false);
					gdt::gdtlist<gdtnode> 	 order;
					V1 = DAG.find_source();
					DAG.topological_sort (V1,reached,order);
					
					forall (V,order)
						forall_edges_leaving_node (E,V,DAG)
							{
							e = corresponding_edge(E,DAG);
							gdtedge efn = fn_edge[e];
							int  len = fn.get_flow(efn);
							V1 = DAG.opposite(V,E);
							switch (edge_scan_slope)
								{
								case horizontal: X_DAG[V1] = X_DAG[V] + len; break;
								case vertical  : Y_DAG[V1] = Y_DAG[V] + len; break;
								default: break; 
								}
							}
						
					if (edge_scan_slope == horizontal)
						{
						forall_nodes (v, *this)
							{
							V = DAG_node[v];
							X[v] = X_DAG[V];
							//cout << "\nX[" << id(v) << "] = " << X[v] << flush;
							}
						//cout << "\nHORIZONTAL SLOW COMPACTION EXECUTED" << flush;
						}
					else //vertical
						{ 
						forall_nodes (v, *this)
							{
							V = DAG_node[v];			   
							Y[v] = Y_DAG[V];
							//cout << "\nY[" << id(v) << "] = " << Y[v] << flush;
							}
						//cout << "\nVERTICAL SLOW COMPACTION EXECUTED" << flush;	
						}
					}
					break;
						
				default: break;
				}
			}
		}

	forall_nodes (v,*this) set_position_of_node (v,gdt::gdtpoint(X[v],Y[v]));
	forall (e, dummy_edges) mark(e, RM3_ADDED_BY_RECTANGULARIZATION);
	forall (v, dummy_nodes) mark(v, RM3_ADDED_BY_RECTANGULARIZATION);
	forall (v, bend_nodes)  mark(v, RM3_REPLACES_A_BEND);
	}



	int 
	dime_orth_plan_undi_graph::
init_headings_and_positions_with_regularity
	(algorithm_type alg)
	{
	//cout << "\n dime_orth_plan_undi_graph::init_headings_and_positions_with_regularity\n" << flush;
	
	int  num_irr_faces = 0;		  // number of irregular faces
	
	gdtnode v;                           // Node in *this
	gdtedge e;                           // Edge in *this
	gdt::gdtlist<gdt::gdtpoint> ebp;                  // List of bend-points along a *this gdtedge

	gdt::gdtlist<gdtnode> bend_nodes;            // Nodes temporarily replacing a bend in *this
	gdt::gdtlist<gdtedge> splitted_edges;        // Edges splitted in *this when replacing bends with nodes

	gdt::gdtlist<gdtnode> dummy_nodes;           // Nodes temporarily added to *this during face decomposition
	gdt::gdtlist<gdtedge> dummy_edges;           // Edges temporarily added to *this during face decomposition

	//cout << "\n dime_orth_plan_undi:: init_headings_and_positions - print(COMPLETE) before replace....." << flush;
	//print(COMPLETE);
	
	
	// ---------------------- 0 ------------------------
	// Merge all edges forming a zero angle on a vertex,
	// in order to make the graph 4-planar
	// -------------------------------------------------

	// This code must be replaced by the below one, for
	// handling a model with zero angles on no 4-vertices
	// also.
	// 
	//gdt::gdtlist<gdtnode> high_degree_nodes;
	//forall_nodes (v,*this)
	//	if (degree(v) > 4) high_degree_nodes.append(v);
	//forall (v,high_degree_nodes) merge_edges_on_same_side (v);
	//
	
	// NEW CODE
	// --------
	
	gdt::gdtlist<gdtnode> Lv = all_nodes();
	forall (v,Lv) merge_edges_on_same_side (v);
		
	// -----------------
	
	// CHECKING
	//
	//if (!is_orthogonal()) 
	//	gdt_error("not orthogonal obtained");
		
	// ---------------------- 1 -----------------------
	// Check regularity for each face of the orthogonal
	// ------------------------------------------------
	
	// --------------------- 1.1 ----------------------
	// Linearize graph, that is replace each bend with 
	// a gdtnode
	// ------------------------------------------------
		
	// Modified by W.D. on April 7
	//
	orth_plan_undi_graph::replace_bends_with_nodes (bend_nodes, splitted_edges); 

	// --------------------- 1.2 ----------------------
	// Regularize all faces of the orthogonal  
	// ------------------------------------------------
		
	num_irr_faces = regularize_all_faces (dummy_nodes, dummy_edges, alg);
		
	// ---------------------- 2 -----------------------
	// Assign heading for each border step
	// ------------------------------------------------
		
	init_headings ();	
		
	//cout << "\n\n DUMMY ELEMENTS : \n" << flush;
	//cout << "\n - DUMMY EDGES    : " << "\n" << flush; forall (e,dummy_edges)    cout << id(e) << " ";
	//cout << "\n - DUMMY NODES    : " << "\n" << flush; forall (v,dummy_nodes)    cout << id(v) << " ";
	//cout << "\n - BEND NODES     : " << "\n" << flush; forall (v,bend_nodes)     cout << id(v) << " ";
	//cout << "\n - SPLIT_EDGES : " << "\n" << flush; forall (e,splitted_edges) cout << id(e) << " "; 
	//cout << flush;
	
	// ---------------------- 3 -----------------------
	// Construct the embedded planar posets, horizontal
	// and vertical, associated to the orthogonal repr.
	// The two posets are two satureted regular upward 
	// planar graphs.
	// ------------------------------------------------
	
	plan_undi_graph pug_v, 	// vertical poset 
		        pug_h;	// horizontal poset
	
	gdt::gdtnode_map<gdtnode>  node_in_pug_v (*this,NULL_NODE),
			node_in_pug_h (*this,NULL_NODE);

	build_embedded_posets (pug_v, pug_h, node_in_pug_v, node_in_pug_h);
	
		
	// ---------------------- 4 -----------------------
	// Determine coordinates of all nodes of the
	// orthogonal representation, using the specified
	// algorithm (either FAST or SLOW).
	// ------------------------------------------------
		
	gdt::gdtnode_array<int> X (*this,0);	// x coordinates
	gdt::gdtnode_array<int> Y (*this,0);	// y coordinates
	
	slope_type slope = vertical;
	
	for (int i=1; i<=2; i++)
		{
		dire_graph DAG;
		gdtnode V,V1;
		if 	(slope == vertical) DAG = pug_v;
		else			    DAG = pug_h;	
		
		gdt::gdtedge_array<int>  length     (DAG,1);
		gdt::gdtnode_array<int>  distance   (DAG);
		gdt::gdtnode_array<gdtedge> pred_edge  (DAG);

		V  = DAG.find_source();
		V1 = DAG.find_sink();
		DAG.find_longest_path (V, length, distance, pred_edge);
			
		switch (alg)
			{
			case FAST_REGULAR_COMPACTION_1:
			case FAST_REGULAR_COMPACTION_2:
			case FAST_REGULAR_COMPACTION_1_REFINED:
			case FAST_REGULAR_COMPACTION_2_REFINED:
				{
				switch (slope)
					{
					case horizontal: 
						{
						forall_nodes (v,*this)
							{
							gdtnode u = node_in_pug_h[v];
							u = DAG.corresponding_node(u,pug_h);
							if (!distance[u]) X[v] = 0;
							else 		  X[v] = distance[u] - 1;
							}
						//cout << "\nHORIZONTAL FAST COMPACTION EXECUTED" << flush;
						} 
						break;
					case vertical  : 
						{
						forall_nodes (v,*this) 
							{
							gdtnode u = node_in_pug_v[v];
							u = DAG.corresponding_node(u,pug_v);
							if (!distance[u]) Y[u] = 0;
							else		  Y[v] = distance[u] - 1;
							}
						//cout << "\nVERTICAL FAST COMPACTION EXECUTED" << flush;
						} 
						break;
					default: break;
					}		      	
				}
				break;
				
			case SLOW_REGULAR_COMPACTION_1:
			case SLOW_REGULAR_COMPACTION_2:
			case SLOW_REGULAR_COMPACTION_1_REFINED:
			case SLOW_REGULAR_COMPACTION_2_REFINED:
				{
				plan_undi_graph* pugp;
				if (slope == vertical)  pugp = &pug_v;
				else		        pugp = &pug_h;
				plan_undi_graph& pug = *pugp;
				
				// -------------------------------------
				// since the graph pug is an st-digraph,
				// we can directly apply the visibility 
				// representation (compacted) algorithm,
				// setting to zero the cost on edges 
				// added by saturation or regulzrization
				// heuristic.
				// -------------------------------------
			
				gdtnode vs = pug.corresponding_node(V ,DAG);
				gdtnode vt = pug.corresponding_node(V1,DAG);
				
				if (vs == vt) 
					{					
					if (slope == horizontal) X[vs] = 0;
					else Y[vs] = 0;				
					}
				else
					{
					face f;
					face st_face = NULL_FACE;
					gdtedge e_st = NULL_EDGE;
				
					forall_faces (f,pug)
						if (pug.node_belongs_to_face(vs,f) && 
						    pug.node_belongs_to_face(vt,f)) {st_face = f; break;}
				
					if (st_face)
						{		
						e_st = pug.new_edge(vs,vt,st_face);				
						pug.make_directed(e_st,vs);
						}
					else 
						gdt_error("not st-face present");
					
					
					gdt::gdtnode_array<horizontal_segment>  seg_node (pug)  ;
					gdt::gdtedge_array<vertical_segment>	seg_edge (pug)  ;
					gdt::gdtedge_array<int>			cost	 (pug,1);	

					forall_edges (e,pug)
						if (pug.is_marked(e,RM3_ADDED_BY_REGULARIZATION)) cost[e]=0;
				
					pug.compute_visibility_representation (seg_node,seg_edge,e_st,true,&cost);
				
					forall_nodes (v,*this)
						{
						switch (slope)
							{
							case horizontal: 
								{
								forall_nodes (v,*this) 
									{
									gdtnode u = node_in_pug_h[v];
									X[v]=seg_node[u].y - 1;
									}
								//cout << "\nHORIZONTAL SLOW COMPACTION EXECUTED" << flush;
								} 
								break;
							case vertical  : 
								{
								forall_nodes (v,*this) 
									{
									gdtnode u = node_in_pug_v[v];
									Y[v]=seg_node[u].y - 1;
									}
								//cout << "\nVERTICAL SLOW COMPACTION EXECUTED" << flush;
								} 
								break;
							default: break;
							}		     
						}
					break;
					}
				}
				
			default: break;
			}
			
		slope = horizontal;
		}
	
	forall_nodes (v,*this)
		set_position_of_node (v,gdt::gdtpoint(X[v],Y[v]));
	
	
	// CHECKER ON OVERLAPPING 
	/* 
		{
		gdtnode v,u;
		forall_nodes (v,*this)
			forall_nodes (u,*this)
				if (u != v)
					if (X[v]==X[u] && Y[v]==Y[u])
						{
						cout << "\ncollision between gdtnode " << id(u) << " and gdtnode " << id(v) << flush;
						cout << "\n DIME ORTH : " << flush;
						print(COMPLETE); cout << flush;
						gdt_error("overlapping");
						}
		}
	
	// END CHECK
	*/
	
	forall (e, dummy_edges) mark(e, RM3_ADDED_BY_RECTANGULARIZATION);
	forall (v, dummy_nodes) mark(v, RM3_ADDED_BY_RECTANGULARIZATION);
	forall (v, bend_nodes)  mark(v, RM3_REPLACES_A_BEND);
	
	return num_irr_faces;
	}



	void
	dime_orth_plan_undi_graph::
build_embedded_posets 
	(
	plan_undi_graph& pug_v, 
	plan_undi_graph& pug_h,
	gdt::gdtnode_map<gdtnode>&  node_in_pug_v,
	gdt::gdtnode_map<gdtnode>&  node_in_pug_h
	) 	
	{
	//cout << "\n\tSTART POSETS COMPUTATION.." << flush;
	
	gdtnode v1,v2,v,u;
	gdtedge e;
	int  idu;
	heading_type h;
	slope_type s;
	
	// ----------------------------------------------------------------
	// STEP 1 - temporarily orient all the vertical edges from south to 
	//	    north and all the horizontal gdtedge from west to east,
	//	    saving in an array the old direction, to restore them
	//	    at the end of the computation.
	// ----------------------------------------------------------------
	
	//cout << "\n\tSTEP 1 - ORIENTING EGDES.." << flush;
	
	gdt::gdtedge_array<gdtnode> old_start (*this);
	
	forall_edges (e,*this)
			{
			old_start[e] = start(e);
			v1 = source(e);
			v2 = opposite(v1,e);
			h  = position_of_edge_with_respect_to_node (e,v1);
			
			if 	(h == north || h == east) make_directed (e,v1);
			else if (h == south || h == west) make_directed (e,v2);
			else gdt_error("internal heading error");
			}
	
	//cout << "\n\tEDGES ORIENTED.." << flush;
	//cout << "\n\tORTHGONAL AFTER ORIENTATION: " << flush;
	//print(COMPLETE); cout << flush;
	
	
	s = vertical;
	
	for (int i = 1; i<=2; i++)
		{
		//if (s == vertical) cout << "\n\tCOMPUTING VERTICAL POSET.." << flush;
		//else		     cout << "\n\tCOMPUTING HORIZONTAL POSET.." << flush;
		
		// ------------------------------------------------------------------
		// STEP 2 - Find all horizontal/vertical chains and for each of 
		//	    them store:
		// 
		//	(a) the ordered list (west->east / north->south) of its nodes
		//	(b) the ordered list (west->east / north->south) of its edges
		//	(c) the west-most/north-most vertical/horizontal out-gdtedge
		//	(d) the west-most/north-most vertical/horizontal in-gdtedge
		//
		// Each chain is represented by an integer that is the 
		// identifier of the west-most gdtnode of the chain.
		// ------------------------------------------------------------------
		
		//cout << "\n\tSTEP 2 - FINDING CHAINS " << flush;
		
		gdt::gdtlist<int> 	      chains;	// horizontal/vertical chains
		gdt::gdtmap<int,gdt::gdtlist<gdtnode> > N;	// ordered list of the nodes in the chain
		gdt::gdtmap<int,gdt::gdtlist<gdtedge> > E;	// ordered list of the edged in the chain
		gdt::gdtmap<int,gdtedge> e_in(NULL_EDGE);	// first vertical in-gdtedge
		gdt::gdtmap<int,gdtedge> e_out(NULL_EDGE);	// first vertical out-gdtedge

		gdt::gdtnode_map<bool> marked (*this,false);
		slope_type     s_opp = opposite_slope(s);
		
		forall_nodes (v,*this)
			{
			if (!marked[v])
				{
				gdt::gdtlist<gdtnode> node_chain;
				gdt::gdtlist<gdtedge> edge_chain;
				node_chain = nodes_reachable_moving_from_node_with_slope (v,s_opp,edge_chain);
				u   = node_chain.head();	// west-most/north-most gdtnode of the chain
				idu = id(u);
				chains.append(idu);
				N[idu] = node_chain;
				E[idu] = edge_chain;
				
				// -------------------------------------------------------
				// look for e_in and e_out and mark all nodes of the chain
				// -------------------------------------------------------

				e_in [idu] = NULL_EDGE;
				e_out[idu] = NULL_EDGE;
				
				heading_type h_in, h_out;
				
				if (s == vertical) {h_in = south; h_out = north;}
				else		   {h_in = west ; h_out = east ;}
				
				forall (u,N[idu])
					{
					marked[u] = true;
					if (!e_in[idu])  e_in[idu] = find_edge_leaving_node_with_heading(u,h_in);
					if (!e_out[idu]) e_out[idu]= find_edge_leaving_node_with_heading(u,h_out);
					}
				}
			}
		
		// ---------------------------------------------------------------
		// STEP 3 - build an undi_graph ug, by contracting all the edges
		//	    in the chains found above.
		// ---------------------------------------------------------------
		
		//cout << "\n\tSTEP 3 - CONTRACTING EDGES IN CHAINS.." << flush;
		
		undi_graph ug (*this);
		ug.del_all_constraints();
		
		int c;

		forall (c,chains)
			{			
			u = NULL_NODE;
			forall (e,E[c])
				{				
				gdtedge e_ug = ug.corresponding_edge(e, *this);											
				u = ug.contract(e_ug);				
				}
			if (u && ug.id(u) != c) ug.assign_id(u,c);
			}
		
		//cout << "\n\tUNDI_GRAPH AFTER CONTRACTION : " << flush;
		//ug.print(COMPLETE); cout << flush;
		
		
		// -----------------------------------------------------------------
		// STEP 4 - build an embedded upward planar according to the 
		//          candidate lists induced by the e_in and e_out edges for 
		//	    each gdtnode u of *pugp, and saturate it.
		// -----------------------------------------------------------------
		
		//cout << "\n\tSTEP 4 - CONSTRUCTING UPWARD PLANAR.. " << flush;
		
		plan_undi_graph  pug (ug);
		gdt::gdtnode_array<gdtedge> fce (pug,NULL_EDGE);
		
		forall_nodes (u,pug)
			{
			int idu = pug.id(u);
			if (pug.is_source(u))
				{
				e = pug.corresponding_edge(e_out[idu],*this);
				fce[u] = e;
				}
			else if (pug.is_target(u))
				{
				e = pug.corresponding_edge(e_in[idu],*this);
				fce[u] = e;
				}
			}
		
		
		upwa_plan_undi_graph upug (pug,fce);
		
		//cout << "\n\tSATURATING.." << flush;
		
		gdt::gdtlist<gdtedge> LS;
		gdtnode vs,vt;
		
		if (upug.number_of_edges() != 0)
			{
			LS = upug.make_st(vs,vt);
			forall (e,LS) upug.mark(e,RM3_ADDED_BY_REGULARIZATION);
			}
			
			//cout << "\n\tUPWARD AFTER SATURATION : " << flush;
			//upug.print(COMPLETE,true); cout << flush;
			
			
		// -----------------------------------------------------------------
		// STEP 5 - build the plan_undi_graph pug_v/pug_h as a copy of upug,
		//	    and make the correspondence between nodes of this 
		//	    and nodes of pug_v/pug_h.
		// -----------------------------------------------------------------					

		plan_undi_graph* pugp;
		
		if (s == vertical) pugp = &pug_v;
		else		   pugp = &pug_h;   
			
		(*pugp) = upug;
		
		forall (c,chains)
			{
			u = pugp -> find_node(c);
			forall (v,N[c]) 
				if (s == vertical) node_in_pug_v[v] = u; 		
				else		   node_in_pug_h[v] = u; 
			}
		
		// ------------
		// change slope
		// ------------
		
		s = horizontal;	
		}
	
	// ---------------------------------------------------------
	// restore the old directions on the edges of the orthogonal
	// ---------------------------------------------------------
	
	forall_edges (e,*this)
		if (old_start[e]) make_directed(e,old_start[e]);
		else		  make_undirected(e);
	
	}
	
	
	
	void		
	dime_orth_plan_undi_graph::
position_bottom_left_corners_and_incid_edges
	(
	const gdt::gdtnode_array<int>& w,
	const gdt::gdtnode_array<int>& h,
	      gdt::gdtnode_array<gdt::gdtpoint>& pos_bottom_left,
	      gdt::gdtnode_array<pos_incid_edges>& pie
	) const
	{
	gdtnode v;	
	
	gdt::gdtnode_array<double> cx (*this);
	gdt::gdtnode_array<double> cy (*this);
	
	gdt::gdtlist<gdtnode> nodes_to_expand_horizontally;
	gdt::gdtlist<gdtnode> nodes_to_expand_vertically;
	
	// --------- 0 ------------
	// initialize all variables
	// ------------------------
		
	forall_nodes (v, *this)
		{
		cx[v] = (position_of_node (v)).xcoord();
		cy[v] = (position_of_node (v)).ycoord();
		pie[v].north_edge = 0;
		pie[v].south_edge = 0;
		pie[v].east_edge  = 0;
		pie[v].west_edge  = 0;
		
		if (w[v] > 0) nodes_to_expand_horizontally.append(v);  
		if (h[v] > 0) nodes_to_expand_vertically.append(v); 
		}
	
	// ------------- 1 --------------
	// shift for horizontal expansion
	// ------------------------------
	
	forall (v,nodes_to_expand_horizontally)
		{
		gdtedge 	    e_up   	= find_edge_leaving_node_with_heading (v,north);
		gdtedge 	    e_down 	= find_edge_leaving_node_with_heading (v,south); 
		border_step s_left_up   = NULL_BORDER_STEP;
		border_step s_right_up  = NULL_BORDER_STEP;
		border_step s_left_down = NULL_BORDER_STEP;
		border_step s_right_down= NULL_BORDER_STEP;
		int 	    t_left_up 	= 0;
		int 	    t_right_up	= 0;
		int 	    t_left_down = 0;
		int 	    t_right_down= 0;
		
		if (e_up) 
			{
			s_right_up = border_step_moving_along_edge (e_up,v);
			s_left_up  = opposite_border_step     (s_right_up) ;
			t_right_up = thickness_of_border_step (s_right_up) ;
			t_left_up  = thickness_of_border_step (s_left_up)  ;
			}

		if (e_down)
			{
			s_right_down = border_step_moving_along_edge (e_down,v);
			s_left_down  = opposite_border_step     (s_right_down) ;
			t_right_down = thickness_of_border_step (s_right_down) ;
			t_left_down  = thickness_of_border_step (s_left_down)  ;
			}
		
		int number_of_top_edges     = t_left_up   + t_right_up   - 1;
		int number_of_bottom_edges  = t_left_down + t_right_down - 1;

		double vx  = (position_of_node(v)).xcoord();
		double vy  = (position_of_node(v)).ycoord();
			
		
		// ------------------------------------------------------
		// shift all nodes on the right of "v", that is all nodes
		// that are in a column successive to the column of "v",
		// if it is needed.
		// ------------------------------------------------------
				
		gdtnode u;
		forall_nodes (u,*this)
			{
			double ux = (position_of_node(u)).xcoord();
			if (ux > vx) cx[u] += w[v];
			}
		
		// ------------------------------------------------------
		// shift all nodes that are in the same vertical chain as
		// "v", and compute the relative positions of the edges
		// incident "v" in the north and south directions.
		// ------------------------------------------------------

		// "dx_up" was the distance between two incident edges in the 
		// upper side of v, if edges could be distributed with non-integer
		// coordinates
		//
		double dx_up   = (double)(w[v]+1)/(double)number_of_top_edges;
		double dx_down = (double)(w[v]+1)/(double)number_of_bottom_edges;

		dx_up   = min(1,dx_up);
		dx_down = min(1,dx_down); 

		// "s_up" is the space of w[v] that remains (to the left) when all the right 
		// edges are distributed (the smallest integer)
		//
		int	s_up   = w[v] - (int)((t_right_up - 1) * dx_up);
		int	s_down = (int)((t_right_down - 1) * dx_down);
		
		gdt::gdtlist<gdtnode> north_south_chain = nodes_reachable_moving_from_node_with_slope (v,vertical);
		forall (u,north_south_chain)
			if (u != v)
				{
				double uy = (position_of_node(u)).ycoord();
				if 	(uy > vy) cx[u] += s_up;	// u is above v
				else if (uy < vy) cx[u] += s_down;	// u is below v
				}
		
		pie[v].north_edge = s_up;
		pie[v].south_edge = s_down;
		}
	
	
	
	// ------------- 2 ------------
	// shift for vertical expansion
	// ----------------------------
	
	forall (v,nodes_to_expand_vertically)
		{
		gdtedge 	    e_left   	  = find_edge_leaving_node_with_heading (v,west);               
		gdtedge 	    e_right 	  = find_edge_leaving_node_with_heading (v,east); 
		border_step s_left_left   = NULL_BORDER_STEP;
		border_step s_right_left  = NULL_BORDER_STEP;
		border_step s_left_right  = NULL_BORDER_STEP;
		border_step s_right_right = NULL_BORDER_STEP;
		int 	    t_left_left   = 0;
		int 	    t_right_left  = 0;
		int 	    t_left_right  = 0;
		int 	    t_right_right = 0;
		
		if (e_left) 
			{
			s_right_left  = border_step_moving_along_edge (e_left,v);
			s_left_left   = opposite_border_step     (s_right_left) ;
			t_right_left  = thickness_of_border_step (s_right_left) ;
			t_left_left   = thickness_of_border_step (s_left_left)  ;
			}

		if (e_right)
			{
			s_right_right = border_step_moving_along_edge (e_right,v);
			s_left_right  = opposite_border_step     (s_right_right) ;
			t_right_right = thickness_of_border_step (s_right_right) ;
			t_left_right  = thickness_of_border_step (s_left_right)  ;
			}
		
		int number_of_left_edges  = t_left_left + t_right_left - 1;
		int number_of_right_edges = t_left_right + t_right_right - 1;

		double vx = (position_of_node(v)).xcoord();
		double vy = (position_of_node(v)).ycoord();


		// ------------------------------------------------------
		// shift all nodes above "v", that is all nodes 
		// that are in a row successive to the row of "v".
		// ------------------------------------------------------

		gdtnode u;
		forall_nodes (u,*this)
			{
			double uy = (position_of_node(u)).ycoord();
			if (uy > vy) cy[u] += h[v];
			}
		
		double dx_left  = (double)(h[v]+1)/(double)number_of_left_edges;
		double dx_right = (double)(h[v]+1)/(double)number_of_right_edges;
		//int    s_left   = (int)( (t_left_left - 1) * dx_left);
		//int    s_right  = (int)( (t_right_right - 1) * dx_right); 

		// new code
		// --------
		
		dx_left  = min(1,dx_left);
		dx_right = min(1,dx_right);
		int	s_left  = h[v] - (int)((t_right_left - 1) * dx_left);
		int	s_right = (int)((t_right_right - 1) * dx_right);
		
		// --------
				
		gdt::gdtlist<gdtnode> west_east_chain = nodes_reachable_moving_from_node_with_slope (v,horizontal);
		forall (u,west_east_chain)
			if (u != v)
				{
				double ux = (position_of_node(u)).xcoord();
				if 	(ux > vx) cy[u] += s_right;	// u is on the right of v
				else if (ux < vx) cy[u] += s_left;	// u is on the left of v
				}
		
		pie[v].east_edge = s_right;
		pie[v].west_edge = s_left;
		}
		
	
	// -------------------------------
	// update "pos_bottom_left" vector
	// -------------------------------
	
	forall_nodes (v,*this) 
		{
		gdt::gdtpoint cv = gdt::gdtpoint(cx[v],cy[v]);
		pos_bottom_left[v] = cv;
		}
	}



	void		
	dime_orth_plan_undi_graph::
position_bottom_left_corners_and_incid_edges_centered
	(
	const gdt::gdtnode_array<int>& w,
	const gdt::gdtnode_array<int>& h,
	      gdt::gdtnode_array<gdt::gdtpoint>& pos_bottom_left,
	      gdt::gdtnode_array<pos_incid_edges>& pie
	) const
	{
	gdtnode v;	
	
	gdt::gdtnode_array<double> cx (*this);
	gdt::gdtnode_array<double> cy (*this);
	
	gdt::gdtlist<gdtnode> nodes_to_expand_horizontally;
	gdt::gdtlist<gdtnode> nodes_to_expand_vertically;
	
	// --------- 0 ------------
	// initialize all variables
	// ------------------------
		
	forall_nodes (v, *this)
		{
		cx[v] = (position_of_node (v)).xcoord();
		cy[v] = (position_of_node (v)).ycoord();
		pie[v].north_edge = 0;
		pie[v].south_edge = 0;
		pie[v].east_edge  = 0;
		pie[v].west_edge  = 0;
		
		if (w[v] > 0) nodes_to_expand_horizontally.append(v);
		if (h[v] > 0) nodes_to_expand_vertically.append(v);
		}
	
	// ------------- 1 --------------
	// shift for horizontal expansion
	// ------------------------------
	
	forall (v,nodes_to_expand_horizontally)
		{
		double vx  = (position_of_node(v)).xcoord();
		double vy  = (position_of_node(v)).ycoord();
			
		
		// ------------------------------------------------------
		// shift all nodes on the right of "v", that is all nodes
		// that are in a column successive to the column of "v",
		// if it is needed.
		// ------------------------------------------------------
				
		gdtnode u;
		forall_nodes (u,*this)
			{
			double ux = (position_of_node(u)).xcoord();
			if (ux > vx) cx[u] += w[v];
			}
		
		// ------------------------------------------------------
		// shift all nodes that are in the same vertical chain as
		// "v", and compute the relative positions of the edges
		// incident "v" in the north and south directions.
		// ------------------------------------------------------
				
		int	s_up   = (int)(w[v]/2);
		int	s_down = (int)(w[v]/2);
		
		gdt::gdtlist<gdtnode> north_south_chain = nodes_reachable_moving_from_node_with_slope (v,vertical);
		forall (u,north_south_chain)
			if (u != v)
				{
				double uy = (position_of_node(u)).ycoord();
				if 	(uy > vy) cx[u] += s_up;	// u is above v
				else if (uy < vy) cx[u] += s_down;	// u is below v
				}
		
		pie[v].north_edge = s_up;
		pie[v].south_edge = s_down;
		}

	
	// ------------- 2 ------------
	// shift for vertical expansion
	// ----------------------------
	
	forall (v,nodes_to_expand_vertically)
		{
		double vx = (position_of_node(v)).xcoord();
		double vy = (position_of_node(v)).ycoord();

		// ------------------------------------------------------
		// shift all nodes above "v", that is all nodes
		// that are in a row successive to the row of "v".
		// ------------------------------------------------------

		gdtnode u;
		forall_nodes (u,*this)
			{
			double uy = (position_of_node(u)).ycoord();
			if (uy > vy) cy[u] += h[v];
			}
				
		int	s_left  = (int)(h[v]/2);
		int	s_right = (int)(h[v]/2);
				
		gdt::gdtlist<gdtnode> west_east_chain = nodes_reachable_moving_from_node_with_slope (v,horizontal);
		forall (u,west_east_chain)
			if (u != v)
				{
				double ux = (position_of_node(u)).xcoord();
				if 	(ux > vx) cy[u] += s_right;	// u is on the right of v
				else if (ux < vx) cy[u] += s_left;	// u is on the left of v
				}
		
		pie[v].east_edge = s_right;
		pie[v].west_edge = s_left;
		}
		
	
	// -------------------------------
	// update "pos_bottom_left" vector
	// -------------------------------
	
	forall_nodes (v,*this)
		{
		gdt::gdtpoint cv = gdt::gdtpoint(cx[v],cy[v]);
		pos_bottom_left[v] = cv;
		}
	}



	void		
	dime_orth_plan_undi_graph::
position_bottom_left_corners_and_incid_edges_with_pins
	(
	const gdt::gdtnode_array<int>& 		w,
	const gdt::gdtnode_array<int>& 		h,
	      gdt::gdtnode_array<gdt::gdtpoint>& 	pos_bottom_left,	// output
	      gdt::gdtnode_array<pos_incid_edges>& pie			// output
	) const
	{
	gdtnode v;	
	
	gdt::gdtnode_array<double> cx (*this);	// center of gdtnode
	gdt::gdtnode_array<double> cy (*this);
	
	gdt::gdtlist<gdtnode> nodes_to_expand_horizontally;
	gdt::gdtlist<gdtnode> nodes_to_expand_vertically;
	
	// --------- 0 ------------
	// initialize all variables
	// ------------------------
		
	forall_nodes (v, *this)
		{
		cx[v] = (position_of_node (v)).xcoord();
		cy[v] = (position_of_node (v)).ycoord();
		pie[v].north_edge = 0;
		pie[v].south_edge = 0;
		pie[v].east_edge  = 0;
		pie[v].west_edge  = 0;
		
		if (w[v] > 0) nodes_to_expand_horizontally.append(v);  
		if (h[v] > 0) nodes_to_expand_vertically.append(v); 
		}
	
	// ------------- 1 --------------
	// shift for horizontal expansion
	// ------------------------------
	
	forall (v,nodes_to_expand_horizontally)
		{
		gdtedge e_up   = find_edge_leaving_node_with_heading (v,north);	// possibly null
		gdtedge e_down = find_edge_leaving_node_with_heading (v,south); 	// possibly null

		double vx  = (position_of_node(v)).xcoord();
		double vy  = (position_of_node(v)).ycoord();
		
		// ------------------------------------------------------
		// shift all nodes on the right of "v", that is all nodes 
		// that are in a column successive to the column of "v",
		// if it is needed.
		// ------------------------------------------------------
				
		gdtnode u;
		forall_nodes (u,*this)
			{
			double ux = (position_of_node(u)).xcoord();
			if (ux > vx) cx[u] += w[v];
			}
		
		// ------------------------------------------------------
		// shift all nodes that are in the same vertical chain as
		// "v", and compute the relative positions of the edges
		// incident "v" in the north and south directions.
		// ------------------------------------------------------
		
		int s_up = 0;             // stands for "shift up" and is expressed
					  //  wrt bottom left corner
		if (e_up != NULL_EDGE) 
			{
			gdtedge last_e_up = e_up;
			gdtnode last_v_up = opposite(v,last_e_up);

			/*
						O  <------- last_v_up
			    this     ------->  ||^
			    border_step        |||    <---- last_e_up
			    contains the       V||
			    needed pin_number	O
			    for the gdtedge of
			    interest and the
			    gdtnode it points to
			*/

			while (thickness_of_edge(last_e_up) > 1)
				{
				last_e_up = find_edge_leaving_node_with_heading (last_v_up,north);
				last_v_up = opposite(last_v_up,last_e_up);
				}

                        // ### Begin import from GDT fork of W. Didimo on June, 18 2004
			int pn_up = pin_number_of_border_step(
				border_step_moving_along_edge(last_e_up,last_v_up));
			// debugging
			//
			//cout << "\nTHE TOP EDGE OF NODE " << id(v) << " HAS PIN NUMBER" << pn_up << flush;
			//
			
			// case of pin number not specified
			//	
			if (pn_up == UNDEFINED_PIN_NUMBER)
				{
				// debugging
				//
				//cout << "\n THE TOP SIDE OF NODE " << id(v) << " HAS EDGES WITH UNDEFINED PINS" << flush;
				//				
				border_step s_right_up = border_step_moving_along_edge (e_up,v);
				border_step s_left_up  = opposite_border_step     (s_right_up) ;
				int t_right_up = thickness_of_border_step (s_right_up) ;
				int t_left_up  = thickness_of_border_step (s_left_up)  ;
				int number_of_top_edges     = t_left_up   + t_right_up   - 1;
				double dx_up   = (double)(w[v]+1)/(double)number_of_top_edges;
				dx_up = min(1,dx_up);
				s_up = w[v] - (int)((t_right_up - 1) * dx_up);
				}
			else {s_up = pn_up;}			
				
                        // ### End import
			}
		
		int s_down = 0;
		if (e_down != NULL_EDGE)
			{
			gdtedge last_e_down = e_down;
			gdtnode last_v_down = opposite(v,last_e_down);

			/*
			      O 
			     ||^ <--- this border_step 
			     |||      contains my pin_number
			     V|| <--- last_e_down
			      O  <--- last_v_down

			*/

			while (thickness_of_edge(last_e_down) > 1)
				{
				last_e_down = find_edge_leaving_node_with_heading (last_v_down,south);
				last_v_down = opposite(last_v_down,last_e_down);
				}

                        // ### Begin import from GDT fork of W. Didimo on June, 18 2004
			int pn_down = pin_number_of_border_step(
				border_step_moving_along_edge(last_e_down,last_v_down));
													
			// debugging
			//
			//cout << "\nTHE BOTTOM EDGE OF NODE " << id(v) << " HAS PIN NUMBER " << pn_down << flush;
			//
				
			// case of pin number of specified
			//
			if (pn_down == UNDEFINED_PIN_NUMBER)
				{
				// debugging
				//
				//cout << "\n THE BOTTOM SIDE OF NODE " << id(v) << " HAS EDGES WITH UNDEFINED PINS " << flush;
				//
				border_step s_right_down = border_step_moving_along_edge (e_down,v);
				border_step s_left_down  = opposite_border_step     (s_right_down) ;
				int t_right_down = thickness_of_border_step (s_right_down) ;
				int t_left_down  = thickness_of_border_step (s_left_down)  ;
				int number_of_bottom_edges  = t_left_down + t_right_down - 1;
				double dx_down = (double)(w[v]+1)/(double)number_of_bottom_edges;
				dx_down = min(1,dx_down); 
				s_down = (int)((t_right_down - 1) * dx_down);
				}			
			else {s_down = w[v] - pn_down;}	

                        // ### End import


			}

		// shift horizontally all gdtnode above and below gdtnode "v" (changing cx[] values)
		//
		gdt::gdtlist<gdtnode> north_south_chain = nodes_reachable_moving_from_node_with_slope (v,vertical);
		forall (u,north_south_chain)
			if (u != v)
				{
				double uy = (position_of_node(u)).ycoord();
				if 	(uy > vy) cx[u] += s_up;	// u is above v
				else if (uy < vy) cx[u] += s_down;	// u is below v
				}
		
		pie[v].north_edge = s_up; 
		pie[v].south_edge = s_down;

                // ### Begin import from GDT fork of W. Didimo on June, 18 2004
		// debugging	
		// 
	  	// {
	  	// cout << "\n PRINT OF PIE DATA STRUCTURE FOR ALL NODES: \n" << flush;
	  	// forall_nodes (v, *this)
		//	{						
		//	cout << "\npie[" << id(v) << "].north=" << pie[v].north_edge << flush;
		//	cout << "\npie[" << id(v) << "].south=" << pie[v].south_edge << flush;
		//	cout << "\npie[" << id(v) << "].east=" << pie[v].east_edge << flush;
		//	cout << "\npie[" << id(v) << "].west=" << pie[v].west_edge << flush;
		//	}
		// }
		//	
                // ### End import
			
		}
	
	
	
	// ------------- 2 ------------
	// shift for vertical expansion
	// ----------------------------

	forall (v,nodes_to_expand_vertically)
		{
		gdtedge 	    e_left   	  = find_edge_leaving_node_with_heading (v,west);	// possibly null
		gdtedge 	    e_right 	  = find_edge_leaving_node_with_heading (v,east); 	// possibly null

		double vx = (position_of_node(v)).xcoord();
		double vy = (position_of_node(v)).ycoord();

		// ------------------------------------------------------
		// shift all nodes above "v", that is all nodes 
		// that are in a row successive to the row of "v".
		// ------------------------------------------------------

		gdtnode u;
		forall_nodes (u,*this)
			{
			double uy = (position_of_node(u)).ycoord();
			if (uy > vy) cy[u] += h[v];
			}
		
		// ------------------------------------------------------
		// shift all nodes that are in the same horizontal chain as
		// "v", and compute the relative positions of the edges
		// incident "v" in the left and right directions.
		// ------------------------------------------------------

		int s_left = 0;                 // stands for "shift left" and is computed
 						// wrt the bottom left corner
		if (e_left != NULL_EDGE)
			{
			gdtedge last_e_left = e_left;
			gdtnode last_v_left = opposite(v,last_e_left);
			
			/* 
			                      last_e_left
			    last_v_left --> O------------O
			                        ----->      <---this border step I need
			*/
			while (thickness_of_edge(last_e_left) > 1)
				{
				last_e_left = find_edge_leaving_node_with_heading (last_v_left,west);
				last_v_left = opposite(last_v_left,last_e_left);
				}
                        // ### Begin import from GDT fork of W. Didimo on June, 18 2004
			int pn_left = pin_number_of_border_step(
				border_step_moving_along_edge(last_e_left,last_v_left));			
			
			// debugging
			//
			//cout << "\nTHE LEFT EDGE OF NODE " << id(v) << " HAS PIN NUMBER " << pn_left << flush;
			//
				
			// case of pin number not specified
			//				
			if (pn_left == UNDEFINED_PIN_NUMBER)
				{
				// debugging
				//
				//cout << "\n THE LEFT SIDE OF NODE " << id(v) << " HAS EDGES WITH UNDEFINED PINS" << flush;
				//
				border_step s_right_left  = border_step_moving_along_edge (e_left,v);
				border_step s_left_left   = opposite_border_step     (s_right_left) ;
				int t_right_left  = thickness_of_border_step (s_right_left) ;
				int t_left_left   = thickness_of_border_step (s_left_left)  ;
				int number_of_left_edges  = t_left_left + t_right_left - 1;
				double dx_left  = (double)(h[v]+1)/(double)number_of_left_edges;
				dx_left  = min(1,dx_left);
				s_left  = h[v] - (int)((t_right_left - 1) * dx_left);
				}
			else {s_left = pn_left;}
				

                        // ### End import
			}
		
		int s_right = 0;            // stands for "shift right" and is computed
					    // wrt the bottom left corner
		if (e_right != NULL_EDGE)
			{
			gdtedge last_e_right = e_right;
			gdtnode last_v_right = opposite(v,last_e_right);

			/* 
			        <-----      <---this border step I need
			     O------------O  <---last_v_right
			      last_e_right 
			*/

			while (thickness_of_edge(last_e_right) > 1)
				{
				last_e_right = find_edge_leaving_node_with_heading (last_v_right,east);
				last_v_right = opposite(last_v_right,last_e_right);
				}
        		// ### Begin import from GDT fork of W. Didimo on June, 18 2004

                        //s_right = h[v] - pin_number_of_border_step(
			//	border_step_moving_along_edge(last_e_right,last_v_right));

			int pn_right = pin_number_of_border_step(
				border_step_moving_along_edge(last_e_right,last_v_right));
			
			// debugging
			//
			//cout << "\nTHE RIGHT EDGE OF NODE " << id(v) << " HAS PIN NUMBER " << pn_right << flush;
			//
			
			// case of pin number not specified
			//	
			if (pn_right == UNDEFINED_PIN_NUMBER)
				{
				// debugging
				//
				//cout << "\n THE RIGHT SIDE OF NODE " << id(v) << " HAS EDGES WITH UNDEFINED PINS" << flush;
				//
				border_step s_right_right = border_step_moving_along_edge (e_right,v);
				border_step s_left_right  = opposite_border_step     (s_right_right) ;
				int t_right_right = thickness_of_border_step (s_right_right) ;
				int t_left_right  = thickness_of_border_step (s_left_right)  ;
				int number_of_right_edges = t_left_right + t_right_right - 1;
				double dx_right = (double)(h[v]+1)/(double)number_of_right_edges;
				dx_right = min(1,dx_right);
				s_right = (int)((t_right_right - 1) * dx_right);								
				}	
			else {s_right = h[v] - pn_right;}

        		// ### End import
			


			}

		gdt::gdtlist<gdtnode> west_east_chain = nodes_reachable_moving_from_node_with_slope (v,horizontal);
		forall (u,west_east_chain)
			if (u != v)
				{
				double ux = (position_of_node(u)).xcoord();
				if 	(ux > vx) cy[u] += s_right;	// u is on the right of v
				else if (ux < vx) cy[u] += s_left;	// u is on the left of v
				}
		
		pie[v].east_edge = s_right;
		pie[v].west_edge = s_left;

			// debugging
			//
			//{
			//cout << "\nnode " << id(v) << " has pos_inc_edge heading east = " << pie[v].east_edge;
			//cout << "\nnode " << id(v) << " has pos_inc_edge heading west = " << pie[v].west_edge;
			//}
			//
		}
		
	
	// -------------------------------
	// update "pos_bottom_left" vector
	// -------------------------------
	
	forall_nodes (v,*this) 
		{
		gdt::gdtpoint cv = gdt::gdtpoint(cx[v],cy[v]);
		pos_bottom_left[v] = cv;
		}
	}
	
	
	
	
	void
	dime_orth_plan_undi_graph::
expand_nodes_into_super_nodes 
	(
	const dime_orth_plan_undi_graph& dopug,
	const gdt::gdtnode_array<int>& w,
	const gdt::gdtnode_array<int>& h,
	const gdt::gdtnode_array<gdt::gdtpoint>& pos_bottom_left,
	const gdt::gdtnode_array<pos_incid_edges>& pie,
	      gdt::gdtnode_map<gdtnode>& super_node
	)
	{
	gdtnode v;
	
	forall_nodes (v,dopug)
		{
		gdtnode 		v1      = corresponding_node (v,dopug);
		int  		width   = w[v];
		int  		height  = h[v];
		int 	  	bl_x    = (int)(pos_bottom_left[v]).xcoord();  
		int  		bl_y    = (int)(pos_bottom_left[v]).ycoord();
		pos_incid_edges pie_v 	= pie[v];
		gdt::gdtlist<gdtnode> 	N;
		
		//cout << "\n  - Expanding gdtnode " << id(v1) << ".." << flush;
		
		expand_node_into_super_node (v1, width, height, bl_x, bl_y, pie_v, N);
		
		//cout << "\n  - Expansion OK!" << flush;

		forall (v1,N) 
			super_node[v1] = v;
		}
	}		
	
	

// ### Begin import from GDT fork of W. Didimo on June, 18 2004


// Added by W.D. on August 2002
// ----------------------------
	void 
	dime_orth_plan_undi_graph::
expand_nodes_into_super_nodes_with_pins 
	(
	const dime_orth_plan_undi_graph& dopug,
	const gdt::gdtnode_array<int>& w,
	const gdt::gdtnode_array<int>& h,
	const gdt::gdtnode_array<gdt::gdtpoint>& pos_bottom_left,
	const gdt::gdtnode_array<pos_incid_edges>& pie,
	      gdt::gdtnode_map<gdtnode>& super_node,
	      gdt::gdtnode_map<bool>& is_removable,
	      gdt::gdtedge_map<bool>& removable_edge      
	)
	{
	gdtnode v;
	gdtedge e;
	
	forall_edges (e,dopug)
		removable_edge[e]=false;
		
	forall_nodes (v,dopug)
		{
		gdtnode 	v1      = corresponding_node (v,dopug);
		int  		width   = w[v];
		int  		height  = h[v];
		int 	  	bl_x    = (int)(pos_bottom_left[v]).xcoord();  
		int  		bl_y    = (int)(pos_bottom_left[v]).ycoord();
		
		is_removable[v1]=false;
		
		pos_incid_edges pie_v 	= pie[v];
		gdt::gdtlist<gdtnode> 	N;
		
		//cout << "\n  - Expanding node " << id(v1) << ".." << flush;
		
		expand_node_into_super_node_with_pins (v1, width, height, bl_x, bl_y, pie_v, N, is_removable, removable_edge);
		
		//cout << "\n  - Expansion OK!" << flush;
		
		forall (v1,N) super_node[v1] = v;
		}
		
	}		


// ### End import

	void 
	dime_orth_plan_undi_graph::
expand_node_into_super_node 
	(
	gdtnode 	v, 
	int  		width, 
	int  		height, 
	int  		bl_x, 		// bottom left x
	int  		bl_y, 		// bottom left y
	pos_incid_edges pie_v, 
	gdt::gdtlist<gdtnode>&     N
	)
	{
	bool expanded_horiz  = false;
	bool expanded_vertic = false;	
	N.clear();
	
	// ------------------ 0 -------------------
	// update the coordinates of v that will be 
	// the bottom-left corner of the super-gdtnode
	// and append the gdtnode "v" to the list N. 
	// ----------------------------------------
	
	gdt::gdtpoint bl = gdt::gdtpoint(bl_x,bl_y);
	set_position_of_node (v,bl);
	N.append(v);
	
	// ------------------ 1 -------------------
	// expand the gdtnode into a super-gdtnode 
	// according to its dimensions
	// ----------------------------------------

	gdtnode v_bl = v;		// bottom-left  gdtnode
	gdtnode v_tl = NULL_NODE;	// top-left     gdtnode
	gdtnode v_br = NULL_NODE;	// bottom-right gdtnode
	gdtnode v_tr = NULL_NODE;	// top-right    gdtnode
	
	// --------------------
	// horizontal expansion
	// --------------------
	 	
	if ((width > 0) && (height == 0)) 	
		{
		//cout << "\n      expanding in a horizontal chain of " << "length " << width << flush; 
		expanded_horiz = true;
		expand_node_into_chain (v,width,east,N);
		v_tl = v_bl;
		v_br = N.tail();
		v_tr = v_br;
		//
		gdtnode u;
		forall (u,N) mark(u,RM3_NODE_IN_SUPER_NODE);
		//
		}
		
	// ====================================================
	//
	//
	//
	//
	//    
	//     |                          |
	//    v|               v_tl = v_bl|         v_br = v_tr
	//  -- O --     ==>   	       -- O--O--O--O --
	//     |                          | 
	//     |                          | 
	//
	// ====================================================	
		
	// ------------------
	// vertical expansion
	// ------------------		
		
	else if ((width == 0) && (height > 0))  
		{
		//cout << "\n      expanding in a vertical chain of " << "length " << height << flush;
		expanded_vertic = true;
		expand_node_into_chain (v,height,north,N);
		v_tl = N.tail();
		v_br = v_bl;
		v_tr = v_tl;
		//
		gdtnode u;
		forall (u,N) mark(u,RM3_NODE_IN_SUPER_NODE);
		//
		}
	
	// ====================================================
	//
	//                                 |   
	//                                 |
	//                                 O v_tl = v_tr 
	//                                 |   
	//     |                           O
	//    v|                v_br = v_bl| 
	//  -- O --      ==>	        -- O --
	//     |                           |    
	//     |                           |
	//
	// ====================================================
	
	
	
	else if ((width > 0) && (height > 0))   
		{
		//cout << "\n      expanding in a box " << width << " x " << height << flush;
		expanded_horiz  = true;
		expanded_vertic = true;
		expand_node_into_chain (v_bl,height,north,N);
		v_tl = N.tail();
		expand_node_into_chain (v_bl,width,east,N);
		v_br = N.tail();
		expand_node_into_chain (v_tl,width,east,N);
		v_tr = N.tail();
		attach_nodes_by_chain  (v_br,v_tr,N);
		//
		gdtnode u;
		forall (u,N) mark(u,RM3_NODE_IN_SUPER_NODE);
		//			
		}
	
	
	// ====================================================
	//
	//                                 |   
	//                                 |v_tl     
	//                                 O--O--O--O v_tr 
	//                                 |        | 
	//     |                           O        O
	//    v|    		       v_bl|        |
	//  -- O --      ==>	        -- O--O--O--O --
	//     |                           |        v_br 
	//     |                           |
	//
	// ====================================================
	
	
	if (expanded_horiz || expanded_vertic)
		{ 
		// ------------------ 2 -------------------
		// translate the edges according to "pie_v"
		// ----------------------------------------	

		gdtedge e_north = find_edge_leaving_node_with_heading (v_tl,north);
		gdtedge e_south = find_edge_leaving_node_with_heading (v_bl,south);
		gdtedge e_east  = find_edge_leaving_node_with_heading (v_br,east) ;
		gdtedge e_west  = find_edge_leaving_node_with_heading (v_bl,west) ;

		gdtnode v_north = v_tl;
		gdtnode v_south = v_bl;
		gdtnode v_east  = v_br;
		gdtnode v_west  = v_bl;

		int i;

		for (i=1; i<=pie_v.north_edge; i++) 
			v_north = find_node_reachable_moving_from_node_with_heading (v_north,east);
		for (i=1; i<=pie_v.south_edge; i++)
			v_south = find_node_reachable_moving_from_node_with_heading (v_south,east);
		for (i=1; i<=pie_v.east_edge; i++)
			v_east  = find_node_reachable_moving_from_node_with_heading (v_east,north);
		for (i=1; i<=pie_v.west_edge; i++)
			v_west  = find_node_reachable_moving_from_node_with_heading (v_west,north); 


		//debugging
		//   {
		//   draw_undi_graph dug(0,*this);
		//   dug.write("lastexp.gdt");
		//   }

		
		if ((v_north != v_tl) && (e_north)) change_node_of_edge_with_node (v_tl, e_north, v_north);
		if ((v_south != v_bl) && (e_south)) change_node_of_edge_with_node (v_bl, e_south, v_south);
		if ((v_east  != v_br) && (e_east))  change_node_of_edge_with_node (v_br, e_east , v_east);
		if ((v_west  != v_bl) && (e_west))  change_node_of_edge_with_node (v_bl, e_west , v_west);
		}
	}
	
// ### Begin import from GDT fork of W. Didimo on June, 18 2004
	void 
	dime_orth_plan_undi_graph::
expand_node_into_super_node_with_pins 
	(
	gdtnode 	v, 
	int  		width, 
	int  		height, 
	int  		bl_x, 		// bottom left x
	int  		bl_y, 		// bottom left y
	pos_incid_edges pie_v, 
	gdt::gdtlist<gdtnode>&     N,
	gdt::gdtnode_map<bool>& is_removable,
	gdt::gdtedge_map<bool>& removable_edge
	)
	{
	bool expanded_horiz  = false;
	bool expanded_vertic = false;	
	N.clear();
	
	// ------------------ 0 -------------------
	// update the coordinates of v that will be 
	// the bottom-left corner of the super-node
	// and append the node "v" to the list N. 
	// ----------------------------------------
	
	gdt::gdtpoint bl = gdt::gdtpoint(bl_x,bl_y);
	set_position_of_node (v,bl);
	N.append(v);
	
	// ------------------ 1 -------------------
	// expand the node into a super-node 
	// according to its dimensions
	// ----------------------------------------
	
	gdtnode v_bl = v;		// bottom-left  node
	gdtnode v_tl = NULL_NODE;	// top-left     node
	gdtnode v_br = NULL_NODE;	// bottom-right node
	gdtnode v_tr = NULL_NODE;	// top-right    node
	
	// --------------------
	// horizontal expansion
	// --------------------
	 	
	if ((width > 0) && (height == 0)) 	
		{
		//cout << "\n      expanding in a horizontal chain of " << "length " << width << flush; 
		expanded_horiz = true;
		expand_node_into_chain (v,width,east,N);
		v_tl = v_bl;
		v_br = N.tail();
		v_tr = v_br;
		//
		gdtnode u;
		forall (u,N) mark(u,RM3_NODE_IN_SUPER_NODE);
		//
		}
		
	// ====================================================
	//
	//
	//
	//
	//    
	//     |                          |
	//    v|               v_tl = v_bl|         v_br = v_tr
	//  -- O --     ==>   	       -- O--O--O--O --
	//     |                          | 
	//     |                          | 
	//
	// ====================================================	
		
	// ------------------
	// vertical expansion
	// ------------------		
		
	else if ((width == 0) && (height > 0))  
		{
		//cout << "\n      expanding in a vertical chain of " << "length " << height << flush;
		expanded_vertic = true;
		expand_node_into_chain (v,height,north,N);
		v_tl = N.tail();
		v_br = v_bl;
		v_tr = v_tl;
		//
		gdtnode u;
		forall (u,N) mark(u,RM3_NODE_IN_SUPER_NODE);
		//
		}
	
	// ====================================================
	//
	//                                 |   
	//                                 |
	//                                 O v_tl = v_tr 
	//                                 |   
	//     |                           O
	//    v|                v_br = v_bl| 
	//  -- O --      ==>	        -- O --
	//     |                           |    
	//     |                           |
	//
	// ====================================================
	
	
	
	else if ((width > 0) && (height > 0))   
		{
		//cout << "\n      expanding in a box " << width << " x " << height << flush;
		expanded_horiz  = true;
		expanded_vertic = true;
		expand_node_into_chain (v_bl,height,north,N);
		v_tl = N.tail();
		expand_node_into_chain (v_bl,width,east,N);
		v_br = N.tail();
		expand_node_into_chain (v_tl,width,east,N);
		v_tr = N.tail();
		attach_nodes_by_chain  (v_br,v_tr,N);
		//
		gdtnode u;
		forall (u,N) mark(u,RM3_NODE_IN_SUPER_NODE);
		//			
		}
	
	
	// ====================================================
	//
	//                                 |   
	//                                 |v_tl     
	//                                 O--O--O--O v_tr 
	//                                 |        | 
	//     |                           O        O
	//    v|    		       v_bl|        |
	//  -- O --      ==>	        -- O--O--O--O --
	//     |                           |        v_br 
	//     |                           |
	//
	// ====================================================
	
	
	if (expanded_horiz || expanded_vertic)
		{ 
		// ------------------ 2 -------------------
		// translate the edges according to "pie_v"
		// ----------------------------------------	

		gdtedge e_north = find_edge_leaving_node_with_heading (v_tl,north);
		gdtedge e_south = find_edge_leaving_node_with_heading (v_bl,south);
		gdtedge e_east  = find_edge_leaving_node_with_heading (v_br,east) ;
		gdtedge e_west  = find_edge_leaving_node_with_heading (v_bl,west) ;

		gdtnode v_north = v_tl;
		gdtnode v_south = v_bl;
		gdtnode v_east  = v_br;
		gdtnode v_west  = v_bl;

		int i;

		for (i=1; i<=pie_v.north_edge; i++) 
			v_north = find_node_reachable_moving_from_node_with_heading (v_north,east);
		for (i=1; i<=pie_v.south_edge; i++)
			v_south = find_node_reachable_moving_from_node_with_heading (v_south,east);
		for (i=1; i<=pie_v.east_edge; i++)
			v_east  = find_node_reachable_moving_from_node_with_heading (v_east,north);
		for (i=1; i<=pie_v.west_edge; i++)
			v_west  = find_node_reachable_moving_from_node_with_heading (v_west,north); 


		// find the nodes that are removable before compaction
		// and the edges that can have zero length after compaction
		//
		if (e_north)
			{
			bool removable = false;
			border_step s = border_step_moving_along_edge (e_north, opposite(v_tl,e_north));
			if (pin_number_of_border_step(s) == UNDEFINED_PIN_NUMBER)
				removable = true;
			gdtnode v = v_tl;			
			if (width > 0)
				{				
				gdtedge e_first = find_edge_leaving_node_with_heading (v,east);
				removable_edge[e_first]=removable;
				for (i=1; i<=width-1; i++)
					{				
					v = find_node_reachable_moving_from_node_with_heading (v,east);	
					is_removable[v]=removable; 
					}
				gdtedge e_last = find_edge_leaving_node_with_heading (v,east);
				removable_edge[e_last]=removable;
				}				
			}
		if (e_south)
			{
			bool removable = false;
			border_step s = border_step_moving_along_edge (e_south, opposite(v_bl,e_south));
			if (pin_number_of_border_step(s) == UNDEFINED_PIN_NUMBER)
				removable = true;
			gdtnode v = v_bl;
			if (width > 0)
				{
				gdtedge e_first = find_edge_leaving_node_with_heading (v,east);
				removable_edge[e_first]=removable;						
				for (i=1; i<=width-1; i++)
					{				
					v = find_node_reachable_moving_from_node_with_heading (v,east);	
					is_removable[v]=removable; 
					}
				gdtedge e_last = find_edge_leaving_node_with_heading (v,east);
				removable_edge[e_last]=removable;
				}					
			}
		if (e_east)
			{
			bool removable = false;
			border_step s = border_step_moving_along_edge (e_east, opposite(v_br,e_east));
			if (pin_number_of_border_step(s) == UNDEFINED_PIN_NUMBER)
				removable = true;
			gdtnode v = v_br;
			if (height > 0)
				{
				gdtedge e_first = find_edge_leaving_node_with_heading (v,north);
				removable_edge[e_first]=removable;				
				for (i=1; i<=height-1; i++)
					{				
					v = find_node_reachable_moving_from_node_with_heading (v,north);	
					is_removable[v]=removable; 
					}	

				gdtedge e_last = find_edge_leaving_node_with_heading (v,north);
				removable_edge[e_last]=removable;
				}				
			}
		if (e_west)
			{
			bool removable = false;
			border_step s = border_step_moving_along_edge (e_west, opposite(v_bl,e_west));
			if (pin_number_of_border_step(s) == UNDEFINED_PIN_NUMBER)
				removable = true;
			gdtnode v = v_bl;
			if (height > 0)
				{ 
				gdtedge e_first = find_edge_leaving_node_with_heading (v,north);
				removable_edge[e_first]=removable;	
				for (i=1; i<=height-1; i++)
					{				
					v = find_node_reachable_moving_from_node_with_heading (v,north);	
					is_removable[v]=removable; 
					}				
				gdtedge e_last = find_edge_leaving_node_with_heading (v,north);
				removable_edge[e_last]=removable;
				}
			}
		//

		//debugging
		//   {
		//   draw_undi_graph dug(0,*this);
		//   dug.write("lastexp.gdt");
		//   }

		
		if ((v_north != v_tl) && (e_north)) change_node_of_edge_with_node (v_tl, e_north, v_north);
		if ((v_south != v_bl) && (e_south)) change_node_of_edge_with_node (v_bl, e_south, v_south);
		if ((v_east  != v_br) && (e_east))  change_node_of_edge_with_node (v_br, e_east , v_east);
		if ((v_west  != v_bl) && (e_west))  change_node_of_edge_with_node (v_bl, e_west , v_west);
		}
	}
// ### End import


	void 
	dime_orth_plan_undi_graph::
expand_node_into_chain
	(
	gdtnode v, 
	int length, 
	heading_type dir, 
	gdt::gdtlist<gdtnode>& N
	)
	{
	gdtnode w;
	
	// ====================================================
	//
	//    
	//     |                          |
	//    v|  e_dir                  v|   ..w..  e_dir
	//  -- O --        ==>         -- O--O--O--O --
	//     |                          | 
	//     |                          | 
	//
	// ====================================================	
	
	// -------------------------
	// find the gdtedge "e_dir" or 
	// create it if there is not
	// -------------------------
	
	bool edge_added = false;
	gdtedge e_dir = find_edge_leaving_node_with_heading (v,dir);
	
	if (!e_dir)
		{
		w          = new_node (v,dir,length);	// attach vertex
		e_dir      = first_adj_edge(w); 
		edge_added = true;
		}
	w = v;
	
	// --------------------------
	// split the gdtedge "e_dir" in
	// a chain with a number of 
	// edges equal to "length"
	// --------------------------
	
	int i,
	    old_id;
	    
	if (edge_added) length -= 1; // because the last gdtnode of the 
							     // chain has been already added
	for (i = 1; i <= length; i++)
		{
		old_id = id(e_dir);
		w = new_node (e_dir,w,1);	
		N.append(w);
		
		  //cout << "\n       new gdtnode   : " << id(w) << flush;
		  //cout << "\n       at position: " << position_of_node(w) << flush;
		  
		e_dir = find_edge_leaving_node_with_heading (w,dir);
		assign_id (e_dir,old_id);
		
		// the thickness of the edges in the chain must be reset
		// as well the markers, directions, and constraints
		//
		gdtedge e_dir_inv = cyclic_adj_succ (e_dir,w);
		border_step s1 = border_step_moving_along_edge (e_dir_inv,w);
		border_step s2 = opposite_border_step (s1);
		set_thickness_of_border_step (s1,1);
		set_thickness_of_border_step (s2,1);
		unmark_all (e_dir_inv);
		make_undirected(e_dir_inv);
		del_all_constraints_involving_edge (e_dir_inv);
		del_all_constraints_involving_node (w);
		//
		}
	if (edge_added)
		{
		w = opposite (w,e_dir);
		
		  //cout << "\n       new gdtnode   : " << id(w) << flush;
		  //cout << "\n       at position: " << position_of_node(w) << flush;
		  
		N.append(w); 
		}
	}



	gdtedge
	dime_orth_plan_undi_graph::
change_node_of_edge_with_node 
	(gdtnode v1, gdtedge e1, gdtnode v2)
	{
	// ------------------------------------------------------------
	//  w1                                         w1
	//  O  					       O
	//  |					       |
	//  |  e1           			       |  e
	//  o-----o          		  ==>	       o-----o 	
	//        |                                          |  
	//        |      e_succ   e_pred              e_succ | e_pred
	//      --O--      -----O-----         --O--    -----O-----
	//        v1            v2               v1          v2 
	// ------------------------------------------------------------
	
	//cout << "\n     changing position of gdtedge " << id(e1) << " from gdtnode " << id(v1) << " to gdtnode " << id(v2) << flush;
	
	gdtnode         w1        = opposite(v1,e1);
	border_step  s_e1_v1   = border_step_moving_along_edge (e1,v1);
	border_step  s_e1_w1   = opposite_border_step (s_e1_v1);
	
	heading_type dir_e     = initial_heading_of_border_step (s_e1_v1);

	// --------------------------------------
	// find the pred. gdtedge of "e" around "v2"
	// in the counterclockwise direction
	// --------------------------------------

	gdtedge         e_pred     = NULL_EDGE;
	heading_type dir_e_pred = dir_e;
	while (e_pred == NULL_EDGE) 
		{
		dir_e_pred = heading_after_rotation (dir_e_pred,_090);
		e_pred     = find_edge_leaving_node_with_heading (v2,dir_e_pred);
		  //cout << "\n     e_pred = " << id(e_pred) << flush;
		}
	border_step s_e_pred_v2 = border_step_moving_along_edge (e_pred,v2);
	
	// --------------------------------------
	// find the succ. gdtedge of "e" around "v2"
	// in the counterclockwise direction
	// --------------------------------------

	gdtedge         e_succ 	 = cyclic_adj_succ (e_pred,v2);
	
	  //cout << "\n     e_succ = " << id(e_succ) << flush;
	
	border_step  s_e_succ_v2 = border_step_moving_along_edge (e_succ,v2); 
	heading_type dir_e_succ  = initial_heading_of_border_step(s_e_succ_v2);
	
	// ------------------------------------
	// remember the face that will be split
	// and the identifier of the gdtedge "e1"
	// ------------------------------------

	face f = right_face_moving_along_edge (e_succ,v2);
	bool f_was_external = (f == external_face());
	int  old_id = id(e1);
		
		
	// ---------------------------------
	// create the new gdtedge and update
	// the external face if it is needed
	// ---------------------------------
	
	gdtedge e_ref_w1 = NULL_EDGE;
	
	if (face_of_border_step(s_e1_w1) == f) e_ref_w1 = cyclic_adj_pred(e1,w1);
	else				       e_ref_w1 = e1;  
	
	gdtedge e  = plan_undi_graph::new_edge (w1,e_ref_w1,v2,e_pred);
	if (f_was_external) 
		if (e_ref_w1 == e1) ext_face = right_face_moving_along_edge (e,v2);	
		else		    ext_face = left_face_moving_along_edge (e,v2);
	
	// ----------------------
	// update constraints,
	// markers and directions
	// ----------------------

	update_constraints_after_edge_translation (e1,w1,v1,e,w1,v2);
	mark(e,markers(e1));
	if (start(e1) == w1)      make_directed (e,w1);
	else if (start(e1) == v1) make_directed (e,v2);

	// -----------------------------------------
	// set angles, bends, tickness, and headings
	// -----------------------------------------

	border_step s_e_v2   = border_step_moving_along_edge (e,v2);	
	border_step s_e_w1   = opposite_border_step (s_e_v2);
	border_step s_e_pred = opposite_border_step (s_e_pred_v2); 	
	
	// angles
	//
	angle_type alpha_pred = angle_required_to_change_heading (dir_e     ,dir_e_pred);	
	angle_type alpha_succ = angle_required_to_change_heading (dir_e_succ,dir_e);
	angle_type beta       = angle_of_border_step (s_e1_v1);  	
	set_angle_of_border_step (s_e_pred,alpha_pred);
	set_angle_of_border_step (s_e_w1  ,alpha_succ);
	set_angle_of_border_step (s_e_v2  ,beta);

	// bends and bend-positions
	//
	set_bends_of_border_step (s_e_v2, bends_of_border_step(s_e1_v1));
	set_bends_of_border_step (s_e_w1, bends_of_border_step(s_e1_w1));
	set_position_of_bends_along_edge (e,v2,position_of_bends_along_edge(e1,v1));
	set_position_of_bends_along_edge (e,w1,position_of_bends_along_edge(e1,w1));
	
	// tickness
	// 
	set_thickness_of_border_step (s_e_v2, thickness_of_border_step(s_e1_v1));
	set_thickness_of_border_step (s_e_w1, thickness_of_border_step(s_e1_w1));
	
	// pin_number
	//
	set_pin_number_of_border_step (s_e_v2, pin_number_of_border_step(s_e1_v1));
	set_pin_number_of_border_step (s_e_w1, pin_number_of_border_step(s_e1_w1));

	// headings
	//
	set_initial_heading_of_border_step_pair (s_e_v2,dir_e);
		
	// ---------------------------------
	// delete the gdtedge "e1" and reassign 
	// the identifier of the gdtedge "e". 
	// The external face might be 
	// updated after the del_edge 
	// operation
	// ---------------------------------
	
	gdtedge e1_pred_v1 = cyclic_adj_pred(e1,v1);
	gdtedge e1_succ_v1 = cyclic_adj_succ(e1,v1); 
	
	border_step s_e1_pred_v1 = border_step_moving_along_edge (e1_pred_v1, opposite(v1,e1_pred_v1));
	border_step s_e1_succ_v1 = border_step_moving_along_edge (e1_succ_v1, v1);

	face f_pred = face_of_border_step (s_e1_pred_v1);
	face f_succ = face_of_border_step (s_e1_succ_v1);
	
	heading_type heading_init  = initial_heading_of_border_step (s_e1_succ_v1);
	heading_type heading_final = initial_heading_of_border_step (opposite_border_step(s_e1_pred_v1));
	
	angle_type a = angle_required_to_change_heading (heading_init, heading_final);
	
	// change the reference border step if it is needed
	//
	border_step rbs = get_ref_border_step();
	if 	(rbs == s_e1_v1) ref_border_step = s_e_v2;
	else if (rbs == s_e1_w1) ref_border_step = s_e_w1;
	
	// Added by Marcandalli 10 2000
	undefine(e1);
	orth_plan_undi_graph::undefine(e1);
	//
		
	// del the gdtedge e1
	//
	plan_undi_graph::del_edge (e1);	
	
	
	assign_id (e,old_id);	
	
	set_angle_of_border_step (s_e1_pred_v1,a); 
	
	if      (ext_face == f_pred) ext_face = face_of_border_step (s_e1_pred_v1);
	else if (ext_face == f_succ) ext_face = face_of_border_step (s_e1_succ_v1);
	
	return e;
	}


	
	void 
	dime_orth_plan_undi_graph::
uncompress_edges 
	(
	const dime_orth_plan_undi_graph&   dopug,
	const gdt::gdtnode_array<int>&             w,
	const gdt::gdtnode_array<int>&             h,
	const gdt::gdtnode_array<pos_incid_edges>& pie,
	      gdt::gdtnode_map<gdtnode>&	   super_node
	)
	{
		//debugging
		//
		//{
		//draw_undi_graph dug (0,*this);
		//dug.write("before_uncompress");
		//}
		//
	
	// --------------------- 0 --------------------
	// look for the initial border steps to expand. 
	// This means that it takes in account the 
	// border steps with thickness more than one 
	// and such that one extremal gdtnode is an 
	// element of a super_node with non-zero area.
	// These border steps are put in the "init_bs"
	// list.
	// --------------------------------------------
	
	gdtedge 	    	  e;
	gdt::gdtlist<border_step> init_bs;
	
	forall_edges (e,*this)
		{
		gdtnode 	    v1 = source(e);
		gdtnode        v2 = target(e);
		
		gdtnode        V1 = super_node[v1];
		gdtnode        V2 = super_node[v2];

		
		if ( ((w[V1] > 0) || (h[V1] > 0)) || 
		     ((w[V2] > 0) || (h[V2] > 0)) )

			{ 
			border_step s1 = border_step_moving_along_edge (e,v1);
			border_step s2 = border_step_moving_along_edge (e,v2);
			int 	    t1 = thickness_of_border_step (s1);
			int         t2 = thickness_of_border_step (s2);
			
			if (t1 > 1) 
				{
				//cout << "\ninserted border step of gdtedge " << id(e) << " at gdtnode " << id(v1) << flush;
				init_bs.append(s1);
				}
			if (t2 > 1) 
				{
				//cout << "\ninserted border step of gdtedge " << id(e) << "at gdtnode " << id(v2) << flush;
				init_bs.append(s2); 
				}
			}
		}
		
	// --------------------- 1 --------------------
	// for each initial border step, uncompress
	// the corresponding gdtedge, depending on the 
	// thickness of the border step and on the 
	// dimensions of the gdtnode.
	// --------------------------------------------	
		
	border_step s;
	
	forall (s,init_bs)
		{
		gdtnode start_s = start_of_border_step(s);
		gdtnode stop_s  = stop_of_border_step (s);

		gdtnode START_s = super_node[start_s];
		gdtnode STOP_s  = super_node[stop_s] ;
		
		
		// -----------------------------
		// determine the real extremal
		// gdtnode v of s and the bend
		// extremal gdtnode u of s.
		// -----------------------------  
		
		gdtnode v = NULL_NODE;	// real gdtnode
		gdtnode V = NULL_NODE;	// super-gdtnode
		gdtnode u = NULL_NODE;	// bend gdtnode
		 
		if (is_marked(start_s,RM3_BEND_NODE))
			{
			u = start_s;
			v = stop_s;
			V = STOP_s;
			}
		else if (is_marked(stop_s,RM3_BEND_NODE))
			{
			u = stop_s;
			v = start_s;
			V = START_s;
			}
		else gdt_error("structure inconsistency");
			
		// ------------------------------
		// determine the direction for 
		// expanding the gdtedge.
		// ------------------------------
		
		heading_type dir_s     = initial_heading_of_border_step(s);
		heading_type dir_s_inv = heading_after_inversion (dir_s); 
		heading_type dir = east;	// direction for expanding
						// initialized to avoid a compilation warning

		switch (dir_s)
			{
			case north: dir = east ; break;
			case south: dir = west ; break;
			case east : dir = south; break;
			case west : dir = north; break;
			default   : break;
			}
		
		//cout << "\nExpanding at gdtnode " << id(v) << " in direction " << heading_to_string(dir) << flush;
		
		// ------------------------------
		// determine the original free
		// space in the direction dir.
		// ------------------------------
		
		int free_space = 0;
		
		switch (dir)
			{
			case east:
				  if (v == start_s) free_space = w[V] - pie[V].north_edge;
				  else		    free_space = w[V] - pie[V].south_edge; 
			          break; 
			case west:
				  if (v == start_s) free_space = pie[V].south_edge;
				  else		    free_space = pie[V].north_edge;
				  break;
			case north: 
				  if (v == start_s) free_space = h[V] - pie[V].west_edge;
				  else		    free_space = h[V] - pie[V].east_edge;
				  break;
			case south: 
				  if (v == start_s) free_space = pie[V].east_edge;
				  else		    free_space = pie[V].west_edge;
			default: break;
			}
		
		//cout << "\nOriginal Free space = " << free_space << flush;
			
		// -------------------
		// start uncompression
		// -------------------
		
		gdt::gdtlist<border_step> uncompressed_bs;
		bool 		  end_uncompression = false;
		gdtnode 		  z = v;
		angle_type        as;
		
		if (v == start_s) as = angle_of_border_step(s);
		else		  as = angle_of_border_step(cyclic_pred_border_step(s));
		
		while (!end_uncompression)
			{
			//cout << "\nangle = " << angle_to_int(as) << flush; 
			if (as == _090) 
				{				
				if (free_space > 0)
					{
					// ==================================
					// uncompress the current border step
					// ==================================
					
					gdtedge eu    = find_edge_leaving_node_with_heading (u,dir);
					if (!eu) gdt_error("angle inconsistency");
					int  eu_id = id(eu); 
					
					int ts    = thickness_of_border_step (s);
					int shift = min (ts-1,free_space); 
					
					// ---------------------------------------
					// find the gdtnode in which will be attached
					// the uncompressed gdtedge
					// ---------------------------------------
					//
					gdtnode w = v;
					int i;
					for (i = 1; i<=shift; i++) 
						w = find_node_reachable_moving_from_node_with_heading (w,dir);
					
					// --------------------------------------------------
					// shift the gdtedge at gdtnode w and update the free space
					// and the thickness of the border steps
					// --------------------------------------------------
					//
					gdtnode q  = new_node (eu,u,shift);
					gdtedge eq = find_edge_leaving_node_with_heading (q,dir);
					     eu = find_edge_leaving_node_with_heading (u,dir);
					assign_id(eq,eu_id);
					mark (q,RM3_REPLACES_A_BEND);
					super_node[q] = NULL_NODE;
					
					new_straight_edge (w,q);
					del_edge (eu);
					
					uncompressed_bs.append(s);
					border_step s_temp;
					forall (s_temp,uncompressed_bs) 
						{
						int ts_temp = thickness_of_border_step (s_temp);
						set_thickness_of_border_step (s_temp,ts_temp-1);
						}
					
					free_space --;
					}
				else end_uncompression = true;
				}
			else uncompressed_bs.append(s);
			
			// --------------------------
			// chose the next border step
			// to uncompress.
			// --------------------------
			
			if (thickness_of_border_step(s) > 1)
				{
				bool forward = true;
				if (z == start_of_border_step(s)) forward = true;
				else		  		  forward = false;

				z = u;

				gdtedge ezu = NULL_EDGE;

				if (forward) ezu = find_edge_leaving_node_with_heading (z,dir_s);
				else	     ezu = find_edge_leaving_node_with_heading (z,dir_s_inv);

				//cout << "\nnode z   = " << id(z) << flush; 
				//cout << "\nedge ezu = " << id(ezu) << flush;

				u = opposite(z,ezu);
				//if (!is_marked(u,RM3_BEND_NODE)) end_uncompression = true; 
				if (forward) 
					{
					s  = border_step_moving_along_edge (ezu,z);
					as = angle_of_border_step (s);
					}
				     else 	  
					{
					s  = border_step_moving_along_edge (ezu,u);
					as = angle_of_border_step (cyclic_pred_border_step(s));
					}
				}
			else end_uncompression = true;
			}
		}
	
	// ------------- 2 --------------
	// delete all nodes with degree 2
	// that are marked RM3_BEND_NODE
	// ------------------------------
	
		//debugging
		//{
		//draw_undi_graph dug (0,*this);
		//dug.write("after_uncompress");
		//}

	
	gdtnode v;
	gdt::gdtlist<gdtnode> Lv = all_nodes();
	forall (v,Lv)
		if (is_marked(v,RM3_BEND_NODE) && degree (v) == 2) 
			weld_edges_at_node (v);	
	}


	void 
	dime_orth_plan_undi_graph::
uncompress_edges_with_pins
	(
	const dime_orth_plan_undi_graph&   dopug,
	const gdt::gdtnode_array<int>&             w,
	const gdt::gdtnode_array<int>&             h,
	const gdt::gdtnode_array<pos_incid_edges>& pie,
	      gdt::gdtnode_map<gdtnode>&	   super_node,
	      gdt::gdtnode_map<bool>&		   is_removable,
	      gdt::gdtedge_map<bool>&		   removable_edge
	)
	{
		//debugging
	        //
		//{
		//draw_undi_graph dug (0,*this);
		//dug.write("before_uncompress.gdt");
		//}
	
	// --------------------- 0 --------------------
	// look for the initial border steps to expand,
	// i.e., put into the "init_bs" list all 
	// border steps with thickness more than one 
	// and such that one extremal gdtnode is an 
	// element of a super_node with non-zero area.
	// --------------------------------------------
	
	gdtedge 	    	  e;
	gdt::gdtlist<border_step> init_bs;  /* all border_steps with thicknes
				       more than one and one extremal
				       gdtnode element of a super_node
				       with non zero area */
	
	forall_edges (e,*this)
		{
		gdtnode v1 = source(e);
		gdtnode v2 = target(e);
		
		gdtnode V1 = super_node[v1];
		gdtnode V2 = super_node[v2];
		
		
		if ( ((w[V1] > 0) || (h[V1] > 0)) || 
		     ((w[V2] > 0) || (h[V2] > 0)) )

			{ 
			border_step s1 = border_step_moving_along_edge (e,v1);
			border_step s2 = border_step_moving_along_edge (e,v2);
			int 	    t1 = thickness_of_border_step (s1);
			int         t2 = thickness_of_border_step (s2);
			
			if (t1 > 1) 
				{
				//cout << "\ninserted border step of gdtedge " << id(e) << " at gdtnode " << id(v1) << flush;
				init_bs.append(s1);
				}
			if (t2 > 1) 
				{
				//cout << "\ninserted border step of gdtedge " << id(e) << "at gdtnode " << id(v2) << flush;
				init_bs.append(s2); 
				}
			}
		}
		
	// --------------------- 1 -------------------- 
	// for each initial border step in the init_bs list,
	// uncompress the corresponding gdtedge, according to the gdtedge
	// attachment constraints specified by the pin_number member
	// of the border_step.
	// --------------------------------------------
		
	border_step s;
	
	forall (s,init_bs)
		{
		gdtnode start_s = start_of_border_step(s);
		gdtnode stop_s  = stop_of_border_step (s);
		
		gdtnode START_s = super_node[start_s];
		gdtnode STOP_s  = super_node[stop_s] ;
		
		
		// -----------------------------
		// determine the real extremal
		// gdtnode v of s and the bend
		// extremal gdtnode u of s.
		// -----------------------------  
		
		gdtnode v = NULL_NODE;   // the gdtnode we are considering and ...
		gdtnode V = NULL_NODE;	 // ...its super-gdtnode
		gdtnode u = NULL_NODE;	 // bend gdtnode
		 
		if (is_marked(start_s,RM3_BEND_NODE))
			{
			u = start_s;
			v = stop_s;
			V = STOP_s;
			}
		else if (is_marked(stop_s,RM3_BEND_NODE))
			{
			u = stop_s;
			v = start_s;
			V = START_s;
			}
		else gdt_error("structure inconsistency");
			
		// ------------------------------
		// determine the direction for 
		// expanding the gdtedge.
		// ------------------------------
		
		heading_type dir_s     = initial_heading_of_border_step(s);
		heading_type dir_s_inv = heading_after_inversion (dir_s); 
		heading_type dir = east;	// direction of expansion
						// assigned east to avoid a comp. warning
		heading_type dir_away = east;   // direction used by the gdtedge to leave gdtnode v
						// assigned east to avoid a comp. warning
		
		switch (dir_s)
			{
			case north: dir = east ; break;
			case south: dir = west ; break;
			case east : dir = south; break;
			case west : dir = north; break;
			default   : break;
			}
		
		switch (dir_s)
			{
			case north:
				dir_away = (v == start_s ? north : south);
	          		break; 
			case west:
				dir_away = (v == start_s ? west  : east);
				break;
			case south:
				dir_away = (v == start_s ? south : north);
				break;
			case east:
				dir_away = (v == start_s ? east  : west);
			default: break;
			}

		//cout << "\nExpanding at gdtnode " << id(v) << " in direction " << heading_to_string(dir) << flush;


                // ### Begin import from GDT fork of W. Didimo on June, 18 2004

		// ADDED BY W.D. ON August 2002	
		// ----------------------------
		// introduced a branch to handle the case with unspecified pins

		if (pin_number_of_border_step(s) == UNDEFINED_PIN_NUMBER){
		// ---------------------------------------
		// EXECUTES THE CODE FOR UNSPECIFIED PINS
		// ---------------------------------------	
		// ------------------------------
		// determine the original free
		// space in the direction dir.
		// ------------------------------
		
		int free_space = 0;
		
		switch (dir)
			{
			case east:
				  if (v == start_s) free_space = w[V] - pie[V].north_edge;
				  else		    free_space = w[V] - pie[V].south_edge; 
			          break; 
			case west:
				  if (v == start_s) free_space = pie[V].south_edge;
				  else		    free_space = pie[V].north_edge;
				  break;
			case north: 
				  if (v == start_s) free_space = h[V] - pie[V].west_edge;
				  else		    free_space = h[V] - pie[V].east_edge;
				  break;
			case south: 
				  if (v == start_s) free_space = pie[V].east_edge;
				  else		    free_space = pie[V].west_edge;
			default: break;
			}
		
		//cout << "\nOriginal Free space = " << free_space << flush;
			
		// -------------------
		// start uncompression
		// -------------------
		
		gdt::gdtlist<border_step> uncompressed_bs;
		bool 		  end_uncompression = false;
		gdtnode 	  z = v;
		angle_type        as;
		
		if (v == start_s) as = angle_of_border_step(s);
		else		  as = angle_of_border_step(cyclic_pred_border_step(s));
		
		while (!end_uncompression)
			{
			//cout << "\nangle = " << angle_to_int(as) << flush; 
			if (as == _090) 
				{				
				if (free_space > 0)
					{
					// ==================================
					// uncompress the current border step
					// ==================================
					
					gdtedge eu    = find_edge_leaving_node_with_heading (u,dir);
					if (!eu) gdt_error("dime_orth_plan_undi_graph::uncompress_edges : angle inconsistency");
					int  eu_id = id(eu); 
					
					int ts    = thickness_of_border_step (s);
					int shift = min (ts-1,free_space); 
					
					// ---------------------------------------
					// find the node in which will be attached
					// the uncompressed edge
					// ---------------------------------------
					//
					gdtnode w = v;
					int i;
					for (i = 1; i<=shift; i++) 
						w = find_node_reachable_moving_from_node_with_heading (w,dir);
					
					// --------------------------------------------------
					// shift the edge at node w and update the free space
					// and the thickness of the border steps
					// --------------------------------------------------
					//
					gdtnode q  = new_node (eu,u,shift);
					gdtedge eq = find_edge_leaving_node_with_heading (q,dir);
					     eu = find_edge_leaving_node_with_heading (u,dir);
					assign_id(eq,eu_id);
					mark (q,RM3_REPLACES_A_BEND);
					super_node[q] = NULL_NODE;
					is_removable[q] = false; 
					
					gdtedge e_temp = new_straight_edge (w,q); 
					removable_edge[e_temp] = false;
					del_edge (eu);
					
					uncompressed_bs.append(s);
					border_step s_temp;
					forall (s_temp,uncompressed_bs) 
						{
						int ts_temp = thickness_of_border_step (s_temp);
						set_thickness_of_border_step (s_temp,ts_temp-1);
						}
					
					free_space --;
					}
				else end_uncompression = true;
				}
			else uncompressed_bs.append(s);
			
			// --------------------------
			// chose the next border step
			// to uncompress.
			// --------------------------
			
			if (thickness_of_border_step(s) > 1)
				{
				bool forward = true;
				if (z == start_of_border_step(s)) forward = true;
				else		  		  forward = false;

				z = u;

				gdtedge ezu = NULL_EDGE;

				if (forward) ezu = find_edge_leaving_node_with_heading (z,dir_s);
				else	     ezu = find_edge_leaving_node_with_heading (z,dir_s_inv);

				//cout << "\nnode z   = " << id(z) << flush; 
				//cout << "\nedge ezu = " << id(ezu) << flush;

				u = opposite(z,ezu);
				//if (!is_marked(u,RM3_BEND_NODE)) end_uncompression = true; 
				if (forward) 
					{
					s  = border_step_moving_along_edge (ezu,z);
					as = angle_of_border_step (s);
					}
				     else 	  
					{
					s  = border_step_moving_along_edge (ezu,u);
					as = angle_of_border_step (cyclic_pred_border_step(s));
					}
				}
			else end_uncompression = true;
			}				
		} // END CODE FOR UNSPECIFIED PINS
		
		else{
		// ---------------------------------------
		// EXECUTES THE CODE FOR SPECIFIED PINS
		// ---------------------------------------

                // ### End import



		// -------------------
		// start uncompression
		// -------------------

		gdt::gdtlist<border_step> uncompressed_bs;
		bool 		  end_uncompression = false;
		gdtnode 		  z = v;
		angle_type        as;

		if (v == start_s) as = angle_of_border_step(s);
		else		  as = angle_of_border_step(cyclic_pred_border_step(s));

		while (!end_uncompression)
			{
			//cout << "\nangle = " << angle_to_int(as) << flush;
			if (as == _090)
				{

				// ==================================
				// uncompress the current border step
				// ==================================

				gdtedge eu    = find_edge_leaving_node_with_heading (u,dir);
				if (!eu) gdt_error("angle inconsistency");
				int  eu_id = id(eu);

				int shift = 0;  // always computed wrt bottom left corner
						// the initialization to zero is to avoid
						// compilation warnings
                                switch (dir_away)
                                        {
                                        case north:
                                                shift = pin_number_of_border_step(border_step_moving_along_edge(eu,opposite(u,eu)));
                                                break;
                                        case west:
                                                shift = pin_number_of_border_step(border_step_moving_along_edge(eu,opposite(u,eu)));
                                                break;
                                        case south:
                                                shift = w[V] - pin_number_of_border_step(border_step_moving_along_edge(eu,opposite(u,eu)));
                                                break;
                                        case east:
					        shift = h[V] - pin_number_of_border_step(border_step_moving_along_edge(eu,opposite(u,eu)));
					  //shift = pin_number_of_border_step(border_step_moving_along_edge(eu,opposite(u,eu)));
						break;
                                        default: break;
                                        }
				std::cout << "shift before difference = " << shift << "\n";

                                switch (dir_away)
                                        {
                                        case north:
                                                shift = abs(pie[V].north_edge - shift);
                                                break; 
                                        case west:
                                                shift = abs(pie[V].west_edge  - shift);
                                                break;
                                        case south: 
                                                shift = abs(pie[V].south_edge - shift);
                                                break;
                                        case east: 
                                                shift = abs(pie[V].east_edge  - shift);
                                        default: break;
                                        }
				
				if ( shift > 0 )
					{	
					// debugging
					//
					// cout << "\n\nEDGE EXPANSION BY SHIFTING OF " << shift << " UNIT(S)";

					// ---------------------------------------
					// find the gdtnode to which will be attached
					// the uncompressed gdtedge
					// ---------------------------------------
					//
					gdtnode x = v;
					int i;
					for (i = 1; i<=shift; i++)
						{
						if ( find_node_reachable_moving_from_node_with_heading (x,dir) )
							x = find_node_reachable_moving_from_node_with_heading (x,dir);
						// debugging
						//
						
						else 
							{
							std::cout << "\nERROR: YOU'RE PASSING THE NODE'S BOUNDS!\n";
							std::cout << "             gdtnode = " << id(v);
							std::cout << " (supernode V = " << id(V) << ")\n";
							std::cout << "     size (w x h) = " << w[V] << " x " << h[V] << "\n";
			                                switch (dir_away)
                        		                {
                                        		case north:
		                                                std::cout << "pie[V].north_edge = " << pie[V].north_edge << "\n";
							        std::cout << "         dir_away = north\n";
                		                                break;
                                		        case west:
		                                                std::cout << "pie[V].west_edge  = " << pie[V].west_edge << "\n";
							        std::cout << "         dir_away = west\n";
        	        	                                break;
                	        	                case south:
		                                                std::cout << "pie[V].west_south = " << pie[V].south_edge << "\n";
							        std::cout << "         dir_away = south\n";
		                                                break;
                		                        case east:
		                                                std::cout << "pie[V].east_edge  = " << pie[V].east_edge << "\n";
							        std::cout << "         dir_away = east\n";
			                                default: break;
                        		                }

							std::cout << "            shift = " << shift << "\n";

                                                        std::cout << "    my pin_number = " <<
							  pin_number_of_border_step(border_step_moving_along_edge(eu,opposite(u,eu))) <<
							  "\n";
			                                switch (dir)
                        		                {
                                        		case north: std::cout << "    expansion dir = north\n"; break;
                                		        case west: std::cout <<  "    expansion dir = west\n"; break;
                	        	                case south: std::cout << "    expansion dir = south\n"; break;
                		                        case east: std::cout <<  "    expansion dir = east\n"; break;
		                                        default: break;
                        		                }
							std::cout << "        iteration = " << i << "\n";
							exit(1);
							}

						}

					// --------------------------------------------------
					// shift the gdtedge at gdtnode x and update the free space
					// and the thickness of the border steps
					// --------------------------------------------------
					//
					gdtnode q  = new_node (eu,u,shift);
					gdtedge eq = find_edge_leaving_node_with_heading (q,dir);
					     eu = find_edge_leaving_node_with_heading (u,dir);
					assign_id(eq,eu_id);
					mark (q,RM3_REPLACES_A_BEND);
					super_node[q] = NULL_NODE;
			                // ### Begin import from GDT fork of W. Didimo on June, 18 2004
					is_removable[q] = false;
			                // ### End import

					/*
							    |	                |         ^
						----eu-----u|    --eq---q--eu--u|         |
							   ||                  ||         | dir_away
							   ||                  ||         |
					         ----------||---     ----------||---
							   v	               v
					*/

					border_step s_temp;
					gdt::gdtlist<border_step> increase_my_thickness;
					gdtedge e_from_x_dir_away = find_edge_leaving_node_with_heading(x,dir_away);
					while( e_from_x_dir_away != NULL_EDGE )
						{
						s_temp = border_step_moving_along_edge(e_from_x_dir_away,x);
						if( dir_s!=initial_heading_of_border_step(s_temp) )
							s_temp=opposite_border_step(s_temp);
						increase_my_thickness.append(s_temp);
						x = opposite(x,e_from_x_dir_away);
						e_from_x_dir_away = find_edge_leaving_node_with_heading(x,dir_away);
 						}

					if (is_marked(x,RM3_REPLACES_A_BEND))
						{
						unmark (x,RM3_REPLACES_A_BEND);
						mark (x,RM3_BEND_NODE);
						}

					/*
						                        |
					               	 --eq---q--eu--u|
					               		.      ||
					          e_x_q ----->  .      ||
					               		.      ||
					(prev. expansions) -----x      ||
					               		|      ||
						               	|      ||
					               	--------|------||---
						               	        v
					*/



					gdtedge e_x_q = new_straight_edge (x,q);
                                        // ### Begin import from GDT fork of W. Didimo on June, 18 2004
					removable_edge[e_x_q]=false;
                                        // ### End import
					del_edge (eu);

					forall (s_temp,increase_my_thickness)
						{
						int ts_temp = thickness_of_border_step (s_temp);
						set_thickness_of_border_step (s_temp,ts_temp+1);
						}
					uncompressed_bs.append(s);
					forall (s_temp,uncompressed_bs)
						{
						int ts_temp = thickness_of_border_step (s_temp);
						set_thickness_of_border_step (s_temp,ts_temp-1);
						}
					// ------------------------------
					// resetting the pin_number values
					// ------------------------------

					/*
					               s_eq_out
						         <---
					            -------eq---q
					                  --->  | <-- e_x_q
					               s_eq_in	|
					                        |^
					               	       |||
                                                    s_e_x_q_in ||| s_e_x_q_out
	                                                       V|
                                                                |
                                                                x
					*/

					border_step s_eq_out    = border_step_moving_along_edge(eq,q),
                                         	    s_eq_in     = border_step_moving_along_edge(eq,opposite(q,eq)),
						    s_e_x_q_in  = border_step_moving_along_edge(e_x_q,q),
						    s_e_x_q_out = border_step_moving_along_edge(e_x_q,x);

					set_pin_number_of_border_step(s_e_x_q_out,pin_number_of_border_step(s_eq_out));
					set_pin_number_of_border_step(s_e_x_q_in,pin_number_of_border_step(s_eq_in));

					// debugging
					//
					// cout << "\nsetting pin number (other gdtnode attach.) " << pin_number_of_border_step(s_eq_out);
					// cout << "\nsetting pin number  (this gdtnode attach.) " << pin_number_of_border_step(s_eq_in);


					/*
                                                                |
                                                               ||^
                                                        s_next |||
					                       V||
						     removed    |              ^
					    <----     ..........u----?----     |
					    dir                 |              | dir_away
					                        |^             |
					               	       |||
                                                             s |||
	                                                       V|
                                                                |
                                                 ---------------v--------------
					*/

					border_step s_next;
					if( heading_after_rotation(dir,_090) == dir_away ) // rotation is clockwise
						{
						std::cout << "\n(gdtedge expansion was performed on the left)";
						s_next = cyclic_pred_border_step(s);
						}
					else
						{
						std::cout << "\n(gdtedge expansion was performed on the right)";
						s_next = cyclic_succ_border_step(s);
						}

						// update the pin numbers of the chain of
						// border_steps from s to v with s_next value
						//
						{
						gdtnode n = u;
						border_step b = s;

						while( n != v )
							{
							set_pin_number_of_border_step(b,pin_number_of_border_step(s_next));
							if( heading_after_rotation(dir,_090) == dir_away ) // rotation is clockwise
								{
								b = cyclic_succ_border_step(b);
								n = start_of_border_step(b);
								}
							else
								{
								b = cyclic_pred_border_step(b);
								n = stop_of_border_step(b);
								}
							}
						}

						// debugging
						//
						//
						//{
						//cout << "\n(updating the \"eagle\")";
						//cout << "\n (second segment: has pin number "
     						//     << pin_number_of_border_step(s_next) << ")";
						//cout << "\n (second segment: opposite border_step has "
						//     << pin_number_of_border_step(opposite_border_step(s_next))
						//     << ")";
						//cout << "\n (bend gdtnode " << id(u) << ")";
						//cout << "\n (first  segment: border_step set to "
						//     << pin_number_of_border_step(s)
						//     << ")";
						//cout << "\n (first  segment: opposite border_step has "
						//     << pin_number_of_border_step(opposite_border_step(s))
						//     << ")";
						//}

				}
				else end_uncompression = true;
				}
			else uncompressed_bs.append(s);
			
			// --------------------------
			// chose the next border step
			// to uncompress.
			// --------------------------
			
			if (thickness_of_border_step(s) > 1)
				{
				bool forward = true;
				if (z == start_of_border_step(s)) forward = true;
				else		  		  forward = false;

				z = u;

				gdtedge ezu = NULL_EDGE;

				if (forward) ezu = find_edge_leaving_node_with_heading (z,dir_s);
				else	     ezu = find_edge_leaving_node_with_heading (z,dir_s_inv);

				//cout << "\nnode z   = " << id(z) << flush; 
				//cout << "\nedge ezu = " << id(ezu) << flush;

				u = opposite(z,ezu);
				//if (!is_marked(u,RM3_BEND_NODE)) end_uncompression = true; 
				if (forward) 
					{
					s  = border_step_moving_along_edge (ezu,z);
					as = angle_of_border_step (s);
					}
				     else 	  
					{
					s  = border_step_moving_along_edge (ezu,u);
					as = angle_of_border_step (cyclic_pred_border_step(s));
					}
				}
			else end_uncompression = true;
			}
// ### Begin import from GDT fork of W. Didimo on June, 18 2004
		} // END CODE FOR UNSPECIFIED PINS
		
		
		} // END UNCOMPRESSION OF THE EDGES INCIDENT ON THE SAME SIDE 
	// -------------------------------
	// END BRANCHING W.D. August 2002 
// ### End import
	
	// ------------- 2 --------------
	// delete all nodes with degree 2
	// that are marked RM3_BEND_NODE
	// ------------------------------
	
		//debugging
		//
		//{
		//draw_undi_graph dug (0,*this);
		//dug.write("after_uncompress");
		//}

	
	gdtnode v;
	gdt::gdtlist<gdtnode> Lv = all_nodes();
	forall (v,Lv)
		if (is_marked(v,RM3_BEND_NODE) && degree (v) == 2) 
			weld_edges_at_node (v);	
	}





	gdt::gdtlist<gdtnode> 
	dime_orth_plan_undi_graph::
expand_chain 
	(
	gdtnode v,
	slope_type dir,
	int length,
	gdt::gdtlist<gdtedge>& zero_edges
	)
	{
	
	//cout << "\nexpanding chain with start gdtnode " << id(v) << flush;
	
	gdt::gdtlist<gdtnode> Lv;
		
	heading_type top_dir;		// direction for expanding the chain (north or east)
	heading_type chain_dir; 	// direction of the chain (east or north)
	
	gdtnode z; // last gdtnode of the chain
	gdtnode u;	// medium nodes
	int v_coord;
	int u_coord;
	
	gdtedge e;
	
	if (dir == vertical)	// vertical expansion
		{
		top_dir   = north;
		chain_dir = east; 
		}
	else    		// horizontal expansion
		{
		top_dir   = east;
		chain_dir = north; 
		}
	
	gdt::gdtlist<gdtnode> top_nodes;	// medium nodes that will be on the top
	gdt::gdtlist<gdtedge> top_edges;	// medium edges that will be on the top

	// -------------------------------------------------------------
	// constructs the ordered (from left to right) list of the nodes
	// of the chain that will be on the top side of the new box,
	// excluding the first and the last nodes of the chain
	// -------------------------------------------------------------

	u = v;
	e = find_edge_leaving_node_with_heading (u,chain_dir);
	u = opposite (u,e);

	if (dir == vertical) v_coord = (int)(position_of_node(v).xcoord());
	else		     v_coord = (int)(position_of_node(v).ycoord());
	if (dir == vertical) u_coord = (int)(position_of_node(u).xcoord());
	else		     u_coord = (int)(position_of_node(u).ycoord());

	int dist = (u_coord - v_coord);

	while (dist < length)
		{
		// check if there exists the gdtedge in the top_dir direction
		//
		gdtedge e = find_edge_leaving_node_with_heading(u,top_dir);
		if (e) 
			{
			top_nodes.append(u);
			top_edges.append(e);
			//cout << "\nappending new medium gdtnode " << id(u) << flush;
			}

		// take the next gdtedge of the chain
		//
		e = find_edge_leaving_node_with_heading (u,chain_dir);
		u = opposite (u,e);
		
		if (dir == vertical) u_coord = (int)(position_of_node(u).xcoord());
		else		     u_coord = (int)(position_of_node(u).ycoord());
		dist = (u_coord - v_coord);
		}
	z = u;	// last gdtnode
	//cout << "\nlast gdtnode = " << id(z) << flush;

	// -----------------------------------
	// create a box with height/width zero
	// -----------------------------------

	// first gdtnode
	// 
	gdtnode v_copy;
	e = find_edge_leaving_node_with_heading (v,top_dir);
	if (e) 					// split gdtedge 
		{
		int old_ide = id(e); 					
		v_copy = new_node (e,v,0);
		gdtedge e = find_edge_leaving_node_with_heading (v_copy,top_dir);
		assign_id(e,old_ide);
		}	
	else   v_copy = new_node (v,top_dir,0);	// attach vertex
	
	Lv.append(v_copy);
	mark (v_copy,markers(v));
	e = find_edge_leaving_node_with_heading (v,top_dir);
	zero_edges.append(e);
	
	gdtnode u_copy = v_copy;

	// medium nodes
	//
	while (!top_nodes.empty())
		{
		u = top_nodes.pop();
		e = top_edges.pop();
		int old_edge_id = id(e);
		int old_node_id = id(u);

		gdtnode w = new_node(e,u,0);	// split gdtedge
		Lv.append(w);
		mark (w,markers(u));
		new_straight_edge (u_copy,w);
		
		gdtedge ew_top_dir     = find_edge_leaving_node_with_heading(w,top_dir);
		gdtedge ew_inv_top_dir = find_edge_leaving_node_with_heading(w,heading_after_inversion(top_dir));

		del_edge(ew_inv_top_dir);
		assign_id(ew_top_dir,old_edge_id);
		
		if (degree(u)==2) 
			{
			weld_edges_at_node (u);
			assign_id(w,old_node_id);
			}

		u_copy = w;
		}


	// last gdtnode
	//
	gdtnode z_copy;
	e = find_edge_leaving_node_with_heading (z,top_dir);
	if (e) 					// split gdtedge 
		{
		int old_ide = id(e); 					
		z_copy = new_node (e,z,0);
		gdtedge e = find_edge_leaving_node_with_heading (z_copy,top_dir);
		assign_id(e,old_ide);
		}		
	else   z_copy = new_node (z,top_dir,0);	// attach vertex
	
	Lv.append(z_copy);
	mark (z_copy,markers(z));
	e = find_edge_leaving_node_with_heading (z,top_dir);
	zero_edges.append(e);
	
	new_straight_edge (u_copy,z_copy);
	
	return Lv;
	}


	
	gdt::gdtlist<gdtnode>
	dime_orth_plan_undi_graph::
corners_of_super_nodes 
	(
	const dime_orth_plan_undi_graph& dopug,
	const gdt::gdtnode_map<gdtnode>&		 super_node,
	const gdt::gdtnode_array<int>&		 w,
	const gdt::gdtnode_array<int>&		 h
	) const
	{
	gdt::gdtlist<gdtnode> corners;
	gdtnode V;
	forall_nodes (V,dopug)
		{
		//cout << "\nconsidering gdtnode " << dopug.id(V) << flush;
		if (w[V] > 0 || h[V] > 0)
			{
			gdtnode v = corresponding_node(V,dopug); // bottom-left corner of a super gdtnode
			gdtedge e = find_edge_leaving_node_with_heading(v,north);
			if (e)
				{
				face f = right_face_moving_along_edge (e,v);
		
				border_step s;
				forall_face_border_steps (s,f)
					if (angle_of_border_step(s)==_090) corners.append(stop_of_border_step(s));
				}
			}
		}
	return corners;
	}



	gdtedge
	dime_orth_plan_undi_graph::
move_edge_close_to_corner_of_super_node
	(
	gdtedge e, 
	gdtnode v,
	gdt::gdtnode_map<gdtnode>& super_node,
	gdt::gdtedge_map<bool>& is_zero_edge
	)
	{
	gdtnode u; 	// new gdtnode
	gdtedge euv;	// new gdtedge
	
	//cout << "\nexamining gdtedge " << id(e) << " at corner " << id(v) << flush;
	
	// --------------------------------
	//        o w                  o w
	// 	 e|                   e|
	//	  |                    |
	// 	v o---   ==>	v o----o---
	// 	  |  ev           | euv u
	//     	  |               |
	// 
	//      before           after
	// --------------------------------
	
	border_step s = border_step_moving_along_edge (e,v);
	heading_type dir_s = initial_heading_of_border_step (s);
	
	// ------------------------
	// deteremine the direction 
	// for moving the gdtedge "e"
	// ------------------------
	
	std::string sdir;	//temporary
	heading_type dir = east;  // this assignment is here to avoid a compilation warning
	switch (dir_s) 
		{
		case north:
		case south: 
			{
			gdtnode v_east = find_node_reachable_moving_from_node_with_heading(v,east);
			if (v_east && super_node[v_east] == super_node[v]) {dir = east; sdir = "east";}
			else					 	   {dir = west; sdir = "west";} 
			}
			break;
		case east:
		case west:
			{
			gdtnode v_north = find_node_reachable_moving_from_node_with_heading(v,north);
			if (v_north && super_node[v_north] == super_node[v]) {dir = north; sdir = "north";}
			else					  	     {dir = south; sdir = "south";} 
			}
			break;
		default : break;
		}
	
	
	//cout << "\nmoving gdtedge " << id(e) << " in dir = " << sdir << flush;
	
	// -------------------------------------
	// add the new gdtnode "u", by splitting 
	// the gdtedge that is at "dir" of "v".
	// The gdtnode "u" will be marked as
	// RM3_NODE_IN_SUPER_NODE and will be 
	// put in the same super-gdtnode as "v" one.
	// --------------------------------------
	
	bool split_zero_edge = false;
	
	gdtedge ev = find_edge_leaving_node_with_heading (v,dir); // gdtedge to split
	
	int old_idev = id(ev);
	if (is_zero_edge[ev]) 
		{
		split_zero_edge = true;
		//cout << "\nsplitting gdtedge with zero length = " << id(ev) << flush;
		}
	
	u = new_node (ev,v,0);		// split gdtedge
	
	// WARNING strange bug under windows if code is super_node[u]=super_node[v] 
	// with visual C++ 6 
	gdtnode temp= super_node[v];
	super_node[u] = temp;	// set the super-gdtnode of the new gdtnode

	mark(u,RM3_NODE_IN_SUPER_NODE);
	
	euv = find_edge_leaving_node_with_heading (v,dir);
	assign_id(euv,old_idev);
	
	gdtedge euv_succ = find_edge_leaving_node_with_heading (u,dir);
	
	if (split_zero_edge) 
		{
		is_zero_edge[euv] 	= true;
		is_zero_edge[euv_succ]  = true;
		}
	
	
	// ---------------
	// move the gdtedge e
	// ---------------

	// added by Marcandalli on 30 11 2001
	//
	bool e_is_zero_edge = is_zero_edge[e];
	is_zero_edge.undefine(e);
	gdtedge e_new = change_node_of_edge_with_node (v,e,u);
	is_zero_edge[e_new] = e_is_zero_edge;
	
	//Debug
	/*
	 {
	 cout << "\nINSIDE MOVE EDGE CLOSE ..." << flush;
	 cout << "\n" << id(euv) << " is zero gdtedge = " << is_zero_edge[euv] << flush;
	 cout << "\n" << id(euv_succ) << " is zero gdtedge = " << is_zero_edge[euv_succ] << flush;
	 cout << "\n" << 0 << " is zero gdtedge = " << is_zero_edge[find_edge(0)] << flush;	
	 }
	*/
	
	return euv;
	}


	
//	void
//	dime_orth_plan_undi_graph::
//change_ref_border_step
//	(border_step rs)
//	{
//	if (!rs) rs = first_border_step(first_face());
//	heading_type rh = initial_heading_of_border_step(rs);
//	if (rh == undefined_heading) rh = north;
//	
//	reference_border_step = rs;
//	reference_heading     = rh;
//	}



	void
	dime_orth_plan_undi_graph:: 
init_headings
	()
	{
	//cout << "\n dime_orth_plan_undi_graph::init_headings" << flush;
	face f;
	gdt::gdtlist<face> faces_to_be_oriented;
	border_step s,s0,s1,s2;
	heading_type s2_dir;

	// Check the correctness of 
	// the reference border step
	//
		{
		border_step rbs = get_ref_border_step();
		bool rs_exists = false;
		forall_faces (f,*this)
			forall_face_border_steps (s,f)
				if (s == rbs) rs_exists = true;
		//if (!rs_exists) change_ref_border_step();
		if (!rs_exists) set_reference_border_step(first_border_step(first_face()));
		}
		
	// Mark all headings as undefined
	//
	forall_faces (f,*this)
		forall_face_border_steps (s,f)
			(*border_step_info)[s].initial_heading = undefined_heading;
	
	// Set the reference heading 
	// on the reference border step
	//
	if (reference_heading == undefined_heading) reference_heading = east;
	set_initial_heading_of_border_step_pair (get_ref_border_step(), reference_heading);


	// Set the heading of all the other border_steps
	// consistently with the heading of the reference one.
	//
	faces_to_be_oriented = all_faces();
	while (!faces_to_be_oriented.empty())
		{
		f  = faces_to_be_oriented.pop();
		s0 = find_first_border_step_with_defined_heading_around_face (f);

		if (!s0)
			{
			faces_to_be_oriented.append(f);
			//cout << "\nFace " << id(f) << "will be oriented later";
			}
		else
			{
			s1 = s0;
			s2 = cyclic_succ_border_step (s1);
			while (s2!=s0)
				{
				s2_dir = heading_after_border_step(s1);
				set_initial_heading_of_border_step_pair(s2, s2_dir);

				s1=s2;
				s2=cyclic_succ_border_step (s1);
				}
			//cout << "\nFace " << id(f) << "has been oriented";
			}
		}
	}



	void
	dime_orth_plan_undi_graph::
local_set
	(
	bool p1,
	heading_type p2,
	gdt::gdtnode_map<struct_dime_node_info>* p3,
	gdt::gdtedge_map<struct_dime_edge_info>* p4,
	gdt::gdtmap<border_step,struct_dime_border_step_info>* p5
	)
	{
	frozen_layout         = p1;
	reference_heading     = p2;
	node_info             = p3;
	edge_info             = p4;
	border_step_info      = p5;
	} 	
		


	void 
	dime_orth_plan_undi_graph::
set_initial_heading_of_border_step
	(border_step s,heading_type d)
	{
	(*border_step_info)[s].initial_heading = d;  
	} 



	void 
	dime_orth_plan_undi_graph::
set_initial_heading_of_border_step_pair
	(border_step s,heading_type d)
	{
	(*border_step_info)[s].initial_heading = d;  
	d = heading_after_bend_sequence(d,bends_of_border_step(s));
	d = heading_after_inversion (d);
	set_initial_heading_of_border_step (opposite_border_step(s),d);
	} 

	
	

	void
	dime_orth_plan_undi_graph::
set_position_of_node
	(gdtnode v, gdt::gdtpoint p)
	{
	(*node_info)[v].position = p;
	}
	



	void
	dime_orth_plan_undi_graph::
set_position_of_bends_along_edge
	(gdtedge e, gdtnode v, gdt::gdtlist<gdt::gdtpoint> L)
	{
	if (v!=source(e))
		L.reverse();
		//for (int i=1; i<L.size(); i++) L.move_to_front(L.get_item(i));
	(*edge_info)[e].bends_position = L;
	}
	
	
	
	void
	dime_orth_plan_undi_graph::
set_source
	(gdtedge e, gdtnode v)
	{
	if (v == target(e))
		{
		plan_undi_graph::set_source(e,v);
		// reverse the bends_position list of gdtedge e
		//
		gdt::gdtlist<gdt::gdtpoint>& L = (*edge_info)[e].bends_position;
		//for (int i=1; i<L.size(); i++) L.move_to_front(L.get_item(i));
		L.reverse();
		}
	}



	border_step
	dime_orth_plan_undi_graph::
find_first_border_step_with_defined_heading_around_face
	(face f) const
	{
	border_step s;
	forall_face_border_steps(s,f)
		if (initial_heading_of_border_step(s) != undefined_heading) return s;
	return NULL;
	}



	std::string
	dime_orth_plan_undi_graph::
heading_to_string
	(heading_type h)
	{
	std::string s;
	switch (h)
		{
		case north: s = "north"	    ; break;
		case south: s = "south"	    ; break;
		case east : s = "east" 	    ; break;
		case west : s = "west" 	    ; break;
		default   : s = "no heading"; break;
		}
	return s;
	}



	void
	dime_orth_plan_undi_graph::
build_orthogonal_flow_network 
	(
	flow_dire_graph& fn, 
	gdt::gdtedge_map<gdtedge>& fn_edge, 
	gdt::gdtlist<gdtedge>& L, 
	slope_type scan_edge_slope
	) const
	{
	gdtnode ffn;
	gdtedge e,efn;
	
	// ---------------------------
	// create the face-nodes of fn 
	// ---------------------------
	
	face f;
	gdt::gdtmap<face,gdtnode>	face_node(NULL_NODE);	// gdtnode --> face-gdtnode mapping
	
	gdtnode S = fn.new_node ();	// dummy-source gdtnode for external face
	gdtnode T = fn.new_node ();	// dummy-target gdtnode for external face
	
	fn.set_balance(S, L.size());
	fn.set_balance(T,-L.size());
		
	forall_faces (f,*this)
		if (f != external_face())
			{
			ffn = fn.new_node ();
			fn.set_balance(ffn,0);
			face_node[f] = ffn;
			}
	
	// -----------------------------------------------------------------------------------
	// create all the edges of fn. All the dummy edges added by ractangularization are set
	// to 0 cost and the others are set to 1 cost. Also, all the edges have lower_cap = 1.
	// Finally, the upper_cap of the edges associated to the longest path L is set to 1, 
	// and for the remaining edges it is set to INFINITE.
	// -----------------------------------------------------------------------------------
	
	forall_edges (e,*this)
		switch (scan_edge_slope)
			{
			case horizontal: 
				if (slope_of_edge(e) == horizontal)
					{
					face low_f, high_f;
					border_step s = border_step_moving_along_edge (e,source(e));
					if (initial_heading_of_border_step(s) == east) 
						{
						low_f  = face_of_border_step(s);
						high_f = face_of_border_step(opposite_border_step(s));
						}
					else if (initial_heading_of_border_step(s) == west)
						{
						high_f = face_of_border_step(s);
						low_f  = face_of_border_step(opposite_border_step(s));
						}
					else gdt_error("invalid heading");
					
					gdtnode vfn, wfn;
					if (low_f == external_face())  vfn = S;
					else			       vfn = face_node[low_f];
					if (high_f == external_face()) wfn = T;
					else			       wfn = face_node[high_f];
					
					efn = fn.new_edge (vfn,wfn,id(e));
					fn_edge[e] = efn;
					int uc = INFINITE,
					    lc = 1,
					    co = 1,
					    fl = 0;
					if (is_marked(e,RM3_ADDED_BY_RECTANGULARIZATION)) co = 0;
					fn.set_edge_info (efn,uc,lc,co,fl);	
					}
				break;
				
			case vertical  : 
				if (slope_of_edge(e) == vertical)
					{
					face left_f, right_f;
					border_step s = border_step_moving_along_edge (e,source(e));
					if (initial_heading_of_border_step(s) == south) 
						{
						left_f  = face_of_border_step(s);
						right_f = face_of_border_step(opposite_border_step(s));
						}
					else if (initial_heading_of_border_step(s) == north)
						{
						right_f = face_of_border_step(s);
						left_f  = face_of_border_step(opposite_border_step(s));
						}
					else gdt_error("invalid heading");
					
					gdtnode vfn, wfn;
					if (left_f == external_face())  vfn = S;
					else			        vfn = face_node[left_f];
					if (right_f == external_face()) wfn = T;
					else			        wfn = face_node[right_f];
					
					efn = fn.new_edge (vfn,wfn,id(e));
					fn_edge[e] = efn;
					int uc = INFINITE,
					    lc = 1,
					    co = 1,
					    fl = 0;
					if (is_marked(e,RM3_ADDED_BY_RECTANGULARIZATION)) co = 0;
					fn.set_edge_info (efn,uc,lc,co,fl);	
					}				
				break;	
			default: break;
			}
			
	forall (e,L) fn.set_upper_capacity(fn_edge[e],1);
	}

//Added by Marcandalli 16 10 2001

//Da rivedere
void dime_orth_plan_undi_graph::undefine(gdtnode n) {
	gdtedge e;
	forall_edges_adjacent_node(e, n, *this) undefine(e);
	node_info->undefine(n);
}

void dime_orth_plan_undi_graph::undefine(gdtedge e) {
	border_step b1 = border_step_moving_along_edge(e,source(e));
	border_step b2 = border_step_moving_along_edge(e,target(e));
	border_step_info->undefine(b1);
	border_step_info->undefine(b2);
	edge_info->undefine(e);
}
//












