/********************************************************************************
+
+  rm3_plan_undi_graph.h
+
+  This file is part of GDToolkit. It can be
+  used free of charge in academic research and teaching.
+  Any direct or indirect commercial use of this software is illegal
+  without a written authorization of
+  the Dipartimento di Informatica e Automazione
+  Universita' di Roma Tre, Roma, ITALIA
++
+
*******************************************************************************/
/*! \file */


#ifndef RM3_PLAN_UNDI_GRAPH_H
#define RM3_PLAN_UNDI_GRAPH_H


#include <GDT/gdtlist.h>
#include <GDT/gdtmap.h>
#include <GDT/rm3_global.h>
#include <GDT/rm3_undi_graph.h>

#define nil 0


//#ifndef face
#undef face
#define face GDT_face
//#endif

	/*
	SOURCEFILE rm3_plan_undi_graph.h
	To be defined.
	*/

//-----------------------------------------------------------------------------
// plan_undi_graph:	base class for all planar undirected graphs
// 			with planar embedding.
//
// W.Didimo, A.Leonforte (1996)
//-----------------------------------------------------------------------------

class struct_face; //Forward declaration       					

class struct_border_step; //Forward declaration					
	
typedef struct_face* face;  //Forward declaration					

typedef struct_border_step* border_step; //Forward declaration			

const face 	  NULL_FACE = (face)NULL;			// NULL face
const border_step NULL_BORDER_STEP = (border_step)NULL;		// NULL border_step

inline int compare(const face& x, const face& y) 
{ if (x < y) return -1; 
  else if (x > y) return  1; 
  else return 0;
}

inline int compare(const border_step& x, const border_step& y) 
{ if (x < y) return -1; 
  else if (x > y) return  1; 
  else return 0;  
}



// -----------------------
// useful type definitions
// -----------------------



	struct 		// vertical segment representing an gdtedge in a visibility representation
vertical_segment
	{
	int y_min;
	int y_max;
	int x;
	};
	
	struct		// horizontal segment representing a gdtnode in a visibility representation
horizontal_segment
	{
	int x_min;
	int x_max;
	int y;
	};


	/**
	GLOBALTYPE struct_separation_pair
	Local-plan_undi structure for separation_pair.
	In this constext, a separation_pair represents
	a pair of nodes such that, if deleted, destroy
	the graph's connectivity. 
	This structure contains the pair of nodes, 
	'pole1', 'pole2' and a list of faces shared by
	these nodes.
	*/

	class GDT_NONSTANDARD_DECL
struct_separation_pair
	{
	friend class struct_face;
	friend class struct_border_step;
	friend class plan_undi_graph;
	
	private:
		gdtnode	   pole1,	 // separation_pair's nodes
	     		   pole2;
		gdt::gdtlist<face> shared_faces; // faces shared by separation_pair
		
	public:
	
		~struct_separation_pair
			(
			);
	};


	/**
	GLOBALTYPE separation_pair
	Reference to the struct_separation_pair global type.
	*/

	typedef struct_separation_pair*
separation_pair;


	/**
	GLOBALTYPE struct_border_step
	Local-plan_undi structure for border_step.
	In this context, a border_step represents an
	gdtedge with an associated moving direction. 
	Namely, each gdtedge e=(v1,v2) has associated 
	exactly two border_steps: 'bs1' moving from v1
	and 'bs2' moving from v2. Nodes v1 and v2 are  
	called 'start' ('stop') of border_steps bs1(bs2) 
	and bs2(bs1), respectively 
	(don't confuse with the start and stop nodes 
	of directed edges). bs1(bs2) is said to be the 
	the 'opposite' border_step of bs2(bs1).
	Also, the right face that we see moving along
	gdtedge e from gdtnode v1(v2) will be called the 
	'face of border_step bs1 (bs2)'.
	Finally, gdtedge e is called 'gdtedge of border 
	steps bs1 and bs2'.
	*/

	class GDT_NONSTANDARD_DECL
struct_border_step
	{
	friend class struct_face;
	friend class plan_undi_graph;
	
	
	
	private:
	  	gdtedge 	  owner_edge;		// gdtedge of border_step		
		face 	  owner_face;		// face of border_step
		gdtnode 	  start_node;		// start gdtnode of border_step
		gdt::list_item pos_in_border_steps;	// position-item of border_step into the border_steps list
	
	public:
		static int counter;
		
		struct_border_step();
		~struct_border_step();
	};
	
	/**
	GLOBALTYPE border_step
	Reference to the 'struct_border_step' global type.
	*/
	
	typedef struct_border_step*
border_step;


	/**
	GLOBALTYPE struct_face
	Local-plan_undi structure for face.
	In this constext, a face is represnted by two 
	ordered lists, that describe the face border:
	1 - a list of edges;	(edges belonging to face)
	2 - a list of border_steps;
	Cardinality of the list 2 is greather or equal
	than the cardinality of the list 1. Infact, 
	both the two border_steps of an gdtedge might 
	have the same face.
	*/

	class GDT_NONSTANDARD_DECL
struct_face
	{
	friend class plan_undi_graph;
	
	private:
		int 		  id;			// face identifier
		plan_undi_graph*  owner;		// pointer to *this plan_undi_graph
		gdt::list_item 	  pos_in_f_list;	// position-item of face into the faces list
		gdt::gdtlist<gdtedge> 	  edges; 		// list of edges forming face
		gdt::gdtlist<border_step> border_steps;		// list of border_steps forming face
		
	public:

		/*
		CATEGORYremoved constructors_destructors
		Constructors and destructors.
		*/
		
			/**
			Destructor.
			*/

		~struct_face
			(
			);
		/**
		Constructor.
		*/

		struct_face
			(
			);
		/**
		Return a reference to *this plan_undi_graph.
		*/

			plan_undi_graph& 
		get_owner
			(
			);		
	};
	
	
	/**
	GLOBALTYPE struct_plan_edge_info
	Local-plan_undi structure for gdtedge.
	*/


	class 
struct_plan_edge_info
	{
	friend class plan_undi_graph;
	
	private:
		face left_face;				// faces shared by gdtedge
		face right_face;
		//
		border_step left_face_border_step;	// border_steps of gdtedge
		border_step right_face_border_step;
		//
		gdt::list_item pos_in_left_face_edges;	// position-items of gdtedge into the two lists of shared faces
		gdt::list_item pos_in_right_face_edges;
	public:

		/*
		CATEGORYremoved constructors_destructors
		Constructors and destructors.
		*/
		
			/**
			Constructor.
			*/

		struct_plan_edge_info
			(				
			);
	};


	/**
	CLASS plan_undi_graph
	A 'plan_undi_graph' represents a connected undi_graph with at least one gdtedge 
	and a given planar embedding.
	Each plan_undi_graph has a set of faces determineted by its planar embedding. 
	All update operations preserve the planarity of the embedding. 
	*/


	class GDT_NONSTANDARD_DECL
plan_undi_graph
	: public undi_graph
	{
	friend class undi_graph;
	private:
		// -----------------
		// private variables
		// -----------------
		
		
		gdt::gdtlist<face>*          		 f_list;				// list of all faces of the graph
		gdt::gdtedge_map<struct_plan_edge_info>* edge_info;				// correspondence gdtnode --> plan_undi-gdtedge structure
		gdt::gdtmap<int,face>*			 face_with_id;				// correspondence face-identifier --> face
		int 	 	                 max_face_id;				// maximum value of a fec-identifier
		int				 last_crosses;				// number of cross nodes added by the last planarization step 		
		
		void local_new  ();   							// create a new set of pointers for the not-inherited class-part
		void local_del  ();   							// delete all the not-inherited class-part
		void local_renew(); 							// utility function : just local_del(), then local_new()
		void local_init (planarize_options po, bool err_mess); 			// init the not-inherited class-part
		
		bool build_faces(); 							// if the given embedding is already planar build the faces, else return false
		
		void local_set								// set all private variables
			(
			gdt::gdtlist<face>*,         
			gdt::gdtedge_map<struct_plan_edge_info>*,
			gdt::gdtmap<int,face>*,
			int,
			int
			);
		
		// ---------------------------------------------------
		// These methods are needed to hide the corresponding
		// undi_graph public methods.
		// ---------------------------------------------------
		
		gdtnode new_node (int new_id=AUTO_ID);
		gdtedge new_edge (gdtnode, gdtedge, gdtnode, int new_id=AUTO_ID);
		gdtedge new_edge (gdtnode, gdtnode, int new_id=AUTO_ID);

		// ---------------------------------------------------



		face new_face (int new_id=AUTO_ID);							// create a new face structure with id new_id and return its pointer
		void del_face (face);									// delete the face structure with pointer face

		border_step new_border_step 	      (gdtedge, gdtnode, face);				// create a new border_step structure with gdtedge and face as owner_edge
													// and owner_face, respectively, and with gdtnode as start_node. Then return its pointer

		void set_right_face_moving_along_edge (gdtedge e,gdtnode v,face f);				// append gdtedge e to the border of face f.
													// Set face f as the one visible on the right when moving along gdtedge e from v to its opposite.
													// PRECONDITIONS: v belongs to e

		void set_right_face_moving_along_edge (gdtedge e,gdtnode v,gdtedge eref,gdtnode vref,int dir=gdt::after);
													// find the face f visible on the right when moving along gdtedge eref from gdtnode vref to its opposite.
													// Insert gdtedge e to the border of face f, after or before gdtedge eref according to dir.
													// Set face f as the one visible on the right when moving along gdtedge e from v to its opposite.
													// PRECONDITIONS: v belongs to e and vref belongs to eref
	
		void reset_edge_info (gdtedge);								// reset all plan_edge_info structure internal variables
		
		void find_vertical_lengths (gdt::gdtedge_array<int>& len, gdtedge e_st, gdt::gdtedge_array<int>& cost);	// see below..	
		
				// -----------------------------------------------------
				// find vertical lengths for the edges of *this, using
				// a min-cost flow technique. 
				// len  = lengths returned;
				// cost = gdtedge costs;
				// e_st = st-gdtedge.
				// 
				// PRECONDITION: graph must be st-planar with e_st=(s,t) 
				// -----------------------------------------------------
				
				

		
	protected:
	
		void set_source (gdtedge, gdtnode);								// set source of gdtedge with gdtnode
		void set_faces_shared_by_separation_pair (const gdt::gdtlist<face>&, separation_pair);		// set list of faces shared by separation_pair
		void set_separation_pair_pole1		 (gdtnode, separation_pair);			// set pole1 of separation_pair with gdtnode
		void set_separation_pair_pole2		 (gdtnode, separation_pair);			// set pole2 of separation_pair with gdtnode
		
		gdtnode first_node_visited_while_moving_on_edge_around_face  (gdtedge, face) const;			// return the first gdtnode visited while moving on gdtedge around face
		gdtnode second_node_visited_while_moving_on_edge_around_face (gdtedge, face) const;			// return the second gdtnode visited while moving on gdtedge around face 

		void calculate_dual (plan_undi_graph&, gdt::gdtmap<face,gdtnode>&);				// see below..				
		void calculate_dual (plan_undi_graph&, gdtedge, gdt::gdtmap<face,gdtnode>&);				// see below..
					
				// -------------------------------------------------------------------------
				// Calculate the dual graph (eventually specifying an st-gdtedge on the 
				// external face) and return the mapping face --> gdtnode.
				// ADVISE: These two methods are not safe in general.
				// 	   They are used for visibility drawing 
				//         (directed, biconnected graphs) only.
				// -------------------------------------------------------------------------

					
	public:
		/*
		CATEGORY constructors_destructors
		Constructors and destructors.
		*/
		
			
			/**
			Empty constructor.
			*/
	
		plan_undi_graph  
			(
			);								
		
			
			/**
			Destructor.
			*/

		~plan_undi_graph 
			(
			);	
			
			
			/**
			Constructor from the undi_graph class. 
			If the embedding of the undi_graph is not planar
			a planarization step is executed in which dummy nodes 
			may be added to replace crosses.
			Cross nodes are marked RM3_CROSS_ON_REAL_EDGE.
			Also, two possible planarization option 
			can be specified: PRESERVE_EMBEDDING and 
			DO_NOT_PRESERVE_EMBEDDING, in order to preserve (not preserve)
			the original embedding. Note that if DO_NOT_PRESERVE_EMBEDDING 
			is specified, no dummy nodes are added when the graph is 
			planar (that is it admits a planar embedding).
			If err_mess == true, an error message is returned when the 
			embedding is not found. <P>
			PRECONDITIONS: undi_graph must be connected and with at least an gdtedge
			*/

		plan_undi_graph 
			(
			const undi_graph& ug, 
			planarize_options po = DO_NOT_PRESERVE_EMBEDDING,
			bool err_mess = true
			);														  
			
			
			/**
			Copy constructor.
			*/
			
		plan_undi_graph 
			(
			const plan_undi_graph& pug
			);						
		
		/*
		CATEGORY operators
		Operators.
		*/
		
			
			/**
			Equality operator from the undi_graph class. 
			If the embedding of the undi_graph is not planar
			a planarization step is executed in which dummy nodes 
			may be added to replace crosses.
			Cross nodes are marked RM3_CROSS_ON_REAL_EDGE. 
			If the graph is planar (that is it admits a planar embedding)
			no dummy nodes are added. <P>
			PRECONDITIONS: undi_graph must be connected and with at least an gdtedge
			*/

			plan_undi_graph& 
		operator = 
			(
			const undi_graph& ug
			);																		
			
			/*
			Copy equality operator.
			*/

			plan_undi_graph& 
		operator = 
			(
			const plan_undi_graph& pug
			);					
		
		/**
		CATEGORY access
		Access operations.
		*/

			
			/**
			Get all private variables.
			*/

			void 
		local_get												
			(
			gdt::gdtlist<face>*& 			  p1,         
			gdt::gdtedge_map<struct_plan_edge_info>*& p2,
			gdt::gdtmap<int,face>*& 		  p3,
			int&				  p4,
			int&				  p5
			);

			
			/**
			Automatically generate a new compatible face identifier.
			*/
	
			int 
		generate_face_id 
			(
			);								

			
			/**
			Return the idendifier of gdtnode.
			*/
		
			int 
		id 
			(
			gdtnode v
			) const;								

			/**
			Return the identifier of gdtedge.
			*/

			int 
		id 
			(
			gdtedge e
			) const;								

			/**
			Return the identifier of face.
			*/
				
			int 
		id 
			(
			face f
			) const;								


			/**
			Return the maximum value for a face-identifier.			
			*/
			
			inline	
			int 
		get_max_face_id       
			(
			) const;				


			/**
			Return the number of edges incident gdtnode.
			*/
				
			int 
		degree 
			(
			gdtnode v
			) const;							

			/**
			Return either the number of edges or border_steps in face
			according to bool is false or true.
			*/
				
			int 
		degree 
			(
			face f, 
			bool bridge = false 
			) const; 						

			/**
			Return the number of faces of the graph.
			*/
				
			int 
		number_of_faces 
			(
			) const;						
		
			/**
			Return the number of dummy nodes added as crosses.
			Return INFINITE, if the last planarization step failed
			*/
			
			int
		number_of_cross_nodes
			(
			) const;
			
			/**
			Return true if gdtnode belongs to face.
			*/
				
			bool 
		node_belongs_to_face 
			(
			gdtnode v, 
			face f
			) const;		

			/**
			Return true if gdtedge belongs to gdtedge.
			*/
				
			bool 
		edge_belongs_to_face 
			(
			gdtedge e, 
			face f
			) const;		

			/**
			Return the pole1 of separation_pair.
			*/
				
			gdtnode 
		separation_pair_pole1 
			(
			separation_pair sp
			) const;		

			/**
			Return the pole2 of separation_pair.
			*/
				
			gdtnode 
		separation_pair_pole2 
			(
			separation_pair sp
			) const;		

			/**
			Return the face with identifier id if there exists, 
			else return NULL_FACE.
			*/
				
			face 
		find_face 
			(
			int ref_id
			) const;		 

			/**
			Return the first face in the face-list of the graph.
			*/
					
			face 
		first_face
			(
			) const;	

			/**
			Return the last face in the face-list of the graph.
			*/
				
			face 
		last_face 
			(
			) const;	

			/**
			Return the successive of face in the face-list of the graph.
			*/
				
			face 
		succ_face 
			(
			face f
			) const;					

			/**
			Return the previous of face in the face-list of the graph.
			*/
				
			face 
		pred_face 
			(
			face f
			) const;		

			/**
			Return the face adjacent to face by gdtedge.
			*/
				
			face 
		adj_face 			 
			(
			face f, 
			gdtedge e
			) const;		

			/**
			Return the right face moving along gdtedge starting from gdtnode.
			*/
			
			face 
		right_face_moving_along_edge 
			(
			gdtedge e, 
			gdtnode v
			) const;	

			/**
			Return the left face moving along gdtedge starting from gdtnode.
			*/
				
			face 
		left_face_moving_along_edge  
			(
			gdtedge e, 
			gdtnode v
			) const;	

			/**
			Return the face of border_step.
			*/
			
			face 
		face_of_border_step 	  
			(
			border_step s
			) const;	

			/**
			Return a constant reference to the face-list of the graph.
			*/
			
			const gdt::gdtlist<face>& 
		all_faces	
			(
			) const;					

			/**
			Return the first gdtedge of the adj-edges list of gdtnode.
			*/
		
			gdtedge 
		first_adj_edge  
			(
			gdtnode v
			) const;		

			/**
			Return the first gdtedge of the adj-edges list of face.
			*/
				
			gdtedge 
		first_adj_edge  
			(
			face f
			) const;	

			/**
			Return the last gdtedge of the adj-edges list of gdtnode.
			*/
			
			gdtedge 
		last_adj_edge   
			(
			gdtnode v
			) const;		

			/**
			Return the last gdtedge of the adj-edges list of face.
			*/
				
			gdtedge 
		last_adj_edge   
			(
			face f
			) const;		

			/**
			Return the previous of gdtedge in the adj-edges list of gdtnode.
			*/
				
			gdtedge 
		adj_pred        
			(
			gdtedge e,
			gdtnode v
			) const;		

			/**
			Return the previous of gdtedge in the adj-edges list of face.
			*/
				
			gdtedge 
		adj_pred        
			(
			gdtedge e,
			face f
			) const;		

			/**
			Return the successive of gdtedge in the adj-edges list of gdtnode.
			*/
		
			gdtedge 
		adj_succ        
			(
			gdtedge e,
			gdtnode v
			) const;	

			/**
			Return the successive of gdtedge in the adj-edges list of face.
			*/
				
			gdtedge 
		adj_succ 
			(
			gdtedge e,
			face f
			) const;	

			/**
			Return the cyclic previous of gdtedge in the adj-edges list of gdtnode.
			*/
				
			gdtedge 
		cyclic_adj_pred 
			(
			gdtedge e,
			gdtnode v
			) const;	

			/**
			Return the cyclic previous of gdtedge in the adj-edges list of face.
			*/
			
			gdtedge 
		cyclic_adj_pred 
			(
			gdtedge e,
			face f
			) const;		

			/**
			Return the cyclic successive of gdtedge in the adj-edges list of gdtnode.
			*/
				
			gdtedge 
		cyclic_adj_succ 
			(
			gdtedge e,
			gdtnode v
			) const;		

			/**
			Return the cyclic successive of gdtedge in the adj-edges list of face.
			*/
				
			gdtedge 
		cyclic_adj_succ 
			(
			gdtedge e,
			face f
			) const;		

			/**
			Return the gdtedge of border_step.
			*/
				
			gdtedge 
		edge_of_border_step  
			(
			border_step s
			) const;		

			/**
			Return the list of nodes shared by the two faces.
			*/
				
			gdt::gdtlist<gdtnode> 
		nodes_shared_by_faces 
			(
			face f1,
			face f2
			) const;		 

			/**
			Return the circular clockwise list of gdtnode belonging to face. <P>
			NOTE: a gdtnode might appear more than one time, walking on the face border
			*/
				
			gdt::gdtlist<gdtnode> 
		adj_nodes 
			(
			face f
			) const;  	

			/**
			Return the adj-edges list of gdtnode.
			*/
		
			gdt::gdtlist<gdtedge> 
		adj_edges 
			(
			gdtnode v
			) const;		

			/**
			Return the list of gdtedge of face if v == NULL_NODE, else return
			the edges adjacent to v on face. <P>
			PRECONDITIONS: if v!=NULL_NODE it must belong to face
			*/
				
			gdt::gdtlist<gdtedge> 
		adj_edges 
			(
			face f, 
			gdtnode v=NULL_NODE
			) const;		

			/**
			Return a list of all faces adjancet gdtnode.
			*/
												
			gdt::gdtlist<face> 
		adj_faces 
			(
			gdtnode v
			) const;	 

			/**
			Return the list of edges shared by the two faces.
			*/
			
			gdt::gdtlist<gdtedge> 
		edges_shared_by_faces 
			(
			face f1,
			face f2
			) const;	

			/**
			Return the list of edges entering gdtnode while moving around face.
			*/
			
			gdt::gdtlist<gdtedge> 
		edges_entering_node_while_moving_around_face 
			(
			gdtnode v,
			face f
			) const;	

			/**
			Return the list of faces shared by separation_pair.
			*/
				
			gdt::gdtlist<face> 
		faces_shared_by_separation_pair 
			(
			separation_pair sp
			) const;	

			/**
			Return the position-item of gdtedge into the gdtedge-list of face.
			*/
		
			gdt::list_item 
		pos_in_border 
			(
			gdtedge e,
			face f
			) const;	

			/**
			Return the position-item of border_step into the border_step-list of face.
			*/
					
			gdt::list_item 
		pos_in_border 
			(
			border_step s
			) const;	

			/**
			Return the start gdtnode of border_step.
			*/
				
			gdtnode 
		start_of_border_step 
			(
			border_step s
			) const;	

			/**
			Return the stop gdtnode of border_step.
			*/
		
			gdtnode 
		stop_of_border_step  
			(
			border_step s
			) const;	

			/**
			Return the first border_step of the border_step-list of face.
			*/
				
			border_step 
		first_border_step        	  
			(
			face f
			) const;

			/**
			Return the last border_step of the border_step-list of face.
			*/
					
			border_step 
		last_border_step         	  
			(
			face f
			) const;
		
			/**
			Return the successive of border_step in the border_step-list
			of face of border_step.
			*/
					
			border_step 
		succ_border_step         	  
			(
			border_step s
			) const;

			/**
			Return the previous of border_step in the
			border_step-list of face of border_step.
			*/
				
			border_step 
		pred_border_step         	  
			(
			border_step s
			) const;	

			/**
			Return the cyclic successive of border_step in the 
			border_step-list of face of border_step.
			*/
					
			border_step 
		cyclic_succ_border_step  	  
			(
			border_step s
			) const;	

			/**
			Return the cyclic previous of border_step in the 
			border_step-list of face of border_step.
			*/
					
			border_step 
		cyclic_pred_border_step  	  
			(
			border_step s	
			) const;	

			/**
			Return the opposite of border_step.
			*/
				
			border_step 
		opposite_border_step     	  
			(
			border_step s
			) const;	 

			/**
			Return the border_step of gdtedge and having gdtnode as start_node.
			*/
			
			border_step 
		border_step_moving_along_edge 
			(
			gdtedge e, 
			gdtnode v
			) const;	

			/**
			Return the border_step-list of face.
			*/
				
			gdt::gdtlist<border_step> 
		adj_border_steps 		
			(
			face f
			) const; 	

			/**
			Return the list of border_steps having gdtnode as start_node.
			*/
					
			gdt::gdtlist<border_step> 
		border_steps_starting_at_node 
			(
			gdtnode v
			) const; 	  

			/**
			Return the border_step sequence between two border_steps.
			*/
					
			gdt::gdtlist<border_step> 
		border_step_sequence 		
			(
			border_step s1,
			border_step s2
			) const;	 

			/**
			Put (appending) in L the border step sequence between two border_steps.
			*/
					
			void 
		border_step_sequence 		
			(
			border_step s1,
			border_step s2,
			gdt::gdtlist<border_step>& L
			) const;	 


			/**
			Return the shortest sequence of border_steps from v1 to v2
			walking on border of face f.
			*/
		
			gdt::gdtlist<border_step> 
		border_step_sequence		
			(
			gdtnode v1,
			gdtnode v2, 
			face f
			) const;	
			
			
			/**
			Put (appending) in L the shortest sequence of border_steps from v1 to v2
			walking on border of face f.
			*/
		
			void 
		border_step_sequence		
			(
			gdtnode v1,
			gdtnode v2, 
			face f,
			gdt::gdtlist<border_step>& L
			) const;									  

			/**
			Return the separation_pair sp in sp_list such that its poles 
			have identifier idv1 and idv2 (or vice-versa), respectively.
			If it does not exist return NULL.
			*/
		
			separation_pair       
		find_separation_pair  
			(
			int idv1, 	
			int idv2, 
			const gdt::gdtlist<separation_pair>& sp_list
			) const; 								
						
			/**
			Return the list of all separation_pairs of the graph.
			*/
								
			gdt::gdtlist<separation_pair> 
		find_separation_pairs 
			(
			) const;			

			/**
			Make a dual plan_undi_graph 'dual_pug' and all mapping between face/gdtnode of 
     			primal and dual graphs. <P>
			NOTE:	bridges in the primal graph should cause loops in the dual graph,
				but loops are not allowed for plan_undi_graphs. So, bridges don't have
				a corresponding gdtedge in the dual graph => several primal nodes may fall
				in the same dual face => the 'dual_face_2_primal_node' map could not
				be defined.
			*/
		
			void 
		make_dual 
			(
			plan_undi_graph& dual_pug,
			gdt::gdtmap<face,gdtnode>& primal_face_2_dual_node, 
			gdt::gdtmap<gdtnode,face>& dual_node_2_primal_face,
     			gdt::gdtmap<gdtedge,gdtedge>& primal_edge_2_dual_edge, 
     			gdt::gdtmap<gdtedge,gdtedge>& dual_edge_2_primal_edge
     			) const;						
     				
 			/**
			Compute a visibility representation of the planar graph, having ext_face as
			external face. If compacted == true a min_cost_flow technique is used to reduce
			the total gdtedge length with respect to the gdtedge-cost array (*cp). If cp==NULL, 
			all edges are considered with same cost.
			Visibility is returned as an array of horizontal_segment structures (for nodes),
			and an array of vertical_segment structures (for edges). These structures are 
			defined as follows:
				<PRE>
				struct 
				     vertical_segment
					{
					int y_min;
					int y_max;
					int x;
					};
				
				struct		
				     horizontal_segment
					{
					int x_min;
					int x_max;
					int y;
					};
				</PRE><P>
			PRECONDITIONS: graph must be biconnected.
			*/
				
			void 
		compute_visibility_representation 
			(
			gdt::gdtnode_array<horizontal_segment>& seg_node, 
			gdt::gdtedge_array<vertical_segment>& seg_edge, 
			face ext_face, 
			bool compacted = false,
			gdt::gdtedge_array<int>* cp = NULL
			//gdt::gdtedge_array<int>* cp = NULL
			);						
				
			/**
			Compute a visibility representation of the planar st-digraph, having gdtedge e_st=(s,t) on
			the external face. If compacted == true a min_cost_flow technique is used to reduce
			the total gdtedge length with respect to the gdtedge-cost array (*cp). If cp==NULL, 
			all edges are considered with same cost.
			Visibility is returned as an array of horizontal_segment structures (for nodes),
			and an array of vertical_segment structures (for edges). These structures are 
			defined as follows:
			<PRE>	
				   struct 
					vertical_segment
						{
						int y_min;
						int y_max;
						int x;
						};
				
				   struct		
					horizontal_segment
						{
						int x_min;
						int x_max;
						int y;
						};
			</PRE><P>
			PRECONDITION: graph must be biconnected and an e_st-digraph).
			*/
			
			void 
		compute_visibility_representation 
			(
			gdt::gdtnode_array<horizontal_segment>& seg_node, 
			gdt::gdtedge_array<vertical_segment>& seg_edge, 
			gdtedge e_st, 
			bool compacted = false, 
			gdt::gdtedge_array<int>* cp = NULL
			);							
			
		/*
		CATEGORY translate
		Translate operations.
		*/

			/**
			Return the face with the same identifier as face 
			in plan_undi_graph if there exists, else return NULL_NODE.
			*/
				
			face        
		corresponding_face        
			(
			face f,        
			const plan_undi_graph& pug
			);  	

			/**
			Return the border_step such that gdtedge of border_step 
			and start of border_step are corrispondent in plan_undi_graph.
			*/

			border_step 
		corresponding_border_step 
			(
			border_step s, 
			const plan_undi_graph& pug
			);  	  

		/*
		CATEGORY update
		Update operations.
		*/

			/**
			Split gdtedge with a new gdtnode with identifier new_id, 
			and return such gdtnode.
			*/
		              
			gdtnode 
		new_node 
			(
			gdtedge e,                   
			int new_id=AUTO_ID
			);  			 

			/**
			Attach a new gdtnode to gdtnode after gdtedge (in counterclocwise circular order)
			and return it. The new gdtnode and new gdtedge have identifier 
			node_id and edge_id, respectively 
			*/
				
			gdtnode 
		new_node 
			(
			gdtnode n, 
			gdtedge e, 	       
			int node_id=AUTO_ID,			
			int edge_id=AUTO_ID
			);  		

			/**
			Insert a gdtnode inside face and link it with all nodes of face.
			*/
				
			gdtnode 
		star_in_face
			(
			face f
			);  					

			/**
			Insert a new undirected gdtedge with source v1 and terget v2,
			and id new_id, by splitting face f, and return it.
			PRECONDITIONS: both v1 and v2 belongs to f.
			*/
			
			gdtedge 
		new_edge 
			(
			gdtnode v1, 
			gdtnode v2, 
			face f, 
			int new_id=AUTO_ID
			);			

			/**
			Insert a new undirected gdtedge with source v1 and target v2,
			and identifier new_id, and return it.			
			Also the gdtedge is inserted after e1 around v1 anf after e2 around v2. <P>
			PRECONDITIONS: v1 and v2 belongs to a same face and the 
			operation can be executed preserving the planarity
			*/
														
			gdtedge 
		new_edge 
			(
			gdtnode v1, 
			gdtedge e1, 
			gdtnode v2, 
			gdtedge e2, 
			int new_id=AUTO_ID
			);	

			/**
			Delete gdtedge and return the face obtained by merging the 
			two previous faces of the deleted gdtedge.
			*/
			
			face 
		del_edge 
			(
			gdtedge e
			); 							

			/**
			Delete gdtnode and return the face obtained by recursively 
			merging the faces of the deleted edges.
			*/
				
			face 
		del_node 
			(
			gdtnode v
			);	
		
			/**
			Delete the edges incident on "v" and merge them
			in a single gdtedge, which is returned. <P>
			PRECONDITIONS: "v" has degree two.
			*/
				
			gdtedge 
		weld_edges_at_node 
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
			This method is used to make an undi_graph as a plan_undi_graph, 
			manteining the same internal variables.
			If err_mess == true an error message is returned when the 
			embedding is not found. <P>
			WARNING: ug has an empty body after this method is applied. <P>
			PRECONDITIONS: ug must be connected and with at least one gdtnode.
			*/
				
			void 
		steal_from 
			(
			undi_graph& ug,
			planarize_options po=DO_NOT_PRESERVE_EMBEDDING,
			bool err_mess = true
			);

			/**
			Copy all private variables of plan_undi_graph in *this.
			*/
														
			void 
		mirror 	
			(
			plan_undi_graph& pug
			);

			/**
			Cut all private variables in *this.
			*/
							
			void 
		forget 	
			(
			);								

			/**
			Assign identifier new_id to gdtnode. <P>
			PRECONDITIONS: there is not any gdtnode with new_id
			*/
		
			void 
		assign_id 
			(
			gdtnode v, 
			int new_id = AUTO_ID
			);				

			/**
			Assign identifier new_id to gdtedge. <P>
			PRECONDITIONS: there is not any gdtedge with new_id
			*/
																
			void 
		assign_id 
			(
			gdtedge e, 
			int new_id = AUTO_ID
			);					

			/**
			Assign identifier new_id to face. <P>
			PRECONDITIONS: there is not any face with new_id
			*/
				
			void 
		assign_id 
			(
			face f, 
			int new_id = AUTO_ID
			);				
		
		
			/**
			Update the max_facee_id value to the current maximum face-identifier.
			*/
			
			void 
		update_max_face_id       
			(
			);	
		
		
			/**
			Make the undi_graph part of the graph with ug and return the face that is the
			right face moving along gdtedge start_edge (i.e. the corresponding gdtedge in *this), 
			starting from gdtnode start_node (i.e. the corresponding gdtnode in *this). <P>
			NOTE:	start_edge and start_node are edges and nodes of ug, respectively; also,
				this method does not modify ug.
			*/
				
			face 
		plan_with_fixed_face_of_undi_graph 
			(
			undi_graph& ug, 
			gdtedge start_edge, 
			gdtnode start_node
			);
		
			/**
			Make the current planar embedding equal to the planar embedding of pug.
			*/
				
			void 
		make_embedding_as 
			(
			const plan_undi_graph& pug
			); 		

			/**
			Split the gdtnode v into two nodes with gdtedge lists. 
			L1=(e_init,e_end) and L2=(cyclic_adj_succ(e_end,v), cyclic_adj_pred(e_init,v)),
			respectively, by inserting a new dummy gdtedge. Also, return the added gdtedge and
			mark it as RM3_ADDED_BY_EXPAND.
			*/
				
			gdtedge 
		expand 
			(
			gdtnode v, 
			gdtedge e_init, 
			gdtedge e_end
			);		

			/**
			Merge the extremal nodes v1, v2 of gdtedge e into a gdtnode v and make
			adj-edges(v) = adj-edges(v1) U adj-edges(v2) \ {e: e=(v1,v2)},
			then return v.
			*/
												
			gdtnode 
		contract 
			(
			gdtedge e
			);					

			/**									
			Contract all the edges marked by the expand method.
			*/
												
			gdt::gdtlist<gdtnode> 
		contract 
			(
			);	
			
			/**
			Make the graph simple, that is add a minimum number of dummy nodes to remove
			multiple edges. Return the list of dummy nodes.
			These nodes are also marked RM3_ADDED_BY_MAKE_SIMPLE	
			*/

			gdt::gdtlist<gdtnode> 
		make_simple 
			(
			);				


			/**
			Make an upward embedding of the graph. This is done by two steps:
			<OL>
			<LI> Assigning an orientation to all the edges of the graph;
			<LI> Establishing, for each source and sink, its incident gdtedge that 
                             will be the left-most gdtedge in the upward embedding. 
			     Note that such gdtedge is automatically determined by the orientation
			     if the gdtnode is neither a source nor a sink.
			</OL>
			The meaning of the parameters is the following:
			<UL>
			<LI> "ext_face" = the external face of the upward embedding
			<LI> "fe" = the gdt::gdtnode_array that informs what is the left-most gdtedge of
			     each source or sink. fe[v]=NULL_EDGE if v is not a source or a sink
			<LI> "minimum_switches" = n enforces the upward orientation to have 
			     the minimum number of sources and sinks minus n (n is in [0,..,INFINITE])  
			</UL>
			<B>PRECONDITIONS:</B> "ext_face" must be a face of the graph, and the 
                                              gdt::gdtnode_array "fe" will be dimensioned on this graph. 
			*/

			void
		make_upward_embedding
			(
			face ext_face,
			gdt::gdtnode_array<gdtedge>& fe,
			int minimum_switches = 0
			);


			/**
			Make the graph as a randomly generated biconnected graph
			with n nodes and manteining it k-planar.
			Also, if multiple = false, no multiple edges are generated.
			The basic technique used to generate graphs is the following:
			<OL>
			<LI> start from a triangle graph;
			<LI> at each step is randomly chosen either the operation 
			    "insert vertex" (split gdtedge) or the operation 
			    "insert gdtedge" (split face);
			<LI> the "insert vertex" operation is selected with probability "prob_iv", 
			    and the "insert gdtedge" operation is selected with probability "1 - prob_iv",
			    where   0 < prob_iv < 1.
			</OL>
			*/
				
			void 
		generate_biconnected 
			(
			int n, 
			double prob_iv, 
			int k=INFINITE,
			bool multiple = true
			);
				
		/*
		CATEGORY io_operations
		Input output operations.
		*/

			/**
			Print the graph on ostream os; print_mode can be BY_NODES, BY_EDGE, BY_FACES, COMPLET.
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
			std::ostream& os=std::cout
			) const;	

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
			Print constraint on ostream os.
			*/
					
			void
		print 
			(
			constraint c,
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

		/*
		CATEGORY consistency_check
		COnsistency check.
		*/

			/**
			Check consistency of the face/border_step internal structure. <P>
		    	NOTE: It assume that the underlaying undi_graph is consistent.
			*/
				
			bool 
		internal_consistency_check
			(
			) const; 			
		    					 			
	};



// -----------------------------------------------------
// Macro for ordered gdtedge, border_step and face scanning
// -----------------------------------------------------


#define forall_GDT_faces(f,G)\
for(f=(G).first_face();f;f=(G).succ_face(f))
//forall (f,(G).all_faces()) // this version works as well, but generates compiler warnings

#define forall_GDT_face_edges(e,f)\
for(e=f->get_owner().first_adj_edge(f);e;e=f->get_owner().adj_succ(e,f))
//forall (e,(f)->all_edges()) // this version works as well, but generates compiler warnings

#define forall_GDT_face_border_steps(s,f)\
for(s=f->get_owner().first_border_step(f);s;s=f->get_owner().succ_border_step(s))
//forall (s,(f)->all_border_steps()) // this version works as well, but generates compiler warnings


#ifdef forall_faces
#undef forall_faces
#endif
#define forall_faces forall_GDT_faces
//
#ifdef forall_face_edges
#undef forall_face_edges
#endif
#define forall_face_edges forall_GDT_face_edges
//
#ifdef forall_face_border_steps
#undef forall_face_border_steps
#endif
#define forall_face_border_steps forall_GDT_face_border_steps

#endif
