/*******************************************************************************
+
+  rm3_constraints.h
+
+  This file is part of GDToolkit. It can be 
+  used free of charge in academic research and teaching. 
+  Any direct or indirect commercial use of this software is illegal 
+  without a written authorization of
+  the Dipartimento di Informatica e Automazione
+  Universita' di Roma Tre, Roma, ITALIA
+  
*******************************************************************************/
/*! \file */


#ifndef RM3_CONSTRAINTS_H
#define RM3_CONSTRAINTS_H

#if !defined(ROOT_INCL_ID)
#define ROOT_INCL_ID 34014
#endif

#include <fstream>
#include <GDT/gdtlist.h>
#include <GDT/gdt_graph_array.h>
#include <GDT/gdtgeometry.h>
#include <GDT/rm3_global.h>

	typedef enum
	{
	UNCROSSABLE_EDGES,
	NUMBER_OF_BENDS_ON_EDGE,
	BEND_DIRECTION_ON_EDGE,
	SAME_FACE_NODES,
	SAME_FACE_ORDERED_NODES,
	NODE_WIDTH,
	NODE_HEIGHT,
	ANGLE_SWEEP,
	EDGE_ATTACHMENT_WRT_PREVIOUS_CORNER,
	MIN_TIEING
	}
constraint_type;

class undi_graph;  					// forward declaration
class orth_plan_undi_graph;				// forward declaration
class upwa_plan_undi_graph;				// forward declaration
class struct_constraint;				// forward declaration
class struct_constraint_uncrossable_edge;		// forward declaration
class struct_constraint_number_of_bends_on_edge;	// forward declaration
class struct_constraint_bend_direction_on_edge;		// forward declaration
class struct_constraint_same_face_node;			// forward declaration
class struct_constraint_same_face_ordered_nodes; 	// forward declaration
class struct_constraint_node_width; 	        	// forward declaration
class struct_constraint_node_height; 	        	// forward declaration
class struct_constraint_angle_sweep;			// forward declaration
class struct_constraint_edge_attachment_wrt_previous_corner;	// forward declaration
class struct_constraint_min_tieing;	                // forward declaration

	typedef struct_constraint* 
constraint;

	typedef struct_constraint_uncrossable_edge* 
constraint_uncrossable_edge;

	typedef struct_constraint_number_of_bends_on_edge*
constraint_number_of_bends_on_edge;

	typedef struct_constraint_bend_direction_on_edge*
constraint_bend_direction_on_edge;

	typedef struct_constraint_same_face_node*
constraint_same_face_node;

	typedef struct_constraint_same_face_ordered_nodes*
constraint_same_face_ordered_nodes;

	typedef struct_constraint_node_width* 
constraint_node_width;

	typedef struct_constraint_node_height* 
constraint_node_height;

	typedef struct_constraint_angle_sweep* 
constraint_angle_sweep;

	typedef struct_constraint_edge_attachment_wrt_previous_corner* 
constraint_edge_attachment_wrt_previous_corner;

        typedef struct_constraint_min_tieing*
constraint_min_tieing;


//-----------------------------------------------------------------------------
// struct_constraint: abstract class for all constraints of undi_graph class
//
// (1998)
//-----------------------------------------------------------------------------


/* WARNING: 
	THE FOLLOWING COMMENTS COULD BE USEFUL IF ANY RE-DESIGN WILL BE UNDERTAKEN IN THE FUTURE.
	
	We recognize a very bad programming practice here.
	struct_constraint and its descendants directely access undi_graph internal structure.
	See for example the destructors and undi_graph::local_del
	This is an unnecessary coupling, since deletion of constraints is only performed by undi_graph.
	Pizzo & PJ
*/

	class GDT_NONSTANDARD_DECL
struct_constraint      // ABSTRACT CLASS
	{
	friend class undi_graph;		
	
	private:

		// -----------------
		// private variables
		// -----------------

		
		constraint_type type;				// type of constraint
		undi_graph*	owner_graph;			// graph whome the constraint belongs to
		gdt::list_item       pos_in_owner_graph_list;	// position in owner graph constraint list

		// ---------------
		// private methods
		// ---------------

		void set_id         		(const int);			// assign constraint-identifier with int
		void set_type       		(const constraint_type);	// assign type of constraint with constraint_type
		void set_owner_graph		(undi_graph&);			// assign the graph whome the constraint belongs to
		void set_pos_in_owner_graph_list(const gdt::list_item);		// assign the graph whome the constraint belongs to
			
		
	protected: 
		
		int 		id;				// constraint-identifier
		
		// ----------------------
		// protected constructors
		// ----------------------
	
		struct_constraint (constraint_type, undi_graph&);	// create new struct_constraint, and put it in owner_graph constraint list
		
		void remove_from_owner_graph_list();			// remove constraint from the owner graph constraint list
		void remove_from_edge_list	 (gdtedge);		// remove constraint from the gdtedge constraint list
		void remove_from_node_list	 (gdtnode);		// remove constraint from the gdtnode constraint list
		
		gdt::list_item add_to_edge_list	 (gdtedge);		// add constraint to the gdtedge constraint list
		gdt::list_item add_to_node_list	 (gdtnode);		// add constraint to the gdtnode constraint list
		
		// ------------------------------------------------------------
		// Updating operations (intelligent handling of constraints)
		// 
		// all methods below must be reimplemented for each constraint
		// ------------------------------------------------------------
		
	virtual bool update_after_del_edge (gdtedge e) = 0;			// update constraint after gdtedge e is deleted and return true if the constraint can be deleted after updating
	virtual bool update_after_del_node (gdtnode e) = 0;			// update constraint after gdtnode v is deleted and return true if the constraint can be deleted after updating
	
	virtual bool update_after_split_edge (gdtedge e, gdtedge e1, gdtedge e2) = 0;	// update constraint after gdtedge e is splitted into two new edges e1, e2 and return true if the constraint can be deleted after updating
	virtual bool update_after_split_node (gdtnode v, gdtnode v1, gdtnode v2) = 0;	// update constraint after gdtnode v is splitted into two new nodes v1, ve and return true if the constraint can be deleted after updating
		
	virtual bool update_after_merge_edges (gdtedge e1, gdtedge e2, gdtedge e) = 0;	// update constraint after edges e1, e2 are merged into an gdtedge e, and return true if the constraint can be deleted after updating
	virtual bool update_after_merge_nodes (gdtnode v1, gdtnode v2, gdtnode v) = 0;	// update constraint after nodes v1, v2 are merged into a gdtnode v, and return true if the constraint can be deleted after updating
	
	virtual void update_after_edge_translation 
			(
			gdtedge e  , 
			gdtnode ve1, 
			gdtnode ve2, 
			gdtedge d  ,
			gdtnode vd1,
			gdtnode vd2
			) = 0;							// see below..
			
			// --------------------------------------------------
			// translate the constraint from gdtedge e to gdtedge d.
			// vd1 and vd2 are the extremal nodes of d that 
			// replace the extremal nodes ve1 and ve2 of e, 
			// respectively.
			// --------------------------------------------------
			

		
	virtual	void update_on_path_replacing_edge
			(
			gdtedge e,
			gdt::gdtlist<gdtedge> path,
			undi_graph& ug
			) = 0;							// see below..
		
		// ---------------------------------------------------------------------------------------
		// generate new constraints for path, which replaces gdtedge e. based on this constraint.
		// If gdtedge e is not present in constraint, nothing to do.
		// NOTICE: - path is a sequence of edges of ug;
		//	   - e is an gdtedge of owner_graph of c (not necessarily ug) 
		//	   - the extremal nodes of e must have the same id than the extremal nodes of path
		// ---------------------------------------------------------------------------------------
	
		
	public: 

		// --------------------------
		// Constructors and Operators
		// --------------------------
		
		
		virtual  	~struct_constraint();					// destructor
	
		// -----------------
		// Access operations
		// -----------------
		
		int 		get_id  () const;			// return constraint-identifier
		constraint_type get_type() const;			// return type of constraint
		undi_graph&	get_owner_graph() const; 		// return type of constraint
		gdt::list_item 	get_pos_in_owner_graph_list() const; 	// return type of constraint
		
 		// --------------------
		// Translate operations
		// --------------------

	 	virtual 	constraint copy(undi_graph&) = 0;	// copy current constraint from owner_graph to undi_graph if it is possible
									// and return pointer to new constraint (NULL_CONSTRAINT if constraint is not copied)

		// -------------------------
		// Input / output operations
		// -------------------------

		virtual   void print (std::ostream& os=std::cout) const = 0;
	};



//-----------------------------------------------------------------------------
// UNCROSSABLE_EDGES constraint: planarization constraint specifying that the
//                               gdtedge can not be crossed
//
// (1998)
//-----------------------------------------------------------------------------

	class GDT_NONSTANDARD_DECL
struct_constraint_uncrossable_edge: public struct_constraint
	{

	friend class undi_graph;
		
	private:

		// -----------------
		// private variables
		// -----------------

		gdtedge uncrossable_edge;

		// ---------------
		// private methods
		// ---------------

		void set_uncrossable_edge(gdtedge);
	
	
		// --------------------
		// private constructors
		// --------------------

		struct_constraint_uncrossable_edge(gdtedge, undi_graph&);


	protected:
	
		// ---------------------------------------------------------
		// Updating operations (intelligent handling of constraints)
		// ---------------------------------------------------------
		
		bool update_after_del_edge (gdtedge e);				// update constraint after gdtedge e is deleted and return true if the constraint can be deleted after updating
		bool update_after_del_node (gdtnode v);				// update constraint after gdtnode v is deleted and return true if the constraint can be deleted after updating		
		
		bool update_after_split_edge (gdtedge e, gdtedge e1, gdtedge e2);	// update constraint after gdtedge e is splitted into two new edges e1, e2 and return true if the constraint can be deleted after updating
		bool update_after_split_node (gdtnode v, gdtnode v1, gdtnode v2);	// update constraint after gdtnode v is splitted into two new nodes v1, ve and return true if the constraint can be deleted after updating
		
		bool update_after_merge_edges (gdtedge e1, gdtedge e2, gdtedge e);	// update constraint after edges e1, e2 are merged into an gdtedge e, and return true if the constraint can be deleted after updating
		bool update_after_merge_nodes (gdtnode v1, gdtnode v2, gdtnode v);	// update constraint after nodes v1, v2 are merged into a gdtnode v, and return true if the constraint can be deleted after updating

		void update_after_edge_translation 
			(
			gdtedge e  , 
			gdtnode ve1, 
			gdtnode ve2, 
			gdtedge d  ,
			gdtnode vd1,
			gdtnode vd2
			);							// see below..
			
			// --------------------------------------------------
			// translate the constraint from gdtedge e to gdtedge d.
			// vd1 and vd2 are the extremal nodes of d that 
			// replace the extremal nodes ve1 and ve2 of e, 
			// respectively.
			// --------------------------------------------------
			
		
		void update_on_path_replacing_edge
			(
			gdtedge e,
			gdt::gdtlist<gdtedge> path,
			undi_graph& ug
			);							// see below..
		
		// ---------------------------------------------------------------------------------------
		// generate new constraints for path, which replaces gdtedge e, based on this constraint.
		// If gdtedge e is not present in constraint, nothing to do.
		// NOTICE: - path is a sequence of edges of ug;
		//	   - e is an gdtedge of owner_graph of c (not necessarily ug) 
		//	   - the extremal nodes of e must have the same id than the extremal nodes of path
		// ---------------------------------------------------------------------------------------
	
	


	public: 

		// --------------------------
		// Constructors and Operators
		// --------------------------

		 ~struct_constraint_uncrossable_edge();

		// -----------------
		// Access operations
		// -----------------

	         gdtedge get_uncrossable_edge() const;

 		// --------------------
		// Translate operations
		// --------------------
		
		 constraint copy(undi_graph&);	// copy current constraint from owner_graph to undi_graph, if it is possible,
						// and the new constraint (NULL_CONSTRAINT if constraint is not copied)

		// -------------------------
		// Input / output operations
		// -------------------------

		 void print (std::ostream& os=std::cout) const;

	};


//-----------------------------------------------------------------------------
// NUMBER_OF_BENDS_ON_EDGE constraints: constraint on the number of bends on an
//                           		an gdtedge of an orthogonal drawing or an
//					upward drawing.
//					Possible constraints are
//					NONE    = no bends on gdtedge (straight gdtedge);
//					ANY     = any number of bends;
//
// (1998)
//-----------------------------------------------------------------------------


// ----------------------------------------------------------------------
// enumerate types for gdtedge-bend constraints in planar drawing algorithms
// ----------------------------------------------------------------------

	typedef enum
	{
	NONE,			// no bend on gdtedge
	ANY,			// any number of bends on gdtedge
	MINIMAL			// a minimal number of bends on the total of edges (default in flow algorithms)
				// NOTICE: this value can not be used for setting new constraints
	}
bend_constraint;


	class GDT_NONSTANDARD_DECL
struct_constraint_number_of_bends_on_edge: public struct_constraint
	{

	friend class undi_graph;
	friend class orth_plan_undi_graph;
	friend class upwa_plan_undi_graph;
		
	private:

		// -----------------
		// private variables
		// -----------------

		gdtedge 		edge_with_nbc;		// gdtedge with bend_constraint
		bend_constraint	nbc;			// number_of_bends constraint

		// ---------------
		// private methods
		// ---------------

		void set_edge_with_nbc  (gdtedge);			
		void set_nbc		(bend_constraint);	
		

		// --------------------
		// private constructors
		// --------------------

		struct_constraint_number_of_bends_on_edge (gdtedge, bend_constraint, undi_graph&);
		
		
	protected:

		// ---------------------------------------------------------
		// Updating operations (intelligent handling of constraints)
		// ---------------------------------------------------------
		
		bool update_after_del_edge (gdtedge e);				// update constraint after gdtedge e is deleted and return true if the constraint can be deleted after updating
		bool update_after_del_node (gdtnode v);				// update constraint after gdtnode v is deleted and return true if the constraint can be deleted after updating		
		
		bool update_after_split_edge (gdtedge e, gdtedge e1, gdtedge e2);	// update constraint after gdtedge e is splitted into two new edges e1, e2 and return true if the constraint can be deleted after updating
		bool update_after_split_node (gdtnode v, gdtnode v1, gdtnode v2);	// update constraint after gdtnode v is splitted into two new nodes v1, ve and return true if the constraint can be deleted after updating
		
		bool update_after_merge_edges (gdtedge e1, gdtedge e2, gdtedge e);	// update constraint after edges e1, e2 are merged into an gdtedge e, and return true if the constraint can be deleted after updating
		bool update_after_merge_nodes (gdtnode v1, gdtnode v2, gdtnode v);	// update constraint after nodes v1, v2 are merged into a gdtnode v, and return true if the constraint can be deleted after updating

		void update_after_edge_translation 
			(
			gdtedge e  , 
			gdtnode ve1, 
			gdtnode ve2, 
			gdtedge d  ,
			gdtnode vd1,
			gdtnode vd2
			);							// see below..
			
			// --------------------------------------------------
			// translate the constraint from gdtedge e to gdtedge d.
			// vd1 and vd2 are the extremal nodes of d that 
			// replace the extremal nodes ve1 and ve2 of e, 
			// respectively.
			// --------------------------------------------------
		
		void update_on_path_replacing_edge
			(
			gdtedge e,
			gdt::gdtlist<gdtedge> path,
			undi_graph& ug
			);							// see below..
		
		// ---------------------------------------------------------------------------------------
		// generate new constraints for path, which replaces gdtedge e, based on this constraint.
		// If gdtedge e is not present in constraint, nothing to do.
		// NOTICE: - path is a sequence of edges of ug;
		//	   - e is an gdtedge of owner_graph of c (not necessarily ug) 
		//	   - the extremal nodes of e must have the same id than the extremal nodes of path
		// ---------------------------------------------------------------------------------------
	

	public: 

		// --------------------------
		// Constructors and Operators
		// --------------------------

		 ~struct_constraint_number_of_bends_on_edge();

		// -----------------
		// Access operations
		// -----------------

		 gdtedge 		get_edge_with_nbc () const;	
		 bend_constraint	get_nbc		  () const;

 		// --------------------
		// Translate operations
		// --------------------
		
		 constraint copy(undi_graph&);	// copy current constraint from owner_graph to undi_graph, if it is possible,
						// and return the new constraint (NULL_CONSTRAINT if constraint is not copied)
		
		
		// -------------------------
		// Input / output operations
		// -------------------------

		 void print (std::ostream& os=std::cout) const;

	};

//--------------------------------------------------------------------------------
// BEND_DIRECTION_ON_EDGE constraints:  constraint on the bend direction on
//                           		an gdtedge of an orthogonal drawing or an
//					upward drawing.
//					Constraint is specified by two parameters:
//					gdtedge e, gdtnode v, and it indicates that gdtedge e
// 					can be spiralize only from the left face
//					to the right face, moving along gdtedge e
//					starting from gdtnode v.
//					Obiouvsly, v must be an extremal gdtnode of e
// (1998)
//--------------------------------------------------------------------------------


	class GDT_NONSTANDARD_DECL
struct_constraint_bend_direction_on_edge: public struct_constraint
	{
	friend class undi_graph;
	friend class orth_plan_undi_graph;
	friend class upwa_plan_undi_graph;
		
	private:

		// -----------------
		// private variables
		// -----------------

		gdtedge edge_with_bdc;		// gdtedge with bend_direction constraint
		gdtnode start_node;		// start gdtnode for evaluating direction

		// ---------------
		// private methods
		// ---------------

		void set_edge_with_bdc  (gdtedge);			
		void set_start_node	(gdtnode);	

		// --------------------
		// private constructors
		// --------------------

		struct_constraint_bend_direction_on_edge (gdtedge, gdtnode, undi_graph&);
		
	protected:
	
		// ---------------------------------------------------------
		// Updating operations (intelligent handling of constraints)
		// ---------------------------------------------------------
		
		bool update_after_del_edge (gdtedge e);				// update constraint after gdtedge e is deleted and return true if the constraint can be deleted after updating
		bool update_after_del_node (gdtnode v);				// update constraint after gdtnode v is deleted and return true if the constraint can be deleted after updating		
		
		bool update_after_split_edge (gdtedge e, gdtedge e1, gdtedge e2);	// update constraint after gdtedge e is splitted into two new edges e1, e2 and return true if the constraint can be deleted after updating
		bool update_after_split_node (gdtnode v, gdtnode v1, gdtnode v2);	// update constraint after gdtnode v is splitted into two new nodes v1, ve and return true if the constraint can be deleted after updating
		
		bool update_after_merge_edges (gdtedge e1, gdtedge e2, gdtedge e);	// update constraint after edges e1, e2 are merged into an gdtedge e, and return true if the constraint can be deleted after updating
		bool update_after_merge_nodes (gdtnode v1, gdtnode v2, gdtnode v);	// update constraint after nodes v1, v2 are merged into a gdtnode v, and return true if the constraint can be deleted after updating

		void update_after_edge_translation 
			(
			gdtedge e  , 
			gdtnode ve1, 
			gdtnode ve2, 
			gdtedge d  ,
			gdtnode vd1,
			gdtnode vd2
			);							// see below..
			
			// --------------------------------------------------
			// translate the constraint from gdtedge e to gdtedge d.
			// vd1 and vd2 are the extremal nodes of d that 
			// replace the extremal nodes ve1 and ve2 of e, 
			// respectively.
			// --------------------------------------------------

		void update_on_path_replacing_edge
			(
			gdtedge e,
			gdt::gdtlist<gdtedge> path,
			undi_graph& ug
			);							// see below..
		
		// ---------------------------------------------------------------------------------------
		// generate new constraints for path, which replaces gdtedge e, based on this constraint.
		// If gdtedge e is not present in constraint, nothing to do.
		// NOTICE: - path is a sequence of edges of ug;
		//	   - e is an gdtedge of owner_graph of c (not necessarily ug) 
		//	   - the extremal nodes of e must have the same id than the extremal nodes of path
		// ---------------------------------------------------------------------------------------
		

	public: 

		// --------------------------
		// Constructors and Operators
		// --------------------------

		 ~struct_constraint_bend_direction_on_edge();

		// -----------------
		// Access operations
		// -----------------

		 gdtedge 	get_edge_with_bdc () const;	
		 gdtnode	get_start_node    () const;

 		// --------------------
		// Translate operations
		// --------------------
		
		 constraint copy(undi_graph&);	// copy current constraint from owner_graph to undi_graph, if it is possible,
						// and return the new constraint (NULL_CONSTRAINT if constraint is not copied)

		// -------------------------
		// Input / output operations
		// -------------------------

		 void print (std::ostream& os=std::cout) const;

	};


//-----------------------------------------------------------------------------
// SAME_FACE_NODES constraint: planarization constraint specifying that the
//                             gdtnode must belong to the same face of other nodes
//                             that have the same face-label
//
// (1998)
//-----------------------------------------------------------------------------

	class GDT_NONSTANDARD_DECL
struct_constraint_same_face_node: public struct_constraint
	{

	friend class undi_graph;

	private:

		// -----------------
		// private variables
		// -----------------

		gdtnode same_face_node;
		int  face_label;

		// ---------------
		// private methods
		// ---------------

		void set_same_face_node(gdtnode);
		void set_face_label(int);
	
	
		// --------------------
		// private constructors
		// --------------------

		struct_constraint_same_face_node(gdtnode, undi_graph&, int);
		
	protected:
	
		// ---------------------------------------------------------
		// Updating operations (intelligent handling of constraints)
		// ---------------------------------------------------------
		
		bool update_after_del_edge (gdtedge e);				// update constraint after gdtedge e is deleted and return true if the constraint can be deleted after updating
		bool update_after_del_node (gdtnode v);				// update constraint after gdtnode v is deleted and return true if the constraint can be deleted after updating		
		
		bool update_after_split_edge (gdtedge e, gdtedge e1, gdtedge e2);	// update constraint after gdtedge e is splitted into two new edges e1, e2 and return true if the constraint can be deleted after updating
		bool update_after_split_node (gdtnode v, gdtnode v1, gdtnode v2);	// update constraint after gdtnode v is splitted into two new nodes v1, ve and return true if the constraint can be deleted after updating
		
		bool update_after_merge_edges (gdtedge e1, gdtedge e2, gdtedge e);	// update constraint after edges e1, e2 are merged into an gdtedge e, and return true if the constraint can be deleted after updating
		bool update_after_merge_nodes (gdtnode v1, gdtnode v2, gdtnode v);	// update constraint after nodes v1, v2 are merged into a gdtnode v, and return true if the constraint can be deleted after updating

		void update_after_edge_translation 
			(
			gdtedge e  , 
			gdtnode ve1, 
			gdtnode ve2, 
			gdtedge d  ,
			gdtnode vd1,
			gdtnode vd2
			);							// see below..
			
			// --------------------------------------------------
			// translate the constraint from gdtedge e to gdtedge d.
			// vd1 and vd2 are the extremal nodes of d that 
			// replace the extremal nodes ve1 and ve2 of e, 
			// respectively.
			// --------------------------------------------------

		
		void update_on_path_replacing_edge
			(
			gdtedge e,
			gdt::gdtlist<gdtedge> path,
			undi_graph& ug
			);							// see below..
		
		// ---------------------------------------------------------------------------------------
		// generate new constraints for path, which replaces gdtedge e, based on this constraint.
		// If gdtedge e is not present in constraint, nothing to do.
		// NOTICE: - path is a sequence of edges of ug;
		//	   - e is an gdtedge of owner_graph of c (not necessarily ug) 
		//	   - the extremal nodes of e must have the same id than the extremal nodes of path
		// ---------------------------------------------------------------------------------------
	
	


	public: 

		// --------------------------
		// Constructors and Operators
		// --------------------------

		 struct_constraint_same_face_node();
                 ~struct_constraint_same_face_node();

		// -----------------
		// Access operations
		// -----------------

	 	gdtnode get_same_face_node() const;
	 	int  get_face_label() const;

 		// --------------------
		// Translate operations
		// --------------------
		
		 constraint copy(undi_graph&);	// copy current constraint from owner_graph to undi_graph, if it is possible,
						// and the new constraint (NULL_CONSTRAINT if constraint is not copied)

		// -------------------------
		// Input / output operations
		// -------------------------

		 void print (std::ostream& os=std::cout) const;

	};


//-----------------------------------------------------------------------------
// SAME_FACE_ORDERED_NODES constraint: planarization constraint specifying that
//               the given nodes must belong to the same face in the order they
//               are specified
//-----------------------------------------------------------------------------

	class GDT_NONSTANDARD_DECL
struct_constraint_same_face_ordered_nodes: public struct_constraint
	{

	friend class undi_graph;

	private:

		// -----------------
		// private variables
		// -----------------

		gdt::gdtlist<gdtnode> node_list;

		// ---------------
		// private methods
		// ---------------

		void set_node_list(gdt::gdtlist<gdtnode>&);
	
		// --------------------
		// private constructors
		// --------------------

		struct_constraint_same_face_ordered_nodes(gdt::gdtlist<gdtnode>, undi_graph&);

	protected:
	
		// ---------------------------------------------------------
		// Updating operations (intelligent handling of constraints)
		// ---------------------------------------------------------
		
		bool update_after_del_edge (gdtedge e);				// update constraint after gdtedge e is deleted and return true if the constraint can be deleted after updating
		bool update_after_del_node (gdtnode v);				// update constraint after gdtnode v is deleted and return true if the constraint can be deleted after updating		
		
		bool update_after_split_edge (gdtedge e, gdtedge e1, gdtedge e2);	// update constraint after gdtedge e is splitted into two new edges e1, e2 and return true if the constraint can be deleted after updating
		bool update_after_split_node (gdtnode v, gdtnode v1, gdtnode v2);	// update constraint after gdtnode v is splitted into two new nodes v1, ve and return true if the constraint can be deleted after updating
		
		bool update_after_merge_edges (gdtedge e1, gdtedge e2, gdtedge e);	// update constraint after edges e1, e2 are merged into an gdtedge e, and return true if the constraint can be deleted after updating
		bool update_after_merge_nodes (gdtnode v1, gdtnode v2, gdtnode v);	// update constraint after nodes v1, v2 are merged into a gdtnode v, and return true if the constraint can be deleted after updating

		void update_after_edge_translation 
			(
			gdtedge e  , 
			gdtnode ve1, 
			gdtnode ve2, 
			gdtedge d  ,
			gdtnode vd1,
			gdtnode vd2
			);							// see below..
			
			// --------------------------------------------------
			// translate the constraint from gdtedge e to gdtedge d.
			// vd1 and vd2 are the extremal nodes of d that 
			// replace the extremal nodes ve1 and ve2 of e, 
			// respectively.
			// --------------------------------------------------
			
		
		void update_on_path_replacing_edge
			(
			gdtedge e,
			gdt::gdtlist<gdtedge> path,
			undi_graph& ug
			);							// see below..
		
		// ---------------------------------------------------------------------------------------
		// generate new constraints for path, which replaces gdtedge e. based on this constraint.
		// If gdtedge e is not present in constraint, nothing to do.
		// NOTICE: - path is a sequence of edges of ug;
		//	   - e is an gdtedge of owner_graph of c (not necessarily ug) 
		//	   - the extremal nodes of e must have the same id than the extremal nodes of path
		// ---------------------------------------------------------------------------------------
	
	

	public: 

		// --------------------------
		// Constructors and Operators
		// --------------------------

		 ~struct_constraint_same_face_ordered_nodes();

		// -----------------
		// Access operations
		// -----------------

	 	gdt::gdtlist<gdtnode> get_node_list() const;

 		// --------------------
		// Translate operations
		// --------------------
		
		 constraint copy(undi_graph&);	// copy current constraint from owner_graph to undi_graph, if it is possible,
						// and the new constraint (NULL_CONSTRAINT if constraint is not copied)
		

		// -------------------------
		// Input / output operations
		// -------------------------

		 void print (std::ostream& os=std::cout) const;
	
	};


//-----------------------------------------------------------------------------
// NODE_WIDTH constraint: fixes the width of a gdtnode in the final drawing
//-----------------------------------------------------------------------------

	class GDT_NONSTANDARD_DECL
struct_constraint_node_width: public struct_constraint
	{

	friend class undi_graph;
		
	private:

		// -----------------
		// private variables
		// -----------------

		gdtnode 	constrained_node;
                double	value;

		// ---------------
		// private methods
		// ---------------

		void set_constrained_node(gdtnode);
                void set_value(double);
	
		// --------------------
		// private constructors
		// --------------------

                struct_constraint_node_width(gdtnode, double, undi_graph&);   


	protected:
	
		// ---------------------------------------------------------
		// Updating operations (intelligent handling of constraints)
		// ---------------------------------------------------------
		
		bool update_after_del_edge (gdtedge e);				// update constraint after gdtedge e is deleted and return true if the constraint can be deleted after updating
		bool update_after_del_node (gdtnode v);				// update constraint after gdtnode v is deleted and return true if the constraint can be deleted after updating		
		
		bool update_after_split_edge (gdtedge e, gdtedge e1, gdtedge e2);	// update constraint after gdtedge e is splitted into two new edges e1, e2 and return true if the constraint can be deleted after updating
		bool update_after_split_node (gdtnode v, gdtnode v1, gdtnode v2);	// update constraint after gdtnode v is splitted into two new nodes v1, ve and return true if the constraint can be deleted after updating
		
		bool update_after_merge_edges (gdtedge e1, gdtedge e2, gdtedge e);	// update constraint after edges e1, e2 are merged into an gdtedge e, and return true if the constraint can be deleted after updating
		bool update_after_merge_nodes (gdtnode v1, gdtnode v2, gdtnode v);	// update constraint after nodes v1, v2 are merged into a gdtnode v, and return true if the constraint can be deleted after updating

		void update_after_edge_translation 
			(
			gdtedge e  , 
			gdtnode ve1, 
			gdtnode ve2, 
			gdtedge d  ,
			gdtnode vd1,
			gdtnode vd2
			);							// see below..
			
			// --------------------------------------------------
			// translate the constraint from gdtedge e to gdtedge d.
			// vd1 and vd2 are the extremal nodes of d that 
			// replace the extremal nodes ve1 and ve2 of e, 
			// respectively.
			// --------------------------------------------------
			
		
		void update_on_path_replacing_edge
			(
			gdtedge e,
			gdt::gdtlist<gdtedge> path,
			undi_graph& ug
			);							// see below..
		
		// ---------------------------------------------------------------------------------------
		// generate new constraints for path, which replaces gdtedge e, based on this constraint.
		// If gdtedge e is not present in constraint, nothing to do.
		// NOTICE: - path is a sequence of edges of ug;
		//	   - e is an gdtedge of owner_graph of c (not necessarily ug) 
		//	   - the extremal nodes of e must have the same id than the extremal nodes of path
		// ---------------------------------------------------------------------------------------

	public: 

		// --------------------------
		// Constructors and Operators
		// --------------------------

		 ~struct_constraint_node_width();

		// -----------------
		// Access operations
		// -----------------

	         gdtnode 	get_constrained_node() const;
                 double	get_value() const;

 		// --------------------
		// Translate operations
		// --------------------
		
		 constraint copy(undi_graph&);	// copy current constraint from owner_graph to undi_graph, if it is possible,
						// and the new constraint (NULL_CONSTRAINT if constraint is not copied)

		// -------------------------
		// Input / output operations
		// -------------------------

		 void print (std::ostream& os=std::cout) const;
	
	};


//-----------------------------------------------------------------------------
// NODE_HEIGHT constraint: fixes the height of a gdtnode in the final drawing
//-----------------------------------------------------------------------------

	class GDT_NONSTANDARD_DECL
struct_constraint_node_height: public struct_constraint
	{

	friend class undi_graph;
		
	private:

		// -----------------
		// private variables
		// -----------------

		gdtnode 	constrained_node;
                double	value;

		// ---------------
		// private methods
		// ---------------

		void set_constrained_node(gdtnode);
                void set_value(double);
	
		// --------------------
		// private constructors
		// --------------------

                struct_constraint_node_height(gdtnode, double, undi_graph&);   


	protected:
	
		// ---------------------------------------------------------
		// Updating operations (intelligent handling of constraints)
		// ---------------------------------------------------------
		
		bool update_after_del_edge (gdtedge e);				// update constraint after gdtedge e is deleted and return true if the constraint can be deleted after updating
		bool update_after_del_node (gdtnode v);				// update constraint after gdtnode v is deleted and return true if the constraint can be deleted after updating		
		
		bool update_after_split_edge (gdtedge e, gdtedge e1, gdtedge e2);	// update constraint after gdtedge e is splitted into two new edges e1, e2 and return true if the constraint can be deleted after updating
		bool update_after_split_node (gdtnode v, gdtnode v1, gdtnode v2);	// update constraint after gdtnode v is splitted into two new nodes v1, ve and return true if the constraint can be deleted after updating
		
		bool update_after_merge_edges (gdtedge e1, gdtedge e2, gdtedge e);	// update constraint after edges e1, e2 are merged into an gdtedge e, and return true if the constraint can be deleted after updating
		bool update_after_merge_nodes (gdtnode v1, gdtnode v2, gdtnode v);	// update constraint after nodes v1, v2 are merged into a gdtnode v, and return true if the constraint can be deleted after updating

		void update_after_edge_translation 
			(
			gdtedge e  , 
			gdtnode ve1, 
			gdtnode ve2, 
			gdtedge d  ,
			gdtnode vd1,
			gdtnode vd2
			);							// see below..
			
			// --------------------------------------------------
			// translate the constraint from gdtedge e to gdtedge d.
			// vd1 and vd2 are the extremal nodes of d that 
			// replace the extremal nodes ve1 and ve2 of e, 
			// respectively.
			// --------------------------------------------------
			
		
		void update_on_path_replacing_edge
			(
			gdtedge e,
			gdt::gdtlist<gdtedge> path,
			undi_graph& ug
			);							// see below..

		// ---------------------------------------------------------------------------------------
		// generate new constraints for path, which replaces gdtedge e, based on this constraint.
		// If gdtedge e is not present in constraint, nothing to do.
		// NOTICE: - path is a sequence of edges of ug;
		//	   - e is an gdtedge of owner_graph of c (not necessarily ug)
		//	   - the extremal nodes of e must have the same id than the extremal nodes of path
		// ---------------------------------------------------------------------------------------

	public:

		// --------------------------
		// Constructors and Operators
		// --------------------------

		 ~struct_constraint_node_height();

		// -----------------
		// Access operations
		// -----------------

	         gdtnode 	get_constrained_node() const;
                 double	get_value() const;

 		// --------------------
		// Translate operations
		// --------------------

		 constraint copy(undi_graph&);	// copy current constraint from owner_graph to undi_graph, if it is possible,
						// and the new constraint (NULL_CONSTRAINT if constraint is not copied)

		// -------------------------
		// Input / output operations
		// -------------------------

		 void print (std::ostream& os=std::cout) const;

	};


//-----------------------------------------------------------------------------
// ANGLE_SWEEP constraint: fixes an angle at vertex to the right of an gdtedge
//-----------------------------------------------------------------------------

	class GDT_NONSTANDARD_DECL
struct_constraint_angle_sweep: public struct_constraint
	{
	friend class undi_graph;
		
	private:

		// -----------------
		// private variables
		// -----------------

		gdtnode 		ref_node;
		gdtedge 		ref_edge;
                angle_type	angle_value;

		// ---------------
		// private methods
		// ---------------

		void set_ref_node(gdtnode);
		void set_ref_edge(gdtedge);
                void set_angle_value(angle_type);
	
		// --------------------
		// private constructors
		// --------------------

                struct_constraint_angle_sweep (gdtnode, gdtedge, angle_type, undi_graph&);


	protected:
	
		// ---------------------------------------------------------
		// Updating operations (intelligent handling of constraints)
		// ---------------------------------------------------------
		
		bool update_after_del_edge (gdtedge e);				// update constraint after gdtedge e is deleted and return true if the constraint can be deleted after updating
		bool update_after_del_node (gdtnode v);				// update constraint after gdtnode v is deleted and return true if the constraint can be deleted after updating		
		
		bool update_after_split_edge (gdtedge e, gdtedge e1, gdtedge e2);	// update constraint after gdtedge e is splitted into two new edges e1, e2 and return true if the constraint can be deleted after updating
		bool update_after_split_node (gdtnode v, gdtnode v1, gdtnode v2);	// update constraint after gdtnode v is splitted into two new nodes v1, ve and return true if the constraint can be deleted after updating
		
		bool update_after_merge_edges (gdtedge e1, gdtedge e2, gdtedge e);	// update constraint after edges e1, e2 are merged into an gdtedge e, and return true if the constraint can be deleted after updating
		bool update_after_merge_nodes (gdtnode v1, gdtnode v2, gdtnode v);	// update constraint after nodes v1, v2 are merged into a gdtnode v, and return true if the constraint can be deleted after updating

		void update_after_edge_translation 
			(
			gdtedge e  , 
			gdtnode ve1, 
			gdtnode ve2, 
			gdtedge d  ,
			gdtnode vd1,
			gdtnode vd2
			);							// see below..
			
			// --------------------------------------------------
			// translate the constraint from gdtedge e to gdtedge d.
			// vd1 and vd2 are the extremal nodes of d that 
			// replace the extremal nodes ve1 and ve2 of e, 
			// respectively.
			// --------------------------------------------------
			
		
		void update_on_path_replacing_edge
			(
			gdtedge e,
			gdt::gdtlist<gdtedge> path,
			undi_graph& ug
			);							// see below..
		
		// ---------------------------------------------------------------------------------------
		// generate new constraints for path, which replaces gdtedge e, based on this constraint.
		// If gdtedge e is not present in constraint, nothing to do.
		// NOTICE: - path is a sequence of edges of ug;
		//	   - e is an gdtedge of owner_graph of c (not necessarily ug) 
		//	   - the extremal nodes of e must have the same id than the extremal nodes of path
		// ---------------------------------------------------------------------------------------

	public: 

		// --------------------------
		// Constructors and Operators
		// --------------------------

		 ~struct_constraint_angle_sweep();

		// -----------------
		// Access operations
		// -----------------

	         gdtnode 		get_ref_node()    const;
		 gdtedge   	get_ref_edge()    const;
                 angle_type	get_angle_value() const;

 		// --------------------
		// Translate operations
		// --------------------
		
		 constraint copy(undi_graph&);	// copy current constraint from owner_graph to undi_graph, if it is possible,
						// and the new constraint (NULL_CONSTRAINT if constraint is not copied)

		// -------------------------
		// Input / output operations
		// -------------------------

		 void print (std::ostream& os=std::cout) const;

	};


//-----------------------------------------------------------------------------
// EDGE_ATTACHMENT_WRT_PREVIOUS_CORNER constraint: fixes the distance between an
// gdtedge attachment and the previous (clockwise) corner of a vertex
//-----------------------------------------------------------------------------

	class GDT_NONSTANDARD_DECL
struct_constraint_edge_attachment_wrt_previous_corner: public struct_constraint
	{
	friend class undi_graph;
		
	private:

		// -----------------
		// private variables
		// -----------------

		gdtnode 		ref_node;
		gdtedge 		ref_edge;
                int 	        attachment;

		// ---------------
		// private methods
		// ---------------

		void set_ref_node(gdtnode);
		void set_ref_edge(gdtedge);
                void set_attachment(int);
	
		// --------------------
		// private constructors
		// --------------------

                struct_constraint_edge_attachment_wrt_previous_corner 
			(gdtnode, gdtedge, int, undi_graph&);   


	protected:
	
		// ---------------------------------------------------------
		// Updating operations (intelligent handling of constraints)
		// ---------------------------------------------------------
		
		bool update_after_del_edge (gdtedge e);				// update constraint after gdtedge e is deleted and return true if the constraint can be deleted after updating
		bool update_after_del_node (gdtnode v);				// update constraint after gdtnode v is deleted and return true if the constraint can be deleted after updating		
		
		bool update_after_split_edge (gdtedge e, gdtedge e1, gdtedge e2);	// update constraint after gdtedge e is splitted into two new edges e1, e2 and return true if the constraint can be deleted after updating
		bool update_after_split_node (gdtnode v, gdtnode v1, gdtnode v2);	// update constraint after gdtnode v is splitted into two new nodes v1, ve and return true if the constraint can be deleted after updating
		
		bool update_after_merge_edges (gdtedge e1, gdtedge e2, gdtedge e);	// update constraint after edges e1, e2 are merged into an gdtedge e, and return true if the constraint can be deleted after updating
		bool update_after_merge_nodes (gdtnode v1, gdtnode v2, gdtnode v);	// update constraint after nodes v1, v2 are merged into a gdtnode v, and return true if the constraint can be deleted after updating

		void update_after_edge_translation 
			(
			gdtedge e  , 
			gdtnode ve1, 
			gdtnode ve2, 
			gdtedge d  ,
			gdtnode vd1,
			gdtnode vd2
			);							// see below..
			
			// --------------------------------------------------
			// translate the constraint from gdtedge e to gdtedge d.
			// vd1 and vd2 are the extremal nodes of d that 
			// replace the extremal nodes ve1 and ve2 of e, 
			// respectively.
			// --------------------------------------------------
			

		void update_on_path_replacing_edge
			(
			gdtedge e,
			gdt::gdtlist<gdtedge> path,
			undi_graph& ug
			);							// see below..
		
		// ---------------------------------------------------------------------------------------
		// generate new constraints for path, which replaces gdtedge e, based on this constraint.
		// If gdtedge e is not present in constraint, nothing to do.
		// NOTICE: - path is a sequence of edges of ug;
		//	   - e is an gdtedge of owner_graph of c (not necessarily ug) 
		//	   - the extremal nodes of e must have the same id than the extremal nodes of path
		// ---------------------------------------------------------------------------------------

	public: 

		// --------------------------
		// Constructors and Operators
		// --------------------------

		 ~struct_constraint_edge_attachment_wrt_previous_corner();

		// -----------------
		// Access operations
		// -----------------

	         gdtnode 		get_ref_node()    const;
		 gdtedge   	get_ref_edge()    const;
                 int 		get_attachment()  const;

 		// --------------------
		// Translate operations
		// --------------------
		
		 constraint copy(undi_graph&);	// copy current constraint from owner_graph to undi_graph, if it is possible,
						// and the new constraint (NULL_CONSTRAINT if constraint is not copied)

		// -------------------------
		// Input / output operations
		// -------------------------

		 void print (std::ostream& os=std::cout) const;
	
	};


//---------------------------------------------------------------------------------------
// MIN_TIEING constraint: it is a global constraint, it is not associated to any element.
// Using this constraint the user can specify the lower boud in the tieing compacion
// This means that any distance (horizontal and vertical) between two nodes and the 
// length of any gdtedge segment in the drawing will be greather than the specified distance.
//---------------------------------------------------------------------------------------

	class GDT_NONSTANDARD_DECL
struct_constraint_min_tieing: public struct_constraint
        {
	friend class undi_graph;

	private:

	const int the_value;

	struct_constraint_min_tieing( int x, undi_graph& ug):
	  struct_constraint(MIN_TIEING, ug),
	  the_value(x) 
	  {}

	private:

	virtual bool update_after_del_edge (gdtedge e) { return false; }
	virtual bool update_after_del_node (gdtnode e) { return false; }
	
	virtual bool update_after_split_edge (gdtedge e, gdtedge e1, gdtedge e2) { return false; }
	virtual bool update_after_split_node (gdtnode v, gdtnode v1, gdtnode v2) { return false; }
		
	virtual bool update_after_merge_edges (gdtedge e1, gdtedge e2, gdtedge e) { return false; }
	virtual bool update_after_merge_nodes (gdtnode v1, gdtnode v2, gdtnode v) { return false; }
	virtual void update_after_edge_translation
			(
			gdtedge e  ,
			gdtnode ve1,
			gdtnode ve2,
			gdtedge d  ,
			gdtnode vd1,
			gdtnode vd2
			) {}
	virtual	void update_on_path_replacing_edge
			(
			gdtedge e,
			gdt::gdtlist<gdtedge> path,
			undi_graph& ug
			) {}


	public:


	virtual constraint copy(undi_graph&);

	virtual void       print (std::ostream& os=std::cout) const;

	int get_value() const
	    { return the_value; }

	};


#endif // RM3_CONSTRAINTS_H
