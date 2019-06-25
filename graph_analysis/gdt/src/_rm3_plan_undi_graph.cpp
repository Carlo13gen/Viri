/*******************************************************************************
+
+  _rm3_plan_undi_graph.c
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

//#include <stdlib.h>
#include <assert.h>
//#include <stdio.h>
//#include <time.h>

#include <GDT/gdtnode_matrix.h>
#include <GDT/gdtqueue.h>
#include <GDT/rm3_global.h>
#include <GDT/rm3_undi_graph.h>
#include <GDT/rm3_flow_dire_graph.h>
#include <GDT/rm3_plan_undi_graph.h>
#include <GDT/gdt_error.h>

extern void* debug_pointer1;
extern void* debug_pointer2;
using namespace std;

//#define DEBUG
//-----------------------------------------------------------------------------
// plan_undi_graph:	base class for all planar undirected graphs
// 			with planar embedding.
//
// W.Didimo, A.Leonforte (1996)
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// separation pair
//-----------------------------------------------------------------------------

	struct_separation_pair::
~struct_separation_pair
	()
	{
	//face f;
	shared_faces.clear();
	}

//-----------------------------------------------------------------------------
// face
//-----------------------------------------------------------------------------	
		
	struct_face::
~struct_face
	()
	{
	border_step s;
	// Border_steps must be deleted only when they still belong to the face being deleted.
	// Sometimes, e.g. during a face split, border_steps are moved from one face to another one,
	// and in that case they must NOT be deleted when the old owner face is deleted.
	// 
	forall (s, border_steps)
		if (s->owner_face == this) 
		{
			safe_delete(s);
		}
		//else
		//{
		//cout << "CAUTION: border step " << s << " has not been removed" << endl;
		//}	

	}
	
	
	
	struct_face::
struct_face
	()
	{
	id = NULL_ID;
	owner = NULL;
	pos_in_f_list = NULL;
	}
	
	
	
	plan_undi_graph&
	struct_face::
get_owner
	()
	{
	return *owner;
	}



//-----------------------------------------------------------------------------
// struct_plan_edge_info
//-----------------------------------------------------------------------------

	struct_plan_edge_info::
struct_plan_edge_info
	()
	{
	//cout << "\n struct_plan_edge_info () creator" << flush;
	left_face              = NULL_FACE;
	right_face             = NULL_FACE;
	left_face_border_step  = NULL_BORDER_STEP;
	right_face_border_step = NULL_BORDER_STEP;
	pos_in_left_face_edges = NULL;
	pos_in_right_face_edges = NULL;
	}



//-----------------------------------------------------------------------------
// plan_undi_graph
//-----------------------------------------------------------------------------



	plan_undi_graph::
plan_undi_graph
	()
	{
	//cout << "\n plan_undi_graph () creator" << flush;
	gdtnode v,w;
	local_new();
	v=new_node();
	w=new_node();
	new_edge(v,w);
	local_init(DO_NOT_PRESERVE_EMBEDDING, true);
	}



	plan_undi_graph::
~plan_undi_graph
	()
	{
	local_del();
	}



	plan_undi_graph::
plan_undi_graph
	(const undi_graph& ug, planarize_options po, bool err_mess)
	{
	#ifdef DEBUG
	cout << "\n plan_undi_graph (undi_graph) creator \t" << flush;
	cout << "this=" << this << endl;
	#endif
	undi_graph::operator=(ug);
	local_new();
	local_init(po,err_mess);
	#ifdef DEBUG
	cout << "End of plan_undi_graph (undi_graph) creator" << endl;
	#endif
	}



	plan_undi_graph::
plan_undi_graph
	(const plan_undi_graph& pug)
	{
	//cout << "\n plan_undi_graph (plan_undi_graph) creator" << flush;
	local_new();  // required because operator= calls renew()
	operator=(pug);
	}



	plan_undi_graph&
	plan_undi_graph::
operator=
	(const undi_graph& ug)
	{
	//cout << "\n undi_graph -(copy)-> plan_undi_graph" << flush;
	if (!ug.is_connected())
		gdt_error("not connected");
	undi_graph::operator=(ug);
	local_renew();
	local_init(DO_NOT_PRESERVE_EMBEDDING, true);
	return *this;
	}



	plan_undi_graph&
	plan_undi_graph::
operator=
	(const plan_undi_graph& pug)
	{
	// the undi_graph-part of *this is built according
	// to the corresponding undi_graph-part of pug.
	//

	face f,fc; // face, face-copy
	border_step s;

	//cout << "\n plan_undi_graph -(copy)-> plan_undi_graph" << flush;

	undi_graph::operator=(pug);
	local_renew();

	forall_faces(f,pug)
		{
		fc = new_face(pug.id(f));
		forall_face_border_steps(s,f)
			set_right_face_moving_along_edge
				(
				find_edge(pug.id(s->owner_edge)),
				find_node(pug.id(s->start_node)),
				fc
				);
		}
	return *this;
	}
	
	

	// -----------------
	// Access operations
	// -----------------
	
	void 
	plan_undi_graph::
local_get
	(
	gdt::gdtlist<face>*& 			  p1,         
	gdt::gdtedge_map<struct_plan_edge_info>*& p2,
	gdt::gdtmap<int,face>*& 		  p3,
	int&	 			  p4,
	int&				  p5
	)
	{
	p1 = f_list;
	p2 = edge_info;
	p3 = face_with_id;
	p4 = max_face_id;
	p5 = last_crosses;
	}
	
	

	int
	plan_undi_graph:: 
generate_face_id
	()
	{
	return ++max_face_id;
	}



	int 
	plan_undi_graph::
id
	(gdtnode v) const
	{
	return undi_graph::id (v);
	}



	int
	plan_undi_graph::
id
	(gdtedge e) const
	{
	return undi_graph::id (e);
	}

      

	int 
	plan_undi_graph::
id
	(face f) const
	{
	return f->id;
	}
	
	


	int
	plan_undi_graph::
get_max_face_id
	() const
	{
	return max_face_id;
	}
	
	
	
	int
	plan_undi_graph::
degree
	(gdtnode v) const
	{
	return undi_graph::degree(v);
	}



	int
	plan_undi_graph::
degree
	(face f,bool bridge) const 
	{
	if (!bridge) return f->edges.size();
	else	     return f->border_steps.size();
	}



	int
	plan_undi_graph::
number_of_faces
	() const
	{
	return f_list->size();
	}
	
	
	
	int
	plan_undi_graph::
number_of_cross_nodes
	() const
	{
	int crosses = 0;
	if (last_crosses == INFINITE) crosses = INFINITE;
	else
		{
		gdt::gdtlist<gdtnode> Lv = find_nodes_marked_as (RM3_CROSS_ON_REAL_EDGE);
		crosses = Lv.size();
		}
	return crosses;
	}
	
	
		
	bool
	plan_undi_graph::
node_belongs_to_face 
	(gdtnode v, face f) const
	{
	// return true if gdtnode v belong to face f.
	//
	gdtedge e;
	
	forall_edges_adjacent_node (e,v,*this)
		if (edge_belongs_to_face (e,f)) return true;
	return false;
	}



	bool
	plan_undi_graph::
edge_belongs_to_face
	(gdtedge e, face f) const
	{
	// return true if gdtedge e belong to face f.
	//
	gdtnode v = source(e);
	
	if ((left_face_moving_along_edge (e,v) == f) || 
	   (right_face_moving_along_edge (e,v) == f))
		return true;
	return false;
	}
	
	
	
	gdtnode
	plan_undi_graph::
separation_pair_pole1 
	(separation_pair sp) const
	{
	return sp->pole1;
	}



	gdtnode
	plan_undi_graph::
separation_pair_pole2
	(separation_pair sp) const
	{
	return sp->pole2;
	}
	


	face
	plan_undi_graph::
find_face
	(int ref_id) const
	{
	return (*face_with_id)[ref_id];
	}
	
	
	
	
	face
	plan_undi_graph:: 
first_face
	() const
	{
	if (!f_list->empty()) return f_list->head();
	else		      return NULL_FACE;
	}



	face
	plan_undi_graph::
last_face
	() const
	{
	if (f_list->empty()) return NULL_FACE;
	else return f_list->tail();
	}



	face
	plan_undi_graph::
succ_face
	(face f) const
	{
	gdt::list_item it = f_list->succ(f->pos_in_f_list);
	return (it) ? f_list->contents(it) : nil;
	}
	
	
	
	face
	plan_undi_graph::
pred_face
	(face f) const
	{
	gdt::list_item it = f_list->pred(f->pos_in_f_list);
	return (it) ? f_list->contents(it) : nil;
	}
	
	
	
	face
	plan_undi_graph::
adj_face
	(face f, gdtedge e) const
	{
	struct_plan_edge_info efi =(*edge_info)[e];
	if      (f == efi.left_face) return efi.right_face;
	else if (f == efi.right_face) return efi.left_face;
	else gdt_error("invalid face or gdtedge");
	return efi.left_face; // Avoids compiler warning
	}
	
	
	
	
	face
	plan_undi_graph::
right_face_moving_along_edge
	(gdtedge e, gdtnode v) const
	{
	// Returns the face visible on the right
	// moving along gdtedge e from v to its opposite.
	//
	if      (v==source(e))  return ((*edge_info)[e]).right_face;
	else if (v==target(e))  return ((*edge_info)[e]).left_face;
	else gdt_error("invalid gdtnode or gdtedge");
	return NULL_FACE;
	}



	face
	plan_undi_graph::
left_face_moving_along_edge
	(gdtedge e, gdtnode v) const
	{
	// Returns the face visible on the left
	// moving along gdtedge e from v to its opposite.
	//
	if      (v==target(e)) return ((*edge_info)[e]).right_face;
	else if (v==source(e)) return ((*edge_info)[e]).left_face;
	else gdt_error("invalid gdtnode or gdtedge");
	return NULL_FACE;
	}



	face
	plan_undi_graph::
face_of_border_step
	(border_step s) const
	{
	return s->owner_face;
	}
	
	

	const gdt::gdtlist<face>& 
	plan_undi_graph::
all_faces
	() const
	{
	return (*f_list);
	}



	gdtedge
	plan_undi_graph::
first_adj_edge
	(gdtnode v) const
	{
	return undi_graph::first_adj_edge(v);
	}
	
	
	
	gdtedge
	plan_undi_graph::
first_adj_edge (face f) const
	{
	if (!f->edges.empty()) return f->edges.head();
	else		       return NULL_EDGE;
	}
	
	
	
	gdtedge
	plan_undi_graph::
last_adj_edge (gdtnode v) const
	{
	return undi_graph::last_adj_edge(v);
	}



	gdtedge
	plan_undi_graph::
last_adj_edge
	(face f) const
	{
	if (f->edges.empty()) return NULL_EDGE;
	else return f->edges.tail();
	}



	gdtedge 
	plan_undi_graph::
adj_pred
	(gdtedge e, gdtnode v) const
	{
	return undi_graph::adj_pred(e,v);
	}
	
	

	gdtedge
	plan_undi_graph::
adj_pred
	(gdtedge e, face f) const
	{
	gdt::gdtlist<gdtedge>& be=f->edges;
	gdt::list_item pos = pos_in_border(e,f);
	pos = be.pred(pos);
	if (pos) return be.contents(pos);
	else return NULL_EDGE;
	}
	
	

	gdtedge 
	plan_undi_graph::
adj_succ
	(gdtedge e, gdtnode v) const
	{
	return undi_graph::adj_succ(e,v);
	}
	
	
	
	gdtedge
	plan_undi_graph::
adj_succ
	(gdtedge e, face f) const
	{
	gdt::gdtlist<gdtedge>& be=f->edges;
	gdt::list_item pos = pos_in_border(e,f);
	pos = be.succ(pos);
	if (pos) return be.contents(pos);
	else return NULL_EDGE;
	}
	
	

	gdtedge 
	plan_undi_graph::
cyclic_adj_pred
	(gdtedge e, gdtnode v) const
	{
	return undi_graph::cyclic_adj_pred(e,v);
	}
	
	
	
	gdtedge
	plan_undi_graph::
cyclic_adj_pred
	(gdtedge e, face f) const
	{
	gdt::gdtlist<gdtedge>& be=f->edges;
	return be.contents(be.cyclic_pred(pos_in_border(e,f)));
	}
	

	
	gdtedge 
	plan_undi_graph::
cyclic_adj_succ
	(gdtedge e, gdtnode v) const
	{
	return undi_graph::cyclic_adj_succ(e,v);
	}
	
	
	
	gdtedge
	plan_undi_graph::
cyclic_adj_succ
	(gdtedge e, face f) const
	{
	gdt::gdtlist<gdtedge>& be=f->edges;
	return be.contents(be.cyclic_succ(pos_in_border(e,f)));
	}
	


	gdtedge
	plan_undi_graph::
edge_of_border_step
	(border_step s) const
	{
	return s->owner_edge;
	}
	
	

	gdt::gdtlist<gdtnode>
	plan_undi_graph::
nodes_shared_by_faces
	(face f1, face f2) const
	{
	// returns all nodes belonging to face f1 and face f2;
	// if f1=f2, all nodes into f1 are returned.
	//
	gdtnode v;
	border_step s;
	gdt::gdtlist<gdtnode> result;
	
	forall_face_border_steps (s,f1)
		{
		v = start_of_border_step (s);
		if (node_belongs_to_face(v,f2)) result.append (v);
		}
	return result;
	}



	gdt::gdtlist<gdtnode>
	plan_undi_graph::
adj_nodes
	(face f) const
	{
	// returns all nodes belonging to face f;
	//
	border_step s;
	gdt::gdtlist<gdtnode> result;
	forall_face_border_steps (s,f) result.append (start_of_border_step (s));
	return result;
	}
	

	
	gdt::gdtlist<gdtedge>
	plan_undi_graph::
adj_edges
	(gdtnode v) const
	{
	return undi_graph::adj_edges(v);
	}
	


	gdt::gdtlist<gdtedge>
	plan_undi_graph::
adj_edges
	(face f, gdtnode v) const
	{
	gdtedge e;
	gdt::gdtlist<gdtedge> l, le;
	//border_step bs;

 	if (v != NULL_NODE)
		{
		if (!node_belongs_to_face(v,f))  gdt_error("invalid gdtnode");
		else
			{
			l = adj_edges(v);
			forall(e,l)
				if (edge_belongs_to_face (e,f)) le.append(e); 
			}
		}
	else le = f->edges;
	
	return le;
	}



	gdt::gdtlist<face>
	plan_undi_graph::
adj_faces
	(gdtnode v) const
	{
	gdt::gdtlist<face> lf;
	face f;
	
	forall_faces(f,*this)
		if (node_belongs_to_face(v,f))  lf.append(f);
	
	return lf;
	}



	gdt::gdtlist<gdtedge> 
	plan_undi_graph::
edges_shared_by_faces 
	(face f1, face f2) const 
	{
	// returns all edges belonging to face f1 and face f2;
	// if f1=f2, all bridges into face f1 are returned.
	//
	gdtedge e;
	face lf,rf;
	gdt::gdtlist<gdtedge> result;
	struct_plan_edge_info efi;
	
	forall_face_edges (e,f1)
		{
		efi=(*edge_info)[e];
		rf = efi.right_face;
		lf = efi.left_face;
		if ((f1==rf)&&(f2==lf) || (f1==lf)&&(f2==rf))
			result.append(e);	
		}
	return result;		
	}
	


	gdt::gdtlist<gdtedge> 
	plan_undi_graph::
edges_entering_node_while_moving_around_face
	(gdtnode v, face f) const
	{
	// return all edges e in f such that e and succ(e) in f
	// are incident on v.
	//
	border_step s;
	gdt::gdtlist<gdtedge> result;
	
	forall_face_border_steps (s,f)
		if (opposite(s->start_node,s->owner_edge)==v) 
			result.append(s->owner_edge);
	return result;		
	}




	gdt::gdtlist<face>
	plan_undi_graph::
faces_shared_by_separation_pair
	(separation_pair sp) const
	{
	return sp->shared_faces;
	}




	gdt::list_item
	plan_undi_graph::
pos_in_border
	(gdtedge e,face f) const
	{
	gdt::list_item pos = NULL;
	struct_plan_edge_info efi = (*edge_info)[e];
	if      (f==efi.left_face)  pos = efi.pos_in_left_face_edges;
	else if (f==efi.right_face) pos = efi.pos_in_right_face_edges;
	else gdt_error("invalid gdtedge or face");
	return pos;
	}



	gdt::list_item
	plan_undi_graph::
pos_in_border
	(border_step s) const
	{
	return s->pos_in_border_steps;
	}



	gdtnode
	plan_undi_graph::
start_of_border_step
	(border_step s) const
	{
	return s->start_node;
	}
	
	
	
	gdtnode
	plan_undi_graph::
stop_of_border_step
	(border_step s) const
	{
	gdtedge e = edge_of_border_step  (s);
	gdtnode v = start_of_border_step (s);
	return opposite (v,e);
	}



	border_step
	plan_undi_graph::
first_border_step
	(face f) const
	{
	if (!f->border_steps.empty()) return f->border_steps.head();
	else			      return NULL_BORDER_STEP;
	}



	border_step
	plan_undi_graph::
last_border_step
	(face f) const
	{
	if (f->border_steps.empty()) return NULL_BORDER_STEP;
	else return f->border_steps.tail();
	}



	border_step
	plan_undi_graph::
succ_border_step
	(border_step s) const
	{
	gdt::gdtlist<border_step>& bs=s->owner_face->border_steps;
	gdt::list_item pos = pos_in_border(s);
	pos = bs.succ(pos);
	if (pos) return bs.contents(pos);
	else     return NULL_BORDER_STEP; 
	}



	border_step
	plan_undi_graph::
pred_border_step
	(border_step s) const
	{
	gdt::gdtlist<border_step>& bs=s->owner_face->border_steps;
	gdt::list_item pos = pos_in_border(s);
	pos = bs.pred(pos);
	if (pos) return bs.contents(pos);
	else     return NULL_BORDER_STEP; 
	}
	


	border_step
	plan_undi_graph::
cyclic_succ_border_step
	(border_step s) const
	{
	gdt::gdtlist<border_step>& bs=s->owner_face->border_steps;
	return bs.contents(bs.cyclic_succ(pos_in_border(s)));
	}



	border_step
	plan_undi_graph::
cyclic_pred_border_step
	(border_step s) const
	{
	gdt::gdtlist<border_step>& bs=s->owner_face->border_steps;
	return bs.contents(bs.cyclic_pred(pos_in_border(s)));
	}
	
		
	
	border_step
	plan_undi_graph::
opposite_border_step
	(border_step s) const
	{
	gdtedge e = edge_of_border_step(s);
	gdtnode v = stop_of_border_step(s);
	return border_step_moving_along_edge(e,v);
	}
	


	border_step
	plan_undi_graph::
border_step_moving_along_edge
	(gdtedge e, gdtnode v) const
	{
	struct_plan_edge_info efi=(*edge_info)[e];
	border_step rbs = efi.right_face_border_step;
	border_step lbs = efi.left_face_border_step;
	if (rbs->start_node == v) return rbs;
	else if (lbs->start_node == v) return lbs;
	else gdt_error("invalid gdtedge or gdtnode");
	return lbs; // Avoids compiler warning
	}

	

	gdt::gdtlist<border_step>
	plan_undi_graph::
adj_border_steps
	(face f)  const
	{
	return f->border_steps;
	}



	gdt::gdtlist<border_step>
	plan_undi_graph::
border_steps_starting_at_node
	(gdtnode v) const
	{
	gdtedge e;
	gdt::gdtlist<border_step> result;
	forall_edges_adjacent_node (e,v,*this) result.append(border_step_moving_along_edge(e,v));
	return result;
	}
	
	

	gdt::gdtlist<border_step>
	plan_undi_graph::
border_step_sequence
	(border_step s1, border_step s2) const
	{
	border_step s;
	gdt::gdtlist<border_step> seq;

	if (face_of_border_step(s1) != face_of_border_step(s2))
		gdt_error("border_steps do not belong to the same face");
	
	s = s1;
	seq.append(s);
	while (s != s2)
		{
		s = cyclic_succ_border_step(s);
		seq.append(s);
		}

	return seq;
	}
	

	void
	plan_undi_graph::
border_step_sequence
	(border_step s1, border_step s2, gdt::gdtlist<border_step>& seq) const
	{
	border_step s;
	//gdt::gdtlist<border_step> seq;

	if (face_of_border_step(s1) != face_of_border_step(s2))
		gdt_error("border_steps do not belong to the same face");
	
	s = s1;
	seq.append(s);
	while (s != s2)
		{
		s = cyclic_succ_border_step(s);
		seq.append(s);
		}
	}


	
	gdt::gdtlist<border_step>
	plan_undi_graph::
border_step_sequence
	(gdtnode v1, gdtnode v2, face f) const
	{
	border_step s;
	gdt::gdtlist<border_step> seq;
	
	forall_face_border_steps (s,f)
		if (start_of_border_step(s) == v1) break;
	
	seq.append(s);
	while (stop_of_border_step(s) != v2)
		{
		s = cyclic_succ_border_step(s);
		if (start_of_border_step(s) == v1) seq.clear();
		seq.append(s);
		}

	return seq;
	}
		

	void
	plan_undi_graph::
border_step_sequence
	(gdtnode v1, gdtnode v2, face f, gdt::gdtlist<border_step>& seq) const
	{
	border_step s;
	//gdt::gdtlist<border_step> seq;
	
	forall_face_border_steps (s,f)
		if (start_of_border_step(s) == v1) break;
	
	seq.append(s);
	while (stop_of_border_step(s) != v2)
		{
		s = cyclic_succ_border_step(s);
		if (start_of_border_step(s) == v1) seq.clear();
		seq.append(s);
		}
	}


	separation_pair
	plan_undi_graph::
find_separation_pair
	(int idv, int idw, const gdt::gdtlist<separation_pair>& sp_list) const
	{
	// returns the separation_pair sp with (idv,idw) poles in sp_list;
	// if it doesn't exist returns NULL.
	separation_pair sp;
	gdtnode v, w;
	
	v = find_node (idv);
	w = find_node (idw);
	
	forall (sp,sp_list)
		if (((separation_pair_pole1(sp) == v) && (separation_pair_pole2(sp) == w)) ||
	  	   ((separation_pair_pole1(sp) == w) && (separation_pair_pole2(sp) == v)))
	  	   return sp;
	return NULL;
	}




	gdt::gdtlist<separation_pair>
	plan_undi_graph::
find_separation_pairs
	() const
	{
	// returns all graph's separation pairs. They are searched as gdtnode's pairs
	// shared by two or more faces.

	gdt::gdtlist<separation_pair> result;
	gdt::gdtlist<gdtnode>	      common_nodes;
	face f,g,h;
	gdtnode v,w;
	gdtedge e;
	gdt::list_item posv, posw;
	separation_pair   sp;
	bool separation_is_an_edge,
	     separation_pairs_already_exist;
	gdt::gdtmap<face,bool> face_already_insert(false);
	
	forall_faces (f,*this)
		forall_faces (g,*this)
			if (id(g) > id(f))
			{
			common_nodes.clear();
			common_nodes = nodes_shared_by_faces (f,g);
			
			// TEMPORARY PRINT
			// ----------------
			//cout << "\n NODES SHARED BY FACES " << "f" << id(f) << " AND f" << id(g) << ":\n" << flush;
			//forall (v,common_nodes)
			//	{
			//	cout << id(v) << " ";
			//	}
			//cout << "\n" << flush;
			// ----------------
			
			posv = common_nodes.first();
			while (posv)
				{
				v = common_nodes.contents(posv);
				posw = common_nodes.succ(posv);
				while (posw)
					{
					w = common_nodes.contents(posw);
					separation_is_an_edge = false;

					forall_edges_adjacent_node (e,v,*this)
						if (((opposite (v,e) == w) && 
						   ((edge_belongs_to_face (e,f)) && (edge_belongs_to_face (e,g)))))
						  	{
						  	separation_is_an_edge = true;
						  	break;
						  	}
						  	
					if (!separation_is_an_edge)
						   	{
						   	// (v,w) is a separation pairs
						   	// 
						   	//cout << "\nFOUND SEPARATION PAIRS " << id(v) << " " << id(w);
						   	separation_pairs_already_exist = false;
						   	forall (sp,result)
						   		if (((sp->pole1 == v) && (sp->pole2 == w)) || 
						   		   ((sp->pole1 == w) && (sp->pole2 == v)))
						   		   {
						   		   separation_pairs_already_exist = true;
						   		   break;
						   		   }
						   	if (!separation_pairs_already_exist)
						   		{
						   		// make a new separation pairs
						   		// 
								sp = new (struct_separation_pair);
								sp->pole1 = v;
								sp->pole2 = w;
								sp->shared_faces.append(f);
								sp->shared_faces.append(g);
								result.append (sp);
								}
							else
								{
								//cout << "\nSEPARATION ALREADY INSERT" << flush;
								face_already_insert[f] = false;
								face_already_insert[g] = false;
								forall (h,sp->shared_faces)
									if (h == f)
										face_already_insert[f] = true;
									else if (h == g)
										face_already_insert[g] = true;
								if (!face_already_insert[f]) sp->shared_faces.append(f);
								if (!face_already_insert[g]) sp->shared_faces.append(g);
								}
						   	}
					posw = common_nodes.succ(posw);
					}
				posv = common_nodes.succ(posv);
				}
			
			}
	return result;
	}




	
	
	
	void
	plan_undi_graph::		
make_dual (plan_undi_graph& dual,

        	gdt::gdtmap<face,gdtnode>& primal_face_2_dual_node, 
        	gdt::gdtmap<gdtnode,face>& dual_node_2_primal_face,

     		gdt::gdtmap<gdtedge,gdtedge>& primal_edge_2_dual_edge, 
     		gdt::gdtmap<gdtedge,gdtedge>& dual_edge_2_primal_edge
     		) const
	{
	
	undi_graph ug;  // we build dual graph as 'undi_graph' and then 
	                // copy it to 'dual' (class plan_undi_graph)

	// computation of dual graph as 'undi_graph'
		{
		face f;
		forall_faces( f , *this ) primal_face_2_dual_node[f]= ug.new_node( id(f) );  // this puts all the nodes of '*this' in 'ug' with the same id.

		gdtedge e;
		forall_edges( e , nodes_and_edges() ) primal_edge_2_dual_edge[e] = NULL_EDGE;  // initialize gdtedge bijection to "no gdtedge has corresponding in the dual graph", yet.
		
		
		forall_edges( e , nodes_and_edges() )
			{
			gdtnode n  = source(e);
			gdtnode n1 = target(e);
			face rf = right_face_moving_along_edge (e, n);
			face lf = left_face_moving_along_edge  (e, n);
			
			if( rf != lf )  // we get a dual gdtedge iff (rightface != leftface) for the primal gdtedge
				{
				gdtedge rfip = NULL_EDGE; 	// right face insertion point: the future gdtedge
							// will be inserted after this gdtedge in the dual gdtnode
							// incidence list (counter-clockwise ordered)
				gdtedge lfip = NULL_EDGE;	// the same for the left face
				
				
				// Inserting the dual gdtedge of 'e' (we call it 'ne' in the next) we have to solve
				// a problem: which position has it to take in the circular order?
				// This chunk computes those position for both extremes.
				// If the degree is <= 1 we do not need the position, 
				// then we leave 'rfip' and/or 'lfip' unchanged
				
					if( ug.degree(primal_face_2_dual_node[rf]) > 1 )
						{
						border_step bs  = border_step_moving_along_edge (e, n);
						if ( face_of_border_step(bs) != rf )
							bs = border_step_moving_along_edge (e, n1);

						bs = cyclic_succ_border_step(bs);
						while( primal_edge_2_dual_edge[edge_of_border_step(bs)] == NULL_EDGE )
							bs = cyclic_succ_border_step( bs );
						rfip = primal_edge_2_dual_edge[edge_of_border_step(bs)];
						}
					if( ug.degree(primal_face_2_dual_node[lf]) > 1 )
						{
						border_step bs  = border_step_moving_along_edge (e, n);
						if ( face_of_border_step(bs) != lf )
							bs = border_step_moving_along_edge (e, n1);

						bs = cyclic_succ_border_step(bs);
						while( primal_edge_2_dual_edge[edge_of_border_step(bs)] == NULL_EDGE )
							bs = cyclic_succ_border_step( bs );
						lfip = primal_edge_2_dual_edge[edge_of_border_step(bs)];

						}
					
				// now adding new gdtedge 'ne', the embedding is according the circular order of the primal edges in the face.
				
				       gdtedge ne = NULL_EDGE;
				       if( rfip == NULL_EDGE && lfip == NULL_EDGE )
					       ne = ug.new_edge (primal_face_2_dual_node[rf], primal_face_2_dual_node[lf], id(e) );

				       else if( rfip != NULL_EDGE && lfip != NULL_EDGE )
				               ne = ug.new_edge (primal_face_2_dual_node[rf], rfip, primal_face_2_dual_node[lf], lfip, id(e) );

				       else if( rfip == NULL_EDGE )
					       ne = ug.new_edge (primal_face_2_dual_node[lf], lfip, primal_face_2_dual_node[rf], id(e) );

				       else if( lfip == NULL_EDGE )
					       ne = ug.new_edge (primal_face_2_dual_node[rf], rfip, primal_face_2_dual_node[lf], id(e) );

				       primal_edge_2_dual_edge[e] = ne;	// updating primal_edge_2_dual_edge
			
				}
			}

		}
	dual = ug;      // copying 'ug' to output graph 'plan_undi_graph& dual' 
		
	// updating primal_face_2_dual_node and primal_edge_2_dual_edge with 
	// 'dual' values instead of 'ug' values
		{
		face f;
		forall_faces( f , *this ) 
			{
			gdtnode dual_node = dual.find_node( ug.id(primal_face_2_dual_node[f]) );
			primal_face_2_dual_node[f] = dual_node;        // updating
			dual_node_2_primal_face[dual_node] = f;        // setting
			}
		gdtedge e;
		forall_edges( e , nodes_and_edges() ) 
			if( primal_edge_2_dual_edge[e] != NULL_EDGE )     // 'cause bridges does not generate dual edges
				{
				gdtedge D_e = dual.find_edge( ug.id(primal_edge_2_dual_edge[e]) );
				primal_edge_2_dual_edge[e] = D_e;
				dual_edge_2_primal_edge[D_e] = e;
				}
		}

	/*	
	// building output map primal_node_2_dual_face
		{
		gdtnode primal_node;
		forall_nodes( primal_node , nodes_and_edges() )
			{
			gdtedge primal_edge = first_adj_edge(primal_node);
			gdtedge dual_edge   = primal_edge_2_dual_edge[primal_edge];
			if( dual_edge != NULL_EDGE )   // dual_edge is NULL if primal_edge was a bridge
				{
				face primal_face = left_face_moving_along_edge( primal_edge, primal_node );
				gdtnode dual_node   = primal_face_2_dual_node[primal_face];
				face dual_face   = dual.right_face_moving_along_edge( dual_edge, dual_node );
				primal_node_2_dual_face[primal_node] = dual_face;
				}
		}
	*/
	}

	
	
	void
	plan_undi_graph:: 
compute_visibility_representation
	(
	 gdt::gdtnode_array<horizontal_segment>& seg_node, 
	 gdt::gdtedge_array<vertical_segment>& seg_edge, 
	 face ext_face,
	 bool compacted,
	 gdt::gdtedge_array<int>* cost_p
	)
	{
    #ifdef DEBUG
    cout << "Function compute_visibility_representation" << endl;
	cout << "Nodes: " << number_of_nodes() << "\tedges: " << number_of_edges() << endl;
	#endif
	if (!is_biconnected())
		gdt_error("graph is not biconnected");

 	gdtedge e_st;
 	gdtedge e;
 	face f;
 	gdtnode v,vd;
 	border_step bs;
 	plan_undi_graph dpug;		// dual of *this

 	gdt::gdtnode_array<int>  level_of_node;
 	gdt::gdtnode_array<int>  level_of_dual_node;
 	gdt::gdtmap<face,gdtnode>   dual_node_representing_face(NULL_NODE);
  	gdt::gdtedge_array<gdtnode> original_start_of_edge;

	// --------------
 	// set gdtedge costs 
 	// --------------
 	bool allocated=false;
	//cout << endl << "1- valore di cost_p=" << cost_p << endl;
	if (!cost_p) 
	{
		cost_p = new gdt::gdtedge_array<int>(nodes_and_edges(),1);
		allocated=true;
	}
	//cout << endl << "2- valore di cost_p=" << cost_p << endl;
	gdt::gdtedge_array<int>& cost = *cost_p; 
	
	// -----------------------------------------------------
	// Make an st-number of *this, saving edges initial 
	// direction in order to restore them after computation. 
	// -----------------------------------------------------
	
 	original_start_of_edge.init (nodes_and_edges(), NULL_NODE);
	
	forall_edges (e,*this)
		original_start_of_edge[e]=start(e);
 	
	bs    = first_border_step (ext_face);
	e_st  = edge_of_border_step (bs);	
 	make_directed  (start_of_border_step(bs),stop_of_border_step(bs)); // make an st-numbering of *this
 	
  	// --------------------------------------------------------
 	// compute X-coordinates and Y-coordinates using a primal
 	// and a dual visit of the st-digraph *this
 	// --------------------------------------------------------
	
  	calculate_dual (dpug, e_st, dual_node_representing_face);
  	
  	level_of_node.init      (nodes_and_edges());
	level_of_dual_node.init (dpug.nodes_and_edges());

 	//cout << "done";
 	//cout << "\n. Creating visibility representation ... ";

 	calculate_level_of_all_nodes (level_of_node);	// levels corresponding to Y coordinates
 	dpug.calculate_level_of_all_nodes (level_of_dual_node);	// levels corresponding to X coordinates
 	
  	forall_edges (e,*this)
 		{
		f  = left_face_moving_along_edge (e,start(e));
 		vd = dual_node_representing_face [f];

		seg_edge[e].x     = level_of_dual_node [vd];
  		seg_edge[e].y_min = level_of_node [start(e)];
		seg_edge[e].y_max = level_of_node [stop(e)];
 		}

	forall_nodes (v,*this)
 		{
 		int cur_adj_face_level;
 		int min_adj_face_level;
 		int max_adj_face_level;
 		e  = first_adj_edge(v);
		f  = left_face_moving_along_edge (e,start(e));
 		vd = dual_node_representing_face [f];
 		min_adj_face_level = level_of_dual_node [vd];
 		max_adj_face_level = level_of_dual_node [vd];
 		
 		//Modified by A.M.
		forall_edges_adjacent_node(e,v,*this)
 			{
			f  = left_face_moving_along_edge (e,start(e));
 			vd = dual_node_representing_face [f];
 			cur_adj_face_level = level_of_dual_node [vd];
 			min_adj_face_level = gdt_minimum (cur_adj_face_level, min_adj_face_level);
 			max_adj_face_level = gdt_maximum (cur_adj_face_level, max_adj_face_level);
 			}
 				 		
 		seg_node[v].y     = level_of_node [v];
  		seg_node[v].x_min = min_adj_face_level;
		seg_node[v].x_max = max_adj_face_level;
		}
	
	// --------------------------------------------------------
	// execute a slow Y-compaction algorithm if alg == SLOW
	// --------------------------------------------------------
	   if (compacted)
	   	{
	   	gdtnode s = start(e_st);
	   	gdt::gdtedge_array<int> len (nodes_and_edges(),0);
	    	find_vertical_lengths (len, e_st, cost);
	    	
	    	dire_graph DAG (*this);
	    	gdt::gdtnode_array<bool> reached (DAG.nodes_and_edges(),false);
	    	gdt::gdtlist<gdtnode>	 order;  
	    	
	    	gdtnode S = DAG.find_node(id(s));
	    	DAG.topological_sort (S, reached, order);
	    	
		forall (v,order)
			{
			// assign Y coordinate for gdtnode v
			//
			v = corresponding_node(v,DAG);
			seg_node[v].y = 0;
			gdtnode w;
			e = first_in_edge (v);
			if (e)  {
				w = opposite(v,e);
				seg_node[v].y = seg_node[w].y + len[e];
				}
				
			// assign Y coordinates for edges entering v
			//
			forall_edges_entering_node (e,v,*this)
				{
				w = opposite(v,e);
				seg_edge[e].y_min = seg_node[w].y;
				seg_edge[e].y_max = seg_node[v].y; 
				}
			}
 	    	}	

 	//cout << "done";
 	//cout << "\n. visibility representation created ... ";
 
	// A visibility representation is now stored into the coordinate arrays.
	// The following commented code is just a test print-out
	// to check the representation is correct.
	//
	   forall_nodes (v,*this)
	    	{
	   	//cout << "\nNode "; print(v);
	    	//cout << seg_node[v].x_min << " ---(" << seg_node[v].y << ")--- " << seg_node[v].x_max;
	    	}
	   
	   forall_edges (e,*this)
	    	{
	    	//cout << "\nEdge "; print(e);
	    	//cout << seg_edge[e].y_min << " ---(" << seg_edge[e].x << ")--- " << seg_edge[e].y_max;
	    	}


	// ----------------------------------------------
	// Restore the original direction on every gdtedge.
	// ----------------------------------------------

	forall_edges (e,*this)
		{
		v = original_start_of_edge[e];
		if (v) make_directed(e,v);
		else make_undirected(e);
		}	
	if (allocated) delete (cost_p);
	}
	
	
	
	void
	plan_undi_graph::
compute_visibility_representation 
	(
	 gdt::gdtnode_array<horizontal_segment>& seg_node, 
	 gdt::gdtedge_array<vertical_segment>&   seg_edge, 
	 gdtedge e_st, 
	 bool compacted,
	 gdt::gdtedge_array<int>* cost_p
	)
	{
	if (!is_biconnected()) 
		gdt_error("graph is not biconnected"); 
	
	gdtedge e;
 	face f;
 	gdtnode v,vd;
 	plan_undi_graph dpug;		// dual of *this
 	
 	// --------------
 	// set gdtedge costs 
 	// --------------
 	bool allocated=false;
 	if (!cost_p)
 	{ 
 		cost_p = new gdt::gdtedge_array<int>(nodes_and_edges(),1);
 		allocated=true;
 	} 
	gdt::gdtedge_array<int>& cost = *cost_p; 
	
	gdt::gdtnode_array<int>  level_of_node;
 	gdt::gdtnode_array<int>  level_of_dual_node;
 	gdt::gdtmap<face,gdtnode>   dual_node_representing_face(NULL_NODE);
 
 	//cout << "\n. Creating visibility representation ... ";

 	// --------------------------------------------------------
 	// compute X-coordinates and Y-coordinates using a primal
 	// and a dual visit of the st-digraph *this
 	// --------------------------------------------------------
 	
  	calculate_dual (dpug, e_st, dual_node_representing_face);
  	
 	level_of_node.init      (     nodes_and_edges(), 0);
	level_of_dual_node.init (dpug.nodes_and_edges(), 0);
	
	     calculate_level_of_all_nodes (level_of_node);	// levels corresponding to Y coordinates
 	dpug.calculate_level_of_all_nodes (level_of_dual_node); // levels corresponding to X coordinates
 	
  	forall_edges (e,*this)
 		{
		f  = left_face_moving_along_edge (e,start(e));
 		vd = dual_node_representing_face [f];
 		
		seg_edge[e].x     = level_of_dual_node [vd];
  		seg_edge[e].y_min = level_of_node [start(e)];
		seg_edge[e].y_max = level_of_node [stop(e)];
 		}

	forall_nodes (v,*this)
 		{
 		int cur_adj_face_level;
 		int min_adj_face_level;
 		int max_adj_face_level;
 		e  = first_adj_edge(v);
		f  = left_face_moving_along_edge (e,start(e));
 		vd = dual_node_representing_face [f];
 		min_adj_face_level = level_of_dual_node [vd];
 		max_adj_face_level = level_of_dual_node [vd];
 		
		//Modified by A.M.
		forall_edges_adjacent_node(e,v,*this)
 			{
			f  = left_face_moving_along_edge (e,start(e));
 			vd = dual_node_representing_face [f];
 			cur_adj_face_level = level_of_dual_node [vd];
 			min_adj_face_level = gdt_minimum (cur_adj_face_level, min_adj_face_level);
 			max_adj_face_level = gdt_maximum (cur_adj_face_level, max_adj_face_level);
 			}
 				 		
 		seg_node[v].y     = level_of_node [v];
  		seg_node[v].x_min = min_adj_face_level;
		seg_node[v].x_max = max_adj_face_level;
		}
	
	// --------------------------------------------------------
	// execute a slow Y-compaction algorithm if alg == SLOW
	// --------------------------------------------------------
	   if (compacted)
	   	{
	   	gdtnode s = start(e_st);
	   	gdt::gdtedge_array<int> len (nodes_and_edges(),0);
	    	find_vertical_lengths (len, e_st, cost);
	    	
	    	dire_graph DAG (*this);
	    	gdt::gdtnode_array<bool> reached (DAG.nodes_and_edges(),false);
	    	gdt::gdtlist<gdtnode>	 order;  
	    	
	    	gdtnode S = DAG.find_node(id(s));
	    	DAG.topological_sort (S, reached, order);
	    	
		forall (v,order)
			{
			// assign Y coordinate for gdtnode v
			//
			v = corresponding_node(v,DAG);
			seg_node[v].y = 0;
			gdtnode w;
			e = first_in_edge (v);
			if (e)  {
				w = opposite(v,e);
				seg_node[v].y = seg_node[w].y + len[e];
				}
				
			// assign Y coordinates for edges entering v
			//
			forall_edges_entering_node (e,v,*this)
				{
				w = opposite(v,e);
				seg_edge[e].y_min = seg_node[w].y;
				seg_edge[e].y_max = seg_node[v].y; 
				}
			}
 	    	}
	if (allocated) delete (cost_p);
	}


	
 	// --------------------
	// Translate operations
	// --------------------

	face
	plan_undi_graph::
corresponding_face
	(face f, const plan_undi_graph& pug)
	{
	return (f) ? find_face(pug.id(f)):NULL_FACE;
	}



	border_step
	plan_undi_graph::
corresponding_border_step
	(border_step s, const plan_undi_graph& pug)
	{
	if (!s) return NULL_BORDER_STEP;
	else	{
		gdtedge pug_e = pug.edge_of_border_step (s);
		gdtnode pug_v = pug.start_of_border_step(s);
		return border_step_moving_along_edge
			(
			corresponding_edge (pug_e,pug),
			corresponding_node (pug_v,pug)
			);
		}
	}



	// -----------------
	// Update operations
	// -----------------

	// Description  : Split gdtedge e by adding a new gdtnode between its end-nodes.
	// Precondition : -
	// Notes        : -
	gdtnode
	plan_undi_graph::
new_node 
	(gdtedge e, int new_id) 
	{
	//cout << "\nplan_undi_graph::new_node " << flush;
	gdtnode u, v=source(e), w=target(e);
	border_step sv, sw, sv1, sv2, sw1, sw2;
	gdtedge 	    ev1, evu, euw;
	face 	    fv,fw;
	gdt::gdtlist<marker_type> ml;	
	
	//cout << "\Edge " << id(e) << " between " << id(v) << " and " << id(w) << flush;
	
	// ---------------------- 1 -----------------------
	// look for the two edges adjacent e into its faces 
	// ------------------------------------------------
	
	sv = border_step_moving_along_edge (e,v);
	sw = border_step_moving_along_edge (e,w);
	fv = face_of_border_step (sv);
	fw = face_of_border_step (sw);
	sv1= cyclic_pred_border_step (sv); ev1 = edge_of_border_step (sv1);
	sv2= cyclic_succ_border_step (sv); 
	sw1= cyclic_pred_border_step (sw);
	sw2= cyclic_succ_border_step (sw);
		
	//cout << "\nfv = " << id(fv) << " fw = " << id(fw) << " ev1 = " << id(ev1) << flush;

	// ---------------------- 2 ---------------------
	// add the two edges resulting from the splitting
	// and update markers, directions and constraints
	// ----------------------------------------------
	
	ml = markers(e);
	u   = new_node (new_id); 
	evu = undi_graph::new_edge (v,ev1,u);   reset_edge_info(evu);  
 	euw = undi_graph::new_edge (u,evu,w,e); reset_edge_info(euw);
	
	mark(evu,ml);
	mark(euw,ml);

 	// check gdtedge direction
 	// 
 	if (start(e) == v) 	
 		{
 		make_directed (euw,u);
 		make_directed (evu,v);
 		}	
 		
 	else if (start(e) == w) 
 		{
 		make_directed (evu,u);
 		make_directed (euw,w);
 		}

	set_right_face_moving_along_edge (evu,v,e,v,   gdt::after );
	set_right_face_moving_along_edge (evu,u,e,w,   gdt::before);
	set_right_face_moving_along_edge (euw,u,evu,v, gdt::after );
	set_right_face_moving_along_edge (euw,w,evu,u, gdt::before);

	update_constraints_after_split_edge (e,evu,euw);
	

	// ------- 3 -------
	// delete the gdtedge e
	// -----------------
	
	undi_graph::del_edge (e);
	gdt::gdtlist<gdtedge>& befv = fv->edges;
	gdt::gdtlist<gdtedge>& befw = fw->edges;
	befv.del_item (pos_in_border(e,fv));
	if (fv!=fw) befw.del_item (pos_in_border(e,fw));
	gdt::gdtlist<border_step>& bsfv = fv->border_steps;
	bsfv.del_item (pos_in_border(sv)); 
        safe_delete(sv);
	gdt::gdtlist<border_step>& bsfw = fw->border_steps;
	bsfw.del_item (pos_in_border(sw)); 
        safe_delete(sw);
 
	//Added by Marcandalli 8 2001	
	edge_info->undefine(e);
	//

	return u;
	}
	

	// Description  : Attach vertex to vertex 'n' next to gdtedge 'e'
	gdtnode
	plan_undi_graph::
new_node(gdtnode n, gdtedge e, int node_id, int edge_id)
	{
        //assert(internal_consistency_check());
	// compute a few useful information
	border_step s1= border_step_moving_along_edge( e, opposite(n,e) );
	border_step s2= cyclic_succ_border_step( s1 );
	gdtedge e_succ= edge_of_border_step( s2 );
	//face f= face_of_border_step(s1);  // this is the face in which the gdtnode and the gdtedge should be placed
	//struct_plan_edge_info* e_info= &(*edge_info)[e];



	/*
	  initial state

	  O
	  | ^
   e_succ | | s2 
	  | 
	n O
	  |
	e | ^
	  | | s1
	  O
	  
	  final state

	  O
	  | ^
   e_succ | | s2        s_from_nnew
	  |      e_new  <--
	n O----------------O n_new
	  |             -->
	e | ^            s_from_n
	  | | s1
	  O

	 */

        

	// fix underlying undi_graph strucutre
	gdtnode n_new = undi_graph::new_node(node_id);
        gdtedge e_new = undi_graph::new_edge(n,e,n_new,edge_id);

	set_right_face_moving_along_edge (e_new, n, e, opposite(n,e) );
	set_right_face_moving_along_edge (e_new, n_new, e_succ, n, gdt::before );

        //assert(internal_consistency_check());
	return n_new;
	}

	/*

	// Description  : Attach vertex to vertex 'n' next to gdtedge 'e'
	gdtnode
	plan_undi_graph::
new_node(gdtnode n, gdtedge e, int node_id, int edge_id)
	{
	gdtnode n2 = opposite(n, e);
	face f = left_face_moving_along_edge(e, n);
	gdtedge edge_to_split = new_edge(n, n2, f, edge_id);
	gdtnode result = new_node(edge_to_split, node_id);
	
	gdtedge edge_to_delete = first_adj_edge(result);
	if( opposite(result, edge_to_delete) == n ) edge_to_delete = last_adj_edge(result);
	
	del_edge(edge_to_delete);

	return result;
	}

	*/


 	
	// Description  : Split face 'f' by adding a new gdtedge between 'v' and 'w'.
	// Precondition : ('v' and 'w' are vertices of the border of 'f') and ('v' != 'w')
	// Notes        : When 'v' (or 'w') is the start-gdtnode of a bridge, the new gdtedge is inserted on the right of that bridge, moving on the bridge from 'v' ('w') to its opposite.
	gdtedge
	plan_undi_graph:: 
new_edge 
	(gdtnode v, gdtnode w, face f, int new_id) 
	{
	//gdt::gdtlist<border_step> seq = border_step_sequence (v,w,f);
	gdt::gdtlist<border_step> seq;
	border_step_sequence (v,w,f,seq);
	
	border_step sv  = seq.head();
	border_step sw  = seq.tail();
	gdtedge		  ev  = edge_of_border_step (cyclic_pred_border_step(sv));
	gdtedge              ew  = edge_of_border_step (sw);
	return new_edge (v,ev, w,ew, new_id);
	}


	// Description  : Split face 'f' by adding a new gdtedge between 'v' and 'w'. The new gdtedge is placed around v and w, respectively after ev and ew.
	// Precondition : ('v' and 'w' are vertexes of the border of 'f') and ('v' != 'w')
	// Notes        : When 'v' (or 'w') is the start-gdtnode of a bridge, the new gdtedge is inserted on the right of that bridge, moving on the bridge from 'v' ('w') to its opposite.
	gdtedge
	plan_undi_graph:: 
new_edge 
	(gdtnode v, gdtedge ev, gdtnode w, gdtedge ew, int new_id)
	{
	
	
	//cout << "\nplan_undi_graph::new_edge(gdtnode,gdtedge,gdtnode,gdtedge,face) begin";
	face f, fv, fw;                // Faces resulting from face 'f' split.
	gdtedge e;                        // The new gdtedge splitting face 'f'
	//gdtedge ce;                       // Current gdtedge during scan of face 'f' semi-border
	border_step cs;                // Current border_step during scan of face 'f' semi-border
	border_step sv, sw;            // Border_steps along the new gdtedge
	border_step sv1, sv2;          // Border_steps before and after gdtnode 'v', before split
	border_step sw1, sw2;          // Border_steps before and after gdtnode 'w', before split
	gdt::gdtlist<border_step> seqv, seqw;  // Face 'f' semi-borders.

	//if( internal_consistency_check() ) cout << "\nOK";

	sv1 = border_step_moving_along_edge (ev,opposite(v,ev));
	
	//
	//if(edge_of_border_step(sv1) == NULL_EDGE)
	//	{
	//	cout << "\nv = " << v << " with id " << id(v) << flush;
	//	cout << "\nev = " << ev << " with id " << id(ev) << flush;
	//	cout << "\new = " << ew << " with id " << id(ew) << flush;
	//	cout << "\nopposite(v,ev) = " << opposite(v,ev) << " with id " << id(opposite(v,ev)) << flush;
	//	assert(false);
	//	}
	//
	
	sw1 = border_step_moving_along_edge (ew,opposite(w,ew));
	sv2 = cyclic_succ_border_step (sv1);
	sw2 = cyclic_succ_border_step (sw1);
	
	fv = face_of_border_step (sv1);
	assert(fv != NULL_FACE);
	fw = face_of_border_step (sw1);
	if (fv!=fw) gdt_error("planarity violation."); 

	f   = fv;

	border_step_sequence (sv2,sw1,seqv);
	border_step_sequence (sw2,sv1,seqw);
	
	//	
	//cout << "\nseqv = ";
	//seqv.print();
	//cout << "\nseqw = ";
	//seqw.print();	
	//cout << "\nsw2 = " << sw2 << " and its gdtedge is " << edge_of_border_step(sw2) << flush;
	//cout << "\nsv1 = " << sv1 << " and its gdtedge is " << edge_of_border_step(sv1) << flush;
	//	
/*	
	//
	//    o----ev-----(v)-----------o
	//   /   --sv1->       --sv2->   \
	//  /                             \          
	//                                    
        // seqw            f             seqv  
	//                                          
	//  \                             /           
	//   \   <-sw2--       <-sw1--   /          
	//    o-----------(w)----ew-----o
	//
*/
	fv = new_face ();
	fw = new_face ();
	e  = undi_graph::new_edge(v,ev, w,ew, new_id); reset_edge_info(e);

/*	//
	//    o----ev-----(v)-----------o
	//   /   --sv1->   |   --sv2->   \
	//  /              |              \          
	//                 |                   
        // seqw   fv       e      fw     seqv
	//                 |            
	//  \              |              /           
	//   \   <-sw2--   |   <-sw1--   /          
	//    o-----------(w)----ew-----o
	//
*/
	// Make all the edges and border_steps along seqw
	// pointing to the new face 'fv' in place of the old one 'f'.
	// During the scan, build the list of edges and border_steps of face 'fv'.
	//
	forall (cs,seqw)
		{
		gdtedge ce = cs->owner_edge;
		struct_plan_edge_info& cei = (*edge_info)[ce];
		if (cei.right_face_border_step == cs)
			{
			cei.right_face = fv;
			cei.pos_in_right_face_edges = fv->edges.append(ce);	
			}
		else if (cei.left_face_border_step  == cs)
			{
			cei.left_face = fv;
			cei.pos_in_left_face_edges = fv->edges.append(ce);	
			}
		else gdt_error("malformed face");
		cs->owner_face = fv;
		cs->pos_in_border_steps =  fv->border_steps.append(cs);
		}		
	
	// Make all the edges and border_steps along seqv
	// pointing to the new face 'fw' in place of the old one 'f'.
	// During the scan, build the list of edges and border_steps of face 'fv'.
	//
	forall (cs,seqv)
		{
		gdtedge ce = cs->owner_edge;
		struct_plan_edge_info& cei = (*edge_info)[ce];
		if (cei.right_face_border_step == cs)
			{
			cei.right_face = fw;
			cei.pos_in_right_face_edges = fw->edges.append(ce);	
			}
		else if (cei.left_face_border_step  == cs)
			{
			cei.left_face = fw;
			cei.pos_in_left_face_edges = fw->edges.append(ce);	
			}
		else gdt_error("malformed face");
		cs->owner_face = fw;
		cs->pos_in_border_steps =  fw->border_steps.append(cs);
		}

	// Create two new border_steps for the new gdtedge,
	// and set properly the information associated to them.
	//
	sv = new_border_step (e,v,fv);
	sw = new_border_step (e,w,fw);
	sv->pos_in_border_steps =  fv->border_steps.append(sv);
	sw->pos_in_border_steps =  fw->border_steps.append(sw);
			
	// Since 'v' was the first argument of the new_edge() call,
	// we can assume it is the source(e), therefore we set
	// fv as the right face of e, and fw as the left one.
	//
	if (v!=source(e)) gdt_error("malformed gdtedge");

	struct_plan_edge_info& ei = (*edge_info)[e];
	ei.right_face = fv;
	ei.right_face_border_step = sv;
	ei.pos_in_right_face_edges = fv->edges.append(e);	
	ei.left_face = fw;
	ei.left_face_border_step = sw;
	ei.pos_in_left_face_edges = fw->edges.append(e);

	// Remove the old face 'f'
	//
	del_face (f);

	// Test
	//
	//	{
	//	int i;
	//	gdtedge e;
	//	face f,f1,f2;
	//	border_step s, s1, s2;
	//	cout <"\n BEGIN TESTING";
	//	print(COMPLETE);
	//	forall_faces (f,*this)
	//		{
	//		i=0;
	//		cout << "\n Face "; print(f);
	//		cout << "\n Face "; print(f_list->contents(f->pos_in_f_list));
	//		forall_face_border_steps (s,f)
	//			{
	//			i++;
	//			e = edge_of_border_step(s);
	//			struct_plan_edge_info& ei = (*edge_info)[e];
	//			cout << "\n  Border step " << i;
	//			cout << "\n   Start "; print(start_of_border_step(s));
	//			cout << "\n   Start "; print(start_of_border_step(f->border_steps.contents(s->pos_in_border_steps)));
	//			cout << "\n   Stop  "; print(stop_of_border_step(s));
	//			cout << "\n   Face  "; print(face_of_border_step(s));
	//			cout << "\n   Edge  "; print(e);
	//			cout << "\n   Edge  "; if (f==ei.right_face) print(f->edges.contents(ei.pos_in_right_face_edges)); else print(f->edges.contents(ei.pos_in_left_face_edges));
	//			cout << "\n   Edge source "; print(source(e));
	//			cout << "\n   Edge target "; print(target(e));
	//			cout << "\n   Edge left  face "; print(ei.left_face);
	//			cout << "\n   Edge right face "; print(ei.right_face);
	//			cout << "\n   Edge left  border step start "; print(start_of_border_step(ei.left_face_border_step));
	//			cout << "\n   Edge right border step start "; print(start_of_border_step(ei.right_face_border_step));
	//			cout << "\n   Prev start "; print(start_of_border_step(cyclic_pred_border_step(s)));
	//			cout << "\n   Next start "; print(start_of_border_step(cyclic_succ_border_step(s)));
	//			}
	//		}
	//	cout <"\n STOP TESTING";
	//	}
	return e;
	}


	
	
	face
	plan_undi_graph::
del_node
	(gdtnode v)
	{
	// delete the gdtnode v and all its adjacent edges.
	//
	gdtedge e;
	face f = NULL_FACE;
	gdt::gdtlist<gdtedge> edges_list;
	
	edges_list = adj_edges (v);
	forall (e,edges_list) f=del_edge (e);
	return f;
	}



	face
	plan_undi_graph::
del_edge
	(gdtedge e)
	{	
	// delete the gdtedge e and merge the two faces adjacent to gdtedge.
	// NOTE: if a gdtnode adjacent to gdtedge e has degree 0 after the
	//	 deleting operation, this gdtnode is removed.
	//
	gdtnode v,w;
	border_step sv,sw,sv1,sv2,sw1,sw2,sx;
	face 	    fv,fw;
	gdtedge	    ev1,ev2,ew1,ew2,ex;
	gdt::list_item   edge_pos_in_fv, border_step_pos_in_fv,
		    border_step_pos_in_fw;
	
	v  = source (e);
	w  = target (e);
	sv = border_step_moving_along_edge (e,v);
	sw = border_step_moving_along_edge (e,w);
	
	fv = face_of_border_step (sv);
	fw = face_of_border_step (sw);
	sv1= cyclic_pred_border_step (sv);
	sv2= cyclic_succ_border_step (sv);
	sw1= cyclic_pred_border_step (sw);
	sw2= cyclic_succ_border_step (sw);
	ev1= edge_of_border_step (sv1);
	ev2= edge_of_border_step (sv2);
	ew1= edge_of_border_step (sw1);
	ew2= edge_of_border_step (sw2);
	
	
	if (fv != fw) 
		{
		// cout << "\nGENERAL CASE\n" << flush;
		/* 
		 	      GENERAL CASE
		
		      o----ev1-----(v)----ew2----o
		     /   --sv1->    |    --sw2->  \       
		    /               |              \
		   o          	    |               O
		   |          	    |+		    |     
		   |               |||sw            |   
		   |   	  fv       |e|      fw      | 
		   |             sv|||              |
		   |		   +|		    |
		   |                |               |           
		   |         	    |   	    |
		   O		    |               O
		    \      <-sv2-   |   <-sw1--    /
		     o----ev2------(w)----ew1-----o   
		*/
		

		border_step_pos_in_fv = pos_in_border (sv1);
		border_step_pos_in_fw = pos_in_border (sw2);
		
		while (border_step_pos_in_fw != pos_in_border(sw))
			{
			sx = fw->border_steps.contents(border_step_pos_in_fw);
			ex = edge_of_border_step(sx);
			
			//cout << "\nEXAME OF EDGE " << id(ex) << "\n" << flush;
			
			// substitute the face fw with fv in ex edge_info
			//
			if ((*edge_info)[ex].right_face == fw)  
				(*edge_info)[ex].right_face = fv;
			else	
				(*edge_info)[ex].left_face  = fv;
				
			// add the border_step sx to face fv
			//
			border_step_pos_in_fv = fv->border_steps.insert (sx,border_step_pos_in_fv);
			sx->owner_face = fv;
			//cout << "\nADDED A NEW BORDER STEP TO FACE " << id(fv);
			
			// increment the border_step's pos in face fw and update the sx pos in fv
			//
			border_step_pos_in_fw = fw->border_steps.cyclic_succ(border_step_pos_in_fw);
			sx->pos_in_border_steps = border_step_pos_in_fv;
			}
		
		// delete the border_step sv from fv border_steps list
		//	
		fv->border_steps.del_item (pos_in_border(sv));
		//delete sv; //Added by P.F.C 9/10/2006
		
	
		
		
		// rebuild the fv edges list
		//
		fv->edges.clear();
		gdt::gdtedge_array<bool> edge_already_insert (nodes_and_edges(),false);
		forall_face_border_steps (sx,fv)
			{
			ex = edge_of_border_step (sx);
			if (!edge_already_insert[ex])
				{
				edge_pos_in_fv = fv->edges.append(ex);
				
				if ((*edge_info)[ex].right_face == fv)
					(*edge_info)[ex].pos_in_right_face_edges = edge_pos_in_fv;
				if ((*edge_info)[ex].left_face == fv)		
					(*edge_info)[ex].pos_in_left_face_edges  = edge_pos_in_fv;

				edge_already_insert[ex]=true;
				}
			}
		//cout << "\nEDGES LIST OF FACE f" << id(fv) << " : " << flush;
		//forall_face_edges (ex,fv) cout << id(ex) << " ";
		//cout << "\n" << flush;
		
		// delete the face fw
		//
		del_face (fw);
		}
		
	else
		{
		// cout << "\nPARTICULAR CASE fv=fw\n" << flush;
		/* 
			 PARTICULAR CASE fv=fw
		
			o---------(v)----------o
		       /  --sv1->  |   --sw2->	\
		      /		   |	 	 \
		     o		  ||+	  	  o
		     |      sw1=sv|e|		  |
		     |		  |||sw=sv2	  |		
		     |		  +||	  	  |
		     o	   fv	   |		  o 
		      \		  (w)		 /
		       \			/
	         	o----------------------o
		*/

		fv->border_steps.del_item (pos_in_border(sw));
		fv->border_steps.del_item (pos_in_border(sv));
		fv->edges.del_item (pos_in_border(e,fv));
		////////////// added by P.F.C 9/10/2006
		//delete sv;
		delete sw;
		/////////////////////////////////////////
		}
		
	// delete the gdtedge e and the
	// corresponding border_step in fv
	//
	
	undi_graph::del_edge(e);
	//Added by Marcandalli 8 2001
	edge_info->undefine(e);
	//
	safe_delete (sv);
	
	// if degree(v)=0 (degree(w)=0) remove v (w)
	//
	if (degree(v) == 0)
		{
		//cout << "\nREMOVED NODE " << id(v) << flush;
		undi_graph::del_node(v);
		}
	if (degree(w) == 0) 
		{
		//cout << "\nREMOVED NODE " << id(w) << flush;
		undi_graph::del_node(w);
		}
	return fv;
	}



	gdtedge
	plan_undi_graph::
weld_edges_at_node
	(gdtnode v)
	{
	// -----------------------------
	// before applying the operation
	/* -----------------------------
	               fwv
	                            ew
	       \    svu      swv   / 
	        \  <---  v  <---  /
	       u o--euv--O--ewv--o w
	        /  --->    --->   \
	       /   suv     svw     \
	     eu
	                fuv
	----------------------------- */
	
	if (degree(v)!=2) gdt_error("gdtnode has not degree 2");
					
	gdtedge euv = first_adj_edge(v);
	gdtedge ewv = last_adj_edge(v);
	
	// debug
	//
	//std::cout << "\nWelding edges " << id(euv) << " and " << id(ewv) << " at node " << id(v) << " in plan" << std::flush;
	//
	
	gdtnode u   = opposite(v,euv);
	gdtnode w   = opposite(v,ewv);
	
	gdtnode temp_u_pred = NULL_NODE;
	gdtnode temp_w_pred = NULL_NODE;
	
	
	// Added by W.D. on 21 October 2008
	// for handling the cases in which u and/or w have degree 1
	//
	if (degree(u) == 1)
	  temp_u_pred = new_node (u,euv);
	if (degree(w) == 1)
	  temp_w_pred = new_node (w,ewv);
	//
	
	border_step swv = border_step_moving_along_edge (ewv,w);
	border_step svu = border_step_moving_along_edge (euv,v);
	border_step suv = border_step_moving_along_edge (euv,u);
	border_step svw = border_step_moving_along_edge (ewv,v);
	
	border_step swv_pred = cyclic_pred_border_step (swv);
	border_step suv_pred = cyclic_pred_border_step (suv);
	gdtedge 	    ew       = edge_of_border_step (swv_pred);
	gdtedge 	    eu       = edge_of_border_step (suv_pred);
	gdtnode 	    w_pred   = opposite(w,ew);
	gdtnode	    u_pred   = opposite(u,eu);
	
	face fwv = face_of_border_step (swv);
	face fuv = face_of_border_step (suv);
	
	gdt::list_item pos_ewv_in_fwv = pos_in_border(ewv,fwv);
	gdt::list_item pos_ewv_in_fuv = pos_in_border(ewv,fuv);
	gdt::list_item pos_euv_in_fuv = pos_in_border(euv,fuv);
	gdt::list_item pos_euv_in_fwv = pos_in_border(euv,fwv);
	
	// debug
	//
	//std::cout << "\n new edge e = " << id(e) << std::flush;
	//std::cout << "\n ew = " << id(ew) << std::flush;	
	//std::cout << "\n w_pred = " << id(w_pred) << std::flush;	
	//std::cout << "\n eu = " << id(eu) << std::flush;	
	//std::cout << "\n u_pred = " << id(u_pred) << std::flush;		
	//
		
	// -----------------------------
	// after applying the operation
	/* -----------------------------
	                fwv
	                            ew 
	       \      swu          / 
	        \    <---         /  
	       u o-------e-------o w
	        /          --->   \
	       /           suw     \
	     eu
	                fuv
	   ---------------------------*/
	
	gdtedge e = undi_graph::weld_edges_at_node(v);
	
	// debug
	//
	//std::cout << "\n new edge e = " << id(e) << std::flush;
	//std::cout << "\n ew = " << id(ew) << std::flush;	
	//std::cout << "\n w_pred = " << id(w_pred) << std::flush;	
	//std::cout << "\n eu = " << id(eu) << std::flush;	
	//std::cout << "\n u_pred = " << id(u_pred) << std::flush;		
	//
	
	set_right_face_moving_along_edge (e,w,ew,w_pred);
	set_right_face_moving_along_edge (e,u,eu,u_pred);
	if (temp_u_pred)
	  del_node (temp_u_pred);
	if (temp_w_pred)
	  del_node (temp_w_pred);
	
	// -------------------------------------------
	// delete the border steps and their positions
	// -------------------------------------------
	
	
	// del the positions of the edges
	// in the border of the faces 
	//
	gdt::gdtlist<gdtedge>& be_fwv = fwv -> edges;
	gdt::gdtlist<gdtedge>& be_fuv = fuv -> edges;
	
	be_fwv.del_item (pos_ewv_in_fwv);
	be_fuv.del_item (pos_euv_in_fuv);
	if (fwv!=fuv) 
		{
		be_fuv.del_item (pos_ewv_in_fuv);
		be_fwv.del_item (pos_euv_in_fwv);
		}
		
	// delete the positions of the border steps
	// in the border of the faces
	//
	gdt::gdtlist<border_step>& bs_fwv = fwv -> border_steps;
	gdt::gdtlist<border_step>& bs_fuv = fuv -> border_steps;
	
	bs_fwv.del_item (pos_in_border(swv));
	bs_fwv.del_item (pos_in_border(svu)); 
	bs_fuv.del_item (pos_in_border(suv));
	bs_fuv.del_item (pos_in_border(svw)); 
	
	// delete the border steps
	//
	safe_delete(swv);
	safe_delete(svu);
        safe_delete(suv);
	safe_delete(svw);

	//Added by Marcandalli
	edge_info->undefine(euv);
	edge_info->undefine(ewv);
	
	// check to remove
	//
	//assert(internal_consistency_check());
	
	return e;
	}



	void
	plan_undi_graph::
clear
	()
	{
	undi_graph::clear();
	local_renew();
	}
	

	
	void
	plan_undi_graph::
steal_from (undi_graph& ug, planarize_options po, bool err_mess)
	{
	//cout << "\n undi_graph -(move)-> plan_undi_graph" << flush;
	//cout << "prima di clear()" << endl;
	clear();
	//cout << "dopo clear()" << endl;
	//
	// Copy all the pointers of ug
	// that is, the undi_graph part of this class
	//
	undi_graph::mirror (ug);
	//
	// Create a new empty environment for ug
	// in order to avoid side-effects with this
	//
	ug.forget();
	//
	// Build the extra part (i.e. the faces) of this,
	// according to the plan_undi part stolen from ug.
	// 
	local_renew();
	local_init(po,err_mess);
	}
	
	
	
	void
	plan_undi_graph:: 
mirror 
	(plan_undi_graph& pug)
	{
	gdt::gdtlist<face>* 			 p1;
	gdt::gdtedge_map<struct_plan_edge_info>* p2;
	gdt::gdtmap<int,face>* 			 p3;
	int				 p4;
	int				 p5;

	undi_graph::mirror (pug);
	pug.local_get (p1,p2,p3,p4,p5);
	local_set (p1,p2,p3,p4,p5);
	}
	
	
	
	void 
	plan_undi_graph::
forget 
	()
	{
	undi_graph::forget();
	local_new ();
	}
	
	

	void 
	plan_undi_graph::
assign_id 
	(gdtnode v, int new_id) 
	{
	undi_graph::assign_id (v, new_id);
	}
	
	
	
	void 
	plan_undi_graph::
assign_id 
	(gdtedge e, int new_id) 
	{
	undi_graph::assign_id (e, new_id);
	}
	
	

	void 
	plan_undi_graph::
assign_id 
	(face f, int new_id) 
	{
	int old_id = f->id;
	if (new_id <  NULL_ID)  gdt_error("illegal id."); 
	if (old_id != NULL_ID) (*face_with_id)[old_id] = NULL_FACE;
	if (new_id == AUTO_ID) new_id = generate_face_id();
	else if (new_id > max_face_id) max_face_id = new_id;
	else if (find_face(new_id)) gdt_error("duplicate id."); 
	(*face_with_id)[new_id]=f;
	f->id = new_id;
	}


	void
	plan_undi_graph::
update_max_face_id
	()
	{
	face f;
	max_face_id = NULL_ID;
	forall_faces (f,*this)
		if (max_face_id < id(f)) max_face_id = id(f);
	}

	
	gdtnode
	plan_undi_graph::
star_in_face(face f)
	{
	/*                                                                              

              o--o--o          o--o--o 
             /       \        / \ | / \   
            /         \      /   \|/   \  
           o           o    o-----o-----o
            \         /      \   /|\   /
             \       /        \ / | \ /   
              o--o--o          o--o--o     
  

	   We will apply the following algorithm:                        
                                                                         
		place a gdtnode in the middle of the face as follows:       
			take a border step                                
			take its stop_node and its gdtedge                   
			insert a gdtnode adj to stop_node, next to gdtedge found
		while there are other nodes around the face:              
			link these nodes with center gdtnode.
			
           first_node --->  o    _
                             \  |\	
           ref_edge --->      \   \  <---- border_step (face 'f' side)
                               \   \
                                o	
			                
	*/
	                                                                
	gdt::gdtlist<gdtnode> nodes_of_face = adj_nodes(f);     // -- nodes are in clockwise order
					             // -- if exists a bridge "dangling" 
						     //    inside the face the gdtnode appares 
						     //    more than one time
								
	border_step    bs = first_border_step(f);
	gdtnode   first_node = stop_of_border_step(bs);
	gdtedge     ref_edge = edge_of_border_step(bs);
	
	gdtnode center = new_node(first_node, ref_edge);   // insert a gdtnode adjacent to 'first_node' and next to 'ref_edge'
	
	// old wronged code
	/*	
	while( !nodes_of_face.empty() )
		{
		gdtnode node_to_link = nodes_of_face.pop();
		if( node_to_link != first_node )
			{
			gdtedge new_e = new_edge( node_to_link, center, f );
			f = right_face_moving_along_edge( new_e, center );  // updating f
			}
		}
	*/
	
	// new code (by Walter Didimo, February 2001) 
	//
	gdtedge e_new_first = first_adj_edge (center);	// the first time it is the only gdtedge that incides on gdtnode center
	bs = border_step_moving_along_edge(e_new_first,center);	
	bs = cyclic_succ_border_step (bs);
	gdtnode v = stop_of_border_step (bs);
	gdtedge e = edge_of_border_step (bs);
	gdtedge e_new = new_edge (center, e_new_first, v, e); 
	
	bs = border_step_moving_along_edge (e_new,center);
	bs = cyclic_succ_border_step (bs);
	v  = stop_of_border_step (bs);	
	e  = edge_of_border_step (bs);

	while (!(e == ref_edge && v == first_node))
		{
		e_new = new_edge (center, e_new_first, v, e); 
		bs = border_step_moving_along_edge (e_new,center);
		bs = cyclic_succ_border_step (bs);
		v  = stop_of_border_step (bs);	
		e  = edge_of_border_step (bs);
		}	
	//
	
	return center;
	}


	
	face 
	plan_undi_graph::
plan_with_fixed_face_of_undi_graph 
	(undi_graph& ug, gdtedge start_edge , gdtnode start_node)
	{
	//cout << "\nstarting plan_with_ext_face..\n" << flush;
	clear();
	*this = ug;
	gdtedge e_pug;
	gdtnode v_pug;
	face fix_face;
	
	if (start_node != ug.source(start_edge) && start_node != ug.target(start_edge))
		gdt_error("start_node is not a gdtnode of start_edge");	

	v_pug    = corresponding_node (start_node, ug); 
	e_pug    = corresponding_edge (start_edge, ug); 
	fix_face = right_face_moving_along_edge (e_pug,v_pug);
	
	return fix_face;
	}
	
	
	
	void
	plan_undi_graph::
make_embedding_as
	(const plan_undi_graph& pug)
	{
	//cout << "\nplan_undi_graph::make_embedding_as" << flush;
	undi_graph::make_embedding_as (pug);
	local_renew();
	build_faces();
	}

	
	
	gdtedge
	plan_undi_graph:: 
expand 
	(gdtnode v, gdtedge e_init, gdtedge e_end)
	{
	// This code is temporary, because it is inefficient. 
	// In fact, it executes a total rebuilding of the faces, 
	// instead of a local rebuilding.
	//
	gdtedge e = undi_graph::expand (v,e_init,e_end);
	local_renew();
	build_faces();
	return e;
	}
	
	
	
	gdt::gdtlist<gdtnode>
	plan_undi_graph::
make_simple
	()
	{
	gdt::gdtlist<gdtnode> dummy_nodes = undi_graph::make_simple();
	if (!dummy_nodes.empty())
		{
		local_renew();
		build_faces();
		}
	return dummy_nodes;
	}
	
	
	
	gdtnode 
	plan_undi_graph::
contract
	(gdtedge e)
	{
	// This code is temporary, because it is inefficient. 
	// In fact, it executes a total rebuilding of the faces, 
	// instead of a local rebuilding.
	//
	gdtnode v = undi_graph::contract (e);	
	local_renew();
	build_faces();	
	return v;
	}
	
	
	gdt::gdtlist<gdtnode>
	plan_undi_graph::
contract
	()
	{
	// This code is temporary, because it is inefficient. 
	// In fact, it executes a total rebuilding of the faces, 
	// instead of a local rebuilding.
	//
	gdt::gdtlist<gdtnode> Lv = undi_graph::contract ();
	local_renew();
	build_faces();	
	return Lv;	
	}
	

	void
	plan_undi_graph::
make_upward_embedding
	(
	face ext_face,
	gdt::gdtnode_array<gdtedge>& fe,
	int minimum_switches
	)
	{
	// ------------------------------ 0 ---------------------------
	// check that the external face ext_face is a face of the graph
	// and also make all the edges as undirected
	// ------------------------------------------------------------
	
	face f;				// face of the graph
	bool face_exists = false;
	forall_faces (f,*this)
		if (f == ext_face)
			{ 
			face_exists = true;
			break;
			}
	if (!face_exists) 
		gdt_error("invalid external face");
	
	// make the edges undirected
	//	
		{
		gdtedge e;
		forall_edges (e,*this) make_undirected(e);
		}
		
	// ------------------------------ 1 ---------------------------
	// create the flow-network associated with the planar embedding
	// ------------------------------------------------------------
	
	flow_dire_graph fn;	// the flow-net
	gdtnode v;			// vertex of the graph
	gdtnode n;			// gdtnode of the flow-net	
	gdtedge e;			// gdtedge of the graph
	gdtedge a;			// arc of the flow-net
	border_step s;		// border-step of the graph

	gdt::gdtmap<face,gdtnode>        from_face_to_node (NULL_NODE);		 // mapping from graph's faces to network's nodes 
	gdt::gdtmap<gdtnode,face> 	      from_node_to_face (NULL_FACE);		 // mapping from network's nodes to graph's faces
	gdt::gdtmap<border_step,gdtedge> from_border_step_to_arc (NULL_EDGE);	 // mapping from graph's border steps to network's arcs
	gdt::gdtmap<gdtedge,border_step> from_arc_to_border_step (NULL_BORDER_STEP);// mapping from network's arcs to graph's border steps
	        
	// NOTE: the mapping from graph's vertices to network's nodes (and vice-versa)
	//       will be guaranteed from the identifiers of the nodes
	
	// ------------------- 1.1 ----------------------
	// create all the nodes of the flow-net:
	// - there is a gdtnode for each vertex of the graph
	// - there is a gdtnode for each face of the graph
	// ----------------------------------------------
	
	forall_nodes (v,*this)
		{
		n = fn.new_node (id(v));
		fn.set_balance (n,2);
		}
	forall_faces (f,*this)
		{
		n = fn.new_node ();
		if (f == ext_face) fn.set_balance (n,-degree(f,true)-2);
		else		   fn.set_balance (n,-degree(f,true)+2);
		from_face_to_node[f] = n;
		from_node_to_face[n] = f;
		}
		
	// ------------------- 1.2 -------------------------
	// create all the arcs of the flow-net:
	// for each border step s, starting from vertex v,
	// in face f, there is a pair {a1,a2} of arcs 
	// from v to f. 
	// The settings of capacities and costs and the meaning 
	// of the flow on arcs a1 and a2 depend on the
	// setting of variable "minimum_switching":
	//
	// case minimum_switching = n (finite) 
	// 
	// - a1 has cost 0 and a2 has cost 1;
	// - both a1 and a2 have upper capacity 1;
	// - a unit of flow just on arc a1 means a 
	//   180 degree angle (non-switch angle);
	// - a unit of flow on both arcs a1 and a2 means
	//   a 360 degree angle (switch angle);
	// - a unit of flow just on arc a2 is not possible,
	//   in a min-cost-flow solution, 
	//   due to the costs of the two arcs.
	// 
	// case minimum switching = INFINITE 
	// 
	// - both a1 and a2 have cost and upper capacity 1;
	// - a unit of flow just either on arc a1 or arc a2
	//   means a 180 degree angle (non-switch angle);
	// - a unit of flow on both arcs a1 and a2 means
	//   a 360 degree angle (switch angle);
	// 
	// Finally, arcs a1 and a2 have consecutive 
	// identifiers (id(a2) = id(a1) + 1). 
	// -------------------------------------------------
	
	forall_faces (f,*this)
		forall_face_border_steps (s,f)
			{
			v = start_of_border_step(s);
			gdtnode nv = fn.corresponding_node (v,*this);
			gdtnode nf = from_face_to_node[f];
			gdtedge a1 = fn.new_edge (nv,nf);
			if (minimum_switches < INFINITE)
				fn.set_edge_info (a1,1,0,0,0); // up-cap., low-cap., cost, flow			
			else    fn.set_edge_info (a1,1,0,1,0); // up-cap., low-cap., cost, flow
			
			from_border_step_to_arc[s] = a1;
			from_arc_to_border_step[a1] = s;
			
			gdtedge a2 = fn.new_edge (nv,nf,fn.id(a1)+1);
			
			fn.set_edge_info (a2,1,0,1,0); // up-cap., low-cap., cost, flow			
			
			from_arc_to_border_step[a2] = NULL_BORDER_STEP;
			}	
	
	// ------------------- 1.3 -------------------------
	// set flow constraints on the angles at crosses 
	// vertices. Namely, a cross-vertex will be never
	// a source or a sink
	// ------------------------------------------------- 
	/*	
	forall_nodes (v,*this)
		{
		if (is_marked(v,RM3_CROSS_ON_REAL_EDGE))
			{
			gdtedge e1 = first_adj_edge  (v);
			gdtedge e2 = cyclic_adj_succ (e1,v);
			gdtedge e3 = cyclic_adj_succ (e2,v);
			gdtedge e4 = cyclic_adj_succ (e3,v);
			border_step s1 = border_step_moving_along_edge (e1,v);			
			border_step s2 = border_step_moving_along_edge (e2,v);			
			border_step s3 = border_step_moving_along_edge (e3,v);			
			border_step s4 = border_step_moving_along_edge (e4,v);
			
			gdtedge a1 = from_border_step_to_arc[s1];
			gdtedge a2 = from_border_step_to_arc[s2];
			gdtedge a3 = from_border_step_to_arc[s3];
			gdtedge a4 = from_border_step_to_arc[s4];						
			
			a1 = fn.find_edge (fn.id(a1) + 1);
			a2 = fn.find_edge (fn.id(a2) + 1);
			a3 = fn.find_edge (fn.id(a3) + 1);
			a4 = fn.find_edge (fn.id(a4) + 1);
			
			fn.set_edge_info (a1,0,0,0,0); // up-cap., low-cap., cost, flow
			fn.set_edge_info (a2,0,0,0,0); // up-cap., low-cap., cost, flow
			fn.set_edge_info (a3,0,0,0,0); // up-cap., low-cap., cost, flow
			fn.set_edge_info (a4,0,0,0,0); // up-cap., low-cap., cost, flow
			}
		}
	*/
	// -------------------- 2 --------------------
	// compute a min-cost-flow in the flow-network
	// and check the correctness of the solution
	// -------------------------------------------

	if (!fn.min_cost_flow ())
		gdt_error("upward embedding doesn't exist");
	
	forall_edges (a,fn)
		if (from_arc_to_border_step[a])
			{
			gdtedge a1 = a;
		 	gdtedge a2 = fn.find_edge(fn.id(a1)+1);
			if (minimum_switches<INFINITE && !fn.get_flow(a1) && fn.get_flow(a2))
				gdt_error("invalid pair of flow values"); 
			}
	
	// ------------------ 2.1 ---------------------
	// make n cutting nodes as switches if possible
	// by locally modifying the computed flow, where
	// n is the value of minimum switches parameter
	// --------------------------------------------
	if (minimum_switches < INFINITE && minimum_switches > 0)
		{
		int count = 0;
		gdtnode v;
		gdtedge a1,a2;
		forall_nodes (v,*this)
			{
			gdtnode u = fn.find_node(id(v));
			gdtedge a1ref = NULL_EDGE;
			//Modified by A.M.
			forall_edges_adjacent_node(a1,u,fn)
				if (from_arc_to_border_step[a1]) // a1 is the representative arc
					{
					a2 = fn.find_edge(fn.id(a1)+1); 
					if ( (fn.get_flow(a1)==1 && fn.get_flow(a2)==0) ||
					     (fn.get_flow(a1)==0 && fn.get_flow(a2)==1) )
						{
						if (!a1ref) a1ref = a1;
						else if (fn.opposite(u,a1)==fn.opposite(u,a1ref))
							{
							int k = random (0,1);
							gdtedge a2ref = fn.find_edge(fn.id(a1ref)+1);
							// set flow k on a1 and a2
							// set flow 1-k on a1ref and a2ref
							fn.set_flow (a1,k); fn.set_flow (a2,k);
							fn.set_flow (a2ref,1-k); fn.set_flow (a1ref,1-k);
							count ++;
							break;
							}

						}
					}
			if (count == minimum_switches) break;
			}
		}
		
	// -------------------- 3 --------------------
	// construct the upward embedding depending on
	// the computed min-cost-flow
	// -------------------------------------------

	forall_nodes (v,*this) fe[v] = NULL_EDGE;

	// -------------------- 3.1 ---------------------
	// find a first vertex of the graph that can be 
	// considered as a source in the upward embedding
	// ----------------------------------------------

	forall_edges (a,fn)
		if (from_arc_to_border_step[a])
			{
			gdtedge a1 = a;
		 	gdtedge a2 = fn.find_edge(fn.id(a1)+1);
			int ax = fn.get_flow(a1) + fn.get_flow(a2);
			if (ax==2) break;
			}
	n = fn.start(a);
	v = corresponding_node (n,fn);	// the first source

	// -------------------- 3.2 --------------------
	// all the edges incident on v will be oriented
	// outgoing from v, and the left-most gdtedge will
	// be choosen by using the flow values. Then, a 
	// BFS of the graph is performed in order to 
	// orient all the other edges consistency with
	// the choice that v is a source and the flow is
	// used to determine the angles at vertices
	// ---------------------------------------------

	gdt::gdtqueue<gdtedge> Q_edges;
	gdt::gdtqueue<gdtnode> Q_nodes;
	forall_edges_adjacent_node (e,v,*this)
		{
		Q_edges.append(e);
		Q_nodes.append(opposite(v,e));
		make_directed (e,v);
		}
	s = from_arc_to_border_step[a];
	s = cyclic_pred_border_step(s);
	fe[v] = edge_of_border_step(s);
 
	while (!Q_edges.empty())
	   {
	   e = Q_edges.pop();
	   v = Q_nodes.pop();
	   
	   for (int i=1; i<=degree(v); i++)
		{
		gdtedge e_succ = cyclic_adj_succ (e,v);
		s = border_step_moving_along_edge (e_succ,v);
		gdtedge a1 = from_border_step_to_arc[s];
		gdtedge a2 = fn.find_edge (fn.id(a1) + 1);
		int  ax1 = fn.get_flow(a1);
		int  ax2 = fn.get_flow(a2);
		int  ax  = ax1 + ax2;
				
		if (!edge_is_directed(e_succ)) // orient the gdtedge and set fc
			{
			Q_edges.append(e_succ);
			Q_nodes.append(opposite(v,e_succ));
			switch (ax)
				{
				case 0: if (start(e) == v) make_directed(e_succ,v);
					else		   make_directed(e_succ,opposite(v,e_succ));						
					break;
				case 1: if (start(e) == v) make_directed(e_succ,opposite(v,e_succ)); 	
					else		   make_directed(e_succ,v);
					break;
				case 2: if (start(e) == v) // v is a source 
						{
						make_directed(e_succ,v);
						fe[v] = e;
						}
					else    	   // v is a sink	
						{
						make_directed(e_succ,opposite(v,e_succ));
						fe[v] = e_succ;
						}
					break;
				default:
					gdt_error("invalid flow value"); 
					break; 
				}
			}
		else // check flow/upward consistency and set fc
			{
			switch (ax)
				{
				case 0:
				case 2: if (start(e) != start(e_succ) && 
					    stop(e) != stop(e_succ))
						gdt_error("flow/upward inconsistency");
					if (ax == 2) 
						if (start(e) == v) fe[v] = e;	   // v is a source
						else        	   fe[v] = e_succ; // v is a sink	
					break;
				case 1: if (start(e) == start(e_succ) ||
					    stop(e) == stop(e_succ))
						gdt_error("flow/upward inconsistency");
					break;
				default:
					gdt_error("invalid flow value"); 
					break;
				}
			}
		e = e_succ;
		}
	   }
	}


	void
	plan_undi_graph::
generate_biconnected
	(int num_nodes, double prob_iv, int max_degree, bool multiple)
	{
	if (prob_iv <= 0 || prob_iv >= 1)
		gdt_error("invalid value for prob_iv");
	
	cout << "\nGENERATING GRAPH WITH " << num_nodes << " NODES\n" << flush;
	
	//double prob_iv = (1/(1+density))*100;
	
	double d;			// density (number of edges/ number of nodes)
	prob_iv = prob_iv * 100;
	
	//do
		{
		double num_iv = 0;
		double num_ie = 0;
	
		// --------------------------------------------
		// Initialize a plan_undi_graph with a triangle
		// --------------------------------------------
		//
		undi_graph ug;
		gdtnode v1 = ug.new_node ();
		gdtnode v2 = ug.new_node ();
		gdtnode v3 = ug.new_node ();
		ug.new_edge (v1,v2);
		ug.new_edge (v2,v3);
		ug.new_edge (v1,v3);
	
		*this = ug;
	
		// ----------------------------------
		// Make all the other nodes and edges
		// ----------------------------------
		
		bool insert_edge_failed = false;
		
		while (num_iv < (num_nodes - 3))
			{
			// -------------------------------
			// Choose a new operation iv or ie
			// -------------------------------
			
			int n;
			
			if (insert_edge_failed) {n = 0; insert_edge_failed = true;}
			else 			n = random (1,100);

			if (n <= prob_iv) 
				{
				// -----------------
				// Operation type iv
				// -----------------
				//
				//cout << "\nOPERATION TYPE iv" << flush;
				
				int 	  i   = random (0,number_of_edges() - 1);
								
				gdt::list_item pos = (all_edges()).get_item(i);				
				
				//
				gdtedge 	  e   = (all_edges()).inf(pos);
				int	  ide = id(e);
				gdtnode 	  v   = new_node (e);
				gdtedge      e1  = first_adj_edge (v);
				gdtedge      e2  = adj_succ (e1,v);
				if (id(e1) < id(e2)) assign_id (e2,ide);
				else		     assign_id (e1,ide);
				update_max_edge_id();	
				//cout << "\nADDED NEW NODE " << id(v) << flush;
				num_iv ++;
				}
			else
				{
				// -----------------
				// Operation type ie
				// -----------------
				//
				//cout << "\nOPERATION TYPE ie" << flush;
				
				int k  = number_of_nodes();
				int k2 = k*k;
				int number_of_trials  = 0;
				bool found_nodes = false;
				
				while (number_of_trials<=k2 && !found_nodes)
					{
					int i;   					 
					gdtnode v1,v2;
					
					// -----------------
					// Choose a gdtnode v1
					// -----------------
					//
					//cout << "\nEXTRACTING NUMBER FROM 0 TO " << k-1 << flush;
					i   = random (0,k-1);
										
					gdt::list_item pos;					
					pos = (all_nodes()).get_item(i);
					v1  = (all_nodes()).inf(pos);
					if (!v1) continue;

					// ----------------------
					// Choose a gdtnode v2 != v1
					// ----------------------
					//
					v2 = v1;
					while (v2 == v1)
						{
						i   = random (0,(number_of_nodes()-1));
						pos = (all_nodes()).get_item(i);
						v2  = (all_nodes()).inf(pos);
						}

					if (!v2) continue;
					//cout << "\n  choosed gdtnode " << pug.id(v1) << " and " << pug.id(v2) << flush;

					// -----------------------------------------------------
					// Check if it is possible add an gdtedge between v1 and v2
					// -----------------------------------------------------
					//

					if (degree(v1) < max_degree && degree(v2) < max_degree)
						{
						// ----------------------------------------------
						// Make a list of faces containing both v1 and v2
						// and, if it's not empty, then choose a face in
						// this list
						// ----------------------------------------------
						//
						face f;
						gdt::gdtlist<face> f_list;
						forall_faces(f,*this)
							if (node_belongs_to_face (v1,f) && node_belongs_to_face (v2,f))
								f_list.append(f);
						if (f_list.size() > 0)
							{
							//cout << "\n  new gdtedge is possible" << flush;
							i   = random (0,f_list.size() - 1);

							//Cast (dlink*) added to compile with VC++
							//to remove (è sbagliato ma innocuo)
							//A. Marcandalli 3-8-2001
							//pos = (dlink*) f_list.get_item(i);
                            // Rimosso Pier Francesco Cortese 4-2-2004
							pos = f_list.get_item(i);

							f   = f_list.inf( (void*) pos);

							new_edge (v1,v2,f);
							//cout << "\n  added new gdtedge " << id(e) << flush;
							num_ie ++;
							found_nodes = true;
							}
						else number_of_trials++;
						}
						
					else number_of_trials++;
					}
				if (number_of_trials >= k2) insert_edge_failed = true;
				}
			}
		
		//d = ((double)(num_ie)/(double)(num_iv));
		
		if (!multiple)
			{
			// -------------------------
			// remove all multiple edges
			// -------------------------
			
			gdtnode v;
			gdtedge e;
			forall_nodes (v,*this)
				{
				gdt::gdtnode_map<bool> mult (nodes_and_edges(),false);
				gdt::gdtlist<gdtedge> E = adj_edges(v);
				forall (e,E)
					{
					gdtnode w = opposite(v,e);
					if (!mult[w]) mult[w] = true;
					else del_edge(e);
					}
				}
			}
		
		
		d = ((double)number_of_edges()/((double)number_of_nodes()));
		cout << "\nOPERATIONS insert gdtnode = " << num_iv << flush;
		cout << "\nOPERATIONS insert gdtedge = " << num_ie << flush;
		cout << "\ndensity = " << d << flush;
		}
	//while (gdt_maximum(d-density,density-d) > 0.2);
	}
	
	
	
	// -------------------------
	// Input / output operations
	// -------------------------

 	void
	plan_undi_graph::
print
	(print_mode mode, ostream& os) const 
	{	
	switch (mode)
		{
		case BY_NODES:			
		case BY_EDGES:
			undi_graph::print(mode,os);
			break;
		case BY_FACES:
			face f;
			forall_faces(f,(*this))
				{
				print (f,os);
				os << "\n" << flush;
				}
			break;
		case COMPLETE:
			undi_graph::print(COMPLETE,os);
			print(BY_FACES,os);
			break;
		case FOR_DEBUG: //Added by Marcandalli
			undi_graph::print(FOR_DEBUG);
			os << "faces" << endl;
			print(BY_FACES);
			break;
		default:
			break;
		}
	}
						


	void
	plan_undi_graph::
print
	(gdtnode v, ostream& os) const 
	{
	undi_graph::print(v,os);
	}



	void
	plan_undi_graph::
print
	(gdtedge e, ostream& os) const 
	{
	undi_graph::print(e,os);
	}



	void
	plan_undi_graph::
print
	(face f, ostream& os)  const 
	{
	gdtnode v;
	gdtedge e;
	border_step s;

	//Debug
	os << "<" << f << " " << id(f) << "> "  << " edges " << f->edges << endl;
	//

	os << "<" << id(f) << "> : ";
	forall_face_border_steps (s,f)
		{
		v = start_of_border_step(s);
		e = edge_of_border_step(s);
		print(v);
		if (edge_is_directed(e) && (v==stop(e))) os << "<";
		print(e);
		if (edge_is_directed(e) && (v==start(e))) os << ">";
		}
	}
	
		

	void
	plan_undi_graph::
print
	(constraint c, ostream& os) const
	{
	undi_graph::print(c,os);
	}
	
	
	
	
	void
	plan_undi_graph::
print
	(separation_pair sp, ostream& os) const 
	{
	face f;
	
	os << "(" << id(separation_pair_pole1(sp)) << "," << id(separation_pair_pole2(sp)) << ")";
	os << "  SHARED FACES: ";
	gdt::gdtlist<face> F=faces_shared_by_separation_pair(sp);
	forall (f,F)
		{
		os << "f" << id(f) << " ";
		}
	os << "\n" << flush;
	}	

//-----------------------------------------------------------------------------
//	PROTECTED
//-----------------------------------------------------------------------------

	void
	plan_undi_graph::
set_source 
	(gdtedge e, gdtnode new_source)
	{
	//cout << "\nplan_undi_graph::set_source" << flush;

	gdtnode old_source = source(e);
	gdtnode old_target = target(e);

	if (new_source != old_source)
		{
		if (new_source != old_target) gdt_error("invalid gdtnode or gdtedge"); 
		else 
			{
			//gdtnode new_target = old_source;

			face old_left_face  = (*edge_info)[e].left_face;
			face old_right_face = (*edge_info)[e].right_face;

			border_step old_left_face_border_step  = (*edge_info)[e].left_face_border_step;
			border_step old_right_face_border_step = (*edge_info)[e].right_face_border_step;

			gdt::list_item old_pos_in_left_face_edges   = (*edge_info)[e].pos_in_left_face_edges;
			gdt::list_item old_pos_in_right_face_edges  = (*edge_info)[e].pos_in_right_face_edges;

			undi_graph::set_source(e,new_source);

			(*edge_info)[e].left_face  = old_right_face;
			(*edge_info)[e].right_face = old_left_face;

			(*edge_info)[e].right_face_border_step = old_left_face_border_step;
			(*edge_info)[e].left_face_border_step  = old_right_face_border_step;

			(*edge_info)[e].pos_in_right_face_edges = old_pos_in_left_face_edges;
			(*edge_info)[e].pos_in_left_face_edges  = old_pos_in_right_face_edges;
			}
		}
	}



	void
	plan_undi_graph::
set_faces_shared_by_separation_pair
	(const gdt::gdtlist<face>& f_list, separation_pair sp)
	{
	face f;
	
	forall (f,sp->shared_faces) safe_delete(f);
	sp->shared_faces.clear();
	forall (f,f_list)
		{
		sp->shared_faces.append(f);
		}
	}
	

	
	void
	plan_undi_graph::
set_separation_pair_pole1
	(gdtnode v, separation_pair sp)
	{
	sp->pole1 = v;
	}
	
	
	
	void
	plan_undi_graph::
set_separation_pair_pole2
	(gdtnode v, separation_pair sp)
	{
	sp->pole2 = v;
	}
	


	gdtnode
	plan_undi_graph::
first_node_visited_while_moving_on_edge_around_face
	(
	gdtedge e,
	face f
	) const
	{
	gdtnode vs = source(e);
	gdtnode vt = target(e);
	if (right_face_moving_along_edge(e,vs)==f)  return vs;
	else                                        return vt;
	}



	gdtnode
	plan_undi_graph::
second_node_visited_while_moving_on_edge_around_face
	(
	gdtedge e,
	face f
	) const
	{
	gdtnode vs = source(e);
	gdtnode vt = target(e);
	if (right_face_moving_along_edge(e,vs)==f) return vt;
	else                                       return vs;
	}



	void
	plan_undi_graph::
calculate_dual
	(plan_undi_graph& dual, gdt::gdtmap<face,gdtnode>& dual_node_of_face)
	{
	calculate_dual (dual,first_edge(),dual_node_of_face);
	}


/* -----------------------------------------------------------------------
	// Note: since plan_undi_graph cannot represent self-loops,
	// this function does not work properly on a 2-nodes graphs
	// with a unique gdtedge. 
	//
	void
	plan_undi_graph::
calculate_dual
	(plan_undi_graph& dual, gdtedge st_edge, gdt::gdtmap<face,gdtnode>& dual_node_of_face)
	{
	gdtedge e,ed;
	face f,fe,fl;
	undi_graph ug;
	gdtnode v1,v2,vd1,vd2,link;
	gdt::gdtedge_array<gdtnode>  dual_link_on_edge;         // *this.gdtedge -> ug.link		
	gdt::gdtnode_array<bool>  link_not_redundant;           // ug.gdtnode -> true|false		
	gdt::gdtnode_matrix<bool> dual_nodes_already_connected; // (ug.gdtnode, ug.gdtnode) -> true|false

	dual_link_on_edge.init (nodes_and_edges(), NULL_NODE);

	forall_faces (f,*this) dual_node_of_face[f] = ug.new_node();
	forall_edges (e,*this) dual_link_on_edge[e] = ug.new_node();

	link_not_redundant.init           (ug.nodes_and_edges(), false);	
	dual_nodes_already_connected.init (ug.nodes_and_edges(), false);
		
	forall_faces (f,*this)
		{
		e  = last_adj_edge (f);
		while (e)
			{
			fe = adj_face (f,e);
			//cout << "\nFace" << id(fe) << flush;
			if (f != fe) // if e is not a bridge
				{
				vd1 = dual_node_of_face [f];
				vd2 = dual_node_of_face [fe];

				link = dual_link_on_edge[e];
				ed   = ug.new_edge (vd1,link);

				if (edge_is_directed(e))
					{
					v1 = start(e);
					v2 = stop(e);
					fl = left_face_moving_along_edge(e,v1);

					if (f==fl) ug.make_directed (ed,vd1);
					else       ug.make_directed (ed,link);

					// Handle the special case, when e as the gdtedge
					// which induced an st-orientation on *this.
					// In that case, direction along ed is reversed.
					//
					if (e == st_edge) ug.reverse(ed);
					}									

				// Check against multiple edges in the dual graph.
				// Non-redundant links are marked in order to preserve them
				// once the embedding is completely defined.
				//
				if (!dual_nodes_already_connected(vd1,vd2))
					{
					dual_nodes_already_connected(vd1,vd2) = true;
					dual_nodes_already_connected(vd2,vd1) = true;
					link_not_redundant[link]=true;
					}
								  
				}
			e=adj_pred(e,f);
			}
		}
		
	//cout << "\nUNDI NON SALDATO"; ug.print(COMPLETE);
	//forall_faces (f,*this) {cout << "\nFace "; print(f); cout << " is gdtnode "; ug.print(dual_node_of_face[f]);}

	forall_edges (e,*this)
		{
		link = dual_link_on_edge[e];
		if (link_not_redundant[link]) ug.weld_edges_at_node(link);
		else                          ug.del_node(link);
		}		
	//cout << "\nUNDI SALDATO"; ug.print(COMPLETE);
	//forall_faces (f,*this) {cout << "\nFace "; print(f); cout << " is gdtnode "; ug.print(dual_node_of_face[f]);}
	
	// remove multiple edges
	//ug.make_simple();
	
	dual.steal_from(ug);
	}
----------------------------------------------------------------------- */

	// Replaced by W.D. on 2005 January 7 
	// Note: since plan_undi_graph cannot represent self-loops,
	// this function does not work properly on a 2-nodes graphs
	// with a unique gdtedge. 
	//
	void
	plan_undi_graph::
calculate_dual
	(plan_undi_graph& dual, gdtedge st_edge, gdt::gdtmap<face,gdtnode>& dual_node_of_face)
	{
	gdtedge e,ed;
	face f,fl,fr;
	undi_graph ug;
	gdtnode v1,vd1,vd2;

	forall_faces (f,*this) dual_node_of_face[f] = ug.new_node();
	
	forall_edges (e,*this)
		{
		if (edge_is_directed(e)) v1 = start(e);			
		else 			 v1 = source(e);
			
		fl = left_face_moving_along_edge (e,v1);
		fr = right_face_moving_along_edge (e,v1);
		
		if (fr != fl)
			{
			vd1 = dual_node_of_face[fl];
			vd2 = dual_node_of_face[fr];
			if (!ug.find_edge(vd1,vd2))
				{
				ed = ug.new_edge (vd1,vd2);
				if (edge_is_directed(e))
					if (e != st_edge) ug.make_directed (ed,vd1);
					else	          ug.make_directed (ed,vd2);			
				}
			}
		}	
	//cout << "prima di steal from" << endl;
	dual.steal_from(ug);
	//cout << "dopo steal from" << endl;
	}


//-----------------------------------------------------------------------------
//	PRIVATE
//-----------------------------------------------------------------------------

	void
	plan_undi_graph::
local_new
	()
	{
	//cout << "\n plan_undi_graph::local_new" << flush;
	f_list      = new (gdt::gdtlist<face>);
	edge_info   = new gdt::gdtedge_map<struct_plan_edge_info>(*this);
	//edge_info->init(nodes_and_edges());
	face_with_id = new gdt::gdtmap<int,face>(NULL_FACE);
	max_face_id  = NULL_ID;
	last_crosses = 0;
	}



	void
	plan_undi_graph::
local_del
	()
	{
	//cout << "\n plan_undi_graph::local_del" << flush;
	face f;
	//forall_faces (f,*this) 	// W.D. March 1999
	//	delete(f);		// W.D. March 1999
		
	gdt::gdtlist<face> F = all_faces();	// W.D. March 1999
	
	//////////////////////////////////////////// P.F.C 9/10/2006
	//forall(f,F)
	//{
	//	while (!(f->border_steps).empty()) delete (f->border_steps.pop());
	//}
	////////////////////////////////////////////
	
	
	
	forall (f,F) safe_delete(f);	// W.D. March 1999
	//
	
	
	safe_delete (f_list);
	safe_delete (edge_info);
	safe_delete (face_with_id);
	}



	void
	plan_undi_graph::
local_renew
	()
	{
	local_del();
	local_new();
	}
	
			
	
	void
	plan_undi_graph::
local_init
	(planarize_options po, bool err_mess)
	{
	#ifdef DEBUG
	cout << "plan_undi_graph::local_init()\tnodes: " << number_of_nodes() << "\tedges:" << number_of_edges() << endl;;
	#endif

	int crosses = 0;
	if (!is_connected()) gdt_error("graph is not connected");
	if (number_of_nodes()>0)
		{
		// looking for SAME_FACE type planarization constraints.
		// If any, the planarize method is always performed.
		//
		bool same_face_constraints = false;
		constraint c;
		forall_constraints (c,*this)
			if (type_of_constraint(c) == SAME_FACE_NODES ||
			    type_of_constraint(c) == SAME_FACE_ORDERED_NODES)
			    {
			    same_face_constraints = true;
				break;
			    }

		if (same_face_constraints || !build_faces())
			{
			// the given embedding is not planar and then it needs
			// to look for a planar embedding if it exists.
			//
			#ifdef DEBUG
            cout << "The embedding is not planar, the graph must be planarized" << endl;
			#endif
			switch (po)
				{
				case DO_NOT_PRESERVE_EMBEDDING:
					{
					if (same_face_constraints ||
					    !make_embedding_planar()) {
						crosses = planarize (DO_NOT_PRESERVE_EMBEDDING);
						}
					#ifdef DEBUG
                    cout << "case: DO_NOT_PRESERVE_EMBEDDING" << endl;
					#endif
					}
					break;

				case PRESERVE_EMBEDDING:
					{
					crosses = planarize (PRESERVE_EMBEDDING);
					}
					break;
				}
			
			
			// the given graph is planar and a planar embedding has been found.
			// Now we must build the faces with respect this embedding
			//
			local_renew();
			build_faces(); 
			#ifdef DEBUG
			cout << "End of plan_undi_graph::local_init()\n" << flush;
			#endif
			//cout << "\n.. GRAPH AFTER MAKE_PLANAR_EMBEDDING" << flush;
			//print (COMPLETE);
			//cout << flush;
			}
		}
		
	last_crosses = crosses;
	if (err_mess && last_crosses == INFINITE)
		gdt_error("embedding not found");
	//if (!err_mess) cout << "\ncrosses = " << last_crosses << flush;
	}
	
	

	bool
	plan_undi_graph::
build_faces
	()
	{
	gdtnode v,v1;
	gdtedge e,e1;
	face f;	
	int steps_counter   = 0;
	bool too_many_steps = false;
	
	const int expected_steps = 2*number_of_edges();
	const int expected_faces = number_of_edges()- number_of_nodes() + 2;
	
	forall_nodes (v,*this)
		forall_edges_adjacent_node (e,v,*this) 
			{
			if (!right_face_moving_along_edge(e,v))        
				{
				//cout << "\nCreating face" << flush;
				f=new_face(); 
					
				//cout << "\nAdding gdtedge to face" << flush;
				set_right_face_moving_along_edge (e,v,f);
				steps_counter ++;
					
	 			//Append all the other edges around face f		
				v1=opposite(v,e);
				e1=cyclic_adj_succ (e,v1);
				while (!right_face_moving_along_edge(e1,v1) && !too_many_steps)
					{
					//cout << "\nAdding gdtedge to  face" << flush;
					set_right_face_moving_along_edge (e1,v1,f);
					steps_counter++;
						
					// Internal planarity control
					// on the number of edges around face f
					too_many_steps = (steps_counter > expected_steps);
						
					// Jump to next gdtedge 
					// around face f
					v1=opposite(v1,e1);
					e1=cyclic_adj_succ (e1,v1);	
					}
				if ((e1!=e) || too_many_steps) return false;
				}
			}
				
	//cout << "\n" << steps_counter << " border_steps added to " << number_of_faces() << " faces \n" << flush;
	if (number_of_faces() != expected_faces) return false;
	return true;
	}
	
	

	void
	plan_undi_graph::	
local_set
	(
	gdt::gdtlist<face>* 			 p1,         
	gdt::gdtedge_map<struct_plan_edge_info>* p2,
	gdt::gdtmap<int,face>* 			 p3,
	int				 p4,
	int				 p5
	)
	{
	//////////////////////////
	while (!f_list->empty()) delete (f_list->pop_front());
	delete f_list;
	delete edge_info;
	delete face_with_id;
	//////////////////////////
	f_list       = p1;
	edge_info    = p2;
	face_with_id = p3;
	max_face_id  = p4;
	last_crosses = p5;
	}


	
	gdtnode
	plan_undi_graph::
new_node
	(int id) 
	{
	return undi_graph::new_node(id);
	}
	
		

	gdtedge
	plan_undi_graph::
new_edge (gdtnode v1, gdtedge e1, gdtnode v2, int id) 
	{
	gdtedge e;
	e = undi_graph::new_edge(v1,e1,v2,id);
	reset_edge_info(e);
	return e;
	}


	
	gdtedge
	plan_undi_graph::
new_edge (gdtnode v1, gdtnode v2, int ide)
	{
	gdtedge e;
	e = undi_graph::new_edge(v1,v2,ide);
	reset_edge_info(e);
	return e;
	}


	
	face
	plan_undi_graph::
new_face 
	(int new_id) 
	{
	face f;
	f = new(struct_face);
	f->owner = this;
	f->pos_in_f_list = f_list->append(f);
	assign_id(f, new_id);
	return f;
	}



	void
	plan_undi_graph::
del_face
	(face f)
	{
	(*face_with_id)[f->id]=NULL_FACE;
	(*f_list).del_item(f->pos_in_f_list);
	safe_delete (f);
	}



	border_step
	plan_undi_graph::
new_border_step
	(gdtedge e, gdtnode v, face f)
	{
	border_step s;
	s = new struct_border_step;
	s->owner_edge = e;
	s->owner_face = f;
	s->start_node = v;
	return s;
	}
	
	

	/*
	Description
		Append gdtedge 'e' to the border of face 'f'.
		Set face 'f' as the one visible on the right when moving along gdtedge 'e' from 'v' to its opposite.
	*/
	void
	plan_undi_graph::
set_right_face_moving_along_edge
	(gdtedge e, gdtnode v, face f)
	{
	border_step s;
	gdt::list_item edge_pos;
	gdt::list_item step_pos;

	gdt::gdtlist<gdtedge>& be        = f->edges;
	gdt::gdtlist<border_step>& bs = f->border_steps;
	struct_plan_edge_info& ei   = (*edge_info)[e];
		
	// append gdtedge e to border be only if e is not already there.
	// (special case for bridge edges)
	//
	if      (f==ei.left_face)  edge_pos = ei.pos_in_left_face_edges;
	else if (f==ei.right_face) edge_pos = ei.pos_in_right_face_edges;
	else                       edge_pos = be.append(e);

	// create a new border_step
	// and append it to border bs
	//
	s = new_border_step (e,v,f);
	step_pos = bs.append(s);
	s->pos_in_border_steps = step_pos;
	
	if      (v==source(e))
		{
		ei.right_face=f;
		ei.right_face_border_step = s;
		ei.pos_in_right_face_edges = edge_pos;
		}
	else if (v==target(e))
		{
		ei.left_face=f;
		ei.left_face_border_step = s;
		ei.pos_in_left_face_edges = edge_pos;
		}
	}



	/*
	Description
		Find the face 'f' visible on the right when moving along gdtedge 'eref' from gdtnode 'vref' to its opposite.
		Insert gdtedge 'e' to the border of face 'f', after or before gdtedge 'eref' according to 'dir'.
		Set face 'f' as the one visible on the right when moving along gdtedge 'e' from 'v' to its opposite.
	Preconditions
		'v' belongs to 'e'
		'vref' belongs to 'eref'
	*/
	void
	plan_undi_graph::
set_right_face_moving_along_edge
	(gdtedge e, gdtnode v, gdtedge eref, gdtnode vref, int dir) 
	{		

	face f;
	border_step s,sref;
	gdt::list_item edge_pos;
	gdt::list_item step_pos;
	gdt::list_item sref_pos;
	gdt::list_item  pos_of_eref_in_be;
	
	if (v != source(e) && v != target(e))             gdt_error("invalid (gdtedge,gdtnode) pair");
	if (vref != source(eref) && vref != target(eref)) gdt_error("invalid reference (gdtedge,gdtnode) pair");
	
	sref = border_step_moving_along_edge(eref,vref);
	sref_pos = pos_in_border(sref);
	
	f = face_of_border_step(sref);

	//cout << "set_right_face_moving_along_edge" << endl;
	//print(BY_FACES);
	
	gdt::gdtlist<gdtedge>&            be  = f->edges;
	gdt::gdtlist<border_step>&     bs  = f->border_steps;
	struct_plan_edge_info& efi = (*edge_info)[e];
	pos_of_eref_in_be          = pos_in_border(eref,f);
	
	// insert gdtedge 'e' to border 'be'
	// only if 'e' is not already there
	// (special case for bridge edges)
	//
	if (f == efi.left_face)  edge_pos = efi.pos_in_left_face_edges;
	else 
		if (f == efi.right_face) edge_pos = efi.pos_in_right_face_edges;
	    else edge_pos = be.insert(e, pos_of_eref_in_be, dir);
	
	// create a new border_step 
	// and append it at border bs.
	//
	s = new_border_step (e,v,f);
	
	step_pos = bs.insert(s, sref_pos, dir);
	s->pos_in_border_steps = step_pos;
	
	if      (v == source(e))
		{
		efi.right_face = f;
		efi.right_face_border_step = s;
		efi.pos_in_right_face_edges = edge_pos;
		}
	else if (v == target(e))
		{
		efi.left_face = f;
		efi.left_face_border_step = s;
		efi.pos_in_left_face_edges = edge_pos;
		}	

	}
	
	
	
	void
	plan_undi_graph::
reset_edge_info
	(gdtedge e)
	{
	struct_plan_edge_info& ei = (*edge_info)[e];
	ei.left_face              = NULL_FACE;
	ei.right_face             = NULL_FACE;
	ei.left_face_border_step  = NULL_BORDER_STEP;
	ei.right_face_border_step = NULL_BORDER_STEP;  
	}



	void
	plan_undi_graph::
find_vertical_lengths 
	(gdt::gdtedge_array<int>& len, gdtedge e_st, gdt::gdtedge_array<int>& cost)
	{
	face f;					// face of *this
	gdtnode v,					// gdtnode of *this
	     vfn;				// gdtnode of fn
	gdtedge e,					// gdtedge of *this
	     efn;				// gdtedge of fn	
	gdtnode s = start(e_st),			// source gdtnode of *this
	     t = stop (e_st);			// sink   gdtnode of *this
	
	// case only s and t nodes
	//	
	if (number_of_nodes() == 2) 
		forall_edges (e,*this) len[e] = 1;
		
	// general case
	//
	else
		{	
		// ---------------- 0 -----------------
		//  COMPUTE A LONGEST PATH FROM s to t
		//    AND STORE THE EDGES IN A LIST L
		// ------------------------------------

		dire_graph DAG (*this);

		gdt::gdtlist<gdtedge> 	 L;						// edges of a longest path from s to t
		gdt::gdtedge_array<int>  length   (DAG.nodes_and_edges(),1);		// gdtedge weight for computing distances
		gdt::gdtnode_array<int>  distance (DAG.nodes_and_edges(),0);		// longest distances from s to every gdtnode
		gdt::gdtnode_array<gdtedge> pred_edge(DAG.nodes_and_edges(),NULL_EDGE); 	// entering gdtedge for each visited gdtnode

		gdtnode S = DAG.find_node(id(s));
		gdtnode T = DAG.find_node(id(t));

		DAG.find_longest_path (S,length,distance,pred_edge);

		v = T;
		while (v != S)
			{
			e = pred_edge[v];
			gdtedge e1 = find_edge(DAG.id(e));	// W.D. March 1999
			if (e1) L.push(e1);		
			v = DAG.start(e);
			}

		int flow_value = L.size();	// total flow value 

		// CHECKING
		// --------
		//cout << "\nDAG : " << flush; DAG.print(COMPLETE); cout << flush;
		//cout << "\nLONGEST PATH : " << flush;
		//forall (e,L) cout << id(e) << " " << flush;
		//cout << "\nFLOW VALUE = " << flow_value << flush;
		// --------

		// ---------------- 1 -----------------
		// BUILD THE FLOW NETWORK fn ASSOCIATED 
		//    TO THE PLANAR ST-DIGRAPH *this
		// ------------------------------------

		flow_dire_graph fn;			// flow network associated to *this
		gdt::gdtmap<face,gdtnode>  fn_node(NULL_NODE);		// gdtnode of fn corresponding to face of *this

		// ------- 1.1 -------
		// create all fn-nodes
		// -------------------
		//
		gdtnode sfn = fn.new_node (),		// source gdtnode of fn
		     tfn = fn.new_node ();		// sink   gdtnode of fn

		face s_face = right_face_moving_along_edge(e_st,s);
		face t_face = right_face_moving_along_edge(e_st,t);

		fn_node[s_face] = sfn;
		fn_node[t_face] = tfn;    

		fn.set_balance(sfn, flow_value);	// source-gdtnode supply flow_value
		fn.set_balance(tfn,-flow_value);	// sink-gdtnode demand flow_value

		forall_faces (f,*this)
			if (f != s_face && f != t_face)
				{
				vfn = fn.new_node ();
				fn_node[f] = vfn;
				fn.set_balance(vfn,0);
				}

		// ------- 1.2 -------
		// create all fn-edges
		// -------------------
		//
		forall_edges (e,*this) if (e != e_st)
			{
			v = start(e);
			face lf   = left_face_moving_along_edge (e,v),
			     rf   = right_face_moving_along_edge(e,v);
			gdtnode vlfn = fn_node[lf],
			     vrfn = fn_node[rf];

			efn = fn.new_edge (vlfn,vrfn,id(e));     

			int uc = INFINITE,
			    lc = 1,
			    co = cost[e],
			    fl = 0;

			fn.set_edge_info(efn,uc,lc,co,fl); 
			}     

		forall (e,L) 
			{
			efn = fn.corresponding_edge(e,*this);
			fn.set_upper_capacity(efn,1);
			}

		// CHECKING
		// --------
		//cout << "\nFLOW NETWORK : \n" << flush;
		//fn.print(COMPLETE); cout << flush;
		//forall_edges (efn,fn)
		//	{
		//	cout << "\nedge " << fn.id(efn) << "=(" << fn.id(fn.start(efn)) << "," << fn.id(fn.stop(efn)) << ")" << flush;
		//	cout << "\nuc = " << fn.get_upper_capacity(efn) << flush;
		//	cout << "\nlc = " << fn.get_lower_capacity(efn) << flush;
		//	cout << "\nfl = " << fn.get_flow(efn)	        << flush; 
		//	}
		// --------

		// ---------------- 2 -----------------
		//  FIND A MINIMUM COST FLOW IN fn AND
		//         ASSIGN EDGE LENGTHS
		// ------------------------------------
		
		if (!fn.min_cost_flow()) 
			gdt_error("no flow feasibility");

		forall_edges (efn,fn)
			{
			e = corresponding_edge(efn,fn);
			len[e] = fn.get_flow(efn);
			}
		len[e_st] = flow_value;
		}
	}



// ---------------------
//  consistency check
// ---------------------


// this to use gdt::gdtlist<face>::search(face)
// it might be inserted in rm3_plan_undi_graph.h
// DEFINE_DEFAULT_COMPARE(face);



#define inconsistency(message) \
          {\
          gdt_error(message); \
          return false;  /*this is never executed!*/\
          }


		bool
	plan_undi_graph::
internal_consistency_check() const // it assumes that the underlying undi_graph is consistent and
		   		   // check consistency of the face/border step structure.
	{
	gdt::gdtedge_array< gdt::gdtlist< face > > 	  edge2face_membership  (nodes_and_edges());

	gdt::list_item i;
	forall_items(i, (*f_list))
		{

		struct_face& f= *(f_list->inf(i)); 	// it is an ALIAS for the struct_face of the current gdt::list_item
		if ( &f != ((*face_with_id)[f.id]) )	inconsistency("face_with_id: wrong for this face id");
		if ( f.owner != this )			inconsistency("face: wrong 'owner'");
		if ( f.pos_in_f_list != i)		inconsistency("face: wrong 'pos_in_f_list'");

	//	gdtedge e;
	//	forall(e, f.edges)	edge2face_membership[e].push_back( &f);

		// *** check all border steps for the current face ***

		gdt::list_item j;
		forall_items(j, f.border_steps)
			{
			const struct_border_step& bs= *(f.border_steps.inf(j)); // it is an *ALIAS* for the struct_border_step of the current gdt::list_item
			if (f.edges.search(bs.owner_edge) == NULL)	inconsistency("border step: wrong 'owner_edge' does not belong to the 'owner_face'");
			if (bs.owner_face != &f)			inconsistency("border step: wrong 'owner_face'");
			if (bs.pos_in_border_steps != j)		inconsistency("border step: wrong 'pos_in_border_steps'");
			edge2face_membership[bs.owner_edge].append(&f);
			}
 		}

 	gdtedge e;
 	forall_edges(e, nodes_and_edges())
 		{
 		struct_plan_edge_info& 	ei  = ((*edge_info)[e]); 	// this is an ALIAS for the current struct_plan_edge_info
		border_step& 		bsl = ei.left_face_border_step;
		border_step& 		bsr = ei.right_face_border_step;

 		if ( ! edge2face_membership[e].search(ei.left_face) )		 inconsistency("gdtedge: wrong 'left_face' (1)");
 		if ( ! edge2face_membership[e].search(ei.right_face) )		 inconsistency("gdtedge: wrong 'right_face' (1)");
 		if ( bsl->owner_edge != e )		 			 inconsistency("gdtedge: wrong 'left_face_border_step'");
 		if ( bsr->owner_edge != e )					 inconsistency("gdtedge: wrong 'right_face_border_step'");
 		if ( e != ei.left_face->edges.inf(ei.pos_in_left_face_edges ))	 inconsistency("gdtedge: wrong pos_in_left_face_edges");
  		if ( e != ei.right_face->edges.inf(ei.pos_in_right_face_edges )) inconsistency("gdtedge: wrong pos_in_right_face_edges");
		if ( bsl->owner_face != ei.left_face )				 inconsistency("gdtedge: wrong 'left_face' (2)");
		if ( bsr->owner_face != ei.right_face )				 inconsistency("gdtedge: wrong 'right_face' (2)");
		if ( bsl->start_node != this->target(e) )			 inconsistency("border_step: start_node != target(e)");
		if ( bsr->start_node != this->source(e) ) 			 inconsistency("border_step: start_node != source(e)");
 		if ( edge2face_membership[e].size() != 2 )		 	 inconsistency("gdtedge: gdtedge must belong to two and only two faces");
		
		}
 	return true;
	};


int struct_border_step::counter=0;

struct_border_step::struct_border_step()
	{
	//cout << endl << "costruttore del border step " << this << endl;
	//counter++;
	//cout << endl << "counter=" << counter << endl;
	};

struct_border_step::~struct_border_step()
	{
	//cout << endl << "distruttore del border step " << this << endl;
	//counter--;
	//cout << endl << "counter=" << counter << endl;
	}
