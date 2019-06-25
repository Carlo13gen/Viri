/*******************************************************************************
+
+  rm3_draw_undi_graph.h
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

#ifndef DRAW_UNDI_GRAPH_H
#define DRAW_UNDI_GRAPH_H



#include <fstream>
#include <GDT/rm3_undi_graph.h>
#include <GDT/rm3_plan_undi_graph.h>
#include <GDT/rm3_upwa_plan_undi_graph.h>
#include <GDT/rm3_orth_plan_undi_graph.h>
#include <GDT/rm3_dime_orth_plan_undi_graph.h>
#include <GDT/rm3_tree.h>
#include <GDT/rm3_SPQR_tree.h>
#include <GDT/rel_coord_orth.h>
#include <GDT/gdtcolor.h>



#include <set>

struct ltpt
{
  bool operator()(const gdt::gdtpoint p1, const gdt::gdtpoint p2) const
  {
    bool p1ltp2 = true;
    double epsilon = 0.0005;
    if (gdt_maximum(p1.xcoord() - p2.xcoord(), p2.xcoord() - p1.xcoord()) < epsilon) // x-coords are equals
        {
    	if (gdt_maximum(p1.ycoord() - p2.ycoord(), p2.ycoord() - p1.ycoord()) < epsilon) p1ltp2 = false; // p1==p2
	else if (p1.ycoord() > p2.ycoord()) p1ltp2 = false; // p2 > p1
	}
    else
        if (p1.xcoord() > p2.xcoord()) p1ltp2 = false; // p2 > p1
    return p1ltp2;
  }
};


//-----------------------------------------------------------------------------
// draw_undi_graph: base class for all directed and undirected embedded graphs
//                  with basic draw-oriented information
//
// W.Didimo e A.Leonforte (1996)
//-----------------------------------------------------------------------------

	const double
PI=3.1415926535897932385;


// ------------------------------------------------------
// enumerate types for gdtnode and gdtedge graphical attributes
// ------------------------------------------------------

	/**
	Shape of a gdtnode.
        Allowed values: ELLIPSE, RECTANGLE, and TRIANGLE.
	*/

	typedef enum
	{
	ELLIPSE,
	RECTANGLE,
	TRIANGLE
	}
node_shape_type;



	/**
	Shape of an gdtedge.
        Allowed values: NORMAL, DASHED, and DOTTED.
        (currently only NORMAL is available).
	*/

	typedef enum
	{
	NORMAL,
	DASHED,
	DOTTED
	}
edge_shape_type;


	/**
	Enumerate type for direction.
        Allowed values: LEFT, RIGHT, UP, and DOWN.
	*/

	typedef enum
	{
	LEFT,
	RIGHT,
	UP,
	DOWN
	}
direction_type;

	/**
	Enumerate type for zig_zag recognition
	Allowed values: HORIZONTAL_UP_DOWN, HORIZONTAL_DOWN_UP, VERTICAL_LEFT_RIGHT, VERTICAL_RIGHT_LEFT, NO_ZIG_ZAG
	*/
	typedef enum
	{
	HORIZONTAL_UP_DOWN, 
	HORIZONTAL_DOWN_UP, 
	VERTICAL_LEFT_RIGHT, 
	VERTICAL_RIGHT_LEFT,
	NO_ZIG_ZAG
	}
zig_zag_type;


	/**
	Local structure for deafult graphical attributes.
	This class holds default values
	for data fields you can set
	individually on each gdtnode.
	*/

	class GDT_NONSTANDARD_DECL
struct_node_defaults
	{
	friend class draw_undi_graph;
	private:
		double width;
		double height;
		double altitude;
		color  body_color;
		color  border_color;
		color  label_color;
		node_shape_type shape;
	public:

		struct_node_defaults
			(
			);
	};

	/**
	This class holds default values
	for data fields you can set
	individually on each gdtedge.
	*/

	class GDT_NONSTANDARD_DECL
struct_edge_defaults
	{
	friend class draw_undi_graph;
	private:
		gdt::gdtpoint		anchor;
		color  	        body_color;
		color           label_color;
		edge_shape_type shape;
	public:

		struct_edge_defaults
			(
			);
	};

	/**
	Local-draw structure for general attributes.
	This class holds current/default values
	of data fields you can only set
	globally for the whole drawing.
	*/
	
	class GDT_NONSTANDARD_DECL
struct_draw_info
	{
	friend class draw_undi_graph;
	private:
		double scale;				// drawing scale
		double bend_radius;			// bend_radius (dimesion of the circles representing bends)
		double edge_sensivity; 			// how near you have to click to catch the gdtedge
		color  shadow_color;			// color of the shadow
		bool   shadows_are_visible;		// enable/disable shadows
		bool   node_labels_are_visible;		// enable/disable gdtnode-labels
		bool   edge_labels_are_visible;		// enable/disable gdtedge-labels
	public:

		struct_draw_info
			(
			);
	};



	/**
	Local-draw_undi structure for gdtnode.
	This class holds current values
	for each gdtnode instance.
	*/

	class GDT_NONSTANDARD_DECL
struct_draw_node_info
	{
	friend class draw_undi_graph;
	private:
		gdt::gdtpoint 		center;			// center of the gdtnode
		node_shape_type	shape;			// shape of the gdtnode (RECTNAGLE, ELLIPSE, TRIANGLE)
		double		width;			// width of the gdtnode
		double		height;			// height of the gdtnode
		double		altitude;		// altitude of the gdtnode (experimental for a 3D-effect)
		color		internal_col;		// body color of the gdtnode
		color		border_col;		// border color of the gdtnode
		color		label_col;		// label color of the gdtnode
		std::string		label;			// label associate to the gdtnode
		gdt::gdtpoint		label_pos;		// label position
	};



	/**
	Local-draw_undi structure for gdtedge.
	This class holds current values
	for each gdtedge instance.
	*/

	class GDT_NONSTANDARD_DECL
struct_draw_edge_info
	{
	friend class draw_undi_graph;
	private:
		gdt::gdtlist<gdt::gdtpoint>	bends;			// bends on gdtedge, starting from its source
		gdt::gdtpoint		anchor_source;		// anchor-distance (dx,dy) with respect to the source gdtnode center of the gdtedge
		gdt::gdtpoint		anchor_target;		// anchor-distance (dx,dy) with respect to the target gdtnode center of the gdtedge
		color		col;			// color of the gdtedge
		color		label_col;		// label color of the gdtedge
		std::string		label;			// label associated to the gdtedge
		gdt::gdtpoint		label_pos;		// label position
		edge_shape_type shape;			// shape of the gdtedge (currently only NORMAL is availbale)
	};

	

inline  std::istream& operator >>(std::istream& in,struct_draw_edge_info& x)
     {in >> *((int *)((void *)&x));return(in); }

//inline  ostream& operator <<(ostream& out,struct_draw_edge_info& x) // before commented
//       {out << *((int *)((void *)&x));return(out); }		    // before commented


//#ifdef __GNUC__  // prima era ifndef (titto 22-1-99)
  inline  std::ostream& operator <<(std::ostream& out,const struct_draw_edge_info& x)
       {out << *((unsigned int *)((void *)&x));return(out); }
//#endif

#ifdef __linux__
  inline  std::ostream& operator <<(std::ostream& out,struct_draw_edge_info& x)
       {out << *((unsigned int *)((void *)&x));return(out); }
#endif



//inline  ostream& operator <<(ostream& out,const NodeInfo x)
    //{out << *((int *)((void *)&x)) ;return(out);}
inline  std::istream& operator >>(std::istream& in,struct_draw_node_info& x)
     {in >> *((int *)((void *)&x));return(in); }

// -----------------------------------------------------

	/**
	A 'draw_undi_graph' represents an undi_graph with coordinates associated to nodes and bends,
	more a series of graphical attributes for nodes and edges.
	The basic idea of this class is that it has a constructor from each other class in which
	coordinates (double in general) are assigned in order to produce an appropriate layout.
	For instance, it is possible to make a draw_undi_graph object initializing it with an
	orth_plan_undi_graph, and specifying one of the compaction algorithms provided in the library.
	More details about the layout algorithms available for each constructor are provided directly
	in the method documentation. <P>

	Technical details about gdtnode and bends coordinates:
	<UL>
	<LI> Each gdtnode has a center point and an associated shape. You can also define width and
	  height of the bounding box including gdtnode; observe that these dimensions are absolute,
	  and do not depending on the scale of the drawing. At each moment you can decide
	  to automatically set the scale (rescale() method) in order to avoid gdtnode collisions due
	  to change of gdtnode dimensions.
	
	<LI> Each gdtedge is represented as a polygonal-line and then it contains a list of points
         (bends) defining such a polygonal-line. Also, at each gdtedge are associated two extremal
	  points, called anchor_points. Namely, if v and w are the two extremal nodes of the
	  gdtedge e, such that v = source(e) and w = target(e), anchor_source(e) and anchor_target(e)
	  represent the signed-distances (dx,dy) of the anchor_points from the center of v and w,
	  respectively. Then, if center of a gdtnode is moved, all anchor_points of the edges
	  incident on it are moved.
	</UL>
	NOTICE: features about gdtnode and gdtedge shadows are experimental.
	*/
	
	class GDT_NONSTANDARD_DECL
draw_undi_graph : public undi_graph
	{
	private:
		// -----------------
		// private variables
		// -----------------
	
		struct_draw_info*     draw_curr; 				// current values for parameters applicable to the whole drawing
		struct_draw_info*     draw_defs; 				// default values for parameters applicable to the whole drawing
		struct_node_defaults* node_defs; 				// default values for parameters applicable to each gdtnode
		struct_edge_defaults* edge_defs; 				// default values for parameters applicable to each gdtedge
		
		gdt::gdtnode_map<struct_draw_node_info>* node_info;			// correspondence gdtnode --> draw_undi-gdtnode structure
		gdt::gdtedge_map<struct_draw_edge_info>* edge_info;			// correspondence gdtedge --> draw_undi-gdtedge structure
		
		// ---------------
		// private methods
		// ---------------

		void local_new  ();   						// create a new set of pointers for the not-inherited class-part
		void local_del  ();   						// delete all the not-inherited class-part
		void local_renew();   						// utility function : just local_del(), then local_new()
		
		
		void local_init
			(
			const undi_graph&, 
			bool remove_crosses = true
			);							// init the not-inherited class-part from the undi_graph class  
			
		void local_init
			(
			const plan_undi_graph&, 
			algorithm_type=DEFAULT_ALGORITHM,
			face=NULL_FACE, 
			bool remove_crosses=true, 
			gdt::gdtedge_array<int>* cp = NULL,
			int min_switches = 0
			);  							// init the not-inherited class-part from the plan_undi_graph class

		void local_init
			(
			const dime_orth_plan_undi_graph&, 
			bool refine_compaction = true, 
			bool = true, 
			bool remove_crosses = true
			);							// init the not-inherited class-part from the dime_orth_plan_undi_graph class

			
		void local_init
			(
			int details,
			const dime_orth_plan_undi_graph& 
			);							// init the not-inherited class-part from the dime_orth_plan_undi_graph class	

			
		void local_init
			(
			const tree&, 
			algorithm_type alg=DEFAULT_ALGORITHM, 
			bool remove_crosses = true
			);							// init the not-inherited class-part from the tree class  

		void local_init
			(
			const upwa_plan_undi_graph&, 
			algorithm_type alg=DEFAULT_ALGORITHM, 
			bool remove_crosses = true
			); 							// init the not-inherited class-part from the upwa_plan_undi_graph class

		void local_init
			(
			const draw_undi_graph&, 
			bool remove_crosses = true
			);							// init the not-inherited class-part from the draw_undi_graph class
			

		void local_init_for_dimensioned_nodes
			(
			const dime_orth_plan_undi_graph& dopug,
			const dime_orth_plan_undi_graph& dopug1,
			const gdt::gdtnode_map<gdtnode>& super_node,
			const gdt::gdtnode_array<int>& w,
			const gdt::gdtnode_array<int>& h,
			bool remove_creosses = true
			);							// init the not-inherited class-part from the dime_orth_plan_undi_graph class
										// and for nodes with fixed dimension


		void remove_bend_nodes(dime_orth_plan_undi_graph& dopug);	// remove nodes marked RM3_BEND_NODE, executing a decompression
										// of edges with thickness greater than one.
										// This method can be used only after dopug has been copied in *this,
										// and it needs informations contained in dopug to correctly
										// execute the egde-decompression.

		void remove_bend_nodes_for_dimensioned_nodes
			(
			const dime_orth_plan_undi_graph& dopug,
			const dime_orth_plan_undi_graph& dopug1,
			const gdt::gdtnode_array<int>&		 w,
			const gdt::gdtnode_array<int>&		 h,
	      		const gdt::gdtnode_map<gdtnode>&            super_node
			);


		void normalize_coordinates   ();				// all gdtnode and bend coordinates are multiplicated for the scale
		void denormalize_coordinates ();				// all gdtnode and bend coordinates are divided for the scale


		// methods for simplifying zig_zag patterns
		//
		zig_zag_type is_zig_zag (gdt::gdtsegment s1, gdt::gdtsegment s2, gdt::gdtsegment s3);		

		bool segment_intersects_node(gdt::gdtsegment s, gdtnode v);
		bool segment_intersects_nodes (gdt::gdtsegment s);
		bool segment_overlaps_edge (gdt::gdtsegment s, gdtedge e, gdt::gdtpoint* p=NULL);
		bool segment_overlaps_edges (gdt::gdtsegment s, gdt::gdtpoint* p=NULL);
		

		// -----------------------------------
		// Methods for refined orth-compaction
		// -----------------------------------

		double refine_orth_compaction (std::set<gdt::gdtpoint,ltpt>& P);			  	// execute a post-processing in order to refine the
										  		// compaction of the drawing. Return the ratio of
										  		// the total length-gdtedge improvement

		direction_type direction_of_edge		   (gdtedge e, gdtnode v);		// return the direction of the gdtedge e with respect to gdtnode v
		direction_type direction_of_second_segment_of_edge (gdtedge e, gdtnode v);		// return the direction of the second segment of e with respect to gdtnode v
		gdt::gdtlist<gdtedge>     edges_in_direction	   (gdtnode v, direction_type d);	// return the list of edges in the specified direction with respect to gdtnode v
		
		bool contract_bridge_with_one_bend
			(
			gdtedge  	       e,
			gdtnode  	       n, 
			gdt::gdtpoint 	       p,
			direction_type s, 
			direction_type d,
			std::set<gdt::gdtpoint,ltpt>&    C,
			int& first
			);									// see below..
			
			// ----------------------------------------------------------------------------
			// execute a contraction of e if it is possible 
			// PRECONDITIONS: e is an gdtedge with a 1-degree extremal gdtnode and exactly two
			//		  segments.
			//
			// e  	 is the gdtedge to contract
			// n  	 is the gdtnode with degree = 1
			// p  	 is the center of gdtnode opposite(n) with degree > 1 from which brings ei
			// s  	 is the direction of the first segment of gdtedge e from the gdtnode opposite(n)
			// d  	 is the direction of the second segment of the gdtedge e
			// C  	 is the set of crosses in the draw
			// first is the first position which is not surely forbidden for the gdtnode n
			// ----------------------------------------------------------------------------
		
		bool contract_bridge_with_no_bends
			(
			gdtnode 	       n, 
			gdt::gdtpoint 	       p,
			direction_type s, 
			int 	       mlfs, 
			std::set<gdt::gdtpoint,ltpt>&    C
			);									// see below..
			
			// ----------------------------------------------------------------------------
			// execute a contraction of e if it is possible 
			// PRECONDITIONS: e is an gdtedge with a 1-degree extremal gdtnode and exactly one
			//		  segment.
			//
			// e  	 is the gdtedge to contract
			// n  	 is the gdtnode with degree = 1
			// p  	 is the center of gdtnode opposite(n) with degree > 1 from which brings ei
			// s  	 is the direction of the first segment of gdtedge e from the gdtnode opposite(n)
			// d  	 is the direction of the second segment of the gdtedge e
			// C  	 is the set of crosses in the draw
			// first is the first position which is not surely forbidden for the gdtnode n
			// ----------------------------------------------------------------------------	 
		
		
		bool position_is_compatible 
			(
			gdt::gdtpoint 	       d,
			gdtnode  	       n,
			gdtedge  	       e, 
			direction_type d2, 
			std::set<gdt::gdtpoint,ltpt>&    C,
			int 	       k, 
			int 	       j, 
			int& 	       j_start
			);									// return true if the point d is in a compatible position for the gdtnode n

		gdtnode   node_with_center     (gdt::gdtpoint p);						// return the gdtnode, if it exists, which have the center in point p in grid unit
		gdtedge   find_orthogonal_edge (gdt::gdtpoint p) const;  					// see below..
		
			// ----------------------------------------------------------------------------------
			// Return the gdtedge, if it exists, that contains the point p with a tolerance of 0.25;
			// the tolerance is high in order to consider every gdtedge with integer coordinates
			// even in the case of double coordinates;
			// this method is fit for coordinates expressed in a grid unit
			// ----------------------------------------------------------------------------------	
		
		// ------------------------------------------------------------------------------------------------
		
		
		void 	    set_scale  	      (double);				  // set the scale of the drawing

		gdt::gdtlist<gdt::gdtpoint> fragment_polyline (const gdt::gdtlist<gdt::gdtpoint>& bl, double c);  // fragment the polyline 'bl', with refinement degree c
		gdt::gdtlist<gdt::gdtpoint> fragment_segment  (gdt::gdtpoint p1, gdt::gdtpoint p2, double c);     // fragment the segment 'p1--p2', with refinement degree c
		
		// ----------------------
		// EDITOR private methods
		// ----------------------
		
		//void	ctrl_editor_functions  (window&, int, gdt::gdtpoint);						// handle the events associate to control-key press
		void	shift_editor_functions (int, gdt::gdtpoint);							// handle the events associate to shift-key press

		//void	draw_bend     (window& W, gdt::gdtpoint p, color col);						// draw a bend in point p and with color col
		//void	draw_polyline (window& W, gdt::gdtlist<gdt::gdtpoint>& pl, color col);					// draw a polyline with pl as control-points list, and with color col

		//void	draw_shadow (gdtnode v, window& W);							// draw shadow for gdtnode v
		//void	draw_shadow (gdtedge e, window& W, bool curve = false, int ref1 = 300, double ref2 = 0.4);	// draw shadow for gdtedge e. If curve == true, gdtedge-shadow is drawn as a bezier curve.
														// ref1 and ref2 specify two refinement parameters for curves

		void	update_embedding_after_node_move (gdtnode u);						// update the adjacent list of gdtnode u, after it is moved

		gdtedge	add_edge_to_embedding (gdtnode v, gdtnode w, gdt::gdtlist<gdt::gdtpoint>& bend_points);			// add an gdtedge (v,w) to embedding, considering the  bend_points list defining it
		gdtedge	add_edge_to_embedding (gdtnode v, gdtnode w, struct_draw_edge_info, gdtnode s);			// add an gdtedge (v,w) to embedding, according to struct_draw_edge_info. If s != NULL_NODE make the gdtedge directed from s.
														// PRECONDITIONS: s can be either v or w, or NULL_NODE.
		
		gdtnode	add_node_to_embedding (gdt::gdtpoint p);							// add a gdtnode with center p to embedding
		gdtnode	add_node_to_embedding (struct_draw_node_info ni);					// add a gdtnode to embedding, according to its struct_draw_node_info ni.
		
		gdtedge	find_prev_adj_edge (gdtnode u, gdt::gdtpoint p, bool& ae) const;					// find the previous gdtedge adjacent u, starting from p and walking clockwise. Bool ae=true if the embedding is ambiguous.
		
		gdtnode	new_node (                                    struct_draw_node_info ni);		// add a new gdtnode with struct_draw_node_info ni
		gdtedge	new_edge (gdtnode v1, gdtnode v2,                   struct_draw_edge_info ei);		// add a new gdtedge (v1,v2) with struct_draw_edge_info ei
		gdtedge	new_edge (gdtnode v1, gdtedge e1, gdtnode v2,          struct_draw_edge_info ei, int dir=gdt::after);
														// add a new gdtedge (v1,v2) with struct_draw_edge_info ei and put it after/before e1 around v1
		gdtedge	new_edge (gdtnode v1, gdtedge e1, gdtnode v2, gdtedge e2, struct_draw_edge_info ei, int dir=gdt::after);
														// add a new gdtedge (v1,v2) with struct_draw_edge_info ei and put it after/before e1 around v1 and after/before e2 around v2.
		
		// --------------------------

		
		// private wrapped constructor
		//
		void construct_draw_undi_graph 				// called by each private constructor from the orth_plan_undi_calss
			(
			const orth_plan_undi_graph& opug,
			heading_type dir, 
			algorithm_type alg = DEFAULT_ALGORITHM, 
			bool remove_crosses = true
			);					

	
	
	public:

		/*
		CATEGORY constructors_destructors
		Constructors and destructors.
		*/
			
			/**
			Empty constructor.
			*/

		draw_undi_graph  
			(
			);

			
			/**
			Destructor.
			*/
		
		~draw_undi_graph 
			(
			);					

			
			/**
			Constructor from the undi_graph class.
			Graph is drawn as a polyline representation.
			If remove_crosses is true, dummy cross-nodes
			are automatically removed.<P>

			PRECONDITIONS: graph must be biconnected
			*/
		
		draw_undi_graph 
			(
			const undi_graph& ug,
			bool remove_crosses = true
			);						
			
			
			/**
			Constructor from the plan_undi_graph class.
			Graph can be drawn with 5 different algorithms:
			<UL>
			<LI> VISIBILITY (DEFAULT_ALGORITHM)	
				execute a standard visibility algorithm;
		
			<LI> POLYLINE
				execute a standard polyline algorithm;

			<LI> ORTH_FROM_VISIBILITY
				execute a standard visibility algorithm
				and then obtains an orthogonal from it;

			<LI> VISIBILITY_COMPACTED
				execute a compacted visibility according
				to the gdtedge costs defined by (*cp). Flow
				techniques are used; 
		
			<LI> POLYLINE_COMPACTED
				execute a compacted polyline according to
			 	the gdtedge costs defined by (*cp). Flow
				techniques are used.

			<LI> ORTH_FROM_VISIBILITY_COMPACTED
				execute a compacted visibility algorithm
				and then obtains an orthogonal from it;
				
			<LI> PLAN_UPWARD_EMBEDDING
				execute a compacted polyline according by first
				computing an upward orientation, and according to
			 	the gdtedge costs defined by (*cp). Also, the int
				min_switches influence the area and the screen ratio
				of the drawing. min_switches = INFINITE means "no optimization";
				min_switches = 0, ensures the minimum number of switches;
				Augmenting min_switches it decreases the height of the drawing 
				and increases its width. 
			</UL>
			 If remove_crosses is true, dummy cross-nodes
			 are automatically removed.<P>

			 PRECONDITIONS: graph must be biconnected<P>
			 NOTES	: if cp == NULL all gdtedge costs are
					  considered equal in _COMPACTED
					  algorithms.
			*/

		draw_undi_graph 
			(
			const plan_undi_graph& pug,
			algorithm_type alg=DEFAULT_ALGORITHM,
			face ef = NULL_FACE,
			bool remove_crosses = true, 
			gdt::gdtedge_array<int>* cp = NULL,
			int min_switches = 0
			);						

			
			/**
			Constructor from the orth_plan_undi_graph class.
			Graph can be drawn with 4 different algorithms:

			<UL>
			<LI> FAST_COMPACTION 	
				execute a linear-time compaction algorithm,
				based on a DFS visit (area-minimiz. after
				rectangularization);
		
			<LI> FAST_COMPACTION_REFINED
				execute a FAST_COMPACTION and then improve 
				the drawing by reducing bridge-lengths, if 
				it is possible; 
		
			<LI> SLOW_COMPACTION
				execute a polynomial-time compaction 
				algorithm, based on flow techniques (length
				and area-minimiz. after rectangularization);

			<LI> SLOW_COMPACTION_REFINED 
				execute a SLOW_COMPACTION and then improve
				the drawing by reducing bridge-lengths, if
				it is possible.
				
		 	<LI> FAST_REGULAR_COMPACTION_1
				execute a linear-time compaction algorithm,
				based on a DFS visit (area-minimiz. after
				regularization with "heuristic 1");
		
			<LI> FAST_REGULAR_COMPACTION_1_REFINED
				execute a FAST_REGULAR_COMPACTION_1 and then improve 
				the drawing by reducing bridge-lengths, if 
				it is possible; 
		
			<LI> SLOW_REGULAR_COMPACTION_1
				execute a polynomial-time compaction 
				algorithm, based on flow techniques (length
				and area-minimiz. after regularization with
				"heuristic 1");
		
			<LI> SLOW_REGULAR_COMPACTION_1_REFINED
				execute a SLOW_COMPACTION_REFINED_1 and then improve
				the drawing by reducing bridge-lengths, if
				it is possible.

			<LI> FAST_REGULAR_COMPACTION_2 	
				execute a linear-time compaction algorithm,
				based on a DFS visit (area-minimiz. after
				regularization with "heuristic 2");
		
			<LI> FAST_REGULAR_COMPACTION_2_REFINED
				execute a FAST_REGULAR_COMPACTION_2 and then improve 
				the drawing by reducing bridge-lengths, if 
				it is possible; 
		
			<LI> SLOW_REGULAR_COMPACTION_2
				execute a polynomial-time compaction
				algorithm, based on flow techniques (length
				and area-minimiz. after regularization with
				"heuristic 1");
		
			<LI> SLOW_REGULAR_COMPACTION_2_REFINED
				execute a SLOW_COMPACTION_REFINED_2 and then improve
				the drawing by reducing bridge-lengths, if
				it is possible.
			</UL>	
				
			 If remove_crosses is true, dummy cross-nodes
			 are automatically removed.
			*/

		draw_undi_graph 
			(
			const orth_plan_undi_graph& opug, 
			algorithm_type alg = DEFAULT_ALGORITHM, 
			bool remove_crosses = true
			);					
			

			
			/**
			Constructor from the orth_plan_undi_graph class.
			Graph can be drawn with 4 different algorithms:
			
			<UL>
			<LI> FAST_COMPACTION 	
				execute a linear-time compaction algorithm,
				based on a DFS visit (area-minimiz. after
				rectangularization);
		
			<LI> FAST_COMPACTION_REFINED
				execute a FAST_COMPACTION and then improve 
				the drawing by reducing bridge-lengths, if 
				it is possible;
		
			<LI> SLOW_COMPACTION
				execute a polynomial-time compaction 
				algorithm, based on flow techniques (length
				and area-minimiz. after rectangularization);

			<LI> SLOW_COMPACTION_REFINED 
				execute a SLOW_COMPACTION and then improve
				the drawing by reducing bridge-lengths, if
				it is possible.

		 	<LI> FAST_REGULAR_COMPACTION_1 	
				execute a linear-time compaction algorithm,
				based on a DFS visit (area-minimiz. after
				regularization with "heuristic 1");
		
			<LI> FAST_REGULAR_COMPACTION_1_REFINED
				execute a FAST_REGULAR_COMPACTION_1 and then improve 
				the drawing by reducing bridge-lengths, if
				it is possible; 
		
			<LI> SLOW_REGULAR_COMPACTION_1
				execute a polynomial-time compaction 
				algorithm, based on flow techniques (length
				and area-minimiz. after regularization with
				"heuristic 1");
		
			<LI> SLOW_REGULAR_COMPACTION_1_REFINED
				execute a SLOW_COMPACTION_REFINED_1 and then improve
				the drawing by reducing bridge-lengths, if
				it is possible.
				
			<LI> FAST_REGULAR_COMPACTION_2 	
				execute a linear-time compaction algorithm,
				based on a DFS visit (area-minimiz. after
				regularization with "heuristic 2");
		
			<LI> FAST_REGULAR_COMPACTION_2_REFINED
				execute a FAST_REGULAR_COMPACTION_2 and then improve 
				the drawing by reducing bridge-lengths, if 
				it is possible; 
		
			<LI> SLOW_REGULAR_COMPACTION_2
				execute a polynomial-time compaction
				algorithm, based on flow techniques (length
				and area-minimiz. after regularization with
				"heuristic 1");
		
			<LI> SLOW_REGULAR_COMPACTION_2_REFINED
				execute a SLOW_COMPACTION_REFINED_2 and then improve
				the drawing by reducing bridge-lengths, if
				it is possible.	
			</UL>	
				
			 If remove_crosses is true, dummy cross-nodes
			 are automatically removed.

			 The "dir" parameter specifies the direction of the drawing with
			 respect the reference border step of "opug".
			*/

		draw_undi_graph 
			(
			const orth_plan_undi_graph& opug,
			heading_type dir, 
			algorithm_type alg = DEFAULT_ALGORITHM, 
			bool remove_crosses = true
			);					

	
			/**
			Constructor from the dime_orth_plan_undi_graph class.
			A copy of coordinates is executed to initialize the 
			drawing. Also, if refine_compaction = true, improve
			the drawing by reducing bridge-lengths, if it
			is possible.
			If remove_crosses is true, dummy cross-nodes
			are automatically removed.
			*/
	
		draw_undi_graph
			(
			const dime_orth_plan_undi_graph& dopug,
			bool refine_compaction = true, 
			bool remove_crosses = true
			);
		
			
			/**
			Constructor from the dime_orth_plan_undi_graph class.
			A drawing with the same shape and orientation as "dopug"
			is computed. The nodes are expanded according to the 
			widths and heights specified by "w" and "h", respectively.
			These values are defined on the nodes of dopug.
			If edges_centered is true, then all the edges will leave
			the sides of their extremal nodes at the center.
			*/
			
		draw_undi_graph 
			(
			const gdt::gdtnode_array<int>& w,
			const gdt::gdtnode_array<int>& h,
			const dime_orth_plan_undi_graph& dopug,
			bool  edges_centered = false
			);						


			/**
			Constructor from the dime_orth_plan_undi_graph class.
			A copy of coordinates is executed to initialize the 
			drawing. The nodes and edges of the "dopug" that will
			be visualized in the drawing depend on the "details" value.
			Namely, the following values are valid for "details":<P>
			
			0 - all nodes and edges are visualized;<BR>
			1 - all nodes and edges are visualized, but those deriving from 
			    linearization, rectangularization, and regularization processes.
			*/

		draw_undi_graph 
			(
			int details,
			const dime_orth_plan_undi_graph& dopug
			);					
			
		
			/**
			Constructor from the tree class.
			Graph can be drawn with 2 different algorithms:

			<UL>
			<LI> TREE_DRAW_CENTERSONS (DEFAULT_ALGORITHMS)
				each gdtnode is centered with respect its sons.<BR>
				NOTICE: Node-coordinates are not on an 
					integral-grid in general.

			 <LI> TREE_DRAW_CENTERSUBTREE
				each gdtnode is centered with respect its subtree.
				NOTICE: much area is needed in general.<BR>
			</UL>

			If remove_crosses is true, dummy cross-nodes
			are automatically removed.
			*/
		
		draw_undi_graph 
			(
			const tree& tr,
			algorithm_type alg=DEFAULT_ALGORITHM, 
			bool remove_crosses = true
			);						
			
			
			/**
			Constructor from the upwa_plan_undi_graph class.
			Graph can be drawn with 4 different algorithms, 
			
			<UL>
			<LI> VISIBILITY 	
				execute a standard visibility algorithm;
			
			<LI> VISIBILITY_COMPACTED
				execute a compacted visibility according
				to the gdtedge costs defined by (*cp). Flow
				techniques are used;

			<LI> POLYLINE
				execute a standard polyline algorithm;

			<LI> POLYLINE_COMPACTED (DEFAULT_ALGORITHM)
				execute a compacted polyline according to
			 	the gdtedge costs defined by (*cp). Flow
				techniques are used.
			</UL>

			If remove_crosses is true, dummy cross-nodes
			are automatically removed.<P>

			PRECONDITIONS: graph upug must describe an upward 
				          drawing, that is the method 
					  apply_layout_algorithm()
					  has been already applied to upug. 
			*/


		draw_undi_graph
			(
			const upwa_plan_undi_graph& upug, 
			algorithm_type alg=DEFAULT_ALGORITHM, 
			bool remove_crosses = true
			);						


			/**
			Constructor from the rel_coord_orth class.
			The boolean specifies if the nodes of the rel_coord_orth
			composing a box representing a node should be replaced
			with a single node as large as the whole box.
			*/

		draw_undi_graph
			(
			rel_coord_orth& rco,
			bool replace_dummy_nodes = true
			);						

			
			/**
			Copy constructor. If remove is true, 
			dummy cross-nodes are automatically removed.
			*/

		draw_undi_graph 
			(
			const draw_undi_graph& dug, 
			bool remove_crosses = true
			);						

		/*
		CATEGORY operators
		Operators.
		*/
	
			
			/**
			Equality operator from undi_graph class.
			*/
	
			draw_undi_graph& 
		operator = 
			(
			const undi_graph& ug
			);			

			
			/**
			Equality operator from plan_undi_graph class.
			*/

			draw_undi_graph& 
		operator =
			(
			const plan_undi_graph& pug
			);		


			/**
			Equality operator from orth_plan_undi_graph class.
			*/

			draw_undi_graph& 
		operator = 
			(
			const orth_plan_undi_graph& opug
			);	

			
			/**
			Equality operator from dime_orth_plan_undi_graph class.
			*/

			draw_undi_graph& 
		operator = 
			(
			const dime_orth_plan_undi_graph& dopug0
			);

			
			/**
			Equality operator from tree class.
			*/

			draw_undi_graph& 
		operator =  
			(
			const tree& tr
			);		

			
			/**
			Equality operator from upa_plan_undi_graph class.
			*/

			draw_undi_graph& 
		operator = 
			(
			const upwa_plan_undi_graph& upug
			);

			
			/**
			Copy equality operator.
			*/

			draw_undi_graph& 
		operator = 
			(
			const draw_undi_graph& dug
			);		
		
		/*
		CATEGORY access_dimensions
		Access methods for informations about dimensions.
		*/
		
		// Dimensions
			
			
			/**
			Return the drawing scale.
			*/

			double
		scale    
			(
			) const;
			
			
			/**
			Return the width of the smallest rectangle covering the drawing.
			*/

			double
		width    
			(
			) const;
			
			
			/**
			Return the width of the smallest rectangle covering gdtnode v.
			*/
			
			double		
		width    
			(
			gdtnode v
			) const; 			

			
			/**
			Return height of the smallest rectangle covering the drawing.
			*/
		
			double		
		height   
			(
			) const; 			

			
			/**
			Return the height of the smallest rectangle covering gdtnode v.
			*/
		
			double		
		height   
			(
			gdtnode v
			) const;

			
			/**
			Return the area of the smallest rectangle covering the drawing.
			*/

			double		
		area     
			(
			) const; 		

			
			/**
			Return the area of the smallest rectangle covering gdtnode v.
			*/
			
			double
		area     
			(
			gdtnode v
			) const; 		


			/**
			Return the length of gdtedge e.
			*/

			double		
		length   
			(
			gdtedge e
			) const; 			
		
		/*
		CATEGORY access_positions
		Access methods for informations about positions.
		*/

			
			/**
			Return the center of the smallest rectangle covering the drawin.
			*/
		
			gdt::gdtpoint
		center 	   	   
			(
			) const; 	

			
			/**
			Return the center of gdtnode v.
			*/
		
			gdt::gdtpoint
		center 	   	   
			(
			gdtnode v
			) const;

			
			/**
			Return the label position of gdtnode v.
			*/
			
			gdt::gdtpoint
		label_position         
			(
			gdtnode v
			) const; 	


			/**
			Return the label position of gdtedge e.
			*/
			
			gdt::gdtpoint
		label_position         
			(
			gdtedge e
			) const; 	
		
			
			/**
			Return the anchor-source distance of gdtedge e.
			*/

			gdt::gdtpoint
		anchor_source	   
			(
			gdtedge e
			) const;
		

			/**
			Return the anchor-target distance of gdtedge e.
			*/
			
			gdt::gdtpoint
		anchor_target	   
			(
			gdtedge e
			) const; 	

			
			/**
			Return the anchor-source point of gdtedge e.
			*/
		
			gdt::gdtpoint
		anchor_source_point
			(
			gdtedge e
			) const; 	

			
			/**
			Return the anchor-target point of gdtedge e.
			*/
		
			gdt::gdtpoint
		anchor_target_point
			(
			gdtedge e
			) const;	


			/**
			Return the anchor-v distance of gdtedge e.
			*/

			gdt::gdtpoint
		anchor		   
			(
			gdtedge e,
			gdtnode v
			) const;	


			/**
			Return the anchor-v point of gdtedge e.
			*/
			
			gdt::gdtpoint
		anchor_point
			(
			gdtedge e,
			gdtnode v
			) const;	
		
			
			/**
			Return the intersection point between gdtedge e and border of gdtnode v.
			*/
		
			gdt::gdtpoint
		intersection_edge_node 
			(
			gdtedge e, 
			gdtnode v
			) const; 			

			
			/**
			Return the intersection points between gdtedge e1 and gdtedge e2.
			*/
			
			gdt::gdtlist<gdt::gdtpoint>
		intersection_edge_edge 
			(
			gdtedge e1,
			gdtedge e2
			) const; 	

			
			/**
			Return the bend positions on gdtedge e, from source(e) to target(e).
			*/
		
			gdt::gdtlist<gdt::gdtpoint>
		bends                  
			(
			gdtedge e
			) const; 	

			
			/**
			Return the bend positions on gdtedge e, from v to opposite(e,v);
			*/
		
			gdt::gdtlist<gdt::gdtpoint>
		bends                  
			(
			gdtedge e, 
			gdtnode v
			) const; 	

			
			/**
			Return all the drawing cross points.
			*/
		
			gdt::gdtlist<gdt::gdtpoint>
		crosses		   
			(
			) const; 	

			
			/**
			Return the altitude (i.e. shadow depth) of gdtnode v.
			*/

			double      
		altitude               
			(
			gdtnode v
			) const; 	
				    

			
			/**
			Return true if the gdtnode v contains p.
			*/
			
			bool
		node_contains_point
			(
			gdtnode v, 
			gdt::gdtpoint p
			) const; 	


			/**
			Return true if the gdtedge e contains p.
			*/

			bool	    
		edge_contains_point
			(
			gdtedge e, 
			gdt::gdtpoint p
			) const; 	
		
		/*
		CATEGORY access_recommendations
		Access operations for informations about recommendations.
		*/
		
		// Recommendations


			/**
			Return a recommended scale of drawing.
			*/
		
			double 
		recommended_scale          
			(
			) const;			

			
			/**
			Return a recommended position for label of gdtnode v.
			*/
		
			gdt::gdtpoint
		recommended_label_position
			(
			gdtnode v
			) const;


			/**
			Return a recommended position for label of gdtedge e.
			*/

			gdt::gdtpoint
		recommended_label_position
			(
			gdtedge e
			) const;

		/*
		CATEGORY access_colors
		Access operations for informations about colors.
		*/

		// Colors


			/**
			Return the body color of gdtnode v.
			*/

			color
		body_color
			(
			gdtnode v
			) const;


			/**
			Return the border color of gdtnode v.
			*/

			color
		border_color
			(
			gdtnode v
			) const;


			/**
			Return the body color of gdtedge e.
			*/

			color
		body_color
			(
			gdtedge e
			) const;


			/**
			Return the label color of gdtnode v.
			*/

			color
		label_color
			(
			gdtnode v
			) const;


			/**
			Return the label color of gdtedge e.
			*/

			color
		label_color
			(
			gdtedge e
			) const;

		/*
		CATEGORY access_shapes
		Access operations for informations about shapes.
		*/

		// Shapes


			/**
			Return the shape of gdtnode v.
			*/

			node_shape_type
		shape 
			(
			gdtnode v
			) const; 	
		
			
			/**
			Return the shape of gdtedge e.
			*/

			edge_shape_type	
		shape 
			(
			gdtedge e
			) const;

		/*
		CATEGORY access_labels
		Access operations for informations about labels.
		*/
		
			
			/**
			Return the label of gdtnode v.
			*/

			std::string
		label 
			(
			gdtnode v
			) const; 	
		

			/**
			Return the label of gdtedge e.
			*/
	
			std::string
		label 
			(
			gdtedge e
			) const; 	
		
		/*
		CATEGORY access_predicates
		Access operations for informations about predicates.
		*/
		
			
			/**
			Return true if shadows are visible.
			*/

			bool 
		shadows_are_visible    
			(
			) const;
		
			
			/**
			Return true if gdtnode-labels are visible.
			*/

			bool 
		node_labels_are_visible
			(
			) const;
		
			
			/**
			Return true if gdtedge-labels are visible.
			*/

			bool 
		edge_labels_are_visible
			(
			) const;
		
		/*
		CATEGORY access_click_detection
		Access operations for informations about click detection.
		*/
		

			/**
			Make visible the same method from undi_graph.
			*/

			gdtnode      
		find_node 
			(
			int id
			) const;
	
			
			/**
			Make visible the same method from undi_graph.
			*/

			gdtedge
		find_edge 
			(
			int id
			) const; 	
		
			
			/**
			Return a gdtnode v containing the point p.
			NULL_NODE if there is not any such a gdtnode.
			*/

			gdtnode      
		find_node 
			(
			gdt::gdtpoint p
			) const;
		
			
			/**
			Return an gdtedge e containing the point p.
			NULL_EDGE if there is not any such an gdtedge.
			*/

			gdtedge      
		find_edge 
			(
			gdt::gdtpoint p
			) const;
		
			
			/**
			Return the gdt::list_item of a bend on point p if there is anyone.
			In this case put in e the gdtedge containing such a bend
			*/

			gdt::list_item 
		find_bend 
			(
			gdt::gdtpoint p,
			gdtedge& e
			) const; 	
		
		/*
		CATEGORY access_statistics
		Access operations for informations about statistics.
		*/
				
			
			/**
			Return the total number of bends in the drawing.
			*/

			int    
		number_of_bends
			(
			) const;  	
		
			
			/**
			Return the number of bends along gdtedge e.
			*/

			int   
		number_of_bends
			(
			gdtedge e
			) const;  	
		

			/**
			Return the total number of bends along the edges in list l.
			*/
	
			int    
		number_of_bends     
			(
			gdt::gdtlist<gdtedge> l
			) const;  	

			
			/**
			Return the maximum number of bends along an gdtedge in the drawing.
			*/

			int    
		number_of_bends_max 
			(
			) const;  	

			
			/**
			Return the average number of bends along an gdtedge in the drawing.
			*/
	
			double 
		number_of_bends_avg
			(
			) const;  	

			
			/**
			Return the standard deviation of the number of bends along the edges.
			*/
	
			double 
		number_of_bends_sdv 
			(
			) const;  	
		
			
			/**
			Return the total length obtained summing the lenght of each gdtedge.
			*/
	
			double 
		edge_length_sum     
			(
			) const;

			
			/**
			Return the length of the longest gdtedge.
			*/
	
			double 
		edge_length_max     
			(
			) const;  	
		
			
			/**
			Return the average of edges length.
			*/
			
			double 
		edge_length_avg     
			(
			) const;
		
			
			/**
			Return the standard deviation of edges length.
			*/
	
			double 
		edge_length_sdv     
			(
			) const;
		
			
			/**
			Return the standard deviation of drawing ratio with
			respect to 4/3 screen ratio.
			*/

			double
		screen_ratio_sdv    
			(
			) const;  	
		
		/*
		CATEGORY access_defaults
		Access operations for informations about defaults.
		*/
		
		// Defaults
		
			
			/**
			Return the draw_defaults internal structure.
			*/

			struct_draw_info
		draw_defaults
			(
			) const; 	
		
			
			/**
			Return the draw_node_defaults internal structure.
			*/

			struct_node_defaults
		node_defaults
			(
			) const;		
		

			/**
			Return the draw_edge_defaults internal structure.
			*/
	
			struct_edge_defaults
		edge_defaults
			(
			) const;		
		
		/*
		CATEGORY access_miscellaneous
		Access operations for informations about miscellaneous.
		*/
		
		// Miscellaneous	

			
			/**
			Return the draw_current internal status of drawing.
			*/
				
			struct_draw_info      
		draw_status
			(
			) const;  	
		
			
			/**
			Return the draw_current internal status of gdtnode.
			*/

			struct_draw_node_info 
		node_status
			(
			gdtnode v
			) const;
		
			
			/**
			Return the draw_current internal status of gdtedge.
			*/

			struct_draw_edge_info 
		edge_status
			(
			gdtedge e
			) const;	

			
			/**
			Return the vertices of the bounding_box of the drawing.
			*/

			void 
		calc_bounding_box
			(
			double& x_min, 
			double& x_max, 
			double& y_min, 
			double& y_max
			) const; 	
		
			
			/**
			Find a gdtnode "sn" and an gdtedge "se", such that
			the right face of gdtedge "se" starting form "sn"
			is the external face of the drawing.
			This detection if possible by using the
			drawing information (nodes and bends coordinates).
			*/

			void
		detect_external_node_and_edge
			(
			gdtnode& sn, 
			gdtedge& se
			);


		/*
		CATEGORY update_preferences
		Update general preferences.
		*/


			/**
			Set the sensivity of the edges to the mouse
			click (default is 1).
			*/

			void
		 set_edge_sensivity
			(
			 double k
			 );
		
		/*
		CATEGORY update_dimensions
		Update methods about dimensions.
		*/
		
		// Dimensions
		
			
			/**
			Set the width of gdtnode with double.
			*/

			void 
		set_width
			(
			gdtnode v,
			double l
			);			     		
		
			
			/**
			Set the height of gdtnode with double.
			*/

			void 
		set_height
			(
			gdtnode v, 
			double l
			);					

			
			/**
			Set the bend radius (bend dimension).
			*/
	
			void 
		set_bend_radius 
			(
			double br
			);
		/*
		CATEGORY update_positions
		Update methods about positions.
		*/
		
			
			/**
			Set center of gdtnode at point.
			*/

			void 
		set_center         
			(
			gdtnode v, 
			gdt::gdtpoint p
			);
		
			
			/**
			Set altitude of gdtnode with double.
			*/

			void
		set_altitude       
			(
			gdtnode v, 
			double d
			);				
		
		
			
			/**
			Set the label position of gdtnode at point.
			*/
	
			void 
		set_label_position 
			(
			gdtnode v,
			gdt::gdtpoint p
			);				 

		

			/**
			Set the label position of gdtedge at point.
			*/
	
			void 
		set_label_position 
			(
			gdtedge e, 
			gdt::gdtpoint p
			);
		
		
			
			/**
			Set the anchor-source distance of gdtedge with point.
			*/
	
			void 
		set_anchor_source  
			(
			gdtedge e, 
			gdt::gdtpoint p
			);				
		

			
			/**
			Set the anchor-target distance of gdtedge with point.
			*/

			void 
		set_anchor_target  
			(
			gdtedge e, 
			gdt::gdtpoint p
			);			
		
		

			/**
			Set the anchor-point of gdtedge at gdtnode with point.
			*/
	
			void 
		set_anchor_point
			(
			gdtedge e,
			gdtnode v, 
			gdt::gdtpoint p
			);
		
			
			/**
			Set bends of gdtedge with gdt::gdtlist<point>, starting from source(e).
			*/
	
			void 
		set_bends          
			(
			gdtedge e,
			gdt::gdtlist<gdt::gdtpoint> bl
			);		
		
		

			/**
			Set bends of gdtedge with gdt::gdtlist<point>, starting from gdtnode.
			PRECONDITIONS: gdtnode belongs to gdtedge
			*/

			void 
		set_bends          
			(
			gdtedge e, 
			gdtnode v, 
			gdt::gdtlist<gdt::gdtpoint> bl
			);		

		
			/**
			Remove from gdtedge e the bends that are avoidable.
			Return the number of bends removed.
			*/
			
			int
		remove_avoidable_bends
			(
			gdtedge e
			);
			
			
			/**
			Remove from all edges the bends that are avoidable.
			Return the number of bends removed.
			*/
			
			int
		remove_avoidable_bends
			(			
			);


			/**
			If gdtedge e is longer than 10 units (which is the standard gdt distance between grid
			lines), replace e with a path. 
			Return the list of introduced nodes.

			Preconditions: 
			(1) all edges are straight-line segments;
			(2) each edge is either parallel to the x or to the y axis
			(3) the distance between the starting point and the ending point of 
			    the edge is a multiple of 10 (gdt standard distance between grid
			    lines).
			Observe that:
			(a) Due to precondition (1) you may have to run replace_bends_with_nodes() first.
			(b) Due to precondition (2) you may have to run undo_space_overlapping_edges() before
			    running replace_bends_with_nodes().
			*/
			
			gdt::gdtlist<gdtnode>
		replace_long_edge_with_chain
			(
			gdtedge e
			);


			/**
			Replace each edge longer than 10 units with a path. 
			Return the list of introduced nodes.
 			All preconditions of method replace_long_edge_with_chain apply.
			*/

			gdt::gdtlist<gdtnode>
		replace_long_edges_with_chains
			(
			);

			/**
			Remove zig-zag orthogonal patterns along the edges
			if possible, that is, when this transformation
			does not introduce crossing or overlap with
			other objects
			*/

			void
		remove_zig_zag_along_edge
			(
			gdtedge e
			);
			

			/**
			Assumes that the edges of *this enter each node overlapping at
			the same point. Distributes edges putting a small space
			between each one of them. This method is meant to be used with
			orthogonal drawings where edges enter the sides of the nodes
			perpendicular to the border of the node. 
			*/
			
			void
		space_overlapping_edges
			(
			);


			/**
			If gdtedge e has been modified by method space_overlapping_edges, 
			return it to its previous state.  
			Assumes that only the first (and last) bend of the edge has been 
			modified by space_overlapping_edges.
			Attention: this method does not change the anchor point of the edge
			*/
			
			void
		undo_space_overlapping_edges
			(
			gdtedge e
			);


			/**
			If *this e has been modified by method space_overlapping_edges, 
			return it to its previous state.  
			All assumptions and caveats of undo_space_overlapping_edges(gdtedge e) apply.
			*/
			
			void
		undo_space_overlapping_edges
			(
			);


			/**
			If gdtedge e has no bends, then it remains unchanged. 
			If gdtedge e has at least one bend, then it is removed and is
			replaced with a path. 
			Return the list of the inserted nodes.
			*/
			
			gdt::gdtlist<gdtnode>
		replace_bends_with_nodes
			(
			gdtedge e
			);


			/**
			Replace all bends with dummy vertices and return the list
			of the dummy vertices introduced.
			*/
			
			gdt::gdtlist<gdtnode>
		replace_bends_with_nodes
			(
			);



									
		/*
		CATEGORY update_colors
		Update methods about colors.
		*/
		
		// Colors
		
		
			
			/**
			Set body-color of gdtnode with color.
			*/

			void
		set_body_color
			(
			gdtnode v, 
			color c
			);



			/**
			Set border-color of gdtnode with color.
			*/
	
			void 
		set_border_color
			(
			gdtnode v, 
			color c
			);



			/**
			Set body-color of gdtedge with color.
			*/

			void
		set_body_color
			(
			gdtedge e,
			color c
			);



			/**
			Set label-color of gdtnode with color.
			*/

			void
		set_label_color
			(
			gdtnode v,
			color c
			);



			/**
			Set label-color of gdtedge with color.
			*/
		
			void 
		set_label_color
			(
			gdtedge e,
			color c
			);

		/*
		CATEGORY update_shapes
		Update methods about shape.
		*/

		// Shapes



			/**
			Set shape of gdtnode with node_shape_type.
			*/

			void 
		set_shape  
			(
			gdtnode v, 
			node_shape_type st
			);		
		
		
			
			/**
			Set shape of gdtedge with edge_shape_type 
			(currently only NORMAL is available).
			*/

			void
		set_shape  
			(
			gdtedge e, 
			edge_shape_type st
			);
		
		/*
		CATEGORY update_labels
		Update methods about labels.
		*/

		// Labels
		
		
			
			/**
			Set label of gdtnode with string.
			*/

			void 
		set_label
			(
			gdtnode v, 
			std::string s
			);					
		
		
			
			/**
			Set label of gdtedge with string.
			*/
	
			void 
		set_label 
			(
			gdtedge e,
			std::string s
			);					
		
		/*
		CATEGORY update_switches
		Update methods about switches.
		*/
		
		// Switches
		

			
			/**
			Enable showing of shadows.
			*/

			void 
		show_shadows     
			(
			);					

		
			
			/**
			Enable showing of gdtnode labels.
			*/
	
			void 
		show_node_labels 
			(
			);				
		
		
			
			/**
			Enable showing of gdtedge labels.
			*/
	
			void 
		show_edge_labels
			(
			);					
		

			
			/**
			Disable showing of shadows.
			*/
	
			void 
		hide_shadows     
			(
			);				
		
		

			/**
			Disable showing of gdtnode labels.
			*/
	
			void 
		hide_node_labels 
			(
			);					
		
		
			
			/**
			Disable showing of gdtedge labels.
			*/
	
			void 
		hide_edge_labels 
			(
			);
		
		/*
		CATEGORY update_initializers
		Update methods about initializers.
		*/

		// Initializers
		
		
			
			/**
			Apply defaults internal values to general draw_info.
			*/

			void 
		apply_defaults
			(
			);					
		
		
			
			/**
			Apply defaults internal values to draw_node_info of nod
			*/

			void 
		apply_defaults 
			(
			gdtnode v
			);					

		
			
			/**
			Apply defaults internal values to draw_edge_info of gdtedge
			*/

			void 
		apply_defaults
			(
			gdtedge e
			);					
		
		/*
		CATEGORY update_drawing_control
		Update methods about drawing controls.
		*/
		
		// Drawing control 
		
		

			/**
			Execute a shifting of double distance in direction_type
			*/

			void 
		shift                   
			(
			double sh, 
			direction_type dir
			);		
		
		
			
			/**
			Execute a rotation of angle_type angle around point ref.
			Possible values for parameter angle are _000, _090, _180, _270
			*/
	
			void 
		rotate
			(
			angle_type angle, 
			gdt::gdtpoint ref
			);
		

			
			/**
			Execute a flipping of the drawing, 
			with respect to a vertical line passing for point.
			*/
	
			void 
		flip_horizontally       
			(
			gdt::gdtpoint fulcrum
			);				
		
		
			
			/**
			Execute a flipping of the drawing, 
			with respect to a horizontal line passing for point.
			*/
	
			void 
		flip_vertically         
			(
			gdt::gdtpoint fulcrum
			);				

		
			
			/**
			Set the scale as k and update all coordinates.
			If k is not specified, it is assumed as recommended_scale.
			*/

			void
		rescale                 
			(
			double k = 0
			);   			
		
		/*
		CATEGORY update_default
		Update methods about defaults.
		*/
		
		// Defaults
		

			
			/**
			Set the draw_info defaults.
			*/

			void 
		set_draw_defaults 
			(
			struct_draw_info dd
			);			
		
		
			
			/**
			Set the draw_node_info defaults.
			*/

			void 
		set_node_defaults
			(
			struct_node_defaults nd
			);					

		

			/**
			Set the draw_edge_info defaults.
			*/

			void 
		set_edge_defaults 
			(
			struct_edge_defaults ed
			);			

		/*
		CATEGORY update_miscellaneous
		Update methods about miscellaneous.
		*/
		
		// Miscellaneous	
		
		
			
			/**
			Set the draw_current internal status with struct_draw_info.
			*/

			void 
		set_draw_status
			(
			struct_draw_info ds
			);					 
		

			
			/**
			Set the draw_current internal status of gdtnode
			with struct_draw_node_info.
			*/

			void 
		set_node_status                      
			(
			gdtnode v, 
			struct_draw_node_info ni
			);			
		
		
			
			/**
			Set the draw_current internal status of gdtedge 
			with struct_draw_edge_info.
			*/

			void 
		set_edge_status                      
			(
			gdtedge e, 	
			struct_draw_edge_info ei
			);			 
		
		
			
			/**
			Set the draw_current internal status of gdtnode 
			with struct_draw_node_info, but do not change center coordinates.
			*/

			void
		set_node_status_preserving_positions 
			(
			gdtnode v,
			struct_draw_node_info ni
			);			

		
			
			/**
			Set the draw_current internal status of gdtedge 
			with struct_draw_edge_info, but do not change bends coordinates.
			*/

			void 
		set_edge_status_preserving_positions 
			(
			gdtedge e, 
			struct_draw_edge_info ei
			);			
		
		
			
			/**
			Update label-position of gdtnode and of each gdtedge adjacent gdtnode.
			*/

			void 
		update_label_positions_after_movement_of_node 	     
			(
			gdtnode v
			);
		
		
			
			/**
			Update label-position of gdtedge.
			*/

			void
		update_label_position_after_movement_of_bend_along_edge 
			(
			gdtedge e
			);				 
		
		
			
			/**
			Update gdtnode and bend coordinates according to the dopug ones.
			PRECONDITIONS: dopug have the same set of nodes, edges and bends as *this
			*/

			void 
		update_node_and_bend_positions_according_to 	     
			(
			dime_orth_plan_undi_graph& dopug
			);
														
		
			
			/**
			Update gdtnode and bend coordinates according to the dopug ones.<P>
			PRECONDITIONS: dug have the same set of nodes, edges and bends as *this
			*/

			void 
		update_node_and_bend_positions_according_to
			(
			draw_undi_graph& dug
			);		
																

			
			/**
			Set the internal status of nodes and edges marked
			as RM3_ADDED_BY_RECTANGULARIZATION and RM3_REPLACE_A_BEND.
			*/
												
			void 
		set_dummy_nodes_and_edges_status
			(
			);				
		
		/*
		CATEGORY update_topology
		Update methods about topology.
		*/

		// Topology
		
		
			
			/**
			Delete all nodes and edges.
			*/

			void 
		clear 
			(
			);												

		

			/**
			Insert a new gdtnode with center in point p, and return it.
			*/

			gdtnode 
		new_node 
			(
			gdt::gdtpoint p,
			int new_id=AUTO_ID
			);

		
			
			/**
			Insert a new gdtedge (v,w) with bends bl from v to w, and return it.
			*/

			gdtedge 
		new_edge 	
			(
			gdtnode v, 
			gdtnode w ,         	 
			gdt::gdtlist<gdt::gdtpoint> bl,
			int new_id=AUTO_ID
			);			
		
		
			
			/**
			Insert a new gdtedge (v,w) with bends bl from v to w, and return it. 
			Topologically, the new gdtedge is put after/before gdtedge e around v.
			*/

			gdtedge 
		new_edge
			(
			gdtnode v, 
			gdtedge e , 
			gdtnode w, 	 
			gdt::gdtlist<gdt::gdtpoint> bl,
			int new_id=AUTO_ID, 
			int dir=gdt::after
			);	

		

			/**
			Insert a new gdtedge (v,w) with bends bl from v to w, and return it. 
			Topologically, the new gdtedge is put after/before gdtedge ev around v and 
			after/before gdtedge ew around w.
			*/

			gdtedge 
		new_edge 
			(
			gdtnode v,
			gdtedge ev,
			gdtnode w,
			gdtedge ew,
			gdt::gdtlist<gdt::gdtpoint> bl,
			int new_id=AUTO_ID,
			int dir=gdt::after
			);

		
			
			/**
			Insert a new bend on gdtedge e, at position i into the bend list of e,
			starting from v, and in point p.
			PRECONDITIONS: gdtedge e has at least i-1 bends
			*/
												
			void 
		new_bend 
			(
			gdtedge e,
			gdtnode v, 
			int i, 
			gdt::gdtpoint p
			);
			

			
			/**
			Insert and return a new gdtnode with center p, by splitting the two edges e1 and e2.
			The new gdtnode is marked as RM3_CROSS_ON_REAL_EDGE.
			PRECONDITIONS: p is an intersection-point between e1 and e2
			*/
												
			gdtnode       
		replace_cross_with_node 
			(
			gdtedge e1, 
			gdtedge e2, 
			gdt::gdtpoint p
			);						
		
		
			
			/**
			Replace all cross-points of the drawing with dummy nodes,
			and return the list of these dummy nodes.
			All added nodes are marked as RM3_CROSS_ON_REAL_EDGE.
			*/

			gdt::gdtlist<gdtnode> 
		replace_crosses_with_nodes
			(
			);								
			
		

			/**
			Remove a cross gdtnode v, by merging the adjacent edges appropriately.
			PRECONDITIONS: v was marked as RM3_CROSS_ON_REAL_EDGE
			by a replace_cross.. method
			*/
												
			void 
		remove_cross_node  
			(
			gdtnode v
			);									
			
		
			
			/**
			Remove a list nl of cross nodes,
			by merging the adjacent edges appropriately.
			PRECONDITIONS: each gdtnode in nl was marked as RM3_CROSS_ON_REAL_EDGE 
			by a replace_cross.. method
			*/
												
			void 
		remove_cross_nodes 
			(
			gdt::gdtlist<gdtnode> nl
			);										
		
		
			
			/**
			Remove all nodes marked as RM3_CROSS_ON_REAL_EDGE by a replace_cross.. method,
			and return the set of center-point of nodes removed (cross-points).
			*/

			std::set<gdt::gdtpoint,ltpt>
		remove_cross_nodes 
			(
			);									

		

			/**
			Remove v and merge its two adjacent edges
			e1=(v1,v), e2=(v2,v) into a new gdtedge e=(v1,v2),
			then return e.
			If Lp is specified, the direction of e
			is chosen as the direction of either e1 or e2
			if v1 or v2 has not any marker in Lp, respectively.
			Else, the direction of e is chosen randomly among e1
			or e2 ones.
			*/

			gdtedge
		remove_two_degree_node
			(
			gdtnode v,
			gdt::gdtlist<marker_type>* Lp = NULL
			);

		/*
		CATEGORY io_operations_draw
		Input / output operations about draw.
		*/

		// Draw



			/**
			Draw all nodes and edges into the window W.
			If curve=true edges is drawn as a bezier-curve.


			void
		draw
			(
			window& W,
			bool curve = false,
			int ref1 = 300,
			double ref2 = 0.4
			);

            */

			/**
			Draw the gdtnode v into the window W.


			void
		draw
			(
			gdtnode v,
			window& W
			);

            */

			/**
			Draw the gdtedge e into the window W.
			If curve=true gdtedge is drawn as a bezier-curve.


			void
		draw
			(
			gdtedge e,
			window& W,
			bool curve = false,
			int ref1 = 300,
			double ref2 = 0.4
			);	 */



			/**
			Draw the gdtnode v and all the edges around it into the window W.


			void
		draw_around
			(
			gdtnode v,
			window& W
			); */
		
		
			
			/**
			Print on file the main properties of the drawing, with the following
			conventions: <br>
					
			 1 - each gdtnode is represented by a rectangular box of dimension w x h;  <br>
			 2 - each gdtedge is represented by a polygonal line p1 -- p2 -- ... -- pk <br>
			 3 - file format is the following:  <br>
				     <br>
				<NODELIST>   <br>
			 		<NODE> id  (cx,cy)  w  h </NODE>        <br>
					... <br>
				</NODELIST>    <br>
			<br>
			 	<EDGELIST> <br>
					<EDGE> id  <DIRECTION> ->  <BEND> (x1,y1)  <BEND> (x2,y2)  ...  <BEND> (xk,yk)  <br> 
					</EDGE> <br>
					...	<br>
				</EDGELIST><br>
			<br>
			 where:   id    = gdtnode/gdtedge identifier;  <br>
				  cx cy = coordinates of the center of gdtnode;  <br>
				  w, h  = width and height of the gdtnode;  <br>
			 	  xj yj = coordinates of the point pj defining the polygonal line  <br>
				  '->' '<-' '--' = indicate the direction of the gdtedge moving from (x1,y1) to (xk,yk)  <br>
			 if an error occured while opening the file, false is returned.  <br>
			*/

			bool    
		draw	    
			(
			std::string file_name
			);	
						
		/*
		CATEGORY io_operations_file
		Input / output operations about file.		
		*/
			
			
			/**
			Write the draw in a file file_name.
			*/

			bool	
		write 
			(
			std::string file_name
			);				
			
		
			
			/**
			Read  the draw from the file file_name.
			*/

			bool
		read
			(
			std::string file_name
			);



			/**
			Append the draw_undi_graph section to ofstream out.
			*/

			void
		append_section_to_fstream
			(
			std::ofstream& out
			);



			/**
			Read the draw_undi_graph section to ifstream in.
			*/

			void
		read_section_from_fstream
			(
			std::ifstream& in
			);

			/**
			Export the draw_undi_graph object into the "fig" file format (version 3.2)
			and append the output to the specified ofstream (cout for default):
			"spline_edge"  = gdtedge approximated with spline if true (default=false);
			"refinement"   = a factor for variate the number of control points of the spline;
			"scale_factor" = indicates a zoom-factor for the drawing in xfig (default = 1 = normal dimension)
			"out"          = ostream (default = cout)
			*/
		
			void
		export_to_fig_file_format 
			(
			bool  spline_edge,
			double refinement, 
			double scale_factor,
			std::ostream& out = std::cout
			);
		
		
			/**
			Export the draw_undi_graph object into the "fig" file format (version 3.2)
			and append the output to the specified ofstream (cout for default):
			"out"          = ostream (default = cout)
			*/
			
			void
		export_to_fig_file_format 
			(
			std::ostream& out = std::cout
			);
			

			/**
			Export the draw_undi_graph object into the "fig" file format (version 3.2)
			and put it in a file named "file_name".
			"spline_edge"  = gdtedge approximated with spline if true (default=false);
			"scale_factor" = indicates a zoom-factor for the drawing in xfig (default = 1 = normal dimension) 
			"refinement"   = a factor for variate the number of control points of the spline;
			"file_name"    = name of the output file
			*/

			void
		export_to_fig_file_format 
			(
			std::string file_name,
			bool  spline_edge = false,
			float refinement = 0.4, 
			float scale_factor = 1
			);		
		


			/**
			Export the draw_undi_graph object to an "xml" file format 
			and append the output to the specified ofstream (cout for default):
			"out"          = ostream (default = cout).
			The DTD of the xml document is defined in the file itself, and it is
			simple to understand. It is based on the description of the attributes
			given by the "draw (string file_name)" method.
			
			Note: currently the xml file only describes the geometry of the drawing
			*/
			
			void
		export_to_xml_file_format 
			(
			std::ostream& out = std::cout
			);
			
		
			/**
			Export the draw_undi_graph object to an "xml" file format
			and put it in a file named "file_name".
			The DTD of the xml document is defined in the file itself, and it is
			simple to understand. It is based on the description of the attributes
			given by the "draw (string file_name)" method.
			
			Note: currently the xml file only describes the geometry of the drawing
			*/
			
			void
		export_to_xml_file_format 
			(
			std::string file_name
			);		
		
			
			/**
			Export the draw_undi_graph object to JGraph Builder file format
			and put it in a file named "file_name".
			*/
			
			bool
		export_to_graphml_file_format
			(
			std::string file_name
			);
			
			
			/**
			Initialize a draw_undi_graph object from a file named "file_name"
			in JGraph Builder format.
			*/
			
			bool
		import_from_graphml_file_format
			(
			std::string file_name
			);
			

		/*
		CATEGORY io_operations_edit
		Input / output operations about edit.		
		*/
			
		// Edit
		
		
			
			/**
			A simple grap-editor is opened in window W, and closed when
			the quit variable is true.


			void
		edit
			(
			window& W,
			bool& quit,
			bool redraw=true
			); */
	};




#endif
