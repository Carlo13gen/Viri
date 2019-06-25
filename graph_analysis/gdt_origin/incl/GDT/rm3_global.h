/*******************************************************************************
+
+  rm3_global.h
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

/*! \file */

#ifndef RM3_GLOBAL_H
#define RM3_GLOBAL_H

#if !defined(ROOT_INCL_ID)
#define ROOT_INCL_ID 34010
#endif


#include <string>
#include <GDT/gdtarray.h>
#include <GDT/gdtlist.h>
#include <GDT/gdtmap.h>


//-----------------------------------------------------------------------------
// Windows compatibility definition for dynamic library export
//-----------------------------------------------------------------------------

/*
#ifdef _WIN32
#ifdef __DLL__
#define GDT_NONSTANDARD_DECL __export
#define GDT_NONSTANDARD_DECL_FUNCTION __declspec(dllexport)
#else							   
#define GDT_NONSTANDARD_DECL __import			   
#define GDT_NONSTANDARD_DECL_FUNCTION __declspec(dllimport)
#endif
#else
#define GDT_NONSTANDARD_DECL
#define GDT_NONSTANDARD_DECL_FUNCTION 
#endif
*/

#define GDT_NONSTANDARD_DECL
#define GDT_NONSTANDARD_DECL_FUNCTION 



//-----------------------------------------------------------------------------
// gloabl: global definitions for the rm3 files
//
// W.Didimo e A.Leonforte (1996)
//-----------------------------------------------------------------------------

// some compilers (e.g MVC 6.0) defines INFINITE
#ifdef INFINITE
#undef INFINITE
#endif
extern const int INFINITE;
extern const int BOUND_ON_NODES;

#define gdt_minimum(a,b) ((a)>(b) ? (b) : (a))
#define gdt_maximum(a,b) ((a)>(b) ? (a) : (b))

#define safe_delete(a) {delete(a); a = NULL;}

//typedef gdt::gdtlist<int> int_perm;


// -------------------------------------------------
// list of possible algorithms to apply in GDToolkit
// -------------------------------------------------

	typedef enum
	{
	DEFAULT_ALGORITHM,

	PLAN_ORTH_QUICK,
	PLAN_ORTH_OPTIMAL,
	PLAN_ORTH_SLOW,
	PLAN_ORTH_UPWA_STRAIGHT_MIDDLE,

	// Described in:
	//
	//   A. Papakostas and I.G. Tollis,
	//   "Efficient Orthogonal Drawings of High Degree Graphs"
	//   Algorithmica (2000) 26: 100-125.
	//
	// Code is in the file rel_coord_orth.cpp
	// see also the corresponding draw_undi constructor
	//
	ORTH_SIMPLE_PT00,

        // Code in _rm3_draw_undi_graph.cpp
	//
	ORTH_FROM_VISIBILITY,
	ORTH_FROM_VISIBILITY_COMPACTED,

	TREE_DRAW_CENTERSONS,
	TREE_DRAW_CENTERSUBTREE,

	VISIBILITY,
	VISIBILITY_COMPACTED,
	VISIBILITY_REGULAR,
	VISIBILITY_REGULAR_COMPACTED,

	POLYLINE,
	POLYLINE_COMPACTED,
	POLYLINE_REGULAR,
	POLYLINE_REGULAR_COMPACTED,
	
	PLAN_UPWARD_OPTIMAL,
	PLAN_UPWARD_SLOW,
	PLAN_UPWARD_EMBEDDING,
	PLAN_UPWARD_EMBEDDING_REGULAR,
	
	FAST_COMPACTION,
	FAST_COMPACTION_REFINED,
	SLOW_COMPACTION,
	SLOW_COMPACTION_REFINED,	
	FAST_REGULAR_COMPACTION_1,
	SLOW_REGULAR_COMPACTION_1,
	FAST_REGULAR_COMPACTION_2,
	SLOW_REGULAR_COMPACTION_2,
	FAST_REGULAR_COMPACTION_1_REFINED,
	SLOW_REGULAR_COMPACTION_1_REFINED,
	FAST_REGULAR_COMPACTION_2_REFINED,
	SLOW_REGULAR_COMPACTION_2_REFINED
	}
algorithm_type;


// ------------------------------------------
// structure used in Branch and Bound methods
// ------------------------------------------

	typedef struct 
 	{
 	int   		number_of_total_nodes;		// number of total nodes in the Branch and Bound tree
	int   		number_of_cut_nodes;		// number of cut nodes by B&B algorithm
	int   		number_of_visited_nodes;	// number of visited nodes by B&B algorithm
	int   		number_of_cuts;			// number of total cuts executed by B&B algorithm	
	gdt::gdtmap<int,int> 	N;			// N(i) = number of nodes of a B&B subtree rooted to depth i
 	}
BB_tree_parameters;
 

// ----------------------
// Generic used functions
// ----------------------

GDT_NONSTANDARD_DECL_FUNCTION int 		random (int a, int b);		// return a randomly generated integer number between a and b
GDT_NONSTANDARD_DECL_FUNCTION int 		fact   (int n);			// return the factorial of the integer n
//array<int_perm> find_all_permutations (int n);	// compute the ordered set of all permutations
GDT_NONSTANDARD_DECL_FUNCTION gdt::gdtarray< gdt::gdtlist<int> > find_all_permutations (int n);	// compute the ordered set of all permutations
						// over the set {0,1,2,..,n-1}. ([0] <-> {0,1,2,..,n-1})..([(n-1)!]<-> {n-1,n-2,..,0})


// -----------
// Marker type
// -----------

	typedef enum
	{
	RM3_UNDEFINED_MARKER,
	RM3_REPLACES_A_BEND,
	RM3_ADDED_BY_RECTANGULARIZATION,
	RM3_CROSS_ON_REAL_EDGE,
	RM3_DD_CROSS_ON_FACE,
	RM3_DD_CROSS_ON_REAL_EDGE,
	RM3_DD_CROSS_ON_DUMMY_EDGE,
	RM3_DD_WALK_ALONG_DUMMY_EDGE,
	RM3_DD_WALK_ALONG_REAL_EDGE,
	RM3_DD_WALK_AND_TURN_LEFT_ALONG_DUMMY_EDGE,
	RM3_DD_WALK_AND_TURN_RIGHT_ALONG_DUMMY_EDGE,
	RM3_DD_WALK_AND_TURN_LEFT_ALONG_REAL_EDGE,
	RM3_DD_WALK_AND_TURN_RIGHT_ALONG_REAL_EDGE,
	RM3_DD_WALK_AND_TURN_LEFT_ALONG_EAGLE_BOUNDARY_EDGE,
	RM3_DD_WALK_AND_TURN_RIGHT_ALONG_EAGLE_BOUNDARY_EDGE,
	RM3_DD_TURN_LEFT_AND_WALK_ALONG_DUMMY_EDGE,
	RM3_DD_TURN_RIGHT_AND_WALK_ALONG_DUMMY_EDGE,
	RM3_DD_TURN_LEFT_AND_WALK_ALONG_REAL_EDGE,
	RM3_DD_TURN_RIGHT_AND_WALK_ALONG_REAL_EDGE,
	RM3_DD_TURN_LEFT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE,
	RM3_DD_TURN_RIGHT_AND_WALK_ALONG_EAGLE_BOUNDARY_EDGE,
	RM3_DD_ACROSS_A_NODE,
	RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_FRONT_AND_LEFT_REAL_EDGES,
	RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_FRONT_AND_RIGHT_REAL_EDGES,
	RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_BACK_AND_LEFT_REAL_EDGES,
	RM3_DD_ACROSS_A_REPLACES_A_BEND_BETWEEN_BACK_AND_RIGHT_REAL_EDGES,
	RM3_DD_TURN_LEFT_ON_NODE,
	RM3_DD_TURN_RIGHT_ON_NODE,
	RM3_DD_TURN_LEFT_ON_NODE_FROM_DUMMY_TO_REAL,
	RM3_DD_TURN_RIGHT_ON_NODE_FROM_DUMMY_TO_REAL,
	RM3_DD_TURN_LEFT_ON_NODE_FROM_REAL_TO_DUMMY,
	RM3_DD_TURN_RIGHT_ON_NODE_FROM_REAL_TO_DUMMY,
	//RM3_DD_TURN_LEFT_ON_A_REPLACES_A_BEND_NODE,  disabled because of the "no walk on REPLACES_A_BEND gdtnode problem"
	//RM3_DD_TURN_RIGHT_ON_A_REPLACES_A_BEND_NODE, disabled because of the "no walk on REPLACES_A_BEND gdtnode problem"
	RM3_DD_CROSS_ON_DUMMY_NODE_BETWEEN_REAL_EDGES,
	RM3_DD_CROSS_ON_DUMMY_NODE_BETWEEN_DUMMY_EDGES,
	RM3_ADDED_BY_MAKE_UPWARD,
	RM3_ADDED_BY_MAKE_ST,
	RM3_ADDED_BY_MAKE_SIMPLE,
	RM3_ADDED_BY_MAKE_CONNECTED,
	RM3_ADDED_BY_EXPAND,
	RM3_BEND_NODE,
	RM3_ANY_BEND,
	RM3_NONE_BEND,
	RM3_MINIMAL_BEND,
	RM3_PLANARIZATION_GADGET_NODE,
	RM3_PLANARIZATION_GADGET_EDGE,
	RM3_ADDED_BY_REGULARIZATION,
	RM3_NODE_IN_SUPER_NODE,
	RM3_CENTER_OF_WHEEL
	}
marker_type; 



	/*
	GLOBALTYPE angle_type
	Different kind of angle.
        Allowed values: _000=0, _090=1, _180=2,
        _270=3, and _360=4.
	*/

	typedef enum
	{
	_000=0,
	_090=1,
	_180=2,
	_270=3,
	_360=4
	}
angle_type;




inline  std::ostream& operator <<(std::ostream& out,const marker_type x)
     {out << *((unsigned int *)((void *)&x)) ;return(out);}
inline  std::istream& operator >>(std::istream& in,marker_type& x)
     {in >> *((unsigned int *)((void *)&x)) ;return(in); }


/* Forward declaration need to allow rm3_constraints.h to work.*/

class undi_graph;
class struct_gdtnode;
class struct_gdtedge;

typedef struct_gdtnode *gdtnode;
typedef struct_gdtedge *gdtedge;



/////////////////////////////////////////////////////////////////
//String manipulation
//////////////////////////////////////////////////////////////////


		std::string
	replace_all(std::string s,std::string s1, std::string s2);
	
	std::string
gdt_itoa
	(
	int n
	);



////////////////////////////////////////////////////////////////////
// Check date and time
////////////////////////////////////////////////////////////////////


	bool
	check_date();



#endif
