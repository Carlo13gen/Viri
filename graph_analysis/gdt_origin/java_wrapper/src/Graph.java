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
Graph
	{
	private int        id;
	private static int lastId = 0;	
	private int lastNodeId = 0;
	private int lastEdgeId = 0;	
	private WrapperImplementation wrapper;
		
		
		public 
	Graph()
		{	
		id      = lastId++;
		wrapper = WrapperImplementation.getInstance();
		wrapper.createGraph(id);
		}
		
				
		public 
		Node 
	newNode()
		{
		//System.out.println("creato nodo: "+lastNodeId);
		return new NodeImplementation(id,lastNodeId++);}
		
		
		public 
		Edge 
	newEdge 
		(Node sourceNode, Node targetNode)
		{return new EdgeImplementation(id,sourceNode.getId(),targetNode.getId(),lastEdgeId++);}
		
		
		public 
		void 
	deleteNode
		(Node n)
		{wrapper.deleteNode(id,n.getId());}
		
		
		public 
		void 
	deleteEdge
		(Edge e)
		{wrapper.deleteEdge(id,e.getId());}
				

		public
		int
	getId()
		{return id;}
	

		protected
		void
	finalize()
		{//wrapper.deleteGraph(id);}
		}
		
		public
		boolean
	isPlanar()
		{
		return wrapper.isPlanar(id);
		}	
	
		
		public
		boolean
	isConnected()
		{
		return wrapper.isConnected(id);
		}


		public
		boolean
	isBiconnected()
		{
		return wrapper.isBiconnected(id);
		}


		public
		void
	zeroBendEdge(Edge e)
		{
		wrapper.zeroBendEdge(id,e.getId());
		return;
		}

		public
		void
	angleSweep(Node n, Edge e, int angle)
		{
		wrapper.angleSweep(id,n.getId(),e.getId(),angle);
		return;
		}

		public
		void
	sameFaceNode(Node n, int face)
		{
		wrapper.sameFaceNode(id,n.getId(),face);
		return;
		}

	// INNER CLASS ------------------------------------------------------------	

		private
		class 
	NodeImplementation 
		extends Node
		{
		private int id;
			
			public 
		NodeImplementation
			(int graphId,int n)
			{
			id = n;
			wrapper.createNode(graphId,id);	
			}
		
		
			public 
			int 
		getId()
			{return id;}
		}
	

	// INNER CLASS ------------------------------------------------------------	

		private
		class 
	EdgeImplementation
		extends Edge
		{
		private int id;
							
			public 
		EdgeImplementation
			(int graphId, int sourceNodeId, int targetNodeId, int e)
			{
			id = e;
			wrapper.createEdge(graphId,sourceNodeId,targetNodeId,id);
			}
		
			public
			int 
		getId()
			{return id;}
		}
							 		
	}
	
