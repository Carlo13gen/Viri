/*******************************************************************************
+
+  rm3_flow_dire_graph.h
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

#ifndef RM3_FLOW_DIRE_GRAPH_H
#define RM3_FLOW_DIRE_GRAPH_H

#if !defined(ROOT_INCL_ID)
#define ROOT_INCL_ID 34013
#endif

#include <GDT/rm3_dire_graph.h>

	/*
	SOURCEFILE rm3_flow_dire_graph.h
	To be defined.
	*/

//-----------------------------------------------------------------------------
// flow_dire_graph:
// 	base class for network flow 
//
// W.Didimo, A.Leonforte (1998)
//-----------------------------------------------------------------------------



	/**
	GLOBALTYPE struct_flow_dire_node_info
	Local-flow_dire structure for gdtnode.
	*/

	class GDT_NONSTANDARD_DECL
struct_flow_dire_node_info
	{
	friend class flow_dire_graph;
	//
	int balance;				
	
	public:

		/*
		CATEGORYremoved constructors_destructors
		Constructor and Destructors.
		*/

			/**
			To be defined.
			*/

	struct_flow_dire_node_info
		(
		);
	};

	

	/**
	GLOBALTYPE struct_flow_dire_edge_info
	Local-flow_dire structure for gdtedge.
	*/

 	class GDT_NONSTANDARD_DECL
struct_flow_dire_edge_info
	{
	friend class flow_dire_graph;
	//
	int upper_cap;
	int lower_cap;
	int cost;
	int flow;
	
	public:

		/*
		CATEGORYremoved constructors_destructors
		Constructor and Destructors
		*/

			/**
			To be defined.
			*/

	struct_flow_dire_edge_info
		(
		);
	};

	

	/**
	CLASS flow_dire_graph
	A flow_dire_graph F is a directed graph such that:
	<UL>
	<LI>	<UL> 
		<LI>Any gdtnode v of F has an associated function "balance(v)" such that Sum_v[balance(v)] = 0:
	        <LI>if (balance(v) > 0) v "supplies" flow;
		<LI>if (balance(v) < 0) v "demands"  flow;
		<LI>if (balance(v) = 0) v "manteins" flow.
		</UL>
	<LI> Any gdtedge e of F has associated the following functions:
		<UL>
		<LI> lower_cap(e) >= 0;
	        <LI> upper_cap(e) >= lower_cap(e);
		<LI> cost(e);
		<LI> flow(e):
			<OL>
			<LI> lower_cap(e) &lt;= flow(e) &lt;= upper_cap(e);
         		<LI> Sum_v[flow(v,w)] - Sum_u[flow(u,v)] = balance(v) for each gdtnode v in F.
			</OL>
		</UL>
	</UL>
	<B>Definition</B>: We call "cost" of F the following: Sum_e[cost(e)*flow(e)].<P>
	<B>NOTE</B>: all the above mathematical functions are considered integral 
			(that is with integer values only)
	*/

	class GDT_NONSTANDARD_DECL
flow_dire_graph
	: public dire_graph
	{
	private:
		// -----------------
		// private variables
		// -----------------
		
		gdt::gdtnode_map<struct_flow_dire_node_info>* node_info;
		gdt::gdtedge_map<struct_flow_dire_edge_info>* edge_info;
		
		// ---------------
		// private methods
		// ---------------
		
		void local_new  ();  	// create a new set of pointers for the not-inherited class-part
		void local_del  ();	// delete all the not-inherited class-part
		void local_renew();	// utility function : just local_del(), then local_new()
		void local_init ();	// init the not-inherited class-part
		
		void local_set		// set all private variables 
			(
			gdt::gdtnode_map<struct_flow_dire_node_info>*, 
			gdt::gdtedge_map<struct_flow_dire_edge_info>*
			);
		
		//void update_edge_costs_in_residual (gdtedge, gdtedge, gdtedge, gdt::gdtedge_map<gdtedge>&, gdt::gdtedge_map<bool>&, gdt::gdtedge_array<int>&, gdt::gdtedge_map<int>&);
		
		
	public:
	
		/*
		CATEGORY constructors_destructors
		Constructors and destructors.
		*/
		
			
			/**
			Empty constructor.
			*/

		flow_dire_graph  
			(
			);					
		
			
			/**
			Destructor.
			*/

		~flow_dire_graph 
			(
			);				
		
			
			/**
			Constructor from the undi_graph class.
			PRECONDITIONS: all undi_graph-edges must be directed
			*/
				
		flow_dire_graph 
			(
			const undi_graph&
			);			

			
			/**
			Constructor from the dire_graph class.
			*/

		flow_dire_graph 
			(
			const dire_graph&
			);	
			
			
			/**
			Copy constructor.
			*/
		
		flow_dire_graph 
			(
			const flow_dire_graph&
			);	
		
		/*
		CATEGORY operators
		Operators.
		*/
		
			
			/**
			Equality operator from the undi_graph.
			PRECONDITIONS: all undi_graph-edges must be directed
			*/

			flow_dire_graph& 
		operator = 
			(
			const undi_graph&
			);									
			
			
			/**
			Equality operator from the dire_graph.
			*/

			flow_dire_graph& 
		operator =
			(
			const dire_graph&
			);	
			
			
			/**
			Copy equality operator.
			*/

			flow_dire_graph& 
		operator = 
			(
			const flow_dire_graph&
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
			gdt::gdtnode_map<struct_flow_dire_node_info>*& p1, 
			gdt::gdtedge_map<struct_flow_dire_edge_info>*& p2
			);

			
			/**
			Return the balance of gdtnode.
			*/		

			int 
		get_balance        
			(
			gdtnode v
			) const;		

			
			/**
			Return the upper capacity of gdtedge.
			*/		

			int 
		get_upper_capacity 
			(
			gdtedge e
			) const;			

			
			/**
			Return the lower capacity of gdtedge.
			*/		

			int 
		get_lower_capacity 
			(
			gdtedge e
			) const;		

			
			/**
			Return the flow of gdtedge.
			*/		

			int 
		get_flow           
			(
			gdtedge e
			) const;			

			
			/**
			Return the cost of gdtedge.
			*/		

			int 
		get_cost	       
			(
			gdtedge e
			) const;		
			
			
			/**
			Return the cost of the flow.
			*/		

			int 
		flow_cost  	       
			(
			) const;		

			
			/**
			Return the flow entering in gdtnode.
			*/		

			int 
		in_flow            
			(
			gdtnode v
			);				

			
			/**
			Return the flow leaving gdtnode.
			*/		

			int 
		out_flow  	       
			(
			gdtnode v
			);				

			
			/**
			Return the difference from out_flow and in_flow of gdtnode.
			*/		

			int 
		diff_flow	       
			(
			gdtnode v
			);			

			
			/**
			Return false if some balance properties are violated.
			*/		

			bool 
		balance_is_consistent	
			(
			) const;	

			
			/**
			Return false if the property <BR>
			lower_cap(e) &lt;= flow(e) &lt;= upper_cap(e)<BR>
			is violated on given gdtedge.
			*/		

			bool 
		flow_is_consistent		
			(
			gdtedge e
			) const;		 

			
			/**
			Return false if the property <BR>
			Sum_v[flow(v,w)] - Sum_u[flow(u,v)] = balance(v) <BR>
                        is violated on given gdtnode.
			*/		

			bool 
		flow_is_consistent		
			(
			gdtnode v
			);		

			
			/**
			Return return false if either:
			<OL>
			<LI> lower_cap(e) &lt;= flow(e) &lt;= upper_cap(e)
         		<LI> Sum_w[flow(v,w)] - Sum_u[flow(u,v)] = balance(v)
			</OL>
			 are violated on some edges or nodes.
			*/		

			bool 
		flow_is_consistent		
			(
			);		

			
			/**
			Return false if some flow network properties are violated.
			*/		

			bool 
		is_consistent     		
			(
			);			 
		
		/*
		CATEGORY update
		Update operations.
		*/
			
			
			/**
			Delete all nodes and edges.
			*/
	
			void 
		clear 
			(
			);						

			
			/**
			Set the balance of gdtnode with int.
			*/
	
			void 
		set_balance	
			(
			gdtnode v,
			int b
			);				

			
			/**
			Set the upper capacity of gdtedge with int.
			*/
	
			void 
		set_upper_capacity 
			(
			gdtedge e,
			int uc
			);			
			
			
			/**
			Set the lower capacity of gdtedge with int.
			*/
	
			void 
		set_lower_capacity	
			(
			gdtedge e,
			int lc
			);			
		
			
			/**
			Set the cost of the gdtedge with int.
			*/
	
			void 
		set_cost     	
			(
			gdtedge e,
			int c
			);						
			
			
			/**
			Set the flow of the gdtedge with int.
			*/
	
			void 
		set_flow     	
			(
			gdtedge e,
			int f
			);		

			
			/**
			Set upper cap., lower cap., cost and flow of gdtedge with the int
			values specified in this order.
			*/
			
			void 
		set_edge_info     	
			(
			gdtedge e,
			int uc,
			int lc,
			int c,
			int f
			);	

			
			/**
			Make *this with the dire_graph dg internal variables 
			and cut these variables from dg. 
			This method is used to make a dire_graph as a flow_dire_graph, manteining
			the same internal variables.
			WARNING: dg has an empty body after this method is applied
			*/
	
			void 
		steal_from 
			(
			dire_graph& dg
			);		

			
			/**
			Copy all private variables of flow_dire_graph in *this.
			*/
	
			void 
		mirror 	
			(
			flow_dire_graph& fdg
			);		

			
			/**
			Cut all private variables in *this.
			*/
	
			void 
		forget 	
			(
			);				
				
			
			/**
			Compute a min cost flow on the network.
			Return false if a fesible flow does not exist.
			PRECONDITIONS: current implementation only works for nonnegative costs.
			*/
	
			bool 
		min_cost_flow  
			(
			);														
		//bool max_flow	    (gdtnode s, gdtnode t);			// compute a max flow from gdtnode s to gdtnode t. Return false if a fesible flow does not exist.
	};								// STILL TO BE CODED
	
#endif
