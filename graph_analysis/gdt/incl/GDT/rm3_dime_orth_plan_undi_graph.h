/*******************************************************************************
+
+  rm3_dime_orth_plan_undi_graph.h
+
+  This file is part of GDToolkit. It can be
+  used free of charge in academic research and teaching.
+  Any direct or indirect commercial use of this software is illegal
+  without a written authorization of
+  the Dipartimento di Informatica e Automazione
+  Universita' di Roma Tre, Roma, ITALIA
+
+
+
*******************************************************************************/


/*! \file */

#ifndef RM3_DIME_ORTH_PLAN_UNDI_GRAPH_H
#define RM3_DIME_ORTH_PLAN_UNDI_GRAPH_H

#if !defined(ROOT_INCL_ID)
#define ROOT_INCL_ID 34009
#endif


#include <GDT/gdtlist.h>
#include <GDT/gdtmap.h>
#include <GDT/gdtgeometry.h>
#include <GDT/rm3_undi_graph.h>
#include <GDT/rm3_flow_dire_graph.h>
#include <GDT/rm3_plan_undi_graph.h>
#include <GDT/rm3_orth_plan_undi_graph.h>

	/*
	SOURCEFILE rm3_dime_orth_plan_undi_graph.h
	To be defined.
	*/


#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3

/**
//-----------------------------------------------------------------------------
// dime_orth_plan_undi_graph:
// 	base class for all dimensioned planar undirected graphs
// 	with planar embedding, orthogonal shape,
//	orientation (north, south, east, west)
//	and integer length for each gdtedge.
//
// W.Didimo, A.Leonforte (1996)
//-----------------------------------------------------------------------------

*/

	class
dime_orth_plan_undi_graph; // forward declaration

// --------------------------
// enumerate type for heading
// -------------------------- 

	/**
	Enumerate type for heading.
        Allowed values: north = 0, east  = 1, south = 2,
        west  = 3, and undefined_heading = 4.
	*/

	typedef enum
	{
	north = 0,
	east  = 1,
	south = 2,
	west  = 3,
	undefined_heading = 4
	}
heading_type;



inline  std::istream& operator >>(std::istream& in,heading_type& x)
     {in >> *((int *)((void *)&x));return(in); }

// -----------------------------------------------------


	/**
	Enumerate type for slope.
        It's a short.
	*/

	typedef short
heading_index_type;



	/**
	Enumerate type for slope.
        Allowed values: horizontal = 0, vertical = 1, and undefined_slope = 2.
	*/

	typedef enum
	{
	horizontal      = 0,
	vertical        = 1,	
	undefined_slope = 2
	}
slope_type;



	/**
	Data structure for dynamic new_edge.
        It's fields are four nodes: north_node, south_node, east_node, 
        and west_node.
	*/

	typedef struct
	{
	gdtnode north_node;
	gdtnode south_node;
	gdtnode east_node;
	gdtnode west_node;
	bool empty()
		{
		return (
			north_node == NULL_NODE &&
			east_node  == NULL_NODE &&
			south_node == NULL_NODE &&
			west_node  == NULL_NODE
			);
		}
	}
map_node_set;

	
	/**
	Data structure to store the relative
	positions of the edges incident a gdtnode
	with respect to its bottom-left corner,
	when the gdtnode is expanded into a face.
	*/

	typedef struct
	{
	int north_edge;
	int south_edge;
	int east_edge;
	int west_edge;
	}
pos_incid_edges;
	
	
	

	/**
	Data structure for dynamic new_edge.
	*/

	typedef struct
	{
	gdtnode                    vt1;                 // source terminal dime-gdtnode of the dime-gdtedge to be created
	gdtnode                    vt2;                 // sink   terminal dime-gdtnode of the dime-gdtedge to be created
	int			bend_cost;	     // bend cost
	int                     cross_cost;          // cross cost
	int 			length_unit_cost;    // length unit cost
	heading_type            initial_heading;     // heading of the first gdtedge of the minimal-cost path
	dire_graph              map;                 // map
	gdt::gdtedge_map<gdtedge>          edge_of_map_edge;    // map-gdtedge  -> real or dummy dime-gdtedge
	gdt::gdtedge_map<gdtnode>          node_of_map_edge;    // map_edge  -> dummy dime-gdtnode
    	gdt::gdtedge_map<face>          face_of_map_edge;    // map-gdtedge  -> face crossed by the map-gdtedge
	gdt::gdtedge_map<std::string>        bends_of_map_edge;   // map_edge  -> sequence of bends
	gdt::gdtedge_map<map_node_set>  map_nodes_of_edge;   // dime-gdtedge -> set(map-gdtnode)
	gdt::gdtnode_map<map_node_set>  map_nodes_of_node;   // dime-gdtnode -> set(map-gdtnode)
	gdt::gdtedge_map<int>           map_edge_cost;       // map gdtedge  -> cost
	gdt::gdtlist<gdtedge>	        map_edge_path;       // list of map-edges along the shortest path from mnt1 to mnt2
	gdt::gdtlist<gdtedge>	        edge_path;           // sequence of the new dime-edges created between vt1 and vt2
	gdtnode             	mnt1;                // map-gdtnode centered on vt1
	gdtnode             	mnt2;                // map-gdtnode centered on vt2
	bool     is_a_terminal_node          (gdtnode n)  {return (n == vt1 || n == vt2);}
	bool     is_the_source_terminal_node (gdtnode n)  {return (n == vt1);}
	bool     is_the_sink_terminal_node   (gdtnode n)  {return (n == vt2);}
	gdtnode     source_terminal             ()        {return vt1;}
	gdtnode     sink_terminal               ()        {return vt2;}
/*	bool     map_node_belongs_to_node    (gdtnode mn, gdtnode n)
		{
		map_node_set mns = map_nodes_of_node[n];
		if (!mns.empty())
			if (mns.north_node == mn || mns.east_node == mn || mns.south_node == mn || mns.west_node == mn) return true;
		return false;
		}*/
	}
DD_struct;

	/**
	Local-dime_orth_plan_undi structure for gdtnode.
	*/

	class GDT_NONSTANDARD_DECL
struct_dime_node_info
	{
	friend class dime_orth_plan_undi_graph;
	private:
		gdt::gdtpoint position;
	};

	/**
	Local-dime_orth_plan_undi structure for gdtedge.
	*/

	class GDT_NONSTANDARD_DECL
struct_dime_edge_info
	{
	friend class dime_orth_plan_undi_graph;
	private:
		gdt::gdtlist<gdt::gdtpoint>  bends_position;  // Ordered from source to target
	};


	/**
	Used only for the Dynamic Draw.
	Enumerate type for terminal gdtnode.
	Says the type of gdtnode that starts a terminal line.
	If the gdtnode is a source, the completator of map subnet
	of the edges of terminal line will create outgoing map edges,
	entering map edges if the type is sink.
        Allowed values: <br>
	  source_terminal = 0<br>
	  sink_terminal   = 1
	*/

	typedef enum
	{
	source_terminal = 0,
	sink_terminal   = 1
	}
DD_terminal_node_type;


	/**
	Used only for the Dynamic Draw.
	<PRE>
	Enumerate type for edges of a terminal line.
	
	    Current heading
	      ---------->
	
	                      o 2
	                     / \
	                    /   \
	                 1 /     \ 3
	       - - - - - -o-------o- - - - - -
	                   \     /
	                    \   /
	                     \ /
	                      o 4
	
	The completator of map subnet of gdtedge of a terminal line may update
        the map subnet according these values:
	ONLY_SINK_OR_SOURCE_DIAGONALS                         = 1   only the edges 1-2 and 1-4
	ONLY_SINK_OR_SOURCE_DIAGONALS_FOR_EAGLE_BOUNDARY_EDGE = 2   only the edges 1-2 and 1-4, but on a eagle boundary gdtedge
	WALK_ALONG_AND_SINK_OR_SOURCE_DIAGONALS               = 3   the straigth gdtedge 1-3 and the edges 1-2 and 1-4
	
	For these edges, the direction is choosen according the value
	of "DD_terminal_node_type" of the gdtnode starting the terminal line.
	</PRE>
	*/

	typedef enum
	{
	ONLY_SINK_OR_SOURCE_DIAGONALS                         = 1,
	ONLY_SINK_OR_SOURCE_DIAGONALS_FOR_EAGLE_BOUNDARY_EDGE = 2,
	WALK_ALONG_AND_SINK_OR_SOURCE_DIAGONALS               = 3
	}
DD_edge_completament_type;


	/**
	GLOBALTYPE struct_dime_border_step_info
	Local-dime_orth_plan_undi structure for border_step.
	*/

	class GDT_NONSTANDARD_DECL
struct_dime_border_step_info
	{
	friend class dime_orth_plan_undi_graph;
	private:
		heading_type initial_heading;
	public:

		/*
		CATEGORYremoved constructors_destructors
		Constructors and destructors.
		*/

			/**
			Constructor.
			*/

		struct_dime_border_step_info
			(
			);
	};
	
	

	
	struct
_solution
	{
	int          score;
	heading_type eagle_heading;
	int          number_of_steps;
	std::string       bend_type;
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
		return *this;
		}
	};
	
	
	
	
	
	
	/**	
	A 'dime_orth_plan_undi_graph' represents a 4-planar orth_plan_undi_graph with 
	integer coordinates for nodes and bends and an heading associated to each border_step.
	Currently, if a dime_orth_plan_undi_graph is initialized with a no 4-planar 
	orth_plan_undi_graph, a compression step is executed before applying the compaction
	algorithm, in which the orthogonal representation is made 4-planar. So, a certain
	number of border_steps could represent more than one border_step and then their 
	thickness indicate this number.
	About the compaction algorithm, it is possible to chose among the follwing enumerate:

	 1 - FAST_COMPACTION		execute a linear-time compaction using a
	 				rectangularization and a DFS <br> 
	 2 - SLOW_COMPACTION		execute a polynomial-time compaction using a
	 				rectangularization and flow techniques <br>
	 3 - FAST_REGULAR_COMPACTION_1	execute a linear-time compaction using a regularization
	 				with "heuristic 1" and a DFS <br>
	 4 - SLOW_REGULAR_COMPACTION_1  execute a polynomial-time compaction using a regularization
	 				with "heuristic 1" and flow techniques <br>
	 5 - FAST_REGULAR_COMPACTION_2  execute a linear-time compaction using a regularization
	 				with "heuristic 2" and a DFS <br>
	 6 - SLOW_REGULAR_COMPACTION_2  execute a polynomial-time compaction using a regularization
	 				with "heuristic 2" and flow techniques <br> 

	Once that the dime_orth_plan_undi_graph has been initialized, it is manteined rectangularized.
	Nodes and edges added by rectangularization algorithm are marked as 
	RM3_ADDED_BY_RECTANGULARIZATION.
	*/

	class GDT_NONSTANDARD_DECL
dime_orth_plan_undi_graph
	: public orth_plan_undi_graph
	{
	private:
		// -----------------
		// private variables
		// -----------------
		
		bool 	     frozen_layout;						  	// is true, the compaction algorithm is not automatically executed after dynamic new gdtedge													
		heading_type reference_heading;						  	// heading of the reference border_step

		gdt::gdtnode_map<struct_dime_node_info>* 	       node_info;		  	// correspondence gdtnode --> dime_orth_plan_undi-gdtnode structure
		gdt::gdtedge_map<struct_dime_edge_info>* 	       edge_info;		  	// correspondence gdtedge --> dime_orth_plan_undi-gdtedge structure
		gdt::gdtmap<border_step,struct_dime_border_step_info>* border_step_info;	  	// correspondence GDT-border_step --> dime_orth_plan_undi-border_step structure
		
		// ---------------
		// private methods
		// ---------------

		//Added by Marcandalli 16 10 2001
		void undefine(gdtnode  n);
		void undefine(gdtedge e);
		//
		
		void local_new();                  					  	// create a new set of pointers for the not-inherited class-part
		void local_del();                  					  	// delete the not-inherited class-part
		void local_renew();                					  	// local_del(), then local_new().
		void local_init(algorithm_type = SLOW_COMPACTION);       			// init the not-inherited class-part
		void local_init(algorithm_type, heading_type dir, int& num_irr_faces);		// init the not-inherited class-part

		void init_headings();							  	// compute all border_step headings after rectangularization step
		
												
		void init_headings_and_positions(algorithm_type alg, heading_type dir, int& num_irr_faces);             // compute all border_step headings, and gdtnode and bend positions, according to the selected algorithm alg.
											             			// The algorithm alg can be one of the following: 
															// FAST_COMPACTION, SLOW_COMPACTION, FAST_COMPACTION_REFINED, SLOW_COMPACTION_REFINED,
											  				// FAST_REGULAR_COMPACTION_1, SLOW_REGULAR_COMPACTION_1, FAST_REGULAR_COMPACTION_1_REFINED, SLOW_REGULAR_COMPACTION_1_REFINED,
															// FAST_REGULAR_COMPACTION_2, SLOW_REGULAR_COMPACTION_2, FAST_REGULAR_COMPACTION_2_REFINED, SLOW_REGULAR_COMPACTION_2_REFINED.
											  				// num_irr_faces is the number of irregular faces when a method based on regularization is applied.
											  	
		void init_headings_and_positions_with_rectangularization(algorithm_type alg);				// This method executes a standard compaction algorithm and alg can be one of the following:
											  				// FAST_COMPACTION, SLOW_COMPACTION, FAST COMPACTION_REFINED, and SLOW_COMPACTION_REFINED.
											  				// FAST_REGULAR_COMPACTION_1, SLOW_REGULAR_COMPACTION_1, FAST_REGULAR_COMPACTION_1_REFINED, SLOW_REGULAR_COMPACTION_1_REFINED,
															// FAST_REGULAR_COMPACTION_2, SLOW_REGULAR_COMPACTION_2, FAST_REGULAR_COMPACTION_2_REFINED, SLOW_REGULAR_COMPACTION_2_REFINED.
											  	
		int  init_headings_and_positions_with_regularity(algorithm_type alg);					// This method executes a new compaction algorithm based on the regularity concept, and
															// alg can be set to 
															// FAST_REGULAR_COMPACTION_1, SLOW_REGULAR_COMPACTION_1, FAST_REGULAR_COMPACTION_1_REFINED, SLOW_REGULAR_COMPACTION_1_REFINED,
															// FAST_REGULAR_COMPACTION_2, SLOW_REGULAR_COMPACTION_2, FAST_REGULAR_COMPACTION_2_REFINED, SLOW_REGULAR_COMPACTION_2_REFINED.
															// The method returns the number of irregular faces
		
		void local_set
			(
			bool,
			heading_type,
			gdt::gdtnode_map<struct_dime_node_info>*,
			gdt::gdtedge_map<struct_dime_edge_info>*,
			gdt::gdtmap<border_step,struct_dime_border_step_info>*
			);								  	  // set all private variables


		void set_initial_heading_of_border_step      (border_step, heading_type); 	  // set the initial heading of border_step with heading_type
		void set_initial_heading_of_border_step_pair (border_step, heading_type); 	  // set the initial haeding of border_step and its opposite with heading_type and its opposite, respectively

		void set_position_of_node                    (gdtnode,gdt::gdtpoint);			  // set position of gdtnode with point
		void set_position_of_bends_along_edge        (gdtedge, gdtnode, gdt::gdtlist<gdt::gdtpoint>);		  // set position of all bends of gdtedge starting from gdtnode with gdt::gdtlist<point>

		void set_source				     (gdtedge, gdtnode);			  // set gdtnode as source of gdtnode

		border_step find_first_border_step_with_defined_heading_around_face (face) const; // find the first border_step of face that has a defined heading

		void propagate_heading (border_step, heading_type);				  // compute all headings	according to the heading_type for border_step

		void build_orthogonal_flow_network
			(
			flow_dire_graph& fn,
			gdt::gdtedge_map<gdtedge>& fn_edge,
			gdt::gdtlist<gdtedge>& L,
			slope_type scan_edge_slope
			) const;				 // see below..

		// ------------------------------------------------------------------------------------
		// build horizontal/vertical flow network associated to the rectangularized orthogonal
		// representation, that is the dual directed graph for computing a compaction algorithm
		// based on flow techniques.
		//
		// fn 			= flow network associated to the orthogonal representation;
		// fn_edge 		= mapping from orthogonal gdtedge to fn gdtedge;
		// L			= list of orthogonal edges on a longest path;
		// scan_edge_slope	= vertical or horizontal
		// ------------------------------------------------------------------------------------


		std::string 	heading_to_string (heading_type h);	// return the string corresponding to the heading h



	         //  This method sets the coordinates of the nodes in
		 //  the graphs according to the length provided in
		 //  "len". The length should be specified for all the
		 //  edges with slope equal to "slp".
		 //  The only gdtnode that is not moved is "v".
		 //  REQUIREMENTS: graph must be linearized (bends replaced with dummy vertices).

		void one_dimensional_metric_propagation( gdtnode v, const gdt::gdtedge_array<int>& len, slope_type slp );



			bool
		from_node_direction_is_free
			(
			gdtnode n,
			heading_type h
			);

			gdt::gdtpoint
		next_position_along_heading
			(
			gdt::gdtpoint p_n,
			heading_type h
			);

			bool
		the_next_position_along_heading_is_free_from_real_node_and_orthogonal_edge
			(
			gdt::gdtpoint p_n,      //start point
			heading_type h, //along this heading the next position is determined
			gdtnode &n,        //possible gdtnode that occupies the next position
			gdtedge &e         //possible gdtedge that occupies the next position
			);
		
		// ---------------------------
		// Methods for dynamic drawing
		// ---------------------------




		
		// Steps of the  attach_vertex dynamic orthogonal algorithm.
		//
			void
		DD_explore_eagle
			(
			_solution &tmp, //temporary solution
			gdtnode n,         //start gdtnode of the eagle
			heading_type h  //heading of the eagle to explore
			);
			
			gdtnode
			//dime_orth_plan_undi_graph::
		DD_build_solution
			(
			const _solution &opt, //optimal solution
			gdtnode n                //gdtnode on which the new vertex must be attached
			);
			
			gdtnode
		DD_attach_straigth_vertex_from_free_direction //a direction is free when there is no gdtedge or a dummy gdtedge
			(
			gdtnode n,         //gdtnode on which the new vertex must be attached
			heading_type h  //heading of the new vertex respect to n
			);
		
		// Steps of the  new_edge dynamic orthogonal algorithm.
		//
		void DD_verify_preliminary_conditions                                       (DD_struct&);	//STEP 1
		void DD_init_mappings                                                       (DD_struct&);	//STEP 2
		void DD_create_map                                                          (DD_struct&);	//STEP 3
		void 	DD_create_map_subnet_for_each_edge                                  (DD_struct&);	//STEP 3.1
		void 	DD_create_map_subnet_for_each_crossable_or_terminal_node            (DD_struct&);	//STEP 3.2
		void 		DD_create_map_nodes_around_each_crossable_or_terminal_node  (DD_struct&);	//STEP 3.2.1
		void 		DD_complete_map_subnet_for_each_crossable_node              (DD_struct&);	//STEP 3.2.2
		void 		DD_complete_map_subnet_for_both_terminal_nodes              (DD_struct&);	//STEP 3.2.3
		void 	DD_connect_map_subnets                                              (DD_struct&);	//STEP 3.3
		void 		DD_connect_map_subnets_for_edges_among_them                 (DD_struct&);	//STEP 3.3.1
		void 		DD_connect_map_subnets_for_nodes_among_them                 (DD_struct&);	//STEP 3.3.2
		void 		DD_connect_map_subnets_for_nodes_with_map_subnets_for_edges (DD_struct&);	//STEP 3.3.3
		void    DD_complete_map_subnet_with_particularity_of_terminal_nodes         (DD_struct&);	//STEP 3.4   added for expansion to >4plan
		void DD_find_shortest_path_through_the_map                                  (DD_struct&);	//STEP 4
		void DD_build_dime_edge_path                                                (DD_struct&);	//STEP 5

		// Utility functions,
		// and subroutines of the algorithm steps

		void DD_print_map (DD_struct&, std::string = "map.txt");
		
		bool DD_node_can_be_crossed (gdtnode v);
		bool DD_from_this_terminal_the_other_one_is_along_heading(gdtnode first_term, DD_struct& DD, heading_type h);
		int  DD_number_of_real_edges_around_node (gdtnode v);
		
		bool edge_belongs_to_node (gdtedge e, gdtnode v);
		
		// For step 3.3.x

			void
		DD_create_map_edges_between_start_node_of_border_steps

			(
			border_step  bs1,  // border_step whose start-gdtnode v1 owns the first map_node to be connected 
 			heading_type hmn1, // heading determining which map-gdtnode must be connected, among the map-nodes associated to v1
			border_step  bs2,  // border_step whose start-gdtnode v2 owns the second map_node to be connected
			heading_type hmn2, // heading determining which map-gdtnode must be connected, among the map-nodes associated to v2
			DD_struct&   DD
			);

			void
		DD_create_map_edges_between_border_steps
			(
			border_step bs1,
			border_step bs2,
			DD_struct&  DD
			);
		
			void
		DD_init_map_node_set
			(
			map_node_set& mns,
			gdtnode mn1,
			gdtnode mn2,
			gdtnode mn3,
			gdtnode mn4,
			heading_type hmn1
			);

			void
		DD_get_map_node_set
			(
			map_node_set mns,
			gdtnode& mn1,
			gdtnode& mn2,
			gdtnode& mn3,
			gdtnode& mn4,
			heading_type hmn1
			);
		
			gdtnode&
		DD_node_of_map_node_set_with_heading
			(
			map_node_set& mns,
			heading_type hmn
			);

		
		// For step 3.4
		
			heading_type
		DD_heading_of_edge_with_max_thickness_around_bend_node
			(
			gdtnode n
			);
			
			void
		DD_complete_map_subnet_for_terminal_node
			(
			DD_struct& DD,
			gdtnode tn,
			DD_terminal_node_type tnt
			);	//STEP 3.4.1

			bool
		DD_node_ends_a_terminal_line
			(
			gdtnode n,
			heading_type h
			);	//STEP 3.4.1.0
			
			gdt::gdtlist<gdtedge>
		DD_edge_sequence_along_terminal_line
			(
			DD_struct& DD,
			gdtnode tn,
			heading_type h,
			gdtedge& eb, //this gdtedge is the eagle boundary gdtedge if in the research of the sequence is detected
			bool& cross_last_node
			);	//STEP 3.4.1.1
			
			gdtedge
		DD_detect_the_eagle_boundary_edge
			(
			DD_struct& DD,
			gdtnode n,
			gdtedge e
			);
			
			void
		DD_complete_map_subnet_of_edge
			(
			DD_struct& DD,
			gdtedge e,
			heading_type h,
			DD_terminal_node_type tnt,
			DD_edge_completament_type ect
			);	//STEP 3.4.1.2
			
			void
		DD_complete_map_subnet_of_dime_node
			(
			DD_struct& DD,
			gdtnode n,
			heading_type h,
			DD_terminal_node_type tnt
			);	//STEP 3.4.1.3
			
			void
		DD_connect_map_subnets_of_node_and_edge
			(
			DD_struct& DD,
			gdtnode n,
			gdtedge e,
			heading_type h,
			DD_terminal_node_type tnt
			);	//STEP 3.4.1.4


		
		// For step 5
		
			void
		DD_increase_thickness_of_border_step
			(
			border_step bs
			);

			angle_type
		DD_angle_of_bends_of_map_edge
			(
			gdtedge me, // map gdtedge
			DD_struct& DD
			);

			angle_type
		DD_angle_of_bends_of_map_edges_before_list_item_starting_from_node
			(
			gdt::list_item start_li, // map gdtedge
			gdtnode n, // dime gdtnode
			DD_struct& DD
			);
	
		

 			
 			
 			
 			/**
 			The specified gdtedge is deleted from graph.
 			If an isolate gdtnode is generated, it is deleted too.
 			*/
 			
 			face
 		DD_delete_edge
 			(
 			gdtedge e
 			);
 			
 			
 			
 			/**
 			The specified gdtnode is deleted from graph.
 			If isolate nodes are generated, they are deleted too.
 			*/
 			
 			face
 		DD_delete_node
 			(
 			gdtnode n
 			);
 			
 			
 			/*
 			// PRECONDITION:
 			//  The gdtnode n must have two and only edges and
 			//  these edges must have the opposite nodes respect to n
 			//  on the same horizontal or vertical line.
 			//  This is the tipical situation of an gdtedge splitted by a gdtnode n
 			//  with orth::new_node (the orth class not know the headings)
 			//
 			*/
 			
 			void
 		set_initial_heading_of_edges_splitted_by_node
 			(
 			gdtnode n
 			);
 			
 			


 		// Auxiliars of the  delete_node dynamic orthogonal algorithm.
 		//
 			bool
 		the_graph_will_be_still_connected_after_deletion_of_node
 			(
 			gdtnode n
 			);
 			
 			/**
 			// PRECONDITION:
 			//  gdtedge e must be real
 			//  gdtedge e must have as extremal at least one real gdtnode
 			//
 			*/
 			gdtedge
 		DD_find_first_partial_edge_with_unit_thick_on_edge
 			(
 			gdtedge e
 			);
 			
 			
 			/**
 			// PRECONDITION:
 			//  gdtedge e must belong to gdtnode n
 			*/
 			face
 		DD_delete_eagle_starting_from_node_along_edge
 			(
 			gdtnode to_be_deleted,
 			gdtedge e,
 			gdtnode connectivity_assurer
 			);
 		
 			gdtnode
 		get_connectivity_assurer_node_when_deleting_node
 			(
 			gdtnode to_be_deleted
 			);
 			
 			void
 		delete_connectivity_assurer_eagle_around_node
 			(
 			gdtnode connectivity_assurer,
 			gdtnode to_be_deleted
 			);
 			
 		// Auxiliars of the  delete_edge dynamic orthogonal algorithm.
 		//
 			bool
 		the_graph_will_be_still_connected_after_deletion_of_edge
 			(
 			gdtedge e
 			);
 			
 			face
 		DD_delete_edge_without_rectangularization
 			(
 			gdtedge e
 			);
 			
 			/*
 			void
 		delete_all_dummy_edges_around_node
 			(
 			gdtnode n
 			);
 			*/
 			
 			void
 		delete_all_dummy_paths_around_node
 			(
 			gdtnode n
 			);
 			
 			void
 		delete_all_dummy_paths_and_nodes_around_node
 			(
 			gdtnode n
 			);
 			
 			gdt::gdtlist<gdtedge>
 		complete_path_from_node_of_edge
 			(
 			gdtnode n,
 			gdtedge e
 			);
 			
 			void
 		DD_decrease_thickness_of_border_step
 			(
 			border_step bs
 			);
 
			
			/**
			return the edges of the path between two real nodes that include the real gdtedge e
			*/

			gdt::gdtlist<gdtedge>
		find_edges_path_corresponding_to_unit_thick_real_edge
			(
			gdtedge e
			);

		



		// ------------------------


	private:
		void position_bottom_left_corners_and_incid_edges
			(
			const gdt::gdtnode_array<int>& w,
			const gdt::gdtnode_array<int>& h,
			      gdt::gdtnode_array<gdt::gdtpoint>& pos_bottom_left,
			      gdt::gdtnode_array<pos_incid_edges>& pie
			) const;					// see below..


		       // ---------------------------------------------------------------
		       // compute the position of the bottom-left corner of each vertex
		       // "v" in order to make free space for expanding "v" into a
		       // super-gdtnode with dimensions (w[v] x h[v]).
		       // All the edges will leave the corresponding side at the center.
		       // Also, for each gdtnode "v", the structure pie[v] stores the new
		       // relative positions of the edges incident with "v" in the
		       // four directions (north, south, east, west). The relative
		       // positions are expressed with respect to the bottom-left
		       // corner of the new super-gdtnode.
		       // ---------------------------------------------------------------


		void position_bottom_left_corners_and_incid_edges_centered
			(
			const gdt::gdtnode_array<int>& w,
			const gdt::gdtnode_array<int>& h,
			      gdt::gdtnode_array<gdt::gdtpoint>& pos_bottom_left,
			      gdt::gdtnode_array<pos_incid_edges>& pie
			) const;					// see below..


		       // ---------------------------------------------------------------
		       // compute the position of the bottom-left corner of each vertex
		       // "v" in order to make free space for expanding "v" into a
		       // super-gdtnode with dimensions (w[v] x h[v]).
		       // Also, for each gdtnode "v", the structure pie[v] stores the new
		       // relative positions of the edges incident with "v" in the
		       // four directions (north, south, east, west). The relative
		       // positions are expressed with respect to the bottom-left
		       // corner of the new super-gdtnode.
		       // ---------------------------------------------------------------





		void position_bottom_left_corners_and_incid_edges_with_pins
			(
			const gdt::gdtnode_array<int>& w,
			const gdt::gdtnode_array<int>& h,
			      gdt::gdtnode_array<gdt::gdtpoint>& pos_bottom_left,
			      gdt::gdtnode_array<pos_incid_edges>& pie
			) const;					// see below..


		       // ---------------------------------------------------------------
		       // compute the position of the bottom-left corner of each vertex
		       // "v" according to the constraints expressed by "pos_edge_on_node"
		       // (see method compute_dime_with_expanded_nodes).
		       // Also, for each gdtnode "v", the structure pie[v] stores the new
		       // relative positions of the edges incident with "v" in the
		       // four directions (north, south, east, west). The relative
		       // positions are expressed with respect to the bottom-left
		       // corner of the new super-gdtnode.
		       // ---------------------------------------------------------------



		void expand_nodes_into_super_nodes
			(
			const dime_orth_plan_undi_graph& dopug,
			const gdt::gdtnode_array<int>& w,
			const gdt::gdtnode_array<int>& h,
		 	const gdt::gdtnode_array<gdt::gdtpoint>& pos_bottom_left,
			const gdt::gdtnode_array<pos_incid_edges>& pie,
			      gdt::gdtnode_map<gdtnode>& super_node
			);				// see below..

			
		       // ---------------------------------------------------------------
		       // expand nodes into super-nodes, according to the dimensions 
		       // "w,h" of each gdtnode, and by using the informations stored in 
		       // the "pie" data structure, to correctly calculate the 
		       // positions of the edges incident with the new super-nodes.
		       // The mapping "super-gdtnode" stores which is the representative 
		       // gdtnode (super-gdtnode) in the current graph that corresponds to 
		       // each gdtnode of dopug. 
		       // PRECONDITIONS : the current graph is a copy of dopug, and it 
		       //                 has been applied the method
		       //                 "position_bottom_left_corners_and_incid_edges".
		       // ---------------------------------------------------------------
	
	
                // ### Begin import from GDT fork of W. Didimo on June, 18 2004
		void expand_nodes_into_super_nodes_with_pins 
			(
			const dime_orth_plan_undi_graph& dopug,
			const gdt::gdtnode_array<int>& w,
			const gdt::gdtnode_array<int>& h,
		 	const gdt::gdtnode_array<gdt::gdtpoint>&  pos_bottom_left, 
			const gdt::gdtnode_array<pos_incid_edges>& pie,
			      gdt::gdtnode_map<gdtnode>&  super_node,
			      gdt::gdtnode_map<bool>&  is_removable,
			      gdt::gdtedge_map<bool>&  removable_edge
			);				// see below..
			
			
		       // ---------------------------------------------------------------
		       // expand nodes into super-nodes, according to the dimensions 
		       // "w,h" of each node, and by using the informations stored in 
		       // the "pie" data structure, to correctly calculate the 
		       // positions of the edges incident with the new super-nodes.
		       // The mapping "super-node" stores which is the representative 
		       // node (super-node) in the current graph that corresponds to 
		       // each node of dopug. 
		       // Mapping "is_removable" informs if a dummy node can be removed before compaction.
		       // Mapping "removable_edge" informs if an edge can have zero length after compaction.		       
		       // PRECONDITIONS : the current graph is a copy of dopug, and it 
		       //                 has been applied the method
		       //                 "position_bottom_left_corners_and_incid_edges".
		       // ---------------------------------------------------------------
                // ### End import
		
		
		void expand_node_into_super_node 
			(
			gdtnode 	v, 
			int  		width, 
			int  		height, 
			int  		bl_x, 
			int  		bl_y, 
			pos_incid_edges pie_v, 
			gdt::gdtlist<gdtnode>&     N
			);				// see below..
		
		
		       // -----------------------------------------------------------------
		       // expand the gdtnode "v" into a super-gdtnode of width "width" and
		       // height "height". The super-gdtnode is constructed as follows:
		       // 
		       // 	1 - the bottom-left gdtnode of the super-gdtnode is "v", 
		       //           and ("bl_x","bl_y") specify the new coordinates of 
		       //	    such a gdtnode.
		       // 	
		       //	2 - the two vertical sides of the super nodes are 
		       //           composed by two equal vertical chains, each one
		       //	    containing a number of nodes equal to ("height"+1).
		       //	    Two consecutive nodes of any chain have distance one.
		       // 
		       //	3 - the two horizontal sides of the super nodes are 
		       //	    composed by two equal horizontal chains, each one
		       //	    containing a number of nodes equal to ("width"+1).
		       //	    Two consecutive nodes of any chain have a distance one.
		       //
		       //       4 - the edges previously connected to "v" are attached to
		       //	    the nodes of the super-gdtnode according to the relative 
		       //	    position specified by pie_v.
		       //
		       // All the nodes of the super-gdtnode are returned in the list "N".
		       // WARNINGS: "N" is initialized to empty in the method.
		       // -----------------------------------------------------------------
		
                // ### Begin import from GDT fork of W. Didimo on June, 18 2004
		void expand_node_into_super_node_with_pins 
			(
			gdtnode 	v, 
			int  		width, 
			int  		height, 
			int  		bl_x, 
			int  		bl_y, 
			pos_incid_edges pie_v, 
			gdt::gdtlist<gdtnode>&  N,
			gdt::gdtnode_map<bool>& is_removable,
			gdt::gdtedge_map<bool>& removable_edge
			);				// see below..
		
		
		       // -----------------------------------------------------------------
		       // expand the node "v" into a super-node of width "width" and
		       // height "height". The super-node is constructed as follows:
		       // 
		       // 	1 - the bottom-left node of the super-node is "v", 
		       //           and ("bl_x","bl_y") specify the new coordinates of 
		       //	    such a node.
		       // 	
		       //	2 - the two vertical sides of the super nodes are 
		       //           composed by two equal vertical chains, each one
		       //	    containing a number of nodes equal to ("height"+1).
		       //	    Two consecutive nodes of any chain have distance one.
		       // 
		       //	3 - the two horizontal sides of the super nodes are 
		       //	    composed by two equal horizontal chains, each one
		       //	    containing a number of nodes equal to ("width"+1).
		       //	    Two consecutive nodes of any chain have a distance one.
		       //
		       //       4 - the edges previously connected to "v" are attached to
		       //	    the nodes of the super-node according to the relative 
		       //	    position specified by pie_v.
		       //
		       //       5 - Mapping "is_removable" inform if a dummy node in the super-node
		       //	    can be removed before compaction. 
		       //           Mapping "removable_edge" informs if an edge can have zero 
		       //	    length after compaction.
		       //
		       // All the nodes of the super-node are returned in the list "N".
		       // WARNINGS: "N" is initialized to empty in the method.
		       // -----------------------------------------------------------------
	        // ### End import

		
		
		void expand_node_into_chain 
			(
			gdtnode v, 
			int length, 
			heading_type dir, 
			gdt::gdtlist<gdtnode>& N
			);				// see below..

			// ------------------------------------------------------------------
			// expand the gdtnode "v" into a chain in the specified direction "dir",
			// where "dir" should be in the set {north, south, east, west}. 
			// The chain has a number of edges equal to "length", and a number of
			// nodes equal to "length"+1 ("v" included). 
			// Two consecutive nodes of the chain have distance one.
			// "N" is the ordered list of the nodes added (not including "v").
			// WARNINGS: "N" is not initialized to empty in the method.
			// ------------------------------------------------------------------
			
		
		
		gdtedge change_node_of_edge_with_node 
			(
			gdtnode v1, 
			gdtedge e1, 
			gdtnode v2
			);				// see below..
			
			// -------------------------------------------------------------------
			// Suppose that "e1=(v1,w1)". This method creates a new gdtedge 
			// "e = (v2,w1)", that has the same identifier, direction, constraints
			// and markers as "e1".
			// -------------------------------------------------------------------
		
		
		void uncompress_edges 
			(
			const dime_orth_plan_undi_graph&   dopug,
			const gdt::gdtnode_array<int>&             w,
			const gdt::gdtnode_array<int>&             h,
			const gdt::gdtnode_array<pos_incid_edges>& pie,
			      gdt::gdtnode_map<gdtnode>&		   super_node
			);				// see below..
			
			// --------------------------------------------------------------------
			// uncompress all the edges as much as possible. This means that, for
			// each side of a super-gdtnode an evaluation of the free space is done,
			// and the compressed-edges are uncompressed until is not more possible
			// attaching an gdtedge to a vertex of the super-gdtnode.
			// --------------------------------------------------------------------

                // ### Begin import from GDT fork of W. Didimo on June, 18 2004
		void uncompress_edges_with_pins 
			(
			const dime_orth_plan_undi_graph&   dopug,
			const gdt::gdtnode_array<int>&             w,
			const gdt::gdtnode_array<int>&             h,
			const gdt::gdtnode_array<pos_incid_edges>& pie,
			      gdt::gdtnode_map<gdtnode>&           super_node,
			      gdt::gdtnode_map<bool>&		   is_removable,
			      gdt::gdtedge_map<bool>&		   removable_edge
		);				// see below..
                // ### End import
			
			// --------------------------------------------------------------------
			// uncompress all the edges according to constraints expressed by 
			// "pos_edge_on_node" parameter. 
			// --------------------------------------------------------------------
			
			
		gdt::gdtlist<gdtnode> expand_chain 
			(
			gdtnode v,
			slope_type dir,
			int length,
			gdt::gdtlist<gdtedge>& zero_edges
			);				// see below..
			
			// --------------------------------------------------------------------
			// expand the horizontal(vertical) chain that has "v" as the most left 
			// (bottom) gdtnode, and length "length", into a box with height (width) 0. 
			// The two edges that have zero length are appended to the list 
			// "zero_edges". Return the nodes added.
			// PRECONDITIONS: the gdtnode "v" is the start gdtnode of a chain
			// --------------------------------------------------------------------
			
			
		gdt::gdtlist<gdtnode> corners_of_super_nodes 
			(
			const dime_orth_plan_undi_graph& dopug,
			const gdt::gdtnode_map<gdtnode>&		 super_node,
			const gdt::gdtnode_array<int>&		 w,
			const gdt::gdtnode_array<int>&		 h
			) const;                         // return the corners of all super-nodes.
		
		
		
		gdtedge move_edge_close_to_corner_of_super_node
			(
			gdtedge e, 
			gdtnode v,
			gdt::gdtnode_map<gdtnode>& super_node,
			gdt::gdtedge_map<bool>& is_zero_edge
			);				// to comment
		
		
		void construct_dime_orth_plan_undi_graph 	// called by each constructor from the undi_graph class
			(
			const undi_graph& ug, 
			algorithm_type alg,
			int& num_irr_faces
			);


		void construct_dime_orth_plan_undi_graph 	// called by each constructor from the plan_undi_graph class
			(
			const plan_undi_graph& pug, 
			algorithm_type alg,
			int& numm_irr_faces
			);
		
		
		void construct_dime_orth_plan_undi_graph 	// called by each constructor from the orth_plan_undi_graph
			(
			const orth_plan_undi_graph& opug, 
			algorithm_type alg,
			heading_type dir,
			int& num_irr_faces
			);
	
		
		
	public:

		/*
		CATEGORY constructors_destructors
		Constructors and Operators.
		*/
			
			/**
			Empty constructor.
			*/

		dime_orth_plan_undi_graph
			(
			);									
			
			/*
			Destructor.	
			*/
	
		~dime_orth_plan_undi_graph 
			(
			);
			

			/**
			Constructor from the undi_graph class, 
			applying the algorithm_type for compaction.
			*/

		dime_orth_plan_undi_graph 
			(
			const undi_graph& ug, 
			algorithm_type alg = SLOW_COMPACTION
			);		
										

			/**
			Constructor from the undi_graph class, 
			applying the algorithm_type for compaction.
			num_irr_faces is the number of irregular faces 
			if an algorithm based on regularization is chosen
			*/

		dime_orth_plan_undi_graph 
			(
			const undi_graph& ug, 
			algorithm_type alg,
			int& num_irr_faces
			);
			
			/**
			Constructor from the plan_undi_graph class,
			applying the algorithm_type for compaction.
			*/
		
		dime_orth_plan_undi_graph 
			(
			const plan_undi_graph& pug, 
			algorithm_type alg = SLOW_COMPACTION
			);		
		

			/**
			Constructor from the plan_undi_graph class,
			applying the algorithm_type for compaction.
			num_irr_faces is the number of irregular faces 
			if an algorithm based on regularization is chosen
			*/
		
		dime_orth_plan_undi_graph 
			(
			const plan_undi_graph& pug, 
			algorithm_type alg,
			int& numm_irr_faces
			);		


			/**
			Constructor from the orth_plan_undi_graph class, 
			applying the algorithm_type for compaction.
			*/
		
		dime_orth_plan_undi_graph 
			(
			const orth_plan_undi_graph& opug, 
			algorithm_type alg = SLOW_COMPACTION
			);
			
			
			/**
			Constructor from the orth_plan_undi_graph class, 
			applying the algorithm_type for compaction.
			"dir" specifies the initial direction fro the reference
			border step. 
			*/
		
		dime_orth_plan_undi_graph 
			(
			const orth_plan_undi_graph& opug, 
			algorithm_type alg,
			heading_type dir
			);


			/**
			Constructor from the orth_plan_undi_graph class, 
			applying the algorithm_type for compaction.
			"num_irr_faces" is the number of irregular faces 
			if an algorithm based on regularization is chosen.
			"dir" specifies the initial direction fro the reference
			border step.
			*/
		
		dime_orth_plan_undi_graph 
			(
			const orth_plan_undi_graph& opug, 
			algorithm_type alg,
			heading_type dir,
			int& num_irr_faces
			);
			
			
			/**
			Copy constructor.
			*/
		
		dime_orth_plan_undi_graph 
			(
			const dime_orth_plan_undi_graph& dopug
			);					
		
		/*
		CATEGORY operators
		Operators.
		*/

			/**
			Equality operator from undi_graph_class.
			*/

			dime_orth_plan_undi_graph& 
		operator = 
			(
			const undi_graph& ug
			);					

			/**
			Equality operator from plan_undi_graph class.
			*/
		
			dime_orth_plan_undi_graph&
		operator = 
			(	
			const plan_undi_graph& pug
			);					

			/**
			Equality operator from orth_plan_undi_graph class.
			*/
		
			dime_orth_plan_undi_graph& 
		operator = 
			(
			const orth_plan_undi_graph& opug
			);				
		
			/**
			Copy equality operator.
			*/

			dime_orth_plan_undi_graph& 
		operator = 
			(
			const dime_orth_plan_undi_graph& dopug
			);			
		
		/*
		CATEGORY access
		Access operations.
		*/		
			
			/**
			Get all private variables.
			*/
						
			void 
		local_get
			(
			bool& p1,
			heading_type& p2,
			gdt::gdtnode_map<struct_dime_node_info>*& p3,
			gdt::gdtedge_map<struct_dime_edge_info>*& p4,
			gdt::gdtmap<border_step,struct_dime_border_step_info>*& p5
			); 								
			
			/**
			Return the initial heading of border_step.		
			*/
			
			heading_type	
		initial_heading_of_border_step  
			(
			border_step s
			) const; 					
			
			/**
			Return the heading after walking on border step.
			*/
			
			heading_type	
		heading_after_border_step       
			(
			border_step s
			) const;		
		
			static
			/**
			Static method.
			Return the heading obtained from heading_type after
			a clockwise angle_type rotation.
			*/
			heading_type	
		heading_after_rotation             
			(
			heading_type d,
			angle_type a
			);   
			

			/**
			Return heading_after_rotation (heading_type, angle_type).
			*/
			
			heading_type	
		heading_after_rotation_around_bend 
			(
			heading_type d, 
			angle_type a
			) const;		
			
			/**
			Return heading_after_rotation (heading_type, _360).	
			*/
			
			heading_type	
		heading_after_inversion            
			(
			heading_type d
			) const;		 
			
			/**
			Return the heading obtained from heading_type after a bend_type.
			*/
			
			heading_type	
		heading_after_bend                 
			(
			heading_type d, 
			bend_type b
			) const;		 
		
			/**
			Return the heading obtained from heading_type after the 
			gdt::gdtlist<bend_type> sequence.
			*/
		
			heading_type	
		heading_after_bend_sequence        
			(
			heading_type d, 
			gdt::gdtlist<bend_type> bl
			) const;		
			
			/**
			Return a minimal list of bend_types required to change
			from first heading_type to the second one.
			*/

			gdt::gdtlist<bend_type> 
		minimal_bend_sequence_required_to_change_heading
			(
			heading_type i, 
			heading_type f
			) const;	
			
			/**
			Return the relative position of gdtnode with respect to gdtedge.
			If v is source or target of e then if e is horizontal 
			the position of v is east or west, if e is vertical 
			the position of v is north or south
			If v is not source nor target an error occurs
			*/
			
			heading_type    
		position_of_node_with_respect_to_edge
			(
			gdtnode v,
			gdtedge e
			) const;			
			
			/**
			Return the relative position of gdtedge with respect to gdtnode.
			If v is source or target of e then if e is horizontal 
			the position of e is east or west, if e is vertical 
			the position of e is north or south
			If v is not source nor target an error occurs
			*/
			
			heading_type    
		position_of_edge_with_respect_to_node
			(
			gdtedge e,
			gdtnode v
			) const;			
			
			/**
			Return the relative position of first gdtnode with respect to second gdtnode.
			For this operation there is no necessity that the gdtedge between the two edges exists.
			*/
			
			heading_type 
		position_of_node_with_respect_to_node
			(
			gdtnode v1,
			gdtnode v2
			) const; 

			/**
			Return the gdtedge (possibly NULL_EDGE) that leave gdtnode with heading_type.
			*/
			
			gdtedge 		
		find_edge_leaving_node_with_heading               
			(
			gdtnode v, 
			heading_type d
			) const;		
			
			/**
			return the opposite (possibly NULL_NODE) of the gdtedge returned by 
			find_edge_leaving_node_with_heading(gdtnode, heading_type);
			*/
			
			gdtnode 		
		find_node_reachable_moving_from_node_with_heading
			(
			gdtnode v,
			heading_type d
			) const;

			/**
			return the opposite (possibly NULL_NODE) of the gdtedge returned by
			find_edge_leaving_node_with_heading(gdtnode, heading_type);
			also store in e the gdtedge returned by the find_edge_leaving_node_with_heading(gdtnode, heading_type)
			*/

			gdtnode
		find_node_reachable_moving_from_node_with_heading
			(
			gdtnode v,
			heading_type d,
			gdtedge& e
			) const;


			//---------------------------------------------------------------
			// Added by F. M.
			// as utility in expanding the Dynamic Drawing to >4plan problems
			//---------------------------------------------------------------
			/**
			return the gdtnode (possibly NULL_NODE) encountered walking from the gdtedge e along the specified heading h
			*/


			gdtnode
		find_node_after_edge_along_heading
			(
			gdtedge e,
			heading_type h
			);

			/**
			return the right face, moving from gdtnode,
			of the gdtedge returned by find_edge_leaving_node_with_heading(gdtnode, heading_type);
			*/

			face
		face_visible_from_node_looking_with_heading
			(
			gdtnode v,
			heading_type h
			) const;

			/**
			Return the position of gdtnode.
			*/

			gdt::gdtpoint
		position_of_node
			(
			gdtnode v
			) const;

			/**
			Return the point-list of bend-positions on gdtedge, starting from gdtnode.
			*/

			gdt::gdtlist<gdt::gdtpoint>
		position_of_bends_along_edge
			(
			gdtedge e,
			gdtnode v
			) const;

			/**
			Return the opposite slope of slope_type.
			*/
			
			slope_type 	
		opposite_slope        
			(
			slope_type s
			) const;					
			
			/**
			Return the slope corresponding to heading_type.
			*/
		
			slope_type 	
		slope_along_heading   
			(
			heading_type d
			) const;				
		
			/**
			Return the slope of border_step.
			*/
			
			slope_type 	
		slope_of_border_step  
			(
			border_step s
			) const;			
			
			/**
			Return the slope of gdtedge.
			*/
		
			slope_type 	
		slope_of_edge
			(
			gdtedge e
			) const;					
			
			/**
			Return the ordered maximal chain of nodes reachable from gdtnode,
			moving according to slope_type.
			If slope is horizontal, nodes are ordered from west to east.
			If slope is vertical, nodes are ordered from north to sud.
			*/

			gdt::gdtlist<gdtnode> 	
		nodes_reachable_moving_from_node_with_slope 
			(
			gdtnode v, 
			slope_type slope
			) const;	
			
			
			/**
			Return the ordered maximal chain of nodes reachable from gdtnode,
			moving according to slope_type.
			If slope is horizontal, nodes are ordered from west to east.
			If slope is vertical, nodes are ordered from north to sud.
			Also, with the same ordering, store the edges of the chain in 
			the list edge_chain.
			*/

			gdt::gdtlist<gdtnode> 	
		nodes_reachable_moving_from_node_with_slope 
			(
			gdtnode v, 
			slope_type slope,
			gdt::gdtlist<gdtedge>& edge_chain
			) const;

			
			
			/**
			Return the length of gdtedge.
			*/
			
			int 		
		length_of_edge 
			(
			gdtedge e
			) const;				
			
			/**
			Distance along the border of the face f, moving clockwise from v1 to v2.
			PRECONDITIONS: v1 and v2 lay on the same face f
			*/
			
			int 		
		border_distance 
			(
			gdtnode v1, 
			gdtnode v2, 
			face f
			) const;		
			
			/**
			Distance along the border of the face f, moving clockwise from v to e. 
			The length of e is not included.
			PRECONDITIONS: v and e lay on the same face f
			*/
															
			int	        
		border_distance 
			(
			gdtnode v,
			gdtedge e, 
			face f
			) const;			
			
			/**
			Distance along the border of the face f, moving clockwise from e to v.
			The length of e is not included.
			PRECONDITIONS: v and e lay on the same face f
			*/
		
			int 		
		border_distance 
			(
			gdtedge e, 
			gdtnode v, 
			face f
			) const;			
			
			/**
			Distance along the border of the face f, moving clockwise from e1 to e2. 
			The length of e1 and e2 are not included.
			PRECONDITIONS: e1 and e2 lay on the same face f
			*/
		
			int 		
		border_distance 
			(
			gdtedge e1, 
			gdtedge e2, 
			face f
			) const;		
			
			/**
			Distance along the border of the face f, moving clockwise or 
			counter-clockwise from v1 to v2.
			Ordered is true if the min. distance moves from v1 clockwise.
			PRECONDITIONS: v1 and v2 lay on the same face f
			*/
					
			int 		
		min_border_distance 
			(
			gdtnode v1, 
			gdtnode v2, 
			face f, 
			bool& ordered
			) const;		
			
			/**
			Distance along the border of the face f, moving clockwise or 
			counter-clockwise from v along e.
			Ordered is true if the min. distance moves from v clockwise. 
			PRECONDITIONS: v1 and e lay on the same face f
			*/		
		
			int 		
		min_border_distance 
			(
			gdtnode v, 
			gdtedge e, 
			face f, 
			bool& ordered
			) const;				
			
			/**
			Distance along the border of the face f, moving clockwise or 
			counter-clockwise from e1 to e2.
			ordered is true if min. distance moves from e1 clockwise.
			PRECONDITIONS: v1 and v2 lay on the same face f
			*/

			int 		
		min_border_distance 
			(
			gdtedge e1, 
			gdtedge e2, 
			face f, 
			bool& ordered
			) const;	
		
		// -------------------------------------------------------------------------------
		// TO COMMENT!!!!! (COMMENTS WILL BE MADE BY ANTONIO LEONFORTE AND SANDRA FOLLARO)
		// -------------------------------------------------------------------------------
		
			/**
			To be defined.
			*/
		
			int 		
		inline_distance    
			(
			gdtnode v1, 
			gdtnode v2, 
			slope_type slope
			) const;
			
			/**
			To be defined.
			*/
			
			int 		
		inline_distance    
			(
			gdtnode v,
			gdtedge e
			) const;

			/**
			To be defined.
			*/
						
			int 		
		inline_distance    
			(
			gdtedge e1,
			gdtedge e2
			) const;
				
			/**
			To be defined.
			*/
						
			int 		
		traversal_distance 
			(
			gdtnode v1,
			gdtnode v2,
			slope_type slope
			) const;
				
			/**
			To be defined.
			*/
						
			int 		
		traversal_distance 
			(
			gdtnode v,
			gdtedge e
			) const;
			
			/**
			To be defined.
			*/
							
			int 		
		traversal_distance 
			(
			gdtedge e1,
			gdtedge e2
			) const;
		
		// --------------------------------------------------------------------------------
		
			/**
			Return the angle on gdtnode in face.
			*/
			
			angle_type 
		angle_on_node 
			(
			gdtnode v, 
			face f
			) const;
							
			/**
			Return the angle rotation around a bend required to change 
			from the first heading to the second one.
			*/

			angle_type 
		rotation_around_bend_required_to_change_heading 
			(
			heading_type initial, 
			heading_type final
			) const;

			

			static
			/**
			Static method. 
			Return the clockwise angle required to
			switch from the heading "initial" to the heading "final" 
			*/

			angle_type 
		angle_required_to_change_heading 
			(
			heading_type initial, 
			heading_type final
			);
			
			
			/**
			Compute a dime_orth_plan_undi_graph dopug that has
			the same shape as the current dime_orth_plan_undi_graph, 
			but such that each gdtnode "v" is represented by a rectangular
			face with dimension (w[v] x h[v]).
			Finally, for each vertex "v" of "dopug", "super_node[v]" is the gdtnode
                        of the current dime_orth_plan_undi_graph to which "v" belongs. <P>
			PRECONDITIONS: the dime dopug has been linearized, that is it has no bends.  
			*/
			
			void
		compute_dime_with_expanded_nodes 
			(
			gdt::gdtnode_array<int>& w,
			gdt::gdtnode_array<int>& h,
			dime_orth_plan_undi_graph& dopug,
			gdt::gdtnode_map<gdtnode>& super_node
			) const;


			/**
			Compute a dime_orth_plan_undi_graph dopug that has
			the same shape as the current dime_orth_plan_undi_graph,
			but such that each gdtnode "v" is represented by a rectangular
			face with dimension (w[v] x h[v]). All the edges will leave their
			side at the center.
			Finally, for each vertex "v" of "dopug", "super_node[v]" is the gdtnode
                        of the current dime_orth_plan_undi_graph to which "v" belongs. <P>
			PRECONDITIONS: the dime dopug has been linearized, that is it has no bends.
			*/
			
			void
		compute_dime_with_expanded_nodes_and_edges_centered
			(
			gdt::gdtnode_array<int>& w,
			gdt::gdtnode_array<int>& h,
			dime_orth_plan_undi_graph& dopug,
			gdt::gdtnode_map<gdtnode>& super_node
			) const;





			/**
			Compute a dime_orth_plan_undi_graph dopug that has the same shape as 
                        the current one, but such that each gdtnode "v" is represented by a 
                        rectangular face with dimension w[v] x h[v].
			For each vertex "v" of "dopug", "super_node[v]" is the gdtnode
                        of the current dime_orth_plan_undi_graph which "v" belongs to. 
			Values of pin_number fields of border_step structure are used
                        to contraint the distance between the bottom left corner of "v" and 
                        the attachment of edges to "v", according to the convention described
                        in the dime_orth_plan_undi.pin_number_of_border_step() method.<P>
			PRECONDITIONS: the dime dopug has been linearized, that is it has no bends.
			*/

			void
		compute_dime_with_expanded_nodes_and_pins 
			(
			const gdt::gdtnode_array<int>& 		w,
			const gdt::gdtnode_array<int>& 		h,
			dime_orth_plan_undi_graph& 	dopug,
			gdt::gdtnode_map<gdtnode>& 		super_node
			) const;

			
			
			/**
			Return true if the gdtedge is marked:
			  RM3_ADDED_BY_RECTANGULARIZATION
			*/
			
			bool
		edge_is_dummy
			(
			gdtedge
			) const;
			
			
			/**
			Return true if the gdtedge is not dummy
			*/
			
			bool
		edge_is_real
			(
			gdtedge
			) const;
			

			/**
			Return true if the gdtnode is marked:
			  RM3_ADDED_BY_RECTANGULARIZATION or
			  RM3_CROSS_ON_REAL_EDGE          or
			  RM3_REPLACES_A_BEND             or
			  RM3_BEND_NODE
			*/
			
			bool
		node_is_dummy
			(
			gdtnode
			) const;
			
			
			/**
			Return true if the gdtnode is not dummy
			*/
			
			bool
		node_is_real
			(
			gdtnode
			) const;
			
			
			
			/**
			Build two regular upward planar graphs "pug_v" and "pug_h"
		        corresponding to two embedded posets derived from
		        the orthogonal representation. They are obtained by
		        clustering all horizontal and vertical chains respectively.
		        "node_in_pug_v" indicate the gdtnode-cluster in "pug_v" that
		        corresponds to each gdtnode in "this".
		        "node_in_pug_h" indicate the gdtnode-cluster in "pug_h" that
		        corresponds to each gdtnode in "this".
			*/
			
			void 
		build_embedded_posets 
			(
			plan_undi_graph& pug_v, 
			plan_undi_graph& pug_h,
			gdt::gdtnode_map<gdtnode>&  node_in_pug_v,
			gdt::gdtnode_map<gdtnode>&  node_in_pug_h
			); 				
			
			
			
		/*
		CATEGORY update
		Update operations.
		*/
			
			/**
			Create and return a new gdtnode by splitting the 
			gdtedge "e". The position of the new gdtnode will be 
			the middle point of gdtedge "e" if there is free
			space, else the graph is expanded to make free 
			space.
			PRECONDITIONS: gdtedge "e" must have no bends.
			*/
										
                	gdtnode 
                new_node 
                	(
                	gdtedge e, 
                	int new_id = AUTO_ID
                	);				
			
			
			/**
			Create and return a new gdtnode by splitting the
			gdtedge "e". The new gdtnode is put at distance "dist"
			from the gdtnode "v". 
			PRECONDITIONS: - gdtedge "e" has no bends and "v" belongs to "e"
			               - there is free space to put the new gdtnode
			*/
										
                	gdtnode 
                new_node 
                	(
                	gdtedge e,
			gdtnode v,
			int dist,
                	int new_id = AUTO_ID
                	);	
					
			
			/**
			Create and return a new gdtnode, by attaching it to the gdtnode "v".
			A new straight gdtedge is created to connect the new gdtnode and "v". 
			Such gdtedge is incident with "v" in the specified direction "dir",
			and the new gdtnode has distance "dist" from "v".
			"dir" should be in {north, south, west, east}.
			"new_node_id" and "new_edge_id" specify the identifiers of the new
			gdtnode and the new gdtedge, respectively.
        		PRECONDITIONS: there is not any other gdtedge incident with "v" in the
			               specified direction.
			WARNINGS: the method does not check if the new gdtedge intersects other edges.
			*/

        		gdtnode 
		new_node 
			(
			gdtnode v,
			heading_type dir,
			int dist,
			int new_node_id = AUTO_ID,
			int new_edge_id = AUTO_ID
			);          	   			
			


                	/**
			Add a new straight gdtedge between "v" and "w". The two nodes should have either
			the same x coordinate or the same y coordinate otherwise the error handler
			is invoked.
			The new gdtedge is returned.
			PRECONDITIONS: v and w are vertices of the border of f and v != w
			*/
		
			gdtedge 
		new_straight_edge
			(
			gdtnode v, 
			gdtnode w, 
			int new_id=AUTO_ID
			);		
			

			
                	/**
                	Split face f by adding a new gdtedge between v and w. 
			The new gdtedge is returned.
			PRECONDITIONS: v and w are vertices of the border of f and v != w
			*/
		
                	
			gdtedge 
		new_edge 
			(
			gdtnode v, 
			gdtnode w,
			face f, 
			int new_id=AUTO_ID
			);		
			
			/**
			Split face f by adding a new gdtedge between v and w and put it 
			after ev around v and after ew after w.
			The new gdtedge is returned.
			PRECONDITIONS: v and w are vertices of the border of f and v != w
			*/
		 
			gdtedge 
		new_edge 
			(
			gdtnode v, 
			gdtedge ev, 
			gdtnode w, 
			gdtedge ew, 
			int new_id=AUTO_ID
			); 	

			/**
			Delete gdtnode.
			*/                             			

			face 
		del_node       
			(
			gdtnode v
			);
			
			/**
			Delete gdtedge.
			*/

			face 
		del_edge 
			(
			gdtedge e
			);
			
						
			/**
			attach the two nodes "v1" and "v2", by creating
			a chain of nodes and edges between them. Namely,
			if there is a distance equal to 'k' between "v1" and "v2",
			the chain will have 'k' edges and 'k-1' nodes.
			The nodes added are returned in the list "N".
			WARNINGS: "N" is not inizialized by this method
			PRECONDITIONS: "v1" and "v2" are aligned
			*/
			
			void 
		attach_nodes_by_chain
			(
			gdtnode v1, 
			gdtnode v2, 
			gdt::gdtlist<gdtnode>& N
			);


			/**
			Delete the edges incident on "v" and merge them
			in a single gdtedge, which is returned.
			PRECONDITIONS: "v" has degree two.
			*/
				
			gdtedge 
		weld_edges_at_node 
			(
			gdtnode v
			);	
			
			
			/**
			Replace the gdtnode "v" with a bend <P>
			PRECONDITIONS: "v" has degree two <P>
			DEPRECATED: use weld_edges_at_node instead.
			*/
			 
			gdtedge 
		replace_node_with_bend 
			(
			gdtnode v
			); 	 	
		
			/**
			replace a bend with a gdtnode. Namely, the bend is 
			in position "pos" when walking on gdtedge "e" from gdtnode "v".
			Return in a list "N" the new gdtnode, and in a list "E" the
			edges deriving by the splitting of the gdtedge "e".
			PRECONDITIONS: there is a bend in the specified position
			*/
			
			void 
		replace_bend_with_node   
			(	
			gdtedge e, 
			gdtnode v, 
			int pos, 
			gdt::gdtlist<gdtnode>& N, 
			gdt::gdtlist<gdtedge>& E
			);		
		


			/* CATEGORY global_modifiers
			   Global Modifiers
			 */
		
			/**
			Replace all the bends of the gdtedge "e" with nodes. 
			Return in a list "N" the new nodes, and in a list "E" the
			edges deriving by the splitting of the gdtedge "e".
			*/
			
			void 
		replace_bends_with_nodes 
			(
			gdtedge e, 
			gdt::gdtlist<gdtnode>& N, 
			gdt::gdtlist<gdtedge>& E
			);				
		
		
			/**
			Replace all the bends of the border of the face "f" with nodes.  
			Return in a list "N" the new nodes, and in a list "E" the
			edges deriving by the splitting of the edges of "f".
			*/
			
			void
		replace_bends_with_nodes
			(
			face f,
			gdt::gdtlist<gdtnode>& N,
			gdt::gdtlist<gdtedge>& E  
			);		
		
		
			/**
			Replace all the bends of the graph with nodes.
			Return in a list "N" the new nodes, and in a list "E" the
			edges deriving by the splitting of the edges of the graph.
			*/
			
			void
		replace_bends_with_nodes
			(
			gdt::gdtlist<gdtnode>& N,
			gdt::gdtlist<gdtedge>& E
			);		
			

			/**
			Update the coordinates of the nodes and bends of the current
			graph according to the coordinates of nodes and bends of dopug.
			PRECONDITIONS:  the shape and the gdtnode- and gdtedge-identifiers of dopug
					are the same as those of the current graph.
			*/
			
			void 
		update_node_and_bend_positions_according_to 	     
			(
			dime_orth_plan_undi_graph& dopug
			);





			/**
			  
			  This method consider the edges whose slope
			  is equal to "slp" and minimize their length
			  as much as possible, avoiding overlapping.
			  <p>

 			  The user should specify constraints on the
			  length of the edges that are taken into
			  account during the computation: <p>

			  min: is a mapping from the ID of an gdtedge to
			  the <b>minimum</b> value of its length.<p>

			  max: is a mapping from the ID of an gdtedge to
			  the <b>maximum</b> value of its length.<p>
			  
			  cost: is a mapping from the ID of an gdtedge to
			  the cost that multiply the length of the
			  gdtedge.<p>

			  The objective function that is minimized is
			  the sum for each gdtedge of the length
			  multiplied for its cost. <p>

			  <b>Warning</b> No check on the feasibility
			  of the constraints is performed.  If no
			  soution can be found, the 
			  "error_handler" is invoked during the
			  computation and the execution is
			  interrupted.

			  <p><code>min_tieing_dist</code> is the lower
			  bound (in grid units) to the distance
			  between two objects (nodes ore edges) in the
			  result.
			  
			*/

			void
		one_dimensional_tieing ( const gdt::gdtmap<int, int>& min,
					 const gdt::gdtmap<int, int>& max,
					 const gdt::gdtmap<int, int>& cost,
					 slope_type slp,
					 unsigned int min_tieing_dist=1
					 );


			// please move this in a more appropriate place! (pizzo)

			/**
			  This method behave as a
			  one_dimensional_tieing but is able to create
			  "buttons holes" in order to allow appropriate
			  resizing of boxes created with a cycle
			  (expanded nodes).
			  
			  <p><code>extent</code> is a mapping from the
			  id's of the nodes to an integer representing
			  a dimension of the box. That is, if a gdtnode
			  has associated a non zero value the gdtnode is
			  considered to be the left bottom gdtnode of a box deriving from an
			  expanded gdtnode.

			  <p>When <code>slp</code> is horizontal the
			  "button hole" is horizontal, between the two
			  vertical side wall of the box. The "button hole"
			  placed at distance zero from the "floor" of
			  the box.

			*/
			
        		void
		one_dimensional_tieing_for_expanded_nodes 
					   ( 
					   const gdt::gdtmap<int, int>& min,
					   const gdt::gdtmap<int, int>& max,
					   const gdt::gdtmap<int, int>& cost,
					   slope_type slp,
					   const gdt::gdtmap<int, int>& extent,
					   unsigned int min_tieing_dist=1
					   );
	

										
			
		/*
		CATEGORY dynamic_methods
		Dynamic methods.
		*/
		
			/**
			Insert a new gdtedge from gdtnode v1 to gdtnode v2 preserving the 
			previous shape of the orthogonal representation, and 
			considering the cross_, bend_ and length_unit_cost.
			Since it is possible to have crosses, they are replaced
			by dummy_nodes (marked as RM3_CROSS_ON_REAL_EDGE or  
			RM3_CROSS_ON_DUMMY_EDGE)
			*/
			
			void
		DD_dynamic_new_edge
			(
			gdtnode v1,
			gdtnode v2,
			int  cross_cost,
			int  bend_cost,
			int  length_unit_cost
			);
			
			
			
			/**
			A new gdtnode, with a corresponding new gdtedge,
			is attached to the given gdtnode
			preserving the previous shape of the orthogonal representation.
			*/
			
			gdtnode
		DD_dynamic_attach_vertex
			(
			gdtnode n
			);
		
				
		// --------------------------------------------------------------------------
		// The following methods are used by draw_undi_graph class, but it
		// should be not public here. Future best solution is to move it into the 
		// draw_undi_graph files. 
		// PLEASE, DO NOT LIST THIS METHOD IN THE PUBLIC METHODS !!!
		// --------------------------------------------------------------------------
		
			void 
		remove_all_dummy_nodes_and_edges
			(
			); 	// remove all dummy nodes and edges
		
		// --------------------------------------------------------------------------
		
			/**
			Remove all nodes and edges.
			*/
							
 			void 
 		clear
 			(
 			);			
 			
 			/**
 		      Make *this with the undi_graph ug internal variables 
			and cut these variables from ug. 
			This method is used to make an undi_graph as a dime_orth_plan_undi_graph,
			manteining the same internal variables.
			WARNING: ug has an empty body after this method is applied
			PRECONDITIONS: ug must be connected and with at least one gdtnode 
			*/
										
			void 
		steal_from 
			(
			undi_graph& ug
			); 			
			
			/**
			Make *this with the plan_undi_graph pug internal variables 
			and cut these variables from pug. 
			This method is used to make a plan_undi_graph as a dime_orth_plan_undi_graph,
			manteining the same internal variables.
			WARNING: pug has an empty body after this method is applied
			*/
		
			void 
		steal_from 
			(
			plan_undi_graph& pug
			);		
			
			/**
			Make *this with the orth_plan_undi_graph opug internal variables 
			and cut these variables from opug. 
			This method is used to make a plan_undi_graph as a dime_orth_plan_undi_graph,
			manteining the same internal variables.
			WARNING: opug has an empty body after this method is applied
			*/
		
			void 
		steal_from 
			(
			orth_plan_undi_graph& opug
			);	
			
			/**
			Copy all private variables of orth_plan_undi_graph in *this.
			*/
		
 			void 
 		mirror 
 			(
 			dime_orth_plan_undi_graph& dopug
 			);	
 			
 			/**
 		 	Cut all private variables in *this.
 		 	*/
 		 	
 			void 
 		forget 
 			(
 			);                    		

		
		/*
		CATEGORY io_operations
		Input / output operations.
		*/ 
		
			/**
			Print the graph on ostream os; print_mode can be BY_NODES, BY_EDGE, BY_FACES, COMPLETE.
			*/
		
			void 
		print 
			(
			print_mode mode=BY_FACES,
			std::ostream& os=std::cout
			) const;		
			
			/**
			Print gdtnode on ostream os.
			*/
				
			void 
		print 
			(
			gdtnode v, 
			std::ostream& os = std::cout
			) const;				
		
			/**
			Print gdtedge on ostream os. If path=true, print also all bend-positions.
			*/
				
			void 
		print 
			(
			gdtedge e, 
			bool path = false, 
			std::ostream& os = std::cout
			) const;		 
		
			/**
			Print face on ostream os.
			*/
				
			void 
		print 
			(
			face f, 
			std::ostream& os = std::cout
			) const;				
	};


#endif
