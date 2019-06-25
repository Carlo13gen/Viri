/*******************************************************************************
+
+  _rm3_SPQR_tree.c
+
+  This file is part of GDToolkit. It can be 
+  used free of charge in academic research and teaching. 
+  Any direct or indirect commercial use of this software is illegal 
+  without a written authorization of
+  the Dipartimento di Informatica e Automazione
+  Universita' di Roma Tre, Roma, ITALIA
+  
+
*******************************************************************************/

#include <string>


#include <GDT/gdtstack.h>
#include <GDT/gdtqueue.h>
#include <GDT/rm3_global.h>
#include <GDT/rm3_tree.h>
#include <GDT/rm3_dire_graph.h>
#include <GDT/rm3_plan_undi_graph.h>
#include <GDT/rm3_orth_plan_undi_graph.h>
#include <GDT/rm3_upwa_plan_undi_graph.h>
#include <GDT/rm3_SPQR_tree.h>
#include <GDT/gdt_error.h>

using namespace std;


#ifdef __GNUC__
inline ostream& operator <<(ostream& out,const gdt::gdtlist<int>& x)
     {out << *((unsigned int *)((void *)&x)) ;return(out);}
inline istream& operator >>(istream& in,gdt::gdtlist<int>& x)
     {in >> *((unsigned int *)((void *)&x)) ;return(in);}
#endif


//-----------------------------------------------------------------------------
// SPQR_tree: base class for SPQR trees
//
// W.Didimo e A.Leonforte (1996)
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// split_component
//-----------------------------------------------------------------------------
	
	split_component::
~split_component
	()
	{
	local_del();
	}
	

	split_component::
split_component 
	(plan_undi_graph& pug)
	{
	//cout << "\n split_component (plan_undi_graph) creator \n" << flush;
	plan_undi_graph::operator=(pug);
	local_new();
	local_init(pug);
	}
	

	
	split_component&
	split_component::
operator=
	(plan_undi_graph& pug)
	{
	//cout << "\n plan_undi_graph -(copy)-> split_component \n" << flush;
	
	if (!pug.is_biconnected())
		gdt_error("pug is not biconnected");	
	
	plan_undi_graph::operator=(pug);
	local_renew();
	local_init (pug);
	return *this;
	}
	
	
	
	
	// -----------------
	// Access operations
	// -----------------


	void 
	split_component::
local_get
	(plan_undi_graph*& p1, split_component_type& p2, gdt::gdtnode_map<bool>*& p3,
	 gdt::gdtlist<separation_pair>*& p4, gdt::gdtnode_map<struct_split_node_info>*& p5,
	 gdt::gdtedge_map<struct_split_edge_info>*& p6)
	{
	p1 = owner_graph;
	p2 = type;
	p3 = node_is_present;
	p4 = separation_pairs;
	p5 = node_info;
	p6 = edge_info;
	}	



	plan_undi_graph&
	split_component::
get_owner_graph
	() const
	{
	return *owner_graph;
	}
	
	

	split_component_type
	split_component::
get_type
	() const
	{
	return type;
	}
	
	
	
	split_component_edge_type
	split_component::
get_type_of_edge
	(gdtedge e) const
	{
	return (*edge_info)[e].type;
	}
	
	
	
	gdt::gdtlist<separation_pair>
	split_component::
get_separation_pairs 
	() const
	{
	return *separation_pairs;	
	}



	bool
	split_component::
get_node_is_present
	(gdtnode v) const
	{
	return (*node_is_present)[v];
	}

	
	
	bool
	split_component::
contain_separation_pair
	(separation_pair sp) const
	{
	separation_pair spx;
	gdt::gdtlist<separation_pair> sp_list;
	
	sp_list = get_separation_pairs();
	forall (spx,sp_list)
		if (spx == sp) return true;
	return false;	
	}
	
	
	
	gdtnode
	split_component::
get_twin_node
	(gdtnode v) const
	{
	return (*node_info)[v].twin_node;
	}
	
	
	
	gdtedge
	split_component::
get_twin_edge
	(gdtedge e) const
	{
	return (*edge_info)[e].twin_edge;
	}
	
	

	gdtedge
	split_component::
edge_associated_to_owner_graph_edge
	(gdtedge e) const
	{
	gdtedge ex;
	
	forall_edges (ex,*this)
		if (get_twin_edge(ex) == e) return ex;
	return NULL_EDGE;
	}
	
	
	
	split_component*
	split_component::
get_twin_edge_owner_split_component
	(gdtedge e) const
	{
	return (*edge_info)[e].twin_edge_owner_split_component;	
	}
	
	
	
	
	// -----------------
	// Update operations
	// -----------------
	
	void
	split_component::
clear
	()
	{
	plan_undi_graph::clear();
	local_renew ();
	}

	
	
	void
	split_component::
mirror
	(split_component& sc)
	{
	plan_undi_graph* 	p1; 
	split_component_type 	p2; 
	gdt::gdtnode_map<bool>* 	p3;
	gdt::gdtlist<separation_pair>*  p4;
	gdt::gdtnode_map<struct_split_node_info>* p5;
	gdt::gdtedge_map<struct_split_edge_info>* p6;
	
	plan_undi_graph::mirror (sc);
	sc.local_get (p1,p2,p3,p4,p5,p6);
	local_set (p1,p2,p3,p4,p5,p6);
	}



	void
	split_component::
forget
	()
	{
	plan_undi_graph::forget();
	local_new();
	}



	bool
	split_component::
split_on_separation_pair 
	(separation_pair sp, split_component*& sc1_pointer, int new_id)
	{
	// debugging
	//	
	//std::cout << "\nSplitting component " << std::flush;
	//this->print(COMPLETE); 	
	//
	
	int idp1, idp2; // W.D. April 2005
	
	gdtnode v1, w1, v, w, u;
	gdtedge e1, e, ex;
	face f1, g1, g, f;
	gdt::gdtnode_array<bool> marked_node (nodes_and_edges(),false);
	gdt::gdtedge_array<bool> marked_edge (nodes_and_edges(),false);
	
	split_component* 	sc_pointer;
	separation_pair  	sp1,spx;
	gdt::gdtlist<separation_pair>   sp_list, sp1_list, spx_list;
	gdt::gdtlist<face> 		f_list, f1_list;
	gdt::gdtlist<gdtedge>       	visited_edges;
	gdt::gdtlist<gdtedge>		edges_list;
	
	gdt::gdtlist<int> sp_poles_id; // two consecutive int represent the poles of a separation pair
	
	bool flag;
		
	if (contain_separation_pair(sp))
		if (get_type() == MAXIMAL_BOND) return false;
		else
			{
			// a splitting of *this is executed on the sp 
			// separation pair. 
			//
			// -------------------------- 0 ----------------------------
			// construct a list of separation pair poles identifiers
			// W.D. April 2005
			// ---------------------------------------------------------  
			
			sp_list  = get_separation_pairs ();
			forall (spx,sp_list)
				{
				v = separation_pair_pole1 (spx);
				w = separation_pair_pole2 (spx);
				idp1 = id(v);
				idp2 = id(w); 
				sp_poles_id.append(idp1);
				sp_poles_id.append(idp2);
				}
			
			
			// -------------------------- 1 ----------------------------
			// mark the separation pair poles
			// ---------------------------------------------------------
			
			v = separation_pair_pole1 (sp);
			w = separation_pair_pole2 (sp);
			marked_node[v] = true;
			marked_node[w] = true;
			
			// -------------------------- 2 ----------------------------
			// search an gdtedge e adjacent to v, such that e != (v,w) 
			// ---------------------------------------------------------
			
			e = first_adj_edge (v);
			while (opposite (v,e) == w)  e = adj_succ(e,v);
			
			// -------------------------- 3 ----------------------------
			// execute a visit starting from opposite(v1,e): all visited
			// edges will be marked.
			// ---------------------------------------------------------
			
			u = opposite (v,e);
			visit_from_node_through_not_marked_nodes (u,marked_node,visited_edges);
			forall (e,visited_edges) marked_edge[e] = true;
			
			// -------------------------- 4 ----------------------------
			// make a new split component sc1, initializing it with
			// a copy of sc.
			// Successively, a new gdtedge (v,w) is added in sc1 and *this by
			// splitting a face that contains the separation pair.
			// The new edges are made virtual and will be twin edges, with
			// label new_id.
			// Finally, all the not marked edges are deleted in sc1 and 
			// all the marked edges are deleted in *this.  
			// ---------------------------------------------------------
			
			sc1_pointer = new split_component(*this);
			//cout << "\nCREATED A NEW SPLIT COMPONENT POINTER = " << sc1_pointer << flush;
			
			f_list = faces_shared_by_separation_pair (sp);
			if (!f_list.empty()) f  = f_list.head();
			else		     f = NULL_FACE;
			
			//cout << "\nCHOOSED FACE f" << id(f) << " FOR SPLITTING" << flush; 
			v1 = (*sc1_pointer).find_node (id(v));
			w1 = (*sc1_pointer).find_node (id(w));
			f1 = (*sc1_pointer).find_face (id(f));
			
			e1 = (*sc1_pointer).new_edge (v1,w1,f1,new_id);
			e  = new_edge (v,w,f,new_id);
			
			(*sc1_pointer).set_type_of_edge (e1,VIRTUAL);
			(*sc1_pointer).set_twin_edge (e1,e);
			(*sc1_pointer).set_twin_edge_owner_split_component (e1,this);

			set_type_of_edge (e,VIRTUAL);
			set_twin_edge (e,e1);
			set_twin_edge_owner_split_component (e,sc1_pointer);
			
			edges_list.clear();
			forall_edges (ex,*this) edges_list.append(ex);

			forall (ex,edges_list)
				if (ex != e)
					if (!marked_edge[ex])
						{
						// delete gdtedge in sc1
						//
						//cout << "\nDELETING EDGE " << id(ex) << " INTO sc1" << flush;  
						e1 = (*sc1_pointer).find_edge(id(ex));
						(*sc1_pointer).del_edge(e1);
						//cout << "\nEDGE DELETING" << flush;
						}
					else 
						{
						// delete gdtedge in *this
						//
						//cout << "\nDELETING EDGE " << id(ex) << " INTO sc" << flush;
						del_edge(ex);
						//cout << "\nEDGE DELETING" << flush;
						}
			//cout << "\nDELETING EDGES TERMINATED" << flush;
			//print (COMPLETE, false);
			//(*sc1_pointer).print(COMPLETE, cout, false);
			
			// -------------------------- 5 ----------------------------
			// forall virtual edges e into sc1 and *this, update the 
			// twin_edge(twin_edge(e)) = e. 
			// Therefore update the node_is_present arrays.  
			// ---------------------------------------------------------
			
			forall_edges (e1,(*sc1_pointer))
				if ((*sc1_pointer).get_type_of_edge(e1) == VIRTUAL)
					{
					ex = (*sc1_pointer).get_twin_edge(e1);
					sc_pointer = (*sc1_pointer).get_twin_edge_owner_split_component(e1);
					(*sc_pointer).set_twin_edge (ex,e1);
					(*sc_pointer).set_twin_edge_owner_split_component (ex,sc1_pointer);
					}
					
			//cout << "\nUPDATED TWIN EDGES FOR sc1" << flush;
			
			forall_edges (e,*this)
				if (get_type_of_edge(e) == VIRTUAL) 
					{
					ex = get_twin_edge(e);
					sc_pointer = get_twin_edge_owner_split_component(e);
					(*sc_pointer).set_twin_edge (ex,e);
					(*sc_pointer).set_twin_edge_owner_split_component (ex,this);
					}
					
			//cout << "\nUPDATED TWIN EDGES FOR sc" << flush;
					
			forall_nodes (v1,(*sc1_pointer))
					if (!((id(separation_pair_pole1(sp)) == (*sc1_pointer).id(v1)) ||
					     (id(separation_pair_pole2(sp)) == (*sc1_pointer).id(v1))))
					   set_node_is_present((*sc1_pointer).get_twin_node(v1),false);
					   
			//cout << "\nUPDATED PRESENT NODES FOR sc1" << flush;
					
			forall_nodes (v,*this)
					if (!((id(separation_pair_pole1(sp)) == id(v)) ||
					     (id(separation_pair_pole2(sp)) == id(v))))
					   (*sc1_pointer).set_node_is_present(get_twin_node(v),false);
					   
			//cout << "\nUPDATED PRESENT NODES FOR sc" << flush;
			
			
			// -------------------------- 6 ----------------------------
			// update the separation_pairs list (and the corresponding 
			// information) in sc1 and *this. Finally, update the sc1
			// and *this type. 
			// ---------------------------------------------------------
			
			//sp_list  = get_separation_pairs (); W.D. April 2005			 
						
			sp1_list.clear();
			spx_list.clear();
			
			// debugging
			//   {
			//   std::cout << "\n\t Sub-Split component sc1: " << std::flush;
			//   sc1_pointer->print(COMPLETE);
			//   }
			//
			 
			//forall (sp,sp_list) W.D. April 2005
			
			list_item it1, it2;
			it1 = sp_poles_id.first_item();
						
			while (it1)
				{
				it2 = sp_poles_id.succ(it1);
				idp1 = sp_poles_id.inf(it1);
				idp2 = sp_poles_id.inf(it2);
				
				// W.D. April 2005
				//v = separation_pair_pole1 (sp);
				//w = separation_pair_pole2 (sp);
				
				// debugging
				//
				//  {
				//  std::cout << "\nchecking separation pair " << std::flush;
				//  std::cout << "(" << idp1 << "," << idp2 << ")" << std::flush;
				//  std::cout << " in sc1" << std::flush;
				//  }
				//
								
				// ---------------------------------------
				// check if sp is a separation pair in sc1
				// ---------------------------------------
				//
				// W.D. April 2005
				//if (((*sc1_pointer).get_node_is_present(get_twin_node(v))) && 
				//   ((*sc1_pointer).get_node_is_present(get_twin_node(w))))
				
				v1 = (*sc1_pointer).find_node (idp1);
				w1 = (*sc1_pointer).find_node (idp2);							
								
				if (v1 && w1)
					{
					// the separation pair poles are present in sc1.
					//
					//v1 = (*sc1_pointer).find_node (id(v));
					//w1 = (*sc1_pointer).find_node (id(w));							
						
					gdt::gdtmap<face,bool> face_already_insert (false);
					gdt::gdtmap<face,bool> v1_in_face (false);
					gdt::gdtmap<face,bool> w1_in_face (false);
					gdt::gdtlist<face>     v1_faces;
					gdt::gdtlist<face>     w1_faces;
					
					forall_faces (f1,(*sc1_pointer))
						{
						// init W.D. aprile 2005
						face_already_insert[f1]=false;
						v1_in_face[f1]=false;
						w1_in_face[f1]=false;
						
					        if ((*sc1_pointer).node_belongs_to_face (v1,f1))
					        	{
					        	v1_in_face[f1] = true;
					        	v1_faces.append(f1);
					        	}
					        if ((*sc1_pointer).node_belongs_to_face (w1,f1))
					        	{
					        	w1_in_face[f1] = true;
					        	w1_faces.append(f1);
					        	}
						}			
					
					f1_list.clear();
					forall (f1,v1_faces)
						forall (g1,w1_faces)
							if ((*sc1_pointer).id(f1) > (*sc1_pointer).id(g1) && w1_in_face[f1] && v1_in_face[g1])
							    {
							    flag = true;
							    forall_edges_adjacent_node (e1,v1,*sc1_pointer) 
						            	if (((*sc1_pointer).opposite(v1,e1) == w1) &&
							           ((((*sc1_pointer).right_face_moving_along_edge(e1,v1) == f1) && 
								   ((*sc1_pointer).left_face_moving_along_edge(e1,v1)    == g1)) ||
								   (((*sc1_pointer).right_face_moving_along_edge(e1,v1)  == g1) && 
								   ((*sc1_pointer).left_face_moving_along_edge(e1,v1)    == f1))))
								   	{
								   	flag = false;
								     	break;
								     	}
							    if (flag)
						            	{
								if (!face_already_insert[f1]) 
									{
								     	f1_list.append(f1);
								     	face_already_insert[f1] = true;
								     	}
								if (!face_already_insert[g1]) 
								     	{
								     	f1_list.append(g1);
								     	face_already_insert[g1] = true;
								     	}
								}
							    }
							    
					if (!f1_list.empty())
						{
						// sp is a separation pair in sc1
						//
						//debugging
						//std::cout << "\n...is a separation pair" << std::flush;
						//
						sp1 = new (struct_separation_pair);
						(*sc1_pointer).set_faces_shared_by_separation_pair (f1_list,sp1);
						(*sc1_pointer).set_separation_pair_pole1 (v1,sp1);
						(*sc1_pointer).set_separation_pair_pole2 (w1,sp1);
						sp1_list.append(sp1);
						}
					// debugging
					//else std::cout << "\n...is not a separation pair" << std::flush;		
					//
					}
				//debugging
				//else std::cout << "\npoles not present" << std::flush;
																
				// -----------------------------------------
				// check if sp is a separation pair in *this
				// -----------------------------------------
				//
				v = find_node(idp1);
				w = find_node(idp2);
				
				//if ((get_node_is_present(get_twin_node(v))) && 
				//   (get_node_is_present(get_twin_node(w))))
				if (v && w)
					{
					// the separation pair poles are present in *this.
					//
					gdt::gdtmap<face,bool> face_already_insert (false);
					gdt::gdtmap<face,bool> v_in_face (false);
					gdt::gdtmap<face,bool> w_in_face (false);
					gdt::gdtlist<face>     v_faces;
					gdt::gdtlist<face>     w_faces;
										
					forall_faces (f,*this)
						{
						// init W.D. aprile 2005
						face_already_insert[f]=false;
						v_in_face[f]=false;
						w_in_face[f]=false;
						
					        if (node_belongs_to_face (v,f))
					        	{
					        	v_in_face[f] = true;
					        	v_faces.append(f);
					        	}
					        if (node_belongs_to_face (w,f))
					        	{
					        	w_in_face[f] = true;
					        	w_faces.append(f);
					        	}
						}	
								
					f_list.clear();
					forall (f,v_faces)
						forall (g,w_faces)
							if (id(f) > id(g) && w_in_face[f] && v_in_face[g])
							    {
							    flag = true;
							    forall_edges_adjacent_node (e,v,*this) 
						            	if ((opposite(v,e) == w) &&
							           (((right_face_moving_along_edge(e,v) == f)  && 
								   (left_face_moving_along_edge(e,v)    == g)) ||
								   ((right_face_moving_along_edge(e,v)  == g)  && 
								   (left_face_moving_along_edge(e,v)    == f))))
								   	{
								   	flag = false;
								     	break;
								     	}
							    if (flag)
						            	{
								if (!face_already_insert[f]) 
									{
								     	f_list.append(f);
								     	face_already_insert[f] = true;
								     	}
								if (!face_already_insert[g]) 
								     	{
								     	f_list.append(g);
								     	face_already_insert[g] = true;
								     	}
								}
							    }
							    
					if (!f_list.empty())
						{
						// sp is a separation pair in *this
						//
						spx = new (struct_separation_pair);
						set_faces_shared_by_separation_pair (f_list,spx);
						set_separation_pair_pole1 (v,spx);
						set_separation_pair_pole2 (w,spx);
						spx_list.append(spx);
						}							    
					}
				it1 = sp_poles_id.succ(it2);				
				}
				
			(*sc1_pointer).set_separation_pairs (sp1_list);
			set_separation_pairs (spx_list);
			
			//cout << "\nUPDATED SEPARATION PAIRS" << flush;
								
			// -------------------------- 7 ----------------------------
			// update the split_component type for sc1 and *this
			// ---------------------------------------------------------
			
			// check the sc1 type
			//
			if (((*sc1_pointer).number_of_nodes() > 2) && ((*sc1_pointer).number_of_faces() == 2)) 
				(*sc1_pointer).set_type (POLYGON);
			else if (((*sc1_pointer).number_of_nodes() == 2) && ((*sc1_pointer).number_of_faces() > 1)) 
					(*sc1_pointer).set_type (MAXIMAL_BOND);
			     else if (sp1_list.empty())
		     			(*sc1_pointer).set_type (TRICONNECTED);
				  else 
					(*sc1_pointer).set_type (NOT_COMPLETE);

			// check the *this type
			//
			if ((number_of_nodes() > 2) && (number_of_faces() == 2)) 
				set_type (POLYGON);
			else if ((number_of_nodes() == 2) && (number_of_faces() > 1)) 
					set_type (MAXIMAL_BOND);
			     else if (spx_list.empty())
		     			set_type (TRICONNECTED);
				  else 
					set_type (NOT_COMPLETE);
					
			//cout << "\nUPDATED TYPES" << flush;
			
			//cout << "\nFIRST SPLIT COMPONENT:";
			//print(COMPLETE);
			//cout << "\n" << flush;
			//cout << "\nSECOND SPLIT COMPONENT:";
			//(*sc1_pointer).print(COMPLETE);
			//cout << "\n" << flush;
			
			///////////////////////////////////
			//while(!sp1_list.empty()) delete (sp1_list.pop_front());
			return true;
			}
	else
		gdt_error("sp is not a separation pair in this split component");
	return true; 
	}
	
	
	
	bool
	split_component::
merge_with_polygon_on_edge
	(split_component*& sc1p, gdtedge e)
	{
	// merge the polygon *this with the polygon sc1, on a common virtual gdtedge e.
	// NOTE: if *this or sc1 are not polygons, or if e is not a common virtual gdtedge
	//	 the function returns false. If the merging is executed, it returns true
	//	 and sc1_pointer becomes NULL;
	//
	gdtnode v,w,v1,w1,u,u1;
	gdtedge ex,e1x,e2x,e1;
	gdt::gdtlist<gdtedge> sc1_edges;
	gdt::gdtlist<separation_pair> sp_list;
	split_component* sc2p;
	
	if ((get_type() != POLYGON) || ((*sc1p).get_type() != POLYGON)) return false;
	if (get_twin_edge_owner_split_component(e) != sc1p) return false;

	// starting merge
	//
	v   = source(e);
	w   = target(e);
	
	e1  = get_twin_edge(e);
	if ((*sc1p).get_twin_node((*sc1p).source(e1)) == get_twin_node(v)) v1 = (*sc1p).source(e1);
	else			     					   v1 = (*sc1p).target(e1);
	w1 = (*sc1p).opposite(v1,e1);
	
	//cout << "\nMERGING POLYGONS:";
	//print(COMPLETE,false);
	//(*sc1p).print(COMPLETE,false);
		
	
	/*		sc (*this)
	         O----O----O-----O			 sc (*this)
	        /		  \               O----O----O-----O
	        \        e        /              /                 \
	       v O---------------O w             \                 /
	 		 |               ---->    O               O
	                 |                       /                 \
	      v1 O---------------O w1            \                 /
	        /        e1       \               O---------------O
	        \                 /
	         O----O----O-----O            
	                sc1
	*/
	
	e1x = (*sc1p).cyclic_adj_succ(e1,w1);
	u1  = w1;
	
	while (e1x != e1)
		{
		u1 = (*sc1p).opposite (u1,e1x);
		if (u1 != v1)
			{
			// split the gdtedge e with a new gdtnode u.
			//
			u  = new_node(e,(*sc1p).id(u1));
			mark(u,sc1p->markers(u1));		// update gdtnode-markers
			
			set_twin_node (u,(*sc1p).get_twin_node(u1));
			set_node_is_present(get_twin_node(u),true);
			ex = first_adj_edge(u);
			if (opposite(u,ex) == v)  ex = cyclic_adj_succ(ex,u);
			e  = cyclic_adj_succ(ex,u);
			if      ((*sc1p).start(e1x) == u1) make_directed(ex,u);
			else if ((*sc1p).stop (e1x) == u1) make_directed(ex,opposite(u,ex));
			}
		else	
			{
			ex = e;
			if 	((*sc1p).start(e1x) == v1) make_directed(ex,v);
			else if ((*sc1p).stop (e1x) == v1) make_directed(ex,opposite(v,ex));
			}
		
		// update the ex gdtedge info and twin_edge(ex) gdtedge info
		//
		if ((ex != e) && ((id(e) == (*sc1p).id(e1x)))) assign_id (e,id(e)+2);
		assign_id (ex,(*sc1p).id(e1x));
		mark      (ex,(*sc1p).markers(e1x));	// update gdtedge-markers
		
		e2x  = (*sc1p).get_twin_edge(e1x);
		sc2p = (*sc1p).get_twin_edge_owner_split_component(e1x);
		set_type_of_edge (ex,(*sc1p).get_type_of_edge(e1x));
		set_twin_edge (ex,e2x);
		set_twin_edge_owner_split_component (ex,sc2p);
		if (get_type_of_edge(ex) == VIRTUAL)
			{
			(*sc2p).set_twin_edge (e2x,ex);
			(*sc2p).set_twin_edge_owner_split_component (e2x,this);
			}
		e1x = (*sc1p).cyclic_adj_succ (e1x,u1);
		}

	// set the separation pairs
	//
	sp_list = find_separation_pairs();
	set_separation_pairs (sp_list);
		
	// delete the split component sc1p
	//
	safe_delete (sc1p);	// W.D. March 1999
	//sc1p = NULL;		W.D. March 1999
	
	//cout << "\nPOLYGON RESULTING: " << flush;
	//print(COMPLETE,false);
	
	return true;
	}
	
	
	
	gdt::gdtlist<split_component*>
	split_component::
decompose_into_triconnected_components
	()
	{
	// return the triconnected components list of this and make sc
	// empty.
	// WARNING: the decomposition is executed on a dynamic copy of this,
	//          because the triconnected component list must contain
	//	    only dynamic split_components.
	
	gdt::gdtlist<split_component*> scp_list;
	split_component* scp = new split_component(*this);
	
	// ------------- 0 --------------
	// suspend all constraints on scp
	// ------------------------------
	
	scp -> del_all_constraints();

	// ------------- 1 --------------
	// recursively decompose scp 
	// ------------------------------
	
	scp_list = scp -> decomposition_in_triconnected_components();
	
	// ------------- 2 --------------
	// restore all constraints, as 
	// much as possible, on each 
	// triconnected component, except
	// for SAME_FACE type constraints
	// ------------------------------
	
	constraint c;
	forall (scp,scp_list)
		forall_constraints (c,*this)
			{
			if (type_of_constraint(c) != SAME_FACE_NODES ||
			    type_of_constraint(c) != SAME_FACE_ORDERED_NODES)
				scp -> new_constraint(c);
			}
	
	
	// ------------- 3 --------------
	// check for constr. translations
	// ------------------------------
	
	//forall (scp,scp_list)
	//	{
	//	std::cout << "\n\nSPLT_COMPONENT:\n" << std::flush;
	//	scp -> print (COMPLETE);
	//	std::cout << "\n" << std::flush; 				
	//	}
	
	clear();	
	return scp_list;
	}
	


	
//-----------------------------------------------------------------------------
// Input / output operations
//-----------------------------------------------------------------------------

	void
	split_component::
print
	(print_mode mode, bool split_component_information, ostream& os) 
	{
	gdtnode v;
	gdtedge e;
	separation_pair sp;
	plan_undi_graph* pugp = &get_owner_graph();
	
	plan_undi_graph::print (mode,os);
	
	if (split_component_information)
		{
		os << "\nSPLIT COMPONENT INFORMATION\n" << flush;
		
		os << "\nTYPE = ";
		if (get_type() == POLYGON)      os << "POLYGON"      << flush;
		if (get_type() == MAXIMAL_BOND) os << "MAXIMAL_BOND" << flush;
		if (get_type() == TRICONNECTED) os << "TRICONNECTED" << flush;
		if (get_type() == NOT_COMPLETE) os << "NOT_COMPLETE" << flush;

		os << "\nPRESENT ORIGINAL NODES = ";
		forall_nodes (v,*pugp)
			if (get_node_is_present(v)) os << (*pugp).id(v) << " ";
		os << flush;

		forall_nodes (v,*this)
			os << "\nNODE " << id(v) << "  TWIN NODES = " << (*pugp).id(get_twin_node(v)) << flush;

		forall_edges (e,*this)
			{
			os << "\nEDGE " << id(e) << " TYPE = ";
			if (get_type_of_edge (e) == REAL)    os << "REAL";
			if (get_type_of_edge (e) == VIRTUAL) os << "VIRTUAL";
			}
		os << flush;

		os << "\n SEPARATION PAIRS:\n";
		gdt::gdtlist<separation_pair> SP=get_separation_pairs();
		forall (sp,SP)
			{
			plan_undi_graph::print (sp);
			os << "\n";
			}
		os << flush;
		}
	}


//-----------------------------------------------------------------------------
// PROTECTED
//-----------------------------------------------------------------------------
	
	
	split_component::
split_component
	()
	{
	//cout << "\n split_component () creator \n" << flush;
	local_new();
	}



	split_component::
split_component 
	(const split_component& sc)
	{
	//cout << "\n split_component (split_component) creator \n" << flush;
	local_new();  // required because operator= calls renew
	operator=(sc);
	}
	
	
	
	split_component&
	split_component::
operator=
	(const split_component& sc)
	{
	//cout << "\n split_component -(copy)-> split_component \n" << flush;
	
	gdtnode v, vc, v1, v2, v1c, v2c;
	gdtedge e, ec;
	face f, fc;
	separation_pair sp, spc;
	gdt::list_item  sp_pos;
	gdt::gdtlist<separation_pair> spc_list, sp_list;
	gdt::gdtlist<face> f_list, fc_list;
	
	
	plan_undi_graph::operator=(sc);
	local_renew();
	
	// copy the split_component part
	//
	set_owner_graph (sc.get_owner_graph());
	node_is_present->init ((*owner_graph).nodes_and_edges(), false);
	
	set_type  	(sc.get_type());
	   
	forall_nodes (v,sc.get_owner_graph()) 
		set_node_is_present(v,sc.get_node_is_present(v));
	
	sp_list = sc.get_separation_pairs();
	sp_pos  = sp_list.first();
	while (sp_pos)
		{
		sp  = sp_list.contents (sp_pos);
		spc = new (struct_separation_pair);
		v1  = sc.separation_pair_pole1(sp);
		v1c = find_node(sc.id(v1));
		v2  = sc.separation_pair_pole2(sp);
		v2c = find_node(sc.id(v2));
		set_separation_pair_pole1 (v1c,spc);
		set_separation_pair_pole2 (v2c,spc);
		
		f_list = sc.faces_shared_by_separation_pair(sp);
		fc_list.clear();
		
		forall (f,f_list)
			{
			fc = find_face (sc.id(f));
			fc_list.append (fc);
			}
		set_faces_shared_by_separation_pair (fc_list,spc);
		spc_list.append (spc);
		sp_pos = sp_list.succ (sp_pos);
		}
	set_separation_pairs (spc_list);
	 
	forall_nodes (v,sc)
		{
		vc = find_node (sc.id(v));
		set_twin_node (vc,sc.get_twin_node(v));
		}
	forall_edges (e,sc)
		{
		ec = find_edge (sc.id(e));
		set_type_of_edge  (ec,sc.get_type_of_edge(e));	
		set_twin_edge  (ec,sc.get_twin_edge(e));
		set_twin_edge_owner_split_component (ec,sc.get_twin_edge_owner_split_component(e));
		}
		
	return (*this);
	}
	
	
	
	gdtnode 
	split_component::
new_node 
	(gdtedge e, int new_id) 
	{
	return plan_undi_graph::new_node (e,new_id); 
	}
	
	
	
	gdtedge 
	split_component::
new_edge 
	(gdtnode v, gdtnode w, face f, int new_id) 
	{
	return plan_undi_graph::new_edge (v,w,f,new_id);
	}



	void 
	split_component::
del_node 
	(gdtnode v)
	{
	plan_undi_graph::del_node (v);
	}
	
	
	
	void
	split_component:: 
del_edge 
	(gdtedge e)
	{
		plan_undi_graph::del_edge (e);
		//Added by Marcandalli 8 2001
		edge_info->undefine(e);
		//
	}

		
	
//-----------------------------------------------------------------------------
// PRIVATE
//-----------------------------------------------------------------------------


	void
	split_component::
local_new
	()
	{
	//cout << "\n split_component::local_new \n" << flush;
	
	node_is_present = new (gdt::gdtnode_map<bool>);
	separation_pairs= new (gdt::gdtlist<separation_pair>);
	node_info	= new (gdt::gdtnode_map<struct_split_node_info>);
	edge_info	= new (gdt::gdtedge_map<struct_split_edge_info>);
	
	node_info->init (nodes_and_edges());
	edge_info->init (nodes_and_edges());
	}
	
	
	
	void
	split_component::
local_del
	()
	{
	//cout << "\n split_component::local_del \n" << flush;
	//cout << "of component pointer = " << this << flush;
	while (!separation_pairs->empty()) delete (separation_pairs->pop_front());
	
	safe_delete (node_is_present);
	safe_delete (separation_pairs);
	safe_delete (node_info);
	safe_delete (edge_info);
	}
	
	
	
	void
	split_component::
local_renew
	()
	{
	local_del();
	local_new();
	}
	
	
	
	void
	split_component::
local_init
	(plan_undi_graph& pug)
	{
	gdtnode v, vc, v1, v2, v1c, v2c;
	gdtedge e, ec;
	face f, fc;
	separation_pair sp, spc;
	gdt::list_item  sp_pos;
	gdt::gdtlist<separation_pair> sp_list, spc_list;
	gdt::gdtlist<face> f_list, fc_list;
	
	// set the split_component part
	//
	set_owner_graph (pug);
	node_is_present->init ((*owner_graph).nodes_and_edges(), false);
	
	sp_list = pug.find_separation_pairs ();
	
	if ((pug.number_of_nodes() > 2) && (pug.number_of_faces() == 2)) 
		set_type (POLYGON);
	else if ((pug.number_of_nodes() == 2) && (pug.number_of_faces() > 1)) 
		set_type (MAXIMAL_BOND);
	     else if (sp_list.empty())
		     	set_type (TRICONNECTED);
		  else 
			set_type (NOT_COMPLETE);
	
	forall_nodes (v,pug)
		{ 
		set_node_is_present(v,true);
		vc = find_node (pug.id(v));
		set_twin_node (vc,v);
		}
	
	sp_pos = sp_list.first();
	while (sp_pos) 
		{
		sp  = sp_list.contents(sp_pos);
		spc = new (struct_separation_pair);
		
		v1  = pug.separation_pair_pole1(sp);
		v1c = find_node (pug.id(v1));
		v2  = pug.separation_pair_pole2(sp);
		v2c = find_node (pug.id(v2));
		
		set_separation_pair_pole1 (v1c,spc);
		set_separation_pair_pole2 (v2c,spc);
		
		f_list = pug.faces_shared_by_separation_pair(sp);
		fc_list.clear();
		
		forall (f,f_list)
			{
			fc = find_face (pug.id(f));
			fc_list.append (fc);
			}
		set_faces_shared_by_separation_pair (fc_list,spc);
		spc_list.append (spc);
		sp_pos = sp_list.succ (sp_pos);
		}
	set_separation_pairs (spc_list);
			
	forall_edges (e,pug)
		{
		ec = find_edge (pug.id(e));
		set_type_of_edge  (ec,REAL);	
		set_twin_edge  (ec,e);
		set_twin_edge_owner_split_component (ec,NULL);
		}
	///////////////////////////
	while (!sp_list.empty()) delete (sp_list.pop_front());
	///////////////////////////
	}
	
	
	
	void
	split_component::
local_set
	(plan_undi_graph* p1, split_component_type p2, gdt::gdtnode_map<bool>* p3,
	 gdt::gdtlist<separation_pair>* p4, gdt::gdtnode_map<struct_split_node_info>* p5,
	 gdt::gdtedge_map<struct_split_edge_info>* p6)
	 {
	 
	 //Added by Pier Francesco Cortese 29/09/2006
	 delete node_is_present;
	 while (!separation_pairs->empty()) delete (separation_pairs->pop_front());
	 delete separation_pairs;
	 delete node_info;
	 delete edge_info;
	 /////////////////
	 owner_graph = p1;
	 type	     = p2;
	 node_is_present = p3;
	 separation_pairs = p4;
	 node_info = p5;
	 edge_info = p6;
	 }
	
	
	
	void
	split_component::
set_owner_graph      
	(plan_undi_graph& pug)
	{
	owner_graph = &pug;
	}					



	void
	split_component::
set_type
	(split_component_type t)
	{
	type = t;
	}
	
	
	
	void
	split_component::
set_node_is_present
	(gdtnode v, bool flag)
	{
	(*node_is_present)[v] = flag;
	}
	
	
	
	void
	split_component::
set_separation_pairs
	(const gdt::gdtlist<separation_pair>& sp_list)
	{
	separation_pair sp;
	
	forall (sp,*separation_pairs) safe_delete(sp);
	(*separation_pairs).clear();
	forall (sp,sp_list)
		(*separation_pairs).append (sp);
	}
	
	
	
	void
	split_component::
set_twin_node
	(gdtnode v, gdtnode v1)
	{
	(*node_info)[v].twin_node = v1;
	}
	
	
	
	void
	split_component::
set_twin_edge
	(gdtedge e, gdtedge e1)
	{
	(*edge_info)[e].twin_edge = e1;
	}
	
	
	
	void
	split_component::
set_type_of_edge
	(gdtedge e, split_component_edge_type t)
	{
	(*edge_info)[e].type = t;
	}
	
	
	
	void
	split_component::
set_twin_edge_owner_split_component
	(gdtedge e, split_component* sc_pointer)
	{
	(*edge_info)[e].twin_edge_owner_split_component = sc_pointer;
	}
	
	
	
	gdt::gdtlist<split_component*>
	split_component::
decomposition_in_triconnected_components
	()
	{
	// return the list of pointers to all triconnected components of *this.
	// NOTE: all the added virtual edges have increasing labels starting
	//	 from the last gdtedge label in *this.
	//  
	bool found_virtual_edge;
	gdtedge e;
	separation_pair sp = NULL;
	gdt::gdtlist<separation_pair> sp_list;
	split_component *scp, *sc1p;
	gdt::gdtstack<split_component*> S;
	gdt::gdtlist<split_component*> polygons_list;
	gdt::gdtlist<split_component*> result;
	gdt::list_item pos;
	int count;
	
	count = generate_edge_id();
		
	S.push(this);
	
	// debugging
	//
	//	{
	//	this->print(COMPLETE);
	//	}
	//
	
	// ----------------------------------------------------------
	// decompose (recursively) scp into complete split components 
	// ----------------------------------------------------------
	
	while (!S.empty())
		{
		scp = S.pop();
		if (scp->get_type() == NOT_COMPLETE)
			{
			sp_list = scp->get_separation_pairs();
			if (!sp_list.empty()) sp = sp_list.head();
			scp->split_on_separation_pair (sp,sc1p,count);
			// debugging
			//
			//	{
			//	std::cout << "\n\n\nNEW SPLITTING:" << std::flush;
			//	std::cout << "\n\nSplit component 1:" << std::flush;
			//	scp->print(COMPLETE);
			//	std::cout << "\n\nSplit component 2:" << std::flush;
			//	sc1p->print(COMPLETE);
			//	int i;
			//	std::cin >> i;
			//	}			
			//
			S.push(scp);
			S.push(sc1p);
			count ++;
			}
		else 
			{
			if ((*scp).get_type() == POLYGON) polygons_list.append(scp);
			else result.append(scp);
			//std::cout << "\nFOUND A COMPLETE SPLIT COMPONENT" << std::flush;
			//(*scp).print(COMPLETE);
			//int i;
			//std::cin >> i;			
			}
		}
	
	//cout << "\nDECOMPOSITION IN SPLIT COMPONENTS TERMINATED" << flush; // FOUNDAMENTAL POINT CHECK
	//cin  >> i;
	
	// ---------------------------------------------------------------
	// merge (recursively) all the polygons with a common virtual gdtedge
	// ---------------------------------------------------------------
	
	while (!polygons_list.empty())	
		{
		scp = polygons_list.head();
		gdt::gdtedge_map<bool> marked_edge ((*scp).nodes_and_edges(),false);
		found_virtual_edge = true;
		//cout << "\nEXAMINING POLYGON\n"; scp->print(); cout << "\n" << flush; 
		
		while (found_virtual_edge)
			{
			found_virtual_edge = false;
			forall_edges(e,*scp)
				if (((*scp).get_type_of_edge(e) == VIRTUAL) && (!marked_edge[e])) 
					{
					found_virtual_edge = true;
 					break;
 					}
 			if (found_virtual_edge)
 				{
 				marked_edge[e] = true;
 				//cout << "\nFOUND EDGE " << scp->id(e) << flush;
 				sc1p = (*scp).get_twin_edge_owner_split_component(e);
 				if ((*sc1p).get_type() == POLYGON)
 					{
 					// delete scp and sc1p from polygons_list, 
 					// merge sc with sc1 and put scp to head 
 					// of the polygons_list. 
 					//
 					pos = polygons_list.first();
 					polygons_list.del_item(pos);
 					pos = polygons_list.first();
 					while (polygons_list.contents(pos) != sc1p) 
 						pos = polygons_list.succ(pos);
 					polygons_list.del_item(pos);
 					
 					//cout << "\nMERGING\n"; scp -> print(); cout << flush;
 					//cout << "\nWITH\n"; sc1p -> print();   cout << flush;
 					
 					(*scp).merge_with_polygon_on_edge (sc1p,e);
 					
 					//cout << "\nMERGED TWO POLYGONS INTO";  
 					//scp->print(); 
 					//cout << flush;
 					
 					polygons_list.push(scp);
 					forall_edges (e,*scp) marked_edge[e] = false;
 					}
 				}
 			}

 		// store the complete polygon into result list
 		//
 		pos = polygons_list.first();
 		result.append(polygons_list.contents(pos));
 		polygons_list.del_item(pos);
		}
		
	//cout << "\nMERGING TERMINATED" << flush; // FOUNDAMENTAL POINT CHECK
	//cin  >> i;
	
	return result;
	}
	
	
	
	
//-----------------------------------------------------------------------------
// skeleton
//-----------------------------------------------------------------------------


	skeleton::
~skeleton
	()
	{
	local_del();
	}
	
	
	
	skeleton::
skeleton 
	()
	{
	//cout << "\n skeleton () creator \n" << flush;
	local_new();
	}

	
	
	skeleton::
skeleton
	(const split_component& sc, gdtedge e)
	{
	//cout << "\n skeleton (split_component) creator \n" << flush;
	local_new();
	split_component::operator=(sc);
	local_init (sc,e);
	}



	// -----------------
	// Access operations
	// -----------------

	
	gdtnode 
	skeleton::
get_pole1		
	() const
	{
	gdtedge e = get_reference_edge();
	if (id(source(e)) < id(target(e))) return source(e);
	else				   return target(e);
	}
	
		
	
	gdtnode 
	skeleton::
get_pole2
	() const
	{
	gdtedge e = get_reference_edge();
	if (id(source(e)) > id(target(e))) return source(e);
	else				   return target(e);
	}	
	
	
	
	gdtedge 
	skeleton::
get_reference_edge	
	() const
	{
	return ref_edge;
	}	



	// -----------------
	// Update operations
	// -----------------
	
	
	void
	skeleton::
clear
	()
	{
	split_component::clear();
	local_renew();
	}



	void
	skeleton::
steal_from
	(split_component& sc, gdtedge es)
	{
	//cout << "\n split_component& -(move)-> skeleton\n" << flush;

	gdtedge e,ex;
	split_component* scxp; 
	
	clear ();
	//
	// Copy all the pointers of sc
	// that is, the sc part of this class
	//
	split_component::mirror (sc);
	//
	// Create a new empty local_environment for sc
	// in order to avoid side_effects with this
	//
	sc.forget();
	//
	// Inizialize the extra part of this, according
	// to the tree part stolen from sc
	//
	local_renew ();
	set_reference_edge (es);
	
	//cout << "\nSKELETON COPY:";
	//print (COMPLETE);
	//cin.get();
	
	//
	// update the gdtedge pointers of the twin edges forall virtual edges
	//
	forall_edges (e,*this)
		if (get_type_of_edge(e) == VIRTUAL)
			{
			ex   = get_twin_edge(e);
			scxp = get_twin_edge_owner_split_component(e);
			scxp->set_twin_edge_owner_split_component(ex,this);
			}
	}
	

	
	void
	skeleton::
set_reference_edge
	(gdtedge e)
	{
	ref_edge = e;
	}



	// --------------------------
	// Input / Output opearations
	// --------------------------
	
	void
	skeleton::
print
	(print_mode mode, bool split_component_information, ostream& os) 
	{
	split_component::print (mode, split_component_information,os);
	os << "\n REFERENCE EDGE = (" << id(get_pole1()) << ")-" << id(get_reference_edge()) << "-(" << id(get_pole2()) << ")" << flush;
	}


	
	
	
//-----------------------------------------------------------------------------
// PRIVATE
//-----------------------------------------------------------------------------

	void
	skeleton::
local_new 
 	()
	{
	//cout << "\n skeleton::local_new \n" << flush;
	};
	
	
	
	void
	skeleton::
local_del 
	()
	{
	//cout << "\n skeleton::local_del \n" << flush;
	}
	
	
	
	void
	skeleton::
local_renew
	()
	{
	local_del ();
	local_new ();
	}	
	
	

	void
	skeleton::
local_init
	(const split_component& sc, gdtedge e)
	{
	ref_edge = find_edge(sc.id(e));
	}
	
	


	
//-----------------------------------------------------------------------------
// SPQR_tree
//-----------------------------------------------------------------------------


	struct_SPQR_node_info::
struct_SPQR_node_info 
	(skeleton* skp)
	{
	//cout << "\n struct_SPQR_node_info () creator \n" << flush;
	skel = skp;
	if (skel) tree_edge.init((*skel).nodes_and_edges());
	maximum_status = 0;
	}



	SPQR_tree::
~SPQR_tree
	()
	{
	//cout << "\n SPQR_tree () destructor \n" << flush;
	local_del();
	}
	
	
	
	SPQR_tree::
SPQR_tree
	(plan_undi_graph& pug, gdtedge e, bool simpl)
	{
	//cout << "\n SPQR_tree () creator \n" << flush;
	local_new();
	local_init(pug,e,simpl);
	}

	
	// -----------------
	// Access operations
	// -----------------
	
	SPQR_node_type
	SPQR_tree::
get_type_of_node
	(gdtnode v) const
	{
	return (*node_info)[v]->type;
	}
	
	
	
	skeleton*
	SPQR_tree::
get_skeleton_of_node
	(gdtnode v) const
	{
	return (*node_info)[v]->skel;
	}
	
	
	
	bool
	SPQR_tree::
get_simpl_node_status
	() const
	{
	return simpl_node_status;
	}
	
	
	
	gdtedge
	SPQR_tree::
tree_edge_of_skeleton_edge_in_node
	(gdtedge e, gdtnode v) const
	{
	return (*node_info)[v]->tree_edge[e];
	}
	
	
	
	gdtedge
	SPQR_tree::
skeleton_edge_of_tree_edge
	(gdtedge e) const
	{
	return (*edge_info)[e].edge_corr_in_father_skeleton;
	}
	
	
	
	gdtnode
	SPQR_tree::
Q_node_of_edge_with_id 
	(int ide) const
	{
	gdtnode v,w;
	gdtedge se;	// skeleton gdtedge
	gdtedge te;	// tree gdtedge
	skeleton* skp;
	
	forall_nodes (v,*this)
		if (get_type_of_node (v) == Q_NODE)
			{
			if (v != root_of_tree()) 
				{
				te  = father_edge (v);
				w   = opposite (v,te);
				skp = get_skeleton_of_node (w);
				se  = skeleton_edge_of_tree_edge (te);
				}
			else
				{		
				te  = first_son_edge (v);
				w   = opposite (v,te);
				skp = get_skeleton_of_node (w);
				se  = skp -> get_reference_edge (); 
				}
			if (skp -> id(se) == ide) return v;
			}
	return NULL_NODE;
	}
	
	
	
	rotation_type
	SPQR_tree::
get_status_R_node_rotation 
	(int i, gdtnode v) const
	{
	if (get_type_of_node(v) != R_NODE)
		gdt_error("gdtnode is not an R_NODE");
	
	return ((*node_info)[v] -> R_status)[i].rotation;	
	}
	
	
	
	gdt::gdtlist<gdtedge>
	SPQR_tree::
get_status_P_node_permutation
	(int i, gdtnode v) const
	{
	if (get_type_of_node(v) != P_NODE)
		gdt_error("gdtnode is not a P_NODE");
	
	return ((*node_info)[v] -> P_status)[i].permutation;
	}
	

	
	int
	SPQR_tree::
number_of_nodes_type 
	(SPQR_node_type nt) const
	{
	gdtnode v;
	int count = 0;
	forall_nodes (v,*this)
		if (get_type_of_node(v) == nt) count++;
	return count;
	}
	
	
	
//	int
//	SPQR_tree::
//max_status_of_node
//	(gdtnode v) const
//	{
//	int max_status;
//	SPQR_node_type nt = get_type_of_node(v);
//	switch (nt)
//		{
//		case S_NODE: max_status = 0; break;
//		case Q_NODE: max_status = 0; break;
//		case R_NODE: max_status = 1; break;
//		case P_NODE:
//			{
//			int n = degree(v) - 1;
//			max_status = fact(n) - 1;
//			}
//		break;
//		}
//	return max_status;
//	}
	

	int
	SPQR_tree::
max_status_of_node
	(gdtnode v) const
	{
	return (*node_info)[v]->maximum_status;
	}
	
	
	bool
	SPQR_tree::
tree_edge_is_virtual
	(gdtedge te) const
	{
	bool virtual_edge = false;
	
	gdtnode v = father_node (te);
	gdtnode w = opposite(v,te);
	
	if ((get_type_of_node(v) != Q_NODE) && (get_type_of_node(w) != Q_NODE))
		virtual_edge = true;
	
	return virtual_edge;
	}
	
	
	
	void
	SPQR_tree::
compute_pre_lower_bound_type_l
	(gdt::gdtnode_matrix<int>& l, algorithm_type alg)
	{
	// the lower bounds l(v,w) is defined as the bend cost of an optimal representation
	// of skeleton(v) (with respect to alg), where the virtual edges and the ref_edge 
	// (i.e. the gdtedge corresponding to the tree gdtedge (v,w) into the skeleton sk) have 0 cost, 
	// and with ref_edge on the external face.
	// This value is a lower bound to the minimum number of bends in an sk optimal
	// orth. representation, with ref_edge on the external face.
	//
	gdtnode 	       v,w;
	gdtedge 	       e,te;
	skeleton*      skp;
	SPQR_node_type nt;
	
	//cout << "\nSTARTING PRE LOWER BOUND TYPE l\n" << flush; 
		
	forall_nodes (v,*this)
		{
		nt = get_type_of_node (v);
		forall_edges_adjacent_node (te,v,*this)
			{
			w  = opposite(v,te);
			
			if ((nt == Q_NODE) || (nt == S_NODE)) l(v,w) = 0;
			else 
				// R_NODE and P_NODE
				{
				skp    = get_skeleton_of_node (v);			// skeleton
				//plan_undi_graph& ow_pug = skp->get_owner_graph();	// owner graph
				gdtnode v1_sk = skp -> get_pole1();			// pole1 of the skeleton
				gdtnode v2_sk = skp -> get_pole2();			// pole2 of the skeleton
				gdtedge re;						// reference gdtedge of the skeleton

				// search the gdtedge into the skeleton sk corresponding to the tree gdtedge (v,w)
				//
				if (w == father_node(v))  re = skp->get_reference_edge();
				else			  re = skeleton_edge_of_tree_edge (te);	
					{
					l(v,w) = 0;
					switch (nt)
						{
						case P_NODE:
							{
							int num_edges_with_1_cost = 0;
							int v1_sk_count = 0;
							int v2_sk_count = 0;
							
							// ---------------------------------------
							// compute the number of edges with 1 cost
							// and the number of different directions
							// ---------------------------------------
							forall_edges (e,*skp)
								{
								if ((skp->get_type_of_edge(e) != VIRTUAL) && (e != re))
									{ 
									num_edges_with_1_cost ++;
									if      (skp->start(e)==v1_sk) v1_sk_count++;
									else if (skp->start(e)==v2_sk) v2_sk_count++; 
									}
								}
									
							// ---------------------------------------
							// compute the local general lower bound l 
							// ---------------------------------------
															
							if (alg == PLAN_ORTH_SLOW)      
								{
								// based on 2-degree faces
								if (num_edges_with_1_cost) l(v,w) = (num_edges_with_1_cost - 1)*2;
								}
								
							else if (alg == PLAN_UPWARD_SLOW) 
								{
								// based on cycles
								l(v,w) = gdt_minimum(v1_sk_count,v2_sk_count)*2;	 
								}
								
							//cout << "\nl(" << id(v) << "," << id(w) << ") = " << l(v,w) << flush;	
							break;
							}

						case R_NODE: 
							{
							//cout << "\n\nEXAMING AN R_NODE" << flush;
							
							face  ext_face;
							
							if (alg == PLAN_ORTH_SLOW)
								{
								plan_undi_graph pug (*skp);
								gdtedge p_re = pug.corresponding_edge(re,*skp);
								ext_face  = pug.right_face_moving_along_edge (p_re,pug.source(p_re));
								pug.del_constraints_type_involving_edge (NUMBER_OF_BENDS_ON_EDGE,p_re); 
								pug.new_constraint_number_of_bends_on_edge(p_re,ANY);
								orth_plan_undi_graph opug (pug,ext_face,PLAN_ORTH_OPTIMAL,false);
								l(v,w)   = opug.cost_of_last_algorithm();
								}
								
							else if (alg == PLAN_UPWARD_SLOW)
								{
								// ---------------------------------------------------------------
								// replace all the virtual edges different from re with a path
								// in the pertinent graph, and set all its edges with zero-cost
								// ---------------------------------------------------------------
								// COMMENT: Current data structure of the SPQR-tree does not allow
								//          to compute this step by an easy technique, since
								//          pertinent graphs are obtained with respect to the current
								//          orientation of the SPQR-tree. 
								//          Then we delay this step when we compute the L lower bounds.   
								l(v,w) = 0;
								}
								
							//cout << "\nl(" << id(v) << "," << id(w) << ") = " << l(v,w) << flush;
							}
						default:
							break;
						}
					}
				}
			}
		}

	}
	
	
	
	void
	SPQR_tree::
compute_pre_lower_bound_type_L
	(gdt::gdtnode_matrix<int>& L, gdt::gdtnode_matrix<int>& l, gdt::gdtnode_matrix<path_info>& P, bool replace_with_path, algorithm_type alg) 
	{
	// --------------------------------------------------------------------------
	// If w and v are two adjacent nodes of SPQR_tree, L(w,v) is defined as
	// a lower bound to min. number of bends in any orth./upwa. representation of
	// pertinent graph of gdtnode v, when it has w as father gdtnode, i.e. when
	// (v,w) is its reference gdtedge. 
	// It is computed recursively by the following expression:  
	// L(v,w) = L(u1,v) + L(u2,v) +...+ L(ur,v)+Cvw, 
	// where ui (i=1,..,r) indicate the sons of gdtnode v, and Cvw is a coefficient
	// obtained applying an optimization algorithm to the skeleton with some
	// edges replaced by paths having zero or real cost on its edges. 
	// --------------------------------------------------------------------------
	
	gdtnode 	         v,w;				// w = father_node (v)
	gdtedge 	         te_vw;				// te_vw = tree_edge (w,v)
	skeleton*        skp;				// skeleton(v)
	SPQR_node_type   nt;				// type of gdtnode v
	gdt::gdtlist<gdtnode>	 order;				// nodes in post-order
	gdt::gdtnode_array<int>  lev (nodes_and_edges(), 0);
	int		 Cvw = 0;		
		
	
	//cout << "\n\nSTARTING PRE LOWER BOUND TYPE L\n" << flush;
	
	// ------------------------ 1 -----------------------------
	// execute a post-order visit of SPQR_tree, and store the
	// visited nodes into the list "order"
	// --------------------------------------------------------
	//
	
	post_order_of_subtree (lev,order);
	
	// ------------------------ 2 -----------------------------
	// compute the lower bounds L(v,w) and the paths P(v,w) 
	// recursively, following the post-order in step 1
	// --------------------------------------------------------
	//
		
	forall (v,order)
		{
		if (v == root_of_tree()) continue;
		te_vw = father_edge (v);
		w     = father_node (v);
		nt    = get_type_of_node (v);
		skp   = get_skeleton_of_node(v);
		
		// ----------------------------- 2.1 ---------------------------
		// compute paths P(v,w) depending on the selected slow algorithm
		// -------------------------------------------------------------
		//
		
		if (nt != Q_NODE)
			{
			gdtnode v1  = skp -> get_pole1();
			gdtnode v2  = skp -> get_pole2();
			int idv1 = skp -> id(v1);
			int idv2 = skp -> id(v2);
			
			if (alg == PLAN_ORTH_SLOW)
				{
				P(v,w).sp = find_shortest_path_into_pertinent_graph_of_node (v,idv1,idv2);
				P(v,w).msp12.clear();
				P(v,w).msp21.clear();
				P(v,w).sw12 = INFINITE;
				P(v,w).sw21 = INFINITE;
				P(v,w).current_path = SP;
				}
				
			else if (alg == PLAN_UPWARD_SLOW)
				{
				int sw12 = 0;
				int sw21 = 0; 		
				P(v,w).sp.clear();
				P(v,w).msp12 = find_minimum_switches_path_into_pertinent_graph_of_node (v,idv1,idv2,sw12,OUTGOING);
				P(v,w).msp21 = find_minimum_switches_path_into_pertinent_graph_of_node (v,idv1,idv2,sw21,INCOMING);
				P(v,w).sw12  = sw12;
				P(v,w).sw21  = sw21;
				if (sw12 <= sw21) P(v,w).current_path = MSP12;
				else		  P(v,w).current_path = MSP21;
				}
			}
		
		// ----------------------------- 2.2 ----------------------------------
		// compute L(v,w), using previous L(u,v) and P(u,v) for each son u of v
		// --------------------------------------------------------------------
		//
		
		switch (nt)
			{
			case Q_NODE: 
				//cout << "\nEXAMING A Q_NODE" << flush;
				L(v,w) = 0;
				break;
				
			case S_NODE:
				{
				//cout << "\nEXAMING AN S_NODE" << flush;
				gdtnode u;
				L(v,w) = 0;
				forall_son_nodes (u,v,*this)  
					{
					if (L(v,w) == INFINITE || L(u,v) == INFINITE) L(v,w) = INFINITE;
					else					      L(v,w) += L(u,v);
					}
				//cout << "\nL(" << id(v) << "," << id(w) << ") = " << L(v,w) << flush;
				break;
				}
			
			case R_NODE: 
				{
				//cout << "\nEXAMING AN R_NODE" << flush;
				plan_undi_graph& ow_pug = skp->get_owner_graph();	// owner graph
				gdtedge 		 re     = skp -> get_reference_edge();	// reference gdtedge
				gdtnode u;
				gdt::gdtqueue<gdtnode> sons_with_null_L ; 
				L(v,w) = 0;
				forall_son_nodes (u,v,*this)
					if (L(u,v) != 0)      			
						{
						if (L(v,w) == INFINITE || L(u,v) == INFINITE) L(v,w) = INFINITE;
						else					      L(v,w) += L(u,v);
						}
					else if (get_type_of_node(u) != Q_NODE) sons_with_null_L.append(u);
					
				//cout << "\nNUM. EDGES WITH NULL L = " << sons_with_null_L.size() << flush;
				
				if (alg == PLAN_ORTH_SLOW)
					{
					Cvw = l(v,w);
					if ((replace_with_path) && (sons_with_null_L.size() != 0)) 
						{
						// ------------------------------------------------------------
						// replace all the virtual edges, having null L, into the 
						// skeleton of gdtnode v, with shortest paths in their pertinent 
						// graphs, and set real cost of these edges.
						// ------------------------------------------------------------
						
						undi_graph ug;
						gdtedge start_edge;
						gdtnode start_node;
						gdt::gdtedge_map<gdtedge> tree_edge (ug.nodes_and_edges());
						merge_undi_graph_with_skeleton_of_node (ug,v,0,start_edge,start_node,tree_edge);
						while (!sons_with_null_L.empty())
							{
							u = sons_with_null_L.pop();
							gdtedge e_ug;
							gdtedge te_uv = father_edge (u);
			     				forall_edges (e_ug,ug) if (tree_edge[e_ug] == te_uv) break;
			     				tree_edge[e_ug] = NULL_EDGE;
			     				gdt::gdtlist<gdtedge> edge_path = ug.replace_edge_with_path_of_graph (e_ug,P(u,v).sp,ow_pug);
			     				ug.del_constraints_type (SAME_FACE_NODES);
			     				ug.del_constraints_type (SAME_FACE_ORDERED_NODES);
			     				forall (e_ug,edge_path) tree_edge[e_ug] = NULL_EDGE;
							}
						
						// ---------------------------------------
						// compute the lower bound coefficient Cvw
						// ---------------------------------------
						
						//plan_undi_graph pug;
						//face ext_face = pug.plan_with_fixed_face_of_undi_graph (ug,start_edge,start_node);
						plan_undi_graph pug(ug);
						gdtedge p_re = pug.corresponding_edge (re,*skp);
						pug.del_constraints_type_involving_edge (NUMBER_OF_BENDS_ON_EDGE,p_re); 
						pug.new_constraint_number_of_bends_on_edge(p_re,ANY);
						face ext_face = pug.right_face_moving_along_edge (p_re,pug.source(p_re));
						orth_plan_undi_graph opug(pug, ext_face, PLAN_ORTH_OPTIMAL, false);
						
						Cvw = opug.cost_of_last_algorithm();
						
						// --------
						// new code
						// --------
						/* the idea it seems to be not totally correct
							{
							// ------------------------------------------------
							// determine the set Z of vertices with degree > 4 
							// and such that v is their proper allocation gdtnode.
							// Also, determine the k coefficient;
							// ------------------------------------------------
							gdtnode z_sk;	// gdtnode in sk
							gdtnode z;		// gdtnode in opug corresponding to z_sk
							
							int k = 0;
							gdt::gdtnode_map<bool> zero_vertex (opug.nodes_and_edges(),false); // set Z
							skeleton* skp_w = NULL;
							if (get_type_of_node(w) != Q_NODE) skp_w = get_skeleton_of_node(w);
								forall_nodes (z_sk,*skp)
									{
									gdtnode z_ow = ow_pug.corresponding_node (z_sk,*skp);
									if ((ow_pug.degree(z_ow) > 4) && (!skp_w || !skp_w->find_node(skp->id(z_sk)))) 
										{
										z = opug.corresponding_node(z_sk,*skp);
										zero_vertex[z] = true;
										k += ow_pug.degree(z_ow) - 4;
										}
									}
									
							// ---------------------------
							// determine the d coefficient
							// and update lower bound
							// ---------------------------
							
							int d = 0;
							gdtedge e;
							forall_edges (e,opug)
								{
								int h = 0; // number of extremal vertices of e in Z
								gdtnode z1 = opug.source(e);
								gdtnode z2 = opug.target(e);
								if (zero_vertex[z1]) h++;
								if (zero_vertex[z2]) h++;
								
								gdtedge e_sk = skp->corresponding_edge(e,opug);
								
								if (!e_sk || skp->get_type_of_edge(e_sk) == REAL)	// real gdtedge
									{
									int p = opug.number_of_bends(e);
									if (h == 1 && p >=1) 	 d++;
									else if (h == 2)
										if (p > 1) 	 d += 2;
										else if (p == 1) d++;
									}
								else 							// virtual gdtedge
									if (e_sk != re)	// not reference gdtedge
										{
										gdtedge te = tree_edge_of_skeleton_edge_in_node (e_sk,v); 
										gdtnode u = opposite(v,te);
										if (h != 0) d += L(u,v);
										}
								}
							
							if (k > d)
								{
								//cout << "\nAdding " << k-d << " to lower bound " << Cvw;
								Cvw += (k - d);
								}
							
							}
						//
						*/
						}					
					//else Cvw = l(v,w);
					}
					
					
				else if (alg == PLAN_UPWARD_SLOW)
					{
					// ------------------------------------------------------------------
					// replace all the virtual edges, having null L, into the skeleton 
					// of gdtnode v with shortest paths in their pertinent graphs, 
					// setting them with their real costs;
					// replace all the virtual edges, having no null L, into the skeleton 
					// of gdtnode v with shortest paths in their pertinent graphs, 
					// setting them with null cost.
					// ------------------------------------------------------------------	
					
					undi_graph ug;	// temporary plan_undi_graph
					gdtedge start_edge;
					gdtnode start_node;
					gdt::gdtedge_map<gdtedge> tree_edge (ug.nodes_and_edges(),NULL_EDGE);
					
					merge_undi_graph_with_skeleton_of_node (ug,v,0,start_edge,start_node,tree_edge);
				
					gdtedge e_ug;
					forall_son_nodes (u,v,*this)
						{
						gdtedge te_uv = father_edge (u);
						if (tree_edge_is_virtual(te_uv))
							{
							gdt::gdtlist<gdtedge> sp;
							if 	(P(u,v).current_path == MSP12) sp = P(u,v).msp12;
							else if (P(u,v).current_path == MSP21) sp = P(u,v).msp21;
							else gdt_error("no path selected"); 
							
			     				forall_edges (e_ug,ug) if (tree_edge[e_ug] == te_uv) break;
			     				tree_edge[e_ug] = NULL_EDGE;
			     				gdt::gdtlist<gdtedge> edge_path = ug.replace_edge_with_path_of_graph (e_ug,sp,ow_pug);
			     				forall (e_ug,edge_path) tree_edge[e_ug] = NULL_EDGE;
			     				if (L(u,v) != 0) 
			     					forall (e_ug,edge_path)
			     						{
			     						ug.del_all_constraints_involving_edge (e_ug);	
			     						ug.new_constraint_number_of_bends_on_edge (e_ug,ANY);
			     						}
			     				}
						}
						
					// ---------------------------------------
					// compute the lower bound coefficient Cvw
					// ---------------------------------------
												
					plan_undi_graph  pug;	// temporary plan_undi_graph
					gdtedge e_pug; 
					face ext_face = pug.plan_with_fixed_face_of_undi_graph (ug,start_edge,start_node);
					e_pug    = pug.corresponding_edge(start_edge,ug);
					ext_face = pug.del_edge(e_pug);
					upwa_plan_undi_graph upug (pug,ext_face,PLAN_UPWARD_OPTIMAL,false);
					Cvw = upug.cost_of_last_algorithm();
					}
								
				if (L(v,w) == INFINITE || Cvw == INFINITE) L(v,w) =  INFINITE;
				else                                       L(v,w) += Cvw;
				//cout << "\nL(" << id(v) << "," << id(w) << ") = " << L(v,w) << flush;
				break;
				}
				
			case P_NODE: 
				{
				//cout << "\nEXAMING A P_NODE" << flush;
				gdtnode u;
				gdt::gdtqueue<gdtnode> sons_with_null_L ;
				gdt::gdtqueue<gdtnode> virtual_sons_with_null_L;
				
				// --------------------------------------------
				// compute the set of all son edges with null L
				// --------------------------------------------
				//
				L(v,w) = 0;
				forall_son_nodes (u,v,*this)
					if (L(u,v) != 0)  
						{
						if ((L(v,w) == INFINITE) || (L(u,v) == INFINITE)) L(v,w) =  INFINITE;
						else 						  L(v,w) += L(u,v);
						}
					else    sons_with_null_L.append(u);
				//

				if (alg == PLAN_ORTH_SLOW)
					{
					// -----------------------------------------------
					// compute the number of all son edges with null L
					// -----------------------------------------------
					//
					int num_of_total_edges_with_null_L = sons_with_null_L.size();
					//
					
					// --------------------------------------------------------------------
					// compute the set and the number of virtual son edges only with null L 
					// --------------------------------------------------------------------
					//
					int num_of_virtual_edges_with_null_L = 0;
					while (!sons_with_null_L.empty())
						{
						u = sons_with_null_L.pop();
						gdtedge te_uv = father_edge (u);
						if (tree_edge_is_virtual(te_uv)) virtual_sons_with_null_L.append(u); 
						}
					num_of_virtual_edges_with_null_L = virtual_sons_with_null_L.size();
					//
					
					//array<int> k(1,3);
					gdt::gdtmap<int,int> k;		// size of shortest paths
					int i = 0;
					while (!virtual_sons_with_null_L.empty())
						{
						i ++;
						u = virtual_sons_with_null_L.pop();
						k[i] = P(u,v).sp.size();
						}
					
					Cvw = 0;
					
					if (num_of_total_edges_with_null_L < 4)		// always for 4-planar graphs
						{
						if (num_of_total_edges_with_null_L == 3)
							{
							switch (num_of_virtual_edges_with_null_L)
								{
								case 0: // 3 real edges and 0 virtual edges
									Cvw = 4; 
									break;		
							
								case 1: // 2 real edges and 1 virtual edges 
									if (k[1] == 2) Cvw = 3;
									if (k[1] >  2) Cvw = 2; 		
									break;
								
								case 2: // 1 real edges and 2 virtual edges
									if ((k[1] == 2) && (k[2] == 2)) Cvw = 2;
									if ((k[1] == 2) && (k[2] >  2)) Cvw = 1;
									if ((k[1] >  2) && (k[2] == 2)) Cvw = 1;
									break;
								
								case 3: // 0 real edges and 3 virtual edges 
									if ((k[1] == 2) && (k[2] == 2) && (k[3] == 2)) Cvw = 2;
									if ((k[1] == 2) && (k[2] == 2) && (k[3] == 3)) Cvw = 1;
									if ((k[1] == 2) && (k[2] == 3) && (k[3] == 2)) Cvw = 1;
									if ((k[1] == 3) && (k[2] == 2) && (k[3] == 2)) Cvw = 1;
								}
							}
					
						else if (num_of_total_edges_with_null_L == 2)
							{
							switch (num_of_virtual_edges_with_null_L)
								{
								case 0: // 2 real edges and 0 or 1 virtual edges 
									Cvw = 2; 
									break;
								
								case 1: // 1 real edges and 1 or 2 virtual edges 
									if (k[1] == 2) 
									Cvw = 1; 
									break;
								}
							}
						
						else Cvw = l(v,w); // at most 1 real gdtedge
						}
					
					else if (num_of_total_edges_with_null_L)	// sometimes for non 4-planar graphs	
						{
						// --------------------------------------------
						// count all shortest path with gdtedge length < 3
						// --------------------------------------------
						
						int num_of_shortest_paths_less_than_three = num_of_total_edges_with_null_L - 
											    num_of_virtual_edges_with_null_L;
						int i;
						for (i=1; i<=num_of_virtual_edges_with_null_L; i++)
							if (k[i] < 3) num_of_shortest_paths_less_than_three ++;
						
						if (get_type_of_node(w) == Q_NODE) num_of_shortest_paths_less_than_three ++;
						else if (get_type_of_node(w) == S_NODE)
							{
							skeleton* skp_v = get_skeleton_of_node (v);
							skeleton* skp_w = get_skeleton_of_node (w);
							gdtnode v1 = skp_v -> get_pole1();
							gdtnode v2 = skp_v -> get_pole2();
							undi_graph ug (*skp_w);
							gdtedge e_ug, e_skp_w;
							forall_edges (e_skp_w, *skp_w)
								if (skp_w -> get_type_of_edge(e_skp_w) == VIRTUAL) 
									{
									e_ug = ug.corresponding_edge (e_skp_w,*skp_w);
									ug.del_edge(e_ug);
									}
								
							gdtnode u1 = ug.corresponding_node (v1,*skp_v);
							gdtnode u2 = ug.corresponding_node (v2,*skp_v); 
							gdt::gdtlist<gdtnode> nodes;	// dummy variable
							gdt::gdtlist<gdtedge> edges;	// dummy variable
							int  l = ug.unweighted_unoriented_shortest_path (u1,u2,nodes,edges); 
							if (l < 3) num_of_shortest_paths_less_than_three ++;
							}
						
						// ----------------------------------
						// assign the lower bound coefficient
						// ----------------------------------
						
						Cvw = (num_of_shortest_paths_less_than_three - 2)*2;
						}
					}
					
				else if (alg == PLAN_UPWARD_SLOW) 
					{
					// based on cycles
					//
					gdtnode v1 = skp->get_pole1();
					gdtnode v2 = skp->get_pole2();
					int  npv12 = 0;			// number of paths p1-->p2
					int  npv21 = 0;			// number of paths p2-->p1
					gdtedge e;
					gdtnode num_virt_edges = 0;
					
					forall_son_nodes (u,v,*this)
						{
						if (get_type_of_node(u) != Q_NODE && L(u,v)==0)
							{
							num_virt_edges ++;
							skeleton* skup = get_skeleton_of_node(u);
							gdtnode u1 = skup -> get_pole1();
							gdtnode u2 = skup -> get_pole2();
							bool reverse = false;
							if      (skup->id(u1) == skp->id(v1)) reverse = false;
							else if (skup->id(u2) == skp->id(v1)) reverse = true;
							else gdt_error("not correspondence of poles");;	
							
							if (!P(u,v).sw12) if (reverse) npv21 ++;
								          else	       npv12 ++;
							if (!P(u,v).sw21) if (reverse) npv12 ++;
							                  else	       npv21 ++;
							}
						}
					
					if  (!num_virt_edges) Cvw = l(v,w);
					else	
						{
						forall_edges (e,*skp) 
							if (skp->get_type_of_edge(e) == REAL)
								if      (skp->start(e) == v1) npv12 ++;
								else if (skp->start(e) == v2) npv21 ++;
						Cvw = gdt_minimum(npv12,npv21)*2;
						}
				
					//cout << "\nFOUND A LOWER BOUND DUE TO CYCLES = " << Cvw << flush;
					}
				
				if ((L(v,w) == INFINITE) || (Cvw == INFINITE))	L(v,w) =  INFINITE;
				else 						L(v,w) += Cvw;
				
				//cout << "\nL(" << id(v) << "," << id(w) << ") = " << L(v,w) << flush;	
				break;
				}
			}
		}
	}
	


	bool
	SPQR_tree::
merge_undi_graph_with_skeleton_of_node 
	(undi_graph& ug, gdtnode v, int i, 
	 gdtedge& start_edge, gdtnode& start_node, 
	 gdt::gdtedge_map<gdtedge>& tree_edge)
	{

	SPQR_node_type nt = get_type_of_node(v);     	// type of gdtnode v
	
	if (nt == Q_NODE)
		gdt_error("v must to be S,P or R gdtnode");	
	
	
	skeleton* skp    = get_skeleton_of_node(v);     // skeleton of gdtnode v
	
	gdtedge	  re_sk  = skp -> get_reference_edge(); // reference gdtedge of sk (*skp)
	gdtedge 	  e_sk,ew1_sk,ew2_sk;  	   	        // utility gdtedge in sk 
	gdtedge      e_ug,ew1_ug,ew2_ug, 
		  re_ug = NULL_EDGE;	        	// utility gdtedge in ug
	
	gdtnode	  v1_sk  = skp -> get_pole1();	    	// pole1 of reference gdtedge re_sk	
	gdtnode      v2_sk  = skp -> get_pole2();	     	// pole2 of reference gdtedge re_sk
	gdtnode	  v1_ug, v2_ug;			     	// pole1 and pole2 in ug
	gdtnode	  w_sk, w1_sk, w2_sk;		     	// utility nodes in sk
	gdtnode	  w_ug, w1_ug, w2_ug;		     	// utility nodes in ug
	
	gdt::gdtnode_map<gdtnode> node_corr_in_ug (skp->nodes_and_edges(),NULL_NODE);  // sk gdtnode -> ug gdtnode
	gdt::gdtedge_map<gdtedge> edge_corr_in_ug (skp->nodes_and_edges(),NULL_EDGE);  // sk gdtedge -> ug gdtedge
	
		{
		gdtedge e;
		forall_edges (e,*skp) 
			edge_corr_in_ug[e] = NULL_EDGE;
		}
	
	
	if (ug.number_of_nodes() == 0)
		{
		// --------------------------------------------
		// INIZIALIZE ug with skeleton sk, and status i
		// --------------------------------------------
		
		forall_nodes (w_sk,*skp) node_corr_in_ug[w_sk] = ug.new_node (skp->id(w_sk));
		v1_ug = node_corr_in_ug[v1_sk];
		v2_ug = node_corr_in_ug[v2_sk];

		switch (nt)
			{
			case S_NODE:
				{
				forall_edges (e_sk,*skp)
					{
					w1_sk = skp->source(e_sk);
					w2_sk = skp->target(e_sk);
					w1_ug = node_corr_in_ug[w1_sk];
					w2_ug = node_corr_in_ug[w2_sk];
					e_ug  = ug.new_edge (w1_ug,w2_ug,skp->id(e_sk));
					gdtnode v_sk = skp->start(e_sk);
					if (v_sk) if (v_sk==w1_sk) ug.make_directed(e_ug,w1_ug);
					          else             ug.make_directed(e_ug,w2_ug);
					edge_corr_in_ug[e_sk] = e_ug;
					tree_edge[e_ug] = tree_edge_of_skeleton_edge_in_node(e_sk,v);
					}
				re_ug = edge_corr_in_ug[re_sk];
				break;
				}

			case P_NODE:
				{
				gdtnode v_sk;
				re_ug = ug.new_edge (v1_ug,v2_ug,skp->id(re_sk));
				tree_edge[re_ug] = tree_edge_of_skeleton_edge_in_node(re_sk,v);
				e_ug = re_ug;
				v_sk = skp->start(re_sk);
				if (v_sk) if (skp->start(re_sk) == v1_sk) ug.make_directed(e_ug,v1_ug);
					  else				  ug.make_directed(e_ug,v2_ug);

				gdt::gdtlist<gdtedge> permut = get_status_P_node_permutation(i,v);
				forall (e_sk,permut)
					{
					e_ug = ug.new_edge (v2_ug,e_ug,v1_ug,re_ug,skp->id(e_sk),gdt::after);
					v_sk = skp->start(e_sk);
					if (v_sk) if (v_sk==v1_sk) ug.make_directed(e_ug,v1_ug);
					          else             ug.make_directed(e_ug,v2_ug);
					tree_edge[e_ug] = tree_edge_of_skeleton_edge_in_node(e_sk,v);
					w1_sk = skp->source(e_sk);
					w2_sk = skp->target(e_sk);
					w1_ug = node_corr_in_ug[w1_sk];
					w2_ug = node_corr_in_ug[w2_sk];
					ug.set_source(e_ug,w1_ug);
					}
				break;
				}
				
			case R_NODE:
				{
				//cout << "\nR-NODE INITIALIZE CASE\n" << flush;
				gdt::gdtedge_array<bool> edge_copied(skp->nodes_and_edges(),false);
				rotation_type	 rot = get_status_R_node_rotation(i,v);
				int		 dir;
				
				if (rot == COUNTER_CLOCKWISE) dir = gdt::after;
				else			      dir = gdt::before;
				
				forall_edges (e_sk,*skp) 
					{
					w1_sk = skp->source(e_sk);
					w2_sk = skp->target(e_sk);
					w1_ug = node_corr_in_ug[w1_sk];
					w2_ug = node_corr_in_ug[w2_sk];
					
					if (ug.degree(w1_ug) > 1)
						if (ug.degree(w2_ug) > 1)
							{
							ew1_sk = skp->cyclic_adj_pred(e_sk,w1_sk);
							while (!edge_copied[ew1_sk]) ew1_sk = skp->cyclic_adj_pred(ew1_sk,w1_sk);
							ew1_ug = edge_corr_in_ug[ew1_sk];
							ew2_sk = skp->cyclic_adj_pred(e_sk,w2_sk);
							while (!edge_copied[ew2_sk]) ew2_sk = skp->cyclic_adj_pred(ew2_sk,w2_sk);
							ew2_ug = edge_corr_in_ug[ew2_sk];
							//cout << "\nADDING THE NEW EDGE " << skp->id(e_sk) << flush;
							e_ug = ug.new_edge(w1_ug,ew1_ug,w2_ug,ew2_ug,skp->id(e_sk),dir);
							edge_corr_in_ug[e_sk] = e_ug;
							}
						else
							{
							ew1_sk = skp->cyclic_adj_pred(e_sk,w1_sk);
							while (!edge_copied[ew1_sk]) ew1_sk = skp->cyclic_adj_pred(ew1_sk,w1_sk);
							ew1_ug = edge_corr_in_ug[ew1_sk];
							//cout << "\nADDING THE NEW EDGE " << skp->id(e_sk) << flush;
							e_ug = ug.new_edge(w1_ug,ew1_ug,w2_ug,skp->id(e_sk),dir);
							edge_corr_in_ug[e_sk] = e_ug;
							}
					else
						if (ug.degree(w2_ug) > 1)
							{
							ew2_sk = skp->cyclic_adj_pred(e_sk,w2_sk);
							while (!edge_copied[ew2_sk]) ew2_sk = skp->cyclic_adj_pred(ew2_sk,w2_sk);
							ew2_ug = edge_corr_in_ug[ew2_sk];
							//cout << "\nADDING THE NEW EDGE " << skp->id(e_sk) << flush;
							e_ug = ug.new_edge(w2_ug,ew2_ug,w1_ug,skp->id(e_sk),dir);
							edge_corr_in_ug[e_sk] = e_ug;
							ug.set_source(e_ug,w1_ug);
							}
						else	
							{
							//cout << "\nFIRST CASE\n" << flush;
							//cout << "\nADDING THE NEW EDGE " << skp->id(e_sk) << flush;
							e_ug = ug.new_edge(w1_ug,w2_ug,skp->id(e_sk));
							edge_corr_in_ug[e_sk] = e_ug;
							//cout << "\nADDED EDGE; e_ug = " << ug.id(e_ug) << flush;
							}
					
					gdtnode v_sk = skp->start(e_sk);
					if (v_sk) if (v_sk==w1_sk) ug.make_directed(e_ug,w1_ug);
					          else             ug.make_directed(e_ug,w2_ug);	
					tree_edge[e_ug] = tree_edge_of_skeleton_edge_in_node (e_sk,v);
					edge_copied[e_sk] = true;
					}
				
				re_ug = edge_corr_in_ug[re_sk];	
				break;
				}
			default:
				break;
			}
			
		start_edge = re_ug;
		start_node = v2_ug;
		}
	
	else 
		{
		re_ug = ug.corresponding_edge(re_sk,*skp);
		if (!re_ug) return false; 
			//gdt_error("ref_edge not present in undi_graph");
			
			
		// ------------------------------------------------------
		// REPLACE the gdtedge re_ug with skeleton sk, and status i
		// ------------------------------------------------------
		
		v1_ug = ug.source(re_ug); if (ug.id(v1_ug) != skp->id(v1_sk)) v1_ug = ug.target(re_ug);
		v2_ug = ug.opposite(v1_ug,re_ug);
		node_corr_in_ug[v1_sk] = v1_ug;
		node_corr_in_ug[v2_sk] = v2_ug;
		
		// add to ug all sk nodes different from v1_sk and v2_sk
		//
		forall_nodes (w_sk,*skp)
			if ((w_sk != v1_sk) && (w_sk != v2_sk))
				{
				//cout << "\nADDING THE NEW NODE " << skp->id(w_sk);
				w_ug = ug.new_node (skp->id(w_sk));
				node_corr_in_ug[w_sk] = w_ug;
				}

		switch (nt)
			{
			case S_NODE:
				{
				// add the gdtedge adjacent to v1_sk and different from re_sk
				//
				e_sk = skp->cyclic_adj_succ (re_sk,v1_sk);
				w_sk = skp->opposite (v1_sk,e_sk);
				w_ug = node_corr_in_ug[w_sk];
				//cout << "\nADDING THE NEW EDGE " << skp->id(e_sk) << flush;
				e_ug = ug.new_edge (v1_ug,re_ug,w_ug,skp->id(e_sk),gdt::before);
				gdtnode v_sk = skp->start(e_sk);
				if (v_sk) if (v_sk==v1_sk) ug.make_directed(e_ug,v1_ug);
					  else             ug.make_directed(e_ug,w_ug);	
				tree_edge[e_ug] = tree_edge_of_skeleton_edge_in_node (e_sk,v);
				ug.set_source (e_ug,node_corr_in_ug[skp->source(e_sk)]);
				
				// add all edges not adjacent v1_sk or v2_sk
				//
				e_sk  = skp->cyclic_adj_succ (e_sk,w_sk);
				w_sk  = skp->opposite (w_sk,e_sk);
				while (w_sk != v2_sk)
					{
					w1_sk = skp->source(e_sk);
					w2_sk = skp->target(e_sk);
					w1_ug = node_corr_in_ug[w1_sk];
					w2_ug = node_corr_in_ug[w2_sk];
					//cout << "\nADDING THE NEW EDGE " << skp->id(e_sk) << flush;
					e_ug  = ug.new_edge (w1_ug,w2_ug,skp->id(e_sk));
					v_sk = skp->start(e_sk);
					if (v_sk) if (v_sk==w1_sk) ug.make_directed(e_ug,w1_ug);
					          else             ug.make_directed(e_ug,w2_ug);	
					tree_edge[e_ug] = tree_edge_of_skeleton_edge_in_node (e_sk,v);
					e_sk  = skp->cyclic_adj_succ (e_sk,w_sk);
					w_sk  = skp->opposite(w_sk,e_sk);
					}
				
				// add gdtedge adj. v2_sk different from re_sk
				//
				//e_sk = skp->cyclic_adj_succ (re_sk,v2_sk);
				w_sk = skp->opposite (v2_sk,e_sk);
				w_ug = node_corr_in_ug[w_sk];
				//cout << "\nADDING THE NEW EDGE " << skp->id(e_sk) << flush;
				e_ug = ug.new_edge (v2_ug,re_ug,w_ug,skp->id(e_sk),gdt::after);
				v_sk = skp->start(e_sk);
				if (v_sk) if (v_sk==v2_sk) ug.make_directed(e_ug,v2_ug);
					  else             ug.make_directed(e_ug,w_ug);	
				tree_edge[e_ug] = tree_edge_of_skeleton_edge_in_node (e_sk,v);
				ug.set_source (e_ug,node_corr_in_ug[skp->source(e_sk)]);
				break;
				}
				
			case P_NODE:
				{
				gdt::gdtlist<gdtedge> permut = get_status_P_node_permutation(i,v);
				e_ug = re_ug;
				forall (e_sk,permut)
					{
					e_ug  = ug.new_edge (v2_ug,e_ug,v1_ug,re_ug,skp->id(e_sk),gdt::after);
					gdtnode v_sk = skp->start(e_sk);
					if (v_sk) if (v_sk==v1_sk) ug.make_directed(e_ug,v1_ug);
					          else             ug.make_directed(e_ug,v2_ug);	
					tree_edge[e_ug] = tree_edge_of_skeleton_edge_in_node (e_sk,v);
					ug.set_source(e_ug,node_corr_in_ug[skp->source(e_sk)]);
					}
				break;
				}
				
			case R_NODE:
				{
				gdt::gdtedge_array<bool> edge_copied(skp->nodes_and_edges(),false);
				rotation_type	 rot = get_status_R_node_rotation(i,v);
				int		 dir;
				
				edge_copied[re_sk] = true;
				edge_corr_in_ug[re_sk] = re_ug;
				
				if (rot == COUNTER_CLOCKWISE) dir = gdt::after;
				else			      dir = gdt::before;
				
				forall_edges (e_sk,*skp) if (e_sk != re_sk)
					{
					w1_sk = skp->source(e_sk);
					w2_sk = skp->target(e_sk);
					w1_ug = node_corr_in_ug[w1_sk];
					w2_ug = node_corr_in_ug[w2_sk];
					if (ug.degree(w1_ug) > 1)
						if (ug.degree(w2_ug) > 1)
							{
							ew1_sk = skp->cyclic_adj_pred(e_sk,w1_sk);
							while (!edge_copied[ew1_sk]) ew1_sk = skp->cyclic_adj_pred(ew1_sk,w1_sk);
							ew1_ug = edge_corr_in_ug[ew1_sk];
							ew2_sk = skp->cyclic_adj_pred(e_sk,w2_sk);
							while (!edge_copied[ew2_sk]) ew2_sk = skp->cyclic_adj_pred(ew2_sk,w2_sk);
							ew2_ug = edge_corr_in_ug[ew2_sk];
							//cout << "\nADDING THE NEW EDGE " << skp->id(e_sk);
							e_ug = ug.new_edge(w1_ug,ew1_ug,w2_ug,ew2_ug,skp->id(e_sk),dir);
							edge_corr_in_ug[e_sk] = e_ug;
							}
						else
							{
							ew1_sk = skp->cyclic_adj_pred(e_sk,w1_sk);
							while (!edge_copied[ew1_sk]) ew1_sk = skp->cyclic_adj_pred(ew1_sk,w1_sk);
							ew1_ug = edge_corr_in_ug[ew1_sk];
							//cout << "\nADDING THE NEW EDGE " << skp->id(e_sk);
							e_ug = ug.new_edge(w1_ug,ew1_ug,w2_ug,skp->id(e_sk),dir);
							edge_corr_in_ug[e_sk] = e_ug;
							}
					else
						if (ug.degree(w2_ug) > 1)
							{
							ew2_sk = skp->cyclic_adj_pred(e_sk,w2_sk);
							while (!edge_copied[ew2_sk]) ew2_sk = skp->cyclic_adj_pred(ew2_sk,w2_sk);
							ew2_ug = edge_corr_in_ug[ew2_sk];
							//cout << "\nADDING THE NEW EDGE " << skp->id(e_sk);
							e_ug = ug.new_edge(w2_ug,ew2_ug,w1_ug,skp->id(e_sk),dir);
							edge_corr_in_ug[e_sk] = e_ug;
							ug.set_source(e_ug,w1_ug);
							}
						else	
							{
							//cout << "\nADDING THE NEW EDGE " << skp->id(e_sk);
							e_ug = ug.new_edge(w1_ug,w2_ug,skp->id(e_sk));
							edge_corr_in_ug[e_sk] = e_ug;
							}
							
					gdtnode v_sk = skp->start(e_sk);
					if (v_sk) if (v_sk==w1_sk) ug.make_directed(e_ug,w1_ug);
					          else             ug.make_directed(e_ug,w2_ug);	
					tree_edge[e_ug] = tree_edge_of_skeleton_edge_in_node (e_sk,v);
					edge_copied[e_sk] = true;
					}
				break;
				}
			default:
				break;
			}
			
			// delete re_ug
			//
			ug.del_edge(re_ug);
		}	
				
		
		// ------------------------------
		// copy all gdtnode and gdtedge markers
		// and constraints.
		// ------------------------------
	
		forall_nodes (w_sk,*skp)
			//if (w_sk != skp->get_pole1() && w_sk != skp->get_pole2())
				{
				w_ug = ug.corresponding_node (w_sk,*skp);
				if (w_ug)
					{
					gdt::gdtlist<marker_type> ml = skp -> markers(w_sk);					
					ug.mark (w_ug,ml); 
					}
				}		
		
		forall_edges (e_sk,*skp)
			{
			e_ug = ug.corresponding_edge (e_sk,*skp);
			if (e_ug)
				{
				gdt::gdtlist<marker_type> ml = skp -> markers(e_sk);
				ug.mark (e_ug,ml);
				}
			}
			
		constraint c;
		forall_constraints (c,*skp) 
			{
			//cout << "\ncopying constraint number " << id(c) << flush;
			ug.new_constraint(c);
			}		
	return true;	
	} 
	
	
	
	bool	
	SPQR_tree::
build_undi_graph_starting_from_node 
	(undi_graph& ug, gdtnode v, gdt::gdtnode_array<int>& status_of_node,
	 gdtedge& start_edge, gdtnode& start_node, gdt::gdtedge_map<gdtedge>& tree_edge)
	{
	gdtnode 		w; 
	gdt::gdtnode_array<int> lev (nodes_and_edges());
	gdt::gdtlist<gdtnode>      order;
	skeleton* 	skp;
	
	if (get_type_of_node(v) == Q_NODE) 
		{
		gdtedge te = father_edge (v);
		gdtedge se = skeleton_edge_of_tree_edge (te);
		skp     = get_skeleton_of_node (opposite(v,te));
		gdtnode w1 = skp -> source(se);
		gdtnode w2 = skp -> target(se);
		gdtnode v1 = ug.new_node(skp->id(w1));
		gdtnode v2 = ug.new_node(skp->id(w2));
		gdtedge e  = ug.new_edge(v1,v2,skp->id(se));
		if      (skp->start(se) == w1) ug.make_directed(e,v1);
		else if (skp->start(se) == w2) ug.make_directed(e,v2);
		}
	else
		{
		BFS_of_subtree(lev,order,v);

		forall (w,order) 
			if (get_type_of_node(w) != Q_NODE)
				{
				skp = get_skeleton_of_node (w);
				//cout << "\nCURRENT UNDI_GRAPH : \n"; ug.print(COMPLETE); cout << flush;
				//cout << "\nMERGING WITH SKELETON "; skp->print(COMPLETE,false); 
				if (!merge_undi_graph_with_skeleton_of_node (ug,w,status_of_node[w],start_edge,start_node,tree_edge))
					return false;
				//cout << "\nCURRENT UNDI_GRAPH : \n"; ug.print(); cout << flush;
				}
		}
		
		
	// CHECK TO REMOVE
	// ---------------
	
	//cout << "\n\n\nUNDI_GRAPH BUILDED:\n" << flush;
	//ug.print(COMPLETE);		
	//cout << "\nEND \n" << flush;	
	//cin.get();
	
	// ---------------
	
	return true;
	}
	
	
	
	void
	SPQR_tree::
pertinent_graph_of_node 
	(undi_graph& ug, gdtnode v)
	{
	gdtnode sn;
	gdtedge se;
	gdt::gdtedge_map<gdtedge>  tree_edge      (ug.nodes_and_edges());
	gdt::gdtnode_array<int> status_of_node (nodes_and_edges(),0);
	
	build_undi_graph_starting_from_node (ug,v,status_of_node,se,sn,tree_edge);
	
	// if tree_edge of gdtedge se is VIRTUAL delete se from ug  
	//
	if (tree_edge_is_virtual(tree_edge[se])) ug.del_edge(se);
	}
	


	gdt::gdtlist<int>	
	SPQR_tree::
find_shortest_path_into_pertinent_graph_of_node 
	(gdtnode w, int id_v1, int id_v2, gdt::gdtlist<int>& edge_id_path)
	{
	gdt::gdtlist<int> node_id_path;
	edge_id_path.clear();
	
	undi_graph ug;
	pertinent_graph_of_node (ug,w);
	
	gdtnode v1 = ug.find_node(id_v1);
	gdtnode v2 = ug.find_node(id_v2);
	gdtnode v;
	gdtedge e;
	
	gdt::gdtnode_array<bool> reached (ug.nodes_and_edges());
	gdt::gdtnode_array<gdtedge> father  (ug.nodes_and_edges());
	gdt::gdtnode_array<int>  dist	 (ug.nodes_and_edges());
	gdt::gdtlist<gdtnode>	 order;  
	
	if ((!v1) || (!v2)) 
		gdt_error("invalid id_v1 or id_v2");	
		
	forall_nodes (v,ug) reached[v]=false; //Added by LEONFORTE 18JAN97
	ug.simple_BFS (v1,reached,father,dist,order);
	
	v = v2;
	while (v != v1) 
		{
		node_id_path.push(ug.id(v));
		e = father[v];
		edge_id_path.push(ug.id(e));
		v = ug.opposite(v,e);
		}

	node_id_path.push (ug.id(v1));
	return node_id_path;
	}
	
	
	
	gdt::gdtlist<gdtedge>
	SPQR_tree::
find_shortest_path_into_pertinent_graph_of_node
	(gdtnode w, int id_v1, int id_v2)
	{
	gdt::gdtlist<gdtedge> edge_path;				  // edges of the path into the owner graph.
	skeleton*  skp = get_skeleton_of_node (w);	  // skeleton of gdtnode w.
	plan_undi_graph& ow_pug = skp->get_owner_graph(); // owner graph.
	undi_graph ug;
	pertinent_graph_of_node (ug,w);			  // pertinent graph of the gdtnode w.
	
	gdtnode v1 = ug.find_node(id_v1);
	gdtnode v2 = ug.find_node(id_v2);
	gdtnode v;
	gdtedge e;
	
	gdt::gdtnode_array<bool> reached (ug.nodes_and_edges());
	gdt::gdtnode_array<gdtedge> father  (ug.nodes_and_edges());
	gdt::gdtnode_array<int>  dist	 (ug.nodes_and_edges());
	gdt::gdtlist<gdtnode>	 order;  
	
	if ((!v1) || (!v2)) 
		gdt_error("invalid id_v1 or id_v2");	
	
	forall_nodes (v,ug) reached[v]=false; 
	ug.simple_BFS (v1,reached,father,dist,order);
	
	v = v2;
	while (v != v1) 
		{
		e = father[v];
		edge_path.push(ow_pug.corresponding_edge(e,ug));
		v = ug.opposite(v,e);
		}
			
	return edge_path;
	}
	


	gdt::gdtlist<gdtedge>
	SPQR_tree::	
find_minimum_switches_path_into_pertinent_graph_of_node 
	(gdtnode w, int id_v1, int id_v2, int& num_switches, visit_direction_type dir)
	{
	gdt::gdtlist<gdtedge> edge_path;				  // edges of the path into the owner graph.
	skeleton*  skp = get_skeleton_of_node (w);	  // skeleton of gdtnode w.
	plan_undi_graph& ow_pug = skp->get_owner_graph(); // owner graph.
	undi_graph ug;
	pertinent_graph_of_node (ug,w);			  // pertinent graph of the gdtnode w.
	dire_graph dg (ug);
	
	gdtnode v1 = dg.find_node(id_v1);
	gdtnode v2 = dg.find_node(id_v2);
	gdtnode v;
	gdtedge e;
	gdt::gdtnode_array<int>  switches (dg.nodes_and_edges());
	gdt::gdtnode_array<gdtedge> father   (dg.nodes_and_edges());
	
	if ((!v1) || (!v2)) gdt_error("invalid id_v1 or id_v2");	
	
	dg.find_path_with_minimum_switches (v1,switches,father,dir);
	
	v = v2;
	while (v != v1) 
		{
		e = father[v];
		edge_path.push(ow_pug.corresponding_edge(e,dg));
		v = dg.opposite(v,e);
		}
	num_switches = switches[v2];
	
	return edge_path;
	}
		
	
	// -----------------
	// Update operations
	// -----------------
	
	
	void
	SPQR_tree::
clear 
	()
	{
	tree::clear ();
	local_renew ();
	}



	void
	SPQR_tree::
make_root 
	(gdtnode rv)
	{
	// cout << "\nMAKE ROOT\n" << flush;
	
	if (get_type_of_node(rv) != Q_NODE) 
		gdt_error("gdtnode is not a Q_NODE");	
		 
	if (rv != root_of_tree())
		{
		gdtnode v;
		gdtedge se;	// skeleton gdtedge
		gdtedge te;	// tree gdtedge
		skeleton* skp;
		gdt::gdtqueue<gdtnode> Q;
		
		// ------------------------------- 0 --------------------------------
		// search the gdtedge corresponding to Q_node rv, before to change root
		// ------------------------------------------------------------------
		
		te  = father_edge (rv);		       // new tree ref gdtedge
		v   = father_node (rv);
		skp = get_skeleton_of_node (v);
		se  = skeleton_edge_of_tree_edge (te); // new skeleton ref gdtedge  
		
		//cout << "\nNEW TREE REF. EDGE = " << id(te) << flush;
		
		// ------------------------------- 1 --------------------------------
		// make a root with tree metod, i.e. update only tree extern structur
		// ------------------------------------------------------------------

		tree::make_root(rv);
		
		//cout << "\nNEW TREE ROOT = " << id(rv) << flush;

		// ------------------------------- 2 --------------------------------
		// update all internal pointers of the SPQR nodes and edges, and the 
		// reference edges of the skeleton nodes.
		// ------------------------------------------------------------------
		
		skp -> set_reference_edge (se);
		//cout << "\nNEW SKEL REF. EDGE = " << skp->id(skp->get_reference_edge()) << flush;
		
		set_skeleton_edge_of_tree_edge (te,NULL);
		create_node_status (v);
		Q.append(v);
		
		while (!Q.empty())
			{
			gdtnode w;
			v = Q.pop();
			//cout << "\nEXTRACTED NODE " << id(v) << flush;
			skp = get_skeleton_of_node (v);
			//cout << "\nWITH SKELETON "; skp -> print(COMPLETE); cout << flush;
			forall_edges (se,*skp)
				if (se != skp->get_reference_edge())
					{
					//cout << "\nEXAMING EDGE" << skp->id(se) << flush;
					te = tree_edge_of_skeleton_edge_in_node (se,v);
					//cout << "\nTREE EDGE CORR. = " << id(te) << flush;
					set_skeleton_edge_of_tree_edge (te,se);
					w  = opposite (v,te);
					if (get_type_of_node(w) != Q_NODE)
						{
						skeleton* skp_w = get_skeleton_of_node (w);
						gdtedge 	  se_w  = skp -> get_twin_edge (se);
						skp_w ->  set_reference_edge (se_w);
						create_node_status (w);
						Q.append(w);
						}
					}
			}
		}
	}



	void
	SPQR_tree::
set_simpl_node_status
	(bool simplificate)
	{
	simpl_node_status = simplificate;
	}



	void
	SPQR_tree::
BB_compute_first_total_upper_bound 
	(gdtedge& re, 
	 const plan_undi_graph& pug, 
	 plan_undi_graph& best_pug, 
	 face& best_ext_face, 
	 int& UB, int& Ub,
	 BB_options& Op, 
	 algorithm_type& alg)
	{
	UB = INFINITE;
	Ub = INFINITE;
	gdtedge e;
	int  n;
	gdtnode v;
	bool flag = false;
	face ext_face = NULL_FACE;
	
	int number_of_PR_nodes = number_of_nodes_type (P_NODE) + 
				 number_of_nodes_type (R_NODE);
	
	for (int i=1; i <= Op.niu; i++)
		{
		// ------------------------
		// extract an gdtedge randomly
		// ------------------------
		//
		const gdt::gdtlist<gdtedge>& E = pug.all_edges(); 
		int j = random (0,E.size()-1); 
		e = E.inf(E.get_item(j));
		
		// cout << "\nEXTRACTED EDGE e = " << pug.id(e) << flush; 
		
		// --------------------------------------------------------------------
		// evert the SPQR-tree T and compute the corresponding base code vector
		// --------------------------------------------------------------------
		//
		gdtnode rv = Q_node_of_edge_with_id (pug.id(e));
		make_root(rv);
		gdt::gdtarray<gdtnode> base_code_vector (number_of_PR_nodes);
		BB_build_base_code_vector (base_code_vector,BB_BFS); 
		n = base_code_vector.high();
		
		// ----------------------------------------------------------------------
		// compute an assignment randomly and rebuild the corresponding embedding
		// ----------------------------------------------------------------------
		//
		BB_assignment A;
		for (int k=0; k<=n; k++)
			{
			int ass = random (0, max_status_of_node(base_code_vector[k]));
			A.append(ass); 
			}
		
		//cout << "\nEXTRACTED ASSIGNMENT:" << flush; A.print(); cout << flush; 
		
		gdt::gdtnode_array<int> status_of_node (nodes_and_edges(),BB_NULL_STATUS);	
		BB_compute_status_of_node_vector (base_code_vector, A, status_of_node);
		
		int  Ub1 = INFINITE;
		v = root_of_tree();
		v = first_son_node (v);
		
		undi_graph ug;			// temporary undi_graph
		plan_undi_graph pug1;		// temporary plan_undi_graph
		
		gdtnode start_node = NULL_NODE;
		gdtedge start_edge = NULL_EDGE;
		gdt::gdtedge_map<gdtedge> tree_edge (ug.nodes_and_edges(),NULL_EDGE);
		
		flag = build_undi_graph_starting_from_node (ug, v, status_of_node, start_edge, start_node, tree_edge);
		
		if (!flag)	// FATAL ERROR //
			cout << "\nERROR REBUILDING THE VECTOR IN COMPUTE FIRST TOTAL UPPER BOUND " << flush;
		else	
			{
			ext_face = pug1.plan_with_fixed_face_of_undi_graph (ug,start_edge,start_node);	
			
			// --------------------------------------------------------
			// compute the upper bound Ub1 as total bend cost
			// of an optimal orth./upwa. representation of pug.  
			// --------------------------------------------------------

			switch (alg)
				{
				case PLAN_ORTH_SLOW:
					{
					orth_plan_undi_graph opug (pug1,ext_face,PLAN_ORTH_OPTIMAL,false);
					Ub1 = opug.cost_of_last_algorithm();
					break;
					}
				case PLAN_UPWARD_SLOW:
					{
					upwa_plan_undi_graph upug (pug1,ext_face,PLAN_UPWARD_OPTIMAL,false);
					Ub1 = upug.cost_of_last_algorithm();
					break;		
					}
				default:
					{
					gdt_error("invalid algorithm alg");
					break;
					}
				}
			}
			
		//cout << "\nUb1 = " << Ub1 << flush;
		
		if (Ub1 < Ub) // update best solution 
			{
			Ub = Ub1;
			UB = Ub;
			re = e;
			best_pug = pug1;
			best_ext_face = best_pug.corresponding_face (ext_face,pug1);
			}
		}
		
	//cout << "\nUb = " << Ub << flush; 
	//cout << "\nUB = " << UB << flush; 
	}	


	int
	SPQR_tree::
find_best_embedding_with_external_face
	(
	plan_undi_graph& pug, 
	face& ext_face, 
	BB_options Op, 
	algorithm_type alg, 
	BB_tree_parameters* BBp
	) 
	{
	bool deallocation = false;
	gdtnode first_no_root_node = first_son_node (root_of_tree());
	
	undi_graph ug;						// pertinent graph of root gdtnode (copy of owner graph)	
	pertinent_graph_of_node (ug, first_no_root_node);	
	
	pug  	 = ug;						// plan_undi_graph copy of ug
	ext_face = pug.first_face();
	
	// -------------------------- 0 -----------------------------
	// 	creation and initialization of the variables 
	// ----------------------------------------------------------
	//
	
	//float time;
	//float dt;	
	gdtedge  init_ref_edge;
	
	if (Op.init_ref_edge != BB_NULL_EDGE) 	init_ref_edge   = pug.find_edge  (Op.init_ref_edge);
	else					init_ref_edge   = pug.first_edge ();
	
	if (!BBp) 
		{
		BBp = new BB_tree_parameters;
		deallocation = true;
		}
	
	BB_tree_parameters& BB = *BBp;
	gdt::gdtnode_array<int> BB_curr_status;

	//set<int> BB_edge_already_tried;		  // set of labels of the gdtedge already tried as reference gdtedge

	// modified by W.D. on April 17, 2003 (set -> gdtmap, in order to remove the use of leda sets
	gdt::gdtmap<int, bool> BB_edge_already_tried (false);  // set of labels of the gdtedge already tried as reference gdtedge
	
	bool  all_computations = Op.all_computations;
	
	BB_curr_status.init (nodes_and_edges(),0);
	
	gdt::gdtnode_matrix<int>        BB_l;		// pre-lower bounds type l, for not rooted tree
	gdt::gdtnode_matrix<int>        BB_L;		// pre-lower bounds type L, for rooted tree
	gdt::gdtnode_matrix<path_info>  BB_P;		// pre-lower paths

	BB_l.init (nodes_and_edges(),0);
	BB_L.init (nodes_and_edges(),0);
	BB_P.init (nodes_and_edges());
	
	// NEW CODE
	// --------------------------------
	// add ANY bend_constraints on
	// all virtual edges into skeletons
	// --------------------------------
	
	gdt::gdtlist<constraint> temp_constr;
		{
		gdtnode v;
		gdtedge e;
		skeleton* skp;
		forall_nodes (v,*this)
			{
			skp = get_skeleton_of_node (v);
			if (skp) forall_edges (e,*skp)
					if (skp -> get_type_of_edge(e) == VIRTUAL)
						temp_constr.append(skp -> new_constraint_number_of_bends_on_edge (e,ANY));
			}
		}
	// -------------------------------------------------------------------------------------------------------

	// OUTPUT
	// ------
	   if (Op.text_interface)
	   	cout << "\nWAIT FOR PREPROCESSING GLOBAL LOWER BOUNDS... " << flush;
	// ------

	//used_time (time);			    // intialize time-counter
	compute_pre_lower_bound_type_l(BB_l,alg);   // compute the preprocessing global lower bound
	//dt = used_time (time);

	// OUTPUT
	// ------
	   if (Op.text_interface)
	   	{
	   	cout << "\nPREPROCESSING GLOBAL LOWER BOUNDS OK! " << flush;
	   	//cout << "\nTIME NEEDED = " << dt << flush;
	   	}
	// ------
	
	
	// cout << "\n"; print_statistics (); cout << "\n" << flush;
	
	int C1 = 50;			 // number of unused extraction before incresing ubf
	int C2 = 20;			 // step for increasing upepr bound frequence
	int ub_update = 0;
	
	int UB;				// Total Upper Bound (updated only after each local computation)
	int Ub;				// Local Upper Bound (updated through each local computation)
	
	int total_edges;		// number of total edges of the plan_undi_graphs pug
	int remaining_edges;		// number of remaining edges
	int remaining_nodes;		// number of nodes into BB tree not still computed
	int number_of_consecutive_steps;// number of consecutive steps
	
	int	Lb;			// Lower Bound
	int	n;			// high index of base_code_vector 
	int	ubf_count;		// Upper Bound Frequence Counter

	gdt::gdtqueue<BB_assignment> Q;
	
	gdtedge		current_ref_edge;			  // current reference gdtedge;
		
	int 		number_of_PR_nodes = number_of_nodes_type (P_NODE) + 
					     number_of_nodes_type (R_NODE);
				 
	gdt::gdtarray<gdtnode> 	base_code_vector (number_of_PR_nodes);	  // base vector for rooted SPQR_tree codify

	gdt::gdtnode_array<int> status_of_node   (nodes_and_edges()),	  // status vector of the SPQR_tree nodes 
			best_solution    (nodes_and_edges());     // best solution of fixed status
			
	BB_assignment  	A;
	
	// modified by W.D. on April 17, 2003. This line seems to be unuseful at this point
	// because the set is already empty (it is never used up to now).
	//BB_edge_already_tried.clear(); 	
				
	UB = INFINITE;
	total_edges		        = pug.number_of_edges();
	BB.number_of_total_nodes        = 0;
	BB.number_of_cut_nodes          = 0;
	BB.number_of_visited_nodes      = 0;
	BB.number_of_cuts	        = 0;
	number_of_consecutive_steps     = 0;
	remaining_edges	                = total_edges;
	current_ref_edge	        = init_ref_edge;

	plan_undi_graph  best_pug;				// current best planar embedding	
	plan_undi_graph* p_best_pug = &best_pug;
	face		 best_ext_face;				// external face for best planar embedding
	
	// -------------------------
	// compute first upper bound
	// -------------------------
	//

	// OUTPUT
	// ------
	   if (Op.text_interface) 
	   	cout << "\nWAIT FOR PREPROCESSING UPPER BOUND.." << flush;
	// ------
	
	//used_time(time);

	if (all_computations)
			{
			BB_compute_first_total_upper_bound (init_ref_edge, pug, best_pug, best_ext_face, UB, Ub, Op, alg);
			if (init_ref_edge != current_ref_edge)
				{
				// evert the SPQR-tree
				//
				current_ref_edge = init_ref_edge;
				gdtnode rv = Q_node_of_edge_with_id (pug.id(init_ref_edge));
				make_root(rv);
				}
			if (Ub == 0)
      	 		    	{ 	
				//cout << "\n"; print_statistics (); cout << "\n" << flush;
				pug      = best_pug;
				ext_face = pug.corresponding_face (best_ext_face,best_pug); 
				if (deallocation) safe_delete(BBp);
				return 0;
				}
			}				
	
	//dt = used_time(time);
	
		
	
	// OUTPUT
	// ------
	   if (Op.text_interface)
	   	{ 
	   	cout << "\nPREPROCESSING UPPER BOUND OK!" << flush;
	   	//cout << "\nTIME NEEDED = " << dt << flush;
	   	}
	// ------

				
	// OUTPUT
	// ------
	   if (Op.text_interface) 
	   	cout << "\n\nSTART TOTAL COMPUTATION\n" << flush;
	// ------				
					
					 // ----------------- 
					 // TOTAL COMPUTATION 
					 // -----------------
					
	// --------------------------------------- 1 ------------------------------------------------
	// starting of local computations, forall possible ref edges or only for the initial ref_edge
	// ------------------------------------------------------------------------------------------
	
	//used_time (time);	// initialize time-counter
	
	do 
		{
		
		// -------------------------------------------------
		// Initialize variables for start local computation,
		// i.e. the computation with a new ref.gdtedge
		// -------------------------------------------------
		//
		// OUTPUT
		// ------
		   if (Op.text_interface) 
			   cout << "\n  START LOCAL COMPUTATION FOR EDGE " << pug.id(current_ref_edge) << flush;
		// ------
		
		int max_pre_lower_bound = INFINITE;
		
		compute_pre_lower_bound_type_L (BB_L,BB_l,BB_P,Op.rppl,alg);	// compute local preprocessing lower bounds
		
		BB_build_base_code_vector(base_code_vector,BB_BFS); 
		n = base_code_vector.high();
				
		BB_compute_number_of_nodes_into_BB_subtrees (base_code_vector, BB.N);
		remaining_nodes = BB.N[n];
		BB.number_of_total_nodes += remaining_nodes;
		ubf_count = 0;
		
		A.clear();		// first assignment will be empty
		Q.clear();		// clear the queue of the assignments
		Q.append(A);		// append the first assignment (empty) at the queue
		
		gdtnode w = root_of_tree();
		gdtnode v = first_son_node (w);
		max_pre_lower_bound = BB_L(v,w); // maximum pre lower bound in a local computation
			
		if (Ub == max_pre_lower_bound) 
			{
			Q.clear(); // end local computation		
				
			// OUTPUT
			// ------
			   if (Op.text_interface)
				cout << "\n  REACHED MAX PRE-LB " << flush;
			// ------
			}
		
		// ----------------------------- 2.1 -------------------------------
		// Scan the Branch and Bound tree, searching the best local solution
		// -----------------------------------------------------------------
		//
		bool stop_local_computation = (Q.empty() || (Op.ncs != BB_INF && number_of_consecutive_steps == Op.ncs));
				
		while (!stop_local_computation)
			{
			//cout << "\n"; print_statistics (); cout << "\n" << flush;
			
			BB_assignment A1; 
			int	      Ub1;
			
			// extract the assignment at the top of the queue Q
			
			A     = Q.pop();
			int k = A.size();  // number of fixed status, i.e.
					   // A fixes the 0,..,(k-1) positions into base_code_vector 
			
			// OUTPUT
			// ------
			   if (Op.text_interface)
			   	{ cout << "\n\n  EXTRACTED ASSIGNMENT : " << flush; A.print(); cout << flush; }
			// ------
			
			//if (Op.ubf == 0 || remaining_edges == total_edges || k == n+1) 
			if (Op.ubf == 0 || k == n+1) 
				{
				Ub1 = BB_compute_upper_bound (base_code_vector, A, status_of_node, total_edges, remaining_edges, BB_curr_status, Op, alg);
				ub_update ++;
				}
			else 
				if (ubf_count == Op.ubf)
					{
					Ub1 = BB_compute_upper_bound (base_code_vector, A, status_of_node, total_edges, remaining_edges, BB_curr_status, Op, alg);
					ubf_count = 0;
					ub_update ++;
					}
			     	else
			     		{
			     		Ub1 = INFINITE; 
			     		ubf_count ++; 		
			     		}
			
			// OUTPUT
			// ------
			   if (Op.text_interface) cout << "\n  Upper Bound = " << Ub1 << flush;
			// ------
			
			if (Ub1 < Ub) 
				{
				// Replace best upper bound 
				// and local best solution vector
				//
				ub_update = 0;
				Ub = Ub1;
				best_solution = status_of_node;
				
				if (Ub == max_pre_lower_bound) 
					{
					Q.clear(); //end local computation
					
					// OUTPUT
					// ------
				   	if (Op.text_interface)
			 			cout << "\n  REACHED MAX PRE-LB " << flush;
					// ------
					}
				
         			}
           		
           		if (ub_update == C1) 
           			{
           			ub_update = 0;
           			Op.ubf += C2;
           			
           			// OUTPUT
				// ------
			   	if (Op.text_interface) 
			   		cout << "\n  INCREASING UPPER BOUND FREQUENCE TO " << Op.ubf << flush;
				// ------
           			}
           		
           		// OUTPUT
           		// ------
           		   if (Op.text_interface) cout << "\n  Best Upper Bound = " << Ub << flush; 
           		// ------
				
			BB.number_of_visited_nodes ++;
			remaining_nodes --;
			
 
 			if (k <= n)
				{
				int ns = max_status_of_node (base_code_vector[k]);	
				
				for (int i=0; i<=ns; i++)  // Internal cycles of lower bounds computations
					{					
					A1.clear();
					A1 = A; 
					A1.append(i);
				
					if (k == n) 
						{
						Q.append(A1);
						
						// OUTPUT
						// ------
						   if (Op.text_interface)
						   	{cout << "\n  APPENDED ASSIGNMENT : "; A1.print();}
						// ------
						}
					else
						{
						// OUTPUT
						// ------
						   if (Op.text_interface) 
						   	{cout << "\n  Computing New Lower Bound From : "; A1.print(); cout << flush;}
						// ------
						
						Lb = BB_compute_lower_bound (base_code_vector, A1, BB_L, BB_P, total_edges, remaining_edges, Op, BB_edge_already_tried,true,alg);

						if (Lb < Ub) 
							{
							// APPENDING
							//
							// OUTPUT 
							// ------
							   if (Op.text_interface) 
							   	{
							   	cout << "\n  Lower Bound = " << Lb << flush;
							   	cout << "\n  APPENDED ASSIGNMENT" << flush;
							   	}
							    
							// ------ 
							Q.append(A1); 
							}
						else  
							{
							// CUTTING
							//
							// OUTPUT 
							// ------
							   if (Op.text_interface) 
							   	{
							   	cout << "\n  Lower Bound = " << Lb << flush;
							   	cout << "\n  CUT ASSIGNMENT" << flush;
							   	}
							    
							// ------ 
							
							int i = A1.size() - 1;	
							remaining_nodes -= BB.N[n-i-1];
							BB.number_of_cut_nodes += BB.N[n-i-1];
							BB.number_of_cuts ++; 
           						}
						}
					}
				}
		
			number_of_consecutive_steps ++;
			stop_local_computation = (Q.empty() || (Op.ncs != BB_INF && number_of_consecutive_steps == Op.ncs) || Ub == 0);
			}
			
		// OUTPUT
		// ------
		   if (Op.text_interface) 
		   	cout << "\n\n  END LOCAL COMPUTATION" << flush;
		// ------
			
		// end local computation
		
		
		if (Ub < UB) // update best solution
			{
			UB = Ub;
			undi_graph ug;
			gdtedge start_edge;
			gdtnode start_node;
			gdt::gdtedge_map<gdtedge> tree_edge (ug.nodes_and_edges(),NULL_EDGE);
			gdtnode v = root_of_tree ();
			v = first_son_node (v);
			best_ext_face = NULL_FACE;
			build_undi_graph_starting_from_node  (ug, v, best_solution, start_edge, start_node, tree_edge);
			best_ext_face = best_pug.plan_with_fixed_face_of_undi_graph (ug, start_edge, start_node);
			
			// OUTPUT
			// ------
			   if (Op.text_interface)
			   	{cout << "\n\nBEST pug :" << flush; (*p_best_pug).print (COMPLETE);}
			// ------
			}
		
		
		if (all_computations) 
			{
			// Mark the current gdtedge as already_tried,
			//
			//BB_edge_already_tried.insert(pug.id(current_ref_edge));
			// modified by W.D. on April 17, 2003
			BB_edge_already_tried[pug.id(current_ref_edge)] = true;
			
			//Choose the new current gdtedge in sequential cyclic way
			//
			current_ref_edge = pug.succ_edge (current_ref_edge);
			if (!current_ref_edge) current_ref_edge = pug.first_edge();  
			
			// Evert T with respect to new current gdtedge
			//
			gdtnode rv = Q_node_of_edge_with_id (pug.id(current_ref_edge));
			make_root(rv);
			}
			
		remaining_edges --;
		}
	while (current_ref_edge != init_ref_edge && UB != 0 && !(Op.ncs != BB_INF && number_of_consecutive_steps == Op.ncs));
	
	// end total computation
	
	//dt = used_time (time);
	
	// OUTPUT
	// ------
	   if (Op.text_interface) 
	   	{
	   	cout << "\n\nEND TOTAL COMPUTATION" << flush;
	   	//cout << "\n\nTIME NEEDED = " << dt << flush;
	   	}
	// ------	
	
	
	// ------------------------------------ 3 -------------------------------------
	// update pug with an optimal embedding and external face 
	// ----------------------------------------------------------------------------
	
	//cout << "\n"; print_statistics (); cout << "\n" << flush;
	
	pug 	 = *p_best_pug;
	ext_face = pug.corresponding_face (best_ext_face, pug);
	
	
	// NEW CODE
	// --------------------------------
	// remove all temporary constraints 
	// added on edges of skeletons
	// --------------------------------
	
	constraint c;
	forall (c,temp_constr) 
		{
		undi_graph& sk = c -> get_owner_graph();
		sk.del_constraint(c); 
		}
	// --------------------------------
	
	if (deallocation) safe_delete(BBp);
	
	return UB;
	}




	// --------------------------
	// Input / Output opearations
	// --------------------------

	
	void 
	SPQR_tree::
print 
	(SPQR_node_type t,ostream& os)
	{
	std::string ctype;

	ctype = from_SPQR_node_type_to_string (t);
	os << ctype;
	}
	


	void
	SPQR_tree::
print_node_status 
	(gdtnode v,ostream& os)
	{
	// print all status of gdtnode v.
	//
	gdtedge e;
	os << "\nPRINTING NODE STATUS" << flush;
	
	if (get_type_of_node(v) == R_NODE)
		for (int i=0; i<=1; i++)
			{
			os << "\n" << i << " STATUS : " << flush;
			//skeleton* skp = get_skeleton_of_node(v);
			if (get_status_R_node_rotation(i,v) == CLOCKWISE) os << "REVERSE ORDER IN ADJACENT LISTS" << flush;
			else						  os << "ORDER IN ADJACENT LISTS" << flush;
			//os << "\nEXTERNAL FACE = ";
			//forall (e,get_status_R_node_ext_face(i,v))  os << skp->id(e) << " ";
			//os << flush;
			}
		
	else if (get_type_of_node(v) == P_NODE)
		{
		int i = 0;
		while (!get_status_P_node_permutation(i,v).empty())
			{
			os << "\n" << i << " STATUS : " << flush;
			skeleton* skp = get_skeleton_of_node(v);
			gdt::gdtlist<gdtedge> permut = get_status_P_node_permutation(i,v);
			forall (e,permut) os << skp->id(e) << " ";
			os << flush;
			i++;
			}
		}
	}
	
	

//-----------------------------------------------------------------------------
// PROTECTED
//-----------------------------------------------------------------------------

	void
	SPQR_tree::
compute_tree_dimensions 
	(gdt::gdtnode_array<int>& width_of_subtree, 
	 gdt::gdtnode_array<int>& lev, gdt::gdtnode_array<int> width_of_node,
	 bool print_Q_nodes)
	{
	gdt::gdtlist<gdtnode> order;
	gdtnode v;
	
	post_order_of_subtree (lev, order);
	
	forall (v,order)
		if (v != root_of_tree())
			{
			if (get_type_of_node(v) == Q_NODE && !print_Q_nodes) 
				width_of_subtree[v] = 0;
			else if (width_of_subtree[v] < width_of_node[v])
				width_of_subtree[v] = width_of_node[v];
			width_of_subtree[father_node(v)] += width_of_subtree[v];
			}
	}

	

//-----------------------------------------------------------------------------
// PRIVATE
//-----------------------------------------------------------------------------

	
	void
	SPQR_tree::
local_new 
	()
	{
	//cout << "\n SPQR_tree::local_new \n" << flush;
	
	node_info = new (gdt::gdtnode_map<struct_SPQR_node_info*>);
	edge_info = new (gdt::gdtedge_map<struct_SPQR_edge_info>);
	
	node_info->init(nodes_and_edges(),NULL);
	edge_info->init(nodes_and_edges());
	
	simpl_node_status = false;
	}



	void
	SPQR_tree::
local_del
	()
	{
	//cout << "\n SPQR_tree::local_del \n" << flush;
	
	// delete all skeletons, maked dinamically.
	//
	gdtnode v;
	forall_nodes (v,*this)
		if (get_type_of_node(v) != Q_NODE)
			{
			skeleton* skp = get_skeleton_of_node (v);
			if (skp) safe_delete(skp);
			}
			
	// delete all struct_SPQR_node_info, made dinamically
	//
	forall_nodes (v,*this) safe_delete ((*node_info)[v]);
	
	// delete nodes and edges info
	//
	safe_delete(node_info); 
	safe_delete(edge_info); 
	}



	void
	SPQR_tree::
local_renew
	()
	{
	local_del(); 
	local_new(); 
	}



	void
	SPQR_tree::
local_init 
	(plan_undi_graph& pug, gdtedge e, bool simpl)
	{
	//cout << "\nSPQR_tree::local init\n" << flush;
	gdtnode v,w;
	gdtedge es = NULL_EDGE,
	     et = NULL_EDGE;
	//split_component		sc(pug);
	split_component*	scp;
	gdt::gdtlist<split_component*>  triconnected_components;
	skeleton*		skp, *sk1p;
	struct_SPQR_node_info*	nip;
	gdt::gdtqueue<gdtnode>		Q;
	int i;
	gdt::gdtlist<gdtedge>		temp_list;
	bool output = false; // set at true for debugging
	
	// When no starting gdtedge has been specified for the decomposition
	// select one of the edges (e.g. ther first) of pug. Any gdtedge would do.
	//
	if (!e) e=pug.first_edge();
	
	//cout << "\nINSERT 0 FOR NO OUTPUT AND AN OTHER INTEGER ELSE" << flush;
	//cin  >> i;
	//if (i != 0) output = true; 
	
	set_simpl_node_status (simpl);
	
	// --------------------------------------------------------------
	// compute the pug triconnected components
	// --------------------------------------------------------------
	
	scp = new split_component(pug);
	triconnected_components = scp -> decompose_into_triconnected_components();
	safe_delete(scp);
	
	if (output) cout << "\nDECOMPOSITION TERMINATED" << flush;
	
	// --------------------------------------------------------------
	// set the SPQR_tree root  
	// --------------------------------------------------------------
	
	v = root_of_tree ();
	nip = new struct_SPQR_node_info (NULL);
	(*node_info)[v] = nip;
	set_type_of_node (v, Q_NODE);
	
	if (output) cout << "\nROOT CREATED" << flush;
	
	// --------------------------------------------------------------
	// search the triconnected component containing the real gdtedge 
	// corresponding to the gdtedge e into pug and make it as the son of 
	// the root.
	// --------------------------------------------------------------
	
	forall (scp,triconnected_components)
		{
		es = scp->edge_associated_to_owner_graph_edge(e);
		if (es) break;
		}
		
	skp = new skeleton ();
	skp->steal_from (*scp,es);
		
	// ---------------------------------------------
	// deallocation added by W.D. on October-27-1999
	// ---------------------------------------------
	
	safe_delete(scp);
	
	// 
	
	v = new_son_of_node (root_of_tree(),skp);
	
	set_type_of_node (v,from_split_component_type_to_SPQR_node_type (skp->get_type()));
	create_node_status (v);
	
	et = father_edge (v);
	set_skeleton_edge_of_tree_edge (et,NULL_EDGE);
	
	if (output)
		{
		cout << "\nCREATE A NEW NODE TYPE : "; print (get_type_of_node (v)); cout << flush;
		cout << "\nNODE SKELETON : "; skp->print(COMPLETE);
		print_node_status(v);
		cin  >> i;
		if (i == 0) output = false; 
		}
			
	// --------------------------------------------------------------
	// create the SPQR_tree nodes and edges remaining
	// --------------------------------------------------------------
	
	Q.append(v);
	while (!Q.empty())
		{
		v = Q.pop();
		skp = get_skeleton_of_node(v);
		temp_list.clear();
		forall_edges (e,*skp)
			temp_list.append(e);
		//temp_list.permute();
		
		forall (e,temp_list)
			if (e != skp->get_reference_edge())
				if (skp->get_type_of_edge(e) == REAL)
					{
					// create a new Q_node and make it as v son
					//
					w = tree::new_son_of_node(v);
					nip = new struct_SPQR_node_info (NULL);
					(*node_info)[w] = nip;
					set_type_of_node (w, Q_NODE);
					et = father_edge (w);
					set_skeleton_edge_of_tree_edge (et,e);
					set_tree_edge_of_skeleton_edge_in_node (e,et,v);
					
					if (output)
						{
						cout << "\nCREATE A NEW NODE TYPE : "; print (get_type_of_node(w)); cout << flush; 
						cin  >> i;
						if (i == 0) output = false; 
						}
					}
				else 
					{
					// create a new S,P or R_node and make it as v son.
					// Therefore, put it into queue.
					//
					es   = skp->get_twin_edge(e);
					scp  = skp->get_twin_edge_owner_split_component(e);
					sk1p = new skeleton ();
					sk1p->steal_from (*scp,es);
					
					// ---------------------------------------------
					// deallocation added by W.D. on October-27-1999
					// ---------------------------------------------
	
					safe_delete(scp);
	
					// 
					
					w = new_son_of_node (v,sk1p);
					et = father_edge (w);
					set_type_of_node (w,from_split_component_type_to_SPQR_node_type (sk1p->get_type()));
					set_skeleton_edge_of_tree_edge (et,e);
					set_tree_edge_of_skeleton_edge_in_node (e,et,v);
					create_node_status (w);
					Q.append(w);
					
					if (output)
						{
						cout << "\nCREATE A NEW NODE TYPE : "; print (get_type_of_node (w)); cout << flush;
						cout << "\nNODE SKELETON : "; sk1p->print(COMPLETE);
						print_node_status (w);
						cin  >> i;
						if (i == 0) output = false;
						} 
					}
			else
				{
				et = father_edge(v);
				set_tree_edge_of_skeleton_edge_in_node (e,et,v);
				}
		}
		
	}
	
	


	void
	SPQR_tree::	
set_type_of_node
	(gdtnode v, SPQR_node_type t)
	{
	(*node_info)[v]->type = t;
	}
	
	
		
	void
	SPQR_tree::	
set_skeleton_of_node		       
	(gdtnode v, skeleton* skp)
	{
	(*node_info)[v]->skel = skp;
	}
	
	
			
	void
	SPQR_tree::	
set_tree_edge_of_skeleton_edge_in_node
	(gdtedge es, gdtedge et, gdtnode v)
	{
	(*node_info)[v]->tree_edge[es] = et;
	}
	
	
											
	void
	SPQR_tree::	
set_skeleton_edge_of_tree_edge	      
	(gdtedge et, gdtedge e)
	{
	(*edge_info)[et].edge_corr_in_father_skeleton = e;
	}				



	void
	SPQR_tree::
create_node_status
	(gdtnode v)
	{
	struct_SPQR_node_info* nip = (*node_info)[v];
	skeleton*	       skp = nip -> skel;			// gdtnode skeleton
	gdtedge 		       re  = skp -> get_reference_edge();       // reference gdtedge of skeleton sk
	gdtedge 		       e;					// utility gdtedge
	gdtnode 		       v1  = skp -> get_pole1();		// pole1 of skeleton sk
	gdtnode 		       v2  = skp -> get_pole2();		// pole2 of skeleton sk
	
	//cout << "\nEXAMING NEW NODE STATUS" << flush;
	//cout << "\nPOLE1 = " << skp -> id(v1) << "   POLE2 = " << skp -> id(v2) << flush;


	if (get_type_of_node(v) == R_NODE)
		{
		// assign 2 gdtnode status, R_status[0] and R_status[1], corresponding
		// to the two possible flipping on poles.
		//
		//cout << "\nMAKING R NODE STATUS" << flush;
		gdt::gdtmap<int,R_node_info>& node_status = (nip -> R_status);	// skeleton status information

		/*
		// ------------------------
		// 	  STATUS 0 
		// ------------------------
		//
		//		 v2 
		//	       __o
		//	      / / \
		//	     / /e1 \e2
		//	 re / o-----o
		//	    \  \e3 /e4
		//	     \  \ /	
		//            \__o
		//           	 v1 
		// ------------------------		
		*/

		node_status[0].rotation = COUNTER_CLOCKWISE;

		/*
		// ------------------------	
		//	   STATUS 1 
		// ------------------------
		//
		//		 v2 
		//	       __o
		//	      / / \
		//	     / /e2 \e1
		//	 re / o-----o
		//	    \  \e4 /e3
		//	     \  \ /	
		//            \__o
		//           	 v1 
		// ------------------------		
		*/

		node_status[1].rotation = CLOCKWISE;
		nip->maximum_status = 1;
		}
		
		
	else if (get_type_of_node(v) == P_NODE)
		{
		// create (m-1)! gdtnode status, P_status[0],...,P_status[m-1],
		// with m number of gdtedge into gdtnode skeleton.
		//
		//cout << "\nMAKING P NODE STATUS" << flush;		
		gdt::gdtmap<int,P_node_info>& node_status = (nip -> P_status);	// skeleton status information

		/*
		// -----------------------------------------------------------
		//		MAKE THE (m-1)! status
		// -----------------------------------------------------------
		//        v2			       v2
		//	__o__ [1]permutation         __o__ [(m-1)!]permutation
		//     / / \ \ 			    / / \ \
		//    / /   \ \                    / /   \ \
		// re| |e1 e2| |e3  ..........  re| |e3 e2| |e1             
		//   |  \   /  |                  |  \   /  |
		//    \  \ /  /                    \  \ /  /
		//     \__o__/                      \__o__/
		//        v1			       v1
		//      	
		// -----------------------------------------------------------	
		*/

		gdt::gdtlist<gdtedge> L;			// list of the total edges
		gdt::gdtlist<gdtedge> L_v1_v2;		// list of the real edges directed from v1 to v2
		gdt::gdtlist<gdtedge> L_v2_v1;		// list of the real edges directed from v2 to v1
		gdt::gdtlist<gdtedge> L_undi;		// list of the undirected edges
		gdt::gdtlist<gdtedge> L_simpl;		// list of the edges for equivalent classes;
		
		e = re;
		e = skp->cyclic_adj_succ(e,v2);
		while (e != re) 
			{
			L.append(e);
			if      (skp->start(e) == v1) L_v1_v2.append(e);
			else if (skp->start(e) == v2) L_v2_v1.append(e);
			else L_undi.append(e);
			//cout << "\nAPPENDED EDGE " << skp->id(e) << flush;
			e = skp->cyclic_adj_succ(e,v2);
			}
		
		// create a simplificate list of edges, 
		// grouped into equivalent classes
		//
		L_simpl.clear();
		if (!L_v1_v2.empty()) L_simpl.append(L_v1_v2.head());
		if (!L_v2_v1.empty()) L_simpl.append(L_v2_v1.head());
		forall (e,L_undi)     L_simpl.append(e);
		
		//cout << "\nSIMPLIFICATE LIST FOR SPQR-TREE NODE " << id(v) << ": " << flush;
		//forall (e,L_simpl) cout << " " << skp->id(e) << flush;
		
		
		// decide the number of elements to permutate
		//
		int number_of_elements;
		if (get_simpl_node_status()) number_of_elements = L_simpl.size();
		else		  	     number_of_elements = L.size();
		int number_of_permutations = fact(number_of_elements);
		
		
		//cout << "\nNUMBER OF EDGES = " << number_of_elements << flush;
		
		//		array<int_perm> permutation_number (number_of_permutations);
		gdt::gdtarray<gdt::gdtlist<int> > permutation_number (number_of_permutations);
		permutation_number  = find_all_permutations (number_of_elements);
		nip->maximum_status = permutation_number.high(); 
		
		for (int i=0; i<=nip->maximum_status; i++)
			{
			gdt::gdtlist<gdtedge>& L1 = node_status[i].permutation;
			gdt::gdtlist<gdtedge> L_v1_v2_temp = L_v1_v2;
			gdt::gdtlist<gdtedge> L_v2_v1_temp = L_v2_v1;
			L1.clear();
			int j;
			if (!get_simpl_node_status()) 
				forall (j,permutation_number[i]) L1.append(L.inf(L.get_item(j)));
			else
				forall (j,permutation_number[i])
					{
					e = L_simpl.inf(L_simpl.get_item(j));
					if      (skp->start(e) == v1) L1.conc(L_v1_v2_temp);
					else if (skp->start(e) == v2) L1.conc(L_v2_v1_temp);
					else L1.append(e);
					}
			
			//cout << "\nSTORED " << L1.size() << " INTO " << i << " PERMUTATION: " << flush;
			//forall (e,L1) cout << " " << skp->id(e) << flush;
			}
			
		//cout << "\nMAKED NODE STATUS" << flush;
		}
		
	else nip->maximum_status = 0;	// for S_NODE and Q_NODE
	}
	
	

	SPQR_node_type
	SPQR_tree::
from_split_component_type_to_SPQR_node_type
	(split_component_type t) const
	{
	SPQR_node_type node_type = Q_NODE;
	
	switch (t)
		{
		case POLYGON	 : node_type = S_NODE; break;
		case MAXIMAL_BOND: node_type = P_NODE; break;
		case TRICONNECTED: node_type = R_NODE; break;
		default: break;
		}
	return node_type;
	}
	
	
	
	string
	SPQR_tree::
from_SPQR_node_type_to_string
	(SPQR_node_type t) const
	{
	string ctype;
	
	switch (t)
		{
		case S_NODE: ctype = "S"; break;
		case P_NODE: ctype = "P"; break;
		case Q_NODE: ctype = "Q"; break;
		case R_NODE: ctype = "R"; break;
		}
	return ctype;
	}
	
	
	
	gdtnode
	SPQR_tree::
new_son_of_node
	(gdtnode v, skeleton* skp)
	{
	gdtnode w;
	struct_SPQR_node_info* nip;
	
	w = tree::new_son_of_node (v);
	nip = new struct_SPQR_node_info (skp);
	(*node_info)[w] = nip;
	return w; 
	}
	
	
	
	void
	SPQR_tree::
BB_build_base_code_vector 
	(gdt::gdtarray<gdtnode>& V, BB_visit_type vt)
	{
	gdt::gdtlist<gdtnode> 	order;
	gdt::gdtnode_array<int> lev (nodes_and_edges());
	gdtnode v;
	int i = 0;
	 
	switch (vt)
		{
		case BB_BFS: BFS_of_subtree (lev,order); break;
		case BB_DFS: DFS_of_subtree (lev,order); break;
		}
		
	forall (v,order)
		{
		SPQR_node_type nt;
		nt = get_type_of_node(v);
		if ((nt != Q_NODE) && (nt != S_NODE)) { V[i] = v; i++; }
		}
	}



	void
	SPQR_tree::
BB_compute_number_of_nodes_into_BB_subtrees
	(gdt::gdtarray<gdtnode>& base_code_vector, gdt::gdtmap<int,int>& N)
	{
	// THIS IS A RECURSIVE FUNCTION DEFINED AS: 
	// ----------------------------------------
	// N(0) = k(n) + 1;
	// N(i) = N(i-1)*k(n-i) + 1     0 < i < n+1
	// ----------------------------------------
	// WITH k(i) = number of sons of a gdtnode with i level.
	// IT COMPUTES THE NUMBER OF NODES INTO THE SUBTREE WITH DEPTH (i+1)
	//
	int n = base_code_vector.high();
	for (int i = 0; i <= n; i++)
		{
		int ki = (max_status_of_node(base_code_vector[n-i])) + 1;
		if (i == 0) N[i] = ki + 1;
		else	    N[i] = N[i-1]*ki +1; 
		}
	}



	BB_assignment
	SPQR_tree::
BB_generate_complete_assignment 
	(gdt::gdtarray<gdtnode>& base_code_vector, BB_assignment A, gdt::gdtnode_array<int>& BB_curr_status, BB_options& Op)
	{
	BB_assignment A1;			// complete assignment
	int n = base_code_vector.high();	// max. index of base_code_vector 
	if (A.size() == 0)
		{
		// 			- FIRST ASSIGNMENT - 
		
		if (Op.fub == BB_CURRENT)
			{
			// ------------------------------------------------------
			// assign current status forall nodes of base_code_vector
			// ------------------------------------------------------
			//
			for (int i=0; i<=n; i++) A1.append(BB_curr_status[base_code_vector[i]]);
			}
			
		else 
			{
			// ------------------------------------------------------
			// assign random status forall nodes of base_code_vector
			// ------------------------------------------------------
			//
			for (int i=0; i<=n; i++) A1.append(random(0,max_status_of_node(base_code_vector[i])));
			}
		}
	
	else	
		{
		//	             - SUCCESSIVE ASSIGNMENTS -
		
		int already_fixed_status = A.size();
		int ass;
		A1 = A;
		
		if (Op.sub == BB_NON_REP)
			{ 
			// ---------------------------------------------------------
			// complete the assignment A with all max_status of nodes if 
			// the last element of A is 0, and with all 0 otherwise.
			// ---------------------------------------------------------
			//
			int last_ass = A.tail();
			if (last_ass == 0) 
				for (int i = already_fixed_status; i <= n; i++)
				{
				ass = max_status_of_node(base_code_vector[i]);  
				A1.append(ass);
				}
			else	for (int i = already_fixed_status; i <= n; i++) A1.append(0);
			}
		else
			{
			// ---------------------------------------------------------
			// complete the assignment A with random gdtnode status
			// ---------------------------------------------------------
			//
			for (int i = already_fixed_status; i <= n; i++)
				{
				ass = random (0, max_status_of_node(base_code_vector[i]));
				A1.append(ass);
				}
			}
		} 
	return A1;
	}



	void
	SPQR_tree::
BB_compute_status_of_node_vector
	(gdt::gdtarray<gdtnode>& base_code_vector, BB_assignment A, gdt::gdtnode_array<int>& status_of_node)
	{
	int n = A.size();
	gdtnode v;
	forall_nodes (v,*this) status_of_node[v] = BB_NULL_STATUS;
	for (int i=0; i<=n-1; i++)
		{
		v = base_code_vector[i];
		int j = A.inf(A.get_item(i));
		status_of_node[v] = j;
		}
		
	// CHECKING
	// --------
	// cout << "\nStatus of gdtnode vector just calculated : " << flush;
	// forall_nodes (v,*this) cout << status_of_node[v] << " ";
	// cout << flush;    
	}
	
	
	
	int  
	SPQR_tree::
BB_compute_upper_bound 		     
	(
	 gdt::gdtarray<gdtnode>& base_code_vector, 
	 BB_assignment A, 
	 gdt::gdtnode_array<int>& status_of_node, 
	 int total_edges, int remaining_edges, 
	 gdt::gdtnode_array<int>& BB_curr_status, 
	 BB_options& Op, 
	 algorithm_type alg
	)
	{
	//cout << "\nstart computation upper bound from : "; A.print(); cout << flush;
	
	int Ub  = INFINITE;
	int old_fub = Op.fub;
	int number_of_trials = 1; 
	
	BB_assignment A1;
	
	undi_graph ug;		// temporary undi_graph
	plan_undi_graph  pug;	// temporary plan_undi_graph
	
	gdtedge start_edge;
	gdtnode start_node;
	gdtnode v;
	face ext_face;
	gdt::gdtedge_map<gdtedge> tree_edge (ug.nodes_and_edges(),NULL_EDGE);
	gdt::gdtnode_array<int> status_of_node_temp (nodes_and_edges(),BB_NULL_STATUS);
	
	if ((A.size() == 0) && (remaining_edges == total_edges) && (!Op.all_computations)) number_of_trials = Op.niu;	// first upper bounds
	else		  					 			   number_of_trials = 1; 		// successive upper bounds
	
	for (int i=1; i <= number_of_trials; i++)
		{
		// -------------------------- 1 ---------------------------
		// complete the assignment A forall P and R nodes
		// --------------------------------------------------------

		A1 = BB_generate_complete_assignment (base_code_vector, A, BB_curr_status, Op);
		if (number_of_trials == 1) BB_compute_status_of_node_vector (base_code_vector, A1, status_of_node);
		else	
			{		  
			BB_compute_status_of_node_vector ( base_code_vector, A1, status_of_node_temp);
			Op.fub = BB_RANDOM;	// the other initial upper bounds will be random
			}

			// CHECKING
			// --------
			// cout << "\ngenerate complete assignment : "; A1.print (); cout << flush; 
			// cout << "\nstatus nodes computed : " << flush;
			// if (number_of_trials == 1)
			// 	  forall_nodes (v,*this) cout << status_of_node[v] << " ";
			// else   forall_nodes (v,*this) cout << status_of_node_temp[v] << " ";
			// cout << flush;
			// --------

		// -------------------------- 2 ------------------------------
		// build the plan_undi_graph corresponding to the fixed status
		// -----------------------------------------------------------
		
		bool flag;
		int  Ub1 = 0;
		
		v = root_of_tree();
		v = first_son_node (v);

		if (number_of_trials == 1)
			flag = build_undi_graph_starting_from_node (ug,v,status_of_node,start_edge,start_node,tree_edge);
			
		else	
			{
			ug.clear();
			tree_edge.init (ug.nodes_and_edges(),NULL_EDGE);		  
			flag = build_undi_graph_starting_from_node (ug,v,status_of_node_temp,start_edge,start_node,tree_edge);
			}
			
		
		if (!flag)	// FATAL ERROR //
			{
			cout << "\nERROR REBUILDING THE VECTOR : " << flush;
			gdtnode v;
			forall_nodes (v,*this) cout << status_of_node[v] << " " << flush;
			cout << "\nWHEN remaining_edges = " << remaining_edges << "\n" << flush;
			Ub1 = INFINITE;
			}
		else	
			{
			ext_face = pug.plan_with_fixed_face_of_undi_graph (ug,start_edge,start_node);	
		

			// CHECKING
			// --------	
			// cout << "\nEMBEDDING : \n" << flush; pugp -> print();
			// cout << "\nEXTERNAL FACE = " << (pugp -> id(ext_face)) << flush;
			// --------

			// -------------------------- 3 ---------------------------
			// compute and return the upper bound Ub as total bend cost
			// of an optimal representation of pug.  
			// --------------------------------------------------------
			
			switch (alg)
				{
				case PLAN_ORTH_SLOW:
					{
					orth_plan_undi_graph opug (pug,ext_face,PLAN_ORTH_OPTIMAL,false);					
					Ub1 = opug.cost_of_last_algorithm();
					break;
					}
					
				case PLAN_UPWARD_SLOW:
					{
					upwa_plan_undi_graph upug (pug,ext_face,PLAN_UPWARD_OPTIMAL,false);
					Ub1 = upug.cost_of_last_algorithm();
					break;
					}
				default:
					{
					gdt_error("invalid algorithm alg");
					break;
					}
				}
			}

			// CHECKING
			// --------
			// cout << "\nupper bound = " << Ub << flush;
			// --------
			
		if (number_of_trials == 1) Ub = Ub1; 
		else if (Ub1 < Ub)
			{
			gdtnode v;
			Ub = Ub1;
			forall_nodes (v,*this) status_of_node[v] = status_of_node_temp[v];
			}
		}
	
	Op.fub = old_fub;	
	return Ub;
	}



	int
	SPQR_tree::
BB_compute_lower_bound
	(
	 gdt::gdtarray<gdtnode>& base_code_vector, 
	 BB_assignment A, 
	 gdt::gdtnode_matrix<int>& BB_L, 
	 gdt::gdtnode_matrix<path_info>& BB_P,
	 int total_edges, int remaining_edges, 
	 BB_options& Op, 
	 //set<int>& BB_edge_already_tried, 
	 // modified by W.D. on April 17, 2003
	 gdt::gdtmap<int,bool>& BB_edge_already_tried,
	 bool replace_with_path,
	 algorithm_type alg
	) 
	{
	int Lb1 = 0;	// lower bound of graph fixed part
	int Lb2 = 0;	// lower bound of graph variable part
	int Lb  = 0;	// total lower bound (Lb1 + Lb2)
	
	bool using_paths = false;
	
	gdt::gdtqueue<gdtnode> Q;
	SPQR_node_type nt;
	gdt::gdtnode_array<int> status_of_node (nodes_and_edges(),BB_NULL_STATUS);
	
	undi_graph ug;
	gdtedge start_edge;	
	gdtedge te;		
	gdtedge e_ug, e_pug;	
	gdtnode start_node;
	gdtnode v;
	gdt::gdtedge_map<gdtedge> tree_edge (ug.nodes_and_edges(),NULL_EDGE);
	gdt::gdtmap<int,bool>  null_cost (false);
			 
	//gdt::gdtlist<int> ZV;	// list af all vertices in ug before replacing with paths
	
	BB_compute_status_of_node_vector (base_code_vector, A, status_of_node);
		
	v  = root_of_tree ();
	v  = first_son_node (v);
	nt = get_type_of_node (v);
	
	skeleton        *skp   = get_skeleton_of_node (v);
	plan_undi_graph ow_pug = skp->get_owner_graph ();
	
		
	if (alg == PLAN_UPWARD_SLOW) using_paths = true;
	
	// --------------------------------- 1 ---------------------------------
	// scan the first SPQR_tree gdtnode different from a Q_NODE. If it is an
	// S_NODE or a gdtnode with fixed status, then merge it with ug and append
	// it into the queue Q.
	// ---------------------------------------------------------------------
	//
	
	if ((nt == S_NODE) || (status_of_node[v] != BB_NULL_STATUS))
		{
		//cout << "\nMERGING NODE " << T->id(v) << flush;
		
		merge_undi_graph_with_skeleton_of_node (ug,v,status_of_node[v],start_edge,start_node,tree_edge);
		
		//cout << "\nnode merged\n" << flush;
		//cout << "\nUNDI GRAPH\n "; ug.print(COMPLETE); cout << "\n" << flush;
		//cout << "\nMAX EDGE ID = " << ug.get_max_edge_id() << flush; 

		// --------
		// new code
		// -----------------------------------------------
		// add all new nodes of the skeleton(w) to ZV list
		// -----------------------------------------------
		/*the idea seems to be not totally correct
		skeleton* skp_v = get_skeleton_of_node (v);
		gdtnode u1 = skp_v -> get_pole1();
		gdtnode u2 = skp_v -> get_pole2();
		gdtnode u;
		forall_nodes (u,*skp_v) if (u != u1 && u != u2) ZV.append(skp_v->id(u));
		*/
		//----------------------------------------------------------------------		
		
		Q.append(v);
		}
		
	// --------------------------------- 2 ---------------------------------
	// merge with ug all remaning SPQR_tree nodes with fixed status and all
	// S_NODE reacheble by them.
	// At the end of merging, ug will be the fixed part of graph.
	// ---------------------------------------------------------------------
	//
	
	while (!Q.empty())
		{
		v = Q.pop();
		gdtnode w;
		forall_son_nodes (w,v,*this)
			{
			nt = get_type_of_node (w);
			if ((nt == S_NODE) || (status_of_node[w] != BB_NULL_STATUS))
				{
				// --------
				// new code
				// -----------------------------------------------
				// add all new nodes of the skeleton(w) to ZV list
				// -----------------------------------------------
				/*the idea seems to be not totally correct
				skeleton* skp_w = get_skeleton_of_node (w);
				gdtnode u1 = skp_w -> get_pole1();
				gdtnode u2 = skp_w -> get_pole2();
				gdtnode u;
				forall_nodes (u,*skp_w) if (u != u1 && u != u2) ZV.append(skp_w->id(u));
				*/
				// ---------------------------------------------------------------------
				//cout << "\nMERGING NODE " << id(w) << flush;
				if (!merge_undi_graph_with_skeleton_of_node (ug,w,status_of_node[w],start_edge,start_node,tree_edge))
					{
					cout << "\nMERGE FATAL ERROR IN LOWER BOUND COMPUTATION " << flush;
					return 0;
					}
				
				Q.append(w);
				}
				
			else if (nt != Q_NODE) 
				{
				te = father_edge (w);   // te = (w,v)
								
				if (BB_L(w,v) != 0) 
					{
					if (Lb2 == INFINITE || BB_L(w,v) == INFINITE) Lb2 =  INFINITE;
					else					      Lb2 += BB_L(w,v);
					if (alg == PLAN_ORTH_SLOW) using_paths = false;
					}
			     	else if (alg == PLAN_ORTH_SLOW) using_paths = replace_with_path;
			     	
			     	if (using_paths) 
			     		{
			     		// --------------------------------------------------
			     		// find a shortest path into the pertinent graph 
			     		// of the gdtnode w between its poles.
			     		// The virtual gdtedge (w,v) of ug is replaced
			     		// with such a path of real edges. 
			     		// NOTE: this part of implementation presents both a
			     		// positive side and a negative side:
			     		// infact it makes more efficient the lower bound
			     		// value but, at the same time, it increases the
			     		// computation time.
			     		// --------------------------------------------------
			     		//
			     					     		
			     		gdt::gdtlist<gdtedge> sp; 
			     		skeleton *skp = get_skeleton_of_node (w);
					plan_undi_graph& ow_pug = skp-> get_owner_graph ();
			     		
			    		switch (BB_P(w,v).current_path)
			    			{
			    			case SP       : sp = BB_P(w,v).sp;    break;
			    			case MSP12    : sp = BB_P(w,v).msp12; break;
			    			case MSP21    : sp = BB_P(w,v).msp21; break;
			    			case NULL_PATH: gdt_error("no path selected");
			    			}
			    			
			     		forall_edges (e_ug,ug) if (tree_edge[e_ug] == te) break;
			     		tree_edge[e_ug] = NULL_EDGE;
			     		
			     		gdt::gdtlist<gdtedge> path_edges = ug.replace_edge_with_path_of_graph (e_ug,sp,ow_pug);
			     		ug.del_constraints_type (SAME_FACE_NODES);
			     		ug.del_constraints_type (SAME_FACE_ORDERED_NODES);
			     		forall (e_ug,path_edges) tree_edge[e_ug] = NULL_EDGE;
			     		
			     		if (alg == PLAN_UPWARD_SLOW && !BB_L(v,w)) 
			     			forall (e_ug,path_edges) null_cost[ug.id(e_ug)]=true;
			     		
			     		//cout << "\nUNDI GRAPH\n "; ug.print(COMPLETE); cout << "\n" << flush;
			     		//cout << "\nMAX EDGE ID = " << ug.get_max_edge_id() << flush;
			     		}
			     	}
			}
		}
			
	// --------------------------------- 3 ---------------------------------
	// Test if the embedding has been already tried in previous computations
	// ---------------------------------------------------------------------
	//
	
	plan_undi_graph  pug;			// temporary plan_undi_graph
	plan_undi_graph* pugp = &pug;
	face ext_face;
	
	ext_face = pug.plan_with_fixed_face_of_undi_graph (ug,start_edge,start_node);
	
	if (remaining_edges < total_edges) 	// Check from the second computation 
		{
		gdt::gdtedge_map<gdtedge> edge_corr_in_ug  (pugp->nodes_and_edges());
		pugp -> build_mapping_edge_to_edge_with_undi_graph (ug,edge_corr_in_ug);
		forall_face_edges (e_pug,ext_face)
			{
			e_ug = edge_corr_in_ug[e_pug];
			te   = tree_edge[e_ug];
			//if (te && !(tree_edge_is_virtual(te)) && BB_edge_already_tried.member(pugp->id(e_pug))) 
			// modified by W.D. on April 17, 2003
			if (te && !(tree_edge_is_virtual(te)) && BB_edge_already_tried[pugp->id(e_pug)]) 
				{
				// OUTPUT
				// ------
				   if (Op.text_interface)
				   	cout << "\n  EDGE " << pugp->id(e_pug) << " ALREADY TRIED" << flush;
				// ------
				return INFINITE;
				}
			}
		}
	
	
	// --------------------------------- 4 ---------------------------------
	// compute the bend cost in an optimal representation of the fixed
	// ug embedding, where some edges have null cost.
	// This cost is stored in L1.
	// ---------------------------------------------------------------------

	switch (alg)
		{
		case PLAN_ORTH_SLOW:
			{
			orth_plan_undi_graph opug (pug,ext_face,PLAN_ORTH_OPTIMAL,false);
			Lb1 = opug.cost_of_last_algorithm();
			
			// --------
			// new code
			// --------
			// Search for an improvement of the Lb1 lower bound.
			// The basic idea is to add, when it is possible, a
			// number of bends due to zero angles around vertices
			// that are a constant number in any case.
			// --------------------------------------------------
			/*the idea seems to be not totally correct
			   {
			   // ------------------------------------------------
			   // determine the set Z of vertices with degree > 4 
			   // Also, determine k coefficient;
			   // ------------------------------------------------
			   
			    gdtnode z;	// gdtnode in opug

			    int k = 0;
			    gdt::gdtnode_map<bool> zero_vertex (opug.nodes_and_edges(),false); // set Z
			    
			    int idv;
			    forall (idv,ZV)
				{
				gdtnode z_ow = ow_pug.find_node (idv);
				if (ow_pug.degree(z_ow) > 4) 
					{
					z = opug.find_node(idv);
					zero_vertex[z] = true;
					k += ow_pug.degree(z_ow) - 4;
					}
				}

			    // -----------------------
			    // determine d coefficient
			    // and update lower bound
			    // -----------------------

			    int d = 0;
			    gdtedge e;	// gdtedge of opug
			    gdtedge e_ug;	// gdtedge of ug
			    gdtedge te;	// gdtedge of *this T
			    
			    forall_edges (e,opug)
				    {
				    int h = 0; // number of extremal vertices of e in Z
				    gdtnode z1 = opug.source(e);
				    gdtnode z2 = opug.target(e);
				    if (zero_vertex[z1]) h++;
				    if (zero_vertex[z2]) h++;
					
				    e_ug = ug.corresponding_edge(e,opug);
				    te   = tree_edge[e_ug];

				    if (!te || !tree_edge_is_virtual(te))	// real gdtedge
					    {
					    int p = opug.number_of_bends(e);
					    if (h == 1 && p >=1) 	 d++;
					    else if (h == 2)
						    if (p > 1) 	 d += 2;
						    else if (p == 1) d++;
					    }
				    else 					// virtual gdtedge
					    {
					    gdtnode v = father_node (te); 
					    gdtnode w = opposite(v,te);
					    if (h != 0) d += BB_L(w,v);
				    	    }
				    }

			    if (k > d)
				    {
				    //cout << "\nAdding to Lb1 the value " << (k-d) << flush;
				    Lb1 += (k-d);
				    }
			    }*/
			
			break;
			}
			
		case PLAN_UPWARD_SLOW:
			{
			gdtedge e;
			forall_edges (e,pug)
				if (null_cost[pug.id(e)])
					{
					pug.del_all_constraints_involving_edge (e);
					pug.new_constraint_number_of_bends_on_edge(e,ANY);
					}
			upwa_plan_undi_graph upug (pug,ext_face,PLAN_UPWARD_OPTIMAL,false);
			Lb1 = upug.cost_of_last_algorithm();
			break;
			}
			
		default:
			{
			gdt_error("invalid algorithm");
			break;
			}
		}
	
	if (Lb1 == INFINITE || Lb2 == INFINITE) Lb = INFINITE;
	else					Lb = Lb1 + Lb2;
	return Lb;
	}
	


	
	
	

	
	
	
	
	
	
	
	
