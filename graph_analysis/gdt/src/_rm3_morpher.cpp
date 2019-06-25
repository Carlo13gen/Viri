/*******************************************************************************
+
+  _rm3_morpher.cpp
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

#include <assert.h>
#include <GDT/rm3_morpher.h>

using namespace std;

// ------------------------------------------
// PRIVATE
// ------------------------------------------

	bool
	morpher::
check_compatibility
	(
	)
	{
	// this method to be done
	//
	return true;
	}
		
// ------------------------------------------
// PUBLIC
// ------------------------------------------


	morpher::
morpher
	(
	const draw_undi_graph& start, 
	const draw_undi_graph& end
	) : start_graph(start), end_graph(end)
	{
	assert( check_compatibility() );
	intermediate_graph = new draw_undi_graph(start);
	}


	morpher::
~morpher
	()
	{
	delete intermediate_graph;
	}
	

	draw_undi_graph*
	morpher::
update_intermediate
	(
	float alpha
	)
	{
	// update the coordinates and size of each vertex
	//
	gdtnode n;
	forall_nodes( n, start_graph.nodes_and_edges() )
		{
		gdtnode n2 	 = end_graph.corresponding_node		  (n,start_graph);
		gdtnode target_node = intermediate_graph->corresponding_node (n,start_graph);
		gdt::gdtpoint p1 = start_graph.center(n);
		gdt::gdtpoint p2 = end_graph.center(n2);
		gdt::gdtpoint p3(p1.xcoord()*(1.0 - alpha) + p2.xcoord()*alpha,
                         p1.ycoord()*(1.0 - alpha) + p2.ycoord()*alpha);
		
		intermediate_graph->set_center(target_node,p3);
		intermediate_graph->set_width(target_node,
					      start_graph.width(n)*(1.0-alpha)+
					      end_graph.width(n2)*alpha);
		intermediate_graph->set_height(target_node,
					      start_graph.height(n)*(1.0-alpha)+
					      end_graph.height(n2)*alpha);
		}
	
	// update the coordinates of each bend
	//
	gdtedge e1;
	forall_edges( e1, start_graph.nodes_and_edges() )
		{
		gdtnode n1 = start_graph.source(e1);
		gdtnode n2 = end_graph.corresponding_node(n1,start_graph);
		gdtnode n3 = intermediate_graph->corresponding_node(n1,start_graph);
		gdtedge e2 = end_graph.corresponding_edge(e1,start_graph);
		gdtedge e3 = intermediate_graph->corresponding_edge(e1,start_graph);
		gdt::gdtlist<gdt::gdtpoint> 	Lb1 = start_graph.bends(e1,n1);
		gdt::gdtlist<gdt::gdtpoint> 	Lb2 = end_graph.bends(e2,n2);
		gdt::gdtlist<gdt::gdtpoint> 	Lb3;
		gdt::list_item 	i1,i2;
		gdt::gdtpoint 		p1,p2;
		for( 	
			i1 = Lb1.first(), i2 = Lb2.first() ; 
			(i1 != nil) && (i2 != nil) ;
			i1 = Lb1.succ(i1), i2 = Lb2.succ(i2) 
		   )
			{
			p1 = Lb1.inf(i1);
			p2 = Lb2.inf(i2);
			gdt::gdtpoint p3(p1.xcoord()*(1.0 - alpha) + p2.xcoord()*alpha,
				 p1.ycoord()*(1.0 - alpha) + p2.ycoord()*alpha);
			Lb3.push_back(p3);
			}
		intermediate_graph->set_bends(e3,n3,Lb3);

		// update the coordinates of the anchor points of each gdtedge
		// NOTICE: we have to handle the case in which the target and the source
		//         are exchanged during the drawing process (this should not be!)
		//
		bool source_and_target_exchanged = false;
		if( end_graph.source(e2) != end_graph.corresponding_node(start_graph.source(e1),start_graph) )
			source_and_target_exchanged = true;

		p1 = start_graph.anchor_source(e1);
		if( source_and_target_exchanged )
			p2 = end_graph.anchor_target(e2);
		else
			p2 = end_graph.anchor_source(e2);
		gdt::gdtpoint p3(p1.xcoord()*(1.0 - alpha) + p2.xcoord()*alpha,
			 p1.ycoord()*(1.0 - alpha) + p2.ycoord()*alpha);
		intermediate_graph->set_anchor_source(e3,p3);

		p1 = start_graph.anchor_target(e1);
		if( source_and_target_exchanged )
			p2 = end_graph.anchor_source(e2);
		else
			p2 = end_graph.anchor_target(e2);
		gdt::gdtpoint p4(p1.xcoord()*(1.0 - alpha) + p2.xcoord()*alpha,
			 p1.ycoord()*(1.0 - alpha) + p2.ycoord()*alpha);
		intermediate_graph->set_anchor_target(e3,p4);

		}
	
	return intermediate_graph;
	}
		

