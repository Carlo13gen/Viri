/*******************************************************************************
+
+  rm3_morpher.h
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


#ifndef RM3_ANIMATE_H
#define RM3_ANIMATE_H


#include <fstream>
#include <GDT/rm3_draw_undi_graph.h>

/**
Global class morpher.
This class allows to trasform an orthogonal drawing of a graph into another drawing of the same graph.
*/
	
	class GDT_NONSTANDARD_DECL
morpher
	{
	private:
		// -----------------
		// private variables
		// -----------------

	  	const draw_undi_graph& start_graph;
		const draw_undi_graph& end_graph;
		draw_undi_graph* intermediate_graph;
		
		// ---------------
		// private methods
		// ---------------

			/*
			Check if the start_graph and the end_graph are compatible with a morphing
			operation between the two. 
			*/

			bool
		check_compatibility
			(
			);
		

	public:

		/*
		CATEGORY constructors_destructors
		Constructors and destructors.
		*/

			/**
			Constructor. <P>
			PRECONDITIONS: "start_graph" and "end_graph" must have corresponding
			nodes and edges (with the same gdtnode and gdtedge id).
			*/
		
		morpher
			(
			const draw_undi_graph& start_graph, 
			const draw_undi_graph& end_graph
			);						

			
			/**
			Destructor.
			*/
		
		~morpher
			(
			);					
		
		

		/*
		CATEGORY update & access
		Access operations for informations about defaults.
		*/
		
			
			/**
			update the intermediate_graph with coordinates
			between those of start_graph and those of end_graph.
			If "alpha" is 0, then intermediate_graph has the same
                        coordinates than start_graph, if "alpha" is 1, then
                        intermediate_graph has the same coordinates than 
			end_graph.
			*/

			draw_undi_graph*
		update_intermediate
			(
			float alpha
			); 	
		
	};
	
	
#endif
