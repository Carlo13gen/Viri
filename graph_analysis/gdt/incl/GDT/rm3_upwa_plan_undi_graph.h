/*******************************************************************************
+
+  rm3_upwa_plan_undi_graph.h
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


#ifndef RM3_UPWA_PLAN_UNDI_GRAPH_H
#define RM3_UPWA_PLAN_UNDI_GRAPH_H

#include <GDT/gdtlist.h>
#include <GDT/gdtmap.h>
#include <GDT/rm3_plan_undi_graph.h>
#include <GDT/rm3_global.h>



//-----------------------------------------------------------------------------
// upwa_plan_undi_graph: base class for handling upward representations of
//			 directed planar graphs
//
// W.Didimo, A.Leonforte (1997)
//-----------------------------------------------------------------------------

	class 
upwa_plan_undi_graph;		// forward declaration

	/**
	GLOBALTYPE struct_upwa_node_info
	Local-upwa_plan_undi structure for gdtnode.
	*/
	
	class 
struct_upwa_node_info
	{
	friend class upwa_plan_undi_graph;
	
	private:
		gdtedge first_in_cand;			// first in-edges in candidate lists of gdtnode
		gdtedge first_out_cand;			// first out-edges in candidate lists of gdtnode
		bool in_cand_edges_are_ordered;		// true when the in-edges candidate list is ordered
		bool out_cand_edges_are_ordered;	// true when the out-edges candidate list is ordered
		
	public:
		
		struct_upwa_node_info
			(
			);
	};

	/**
	CLASS upwa_plan_undi_graph
	An 'upwa_plan_undi_graph' represents a plan_undi_graph with all directed edges 
	and an associated upward representation. Such a representation is given by using two
	linear (left->right) lists of in/out-edges around each gdtnode.
	A necessary condition to make an upwa_plan_undi_graph object is that the graph used
	to initialize it has a candidate planar embedding (that is all in/out-edges around 
	each gdtnode are consecutive). <P>
	When this condition is verified an upwa_plan_undi_graph upug is built, possibly adding 
	a minimum number of switches (sources and sinks) on some edges. In this case, these
	dummy  nodes (that we call "bends") are atumatically removed when passing upug to a 
	draw_undi_graph object; drawings so obtained are called "quasi-upward drawings". 
	An upward drawing is a quasi-upward drawing without bends.<P>

	Observe that this class is very similar to the orth_plan_undi one.<P>
	
	Three different kinds of 'bend_constraint' are possible for an gdtedge:
	<UL>
	<LI> ANY  	= any number of bends on gdtedge;
	<LI> NONE 	= no bends on gdtedge;
	<LI> MINIMAL 	= a 'minimal' number of bends on gdtedge (default). 
	</UL>
	Two different algorithms for producing a quasi-upward drawing upug:

	<UL>
	<LI> PLAN_UPWARD_OPTIMAL: finds an upug with the minimum number of bends, 
				  preserving the embedding. (Polynomial)

	<LI> PLAN_UPWARD_SLOW   : finds an upug with the minimum number of bends, 
				  searching over all possible planar embeddings of 
				  the graphs. It is an exponential-time algorithm based
				  on branch and bound techniques, and currently works
				  for biconnected graphs only.
	</UL>
	Both algorithms handle the bend constraints on edges.
	*/

	class GDT_NONSTANDARD_DECL
upwa_plan_undi_graph
	: public plan_undi_graph
	{
		
	private:
		// -----------------
		// private variables
		// -----------------
		
		face				 ext_face;			// external face 
		algorithm_type 			 layout_algorithm;		// layout algorithm (PLAN_UPWARD_OPTIMAL, PLAN_UPWARD_SLOW)
		gdt::gdtedge_map<bend_constraint>        constraint_on_bends_of_edge;	// mapping of bend_constraint on edges
		gdt::gdtnode_map<struct_upwa_node_info>* node_info;			// correspondence gdtnode -> upwa_node structure
		int 				 last_cost;			// cost of the last flow-algorithm applied
		
		// ---------------
		// private methods
		// ---------------
		
		void local_new  ();   							// create a new set of pointers for the not-inherited class-part
		void local_del  ();   							// delete all the not-inherited class-part
		void local_renew(); 							// utility function : just local_del(), then local_new()
		void local_init (face,algorithm_type, bool err_mess); 			// init the not-inherited class-part
		
		void local_set								// set all private variables  
			(
			face, 
			algorithm_type, 
			gdt::gdtedge_map<bend_constraint>, 
			gdt::gdtnode_map<struct_upwa_node_info>*,
			int
			);									

		void set_first_in_cand   (gdtnode,gdtedge);					// set the first in-gdtedge in the candidate lists of gdtnode with gdtedge
		void set_first_out_cand  (gdtnode,gdtedge);					// set the first out-gdtedge in the candidate lists of gdtnode with gdtedge
		
		void set_in_cand_edges_are_ordered  (gdtnode,bool);			// set the in_edges candidate list of gdtnode as ordered(bool = true)/unordered(bool = false)
		void set_out_cand_edges_are_ordered (gdtnode,bool);			// set the out_edges candidate list of gdtnode as ordered(bool = true)/unordered(bool = false)
		
		
		void update_constraint_internal_structures ();				// update all constraint internal structures, based on contraint list
		void set_bend_constraints   (gdtedge, bend_constraint);			// set bend_constarint for gdtedge
		void set_bend_constraints   (gdt::gdtedge_array<bend_constraint>);		// set bend constraints for all edges
		void reset_bend_constraints ();						// reset all bend constraints on gdtedge

		void order_in_cand  (gdtnode);						// update the first_in_edge of gdtnode to have a candidate list
		void order_out_cand (gdtnode);						// update the first_out_edge of gdtnode to have a candidate list             
		void order_cand	    (gdtnode);						// update the first_in_edge and the first_out_edge of gdtnode to have candidate lists
		void order_cand     ();							// execute the update_cand(v) method for each gdtnode v.
		
		gdt::list_item find_BSS  (gdt::gdtlist<gdtnode>& ext, gdt::gdtmap<gdt::list_item,bool>& is_big);	// look for a sequence BSS (big/small angles) into the estremals list ext.
										        // If there exists one, return the item of the gdtnode with angle B in ext.
		
		
		
		int make_upward_with_constraints ();		 			// kernel of the optimal_upward_for_fixed_embedding_with_constraints() method
								 
		int optimal_upward_for_fixed_embedding_with_constraints ();	 	// compute an upward representation of the graph, preserving the planar embedding and
								 		 	// introducing, when it is needed, a minimum number of dummy nodes and edges.
								 		 	// The function returns the number of dummy nodes added on edges with no zero cost (flow cost)
								 
		int slow_upward ();				 			// compute an upward representation of the graph, searching over all the
								 			// possible planar embedding of the graph and introducing, when it is needed, a minimum number 
								 			// of dummy nodes and egdes.
								 			// The function returns the number of dummy nodes added on edges with no zero cost (flow cost)
								 			// PRECONDITION: graph must be biconnected.
								 
		
		gdtnode new_node (gdtedge,int new_id=AUTO_ID );				// add a new gdtnode splitting an gdtedge e into two new edges with same direction of e
                
                gdtedge new_edge (gdtnode v1, gdtnode v2, face f, int new_id=AUTO_ID);		// add a new directed gdtedge from v1 to v2 into the face f;
		gdtedge new_edge (gdtnode v1, gdtedge e1, gdtnode v2, gdtedge e2, int new_id=AUTO_ID);	// add a new directed gdtedge from v1 to v2 and put it after gdtedge e1 around v1 and after gdtedge e2 around v2 
		
		void del_edge (gdtedge); 							// delete gdtedge;
		
		void reverse (gdtedge);							// reverese direction of gdtedge
		void make_directed (gdtedge,gdtnode);						// make gdtedge directed from gdtnode
	
		void make_big_angle     (gdtnode v, face f);	 			// make a big angle on gdtnode v into face f
	 
		gdt::gdtlist<gdtedge> decompose (face& f);		 	 			// add and return a set of dummy edges on sequences of extremals BSS.
								 			// Also, it puts in f the pointer to the last face of the recursive decomposition.
								 			// NOTE: if f was the external face, after the decomposition f will be the new external face
								 
		gdt::gdtlist<gdtedge> decompose ();			 			// apply decompose to all faces and return all dummy edges added
		
		bool is_upward();							// return true if the internal data structure really describe an upward representation
	
	
	
	public:
		
		/*
		CATEGORY constructors_destructors
		Constructors and destructors.
		*/
		
			
			/**
			Empty constructor.
			*/
			
		upwa_plan_undi_graph  
			(
			);								
				
			
			/**
			Destructor.
			*/
			
		~upwa_plan_undi_graph 
			(
			);								
					
			
			/**
			Constructor from undi_graph class, applying algorithm type.
			If err_mess = true an error message is produced when 
			a layout is not possible. <P> 
			PRECONDITIONS: graph must be directed and candidate
			*/
			
		upwa_plan_undi_graph 
			(
			const undi_graph& ug, 
			algorithm_type alg = PLAN_UPWARD_OPTIMAL,
			bool err_mess = true
			);		
			
			
			/**
			Constructor from plan_undi_graph class, applying algorithm type.
			If err_mess = true an error message is produced when 
			a layout is not possible. <P> 
			PRECONDITIONS: graph must be directed and candidate 
			*/
													
		upwa_plan_undi_graph 
			(
			const plan_undi_graph& pug, 
			algorithm_type alg = PLAN_UPWARD_OPTIMAL,
			bool err_mess = true
			);	
			
			
			/**
			Constructor from plan_undi_graph class, applying algorithm_type, 
			with face as external face.
			If err_mess = true an error message is produced when 
			a layout is not possible. <P> 
			PRECONDITIONS: graph must be directed and candidate		
			*/
			
		upwa_plan_undi_graph 
			(
			const plan_undi_graph& pug, 
			face ef, 
			algorithm_type alg = PLAN_UPWARD_OPTIMAL,
			bool err_mess = true
			);										
			
			
			
			/**
			Constructor from plan_undi_graph class, applying a predefined shape.
			Shape is specified by using the gdt::gdtnode_array 'first_edge'. Namely,
			first_edge[v] (where v is a gdtnode of pug) indicate the left-most gdtedge 
			incident v in the upward representation (candidate lists).
			Obiouvsly, this gdtedge is considered for source and sink nodes only.
			*/
			
			
		upwa_plan_undi_graph
			(
			const plan_undi_graph& pug,
			gdt::gdtnode_array<gdtedge>& first_edge
			);
			
			
			
			/**
			Copy constructor.
			*/
											
		upwa_plan_undi_graph 
			(
			const upwa_plan_undi_graph& upug
			);					
		
		       
		/*
		CATEGORY operators
		Operators.
		*/
				
			
			/**
			Equality operator from undi_graph class. <P>
			PRECONDITIONS: graph must be directed and candidate
			*/
			
			upwa_plan_undi_graph& 
		operator = 
			(
			const undi_graph& ug
			);					
					
			
			/**
			Equality operator from plan_undi_graph_class. <P>
			PRECONDITIONS: graph must be directed and candidate
			*/
													
			upwa_plan_undi_graph& 
		operator = 
			(
			const plan_undi_graph& pug
			);				
					
			
			/**
			Copy equality operator.
			*/
													
			upwa_plan_undi_graph& 
		operator = 
			(
			const upwa_plan_undi_graph& upug
			);				
		
		/*
		CATEGORY access
		Access operations.
		*/
		
			
			/**
			Get all local variables.
			*/
			
			void 
		local_get						 
			(
			face& p1, 
			algorithm_type& p2, 
			gdt::gdtedge_map<bend_constraint>& p3, 
			gdt::gdtnode_map<struct_upwa_node_info>*& p4,
			int& p5
			);										
		
		// DO NOT LIST IN PUBLIC METHODS
		// ---------------------------------------
		bool get_in_cand_edges_are_ordered  (gdtnode)  		const;	// get the internal variable in_cand_edges_are_ordered	
		bool get_out_cand_edges_are_ordered (gdtnode) 	 	const;	// get the internal variable out_cand_edges_are_ordered
		gdtedge get_first_in_cand (gdtnode)  				const;	// get the internal variable first_in_cand of gdtnode
		gdtedge get_first_out_cand(gdtnode)  				const;	// get the internal variable frist_out_cand of gdtnode
		// ---------------------------------------	
					
			
			/**
			Return the first in-gdtedge  (left->right) of the gdtnode v.
			*/
				
			gdtedge 
		first_in_cand_edge  
			(
			gdtnode v
			);	   			
				
			
			/**
			Return the first out_edge (left->right) of the gdtnode v.
			*/
			
			gdtedge 
		first_out_cand_edge 
			(
			gdtnode v
			); 	   			
				
			
			/**
			Return the successive adjacent gdtedge of e in v (left->right).
			*/
			
			gdtedge 
		cand_edge_succ      
			(
			gdtedge e, 
			gdtnode v
			); 			
				
			
			/**
			Return the previous adjacent gdtedge of e in v (left-right).
			*/
			
			gdtedge 
		cand_edge_pred      
			(
			gdtedge e, 
			gdtnode v
			); 			
				
			
			/**
			Return the current external face of the graph.
			*/
			
			face            
		external_face  
			(
			) const;	
				
			
			/**
			Return the current layout algorithm.
			*/
			
			algorithm_type  
		get_layout_algorithm 
			(
			) const;	
				
			
			/**
			Return the current bend constraint on the gdtedge e.
			*/
			
			bend_constraint	
		get_constraint_on_bends_of_edge 
			(
			gdtedge e
			) const;	
		
			
			/**
			Return true if v is a source.
			*/
			
			bool 
		is_source		 
			(
			gdtnode v
			) const;	
				
			
			/**
			Return true if v is a target.
			*/
			
			bool 
		is_target		 
			(
			gdtnode v
			) const;	
					
			
			/**
			Return true if v is either a source or a target.
			*/
			
			bool 
		is_extremal	 
			(
			gdtnode v
			) const;	
				
			
			/**
			return true if v is neither a source nor a target
			*/
			
			bool 
		is_internal	 
			(
			gdtnode v
			) const;	
					
			
			/**
			Return true if the gdtedge e and adj_cyclic_succ(e,v) have the same direction. <P>
			PRECONDITIONS: e must be adjacent to v.
			*/
			
			bool 
		is_extremal	 
			(
			gdtnode v, 
			gdtedge e
			) const;	
			
			
			/**
			Return true if the gdtedge e and adj_cyclic_succ(e,v) have opposite direction.<P>
			PRECONDITIONS: e must be adjacent to v.
			*/
										
			bool 
		is_internal	 
			(
			gdtnode v, 
			gdtedge e
			) const;	
			
			
			/**
			Return true if the gdtnode v is source into the face f.
			*/
									
			bool 
		is_source		 
			(
			gdtnode v, 
			face f
			) const;  
				
			
			/**
			Return true if the gdtnode v is target into the face f.
			*/
			
			bool 
		is_target           
			(
			gdtnode v,
			face f
			) const;	
				
			
			/**
			Return true if the gdtnode v is either source or target (extremal) into 
			the face f (only for biconnected).
			*/
			
			bool 
		is_extremal 	 
			(
			gdtnode v, 
			face f
			) const;	
			
			
			/**
			Return true if the gdtnode v is neither source nor target (internal) into 
			the face f (only for biconnected).
			*/
			
			bool 
		is_internal	 
			(
			gdtnode v, 
			face f
			) const;	
			
			
			/**
			Return true if there is a big angle (>180 deg) on v into f 
			(only for biconnected graph).
			*/
			
			bool 
		is_big_angle        
			(
			gdtnode v, 
			face f
			);	  		
			
			
			/**
			Return true if there is a big angle (>180 deg) on v between e and the 
			succ_adj_edge (e,v).
			*/
			
			bool 
		is_big_angle	 
			(
			gdtnode v, 
			gdtedge e
			);			
			
			
			/**
			Return the left->right list of the edges entering gdtnode 
			(in-edges candidate list).
			*/
			
			gdt::gdtlist<gdtedge> 
		in_cand_edges 
			(
			gdtnode v
			);	   			
				
			
			/**
			Return the left->right list of the edges leaving gdtnode  
			(out-edges candidate list).
			*/
			
			gdt::gdtlist<gdtedge> 
		out_cand_edges
			(
			gdtnode v
			);	   			
				
			
			/**
			Return the total number of extremal nodes.
				
			*/
			
			int  
		number_of_extremals 
			(
			) const;	
				
			
			/**
			Return the number of extremal nodes into the face f.
			Observe that if the graph is not biconnected a gdtnode might be counted 
			more than one time as extremal in f. 
			*/
			
			int  
		number_of_extremals 
			(
			face f
			) const;	 
					
			
			/**
			Return the total number of dummy nodes added.
			*/
						
			int  
		number_of_bends 
			(
			) const;								
					
			
			/**
			Return the maximum number of dummy nodes added on a single gdtedge.
			*/
			
			int  
		max_bends_on_edge 
			(
			) const;	
				
			
			/**
			Return the capacity of the face, defined as follow:
			capacity(f) = number_of_extremals(f)/2 - 1 if f is an internal face;
			capacity(f) = number_of_extremals(f)/2 + 1 if f is the external face. 					
			*/
			
			int  
		capacity 
			(
			face f
			) const;	
			
			
			/**
			Return the flow cost in the last computation apply_layout_algorithm.
			(INFINITE when no solution is found)
			*/
					 
			int  
		cost_of_last_algorithm
			(
			) const;	
		
					
			
			/**
			Return all the extremal nodes for the face f.
			Observe that if the graph is not biconnected a gdtnode is might be counted 
			more than one time as extremal in f. 
			*/
									
			gdt::gdtlist<gdtnode>  
		extremals  
			(
			face f
			) const;	
			
			
			/**
			Return all the extremals nodes for the face f and the 
			associated mapping of big (true) or small (false) angles in f.
			*/
									
			void        
		extremals  
			(
			face f,
			gdt::gdtlist<gdtnode>& ext, 
			gdt::gdtmap<gdt::list_item,bool>& is_big
			); 				
			
			
			/**
			Return the list of the nodes having a big angle into the face f.
			*/
			
			gdt::gdtlist<gdtnode>  
		big_angles 
			(
			face f
			);
			
			
 			/**
			Return true when the face is regular, that is
			when the face has not a pair of kitty corners
			(see definition of switch-regularity).
			If the face f is not regular a pair of border steps
			r1 and r2 is returned, such that their stop nodes are
			a pair of kitty corners.
			*/
 			
 			bool
 		face_is_regular
 			(
 			face f, 
 			border_step& r1, 
 			border_step& r2
 			);	
			
 			/**
			Return the angle formed by border step bs 
			with the successive border step, according to the
			following correspondence:
			
			_090: denote a small angle (less than 90 degrees)
			_180: denote a flat angle (in the range (90,180) degrees)
			_270: denote a big (large) angle when the stop of bs is not a vertex of degree one (in the range (180,60) degrees)
			_360: denote a big (large) angle when the stop of bs is a vertex of degree one (exactly 360 degrees)
			*/
 			
 			angle_type
 		angle_of_border_step
 			(
 			border_step bs 
 			);	
			
		
			/**
			Scan all faces and return the one
			that has two big angles more than 
			the number of small angles;
			Return NULL_FACE is such a face
			is not found
			*/
			
			face
		find_external_face
			();
		
					
			
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
			Make *this with the undi_graph ug internal variables 
			and cut these variables from ug. 
			This method is used to make an undi_graph as an upwa_plan_undi_graph, 
			manteining the same internal variables.
			If err_mess = true return an error message when
			a layout is not possible
			WARNING: ug has an empty body after this method is applied. <P>
			PRECONDITIONS: ug must be connected and with at least one gdtnode. 
			*/
			
			void 
		steal_from 
			(
			undi_graph& ug, 
			algorithm_type alg = PLAN_UPWARD_OPTIMAL,
			bool err_mess = true
			);			
			
			
			/**
			Make *this with the plan_undi_graph pug internal variables 
			and cut these variables from pug. 
			This method is used to make a plan_undi_graph as an upwa_plan_undi_graph, 
			manteining the same internal variables.
			If err_mess = true return an error message when
			a layout is not possible. <P>
			WARNING: pug has an empty body after this method is applied
			*/
  
			void 
		steal_from 
			(
			plan_undi_graph& pug, 
			algorithm_type alg = PLAN_ORTH_OPTIMAL,
			bool err_mess = true
			);		
			
			
			/**
			Make *this with the plan_undi_graph pug internal variables and setting external face
			before applying the selected layout algorithm,
			and cut these variables from pug. 
			This method is used to make a plan_undi_graph as an upwa_plan_undi_graph, 
			manteining the same internal variables.
			If err_mess = true return an error message when
			a layout is not possible. <P>
			WARNING: pug has an empty body after this method is applied
			*/
								
			void 
		steal_from 
			(
			plan_undi_graph& pug, 
			face ef, 
			algorithm_type alg = PLAN_ORTH_OPTIMAL,
			bool err_mess = true
			);					
			
			
			/**
			Copy all private variables of upwa_plan_undi_graph in *this.
			*/
			
			void 
		mirror 	
			(
			upwa_plan_undi_graph& upug
			);						
				
			
			/**
			Cut all private variables in *this.
			*/
			
			void 
		forget 	
			(
			);									
				
			
			/**
			Set face as external face.
			*/
			
			void 
		set_external_face    
			(
			face ef
			);							
				
			
			/**
			Set algorithm_type as the layout algorithm.
			*/
			
			void 
		set_layout_algorithm 
			(
			algorithm_type alg
			);						

			
			/**
			Add dummy edges and nodes (s and t) to make the graph as an st-graph.
			The augmentation performed is a standard technique.
			Return the list of dummy edges and the dummy source s and target t.	
			*/
			
			gdt::gdtlist<gdtedge> 
		make_st
			(
			gdtnode& s, 
			gdtnode& t
			);		 		 					
					
	
			/**
			Add dummy edges and nodes (s and t) to make the graph as an st-graph.
			The augmentation performed uses a switch-regularity technique.
			Return the list of dummy edges and the dummy source s and target t.	
			*/
			
			gdt::gdtlist<gdtedge> 
		make_st_with_regularity
			(
			gdtnode& s, 
			gdtnode& t
			);

	
			
			/**
			Compute a quasi-upward representation, by applying the current layout algorithm.
			If err_mess = true return an error message if a layout is true.
			*/
								 
			int 
		apply_layout_algorithm 
			(
			bool err_mess = true
			);	 
			
			/**
			Add a number of dummy edges to f in order to make the face regular;
			return true if the face was not regular.
			Store in dummy_edges the edges added
			*/
								 
			bool 
		regularize_face
			(
			face f, 
			gdt::gdtlist<gdtedge>& dummy_edges
			);				
			
			/**
			Apply the method regularize_face to all faces of the graph
			Store in dummy_edges the edges added.
			Return the number of irregular faces found
			*/
			
			int
		regularize_all_faces	
			(
			gdt::gdtlist<gdtedge>& dummy_edges
			);
			
			
			
		/*
		CATEGORY io_operations
		Input / output operations.
		*/
		
			
			/**
			Print the graph on ostream os; print_mode can be BY_NODES, BY_EDGE, BY_FACES, COMPLETE.
			if candidate is true print the two linear candidate lists for each gdtnode
			*/
			
			void 
		print 
			(
			print_mode mode=BY_FACES, 
			bool candidate = false, 
			std::ostream& os=std::cout
			);	
			
			
			/**
			Print gdtnode on ostream os
			if candidate is true print the two linear candidate lists of gdtnode.
			*/
			
			void 
		print 
			(
			gdtnode v, 
			bool candidate = false, 
			std::ostream& os=std::cout
			);				
			
			
			/**
			Print gdtedge on ostream os.
			*/
			
			void 
		print 
			(
			gdtedge e, 	
			std::ostream& os=std::cout
			) const;					
				
			
			/**
			Print face on ostream os.
			*/
			
			void 
		print 
			(
			face f, 
			std::ostream& os=std::cout
			) const;					
					
			
			/**
			Print separation_pair on ostream os.
			*/
			
			void 
		print 
			(
			separation_pair sp, 
			std::ostream& os=std::cout
			) const;									
					
			
			/**
			Print constraint on ostream os.
			*/
			
			void 
		print 
			(
			constraint c, 
			std::ostream& os = std::cout
			) const;					


	};



#define forall_candidate_edges_entering_node(e,v,G)\
for(e=(G).first_in_cand_edge(v);e;e=(G).cand_edge_succ(e,v))

#define forall_candidate_edges_leaving_node(e,v,G)\
for(e=(G).first_out_cand_edge(v);e;e=(G).cand_edge_succ(e,v))


#endif
