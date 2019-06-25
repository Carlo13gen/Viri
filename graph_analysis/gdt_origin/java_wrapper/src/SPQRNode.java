package GdtJavaWrapper;

import java.util.*;
import java.io.*;


public  class 
	SPQRNode 
	{
		private int id;
		private int treeId;
		private int father;  //id of the father node, if father==-1 then this is the root node
		private List<Integer> sons;

		private WrapperImplementation wrapper;
		


		public 
		SPQRNode (int graphId, int nodeId, int fatherId)
			{
			wrapper = WrapperImplementation.getInstance();			
			treeId=graphId;
			id=nodeId;	
			father=fatherId;
			sons=new ArrayList<Integer>();
			}
		
		
		public // create the root node
		SPQRNode (int graphId, int nodeId)
			{
			wrapper = WrapperImplementation.getInstance();			
			treeId=graphId;
			id=nodeId;	
			father=-1;
			sons=new ArrayList<Integer>();
			}

		public  int getId(){return id;};

		public  char 
		getNodeType()
			{
			return wrapper.nodeType(treeId, id);
			}

		
		public int 
		getFatherId()
			{
			return father;
			}

		

		public List<Integer>
		getSons()
			{
			return sons;
			}

		
		public void
		addSon(int sonId)
			{
			Integer I=new Integer(sonId);
			sons.add(I);
			}

	}










