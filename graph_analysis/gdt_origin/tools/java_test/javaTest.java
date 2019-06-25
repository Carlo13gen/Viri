import GdtJavaWrapper.*; 
import java.util.*;

public class javaTest {
	
	static {System.loadLibrary("GdtJavaWrapper");}
	
	public static void main(String[] args) {
	    
	System.out.println("\n\nStarting GDTooklkit java wrapper test...\n");	
	Graph ug=new Graph();
    
	Node n0=ug.newNode();
        Node n1=ug.newNode();
	Node n2=ug.newNode();
        Node n3=ug.newNode();
        Node n4=ug.newNode();
        Node n5=ug.newNode();       
	Node n6=ug.newNode();      

	Node n7=ug.newNode();              
	Node n8=ug.newNode();      

	Edge e0=ug.newEdge(n0, n1);
	Edge e1=ug.newEdge(n1, n2);
	Edge e2=ug.newEdge(n2, n3);
	Edge e3=ug.newEdge(n3, n4);
	Edge e4=ug.newEdge(n4, n5);
	Edge e5=ug.newEdge(n5, n6);
	Edge e6=ug.newEdge(n6, n0);

	Edge e7=ug.newEdge(n1, n7);
	Edge e8=ug.newEdge(n3, n7);
	Edge e10=ug.newEdge(n7, n8);
	Edge e13=ug.newEdge(n8, n2);

	System.out.println("Graph created");     
	if (ug.isPlanar())
		System.out.println("The graph is planar");
	else
		System.out.println("The graph is planar");
	///////////////////////////////////////
	if (ug.isBiconnected())
		{
		System.out.println("Biconnected graph.");
		SPQRTree T=new SPQRTree(ug);
		int number=T.numberOfNodes();
		//System.out.println("****************************************");
		System.out.println("SPQR tree created, nodes="+number);	
		HashMap<Integer,SPQRNode> nodes=T.getNodes();
		/*		
		for(int i : nodes.keySet())
			{
			System.out.println(nodes.get(i).getNodeType()+"\tindex:"+nodes.get(i).getId()+"\tfather: "+nodes.get(i).getFatherId());
			System.out.println("---------------------------------");
			System.out.println("Sons: "+ nodes.get(i).getSons());
			System.out.print("---------------------------------");
			System.out.println();
			}
		System.out.println("Cheking some Q node...");
		SPQRNode Q=T.getQNode(e7.getId());		
		System.out.println("Edge:"+e7.getId()+"\t"+Q.getNodeType()+"\tlevel:"+T.getLevel(Q)+"\t\tsubtree depth:"+T.depthOfSubtree(Q));
		Q=T.getQNode(e3.getId());		
		System.out.println("Edge:"+e3.getId()+"\t"+Q.getNodeType()+"\tlevel:"+T.getLevel(Q)+"\t\tsubtree depth:"+T.depthOfSubtree(Q));
		Q=T.getQNode(e0.getId());		
		System.out.println("Edge:"+e0.getId()+"\t"+Q.getNodeType()+"\tlevel:"+T.getLevel(Q)+"\t\tsubtree depth:"+T.depthOfSubtree(Q));
		System.out.println("****************************************");  

		*/		
		}
	else
		{
		System.out.println("Non-biconnected graph.");	
		}	

	
	
	ug.zeroBendEdge(e0);
	ug.zeroBendEdge(e1);
	ug.zeroBendEdge(e2);
	ug.zeroBendEdge(e3);
	ug.zeroBendEdge(e4);
	ug.zeroBendEdge(e5);
	ug.zeroBendEdge(e6);

	Node[] externals;
	externals=new Node[7];	
	externals[0]=n0;
	externals[1]=n1;
	externals[2]=n2;
	externals[3]=n3;
	externals[4]=n4;
	externals[5]=n5;
	externals[6]=n6;

	
	////////////////////////////////////////////////
	System.out.println("Constraints on Graph: OK");   
	OrthogonalGraph opug=new OrthogonalGraph(ug,externals);
	System.out.println("Orthogonal Graph created");   
	
	//opug.setNodeWidth(n1,10);
	//opug.setNodeHeight(n1,1);
	
	//opug.setNodeWidth(n6,15);
	//opug.setNodeHeight(n6,1);

	System.out.println("Constraints on OrthogonalGraph: OK");

	OrthogonalDrawer D=new OrthogonalDrawer(opug);
	System.out.println("Orthogonal Drawing created");	

	D.rotate(90);	
	D.flipHorizontally();
	D.flipVertically();

	System.out.println("Rotation and flip: OK");

	System.out.println("\n\nJava Wrapper test: OK\n");

	}
}
