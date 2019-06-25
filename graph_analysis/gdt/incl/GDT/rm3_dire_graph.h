/*******************************************************************************
+
+  rm3_dire_graph.h
+
+  This file is part of GDToolkit. It can be 
+  used free of charge in academic research and teaching. 
+  Any direct or indirect commercial use of this software is illegal 
+  without a written authorization of
+  the Dipartimento di Informatica e Automazione
+  Universita' di Roma Tre, Roma, ITALIA
+  
+
+  All rights reserved.
+ 
*******************************************************************************/


/*! \file */

#ifndef RM3_DIRE_GRAPH_H
#define RM3_DIRE_GRAPH_H

#if !defined(ROOT_INCL_ID)
#define ROOT_INCL_ID 34004
#endif

#include <GDT/rm3_undi_graph.h>
#include <GDT/gdtnode_pq.h>

	/*
	SOURCEFILE rm3_dire_graph.h
	To be defined.
	*/

//-----------------------------------------------------------------------------
// dire_graph:
// 	base class for all directed embedded graphs
//
// W.Didimo, A.Leonforte (1996)
//-----------------------------------------------------------------------------

	/**
	A 'dire_graph' represents an undi_graph with all directed edges. 
	In particular, all visit methods in this class are referred to the effective  
	direction of the edges, and not to the underlying graph.
	*/

	class GDT_NONSTANDARD_DECL
dire_graph
	: public undi_graph
	{
	private:
		// -----------------
		// private variables
		// -----------------
		// empty
		
		
		// ---------------
		// private methods
		// ---------------
		
		void local_new  ();      // create a new set of pointers for the not-inherited class-part
		void local_del  ();      // delete all the not-inherited class-part
		void local_renew();      // utility function : just local_del(), then local_new()
		void local_init ();      // init the not-inherited class-part

		void make_undirected (); // hides undesired inherited method
	

	public:

		/*
		CATEGORY constructors_destructors
		Constructors and Destructors
		*/
	
		
			
			/**
			Empty constructor.
			*/

		dire_graph
			(
			);							

		
			
			/**
			Constructor from the undi_graph class.<br>
			PRECONDITIONS: all undi_graph-edges must be directed.
			*/

		dire_graph
			(
			const undi_graph&
			);
			
		/*
		CATEGORY opeartors
		Operators.
		*/
			
		
			
			/**
			operator from the undi_graph class <br> 				
			PRECONDITIONS: all undi_graph-edges must be directed
			*/
						
			dire_graph&
		operator=
			(
			const undi_graph&
			);		

		

		/*
		CATEGORY access
		Access operations
		*/
												
		
			
			/**
			Return a source gdtnode (that is a gdtnode without in-edges) if there 
			exists one, else return NULL_NODE.
			*/ 	

			gdtnode 
		find_source
			(
			);					
			
		
			
			/**
			Return a sink gdtnode (that is a gdtnode without out-edges) if there 
			exists one, else return NULL_NODE.
			*/

			gdtnode 
		find_sink
			(
			);
			
		
			
			/**
			Find a shortest path from gdtnode s to all other nodes:
			length[e]   = length of gdtedge e;
			distance[v] = distance of the shortest path s-->v;
			pred_edge[v]= previous gdtedge of the gdtnode v in the shortest path s-->v
			NOTE: if there is not a path from s to a gdtnode v, distance[v] = INFINITE.
			This method uses Dijkstra method. 
			*/

			void 
		find_shortest_path					
			(
			gdtnode s, 
			gdt::gdtedge_array<int>&  length, 
			gdt::gdtnode_array<int>&  distance, 
			gdt::gdtnode_array<gdtedge>& pred_edge
			);
						
		
			
			/**
			Find a longest path from gdtnode s to all the other nodes:
			length[e]   = length of gdtedge e;
			distance[v] = distance of the longest path s-->v;
			pred_edge[v]= previous gdtedge of the gdtnode v in the longest path s-->v;
			PRECONDITIONS: (1) graph must be acyclic;
				       (2) all nodes must be reacheble starting from gdtnode s;
			*/
				
			void 
		find_longest_path				 
			(
			gdtnode s, 
			gdt::gdtedge_array<int>&  length, 
			gdt::gdtnode_array<int>&  distance, 
			gdt::gdtnode_array<gdtedge>& pred_edge
			);
			
		
			
			/**
			Find a path from the gdtnode s to all the other nodes having the minimum 
			number of swithes nodes, that is the minimum number of nodes in which 
			the direction of the edges is reversed, starting from an outgoing gdtedge
			of s if dir = OUTGOING, and from an incoming gdtedge of s if dir = INCOMING.
			A directed path is a path with 0 switches.
			switches[v]  = minimum number of swithces needed to reach v from s;
			pred_edge[v] = previous gdtedge of v in the path s-->v; 
			*/ 
				
			void 
		find_path_with_minimum_switches 
			(
			gdtnode s, 
			gdt::gdtnode_array<int>&  switches, 
			gdt::gdtnode_array<gdtedge>& pred_edge, 
			visit_direction_type dir
			);
			
		
			
			/**
			Find a path from the gdtnode s to gdtnode t having the minimum number of 
			swithes nodes, that is the minimum number of nodes in which the
			direction of the edges is reversed. 
			A directed path is a path with 0 switches.
			switches[v]  = minimum number of swithces needs to reach v from s;
			pred_edge[v] = previous gdtedge of v in the path s-->v;
			*/   
						
			void 
		find_path_with_minimum_switches 
			(
			gdtnode s, 
			gdtnode t, 
			gdt::gdtnode_array<int>&  switches, 
			gdt::gdtnode_array<gdtedge>& pred_edge
			);										
			
		
			
			/**
			Find a shortest path from gdtnode s to all other nodes:
			length[e]   = length of gdtedge e;
			distance[v] = distance of the shortest path s-->v;
			pred_edge[v]= previous gdtedge of the gdtnode v in the shortest path s-->v
			NOTE: if there is not a path from s to a gdtnode v, distance[v] = INFINITE.
			*/
				
			void 
		Dijkstra 
			(
			gdtnode s, 
			gdt::gdtedge_array<int>&  length, 
			gdt::gdtnode_array<int>&  distance, 
			gdt::gdtnode_array<gdtedge>& pred_edge
			);					
			
		
			
			/**
			Execute a DFS visit of the graph, starting from gdtnode v, 
			and return the list of back edges (if this list is empty, the graph
			is directed acyclic).
			reached [v] = true if gdtnode v is visited.
			father  [v] = father gdtedge of v in the DFS visit.
			order       = ordered list of the visited nodes.
			*/

			gdt::gdtlist<gdtedge> 
		simple_DFS 
			(
			gdtnode v, 
			gdt::gdtnode_array<bool>& reached, 
			gdt::gdtnode_array<gdtedge>& father, 
			gdt::gdtlist<gdtnode>& 	  order, 
			visit_direction_type dt = OUTGOING
			);		
						
		
			
			/**
			Return true if the graph is directed acyclic.
			*/
		
			bool 
		is_acyclic 
			(
			);											
		
			
			/**
			Reverse a minimal number of edges in order to make
			the graph directed acyclic; the inverted edges
			are returned
			*/ 
			
			gdt::gdtlist<gdtedge>
		make_acyclic
			(
			);
		
			
			/**
			Execute a topological sort of the graph, starting from 
			gdtnode v, and return false if it is directed acyclic.
			reached [v] = true if gdtnode v is visited.
			order       = topological ordered list of the visited nodes.
			*/
			
			bool 
		topological_sort 
			(
			gdtnode v, 
			gdt::gdtnode_array<bool>& reached, 
			gdt::gdtlist<gdtnode>& order
			);						
		
		/*
		CATEGORY update
		Update operations
		*/

		
			
			/**
			Insert a new directed gdtedge with start v1 and stop v2, and 
			identifier new_id, and return it.
			*/
	
			gdtedge
		new_edge
			(
			gdtnode v1,
			gdtnode v2,
			int new_id=AUTO_ID
			);
			
		
			
			/**
			Insert a new directed gdtedge with start v1 and stop v2, and
			identifier new_id, and return it.
			Also the gdtedge is inserted after(before) e1 around v1.
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
			Insert a new directed gdtedge with start v1 and stop v2, and
			identifier new_id, and return it.
			Also the gdtedge is inserted after(before) e2 around v2.
			*/



			gdtedge
		new_edge
			(
			gdtnode v1,
			gdtnode v2,
			gdtedge e2,
			int new_id=AUTO_ID,
			int dir=gdt::after
			);



			/**
			Insert a new directed gdtedge with start v1 and stop v2, and
			identifier new_id, and return it.
			Also the gdtedge is inserted after(before) e1 around v1 and
			after(before) e2 around v2.
			*/

			gdtedge
		new_edge
			(
			gdtnode v,
			gdtedge ev,
			gdtnode w,
			gdtedge ew,
			int new_id=AUTO_ID,
			int dir=gdt::after
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
			This method is used to make an undi_graph as a dire_graph, manteining 
			the same internal variables.
			WARNING: ug has an empty body after this method is applied
			PRECONDITIONS: all edges of ug must be directed
			*/

			void
		steal_from 
			(
			undi_graph& ug
			);					
	};											
	
												

#endif
