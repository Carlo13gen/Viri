/*******************************************************************************
+
+  rm3_orth_plan_undi_graph.h
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

#ifndef RM3_ORTH_PLAN_UNDI_GRAPH_H
#define RM3_ORTH_PLAN_UNDI_GRAPH_H


#include <GDT/gdtlist.h>
#include <GDT/gdtmap.h>
#include <GDT/rm3_global.h>
#include <GDT/rm3_undi_graph.h>
#include <GDT/rm3_dire_graph.h>
#include <GDT/rm3_plan_undi_graph.h>

	/*
	SOURCEFILE rm3_orth_plan_undi_graph.h
	To be defined.
	*/
 
//-----------------------------------------------------------------------------
// ortho_plan_undi_graph:
// 	base class for all planar undirected graphs
// 	with planar embedding and orthogonal shape
//
// W.Didimo, A.Leonforte (1996)
//-----------------------------------------------------------------------------


	class
orth_plan_undi_graph; 		// forward declaration


	class
upwa_plan_undi_graph;		// forward declaration

// ----------
// constants
// ----------

const int UNDEFINED_PIN_NUMBER = -1;

// ------------------------------------------------
// enumerate types for bend type on gdtedge and angles
// ------------------------------------------------

	/**
	GLOBALTYPE bend_type
	Different kind of bend.
        Allowed values: right, left, and straight.
	*/

	typedef enum
	{
	right,
	left,
	straight
	}
bend_type;



inline  std::istream& operator >>(std::istream& in,const bend_type& x)
     {in >> *((unsigned int *)((void *)&x));return(in); }

inline  std::ostream& operator <<(std::ostream& out,const bend_type& x)
     {out << *((unsigned int *)((void *)&x));return(out); }

// -----------------------------------------------------


	/**
	GLOBALTYPE struct_orth_border_step_info
	Local-orth_plan_undi structure for border_step.
	*/

	class GDT_NONSTANDARD_DECL
struct_orth_border_step_info
	{
	friend class orth_plan_undi_graph;

	private:
		gdt::gdtlist<bend_type> bends;			// sequence of bends on the border step
		angle_type      angle;			// angle formed from the border step with the succ border step of the face
		int 	        thickness;		// number of border steps contracted in the current border step
	  	int 		pin_number;		/* the meaning of the pin_number varies depending on the value of the
							   thickness of the border_step:

                                                           * If the thickness value is equal to 1, called v the first non
                                                             bend_node vertex reachable through a direct chain of border_steps
                                                             beginning with the current one,
                                                             the pin_number holds the desired distance between the leftmost
                                                             corner of v and the attachment (on v) of the gdtedge which the
                                                             border_step belongs to.

			                                   * If the thickness value is greater than 1, then the gdtedge
                                                             represents different, overlapped edges, all incident to the same
                                                             vertex v, and the pin_numer is used to specify the desired attachment
                                                             of the overlapping edges to such vertex.
                                                             Namely, let e be the gdtedge (if any) orthogonal to the gdtedge which
                                                             the border step belongs to, and such that e is incident to the
                                                             start of the border_step if v is reachable through a directed path
                                                             beginning with the current border step, or to the stop of border_step,
                                                             otherwise.
						             Then, the pin_number holds the desired distance between the
                                                             leftmost corner of v and the attachment of e to v. (good luck!)
							*/

	public:

		struct_orth_border_step_info
			(
			);
	};
	
	/**
	CLASS orth_plan_undi_graph
	An 'orth_plan_undi_graph' represents a plan_undi_graph with an orthogonal shape
	associated to each gdtedge.<BR>
	Namely each gdtedge can have a certain number of right or/and left turns (bends), 
	represented by the enumerate: 'bend_type' = {right, left, stright}.<BR>
	Also, five different kind of angle are possible in a vertex: 
	0, 90, 180, 270, 360, represented by the following enumerate: 
	'angle_type' = {_000,_090, _180, _270, _360} (defined in rm3_global.h).<P>
	Three different kinds of 'bend_constraint' are possible for an gdtedge:
	<OL>
	<LI> ANY  	= any number of bends on gdtedge;
	<LI> NONE 	= no bends on gdtedge;
	<LI> MINIMAL 	= a 'minimal' number of bends on gdtedge (deafult).
	</OL>
 	Finally, an orthogonal representation can be obtained by applying one of the
        following algorithms:
	<UL>
	<LI> PLAN_ORTH_QUICK   = a heuristic for bend-minimization within the given 
				 embedding (linear-time); only for 4-planar and biconnected
	<LI> PLAN_ORTH_OPTIMAL = an optimal bend-minimization within the given embedding 
		  		 (polynomial-time); variation of Tamassia's algorithm also
				 considering the bend-constraints

	<LI> PLAN_ORTH_SLOW    = an optimal bend-minimization no-preserving the embedding
				 (exponential-time); based on a branch and bound technique
				 and considering the bends-constraints			
	</UL>
	*/
	
	class GDT_NONSTANDARD_DECL
orth_plan_undi_graph   
	: public plan_undi_graph  
	{
	friend class dime_orth_plan_undi_graph;
	private:
		// -----------------
		// private variables
		// -----------------

		face 	       		  			ext_face;				// external face
		algorithm_type 		  			layout_algorithm;			// current layout algorithm
		gdt::gdtedge_map<bend_constraint> 			constraint_on_bends_of_edge;		// bend-constraint type on edges
		gdt::gdtmap<border_step,struct_orth_border_step_info>*  border_step_info;			// store the shape of the orthogonal representation
		bool 						local_consistency_is_suspended;		// flag to suspend/enable the local consistentcy of the data structures
		bool 						diagnostic_printouts_are_enabled;	// flag to enable/disable the printouts for checking and debugging
		border_step					ref_border_step;			// reference border step, used to orient the graph when it is passed to a dime constructor
		int  						last_cost;				// cost of the last flow-algorithm applied
		
		// ---------------
		// private methods
		// ---------------

		//Method added by Marcandalli 16 10 2001
		void undefine(gdtedge e);
		//

		void local_new  ();             							// create a new set of pointers for the not-inherited class-part
		void local_del  ();             							// delete the not-inherited class-part
		void local_renew();									// utility function : just local_del(), then local_new()
		void local_init (face, algorithm_type, bool err_mess);    				// init the not-inherited class-part
		void local_init (const upwa_plan_undi_graph& upug, algorithm_type alg);			// init the not-inherited class-part
		
		void local_set										// set all private variables
			(
			face,
			algorithm_type,
			gdt::gdtedge_map<bend_constraint>,					
			gdt::gdtmap<border_step,struct_orth_border_step_info>*,
			bool,
			bool,
			border_step,
			int
			); 
		
		void update_constraint_internal_structures ();						// update all constraint internal structures, based on contraint list
		void set_bend_constraints   (gdtedge, bend_constraint);					// set the bend constraints on gdtedge with bend_constraint
		void set_bend_constraints   (gdt::gdtedge_array<bend_constraint>);				// set the bend constraints on edges with gdt::gdtedge_array<bend_constraints>
		void reset_bend_constraints ();								// reset all bend_constraints on edges
		
		gdt::gdtlist<bend_type> invert_bend_list 	   (gdt::gdtlist<bend_type>);				// execute a reversing of the bend list 

		void set_thickness_of_border_step          (border_step s, int t); 			// set the thickness of border step s with t
		void set_pin_number_of_border_step	   (border_step s, int p);			// set the pin number of border step "s" with "p"
		void reset_edge_info                       (gdtedge);					// only to hide the method of plan_undi_graph 
		void reset_border_step_info                (border_step);				// reset all border_step informations
		void set_bends_of_border_step              (border_step,gdt::gdtlist<bend_type>);		// set the bends of border step with gdt::gdtlist<bend_type>
		void set_angle_of_border_step              (border_step,angle_type);			// set the angle formed between the border_step and its successive with angle_type 
		
		
		int  	     angle_to_int 	           (angle_type alpha)   const; 			// return the integer corresponding to the given angle "alpha"
		angle_type   int_to_angle		   (int a)		const;			// return the angle corresponding to the given integer "a"
		
		
		void make_border_step_turn_left   	   (border_step);				// set the angle of border_step in order to make it turn left with respect its previous border_step
 		void make_border_step_turn_right  	   (border_step);				// set the angle of border_step in order to make it turn right with respect its previous border_step
 		void make_border_step_turn_back	  	   (border_step);				// set the angle of border_step in order to make it turn back with respect its previous border_step
		void make_border_step_go_straight 	   (border_step);				// set the angle of border_step in order to make it go straight with respect its previous border_step

		void set_edge_bends (gdtedge,gdtnode,int,bend_type);						// set an int number of bend_type on gdtedge starting from gdtnode
		void set_edge_shape (gdtedge,gdtnode,int,bend_type,                  angle_type,angle_type);	// set an int number of bend_type on gdtedge starting from gdtnode and set the extremal angles
		void set_edge_shape (gdtedge,gdtnode,std::string,                         angle_type,angle_type);	// set bends and angles of gdtedge. Namely, starting from gdtnode, a left-turn or a right-turn is created for each symbol 'L' or 'R', respectively in string
		void set_edge_shape (gdtedge,gdtnode,gdt::gdtlist<bend_type>,gdt::gdtlist<bend_type>,angle_type,angle_type);	// set bends and angles of gdtedge. Namely the first gdt::gdtlist<bend_list> is set on gdtedge starting from gdtnode and the second one is set starting from opposite of gdtnode.
		void set_edge_shape_near_node (gdtedge,gdtnode,std::string,angle_type);				// set bends on gdtedge. Namely, starting from gdtnode, a left-turn or a right-turn is created for each symbol 'L' or 'R', respectively in string. Also the angle of border_step entering gdtnode is set
	
		void remove_LRR_sequences_from_border_of_face (face, gdt::gdtlist<gdtnode>&, gdt::gdtlist<gdtedge>&);		// remove all circular sequences LRR in face, by adding a dummy gdt::gdtlist<gdtnode> and a dummy gdt::gdtlist<gdtedge>
		void remove_LRL_sequences_from_border_of_external_face (gdt::gdtlist<gdtnode>&, gdt::gdtlist<gdtedge>&);	// remove all circular sequences LRL in external face, by adding a dummy gdt::gdtlist<gdtnode> and a dummy gdt::gdtlist<gdtedge>.
		bool regularize_face (face, gdt::gdtlist<gdtnode>&, gdt::gdtlist<gdtedge>&, algorithm_type alg);		// regularize the face f if it is not regular and append the list of nodes and edges added. The regularization heuristic is given by alg
													// true is returned if the face was not regular
		
		gdtedge split_internal_face (gdtnode v1, gdtedge e1, gdtnode v2, gdtedge e2, int new_id=AUTO_ID);	// split an internal face by adding an gdtedge (v1,v2) and put it after gdtedge e1 around v1 and after gdtedge e2 around v2. The gdtedge added is returned
													// NOTICE: this method works for 4-planar graph only
													
		gdtedge split_external_face (gdtnode v1, gdtedge e1, gdtnode v2, gdtedge e2, int new_id=AUTO_ID);	// split the external face by adding an gdtedge (v1,v2) and put it after gdtedge e1 around v1 and after gdtedge e2 around v2. The gdtedge added is returned
													// NOTICE: this method works for 4-planar graph only
		
		void change_ref_border_step ();								// chose a new reference border step

		
												
		// ----------------------------------							
		// Shape calculation basic algorithms
		// ----------------------------------
		
 		void quick_orth_for_fixed_embedding   ();		 // execute a linear-time algorithm for computing an orthogonal representation of the graph
 									 // NOTICE: currently this method works for 4-planar and biconnected graphs only and preserves the planar embedding
 													
 		int optimal_orth_for_fixed_embedding_with_constraints(); // execute a polynomial-time algorithm for computing an orthogonal representation of the graph with minimum number of bends				
 									 // and considering the set of bend-constraints on edges. This algorithm is a variation to the Tamassia's one and extend it
 									 // for no 4-planar graphs
 								  	 // NOTICE: this method preserves the planar embedding
 		
 		int slow_orth ();					 // execute an exponential-time algorithm for computing an orthogonal representation of the graph with minimum number of bends
 									 // searching over all possible planar embeddings of the graph and considering the set of bend-constraints on gdtedge.
 									 // This algorithm is based on a branch and bound technique, that uses the SPQR-tree class.
 									 // NOTICE: this method does not preserve the planar embedding in general
 	
		
		// ----------------------------------------------------------------------
		// Shape adjustment low_level functions used by the quick_orth algorithm.
		// They MUST be called in order, that's why they are private.
		// For safe bend-stretching, use the stretch_bends() public method.
		// ----------------------------------------------------------------------
		
		int apply_t1_bend_stretching();
		int apply_t2_bend_stretching();
		int apply_t3_bend_stretching();
		
		// ----------------------------------------------------------------------
		
		
		face find_external_face ();	// explore all face and return the external one (NULL if it is not found)

		bool is_main_edge_on_node_side	(gdtedge e, gdtnode v); // return true if gdtedge e is the "main" gdtedge on the gdtnode side it incides on
		

	protected:
		gdtedge split_face_with_shaped_edge							
			(
			gdtnode v, gdtedge ev, angle_type av,
			gdtnode w, gdtedge ew, angle_type aw,
			std::string bends_sequence,
			int new_id=AUTO_ID
			);										// split a face by adding an gdtedge (v,w) and put it after gdtedge ev around v ang after gdtedge ew around w. Also set the shape of the gdtedge and return the gdtedge.
			
		void split_list_of_edges_on_side 
			(
			gdt::gdtlist<gdtedge>& L, 
			gdtnode v, 
			gdt::gdtlist<gdtedge>& L_left, 
			gdtedge& ec, 
			gdt::gdtlist<gdtedge>& L_right
			);										// split the list L of the edges on a side (edges with zero angle) of v in this way:
													// L_left <-- ec --> L_right
		
		void merge_edges_on_same_side (gdtnode v);							// merge in a single gdtedge all the edges on a same side of v. 
													// This method replace some bends with nodes (see source code) and
		
	public:
		/*
		CATEGORY constructors_destructors
		Constructors and destructors.
		*/
			
			
			/**
			Empty constructor.
			*/
 
		orth_plan_undi_graph
			(
			);									
			
			
			/**
			Destructor.
			*/
		
		~orth_plan_undi_graph 
			(
			);								
			
			
			/**
			Constructor from undi_graph applying algorithm_type.
			If err_mess = true an error message is produced when 
			a layout is not possible
			*/

		orth_plan_undi_graph 
			(
			const undi_graph& ug, 
			algorithm_type alg = PLAN_ORTH_OPTIMAL,
			bool err_mess      = true 
			);	

			
			/**
			Constructor from plan_undi_graph applying algorithm_type.
			If err_mess = true an error message is produced when 
			a layout is not possible
			*/
		
		orth_plan_undi_graph 
			(
			const plan_undi_graph& pug, 
			algorithm_type alg = PLAN_ORTH_OPTIMAL,
			bool err_mess	   = true 
			);	

			
			/**
			Constructor from plan_undi_graph, applying algorithm_type,
			with face as external face.
			If err_mess = true an error message is produced when 
			a layout is not possible
			*/
		
		orth_plan_undi_graph 
			(
			const plan_undi_graph& pug,
			face ef, 
			algorithm_type alg = PLAN_ORTH_OPTIMAL,
			bool err_mess	   = true 
			);																							 
			
			
			/**
			Constructor from upwa_plan_undi_graph.
			WARNING: constraints are not handled 
			*/
		
		orth_plan_undi_graph 
			(
			const upwa_plan_undi_graph& upug,
			algorithm_type alg = PLAN_ORTH_UPWA_STRAIGHT_MIDDLE
			);			
			
			
			
			/**
			Copy constructor.
			*/
		
		orth_plan_undi_graph 
			(
			const orth_plan_undi_graph& opug
			);					
		
		/*
		CATEGORY operators
		Operators.
		*/

			
			/**
			Equality operator from the undi_graph class.
			*/

			orth_plan_undi_graph&
		operator = 
			(
			const undi_graph& ug
			);

			
			/**
			Equality operator from the plan_undi_graph class.
			*/

			orth_plan_undi_graph& 
		operator = 
			(
			const plan_undi_graph& pug
			);			
		
			
			/**
			Equality operator from the upwa_plan_undi_graph class.
			*/

			orth_plan_undi_graph& 
		operator = 
			(
			const upwa_plan_undi_graph& upug
			);
			
			
			/**
			Copy equality operator.
			*/

			orth_plan_undi_graph& 
		operator = 
			(
			const orth_plan_undi_graph& opug
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
			face& 						p1,
			algorithm_type& 				p2,
			gdt::gdtedge_map<bend_constraint>& 			p3,
			gdt::gdtmap<border_step,struct_orth_border_step_info>*& p4,
			bool& 						p5,
			bool& 						p6,
			border_step&					p7,
			int   						p8
			);
			
			
			
			/**
			Return the reference_border_step.
			orth_plan_undi_graphs do not have a preferred upward
                        direction to see them straight up. You can
                        rotate them as you like.
                        dime_orth_plan_undi_graphs, instead, do handle a 
                        preferred direction: this information in the 
                        dime_orth_plan_undi_graph
			is represented by means of a pair of values: a 
			reference_border_step, and a heading (north, south,
                        east, or west).
			The reference_border_step, though, is a member
                        of the orth_plan_undi_graph for updating
                        reasons. 
       			*/
			
			border_step
		get_ref_border_step
			() const;
			
			
			/**
			Return the sequence of bends of border_step.
			*/
	
			gdt::gdtlist<bend_type> 
		bends_of_border_step   
			(
			border_step s
			) const;			
		
			
			/**
			Return the angle formed between the border_step and its successive.
			(Remember that border_steps are in clockwise order inside the face).
			*/

			angle_type	
		angle_of_border_step   
			(
			border_step s
			) const;			
			
			
			/**
			Return the thickness of the border_step s, that is 
			the number of border_steps compressed in s. 
			To represent overlapped edges, some information is needed
			to know 
			<OL>
			<LI>how many edges are overlapped on the same segment
			<LI>how many edges will leave the common path from one side and
                            how many will leave the common path from the other side (this
                            information is used to expand the overlapped edges into a very
			    close boundle)
			</oL>
			*/
		
			int 
		thickness_of_border_step 
			(
			border_step s
			) const;		

			
			/**
			Return the pin_numbero of the border_step s. <P>
                        The meaning of the pin_number varies depending on the value 
                        of the thickness of the border_step:
			<UL>
                        <LI>
                            If the thickness value is equal to 1, called v the first non 
                            bend_node vertex reachable through a direct chain of border_steps
                            beginning with the current one,
                            the pin_number holds the desired distance between the leftmost 
                            corner of v and the attachment (on v) of the gdtedge which the 
                            border_step belongs to. 
			<LI>
			    If the thickness value is greater than 1, then the gdtedge 
                            represents different, overlapped edges, all incident to the same 
                            vertex "v", and the pin_numer is used to specify the desired attachment 
                            of the overlapping edges to such vertex.
                            Namely, let "e" be the gdtedge (if any) orthogonal to the gdtedge which 
                            the border step belongs to, and such that "e" is incident to the 
                            start of the border_step if "v" is reachable through a directed path 
                            beginning with the current border step, or to the stop of border_step,
                            otherwise.
			    Then, the pin_number holds the desired distance between the 
                            leftmost corner of "v" and the attachment of "e" to "v". 
			</UL>
			*/
		
			int 
		pin_number_of_border_step 
			(
			border_step s
			) const;		
			
			/**
			Return the sum of the thickness of the two border steps of e minus 1.
			*/

			int 
		thickness_of_edge	     
			(
			gdtedge e
			) const;			

			
			/**
			Return the thickness of border_step of gdtedge e with start gdtnode v.
			*/
		
			int 
		right_thickness_of_edge  
			(
			gdtedge e, 
			gdtnode v
			) const;			
	
			
			/**
			Return the thickness of border_step of gdtedge e with start gdtnode the opposite(v,e).		
			*/

			int 
		left_thickness_of_edge   
			(
			gdtedge e, 
			gdtnode v
			) const;			
			
			
			/**
			Return the total number of bends of the orthogonal representation.
			*/

			int 
		number_of_bends 	     
			(
			) const;			
			
			
			/**
			Return the number of bends on gdtedge.
			*/

			int 
		number_of_bends 	     
			(
			gdtedge e
			) const;			
			
			
			/**
			Return the number of bends on a list of edges.
			*/

			int 
		number_of_bends 	    
			(
			gdt::gdtlist<gdtedge> ls_edge
			) const;		
			
			
			/**
			Return the number of border_step.
			*/

			int 
		number_of_bends 	     
			(
			border_step s
			) const;			
			
			
			/**
			Return the number of left-turns of border_step.
			*/

			int 
		number_of_left_bends     
			(
			border_step s
			) const;
			
			
			/**
			Return the number of right-turns of border_step.
			*/

			int 
		number_of_right_bends    
			(
			border_step s
			) const;			
			
			
			/**
			Return the maximum number of bends on an gdtedge.
			*/
		
			int 
		max_number_of_bends_on_edge 
			(
			) const;
			
			
			/**
			Return the number of left-turns wolking on border of face.
			*/
		
			int 
		number_of_left_turns_around_face   
			(
			face f
			) const;			
			
			
			/**
			Return the number of right-turns wolking on border of face.
			*/
			
			int 
		number_of_right_turns_around_face  
			(
			face f
			) const;			
			
			
			/**
			Return the number of left-turns wolking from a border_step
			to another of the same face.
			*/
		
			int 
		number_of_left_turns_along_border  
			(
			border_step sv,
			border_step sw
			) const;
			
			
			/**
			Return the number of right-turns wolking from a border_step 
			to another of the same face.
			*/			
		
			int 
		number_of_right_turns_along_border 
			(
			border_step sv,
			border_step sw
			) const;			
			
			
			/**
			Return the mean number of bends for gdtedge.
			*/

			double 
		mean_number_of_bends_on_edge 
			(
			) const;		
				
			
			/**
			Return the bends standard deviation.
			*/
		
			double
		bend_standard_deviation 	    
			(
			) const;			
			
			
			/**
			Return the external face.
			*/

			face
		external_face 
			(
			) const;			

			
			/**
			Return the bend-constraint type on gdtedge.
			*/
		
			bend_constraint	
		get_constraint_on_bends_of_edge 
			(
			gdtedge e
			) const;			
		
			
			/**
			Return the current layout algorithm.
			*/

			algorithm_type  
		get_layout_algorithm	        
			(
			) const;			
			
			
			/**
			Return true if the checking on local consistency is suspended.
			*/
		
			bool 
		get_local_consistency_is_suspended   
			(
			) const;			

			
			/**
			Return true if the diagnostic_printouts flag is enabled.
			*/

			bool 
		get_diagnostic_printouts_are_enabled 
			(
			) const;			
			
			
			/**
			Return true if the border of face is really closed.
			*/

			bool 
		border_is_closed          
			(
			face f
			) const;		
	
			
			/**
			Return true if border_step turns left with respect its 
			previous border_step.
			*/

			bool 
		border_step_turns_left    
			(
			border_step s
			) const;		
			
			
			/**
			Return true if border_step turns right with respect its
			previous border_step.
			*/

			bool 
		border_step_turns_right   
			(
			border_step s
			) const;			
			
			
			/**
			Return true if border_step turns back with respect its 
			previous border_step.
			*/

			bool 
		border_step_turns_back    
			(
			border_step s
			) const;			

			
			/**
			Return true if border_step goes straight with respect its 
			previous border_step.
			*/

			bool 
		border_step_goes_straight 
			(
			border_step s
			) const;			


			/**
			Find a border_step turning left in gdt::gdtlist<border_step>, 
			starting from position start_pos.				
			*/

			gdt::list_item 
		find_border_step_turning_left  		
			(
			gdt::gdtlist<border_step>& bsl, 
			gdt::list_item start_pos = NULL
			) const;			

			
			/**
			Find a border_step turning right in gdt::gdtlist<border_step>, 
			starting from position start_pos.
			*/

			gdt::list_item 
		find_border_step_turning_right 		
			(
			gdt::gdtlist<border_step>& bs, 
			gdt::list_item start_pos = NULL
			) const;			

			
			/**
			Find a border_step turning right or left in gdt::gdtlist<border_step>, 
			starting from position start_pos.
			*/

			gdt::list_item 
		find_border_step_turning_left_or_turning_back 
			(
			gdt::gdtlist<border_step>& bs,
			gdt::list_item start_pos = NULL
			) const;		

			
			/**
			Return the flow cost in the last computation apply_layout_algorithm
			(INFINITE when no solution is found)
			*/
		
			int  
		cost_of_last_algorithm
			(
			) const;	
			
			
			/**
			Return the list of border_steps that do not go straight 
			on border of face.
			*/

			gdt::gdtlist<border_step> 
		turning_border_steps_of_face 
			(
			face f
			) const;		

			
			/**
			Return all edges on each side of the gdtnode v in 
			counterclockwise order.
			*/

			void 
		edges_on_each_side_of_node 
			(
			gdtnode v, 
			gdt::gdtlist<gdtedge>& L1, 
			gdt::gdtlist<gdtedge>& L2, 
			gdt::gdtlist<gdtedge>& L3, 
			gdt::gdtlist<gdtedge>& L4
			) const;			
 		
 			
			
 			/**
			Return true when the face is regular, that is
			when the face has not a pair of kitty corners 
			(see definition of turn-regularity).
			If the face f is not regular a pair of border steps
			r1 and r2 is returned, such that their stop nodes are
			a pair of kitty corners.
			PRECONDITIONS: face must be linearized, that is it has
			not bends.
			*/
 			
 			bool
 		face_is_regular
 			(
 			face f, 
 			border_step& r1, 
 			border_step& r2
 			) const;
 			
 			
			/**
			Return true if the graph is really orthogonal
			This method can be used to check the consistency
			of the data structures
			*/
			
 			bool
		is_orthogonal 
			();
			
			
			
			/**
			return true if the gdtnode has degree two and two angles of 180 degree			
			*/
			
			bool
		node_is_flat
			(
			gdtnode v
			) const;
			
		
			

 		/*
		CATEGORY update
		Update operations.
		*/
			
			
			/**
			Split gdtedge by creating a new gdtnode on it, and returning it.
        		PRECONDITIONS: gdtedge must have no bends.
        		NOTICE	: this method is implemented to correctly work for
			 	  4-planar graphs only.
			*/

        		gdtnode 
		new_node 
			(
			gdtedge e, 
			int new_id = AUTO_ID
			);          	   				


			/**
			Split gdtedge e by creating a new gdtnode on it, and returning it.
			The new gdtnode is added on segment number seg_num starting from
			gdtnode v.
        		PRECONDITIONS: seg_num must be not greater than the number of segments of gdtedge e;
			               also, the segment must have sufficient space to host a new gdtnode;
			*/

        		gdtnode 
		new_node 
			(
			gdtedge e,
			gdtnode v,
			int seg_num, 
			int new_id = AUTO_ID
			);          	   				
			
			
			/**
			Create and return a new gdtnode, by attaching it to the gdtnode "v".
			A new straight gdtedge is created to connect the new gdtnode and "v". 
			Such gdtedge is inserted after the gdtedge "e" in counterclockwise 
			order around "v", with an angle equal to "alpha". 
			"new_node_id" and "new_edge_id" specify the identifiers of the new
			gdtnode and the new gdtedge, respectively.
        		PRECONDITIONS: "alpha" should be in {_090, _180, _270, _360}, and it should be
			               possible to add the new gdtnode without generating _000 angle.
			*/

        		gdtnode 
		new_node 
			(
			gdtnode v,
			gdtedge e,
			angle_type alpha, 
			int new_node_id = AUTO_ID,
			int new_edge_id = AUTO_ID
			);          	   
			
			
			/**
			Split face f by adding a new gdtedge between v and w. The new gdtedge is returned.
			PRECONDITIONS: v and w are vertices of the border of f and v != w
			NOTICE       : When v (or w) is the start-gdtnode of a bridge,
			the new gdtedge is inserted on the right of that bridge, 
			moving on the bridge from v (w) to its opposite.
			This method is implemented to correctly wotk for 4-planar graphs only.
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
			Split a face by adding a new gdtedge between v and w, 
			and put it after ev around v and after ew around w. 
			The new gdtedge is returned.
			PRECONDITIONS: v and w are vertices of the border of a same 
			face and operation can be executed preserving the planarity
			NOTICE       : this method is implemented to correctly wotk 
			for 4-planar graphs only.
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
			Replace gdtnode v with bend.
			PRECONDITIONS: v has degree 2
			*/

			gdtedge 
		replace_node_with_bend   
			(
			gdtnode v
			);						
		
			
			/**
			Replace the bend at position pos along gdtedge e,
			starting from v, with a dummy gdtnode.
			Dummy gdtnode is stored in N, and the two dummy edges derived
			by splitting are stored in E.			
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

			
			/**
			Replace all bends of gdtedge e with dummy nodes. 
			Dummy nodes are stored in N and dummy edges are stored in E.
			*/ 

			void 
		replace_bends_with_nodes 
			(
			gdtedge e, 
			gdt::gdtlist<gdtnode>& N, 
			gdt::gdtlist<gdtedge>& E
			);		
			
			
			/**
			Replace all bends of edges of face f with dummy nodes.
			Dummy nodes are stored in N and dummy edges are stored in E.
			*/ 
													
			void 
		replace_bends_with_nodes 
			(
			face f, 
			gdt::gdtlist<gdtnode>& N, 
			gdt::gdtlist<gdtedge>& E
			);		

			
			/**
			Replace all bends of graph with dummy nodes.
			Dummy nodes are stored in N and dummy edges are stored in E.
			*/ 
													
			void 
		replace_bends_with_nodes 
			(
			gdt::gdtlist<gdtnode>& N, 
			gdt::gdtlist<gdtedge>& E
			);			

			
			/**
			Execute a rectangularization of graph and store all dummy nodes 
			in N and all dummy edges in E.
			Preconditions : the graph must be 4-planar
			*/

		
			void 
		decompose_all_faces_into_smaller_rectangular_faces 
			(
			gdt::gdtlist<gdtnode>& N, 
			gdt::gdtlist<gdtedge>& E
			);


			
			/**
			Execute a regularization heuristic of graph and store all dummy nodes 
			in N and all dummy edges in E. The heuristic is chosen according to the 
			given alg. Return the number of irregular faces.
			Preconditions : the graph must be 4-planar.
			*/

			int 
		regularize_all_faces
			(
			gdt::gdtlist<gdtnode>& N, 
			gdt::gdtlist<gdtedge>& E,
			algorithm_type alg
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
			This method is used to make an undi_graph as an 
			orth_plan_undi_graph, manteining the same internal variables.
			If err_mess = true return an error message when
			a layout is not possible
			WARNING: ug has an empty body after this method is applied
			PRECONDITIONS: ug must be connected and with at least one gdtnode
			*/
										
			void 
		steal_from 
			(
			undi_graph& ug, 
			algorithm_type alg = PLAN_ORTH_OPTIMAL,
			bool err_mess = true
			);		

			
			/**
			Make *this with the plan_undi_graph pug internal variables 
			and cut these variables from pug. 
			This method is used to make a plan_undi_graph as an 
			orth_plan_undi_graph, manteining the same internal variables.
			If err_mess = true return an error message when
			a layout is not possible
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
			Make *this with the plan_undi_graph pug internal variables 
			and setting external face before applying the selected layout algorithm,
			and cut these variables from pug. 
			This method is used to make a plan_undi_graph as an 
			orth_plan_undi_graph, manteining the same internal variables.
			If err_mess = true return an error message when
			a layout is not possible
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
			Copy all private variables of orth_plan_undi_graph in *this.		
 			*/
	
			void 
		mirror 
			(
			orth_plan_undi_graph& opug
			);							

			
			/**
			Cut all private variables in *this.
			*/
 			
			void 
		forget 
			(
			);                    						
			
			
			/**
			Set the external face with face.
			NOTICE: if local consistency is not suspended, 
			the current layout algorithm is executed automatically
			*/

			void 
		set_external_face      
			(
			face ef
			);						
			
			
			/**
			Make "s" as the new reference border step
			*/
			
			void
		set_reference_border_step
			(
			border_step s
			);
			
			
			/**
			Make the border step of gdtedge "e" with start gdtnode "v"
			as the new reference border step
			*/
			
			void
		set_reference_border_step
			(
			gdtnode v,
			gdtedge e
			);
			
			
			/**
			Set the layout algorithm with algorithm_type. 
			NOTICE: if local consistency is not suspended,
			the set layout algorithm is automatically applied
			*/
										
			void
		set_layout_algorithm   
			(
			algorithm_type la
			);					

			
			/**
			Apply the current layout algorithm,
			and return the flow-cost if the algorithm
			applied is PLAN_ORTH_OPTIMAL or 
			PLAN_ORTH_SLOW, and zero otherwise.
			If err_mess = true return an error message when
			a layout is not possible
			*/
													
			int
		apply_layout_algorithm 
			(
			bool err_mess = true
			);								
			
			
			/**
			Suspend local consistency on data structure.
			*/

 			void 
		suspend_local_consistency   
			(
			);							

			
			/**
			Restore local consistency on data structure.
			*/
		
			void 
		restore_local_consistency   
			(
			);							
			
			
			/**
			Enable diagnostic printouts.
			*/

			void 
		enable_diagnostic_printouts 
			(
			);							

			
			/**
			Disable diagnostic printouts.
			*/
		
			void 
		disable_diagnostic_printouts
			(
			);						
	
		/*
		CATEGORY io_operations
		Input / output operations
		*/
			
			
			/**
			Print the graph on ostream os; print_mode can be BY_NODES,
			BY_EDGE, BY_FACES, COMPLETE.
			*/

			void
		print
			(
			print_mode mode = BY_FACES,
			std::ostream& os = std::cout
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
			Print gdtedge on ostream os.
			*/
			
			void 
		print 
			(
			gdtedge e, 
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
			
			
			/**
			Print constraint on ostream os.
			*/
			
			void 
		print 
			(
			constraint c,  
			std::ostream& os = std::cout
			) const;				
			
			
			/**
			Print the separation pair on ostream os.
			*/
			
			void
		print 
			(
			separation_pair sp, 
			std::ostream& os = std::cout
			) const;				
	};


#endif
