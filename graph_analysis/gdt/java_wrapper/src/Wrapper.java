//-----------------------------
//
//   GdtJavaWrapper / Java
//   (c)2005 by INTEGRA Sistemi                               
//   Luigi Del Negro          
//                               
//-----------------------------

package GdtJavaWrapper;

import java.util.List; 
 	
 	public 
 	class
Wrapper
 	{public static String getVersion() {return "1.0";}}
 	
 
	class 
WrapperImplementation
	extends Wrapper
	{
		private static WrapperImplementation instance;
		
		public static 
		WrapperImplementation
	getInstance()
		{
		if(instance==null) instance = new WrapperImplementation();
		return instance;	
		}
		
		
	// JNI METHODS ------------------------------------------------------------
	
	public native void createGraph (int graphId);
	public native void createNode  (int graphId, int nodeId);
	public native void createEdge  (int graphId, int sourceNodeId, int targetNodeId, int edgeId);
	public native void createOrthogonalGraph (int graphId);		
	public native void createConstrainedOrthogonalGraph (int graphId, int[] external_nodes, int dimension);		
	

	public native void deleteGraph      (int graphId);
	public native void deleteDrawnGraph (int graphId);
	public native void deleteOrthogonalGraph (int graphId);
	public native void deleteNode       (int graphId, int nodeId);
	public native void deleteEdge       (int graphId, int edgeId);
	public native boolean isPlanar		(int graphId);
	public native boolean isConnected	(int graphId);
	public native boolean isBiconnected	(int graphId);

	public native void drawOrthogonalGraph (int graphId); 
	public native void drawOrthogonalGraphUsingOrthogonal (int graphId); 

	/////////////
	//Constraint
	public native void zeroBendEdge	(int graphId, int edgeId); 
	public native void angleSweep	(int graphId, int nodeid, int edgeId, int angle); 
	public native void nodeWidth	(int graphId, int nodeid, double w); 
	public native void nodeHeight	(int graphId, int nodeid, double h);
	public native void sameFaceNode	(int graphId, int nodeid, int faceId);
	///////////
	
	public native double getNodeWidth         (int graphId, int nodeId);
	public native double getNodeHeight        (int graphId, int nodeId);
	public native double getNodeX             (int graphId, int nodeId);
	public native double getNodeY             (int graphId, int nodeId);
	public native int    getEdgeBendsCount    (int graphId, int edgeId);
	public native double getEdgeBendPointX    (int graphId, int edgeId, int bendIndex);
	public native double getEdgeBendPointY    (int graphId, int edgeId, int bendIndex);
	public native double getEdgeAnchorSourceX (int graphId, int edgeId);
	public native double getEdgeAnchorSourceY (int graphId, int edgeId);
	public native double getEdgeAnchorTargetX (int graphId, int edgeId);
	public native double getEdgeAnchorTargetY (int graphId, int edgeId);
	public native double getEdgeLength 	  (int graphId, int edgeId);

	public native void rotate		  (int graphId, int angle);
	public native void flipHorizontally	  (int graphId);
	public native void flipVertically	  (int graphId);

	//SPQR Tree
	public native void createSPQRTree (int graphId);
	public native void deleteSPQRTree (int graphId);

	public native int getRoot(int graphId);
	public native char nodeType(int graphId, int nodeId);
	public native int numberOfSons(int graphId, int nodeId);
	public native int firstSon(int graphId, int nodeId);
	public native int succSon(int graphId, int nodeId, int sonId);
	public native int numberOfNodes(int graphId);
	public native int getQNode(int graphId, int edgeId);
	public native int getLevel(int graphId, int nodeId);
	public native int depthOfSubtree(int graphId, int nodeId);
	}
