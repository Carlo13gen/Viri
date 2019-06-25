//-----------------------------
//
//   GdtJavaWrapper / Cpp
//                               
//-----------------------------


#include <GDT/rm3_draw_undi_graph.h>


using namespace std;
using namespace gdt;

	class 
Wrapper
	{
	public:
		
		~Wrapper();

		static void 	createGraph   			(int graphId);
		static void 	createNode    			(int graphId, int nodeId);
		static void 	createEdge    			(int graphId, int sourceNodeId, int targetNodeId, int edgeId);
		static void 	createSPQRTree			(int graphId);

		static void 	deleteGraph   			(int graphId);
		static void 	deleteSPQRTree 			(int graphId);
		static void     deleteDrawnGraph        	(int graphId);  
		static void     deleteOrthogonalGraph        	(int graphId);  
		static void 	deleteNode			(int graphId, int nodeId);
		static void 	deleteEdge			(int graphId, int edgeId);

		//constraint	
		static void 	zero_bend_edge			(int graphId, int edgeId);
		static void	angleSweep			(int graphId, int nodeId, int edgeId, int angle);
		static void 	nodeWidth			(int graphId, int nodeId, double w);
		static void 	nodeHeight			(int graphId, int nodeId, double h);
		static void 	sameFaceNode			(int graphId, int nodeId, int faceId);
		//////////////////////////////////
		static void 	drawOrthogonalGraph		(int graphId);
		static void 	drawOrthogonalGraphUsingOrthogonal	(int graphId);
		
		static void 	createOrthogonalGraph		(int graphId);
		static void 	createOrthogonalGraph		(int graphId, gdtlist<int> external_nodes);		

		static void 	drawOrthogonalBoxedGraph	(int graphId);

		static jdouble 	getNodeWidth			(int graphId, int nodeId);
		static jdouble 	getNodeHeight			(int graphId, int nodeId);
		static jdouble 	getNodeX				(int graphId, int nodeId);
		static jdouble 	getNodeY				(int graphId, int nodeId);
		static jint    	getEdgeBendsCount	    (int graphId, int edgeId);
		static jdouble 	getEdgeBendPointX		(int graphId, int edgeId, int bendIndex);
		static jdouble 	getEdgeBendPointY		(int graphId, int edgeId, int bendIndex);
		static jdouble	getEdgeAnchorSourceX    (int graphId, int edgeId);
		static jdouble	getEdgeAnchorSourceY    (int graphId, int edgeId);
		static jdouble	getEdgeAnchorTargetX    (int graphId, int edgeId);
		static jdouble  getEdgeAnchorTargetY    (int graphId, int edgeId);
		static jdouble	getEdgeLength		(int graphId, int edgeId);		

		static jboolean		isPlanar			(int graphId);
		static jboolean		isConnected			(int graphId);
		static jboolean		isBiconnected			(int graphId);

		static void	rotate			(int graphId, int angle);
		static void	flipHorizontally	(int graphId);
		static void	flipVertically		(int graphId);

		/////////////////////////////// SPQR
		static jint getRoot(int graphId);
		static jchar nodeType(int graphId, int nodeId);
		static int numberOfSons(int graphId, int nodeId);
		static int firstSon(int graphId, int nodeId);
		static int succ_son(int graphId, int nodeId, int sonId);
		static int spqr_number_of_nodes(int graphId);
		static int spqr_get_Q_node(int graphId, int edgeId);
		static int spqr_get_level(int graphId, int nodeId);
		static int spqr_depth_of_subtree(int graphId, int nodeId);

	private:
		// draw_undi_graph ids are the same of corresponding undi_graph 
		static gdtmap<int,undi_graph*>      graphMap;
		static gdtmap<int,draw_undi_graph*> drawnGraphMap;
		static gdtmap<int,orth_plan_undi_graph*>  orthogonalMap;
		static gdtmap<int,SPQR_tree*> SPQRMap;
		

		static gdtpoint getEdgeBendPoint    (int graphId, int edgeId, int bendIndex);
		static gdtpoint getEdgeAnchorSource (int graphId, int edgeId);
		static gdtpoint getEdgeAnchorTarget (int graphId, int edgeId);
		static gdtpoint getNodeCenter	    (int graphId, int nodeId);
	};


	










