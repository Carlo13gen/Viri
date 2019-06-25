/*******************************************************************************
+
+  rm3_tree.h
+
+  This file is part of GDToolkit. It can be
+  used free of charge in academic research and teaching.
+  Any direct or indirect commercial use of this software is illegal
+  without a written authorization of
+  the Dipartimento di Informatica e Automazione
+  Universita' di Roma Tre, Roma, ITALIA
+
+  All rights reserved.
+
*******************************************************************************/
/*! \file */

#ifndef RM3_TREE_H
#define RM3_TREE_H


#include <GDT/rm3_undi_graph.h>

	/*
	SOURCEFILE rm3_tree.h
	To be defined.
	*/

//-----------------------------------------------------------------------------
// tree: base class for embedded trees.
//
// W.Didimo e A.Leonforte (1996)
//-----------------------------------------------------------------------------



	/**
	GLOBALTYPE struct_tree_node_info
	Local-tree structure for gdtnode.
	*/

 	class GDT_NONSTANDARD_DECL
struct_tree_node_info
	{
	friend class tree;
	//
	gdtedge father;    //father gdtedge of gdtnode in rooted tree
	int  level;	//depth level of gdtnode in rooted tree
	};



	/**
	CLASS tree
	A 'tree' represents a connected acyclic undi_graph with a gdtnode chosen as root.
	Each gdtnode of a tree, different from root, has a 'father_node', a
	'father_edge', and an ordered linear list (left-right) of son-nodes. Also,
	each gdtnode has associated a level, that is the length of a shortest path from
	root to the given gdtnode (root is at level 0).
	*/

	class GDT_NONSTANDARD_DECL
tree: public undi_graph
	{
	private:
		gdtnode root;					// root of tree
		gdt::gdtnode_map<struct_tree_node_info>* node_info;

		// --------------
		// private metods
		// --------------

		void local_new   ();              		// create a new set of pointers for the not-inherited class-part
		void local_del   ();              		// delete all the not-inherited class-part
		void local_renew ();		  		// utility function : just local_del(), then local_new()

		void local_init(gdtnode=NULL_NODE);  		// init the not-inherited class-part

		void local_set 					// set all private variables
			(
			gdtnode,
			gdt::gdtnode_map<struct_tree_node_info>*
			);

		void set_father_edge 	   (gdtnode v, gdtedge e); 		    // make e the father gdtedge of gdtnode v
		void set_level 	     	   (gdtnode v, int i);		    // make i the level of gdtnode v

		void set_levels_in_subtree (gdtnode v = NULL_NODE, int i = 0); // compute and set the gdtnode-levels in subtree rooted at v,
									    // assuming level(v)=0.
									    // NOTE: if v = NULL_NODE set v as root



		// ---------------------------------------------------
		// These methods are needed to hide undi_graph methods
		// ---------------------------------------------------

		void new_node () {};
		void new_edge () {};


	protected:
		//void compute_tree_dimension     (gdt::gdtnode_array<int>& width_of_subtree, gdt::gdtnode_array<int>& lev, gdt::gdtnode_array<int> width_of_node);
		//void compute_node_coordinates   (gdt::gdtnode_array<double>& x,gdt::gdtnode_array<double>& y, gdt::gdtnode_array<int> width_of_subtree, gdt::gdtnode_array<int>& lev);
		//void normalize_node_coordinates (gdt::gdtnode_array<double>& x,gdt::gdtnode_array<double>& y, double scale, double xinit, double yinit);

	public:

		/*
		CATEGORY constructors_destructors
		Constructors and destructors.
		*/


			/**
			Empty constructor.
			*/

		tree
			(
			);


			/**
			Destructor.
			*/

		~tree
			(
			);


			/**
			Constructor from the undi_graph class. Node v will be the root.
			PRECONDITIONS: undi_graph must be acyclic and connected
			*/

		tree
			(
			const undi_graph& ug,
			gdtnode v
			);


			/**
			Copy constructor.
			*/

		tree
			(
			const tree& tr
			);

		/*
		CATEGORY operators
		Operators.
		*/


			/**
			Equality operator from the undi_graph. Root will be the first_node() of undi_graph.
			PRECONDITIONS: undi_graph must be acyclic and connected
			*/

			tree&
		operator =
			(
			const undi_graph&
			);


			/**
			Copy equality operator.
			*/

			tree&
		operator =
			(
			const tree&
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
			gdtnode p1,
			gdt::gdtnode_map<struct_tree_node_info>* p2
			);
		
			
			/**
			Return the root-gdtnode of tree
			*/

			gdtnode
		root_of_tree
			(
			) const;
			
			
			/**
			Return the father gdtnode of gdtnode v.
			*/
			
			gdtnode 
		father_node
			(
			gdtnode v
			) const;
			
			
			/**
			Return the father gdtnode of gdtedge e. 
			*/
	
			gdtnode
		father_node     
			(
			gdtedge e
			) const;

			
			/**
			Return the first son gdtnode of gdtnode v.
     			*/
		
			gdtnode
		first_son_node  
			(
			gdtnode v
			) const;	
		
			
			/**
			Return the last son gdtnode of gdtnode v.
			*/

			gdtnode
		last_son_node
			(
			gdtnode v
			) const;
			
			
			/**
			Return the son gdtnode of gdtnode v, successive to gdtnode w.
	 		*/
		
			gdtnode 
		son_succ 	     
			(
			gdtnode w,
			gdtnode v
			) const;	
		

			/**
			Return the son gdtnode of gdtnode v, previous to gdtnode w.
			*/

			gdtnode 
		son_pred	     
			(
			gdtnode w,
			gdtnode v
			) const;	

			
			/**
			Return the father gdtedge of gdtnode v.
			*/
		 
			gdtedge 
		father_edge     
			(
			gdtnode v
			) const;	

			
			/**
			Return the father gdtedge of gdtedge e.
			*/
			
			gdtedge 
		father_edge     
			(
			gdtedge e
			) const;	

			
			/**
			Return the first son gdtedge of gdtnode v.			
			*/
	
			gdtedge
		first_son_edge  
			(
			gdtnode v
			) const;	

			
			/**
			Return the last son gdtedge of gdtnode v.
			*/
		
			gdtedge 
		last_son_edge   
			(
			gdtnode v
			) const;	

			
			/**
			Return the son gdtedge of gdtnode v, successive to gdtedge e.
			*/

			gdtedge 
		son_succ	     
			(
			gdtedge e, 
			gdtnode v
			) const;			

			
			/**
			Return the son gdtedge of gdtnode v, previous to gdtedge e.
			*/

			gdtedge 
		son_pred
			(
			gdtedge e, 
			gdtnode v
			) const;

			
			/**
			Return the level of gdtnode v.
			*/ 
			
			int 
		get_level	     
			(
			gdtnode v
			) const;	
			
			
			/**
			Return the number of sons of gdtnode v.
			*/ 
			
			int 
		number_of_sons   
			(
			gdtnode v
			) const;	

			
			/**
			Return the depth of subtree rooted at v, with
			respect to the start-level i.
			NOTE: if v = NULL_NODE, assume v as root gdtnode.
			*/
 		
			int
		depth_of_subtree 
			(
			gdtnode v = NULL_NODE,
			int i = 0
			) const;
										
			
			/**
			Execute a BFS of subtree rooted at v, scanning the sons
			of all nodes of subtree from left to right.
			NOTE: if v = NULL_NODE assume v as root gdtnode.
			lev[w] = level of gdtnode w with respect to the start-level i
			order  = ordered list of visited nodes.
			Method returns the depth of the subtree from the start-level i.
			(ADVICE: the start-level i is not the v level in the general case,
			but only a reference start level).
			*/
								
			int 
		BFS_of_subtree   
			(
			gdt::gdtnode_array<int>& lev, 
			gdt::gdtlist<gdtnode>& order, 
			gdtnode v = NULL_NODE, 
			int i = 0
			) const;
			
			
			/**
			Execute a DFS of subtree rooted at v, scanning the sons
			of all nodes of subtree from left to right.
			NOTE: if v = NULL_NODE assume v = root.
			lev[w] = level of gdtnode w with respect to the start-level i
			order  = ordered list of visited nodes.
			Method returns the depth of the subtree from the start-level i.
			(ADVICE: the start-level i is not the v level in the general case,
			but only a reference start level).
			*/

			int 
		DFS_of_subtree   
			(
			gdt::gdtnode_array<int>& lev, 
			gdt::gdtlist<gdtnode>& order, 
			gdtnode v = NULL_NODE, 	
			int i = 0
			) const;
		
			
			/**
			Execute a post-order visit of subtree rooted v (a gdtnode is
			visited only after that all its sons are visited).
			NOTE: if v = NULL_NODE assume v = root.
			lev[w] = level of gdtnode w with respect to the start-level i
			order  = ordered list of visited nodes.
			Method returns the depth of the subtree from the start-level i.
			(ADVICE: the start-level i is not the v level in the general case,
			but only a reference start level).
			*/

			int 
		post_order_of_subtree 
			(
			gdt::gdtnode_array<int>& lev, 
			gdt::gdtlist<gdtnode>& order, 
			gdtnode v = NULL_NODE, 
			int i = 0
			) const;

		/*
		CATEGORY update
		Update operations.
		*/
		
			
			/**
			Append, and return, a new son gdtnode to gdtnode v;
			new_node_id and new_edge_id will be the son 
			gdtnode- and son gdtedge-identifier, respectively.
			*/					

			gdtnode 
		new_son_of_node 
			(
			gdtnode v, 
			int new_node_id = AUTO_ID, 
			int new_edge_id = AUTO_ID
			);				
			
			
			/**
			Append after(before) son gdtnode w, and return, 
			a new son gdtnode to gdtnode v;
			new_node_id and new_edge_id will be the son 
			gdtnode- and son gdtedge-identifier, respectively.
			*/
	
			gdtnode 
		new_son_of_node 
			(
			gdtnode v, 
			gdtnode w, 
			int new_node_id = AUTO_ID, 
			int new_edge_id = AUTO_ID,
			int dir = gdt::after
			);
			
			/**
			Evert tree, making gdtnode v as new root.
			*/

			void 
		make_root 	     
			(
			gdtnode v
			);
			
			
			/**
			Delete the subtree rooted at v.			
			*/

			void 
		del_subtree     
			(
			gdtnode v
			);			
		
			
			/**
			Delete all nodes and edges.
			*/

			void 
		clear 	     
			(
			);		
		

			/**
			Make *this with the undi_graph ug internal variables 
			and cut these variables from ug. 
			This method is used to make an undi_graph as a tree,
			manteining the same internal variables.
			WARNING: ug has an empty body after this method is applied
			PRECONDITIONS: ug must be acyclic and connected
			*/					

			void 
		steal_from      
			(
			undi_graph& ug,
			gdtnode
			);	
								
			
			/**
			Copy all private variables of tree in *this.
			*/

			void 
		mirror          
			(
			tree&
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
			Print the graph on ostream os.
			*/

			void 
		print_tree 
			(
			std::ostream& os=std::cout
			);

		//void graphic_print_tree (window&);


		/*
		CATEGORY LCA
		Lowest Common Ancestor in O(1) time
		*/

		
		private:
			gdt::gdtnode_map<unsigned int> PREORDER;
			gdt::gdtnode_map<unsigned int> SIZE;
			gdt::gdtnode_map<unsigned int> INLABEL;
			gdt::gdtnode_map<unsigned int> ASCENDANT;
			gdt::gdtmap<unsigned int, gdtnode> HEAD;
			
			void
		preorder_visit
			(
			gdtnode current,
			gdt::gdtlist<gdtnode>& visited
			);

		
		public:
		
		/**
		Perform the preprocessing phase of the Schieber and Vishkin algorithm.
		This preprocessing allows to find the Lowest Common Ancestor of two nodes in constant time.
		*/
			void
		preprocessing_lca();

		/**
		Find the lowest common ancestor (LCA) of two nodes.
 		PRECONDITION: method preprocessing_lca must be applied.
		*/
			gdtnode
		LCA
			(
			gdtnode n1, 
			gdtnode n2
			);		

	};


// --------------------------------------------
// Macro for ordered son gdtedge and gdtnode scanning
// --------------------------------------------


#define forall_son_nodes(w,v,G)\
for(w=(G).first_son_node(v);w;w=(G).son_succ(w,v))

#define forall_son_edges(e,v,G)\
for(e=(G).first_son_edge(v);e;e=(G).son_succ(e,v))

#endif
