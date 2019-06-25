/*******************************************************************************
+
+  _rm3_constraints.c
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


#include <GDT/rm3_constraints.h>
#include <GDT/rm3_global.h>
#include <GDT/rm3_undi_graph.h>
#include <GDT/gdt_error.h>

using namespace std;

//-----------------------------------------------------------------------------
// struct_constraint
//
// (1998)
//-----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// 	PROTECTED
// -----------------------------------------------------------------------------

	struct_constraint::
struct_constraint 
	(constraint_type t, 
	undi_graph& ug)
	{
	set_type(t);
	set_owner_graph(ug);
	gdt::list_item li = ug.constraints->append(this);
	set_pos_in_owner_graph_list(li);
	}



	void
	struct_constraint::
remove_from_owner_graph_list
	()
	{
	get_owner_graph().constraints->del_item(get_pos_in_owner_graph_list());
	}



	void
	struct_constraint::
remove_from_edge_list
	(gdtedge e)
	{
	undi_graph&       ug = get_owner_graph();
	gdt::gdtlist<constraint>& Le = ug.all_constraints(e);
	
	// linear-time cost
	//
	constraint c;
	gdt::list_item it;
	forall_items (it, Le)
		{
		c = Le.inf(it);
		if (c == this) break;
		}
	Le.del_item(it);			
	}



	void
	struct_constraint::
remove_from_node_list
	(gdtnode v)
	{
	undi_graph& 	  ug = get_owner_graph();
	gdt::gdtlist<constraint>& Lv = ug.all_constraints(v);
	
	// linear-time cost
	//
	constraint c;
	gdt::list_item it;
	forall_items (it, Lv)
		{
		c = Lv.inf(it);
		if (c == this) break;
		}
	Lv.del_item(it);			
	}



	gdt::list_item
	struct_constraint::
add_to_edge_list
	(gdtedge e)
	{
	undi_graph&	  ug = get_owner_graph();
	gdt::gdtlist<constraint>& Le = ug.all_constraints(e); 
	return Le.append(this);
	}



	gdt::list_item
	struct_constraint::
add_to_node_list
	(gdtnode v)
	{
	undi_graph& 	  ug = get_owner_graph();
	gdt::gdtlist<constraint>& Lv = ug.all_constraints(v);
	return Lv.append(this);
	}





// -----------------------------------------------------------------------------
// 	PRIVATE
// -----------------------------------------------------------------------------


	void 
	struct_constraint::
set_id
	(const int new_id)
	{
	id = new_id;
	}
	

	
	void 
	struct_constraint::
set_type
	(const constraint_type new_type)
	{
	type = new_type;
	}



	void 
	struct_constraint::
set_owner_graph
	(undi_graph& ug)
	{
	owner_graph = &ug;
	}

	

	void 
	struct_constraint::
set_pos_in_owner_graph_list
	(const gdt::list_item li)
	{
	pos_in_owner_graph_list = li;
	}



// -----------------------------------------------------------------------------
// 	PUBLIC
// -----------------------------------------------------------------------------


	struct_constraint::
~struct_constraint 
	()
	{
	remove_from_owner_graph_list();
	}


	int 
	struct_constraint::
get_id
	() const
	{
	return id;
	}



	constraint_type 
	struct_constraint::
get_type
	() const
	{
	return type;
	}



	undi_graph&	
	struct_constraint::
get_owner_graph
	() const
	{
	return *owner_graph;
	}
	
	
	
	gdt::list_item 	
	struct_constraint::
get_pos_in_owner_graph_list
	() const
	{
	return pos_in_owner_graph_list;
	}



//-----------------------------------------------------------------------------
// struct_uncrossable_edge_constraint 
// 
// (1998)
//-----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// 	PRIVATE
// -----------------------------------------------------------------------------



		
	void 
	struct_constraint_uncrossable_edge::
set_uncrossable_edge
	(gdtedge e)
	{
	uncrossable_edge = e;
	}



	struct_constraint_uncrossable_edge::
struct_constraint_uncrossable_edge 
	(gdtedge e, undi_graph& ug) : struct_constraint(UNCROSSABLE_EDGES,ug)
	{
	//cout << endl << "costruttore di " << this << endl;
	set_uncrossable_edge(e);
	add_to_edge_list    (e); 
	}


// -----------------------------------------------------------------------------
// 	PROTECTED
// -----------------------------------------------------------------------------


	bool 
	struct_constraint_uncrossable_edge::
update_after_del_edge 
	(gdtedge e)
	{
	gdtedge ue = get_uncrossable_edge();
	if (ue == e) return true;
	else	     return false;	
	}
	


	bool
	struct_constraint_uncrossable_edge::
update_after_del_node
	(gdtnode v)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	cout << "\nNode" << get_owner_graph().id(v) 
	     << "cannot be involved in this constraint" << flush; // to avoid warning
	return false;
	}



	bool 
	struct_constraint_uncrossable_edge::
update_after_split_edge 
	(gdtedge e, gdtedge e1, gdtedge e2)
	{
	gdtedge ue = get_uncrossable_edge ();
	if (ue != e) return false;
	
	// --------------------------------
	// make two new constraints similar 
	// to this on edges e1 and e2
	// --------------------------------
	
	undi_graph& ug = get_owner_graph();
	ug.new_constraint_uncrossable_edge (e1);
	ug.new_constraint_uncrossable_edge (e2);
	
	// -------------------------------
	// this constraint will be deleted 
	// at the end of the method	
	// -------------------------------
	return true;
	}
			
	
		
	bool 
	struct_constraint_uncrossable_edge::	
update_after_split_node 
	(gdtnode v, gdtnode v1, gdtnode v2)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	cout << "\nNodes" << get_owner_graph().id(v) 
	     << ", " << get_owner_graph().id(v1) 
	     << ", " << get_owner_graph().id(v2)
	     << "cannot be involved in this constraint" << flush; // to avoid warning
	return false;
	}
		
		
		
	bool 
	struct_constraint_uncrossable_edge::
update_after_merge_edges 
	(gdtedge e1, gdtedge e2, gdtedge e)
	{
	gdtedge ue = get_uncrossable_edge ();
	if (ue != e1) return false;
	
	// --------------------------------------
	// make a new constraint similar to this,
	// but with gdtedge e instead of e1
	// --------------------------------------
	
	undi_graph& ug = get_owner_graph();
	ug.new_constraint_uncrossable_edge(e);
		
	// ------------------------------------	
	// delete all constraints involving e2, 
	// being the same type as this	   
	// ------------------------------------
	
	constraint c = NULL_CONSTRAINT;
	gdt::gdtlist<constraint> Le2 = ug.constraints_on_edge(e2);
	//forall (c,Le2) if (c->get_type() == get_type()) ug.del_constraint(c);

	for(
		 list_item loop_private_358= ( (((Le2).size()!=0)?(c=(Le2).inf((Le2).first()),0):0) , (Le2).first() ) ;
	
	loop_private_358 ;
	
	loop_private_358=(Le2).succ(loop_private_358),((loop_private_358!=0) ? (c=(Le2).inf(loop_private_358),0):0) )

 if (c->get_type() == get_type()) ug.del_constraint(c);
	
	// -------------------------------
	// this constraint will be deleted 
	// at the end of method
	// -------------------------------
	
	return true;
	}
				
		
		
	bool 
	struct_constraint_uncrossable_edge::
update_after_merge_nodes 
	(gdtnode v1, gdtnode v2, gdtnode v)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	cout << "\nNodes" << get_owner_graph().id(v) 
	     << ", " << get_owner_graph().id(v1) 
	     << ", " << get_owner_graph().id(v2)
	     << "cannot be involved in this constraint" << flush; // to avoid warning
	return false;
	}



	void
	struct_constraint_uncrossable_edge:: 
update_after_edge_translation 
	(
	gdtedge e  , 
	gdtnode ve1, 
	gdtnode ve2, 
	gdtedge d  ,
	gdtnode vd1,
	gdtnode vd2
	)
	{
	gdtedge ue = get_uncrossable_edge();
	if (ue != e) 
		gdt_error("gdtedge is not involved in constraint"); 

	// -------------------------------
	// check consistency of parameters
	// 	  (avoid warnings)
	// -------------------------------
	
	undi_graph& ug = get_owner_graph();
	if ( 
	   (!ug.node_belongs_to_edge(ve1,e)) || 
	   (!ug.node_belongs_to_edge(ve2,e)) ||
	   (!ug.node_belongs_to_edge(vd1,d)) ||
	   (!ug.node_belongs_to_edge(vd2,d))
	   )	gdt_error("invalid nodes or edges"); 


	// ------------------------------------------------
	// translate parameters and update constraint-lists
	// ------------------------------------------------
	
	set_uncrossable_edge (d);	// set the new uncrossable gdtedge 
	remove_from_edge_list(e);	// remove constraint from constraint-list of gdtedge e
	add_to_edge_list     (d);	// add constraint to constraint-list of gdtedge d
	}			



	void 
	struct_constraint_uncrossable_edge::
update_on_path_replacing_edge
	(
	gdtedge e,
	gdt::gdtlist<gdtedge> path,
	undi_graph& ug
	)
	{
	gdtedge ue = get_uncrossable_edge();
	if (ue == e)
		{
		// -----------------------------------
		// make a sequence of new constraints,
		// with the same type as this, one
		// for each gdtedge of path in ug
		// -----------------------------------
		
		gdtedge d = NULL_EDGE;
		forall (d,path) ug.new_constraint_uncrossable_edge (d);
		}
		
	else; // nothing to do
	}			



// -----------------------------------------------------------------------------
// 	PUBLIC
// -----------------------------------------------------------------------------


	struct_constraint_uncrossable_edge::
~struct_constraint_uncrossable_edge 
	() 
	{
	//cout << endl << "distruttore di " << this << endl;
	gdtedge		       e  = get_uncrossable_edge();
	remove_from_edge_list (e);
	}



		
	gdtedge 
	struct_constraint_uncrossable_edge::
get_uncrossable_edge 
	() const
	{
	return uncrossable_edge;
	}



	constraint 
	struct_constraint_uncrossable_edge::
copy
	(undi_graph& ug)
	{
	undi_graph& ug0 = get_owner_graph();
	gdtedge e0  	= get_uncrossable_edge();
	gdtedge e   	= ug.corresponding_edge(e0, ug0);
	
	if (!e) return NULL_CONSTRAINT;	// copy is not possible

	int c_id = get_id();
	int new_id;
	
	if (ug.find_constraint(c_id)) 	new_id = AUTO_ID;
	else 				new_id = c_id;

	return ug.new_constraint_uncrossable_edge(e, new_id);
	}



	void 
	struct_constraint_uncrossable_edge::
print 
	(ostream& os) const
	{
	os << "\n-UNCROSSABLE_EDGE constraint" << flush;
	os << "\n constraint number : " << get_id() << flush;
	os << "\n on gdtedge number    : " << get_owner_graph().id(get_uncrossable_edge()) << flush;
	}


//-----------------------------------------------------------------------------
// struct_constraint_number_of_bends_on_edge
//
// (1998)
//-----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// 	PRIVATE
// -----------------------------------------------------------------------------



		
	void 
	struct_constraint_number_of_bends_on_edge::
set_edge_with_nbc
	(gdtedge e)
	{
	edge_with_nbc = e;
	}


		
	void 
	struct_constraint_number_of_bends_on_edge::
set_nbc
	(bend_constraint b)
	{
	nbc = b;
	}



	struct_constraint_number_of_bends_on_edge::
struct_constraint_number_of_bends_on_edge 
	(gdtedge e, bend_constraint b, undi_graph& ug) : struct_constraint(NUMBER_OF_BENDS_ON_EDGE,ug)
	{
	id=NULL_ID;
	set_edge_with_nbc (e);
	set_nbc		  (b);
	add_to_edge_list  (e);
	}


// -----------------------------------------------------------------------------
// 	PROTECTED
// -----------------------------------------------------------------------------

	bool 
	struct_constraint_number_of_bends_on_edge::
update_after_del_edge 
	(gdtedge e)
	{
	gdtedge nbce = get_edge_with_nbc();
	if (nbce == e) return true;
	else	       return false;	
	}



	bool
	struct_constraint_number_of_bends_on_edge::
update_after_del_node
	(gdtnode v)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	cout << "\nNode" << get_owner_graph().id(v) 
	     << "cannot be involved in this constraint" << flush;	// to avoid warning

	return false;
	}



	bool 
	struct_constraint_number_of_bends_on_edge::
update_after_split_edge 
	(gdtedge e, gdtedge e1, gdtedge e2)
	{
	gdtedge nbce = get_edge_with_nbc();
	if (nbce != e) return false;
	
	// ---------------------------------
	// make two new constraints, similar 
	// to this, on edges e1 and e2
	// ---------------------------------
	
	undi_graph& 	ug = get_owner_graph();
	bend_constraint bc = get_nbc(); 	
	ug.new_constraint_number_of_bends_on_edge (e1,bc);
	ug.new_constraint_number_of_bends_on_edge (e2,bc);
	
	// --------------------------
	// constraint will be deleted
	// at the end of the method
	// --------------------------
	
	return true;
	}
			
	
		
	bool 
	struct_constraint_number_of_bends_on_edge::	
update_after_split_node 
	(gdtnode v, gdtnode v1, gdtnode v2)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	cout << "\nNodes" << get_owner_graph().id(v) 
	     << ", " << get_owner_graph().id(v1) 
	     << ", " << get_owner_graph().id(v2)
	     << "cannot be involved in this constraint" << flush; // to avoid warning
	return false;
	}
		
		
		
	bool 
	struct_constraint_number_of_bends_on_edge::
update_after_merge_edges
	(gdtedge e1, gdtedge e2, gdtedge e)
	{
	gdtedge nbce = get_edge_with_nbc ();
	if (nbce != e1) return false;
	
	// --------------------------------------
	// make a new constraint similar to this, 
	// but with gdtedge e instead of e1
	// --------------------------------------
	
	undi_graph& ug = get_owner_graph();
	ug.new_constraint_number_of_bends_on_edge(e,get_nbc());
	
	// ------------------------------------
	// delete all constraints involving e2, 
	// being the same type as this	   
	// ------------------------------------
	
	constraint c = NULL_CONSTRAINT;
	gdt::gdtlist<constraint> Le2 = ug.constraints_on_edge(e2);
	forall (c,Le2) if (c->get_type() == get_type()) ug.del_constraint(c);
	
	// -------------------------------
	// this constraint will be deleted 
	// at the end of the method
	// -------------------------------
	
	return true;
	}
				
		
		
	bool 
	struct_constraint_number_of_bends_on_edge::
update_after_merge_nodes
	(gdtnode v1, gdtnode v2, gdtnode v)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	cout << "\nNodes" << get_owner_graph().id(v) 
	     << ", " << get_owner_graph().id(v1) 
	     << ", " << get_owner_graph().id(v2)
	     << "cannot be involved in this constraint" << flush; // to avoid warning
	return false;
	}



	void
	struct_constraint_number_of_bends_on_edge:: 
update_after_edge_translation 
	(
	gdtedge e  , 
	gdtnode ve1, 
	gdtnode ve2, 
	gdtedge d  ,
	gdtnode vd1,
	gdtnode vd2
	)
	{
	gdtedge nbce = get_edge_with_nbc();
	if (nbce != e) 
		gdt_error("gdtedge is not involved in constraint"); 
	
	// -------------------------------
	// check consistency of parameters
	// 	  (avoid warnings)	
	// -------------------------------
	
	undi_graph& ug = get_owner_graph();
	if ( 
	   (!ug.node_belongs_to_edge(ve1,e)) || 
	   (!ug.node_belongs_to_edge(ve2,e)) ||
	   (!ug.node_belongs_to_edge(vd1,d)) ||
	   (!ug.node_belongs_to_edge(vd2,d))
	   )	gdt_error("invalid nodes or edges"); 
	
	
	// ------------------------------------------------
	// translate parameters and update constraint-lists
	// ------------------------------------------------

	set_edge_with_nbc    (d);	// set the new gdtedge with number of bends constraint 
	remove_from_edge_list(e);	// remove constraint from constraint-list of gdtedge e
	add_to_edge_list     (d);	// add constraint to constraint-list of gdtedge d
	}			



	void 
	struct_constraint_number_of_bends_on_edge::
update_on_path_replacing_edge
	(
	gdtedge e,
	gdt::gdtlist<gdtedge> path,
	undi_graph& ug
	)
	{
	gdtedge nbce = get_edge_with_nbc();
	if (nbce == e)
		{
		// -----------------------------------
		// make a sequence of new constraints,
		// with the same type as this, one
		// for each gdtedge of path in ug
		// -----------------------------------
		
		bend_constraint bc = get_nbc();
		gdtedge d = NULL_EDGE;
		forall (d,path) ug.new_constraint_number_of_bends_on_edge (d,bc);
		}
		
	else; // nothing to do
	}	


// -----------------------------------------------------------------------------
// 	PUBLIC
// -----------------------------------------------------------------------------


	struct_constraint_number_of_bends_on_edge::
~struct_constraint_number_of_bends_on_edge 
	() 
	{
	gdtedge		       e  = get_edge_with_nbc();
	remove_from_edge_list (e);
	}



		
	gdtedge 
	struct_constraint_number_of_bends_on_edge::
get_edge_with_nbc 
	() const
	{
	return edge_with_nbc;
	}



	
	bend_constraint
	struct_constraint_number_of_bends_on_edge::
get_nbc
	() const
	{
	return nbc;
	}	




	constraint 
	struct_constraint_number_of_bends_on_edge::
copy
	(undi_graph& ug)
	{
	undi_graph& ug0    = get_owner_graph();
	gdtedge e0  	   = get_edge_with_nbc();
	bend_constraint b0 = get_nbc();
	
	gdtedge e = ug.corresponding_edge(e0, ug0);
	
	// CHECK TO REMOVE
	// ---------------
	//cout << "\ncopying gdtedge " << ug0.id(e0) << flush; 
	//cout << "\non gdtedge " ;
	//if (e) cout << ug.id(e) << flush;
	//else   cout << "NULL" << flush;
	// ---------------
	
	if (!e) return NULL_CONSTRAINT; // copy is not possible
	
	int c_id = get_id();
	int new_id;
	
	if (ug.find_constraint(c_id)) 	new_id = AUTO_ID;
	else 				new_id = c_id;

	return ug.new_constraint_number_of_bends_on_edge(e,b0,new_id);
	}



	void 
	struct_constraint_number_of_bends_on_edge::
print 
	(ostream& os) const
	{
	os << "\n-NUMBER_OF_BENDS_ON_EDGE constraint" << flush;
	os << "\n constraint number : " << get_id() << flush;
	os << "\n on gdtedge number    : " << get_owner_graph().id(get_edge_with_nbc()) << flush;
	std::string s;
	switch (get_nbc())
		{
		case NONE   : s = "NONE"   ; break;
		case ANY    : s = "ANY"    ; break;
		default	    : break;	
		}
	os << "\n constraint type   : " << s << flush;
	}



//-----------------------------------------------------------------------------
// struct_constraint_bend_direction_on_edge
//
// (1998)
//-----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// 	PRIVATE
// -----------------------------------------------------------------------------



		
	void 
	struct_constraint_bend_direction_on_edge::
set_edge_with_bdc
	(gdtedge e)
	{
	edge_with_bdc = e;
	}


		
	void 
	struct_constraint_bend_direction_on_edge::
set_start_node
	(gdtnode v)
	{
	start_node = v;
	}


	struct_constraint_bend_direction_on_edge::
struct_constraint_bend_direction_on_edge 
	(gdtedge e, gdtnode v, undi_graph& ug) : struct_constraint(BEND_DIRECTION_ON_EDGE,ug)
	{
	id=NULL_ID;
	set_edge_with_bdc (e);
	set_start_node	  (v);
	add_to_edge_list  (e);
	add_to_node_list  (v);
	}


// -----------------------------------------------------------------------------
// 	PROTECTED
// -----------------------------------------------------------------------------

	bool 
	struct_constraint_bend_direction_on_edge::
update_after_del_edge 
	(gdtedge e)
	{
	gdtedge bdce = get_edge_with_bdc();
	if (bdce == e) return true;
	else	       return false;	
	}
	


	bool
	struct_constraint_bend_direction_on_edge::
update_after_del_node
	(gdtnode v)
	{
	gdtnode sn = get_start_node();
	if (sn == v) return true;
	else	     return false;
	}



	bool 
	struct_constraint_bend_direction_on_edge::
update_after_split_edge 
	(gdtedge e, gdtedge e1, gdtedge e2)
	{
	gdtedge bdce = get_edge_with_bdc();
	if (bdce != e) return false;
	
	// ---------------------------------
	// make two new constraints, similar 
	// to this, on edges e1 and e2
	// ---------------------------------
	
	undi_graph& ug = get_owner_graph();
	gdtnode 	    sn = get_start_node();
	
	gdtnode v  = ug.node_between_adj_edges (e1,e2);	// gdtnode produced by splitting
	gdtnode v1 = ug.opposite(v,e1);
	gdtnode v2 = ug.opposite(v,e2);
	
	if (v1 == sn) 
		{
		ug.new_constraint_bend_direction_on_edge (e1,v1);
		ug.new_constraint_bend_direction_on_edge (e2,v);
		}
	else if (v2 == sn)
		{
		ug.new_constraint_bend_direction_on_edge (e2,v2);
		ug.new_constraint_bend_direction_on_edge (e1,v);
		}
	else gdt_error("invalid gdtedge e1 or e2"); 

	// --------------------------
	// constraint will be deleted
	// at the end of the method
	// --------------------------
	
	return true;
	}
			
	
		
	bool 
	struct_constraint_bend_direction_on_edge::	
update_after_split_node 
	(gdtnode v, gdtnode v1, gdtnode v2)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	return false;
	
	cout << "\nNodes" << get_owner_graph().id(v) 
	     << ", " << get_owner_graph().id(v1) 
	     << ", " << get_owner_graph().id(v2)
	     << "nothing to do" << flush; 		// to avoid warning
	}
		
		
		
	bool 
	struct_constraint_bend_direction_on_edge::
update_after_merge_edges
	(gdtedge e1, gdtedge e2, gdtedge e)
	{
	gdtedge bdce = get_edge_with_bdc ();
	if (bdce != e1) return false;
	
	// --------------------------------------
	// make a new constraint similar to this, 
	// but with gdtedge e instead of e1
	// --------------------------------------
	
	undi_graph& ug = get_owner_graph();
	gdtnode 	    sn = get_start_node ();
	gdtnode 	    v  = ug.node_between_adj_edges (e1,e2);
	gdtnode 	    v1 = ug.opposite (v,e1);
	gdtnode 	    v2 = ug.opposite (v,e2);
	
	if 	(v1 == sn) ug.new_constraint_bend_direction_on_edge(e,v1);
	else if (v  == sn) ug.new_constraint_bend_direction_on_edge(e,v2);
	else gdt_error("invalid gdtedge e1 or e2");
	
	// ------------------------------------
	// delete all constraints involving e2, 
	// being the same type as this	   
	// ------------------------------------
	
	constraint c = NULL_CONSTRAINT;
	gdt::gdtlist<constraint> Le2 = ug.constraints_on_edge(e2);
	forall (c,Le2) if (c->get_type() == get_type()) ug.del_constraint(c);
	
	// -------------------------------
	// this constraint will be deleted 
	// at the end of the method
	// -------------------------------
	
	return true;
	}
				
		
		
	bool 
	struct_constraint_bend_direction_on_edge::
update_after_merge_nodes
	(gdtnode v1, gdtnode v2, gdtnode v)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	return false;
	
	cout << "\nNodes" << get_owner_graph().id(v) 
	     << ", " << get_owner_graph().id(v1) 
	     << ", " << get_owner_graph().id(v2)
	     << "nothing to do" << flush; 		// to avoid warning			
	}



	void
	struct_constraint_bend_direction_on_edge:: 
update_after_edge_translation 
	(
	gdtedge e  , 
	gdtnode ve1, 
	gdtnode ve2, 
	gdtedge d  ,
	gdtnode vd1,
	gdtnode vd2
	)
	{
	gdtedge bdce = get_edge_with_bdc();
	if (bdce != e) 
		gdt_error("gdtedge is not involved in constraint"); 
	
	// -------------------------------
	// check consistency of parameters
	// 	  (avoid warnings)	
	// -------------------------------
	
	undi_graph& ug = get_owner_graph();
	if ( 
	   (!ug.node_belongs_to_edge(ve1,e)) ||
	   (!ug.node_belongs_to_edge(ve2,e)) ||
	   (!ug.node_belongs_to_edge(vd1,d)) ||
	   (!ug.node_belongs_to_edge(vd2,d))
	   )	gdt_error("invalid nodes or edges"); 
	
	gdtnode sne = get_start_node();
	gdtnode snd = NULL_NODE;
	
	if (sne == ve1) 	snd = vd1;
	else if (sne == ve2)	snd = vd2;
	else gdt_error("constraint does not consistent"); 
		
	// ------------------------------------------------
	// translate parameters and update constraint-lists
	// ------------------------------------------------
		
	set_edge_with_bdc    (d);	// set the new gdtedge with number of bends constraint 
	remove_from_edge_list(e);	// remove constraint from constraint-list of gdtedge e
	add_to_edge_list     (d);	// add constraint to constraint-list of gdtedge d
	set_start_node	     (snd);	// set the new start gdtnode
	remove_from_node_list(sne);	// remove constraint from constraint-list of gdtedge e
	add_to_node_list     (snd);	// add constraint to constraint-list of gdtedge d	
	}			



	void 
	struct_constraint_bend_direction_on_edge::
update_on_path_replacing_edge
	(
	gdtedge e0,
	gdt::gdtlist<gdtedge> path,
	undi_graph& ug
	)
	{
	gdtedge bdce = get_edge_with_bdc();
	if (bdce == e0)
		{
		// -----------------------------------
		// make a sequence of new constraints,
		// with the same type as this, one
		// for each gdtedge of path in ug
		// -----------------------------------
		undi_graph& ug0 = get_owner_graph();
		gdtnode sn0 = get_start_node ();
		gdtedge e1 = path.head();		// first gdtedge of path
		gdtedge e2 = path.tail();		// last  gdtedge of path
		gdtnode sn = NULL_NODE;
		
		if 	(ug.id(ug.source(e1)) == ug0.id(sn0))  sn = ug.source(e1);
		else if (ug.id(ug.target(e1)) == ug0.id(sn0))  sn = ug.target(e1);		
		else if (ug.id(ug.source(e2)) == ug0.id(sn0)) {sn = ug.source(e2); path.reverse();}
		else if (ug.id(ug.target(e2)) == ug0.id(sn0)) {sn = ug.target(e2); path.reverse();}
		gdtedge e = NULL_EDGE;
		forall (e,path)
			{
			ug.new_constraint_bend_direction_on_edge(e,sn);
			sn = ug.opposite(sn,e);
			}
		}
		
	else; // nothing to do
	}	


// -----------------------------------------------------------------------------
// 	PUBLIC
// -----------------------------------------------------------------------------


	struct_constraint_bend_direction_on_edge::
~struct_constraint_bend_direction_on_edge 
	() 
	{
	gdtedge e = get_edge_with_bdc();
	gdtnode v = get_start_node ();
	remove_from_edge_list (e);
	remove_from_node_list (v);
	}



		
	gdtedge 
	struct_constraint_bend_direction_on_edge::
get_edge_with_bdc 
	() const
	{
	return edge_with_bdc;
	}



	
	gdtnode
	struct_constraint_bend_direction_on_edge::
get_start_node
	() const
	{
	return start_node;
	}	



	constraint 
	struct_constraint_bend_direction_on_edge::
copy
	(undi_graph& ug)
	{
	undi_graph& ug0 = get_owner_graph();
	gdtedge e0  	= get_edge_with_bdc();
	gdtnode v0 	= get_start_node();
	
	gdtedge e = ug.corresponding_edge (e0, ug0);
	gdtnode v = ug.corresponding_node (v0, ug0);
	
	if (!e) return NULL_CONSTRAINT; 			   // copy is not possible
	if (!v) return NULL_CONSTRAINT;				   // copy is not possible
	if (!ug.node_belongs_to_edge(v,e)) return NULL_CONSTRAINT; // copy is not posiible
	
	
	int c_id = get_id();
	int new_id;
	
	if (ug.find_constraint(c_id)) 	new_id = AUTO_ID;
	else 				new_id = c_id;

	return ug.new_constraint_bend_direction_on_edge(e,v,new_id);
	}



	void 
	struct_constraint_bend_direction_on_edge::
print 
	(ostream& os) const
	{
	os << "\n-BEND_DIRECTION_ON_EDGE constraint" << flush;
	os << "\n constraint number : " << get_id() << flush;
	os << "\n on gdtedge number    : " << get_owner_graph().id(get_edge_with_bdc()) << flush;
	os << "\n with start gdtnode   : " << get_owner_graph().id(get_start_node()) << flush;
	}




//-----------------------------------------------------------------------------
// struct_constraint_same_face_node
// 
// (1998)
//-----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// 	PRIVATE
// -----------------------------------------------------------------------------


		
	void 
	struct_constraint_same_face_node::
set_same_face_node
	(gdtnode n)
	{
	same_face_node = n;
	}


		
	void 
	struct_constraint_same_face_node::
set_face_label
	(int num)
	{
	face_label = num;
	}


	struct_constraint_same_face_node::
struct_constraint_same_face_node
	(gdtnode n, undi_graph& ug, int num) : struct_constraint(SAME_FACE_NODES,ug)
	{
	set_same_face_node (n);
	set_face_label     (num);
	add_to_node_list   (n); 
	}


// -----------------------------------------------------------------------------
// 	PROTECED
// -----------------------------------------------------------------------------


	bool 
	struct_constraint_same_face_node::
update_after_del_edge 
	(gdtedge e)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	cout << "\nEdge" << get_owner_graph().id(e) 
	     << "cannot be involved in this constraint" << flush; // to avoid warning
	return false;
	}
	


	bool
	struct_constraint_same_face_node::
update_after_del_node
	(gdtnode n)
	{
	gdtnode un = get_same_face_node();
	if (un == n) return true;
	else	     return false;	
	}



	bool 
	struct_constraint_same_face_node::
update_after_split_edge 
	(gdtedge e, gdtedge e1, gdtedge e2)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	cout << "\nEdges" << get_owner_graph().id(e) 
	     << ", " << get_owner_graph().id(e1) 
	     << ", " << get_owner_graph().id(e2)
	     << " cannot be involved in this constraint" << flush; // to avoid warning
	return false;
	}
			
	
		
	bool 
	struct_constraint_same_face_node::	
update_after_split_node 
	(gdtnode n, gdtnode n1, gdtnode n2)
	{
	gdtnode un = get_same_face_node ();
	if (un != n) return false;
	
	// --------------------------------
	// make two new constraints similar 
	// to this on edges e1 and e2
	// --------------------------------
	
	int         fl 	= get_face_label();
	undi_graph& ug  = get_owner_graph();
	ug.new_constraint_same_face_node (n1, fl);
	ug.new_constraint_same_face_node (n2, fl);
	
	// -------------------------------
	// this constraint will be deleted 
	// at the end of the method	
	// -------------------------------
	return true;

	}
		

		
	bool 
	struct_constraint_same_face_node::
update_after_merge_edges 
	(gdtedge e1, gdtedge e2, gdtedge e)
	{
	undi_graph& ug = get_owner_graph ();
	gdtnode n = ug.node_between_adj_edges (e1,e2);
	if (n != get_same_face_node()) return false;
	
	// -------------------------------
	// this constraint will be deleted 
	// at the end of method
	// -------------------------------
	
	else return true;

	cout << ug.id(e); 	// to avoid a compilation warning "unused parameter"
	}
				
		
		
	bool 
	struct_constraint_same_face_node::
update_after_merge_nodes 
	(gdtnode n1, gdtnode n2, gdtnode n)
	{
	gdtnode sfn = get_same_face_node();
	if (sfn != n1) return false;
	
	// --------------------------------------
	// make a new constraint similar to this, 
	// but with gdtnode n instead of n1
	// --------------------------------------
	
	undi_graph& ug = get_owner_graph ();
	int         fl = get_face_label  ();
	ug.new_constraint_same_face_node(n, fl);
	
	// ------------------------------------
	// delete all constraints involving n2, 
	// being the same type as this	   
	// ------------------------------------
	
	constraint c = NULL_CONSTRAINT;
	gdt::gdtlist<constraint> Ln2 = ug.constraints_on_node(n2);
	forall (c,Ln2) if (c->get_type() == get_type()) ug.del_constraint(c);
	
	// -------------------------------
	// this constraint will be deleted 
	// at the end of the method
	// -------------------------------
	
	return true;
	}



	void
	struct_constraint_same_face_node:: 
update_after_edge_translation 
	(
	gdtedge e  , 
	gdtnode ve1, 
	gdtnode ve2, 
	gdtedge d  ,
	gdtnode vd1,
	gdtnode vd2
	)
	{
	// ---------------
	// nothing to do
	// ---------------

	return;	

	// the following is to avoid compilation warning "unused parameter"
	//
	cout << get_owner_graph().id(e) 
	     << get_owner_graph().id(d) 
	     << get_owner_graph().id(ve1) 
	     << get_owner_graph().id(ve2) 
	     << get_owner_graph().id(vd1) 
	     << get_owner_graph().id(vd2) << flush; 	

	}			



	void 
	struct_constraint_same_face_node::
update_on_path_replacing_edge
	(
	gdtedge e,
	gdt::gdtlist<gdtedge> path,
	undi_graph& ug
	)
	{
	// ---------------
	// nothing to do
	// ---------------

	return;
	
	// the following is to avoid compilation warning "unused parameter"
	//	
	cout << ug.id(e) << flush;
	path.head();
	}			




// -----------------------------------------------------------------------------
// 	PUBLIC
// -----------------------------------------------------------------------------


	struct_constraint_same_face_node::
~struct_constraint_same_face_node
	() 
	{
	gdtnode		       n  = get_same_face_node();
	remove_from_node_list (n);
	}



		
	gdtnode 
	struct_constraint_same_face_node::
get_same_face_node 
	() const
	{
	return same_face_node;
	}



		
	int
	struct_constraint_same_face_node::
get_face_label
	() const
	{
	return face_label;
	}



	constraint 
	struct_constraint_same_face_node::
copy
	(undi_graph& ug)
	{
	undi_graph& ug0 = get_owner_graph();
	gdtnode n0		= get_same_face_node();
	gdtnode n   	= ug.corresponding_node(n0, ug0);
	
	if (!n) return NULL_CONSTRAINT;	// copy is not possible

	int c_id = get_id();
	int fl = get_face_label();
	int new_id;
	
	if (ug.find_constraint(c_id)) 	new_id = AUTO_ID;
	else 				new_id = c_id;

	return ug.new_constraint_same_face_node(n, fl, new_id); 
	}



	void 
	struct_constraint_same_face_node::
print 
	(ostream& os) const
	{
	os << "\n-SAME_FACE_NODES constraint" << flush;
	os << "\n constraint number : " << get_id() << flush;
	os << "\n on gdtnode number    : " << get_owner_graph().id(get_same_face_node()) << flush;
	os << "\n with face label   : " << get_face_label() << flush;
	}


//-----------------------------------------------------------------------------
// struct_constraint_same_face_ordered_nodes
// 
// (1998)
//-----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// 	PRIVATE
// -----------------------------------------------------------------------------

	void 
	struct_constraint_same_face_ordered_nodes::
set_node_list
	(gdt::gdtlist<gdtnode>& Ln)
	{
	node_list = Ln;
	}


	struct_constraint_same_face_ordered_nodes::
struct_constraint_same_face_ordered_nodes
	(gdt::gdtlist<gdtnode> Ln, undi_graph& ug) : struct_constraint(SAME_FACE_ORDERED_NODES,ug)
	{
	id=NULL_ID;
	set_node_list (Ln);
	gdtnode n = NULL_NODE;
	forall (n,Ln)
		{
		add_to_node_list (n); 
		}
	}


// -----------------------------------------------------------------------------
// 	PROTECED
// -----------------------------------------------------------------------------


	bool 
	struct_constraint_same_face_ordered_nodes::
update_after_del_edge 
	(gdtedge e)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	cout << "\nEdge" << get_owner_graph().id(e) 
	     << "cannot be involved in this constraint" << flush; // to avoid warning
	return false;
	}
	


	bool
	struct_constraint_same_face_ordered_nodes::
update_after_del_node
	(gdtnode n)
	{
	bool found = false;
	gdtnode sfon = NULL_NODE;
	
	forall (sfon, node_list)
		{
		if(n == sfon) found = true; 
		}
	
	if (found) 	return true;
	else		return false;	
	}



	bool 
	struct_constraint_same_face_ordered_nodes::
update_after_split_edge 
	(gdtedge e, gdtedge e1, gdtedge e2)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	return false;

	// to avoid compilation warning "unused parameter"
	//
	cout << "\nEdges" << get_owner_graph().id(e) 
	     << ", " << get_owner_graph().id(e1) 
	     << ", " << get_owner_graph().id(e2)
	     << " cannot be involved in this constraint" << flush; 
	}
			
	
		
	bool 
	struct_constraint_same_face_ordered_nodes::	
update_after_split_node 
	(gdtnode n, gdtnode n1, gdtnode n2)
	{
	gdt::list_item li;	// gdt::list_item cursor
	gdt::list_item lip;	// previous gdt::list_item
	gdt::gdtlist<gdtnode> Ln = get_node_list();

	forall_items (li,Ln)
		{
		if( Ln.inf(li) == n )
			{
			lip = Ln.cyclic_pred(li);
			Ln.erase(li);
			Ln.insert(n1,lip,gdt::after);
			Ln.insert(n2,lip,gdt::after);
			add_to_node_list(n1);
			add_to_node_list(n2);
			remove_from_node_list(n);
			break;
			}
		}

	return false;	// don't delete this constraint
	}
		
		
		
	bool 
	struct_constraint_same_face_ordered_nodes::
update_after_merge_edges 
	(gdtedge e1, gdtedge e2, gdtedge e)
	{
	bool output = false;
	undi_graph& ug = get_owner_graph ();
	gdtnode n0 = ug.node_between_adj_edges (e1,e2);
	gdt::gdtlist<gdtnode> Ln = get_node_list();
	
	gdtnode n = NULL_NODE;
	forall(n,Ln)
		{
		if( n == n0 ) 	output = true;
		}
	
	return output;

	cout << ug.id(e); 	// to avoid the compilation warning 
				// "unused parameter"
	}
				
		
		
	bool 
	struct_constraint_same_face_ordered_nodes::
update_after_merge_nodes 
	(gdtnode n1, gdtnode n2, gdtnode n)
	{
	gdt::gdtlist<gdtnode> Ln = get_node_list();
	gdt::list_item li;
	gdt::list_item li1 = NULL;
	gdt::list_item li2 = NULL;
	gdt::list_item lip = NULL;	// previous list item
	
	forall_items (li,Ln)
		{
		gdtnode np = Ln.inf(li);
		if( np == n1 ) li1 = li;
		if( np == n2 ) li2 = li;
		}

	forall_items (li,Ln)
		{
		gdtnode np = Ln.inf(li);
		if( np == n1 || np == n2 )		// the first you find ...
			{
			lip = Ln.cyclic_pred(li);	// ... set lip and exit
			break;
			}
		}
		
	if( li1 != NULL ) 	// you found them ( li1 and li2 )
		{
		Ln.erase(li1);
		Ln.erase(li2);
		Ln.insert(n,lip,gdt::after);
		add_to_node_list(n);
		remove_from_node_list(n1);
		remove_from_node_list(n2);
		}

	return false;
	}



	void
	struct_constraint_same_face_ordered_nodes:: 
update_after_edge_translation 
	(
	gdtedge e  , 
	gdtnode ve1, 
	gdtnode ve2, 
	gdtedge d  ,
	gdtnode vd1,
	gdtnode vd2
	)
	{
	// ---------------
	// nothing to do
	// ---------------

	return;	

	// the following is to avoid compilation warning "unused parameter"
	//
	cout << get_owner_graph().id(e) 
	     << get_owner_graph().id(d) 
	     << get_owner_graph().id(ve1) 
	     << get_owner_graph().id(ve2) 
	     << get_owner_graph().id(vd1) 
	     << get_owner_graph().id(vd2) << flush; 	

	}			



	void 
	struct_constraint_same_face_ordered_nodes::
update_on_path_replacing_edge
	(
	gdtedge e,
	gdt::gdtlist<gdtedge> path,
	undi_graph& ug
	)
	{
	// ---------------
	// nothing to do
	// ---------------

	return;
	
	// the following is to avoid compilation warning "unused parameter"
	//	
	cout << ug.id(e) << flush;
	path.head();
	}			


// -----------------------------------------------------------------------------
// 	PUBLIC
// -----------------------------------------------------------------------------


	struct_constraint_same_face_ordered_nodes::
~struct_constraint_same_face_ordered_nodes
	() 
	{
	gdtnode n = NULL_NODE;
	forall(n,node_list)
		{
		remove_from_node_list(n);
		}
	}



		
	gdt::gdtlist<gdtnode>
	struct_constraint_same_face_ordered_nodes::
get_node_list 
	() const
	{
	return node_list;
	}



	constraint 
	struct_constraint_same_face_ordered_nodes::
copy
	(undi_graph& ug)
	{
	undi_graph& ug0 = get_owner_graph();
	
	gdtnode       n = NULL_NODE;
	gdtnode       new_node = NULL_NODE;
	gdt::gdtlist<gdtnode> new_list;

	forall (n,node_list)
		{
		new_node = ug.corresponding_node(n,ug0);
		if( new_node != NULL_NODE )
			new_list.append(new_node);
		}
	
	if( new_list.size() == 0 ) return NULL_CONSTRAINT;

	int c_id = get_id();
	int new_id;
	
	if (ug.find_constraint(c_id)) 	new_id = AUTO_ID;
	else 				new_id = c_id;

	return ug.new_constraint_same_face_ordered_nodes(new_list, new_id); // ???
	}



	void 
	struct_constraint_same_face_ordered_nodes::
print 
	(ostream& os) const
	{
	os << "\n-SAME_FACE_ORDERED_NODES constraint" << flush;
	os << "\n constraint number : " << get_id() << flush;
	os << "\n on nodes number   : ";
	gdtnode n = NULL_NODE;
	forall(n, node_list)
		{
		os << get_owner_graph().id(n) << " " << flush;
		}
	}


//-----------------------------------------------------------------------------
// struct_constraint_node_width
//-----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// 	PRIVATE
// -----------------------------------------------------------------------------

	void 
	struct_constraint_node_width::
set_constrained_node
	(gdtnode n)
	{
	constrained_node = n;
	}


	void 
	struct_constraint_node_width::
set_value
	(double x)
	{
	value = x;
	}



	struct_constraint_node_width::
struct_constraint_node_width 
	(gdtnode n, double x, undi_graph& ug) : struct_constraint(NODE_WIDTH,ug)
	{
	//cout << "Costruttore " << this << endl;
	id=NULL_ID;
	set_constrained_node(n);
        set_value(x);
	add_to_node_list(n); 
	}

// -----------------------------------------------------------------------------
// 	PROTECTED
// -----------------------------------------------------------------------------


	bool 
	struct_constraint_node_width::
update_after_del_edge 
	(gdtedge e)
	{
	// ---------------
	// nothing to do
	// ---------------

	cout << "\nEdge" << get_owner_graph().id(e) 
	     << "cannot be involved in this constraint" << flush; // to avoid warning
	return false;		
	}


	bool
	struct_constraint_node_width::
update_after_del_node
	(gdtnode v)
	{
	gdtnode n2 = get_constrained_node();
	if (n2 == v) return true;     // delete this structure
	else	     return false;    // don't delete this structure
	}


	bool 
	struct_constraint_node_width::
update_after_split_edge 
	(gdtedge e, gdtedge e1, gdtedge e2)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	cout << "\nEdges" << get_owner_graph().id(e) 
	     << ", " << get_owner_graph().id(e1) 
	     << ", " << get_owner_graph().id(e2)
	     << "cannot be involved in this constraint" << flush; // to avoid warning
	return false;
	}
			
		
	bool 
	struct_constraint_node_width::	
update_after_split_node 
	(gdtnode v, gdtnode v1, gdtnode v2)
	{
	if ( v != get_constrained_node() ) return false;
	
	// --------------------------------
	// make two new constraints similar 
	// to this on nodes v1 and v2
	// --------------------------------
	
	undi_graph& ug = get_owner_graph();
        double x = get_value();
	ug.new_constraint_node_width(v1, x);
	ug.new_constraint_node_width(v2, x);
	
	return true;      // delete this constraint at the end of the calling method
	}
		
		
	bool 
	struct_constraint_node_width::
update_after_merge_edges 
	(gdtedge e1, gdtedge e2, gdtedge e)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	cout << "\nEdges" << get_owner_graph().id(e) 
	     << ", " << get_owner_graph().id(e1) 
	     << ", " << get_owner_graph().id(e2)
	     << "cannot be involved in this constraint" << flush; // to avoid warning
	return false;
	}
				
		
	bool 
	struct_constraint_node_width::
update_after_merge_nodes 
	(gdtnode v1, gdtnode v2, gdtnode v)
	{
	gdtnode cn = get_constrained_node ();
	if (cn != v1) return false;               // don't delete this constraint
	
	// --------------------------------------
	// make a new constraint similar to this,
	// but with gdtnode v instead of v1
	// --------------------------------------
	
	undi_graph& ug = get_owner_graph();
        double x = get_value();
	ug.new_constraint_node_width(v, x);
		
	// ------------------------------------	
	// delete all constraints involving v2, 
	// being the same type as this	   
	// ------------------------------------
	
	constraint c = NULL_CONSTRAINT;
	gdt::gdtlist<constraint> Lv2 = ug.constraints_on_node(v2);
	forall (c,Lv2) if (c->get_type() == get_type()) ug.del_constraint(c);
	
	// -------------------------------
	// this constraint will be deleted 
	// at the end of method
	// -------------------------------
	
	return true;

	}


	void
	struct_constraint_node_width:: 
update_after_edge_translation 
	(
	gdtedge e  , 
	gdtnode ve1, 
	gdtnode ve2, 
	gdtedge d  ,
	gdtnode vd1,
	gdtnode vd2
	)
	{
	// ---------------
	// nothing to do
	// ---------------

	return;	

	// the following is to avoid compilation warning "unused parameter"
	//
	cout << get_owner_graph().id(e) 
	     << get_owner_graph().id(d) 
	     << get_owner_graph().id(ve1) 
	     << get_owner_graph().id(ve2) 
	     << get_owner_graph().id(vd1) 
	     << get_owner_graph().id(vd2) << flush; 	
        }


	void 
	struct_constraint_node_width::
update_on_path_replacing_edge
	(
	gdtedge e,
	gdt::gdtlist<gdtedge> path,
	undi_graph& ug
	)
        {
	// ---------------
	// nothing to do
	// ---------------

	return;
	
	// the following is to avoid compilation warning "unused parameter"
	//	
	cout << ug.id(e) << flush;
	path.head();
	}			



// -----------------------------------------------------------------------------
// 	PUBLIC
// -----------------------------------------------------------------------------


	struct_constraint_node_width::
~struct_constraint_node_width
	() 
	{
	//cout << "Distruttore di " << this << endl;
	gdtnode n = get_constrained_node();
	remove_from_node_list(n);
	}

	gdtnode 
	struct_constraint_node_width::
get_constrained_node 
	() const
	{
	return constrained_node;
	}


	double
	struct_constraint_node_width::
get_value
	() const
	{
	return value;
	}


	constraint 
	struct_constraint_node_width::
copy
	(undi_graph& ug)
	{
	undi_graph& ug0 = get_owner_graph();
	gdtnode n0  	= get_constrained_node();
        double x        = get_value();
	gdtnode n   	= ug.corresponding_node(n0, ug0);
	
	if (!n) return NULL_CONSTRAINT;	// copy is not possible

	int c_id = get_id();
	int new_id;
	
	if (ug.find_constraint(c_id)) 	new_id = AUTO_ID;
	else 				new_id = c_id;

	return ug.new_constraint_node_width(n, x, new_id);
	}



	void 
	struct_constraint_node_width::
print 
	(ostream& os) const
	{
	os << "\n-NODE_WIDTH constraint" << flush;
	os << "\n constraint number : " << get_id() << flush;
	os << "\n on gdtnode number    : " << get_owner_graph().id(get_constrained_node()) << flush;
	os << "\n value             : " << get_value() << flush;
	}



//-----------------------------------------------------------------------------
// struct_constraint_node_height
//-----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// 	PRIVATE
// -----------------------------------------------------------------------------

	void 
	struct_constraint_node_height::
set_constrained_node
	(gdtnode n)
	{
	constrained_node = n;
	}


	void 
	struct_constraint_node_height::
set_value
	(double x)
	{
	value = x;
	}



	struct_constraint_node_height::
struct_constraint_node_height 
	(gdtnode n, double x, undi_graph& ug) : struct_constraint(NODE_HEIGHT,ug)
	{
	id=NULL_ID;
	set_constrained_node(n);
        set_value(x);
	add_to_node_list(n); 
	}

// -----------------------------------------------------------------------------
// 	PROTECTED
// -----------------------------------------------------------------------------


	bool 
	struct_constraint_node_height::
update_after_del_edge 
	(gdtedge e)
	{
	// ---------------
	// nothing to do
	// ---------------

	cout << "\nEdge" << get_owner_graph().id(e) 
	     << "cannot be involved in this constraint" << flush; // to avoid warning
	return false;		
	}


	bool
	struct_constraint_node_height::
update_after_del_node
	(gdtnode v)
	{
	gdtnode n2 = get_constrained_node();
	if (n2 == v) return true;     // delete this structure
	else	     return false;    // don't delete this structure
	}


	bool 
	struct_constraint_node_height::
update_after_split_edge 
	(gdtedge e, gdtedge e1, gdtedge e2)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	cout << "\nEdges" << get_owner_graph().id(e) 
	     << ", " << get_owner_graph().id(e1) 
	     << ", " << get_owner_graph().id(e2)
	     << "cannot be involved in this constraint" << flush; // to avoid warning
	return false;
	}
			
		
	bool 
	struct_constraint_node_height::	
update_after_split_node 
	(gdtnode v, gdtnode v1, gdtnode v2)
	{
	if ( v != get_constrained_node() ) return false;
	
	// --------------------------------
	// make two new constraints similar 
	// to this on nodes v1 and v2
	// --------------------------------
	
	undi_graph& ug = get_owner_graph();
        double x = get_value();
	ug.new_constraint_node_height(v1, x);
	ug.new_constraint_node_height(v2, x);
	
	return true;      // delete this constraint at the end of the calling method
	}
		
		
	bool 
	struct_constraint_node_height::
update_after_merge_edges 
	(gdtedge e1, gdtedge e2, gdtedge e)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	cout << "\nEodes" << get_owner_graph().id(e) 
	     << ", " << get_owner_graph().id(e1) 
	     << ", " << get_owner_graph().id(e2)
	     << "cannot be involved in this constraint" << flush; // to avoid warning
	return false;
	}
				
		
	bool 
	struct_constraint_node_height::
update_after_merge_nodes 
	(gdtnode v1, gdtnode v2, gdtnode v)
	{
	gdtnode cn = get_constrained_node ();
	if (cn != v1) return false;               // don't delete this constraint
	
	// --------------------------------------
	// make a new constraint similar to this,
	// but with gdtnode v instead of v1
	// --------------------------------------
	
	undi_graph& ug = get_owner_graph();
        double x = get_value();
	ug.new_constraint_node_height(v, x);
		
	// ------------------------------------	
	// delete all constraints involving v2, 
	// being the same type as this	   
	// ------------------------------------
	
	constraint c = NULL_CONSTRAINT;
	gdt::gdtlist<constraint> Lv2 = ug.constraints_on_node(v2);
	forall (c,Lv2) if (c->get_type() == get_type()) ug.del_constraint(c);
	
	// -------------------------------
	// this constraint will be deleted 
	// at the end of method
	// -------------------------------
	
	return true;

	}


	void
	struct_constraint_node_height:: 
update_after_edge_translation 
	(
	gdtedge e  , 
	gdtnode ve1, 
	gdtnode ve2, 
	gdtedge d  ,
	gdtnode vd1,
	gdtnode vd2
	)
	{
	// ---------------
	// nothing to do
	// ---------------

	return;	

	// the following is to avoid compilation warning "unused parameter"
	//
	cout << get_owner_graph().id(e) 
	     << get_owner_graph().id(d) 
	     << get_owner_graph().id(ve1) 
	     << get_owner_graph().id(ve2) 
	     << get_owner_graph().id(vd1) 
	     << get_owner_graph().id(vd2) << flush; 	
        }


	void 
	struct_constraint_node_height::
update_on_path_replacing_edge
	(
	gdtedge e,
	gdt::gdtlist<gdtedge> path,
	undi_graph& ug
	)
        {
	// ---------------
	// nothing to do
	// ---------------

	return;
	
	// the following is to avoid compilation warning "unused parameter"
	//	
	cout << ug.id(e) << flush;
	path.head();
	}			



// -----------------------------------------------------------------------------
// 	PUBLIC
// -----------------------------------------------------------------------------


	struct_constraint_node_height::
~struct_constraint_node_height
	() 
	{
	gdtnode n = get_constrained_node();
	remove_from_node_list(n);
	}

	gdtnode 
	struct_constraint_node_height::
get_constrained_node 
	() const
	{
	return constrained_node;
	}


	double
	struct_constraint_node_height::
get_value
	() const
	{
	return value;
	}


	constraint 
	struct_constraint_node_height::
copy
	(undi_graph& ug)
	{
	undi_graph& ug0 = get_owner_graph();
	gdtnode n0  	= get_constrained_node();
        double x        = get_value();
	gdtnode n   	= ug.corresponding_node(n0, ug0);
	
	if (!n) return NULL_CONSTRAINT;	// copy is not possible

	int c_id = get_id();
	int new_id;
	
	if (ug.find_constraint(c_id)) 	new_id = AUTO_ID;
	else 				new_id = c_id;

	return ug.new_constraint_node_height(n, x, new_id);
	}



	void 
	struct_constraint_node_height::
print 
	(ostream& os) const
	{
	os << "\n-NODE_HEIGHT constraint" << flush;
	os << "\n constraint number : " << get_id() << flush;
	os << "\n on gdtnode number    : " << get_owner_graph().id(get_constrained_node()) << flush;
	os << "\n value             : " << get_value() << flush;
	}



//-----------------------------------------------------------------------------
// struct_constraint_angle_sweep
//-----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// 	PRIVATE
// -----------------------------------------------------------------------------

	void 
	struct_constraint_angle_sweep::
set_ref_node
	(gdtnode v)
	{
	ref_node = v;
	}


	void 
	struct_constraint_angle_sweep::
set_ref_edge
	(gdtedge e)
	{
	ref_edge = e;
	}



	void 
	struct_constraint_angle_sweep::
set_angle_value
	(angle_type alpha)
	{
	angle_value = alpha;
	}



	struct_constraint_angle_sweep::
struct_constraint_angle_sweep 
	(gdtnode v, gdtedge e, angle_type alpha, undi_graph& ug) : struct_constraint(ANGLE_SWEEP,ug)
	{
	id=NULL_ID;
	set_ref_node(v);
	set_ref_edge(e);
        set_angle_value(alpha);
	add_to_node_list(v);
	add_to_edge_list(e); 
	}

// -----------------------------------------------------------------------------
// 	PROTECTED
// -----------------------------------------------------------------------------

	bool 
	struct_constraint_angle_sweep::
update_after_del_edge 
	(gdtedge e)
	{
	gdtedge re = get_ref_edge();
	if (re == e) return true;
	else	     return false;	
	}
	


	bool
	struct_constraint_angle_sweep::
update_after_del_node
	(gdtnode v)
	{
	gdtnode rn = get_ref_node();
	if (rn == v) return true;
	else	     return false;
	}



	bool 
	struct_constraint_angle_sweep::
update_after_split_edge 
	(gdtedge e, gdtedge e1, gdtedge e2)
	{
	gdtedge re = get_ref_edge();
	if (re != e) return false;
	
	gdtnode 	   rn    = get_ref_node();
	angle_type alpha = get_angle_value();
	
	// ---------------------------------
	// keep the constraint on the gdtedge
	// that is still incident to "rn". 
	// after the splitting
	// ---------------------------------
	
	undi_graph& ug = get_owner_graph();

	gdtnode v  = ug.node_between_adj_edges (e1,e2);	// gdtnode produced by the splitting
	gdtnode v1 = ug.opposite(v,e1);
	gdtnode v2 = ug.opposite(v,e2);
	
	if 	(v1 == rn) ug.new_constraint_angle_sweep (rn,e1,alpha);
	else if (v2 == rn) ug.new_constraint_angle_sweep (rn,e2,alpha);
	else gdt_error("invalid gdtedge e1 or e2"); 

	// --------------------------
	// constraint will be deleted
	// at the end of the method
	// --------------------------
	
	return true;
	}
			
	
		
	bool 
	struct_constraint_angle_sweep::	
update_after_split_node 
	(gdtnode v, gdtnode v1, gdtnode v2)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	return false;
	
	cout << "\nNodes" << get_owner_graph().id(v) 
	     << ", " << get_owner_graph().id(v1) 
	     << ", " << get_owner_graph().id(v2)
	     << "nothing to do" << flush; 		// to avoid warning
	}
		
		
		
	bool 
	struct_constraint_angle_sweep::
update_after_merge_edges
	(gdtedge e1, gdtedge e2, gdtedge e)
	{
	gdtedge re = get_ref_edge ();
	if (re != e1) return false;
	
	// --------------------------------------
	// make a new constraint similar to this, 
	// but with gdtedge e instead of e1
	// --------------------------------------
	
	undi_graph& ug    = get_owner_graph();
	gdtnode 	    rn    = get_ref_node   ();
	angle_type  alpha = get_angle_value();
	
	gdtnode u = ug.source(e);
	gdtnode v = ug.target(e);
	
	if ((rn == u) || (rn == v)) ug.new_constraint_angle_sweep (rn,e,alpha);

	// -------------------------------
	// this constraint will be deleted 
	// at the end of the method
	// -------------------------------
	
	return true;
	
	cout << ug.id(e2);  // just to avoid warning "unused parameters"
	}
				
		
		
	bool 
	struct_constraint_angle_sweep::
update_after_merge_nodes
	(gdtnode v1, gdtnode v2, gdtnode v)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	return false;
	
	cout << "\nNodes" << get_owner_graph().id(v) 
	     << ", " << get_owner_graph().id(v1) 
	     << ", " << get_owner_graph().id(v2)
	     << "nothing to do" << flush; 		// to avoid warning			
	}




	void
	struct_constraint_angle_sweep:: 
update_after_edge_translation 
	(
	gdtedge e  , 
	gdtnode ve1, 
	gdtnode ve2, 
	gdtedge d  ,
	gdtnode vd1,
	gdtnode vd2
	)
	{
	gdtedge re = get_ref_edge ();
	if (re != e) 
		gdt_error("gdtedge is not involved in constraint"); 
	
	// -------------------------------
	// check consistency of parameters
	// 	 (to avoid warnings)	
	// -------------------------------
	
	undi_graph& ug = get_owner_graph();
	if ( 
	   (!ug.node_belongs_to_edge(ve1,e)) || 
	   (!ug.node_belongs_to_edge(ve2,e)) ||
	   (!ug.node_belongs_to_edge(vd1,d)) ||
	   (!ug.node_belongs_to_edge(vd2,d))
	   )	gdt_error("invalid nodes or edges"); 
	
	gdtnode rne = get_ref_node();
	gdtnode rnd = NULL_NODE;
	
	if (rne == ve1) 	rnd = vd1;
	else if (rne == ve2)	rnd = vd2;
	else gdt_error("the constraint is not consistent"); 
		
	// ------------------------------------------------
	// translate parameters and update constraint-lists
	// ------------------------------------------------
		
	set_ref_edge   	     (d);	// set the new reference gdtedge 
	remove_from_edge_list(e);	// remove constraint from constraint-list of gdtedge e
	add_to_edge_list     (d);	// add constraint to constraint-list of gdtedge d
	set_ref_node	     (rnd);	// set the new reference gdtnode
	remove_from_node_list(rne);	// remove constraint from constraint-list of gdtedge e
	add_to_node_list     (rnd);	// add constraint to constraint-list of gdtedge d	
	}			



	void 
	struct_constraint_angle_sweep::
update_on_path_replacing_edge
	(
	gdtedge e0,
	gdt::gdtlist<gdtedge> path,
	undi_graph& ug
	)
	{
	gdtedge re = get_ref_edge();
	if (re == e0)
		{
		// --------------------------
		// copy the constraint on an 
		// terminal gdtedge of the path
		// --------------------------
		
		undi_graph& ug0 = get_owner_graph();
		gdtnode 	    rn  = get_ref_node();
		
		gdtedge e1 = path.head();		// first gdtedge of the path
		gdtedge e2 = path.tail();		// last  gdtedge of the path
		
		gdtedge re_new = NULL_EDGE;
		gdtnode rn_new = NULL_NODE;
		
		if 	(ug.id(ug.source(e1)) == ug0.id(rn))  {rn_new = ug.source(e1); re_new = e1;}
		else if (ug.id(ug.target(e1)) == ug0.id(rn))  {rn_new = ug.target(e1); re_new = e1;}		
		else if (ug.id(ug.source(e2)) == ug0.id(rn))  {rn_new = ug.source(e2); re_new = e2;} 
		else if (ug.id(ug.target(e2)) == ug0.id(rn))  {rn_new = ug.target(e2); re_new = e2;}
		
		ug.new_constraint_angle_sweep (rn_new, re_new, get_angle_value());
		}
		
		
	else; // nothing to do
	}	


// -----------------------------------------------------------------------------
// 	PUBLIC
// -----------------------------------------------------------------------------


	struct_constraint_angle_sweep::
~struct_constraint_angle_sweep 
	() 
	{
	gdtedge e = get_ref_edge();
	gdtnode v = get_ref_node ();
	remove_from_edge_list (e);
	remove_from_node_list (v);
	}



		
	gdtedge 
	struct_constraint_angle_sweep::
get_ref_edge 
	() const
	{
	return ref_edge;
	}



	
	gdtnode
	struct_constraint_angle_sweep::
get_ref_node
	() const
	{
	return ref_node;
	}	


	angle_type
	struct_constraint_angle_sweep::
get_angle_value
	() const
	{
	return angle_value;
	}	


	constraint 
	struct_constraint_angle_sweep::
copy
	(undi_graph& ug)
	{
	undi_graph& ug0  = get_owner_graph();
	gdtedge e0  	 = get_ref_edge();
	gdtnode v0 	 = get_ref_node();
	angle_type alpha = get_angle_value();
	
	gdtedge e = ug.corresponding_edge (e0, ug0);
	gdtnode v = ug.corresponding_node (v0, ug0);
	
	if (!e) return NULL_CONSTRAINT; 			   // the copy is not possible
	if (!v) return NULL_CONSTRAINT;				   // the copy is not possible
	if (!ug.node_belongs_to_edge(v,e)) return NULL_CONSTRAINT; // the copy is not possible
	
	int c_id = get_id();
	int new_id;
	
	if (ug.find_constraint(c_id)) 	new_id = AUTO_ID;
	else 				new_id = c_id;

	return ug.new_constraint_angle_sweep (v,e,alpha,new_id);
	}



	void 
	struct_constraint_angle_sweep::
print 
	(ostream& os) const
	{
	angle_type alpha = get_angle_value();
	std::string a;
	switch (alpha)
		{
		case _000: a = "0"  	   ; break;
		case _090: a = "90" 	   ; break;
		case _180: a = "180"	   ; break;
		case _270: a = "270"	   ; break;
		case _360: a = "360"	   ; break;
		default  : a = "UNDEFINED" ; break;
		}
	
	os << "\n-ANGLE SWEEP constraint" << flush;
	os << "\n constraint number 	      : " << get_id() << flush;
	os << "\n angle sweep                 : " << a << flush;
	os << "\n to the right of gdtedge number : " << get_owner_graph().id(get_ref_edge()) << flush;
	os << "\n starting from gdtnode          : " << get_owner_graph().id(get_ref_node()) << flush;
	}


//-----------------------------------------------------------------------------
// struct_constraint_edge_attachment_wrt_previous_corner
//-----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// 	PRIVATE
// -----------------------------------------------------------------------------

	void 
	struct_constraint_edge_attachment_wrt_previous_corner::
set_ref_node
	(gdtnode v)
	{
	ref_node = v;
	}


	void 
	struct_constraint_edge_attachment_wrt_previous_corner::
set_ref_edge
	(gdtedge e)
	{
	ref_edge = e;
	}



	void 
	struct_constraint_edge_attachment_wrt_previous_corner::
set_attachment
	(int value)
	{
	attachment = value;
	}



	struct_constraint_edge_attachment_wrt_previous_corner::
struct_constraint_edge_attachment_wrt_previous_corner
	(gdtnode v, gdtedge e, int value, undi_graph& ug) : struct_constraint(EDGE_ATTACHMENT_WRT_PREVIOUS_CORNER,ug)
	{
	id=NULL_ID;
	set_ref_node(v);
	set_ref_edge(e);
        set_attachment(value);
	add_to_node_list(v);
	add_to_edge_list(e); 
	}

// -----------------------------------------------------------------------------
// 	PROTECTED
// -----------------------------------------------------------------------------

	bool 
	struct_constraint_edge_attachment_wrt_previous_corner::
update_after_del_edge 
	(gdtedge e)
	{
	gdtedge re = get_ref_edge();
	if (re == e) return true;	// i.e. delete this constraint
	else	     return false;	
	}
	


	bool
	struct_constraint_edge_attachment_wrt_previous_corner::
update_after_del_node
	(gdtnode v)
	{
	gdtnode rn = get_ref_node();
	if (rn == v) return true;
	else	     return false;
	}



	bool 
	struct_constraint_edge_attachment_wrt_previous_corner::
update_after_split_edge 
	(gdtedge e, gdtedge e1, gdtedge e2)
	{
	gdtedge re = get_ref_edge();
	if (re != e) return false;
	
	gdtnode	rn    	= get_ref_node();
	int 	value 	= get_attachment();
	
	// ---------------------------------
	// keep the constraint on the gdtedge
	// that is still incident to "rn". 
	// after the splitting
	// ---------------------------------
	
	undi_graph& ug = get_owner_graph();

	gdtnode v  = ug.node_between_adj_edges (e1,e2);	// gdtnode produced by the splitting
	gdtnode v1 = ug.opposite(v,e1);
	gdtnode v2 = ug.opposite(v,e2);
	
	if 	(v1 == rn) ug.new_constraint_edge_attachment_wrt_previous_corner (rn,e1,value);
	else if (v2 == rn) ug.new_constraint_edge_attachment_wrt_previous_corner (rn,e2,value);
	else gdt_error("invalid gdtedge e1 or e2"); 

	// --------------------------
	// constraint will be deleted
	// at the end of the method
	// --------------------------
	
	return true;
	}
			
	
		
	bool 
	struct_constraint_edge_attachment_wrt_previous_corner::	
update_after_split_node 
	(gdtnode v, gdtnode v1, gdtnode v2)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	return false;
	
	cout << "\nNodes" << get_owner_graph().id(v) 
	     << ", " << get_owner_graph().id(v1) 
	     << ", " << get_owner_graph().id(v2)
	     << "nothing to do" << flush; 		// to avoid warning
	}
		
		
		
	bool 
	struct_constraint_edge_attachment_wrt_previous_corner::
update_after_merge_edges
	(gdtedge e1, gdtedge e2, gdtedge e)
	{
	gdtedge re = get_ref_edge ();
	if (re != e1) return false;
	
	// --------------------------------------
	// make a new constraint similar to this, 
	// but with gdtedge e instead of e1
	// --------------------------------------
	
	undi_graph& ug    = get_owner_graph();
	gdtnode 	    rn    = get_ref_node   ();
	int 	    value = get_attachment ();
	
	gdtnode u = ug.source(e);
	gdtnode v = ug.target(e);
	
	if ((rn == u) || (rn == v)) ug.new_constraint_edge_attachment_wrt_previous_corner (rn,e,value);

	// -------------------------------
	// this constraint will be deleted 
	// at the end of the method
	// -------------------------------
	
	return true;
	
	cout << ug.id(e2);  // just to avoid compilation warning "unused parameters"
	}
				
		
		
	bool 
	struct_constraint_edge_attachment_wrt_previous_corner::
update_after_merge_nodes
	(gdtnode v1, gdtnode v2, gdtnode v)
	{
	// ---------------
	// nothing to do
	// ---------------
	
	return false;
	
	cout << "\nNodes" << get_owner_graph().id(v) 
	     << ", " << get_owner_graph().id(v1) 
	     << ", " << get_owner_graph().id(v2)
	     << "nothing to do" << flush; 		// to avoid warning			
	}




	void
	struct_constraint_edge_attachment_wrt_previous_corner:: 
update_after_edge_translation 
	(
	gdtedge e  , 
	gdtnode ve1, 
	gdtnode ve2, 
	gdtedge d  ,
	gdtnode vd1,
	gdtnode vd2
	)
	{
	gdtedge re = get_ref_edge ();
	if (re != e) 
		gdt_error("gdtedge is not involved in constraint"); 
	
	// -------------------------------
	// check consistency of parameters
	// 	 (to avoid warnings)	
	// -------------------------------
	
	undi_graph& ug = get_owner_graph();
	if ( 
	   (!ug.node_belongs_to_edge(ve1,e)) || 
	   (!ug.node_belongs_to_edge(ve2,e)) ||
	   (!ug.node_belongs_to_edge(vd1,d)) ||
	   (!ug.node_belongs_to_edge(vd2,d))
	   )	gdt_error("invalid nodes or edges"); 
	
	gdtnode rne = get_ref_node();
	gdtnode rnd = NULL_NODE;
	
	if (rne == ve1) 	rnd = vd1;
	else if (rne == ve2)	rnd = vd2;
	else gdt_error("the constraint is not consistent"); 
		
	// ------------------------------------------------
	// translate parameters and update constraint-lists
	// ------------------------------------------------
		
	set_ref_edge   	     (d);	// set the new reference gdtedge 
	remove_from_edge_list(e);	// remove constraint from constraint-list of gdtedge e
	add_to_edge_list     (d);	// add constraint to constraint-list of gdtedge d
	set_ref_node	     (rnd);	// set the new reference gdtnode
	remove_from_node_list(rne);	// remove constraint from constraint-list of gdtedge e
	add_to_node_list     (rnd);	// add constraint to constraint-list of gdtedge d	
	}			



	void 
	struct_constraint_edge_attachment_wrt_previous_corner::
update_on_path_replacing_edge
	(
	gdtedge e0,
	gdt::gdtlist<gdtedge> path,
	undi_graph& ug
	)
	{
	gdtedge re = get_ref_edge();
	if (re == e0)
		{
		// --------------------------
		// copy the constraint on a
		// terminal gdtedge of the path
		// --------------------------
		
		undi_graph& ug0 = get_owner_graph();
		gdtnode 	    rn  = get_ref_node();
		
		gdtedge e1 = path.head();		// first gdtedge of the path
		gdtedge e2 = path.tail();		// last  gdtedge of the path
		
		gdtedge re_new = NULL_EDGE;
		gdtnode rn_new = NULL_NODE;
		
		if 	(ug.id(ug.source(e1)) == ug0.id(rn))  {rn_new = ug.source(e1); re_new = e1;}
		else if (ug.id(ug.target(e1)) == ug0.id(rn))  {rn_new = ug.target(e1); re_new = e1;}		
		else if (ug.id(ug.source(e2)) == ug0.id(rn))  {rn_new = ug.source(e2); re_new = e2;} 
		else if (ug.id(ug.target(e2)) == ug0.id(rn))  {rn_new = ug.target(e2); re_new = e2;}
		
		ug.new_constraint_edge_attachment_wrt_previous_corner (rn_new, re_new, get_attachment());
		}
		
		
	else; // nothing to do
	}	


// -----------------------------------------------------------------------------
// 	PUBLIC
// -----------------------------------------------------------------------------


	struct_constraint_edge_attachment_wrt_previous_corner::
~struct_constraint_edge_attachment_wrt_previous_corner
	() 
	{
	gdtedge e = get_ref_edge();
	gdtnode v = get_ref_node ();
	remove_from_edge_list (e);
	remove_from_node_list (v);
	}

		
	gdtedge 
	struct_constraint_edge_attachment_wrt_previous_corner::
get_ref_edge 
	() const
	{
	return ref_edge;
	}

	
	gdtnode
	struct_constraint_edge_attachment_wrt_previous_corner::
get_ref_node
	() const
	{
	return ref_node;
	}	


	int
	struct_constraint_edge_attachment_wrt_previous_corner::
get_attachment
	() const
	{
	return attachment;
	}	


	constraint 
	struct_constraint_edge_attachment_wrt_previous_corner::
copy
	(undi_graph& ug)
	{
	undi_graph& ug0  = get_owner_graph();
	gdtedge e0  	 = get_ref_edge();
	gdtnode v0 	 = get_ref_node();
	int value        = get_attachment();
	
	gdtedge e = ug.corresponding_edge (e0, ug0);
	gdtnode v = ug.corresponding_node (v0, ug0);
	
	if (!e) return NULL_CONSTRAINT; 			   // the copy is not possible
	if (!v) return NULL_CONSTRAINT;				   // the copy is not possible
	if (!ug.node_belongs_to_edge(v,e)) return NULL_CONSTRAINT; // the copy is not possible
	
	int c_id = get_id();
	int new_id;
	
	if (ug.find_constraint(c_id)) 	new_id = AUTO_ID;
	else 				new_id = c_id;

	return ug.new_constraint_edge_attachment_wrt_previous_corner (v,e,value,new_id);
	}


	void 
	struct_constraint_edge_attachment_wrt_previous_corner::
print 
	(ostream& os) const
	{
	os << "\n-EDGE ATTACHMENT WRT PREVIOUS CORNER constraint" << flush;
	os << "\n constraint number 	      : " << get_id() << flush;
	os << "\n distance of gdtedge attachment : " << get_attachment() << flush;
	os << "\n reference gdtedge              : " << get_owner_graph().id(get_ref_edge()) << flush;
	os << "\n reference gdtnode              : " << get_owner_graph().id(get_ref_node()) << flush;
	}


//-----------------------------------------------------------------------------
//  struct_constraint_min_tieing
// 
// (Aug, 1999)
//-----------------------------------------------------------------------------

	constraint 
	struct_constraint_min_tieing::
copy
	(undi_graph& ug)
	{
	int c_id = get_id();
	int new_id;
	
	if (ug.find_constraint(c_id)) 	new_id = AUTO_ID;
	else 				new_id = c_id;

	return ug.new_constraint_min_tieing( get_value() );
	}


	void 
	struct_constraint_min_tieing::
print 
	(ostream& os) const
	{
	os << "\n-MIN TIEING constraint" << flush;
	os << "\n constraint number 	      : " << get_id() << flush;
	os << "\n minimum tieing value        : " << get_value() << flush;
	}
