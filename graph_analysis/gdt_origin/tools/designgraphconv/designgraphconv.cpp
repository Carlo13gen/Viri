

#include <GDT/rm3_draw_undi_graph.h>
#include <GDT/rm3_upwa_plan_undi_graph.h>
#include <GDT/stopwatch.h>
#include <GDT/rm3_tree.h>
#include <string>
#include <stdio.h>

using namespace std;



    int main
    (int argc, char* argv[])
    {
  
	  if (argc!=3) {
		cout << "USAGE: " << argv[0] << " FILE_NAME  DESIGN_ALGORITHM"  << endl;
		cout << "THERE ARE 3 ALGORITHM: ORTH_PLAN, TREE, UPWARD" << endl;
		return 1; 
	  }
	  gdtedge e;
	  gdtnode v;
	  gdt::gdtpoint _point;
	  draw_undi_graph dug,dug3;
	  draw_undi_graph* dug2;
	  undi_graph ug,ug2;
	  std::string sec_arg;
      bool riuscito;
	  int max_id;
	  
	  sec_arg = argv[2];
	  if ((sec_arg != "ORTH_PLAN") && (sec_arg != "TREE") && (sec_arg != "UPWARD")){
			cout << "Error: the secund argoment is not correct or is not present; abort" << endl;
			return 1;		  
	  }	  
	  dug.import_from_graphml_file_format(argv[1]);
	  max_id = dug.get_max_node_id();
	  
	  //double height_nodes[max_id+1];
	  double* height_nodes=new double[max_id+1];
	  //double width_nodes[max_id+1];
	  double* width_nodes=new double[max_id+1];
	  //  Save the dimension of each nodes, because the algorithms tends to change it
	  forall_nodes(v,dug){
			height_nodes[dug.id(v)] = dug.height(v);
			width_nodes[dug.id(v)] = dug.width(v);
	  }
	  
	  riuscito = dug.write("risultato.gdt");
	  if (riuscito)
	  		cout << "Result file created successfully" << endl;
	  
	  riuscito = ug2.read("risultato.gdt");
	  if (riuscito)
	  	cout << "Undi_graph created successfully" << endl;
	  
	  if(sec_arg == "ORTH_PLAN"){		  
			  orth_plan_undi_graph opug(ug2,PLAN_ORTH_OPTIMAL,true) ;
			  dug2 = new draw_undi_graph(opug,DEFAULT_ALGORITHM,true);
	  }
	  else if(sec_arg == "TREE"){  
		  	  v = ug2.first_node();
	  		  tree tr(ug2,v);
	  		  dug2 = new draw_undi_graph(tr,DEFAULT_ALGORITHM,false);
	  }
	  else if(sec_arg == "UPWARD"){	  
		  	  plan_undi_graph pug(ug2,DO_NOT_PRESERVE_EMBEDDING,false);	
		  	  upwa_plan_undi_graph upug(pug,PLAN_UPWARD_SLOW,false);
		  	  dug2 = new draw_undi_graph(upug,DEFAULT_ALGORITHM,false);
	  }
	  else{
			  cout << "Error: missing second argument or argument not valid" << endl;
		  	  dug2 = new draw_undi_graph(dug,true);
	  }
		  if ( remove("risultato.gdt") == -1)
			  cout << "Error deleting the file" << endl;
		  else
			  cout << "File deleted successfully" << endl;
		  
		 /*forall_nodes(v,*dug2){
				dug2->set_height(v,height_nodes[dug2->id(v)]);
				dug2->set_width(v,width_nodes[dug2->id(v)]);
		 }*/
		  dug2->export_to_graphml_file_format("risultato.graphml");
	 
	  
	  return EXIT_SUCCESS;

}
