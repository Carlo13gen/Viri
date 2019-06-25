/*******************************************************************************
+
+  rm3_undi_graph.h
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

#ifndef RM3_UNDI_GRAPH_H
#define RM3_UNDI_GRAPH_H


#include <fstream>
#include <limits.h>
#include<cstdlib>
#include<cmath>
#include <string>

#include <GDT/rm3_global.h>
#include <GDT/rm3_constraints.h>
#include <GDT/gdtlist.h>
#include <GDT/gdtstack.h>


//#define LABEL
//#define OUTPUT
//#define COUNTER

//-----------------------------------------------------------------------------
// undi_graph: base class for all directed and undirected embedded graphs
//
// W.Didimo e A.Leonforte (1996)
//-----------------------------------------------------------------------------

class SPQR_tree;         // forward declarations
class plan_undi_graph;	 // forward declarations


#define AUTO_ID -1        // AUTO identifier
#define NULL_ID -1        // NULL identifier

extern const gdtnode 	 NULL_NODE;		// NULL gdtnode
extern const gdtedge 	 NULL_EDGE;		// NULL gdtedge
extern const constraint  NULL_CONSTRAINT;	// NULL constraint

	
	/*! \var typedef enum visit_direction_type
	    \brief Global type used in graph visit algorithms. 
	Possible values are OUTGOING and INCOMING. 	
	For example, in the simple_DFS() method for a directed
	graph dire_graph you can specify the direction of the edges along
	which you want to walk; if you specify OUTGOING (default), you move from
	a gdtnode only along the outgoing edges; if you specify INCOMING, you
	move from a gdtnode only along the incoming edges.
	*/

	typedef enum
	{
	OUTGOING,
	INCOMING
	}
visit_direction_type;

      /**
	Global type used in print() methods. 
	Possible values are: BY_NODES, BY_EDGES, BY_FACES, BY_PATHS, and COMPLETE.	
	Choosing BY_NODES, only the adjacent lists of the edges around
	nodes are printed. Choosing BY_EDGES, it is printed a list of all
	edges. Choosing BY_FACES a list of all faces (with all edges and nodes
	on the border) is printed; this choice is possible only for an object
	of the class plan_undi_graph (or inherited). The choice BY_PATHS is
	allowed for an object of class dime_orth_plan_undi_graph, and it
	prints each gdtedge by a path from the source extremal gdtnode to the target
	extremal gdtnode, through the list of bend points of the gdtedge. Choosing
	COMPLETE, a printing of all possible information for the current
	object is provided.
	*/

	
	typedef enum
	{
	BY_NODES,
	BY_EDGES,
	BY_FACES,
	//Debug
	FOR_DEBUG,
	//
	BY_PATHS,
	COMPLETE
	}
print_mode;


	/**
	Global type used in compare() methods. 
	Possible values are: MIN and MAX.
        (See the compare methods for more details)
	*/

	typedef enum
	{
	MIN,
	MAX
	}
compare_option;



	/**
	Global type used as DFS gdtedge markers. 
	Possible values are: TREE_EDGE, BACK_EDGE, and FORW_EDGE.
        When a DFS visit is executed, we call TREE_EDGE a gdtedge of the
        spanning tree induced by the DFS. A gdtedge that is not in the spanning
        tree can be a BACK_EDGE or FORW_EDGE depending on its orientation. 
	(See the DFS_edge() method for more details).
        */
        
	typedef enum
	{
	TREE_EDGE,
	BACK_EDGE,
	FORW_EDGE
	}
DFS_edge_type;



	/**
	Global type used as a parameter by the planarize() method.
        Two are the possible options: DO_NOT_PRESERVE_EMBEDDING leaves the
        planarization step the freedom of changing the order of edges around
        nodes; PRESERVE_EMBEDDING forbids the planarize() method to change
        such order. Other planarization vincula, referring to smaller parts
        of the graph and not to the whole graph, are handled by means of
        constraints.
	*/
	typedef enum
	{
	DO_NOT_PRESERVE_EMBEDDING,
	PRESERVE_EMBEDDING
	}
planarize_options;




// ----------------------------------------
// Global classes for the planarity testing
// ----------------------------------------



	/**
	Global class containing information collected during 
	a DFS traversal of the graph.
	*/

class dfs_node_info {
	public:
	static int counter;
	/** Depth First Index (DFI) of the node (the root node has index 0) */
	int DFI;    			
	/** DFI of the lowpoint node of this node */
	int lowpoint;
	/** Edge conneting this node to its parent node*/ 			
	gdtedge parent;			
	/**
	Empty constructor.
	Creates a dfs_node_info object.
	*/
	dfs_node_info();
	/**
	Destructor.
	*/
	~dfs_node_info();
};



	/**
	Global class containing information collected during 
	a DFS traversal of the graph.
	*/


class dfs_edge_info {
	public:	
	int index;
	DFS_edge_type type;
	dfs_edge_info();
	~dfs_edge_info();

};

	
	/**
	Global class containing information to execute 
	the Boyer & Myrvold's algorithm.
	*/
class bm_node_info {
	public:

	static bm_node_info* my_new_bm_node_info();
	//int DFI;    				// Depth First Index (root vertex is 0)
	int lowpoint;				// DFI of the node that is the lowpoint of current node
	gdtedge parent;				// edge conneting to parent node;
	gdt::gdtlist<gdtnode> children;  			// children in the DFS_TREE
	gdt::gdtlist<gdtnode> in_back_edges;  		// entering back_edges

    int first_back_edge_dfi;
	bool ordered_first_black;

	gdt::gdtlist<gdtnode> children_ordered;		// children in the DFS tree ordered by lowpoint values
	gdt::list_item position_in_parent_list;      // position in the children_ordered list of the parent node

	bm_node_info();
	~bm_node_info();
};


	/** 
	Global class containing information to execute 
	the Boyer & Myrvold's algorithm.
	*/

class bm_edge_info {
	public:
	static bm_edge_info* my_new_bm_edge_info();
	DFS_edge_type type;
	bool inserted;

	bm_edge_info();
	~bm_edge_info();
};








	/* 
	Global class containing information to execute 
	the Boyer & Myrvold's algorithm.
	Each bic_obj may represent a node (node bic_obj) or an edge (edge bic_obj)
	in a biconnected component.
	*/



class bic_obj {


		protected:
		#ifdef OUTPUT
		std::string label;  		// the node id in case of a node bic_obj, the catenation of node ids (with a comma in the middle) otherwise
		#endif
		bool is_node;   	// true if this bic_obj represents a node, false if it represents an edge
		bic_obj* twin_link;	// points to related bic_obj in the embedding (see the Boyer and Myrvold paper)
		bic_obj* black;		// a pointer of the circular adjacency list (see the paper again)
		bic_obj* white;		// the other pointer of the circular adjacency list

		gdtnode graph_node;	// if this is a node bic_obj, the this field contains a pointer to the node of the original graph (undi_graph)
		gdtedge graph_edge;	/* if this is an edge bic_obj, and is not a ficticious edges, pointer
							   to the edge of the original graph (undi_graph) */


		int marked;					/* 0=not marked; 1=marked; 2=marked twice (only edge bic_obj can be marked twice, if they are
										in an elementary bicomp)   */



		// PROTECTED METHODS






		/*
		METHOD is_active (and is_active_verbose)
		Test if a node is active respect to node target.
		A node is active if it has a back edge over target, or it has a child bicomp which has a back edge over target.
		<UL>
		<LI>target: target node
		<LI>node_vector: pre-processing informations on graph nodes
		</UL>
		*/

			bool
		is_active
			(
			int target,
			bm_node_info node_vector[]
			//int lowpoint[],
			//int first_back_edge_dfi[],
			//list<node> children_ordered[]
			);


        /*
        METHOD is_root_externally_active
		Test if a root node is esternally active.
		A root is externally active if the correspondig node in its parent bicomp has a back-edge over target, or
		it has a child node different from child which lowpoint is less than target.
		Node child id the child node in the current bicomp.
		This method must be applied to root bic_obj only.
		<UL>
        <LI>target: target node
        <LI>child: first child node in this bicomp
		<LI>node_vector: pre-processing informations on graph nodes
		</UL>
		*/
			bool
		is_root_externally_active
			(
            int target,
			gdtnode child,
			bm_node_info node_vector[]
			//int lowpoint[],
			//int first_back_edge_dfi[],
			//list<node> children_ordered[]
			);

			bool
		is_internally_active_node
			(
            int target,
			bm_node_info node_vector[]
			//int first_back_edge_dfi[],
			//list<node> children_ordered[]
			);

		/////////////////////////
		// public
		/////////////////////////

		public:

		static bic_obj* my_new_bic_obj(bic_obj*& bic_obj_actual_pointer);

		static bic_obj* my_new_bic_obj(gdtedge e,bic_obj*& bic_obj_actual_pointer);

		friend class undi_graph;
		friend class struct_gdtnode;
		friend class bic_obj_node;


		/*
		CATEGORYremoved constructors_destructors
		Constructors and destructors
		*/

			/*
			Bic_obj empty constructor.
			This constructor creates bic_obj which may represent ficticious back_edges
			*/
			bic_obj();

			/*
			Bic_obj destructor
			*/

			~bic_obj();


			/*
			Node bic_obj constructor
			*/

			bic_obj(gdtnode n);


			/*
			Edge bic_obj constructor
			*/

			bic_obj(gdtedge e);


		/*
		CATEGORY check
		Methods to check bicomp structure
		*/



			/*METHOD check_node
			Check the circular adjacency list of the node. Return true if it is correct.
			

			bool
		check_node();
			*/


		/*
		CATEGORY access
		Access operations
		*/


			/*METHOD read_node_embedding
			Read the embedding from the bicomp structure and copy it to graph ug
			Return true if operations end correctly.
			*/

			bool
		read_node_embedding
			(
			undi_graph& ug
			);



			/*
			METHOD get_label
			Return the label of the bic_obj.
			*/

			std::string
		get_label
			(
			);

	};

	class
bic_obj_node: public bic_obj
	{
    protected:

    bic_obj_node* twin_link;
	gdt::gdtlist <bic_obj_node*> active_bicomp;
	gdt::gdtlist <bic_obj_node*> not_active_bicomp;
	gdtnode first_child;
    int active_passages;
	bool actual_previous_connector;
	int DFI;
	bool can_be_flipped;
	bool active_walk_up;

	public:
	bic_obj_node();

	bic_obj_node(gdtnode n);
	static bic_obj_node* my_new_bic_obj(gdtnode n,bic_obj_node*& bic_obj_node_actual_pointer);

	friend class undi_graph;


	protected:

    /*
		METHOD next_node
		Return the next node on the external side of the bicomp.
		It must be applied to a node bic_obj.
		<UL>
		<LI> black = if true(false), return the next node starting from the black(white) pointer of this node
		</UL>
		*/


			bic_obj_node*
		next_node
			(
			bool black
			);



	/*
		METHOD walk_up
		Realize the walk_up phase.It must be applied to node bic_obj which are the start nodes of back edges.
		This method return true if a back_edge between target and this node can be added to the structure without violate planarity.
		<UL>
		<LI> target: graph node which is target of back_edge.
		<LI> node_vector: pre-processing informations
		<LI> secondary: list of the root bic_obj corresponding to target node reached by all walk_up phases on the same target node
		<LI> walk_up_stack: this stack contains all the bic_obj visited in walk_up phase
		</UL>
		*/
			bool
		walk_up
			(
			int target,
			//int lowpoint[],
			//int first_back_edge_dfi[],
			//list<node> children_ordered[],
			bm_node_info node_vector[],
			gdt::gdtlist<bic_obj_node*>& secondary,
			undi_graph& ug
			//stack<bic_obj*>&  embedding_stack
			);


		/*
		METHOD walk_down (and walk_down_verbose)
		Realize the walk_down phase. It inserts all back edges examinated in the previous walk_up_phases updating the bicomp sructure.
		Return true if the structure was updated correctly, otherwise return false.
		PRECONDITIONS: this method must be activated only after the correct executions af all walk_up phases.
		<UL>
		<LI>node_vector: pre-processing informations on graph nodes
		<LI>edge_vector: pre-processing informations on graph edges
		<LI>secondary: list of root bic_obj which are target nodes of back edges. This list is calculated during the walk_up phases
		<LI>ug: original graph (undi_graph)
		<LI>embedding_stack: this stack will contain a pointer for each edge bic_obj inserted (it needs to read embedding)
		<LI>edges_inserted: for each edge, it became true when it is inserted in bicomp structure
		</UL>
		*/

			bool
		walk_down
			(
			bm_node_info node_vector[],
            //list_item position_in_parent_list[],
			//list<node> childen_orderd[],
			gdt::gdtlist<bic_obj_node*>& secondary,
			undi_graph* ug,
			//stack<bic_obj*>& embedding_stack,
			//edge_map<bool>& edge_to_be_inserted
			bool edge_to_be_inserted[],
			int flip[],
			bic_obj*& bic_obj_actual_pointer
			);

		/*
		METHOD extract_embedding
		This method visit the final bicomp structure at the end of all walk_up and walk_down phases, and for each node orders its
		circular adjacency list.
		<UL>
		<LI> embedding_stack = stack of pointers to edge_bic_obj inserted during walk_down phases.
		<LI> node_vector: pre-processing informations on graph nodes
		<LI> edge_vector: pre-processing informations on graph edges
		</UL>
		*/
			bool
		extract_embedding
			(
			gdt::gdtstack<bic_obj*>& embedding_stack,
			//edge_map<bm_edge_info*>& edge_vector,
			undi_graph& ug
			);


			bool
		embedding_reporting
        	(
			bm_node_info node_vector[],
			//bool ordered_first_black[],
			//list<node> children[],
			//edge parent[],
			undi_graph& ug,
			int flip[],
			bic_obj_node* IMM[]
			);

		public:
		/*
			METHOD check_bicomp
			Visit the external side of a bicomp. Return true if the visit is correct.
			*/

			bool
		check_bicomp();



	}; // End class bic_obj_node

	

// -------------------------------
// Classes for nodes and edges 
// internal structures.
// -------------------------------

	/**
	A struct_gdtnode object represents the internal structure 
	of a node of a graph. 
	*/


	class struct_gdtnode {
		friend class undi_graph;
		friend class bic_obj;
		friend class bic_obj_node;
		//Modified by A.M. 10/2002
		//gdtnode lnode;
		gdt::list_item pos_in_nodes;

		int plan_id;   // node-identifier used by the planarity test  P.F. Cortese 10/11/2003
		public:
		struct_gdtnode() {}

		int get_plan_id() {return plan_id;}
		//Removed by A.M. 10/2002
		/*
		struct_gdtnode(gdtnode _lnode) {
			lnode = _lnode;
		}
		*/
	};


	/**
	A struct_gdtedge object represents the internal structure 
	of an edge of a graph.	
	*/	
	class struct_gdtedge {
		friend class undi_graph;
		friend class bic_obj;
		friend class bic_obj_node;

		//Modified by A.M. 10/2002
		//gdtedge ledge;
		gdt::list_item pos_in_edges;
		int plan_id;   // edge-identifier used by the planarity test  P.F. Cortese 10/11/2003

		public:
		struct_gdtedge() {}
		//Removed by A.M. 10/2002
		/*
		struct_gdtedge(gdtedge _ledge) {
			ledge = _ledge;
		}
		*/

	};


	/**	
	This structure contains additional information 
	for a node of an undi_graph class object.
	In particular, it contains:
	<br>1 - a non-negative integer identifier for the node;
	<br>2 - the counterclockwise circular list of the incident incoming gdtedges;
	<br>3 - the counterclockwise circular list of the incident outgoing gdtedges;
	<br>4 - the counterclockwise circular list of all incident gdtedges;
	<br>5 - a list of all markers of the node;
	<br>6 - a list of all constraints involving the node.
	*/
	class GDT_NONSTANDARD_DECL
struct_undi_node_info
	{
	friend class undi_graph;
	friend class struct_constraint;
	friend class bic_obj;
	friend class bic_obj_node;
	//
	private:
		int id;				// gdtnode-identifier
		gdt::gdtlist<gdtedge> in_edges;		// in_edges list of gdtnode
		gdt::gdtlist<gdtedge> out_edges;		// out_edges list of gdtnode
		gdt::gdtlist<gdtedge> adj_edges;		// adj_edges list of gdtnode
		//
		bool in_edges_are_ordered;	// true when in_edges list is ordered
		bool out_edges_are_ordered;	// true when out_edges list is ordered
		//
		gdt::gdtlist<marker_type> markers;	// list of all markers on gdtnode
		gdt::gdtlist<constraint>  constraints;	// list of all constraints involving gdtnode
		//Removed by A.M. 18/10/2002
		//gdtnode gnode;
		//
	public:
		/*
		CATEGORYremoved constructors_destructors
		Constructors and destructors
		*/

			/*
			METHODremoved struct_undi_node_info
			Struct-gdtnode constructor.
			*/

		struct_undi_node_info
			(
			);
	};



	/**
	This structure contains additional information 
	for an edge of an undi_graph class object.
	In particular, it contains:
	<br>1 - a non-negative integer identifier for the edge;
	<br>2 - a "source" gdtnode;
	<br>3 - a "target" gdtnode;
	<br>4 - a "start" gdtnode if the edge is directed
	    (if the edge is not directed, this field is equal to NULL_EDGE);
	<br>5 - the positions of the edge in all the adjacent lists of its end-gdtnodes;
	<br>6 - a list of all markers of the edge;
	<br>7 - a list of all constraints involving the edge.
	*/

	class GDT_NONSTANDARD_DECL
struct_undi_edge_info
	{
	friend class undi_graph;
	friend class struct_constraint;
	friend class bic_obj;
	friend class bic_obj_node;
	//
	private:
		int id;					// gdtedge-indentifier
		gdtnode source;  				// source extremal gdtnode (dummy reference) of gdtedge
		gdtnode target;  				// terget extermal gdtnode (dummy reference) of gdtedge
		gdtnode start;				// start gdtnode if gdtedge is directed (NULL_NODE if gdtedge is undirected)
		gdt::list_item pos_into_in_edges;		// positions of gdtedge into in_edges list
		gdt::list_item pos_into_out_edges;		// position of gdtedge into out_edges list
		gdt::list_item pos_into_source_adj_edges;	// position of gdtedge into source's adjacent gdtedge list
		gdt::list_item pos_into_target_adj_edges;	// position of gdtedge into target's adjacent gdtedge list
		//
		gdt::gdtlist<marker_type> markers;		// list of all markers on gdtedge
		gdt::gdtlist<constraint>  constraints;		// list of all constraints involving gdtedge

		//Removed by A.M. 18/10/2002
		//gdtedge gedge;
		//
	public:

		/*
		CATEGORYremoved constructors_destructors
		Constructors and destructors
		*/

			/*
			METHODremoved struct_undi_edge_info
			Struct-gdtedge constructor.
			*/

		struct_undi_edge_info
			(
			);
	};

	
	/**	
	Represents a reference to a node of a graph
	A node can be created only using graph methods
	*/
	typedef struct_gdtnode *gdtnode;

	/**
	Represents a reference to an edge of a graph
	An edge can be created only using graph methods
	*/
	typedef struct_gdtedge *gdtedge;

	// ------------------------------------------------------------------------------------------------
//                                                UNDI_GRAPH
// ------------------------------------------------------------------------------------------------

	/**	
	An <B><I>undi_graph</I></B> object represents a multi-graph that is not necessarily connected.
	The nodes of the graph are denoted by gdtnode references, and the edges
	are denoted by gdtedge references. 
	Each edge of an undi_graph can be directed or not. This also means that 
	the graph may have both directed and unidirected edges at the same time.	
	Also, an undi_graph is always equipped with an <I>embedding</I>, which 
	describes the counterclockwise ordering of the edges incident on each node.
	Such embedding is preserved when the undi_graph is copied into another 
	undi_graph object.		
	<P>Nodes and edges of an undi_graph object have non-negative integer identifiers.
	Two distinct nodes (resp. edges) can not have the same identifier.
	Identifiers are preserved during each copy of the graph. 			
	
	In addition to nodes and edges, an undi_graph object can have a list of <I>constraints</I>.	 
	A constraint can be a topological or a drawing constraint. For example, 
	a topological constraint may require that a specified edge is not allowed to cross
	in the current graph embedding; a drawing constraint may require that a specified
	node has a given size. Constraint can be created and added to an undi_graph
	with public methods. As for nodes and edges, each constraint has a non-negative
	integer identifier. Constraints and their identifiers 
	are preserved during copy operations. 
	
	<P><B>NOTICE</B>: selfloops are not handled.
	*/

	class GDT_NONSTANDARD_DECL
undi_graph
	{
	friend class SPQR_tree;
	friend class struct_constraint;
	friend class bic_obj;
	friend class bic_obj_node;
	//
	private:

		// -----------------
		// private variables
		// -----------------

		//Added by A.M. 2/2002
		gdt::gdtlist<gdtnode> *nodes;	//Node list
		gdt::gdtlist<gdtedge> *edges;	//Edge list
		//

		// commented out by pizzo
		gdt::gdtnode_map<struct_undi_node_info>* node_info;	// correspondence gdtnode --> undi-gdtnode structure
		gdt::gdtedge_map<struct_undi_edge_info>* edge_info;	// correspondence gdtedge --> undi-gdtedge structure

		gdt::gdtlist<constraint>* constraints;			// list of constraints for graph

		gdt::gdtmap<int,gdtnode>* node_with_id;			// correspondence gdtnode-identifier --> gdtnode
		gdt::gdtmap<int,gdtedge>* edge_with_id;			// correspondence gdtedge-identifier --> gdtedge
		gdt::gdtmap<int,constraint>* constraint_with_id;	// correspondence constraint-identifier --> constraint

		int max_node_id;				// maximum value of a  gdtnode-identifier
		int max_edge_id;				// maximum value of an gdtedge-identifier
		int max_constraint_id;				// maximum value of a  constraint-identifier
		int max_edge_plan_id;			// maximun value of a gdtedge-plan-identifier: this value is used by the planarity testing algorithm
		bool keep_ordering_of_directed_edges;		// if true, also the embedding of the directed edges is kept
								// consistently with the embedding of all underlying edges.
								// TRUE is default. You can disable this flag when you do not
								// need of knowing the exactly embedding of the directed edges,
								// and you want to save computation time.


		// ---------------
		// private methods
		// ---------------

		void local_new();   				// create a new set of pointers for the not-inherited class-part
		void local_del();   				// delete all the not-inherited class-part
		void local_renew(); 				// utility function : just local_del(), then local_new()
		void local_init();  				// init the not-inherited class-part

		void local_set	    				// set all private variables
			(
			gdt::gdtlist<gdtnode>*,
			gdt::gdtlist<gdtedge>*,
			//graph*,
			gdt::gdtnode_map<struct_undi_node_info>*,
			gdt::gdtedge_map<struct_undi_edge_info>*,
			gdt::gdtlist<constraint>*,
			gdt::gdtmap<int,gdtnode>*,
			gdt::gdtmap<int,gdtedge>*,
			gdt::gdtmap<int,constraint>*,
			int,
			int,
			int,
			bool
			);

		void order_in_edges  (gdtnode);			// make ordered the in-edges  adjacent list of gdtnode
		void order_out_edges (gdtnode);			// make ordered the out-edges adjacent list of gdtnode
		void mark_in_edges_as_not_ordered  (gdtnode);	// mark the in_edges  adjacent list of gdtnode as not marked
		void mark_out_edges_as_not_ordered (gdtnode);	// mark the out_edges adjacent list of gdtnode as not marked

		void remove_from_in_edges  (gdtedge);		// remove the gdtedge from the in_edge  adjacent lists
		void remove_from_out_edges (gdtedge);		// remove the gdtedge from the out_edge adjacent lists
		void remove_from_adj_edges (gdtedge);		// remove the gdtedge from all adjacent lists

		bool make_embedding_planar0 ();			// make the embedding planar if there exists one
		
		gdt::gdtlist<constraint>* get_constraints();		// return a pointer to list of all constraints of graph
		
		gdt::gdtlist<constraint>& all_constraints(gdtedge);	// return a reference to the list of all constraints involving gdtedge
		gdt::gdtlist<constraint>& all_constraints(gdtnode);	// return a reference to the list of all constraints involving gdtnode
		
		bool  planarize_kernel 
			(
			gdt::gdtlist<gdtedge>&,
			gdt::gdtlist<gdtedge>&,
			planarize_options po = DO_NOT_PRESERVE_EMBEDDING
			); 						// see below...

		 	// ---------------------------------------------------------------------
		 	// method called by undi_graph::planarize()
		 	// planarize the graph with given planarize_options
			// without crossing edges in first given gdt::gdtlist<gdtedge>
		 	// and removing edges in the second gdt::gdtlist<gdtedge> after
		 	// the maximal planar graph is found.
		 	// Return false when no solution is found due to constraints.
		 	// ---------------------------------------------------------------------
								

		// these 2 methods should be private in the final version
		public:
		bool make_embedding_planar_boyer ();			// make the embedding planar if there exists one
		
		private:
		// *************************************************************************************************
		// --------------------------------------------------------------------
		// THIS METHODS ARE NEEDED FOR IMPLEMENTATING THE ST-NUMBERING METHOD.
		// MORE DETAILS ARE PROVIDED IN THE SOURCE FILES.
		// --------------------------------------------------------------------

		
		gdt::gdtlist<gdtedge> find_path 
			(
			gdtnode v, 
			bool& exist_path1, 
			bool& exist_path2,
			bool& exist_path3,
		 	gdt::gdtnode_array<gdtedge>& father, 
		 	gdt::gdtnode_array<int>& num_visit, 
		 	gdt::gdtnode_array<gdtnode>& lowpt1, 
		 	gdt::gdtnode_array<bool>& marked
		 	);
		 		      
		 	// ---------------------------------------------------------------------
		 	// search a specified type path (v,v1,...,vn,w) in graph, after
		 	// a DFS-visit of it has been made. Nodes v,v1,...,vn are not marked
			// and w is marked. 
		 	// ---------------------------------------------------------------------

		
		DFS_edge_type DFS_edge 
			(
			gdtedge e, 
			gdtnode v,
			gdt::gdtnode_array<gdtedge>& father, 
			gdt::gdtnode_array<int>&  num_visit
			) const;

			// ------------------------------------------------------------------------
			// return the type of gdtedge in a specified DFS order.
			// (e,v)        = gdtedge to testing, starting from gdtnode v.
			// father[w]    = gdtedge father of gdtnode w in the DFS.
			// num_visit[w] = number of visit for gdtnode w in the DFS 
			// (i.e. num_visit[v] < num_visit[w] if v is visited before than w).
			// NOTE: it's necessary to specify the num_visit for to have
			// a constant time cost.
			// ------------------------------------------------------------------------
		
		// *************************************************************************************************	
		


	protected:
	
		void set_source (gdtedge, gdtnode);						// set the source of the gdtedge with gdtnode

		// ------------------------------------------------------------
		// protected methods for an intelligent updating of constraints
		// ------------------------------------------------------------
		
		void update_constraints_after_del_edge (gdtedge e);			// update constraints involving gdtedge e, when a delete gdtedge operation on e is applied
		void update_constraints_after_del_node (gdtnode v);			// update constraints involving gdtnode v, when a delete gdtnode operation on v is applied
		
		void update_constraints_after_split_edge (gdtedge e, gdtedge e1, gdtedge e2);	// update constraints involving gdtedge e, when a split operation e --> e2,e2 is applied
		void update_constraints_after_split_node (gdtnode v, gdtnode v1, gdtnode v2);	// update constraints involving gdtnode v, when a split operation v --> v1,v2 is applied

		void update_constraints_after_merge_edges (gdtedge e1, gdtedge e2, gdtedge e);	// update constraints involving gdtedge e1 and gdtedge e2, when a merge operation e1,e2 --> e is applied
		void update_constraints_after_merge_nodes (gdtnode v1, gdtnode v2, gdtnode v);	// update constraints involving gdtnode v1 and gdtnode v2, when a merge operation v1,v2 --> v is applied
 		
		void update_constraints_after_edge_translation
			(
			gdtedge e  ,
			gdtnode ve1, 
			gdtnode ve2, 
			gdtedge d  ,
			gdtnode vd1,
			gdtnode vd2
			);								// modify all constraints involving gdtedge e, replacing this gdtedge with d and, if it is needed, update also the extremal vertices
		
															
		void update_constraints_on_path_replacing_edge				
			(
			gdtedge e,
			undi_graph& ug,
			gdt::gdtlist<gdtedge> path
			);								// generate new constraints for path, which replaces gdtedge e , based on constraints involving e
											// NOTICE: - path is a sequence of edges of this graph;
											// 	   - e is an gdtedge of ug (not necessarily this graph)
											//	   - the extremal nodes of e must have the same id than the extremal gdtnode of path
											
											
		void copy_constraints_from_subgraph_of_undi_graph				
			(
			gdt::gdtlist<gdtedge>& sub,
			undi_graph& ug
			);								// copy on this, as much as possible, all constraints of ug involving edges or nodes of sub.
											// PRCONDITIONS: sub is a set of edges of ug 
		
		gdt::gdtlist<gdtedge> path_corresponding_to_edge (gdtedge, undi_graph&)	const;	// finds a path in *this such that:
											// 	1) two edges of path are adjacent via a vertex marked "RM3_CROSS_ON_REAL_EDGE"
											//	2) one of the edges of path has edge_id equal to that of given gdtedge of undi_graph
		

		

		int count_not_dummy_nodes ()					const;  // return the number of nodes that are not dummy


	public: 									
		/*
		CATEGORY constructors_destructors
		Constructors and destructors.
		*/

			/**			
			Empty constructor. 
			Creates an undi_graph object 
			without nodes and edges.
			@see undi_graph(const undi_graph& ug)
			*/
		undi_graph
			(
			);
			
			

			
			/**
			Destructor. 
			Deallocates the memory required by the undi_graph object.
			*/

		~undi_graph 
			(
			);					
		
			
			
			/**			
			Copy constructor. 
			Creates an undi_graph object, and
			initializes it with the same topology and embedding 
			as ug.
			@param &ug an undi_graph object 
			@see undi_graph()			
			*/		
									
		undi_graph 
			(
			const undi_graph& ug
			);



		/*
		CATEGORY operators
		Operators.
		*/
			
			

			/**			
			Copy operator. 
			Deletes all nodes and edges of the
			undi_graph object, and makes it as a copy of the undi_graph ug.			
			*/	
			
			undi_graph& 
		operator = 
			(
			const undi_graph& ug
			);	

		/*
		CATEGORY access
		Access operations.
		*/


			/* (Hidden to public users)
			Although this method is public, it should be not used by programmers			
			Get all private variables.
			It stores in the "pi" (i=1,..,10) variables all
			the private information of the
			undi_graph object. Namely:
                        <UL>
			<LI> p2 = pointer to the "gdtnode --> struct_undi_node" mapping;
			<LI> p3 = pointer to the "gdtedge --> struct_undi_edge" mapping;
			<LI> p4 = pointer to the list of constraints
			<LI> p5 = pointer to "gdtnode-identifier --> gdtnode" mapping;
			<LI> p6 = pointer to "gdtedge-identifier --> gdtedge" mapping;
			<LI> p7 = pointer to "constraint-identifier --> constraint" mapping;
			<LI> p8 = maximum value of a gdtnode-identifier;
			<LI> p9 = maximum value of an gdtedge-identifier;
			<LI> p10= maximum value of a constraint-identifier.
			<LI> p11= flag for keeping updated the embedding of directed edges
                        </UL>
			*/

			void
		local_get					
			(
			gdt::gdtlist<gdtnode>*& p,
			gdt::gdtlist<gdtedge>*& p0,
			gdt::gdtnode_map<struct_undi_node_info>*& p2,
			gdt::gdtedge_map<struct_undi_edge_info>*& p3,
			gdt::gdtlist<constraint>*& p4,
			gdt::gdtmap<int,gdtnode>*& p5,
			gdt::gdtmap<int,gdtedge>*& p6,
			gdt::gdtmap<int,constraint>*& p7,
			int& p8,
			int& p9,
			int& p10,
			bool& p11
			);
		
			

			
			/* (Hidden to public users)			
			Returns a constant reference to this undi_graph.	
                       	*/
							

			const undi_graph&
		nodes_and_edges
			(
			) const;

			
					
			
			/**			
			Automatically generates a 
			new <I>valid</I> gdtnode identifier for this graph.
			A gdtnode identifier is valid when it is distinct
			from any other gdtnode identifier in the graph.			
			@return a non-negative integer that is a valid gdtnode identifier
			*/	
			
			  
			int 
		generate_node_id        
			(
			);					
			
			
					
			/**			
			Automatically generates a 
			new <I>valid</I> gdtedge identifier for this graph.
			A gdtedge identifier is valid when it is distinct
			from any other gdtedge identifier in the graph.			
			@return a non-negative integer that is a valid gdtedge identifier
			*/	
			  
			int
		generate_edge_id       
			(
			);
			
			
					
			
			/**			
			Automatically generates a 
			new <I>valid</I> constraint identifier for this graph.
			A constraint identifier is valid when it is distinct
			from any other constraint identifier in the graph.			
			@return a non-negative integer that is a valid constraint identifier
			*/
			int 
		generate_constraint_id 
			(
			);					


			
			
			/**
			Returns a gdtnode identifier.
			@param v a gdtnode of this graph.
			@return the identifier of v.
			*/
			
			
			int 
		id 
			(
			gdtnode v
			) const;					
					


			
			/**
			Returns a gdtedge identifier.
			@param e a gdtedge of this graph.
			@return the identifier of e.
			*/	
			int 
		id 
			(
			gdtedge e
			) const;					

			

			
			/**
			Returns a constraint identifier.
			@param c a constraint of this graph.
			@return the identifier of c.
			*/	
			int 
		id 
			(
			constraint c
			) const;					


				
			
			/**
			Returns the maximum value of a gdtnode identifier in this graph.			
			@return the maximum value of a gdtnode identifier in this graph.			 						 			
			*/	
			int 
		get_max_node_id       
			(
			) const;				

			
			
			
			/**
			Returns the maximum value of a gdtedge identifier in this graph.			
			@return the maximum value of a gdtedge identifier in this graph.			 			
			*/
			int 
		get_max_edge_id
			(
			) const;
			
			
			

			/**
			@Returns the maximum value of a constraint identifier in this graph.			 		
			@return the maximum value of a constraint identifier in this graph.			 			
			*/
			int 
		get_max_constraint_id 
			(
			) const;				
			
			
				
			
			/**			
			Counts the number of edges incident on a gdtnode.
			@param v a gdtnode of this graph
			@return the number of edges incident on v			
			*/	
			int
		degree     
			(
			gdtnode v
			) const;					
			
			
			
			
			/**			
			Counts the number of incoming edges incident on a gdtnode.
			@param v a gdtnode of this graph
			@return the number of incoming edges incident on v			
			*/	
			int 
		degree_in  
			(
			gdtnode v
			) const;
			
			
			
			
			/**			
			Counts the number of outgoing edges incident on a gdtnode.
			@param v a gdtnode of this graph
			@return the number of outgoing edges incident on v			
			*/	
			int 
		degree_out
			(
			gdtnode v
			) const;					
			
			
				

			/**	
			Counts the number of nodes of the graph.					 
			@return the number of nodes of the graph.		
			*/	
			int 
		number_of_nodes       
			(
			) const;				

			
			
			
			/**
			Counts the number of edges of the graph.
			@return the number of edges of the graph.
			*/	
			int 
		number_of_edges       
			(
			) const;				

			
			
			
			/**			
			Counts the number of constraints on the graph.
			@return the number of constraints on the graph.
			*/		
			int 
		number_of_constraints 
			(
			) const;
			
			
			

			/**			
			Reads the type of a constraint.
			@param c a constraint on this graph
			@return the constraint_type of c 
			*/
			constraint_type 
		type_of_constraint    
			(
			constraint c
			) const;	

			
				
			
			/**			
			Counts the maximum number of edges incident on a node of this graph.
			@return the maximum number of edges incident on a node of this graph.		
			*/		
			int 
		max_degree     
			(
			) const;					
			

				
			
			/**
			Counts the minimum number of edges incident on a node of this graph.
			@return the minimum number of edges incident on a node of this graph.			
			*/
			int 
		min_degree	   
			(
			) const;					
			
			
				
			
			/**
			Returns the total number of <I>extremal</I> nodes of the graph. 
			A node is extremal if it is a source node 
			(node with only outgoing edges) or if it is a sink node 
			(node with only incoming edges).
			<P><B>NOTICE</B>: An undirected edge is considered both incoming 
				and outgoing.
			@return the total number of extremal nodes of the graph.
			*/
			int 
		number_of_extremals 
			(
			) const;				
			
			
				
			
			/**
			Computes the set <I>C</I> of all connected components of the graph.
			Each connected component is identified by an integer number in the range
			[1, ..., |C|].
			The array <I>component[v]</I> stores the number of
		        the component containing the gdtnode <I>v</I>.
			@param component a gdtnode_array of integer numbers
			@return the total number of connected components, that is, |C|.
			*/
			int
		connected_components
			(
			gdt::gdtnode_array<int>& component
			)  const;


						
			/**			
			Computes the set of all biconnected components of the graph. 
			The graph is visited starting by a specified node. 
			The biconnected components are represented as undi_graph objects.
			Each biconnected component preserves the node and edge
		        identifiers of the original graph.
			<P><B>PRECONDITIONS</B>: the graph must be connected.
			@param v the gdtnode used as the starting point for visiting the graph
			@param node_information[u] stores the dfs_node_info of each node
			@components list of the biconnected components.
			@return the number of biconnected components.	    			
			*/
			int
		biconnected_components
			(
			gdtnode v,
			gdt::gdtnode_map<dfs_node_info*>& vettore_nodi,
			gdt::gdtlist<undi_graph*>& bic
			//list<node>& cut_nodes
			) const;


			/**			
			Informs if a node is an end-node of an edge.
			@param v a gdtnode
			@param e a gdtedge
			@return true if <I>v</I> is an end-node of <I>e</I>, and false otherwise. 			
			*/
			bool 
		node_belongs_to_edge
			(
			gdtnode v, 
			gdtedge e
			) const;
			
			
				
			
			/**
			Informs if an edge is directed.
			@param e a gdtedge
			@return true if <I>e</I> is directed, and false otherwise.
			*/	
			bool 
		edge_is_directed       
			(
			gdtedge e
			) const;  		
			
			
					
			
			/**
			Informs if all edges of the graph are directed.
			@return true if all edges are directed, and false otherwise.
			*/				
			bool 
		all_edges_are_directed 
			(
			) const;  		
			

				
			
			/**
			Checks if the graph is an <I>st-digraph</I>.
			An st-digraph is an acyclic directed graph with a single source
			and a single sink.			
                        @param &s a gdtnode in which the source node is stored in positive case
			@param &t a gdtnode in which the sink node is stored in positive case
			@return true if the graph is an st-digraph.			
			*/
			bool 
		is_st_digraph	    
			(
			gdtnode& s, 
			gdtnode& t
			);
			
			
			
			
			/**
			Checks if the graph is connected.
			@return true if the graph is connected, and false otherwise.
			*/
			bool 
		is_connected   	    
			(
			) const;  		
				


			
			/**
			Checks if the graph is biconnected.
			@return true if the graph is biconnected, and false otherwise..
			*/
			bool 
		is_biconnected 	    
			(
			) const;
				


			
			/**
			Checks if the (undirected) graph is acyclic, i.e., if it is a forest.
			@return true if the graph is acyclic, and false otherwise.
			*/

			bool 
		is_acyclic     	    
			(
			) const;  		
			
			

			/**
			Checks if a gdtnode is <I>candidate</I> (<I>bimodal</I>), that 
			is, if both its incoming edges and its outgoing edges appear consecutive 
			in a circular order. 
			@param v a gdtnode
			@return true if node <I>v</I> is bimodal, and else otherwise. 
			*/	
			bool
		is_candidate           
			(
			gdtnode v
			) const;		

			

			
			/**
			Checks if all nodes are <I>candidate</I> (<I>bimodal</I>). 
			@return true if each node is bimodal.
			@see is_candidate(gdtnode v)
			*/
			bool 
		is_candidate           
			(
			) const;		



			
			/**
			Checks if a node is a source node (without incoming edges) 
			@param v a gdtnode 
			@return true if <I>v</I> is a source node, and false otherwise.                        
			*/	
		
			bool 
		is_source              
			(
			gdtnode v
			) const;		
			
			
				
			
			/**
			Checks if a node is a target (sink) node (without outgoing edges) 
			@param v a gdtnode 
			@return true if <I>v</I> is a target node, and false otherwise.                        
			*/
			bool 
		is_target              
			(
			gdtnode v
			) const;		

			
					
			/*
			Checks if a node is extremal, i.e., if it is a source or a sink node.
			@param v a gdtnode
			@return true if <I>v</I> is extremal, and false otherwise.
			@see number_of_extremals ()
			@see is_source (gdtnode v)
			@see is_target (gdtnode v)
			*/			
			bool 
		is_extremal            
			(
			gdtnode v
			) const;		
			
			
				
			
			/**
			Checks if edge <I>e</I> and its successive edge in the counterclocwise incident 
			list of node <I>v</I> are both incoming or both outgoing.  
			@param v a gdtnode
			@param e a gdtedge incident on <I>v</I>
			@return true if <I>e</I> and its successor are both incoming or both outgoing, and false otherwise.
			<P>
			<B>PRECONDITIONS</B>: <I>v</I> is an end-node of <I>e</I>.			
			*/	
			
			bool 
		is_extremal	    
			(
			gdtnode v, 
			gdtedge e
			) const;		
			
				
			 	
			
			/**
			Checks if node <I>v</I> is an <I>internal</I> vertex, that is, 
			if it is neither a source nor a sink.
			@param v a gdtnode
			@return true if <I>v</I> is internal, and false otherwise. 
			*/	
			bool 
		is_internal            
			(
			gdtnode v
			) const;
			
			
			 	
			
			/**
			Checks if edge <I>e</I> and its successive edge in the counterclocwise incident 
			list of node <I>v</I> have opposite orientation.  
			@param v a gdtnode
			@param e a gdtedge incident on <I>v</I>
			@return true if <I>e</I> and its successor have opposite orientation, and false otherwise.
			<P>
			<B>PRECONDITIONS</B>: <I>v</I> is an end-node of <I>e</I>.			
			*/				 
			bool 
		is_internal	    
			(
			gdtnode v,
			gdtedge e
			) const;		
			
			
			 	

			/**
			Checks if there are some other edges with the same end-node as edge <I>e</I>.
			@param e a gdtedge
			@return true if there are other edges with the same end-nodes as <I>e</I>, and
			false otherwise.
			*/
			bool 
		is_multiple	    
			(
			gdtedge e
			) const;
		
		
			 	
			
			/**
			Checks if node <I>v</I>	has a marker of type <I>m</I>.
			@param v a gdtnode
			@param m a valid marker_type
			@return true if <I>v</I> has a marker of type <I>m</I>, and false otherwise.
			*/
			bool 
		is_marked 
			(
			gdtnode v, 
			marker_type m
			) const;  		
			
			
			 	
			
			/**
			Checks if edge <I>e</I>	has a marker of type <I>m</I>.
			@param e a gdtedge
			@param m a valid marker_type
			@return true if <I>e</I> has a marker of type <I>m</I>, and false otherwise.
			*/
			bool 
		is_marked 
			(
			gdtedge e, 
			marker_type m
			) const;  		
			
			
			 	
			
			/**
			Finds the first node with the specified marker type <I>m</I>.			
			@param m a marker type.
			@return the first gdtnode in the gdtnode-list of the graph that is marked <I>m</I>.			
			*/			 			 
			gdtnode 
		find_first_node_marked_as  
			(
			marker_type m
			) const;
			
			
			 	
			
			/**
			Finds the first edge with the specified marker type <I>m</I>.			
			@param m a marker type.
			@return the first gdtedge in the gdtedge-list of the graph that is marked <I>m</I>.				
			*/			 
			gdtedge 
		find_first_edge_marked_as  
			(
			marker_type m
			) const; 
			
			
			 	
			
			/**
			Finds all nodes having the specified marker type <I>m</I>.
			@param m a marker type.			
			@return the list of gdtnodes marked <I>m</I>.
			*/			 
			gdt::gdtlist<gdtnode>
		find_nodes_marked_as 
			(
			marker_type m
			) const; 
			

			

			/**
			Finds all edges having the specified marker type <I>m</I>.
			@param m a marker type.			
			@return the list of gdtedges marked <I>m</I>.
			*/				 			 
			gdt::gdtlist<gdtedge>
		find_edges_marked_as 
			(
			marker_type m
			) const; 
			
			
			 	
			
			/**
			Finds all edges incident on node <I>v</I> that have the specified marker type <I>m</I>.
			@param v a gdtnode.
			@param m a marker type.			
			@return the list of gdtedges marked <I>m</I> and incident on <I>v</I>.			
			*/			 			 
			gdt::gdtlist<gdtedge> 
		find_adj_edges_marked_as 
			(
			gdtnode v,
			marker_type m
			) const; 
			

			 	
			
			/**
			Finds all edges incident on node <I>v</I> that do not have the specified marker type <I>m</I>.
			@param v a gdtnode
			@param m a marker type
			@return the list of gdtedges not marked <I>m</I> and incident on <I>v</I>.			
			*/
			 
			gdt::gdtlist<gdtedge> 
		find_adj_edges_not_marked_as
			(
			gdtnode v,
			marker_type m
			) const; 
			
			
			 
			/**
			Finds all edges incoming on node <I>v</I> that have the specified marker type <I>m</I>.
			@param v a gdtnode.
			@param m a marker type.			
			@return the list of gdtedges marked <I>m</I> and incoming on <I>v</I>.			
			*/
			 
			gdt::gdtlist<gdtedge> 
		find_in_edges_marked_as  
			(
			gdtnode v,
			marker_type m
			) const; 
			
			
			 	
			
			/**
			Finds all edges outgoing from node <I>v</I> that have the specified marker type <I>m</I>.
			@param v a gdtnode.
			@param m a marker type.			
			@return the list of gdtedges marked <I>m</I> and outgoing from <I>v</I>.			
			*/				
			gdt::gdtlist<gdtedge>
		find_out_edges_marked_as 
			(
			gdtnode v,
			marker_type m
			) const; 
			
			
			
			/**
			Finds the list of the edges with end-nodes <I>v1</I> and <I>v2</I>.
			If the graph has not multiple edges, such a list contains at most one
			element.			
			@param v1 a gdtnode.
			@param v2 a gdtnode.
			@return the list of the edges with end-nodes <I>v1</I> and <I>v2</I>.						
			*/			
			gdt::gdtlist<gdtedge>
		edges_with_extremal_nodes 
			(
			gdtnode v1, 
			gdtnode v2
			) const; 
			
			
			 
			
			/**
			Finds, if it exists, a node shared by the two edges <I>e1</I> and <I>e2</I>.
			@param e1 a gdtedge.
			@param e2 a gdtedge.
			@return the common end-node of <I>e1</I> and <I>e2</I>, if it exists, otherwise 
			NULL_NODE is returned.
			*/						
			gdtnode 
		node_between_adj_edges
			(
			gdtedge e1, 
			gdtedge e2
			) const;
			
			

			
			/**
			Performs a depth first search (DFS) of the underlying undirected graph, starting
			from the specified node <I>v</I>. Information about the DFS tree are returned in the parameters.
			<B>NOTICE</B>: you should initialize all array and list parameters 
			before calling the method.			
			@param v a gdtnode representing the starting node
			@param reached [w] = true if the gdtnode <I>w</I> is visited.
			@param father [w] = father gdtedge of <I>w</I> in the DFS tree.
			@param order    = ordered list of the visited nodes.
			@return false if the underlying undirected graph is acyclic.
			@see complex_DFS		
			*/
			bool 
		simple_DFS 
			(
			gdtnode v, 
			gdt::gdtnode_array<bool>& reached, 
			gdt::gdtnode_array<gdtedge>& father, 
			gdt::gdtlist<gdtnode>& order
			)  const;						


			 
			
			/**
			Performs a breadth first search (BFS) of the underlying undirected graph, starting
			from the specified node <I>v</I>. Information about the BFS tree are returned in the parameters.
			<B>NOTICE</B>: you should initialize all array and list parameters 
			before calling the method.                        
			@param v a gdtnode representing the starting node
			@param reached[w] = true if the gdtnode <I>w</I> is visited.
			@param father[w] = father gdtedge of <I>w</I> in the BFS tree.
			@param dist[w] = minimum distance of <I>w</I> from v
			@param order    = ordered list of the visited nodes.
			@return false if the underlying undirected graph is acyclic.
			*/						
			bool 
		simple_BFS 
			(
			gdtnode v,
			gdt::gdtnode_array<bool>& reached, 
			gdt::gdtnode_array<gdtedge>& father,
			gdt::gdtnode_array<int>& dist, 
			gdt::gdtlist<gdtnode>& order
			) const;			

			

			
			/**
			Performs a depth first search (DFS) of the underlying undirected graph, starting
			from the specified node <I>v</I>. 
			With respect to method simple_DFS, more information about the DFS tree are returned in the parameters.
			<B>NOTICE</B>: you should initialize all array and list parameters 
			before calling the method.
                        @param reached[w] = true if gdtnode <I>w</I> is visited.
			@param reached[w] = true if the gdtnode <I>w</I> is visited.
			@param father[w] = father gdtedge of <I>w</I> in the DFS tree.
			@param lowpt1[w] = the highest ancestor of <I>w</I>, reacheable from <I>w</I> or from 
					a descendant of <I>w</I> with a backward edge.
			@param lowpt2[w] = the second highest ancestor of <I>w</I>, reacheable from <I>w</I> or from 
					a descendant of <I>w</I> with a backward edge.			
			@param num_succ[w] = number of descendants of <I>w</I>.	
			@param root_is_articulated = true if <I>v</I> is a cut-node of the graph.
                   	@return false if the underlying undirected graph is acyclic.
			@see simple_DFS.		
			*/				
				
			bool 
		complex_DFS
			(
			gdtnode v,
			gdtedge e,
			gdt::gdtnode_array<bool>& reached,
			gdt::gdtnode_array<gdtedge>& father,
			gdt::gdtlist<gdtnode>& order,
			gdt::gdtnode_array<gdtnode>& lowpt1,
			gdt::gdtnode_array<gdtnode>& lowpt2,
			gdt::gdtnode_array<int>& num_succ,
			bool& root_is_articulated
			) const;
			
			
			 	  		  
			
			/**			
			Computes the list <I>spanned_edges</I> of the edges in 
			a spanning tree of 
			the graph. The visit starts from the gdtnode <I>start_node</I> 
			(if given). The list "unspanned_edges" is the list of the edges that 
			do not belong to the spanning tree.<P>
		        <B>PRECONDITIONS</B>: lists <I>spanned_edges</I> 
			and <I>unspanned_edges</I> must be empty (checked).			
			@param spanned_edges the computed list of spanned edges
			@param unspanned_edges the computed list of unspanned edges 
			@return false if the graph has no vertices.
			*/			 			 
			bool 
		spanning_tree
			(
			gdt::gdtlist<gdtedge>& spanned_edges, 
		        gdt::gdtlist<gdtedge>& unspanned_edges, 
		        gdtnode start_node = NULL_NODE
		        ) const;			
		        
		        
		         
			
		        /**			
			Computes an unoriented shortest path from node <I>start</I> 
			and node <I>end</I>. 
			The lists <I>edges</I> and <I>nodes</I> represent 
			the ordered sequence of all
			edges and nodes (including nodes <I>start</I> and <I>end</I>) 
			in the path.
			@param start the starting gdtnode
			@param end the ending gdtnode
			@param nodes the ordered list of gdtnodes in the path 
			@param edges the ordered list of gdtedges in the path
			@return the length (number of edges) of the shortest path. 
			*/						
			int 
		unweighted_unoriented_shortest_path
			(
			gdtnode start_node,
			gdtnode end_node,
			gdt::gdtlist<gdtnode>& nodes,
			gdt::gdtlist<gdtedge>& edges
			) const;			


			 
			
			/**			
			Executes an unoriented BFS-visit starting from node <I>v</I> 
			and traversing only nodes that are not in the list <I>marked_nodes</I>.
			The list <I>visited_edges</I> is the list of the visited edges.			
			@param v the starting gdtnode
			@param marked_nodes the list of nodes that cannot be traversed
			@param visited_edges the list of edges visited in the BFS.
			*/			 			 
			void 
		visit_from_node_through_not_marked_nodes 
			(
			gdtnode v, 
			gdt::gdtnode_array<bool>& marked_node, 
			gdt::gdtlist<gdtedge>& visited_edges
			) const;			
		
		
			/**			
			Computes the minimum (if <I>co</I> == MIN) or the maximum (if <I>co</I> == MAX)
			between <I>f[v]</I> and <I>f[w]</I>, where <I>v</I> and <I>w</I> 
			are two nodes of the
			graph, and <I>f</I> is an integer function on the nodes of the graph.
			@param v a gdtnode
			@param w a gdtnode
			@param f a gdtnode_array<int> function
			@param co an enumerate value in {MIN,MAX}
			*/						
			gdtnode 
		compare 
			(
			 gdtnode v, 
			 gdtnode w,
			 gdt::gdtnode_array<int>& f, 
			 compare_option co = MIN
			 ) const;				
			
			

			/**
			Finds the gdtnode with the specified identifier.
			@param id an integer number
			@return the gdtnode with identifier <I>id</I> if it exists, 
			otherwise NULL_NODE is returned.						
			*/	
				
			gdtnode
		find_node 
			(
			int id
			) const;		
			
			
			 	
			
			/**
			@return the first gdtnode in the gdtnode-list of the graph.			
			*/
			gdtnode 
		first_node
			(
			) const;		
			
			
			 	

			/**			
			@return the last gdtnode in the gdtnode-list of the graph.			
			*/
			gdtnode 
		last_node 
			(
			) const;		
			
			
			 	
			
			/**
			@param v a gdtnode
			@return the gdtnode that follows <I>v</I> in the gdtnode-list of the graph.			
			*/ 
			gdtnode 
		succ_node 
			(
			gdtnode v
			) const;		
			
			
			 	
			
			/**
			@param v a gdtnode
			@return the gdtnode that precedes <I>v</I> in the gdtnode-list of the graph.			
			*/			
			gdtnode 
		pred_node 
			(
			gdtnode v
			) const;		

			

			
			/**		
			@param e a gdtedge	
			@returns the source node of edge <I>e</I>.			
			*/
			inline	
			gdtnode 
		source   
			(
			gdtedge e
			) const
			{
			return (*edge_info)[e].source;
			}		
			
			
			 
			
			/**		
			@param e a gdtedge	
			@returns the target node of edge <I>e</I>.			
			*/
			inline	
			gdtnode 
		target
			(
			gdtedge e
			) const
			{
			return (*edge_info)[e].target;
			}		
			
			

			
			/**
			@param v a gdtnode
			@param e a gdtedge
			@return the gdtnode opposite to <I>v</I> on edge <I>e</I>.<P>
			<B>PRECONDITIONS</B>: <I>v</I> is an end-node of <I>e</I>.			
			*/						
			inline
			gdtnode 
		opposite 
			(
			gdtnode v, 
			gdtedge e
			) const
			{
			gdtnode n = NULL_NODE;
			if ( v==source(e) )
				n=target(e);
			else if (v==target(e))
				n=source(e);
			else
				assert(!"wrong arguments to undi_graph::opposite()");
			
			return n;
			}			
			
			
			 	
			/**
			@param e a gdtedge
			@returns the start node of edge <I>e</I> if <I>e</I> is directed, otherwise 
			NULL_NODE is returned.			
			*/
			inline 
			gdtnode 
		start 
			(
			gdtedge e
			) const
			{
			return (*edge_info)[e].start;
			}		
			
			

			
			/**
			@param e a gdtedge
			@returns the stop node of edge <I>e</I> if <I>e</I> is directed, otherwise 
			NULL_NODE is returned.	
			*/			 
			gdtnode 
		stop  
			(
			gdtedge e
			) const;		
		
		
			 
			/**
			@param v a gdtnode
			@return the list of all nodes that are adjacent to <I>v</I>.			
			*/
			gdt::gdtlist<gdtnode>  
		adj_nodes 
			(
			gdtnode v
			) const;		
			
			
			 

			/**			
			@return a constant reference to the gdtnode-list of the graph.			
			*/
			//Modified by A.M. 10/2002
			const 
			gdt::gdtlist<gdtnode>&
		all_nodes 
			(
			) const;		
			
			
			 	
			
			/**
			Finds the edge with the specified identifier.			
			@param id an integer number
			@return the gdtedge with identifier id if it exists,
			otherwise NULL_EDGE is returned.					
			*/
							
			gdtedge 
		find_edge 
			(
			int id
			) const;				
			

			 
			
			/**
			Finds an edge having the specified end-nodes.
			@param v1 a gdtnode
			@param v2 a gdtnode
			@return a gdtedge with end-nodes <I>v1</I> and <I>v2</I> if there 
			exists one, otherwise NULL_EDGE is returned.
			*/			 
			gdtedge
		find_edge
			(
			gdtnode v1,
			gdtnode v2
			) const;
			
			

			
			/**			
			@return the first gdtedge of the gdtedge-list of the graph.
			*/			
			gdtedge 
		first_edge
			(
			) const;	
			
			
			
			/**
			@return the last gdtedge of the gdtedge-list of the graph.			
			*/			
			gdtedge 
		last_edge
			(
			) const;		
			
			
			 
			
			/**
			@param e a gdtedge
			@return the edge that follows <I>e</I> in the gdtedge-list of the graph.
			*/
			gdtedge 
		succ_edge 
			(
			gdtedge e
			) const;		

			
			 
			
			/**
			@param e a gdtedge
			@return the edge that precedes <I>e</I> in the gdtedge-list of the graph.			
			*/			 			 
			gdtedge 
		pred_edge 
			(
			gdtedge e
			) const;		
			
			
			 	
			
			/**
			@param v a gdtnode
			@return the first edge in the list of the incoming edges of <I>v</I>.			
			*/			 			 
			gdtedge 
		first_in_edge  
			(
			gdtnode v
			);				
			
			

			
			/**
			@param v a gdtnode
			@return the first edge in the list of the outgoing edges of <I>v</I>.
			*/			 
			gdtedge 
		first_out_edge
			(
			gdtnode v
			);				
			
			
			 
			
			/**
			@param v a gdtnode
			@return the first edge in the list of incident edges of <I>v</I>.
			*/			 			 
			gdtedge 
		first_adj_edge 
			(
			gdtnode v
			) const;		
		
		
			 

			/**
			@param v a gdtnode
			@return the last edge in the list of incident edges of <I>v</I>.			
			*/			 			 
			gdtedge 
		last_in_edge   
			(
			gdtnode v
			);				

			
			 
			
			/**
			@param v a gdtnode
			@return the last edge in the list of the outgoing edges of <I>v</I>.
			*/			 
			gdtedge 
		last_out_edge  
			(
			gdtnode v
			);				
			
			
			 
			
			/**
			@param v a gdtnode
			@return the last edge in the list of the incident edges of <I>v</I>.			
			*/						
			gdtedge 
		last_adj_edge  
			(
			gdtnode v
			) const;
			
			
			 	
			
			/**
			@param e a gdtedge
			@param v a gdtnode
			@return the edge that follows <I>e</I> in the list of incoming 
			edges of <I>v</I>.<P>
			<B>PRECONDITIONS</B>: <I>e</I> is incoming <I>v</I>.
			*/
			gdtedge 
		in_succ  
			(
			gdtedge e, 
			gdtnode v
			);
			
			
			 
			
			/**
			@param e a gdtedge
			@param v a gdtnode
			@return the edge that follows <I>e</I> in the list of outgoing 
			edges of <I>v</I>.<P>
			<B>PRECONDITIONS</B>: <I>e</I> is outgoing <I>v</I>.		
			*/						
			gdtedge 
		out_succ 
			(
			gdtedge e, 
			gdtnode v
			);				
			
			
			 

			/**
			@param e a gdtedge
			@param v a gdtnode
			@return the edge that follows <I>e</I> in the list of incident 
			edges of <I>v</I>.<P>
			<B>PRECONDITIONS</B>: <I>e</I> is incident on <I>v</I>.
			*/			
			gdtedge
		adj_succ
			(
			gdtedge e,
			gdtnode v
			) const;		
		
		
			 

			/**
			@param e a gdtedge
			@param v a gdtnode
			@return the edge that precedes <I>e</I> in the list of incoming 
			edges of <I>v</I>.<P>
			<B>PRECONDITIONS</B>: <I>e</I> is incoming <I>v</I>.
			*/						
			gdtedge 
		in_pred  
			(
			gdtedge e, 
			gdtnode v
			);				
			
			
			 
			
			/**
			@param e a gdtedge
			@param v a gdtnode
			@return the edge that precedes <I>e</I> in the list of outgoing 
			edges of <I>v</I>.<P>
			<B>PRECONDITIONS</B>: <I>e</I> is outgoing <I>v</I>.
			*/	
			gdtedge 
		out_pred 
			(
			gdtedge e, 
			gdtnode v
			);
			

			 	
			
			/**
			@param e a gdtedge
			@param v a gdtnode
			@return the edge that precedes <I>e</I> in the list of incident 
			edges of <I>v</I>.<P>
			<B>PRECONDITIONS</B>: <I>e</I> is incident on <I>v</I>.		
			*/
			gdtedge 
		adj_pred 
			(
			gdtedge e, 
			gdtnode v
			) const;		
			
			
			 	
			
			/**
			@param e a gdtedge
			@param v a gdtnode
			@return the edge that cyclically follows <I>e</I> in the circular 
			list of incoming edges of <I>v</I>.<P>
			<B>PRECONDITIONS</B>: <I>e</I> is incoming <I>v</I>.			
			*/			 
			gdtedge
		cyclic_in_succ  
			(
			gdtedge e, 
			gdtnode v
			);			
			
			
			 

			/**
			@param e a gdtedge
			@param v a gdtnode
			@return the edge that cyclically follows <I>e</I> in the circular 
			list of outgping edges of <I>v</I>.<P>
			<B>PRECONDITIONS</B>: <I>e</I> is outgoing <I>v</I>.			
			*/									 			 
			gdtedge
		cyclic_out_succ 
			(
			gdtedge e, 
			gdtnode v
			);			
			
			
			 
			
			/**
			@param e a gdtedge
			@param v a gdtnode
			@return the edge that cyclically follows <I>e</I> in the circular 
			list of incident edges of <I>v</I>.<P>
			<B>PRECONDITIONS</B>: <I>e</I> is incident on <I>v</I>.			
			*/			 			 
			gdtedge 
		cyclic_adj_succ 
			(
			gdtedge e, 
			gdtnode v
			) const;		
			
			 	
			
			/**
			@param e a gdtedge
			@param v a gdtnode
			@return the edge that cyclically precedes <I>e</I> in the circular 
			list of incoming edges of <I>v</I>.<P>
			<B>PRECONDITIONS</B>: <I>e</I> is incoming <I>v</I>.			
			*/			 
			 
			gdtedge 
		cyclic_in_pred  
			(
			gdtedge e, 
			gdtnode v
			);			
			
			
			 
			
			/**
			@param e a gdtedge
			@param v a gdtnode
			@return the edge that cyclically precedes <I>e</I> in the circular 
			list of outgoing edges of <I>v</I>.<P>
			<B>PRECONDITIONS</B>: <I>e</I> is outgoing <I>v</I>.			
			*/			
			gdtedge 
		cyclic_out_pred 
			(
			gdtedge e, 
			gdtnode v
			);			
			
			

			
			/**
			@param e a gdtedge
			@param v a gdtnode
			@return the edge that cyclically precedes <I>e</I> in the circular 
			list of incident edges of <I>v</I>.<P>
			<B>PRECONDITIONS</B>: <I>e</I> is incident on <I>v</I>.			
			*/			 
			gdtedge
		cyclic_adj_pred 
			(
			gdtedge e, 
			gdtnode v
			) const;		
			
			

			
			/**
			Finds an edge having the start and the stop end-nodes specified.
			@param v1 a gdtnode			
			@param v2 a gdtnode			
			@return a directed gdtedge having <I>v1</I> as start end-node
			and <I>v2</I> as stop end-node if 
			there exists any, otherwise NULL_EDGE is returned.
			*/			 
			 
			gdtedge 
		find_directed_edge 
			(
			gdtnode v1, 
			gdtnode v2
			);		
			
			
			 
			
			/**
			Finds an edge having the reverse orientation of the specified edge
			@param e a gdtedge
			@return a gdtedge having the reverse orientation of <I>e</I> 
			if there exists one, otherwise NULL_EDGE is returned.<P>
			<B>NOTICE</B>: the method returns NULL_EDGE even if <I>e</I> 
			is undirected.
			*/			 
			 
			gdtedge 
		reverse_of_edge 
			(
			gdtedge e
			);				

			

			
			/**
			@param v a gdtnode
			@return the list of incoming edges of <I>v</I>.			
			*/

			gdt::gdtlist<gdtedge> 
		in_edges  
			(
			gdtnode v
			);				
			
			
			 
			
			/**
			@param v a gdtnode
			@return the list of outgoing edges of <I>v</I>.			
			*/			 
			gdt::gdtlist<gdtedge> 
		out_edges 
			(
			gdtnode v
			);				
			

			 
			
			/**
			@param v a gdtnode
			@return the list of indicent edges of <I>v</I>.			
			*/			
			gdt::gdtlist<gdtedge> 
		adj_edges
			(
			gdtnode v
			) const;			
			
			
			 
			
			/**			
			@return a constant reference to the gdtedge-list of the graph.			
			*/			
			//Modified by A.M. 10/2002
			const 
			gdt::gdtlist<gdtedge>& 
		all_edges
			(
			) const;			
			
			
			 	
			
			/**
			Finds the position of an edge in the list of incident edges of a node.
			@param e a gdtedge
			@param v a gdtnode
			@return the position of <I>e</I> in the list of incident edges of 
			gdtnode <I>v</I>.<P>
			<B>PRECONDITIONS</B>: <I>e</I> is incident on <I>v</I>.
			*/


			gdt::list_item 
		pos_of_edge_in_adj_edges_of_node 
			(
			gdtedge e,
			gdtnode v
			) const;	
			
			

			
			/**
			Finds the constraint with the specified identifier.
			@param id an integer number 
			@return the constraint with identifier <I>id</I> if it exists,
			otherwise NULL_CONSTRAINT is returned. 			
			*/	
			

			constraint 
		find_constraint 
			(
			int id
			) const;		
			
			
			 
			
			/**			
			@return the first constraint in the constraint-list of the graph.			
			*/			 
			constraint 
		first_constraint
			(
			) const;		
			
			

			
			/**			
			@return the last constraint in the constraint-list of the graph.			
			*/					
			constraint 
		last_constraint 
			(
			) const;		

			
			 
			
			/**
			@param c a constraint
			@return the constraint that follows <I>c</I> in the
			constraint-list of the graph.			
			*/
			constraint 
		succ_constraint 
			(
			constraint c
			) const;		
			
			
			 
			
			/**
			@param c a constraint
			@return the constraint that precedes <I>c</I> in the
			constraint-list of the graph.			
			*/
			constraint 
		pred_constraint 
			(
			constraint c
			) const;		
			
			
			 
			
			/**			
			@return a constant reference to the constraint-list of the graph.
			*/

			const 
			gdt::gdtlist<constraint>& 
		all_constraints
			(
			) const;	

			
			 	
			
			/**			
			@param e a gdtedge
			@returns the list of constraints involving <I>e</I>.		
			*/			 
			gdt::gdtlist<constraint> 
		constraints_on_edge 
			(
			gdtedge e
			);	 	
			
			
			 
			
			/**
			@param v a gdtnode			
			@return a list of all constraints involving gdtnode <I>v</I>.			
			*/			
			gdt::gdtlist<constraint> 
		constraints_on_node 
			(
			gdtnode v
			);		 
			
			

			
			/**
			@param v a gdtnode
			@return the list of all markers of gdtnode <I>v</I>.			
			*/			 
			gdt::gdtlist<marker_type> 
		markers
			(
			gdtnode v
			) const;			
			
			
			 
			
			/**
			@param e a gdtedge
			@return the list of all markers of gdtnode <I>v</I>.		
			*/			
			gdt::gdtlist<marker_type> 
		markers
			(
			gdtedge e
			) const;			
		
		
		

		/*
		CATEGORY translate
		Translate operations.
		*/ 
			
			
			/**			
			Searches in this graph a node with the same identifier as
			as node <I>v</I> of graph <I>ug</I>.			
			@param v a gdtnode 
			@param ug an undi_graph
			@return a gdtnode of this graph with the same identifier as <I>v</I> if it exists,
			otherwise NULL_NODE is returned.<P>
			<B>PRECONDITIONS:</B> <I>v</I> is a node of <I>ug</I>.
			*/
 
				
			gdtnode  	   
		corresponding_node 	    
			(
			gdtnode v,
			const undi_graph& ug
			) const;	
			
			
			 
			
			/**
			Searches in this graph an edge with the same identifier as
			as edge <I>e</I> of graph <I>ug</I>.			
			@param e a gdtedge 
			@param ug an undi_graph
			@return a gdtedge of this graph with the same identifier as <I>e</I> if it exists,
			otherwise NULL_EDGE is returned.<P>
			<B>PRECONDITIONS:</B> <I>e</I> is an edge of <I>ug</I>.
			*/		
			 
				
			gdtedge 	   
		corresponding_edge 	    
			(
			gdtedge e,  	 
			const undi_graph& ug
			) const;	

			
			
			
			/**
			Searches in this graph a constraint with the same identifier as
			as constraint <I>c</I> of graph <I>ug</I>.			
			@param c a constraint 
			@param ug an undi_graph
			@return a constraint of this graph with the same identifier as <I>c</I> if it exists,
			otherwise NULL_CONSTRAINT is returned.<P>
			<B>PRECONDITIONS:</B> <I>c</I> is a constraint of <I>ug</I>.
			*/

			constraint 
		corresponding_constraint 
			(
			constraint c, 
			const undi_graph& ug
			) const;
			
			
			 
			
			/**			
			Builds in linear time a mapping <I>node_corr_in_ug</I> 
			between the nodes 
			of this graph and the nodes of graph <I>ug</I> 
			based on node identifiers 
			@param ug an undi_graph
			@param node_corr_in_ug the gdtnode_map<gdtnode> constructed as output<P> 
			<B>NOTES:</B> <I>node_corr_in_ug</I> is initialized on the nodes of this graph.
			*/			 
			 
			void 
		build_mapping_node_to_node_with_undi_graph 
			(
			const undi_graph& ug, 
			gdt::gdtnode_map<gdtnode>& node_corr_in_ug
			) const; 
			
			
			 

			/**
			Builds in linear time a mapping <I>edge_corr_in_ug</I> 
			between the edges 
			of this graph and the edges of graph <I>ug</I> 
			based on edge identifiers 
			@param ug an undi_graph
			@param edge_corr_in_ug the gdtedge_map<gdtedge> constructed as output<P> 
			<B>NOTES:</B> <I>edge_corr_in_ug</I> is initialized on the nodes of this graph.
			*/			 
			 
			void 
		build_mapping_edge_to_edge_with_undi_graph
			(
			const undi_graph& ug,
			gdt::gdtedge_map<gdtedge>& edge_corr_in_ug
			) const;	
		
		
	   
		/*
		CATEGORY update
		Update operations.
		*/		
		
			
			/**			
			Adds to this graph a new node with identifier <I>new_id</I>.
			If not specified, <I>new_id</I> is automatically assigned
			as the integer that immediately follows the maximum 
			node identifier in the graph.
			@param new_id an integer number
			@return the gdtnode created.<P>
			<B>PRECONDITIONS:</B> if specified, <I>new_id</I> 
			is non-negative and unique.
 			*/


			gdtnode 
		new_node 
			(
			int new_id=AUTO_ID
			); 
			
			
			 			
			
			/**						
			Adds to this graph a new node with identifier <I>new_id</I>.
			If not specified, <I>new_id</I> is automatically assigned
			as the integer that immediately follows the maximum 
			node identifier in the graph. Also, the new node
			is attached to all nodes in the list L, in the same ordering
			they occur.
			@param L, a list of nodes
			@param new_id an integer number			
			@return the gdtnode created.<P>
			<B>PRECONDITIONS:</B> if specified, <I>new_id</I> is 
			non-negative and unique. The nodes in <I>L</I> 
			belongs to this graph.
 			*/									
			gdtnode 
		new_node 
			(
			gdt::gdtlist<gdtnode> L,
			int new_id=AUTO_ID
			);
			

			 			
			
			/**			
			Adds to this graph a new undirected edge connecting 
			<I>v1</I> (the source node) and <I>v2</I> (the target node).
			If not specified, <I>new_id</I> is automatically assigned
			as the integer that immediately follows the maximum 
			edge identifier in the graph.
			@param v1 a gdtnode
			@param v2 a gdtnode
			@param new_id an integer number
			@return the gdtedge created.<P>	
			<B>PRECONDITIONS:</B> if specified, <I>new_id</I> is 
			non-negative and is unique. 						
			*/			 			 
			gdtedge 
		new_edge 
			(
			gdtnode v1, 
			gdtnode v2, 
			int new_id=AUTO_ID
			);					
			
			
			 			
			
			/**
			Adds to this graph a new undirected edge connecting 
			<I>v1</I> (the source node) and <I>v2</I> (the target node).
			If not specified, <I>new_id</I> is automatically assigned
			as the integer that immediately follows the maximum 
			edge identifier in the graph. The new edge is inserted 
			after(before) gdtedge <I>e1</I> around <I>v1</I>.
			@param v1 a gdtnode
			@param e1 a gdtedge
			@param v2 a gdtnode
			@param new_id an integer number
			@param dir a value between {<I>after</I> (default), <I>before</I>}
			@return the gdtedge created.<P>	
			<B>PRECONDITIONS:</B> if specified, <I>new_id</I> is non-negative
			and is unique. Edge <I>e1</I> is an incident edge of <I>v1</I>. 					
			*/
			
			gdtedge 
		new_edge 
			(
			gdtnode v1, 
			gdtedge e1, 
			gdtnode v2, 
			int new_id=AUTO_ID,
			int dir=gdt::after
			);




			/**
			Adds to this graph a new undirected edge connecting 
			<I>v1</I> (the source node) and <I>v2</I> (the target node).
			If not specified, <I>new_id</I> is automatically assigned
			as the integer that immediately follows the maximum 
			edge identifier in the graph. The new edge is inserted 
			after(before) gdtedge <I>e1</I> around <I>v1</I> and 
			after(before) gdtedge <I>e2</I> around <I>v2</I>.
			@param v1 a gdtnode
			@param e1 a gdtedge
			@param v2 a gdtnode
			@param e2 a gdtedge
			@param new_id an integer number
			@param dir a value between {<I>after</I> (default), <I>before</I>}
			@return the gdtedge created.<P>	
			<B>PRECONDITIONS:</B> if specified, <I>new_id</I> is non-negative
			and is unique. Edge <I>e1</I> is an incident edge of <I>v1</I> and 
			edge <I>e2</I> is an incident edge of <I>v2</I>.
			*/

			gdtedge
		new_edge
			(
			gdtnode v1,
			gdtedge e1,
			gdtnode v2,
			gdtedge e2,
			int new_id=AUTO_ID,
			int dir=gdt::after
			);




			/**			
			Adds to this graph a new constraint that is a perfect copy
			of the given constraint <I>c</I> (the identifier is also preserved).
			@param c a constraint
			@return the new constraint<P>
                	<B>PRECONDITIONS:</B> <I>c</I> is consistent with the
                	graph topology and belongs to another graph.
			*/

                	constraint
                new_constraint
                	(
                	constraint c
                	);




			/**
			Adds to this graph a new constraint imposing 
			that edge <I>e</I> can not be crossed in the 
			planarization procedure.
			The new constraint will have identifier <I>new_id</I>;
			if not specified, <I>new_id</I> will be assigned as
			the integer number that immediately follows the maximum
			constraint identifier in the graph.
			@param e a gdtedge
			@param new_id an integer number
			@return the new constraint.<P>
			<B>PRECONDITIONS:</B> if specified, <I>new_id</I> is 
			non-negative and is unique. 			
			*/

                	constraint
                new_constraint_uncrossable_edge
                	(
                	gdtedge e,
                	int new_id=AUTO_ID
                	);




			/**
			Adds to this graph a new constraint on the number
			of bends that edge <I>e</I> can have in a drawing.			
			The new constraint will have identifier <I>new_id</I>;
			if not specified, <I>new_id</I> will be assigned as
			the integer number that immediately follows the maximum
			constraint identifier in the graph.
			@param e a gdtedge
			@param b a bend_constraint; possible value for <I>b</I> are:
			<UL>
                	<LI>NONE: no bend on gdtedge (straight gdtedge)
                	<LI>ANY	: any number of bends on gdtedge (zero cost bend on gdtedge).
                        <UL>						
			@param new_id an integer number
			@return the new constraint.<P>
			<B>PRECONDITIONS:</B> if specified, <I>new_id</I> is 
			non-negative and is unique. 		
			*/
                	constraint
                new_constraint_number_of_bends_on_edge
                	(
                	gdtedge 		e,
                	bend_constraint b,
                	int 		new_id=AUTO_ID
                	);




                	/**			
			Adds a new constraint on the bend direction
			of edge <I>e</I>. More precisely,
			the constraint means that <I>e</I>
			can bend only to the right while moving from 
 			<I>v</I>. The new constraint will have identifier <I>new_id</I>;
			if not specified, <I>new_id</I> will be assigned as
			the integer number that immediately follows the maximum
			constraint identifier in the graph.
			@param e a gdtedge
			@param v a gdtnode
			@param new_id an integer number
			@return the new constraint.<P>
			<B>PRECONDITIONS:</B> if specified, <I>new_id</I> is 
			non-negative and is unique. Edge <I>e</I> is incident on <I>v</I>.
			*/

                	constraint 
		new_constraint_bend_direction_on_edge 
                	(
                	gdtedge 	e, 
                	gdtnode	v,
                	int 	new_id=AUTO_ID
                	);				
          	
          	
          		 	
			
                	/**			
			Adds a new planarization constraint
			imposing that node <I>v</I> belong to the same face
                	as all other nodes having the same type of constraint 
                	with the same <I>face_label</I>.
			@param v a gdtnode
			@param face_label an integer label
			@param new_id an integer number
			@return the new constraint.<P>
			<B>PRECONDITIONS:</B> if specified, <I>new_id</I> is 
			non-negative and is unique.			
			*/		
			
			constraint 
		new_constraint_same_face_node
			(
			gdtnode		v, 
			int 		face_label, 
			int 		new_id=AUTO_ID
			); 
			
			
			 	
			
			/**
			Adds a new contraint same_face_node for each node
			in the list <I>Ln</I>.
			@param Ln a gdtlist of gdtnodes
			@param face_label an integer number
			@return the list of new constraints.
			@see new_constraint_same_face_node (gdtnode, int, int) 
			*/			 	
			 	
			gdt::gdtlist<constraint> 
		new_constraint_same_face_node
			(
			gdt::gdtlist<gdtnode>	Ln, 
			int 		face_label 
			);  

			
			 	
			
			/**			
			Adds a new planarization constraint
			imposing that all the nodes in the given list <I>Ln</I>
			will belong to the same face in the order they 
			are given while walking on the border of the face.
			@param Ln a gdtlist of gdtnodes
			@param face_label an integer number
			@return the list of new constraints.<P>
			<B>PRECONDITIONS:</B> if specified, <I>new_id</I> is 
			non-negative and is unique.
			*/			
			
			constraint
		new_constraint_same_face_ordered_nodes
			(
			gdt::gdtlist<gdtnode>	Ln, 
			int 		new_id=AUTO_ID
			); 



			/**			
			Adds a new constraint on the width of a node.
			This means that the node should have the specified
			width (in terms of grid units) in a drawing of the graph.
			@param n a gdtnode
			@param x a real number
			@param new_id an integer number
			@return the new constraint.<P>
			<B>PRECONDITIONS:</B> if specified, <I>new_id</I> is 
			non-negative and is unique.
			*/			
							
                	constraint 
                new_constraint_node_width
                	(
                	gdtnode n,
                        double x,
                	int new_id=AUTO_ID
                	);			
       


			/**			
			Adds a new constraint on the height of a node.
			This means that the node should have the specified
			height (in terms of grid units) in a drawing of the graph.
			@param n a gdtnode
			@param x a real number
			@param new_id an integer number
			@return the new constraint.<P>
			<B>PRECONDITIONS:</B> if specified, <I>new_id</I> is 
			non-negative and is unique.
			*/
							
                	constraint 
                new_constraint_node_height
                	(
                	gdtnode n, 
                        double x,
                	int new_id=AUTO_ID
                	);			
                	
                	
			
			/*			
			Adds a new constraint			
                	imposing an angle of <I>alpha</I> degrees  
			to the right of edge <I>e</I>, starting from
			node <I>v</I>, in a drawing of the graph.
			@param rn a gdtnode
			@param re a gdtedge
			@param alpha an angle_type value in the set 
			{_000,_090, _180, _270, _360} 
			@return the new constraint.<P>
			<B>PRECONDITIONS:</B> if specified, <I>new_id</I> is 
			non-negative and is unique. <I>e</I> is incident on <I>v</I>.
			*/				
                	constraint 
                new_constraint_angle_sweep
                	(
                	gdtnode rn, 
                        gdtedge re,
			angle_type alpha,
                	int new_id=AUTO_ID
                	);				
			
			
			/**			
			Adds a new constraint
                	imposing that edge <I>e</I> attaches to vertex <I>v</I>
			with a distance <I>value</I> from the previous corner 
			of the vertex, in a drawing of a graph. 
			The previous corner is the corner preceding 
			the side which the edge is incident to.
			@param rn a gdtnode
			@param re a gdtedge
			@param value an integer number
			@param new_id an integer number
			@return <B>PRECONDITIONS:</B> if specified, <I>new_id</I> is 
			non-negative and is unique. <I>e</I> is incident on <I>v</I>.
			*/
							
                	constraint 
                new_constraint_edge_attachment_wrt_previous_corner
                	(
                	gdtnode rn, 
                        gdtedge re,
			int value,
                	int new_id=AUTO_ID
                	);				
			
			/*
			THIS CONSTRAINT DOES NOT WORK CORRECTLY
			METHOD new_constraint_min_tieing
			*/
							
                	constraint 
                new_constraint_min_tieing
                	(
			int value,
                	int new_id=AUTO_ID
                	);				
						
			/**
			Delete the node <I>v</I> from this graph.
			@param v a gdtnode
			*/
		 
			void 
		del_node 
			(
			gdtnode v
			); 
			

			 			

			/**
			Removes the edge <I>e</I> from this graph.
			@param e a gdtedge
			*/
			
			void 
		del_edge 
			(
			gdtedge e
			); 
                	
                	
                	 			
			
			/**
			Removes the constraint <I>c</I> from this graph.
			@param c a constraint
			*/
						
                	void 
                del_constraint
                	(
                	constraint c
                	);  
                	
                	

			
			/**
			Removes all constraints with <I>ct</I> type from this graph.
			@param ct a constraint type
			*/
			
                	void 
                del_constraints_type
                	(
                	constraint_type ct
                	);
                	
                	
                	 				
			
			/**
			Removes all constraints from the graph.
			*/
			 	
                	void 
                del_all_constraints 
                	(
                	); 
                	
                	
                	 				
			
			/**
			Removes from this graph all constraints involving the edge <I>e</I>".
			@param e a gdtedge
			*/
			
                	void 
                del_all_constraints_involving_edge 
                	(
                	gdtedge e
                	);
			
			
			 				
			
			/**
			Removes from this graph all constraints involving the node <I>v</I>".
			@param v a gdtnode
			*/


			void 
		del_all_constraints_involving_node
			(
			gdtnode v
			);					
		
		
			 	
			
			/**
			Removes all constraints with <I>ct</I> type involving the edge <I>e</I>.
			@param ct a constraint type
			@param e a gtdtedge
			*/
			
			void
		del_constraints_type_involving_edge
			(
			constraint_type ct,
			gdtedge e
			);
			
			
			 	
			
			/**
			Removes all constraints with <I>ct</I> type involving the node <I>v</I>.
			@param ct a constraint type
			@param v a gtdtnode
			*/

			void
		del_constraints_type_involving_node
			(
			constraint_type ct,
			gdtnode v
			);
			
			

			
			/**
			Removes all nodes and edges of the graph.
			*/
			 	
			void 
		clear
			(
			);						
			
			
			 			
			
			/**
			Copies all private variables 
			(pointers to internal data strauctures) 
			of undi_graph in this graph.<P>
			<B>NOTICE:</B> the owner graph of constraints of undi_graph 
				is replaced with this graph.<P>
			<B>NOTICE:</B> a @forget method on undi_graph should follow 
				this method to avoid collisions. 
			*/
			 	
			void 
		mirror 
			(
			undi_graph& ug
			);			
			
			
			 	
			
			/**
			Cut all private variables
			(pointers to internal data structures) from this graph.
			*/

			void 
		forget 
			(
			);				
			
			
			 		
			
			/**
			METHOD assign_id
			Assign identifier "new_id" to gdtnode "v".<P>
			PRECONDITIONS: there is not already a gdtnode with such 
				       identifier.
			*/
			 	
			void 
		assign_id 
			(
			gdtnode v, 
			int new_id = AUTO_ID
			);				
			
			
			 			
			
			/**
			Assign identifier "new_id" to gdtedge "e".<P>
			PRECONDITIONS: there is not already any gdtedge with such
				       identifier.
			*/
			
			void 
		assign_id 
			(
			gdtedge e,
			int new_id = AUTO_ID
			);
			
			
			 			
			
			/**
			Assign identifier "new_id" to constraint "c".<P>
			PRECONDITIONS: there is not already any constraint with 
				       such identifier.
			*/
			
			void 
		assign_id 
			(
			constraint c, 
			int new_id = AUTO_ID
			);			
			
			
			 	
			
			/**
			Mark gdtnode "v" with marker "m".
			*/
			
			void
		mark  
			(
			gdtnode v, 
			marker_type m
			);  	   			

			
			 				

			/**
			Mark gdtedge "e" with marker "m".
			*/
			 	
			void 
		mark  
			(
			gdtedge e, 
			marker_type m
			);
			
			
			 					
			
			/**
			Mark gdtnode "v" with all markers in the list "ml".
			*/
			
			void 
		mark  
			(
			gdtnode v, 
			gdt::gdtlist<marker_type> ml
			); 						
			
			


			/**
			Mark gdtedge "e" with all markers in the list "ml".
			*/
			 	
			void 
		mark  
			(
			gdtedge e, 
			gdt::gdtlist<marker_type> ml
			); 						

			
			 			
			
			/**
			y marker "m" from the marker-list of gdtnode "v".
			*/
			
			void 
		unmark
			(
			gdtnode v, 
			marker_type m
			);  	   						
			
			
			 			
			
			/**
			Remove marker "m" from the marker-list of gdtnode "v".
			*/
			
			void 
		unmark
			(
			gdtedge e, 
			marker_type m
			);  	   						
			
			

			
			/**
			Remove all markers from the marker-list of gdtnode "v".
			*/
			 	
			void 
		unmark_all
			(
			gdtnode v
			);                   		
			

			 		
			
			/**
			Remove all markers from the marker-list of gdtedge "e".
			*/
			 
			void 
		unmark_all	
			(
			gdtedge e
			);
			
			
			 		
			
			/**
			Make the ordering of the edges around each vertex equal
			to the ordering in graph "ug".<P>
			NOTICE: this graph and "ug" must have the same number
				of nodes and edges, with the same identifiers.
			*/
			 
			void 
		make_embedding_as 
			(
			const undi_graph& ug
			);
			

			 		
			
			/**
			Make the ordering of the edges around vertex "v" as
			candidate, that is both the incoming and outgoing edges
			will be consecutive around "v".
			*/
			
			void 
		make_embedding_cand 
			(
			gdtnode v
			);
			
			
			 		
			
			/**
			Make the ordering of the edges around each vertex as 
			candidate, that is both the incoming and outgoing edges
			will be consecutive around each vertex.
			*/

			void 
		make_embedding_cand 
			(
			);					
			
			
			 		
			
			/**
			Make the ordering of the edges around vertices as
			planar (planar embedding), if there exists any. 
			This means that a planar drawing  exists within such an
			ordering.
			Return false when no planar embeddings exist. 
			*/
			
			bool 
		make_embedding_planar
			(
			);	         			
			
			
			 			
			
			/**
			Make the embedding of the graph both candidate 
			and planar (see above), if there exists any.
			Return false when no candidate planar embeddings exist.
			*/
			 
			bool 
		make_embedding_cand_planar
			(
			);					
			

			
			
			/**
			Make the embedding of the graph as candidate and expand 
			each vertex 'v' having at least 2 incoming edges and 2
			outgoing edges. The expansion consists in splitting
			such a vertex 'v' into two vertices 'v1' and 'v2', 
			the first keeping only the incoming edges of 'v' and the
			second keeping only the outgoing edges of 'v'; also, a 
			directed gdtedge (v1,v2) is added, and marked
			RM3_ADDED_BY_EXPAND.
			Return the list of dummy edges introduced. 
			*/
			 
			gdt::gdtlist<gdtedge> 
		make_embedding_cand_expanded 
			(
			);			
			
			
			 
			
			/**
			Make the graph simple, that is add a minimum number of 
			dummy nodes in order to remove multiple edges. 
			Each dummy gdtnode is marked RM3_ADDED_BY_MAKE_SIMPLE.
			Return the list of dummy nodes. 
			*/
			 
			gdt::gdtlist<gdtnode> 
		make_simple 
			(
			);			

			

			/**
			Make the graph connected, that is add a minimum number of
			edges in order to attach all connected components. 
			Each dummy gdtedge is marked RM3_ADDED_BY_MAKE_CONNECTED.
			Return the list of dummy edges inserted. 
			*/
			 
			gdt::gdtlist<gdtedge>
		make_connected 
			(
			);
				 
			 


			/**
			Make the graph biconnected, that is add a set of edges between the connected components of the graph.
			This method preserves the planarity of the graph. If the original graph is planar, then also the augmented graph will be planar.
			@return the list of the added edges
			*/
			
			gdt::gdtlist<gdtedge>
		make_biconnected
			(
			) ;
			
			/**
			Delete a minimal set of edges in order to have each 
			gdtnode with degree at most "d" (that is with at most d 
			incident edges). 
			Return the number of deleted edges.<P>
			NOTICE: the graph could be not connected after this 
				method is applied.
			*/
								
			int  
		make_max_degree 
			(
			int d
			);				

			
			 
			
			/**
			Make gdtedge "e" directed from gdtnode "v", that is "v" will
			be the start gdtnode of "e".<P>
			PRECONDITIONS: "e" is incident "v".
			*/
			
			void 
		make_directed 
			(
			gdtedge e, 
			gdtnode v
			);


			 			
			
			/**
			Make directed all edges of the graph.
			Each gdtedge is oriented randomly if "random" is true, and
			from its source if "random" is false.
			*/
			
			void 
		make_directed 
			(
			bool randomly = false
			);			
			
			
			 
			
			/**
			Make an st-orientation of the edges with source "s" 
			and sink "t" (see "is_st_digraph()" method).<P>
			PRECONDITIONS: the graph plus an gdtedge (s,t) is 
				       biconnected and "s", "t" are distinct.
			*/

			void
		make_directed 
			(
			gdtnode s, 
			gdtnode t
			);    				
			
			
			 	

			/**
			Make gdtedge "e" undirected.
			*/
			 
			void 
		make_undirected 
			(
			gdtedge e
			);										
			
			
			 		
			
			/**
			Make all edges of the graph undirected.
			*/
			
			void 
		make_undirected 
			(
			);								
			
			
			 		
			
			/**
			Make gdtnode "v" as a source-gdtnode, that is all the edges
			incident "v" will be made as outgoing edges of "v".
			*/
			 
			void
		make_source     
			(
			gdtnode v
			);
			

			 		
			
			/**
			Make gdtnode "v" as a sink-gdtnode, that is all the edges 
			incident "v" will be made as incoming edges of "v".
			*/
			
			void 
		make_sink       
			(
			gdtnode v
			);
			
			
			 		
			
			/**
			Reverse the direction of gdtedge "e".
			*/
			 
			void 
		reverse 	     
			(
			gdtedge e
			);
			
			
			 		
			
			/**
			Computes an st_numbering of the graph,
			with source gdtnode "s" and sink gdtnode the opposite of "s"
			in gdtedge "e_st".<P>
			PRECONDITIONS: graph is biconnected and "e_st" is 
				       incident "s".
			*/
			 
			void 
		st_number 					
			(
			gdtedge e_st, 
			gdtnode s, 
			gdt::gdtnode_array<int>& st_num
			); 								


			/**
			Reverse all adjacent edges lists of each vertex
			WARNING: not applicable for plan_undi_graph
			*/
			void
		reverse_adj_edges_lists
			(
			);	
		
		// ***************************************************************
		// ---------------------------------------------------------------
		// THESE FUNCTIONS ARE INTENDED FOR st-ORIENTED BICONNECTED GRAPHS 
		// ---------------------------------------------------------------
			
			 		
			
			/**
			Return the topological level of gdtnode "v" in an ordering 
			induced by an st-numbering 
			(recursive function).<P>
			PRECONDITIONS: graph is st-oriented
			*/
			
			int
		calculate_level_of_node 	  
			(
			gdtnode v,
			gdt::gdtnode_array<int>& levels_buffer
			);				
			
			
			 			

			/**
			Return the level of all nodes in an ordering induced by 
			an st-numbering.<P>
		 	PRECONDITIONS: graph is st-oriented
			*/
			 
			void 
		calculate_level_of_all_nodes 
			(
			gdt::gdtnode_array<int>& levels_buffer
			);					
			
		// ***************************************************************
		
			
			
			 		
			
			/**
			Make one only gdtedge by merging the two edges incident 
			gdtnode "v" and delete gdtnode.
			PRECONDITIONS: gdtnode "v" has degree 2.
			*/
			
			gdtedge 
		weld_edges_at_node 
			(
			gdtnode v
			);		
			
			
			 
			
			/**
			Split gdtnode "v" into two nodes with gdtedge-lists
			L1=("e_init","e_end") and 
			L2=(cyclic_adj_succ(e_end,v), cyclic_adj_pred(e_init,v)),
			respectively, by inserting a new dummy gdtedge. 
			The dummy gdtedge is marked RM3_ADDED_BY_EXPAND.
			Return the dummy gdtedge.
			*/
			
			gdtedge 
		expand 	
			(
			gdtnode v, 
			gdtedge e_init, 
			gdtedge e_end
			);				
			
			 
			
			/**
			Merge the extremal nodes 'v1', 'v2' of gdtedge "e" into a 
			gdtnode 'v' and make adj-edges(v) = 
			adj-edges(v1) U adj-edges(v2) \ {e: e=(v1,v2)}.
			Return gdtnode 'v'.
			*/
								
			gdtnode 
		contract 
			(
			gdtedge e
			);					
			
			
			 			
			
			/**
			Apply a contract(v) operation for each gdtnode 'v'.
			(see above for more details).
			Return the list of nodes derived by all contractions.
			*/
								
			gdt::gdtlist<gdtnode> 
		contract 
			(
			);					
			
			
			 
			
			/**
			Update the 'max_node_id' value to the current maximum 
			gdtnode-identifier.
			*/
			
			void 
		update_max_node_id       
			(
			);				
			
			
			 
			
			/**
			Update the 'max_edge_id' value to the current maximum 
			gdtedge-identifier.
			*/
			 
			void 
		update_max_edge_id       
			(
			);				
			
			

			
			/**
			Update the 'max_constraint_id' value to the current 
			maximum constraint-identifier. 
			*/
			
			void 
		update_max_constraint_id 
			(
			);			
			
			
			 		
			
			/**
			Update 'max_node_id' (gdtnode identifier) value with 
			"new_max_node_id".<P>
			PRECONDITIONS: "new_max_node_id" is greater than 
				       'max_node_id'.
			*/
			 
			void 
		rise_max_node_id       
			(
			int new_max_node_id
			);			


			 		
			
			/**
			Update "max_edge_id" (gdtedge identifier) value with 
			"new_max_edge_id".<P>
                        PRECONDITIONS: "new_max_edge_id" is greater than 
				       'max_edge_id'.
			*/

			void 
		rise_max_edge_id       
			(
			int new_max_edge_id
			);			

			
			 			
			
			/**
			Update max_constraint_id (constraint identifier) value 
			with "new_max_constraint_id".<P>
			PRECONDITIONS: "new_max_constraint_id" is greater than
				       'max_edge_id'.
			*/
			
			void 
		rise_max_constraint_id 
			(
			int new_max_constraint_id
			);				
		

			 
			
			/**
			Perform a planarization of the graph, introducing dummy 
			nodes in order to make the embedding planar if they are 
			needed.<P>

			Nothing assures that the number of dummy nodes 
			introduced is minimum (the problem of finding the 
			minimum number of crossing is NP-complete).
			All dummy nodes are marked RM3_CROSSES_ON_REAL_EDGES.
			Parameter 'planarize_options po' specifies the type of 
			planarization, chosen in the following list:
                        <UL>
			
				<LI> DO_NOT_PRESERVE_EMBEDDING 
				<LI> PRESERVE_EMBEDDING 
			</UL>	
			In the first case the ordering of the edges around vertices
			could be changed by the algorithm. In the second case, the
			algorithm does not change this ordering. <P>
							 
			Return the number of dummy nodes added (INFINITE when 
			no embedding is found due to infeasible constraints).		 
			*/
			 
			int  
		planarize	
			(
			planarize_options po = DO_NOT_PRESERVE_EMBEDDING
			); 


			 
			
			/**
			Replace gdtedge "e" with an ordered path of nodes and 
			edges with identifiers given in the "node_id_path" and 
			in the "edges_id_path" lists, respectively.
			Return the list of the new edges added. <P>
			PRECONDITIONS: the extremal nodes of gdtedge "e" have the 
				       same identifier as the first and the 
				       last gdtnode in the "node_id_path" list. 
			*/
				
			gdt::gdtlist<gdtedge> 
		replace_edge_with_path 
			(
			gdtedge e, 
			gdt::gdtlist<int>& node_id_path,
			gdt::gdtlist<int>& edge_id_path
			);

			
			 
			
			/**
			Replace gdtedge "e" with an ordered path of nodes and edges 
			that is a copy of "edge_path" in the graph "ug".
			Return the list of new edges added. <P>
			PRECONDITIONS: the extremal nodes of gdtedge "e" have the 
				       same identifier as two nodes in the first 
				       and the last edges in the "edge_path" 
				       list.
			*/
			
			gdt::gdtlist<gdtedge> 
		replace_edge_with_path_of_graph 
			(
			gdtedge e, 
			gdt::gdtlist<gdtedge>& edge_path, 
			undi_graph& ug
			);
			
			

			
			/**
			Make the graph as a randomly generated planar 
			biconnected graph with "n" nodes and keeping it 
			"k"-planar (that is each vertex has degree at most k). 
			Also, if "multiple" is false, no multiple edges are 
			generated.<P>
			The basic technique used to generate graphs is the
			following:
                        <OL>
			<LI> start from a triangle graph;
			<LI> at each step is randomly chosen either the 
			     operation "insert vertex" (split gdtedge) or the 
			     operation "insert gdtedge";
			<LI> the "insert vertex" operation is chosen with 
			     probability "prob_iv", and the "insert gdtedge" 
			     operation is selected with probability 
			     1 - "prob_iv", where   0 < "prob_iv" < 1.
                        </OL>
			*/
			
			void 
		generate_plan_biconnected 
			(
			int n, 
			double prob_iv, 
			int k=INFINITE,
			bool multiple = true
			);
		
		
			/**
			Disable the maintenance of directed edges, that is the lists of in- and out-edges are not 
			updated consistently with the embedding of all underlying edges
			ADVICE: useful to save computation-time if you are not interested in the embedding 
			of directed edges.
			*/
			
			void 
		disable_keep_ordering_of_directed_edges 
			();
			
		
			/**
			Enable the maintenance of the ordering
			of directed edges, that is the lists of in- and out-edges are always
			updated consistently with the embedding of all underlying edges
			NOTICE: an immediate update is done
			*/
			
			void 
		enable_keep_ordering_of_directed_edges 
			();
							
		

		/*
		CATEGORY io_operations
		Input / output operations.
		*/ 
		 	
			
			/**
			Write the graph in a file with name "file_name".
			A GDToolkit file is organized in sections, each one
			independent from each other, and without a specific
			ordering to keep.
			Each class in the GDToolkit library has a dedicated 
			section, and it is possible to omit some of these 
			sections in order to store only the information that 
			are needed (for example only the topology and not
			drawing information).
			The file format restricted to the undi_graph section
			information is organized as follows.
			There are two tags &lt;UNDISECTION>, &lt;/UNDISECTION> at 
			the beginning and at the end of the section, 
			respectively. The topology of the graph is described
			by the ordered list of the edges around each gdtnode.
			Namely, you have to specify the gdtnode-identifier and
			the ordered list of the gdtedge-identifiers for this
			gdtnode. The two tags &lt;NODELIST>, &lt;/NODELIST> indicate
			the beginning and the end of the nodes list, 
			respectively. Before putting a new gdtnode-identifier
			a tag &lt;NODE> is needed, then you have to put a tag
			&lt;EDGE> before each gdtedge-identifier that describes
			an gdtedge incident the gdtnode. Finally, a tag &lt;/NODE>
			indicates the end of the edges incident the gdtnode.
			An example of an undi_graph section is shown:<P>
                        
                        <!--
			<UNDISECTION>
        		       <NODELIST>
                			<NODE>
                        			1
                        			<EDGE> 2 ->
                        			<EDGE> 1 ->
                        			<EDGE> 3 ->
                			</NODE>
                			<NODE>
                        			2
                        			<EDGE> 4 ->
                        			<EDGE> 5 ->
                        			<EDGE> 8 <-
                        			<EDGE> 6 ->
                			</NODE>
                		...
                		...
                		...
        			</NODELIST>
			</UNDISECTION>
			-->

			&lt;UNDISECTION><UL>
        		       &lt;NODELIST><UL>
                			&lt;NODE><UL>
                        			1<BR>
                        			&lt;EDGE> 2 -><BR>
                        			&lt;EDGE> 1 -><BR>
                        			&lt;EDGE> 3 -><BR>
                			</UL>&lt;/NODE><BR>
                			&lt;NODE><UL>
                        			2<BR>
                        			&lt;EDGE> 4 -><BR>
                        			&lt;EDGE> 5 -><BR>
                        			&lt;EDGE> 8 <-<BR>
                        			&lt;EDGE> 6 -><BR>
                			</UL>&lt;/NODE><BR>
                		...<BR>
                		...<BR>
                		...<BR>
        			</UL>&lt;/NODELIST>
			</UL>&lt;/UNDISECTION><P>

			Return true if no file error occurs during the
			operations.
			*/

			bool
		write
			(
			std::string file_name
			);




			/**
			Read the graph from a file with name "file_name" and
			return true if no file error occurs during the
			operations.
			*/

			bool
		read
			(
			std::string file_name
			);




			/**
			Append the undi_graph section to the ofstream "out"
			(See write method for the file format).
			*/

			void
		append_section_to_fstream
			(
			std::
			ofstream& out
			);




			/**
			Read the undi_graph section from the ifstream "in".
			*/

			void
		read_section_from_fstream
			(
			std::ifstream& in
			);

			
			 

			/**
			Print the undi_graph information in the ostream "os";
			print_mode can be: 
                        <UL>
			<LI> BY_NODES: print each gdtnode and the ordered list of its 
				       incident edges;
			<LI> BY_EDGE:  print each gdtedge and its extremal nodes;
			<LI> COMPLETE: print all the information, combining all the 
				       previous modalities.
                        </UL>
			*/
			 
			void 
		print
			(
			print_mode mode=BY_NODES,
			std::ostream& os=std::cout
			) const;




			/**
			Print the identifier of gdtnode v in the ostream "os".
			*/

			void
		print
			(
			gdtnode v,
			std::ostream& os=std::cout
			) const;




			/**
			Print the identifier of gdtedge e in the ostream "os".
			*/
			 
			void 
		print 
			(
			gdtedge e,       
			std::ostream& os=std::cout
			) const;					
			
			
			 			
			
			/**
			Print constraint "c" in the ostream "os".
			The printing features depend on the constraint type
			(See documentation on constraints for more details).
			*/
			 
			void 
		print 	
			(
			constraint c, 
			std::ostream& os=std::cout
			) const; 					
			
			
			 		

			/**
			Print all constraints involving gdtnode "v" in the ostream
			"os".
			*/
			 
			void 
		print_constraints_on_node 
			(
			gdtnode v, 
			std::ostream& os=std::cout
			);					
			

			 		
			
			/**
			Print all constraints involving gdtedge "e" in the ostream
			"os".
			*/
			 
			void 
		print_constraints_on_edge 
			(
			gdtedge e, 
			std::ostream& os=std::cout
			);					
			
			
			
			
			/**
			Print all constraints of the graph in the ostream "os".
			*/
			 
			void
		print_constraints
			(
			std::ostream& os=std::cout
			) const;





		/*
		CATEGORY consistency_check
		Consistency check.
		*/

			/**
			Check consistency of the gdtnode/gdtedge internal structures.
			*/

			bool
		internal_consistency_check
			(
			) const;

	
/***********************************************************************************************************/

		/*
		CATEGORY PLANARITY
		The following functions compute a planar embedding of a biconnected graph.  (algorithm by Boyer % Myrvold)
		*/




			/**
			Compute a spanning tree of the graph and collects informations about nodes and edges.
			Information are stored in node_vector and edge_vector.
			Return false if the graph is not biconnected.
			This method is required by the Boyer and Myrvold planarity testing algorithm.<br>		
			<UL>
			<LI> node_vector: stores informations collected about nodes
			<L I> node_vector: stores informations collected about edges
			</UL>

			PRECONDITIONS: graph must be biconnected.
			*/

			bool
		bm_spanning_tree
			(
            		//node DFI_vector[],
			bm_node_info node_vector[],
			//edge_map<bm_edge_info*>& edge_vector,
			//int lowpoint[],
			//edge parent[],
			//list<node> children[],
			//list<node> in_back_edge[],
			//int first_back_edge_dfi[],
			gdt::gdtlist<gdtedge>& added_edges,
			bic_obj_node* IMM[],
			gdtnode v,
			bool reached[],
			bool e_reached[],
			//edge_map<bool>& e_reached,
			gdtedge iterator[],
			gdtnode graph_nodes[],
			int& nodes_in_component,
			bic_obj*& bic_obj_actual_pointer,
			bic_obj_node*& bic_obj_node_actual_pointer
			) ;


			/**
			Create for each node an ordered list of its children in the spanning tree.
			Children are ordered by increasing lowpoint value.
			This method is required by the Boyer and Myrvold planarity testing algorithm.	<br>
			PRECONDITIONS: Function bm_spannig_tree must be applied.
			<UL>
			<LI> node_vector: information about nodes collected by bm_spanning_tree
                        </UL>
			*/


			void
		create_children_ordered
			(
			bm_node_info node_vector[],
			//int lowpoint[],
			//edge parent[],
			//list<node> children_ordered[],
			//list_item position_in_parent_list[],
			gdtnode graph_nodes[],
			int nodes_in_component
			);



			/**
			Create for each node an ordered list of its back-edges in the spanning tree.
			Back-edges are ordered by increasing DFI value.
			This method is required by the Boyer and Myrvold planarity testing algorithm.<br>
			PRECONDITIONS: Function bm_spannig_tree must be applied.
			<UL>
			<LI> node_vector: information about nodes collected by bm_spanning_tree
   			</UL>
			*/


			void
     	create_out_back_edges_ordered
			(
   			bm_node_info node_vector[]
			);


			/**
			All the pre-processing operation required to execute the algorithm.
			This function compute a spanning tree of the graph, and then applies create_children_ordered
			and create_out_back_edges_ordered.
			This method is required by the Boyer and Myrvold planarity testing algorithm.<br>
			Return false if the graph is not biconnected. <br>
			
			PRECONDITIONS: Graph must be biconnected
			<UL>
			<LI> node_vector: stores informations collected about nodes:
   			<LI> node_vector: stores informations collected about edges:
			<LI> root: node used as root of the spanning tree.
			</UL>
			*/

			bool
    	bm_preprocessing
			(
            //node DFI_vector[],
			bm_node_info node_vector[],
			//int lowpoint[],
			//edge parent[],
			//list<node> children[],
			//list<node> children_ordered[],
			//list<node> in_back_edge[],
			//int first_back_edge_dfi[],
			//list_item position_in_parent_list[],
			//edge_map<bm_edge_info*>& edge_vector,
			gdt::gdtlist<gdtedge>& added_edges,
			gdtnode root,
			bic_obj_node* IMM[],
			bool reached[],
			bool e_reached[],
			//edge_map<bool>& e_reached,
			gdtedge iterator[],
			int& nodes_in_component,
			gdtnode graph_nodes[],
			bic_obj*& bic_obj_actual_pointer,
			bic_obj_node*& bic_obj_node_actual_pointer
			);



			/**
			This method is required by the Boyer and Myrvold planarity testing algorithm.<br>
			Create the data structure of the elementary bicomps described in the algorithm.
			Return the number of the elementary bicomp created.
			PRECONDITIONS: Graph must be biconnected. Function bm_preprocessing must be executed.
			<UL>
			<LI> node_vector: informations about nodes collected by bm_preprocessing:
   			<LI> node_vector: informations about edges collected by bm_preprocessing:
			<LI> root: root of the spanning tree.
			</UL>
			*/

			int
    	create_bicomp
  		   	(
			gdtnode root,
 			bm_node_info node_vector[],
			//edge_map<bm_edge_info*>& edge_vector,
 			gdt::gdtnode_map<bic_obj_node*>& IMM
			);




			/**
			Apply the planarity test to a biconnected graph.
			Call the bm_preprocessing, create the initial bicomp structure.
			Then make a post-order visit of the spanning tree, and for each node with entry back edges, try to insert them
			using pre_walk_up, walk_up and walk_down operations (see class bic_obj).
			If the graph is planar return true, and modify the embedding.
			NOTE: edges_inserted must be initialized with false
			<UL>
			<LI> node_vector: informations about nodes collected by bm_preprocessing
			<LI> edge_vector: informations about edges collected by bm_preprocessing
			<LI> IMM: for each node give the corresponding bic_obj
			<LI> root: root node of the spanning tree
			<LI> edges_inserted: for each edge, true if it was added to the embedding
			</UL>
			*/

			bool
		make_planar_embedding
			(
			bm_node_info node_vector[],
           		//int lowpoint[],
		   	//edge parent[],
		   	//list<node> children[],
			//list<node> children_ordered[],
			//list<node> in_back_edges[],
			//int first_back_edge_dfi[],
			//list_item position_in_parent_list[],

			bic_obj_node* IMM[],
			gdtnode root,
			//edge_map<bool>& edge_to_be_inserted
			bool edge_to_be_inserted[],
			int flip[],
			bic_obj*& bic_obj_actual_pointer
			);



			/**
			Read a list of biconnected planar graph, which are the biconnected components of this graph.
			Each node of this graph must have one and only one corresponding node in one biconnected component.
			This method copy the embedding of the biconnected components into the original graph, and return true if
			the operation ends successfully.
			This method is necessary to apply the algorithm to graphs which are not biconnected.
			<UL>
			<LI> bic: list of the planar biconnected componets
			</UL>
			*/

			bool
		merge_biconnected
			(
			gdt::gdtlist<undi_graph*>& bic
			);



			/**
			Apply the Boyer and Myrvold planarity testing algorithm to connected graphs.<br>
			Split the graph in biconnected components, and apply make_planar_embedding to all them.
			If all the components are planar, modify the embedding of the original graph with merge_biconnected
			If graph is planar return true, otherwhise return false.
			<UL>
			<LI> root: root node of the spanning tree
			</UL>
			*/

			bool
		boyer
			(
			gdtnode root
			);


			bool
		is_planar();

			


/**
			Destroy the bicomp structure created by make_planar_embedding.
			<UL>
			<LI> IMM: for each nodes gives the corresponding bic_obj
			</UL>
			*/
			void
		destroy_bicomp
			(
			gdt::gdtlist<bic_obj*>& bic_obj_created
			);




		/**
		Compare the embeddings of this graph and the undi_graph ug.
		<b>PRECONDITION</b>: the two graphs must be equal.
		@return the number of edge swapping that must be applied to ug to obtain the same embedding ad this graph. 
		*/

			int
		compare_embedding(undi_graph& ug);



		/**
		Extract the induced subgraph of the nodes at distance at most k from node n.
		The subgraph is stored in vg.
		*/

			void
		visible_subgraph(gdtnode n,int k, undi_graph& vg);



		/**
		Compute the maximum distance k from node n from which the visible subgraph from n at 
		distance k is planar 
		*/

			int
		kplanarity
			(
			gdtnode n
			);


			int
		extended_kplanarity
			(
			gdtnode n
			);



		/**
		Compute the k-planarity of each node
		*/

			void
		kplanarity(gdt::gdtnode_map<int>& kplan);		


			void
		extended_kplanarity(gdt::gdtnode_map<int>& kplan);		


/***********************************************************************************************************/

		/*
		CATEGORY C-PLANARITY
		The following functions are needed by the c-planarity testing
		*/

			
		
			/**
			If the graph is planar, compute a minimum spanning tree
			*/

			bool
		quick_minimum_spanning_tree
			(
			gdt::gdtedge_map<int> weights,
			gdt::gdtlist<gdtedge>& span_edges
			);


};  // end of class undi_graph



/*************************************************************************************/

// -------------------------------
// Macro for ordered gdtedge scanning
// -------------------------------

//Added by A.M.
#undef forall_nodes
#undef forall_edges
#define forall_nodes(v,G)\
for(v=(G).first_node();v;v=(G).succ_node(v))

#define forall_edges(e,G)\
for(e=(G).first_edge();e;e=(G).succ_edge(e))
//

#define forall_edges_adjacent_node(e,v,G)\
for(e=(G).first_adj_edge(v);e;e=(G).adj_succ(e,v))

#define forall_edges_entering_node(e,v,G)\
for(e=(G).first_in_edge(v);e;e=(G).in_succ(e,v))

#define forall_edges_leaving_node(e,v,G)\
for(e=(G).first_out_edge(v);e;e=(G).out_succ(e,v))

#define forall_constraints(c,G)\
for(c=(G).first_constraint();c;c=(G).succ_constraint(c))


#endif
