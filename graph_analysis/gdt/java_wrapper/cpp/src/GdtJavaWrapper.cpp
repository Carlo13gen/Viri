//-----------------------------
//
//   GdtJavaWrapper / Cpp
//   (c)2005 by INTEGRA Sistemi   Luigi Del Negro          
//   2007/2008 Pier Francesco Cortese
//-----------------------------

#pragma warning (disable:4786)
 
#include <jni.h>   
#include <iostream>
#include <map>

#include "GDT/rm3_draw_undi_graph.h"

#include "GdtJavaWrapper_WrapperImplementation.h"
#include "GdtJavaWrapper.h"

using namespace std;
using namespace gdt;


gdtmap<int,undi_graph*> Wrapper::graphMap;
gdtmap<int,draw_undi_graph*> Wrapper::drawnGraphMap;
gdtmap<int,orth_plan_undi_graph*> Wrapper::orthogonalMap;
gdtmap<int,SPQR_tree*> Wrapper::SPQRMap;


	Wrapper::
~Wrapper()
	{
	//cout << "Wrapper destructor" << endl;	
	void* i=graphMap.first_item();
	while (i)
		{
		if (graphMap.inf(i)) delete graphMap.inf(i);
		i=graphMap.next_item(i);
		}
	
	i=drawnGraphMap.first_item();
	while (i)
		{
		if (drawnGraphMap.inf(i)) delete drawnGraphMap.inf(i);
		i=drawnGraphMap.next_item(i);
		}	
	

	i=orthogonalMap.first_item();			
	while (i)
		{
		if (orthogonalMap.inf(i)) delete orthogonalMap.inf(i);
		i=orthogonalMap.next_item(i);
		}
	i=orthogonalMap.first_item();			
	while (i)
		{
		if (orthogonalMap.inf(i)) delete orthogonalMap.inf(i);
		i=orthogonalMap.next_item(i);
		}
	i=SPQRMap.first_item();			
	while (i)
		{
		if (SPQRMap.inf(i)) delete SPQRMap.inf(i);
		i=SPQRMap.next_item(i);
		}

	}



	void 
	Wrapper::
createGraph
    (int graphId) 
	{
	if (graphMap[graphId]) {delete graphMap[graphId];  };
	graphMap[graphId] = new undi_graph;
	}


	void 
	Wrapper::
createNode
	(int graphId, int nodeId) 
	{graphMap[graphId]->new_node(nodeId);}


	void 
	Wrapper::
createEdge 
	(int graphId, int sourceNodeId, int targetNodeId, int edgeId)
	{	
	undi_graph* ug = graphMap[graphId]; 		
	gdtnode s = ug->find_node(sourceNodeId);
	gdtnode t = ug->find_node(targetNodeId);
	ug->new_edge(s,t,edgeId);
	}

	void 
	Wrapper::
createSPQRTree
    (int graphId) 
	{
	if (!graphMap[graphId]) 
		gdt_error("The graph does not exists");
	undi_graph* ug= graphMap[graphId];
	if (!(ug->is_planar()))
		gdt_error("The graph is not planar");
	if (!(ug->is_biconnected()))
		gdt_error("The graph is not biconnected");
	if (SPQRMap[graphId])
		delete (SPQRMap[graphId]);
	plan_undi_graph* pug=new plan_undi_graph(*ug);
	SPQRMap[graphId]=new SPQR_tree(*pug);

	/////////////////
	//debug - To be removed
	draw_undi_graph* temp=new draw_undi_graph(*SPQRMap[graphId]);
	temp->write("tree.gdt");
	delete temp;	
	/////////////////
	delete pug;
	
	
	}

	void
	Wrapper::
deleteNode
	(int graphId, int nodeId)
	{
	undi_graph* ug = graphMap[graphId]; 		
	gdtnode n = ug->find_node(nodeId);
	ug->del_node(n);
	}


	void
	Wrapper::
deleteEdge
	(int graphId, int edgeId)
	{
	undi_graph* ug = graphMap[graphId]; 		
	gdtedge e = ug->find_edge(edgeId);
	ug->del_edge(e);
	}


	void
	Wrapper::
deleteGraph
	(int graphId)
	{
	undi_graph* ug = graphMap[graphId];
	if (ug) delete ug;	
	//graphMap.undefine(graphId);
	graphMap[graphId]=NULL;
	}


	void
	Wrapper::
deleteSPQRTree
	(int graphId)
	{
	SPQR_tree* T = SPQRMap[graphId];
	if (T) delete T;	
	SPQRMap[graphId]=NULL;
	}

	void
	Wrapper::
deleteDrawnGraph
	(int graphId)
	{
	draw_undi_graph* dug = drawnGraphMap[graphId];
	//drawnGraphMap.undefine(graphId);
	if (dug) delete dug;
	//cout << "fine cancellazione" << endl;
	}


	void
	Wrapper::
drawOrthogonalGraph
	(int graphId)
	{
	undi_graph* ug = graphMap[graphId];
	plan_undi_graph* pug=new plan_undi_graph(*ug);
	orth_plan_undi_graph opug(*pug);
        if (drawnGraphMap[graphId]) 
		{
		delete drawnGraphMap[graphId];
		drawnGraphMap[graphId]=NULL; 
		}
	drawnGraphMap[graphId] = new draw_undi_graph(opug);
	delete pug;   
	}



	gdtpoint
	Wrapper::
getNodeCenter
	(int graphId, int nodeId)
	{
	draw_undi_graph* dug = drawnGraphMap[graphId]; 	
	if (!dug) {
		cout << "graphid=" << graphId << endl;  
		 gdt_error("NULL graph in getNodeCenter");
		}	
	gdtnode  n = dug->find_node(nodeId);
	return dug->center(n);
	}

	
	gdtpoint
	Wrapper::
getEdgeBendPoint
	(int graphId, int edgeId, int bendIndex)
	{
	draw_undi_graph* dug = drawnGraphMap[graphId]; 		
	if (!dug) {
		cout << "graphid=" << graphId << endl;  
		 gdt_error("NULL graph in getEdgeBendPoint");
		}
	gdtedge e = dug->find_edge(edgeId);
	gdtlist<gdtpoint> bendList = dug->bends(e,dug->source(e));
	return bendList.inf(bendList.get_item(bendIndex));
	}


	jdouble
	Wrapper::
getNodeX
	(int graphId, int nodeId)
	{ //cout << "getNodeX" << endl;
	return getNodeCenter(graphId,nodeId).xcoord() - getNodeWidth(graphId, nodeId)/2.0;	}


	jdouble
	Wrapper::
getNodeY
	(int graphId, int nodeId)
	{//cout << "getNodey" << endl;
	return getNodeCenter(graphId,nodeId).ycoord() + getNodeHeight(graphId, nodeId)/2.0;	}


	jdouble
	Wrapper::
getNodeWidth
	(int graphId, int nodeId)
	{
	//cout << "getNodeWidth" << endl;
	draw_undi_graph* dug = drawnGraphMap[graphId];
	if (!dug) gdt_error("NULL graph in getNodeWidth");
	gdtnode n = dug->find_node(nodeId);
	//if (!n) gdt_error("NULL node in getNodeWidth");
	return dug->width(n);
	}

	
	jdouble
	Wrapper::
getNodeHeight
	(int graphId, int nodeId)
	{
	//cout << "getNodeHeight" << endl;
	draw_undi_graph* dug = drawnGraphMap[graphId];
	if (!dug) gdt_error("NULL graph in getNodeHeight");
	gdtnode n = dug->find_node(nodeId);
	//if (!n) gdt_error("NULL node in getNodeHeight");
	return dug->height(n);
	}

	
	jint
	Wrapper::
getEdgeBendsCount
	(int graphId, int edgeId)
	{	
	draw_undi_graph* dug = drawnGraphMap[graphId];
	gdtedge e = dug->find_edge(edgeId);
	return (dug->bends(e)).size();
	}


	jdouble
	Wrapper::
getEdgeBendPointX
	(int graphId, int edgeId, int bendIndex)
	{return getEdgeBendPoint(graphId,edgeId,bendIndex).xcoord();}


	jdouble
	Wrapper::
getEdgeBendPointY
	(int graphId, int edgeId, int bendIndex)
	{return getEdgeBendPoint(graphId,edgeId,bendIndex).ycoord();}

	
	gdtpoint
	Wrapper::
getEdgeAnchorSource
	(int graphId, int edgeId)
	{
	draw_undi_graph* dug = drawnGraphMap[graphId];
	gdtedge  e      = dug->find_edge(edgeId);  
	return dug->intersection_edge_node(e,dug->source(e));
	}

	gdtpoint
	Wrapper::
getEdgeAnchorTarget
	(int graphId, int edgeId)
	{
	draw_undi_graph* dug = drawnGraphMap[graphId];
	gdtedge  e      = dug->find_edge(edgeId);  
	return dug->intersection_edge_node(e,dug->target(e));
	}


	jdouble
	Wrapper::
getEdgeAnchorSourceX
	(int graphId, int edgeId)
	{return getEdgeAnchorSource(graphId,edgeId).xcoord();}

	
	jdouble
	Wrapper::
getEdgeAnchorSourceY
	(int graphId, int edgeId)
	{return getEdgeAnchorSource(graphId,edgeId).ycoord();}


	jdouble
	Wrapper::
getEdgeAnchorTargetX
	(int graphId, int edgeId)
	{return getEdgeAnchorTarget(graphId,edgeId).xcoord();}


    jdouble
	Wrapper::
getEdgeAnchorTargetY
	(int graphId, int edgeId)
	{return getEdgeAnchorTarget(graphId,edgeId).ycoord();}
	
	
	
////////////////////////////////////////////////////
//////  New methods  - Added by Pier Francesco Cortese
////////////////////////////////////////////////////


	jdouble
	Wrapper::
getEdgeLength
	(int graphId, int edgeId)
	{ 
	draw_undi_graph* dug=drawnGraphMap[graphId];
	if (!dug) gdt_error("NULL graph in getEdgeLength");
	gdtedge e=dug->find_edge(edgeId);
	return dug->length(e);
	}



	// planarity test	
	jboolean
		Wrapper::isPlanar
		(int graphId)
	{
		undi_graph* ug = graphMap[graphId];
		return ug->is_planar();
	}


	// Connectivity test	
	jboolean
		Wrapper::isConnected
		(int graphId)
	{
		undi_graph* ug = graphMap[graphId];
		return ug->is_connected();
	}


	// Biconnectivity test	
	jboolean
		Wrapper::isBiconnected
		(int graphId)
	{
		undi_graph* ug = graphMap[graphId];
		return ug->is_biconnected();
	}


	//Constraint
	void
	Wrapper::zero_bend_edge
	(int graphId, int edgeId)
	{
		undi_graph* ug=graphMap[graphId];
		gdtedge e=ug->find_edge(edgeId);
		ug->new_constraint_number_of_bends_on_edge(e,NONE);
	}
	
	void
	Wrapper::nodeWidth
	(int graphId, int nodeId, double w)
	{
		orth_plan_undi_graph* opug=orthogonalMap[graphId];
		if (!opug) gdt_error("NULL graph");	
		gdtnode n=opug->find_node(nodeId);	
		if (!n) gdt_error("NULL node while setting node width");
		opug->new_constraint_node_width(n,w);
	}

	void
	Wrapper::nodeHeight
	(int graphId, int nodeId, double h)
	{
		orth_plan_undi_graph* opug=orthogonalMap[graphId];
		if (!opug) gdt_error("NULL graph");
		gdtnode n=opug->find_node(nodeId);
		if (!n) gdt_error("NULL node while setting node height");
		opug->new_constraint_node_height(n,h);
	}

	void
	Wrapper::angleSweep
	(int graphId, int nodeId, int edgeId, int angle)
	{
		undi_graph* ug=graphMap[graphId];
		gdtnode n=ug->find_node(nodeId);		
		gdtedge e=ug->find_edge(edgeId);
		switch (angle)
		{
			case 0: ug->new_constraint_angle_sweep(n,e,_000);
				break;
			case 90: ug->new_constraint_angle_sweep(n,e,_090);
				break;
			case 180: ug->new_constraint_angle_sweep(n,e,_180);
				break;
			case 270: ug->new_constraint_angle_sweep(n,e,_270);
				break;
			default: gdt_error("uncorrect angle type");
		}
	}

	void
	Wrapper::sameFaceNode(int graphId, int nodeId, int faceId)
	{
		undi_graph* ug=graphMap[graphId];
		if (!ug) gdt_error("NULL graph in sameFaceNode");
		gdtnode n=ug->find_node(nodeId);
		if (!n) gdt_error("NULL node in sameFaceNode");
		ug->new_constraint_same_face_node(n,faceId);
	}

	
	
	void
	Wrapper::
drawOrthogonalGraphUsingOrthogonal

	(int graphId)
	{
	draw_undi_graph* dug = drawnGraphMap[graphId];	
	if (dug) delete dug;		
	orth_plan_undi_graph* opug=orthogonalMap[graphId];
	dug=new draw_undi_graph(*opug);	
	drawnGraphMap[graphId]=dug;
	//debug
	dug->write("output.gdt");
	////////
	}


	void
	Wrapper::
createOrthogonalGraph
	(int graphId)
	{
	undi_graph* ug=graphMap[graphId];
	if (!ug) gdt_error("Null graph");
	//gdt::gdtlist<gdtedge> added_edges=ug->make_biconnected();
	//gdtedge e;
	//forall(e,added_edges)
	//	ug->new_constraint_number_of_bends_on_edge(e,ANY);
	plan_undi_graph pug(*ug);
	orth_plan_undi_graph* opug=new orth_plan_undi_graph(pug);
	
	//forall(e,added_edges)
	//	{
		//cout << "added edge " << ug->id(e) << endl;
	//	opug->del_edge(opug->find_edge(ug->id(e)));
	//	}
	if (orthogonalMap[graphId]) {delete orthogonalMap[graphId];  }

	orthogonalMap[graphId]=opug;	
	}


	void
	Wrapper::
createOrthogonalGraph
	(int graphId, gdtlist<int> external_nodes)
	{
	undi_graph* ug=graphMap[graphId];
	if (!ug) gdt_error("Null graph");
	//gdt::gdtlist<gdtedge> added_edges=ug->make_biconnected();
	//gdtedge e;
	//forall(e,added_edges)
	//	ug->new_constraint_number_of_bends_on_edge(e,ANY);

	int n;
	forall(n,external_nodes)
		{
		ug->new_constraint_same_face_node(ug->find_node(n),0);
		}
	plan_undi_graph pug(*ug);
	
	face f=NULL_FACE;
	face f_ext=NULL_FACE;
	forall_faces(f,pug)
		{
		//////  DEBUG		
		//cout << "------------------------------------------------" << endl;		
		//cout << "Esamino faccia: " << pug.id(f) << endl;
		//cout << "Numero nodi sulla faccia=" << pug.adj_nodes(f).length() << endl;
		//gdtnode temp_node;
		//gdtlist<gdtnode> nodes_in_f=pug.adj_nodes(f);
		//forall( temp_node, nodes_in_f)
		//	 {cout << "nodo " << pug.id(temp_node) << endl;} 
		//cout << "------------------------------------------------" << endl;	
		//////////////////////////////
		bool found=true;
		int j;
		forall(j,external_nodes)
			if (!(pug.node_belongs_to_face(pug.find_node(j),f)  )) found=false;
			
		if (found && (external_nodes.length()==(pug.adj_nodes(f)).length() ) ) 
			{
			f_ext=f; 
			//cout << "trovata faccia esterna" << endl;			
			break;
			}
		}
	
	if (f_ext==NULL_FACE) gdt_error("External face not found");
	orth_plan_undi_graph* opug=new orth_plan_undi_graph(pug,f_ext,PLAN_ORTH_OPTIMAL);
	
	//forall(e,added_edges)
	//	{
	//	opug->del_edge(opug->find_edge(ug->id(e)));
	//	}
	orthogonalMap[graphId]=opug;	
	}

	void
	Wrapper::
deleteOrthogonalGraph
	(int graphId)
	{
	orth_plan_undi_graph* opug = orthogonalMap[graphId];
	if (opug) delete opug;	
	//orthogonalMap.undefine(graphId);
	orthogonalMap[graphId]=NULL;
	}



	void
	Wrapper::
rotate(int graphId, int angle)
	{
	draw_undi_graph *dug=drawnGraphMap[graphId];
	if (!dug) gdt_error("NULL graph in function rotate");
	switch (angle)
		{
		case 0:  dug->rotate(_000, dug->center()); break;
		case 90:  dug->rotate(_090, dug->center()); break;
		case 180:  dug->rotate(_180, dug->center()); break;
		case 270:  dug->rotate(_270, dug->center()); break;
		default: gdt_error("Wrong angle. The only admissible rotations are 90, 180, and 270 degree.");
		}
	//dug->write("rotated_graph.gdt");
	}



	void
	Wrapper::
flipHorizontally(int graphId)
	{
	draw_undi_graph *dug=drawnGraphMap[graphId];
	if (!dug) gdt_error("NULL graph in function flipHorizontaly");
	dug->flip_horizontally(dug->center());
	//dug->write("flippedH_graph.gdt");
	}


	void
	Wrapper::
flipVertically(int graphId)
	{
	draw_undi_graph *dug=drawnGraphMap[graphId];
	if (!dug) gdt_error("NULL graph in function flipVertically");
	dug->flip_vertically(dug->center());
	//dug->write("flippedV_graph.gdt");
	}



	jint
	Wrapper::
getRoot(int graphId)
	{
	SPQR_tree* T=SPQRMap[graphId];
	if (!T) gdt_error("NULL SPQR Tree");
	return T->id(T->root_of_tree());
	}


	jchar
	Wrapper::
nodeType(int graphId, int nodeId)
	{
	SPQR_tree* T=SPQRMap[graphId];
	if (!T) gdt_error("NULL SPQR Tree");
	gdtnode n=T->find_node(nodeId);
	if (!n) gdt_error("NULL node");
	if (T->get_type_of_node(n)==Q_NODE)
		return 'Q';
	if (T->get_type_of_node(n)==S_NODE)
		return 'S';
	if (T->get_type_of_node(n)==R_NODE)
		return 'R';
	if (T->get_type_of_node(n)==P_NODE)
		return 'P';	
	return 'a';   // to avoid warnings
	}


	jint
	Wrapper::
numberOfSons(int graphId, int nodeId)
	{
	SPQR_tree* T=SPQRMap[graphId];
	if (!T) gdt_error("NULL SPQR Tree");
	gdtnode n=T->find_node(nodeId);
	if (!n) gdt_error("NULL node");
	return T->number_of_sons(n);
	}


	jint
	Wrapper::
firstSon(int graphId, int nodeId)
	{
	SPQR_tree* T=SPQRMap[graphId];
	if (!T) gdt_error("NULL SPQR Tree");
	gdtnode n=T->find_node(nodeId);
	if (!n) gdt_error("NULL node");
	gdtnode fs=T->first_son_node(n);
	if(!fs) gdt_error("The node has no sons");
	return T->id(fs);
	}


	jint
	Wrapper::
succ_son(int graphId, int nodeId,int sonId)
	{
	SPQR_tree* T=SPQRMap[graphId];
	if (!T) gdt_error("NULL SPQR Tree");
	gdtnode father=T->find_node(nodeId);
	if (!father) gdt_error("NULL node");
	gdtnode son=T->find_node(sonId);
	if(!son) gdt_error("NULL son node");
	gdtnode succson=T->son_succ(son,father);
	if(!succson) gdt_error("NULL succ-son node");
	//cout << "GDTOOLKIT: il figlio successivo di " << T->id(father) << "  rispetto a "<< T->id(son) << " e' " << T->id(succson) << endl;
	return T->id(succson);
	}


	jint
	Wrapper::
spqr_number_of_nodes(int graphId)
	{
	SPQR_tree* T=SPQRMap[graphId];
	if (!T) gdt_error("NULL SPQR Tree");
	
	return T->number_of_nodes();
	}


	jint
	Wrapper::
spqr_get_Q_node(int graphId,int edgeId)
	{
	SPQR_tree* T=SPQRMap[graphId];
	if (!T) gdt_error("NULL SPQR Tree");
	
	gdtnode n=T->Q_node_of_edge_with_id(edgeId);
	if (!n) gdt_error("The edge does not exist");
	return T->id(n);
	}


	jint
	Wrapper::
spqr_get_level(int graphId,int nodeId)
	{
	SPQR_tree* T=SPQRMap[graphId];
	if (!T) gdt_error("NULL SPQR Tree");
	
	gdtnode n=T->find_node(nodeId);
	if (!n) gdt_error("The node does not exist");
		
	return T->get_level(n);
	}

	jint
	Wrapper::
spqr_depth_of_subtree(int graphId,int nodeId)
	{
	SPQR_tree* T=SPQRMap[graphId];
	if (!T) gdt_error("NULL SPQR Tree");
	
	gdtnode n=T->find_node(nodeId);
	if (!n) gdt_error("The node does not exist");
		
	return T->depth_of_subtree(n);
	}
///////////////////////////////////////////////////////////////////////////////
// JNI Implementation ---------------------------------------------------------
JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_createGraph(JNIEnv *env, jobject obj,jint graphId) 
{Wrapper::createGraph(graphId);}	

JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_createSPQRTree(JNIEnv *env, jobject obj,jint graphId) 
{Wrapper::createSPQRTree(graphId);}	


JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_createNode(JNIEnv *env, jobject obj,jint graphId, jint nodeId)
{Wrapper::createNode(graphId, nodeId);}
	
JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_createEdge(JNIEnv *env, jobject obj, jint graphId, jint sourceNodeId, jint targetNodeId, jint edgeId)
{Wrapper::createEdge(graphId, sourceNodeId, targetNodeId, edgeId);}

JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_deleteNode(JNIEnv *env, jobject obj,jint graphId, jint nodeId)
{Wrapper::deleteNode(graphId, nodeId);}

JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_deleteEdge(JNIEnv *env, jobject obj,jint graphId, jint edgeId) 
{Wrapper::deleteEdge(graphId, edgeId);}

JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_deleteGraph(JNIEnv *env, jobject obj,jint graphId) 
{Wrapper::deleteGraph(graphId);}


JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_deleteSPQRTree(JNIEnv *env, jobject obj,jint graphId) 
{Wrapper::deleteSPQRTree(graphId);}

JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_deleteOrthogonalGraph(JNIEnv *env, jobject obj,jint graphId) 
{Wrapper::deleteOrthogonalGraph(graphId);}


JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_deleteDrawnGraph(JNIEnv *env, jobject obj,jint graphId) 
{Wrapper::deleteDrawnGraph(graphId);}

JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_drawOrthogonalGraph(JNIEnv *env, jobject obj,jint graphId) 
{Wrapper::drawOrthogonalGraph(graphId);}

JNIEXPORT jdouble JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getNodeX(JNIEnv *env, jobject obj,jint graphId, jint nodeId)
{return Wrapper::getNodeX(graphId, nodeId);}

JNIEXPORT jdouble JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getNodeY(JNIEnv *env, jobject obj,jint graphId, jint nodeId)
{return Wrapper::getNodeY(graphId, nodeId);}

JNIEXPORT jdouble JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getNodeWidth(JNIEnv *env, jobject obj,jint graphId, jint nodeId)
{return Wrapper::getNodeWidth(graphId, nodeId);}

JNIEXPORT jdouble JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getNodeHeight(JNIEnv *env, jobject obj,jint graphId, jint nodeId)
{return Wrapper::getNodeHeight(graphId, nodeId);}

JNIEXPORT jint JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getEdgeBendsCount(JNIEnv *env, jobject obj, jint graphId, jint edgeId) 
{return Wrapper::getEdgeBendsCount(graphId, edgeId);}

JNIEXPORT jdouble JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getEdgeBendPointX(JNIEnv *env, jobject obj, jint graphId, jint edgeId, jint bendIndex) 
{return Wrapper::getEdgeBendPointX(graphId, edgeId, bendIndex);}

JNIEXPORT jdouble JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getEdgeBendPointY(JNIEnv *env, jobject obj, jint graphId, jint edgeId, jint bendIndex) 
{return Wrapper::getEdgeBendPointY(graphId, edgeId, bendIndex);}

JNIEXPORT jdouble JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getEdgeAnchorSourceX(JNIEnv *env, jobject obj, jint graphId, jint edgeId) 
{return Wrapper::getEdgeAnchorSourceX(graphId, edgeId);}

JNIEXPORT jdouble JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getEdgeAnchorSourceY(JNIEnv *env, jobject obj, jint graphId, jint edgeId) 
{return Wrapper::getEdgeAnchorSourceY(graphId, edgeId);}

JNIEXPORT jdouble JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getEdgeAnchorTargetX(JNIEnv *env, jobject obj, jint graphId, jint edgeId) 
{return Wrapper::getEdgeAnchorTargetX(graphId, edgeId);}

JNIEXPORT jdouble JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getEdgeAnchorTargetY(JNIEnv *env, jobject obj, jint graphId, jint edgeId) 
{return Wrapper::getEdgeAnchorTargetY(graphId, edgeId);}



////////////////////////////////////////////////////
//////New Methods - JNI code
////////////////////////////////////////////////////

/***********************************************
*YES!!! YES!!! YES!!! YES!!! YES!!!            *
***********************************************
JNIEXPORT jint JNICALL 
    Java_IntArray_sumArray(JNIEnv *env, jobject obj, jintArray arr)
{
     jint buf[10];
     jint i, sum = 0;
     env->GetIntArrayRegion(arr, 0, 10, buf);
     for (i = 0; i < 10; i++) {
         sum += buf[i];
     }
     return sum;
}
*/

JNIEXPORT jboolean JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_isPlanar(JNIEnv *env, jobject obj, jint graphId) 
{return Wrapper::isPlanar(graphId);}


JNIEXPORT jboolean JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_isConnected(JNIEnv *env, jobject obj, jint graphId) 
{return Wrapper::isConnected(graphId);}

JNIEXPORT jboolean JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_isBiconnected(JNIEnv *env, jobject obj, jint graphId) {return Wrapper::isBiconnected(graphId);}

JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_zeroBendEdge(JNIEnv *env, jobject obj,jint graphId, jint edgeId) 
{Wrapper::zero_bend_edge(graphId, edgeId);}

JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_angleSweep(JNIEnv *env, jobject obj,jint graphId, jint nodeId, jint edgeId, jint angle) 
{Wrapper::angleSweep(graphId, nodeId, edgeId, angle);}

JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_createOrthogonalGraph(JNIEnv *env, jobject obj, jint graphId) 
{return Wrapper::createOrthogonalGraph(graphId);}


JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_createConstrainedOrthogonalGraph(JNIEnv *env, jobject obj, jint graphId, jintArray arr,jint dimension) 
{
	gdtlist<jint> external_nodes;
	jint buf[dimension];
	env->GetIntArrayRegion(arr, 0, dimension, buf);
    	 for (int i = 0; i < dimension; i++)
	 {
        	external_nodes.append(buf[i]); 
	 }
	return Wrapper::createOrthogonalGraph(graphId, external_nodes);
}


JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_drawOrthogonalGraphUsingOrthogonal(JNIEnv *env, jobject obj, jint graphId) 
{return Wrapper::drawOrthogonalGraphUsingOrthogonal(graphId);}

JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_nodeWidth(JNIEnv *env, jobject obj,jint graphId, jint nodeId, double w) 
{Wrapper::nodeWidth(graphId, nodeId, w);}

JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_nodeHeight(JNIEnv *env, jobject obj,jint graphId, jint nodeId, double h) 
{Wrapper::nodeHeight(graphId, nodeId, h);}

JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_sameFaceNode(JNIEnv *env, jobject obj,jint graphId, jint nodeId, int f) 
{Wrapper::sameFaceNode(graphId, nodeId, f);}

JNIEXPORT jdouble JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getEdgeLength(JNIEnv *env, jobject obj, jint graphId, jint edgeId) 
{return Wrapper::getEdgeLength(graphId, edgeId);}

JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_rotate(JNIEnv *env, jobject obj,jint graphId, jint angle) 
{Wrapper::rotate(graphId, angle); }	


JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_flipHorizontally(JNIEnv *env, jobject obj,jint graphId) 
{Wrapper::flipHorizontally(graphId); }	


JNIEXPORT void JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_flipVertically(JNIEnv *env, jobject obj,jint graphId) 
{Wrapper::flipVertically(graphId); }	

JNIEXPORT jint JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getRoot(JNIEnv *env, jobject obj,jint graphId) 
{return Wrapper::getRoot(graphId); }	

JNIEXPORT jchar JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_nodeType(JNIEnv *env, jobject obj,jint graphId, jint nodeId) 
{return Wrapper::nodeType(graphId,nodeId); }	

JNIEXPORT jint JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_numberOfSons(JNIEnv *env, jobject obj,jint graphId, jint nodeId) 
{return Wrapper::numberOfSons(graphId,nodeId); }	

JNIEXPORT jint JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_firstSon(JNIEnv *env, jobject obj,jint graphId, jint nodeId) 
{return Wrapper::firstSon(graphId,nodeId); }	

JNIEXPORT jint JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_numberOfNodes(JNIEnv *env, jobject obj,jint graphId) 
{return Wrapper::spqr_number_of_nodes(graphId); }	

JNIEXPORT jint JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_succSon(JNIEnv *env, jobject obj,jint graphId, jint nodeId,jint sonId) 
{return Wrapper::succ_son(graphId,nodeId,sonId); }	

JNIEXPORT jint JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getQNode(JNIEnv *env, jobject obj,jint graphId, jint edgeId) 
{return Wrapper::spqr_get_Q_node(graphId,edgeId); }	

JNIEXPORT jint JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_getLevel(JNIEnv *env, jobject obj,jint graphId, jint nodeId) 
{return Wrapper::spqr_get_level(graphId,nodeId); }	

JNIEXPORT jint JNICALL 
Java_GdtJavaWrapper_WrapperImplementation_depthOfSubtree(JNIEnv *env, jobject obj,jint graphId, jint nodeId) 
{return Wrapper::spqr_depth_of_subtree(graphId,nodeId); }	


