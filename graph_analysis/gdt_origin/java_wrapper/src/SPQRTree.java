//-----------------------------
//
//   GdtJavaWrapper / Java     //                               
//-----------------------------

package GdtJavaWrapper;

import java.util.*;
import java.io.*;

	public
	class 
SPQRTree 
	{
	private int        id;
	private HashMap <Integer, SPQRNode> nodes;
	private WrapperImplementation wrapper;
	

	/* Codice esempio
	for(String current : this.nodes.keySet()){
			Set<String> current2 = this.node.get(current);
			for(String current3 : current2)
	}*/	
		
		public 
	SPQRTree(Graph g)
		{	
		id      = g.getId();
		wrapper = WrapperImplementation.getInstance();
		wrapper.createSPQRTree(id);
		nodes=new HashMap<Integer,SPQRNode>();
		//int nn=numberOfNodes();
		int rootID=getRootId();
  		SPQRNode Root=new SPQRNode(id,rootID);  //root node
		nodes.put(rootID,Root);
		ArrayList<SPQRNode> buffer=new ArrayList<SPQRNode>();
		buffer.add(Root);
	
		while (buffer.size() >0)
			{
			SPQRNode N=buffer.get(0);
			buffer.remove(0);
			int temp_id=N.getId();
			
			int sons=getNumberOfSons(temp_id);
		
			if (sons>0)
				{
				int sonID=wrapper.firstSon(id,temp_id);
				//System.out.println("Il primo figlio di "+temp_id+" e' "+sonID);
				SPQRNode son=new SPQRNode(id,sonID,temp_id);
				nodes.put(sonID,son);
				N.addSon(sonID);				
				buffer.add(son);  
				
				//System.out.println("Aggiunto al buffer il nodo "+sonID);
				for (int i=1; i<sons; i++)
					{
					sonID=wrapper.succSon(id,temp_id,sonID);
					son=new SPQRNode(id,sonID,temp_id);
					nodes.put(sonID,son);
					N.addSon(sonID);					
					buffer.add(son);
					//System.out.println("Aggiunto al buffer il nodo "+sonID); 
					}
				}
			}
		}
		

		
	

		protected
		void
	finalize()
		{
		}



		private
		int
	getRootId()
		{
		int rootId=wrapper.getRoot(id);
		return rootId;
		}


		private
		int
	getNumberOfSons(int nodeId)
		{
		return wrapper.numberOfSons(id, nodeId);
		}


/*************************************************************
	Public Methods
**************************************************************/		
		
		public
		int
	getId()
		{return id;}

		public
		int
	numberOfNodes()
		{
		return wrapper.numberOfNodes(id);
		}

		public
		HashMap<Integer,SPQRNode>
	getNodes()
		{
		return nodes;
		}
		
		public
		SPQRNode
	getQNode(int edgeID)
		{
		Integer nodeIndex=new Integer(wrapper.getQNode(id, edgeID));
		
		return nodes.get(nodeIndex);
		
		}

		public
		int
	getLevel(SPQRNode n)
		{
		return wrapper.getLevel(id,n.getId());
		}

		public
		int
	depthOfSubtree(SPQRNode n)
		{
		return wrapper.depthOfSubtree(id, n.getId());
		}
							 		
	}// End of Class SPQR Tree
	
