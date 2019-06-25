/*******************************************************************************
+
+  rm3_layered_undi_graph.h
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


#include <GDT/rm3_undi_graph.h>
#include <GDT/rm3_PQ_tree.h>
#include <GDT/gdtmap.h>
#include <GDT/rm3_global.h> //non lo so

#ifndef RM3_LAYERED_UNDI_GRAPH_H 
#define RM3_LAYERED_UNDI_GRAPH_H

#define LEAVES_VALUE gdtnode

/**
Global class layered_undi_graph. <br>
A layered undi_graph is a graph in which the nodes are assigned to levels. 
*/
class 
layered_undi_graph
	: public undi_graph
	{
	
	
	typedef gdt::PQ_node_struct<LEAVES_VALUE>* PQ_node;
	
	private:
		// ------------------------------------
		// private variables
		// ------------------------------------
		
		//gdt::gdtnode_map<int> levels;
		//gdt::gdtnode_map<double> ascisse;
		
		//---------------------------------------
		
		// ---------------------------------------------------
		// These methods are needed to hide the corresponding
		// undi_graph public methods.
		// ---------------------------------------------------
		
		//gdtnode new_node (int new_id=AUTO_ID);
		//gdtedge new_edge (gdtnode, gdtedge, gdtnode, int new_id=AUTO_ID);
		//gdtedge new_edge (gdtnode, gdtnode, int new_id=AUTO_ID);

		// ---------------------------------------------------
			gdt::gdtlist<gdtedge> 
		remove_cycles
			(
			);
			
			
			void
		assign_level
			(
			);
			
			
			void
		create_array
			(
			gdtnode* current_array
			);
			
			
			void
		assign_ascissa
			(
			);
			
			
			void
		restore_original_direction
			(
			gdt::gdtlist<gdtedge> reversed_edge
			);
			
			
			void
		assign_PQ_tree
			(
			);

	protected:
		/*
		protected variables
		*/
		
		gdt::gdtnode_map<int> levels;
		gdt::gdtnode_map<double> ascisse;
		//gdt::gdtmap<int,double> width_of_levels;
		gdt::gdtmap< int,gdt::PQ_tree<LEAVES_VALUE>* > map_level_PQ_tree;
		gdt::gdtnode_map<bool> is_dummy;
		gdt::gdtlist< gdt::gdtlist<gdtnode> > list_of_path_of_dummy;

	public:
		/*
		CATEGORY constructors_destructors
		Constructors and destructors.
		*/
		
			/**
			Empty constructor.
			*/
	
		layered_undi_graph  
			(
			);								
			
			
			/**
			One-argument constructor
			*/
			
		layered_undi_graph
			(
			undi_graph ug
			);
			
			
			/**
			Two-argument constructor
			*/
			
		layered_undi_graph
			(
			undi_graph& ug,
			gdt::gdtnode_map<int> in_levels
			);
			
			
			/**
			Two-argument constructor
			*/
			
		layered_undi_graph
			(
			undi_graph& ug,
			gdt::gdtnode_map<double> in_ascisse
			);
			
			
			/**
			Three-argument constructor
			*/
		layered_undi_graph
			(
			undi_graph& ug,
			gdt::gdtnode_map<int> in_levels,
			gdt::gdtnode_map<double> in_ascisse
			);
			
			
			
			/**
			Destructor.
			*/
			
		~layered_undi_graph 
			(
			);
			
			
			/**
			Returns the number of levels.
			*/
			
			int
		number_of_levels
			(
			);
			
			
			/**
			Sets the level, input: node and level to set.
			*/
			
			void
		set_level
			(
			gdtnode n,
			int level
			);
			
			
			/**
			Sets the level, input: node-id and level to set.
			*/
			
			void
		set_level
			(
			int i,
			int level
			);
			
			
			/**
			Sets the ascissa, input: node and ascissa to set.
			*/
			
			void
		set_ascissa
			(
			gdtnode n,
			double ascissa
			);
			
			
			/**
			Sets the ascissa, input: node-id and ascissa to set.
			*/
			
			void
		set_ascissa
			(
			int i,
			double ascissa
			);
			
			
			/**
			Returns the level of the input node.
			*/
			
			int
		get_level
			(
			gdtnode n
			);
			
			
			/**
			Returns the level of node with id is given as input.
			*/
			
			int
		get_level
			(
			int i
			);
			
			
			/**
			Returns the ascissa of the input node.
			*/
			
			double
		get_ascissa
			(
			gdtnode n
			);
			
			
			/**
			Returns the ascissa of node with id is given as input.
			*/
			
			double
		get_ascissa
			(
			int i
			);
			
			
			/**
			Returns the PQ_tree of level given as input.
			*/
			
			gdt::PQ_tree<gdtnode>*
		get_PQ_tree
			(
			int level
			);
			
			
			
			/**
			Returns the list of dummy vertices.
			*/
			
			gdt::gdtlist<gdtnode>
		get_list_of_dummy
			(
			);
			
			
			
			/**
			Returns the length of the input edge.
			*/
			
			int
		length_of_edge
			(
			gdtedge e
			);
			
			
			/**
			Returns the length of edge with id given as input.
			*/
			
			int
		length_of_edge
			(
			int i
			);
			
			
			/**
			Returns the width of a level given as input.
			*/
			
			double
		width_of_level
			(
			int level
			);
			
			
			/**
			Returns the number of nodes of the given level.
			*/
			
			int
		numbers_of_nodes_of_level
			(
			int level
			);
			
			
			
			/**
			Returns the list of nodes of the given level.
			*/
			gdt::gdtlist<gdtnode>
		create_list_of_node_of_level
			(
			int level
			);
			
			
			
			/**
			Adds the dummy vertices and edges.
			*/
			void
		make_proper_layered
			(
			);
			
			
			
			/**
			Returns the number of crossings between two levels.
			*/
			int
		count_of_crossing_between_two_levels
			(
			int level1,
			int level2
			);
			
			
			
			/**
			Returns the number of crossings of the entire graph.
			*/
			int
		count_of_crossing
			(
			);
			
			
			
			/**
			Sorts nodes of a given level respect to their ascisse.
			*/
			gdt::gdtlist<gdtnode>
		sort_nodes_of_a_level
			(
			int level
			);
			
			
			
			/**
			Finds barycenter of a given node, respect to ascisse of its neighbours of the under level.
			*/
			double
		find_barycenter
			(
			gdtnode node
			);
			
			
			
			/**
			Sets ascisse of nodes of level respect to barycenter of adjacent nodes of the under level.
			*/
			
			void
		set_ascisse_on_barycenter_of_a_level
			(
			int level
			);
			
			
			
			/**
			Sets ascisse of nodes of all levels respect to barycenter of adjacent nodes of the under level.
			*/
			
			void
		set_ascisse_on_barycenter
			(
			);
			
			
			
			/**
			Reduces PQ_trees of all level the constraint is the ordering on barycenter.
			*/
			
			void
		reduce_PQ_trees_on_barycenter
			(
			);
			
	};
	
		
#endif	

