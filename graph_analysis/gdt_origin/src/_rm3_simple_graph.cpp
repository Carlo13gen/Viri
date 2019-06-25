/*******************************************************************************
+
+  rm3_simple_graph.cpp
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


#include<GDT/rm3_simple_graph.h>


struct_edge::struct_edge(node n1, node n2, int id) 
	{
		index=id;
		_start=n1;
		_end=n2;
		position_in_edgelist=NULL;
		position_in_startnode=NULL;
		position_in_endnode=NULL;
	};


ostream& operator<<(ostream& oo, const struct_node* n)
{
	oo << n->get_index();
	return oo;
};


ostream& operator<<(ostream& oo, const struct_edge* e)
{
	oo << e->get_index() << " ("  << (e->start())  << "," << (e->end()) << ")";
	return oo;
};


ostream& operator<<(ostream& oo, const simple_graph& G)
{
	oo << "Nodes:" << G.number_of_nodes() << endl;
	node n;
	forall(n, G._nodes)
		cout << n << "\t";
	oo << endl;

	oo << "Edges:" << G.number_of_edges() << endl;
	edge e;
	forall(e, G._edges)
		cout << e << "\n";
	oo << endl;
	return oo;
};


simple_graph::simple_graph()
{
	_max_node_id=0;
	_max_edge_id=0;
	_nodes.clear();
	_edges.clear();
}


simple_graph::simple_graph(undi_graph& G)
{
	//cout << "Costruttore di simple graph" << endl;
	_max_node_id=0;
	_max_edge_id=0;
	_nodes.clear();
	_edges.clear();
	
	gdt::gdtmap<gdtnode,node> corr;
	gdtnode n;
	
	forall_nodes(n,G)
		corr[n]=new_node(G.id(n));
	gdtedge e;
	forall_edges(e,G)
	{
		gdtnode gn1,gn2;
		gn1=G.source(e);
		gn2=G.target(e);
		new_edge(corr[gn1],corr[gn2],G.id(e));
	}
	
}


simple_graph::~simple_graph()
{
	//cout << "Distruttore di simple_graph" << endl;
	node n;
	forall(n,_nodes)
		delete n;
	edge e;
	forall(e,_edges)
		delete e;
	_nodes.clear();
	_edges.clear();
}



node simple_graph::new_node(int index)
{
	if (index==-1) index=++_max_node_id;
	else
		if (index>_max_node_id) _max_node_id=index;
	
	node n=new struct_node(index);
	_nodes.append(n);
	n->position_in_nodelist=_nodes.last();
	return n;
}

edge simple_graph::new_edge(node n1, node n2, int index)
{
	if (index==-1) index=++_max_edge_id;
	else
		if (index>_max_edge_id) _max_edge_id=index;
	edge e=new struct_edge(n1,n2,index);
	_edges.append(e);
	e->position_in_edgelist=_edges.last();

	n1->incident_edges.append(e);
	n2->incident_edges.append(e);

	e->position_in_startnode=n1->incident_edges.last();
	e->position_in_endnode=n2->incident_edges.last();

	return e;
}

int simple_graph::number_of_nodes() const
{
	return _nodes.length();
}

int simple_graph::number_of_edges() const
{
	return _edges.length();
}

void simple_graph::del_edge(edge& e)
{	//cout << "Funzione del_edge" << endl;
	node n1=e->start();
	node n2=e->end();
	//cout << "n1=" << n1->index << "\tn2=" << n2->index << endl;
	n1->incident_edges.del(e->position_in_startnode);
	n2->incident_edges.del(e->position_in_endnode);
	//cout << "lunghezza di incident_edges1=" << n1->incident_edges.length() << endl;
	//cout << "lunghezza di incident_edges2=" << n2->incident_edges.length() << endl;
	_edges.del_item(e->position_in_edgelist);
	delete e;
	//cout <<"Fine della funzione del_edge" << endl;
}

void simple_graph::del_node(node& n)
{
	//cout << "Funzione del_node" << endl;
	//cout << "Cancellazione del nodo " << n << endl;
	if (n->degree()>0)
	{
		edge e;
		forall(e,n->incident_edges)
			del_edge(e);
	}
	_nodes.del(n->position_in_nodelist);
	//cout << "OK" << endl;
	delete n;
	//cout << "Fine della funzione del_node" << endl;
}

void simple_graph::contract(edge& e, node n1)
{
	node n2;
	//cout << endl << endl;
	//cout << "metodo contract" << endl;
	//cout <<"-------------------------------" << endl;
	//cout << *this << endl;
	//cout <<"-------------------------------" << endl;
	
	//cout << "Il primo nodo e' " << n1->index << endl;
	if (n1==e->start()) n2=e->end();
	else 
		if (n1!=e->end())
			gdt_error("error in edge contraction, wrong edge,node pair");
		else
			n2=e->start();

	//cout << "Il secondo nodo e' " << n2->index  << endl; 
	//cout << "numero di archi incidenti=" << n2->incident_edges.length() << endl;
	
	//check();
	//cout << "cancellazione di " << e << endl;
	del_edge(e);
	//check();
	
	edge temp_edge;
	
	//cout << "Il secondo nodo e' " << n2->index  << endl; 
	//cout << "numero di archi incidenti=" << n2->incident_edges.length() << endl;
	
	//if (!check())
	//	cout << "Errore nel grafo -2- !!!!!!!!!!!!!!!!!!!!!!!!" << endl;
	forall(temp_edge,n2->incident_edges)
	{
		//cout << "esamino l'arco " << temp_edge->index << endl;
		n1->incident_edges.append(temp_edge);
		if (temp_edge->start()==n2)
			{
			temp_edge->_start=n1;
			temp_edge->position_in_startnode=n1->incident_edges.last();
			}
		else
			{
			temp_edge->_end=n1;
			temp_edge->position_in_endnode=n1->incident_edges.last();
			}
		
	}
	
	_nodes.del(n2->position_in_nodelist);
	n2->incident_edges.clear();
	delete n2;
	//cout << "Fine del metodo contract" << endl;
	return;
}


bool simple_graph::is_empty()
{
	return (_nodes.length()==0);
	
}

bool simple_graph::check()
{
	cout << "Funzione check" << endl;
	node n;
	edge e,e1;
	cout <<"************************************" << endl;
	cout << "Nodes:" << endl;
	
	forall(n,_nodes)
	{
		cout << "Node " << n  << endl;
		forall(e,n->incident_edges)
		{
			cout << "edge " << e;
			bool found=false;
			forall(e1,_edges)
				if (e == e1) found =true;
			if (!found) 
			{
				
				cout << "error on node " << n->index << endl;
				cout << "edge " << e << " not found in edge list" << endl;
				gdt_error("");
			}
			else
				cout << "OK" << endl;
		}
					
	}
	
	forall(e,_edges)
	{
		node n1=e->start();
		node n2=e->end();
		bool found1=false;
		bool found2=false;
		forall(n,_nodes)
		{
			if(n == n1) found1=true;
			if(n == n2) found2=true;
		}
		if (!found1 || !found2)
		{
			cout << "error on edge " << e->index << endl;
			gdt_error("");
		}
	}
	cout <<"************************************" << endl;
	return true;
}
