#include <GDT/rm3_draw_undi_graph.h>
#include "gdtcommline_init.h"


void output_points(draw_undi_graph& dug, double delta);
void output_segment(draw_undi_graph& dug, gdt::gdtpoint p1, gdt::gdtpoint p2, double delta);




int main(int argc, char ** argv) {

  init(argc, argv);

  if(verbose) std::cerr << "Opening file \"" << ifile << "\"\n";

  if (action == "stat") {
  
    if ( stat_type == "nodes_and_edges" ) {

        undi_graph ug;
        ug.read(ifile);
        if(verbose) std::cerr << "Graph " << ifile << " read into memory\n";
        std::cout << ug.number_of_nodes() << "\t" << ug.number_of_edges() << std::endl;
        if(verbose) std::cerr << " ug: " << ug.number_of_nodes() << "\t" << ug.number_of_edges() << std::endl;

    } else if (stat_type == "node_degree") {
        undi_graph ug;
        ug.read(ifile);
        if(verbose) std::cerr << "Graph " << ifile << " read into memory\n";
        gdtnode n;
        forall_nodes(n,ug) {
           std::cout << ug.degree(n) << std::endl;
           if(verbose) std::cerr << ug.degree(n) << std::endl;
        }

    } else if (stat_type == "bends") {
        draw_undi_graph dug;
        dug.read(ifile);
        if(verbose) std::cerr << "Graph " << ifile << " read into memory\n";
        int number_of_bends = 0;
        gdtedge e;
        forall_edges(e,dug) {
           number_of_bends += dug.bends(e).size();
        }
        std::cout << number_of_bends << std::endl;
        if(verbose) std::cerr << number_of_bends << std::endl;

    } else if (stat_type == "bends_per_edge") {
        draw_undi_graph dug;
        dug.read(ifile);
        if(verbose) std::cerr << "Draw_undi_graph " << ifile << " read into memory\n";
        gdtedge e;
        forall_edges(e,dug) {
	    std::cout << dug.bends(e).size() << std::endl;
            if(verbose) std::cerr << dug.bends(e).size() << std::endl;
        }

    } else if (stat_type == "face_degree") {
        undi_graph ug;
        ug.read(ifile);
        if(verbose) std::cerr << "Graph " << ifile << " read into memory\n";
        plan_undi_graph pug(ug);  // if it is already planar it preserves the embedding
        face f;
        forall_GDT_faces(f,pug) {
          std::cout << pug.degree(f,false) << std::endl;
          if(verbose) std::cerr << pug.degree(f,false) << std::endl;
        }

    } else if (stat_type == "nn") {
        
	undi_graph ug;
        ug.read(ifile);
        if(verbose) std::cerr << "Graph " << ifile << " read into memory\n";

        int nodes = ug.get_max_node_id()+1;
        
        // Correction to compile with Visual Studio
	// (the original code was: "int distance[nodes][nodes];")
        // distance[nodeid1][nodeid2] gives the distance between the two nodes
        // distance will be deallocated below
        //
        int** distance;
	distance=new int*[nodes];
	for(int temp_index=0; temp_index<nodes; temp_index++) {
		distance[temp_index]=new int[nodes];
	}
	
	int maxdist = 0;  // the maximum distance between any two nodes
 
        /* compute the distance between any two pair of nodes,
	   update distance[id1][id2] with this value,
	   update the value of maxdist. */

        gdtnode s;
        gdtnode t;
        forall_nodes(s,ug) {
   
          gdt::gdtnode_array<bool>    reached(ug.nodes_and_edges(),false); // needed by simple_BFS but not used here
          gdt::gdtnode_array<gdtedge> father(ug.nodes_and_edges(),NULL_EDGE);  // needed by simple_BFS but not used here
          gdt::gdtnode_array<int>     dist(ug.nodes_and_edges(),0);
          gdt::gdtlist<gdtnode>       order; // needed by simple_BFS but not used here

          reached.clear();
          father.clear();
          dist.clear();
          order.clear();
   
          ug.simple_BFS(s,reached,father,dist,order);
	
	  int sid = ug.id(s);

          forall_nodes(t,ug) {
            distance[sid][ug.id(t)] = dist[t];
            if( dist[t] > maxdist ) maxdist = dist[t];	  	
	  }
        }
        
	//std::cout << "maxdist = " << maxdist << "\n";

        // define and initialize an array that will contain the total number of pairs that 
	// have the minimum distance as specified by the index. totpairs[1] is the total
	// number of pairs which are at distance 1 (i.e., the number of edges of the graph)
        //
	int* totpairs;    
	totpairs=new int[maxdist+1];        
	for( int i=0; i <= maxdist; i++ ) totpairs[i]=0;
	
        forall_nodes(s,ug) {
          forall_nodes(t,ug) { 
            int sid = ug.id(s);
            int tid = ug.id(t);
            if( tid > sid ) {  // each pair must be counted one time (tid == sid not interesting)

		/*
		if(distance[sid][tid]!=distance[tid][sid]) {
  		std::cerr << "distance[" << sid << "][" << tid << "]=" << distance[sid][tid] << "\n";
  		std::cerr << "distance[" << tid << "][" << sid << "]=" << distance[tid][sid] << "\n";
  		std::cerr.flush();
  		exit(1);
		}
		*/
	      totpairs[distance[sid][tid]]++;
	    }
          }  // end of forall_nodes(t,ug)
        } // end of forall_nodes(s,ug)
      
        /*
           now we want totpairs[x] to hold the total number of 
	   pairs reachable with AT MOST x steps.  
      
                  totpairs
              *-------------*
	      |     0       |
	      *-------------*
              |     1       |
	      *-------------*
              |    ...      |
	      *-------------*
	      |             |
	      *-------------*
	      | temp_index2 |
	      *-------------*
	      | maxdist-1   |  <---- we have to add totpairs[temp_index2] here ...
	      *-------------*
	      | maxdist     |  <---- ... and here and move temp_index2 up
	      *-------------*
                maxdist+1
	                   
        */
      
        for(int temp_index2 = maxdist-1; temp_index2 > 0; temp_index2-- )
          for( int j = temp_index2+1; j <= maxdist; j++ )
	     totpairs[j] += totpairs[temp_index2];
      
        // now we produce the output
        //
        for(int temp_index3=1; temp_index3 <= maxdist; temp_index3=temp_index3*2 ) {
          std::cout << temp_index3 << "\t" << 2.0*totpairs[temp_index3]/(nodes*nodes) << std::endl;
        }

	// deallocate "int distance[][]" and "int totpairs[]"
	//
	for(int temp_index4=0; temp_index4<nodes; temp_index4++) {
		free(distance[temp_index4]);
	}
	free(distance);
	free(totpairs);

    } else if (stat_type == "bb") {
        draw_undi_graph dug;
        dug.read(ifile);
        if(verbose) std::cerr << "Graph " << ifile << " read into memory\n";
        gdtnode n = dug.first_node();
        gdt::gdtpoint p = dug.center(n);
        double x_min = p.xcoord();
        double x_max = x_min;
        double y_min = p.ycoord();
        double y_max = y_min;
        forall_nodes(n,dug) {
           p = dug.center(n);
	   double x = p.xcoord();
	   double y = p.ycoord();
	   if( x < x_min ) x_min = x;
	   if( x > x_max ) x_max = x;
	   if( y < y_min ) y_min = y;
	   if( y > y_max ) y_max = y;
        }
        std::cout << x_max-x_min << "\t" << y_max-y_min << std::endl;
        if(verbose) std::cerr << x_max-x_min << "\t" << y_max-y_min << std::endl;

     } // end stat

   } else if (action == "draw") {
   
      undi_graph ug;
      ug.read(ifile);
      if(verbose) std::cerr << "Graph " << ifile << " read into memory\n";

      if( drawing_approach == "TSM" ) {  // Topology-Shape-Metrics

         /* Topology (Planarization) */

         plan_undi_graph pug (ug);

         if(verbose) std::cerr << "Graph " << ifile << " planarized\n";

         /* Shape (Orthogonalization) */

         orth_plan_undi_graph opug(pug,orthogonalization_alg);

         if(verbose) std::cerr << "Graph " << ifile << " orthogonalized\n";
         if(verbose) std::cerr << "  ug: " << ug.number_of_nodes() << "\t" << ug.number_of_edges() << std::endl;
         if(verbose) std::cerr << " pug: " << pug.number_of_nodes() << "\t" << pug.number_of_edges() << std::endl;
         if(verbose) std::cerr << "opug: " << opug.number_of_nodes() << "\t" << opug.number_of_edges() << std::endl;

         /* Metrics (Compaction) */
   
         draw_undi_graph dug(opug,compaction_alg);
         if(verbose) std::cerr << "Graph " << ifile << " compacted\n";

         /* saving the file */
         if(verbose) std::cerr << " dug: " << dug.number_of_nodes() << "\t" << dug.number_of_edges() << std::endl;
         dug.write(ofile);
         if(verbose) std::cerr << "Drawing saved in file \"" << ofile << "\"\n";


      } else if (drawing_approach=="POLYLINE") {


         /* Planarization */

         plan_undi_graph pug (ug);

         if(verbose) std::cerr << "Graph " << ifile << " planarized\n";
        
         draw_undi_graph dug (pug, POLYLINE); 

         /* saving the file */
         if(verbose) std::cerr << " dug: " << dug.number_of_nodes() << "\t" << dug.number_of_edges() << std::endl;
         dug.write(ofile);
         if(verbose) std::cerr << "Drawing saved in file \"" << ofile << "\"\n";
      
      } else if (drawing_approach == "RCS") { // relative coordinates scenario

         rel_coord_orth rco(ug);

         bool remove_dummy_vertices = true;
	 if( rcs_alg == "ORTH_SIMPLE_PT00" ) remove_dummy_vertices = true;
	 else if ( rcs_alg == "ORTH_SIMPLE_PT00_DEBUG" ) remove_dummy_vertices = false;
	 draw_undi_graph dug(rco,remove_dummy_vertices); 

         /* saving the file */
         if(verbose) std::cerr << " dug: " << dug.number_of_nodes() << "\t" << dug.number_of_edges() << std::endl;
         dug.write(ofile);
         if(verbose) std::cerr << "Drawing saved in file \"" << ofile << "\"\n";
      } else if (drawing_approach == "OFV") { // orthogonal from visibility
        
         plan_undi_graph pug (ug);

         if(verbose) std::cerr << "Graph " << ifile << " planarized\n";

         draw_undi_graph dug (pug, ORTH_FROM_VISIBILITY); 

         /* saving the file */
         if(verbose) std::cerr << " dug: " << dug.number_of_nodes() << "\t" << dug.number_of_edges() << std::endl;
         dug.write(ofile);
         if(verbose) std::cerr << "Drawing saved in file \"" << ofile << "\"\n";
      
      }
   } else if ( action == "points" ) {
       
       if(verbose) std::cerr << "Opening file " << ifile << "\n";
       draw_undi_graph dug;
       dug.read(ifile);
       if(verbose) std::cerr << "File " << ifile << " opened\n";
       output_points(dug,points_delta);
   }
   return 0;
}

/* output a segment as a collection of points.
   delta is the maximum distance between to points. */

void output_segment(draw_undi_graph& dug, gdt::gdtpoint p1, gdt::gdtpoint p2, double delta) {
  
  double p1x = p1.xcoord();
  double p2x = p2.xcoord();
  double p1y = p1.ycoord();
  double p2y = p2.ycoord();
  
  double dx = p2x-p1x;
  double dy = p2y-p1y;
  
  double module = sqrt(dx*dx+dy*dy);  // module of vector (p1,p2)
  
  double delta_x = dx/module*delta;  // vector whose length is delta and direction (p1,p2)
  double delta_y = dy/module*delta;

  std::cout << p1x << "\t" << p1y << "\n";
  p1x = p1x + delta_x;
  p1y = p1y + delta_y;

  bool inside_segment = false;
  if(delta_x > 0) {
    inside_segment = p1x < p2x;
  } else if (delta_x < 0) {
    inside_segment = p1x > p2x;
  } else if (delta_y > 0) {
    inside_segment = p1y < p2y;
  } else if (delta_y < 0) {
    inside_segment = p1y > p2y;
  } else {
    std::cerr << "segment of length 0 passed to function ";
    std::cerr << "output_segment" << "\n";
    exit(1);
  }
  while( inside_segment ) {

    std::cout << p1x << "\t" << p1y << "\n";
    p1x = p1x + delta_x;
    p1y = p1y + delta_y;

    inside_segment = false;
    if(delta_x > 0) {
      inside_segment = p1x < p2x;
    } else if (delta_x < 0) {
      inside_segment = p1x > p2x;
    } else if (delta_y > 0) {
      inside_segment = p1y < p2y;
    } else if (delta_y < 0) {
      inside_segment = p1y > p2y;
    } 
  }
}

/* output the drawing as a collection of points, that is
   pairs of coordinates (one pair per line). Nodes are translated
   into a single point. Edges are sequences of segments. Each segment
   is translated into a sequence of point at maximum distance delta */

void output_points(draw_undi_graph& dug, double delta) {
  
  // if the dug is void output nothing
  //
  gdtnode n = dug.first_node();
  if( n==NULL_NODE ) return;  
  
  if( points_bends_only ) {

    gdtedge e;
    forall_edges(e,dug) {
      gdt::gdtlist<gdt::gdtpoint> Lb = dug.bends(e,dug.source(e));
      gdt::gdtpoint p;
      forall(p,Lb) {
        std::cout << p.xcoord() << "\t" << p.ycoord() << "\n";;
      }
    }

  } else {
  
    // first output the center of the nodes
    //
    gdt::gdtpoint p;
    forall_nodes(n,dug) {
    
      p = dug.center(n);
      std::cout << p.xcoord() << "\t" << p.ycoord() << "\n";
    }

    if( points_nodes_only ) return;

    gdtedge e;
    forall_edges(e,dug) {
      gdt::gdtlist<gdt::gdtpoint> Lb = dug.bends(e,dug.source(e));

      //Lb.push_front(dug.anchor_source_point(e));
      Lb.push_front(dug.center(dug.source(e)));

      //Lb.push_back(dug.anchor_target_point(e));
      Lb.push_back(dug.center(dug.target(e)));
      
      gdt::gdtpoint p1 = Lb.front();
      Lb.pop_front();
      gdt::gdtpoint p2;
      forall(p2,Lb) {
        output_segment(dug,p1,p2,delta);
        p1 = p2;
      }
    }
  }
}





/*
    {
      if (!curved_edges) dug.export_to_fig_file_format (file_name,false,0.6,WM.scale()/correction);
      else               dug.export_to_fig_file_format (file_name,true,0.6,WM.scale()/correction);
    }
*/
