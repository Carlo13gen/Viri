/*******************************************************************************
+  _rm3_layered_undi_graph.cpp
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



//#include <assert.h> //???
//#include <time.h> //???
#include <fstream> //???
#include <string> //???
#include <iostream> //???
//#include <list> //???

//#include<GDT/rm3_draw_undi_graph.h>
#include <GDT/rm3_undi_graph.h>
#include <GDT/rm3_global.h>
#include <GDT/gdt_error.h>
#include <GDT/rm3_layered_undi_graph.h>//???
//#include <GDT/gdtlist.h> ???
#include <limits.h>
#include <GDT/gdtarray.h>

using namespace std;

//-----------------------------------------------------------------------------
// layered_undi_graph
//-----------------------------------------------------------------------------



	gdt::gdtlist<gdtedge>
	layered_undi_graph::
remove_cycles
	()
	{
	//ci sono i two-cycle in un grafo???
	gdt::gdtlist<gdtnode> left;
	gdt::gdtlist<gdtnode> right;
	gdtnode temp_n, node_to_delete,copy_node;
	int id_node;
	bool sink_discovered;
	bool source_discovered;
	undi_graph ug_2(*this);
	int max;
	int outdeg,indeg;
	while(ug_2.number_of_nodes()!=0)//vorrei fare while !(is_empty)
		{
		sink_discovered=true;
		source_discovered=true;
		while((sink_discovered)&&(ug_2.number_of_nodes()!=0))//finche` ci sono pozzi
			{
			sink_discovered=false;
			forall_nodes(temp_n,ug_2)
				if(ug_2.is_target(temp_n))
					{
					sink_discovered=true;
					//cout<<"pozzo "<<ug_2.id(temp_n)<<endl;
					node_to_delete=temp_n;
					}//vorrei uscire da qui appena trova un pozzo???
			if(sink_discovered)
				{
				id_node=ug_2.id(node_to_delete);
				copy_node=find_node(id_node);
				right.push(copy_node);
				ug_2.del_node(node_to_delete);
				}
			}
		while((source_discovered)&&(ug_2.number_of_nodes()!=0))
			{
			source_discovered=false;
			forall_nodes(temp_n,ug_2)
				if(ug_2.is_source(temp_n))
					{
					source_discovered=true;
					//cout<<"sorgente "<<ug_2.id(temp_n)<<endl;
					node_to_delete=temp_n;
					}//vorrei uscire da qui appena trova una sorgente???
			if(source_discovered) 
				{
				id_node=ug_2.id(node_to_delete);
				copy_node=find_node(id_node);
				left.append(copy_node);
				ug_2.del_node(node_to_delete);
				}
			}
		if(ug_2.number_of_nodes()!=0)
			{
			max=INT_MIN;
			gdtnode current_n;
			forall_nodes(temp_n,ug_2)
				{
				outdeg = ug_2.degree_out(temp_n);
				indeg = ug_2.degree_in(temp_n);
				cout<<"Nodo: "<<ug_2.id(temp_n)<<"; outdeg-indeg: "<<outdeg-indeg<<endl;
				if((outdeg - indeg)>max)
					{
					max=outdeg - indeg;
					cout<<"Il massimo e`"<<max;
					current_n=temp_n;
					cout<<"; il nodo e` "<<ug_2.id(temp_n)<<endl;
					}
				}
				id_node=ug_2.id(current_n);
				copy_node=find_node(id_node);
				left.append(copy_node);
				ug_2.del_node(current_n);
				//string title_graph="my_graph.gdt";
				//ug_2.write(title_graph);
				/*forall_nodes(temp_n,ug_2)
					if(ug_2.is_target(temp_n))
					cout<<"pozzo: "<<ug_2.id(temp_n)<<endl;
					else cout<<"archi uscenti "<<ug_2.degree_out(temp_n)<<endl;*/
			}
		}
	//ug_2.write("empty.gdt");
	left.conc(right);
	//list_item it;
	gdtnode n,n1,n2;
	cout<<"Lista per ridurre gli incroci (left conc right):"<<endl;
	forall(n,left)
		//cout<<"lista: "<<ug.id(left.contents(it))<<endl;
		cout<<id(n)<<" ";
	cout<<endl;
	gdtedge e,e_temp;
	gdt::gdtlist<gdtedge> edge_to_reverse,reversed_edge;
	//gdt::gdtlist<gdtnode> right_tail;
	while(! left.empty())
		{
		n1=left.pop();
		forall(n2,left)
			{
			e=find_directed_edge(n2,n1);
			if(e != NULL_EDGE)
				edge_to_reverse.append(e);
			}
		}
	forall(e_temp,edge_to_reverse)
		{
		reverse(e_temp);
		reversed_edge.append(e_temp);
		}
	return reversed_edge;
	}


	void
	layered_undi_graph::
assign_level
	()
	{
	//int size=number_of_nodes();
	//gdtnode rows_array[size];
	//gdtnode columns_array[size];
	gdt::gdtlist<gdtnode> nodes_list;
	//create_array(rows_array);//crea un'array di tutti i nodi che saranno le righe della matrice
	//create_array(columns_array);//crea un'array di tutti i nodi che saranno le colonne della matrice
	//int matr[size][size];//crea la matrice degli archi uscenti vorrei farla a parte...
	gdtedge e;
	/*for(int i=0;i<size;i++)
		for(int j=0;j<size;j++)
			{
			e=find_directed_edge(rows_array[i],columns_array[j]);
			if(e != NULL_EDGE)
				matr[i][j]=1;
			else
				matr[i][j]=0;
			}
	for(int i=0;i<size;i++)//stampa di prova da togliere...
		{
		for(int j=0;j<size;j++)
			{
			cout<<matr[i][j]<<" ";
			}
		cout<<endl;
		}//...fino a qui!!!*/
	gdtnode temp_n;
	gdt::gdtlist<gdtnode> temp_list;
	/*forall_nodes(temp_n,*this)
		levels[temp_n]=0;//inizializza il livello di ogni nodo banalmente a 0
	forall_nodes(temp_n,*this)
		if(is_target(temp_n))
			levels[temp_n]=1;*/
	forall_nodes(temp_n,*this)
		if(is_target(temp_n))
			levels[temp_n]=1;
		else
			levels[temp_n]=0;
	
	bool trovato=true;
	int max_level=0;
	gdtnode temp_n1, temp_n2;
	while(trovato)
		{
		max_level++;
		trovato=false;
		forall_nodes(temp_n,*this)
			if(levels[temp_n]==max_level)
				temp_list.append(temp_n);
		forall(temp_n1,temp_list)
			forall_nodes(temp_n2,*this)
				{
				e=find_directed_edge(temp_n2,temp_n1);
				if(e != NULL_EDGE)
					{
					trovato=true;
					levels[temp_n2]=max_level+1;
					}
				}
		temp_list.clear();
		}
	}
	
	
	void
	layered_undi_graph::
assign_ascissa
	()
	{
	gdtnode temp_n;
	int height=number_of_levels();
	int ascissa;
	cout<<"Altezza (numero di livelli): "<<height<<endl;
	for(int i=1;i<=height;i++)
		{
		ascissa=0;
		forall_nodes(temp_n,*this)
			if(levels[temp_n]==i)
				{
				ascisse[temp_n]=ascissa;
				ascissa++;
				}
		}
	}
	
	
	void
	layered_undi_graph::
assign_PQ_tree
	()
	{
	gdt::PQ_tree<LEAVES_VALUE>* tree_punt;
	gdt::gdtlist<gdtnode> list_of_node;
	
	//gdtnode temp_node;
	for(int i=1;i<=number_of_levels();i++)
		{
		list_of_node=create_list_of_node_of_level(i);
		tree_punt=new gdt::PQ_tree<gdtnode>(list_of_node);
		map_level_PQ_tree[i]=tree_punt;
		/*forall(temp_pq,tree_punt->get_leaves())
			cout<<id(temp_pq->get_value())<<" ";
		cout<<endl;*/
		list_of_node.clear();
		}
	
	/*gdt::gdtlist<PQ_node> list;
	list=get_PQ_tree(1).get_leaves();
	forall(temp_pq,list)
		cout<<temp_pq->get_id()<<endl;*/
	}
	
	
	void
	layered_undi_graph::
restore_original_direction
	(
	gdt::gdtlist<gdtedge> reversed_edge
	)
	{
	gdtedge temp_e;
	forall(temp_e,reversed_edge)
		reverse(temp_e);
	}
	
	
	layered_undi_graph::
layered_undi_graph
	()
	{
	//cout<< "********Oggetto creato senza argomenti*********"<<endl;
	//levels=new (gdt::gdtnode_map<int>);
	levels=*(new(gdt::gdtnode_map<int>)(*this));
	ascisse=*(new(gdt::gdtnode_map<double>)(*this));
	is_dummy=*(new(gdt::gdtnode_map<bool>)(*this));
	}


	layered_undi_graph::
layered_undi_graph
	(
	undi_graph ug
	):undi_graph(ug)
	{
	//cout<< "********Oggetto creato a partire da ug*********"<<endl;
	//levels=new(gdt::gdtnode_map<int>);
	levels=*(new(gdt::gdtnode_map<int>)(*this));
	ascisse=*(new(gdt::gdtnode_map<double>)(*this));
	is_dummy=*(new(gdt::gdtnode_map<bool>)(*this,false));
	//vedi se nessun arco e` diretto
	gdt::gdtlist<gdtedge> reversed_edge;
	gdtedge temp_e;
	gdtnode temp_n;
	bool undirected=true;
	forall_edges(temp_e,*this)
	if(edge_is_directed(temp_e))
		undirected=false;
	if(undirected)
		cout<<"random_layered(ug)"<<endl;//indiretto fai un layered a caso
	else //costruisci un layered passo passo calcolato
		{
		if(! is_acyclic())//rimozone dei cicli secondo il GREEDY-CYCLE-REMOVAL
			reversed_edge=remove_cycles();//ritorna la lista degli archi invertiti
		cout <<"Lista degli archi invertiti: "<<endl;
		forall(temp_e,reversed_edge)
			cout<<id(temp_e)<<" ";
		cout<<endl;
		assign_level();//assegnazione del livello secondo il LONGEST PATH LAYERING
		forall_nodes(temp_n,*this)//stampa di controllo per i livelli, da togliere...
			{
			cout<<"Nodo: "<<id(temp_n)<<"; livello: "<<levels[temp_n]<<endl;
			}//...fino a qui!
		assign_ascissa();//assegnazione di un'ascissa random per ogni nodo
		forall_nodes(temp_n,*this)//stampa di controllo per le ascisse da togliere...
			{
			cout<<"Nodo: "<<id(temp_n)<<"; ascissa: "<<ascisse[temp_n]<<endl;
			}//...fino a qui!
		restore_original_direction(reversed_edge);
		}
	assign_PQ_tree();
	}
	
	
	layered_undi_graph::
layered_undi_graph
	(
	undi_graph& ug,
	gdt::gdtnode_map<int> in_levels
	):undi_graph(ug)
	{
	cout<< "oggetto creato a partire da ug e da levels"<<endl;
	gdtnode temp_n;
	levels=*(new(gdt::gdtnode_map<int>)(*this));
	ascisse=*(new(gdt::gdtnode_map<double>)(*this));
	is_dummy=*(new(gdt::gdtnode_map<bool>)(*this));
	forall_nodes(temp_n,*this)
		levels[temp_n]=in_levels[ug.find_node(id(temp_n))];
	assign_ascissa();//assegnazione di un'ascissa random per ogni nodo
	assign_PQ_tree();
	}
	
	
	layered_undi_graph::
layered_undi_graph
	(
	undi_graph& ug,
	gdt::gdtnode_map<double> in_ascisse
	):undi_graph(ug)
	{
	cout<< "oggetto creato a partire da ug e da ascisse"<<endl;
	gdtnode temp_n;
	gdt::gdtlist<gdtedge> reversed_edge;
	levels=*(new(gdt::gdtnode_map<int>)(*this));
	ascisse=*(new(gdt::gdtnode_map<double>)(*this));
	is_dummy=*(new(gdt::gdtnode_map<bool>)(*this));
	forall_nodes(temp_n,*this)
		ascisse[temp_n]=in_ascisse[ug.find_node(id(temp_n))];
	if(! is_acyclic())//rimozone dei cicli secondo il GREEDY-CYCLE-REMOVAL
		reversed_edge=remove_cycles();//ritorna la lista degli archi invertiti
	assign_level();//assegnazione del livello secondo il LONGEST PATH LAYERING
	restore_original_direction(reversed_edge);
	assign_PQ_tree();
	}
	
	
	layered_undi_graph::
layered_undi_graph
	(
	undi_graph& ug,
	gdt::gdtnode_map<int> in_levels,
	gdt::gdtnode_map<double> in_ascisse
	):undi_graph(ug)
	{
	cout<< "oggetto creato a partire da ug, da livelli e da ascisse"<<endl;
	gdtnode temp_n;
	levels=*(new(gdt::gdtnode_map<int>)(*this));
	ascisse=*(new(gdt::gdtnode_map<double>)(*this));
	is_dummy=*(new(gdt::gdtnode_map<bool>)(*this));
	forall_nodes(temp_n,*this)
		{
		levels[temp_n]=in_levels[ug.find_node(id(temp_n))];
		ascisse[temp_n]=in_ascisse[ug.find_node(id(temp_n))];
		}
	assign_PQ_tree();
	}
	
	
	layered_undi_graph::
~layered_undi_graph
	()
	{
	gdt::PQ_tree<LEAVES_VALUE> t;
	levels.clear();
	ascisse.clear();
	for(int i=1;i<=number_of_levels();i++)
		{
		delete(map_level_PQ_tree[i]);
		}
	map_level_PQ_tree.clear();
	is_dummy.clear();
	gdt::gdtlist<gdtnode> temp_list;
	forall(temp_list,list_of_path_of_dummy)
		temp_list.clear();
	list_of_path_of_dummy.clear();
	//cout<< "oggetto distrutto"<<endl;
	//local_del(); ???
	}
	
	
	int
	layered_undi_graph::
number_of_levels
	()
	{
	int number_levels=0;
	gdtnode temp_n;
	forall_nodes(temp_n,*this)
		if(levels[temp_n]>number_levels)
			number_levels=levels[temp_n];
	return number_levels;
	}
	
	
	void
	layered_undi_graph::
set_level
	(
	gdtnode n,
	int level
	)
	{
	levels[n]=level;
	}
	
	
	void
	layered_undi_graph::
set_level
	(
	int i, 
	int level
	)
	{
	levels[find_node(i)]=level;
	}
	
	
	void
	layered_undi_graph::
set_ascissa
	(
	gdtnode n,
	double ascissa
	)
	{
	ascisse[n]=ascissa;
	}
	
	
	void
	layered_undi_graph::
set_ascissa
	(
	int i,
	double ascissa
	)
	{
	ascisse[find_node(i)]=ascissa;
	}
	
	
	int
	layered_undi_graph::
get_level
	(
	gdtnode n
	)
	{
	return levels[n];
	}
	
	
	int
	layered_undi_graph::
get_level
	(
	int i
	)
	{
	return levels[find_node(i)];
	}
	
	
	double
	layered_undi_graph::
get_ascissa
	(
	gdtnode n
	)
	{
	return ascisse[n];
	}
	
	
	double
	layered_undi_graph::
get_ascissa
	(
	int i
	)
	{
	return ascisse[find_node(i)];
	}
	
	
	gdt::PQ_tree<gdtnode>*
	layered_undi_graph::
get_PQ_tree
	(
	int level
	)
	{
	/*gdt::PQ_tree<gdtnode> t;
	t=*(map_level_PQ_tree[level]);
	gdt::gdtlist<PQ_node> list_leaves;
	list_leaves=t.get_leaves();
	PQ_node temp_pq;
	forall(temp_pq,list_leaves)
		cout<<temp_pq->get_id()<<endl;*/
	return map_level_PQ_tree[level];
	}
	
	
	gdt::gdtlist<gdtnode>
	layered_undi_graph::
get_list_of_dummy
	()
	{
	gdtnode temp_node;
	gdt::gdtlist<gdtnode> dummy_list;
	forall_nodes(temp_node,*this)
		if(is_dummy[temp_node])
			dummy_list.append(temp_node);
	return dummy_list;
	}
	
	
	int
	layered_undi_graph::
length_of_edge
	(
	gdtedge e
	)
	{
	int length;
	length=abs(levels[source(e)] - levels[target(e)]);
	//cout<<"arco "<<id(e)<<" lunghezza "<<length<<endl;
	return length;
	}
	
	
	int
	layered_undi_graph::
length_of_edge
	(
	int i
	)
	{
	int length;
	length=abs(levels[source(find_edge(i))] - levels[target(find_edge(i))]);
	//cout <<"arco "<< i <<" lunghezza "<< length << endl;
	return length;
	}
	
	
	double
	layered_undi_graph::
width_of_level
	(
	int level
	)
	{
	gdtnode temp_n;
	double max=0;
	forall_nodes(temp_n,*this)
		if(levels[temp_n] == level)
			if(ascisse[temp_n]>max)
				max=ascisse[temp_n];
	return max;
	}
	
	
	int
	layered_undi_graph::
numbers_of_nodes_of_level
	(
	int level
	)
	{
	gdtnode temp_n;
	int number=0;
	forall_nodes(temp_n,*this)
		if(levels[temp_n] == level)
			number++;
	return number;
	}
	
	
	gdt::gdtlist<gdtnode>
	layered_undi_graph::
create_list_of_node_of_level
	(
	int level
	)
	{
	gdt::gdtlist<gdtnode> nodes_list;
	gdtnode temp_n;
	forall_nodes(temp_n,*this)
		{
		if(levels[temp_n]==level)
			nodes_list.append(temp_n);
		}
	return nodes_list;
	}
	
	
	void
	layered_undi_graph::
make_proper_layered
	()
	{
	gdtnode temp_node,prec_node;
	gdtedge temp_edge;
	gdt::gdtlist<gdtedge> edge_list;
	gdt::gdtlist<gdtnode> temp_list;
	forall_edges(temp_edge,*this)
		if(length_of_edge(temp_edge)!=1)
			{
			temp_list.append(source(temp_edge));
			temp_list.append(target(temp_edge));
			if(levels[source(temp_edge)]>levels[target(temp_edge)])
				{
				prec_node=source(temp_edge);
				for(int i=levels[prec_node]-1;i>levels[target(temp_edge)];i--)
					{
					temp_node=new_node();
					levels[temp_node]=i;
					//ascisse[temp_node]=width_of_level(i)+1;
					ascisse[temp_node]=0;
					is_dummy[temp_node]=true;
					map_level_PQ_tree[i]->add_leaf(temp_node);
					new_edge(prec_node,temp_node);
					temp_list.append(temp_node);
					prec_node=temp_node;
					}
				new_edge(prec_node,target(temp_edge));
				edge_list.append(temp_edge);
				list_of_path_of_dummy.append(temp_list);
				temp_list.clear();
				}
			else
				{
				prec_node=target(temp_edge);
				for(int i=levels[prec_node]+1;i<levels[target(temp_edge)];i++)
					{
					temp_node=new_node();
					levels[temp_node]=i;
					//ascisse[temp_node]=width_of_level(i)+1;
					ascisse[temp_node]=0;
					is_dummy[temp_node]=true;
					map_level_PQ_tree[i]->add_leaf(temp_node);
					new_edge(prec_node,temp_node);
					temp_list.append(temp_node);
					prec_node=temp_node;
					}
				new_edge(prec_node,source(temp_edge));
				edge_list.append(temp_edge);
				list_of_path_of_dummy.append(temp_list);
				temp_list.clear();
				}
			}
	forall(temp_edge,edge_list)
		del_edge(temp_edge);
	}
	
	
	int
	layered_undi_graph::
count_of_crossing_between_two_levels
	(
	int level1,
	int level2
	)
	{
	gdt::gdtlist<gdtnode> list_level1,list_level2;
	list_level1=sort_nodes_of_a_level(level1);
	list_level2=sort_nodes_of_a_level(level2);
	int row=0;
	int column=0;
	int list_size1=list_level1.size();
	int list_size2=list_level2.size();
	//gdt::gdtarray< gdt::gdtarray<int> > adjacent_matrix;
	//int adjacent_matrix[list_size1][list_size2];
	
	// Cambio l'allocazione della matrice per compilare con Visual Studio
	int** adjacent_matrix;
	adjacent_matrix=new int*[list_size1];
	int temp_index;
	for(temp_index=0; temp_index<list_size1; temp_index++)
		adjacent_matrix[temp_index]=new int[list_size2];
	//
	gdtnode temp_node1,temp_node2;
	gdtedge e;
	int crossing=0;
	//int count=0;
	//calcolo la matrice di adiacenza
	forall(temp_node1,list_level1)
		{
		forall(temp_node2,list_level2)
			{
			//count++;
			//cout<<count<<endl;
			e=find_edge(temp_node1,temp_node2);
			if(e==NULL_EDGE)
				adjacent_matrix[row][column]=0;
			else adjacent_matrix[row][column]=1;
			column++;
			}
		row++;
		column=0;
		}
	row=0;
	//conteggio degli incroci
	for(int i=0; i<list_size1-1; i++)
		for(int j=i+1; j<list_size1; j++)
			for(int k=0; k<list_size2-1; k++)
				for(int l=k+1; l<list_size2; l++)
					if((adjacent_matrix[j][k]==1) && (adjacent_matrix[i][l]==1))
						crossing++;
	return crossing;
	}
	
	
	int
	layered_undi_graph::
	count_of_crossing
	()
	{
	int levels=number_of_levels();
	int crossing=0;
	for(int i=1;i<levels;i++)
		crossing=crossing+count_of_crossing_between_two_levels(i,i+1);
	return crossing;
	}
	
	
	gdt::gdtlist<gdtnode>
	layered_undi_graph::
sort_nodes_of_a_level
	(
	int level
	)
	{
	gdt::gdtlist<gdtnode> nodes_list;
	gdtnode temp_node;
	nodes_list=create_list_of_node_of_level(level);
	int m,n;
	int size=nodes_list.length();
	for(m=1; m<size; m++)
		for(n=size-1;n>=m;n--)
			{
			if(ascisse[nodes_list.contents(nodes_list.get_item(n-1))]>ascisse[nodes_list.contents(nodes_list.get_item(n))])
				{
				temp_node=nodes_list.contents(nodes_list.get_item(n-1));
				nodes_list.assign(nodes_list.get_item(n-1),nodes_list.contents(nodes_list.get_item(n)));
				nodes_list.assign(nodes_list.get_item(n),temp_node);
				}
			}
	return nodes_list;
	}
	
	
	double
	layered_undi_graph::
find_barycenter
	(
	gdtnode node
	)
	{
	gdt::gdtlist<gdtnode> adjacent_nodes,adjacent_list;
	gdtnode temp_node;
	double sum_ascisse,barycenter;
	sum_ascisse=0;
	adjacent_nodes=adj_nodes(node);
	forall(temp_node,adjacent_nodes)
		if(levels[temp_node]==(levels[node]-1))
			adjacent_list.append(temp_node);
	forall(temp_node,adjacent_list)
		sum_ascisse=sum_ascisse+ascisse[temp_node];
	if(sum_ascisse==0)
		barycenter=0;
	else
		barycenter=sum_ascisse/adjacent_list.size();
	return barycenter;
	}
	
	
	void
	layered_undi_graph::
set_ascisse_on_barycenter_of_a_level
	(
	int level
	)
	{
	
	//int size1,size2;
	gdt::gdtlist<gdtnode> list_level;
	gdtnode temp_n;
	//list_level1=sort_nodes_of_a_level(level1);
	list_level=create_list_of_node_of_level(level);
	//size1=list_level1.size();
	//size2=list_level2.size();
	//double sum_ascisse,barycenter;
	forall(temp_n,list_level)
		ascisse[temp_n]=find_barycenter(temp_n);
	}
	
	
	void
	layered_undi_graph::
set_ascisse_on_barycenter
	()
	{
	int levels=number_of_levels();
	for(int i=2;i<=levels;i++)
		set_ascisse_on_barycenter_of_a_level(i);
	}
	
	
	void
	layered_undi_graph::
reduce_PQ_trees_on_barycenter
	()
	{
	gdt::gdtlist<gdtnode> temp_list,S;
	gdt::gdtlist< gdt::gdtlist<gdtnode> > list_of_all_S;
	gdtnode temp_n;
	gdt::PQ_tree<LEAVES_VALUE> t;
	int levels=number_of_levels();
	for(int i=1;i<=levels;i++)
		{
		temp_list=sort_nodes_of_a_level(i);
		for(int j=0;j<temp_list.size()-1;j++)
			{
			S.clear();
			S.append(temp_list.contents(temp_list.get_item(j)));
			S.append(temp_list.contents(temp_list.get_item(j+1)));
			list_of_all_S.append(S);
			}
		t=*(map_level_PQ_tree[i]);
		forall(S,list_of_all_S)
			{
			forall(temp_n,S)
				cout<<id(temp_n)<<" ";
			cout<<endl;
			}
		forall(S,list_of_all_S)
			{
			t.bubble(S);
			t.reduce(S);
			}
		list_of_all_S.clear();
		cout<<endl;
		}
	}
	
	
	
