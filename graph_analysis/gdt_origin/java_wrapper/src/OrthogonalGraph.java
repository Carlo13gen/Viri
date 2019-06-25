//-----------------------------
//
//   GdtJavaWrapper / Java
//   (c)2005 by INTEGRA Sistemi                               
//   Luigi Del Negro          
//                               
//-----------------------------

package GdtJavaWrapper;

import java.util.*;
import java.io.*;

	public
	class 
OrthogonalGraph 
	{
	private int        id;
	
	private WrapperImplementation wrapper;
		
		
		public 
	OrthogonalGraph(Graph g)
		{	
		id      = g.getId();
		wrapper = WrapperImplementation.getInstance();
		wrapper.createOrthogonalGraph(id);
		}
		
		public 
	OrthogonalGraph(Graph g, Node[] external_nodes)
		{	
		id      = g.getId();
		wrapper = WrapperImplementation.getInstance();
		int l=external_nodes.length;
		int[] ext_index=new int[l];
		int i;
		for (i=0; i<l; i++)
			ext_index[i]=external_nodes[i].getId();
		wrapper.createConstrainedOrthogonalGraph(id,ext_index,l);
		//wrapper.createConstrainedOrthogonalGraph(id,external_nodes,external_nodes.length);
		}
				
		public
		void
	setNodeWidth(Node n, double w)
		{
		wrapper.nodeWidth(id, n.getId(), w);
		}		

		public
		void
	setNodeHeight(Node n, double h)
		{
		wrapper.nodeHeight(id, n.getId(), h);
		}

		protected
		int
	getId()
		{return id;}
	

		protected
		void
	finalize()
		{//wrapper.deleteOrthogonalGraph(id);}
		}


							 		
	}
	
