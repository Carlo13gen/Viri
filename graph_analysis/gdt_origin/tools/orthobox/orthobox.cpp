#include <stdio.h>
#include <assert.h>
#include "orthobox.h"





Orthobox::Orthobox(): 
  opug(NULL), 
  dir_is_valid(false), 
  dug(NULL)
    {};

Orthobox::~Orthobox()
    {
    delete opug;
    delete dug;
    };

void Orthobox::insertNode(int id, int width, int height)
    {
    if (ug.find_node(id)) 
        throw IdError("Duplicated node ID in insertNode");

    gdtnode n= ug.new_node(id);
    ug.new_constraint_node_width( n, width );
    ug.new_constraint_node_height( n, height );
    node_width[id]= width; 
    node_height[id]= height; 
    };




void Orthobox::insertEdge(int id, int nodeIdSource, int nodeIdTarget)
    {
    if (ug.find_edge(id)) 
        throw IdError("Duplicated edge ID in insertEdge");
    gdtnode nsrc= ug.find_node(nodeIdSource);
    if ( nsrc==NULL ) 
        throw IdError("Invalid source node ID in insertEdge");
    gdtnode ntrg= ug.find_node(nodeIdTarget);
    if ( nsrc==NULL ) 
        throw IdError("Invalid target node ID in insertEdge");

    gdtedge e= ug.new_edge(nsrc,ntrg, id);
    ug.make_directed(e, nsrc);
    };



pair<double,double> Orthobox::getNodeCoord(int nodeId)
    {
    gdtnode n=dug->find_node(nodeId);
    if ( n==NULL ) 
        throw IdError("Invalid node ID in getNodeCoord");
    gdt::gdtpoint p= dug->center(n);
    return make_pair(p.xcoord(),p.ycoord());
    };

gdt::gdtlist<int> Orthobox::boxEdges()
    {
    return border_edges;
    }

gdt::gdtlist< pair<double,double> > Orthobox::getEdgeCoord(int edgeId)
    {
    
    gdtedge e= dug->find_edge(edgeId);

    if (e==NULL) 
        throw IdError("Invalid edge ID in getEdgeCoord");

    gdtnode s=dug->start(e);
    gdtnode t=dug->stop(e);

    gdt::gdtlist<gdt::gdtpoint> bend_list= dug->bends(e, s); // get bends
    bend_list.push_front( dug->anchor_point(e,s) ); // source anchor point
    bend_list.append( dug->anchor_point(e,t) ); // target anchor point


    // conversion to STL lists

    gdt::gdtlist< pair<double,double> > C;

    gdt::gdtpoint p;
    forall(p, bend_list)
      C.push_back( make_pair(p.xcoord(),p.ycoord()) );

    return C;
};



void Orthobox::putOnTheBox(int idNode)
    {
    if (!ug.find_node(idNode)) 
        throw IdError("Invalid node ID in putOnTheBox");
    on_the_box.insert(idNode);
    }




void Orthobox::saveDrawing(const char *filename){

        if (!dug)
            gdt_error("No drawing to save, compute it first.");
        if (!filename)
            gdt_error("No filename.");
        dug->write(filename);
}

void Orthobox::saveInput(const char *graphfile, const char *paramsfile){

        if (!graphfile)
            gdt_error("No graph filename.");
        if (!graphfile)
           gdt_error("No parameters filename.");

        ug.write(graphfile);

        std::ofstream out(paramsfile);

        out << "Nodes on the boundary box: \n" << std::endl;

        // std:: is necessary in order to compile on Win MSVC 6.0
        for (std::set<int>::iterator i=on_the_box.begin(); i!=on_the_box.end(); i++ )
            out  <<  (*i) << std::endl;;
            
}






void Orthobox::computeDrawing(draw_undi_graph& result){

    
// phase 1: 

     gdt::gdtlist<gdtnode> L_ext_ug;

     if (on_the_box.size()>0) // set the constraints (if any)
         {
        // std:: is necessary in order to compile on Win MSVC 6.0
         for(std::set<int>::iterator i= on_the_box.begin(); i!= on_the_box.end(); i++)
             L_ext_ug.push_back(ug.find_node(*i));
         ug.new_constraint_same_face_node( L_ext_ug, 1 );  // 1: whatever id
         }

     // make the graph connected augmenting it with dummy edges
     gdt::gdtlist<gdtedge> remove_me;
     if( !ug.is_connected() ) 
         remove_me = ug.make_connected();        


    plan_undi_graph pug = plan_undi_graph (ug,DO_NOT_PRESERVE_EMBEDDING);

    

    gdt::gdtlist<gdtnode> L_ext_pug;
    gdtnode n;
    forall(n,L_ext_ug) 
      L_ext_pug.append(pug.corresponding_node(n,ug));


    // phase 2: constrained orthogonalization. Edges are added to create a
    // frame around the graph before orthogonalization

    GDT_face extFace=NULL;

    if (L_ext_pug.size()>0)
         {
         // find the face the contains all nodes, such face is considered to be external
         // WARNING: the following step is O(n^2)
         GDT_face f;
         bool found;

         forall_faces(f, pug)
           {
           found=true;
           gdtnode n;
           forall(n, L_ext_pug)
             {
             if ( !pug.node_belongs_to_face(n, f) )
                 {
                 found=false;
                 break;
                 }
             }
           if (found) break;
           }
         // now, 'f' contains the face we were looking for
         extFace=f;
         assert(found);


         // find ordered list of nodes on the ext face
         //
         gdt::gdtlist<gdtnode> L_ef_n = pug.adj_nodes(extFace);  // clockwise list (may have duplicates)

         // removing duplicates and nodes not constrained
         //
         gdt::gdtmap<gdtnode,bool> constr_nodes;
         forall(n,L_ext_pug)
           constr_nodes[n] = true;
         
         gdt::gdtlist<gdtnode> L_ext_pug_ordered;
         forall(n,L_ef_n)
           {
           if( constr_nodes[n] )
               {
               L_ext_pug_ordered.append(n);
               constr_nodes[n] = false; // to avoid duplicates
               }
           }



         // ************** builds the frame (that is the box) **************

         gdt::gdtlist<gdtedge> L_pug_frame_e;      // all the edges inserted to form the frame
         //leda_node pug_dummy_n = NULL_NODE;  // a node possibly inserted to avoid a loop

         if (L_ext_pug.size()==1) // this special case is to avoid selfloops
             {

             gdtnode v= L_ext_pug.front();
             gdt::gdtlist<gdtedge> Le= pug.edges_entering_node_while_moving_around_face(v, extFace);
             gdtedge e_ref= Le.front();

             gdtnode pug_dummy_n= pug.new_node(v,e_ref);  // attach the first node/edge to 'v' after 'e_ref'
             gdtedge e_new1= pug.first_adj_edge(pug_dummy_n);
             gdtedge e_new2= pug.new_edge(pug_dummy_n, e_new1, v, e_ref ); // close the chain (this split extFace)

             pug.new_constraint_angle_sweep( v, e_new1, _180 );
             L_pug_frame_e.append(e_new1);
             L_pug_frame_e.append(e_new2);
             pug.new_constraint_bend_direction_on_edge(e_new1, pug_dummy_n);
             pug.new_constraint_bend_direction_on_edge(e_new2, v);

             //update the external face
             extFace= pug.right_face_moving_along_edge( e_new1, v );
             }
         else
             {
             // *** walking around external face counterclockwise ***
             
             // easy case: only one vertex on the external face
             
             
             // extFace: is updated after each edge insertion (which split the exernal face)
             
             list_item j;
             gdtedge first_ins_edge=NULL; // contains first inserted edge
             gdtedge prev_ins_edge=NULL; // contains last inserted edge


             forall_items(j, L_ext_pug_ordered) // scan the list of the constrained nodes.
               {
               bool first= (j==L_ext_pug_ordered.first());  // first and last edge insertion will be treated differently
               bool last=  (j==L_ext_pug_ordered.last() );

               gdtnode curr1= L_ext_pug_ordered.inf(j);   // the current pair of nodes, the new edge will be inserted between these two nodes
               gdtnode curr2= L_ext_pug_ordered.inf(L_ext_pug_ordered.cyclic_succ(j));


               gdtedge e;   // newly inserted edge
               if (first)
                   {
                   e= pug.new_edge(curr1, curr2, extFace);
                   first_ins_edge= e;
                   }
               else if (last)
                   {
                   gdtedge e1= pug.cyclic_adj_pred ( first_ins_edge, curr2 );
                   e= pug.new_edge(curr1, prev_ins_edge, curr2, e1 );
                   }
               else
                   {
                   assert (pug.node_belongs_to_face(curr2, extFace));
                   gdt::gdtlist<gdtedge> Le= pug.edges_entering_node_while_moving_around_face( curr2, extFace);
                   e= pug.new_edge(curr1, prev_ins_edge, curr2, Le.front() );
                   }


               // setting the constraint for the angle of 'curr1'
               pug.new_constraint_angle_sweep( curr1, e, _180 );
               //pug.new_constraint_number_of_bends_on_edge (e, NONE);
               pug.new_constraint_bend_direction_on_edge(e, curr2); // edge can bend on the right only
	

               //update external face
               extFace= pug.right_face_moving_along_edge( e, curr1 );

               // prepare for the next insertion
               prev_ins_edge=e;  
               }

             }
         }

    // if we have no external face it means that no frame has been created
    // so no corrections are necessary, otherwise the orientation of the edges that
    // represent the box should take an arbitrary direction
     if (extFace!=NULL)
         {
         gdtedge e1;
         forall_GDT_face_edges(e1, extFace)
           {
           border_edges.push_back(pug.id(e1));
           pug.make_directed(e1, pug.source(e1)); // arbitrary orientation, used for bends retrieval, in the  final 'dug' all bends should be directed.
           }
         }


     
     if (extFace!=NULL)
         opug= new orth_plan_undi_graph(pug, extFace);   // find a shape minimizing the number of bends with given external face 
     else
         opug= new orth_plan_undi_graph(pug);   // find a shape minimizing the number of bends  

	 /* WARNING: if you want to uncoment this you should rebuild 'border_edges' !!!!
     if (extFace!=NULL)
         {
         gdt::gdtlist<gdtnode> N;  // dummy
         gdt::gdtlist<gdtedge> E;  // dummy
         GDT_face opug_extFace= opug->corresponding_face(extFace,pug);
         opug->replace_bends_with_nodes( opug_extFace, N, E );
         }
     */

     // WARNING: here edges should be set to incide in the middle of the node.

     if (!dir_is_valid)
         {
         // choose an arbitrary orientation of the drawing
         gdtedge e = ug.first_edge();
         gdtnode n = ug.source(e);
         ref_edge= ug.id(e);
         ref_node= ug.id(n); 
         heading_dir= north; 
         dir_is_valid=true;
         }

     
     opug->set_reference_border_step(opug->find_node(ref_node), opug->find_edge(ref_edge));
    
     dug= new draw_undi_graph(*opug, heading_dir);      // compact with default algorithm, cares about rotations

     
     dug->rescale(1); // all the dug's coordinates are rescaled (1 grid unit = 1 real unit)
     
     gdtnode v;
     forall_nodes(v, *dug)
       {
       int id= dug->id(v); 
       dug->set_width (v,node_width[id]);
       dug->set_height(v,node_height[id]);
       }

     // aggiunto da Titto per la connessione del grafo
     gdtedge e;
     forall(e, remove_me) 
       dug->del_edge(dug->corresponding_edge(e,ug));

    result=*dug;
 
};





