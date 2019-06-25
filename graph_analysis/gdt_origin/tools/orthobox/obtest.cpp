
#include "orthobox.h"

using namespace std;


int main(int argc, char** argv)
    {

    if (argc==1)
        {
        cout << "usage: " << argv[0] << " <gdtfile> n1 n2 n3 ... \n" 
             << " The nodes 'n1 n2 n3 ...' are put on the boundary box."
             << endl;
        exit(EXIT_FAILURE);
        }

    undi_graph ug;
    ug.read(argv[1]);
    cout << "nodes: " << ug.number_of_nodes() << endl;
    cout << "edges: " << ug.number_of_edges() << endl;


    try {

    Orthobox d;   // create the drawer


    gdtnode v;
    forall_nodes(v, ug)
        d.insertNode(ug.id(v),0,0);


    gdtedge e;
    forall_edges(e, ug)
      {
      gdtnode src= ug.source(e);
      gdtnode trg= ug.target(e);
      d.insertEdge(ug.id(e), ug.id(src),ug.id(trg));
      }

    

    // set the constraints
    for( int i=2; i<=argc-1; i++)
        {
        int id= atoi(argv[i]);
        cout << id << " on the boundary box" << endl;
        d.putOnTheBox(id);
        }

    d.saveInput("inputgr.gdt", "inputpar.txt");
    draw_undi_graph dug;
    d.computeDrawing(dug);
   
// d.saveDrawing("result.gdt"); // save result in a file.
   dug.write("result.gdt");

    forall_nodes(v, ug)
      {
      int node_id= ug.id(v);
      pair<double,double> c= d.getNodeCoord(node_id);
      cout << node_id << "   " << c.first << ' ' << c.second << endl;
      }
    forall_edges(e, ug)
      {
      int edge_id= ug.id(e);
      cout << edge_id << endl;
      gdt::gdtlist< pair<double,double> > cl= d.getEdgeCoord(edge_id);
      pair<double,double> temp_iterator;
      forall(temp_iterator,cl)
	cout<< "   " << temp_iterator.first << ' ' << temp_iterator.second << endl;
      //for( gdt::gdtlist< pair<double,double> >::iterator i=cl.begin(); i!=cl.end(); i++ )
      //    cout<< "   " << (*i).first << ' ' << (*i).second << endl;
      }

    cout << "border frame edges: " << endl;
    gdt::gdtlist<int> L= d.boxEdges();
    int temp_iterator1;
    forall(temp_iterator1,L)
//    for( gdt::gdtlist<int>::iterator j=L.begin(); j!=L.end(); j++ )
        {
        cout << temp_iterator1 << endl;
        gdt::gdtlist< pair<double,double> > cl= d.getEdgeCoord(temp_iterator1);
	pair<double, double> temp_iterator2;
	forall(temp_iterator2, cl)        
	//for( list< pair<double,double> >::iterator k=cl.begin(); k!=cl.end(); k++ )
            cout<< "   " << temp_iterator2.first << ' ' << temp_iterator2.second << endl;
        }
 

      } 
    catch (Orthobox::IdError ex)
        {
        cout << "Exception: "<< ex.c_str() << endl;
        }

    
    }
