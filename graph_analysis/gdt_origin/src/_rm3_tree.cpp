/*******************************************************************************
+
+  _rm3_tree.c
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

// Needed to compile with Microsoft Visual Studio .NET
//#ifdef WIN32
//#include "stdafx.h"
//#endif

#include <GDT/gdtqueue.h>
#include <GDT/gdtstack.h>
#include <GDT/rm3_undi_graph.h>
#include <GDT/rm3_tree.h>
#include <GDT/gdt_error.h>
#include <GDT/gdtlist.h>

//#define DEBUG

using namespace std;
//-----------------------------------------------------------------------------
// tree: base class for embedded trees
//
// W.Didimo e A.Leonforte (1996)
//-----------------------------------------------------------------------------

// ------------------------------------------
// PUBLIC
// ------------------------------------------

	tree::
tree
	()
	{
	gdtnode v;
	//cout << "\n tree () creator \n" << flush;
	v = undi_graph::new_node ();
	local_new ();
	local_init(v);
	}



	tree::
~tree
	()
	{
	local_del ();
	}



	tree::
tree
	(const undi_graph& ug, gdtnode v)
	{
	//cout << "\n tree (undi_graph) creator \n" << flush;
	if (!(ug.is_acyclic() && ug.is_connected()))
		gdt_error("undi_graph is not acyclic or connected");
	undi_graph::operator=(ug);
	local_new();
	local_init(find_node (ug.id(v)));
	}



	tree::
tree
	(const tree& tr)
	{
	//cout << "\n tree (tree) creator \n" << flush;
	local_new (); // required because operator = calls renew
	operator=(tr);
	}


	tree&
	tree::
operator =
	(const undi_graph& ug)
	{
	//cout << "\n undi_graph -(copy)-> tree \n" << flush;
	if (!(ug.is_acyclic() && ug.is_connected()))
		gdt_error("undi_graph is not acyclic or connected");
	undi_graph::operator=(ug);
	local_renew();
	local_init(find_node(ug.id(ug.first_node())));
	return *this;
	}



	tree&
	tree::
operator =
	(const tree& tr)
	{
	//cout << "\n tree -(copy)-> tree \n" << flush;
	gdtnode v, v1;
	gdtedge e, e1;
	// copy the undi_graph part
	//
	undi_graph::operator=(tr);
	local_renew();

	// copy the tree part
	//
	forall_nodes (v,tr)
		{
		v1 = find_node (tr.id(v));
		if (v != tr.root_of_tree())
			{
			e  = tr.father_edge (v);
			e1 = find_edge  (tr.id(e));
			}
		else
			{
			root = v1;
			e1 = NULL_EDGE;
			}
		set_father_edge (v1,e1);
		set_level (v1,tr.get_level(v));
		}
	return *this;
	}




	// -----------------
	// Access operations
	// -----------------

	void
	tree::
local_get
	(gdtnode p1, gdt::gdtnode_map<struct_tree_node_info>* p2)
	{
	p1 = root;
	p2 = node_info;
	}



	gdtnode
	tree::
root_of_tree
	() const
	{
	return root;
	}



	gdtnode
	tree::
father_node
	(gdtnode v) const
	{
	gdtnode w;
	if (v == root_of_tree()) w = NULL_NODE;
	else
		{
		gdtedge e = father_edge (v);
		w = opposite (v,e);
		}
	return w;
	}



	gdtnode
	tree::
father_node
	(gdtedge e) const
	{
	gdtnode v = source(e);
	if (father_edge (v) == e) v = target(e);
	return v;
	}



	gdtnode
	tree::
first_son_node
	(gdtnode v) const
	{
	gdtedge e;
	gdtnode w;
	e = first_son_edge (v);
	if (e != NULL_EDGE) w = opposite (v,e);
	else 		    w = NULL_NODE;

	return w;
	}



	gdtnode
	tree::
last_son_node
	(gdtnode v) const
	{
	gdtedge e;
	gdtnode w;
	e = last_son_edge (v);
	if (e != NULL_EDGE) w = opposite (v,e);
	else		    w = NULL_NODE;
	return w;
	}



	gdtnode
	tree::
son_succ
	(gdtnode w, gdtnode v) const
	{
	gdtnode u;
	gdtedge ew = father_edge (w);
	gdtedge e  = son_succ (ew,v);
	if (e != NULL_EDGE) u = opposite (v,e);
	else 		    u = NULL_NODE;

	return u;
	}



	gdtnode
	tree::
son_pred
	(gdtnode w, gdtnode v) const
	{
	gdtnode u;
	gdtedge ew = father_edge (w);
	gdtedge e  = son_pred (ew,v);
	if (e != NULL_EDGE) u = opposite (v,e);
	else		    u = NULL_NODE;
	return u;
	}



	gdtedge
	tree::
father_edge
	(gdtnode v) const
	{
	gdtedge e = (*node_info)[v].father;
	return e;
	}



	gdtedge
	tree::
father_edge
	(gdtedge e) const
	{
	gdtnode v = father_node (e);
	return father_edge (v);
	}



	gdtedge
	tree::
first_son_edge
	(gdtnode v) const
	{
	gdtedge e = father_edge (v);

	if (cyclic_adj_succ (e,v) != e) e = cyclic_adj_succ (e,v);
	else				e = NULL_EDGE;

	return e;
	}



	gdtedge
	tree::
last_son_edge
	(gdtnode v) const
	{
	gdtedge e = father_edge (v);
	if (cyclic_adj_pred (e,v) != e) e = cyclic_adj_pred (e,v);
	else				e = NULL_EDGE;
	return e;
	}



	gdtedge
	tree::
son_succ
	(gdtedge e, gdtnode v) const
	{
	gdtedge e1;
	if (e != last_son_edge (v)) e1 = cyclic_adj_succ (e,v);
	else			    e1 = NULL_EDGE;

	return e1;
	}



	gdtedge
	tree::
son_pred
	(gdtedge e, gdtnode v) const
	{
	gdtedge e1;
	if (e != first_son_edge (v)) e1 = cyclic_adj_pred (e,v);
	else			     e1 = NULL_EDGE;
	return e1;
	}



	int
	tree::
get_level
	(gdtnode v) const
	{
	return (*node_info)[v].level;
	}



	int
	tree::
number_of_sons
	(gdtnode v) const
	{
	int n = 0;
	gdtedge e = first_son_edge (v);
	while (e != NULL_EDGE)
		{
		e = son_succ (e,v);
		n ++;
		}
	return n;
	}



	int
	tree::
depth_of_subtree
	(gdtnode v , int i) const
	{
	gdt::gdtnode_array<int> lev (nodes_and_edges(), 0);
	gdt::gdtlist<gdtnode>	order;

	if (!v) v = root_of_tree();
	return BFS_of_subtree (lev,order,v,i);
	}



	int
	tree::
BFS_of_subtree
	(gdt::gdtnode_array<int>& lev, gdt::gdtlist<gdtnode>& order, gdtnode v, int i) const
	{
	gdt::gdtqueue<gdtnode> Q;
	gdtnode w;
	int depth = i;

	if (!v) v = root_of_tree();
	Q.append(v);
	lev[v] = i;

	while (!Q.empty())
		{
		v = Q.pop();
		order.append(v);
		//cout << "\nVISITED NODE " << id(v) << flush;
		//if (!father_edge(v)) cout << "\nWITH NULL FATHER EDGE\n" << flush;
		//else		       cout << "\nWITH NOT NULL FATHER EDGE\n " << flush;

		forall_son_nodes (w,v,*this)
			{
			Q.append(w);
			//cout << "\nAPPENDED NODE " << id(w);
			lev[w] = lev[v] + 1;
			if (depth < lev[w]) depth = lev[w];
			}
		}

	return depth;
	}



	int
	tree::
DFS_of_subtree
	(gdt::gdtnode_array<int>& lev, gdt::gdtlist<gdtnode>& order, gdtnode v, int i) const
	{
	gdt::gdtstack<gdtnode> S;
	gdtnode w;
	gdt::gdtnode_array<gdtnode> iterator (nodes_and_edges(), NULL_NODE);
	int depth = i;

	if (!v) v = root_of_tree();
	iterator[v] = first_son_node(v);
	S.push(v);
	lev[v] = i;
	order.append(v);
	//cout << "\n VISITED NODE " << id(v) << flush;

	while (!S.empty())
		{
		v = S.top();
		w = iterator[v];

		while (w != NULL_NODE)
			{
			iterator[w] = first_son_node(w);
			S.push(w);
			lev[w] = lev[v] + 1;
			if (depth < lev[w]) depth = lev[w];
			order.append (w);
			//cout << "\n VISITED NODE " << id(w) << flush;
			iterator[v] = son_succ (w,v);
			v = w;
			w = iterator[v];
			}
		S.pop();
		}

	return depth;
	}



	int
	tree::
post_order_of_subtree
	(gdt::gdtnode_array<int>& lev, gdt::gdtlist<gdtnode>& order, gdtnode v, int i) const
	{
		gdt::gdtstack<gdtnode> S;
	gdtnode w;
	gdt::gdtnode_array<gdtnode> iterator (nodes_and_edges(),NULL_NODE);
	int depth = i;

	if (!v) v = root_of_tree();
	iterator[v] = first_son_node (v);
	S.push(v);
	lev[v] = i;

	while (!S.empty())
		{
		v = S.top();
		w = iterator[v];

		while (w != NULL_NODE)
			{
			iterator[w] = first_son_node(w);
			S.push(w);
			lev[w] = lev[v] + 1;
			if (depth < lev[w]) depth = lev[w];
			iterator[v] = son_succ (w,v);
			v = w;
			w = iterator[v];
			}
		v = S.pop();
		order.append (v);
		//cout << "\nVISITED NODE " << id(v) << flush;
		}

	return depth;
	}




	// -----------------
	// Update opeartions
	// -----------------


	gdtnode
	tree::
new_son_of_node
	(gdtnode v, int new_node_id, int new_edge_id)
	{
	gdtnode w;
	gdtedge e, ew;

	w  = undi_graph::new_node (new_node_id);
	e  = last_son_edge (v);
	if (!e) ew = undi_graph::new_edge (v,w,new_edge_id);
	else	ew = undi_graph::new_edge (v,e,w,new_edge_id);
	set_father_edge (w,ew);
	set_level (w,get_level(v)+1);
	return w;
	}



	gdtnode
	tree::
new_son_of_node
	(gdtnode v, gdtnode u, int new_node_id, int new_edge_id, int dir)
	{
	gdtnode w;
	gdtedge e, ew;

	w  = undi_graph::new_node (new_node_id);
	e  = father_edge (u);
	ew = undi_graph::new_edge (v,e,w,new_edge_id,dir);
	set_father_edge (w,ew);
	set_level (w,get_level(v)+1);
	return w;
	}



	void
	tree::
make_root
	(gdtnode v)
	{
	//cout << "\ntree::make_root\n" << flush;
	gdtnode w;
	gdt::gdtnode_array<bool> reached (nodes_and_edges(), false);
	gdt::gdtnode_array<gdtedge> fath    (nodes_and_edges(), NULL_EDGE);
	gdt::gdtlist<gdtnode> order;

	forall_nodes (w,*this)
		{
		reached[w] = false;
		fath[w] = NULL_EDGE;
		}
	root = v;
	simple_DFS (v,reached,fath,order);                 //cout << "\nDFS TERMINATED\n" << flush;

	forall_nodes (w,*this)
		set_father_edge(w,fath[w]); //cout << "\nTREE ROOTED\n" << flush;
	set_levels_in_subtree ();
	}



	void
	tree::
del_subtree
	(gdtnode v)
	{
	gdt::gdtnode_array<int> lev (nodes_and_edges(), 0);
	gdt::gdtlist<gdtnode>	order;
	gdtnode w;

	BFS_of_subtree (lev,order,v);
	v = root_of_tree();
	forall (w,order)
		if (w != v) undi_graph::del_node(w);
	}



	void
	tree::
clear
	()
	{
	undi_graph::clear();
	local_renew ();
	}



	void
	tree::
steal_from
	(undi_graph& ug, gdtnode v)
	{
	//cout << "\n undi_graph& -(move)-> tree\n" << flush;
	if ((!ug.is_acyclic()) || (!ug.is_connected()))
		gdt_error("undi_graph is not acyclic or connected");
	clear ();
	//
	// Copy all the pointers of ug
	// that is, the ug part of this class
	//
	undi_graph::mirror (ug);
	//
	// Create a new empty local_environment for ug
	// in order to avoid side_effects with this
	//
	ug.forget ();
	//
	// Inizialize the extra part of this, according
	// to the tree part stolen from ug
	//
	local_renew ();
	make_root (v);
	}



	void
	tree::
mirror
	(tree& tr)
	{
	gdtnode p1 = NULL_NODE;
	gdt::gdtnode_map<struct_tree_node_info>* p2 = NULL;
	undi_graph::mirror (tr);
	tr.local_get (p1,p2);
	local_set (p1,p2);
	}



	void
	tree::
forget
	()
	{
	undi_graph::forget ();
	local_new ();
	}



	// -------------------------
	// Input / output operations
	// -------------------------

	void
	tree::
print_tree
	(ostream& os)
	{
	gdtnode v,v1,w;

	os << "\n";
	forall_nodes(v,nodes_and_edges())
		{
		w = father_node (v);
		if (w != NULL_NODE)
			{
			print(w);
			os << "-[";
			print(v);
			os << "]-";
			}
		else
			{
			os << "(R)-[";
			print(v);
			os << "]-";
			}
		forall_son_nodes (v1,v,*this)
			{
			print(v1);
			os << " ";
			}
		os << "\n" << flush;
		}
	}


/* --------------------------------------------------------------------
	void
	tree::
graphic_print_tree
	(window& W)
	{
	const int    node_width_scale_fact = 10;
	const double
		     scale = W.scale(),
		     xinit = 10,
		     yinit = 10;

	gdtnode v,w;
	gdtedge e;

	gdt::gdtnode_array<double> x		    (nodes_and_edges()),
			   y		    (nodes_and_edges());

	gdt::gdtnode_array<int>	   width_of_node    (nodes_and_edges(),1),
			   width_of_subtree (nodes_and_edges()),
			   lev 		    (nodes_and_edges());



	compute_tree_dimension     (width_of_subtree, lev, width_of_node);
	compute_node_coordinates   (x,y,width_of_subtree, lev);
	normalize_node_coordinates (x,y,scale,xinit,yinit);

	// draw tree
	//
	v = root_of_tree();
	W.clear();
	W.set_node_width (width_of_node[v]*node_width_scale_fact);

	forall_nodes (v,*this)
		W.draw_int_node (x[v],y[v],id(v),yellow);
	forall_edges (e,*this)
		{
		v = father_node (e);
		w = opposite (v,e);
		W.draw_edge (x[v],y[v],x[w],y[w],blue);
		}
	}

----------------------------------------------------------------*/


//----------------------------------------
// PROTECTED
//----------------------------------------

/*-----------------------------------------------------------
	void
	tree::
compute_tree_dimension
	(gdt::gdtnode_array<int>& width_of_subtree,
	 gdt::gdtnode_array<int>& lev, gdt::gdtnode_array<int> width_of_node)
	{
	gdt::gdtlist<gdtnode> order;
	gdtnode v;

	forall_nodes (v,*this)
	width_of_subtree[v] = 0;

	post_order_of_subtree (lev, order);
	forall (v,order)
		if (v != root_of_tree())
			{
			if (width_of_subtree[v] < width_of_node[v])
				width_of_subtree[v] = width_of_node[v];
			width_of_subtree[father_node(v)] += width_of_subtree[v];
			}
	}



	void
	tree::
compute_node_coordinates
	(gdt::gdtnode_array<double>& x, gdt::gdtnode_array<double>& y,
	 gdt::gdtnode_array<int> width_of_subtree, gdt::gdtnode_array<int>& lev)
	{
	gdt::gdtlist<gdtnode> order;
	gdtnode v,w;
	int depth;

	order.clear();
	depth = BFS_of_subtree (lev,order,root_of_tree());

	forall (v,order)
		{
		if (v == root_of_tree()) x[v] = (double)width_of_subtree[v]/2;

		forall_son_nodes (w,v,*this)
			{
			if (w == first_son_node(v)) x[w] = x[v] - (double)width_of_subtree[v]/2;
			else			    x[w] = x[son_pred(w,v)] + (double)width_of_subtree[son_pred(w,v)]/2 + (double)width_of_subtree[w]/2;
			}
		y[v] = depth - lev[v];
		}
	}



	void
	tree::
normalize_node_coordinates
	(gdt::gdtnode_array<double>& x, gdt::gdtnode_array<double>& y,
	 double scale, double xinit, double yinit)
	{
	gdtnode v;

	forall_nodes (v,*this)
		{
		x[v] = x[v]*scale + xinit;
		y[v] = y[v]*scale + yinit;
		}
	}

-----------------------------------------------*/


//----------------------------------------
// PRIVATE
//----------------------------------------


	void
	tree::
local_new
	()
	{
	//cout << "\n tree::local_new\n" << flush;
	node_info = new(gdt::gdtnode_map<struct_tree_node_info>);
	node_info -> init (nodes_and_edges());
	}



	void
	tree::
local_del
	()
	{
	//cout << "\n tree::local_del\n" << flush;
	safe_delete (node_info);
	}



	void
	tree::
local_renew
	()
	{
	//cout << "\n tree::local_new\n" << flush;
	local_del ();
	local_new ();
	}



	void
	tree::
local_init
	(gdtnode v)
	{
	//cout << "\n tree::local_init\n" << flush;
	if (!v) v = first_node();
	make_root(v);
	}



	void
	tree::
local_set
	(gdtnode p1, gdt::gdtnode_map<struct_tree_node_info>* p2)
	{
	root = p1;
	node_info = p2;
	}



	void
	tree::
set_father_edge
	(gdtnode v, gdtedge e)
	{
	(*node_info)[v].father = e;
	}



	void
	tree::
set_level
	(gdtnode v, int i)
	{
	(*node_info)[v].level = i;
	}



	void
	tree::
set_levels_in_subtree
	(gdtnode v, int i)
	{
	gdt::gdtnode_array<int> lev (nodes_and_edges(), 0);
	gdt::gdtlist<gdtnode>	order;
	gdtnode w;

	if (!v) v = root_of_tree();
	BFS_of_subtree (lev, order, v, i);
	forall (w,order) set_level (w,lev[w]);
	}



	void
	tree::
preorder_visit
	(
	gdtnode current,
	gdt::gdtlist<gdtnode>& visited
	)
	{
	//cout << "Visitato in preordine " << id(current) << endl;
	visited.append(current);
	gdtnode w;
	forall_son_nodes(w,current,*this)
		preorder_visit(w,visited);
	return;
	}



	void
	tree::
preprocessing_lca()
	{
	//Preorder traversal

	unsigned int l=(unsigned int)floor(log((double)number_of_nodes())/log(2.0));
	unsigned int current=0;
	gdtnode v,w;
	forall_nodes(v,*this)
		SIZE[v]=0;

	gdt::gdtlist<gdtnode> visited;
	visited.clear();



	gdt::gdtqueue<gdtnode> Q;
	gdtnode root=root_of_tree();
	if (root==NULL_NODE)
		{
		gdt_error("Preprocessing on empty tree");
		}

	preorder_visit(root,visited);
	forall(v,visited)
		PREORDER[v]=++current;

	/*
	Q.append(root);
	while (!Q.empty())
		{
		v=Q.pop();
		PREORDER[v]=++current;
		cout << "visited node " << id(v) << "\tindex=" << PREORDER[v] << endl;
		forall_son_nodes(w,v,*this)
			Q.append(w);
		}
	*/

	// Postorder traversal
	//cout << "---------------------" << endl;
	//cout << "POSTORDER TRAVERSAL" << endl;
	gdt::gdtstack<gdtnode> S;
	S.push(root);
	gdt::gdtlist<gdtnode> visited_nodes;
	visited_nodes.clear();
	while (!S.empty())
		{
		v=S.pop();
		//cout << "visited node " << id(v) << endl;
		visited_nodes.push_front(v);
		forall_son_nodes(w,v,*this)
			S.push(w);
		}
	forall(w,visited_nodes)
		{
		if (number_of_sons(w)==0)
			SIZE[w]=1;
		else
			{
			forall_son_nodes(v,w,*this)
			SIZE[w]+=SIZE[v];
			SIZE[w]++;
			}
		//cout << id(w) << "\tsize="<< SIZE[w] << endl;
		}
	forall_nodes(v,*this)
		{
		unsigned int i=(unsigned int)floor((log((double)((PREORDER[v]-1)^(PREORDER[v]+SIZE[v]-1))))/(log(2.0)));
		INLABEL[v]=(unsigned int)((unsigned int)(pow(2.0,(int)i))*floor((PREORDER[v]+SIZE[v]-1)/(pow(2.0,(int)i))));
		//cout << "node:" << id(v) << "\ti=" <<i << "\tinlabel=" << INLABEL[v]<< endl;
		}
	//cout << "ASCENDANT:" << endl;
	// Second preorder traversal

	//cout << "l=" << l << endl;

	Q.append(root);
	while(!Q.empty())
		{
		v=Q.pop();
		//cout << "Calcolo ascendant per il nodo " << id(v) << endl;

		if (v==root)
			ASCENDANT[root]=(unsigned int)pow(2.0,(int)l);
		else
			{
			gdtnode F_v=father_node(v);
			if (INLABEL[v]==INLABEL[F_v])
				ASCENDANT[v]=ASCENDANT[F_v];
			else
				{
				//cout << "INLABEL[v]=" << INLABEL[v] << endl;
				//cout << "risultato and=" << (INLABEL[v] & (INLABEL[v]-1)) << endl;
				//cout << log(INLABEL[v]-(INLABEL[v] & (INLABEL[v]-1)))/log(2) << endl;
				double i=(log((double)(INLABEL[v]-(INLABEL[v] & (INLABEL[v]-1))))/log(2.0));
				ASCENDANT[v]=ASCENDANT[F_v]+(unsigned int)pow(2,i);
				//cout << "i=" << i << "\tASCENDANT=" << ASCENDANT[v] << endl;
				}
			}
		forall_son_nodes(w,v,*this)
			Q.append(w);
		}
	//Compute HEAD
	forall_nodes(v,*this)
		{
		if (v!=root)
			{
			gdtnode f=father_node(v);
			if (INLABEL[v]!=INLABEL[f])
				HEAD[INLABEL[v]]=v;
			}
		}


	#ifdef DEBUG
	//test
	forall_nodes(v,*this)
		{
		cout << "node:" << id(v) << "\t\tPreor:" << PREORDER[v]  << "\tlevel"  << get_level(v) << "\tsize=" << SIZE[v];
		cout << "\tINLABEL=" << INLABEL[v] <<"\tascendant=" << ASCENDANT[v]<< endl;
		}
	#endif
	return;
	};


	gdtnode
	tree::
LCA
	(gdtnode n1, gdtnode n2)
	{
	if (INLABEL[n1]==INLABEL[n2])
		{
		if (get_level(n1) <= get_level(n2))
			return n1;
		else
			return n2;
		}
	else //INLABEL[n1]!=INLABEL[n2]
		{
		//step 1
		double i1=(log((double)(INLABEL[n1]-(INLABEL[n1] & (INLABEL[n1]-1))))/log(2.0));
		double i2=(log((double)(INLABEL[n2]-(INLABEL[n2] & (INLABEL[n2]-1))))/log(2.0));
		double i3=floor(log((double)(INLABEL[n1]^INLABEL[n2]))/log(2.0));
		double i=MAX(MAX(i1,i2),i3);

		unsigned int b;
		if (INLABEL[n1]>=INLABEL[n2])
			b=(unsigned int)(pow(2,i)*floor(INLABEL[n1]/pow(2,i)));
		else
			b=(unsigned int)(pow(2,i)*floor(INLABEL[n2]/pow(2,i)));
		//cout << endl<< "i=" << i << "\tb=" << b << endl;

		//step 2
		unsigned int COMMON=(ASCENDANT[n1] & ASCENDANT[n2]);
		//cout << "COMMON=" << COMMON << endl;
		unsigned int COMMON_i=(unsigned int)(pow(2.0,(int)i)*floor(COMMON/pow(2.0,(int)i)));
		//cout << "COMMON_i=" << COMMON_i << endl;
		double j=(log((double)(COMMON_i-(COMMON_i & (COMMON_i-1))))/log(2.0));
		unsigned int inlabel_z=((UINT_MAX-(unsigned int)pow(2.0,j+1)+1)&(INLABEL[n1]))+(unsigned int)(pow(2.0,j));
		#ifdef DEBUG
		cout << endl <<"j=" << j<< "\tinlabel_z=" << inlabel_z << endl;
		#endif
		//step 3
		gdtnode x,y;
		if (INLABEL[n1]==inlabel_z) x=n1;
		else
			{
			gdtnode w;
			double k;
			unsigned int inlabel_w;
			unsigned int temp=ASCENDANT[n1]%((unsigned int)pow(2,j));
			//unsigned int temp=ASCENDANT[n1]%(1<<(unsigned int)j);
			#ifdef DEBUG
			cout << "temp=" << temp << endl;
			#endif
			temp=(temp & (temp+1));
			if (temp==0) k=0;
			else k=floor((log((double)temp)/log(2.0)));
			inlabel_w=((UINT_MAX-(unsigned int)pow(2.0,k+1)+1)&(INLABEL[n1]))+(unsigned int)(pow(2.0,k));
			//inlabel_w=(unsigned int)(pow(2,k)*floor(INLABEL[n1]/pow(2,k)));
			w=HEAD[inlabel_w];
			#ifdef DEBUG
			cout << "inlabel_w=" << inlabel_w << endl;
			cout << "k="<< k <<"\tw=" << id(w) << endl;
			#endif
			if (w==NULL_NODE)
				gdt_error("Null node nel calcolo di x");
			x=father_node(w);
			}

		if (INLABEL[n2]==inlabel_z) y=n2;
		else
			{
			gdtnode w;
			double k;
			unsigned int inlabel_w;
			unsigned int temp=ASCENDANT[n2]%((unsigned int)pow(2,j));
			temp=(temp & (temp+1));
			if (temp==0) k=0;
			else k=floor((log((double)temp)/log(2.0)));
			inlabel_w=((UINT_MAX-(unsigned int)pow(2.0,k+1)+1)&(INLABEL[n2]))+(unsigned int)(pow(2.0,k));
			w=HEAD[inlabel_w];
			#ifdef DEBUG
			cout << "inlabel_w=" << inlabel_w << endl;
			cout << "k="<< k <<"\tw=" << id(w) << endl;
			#endif
			if (w==NULL_NODE)
				gdt_error("Null node nel calcolo di y");
			y=father_node(w);
			}
		#ifdef DEBUG
		cout << endl << "x=" << id(x) <<"\ty=" << id(y) << endl;
		#endif
		//step 4
		if (x==NULL_NODE || y==NULL_NODE)
			gdt_error("Null node in lca computation");
		if (get_level(x) <= get_level(y)) return x;
		else return y;
		}

	}



