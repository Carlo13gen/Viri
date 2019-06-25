/*******************************************************************************
+
+  rm3_SPQR_tree.h
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


#ifndef RM3_SPQR_TREE_H
#define RM3_SPQR_TREE_H


#include <GDT/gdtmap.h>
#include <GDT/gdtarray.h>
#include <GDT/gdtnode_matrix.h>
#include <GDT/gdt_graph_array.h>

#include <GDT/rm3_tree.h>
#include <GDT/rm3_plan_undi_graph.h>

//-----------------------------------------------------------------------------
// SPQR_tree: base class for SPQR trees.
//
// W.Didimo e A.Leonforte (1996)
//-----------------------------------------------------------------------------

// ---------------------------------------
// This file contains three classes.
// Comments can be found just before each
// one of them.
// ---------------------------------------


// -----------------------------------------
// enumerate types for split_component class
// -----------------------------------------

	/*
	GLOBALTYPE split_component_edge_type
	Type of gdtedge in a split_component object.
	*/

	typedef enum
	{
	REAL,
	VIRTUAL
	}
split_component_edge_type;		


	/*
	GLOBALTYPE split_component_type
	Type of split_component.
	*/
	
	typedef enum
	{
	POLYGON,
	MAXIMAL_BOND,
	TRICONNECTED,
	NOT_COMPLETE
	}
split_component_type;			
	
	
// -----------------------------------
// enumerate types for SPQR_tree class
// -----------------------------------
	
	/*
	GLOBALTYPE SPQR_node_type
	Type of gdtnode in an SPQR_tree object.	
	*/
	
	typedef enum
	{
	S_NODE,
	P_NODE,
	Q_NODE,
	R_NODE
	}
SPQR_node_type;						

	/*
	GLOBALTYPE rotation_type
	Useful rotation type.	
	*/
	
	typedef enum
	{
	CLOCKWISE,
	COUNTER_CLOCKWISE
	}
rotation_type;				


// ---------------------------------------------------
// useful types for B&B algorithms based on SPQR_trees
// ---------------------------------------------------


	typedef gdt::gdtlist<int> 
BB_assignment;			


	const int 
BB_NULL_STATUS = -1;

	const int
BB_NULL_EDGE   = -1;

	const int 
BB_CURRENT = 0,
BB_RANDOM  = 1,
BB_NON_REP = 0, 
BB_INF = 0;
	

	typedef enum
		{
		BB_BFS,
		BB_DFS
		}
BB_visit_type;

	
	typedef struct
		{
		int  init_ref_edge;     // initial reference gdtedge
		bool all_computations;  // if true, execute all computations, one for each possible choice of reference gdtedge 
		bool text_interface;    // text interface switch (on/off)
		bool rppl;              // replace with path in preprocessing lower bounds
		bool rpdl;              // replace with path in dynamc lower bounds
		int  fub;               // first upper bound computation
		int  sub;  	        // successive upper bounds computations
		int  niu; 	        // number of initial upper bounds
		int  ncs; 	        // number of consecutive steps	
		int  ubf; 	        // computation frequence of upper bound
		} 
BB_options;
	

	const BB_options STANDARD_BB_OPTIONS = 
		    			{
		      			BB_NULL_EDGE, 
					true, 
		      			false, 
		      			true, 
		      			true, 
		      			BB_CURRENT, 
		      			BB_NON_REP, 
		      			25, 
		      			BB_INF, 
		      			0
		      			};

// SPLIT COMPONENTS
// 
// ------------------------------------------------------------------------------------------------
// Here, a split_component object is a biconnected subgraph of a plan_undi_graph (owner graph),
// with possibly virtual edges added. 
// Each virtual gdtedge has a pointer (link) to a corresponding gdtedge in another split component, and
// each real gdtedge has a pointer to the corresponding gdtedge in the owner graph. 
// Also each gdtedge has a pointer to the split component that contains it. 
// In this context a split_component can be one among the following types:
// - POLYGON		: it is a polygon;
// - MAXIMAL_BOND	: it is a maximal series of multiple edges between two nodes;
// - TRICONNECTED	: it is a triconnected graph
// - NOT_COMPLETE	: no of the above cases is verified
// Also, at each step, a split_component contains the list of its separation_pairs.
// ------------------------------------------------------------------------------------------------

class skeleton;	//forward declaration
class split_component; //forward declaration

	/*
	GLOBALTYPE struct_split_node_info	
	Local split_node structure for split_component.
	*/

	class GDT_NONSTANDARD_DECL
struct_split_node_info
	{
	friend class split_component;
	//
	gdtnode	twin_node;		// corresponding gdtnode in the decomposed graph (owner graph)
	};


	/*
	GLOBALTYPE struct_split_edge_info	
	Local split_edge structure for split_component.
	*/




	class GDT_NONSTANDARD_DECL
struct_split_edge_info
	{
	friend class split_component;
	//
	split_component_edge_type type;					// Real or virtual gdtedge.
	gdtedge	  	          twin_edge;				// Edge corresponding in another split component if type = VIRTUAL;
									// if type = REAL, twin_edge = gdtedge into the owner graph.
	split_component*          twin_edge_owner_split_component;	// Pointer to the split_component that contains twin_edge if the gdtedge is
									// virtual, else null pointer.
	};


	/*
	CLASS split_component	
        A split_component object is a biconnected subgraph of a plan_undi_graph (owner graph),
        with possibly virtual edges added. 
        Each virtual gdtedge has a pointer (link) to a corresponding gdtedge in another split component, and
        each real gdtedge has a pointer to the corresponding gdtedge in the owner graph. 
        Also each gdtedge has a pointer to the split component that contains it. 
        In this context a split_component can be one among the following types:
        - POLYGON	: it is a polygon;
        - MAXIMAL_BOND	: it is a maximal series of multiple edges between two nodes;
        - TRICONNECTED	: it is a triconnected graph
        - NOT_COMPLETE	: no of the above cases is verified
        Also, at each step, a split_component contains the list of its separation_pairs.
	*/

	class GDT_NONSTANDARD_DECL
split_component: public plan_undi_graph
	{
	friend class skeleton;
	
	private:
		// -----------------
		// private variables
		// -----------------
		
		plan_undi_graph*		  owner_graph;		// owner graph of the split_component
		split_component_type 		  type;			// type of the split_component
		gdt::gdtnode_map<bool>*			  node_is_present;	// node_is_present[v] = true if the gdtnode v of the owner graph belongs to the split_component:
									// (gdt::gdtnode_map is initializated on owner_graph.nodes_and_edges()).				 	
		gdt::gdtlist<separation_pair>*		  separation_pairs;	// list of all separation pairs of the split component
		 
		gdt::gdtnode_map<struct_split_node_info>* node_info;		// correspondence gdtnode --> split_node-structure
		gdt::gdtedge_map<struct_split_edge_info>* edge_info;		// correspondence gdtedge --> split_edge-structure
		
		// ---------------
		// private methods
		// ---------------
		
		void local_new   ();					// create a new set of pointers for the not-inherited class-part	
		void local_del   ();					// delete all the not-inherited class-part
		void local_renew ();					// utility function : just local_del(), then local_new()
		void local_init  (plan_undi_graph&);			// init the not-inherited class-part
		
		void local_set 						// set all private variables
			(
			plan_undi_graph*, 
			split_component_type, 
			gdt::gdtnode_map<bool>*,
			gdt::gdtlist<separation_pair>*, 
			gdt::gdtnode_map<struct_split_node_info>*,
			gdt::gdtedge_map<struct_split_edge_info>*
			);
				
		void set_owner_graph      (plan_undi_graph& pug);				// set the split_component owner graph with pug pointer
		void set_type 		  (split_component_type t);				// set the split_component type with t
		void set_node_is_present  (gdtnode v, bool flag);					// set gdtnode v as present(true)/no-present(false) (v is a gdtnode of the owner_graph) 
		void set_separation_pairs (const gdt::gdtlist<separation_pair>& sp_list);		// set the split_component separation_pairs list with sp_list
		void set_twin_node	  (gdtnode v, gdtnode v1);					// makes v1 as twin gdtnode of gdtnode v
		void set_twin_edge	  (gdtedge e, gdtedge e1);					// makes e1 as twin gdtedge of gdtedge e
		void set_type_of_edge	  (gdtedge e, split_component_edge_type t);		// set gdtedge type with t
		void set_twin_edge_owner_split_component (gdtedge e, split_component* sc_pointer);	// set twin gdtedge's owner split component of gdtedge e with sc_pointer
		
	protected:
		
		// -------------------------
		// Constructor and Operators
		// -------------------------
		
		split_component ();						// empty constructor
		split_component (const split_component&);			// copy construtor
		split_component& operator= (const split_component&);		// copy equality operator
		
		// ----------------------------------
		// ---------------------------------------------------
		// These methods are needed to hide the corresponding
		// plan_undi_graph public methods.
		// ---------------------------------------------------
	
		gdtnode new_node (gdtedge, int new_id=AUTO_ID);			
		gdtedge new_edge (gdtnode, gdtnode, face, int new_id=AUTO_ID);
		
		void del_node (gdtnode);
		void del_edge (gdtedge);
		
		//
		
		gdt::gdtlist<split_component*> decomposition_in_triconnected_components();	// kernel of method decompose_into_triconnected_components
				
	public:
	
		/*
		CATEGORY constructors_destructors
		Constructors and destructors
		*/

			
			/*
			METHOD ~split_component
			Destructor.
			*/

		~split_component();		
		
			
			/*
			METHOD split_component
			Constructor from the plan_undi_graph class:
			the new split component is initialized with pug plan_undi_graph;
			pug will be made as the owner graph of the split component, and the
			separation pairs of the split component will be all the separation
			pairs of pug. The new split_component has only real edges (i.e. all the edges of pug).
			*/

		split_component 
			(
			plan_undi_graph& pug
			);
			

		/*
		CATEGORY operators
		Operators.
		*/

			
			/*
			METHOD operator=
			Equality operator from the plan_undi_graph class:
			the new split component is initialized with pug plan_undi_graph;
			pug will be made as the owner graph of the split component, and the
			separation pairs of the split component will be all the separation
			pairs of pug. The new split_component has only real edges (i.e. all the edges of pug).
			*/
					
			split_component& 
		operator= 
			(
			plan_undi_graph&
			);	
			
		/*
		CATEGORY access
		Access operations.
		*/
		
			
			/*
			METHOD local_get
			Get all private variables.			
			*/
			
			void 
		local_get					
			(
			plan_undi_graph*&, 
			split_component_type&, 
			gdt::gdtnode_map<bool>*&,
			gdt::gdtlist<separation_pair>*&, 
			gdt::gdtnode_map<struct_split_node_info>*&,
			gdt::gdtedge_map<struct_split_edge_info>*&
			);					
		
			
			/*
			METHOD get_owner_graph
			Return a reference to the owner graph of the split_component.			
			*/
			
			plan_undi_graph&      	   
		get_owner_graph     	   	  	
			(
			)const;	
		
			
			/*
			METHOD get_type
			Return the type of the split_component.
			*/
			
			split_component_type
		get_type	      	   	  	
			(
			)const;	
			
			
			/*
			METHOD get_type_of_edge
			Return the type of gdtedge e of the split_component.
			*/
			
			split_component_edge_type  
		get_type_of_edge	   	  	
			(
			gdtedge e
			)const; 	
			
			
			/*
			METHOD get_twin_edge_owner_split_component  
			Return a pointer to the twin gdtedge's owner split component of gdtedge e
			if e is virtual gdtedge, else return NULL.
			*/			
			
			split_component*	   
		get_twin_edge_owner_split_component  
			(
			gdtedge e
			)const; 

			
			/*
			METHOD getseparation_pairs
			Return the list of separation_pairs of the split_component 
			*/			
																	
			gdt::gdtlist<separation_pair> 	   
		get_separation_pairs	   	  	
			(
			)const;	

			
			/*
			METHOD get_node_is_present
			Return true if gdtnode v of the owner_graph is present in the split component.
			*/			
																		
			bool		      	   
		get_node_is_present 	   	  	
			(
			gdtnode v
			)const;	
			
			
			/*
			METHOD contain_separation_pair
			Return true if sp is a separation pair of the split component.
			*/			
			
			bool			   
		contain_separation_pair	  	
			(
			separation_pair sp
			) const;	

			
			/*
			METHOD get_twin_node
			Return the twin gdtnode of gdtnode v.
			*/			
		
			gdtnode	              	   
		get_twin_node	   	  	
			(
			gdtnode v
			)const;

			
			/*
			METHOD get_twin_edge
			Return the twin gdtedge of gdtedge e.
			*/			
		
			gdtedge		      	   
		get_twin_edge	   	       	
			(
			gdtedge e
			)const;	

			
			/*
			METHOD edge_associated_to_owner_graph_edge
			If the owner_graph gdtedge e is present in the separation_pair return it,
			else return NULL_EDGE.			    
			*/			

			gdtedge			   
		edge_associated_to_owner_graph_edge 	
			(
			gdtedge e
			)const; 
			
		
		/*
		CATEGORY update
		Update operations.
		*/
			
			
			/*
			METHOD clear
			Clear all nodes and edges.
			*/			
		
			void
		clear 
			(
			);						
		
			
			
			/*
			METHOD mirror
			Copy all private variables of SPQR_tree in *this.
			*/			

			void 
		mirror 
			(
			split_component&
			);
			
			
			
			/*
			METHOD forget
			Cut all private variables in *this.
			*/			
						
			void 
		forget 
			(
			);						
		

			
			/*
			METHOD split_on_separation_pair
			Split the current split component into
			two new split components, (*this) and 
			(*sc1_pointer). 
			Return true if the splitting is really
			executed.
			PRECONDITIONS: sp is a separation pair 
			of the current split component. 
			*/			

			bool 
		split_on_separation_pair   
			(
			separation_pair sp, 
			split_component*& sc1_pointer, 
			int new_id = AUTO_ID
			);	
		
			
			/*
			METHOD merge_with_polygon_on_edge 
			Merge the current split component with
			the split component (*sc1_pointer).
			Return true if the merging is really 
			executed.
			PRECONDITION: (*this) and (*sc1_pointer)
			are POLYGON type, and e is
			a virtual gdtedge present in both. 
			*/			
																		
			bool 
		merge_with_polygon_on_edge 
			(
			split_component*& sc1_pointer, 
			gdtedge e
			);
		
			
			/*
			METHOD decompose_into_triconnected_components 
			Compute the decomposition of the split 
			component into its triconnected 
			components,and return the
			list of such split components (that are
			all different from NOT_COMPLETE type).	
			*/			
		 									
			gdt::gdtlist<split_component*> 
		decompose_into_triconnected_components 
			(
			);	
		

		/*
		CATEGORY io_operations
		Input / output operations.
		*/ 
		
			
			/*
			METHOD print 
			Print the graph on ostream os; print_mode can
			be BY_NODES, BY_EDGE, BY_FACES, COMPLETE.
			If split_component_information is true, 
			useful information are printed about the 
			structure of the split_component.
			*/			

			void 
		print 
			(
			print_mode mode = BY_FACES, 
			bool split_component_information = true, 
			std::ostream& os = std::cout
			);	
		
	};
	
	
	
// SKELETON
// A skeleton is a complete split component (triconnected component) with a fixed
// reference gdtedge e=(v,w). The extremal vertices v and w are called "POLES" of the skeleton.
// 



class SPQR_tree;	// forward declaration

	/*
	CLASS skeleton
	A skeleton is a complete split component (triconnected component) with a fixed 
	reference gdtedge e=(v,w). The extremal vertices v and w are called "POLES" of the skeleton.
	*/

	class GDT_NONSTANDARD_DECL
skeleton: public split_component
	{
	friend class SPQR_tree;
	
	private:
		// -----------------
		// private variables
		// -----------------
		
		gdtedge ref_edge;						// reference gdtedge
		
		// ---------------
		// private methods
		// ---------------
		
		void local_new   ();					// create a new set of pointers for the not-inherited class-part	
		void local_del   ();					// delete all the not-inherited class-part
		void local_renew ();					// utility function : just local_del(), then local_new()
		void local_init  (const split_component&, gdtedge);	// init the not-inherited class-part
		
	public:	


		/*
		CATEGORY constructors_destructors
		Constructors and destructors.
		*/

			
			/*
			METHOD ~skeleton 
			Destructor.
			*/

		~skeleton
			(
			);
			
			
			/*
			METHOD skeleton
			Empty constructor.
			*/

		skeleton  
			(
			);
			
			
			/*
			METHOD skeleton
			Constructor from the split_component class:
			make a new skeleton, initializing it with sc 
			and make e as its reference gdtedge.
			PRECONDITIONS: e is an gdtedge of sc.
			*/
			
		skeleton  
			(
			const split_component& sc, 
			gdtedge e
			);
			
					
		/*
		CATEGORY access
		Access operations.
		*/
		
			
			/*
			METHOD get_pole1
			Return the pole 1 of the skeleton.
			*/

			gdtnode 
		get_pole1		
			(
			) const;			
			
			
			
			/*
			METHOD get_pole2
			Return the pole 2 of the skeleton.
			*/
			
			gdtnode 
		get_pole2		
			(
			) const;	

			
			/*
			METHOD get_reference_edge
			Return the reference gdtedge of the skeleton.
			*/
		
			gdtedge 
		get_reference_edge	
			(
			) const;	
		
		
		/*
		CATEGORY update
		Update operations.
		*/
		
			
			/*
			METHOD clear
			Delete all nodes and edges.
			*/

			void 
		clear
			(
			);					

			
			/*
			METHOD steal_from
			Make *this with the split_component sc internal variables 
			and cut these variables from sc. 
			This method is used to make a split_component as a skeleton, manteining the same internal variables.
			WARNING: sc has an empty body after this method is applied
			*/
		
			void 
		steal_from 
			(
			split_component& sc, 
			gdtedge e
			);
			
			
			/*
			METHOD set_reference_edge 
			Set the reference gdtedge with the gdtedge e.
			*/
			
			void 
		set_reference_edge 
			(
			gdtedge e
			);	
		
		/*
		CATEGORY io_operations
		Input / output operations.
		*/ 
		
			
			/*
			METHOD print 
			Print the graph on ostream os; print_mode can
			be BY_NODES, BY_EDGE, BY_FACES, COMPLETE.
			If skeletont_information is true, 
			useful information are printed about the 
			structure of the skeleton.
			*/
		
			void 
		print 
			(
			print_mode mode = BY_FACES, 
			bool skeleton_information = true, 
			std::ostream& os = std::cout
			);		
		
	
	};
	


// -----------------------------------------------------------------------------------------------------
// Classes for SPQR_tree. An SPQR tree represents all embeddings of a plan_undi_graph such that they 
// have a reference gdtedge on the external face. Each gdtnode of the SPQR_tree has an associated skeleton. 
// -----------------------------------------------------------------------------------------------------


	typedef enum
	{
	MSP12,
	MSP21,
	SP,
	NULL_PATH
	}
path_type;


	struct
path_info
	{	
	gdt::gdtlist<gdtedge> msp12;   	// minimum switches path from pole1 to pole2, leaving from pole1
	gdt::gdtlist<gdtedge> msp21;   	// minimum switches path from pole1 to pole2, entering in pole1
	gdt::gdtlist<gdtedge> sp;	   	// shortest path from pole1 to pole2 (undirected)
	int sw12;		// number of switches in path msp12;
	int sw21;		// number of switches in path msp21;
	path_type current_path; // keep a reference to one of the above paths or NULL_PATH;
	};


	
	struct
R_node_info
	{
	rotation_type	rotation;	     // specify the adjacency lists rotation order.
					     // if rotation = COUNTER_CLOCKWISE the order in adjacency lists 
					     // of the skeleton is kept;
					     // if rotation = CLOCKWISE this order is reversed.  
	};


	struct
P_node_info
	{
	gdt::gdtlist<gdtedge>	permutation;	    // specific the edges permutation order starting from the reference gdtedge.
					    // NOTE: the reference gdtedge is not insert into the permutation list; 
					    // moreover, it and the last gdtedge into the permutation list are the edges 
					    // on the external face.
	};


	/*
	GLOBALTYPE struct_SPQR_node_info
	Local-SPQR structure for gdtnode.
	*/

	class GDT_NONSTANDARD_DECL
struct_SPQR_node_info
	{
	friend class SPQR_tree;
	// 
	SPQR_node_type               type;	     // Type SPQR gdtnode
						     // (S = series, P = parallel, Q = single gdtedge, R = rigid).
	skeleton*		     skel;	     // Skeleton pointer. 
						     // NOTE: if type = Q_node, skel = NULL.
	gdt::gdtedge_map<gdtedge>		     tree_edge;      // Each gdtedge of the skeleton has a corresponding gdtedge of the SPQR_tree.	
	gdt::gdtmap<int,R_node_info>	     R_status;	     // An R_node has two status, (*R_status[0]) and (*R_status[1]),
						     // containing all the information on the current embedding of the skeleton.
						     // NOTE: if (type != R_node), R_status = NULL.
	gdt::gdtmap<int,P_node_info>	     P_status;	     // A P_node has (n-1)! status, with n = number of edges into the skeleton.
						     // NOTE: if (type != P_node), P_status = NULL.  
	int 			     maximum_status; // indicate the maximum status of the gdtnode.			
						     
	public:
		struct_SPQR_node_info(skeleton*);						     
							   
	};
	
	
	/*
	GLOBALTYPE struct_SPQR_edge_info
	Local-SPQR structure for gdtedge.
	*/
	
	class GDT_NONSTANDARD_DECL
struct_SPQR_edge_info
	{
	friend class SPQR_tree;
	//
	gdtedge  edge_corr_in_father_skeleton;	// corresponding gdtedge into the skeleton of the father_node.
	};



	class GDT_NONSTANDARD_DECL
SPQR_tree: public tree
	{
	private:
		gdt::gdtnode_map<struct_SPQR_node_info*>* node_info;	
		gdt::gdtedge_map<struct_SPQR_edge_info>*  edge_info;
		bool simpl_node_status;					// simplify for gdtnode-status	
	
		// ---------------
		// private methods
		// ---------------
		
		void local_new   ();
		void local_del   ();
		void local_renew ();
		void local_init  (plan_undi_graph& pug, gdtedge e, bool simpl);

		void set_type_of_node			    (gdtnode v, SPQR_node_type t);		// sets the v node_type with t.
		void set_skeleton_of_node		    (gdtnode v, skeleton* skp);		// sets the v skeleton pointer with skp.
		void set_tree_edge_of_skeleton_edge_in_node (gdtedge es, gdtedge et, gdtnode v);		// sets the tree's gdtedge corresponding to 
												// skeleton's gdtedge e in gdtnode v with te.
		void set_skeleton_edge_of_tree_edge	    (gdtedge et, gdtedge es);			// sets with e the skeleton's gdtedge corresponding to
												// tree's gdtedge et in its father gdtnode.
														
		void create_node_status (gdtnode v);						// create all gdtnode status, i.e. make the mapping
												// int --> R/P_node_info.
												// if semplificate==true, the real edges with same direction 
												// are grouped into an equivalence class

		
		
		SPQR_node_type	from_split_component_type_to_SPQR_node_type  (split_component_type t) const; 	// -------------------------------------------
														// return the SPQR_node_type corresponding to 
														// split_component_type.
														// -------------------------------------------
			

		std::string	from_SPQR_node_type_to_string (SPQR_node_type) const;		// return 'S', 'P', 'Q' or 'R' char.

		gdtnode new_son_of_node (gdtnode v, skeleton* skp);				// make a new SPQR_son_node of gdtnode v,
											// and associate it the skeleton (*skp).




		void          BB_build_base_code_vector 		  (gdt::gdtarray<gdtnode>&, BB_visit_type);
		void          BB_compute_number_of_nodes_into_BB_subtrees (gdt::gdtarray<gdtnode>&, gdt::gdtmap<int,int>&);
		void          BB_compute_status_of_node_vector            (gdt::gdtarray<gdtnode>&, BB_assignment, gdt::gdtnode_array<int>&);
		void	      BB_compute_first_total_upper_bound          (gdtedge&, const plan_undi_graph& , plan_undi_graph&, face&, int&, int&, BB_options&, algorithm_type&);


		BB_assignment BB_generate_complete_assignment             (gdt::gdtarray<gdtnode>&, BB_assignment, gdt::gdtnode_array<int>&, BB_options&);

		int           BB_compute_upper_bound 		     	  (gdt::gdtarray<gdtnode>&, BB_assignment, gdt::gdtnode_array<int>&, int, int, gdt::gdtnode_array<int>&, BB_options&, algorithm_type = PLAN_ORTH_SLOW);
		//int           BB_compute_lower_bound		     	  (gdt::gdtarray<gdtnode>&, BB_assignment, gdt::gdtnode_matrix<int>&, gdt::gdtnode_matrix<path_info>&, int, int, BB_options&, set<int>&, bool = true, algorithm_type = PLAN_ORTH_SLOW);
		
		// modified by W.D. on April 17, 2003. Use gdt mapping instead of leda sets.
		int           BB_compute_lower_bound		     	  (gdt::gdtarray<gdtnode>&, BB_assignment, gdt::gdtnode_matrix<int>&, gdt::gdtnode_matrix<path_info>&, int, int, BB_options&, gdt::gdtmap<int,bool>&, bool = true, algorithm_type = PLAN_ORTH_SLOW);				
		
											
	protected:
		void compute_tree_dimensions (gdt::gdtnode_array<int>& width_of_subtree, 
	 				      gdt::gdtnode_array<int>& lev, gdt::gdtnode_array<int> width_of_node,
	 				      bool print_Q_node);
	 				      
	 
										
									
	public:
		 ~SPQR_tree ();
		 SPQR_tree  (plan_undi_graph&, gdtedge=NULL_EDGE, bool simpl = false);
		
		
		// -----------------
		// Access operations
		// -----------------
		
		 SPQR_node_type  get_type_of_node 		   (gdtnode v)   		 const;	// return the type of gdtnode v 
		 skeleton*	get_skeleton_of_node		   (gdtnode v)   		 const;	// return the pointer of the skeleton of gdtnode v (NULL if v is a Q_node) 	
		
		 bool		get_simpl_node_status		   ()			 const; // return the value of simpl_node_status (simplificate gdtnode status)
		
		 gdtedge		tree_edge_of_skeleton_edge_in_node (gdtedge e, gdtnode v)      const;	// return the tree gdtedge corresponding to the
												// skeleton gdtedge e in gdtnode v.
			
		 gdtedge		skeleton_edge_of_tree_edge	   (gdtedge e)		 const; // return the skeleton gdtedge corresponding to the
												// tree gdtedge e
		
		 gdtnode 		Q_node_of_edge_with_id		   (int ide)		 const;	// return the Q_node corresponding to the gdtedge with id ide
												// if it doesn't exist, return NULL_NODE.
		
		
		 rotation_type	get_status_R_node_rotation	   (int i, gdtnode v)	 const;	// return the rotation_type (CLOCKWISE, COUNTERCLOCKWISE) of status i in the R-gdtnode v
		 gdt::gdtlist<gdtedge>	get_status_P_node_permutation	   (int i, gdtnode v)	 const; // return the gdtedge-list permutation of status i in P-gdtnode v 
		
		 int		number_of_nodes_type		   (SPQR_node_type nt)	 const;	// return the number of the nodes of type nt. 
		int		max_status_of_node		   (gdtnode v)		 const;	// return the maximum status number of gdtnode v.
		
		 bool		tree_edge_is_virtual		   (gdtedge te)		 const; // return true if skeleton_edge of tree_edge e is virtual.
		
		
		// -------------------------------
		//  DO NOT LIST IN PUBLIC METHODS
		// (see src code for more details)
		// -------------------------------
		//
		 void 		compute_pre_lower_bound_type_l	(gdt::gdtnode_matrix<int>& l, algorithm_type = PLAN_ORTH_SLOW); 
		 void		compute_pre_lower_bound_type_L	(gdt::gdtnode_matrix<int>& L, gdt::gdtnode_matrix<int>& l, gdt::gdtnode_matrix<path_info>& P, bool = true, algorithm_type = PLAN_ORTH_SLOW);
		//
		
										
		 bool 		merge_undi_graph_with_skeleton_of_node  (
									undi_graph& ug, 
									gdtnode v, 
									int i, 
							     		gdtedge& start_edge, 
							     		gdtnode& start_node,     
							    		gdt::gdtedge_map<gdtedge>& tree_edge
							    		);				// see below..
							    		
					// ------------------------------------------------------
					// replace the ug gdtedge,corresponding to the reference
					// gdtedge of skeleton(v), with skeleton(v) itself;
					// merging is computed with respect to the status i
					// of the gdtnode v (embedding of the skeleton); 
					// tree_edge[e] will be the gdtedge of the SPQR_tree 
					// corresponding to the gdtedge e of ug;
					// start_edge will be the reference gdtedge that must
					// belong to the external face. Because there are
					// two faces containing start_edge, it needs
					// to specify the start_node of start_edge, in order 
					// to correctly rebuild the external face.
					// If v is an S_NODE, the status i is ignored.
					// If ug is empty, it is inizialized with the skeleton 
					// of the gdtnode v.
					// The function returns true if the merging is correctly
					// executed.
					//
					// PRECONDITIONS: there exists an gdtedge e of ug such that
					// 		  id(e) = id(ref_edge) in skeleton
					//		  of gdtnode v, or ug is empty. 
					//                Therefore, v is not a Q_NODE.
					// ------------------------------------------------------
											
												
		
		 bool		build_undi_graph_starting_from_node     (
									undi_graph& ug, 
									gdtnode v, 
								      	gdt::gdtnode_array<int>& status_of_node, 
								       	gdtedge& start_edge, 
								       	gdtnode& start_node,
								       	gdt::gdtedge_map<gdtedge>& tree_edge
								       	);				// see below..
								     
					// -------------------------------------------------
					// build the undi_graph ug, merging all nodes of the
					// subtree rooted at v, according to the 
					// status_of_node array.
					// tree_edge[e] will be the gdtedge of the SPQR_tree 
					// corresponding to the gdtedge e of ug;
					// start_edge will be the reference gdtedge that must
					// belong to the external face. Because there are
					// two faces containing start_edge, it needs
					// to specify the start_node of start_edge, in order 
					// to correctly rebuild the external face.
					// The function returns true if the building is 
					// correctly executed.
					// -------------------------------------------------
					
					
		 void 		pertinent_graph_of_node		     (undi_graph& ug, gdtnode v);
				
					// --------------------------------------------------
			 		// compute the pertinent graph of gdtnode v,
					// i.e., the graph obtained merging all the skeletons
					// of nodes into the subtree rooted at v.
					// vector.
					// The function returns true if the building is 
					// correctly executed.
					// --------------------------------------------------
					
												
		
		
		 gdt::gdtlist<int>	find_shortest_path_into_pertinent_graph_of_node 	(
											gdtnode w, 
											int id_v1, 
											int id_v2, 
											gdt::gdtlist<int>& edge_id_path
											);		// see below..
					
					// --------------------------------------------------
					// find and return a list of gdtnode-id, representing a
					// shortest path from gdtnode with id_v1 and gdtnode with 
					// id_v2 into the pertinent graph of gdtnode w.
					// Also return in edge_id_path the ordered list of 
					// the gdtedge in the shortest path.
					// --------------------------------------------------
					
		
		 gdt::gdtlist<gdtedge>	find_shortest_path_into_pertinent_graph_of_node 	(
											gdtnode w, 
											int id_v1, 
											int id_v2
											);		// see below..
											
					// --------------------------------------------------
					// find and return a list of edges, representing a
					// shortest path from gdtnode with id_v1 and gdtnode with 
					// id_v2 into the pertinent graph of gdtnode w.
					// --------------------------------------------------
												
											
											
		 gdt::gdtlist<gdtedge>	find_minimum_switches_path_into_pertinent_graph_of_node (
											gdtnode w, 
											int id_v1, 
											int id_v2, 
											int& switches, 
											visit_direction_type
											);		// see below..
											
					// ---------------------------------------------------
					// find and return a list of edges, representing a
					// path with minimum number of switches from gdtnode 
					// with id_v1 and gdtnode with id_v2 into the pertinent 
					// graph of gdtnode w, and following the 
					// visit_direction_type specified.
					// switches will be the number of swithces of the path
					// ---------------------------------------------------
					
		
		
		 int  		find_best_embedding_with_external_face 
									(
									plan_undi_graph& pug, 
									face& ext_face, 
									BB_options Op = STANDARD_BB_OPTIONS, 
									algorithm_type=PLAN_ORTH_SLOW,
									BB_tree_parameters* BBp=NULL
									);				// see below..
		
					// --------------------------------- Kernel of Slow Algorithms ---------------------------------
					// -------------------------------------------------------------------------------------------------
					// search the plan_undi_graph pug and its face ext_face in such a way that:
					//
					// 1) pug has the same nodes and edges as the owner graph of Tp;
					// 2) the embedding of pug with external face ext_face produces the orth_plan_undi_graph
					//    or upwa_plan_undi with the minimum number of bends over all the possible drawings.
					//      
					// The function returns the total number of (non-zero cost) bends in an optimal drawing 
					// of pug with external face ext_face.
					//   
					// NOTE:   This is an NP-HARD problem and thus this is a non polynomial function in general.
					//         Also, this function uses a Branch and Bound technique.
					//
					// NOTICE: This function may requir a very long time if the graph has a high number of nodes and/or
					//	   a high number of triconnected components.
					//	   
					// PRECONDITIONS: The graph must be biconnected 
					//                
					// -------------------------------------------------------------------------------------------------
												
			
		// -----------------								
		// Update operations
		// -----------------
		
		 void clear     ();			// delete all nodes and edges		
		
		 void make_root (gdtnode v);		// make v as root of tree, updating all the pointers and reference edges
							// PRECONDITIONS: v must be a Q_NODE.
						  
		
		 void set_simpl_node_status (bool);	// set simpl_node_status with bool (simplificate gdtnode status);
		
		// --------------------------
		// Input / Output opearations
		// --------------------------
		
		 void print 		(SPQR_node_type, std::ostream& os = std::cout);
		 void print_node_status  (gdtnode v,std::ostream& os = std::cout);
			
	};


#endif
