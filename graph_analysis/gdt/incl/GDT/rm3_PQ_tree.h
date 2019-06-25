/*******************************************************************************
+
+  rm3_PQ_tree.h
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
/*! \file */


#include <string>
#include <GDT/gdtlist.h>
#include <GDT/gdtmap.h>
#include <GDT/gdtqueue.h>
#include <GDT/gdt_error.h>
#include <GDT/rm3_undi_graph.h>
#include<GDT/rm3_draw_undi_graph.h>
#include <GDT/stopwatch.h>

#ifndef RM3_PQ_TREE_H
#define RM3_PQ_TREE_H


namespace gdt {

template <class T> class PQ_tree;
template <class T> class PQ_node_struct_freezed;
template <class T> class PQ_tree_freezed;
 
template <class T> class

/**
Global class PQ_node_struct. <br>
A PQ_node_struct object represents the internal structure of a node of a PQ-tree.

*/

PQ_node_struct
	{
	
	friend class PQ_tree<T>;
	typedef PQ_node_struct<T>* PQ_node;
	
	public:
		int id; //id del nodo
		int child_count; // number of children possessed by the node (only for P-node)
		//gdt::gdtlist<PQ_node> circular_link; // doubly-linked circular list of chidren of a P-node ??
		PQ_node link_left_side, link_right_side; //two link to two siblings ??
		PQ_node left_endmost, right_endmost; // two endmost children (only for Q_nodes)
		gdtlist<PQ_node> full_children; // list containing all of full children
		//gdt::gdtlist<PQ_node> immediate_siblings; //list containig 0 (children of a P_node),1 (endmost children of a Q-node) or 2 (interior children of a Q_node) immediate siblings non c'è più
		int label; // "empty", "full" or "partial"
		int mark;  // initially "unmarked", during bubbling up "queued", during processing "blocked" or "unblocked"
		PQ_node parent; //immediate ancestor
		gdtlist<PQ_node> partial_children; // list containing all of partial children
		int pertinent_child_count; // number of pertinent children
		int pertinent_leaf_count; // number of pertinent leaves which are descendants
		int type;  // "leaf", "P-node" or "Q-node"
		T value;
		
		
	//public:
			/**
			Empty constructor.
			*/
		PQ_node_struct
			(
			);
			
			
			
			/**
			Destructor. It deallocates the memory required by the PQ_node_struct object.
			*/
		~PQ_node_struct
			(
			);
			
			
			
			/**
			Sets the id of a node, input: id.
			*/
			void
		set_id
			(
			int id
			);
			
			
			
			/**
			Sets the type of a node, input: type.
			*/
			void
		set_type
			(
			int type
			);
			
			
			
			/**
			Sets the value of a node, input: value.
			*/
			void
		set_value
			(
			T value
			);
			
			
			
			/**
			Sets the number of children of a node, input: number.
			*/
			void
		set_child_count
			(
			int child_count
			);
			
			
			
			/**
			Sets parent of a node, input:parent.
			*/
			void
		set_parent
			(
			PQ_node parent
			);
			
			
			/**
			Returns the parent of a node.
			*/
			PQ_node
		get_parent
			(
			);
			
			
			
			/**
			Returns the id.
			*/
			int
		get_id
			(
			);
			
			
			
			/**
			Returns the type.
			*/
			int
		get_type
			(
			);
			
			
			
			/**
			Returns the value.
			*/
			T
		get_value
			(
			);
			
			
			
			/**
			Resets the values of a node in the constructor and after bubble.
			*/
			void
		reset_node
			(
			);
			
			
			
			/**
			Returns true if a child of a Q_node is endmost.
			*/
			bool
		is_endmost
			(
			);
			
			
			
			/**
			Removes the node from the doubly-linked circular list.
			*/
			void
		remove_from_circular_link
			(
			);
			
	};



/**
Global class PQ_tree. <br>
A PQ_tree represents all the admissible permutations of a set of objects. A permutation is admissible if it respects 
a set of constraints.

*/

template <class T> class
PQ_tree
	{
	
	
	typedef PQ_node_struct<T>* PQ_node;
	typedef PQ_node_struct_freezed<T>* PQ_node_freezed;
	
	private:
		gdtlist<PQ_node> leaves; //list of the leaves
		int max_id; //max id of the nodes
		int block_count; //number of blocks of blocked nodes
		int blocked_nodes; //number of the blocked nodes
		bool off_the_top; //if the root has been processed
		gdtqueue<PQ_node> queue;  //queue for sequencing the order in which nodes are processed
		PQ_node root; //radice
		int number_of_nodes; //the number of nodes of tree
		gdtqueue<PQ_node> full_queue; //queue for full nodes
		gdtlist<PQ_node> blocked_list; //list of blocked nodes qui per disegnare grafi da mettere in bubble
		gdtmap<T,PQ_node> map_leaves;
		
		
		
	protected:
			int
		generate_id
			(
			);
			
			
	public:
			/**
			Empty constructor
			*/
		PQ_tree
			(
			);
			
	
			/**
			Constructor.
			*/
		PQ_tree
			(
			gdtlist<T> &input_leaves
			);
			
			
			
			/**
			Destructor. It deallocates the memory required by the PQ_tree object.
			*/
		~PQ_tree
			(
			);
			
			
			
			/**
			Copy assignment operator. It clears the topology of the current
			PQ_tree class object, and makes it as a copy of tree "t".
			*/
			PQ_tree<T>& 
		operator=
			(
			const PQ_tree<T>& t
			);
			
			
			
			/**
			Returns the root
			*/
			PQ_node
		get_root
			(
			);
			
			
			
			/**
			Returns the max_id
			*/
			int
		get_max_id
			(
			);
			
			
			
			/**
			Returns the number of nodes
			*/
			int
		get_number_of_nodes
			(
			);
			
			
			/**
			Sets the list of leaves
			*/
			void
		set_leaves
			(
			gdt::gdtlist<gdt::PQ_node_struct<T>*> list_leaves
			);
			
			
			
			/**
			Returns the list of leaves
			*/
			gdtlist<PQ_node>
		get_leaves
			(
			);
			
			
			
			/**
			Returns the leaf with the id as input
			*/
			PQ_node
		find_leaf
			(
			T value
			);
			
			
			
			/**
			Resets the values of a tree in the constructor and after bubble.
			*/
			void
		reset_tree
			(
			);
			
			
			
			/**
			Returns true if the node is the root.
			*/
			bool
		is_root
			(
			PQ_node node
			);
			
			
			
			/**
			Sorts the children full and empty of a PNODE.
			*/
			bool
		pnode_sorting
			(
			PQ_node node
			);
			
			
			
			/**
			Reverses a QNODE.
			*/
			bool
		qnode_reversing
			(
			PQ_node node
			);
			
			
			
			/**
			Assignes the true parent to a pseudonode child.
			*/
			bool
		assign_parent_to_draw_graph
			(
			PQ_node node,
			int count
			);
			
			
			
			/**
			Adds a given leaf to the tree
			*/
			void
		add_leaf
			(
			T value
			);
			
			
			
			/**
			First step for reduction of a PQ_tree.
			Returns false if a reducution does not exist.
			*/
			bool
		bubble
			(
			gdtlist<T> &S
			);
			
			
			
			/**
			A template in the reduce pass.
			Returns false if the template does not exist.
			*/
			bool
		template_L1
			(
			PQ_node node
			);
			
			
			
			/**
			A template in the reduce pass.
			Returns false if the template does not exist.
			*/
			bool
		template_P1
			(
			PQ_node node
			);
			
			
			
			/**
			A template in the reduce pass.
			Returns false if the template doesn't exist.
			*/
			bool
		template_P2
			(
			PQ_node node
			);
			
			
			
			/**
			A template in the reduce pass.
			Returns false if the template does not exist.
			*/
			bool
		template_P3
			(
			PQ_node node
			);
			
			
			
			/**
			A template in the reduce pass.
			Returns false if the template does not exist.
			*/
			bool
		template_P4
			(
			PQ_node node
			);
			
			
			
			/**
			A template in the reduce pass.
			Returns false if the template does not exist.			*/
			bool
		template_P5
			(
			PQ_node node
			);
			
			
			
			/**
			A template in the reduce pass.
			Returns false if the template does not exist.
			*/
			bool
		template_P6
			(
			PQ_node node
			);
			
			
			
			/**
			A template in the reduce pass.
			Returns false if the template does not exist.
			*/
			bool
		template_Q1
			(
			PQ_node node
			);
			
			
			
			/**
			A template in the reduce pass.
			Returns false if the template does not exist.			*/
			bool
		template_Q2
			(
			PQ_node node
			);
			
			
			
			/**
			A template in the reduce pass.
			Returns false if the template does not exist.			*/
			bool
		template_Q3
			(
			PQ_node node
			);
			
			
			
			/**
			Makes empty a pq_tree.
			Returns false if it fails.
			*/
			bool
		make_empty
			(
			);
			
			
			
			/**
			Second step for reduction of a PQ_tree.
			Returns false if th reduction does not exist.
			*/
			bool
		reduce
			(
			gdtlist<T> &S
			);
			
			
			
			/**
			It "freezes" a PQ_tree into a PQ_tree_freezed.
			*/
			void
		freeze
			(
			PQ_tree_freezed<T> &freezed_tree
			);
			
			
			
			/**
			Input: PQ_tree.
			Returns the corresponding graph.
			*/
			undi_graph
		PQ_tree_into_undi_graph
			(
			std::string title
			);
			
	};
	
	
	
template <class T> class PQ_tree_freezed;
	
template <class T> class
PQ_node_struct_freezed
	{
	
	friend class PQ_tree<T>;
	friend class PQ_tree_freezed<T>;
	typedef PQ_node_struct_freezed<T>* PQ_node_freezed;
	
	private:
		int id; //id del nodo
		gdt::gdtlist<PQ_node_freezed> children_list; // list of the chidren
		int type;  // "leaf", "P-node" or "Q-node"
		T value;
		
		
		
	public:
		/**
		Empty constructor.
		*/
		PQ_node_struct_freezed
			(
			);
			
			
			
		/**
		Destructor. It deallocates the memory required by the PQ_node_struct object.
		*/
		~PQ_node_struct_freezed
			(
			);
			
			
			
		/**
		Returns the node id.
		*/
			int
		get_id
			(
			);
			
			
			
		/**
		Returns the node value.
		*/
			T
		get_value
			(
			);
	
	};
	
	
	
template <class T> class
PQ_tree_freezed
	{
	
	
	friend class PQ_tree<T>;
	typedef PQ_node_struct<T>* PQ_node;
	typedef PQ_node_struct_freezed<T>* PQ_node_freezed;
	
	private:
		PQ_node_freezed root; //radice
		int number_of_nodes; //the number of nodes of tree
		gdtlist<PQ_node_freezed> frontier; //tree frontier
		
		
		
	/*protected:
			int
		generate_id
			(
			);*/
			
			
	public:
	
			/**
			Empty constructor.
			*/
		PQ_tree_freezed
			(
			);
			
			
			
			/**
			Destructor. It deallocates the memory required by the PQ_tree_freezed object.
			*/
		~PQ_tree_freezed
			(
			);
			
			
			
			/**
			Visits the freezed tree.
			*/
			gdtlist<T>
		tree_search
			(
			);
			
			
			
			/**
			Builds the frontier.
			*/
			void
		sorted_leaves_list
			(
			gdtlist<PQ_node_struct_freezed<T>*>
			);
			
			
			
			/**
			Visits the freezed tree.
			*/
			void
		tree_frontier
			();
			
			
			
			/**
			Returns the frontier.
			*/
			gdtlist<PQ_node_struct_freezed<T>*>
		get_frontier
			();
	};

};



enum
	{
	NON_VALID=0,
	LEAF=1,
	PNODE=2,
	QNODE=3,
	PSEUDONODE=4,
	
	EMPTY=5,
	PARTIAL=6,
	FULL=7,
	
	UNMARKED=8,
	QUEUED=9,
	MARKED=10,
	BLOCKED=11,
	UNBLOCKED=12
	};
	
	
	

	template<class T>
	gdt::PQ_node_struct<T>::
PQ_node_struct
	()
	{
	id=0;
	child_count=0;
	link_left_side=NULL;
	link_right_side=NULL;
	left_endmost=NULL;
	right_endmost=NULL;
	parent=NULL;
	type=NON_VALID;
	reset_node();
	}
	
	
	
	template<class T>
	gdt::PQ_node_struct<T>::
~PQ_node_struct
	()
	{
	full_children.clear();
	partial_children.clear();
	}
	
	
	
	template<class T>
	void
	gdt::PQ_node_struct<T>::
set_id
	(
	int id
	)
	{
	this->id=id;
	}
	
	
	
	template<class T>
	void
	gdt::PQ_node_struct<T>::
set_child_count
	(
	int child_count
	)
	{
	this->child_count=child_count;
	}
	
	
	template<class T>
	void
	gdt::PQ_node_struct<T>::
set_parent
	(
	PQ_node parent
	)
	{
	this->parent=parent;
	}
	
	
	template<class T>
	gdt::PQ_node_struct<T>*
	gdt::PQ_node_struct<T>::
get_parent
	(
	)
	{
	return parent;
	}
	
	
	template<class T>	
	void
	gdt::PQ_node_struct<T>::
set_type
	(
	int type
	)
	{
	this->type=type;
	}
	
	
	template<class T>	
	void
	gdt::PQ_node_struct<T>::
set_value
	(
	T value
	)
	{
	this->value=value;
	}
	
	
	template<class T>
	int
	gdt::PQ_node_struct<T>::
get_id
	()
	{
	return id;
	}
	
	
	template<class T>
	int
	gdt::PQ_node_struct<T>::
get_type
	()
	{
	return type;
	}
	
	
	
	template<class T>
	T
	gdt::PQ_node_struct<T>::
get_value
	()
	{
	return value;
	}
	
	
	template<class T>
	void
	gdt::PQ_node_struct<T>::
reset_node
	()
	{
	label=EMPTY;
	mark=UNMARKED;
	pertinent_child_count=0;
	pertinent_leaf_count=0;
	full_children.clear();
	partial_children.clear();
	}
	
	
	template<class T>
	bool
	gdt::PQ_node_struct<T>::
is_endmost
	()
	{
	return((this->link_left_side==NULL)||(this->link_right_side==NULL));
	}
	
	
	template<class T>
	void
	gdt::PQ_node_struct<T>::
remove_from_circular_link
	()
	{
	this->link_right_side->link_left_side=this->link_left_side;
	this->link_left_side->link_right_side=this->link_right_side;
	}
	
	
	
	template <class T>
	gdt::PQ_tree<T>::
PQ_tree
	()
	{
	root=NULL;
	number_of_nodes=0;
	max_id=-1;
	block_count=0;
	blocked_nodes=0;
	off_the_top=false;
	}
	
	
	
	template <class T>
	gdt::PQ_tree<T>::
PQ_tree
	(
	gdt::gdtlist<T> &input_leaves
	)
	{
	T temp_i;
	PQ_node temp_n,p_base;
	max_id=-1;
	forall(temp_i,input_leaves)
		{
		temp_n=new(PQ_node_struct<T>);
		temp_n->id=generate_id(); //temp_n->set_id(generate_id());
		temp_n->type=LEAF; //temp_n->set_type(LEAF);
		temp_n->value=temp_i;
		leaves.append(temp_n);
		map_leaves[temp_i]=temp_n;
		//if(temp_i>max_id)
		//	max_id=temp_i;
		
		}
	p_base=new(PQ_node_struct<T>);
	p_base->id=generate_id(); //p_base->set_id(generate_id());
	//max_id=p_base->id; //max_id=p_base->get_id();
	p_base->type=PNODE; //p_base->set_type(PNODE);
	p_base->child_count=leaves.size(); //p_base->set_child_count(leaves.size());
	//p_base->circular_link=leaves; //p_base->set_circular_link(leaves);
	root=p_base;
	forall(temp_n,leaves)
		{
		temp_n->parent=p_base; //temp_n->set_parent(p_base);
		//temp_n->link_left_side=leaves.contents(leaves.cyclic_pred(temp_n));//segmentation fault
		//temp_n->link_right_side=leaves.contents(leaves.cyclic_succ(temp_n));
		}
	for(int i=0;i<leaves.size();i++)
		{
		leaves.contents(leaves.get_item(i))->link_left_side=leaves.contents(leaves.cyclic_pred(leaves.get_item(i)));
		leaves.contents(leaves.get_item(i)) ->link_right_side=leaves.contents(leaves.cyclic_succ(leaves.get_item(i)));
		}
	number_of_nodes=leaves.size()+1;
	reset_tree();
	}
	
	
	
	template <class T>
	gdt::PQ_tree<T>::
~PQ_tree
	()
	{
	make_empty();
	if(root!=NULL)
		{
		delete(root);
		}
	//cout<<"OK"<<endl;
	}
	
	
	
	template <class T>
	gdt::PQ_tree<T>&
	gdt::PQ_tree<T>::
operator=
	(
	const gdt::PQ_tree<T>& t
	)
	{
	PQ_node temp_node,temp_pq,temp_child,current_node;
	gdt::gdtmap<PQ_node,PQ_node> map_PQ;
	gdt::gdtmap<PQ_node,int> node_visited_1(0);
	gdt::gdtmap<PQ_node,int> node_visited_2(0);
	/*forall(temp_node,t.leaves)
		cout<<temp_node->id<<endl;*/
	//this->leaves.clear();
	//this->map_leaves.clear();
	this->make_empty();
	forall(temp_node,t.leaves)
		{
		temp_pq=temp_node;
		while((temp_pq!=NULL)&&(node_visited_1[temp_pq]==0))
			{
			current_node=new(PQ_node_struct<T>);
			current_node->id=temp_pq->id;
			current_node->type=temp_pq->type;
			current_node->value=temp_pq->value;
			current_node->child_count=temp_pq->child_count;
			map_PQ[temp_pq]=current_node;
			node_visited_1[temp_pq]=1;
			temp_pq=temp_pq->parent;
			}
		}
	forall(temp_node,t.leaves)
		{
		temp_pq=temp_node;
		while((temp_pq!=NULL)&&(node_visited_2[temp_pq]==0))
			{
			current_node=map_PQ[temp_pq];
			forall(temp_child,temp_pq->full_children)
				current_node->full_children.append(map_PQ[temp_child]);
			forall(temp_child,temp_pq->partial_children)
				current_node->partial_children.append(map_PQ[temp_child]);
			current_node->parent=map_PQ[temp_pq->parent];
			current_node->link_left_side=map_PQ[temp_pq->link_left_side];
			current_node->link_right_side=map_PQ[temp_pq->link_right_side];
			if(current_node->type==LEAF)
				{
				this->leaves.append(current_node);
				map_leaves[current_node->value]=current_node;
				}
			else
				{
				if(temp_pq->left_endmost!=NULL)
					current_node->left_endmost=map_PQ[temp_pq->left_endmost];
				if(temp_pq->right_endmost!=NULL)
					current_node->right_endmost=map_PQ[temp_pq->right_endmost];
				}
			node_visited_2[temp_pq]=1;
			temp_pq=temp_pq->parent;
			}
		}
	this->max_id=t.max_id;
	this->root=map_PQ[t.root];
	this->root->parent=NULL;
	this->number_of_nodes=t.number_of_nodes;
	this->reset_tree();
	return *this;
	}
	
	
	template<class T>
	int
	gdt::PQ_tree<T>::
generate_id
	()
	{
	return ++max_id;
	}
	
	
	template<class T>
	void
	gdt::PQ_tree<T>::
reset_tree
	()
	{
	block_count=0;
	blocked_nodes=0;
	off_the_top=false;
	queue.clear();
	}
	
	
	template<class T>
	gdt::PQ_node_struct<T>*
	gdt::PQ_tree<T>::
get_root
	()
	{
	return root;
	}
	
	
	template<class T>
	int
	gdt::PQ_tree<T>::
get_max_id
	()
	{
	return max_id;
	}
	
	
	template<class T>
	int
	gdt::PQ_tree<T>::
get_number_of_nodes
	()
	{
	return number_of_nodes;
	}
	
	
	
	template<class T>
	void
	gdt::PQ_tree<T>::
set_leaves
	(
	gdt::gdtlist<gdt::PQ_node_struct<T>*> list_leaves
	)
	{
	leaves=list_leaves;
	}
	
	
	template<class T>
	gdt::gdtlist<gdt::PQ_node_struct<T>*>
	gdt::PQ_tree<T>::
get_leaves
	()
	{
	return leaves;
	}
	
	
	template<class T>
	gdt::PQ_node_struct<T>*
	gdt::PQ_tree<T>::
find_leaf
	(
	T value
	)
	{
	return map_leaves[value];
	}
	
	
	template<class T>
	bool
	gdt::PQ_tree<T>::
is_root
	(
	PQ_node node
	)
	{
	return (root==node);
	}
	
	
	template<class T>
	bool
	gdt::PQ_tree<T>::
pnode_sorting
	(
	PQ_node node
	)
	{
	PQ_node prec_node,temp_node,partial_node;
	if(node->type!=PNODE)
		return false;
	prec_node=node->full_children.head();
	//temp_node=node->full_children.head()->link_right_side;
	temp_node=prec_node->link_right_side;//
	while(temp_node!=node->full_children.head())
		{
		if(temp_node->label==EMPTY)
			{
			prec_node->link_right_side=temp_node;
			temp_node->link_left_side=prec_node;
			prec_node=temp_node;
			}
		temp_node=temp_node->link_right_side;
		}
	
	//PQ_node nodes_array[node->full_children.size()];//
	PQ_node* nodes_array;
	nodes_array=new PQ_node[node->full_children.size()];
	int l=0;
	forall(temp_node,node->full_children)
		{
		nodes_array[l]=temp_node;
		l++;
		}
	for(int i=1; i<node->full_children.size(); i++)
		{
		temp_node=nodes_array[i];
		temp_node->link_right_side=nodes_array[i-1];
		if(i==node->full_children.size()-1)
			temp_node->link_left_side=prec_node;
		else
			temp_node->link_left_side=nodes_array[i+1];
		}
	if(node->full_children.size()==1)
		node->full_children.head()->link_left_side=prec_node;
	else
		node->full_children.head()->link_left_side=nodes_array[1];//
	prec_node->link_right_side=node->full_children.tail();
	/*
	for(int i=1;i<node->full_children.size();i++)
		{
		temp_node=node->full_children.contents(node->full_children.get_item(i)); //
		temp_node -> link_right_side=node->full_children.contents(node->full_children.get_item(i-1));
		if(i==node->full_children.size()-1)
			temp_node->link_left_side=prec_node;
		else
			temp_node ->link_left_side=node->full_children.contents(node->full_children.get_item(i+1));
		}
	if(node->full_children.size()==1)
		node->full_children.head()->link_left_side=prec_node;
	else
		node->full_children.head()->link_left_side=node->full_children.contents(node->full_children.get_item(1));
	prec_node->link_right_side=node->full_children.tail();*/
	if(node->partial_children.size()>0)
		{
		partial_node=node->partial_children.head();
		temp_node=node->full_children.head();//
		partial_node->link_right_side=temp_node->link_right_side;
		temp_node->link_right_side->link_left_side=partial_node;
		partial_node->link_left_side=temp_node;
		temp_node->link_right_side=partial_node;
		if(node->partial_children.size()==2)
			{
			temp_node=node->full_children.tail();//
			partial_node=node->partial_children.tail();
			partial_node->link_left_side=temp_node->link_left_side;
			temp_node->link_left_side->link_right_side=partial_node;
			partial_node->link_right_side=temp_node;
			temp_node->link_left_side=partial_node;
			}
		}
	return true;
	}
	
	
	
	template<class T>
	bool
	gdt::PQ_tree<T>::
qnode_reversing
	(
	PQ_node node
	)
	{
	PQ_node temp_pq,temp_node;
	if(node->type!=QNODE)
		return false;
	temp_pq=node->left_endmost;
	while(temp_pq!=NULL)
		{
		temp_node=temp_pq->link_right_side;
		temp_pq->link_right_side=temp_pq->link_left_side;
		temp_pq->link_left_side=temp_node;
		temp_pq=temp_node;
		}
	temp_pq=node->left_endmost;
	node->left_endmost=node->right_endmost;
	node->right_endmost=temp_pq;
	return true;
	}
	
	
	
	template<class T>
	bool
	gdt::PQ_tree<T>::
assign_parent_to_draw_graph //da togliere serve per disegnare il grafo
	(
	PQ_node node,
	int count
	)
	{
	PQ_node temp_node,current_parent,child;
	/*forall(temp_node,blocked_list)
		cout<<temp_node->id<<" ";
	cout<<endl;*/
	temp_node=blocked_list.head()->link_right_side;
	/*while(!(temp_node->is_endmost()))
		temp_node=temp_node->link_right_side;*/
		
	while(temp_node->link_right_side!=NULL)//
		temp_node=temp_node->link_right_side;//
		
	child=temp_node;
	current_parent=child->parent;
	forall(temp_node,blocked_list)
		{
		temp_node->parent=current_parent;
		if(temp_node->label==FULL)
			current_parent->full_children.append(temp_node);
		}
	current_parent->label=PARTIAL;
	full_queue.append(current_parent);
	if(current_parent->parent!=NULL)
		current_parent->parent->partial_children.append(current_parent);
	current_parent->child_count=current_parent->child_count+count;
	return true;
	}
	
	
	
	template<class T>
	void
	gdt::PQ_tree<T>::
add_leaf
	(
	T value
	)
	{
	PQ_node node,right_node,left_node,leaf;
	leaf=new(PQ_node_struct<T>);
	leaf->id=generate_id();
	leaf->type=LEAF;
	leaf->value=value;
	if(number_of_nodes>1)
		{
		leaf->parent=root;
		right_node=leaves.head();
		while(right_node->parent!=root)
			right_node=right_node->parent;
		left_node=right_node->link_left_side;
		leaf->link_right_side=right_node;
		leaf->link_left_side=left_node;
		right_node->link_left_side=leaf;
		if(left_node!=NULL)
			left_node->link_right_side=leaf;
		}
	else if (number_of_nodes==1)
		{
		leaf->link_right_side=root;
		leaf->link_left_side=root;
		root->link_right_side=leaf;
		root->link_left_side=leaf;
		node=new(PQ_node_struct<T>);
		node->id=generate_id();
		node->type=PNODE;
		node->parent=NULL;
		node->child_count=2;
		number_of_nodes++;
		root->parent=node;
		leaf->parent=node;
		root=node;
		}
	     else
	     	{
		leaf->parent=NULL;
		root=leaf;
		}
	number_of_nodes++;
	leaves.append(leaf);
	map_leaves[value]=leaf;
	}
	
	
	
	template <class T>
	bool
	gdt::PQ_tree<T>::
bubble
	(
	gdt::gdtlist<T> &S
	)
	{
	//gdt::gdtlist<PQ_node> blocked_siblings, unblocked_siblings; non ci sono più utilizzo link
	//stop5.start();
	//stop1.start();
	gdt::gdtlist<PQ_node> blocked_consecutive_siblings;
	T temp_i;
	int blocked_siblings;
	PQ_node temp_pq,current_parent,temp_node,pseudo_node;
	reset_tree();
	forall(temp_i,S)
		{
		temp_pq=find_leaf(temp_i);
		if(temp_pq==NULL)
			gdt_error("A leaf doesn't exist");
		queue.append(temp_pq);
		}
	//stop1.pause();
	while((queue.size()+block_count+off_the_top)>1)
		{
		//stop2.start();
		if(queue.empty())
			{
			make_empty();
			std::cout<<"Could not reduce {";
			forall(temp_i,S)
				if(temp_i!=S.tail())
					std::cout<<find_leaf(temp_i)->id<<",";
			std::cout<<S.tail()<<"}"<<std::endl;
			return false;
			}
		temp_pq=queue.pop();
		blocked_siblings=0;
		//forall(temp_sibling,temp_pq->immediate_siblings)//TOLTO perchè non uso immediate_siblings
		//	if(temp_sibling->mark==BLOCKED)
		//		blocked_siblings.append(temp_sibling);
		//	else if(temp_sibling->mark==UNBLOCKED)
		//		unblocked_siblings.append(temp_sibling);
		//if (unblocked_siblings.size()>0)
		//	{
		//	temp_sibling=(PQ_node)unblocked_siblings.first();
		//	temp_pq->parent=temp_sibling->parent;
		//	temp_pq->mark=UNBLOCKED;
		//	}
		//else if(temp_pq->immediate_siblings.size()<2)
		//	temp_pq->mark=UNBLOCKED;
		if((is_root(temp_pq))||(temp_pq->parent->type==PNODE)||(temp_pq->is_endmost()))
			temp_pq->mark=UNBLOCKED;
		     else if((temp_pq->link_right_side->is_endmost())||(temp_pq->link_right_side->mark==UNBLOCKED))
				{
				temp_pq->parent=temp_pq->link_right_side->parent;
				temp_pq->mark=UNBLOCKED;
				}
			  else if((temp_pq->link_left_side->is_endmost())||(temp_pq->link_left_side->mark==UNBLOCKED))
					{
					temp_pq->parent=temp_pq->link_left_side->parent;
					temp_pq->mark=UNBLOCKED;
					}
			       else
					{
					temp_pq->mark=BLOCKED;
					blocked_nodes++;
					if((temp_pq->link_right_side->mark==BLOCKED) &&(temp_pq->link_left_side->mark==BLOCKED))
						block_count--;
					else if((temp_pq->link_right_side->mark!=BLOCKED) &&(temp_pq->link_left_side->mark!=BLOCKED))
						block_count++;
					blocked_list.append(temp_pq);
					//blocked_siblings=2;
					//block_count=2;
					}
		//stop2.pause();
		//stop3.start();
		if(temp_pq->mark==UNBLOCKED)
			{
			current_parent=temp_pq->parent;
			/*if(current_parent!=NULL)
				cout<<current_parent->id<<" "<<current_parent->pertinent_child_count<<endl;*/
			if(((temp_pq->link_left_side!=NULL)&&(temp_pq->link_left_side->mark==BLOCKED)) || ((temp_pq->link_right_side!=NULL)&&(temp_pq->link_right_side->mark==BLOCKED)))
				{
				//blocked_siblings=1;
				temp_node=temp_pq;
				while(temp_node->link_left_side->mark==BLOCKED)
					{
					blocked_consecutive_siblings.append(temp_node->link_left_side);
					temp_node=temp_node->link_left_side;
					}
				temp_node=temp_pq;
				while(temp_node->link_right_side->mark==BLOCKED)
					{
					blocked_consecutive_siblings.append(temp_node->link_right_side);
					temp_node=temp_node->link_right_side;
					}
				forall(temp_node,blocked_consecutive_siblings)
					{
					temp_node->mark=UNBLOCKED;
					temp_node->parent=current_parent;
					blocked_list.remove(temp_node);
					current_parent->pertinent_child_count=current_parent->pertinent_child_count+1;
					}
				block_count--;
				blocked_nodes=blocked_nodes-blocked_consecutive_siblings.size();
				blocked_consecutive_siblings.clear();
				}
			if(current_parent==NULL)
				off_the_top=1;
			else
				{
				current_parent->pertinent_child_count=current_parent->pertinent_child_count+1;
				if(current_parent->mark==UNMARKED)
					{
					queue.append(current_parent);
					current_parent->mark=QUEUED;
					}
				}
			//block_count=block_count - blocked_siblings;
			//blocked_nodes=blocked_nodes-blocked_consecutive_siblings.size();
			}
		//stop3.pause();
		/*else
			{
			block_count=block_count + 1 - blocked_siblings;
			blocked_list.append(temp_pq);
			blocked_nodes=blocked_nodes + 1;
			}*/
		}
	//stop4.start();
	if(blocked_list.size()>1)
		{
		pseudo_node=new(PQ_node_struct<T>);
		pseudo_node->id=generate_id();
		pseudo_node->type=PSEUDONODE;
		forall(temp_node,blocked_list)
			{
			temp_node->parent=pseudo_node;
			if(temp_node->link_right_side->mark==UNMARKED)
				pseudo_node->right_endmost=temp_node;
			else if(temp_node->link_left_side->mark==UNMARKED)
				pseudo_node->left_endmost=temp_node;
			}
		pseudo_node->child_count=blocked_list.size();
		pseudo_node->pertinent_child_count=blocked_list.size();
		}
	else if(blocked_list.size()==1)
		blocked_list.clear();
	//stop4.pause();
	//stop5.pause();
	return true;
	}
	
	
	
	template<class T>
	bool
	gdt::PQ_tree<T>::
template_L1
	(
	PQ_node node
	)
	{
	if(node->type!=LEAF)
		return false;
	//stop1.start();
	node->label=FULL;
	if(!is_root(node))
		node->parent->full_children.append(node);
	full_queue.append(node);
	node->pertinent_child_count=0;//??
	node->pertinent_leaf_count=0;//??
	node->mark=UNMARKED;
	//l1++;
	//stop1.pause();
	return true;
	}
	
	
	template<class T>
	bool
	gdt::PQ_tree<T>::
template_P1
	(
	PQ_node node
	)
	{
	if(node->type!=PNODE)
		return false;
	if(node->child_count!=node->full_children.size())//basta per controllare se non ci sono partial o empty
		return false;
	//stop2.start();
	node->label=FULL;
	if(!is_root(node))
		node->parent->full_children.append(node);
	full_queue.append(node);
	node->pertinent_child_count=0;//??
	node->pertinent_leaf_count=0;//??
	node->mark=UNMARKED;
	//p1++;
	//stop2.pause();
	return true;
	}
	
	
	template<class T>
	bool
	gdt::PQ_tree<T>::
template_P2
	(
	PQ_node node
	)
	{
	PQ_node full_node,temp_node;
	if(node->type!=PNODE)
		return false;
	if(node->full_children.size()==0)
		return false;
	if(node->partial_children.size()!=0)
		return false;
	if(node->full_children.size()==node->child_count)
		return false;
	//stop3.start();
	full_node=new(PQ_node_struct<T>);
	full_node->id=generate_id();
	full_node->type=PNODE;
	full_node->label=FULL;//tolgo
	full_queue.append(full_node);
	full_node->parent=node;
	pnode_sorting(node);
	forall(temp_node,node->full_children)
		{
		temp_node->parent=full_node;
		full_node->full_children.append(temp_node);//togliere chissà
		full_node->child_count++;
		}
	full_node->link_right_side=node->full_children.head()->link_right_side;
	full_node->link_left_side=node->full_children.tail()->link_left_side;
	full_node->link_right_side->link_left_side=full_node;
	full_node->link_left_side->link_right_side=full_node;
	node->full_children.head()->link_right_side=node->full_children.tail();
	node->full_children.tail()->link_left_side=node->full_children.head();
	node->child_count=node->child_count-node->full_children.size()+1;
	node->full_children.clear();
	node->full_children.append(full_node);//tolgo
	number_of_nodes++;
	node->pertinent_child_count=0;//??
	node->pertinent_leaf_count=0;//??
	node->mark=UNMARKED;
	full_queue.append(node);//
	//p2++;
	//stop3.pause();
	/*cout<<root->id<<endl;
	cout<<root->child_count<<endl;
	forall(temp_node,root->full_children)
		cout<<temp_node->id<<" ";
	cout<<endl;
	forall(temp_node,root->partial_children)
		cout<<temp_node->id<<" ";
	cout<<endl;
	temp_node=root->full_children.head()->link_right_side;
	while(temp_node!=root->full_children.head())
		{
		cout<<temp_node->id<<" ";
		temp_node=temp_node->link_right_side;
		}
	cout<<endl;
	temp_node=root->full_children.head()->link_left_side;
	while(temp_node!=root->full_children.head())
		{
		cout<<temp_node->id<<" ";
		temp_node=temp_node->link_left_side;
		}
	cout<<endl;
	forall(temp_node,root->full_children.head()->full_children)
		cout<<temp_node->id<<" ";
	cout<<endl;
	cout<<root->full_children.head()->child_count<<endl;
	temp_node=root->full_children.head()->full_children.head()->link_right_side;
	while(temp_node!=root->full_children.head()->full_children.head())
		{
		cout<<temp_node->id<<" ";
		temp_node=temp_node->link_right_side;
		}
	cout<<endl;
	temp_node=root->full_children.head()->full_children.head()->link_left_side;
	while(temp_node!=root->full_children.head()->full_children.head())
		{
		cout<<temp_node->id<<" ";
		temp_node=temp_node->link_left_side;
		}
	cout<<endl;*/
	return true;
	}
	
	
	template<class T>
	bool
	gdt::PQ_tree<T>::
template_P3
	(
	PQ_node node
	)
	{
	PQ_node empty_node,full_node,temp_n;
	if(node->type!=PNODE)
		return false;
	if(node->full_children.size()==0)
		return false;
	if(node->partial_children.size()!=0)
		return false;
	if(node->full_children.size()==node->child_count)
		return false;
	//stop4.start();
	pnode_sorting(node);
	if(node->child_count-node->full_children.size()>1)
		{//more than one empty child
		empty_node=new(PQ_node_struct<T>);
		number_of_nodes++;
		empty_node->id=generate_id();
		empty_node->label=EMPTY;// togliere è automaticamente empty
		empty_node->parent=node;
		empty_node->type=PNODE;
		temp_n=node->full_children.head()->link_right_side;
		while(temp_n->label==EMPTY)
			{
			temp_n->parent=empty_node;
			empty_node->child_count++;
			temp_n=temp_n->link_right_side;
			}
		node->full_children.head()->link_right_side->link_left_side=node->full_children.tail()->link_left_side;
		node->full_children.tail()->link_left_side->link_right_side=node->full_children.head()->link_right_side;
		}
	else//one empty child
		empty_node=node->full_children.head()->link_right_side;
	if(node->full_children.size()>1)
		{//more than one full children
		full_node=new(PQ_node_struct<T>);
		number_of_nodes++;
		full_node->id=generate_id();
		full_node->label=FULL;
		full_queue.append(full_node);
		full_node->parent=node;
		full_node->type=PNODE;
		node->full_children.head()->link_right_side=node->full_children.tail();
		node->full_children.tail()->link_left_side=node->full_children.head();
		forall(temp_n,node->full_children)
			{
			temp_n->parent=full_node;
			full_node->child_count++;
			full_node->full_children.append(temp_n);
			}
		}
	else//one full child
		full_node=node->full_children.head();
	empty_node->link_right_side=full_node;
	empty_node->link_left_side=NULL;
	full_node->link_right_side=NULL;
	full_node->link_left_side=empty_node;
	node->child_count=2;
	node->type=QNODE;
	node->label=PARTIAL;
	full_queue.append(node);
	node->full_children.clear();
	node->full_children.append(full_node);
	node->left_endmost=empty_node;
	node->right_endmost=full_node;
	node->parent->partial_children.append(node);
	node->pertinent_child_count=0;//??
	node->pertinent_leaf_count=0;//??
	node->mark=UNMARKED;
	//p3++;
	//stop4.pause();
	/*cout<<root->id<<endl;
	cout<<root->child_count<<endl;
	forall(temp_n,root->full_children)
		cout<<temp_n->id<<" ";
	cout<<endl;
	forall(temp_n,root->partial_children)
		cout<<temp_n->id<<" ";
	cout<<endl;
	temp_n=root->full_children.head()->link_right_side;
	while(temp_n!=root->full_children.head())
		{
		cout<<temp_n->id<<" ";
		temp_n=temp_n->link_right_side;
		}
	cout<<endl;
	temp_n=root->full_children.head()->link_left_side;
	while(temp_n!=root->full_children.head())
		{
		cout<<temp_n->id<<" ";
		temp_n=temp_n->link_left_side;
		}
	cout<<endl;
	forall(temp_n,root->full_children.head()->link_right_side->link_right_side->full_children)
		cout<<temp_n->id<<" ";
	cout<<endl;
	cout<<root->full_children.head()->link_right_side->link_right_side->child_count<<endl;
	temp_n=root->full_children.head()->link_right_side->link_right_side->full_children.head()->link_right_side;
	while(temp_n!=root->full_children.head()->link_right_side->link_right_side->full_children.head())
		{
		cout<<temp_n->id<<" ";
		temp_n=temp_n->link_right_side;
		}
	cout<<endl;
	temp_n=root->full_children.head()->link_right_side->link_right_side->full_children.head()->link_left_side;
	while(temp_n!=root->full_children.head()->link_right_side->link_right_side->full_children.head())
		{
		cout<<temp_n->id<<" ";
		temp_n=temp_n->link_left_side;
		}
	cout<<endl;
	forall(temp_n,root->full_children.head()->link_right_side->link_right_side->partial_children)
		cout<<temp_n->id<<" ";
	cout<<endl;*/
	return true;
	}
	
	
	template<class T>
	bool
	gdt::PQ_tree<T>::
template_P4
	(
	PQ_node node
	)
	{
	PQ_node partial_node,temp_n,full_node,current_parent;
	if(node->type!=PNODE)
		return false;
	if(node->partial_children.size()!=1)
		return false;
	//stop5.start();
	partial_node=node->partial_children.head();
	if(node->full_children.size()!=0)
		{
		pnode_sorting(node);
		if(node->full_children.size()>1)
			{
			full_node=new(PQ_node_struct<T>);
			number_of_nodes++;
			full_node->id=generate_id();
			full_node->label=FULL;//tolgo
			full_node->type=PNODE;
			full_queue.append(full_node);//tolgo ATTENZIONE partial_node->full_children.append(full_node);
			partial_node->link_left_side=node->full_children.tail()->link_left_side;
			node->full_children.tail()->link_left_side->link_right_side=partial_node;
			node->full_children.head()->link_right_side=node->full_children.tail();
			node->full_children.tail()->link_left_side=node->full_children.head();
			forall(temp_n,node->full_children)
				{
				temp_n->parent=full_node;
				full_node->child_count++;
				full_node->full_children.append(temp_n);
				}
			}
		else if(node->full_children.size()==1)
			{
			full_node=node->full_children.head();
			if(node->child_count-node->full_children.size()-node->partial_children.size()!=0)
				{
				partial_node->link_left_side=full_node->link_left_side;
				full_node->link_left_side->link_right_side=partial_node;
				}
			else
				{
				partial_node->link_right_side=NULL;
				partial_node->link_left_side=NULL;
				}
			full_node->link_right_side=NULL;
			full_node->link_left_side=NULL;
			}
		full_node->parent=partial_node;
		partial_node->child_count++;
		partial_node->full_children.append(full_node);
		if(partial_node->left_endmost->label==FULL)
			{
			partial_node->left_endmost->link_left_side=full_node;
			full_node->link_right_side=partial_node->left_endmost;
			partial_node->left_endmost=full_node;
			}
		else
			{
			partial_node->right_endmost->link_right_side=full_node;
			full_node->link_left_side=partial_node->right_endmost;
			partial_node->right_endmost=full_node;
			}
		node->child_count=node->child_count-node->full_children.size();
		node->full_children.clear();
		}
	node->pertinent_child_count=0;//??
	node->pertinent_leaf_count=0;//??
	node->mark=UNMARKED;
	if(node->child_count==1)
		{
		if(!is_root(node))
			{
			current_parent=node->parent;
			partial_node->parent=current_parent;
			current_parent->partial_children.append(partial_node);
			if(current_parent->right_endmost==node)
				current_parent->right_endmost=partial_node;
			else if(current_parent->left_endmost==node)
				current_parent->left_endmost=partial_node;
			partial_node->link_right_side=node->link_right_side;
			if(node->link_right_side!=NULL)
				node->link_right_side->link_left_side=partial_node;
			partial_node->link_left_side=node->link_left_side;
			if(node->link_left_side!=NULL)
				node->link_left_side->link_right_side=partial_node;
			full_queue.append(current_parent);//
			}
		else
			{
			root=partial_node;
			partial_node->parent=NULL;
			}
		delete(node);
		number_of_nodes--;
		}
	else//
		full_queue.append(node);//
	//p4++;
	//stop5.pause();
	/*cout<<root->id<<endl;
	cout<<root->child_count<<endl;
	forall(temp_n,root->full_children)
		cout<<temp_n->id<<" ";
	cout<<endl;
	forall(temp_n,root->partial_children)
		cout<<temp_n->id<<" ";
	cout<<endl;
	if(root->partial_children.size()>0)
		{
		temp_n=root->partial_children.head()->link_right_side;
		while(temp_n!=root->partial_children.head())
			{
			cout<<temp_n->id<<" ";
			temp_n=temp_n->link_right_side;
			}
		cout<<endl;
		temp_n=root->partial_children.head()->link_left_side;
		while(temp_n!=root->partial_children.head())
			{
			cout<<temp_n->id<<" ";
			temp_n=temp_n->link_left_side;
			}
		cout<<endl;
		cout<<root->partial_children.head()->child_count<<endl;
		forall(temp_n,root->partial_children.head()->full_children)
			cout<<temp_n->id<<" ";
		cout<<endl;
		forall(temp_n,root->partial_children.head()->partial_children)
			cout<<temp_n->id<<" ";
		cout<<endl;
		temp_n=root->partial_children.head()->full_children.head()->link_right_side;
		while(temp_n!=NULL)
			{
			cout<<temp_n->id<<" ";
			temp_n=temp_n->link_right_side;
			}
		cout<<endl;
		temp_n=root->partial_children.head()->full_children.head()->link_left_side;
		while(temp_n!=NULL)
			{
			cout<<temp_n->id<<" ";
			temp_n=temp_n->link_left_side;
			}
		cout<<endl;
		}*/
	/*forall(temp_pq,root->partial_children.head()->full_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	temp_pq=root->partial_children.head()->full_children.head()->link_right_side;
	while(temp_pq!=NULL)
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_right_side;
		}
	cout<<endl;
	temp_pq=root->partial_children.head()->full_children.head()->link_left_side;
	while(temp_pq!=NULL)
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_left_side;
		}
	cout<<endl;
	cout<<find_leaf(1)->parent->id<<endl;*/
	return true;
	}
	
	
	template<class T>
	bool
	gdt::PQ_tree<T>::
template_P5
	(
	PQ_node node
	)
	{
	int empty_count=0;
	PQ_node partial,empty_endmost,full_endmost,temp_pq,empty_sibling,full,empty;
	if(node->type!=PNODE)
		return false;
	if(node->partial_children.size()!=1)//qua conviene lista e non due partial children
		return false;
	//stop6.start();
	partial=node->partial_children.head();//the unique element in partial_children
	if(partial->left_endmost->label==FULL)
		{
		full_endmost=partial->left_endmost;
		empty_endmost=partial->right_endmost;
		}
	else
		{
		full_endmost=partial->right_endmost;
		empty_endmost=partial->left_endmost;
		}
	temp_pq=partial->link_right_side;
	while(temp_pq!=partial) //conto gli empty e se uno lo memorizzo
		{
		if(temp_pq->label==EMPTY)
			{
			empty_count++;
			empty_sibling=temp_pq;
			}
		temp_pq=temp_pq->link_right_side;
		}
	/*if(empty_count!=1)
		empty_sibling=NULL;*/
	partial->parent=node->parent;
	partial->pertinent_leaf_count=node->pertinent_leaf_count;
	//partial->label=PARTIAL;
	partial->parent->partial_children.append(partial);//qua va bene la lista non so due elementi
	partial->remove_from_circular_link();
	partial->link_right_side=node->link_right_side;
	/*if(blocked_list.del(node)!=NULL)
		blocked_list.append(partial);*/ //??
	if(blocked_list.search(node)!=NULL)
		{
		blocked_list.remove(node);
		blocked_list.append(partial);
		}
	if(partial->link_right_side!=NULL)
		partial->link_right_side->link_left_side=partial;
	partial->link_left_side=node->link_left_side;
	if(partial->link_left_side!=NULL)
		partial->link_left_side->link_right_side=partial;
	if(partial->parent->left_endmost==node)
		partial->parent->left_endmost=partial;
	else if(partial->parent->right_endmost==node)
		partial->parent->right_endmost=partial;
	if(node->full_children.size()>0)
		{
		if(node->full_children.size()==1)
			{
			full=node->full_children.head();
			full->remove_from_circular_link();
			}
		else
			{
			full=new(PQ_node_struct<T>);
			number_of_nodes++;
			full->id=generate_id();
			full->type=PNODE;
			full->label=FULL;
			full_queue.append(full);
			forall(temp_pq,node->full_children)
				{
				temp_pq->remove_from_circular_link();
				temp_pq->parent=full;
				full->full_children.append(temp_pq);
				}
				
			//PQ_node nodes_array[node->full_children.size()];//
			PQ_node* nodes_array;
			nodes_array=new PQ_node[node->full_children.size()];
			int l=0;
			forall(temp_pq,node->full_children)
				{
				nodes_array[l]=temp_pq;
				l++;
				}
			for(int i=1;i<node->full_children.size()-1;i++)
				{
				nodes_array[i]->link_left_side=nodes_array[i-1];
				nodes_array[i]->link_right_side=nodes_array[i+1];
				}
			nodes_array[0]->link_left_side=nodes_array[node->full_children.size()-1];
			nodes_array[0]->link_right_side=nodes_array[1];
			nodes_array[node->full_children.size()-1] -> link_left_side=nodes_array[node->full_children.size()-2];
			nodes_array[node->full_children.size()-1]->link_right_side=nodes_array[0];//
			/*
			for(int i=0;i<node->full_children.size();i++)
				{
				node->full_children.contents(node->full_children.get_item(i))->link_left_side=node->full_children.contents(node->full_children.cyclic_pred(node->full_children.get_item(i)));
				node->full_children.contents(node->full_children.get_item(i))->link_right_side=node->full_children.contents(node->full_children.cyclic_succ(node->full_children.get_item(i)));
				}*/
			full->child_count=node->full_children.size();
			}
		full->parent=partial;
		partial->full_children.append(full);
		partial->child_count++;
		if(full_endmost->link_left_side==NULL)
			{
			full_endmost->link_left_side=full;
			full->link_right_side=full_endmost;
			full->link_left_side=NULL;
			partial->left_endmost=full;
			}
		else
			{
			full_endmost->link_right_side=full;
			full->link_left_side=full_endmost;
			full->link_right_side=NULL;
			partial->right_endmost=full;
			}
		/*if(partial->left_endmost==full_endmost)
			partial->left_endmost=full;
		else
			partial->right_endmost=full;*/
		}
	//number_empty=node->child_count-node->full_children.size()-node->partial_children.size();
	if(empty_count>0)
		{
		if(empty_count==1)
			empty=empty_sibling;
		else
			{
			empty=new(PQ_node_struct<T>);
			number_of_nodes++;
			empty->id=generate_id();
			empty->type=PNODE;
			empty->label=EMPTY;
			temp_pq=empty_sibling->link_right_side;
			while(temp_pq!=empty_sibling)
				{
				temp_pq->parent=empty;
				temp_pq=temp_pq->link_right_side;
				}
			empty_sibling->parent=empty;
			empty->child_count=empty_count;
			}
		empty->parent=partial;
		partial->child_count++;
		if(empty_endmost->link_left_side==NULL)
			{
			empty_endmost->link_left_side=empty;
			empty->link_right_side=empty_endmost;
			empty->link_left_side=NULL;
			partial->left_endmost=empty;
			}
		else
			{
			empty_endmost->link_right_side=empty;
			empty->link_left_side=empty_endmost;
			empty->link_right_side=NULL;
			partial->right_endmost=empty;
			}
		/*if(partial->left_endmost==empty_endmost)
			partial->left_endmost=empty;
		else
			partial->right_endmost=empty;*/
		}
	delete(node);
	number_of_nodes--;
	partial->pertinent_child_count=0;//??
	partial->pertinent_leaf_count=0;//??
	partial->mark=UNMARKED;
	//p5++;
	//stop6.pause();
	/*cout<<root->id<<endl;
	cout<<root->child_count<<endl;
	forall(temp_pq,root->full_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	forall(temp_pq,root->partial_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	cout<<root->partial_children.head()->link_right_side->link_right_side->id<<endl;*/
	/*temp_pq=root->partial_children.head()->link_right_side;
	while(temp_pq!=root->partial_children.head())
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_right_side;
		}
	cout<<endl;
	temp_pq=root->partial_children.head()->link_left_side;
	while(temp_pq!=root->partial_children.head())
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_left_side;
		}
	cout<<endl;
	cout<<root->partial_children.head()->child_count<<endl;
	forall(temp_pq,root->partial_children.head()->full_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	forall(temp_pq,root->partial_children.head()->partial_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	temp_pq=root->partial_children.head()->full_children.head()->link_right_side;
	while(temp_pq!=NULL)
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_right_side;
		}
	cout<<endl;
	temp_pq=root->partial_children.head()->full_children.head()->link_left_side;
	while(temp_pq!=NULL)
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_left_side;
		}
	cout<<endl;*/
	return true;
	}
	
	
	template<class T>
	bool
	gdt::PQ_tree<T>::
template_P6
	(
	PQ_node node
	)
	{
	if(node->type!=PNODE)
		return false;
	if(node->partial_children.size()!=2)
		return false;
	PQ_node full_node,partial_node,q_partial_node,temp_node,temp_n,prec_node,current_parent;
	//stop7.start();
	if(node->full_children.size()!=0)
		{
		pnode_sorting(node);
		}
	else //vedere
		{
		prec_node=node->partial_children.head();
		//temp_n=node->partial_children.head()->link_right_side;
		temp_n=prec_node->link_right_side;//
		while(temp_n!=node->partial_children.head())
			{
			if(temp_n->label==EMPTY)
				{
				prec_node->link_right_side=temp_n;
				temp_n->link_left_side=prec_node;
				prec_node=temp_n;
				}
			temp_n=temp_n->link_right_side;
			}
		prec_node->link_right_side=node->partial_children.tail();
		node->partial_children.tail()->link_left_side=prec_node;
		node->partial_children.tail()->link_right_side=node->partial_children.head();
		node->partial_children.head()->link_left_side=node->partial_children.tail();
		}
	q_partial_node=new(PQ_node_struct<T>);
	number_of_nodes++;
	q_partial_node->id=generate_id();
	q_partial_node->label=PARTIAL;
	q_partial_node->type=QNODE;
	full_queue.append(q_partial_node);
	if(node->child_count-node->full_children.size()-2>0) //there are empty children
		{
		q_partial_node->link_right_side=node->partial_children.head()->link_right_side;
		node->partial_children.head()->link_right_side->link_left_side=q_partial_node;
		q_partial_node->link_left_side=node->partial_children.tail()->link_left_side;
		node->partial_children.tail()->link_left_side->link_right_side=q_partial_node;
		}
	if(node->full_children.size()>0)
		{
		if(node->full_children.size()>1)
			{
			full_node=new(PQ_node_struct<T>);
			number_of_nodes++;
			full_node->id=generate_id();
			full_node->label=FULL;//tolgo
			full_node->type=PNODE;
			full_queue.append(full_node);//tolgo ATTENZIONE partial_node->full_children.append(full_node);
			node->full_children.head()->link_right_side=node->full_children.tail();
			node->full_children.tail()->link_left_side=node->full_children.head();
			forall(temp_n,node->full_children)
				{
				temp_n->parent=full_node;
				full_node->child_count++;
				full_node->full_children.append(temp_n);
				}
			}
		else //if(node->full_children==1)
			full_node=node->full_children.head();
		full_node->parent=q_partial_node;
		q_partial_node->child_count++;
		q_partial_node->full_children.append(full_node);
		}
	partial_node=node->partial_children.pop();
	if(partial_node->left_endmost->label==EMPTY)
		{
		q_partial_node->left_endmost=partial_node->left_endmost;
		temp_node=partial_node->left_endmost;
		while(temp_node!=NULL)
			{
			temp_node->parent=q_partial_node;
			prec_node=temp_node;
			temp_node=temp_node->link_right_side;
			}
		}
	else //if(partial_node->right_endmost->label==EMPTY)
		{
		q_partial_node->left_endmost=partial_node->right_endmost;
		temp_node=partial_node->right_endmost;
		while(temp_node!=NULL)
			{
			temp_node->parent=q_partial_node;
			temp_n=temp_node->link_left_side;
			temp_node->link_left_side=temp_node->link_right_side;
			temp_node->link_right_side=temp_n;
			prec_node=temp_node;
			temp_node=temp_n;
			}
		}
	if(node->full_children.size()>0)
		{
		prec_node->link_right_side=full_node;
		full_node->link_left_side=prec_node;
		q_partial_node->right_endmost=full_node;
		}
	else
		q_partial_node->right_endmost=prec_node;
	q_partial_node->child_count=q_partial_node->child_count+partial_node->child_count;
	forall(temp_n,partial_node->full_children)
		q_partial_node->full_children.append(temp_n);
	temp_n=full_queue.pop();
	while(temp_n!=partial_node)
		{
		full_queue.append(temp_n);
		temp_n=full_queue.pop();
		}
	delete(partial_node);
	number_of_nodes--;
	partial_node=node->partial_children.pop();
	if(partial_node->left_endmost->label==FULL)
		{
		q_partial_node->right_endmost->link_right_side=partial_node->left_endmost;
		partial_node->left_endmost->link_left_side=q_partial_node->right_endmost;
		q_partial_node->right_endmost=partial_node->right_endmost;
		temp_node=partial_node->left_endmost;
		while(temp_node!=NULL)
			{
			temp_node->parent=q_partial_node;
			temp_node=temp_node->link_right_side;
			}
		}
	else //if(partial_node->right_endmost->label==FULL)
		{
		q_partial_node->right_endmost->link_right_side=partial_node->right_endmost;
		partial_node->right_endmost->link_right_side=q_partial_node->right_endmost;
		q_partial_node->right_endmost=partial_node->left_endmost;
		temp_node=partial_node->right_endmost;
		while(temp_node!=NULL)
			{
			temp_node->parent=q_partial_node;
			temp_n=temp_node->link_left_side;
			temp_node->link_left_side=temp_node->link_right_side;
			temp_node->link_right_side=temp_n;
			temp_node=temp_n;
			}
		}
	q_partial_node->child_count=q_partial_node->child_count+partial_node->child_count;
	forall(temp_n,partial_node->full_children)
		q_partial_node->full_children.append(temp_n);
	temp_n=full_queue.pop();
	while(temp_n!=partial_node)
		{
		full_queue.append(temp_n);
		temp_n=full_queue.pop();
		}
	delete(partial_node);
	number_of_nodes--;
	q_partial_node->parent=node;
	node->child_count=node->child_count-node->full_children.size()-2+1;
	node->full_children.clear();
	node->partial_children.append(q_partial_node); //lo posso togliere
	node->pertinent_child_count=0;//??
	node->pertinent_leaf_count=0;//??
	node->mark=UNMARKED;
	if(node->child_count==1)
		{
		if(!is_root(node))
			{
			current_parent=node->parent;
			q_partial_node->parent=current_parent;
			current_parent->partial_children.append(q_partial_node);
			if(current_parent->right_endmost==node)
				current_parent->right_endmost=q_partial_node;
			else if(current_parent->left_endmost==node)
				current_parent->left_endmost=q_partial_node;
			q_partial_node->link_right_side=node->link_right_side;
			if(node->link_right_side!=NULL)
				node->link_right_side->link_left_side=q_partial_node;
			q_partial_node->link_left_side=node->link_left_side;
			if(node->link_left_side!=NULL)
				node->link_left_side->link_right_side=q_partial_node;
			full_queue.append(current_parent);//
			}
		else
			{
			root=q_partial_node;
			q_partial_node->parent=NULL;
			}
		delete(node);
		number_of_nodes--;
		}
	else
		full_queue.append(node);//
	//p6++;
	//stop7.pause();
	/*cout<<root->id<<endl;
	cout<<root->child_count<<endl;
	forall(temp_n,root->full_children)
		cout<<temp_n->id<<" ";
	cout<<endl;
	forall(temp_n,root->partial_children)
		cout<<temp_n->id<<" ";
	cout<<endl;
	temp_n=root->partial_children.head()->link_right_side;
	while(temp_n!=root->partial_children.head())
		{
		cout<<temp_n->id<<" ";
		temp_n=temp_n->link_right_side;
		}
	cout<<endl;
	temp_n=root->partial_children.head()->link_left_side;
	while(temp_n!=root->partial_children.head())
		{
		cout<<temp_n->id<<" ";
		temp_n=temp_n->link_left_side;
		}
	cout<<endl;
	cout<<root->partial_children.head()->child_count<<endl;
	forall(temp_n,root->partial_children.head()->full_children)
		cout<<temp_n->id<<" ";
	cout<<endl;
	forall(temp_n,root->partial_children.head()->partial_children)
		cout<<temp_n->id<<" ";
	cout<<endl;
	temp_n=root->partial_children.head()->full_children.head()->link_right_side;
	while(temp_n!=NULL)
		{
		cout<<temp_n->id<<" ";
		temp_n=temp_n->link_right_side;
		}
	cout<<endl;
	temp_n=root->partial_children.head()->full_children.head()->link_left_side;
	while(temp_n!=NULL)
		{
		cout<<temp_n->id<<" ";
		temp_n=temp_n->link_left_side;
		}
	cout<<endl;*/
	/*forall(temp_pq,root->partial_children.head()->full_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	temp_pq=root->partial_children.head()->full_children.head()->link_right_side;
	while(temp_pq!=NULL)
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_right_side;
		}
	cout<<endl;
	temp_pq=root->partial_children.head()->full_children.head()->link_left_side;
	while(temp_pq!=NULL)
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_left_side;
		}
	cout<<endl;
	cout<<find_leaf(1)->parent->id<<endl;*/
	return true;
	}
	
	
	template<class T>
	bool
	gdt::PQ_tree<T>::
template_Q1
	(
	PQ_node node
	)
	{
	if(node->type!=QNODE)
		return false;
	if(node->child_count!=node->full_children.size())
		return false;
	//stop8.start();
	node->label=FULL;
	if(!is_root(node))
		node->parent->full_children.append(node);
	full_queue.append(node);
	node->pertinent_child_count=0;//??
	node->pertinent_leaf_count=0;//??
	node->mark=UNMARKED;
	//q1++;
	//stop8.pause();
	return true;
	}
	
	
	template<class T>
	bool
	gdt::PQ_tree<T>::
template_Q2
	(
	PQ_node node
	)
	{
	int full_count=0;
	PQ_node temp_pq,node_y,child_full_endmost,full_sibling,child_empty_endmost,empty_sibling;
	if(node->type!=QNODE)
		return false;
	//if node è uno pseudonode return false
	if(node->partial_children.size()>1)
		return false;
	if(node->full_children.size()>0)
		{
		//if(((node->endmost1->label==FULL)&&(node->endmost2->label==FULL))||((node->endmost1->label==EMPTY) &&(node->endmost2->label==EMPTY)))
		//	return false; //vedere
		forall(temp_pq,node->full_children)
			if((temp_pq==node->left_endmost)||(temp_pq==node->right_endmost))
				{
				full_count++;
				node_y=temp_pq;
				}
		if(full_count!=1)
			return false;
		temp_pq=node_y;
		if(node_y->link_left_side==NULL) //controlla che non ci sono alternanze full ed empty
			for(int i=0;i<node->full_children.size();i++)
				{
				if(temp_pq->label!=FULL)
					return false;
				temp_pq=temp_pq->link_right_side;
				}
		else
			for(int i=0;i<node->full_children.size();i++)
				{
				if(temp_pq->label!=FULL)
					return false;
				temp_pq=temp_pq->link_left_side;
				}
		if(node->partial_children.size()==1)
			if(temp_pq->label!=PARTIAL)
				return false;
		}
	else if((node->left_endmost->label==EMPTY)&&(node->right_endmost->label==EMPTY))
		return false;
	//stop9.start();
	node->label=PARTIAL;
	if(node->parent!=NULL)
		node->parent->partial_children.append(node);
	//full_queue.append(node);
	if(node->partial_children.size()>0)
		{
		node_y=node->partial_children.pop();
		if(node_y->left_endmost->label==FULL)
			{
			child_full_endmost=node_y->left_endmost;
			child_empty_endmost=node_y->right_endmost;
			temp_pq=child_full_endmost;
			/*while(temp_pq->label==FULL)
				{
				node->full_children.append(temp_pq);
				temp_pq=temp_pq->link_right_side;
				}*/ //già messo sotto per disegnare il grafo
			}
		else if(node_y->right_endmost->label==FULL)
			{
			child_full_endmost=node_y->right_endmost;
			child_empty_endmost=node_y->left_endmost;
			temp_pq=child_full_endmost;
			/*while(temp_pq->label==FULL)
				{
				node->full_children.append(temp_pq);
				temp_pq=temp_pq->link_left_side;
				}*/ //già messo sotto per disegnare il grafo
			}
		if((node->left_endmost!=node_y)&&(node_y->link_left_side->label==FULL))
			{
			if(node_y->left_endmost->label==EMPTY)
				qnode_reversing(node_y);
			full_sibling=node_y->link_left_side;
			full_sibling->link_right_side=child_full_endmost;
			child_full_endmost->link_left_side=full_sibling;
			}
		else if((node->right_endmost!=node_y)&&(node_y->link_right_side->label==FULL))
			{
			if(node_y->right_endmost->label==EMPTY)
				qnode_reversing(node_y);
			full_sibling=node_y->link_right_side;
			full_sibling->link_left_side=child_full_endmost;
			child_full_endmost->link_right_side=full_sibling;
			}
		     /*else //node_y non ha sibling con label full
			{
			if(node->left_endmost==node_y)
				node->left_endmost=child_full_endmost;
			else
				node->right_endmost=child_full_endmost;
			}*/
		if((node->left_endmost!=node_y)&&(node_y->link_left_side->label==EMPTY))
			{
			if(node_y->left_endmost->label==FULL)
				qnode_reversing(node_y);
			empty_sibling=node_y->link_left_side;
			empty_sibling->link_right_side=child_empty_endmost;
			child_empty_endmost->link_left_side=empty_sibling;
			}
		else if((node->right_endmost!=node_y)&&(node_y->link_right_side->label==EMPTY))
			{
			if(node_y->right_endmost->label==FULL)
				qnode_reversing(node_y);
			empty_sibling=node_y->link_right_side;
			empty_sibling->link_left_side=child_empty_endmost;
			child_empty_endmost->link_right_side=empty_sibling;
			}
		     else //node_y non ha sibling con label empty
			{
			if(node->left_endmost==node_y)
				node->left_endmost=child_empty_endmost;
			else
				node->right_endmost=child_empty_endmost;
			}
		//child_empty_endmost->parent=node; già messo sotto per disegnare il grafo
		//node->child_count++;
		if(node->full_children.size()==0)
			if(node->left_endmost==node_y)
				node->left_endmost=child_full_endmost;
			else
				node->right_endmost=child_full_endmost;
		//child_full_endmost->parent=node; già messo sotto per disegnare il grafo
		//node->child_count++;
		temp_pq=node_y->left_endmost; //questo è per disegnare il grafo da qui...
		while(temp_pq!=NULL)
			{
			temp_pq->parent=node;
			if(temp_pq->label==FULL)
				node->full_children.append(temp_pq);
			temp_pq=temp_pq->link_right_side;
			} //...a qui
		node->child_count=node->child_count+node_y->child_count;
		temp_pq=full_queue.pop();
		while(temp_pq!=node_y)
			{
			full_queue.append(temp_pq);
			temp_pq=full_queue.pop();
			}
		delete(node_y);
		node->child_count--;
		number_of_nodes--;
		//node->partial_children.clear();
		}
	node->pertinent_child_count=0;//??
	node->pertinent_leaf_count=0;//??
	node->mark=UNMARKED;
	full_queue.append(node);//
	//q2++;
	//stop9.pause();
	/*cout<<root->id<<endl;
	forall(temp_pq,root->full_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	forall(temp_pq,root->partial_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	temp_pq=root->full_children.head()->link_right_side;
	while(temp_pq!=root->full_children.head())
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_right_side;
		}
	cout<<endl;
	temp_pq=root->full_children.head()->link_left_side;
	while(temp_pq!=root->full_children.head())
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_left_side;
		}
	cout<<endl;
	forall(temp_pq,root->partial_children.head()->full_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	temp_pq=root->partial_children.head()->full_children.head()->link_right_side;
	while(temp_pq!=NULL)
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_right_side;
		}
	cout<<endl;
	temp_pq=root->partial_children.head()->full_children.head()->link_left_side;
	while(temp_pq!=NULL)
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_left_side;
		}
	cout<<endl;
	cout<<find_leaf(1)->parent->id<<endl;*/
	return true;
	}
	
	template<class T>
	bool
	gdt::PQ_tree<T>::
template_Q3
	(
	PQ_node node
	)
	{
	if((node->type!=PSEUDONODE)&&(node->type!=QNODE))
		return false;
	if(node->partial_children.size()>2)
		return false;
	if(((node->right_endmost->label==FULL)||(node->left_endmost->label==FULL))&&(node->type==QNODE))
		return false;
	PQ_node temp_pq,node_y1,node_y2,child_full_endmost,child_empty_endmost,full_sibling,empty_sibling,temp_endmost;
	int count=0;
	int l=0;
	if(node->full_children.size()==0)
		{
		if((node->partial_children.head()->link_right_side!=node->partial_children.tail())&& (node->partial_children.head()->link_left_side!=node->partial_children.tail()))
			return false;
		}
	else //if(node->full_children.size()>0)
		{
		if(node->partial_children.size()>0)
			{
			temp_pq=node->partial_children.head();
			if((node->partial_children.head()->link_right_side!=NULL)&& (node->partial_children.head()->link_right_side->label==FULL)) //controlla                    che non ci sono alternanze
				{
				temp_pq=node->partial_children.head()->link_right_side;
				for(int i=0;i<node->full_children.size();i++)
					{
					if(temp_pq->label!=FULL)
						return false;
					temp_pq=temp_pq->link_right_side;
					}
				}
			else if((node->partial_children.head()->link_left_side!=NULL)&& (node->partial_children.head()->link_left_side->label==FULL))
				{
				temp_pq=node->partial_children.head()->link_left_side;
				for(int i=0;i<node->full_children.size();i++)
					{
					if(temp_pq->label!=FULL)
						return false;
					temp_pq=temp_pq->link_left_side;
					}
				}
			     else //il partial_children.head() non ha sibling con label full vicini
				return false;
			}
		else //if(node->partial_children.size()==0)
			{
			temp_pq=node->left_endmost;
			while(temp_pq->label!=FULL)
				temp_pq=temp_pq->link_right_side;
			for(int i=0;i<node->full_children.size();i++)
				{
				if(temp_pq->label!=FULL)
					return false;
				temp_pq=temp_pq->link_right_side;
				}
			}
		}
	//stop10.start();
	if(node->partial_children.size()>0)
		{
		node_y1=node->partial_children.head();
		temp_pq=node_y1->left_endmost; //questo è per disegnare il grafo da qui...
		blocked_list.remove(node_y1);
		count--;
		while(temp_pq!=NULL)
			{
			temp_pq->parent=node;
			if(temp_pq->label==FULL)
				node->full_children.append(temp_pq);
			blocked_list.append(temp_pq);
			count++;
			temp_pq=temp_pq->link_right_side;
			} //...a qui
		if(node_y1->left_endmost->label==FULL)
			{
			child_full_endmost=node_y1->left_endmost;
			child_empty_endmost=node_y1->right_endmost;
			temp_pq=child_full_endmost;
			/*while(temp_pq->label==FULL) //controllare
				{
				node->full_children.append(temp_pq);
				temp_pq=temp_pq->link_right_side;
				}*/ //già messo sopra per disegnare il grafo
			}
		else if(node_y1->right_endmost->label==FULL)
			{
			child_full_endmost=node_y1->right_endmost;
			child_empty_endmost=node_y1->left_endmost;
			temp_pq=child_full_endmost;
			/*while(temp_pq->label==FULL)
				{
				node->full_children.append(temp_pq);
				temp_pq=temp_pq->link_left_side;
				}*/ //già messo sopra per disegnare il grafo
			}
		if((node->left_endmost!=node_y1)&&(node_y1->link_left_side->label==FULL))
			{
			if(node_y1->left_endmost->label==EMPTY)
				qnode_reversing(node_y1);
			full_sibling=node_y1->link_left_side;
			full_sibling->link_right_side=child_full_endmost;
			child_full_endmost->link_left_side=full_sibling;
			}
		else if((node->right_endmost!=node_y1)&&(node_y1->link_right_side->label==FULL))
			{
			if(node_y1->right_endmost->label==EMPTY)
				qnode_reversing(node_y1);
			full_sibling=node_y1->link_right_side;
			full_sibling->link_left_side=child_full_endmost;
			child_full_endmost->link_right_side=full_sibling;
			}
		     else //node_y1 non ha sibling con label full
			temp_endmost=child_full_endmost;
			/*{
			if(node->left_endmost==node_y1)
				node->left_endmost=child_full_endmost;
			else
				node->right_endmost=child_full_endmost;
			}*/
		if((node->left_endmost!=node_y1)&&(node_y1->link_left_side->label==EMPTY))
			{
			if(node_y1->left_endmost->label==FULL)
				qnode_reversing(node_y1);
			empty_sibling=node_y1->link_left_side;
			empty_sibling->link_right_side=child_empty_endmost;
			child_empty_endmost->link_left_side=empty_sibling;
			}
		else if((node->right_endmost!=node_y1)&&(node_y1->link_right_side->label==EMPTY))
			{
			if(node_y1->right_endmost->label==FULL)
				qnode_reversing(node_y1);
			empty_sibling=node_y1->link_right_side;
			empty_sibling->link_left_side=child_empty_endmost;
			child_empty_endmost->link_right_side=empty_sibling;
			}
		     else //node_y1 non ha sibling con label empty
			{
			if((node->left_endmost!=node_y1)&&(node_y1->link_left_side->label==PARTIAL))
				{
				if(node_y1->left_endmost->label==EMPTY)
					qnode_reversing(node_y1);
				}
			else if((node->right_endmost!=node_y1)&&(node_y1->link_right_side->label==PARTIAL))
				{
				if(node_y1->right_endmost->label==EMPTY)
					qnode_reversing(node_y1);
				}
			if(node->type==QNODE)
				if(node->left_endmost==node_y1)
					node->left_endmost=child_empty_endmost;
				else
					node->right_endmost=child_empty_endmost;
			else //if(node->type!=QNODE)
				if(node->left_endmost==node_y1)
					{
					child_empty_endmost->link_left_side=node_y1->link_left_side;
					node_y1->link_left_side->link_right_side=child_empty_endmost;
					}
				else
					{
					child_empty_endmost->link_right_side=node_y1->link_right_side;
					node_y1->link_right_side->link_left_side=child_empty_endmost;
					}
			}
		//child_empty_endmost->parent=node; //già messo sopra per disegnare il grafo
		//node->child_count++;
		/*if(node->full_children.size()==0)
			if(node->left_endmost==node_y1)
				node->left_endmost=child_full_endmost;
			else
				node->right_endmost=child_full_endmost;*/
		//child_full_endmost->parent=node; //già messo sopra per disegnare il grafo
		//node->child_count++;
		node->child_count=node->child_count+node_y1->child_count;
		if((node->partial_children.size()==2))
			{
			node_y2=node->partial_children.tail();
			temp_pq=node_y2->left_endmost; //questo è per disegnare il grafo da qui...
			blocked_list.remove(node_y2);
			count--;
			while(temp_pq!=NULL)
				{
				temp_pq->parent=node;
				if(temp_pq->label==FULL)
					node->full_children.append(temp_pq);
				blocked_list.append(temp_pq);
				count++;
				temp_pq=temp_pq->link_right_side;
				} //...a qui
			if(node_y2->left_endmost->label==FULL)
				{
				child_full_endmost=node_y2->left_endmost;
				child_empty_endmost=node_y2->right_endmost;
				temp_pq=child_full_endmost;
				/*while(temp_pq->label==FULL) //controllare
					{
					node->full_children.append(temp_pq);
					temp_pq=temp_pq->link_right_side;
					}*/ //già messo sopra per disegnare il grafo
				}
			else if(node_y2->right_endmost->label==FULL)
				{
				child_full_endmost=node_y2->right_endmost;
				child_empty_endmost=node_y2->left_endmost;
				temp_pq=child_full_endmost;
				/*while(temp_pq->label==FULL)
					{
					node->full_children.append(temp_pq);
					temp_pq=temp_pq->link_left_side;
					}*/ //già messo sopra per disegnare il grafo
				}
			if((node->left_endmost!=node_y2)&&(node_y2->link_left_side->label==FULL))
				{
				if(node_y2->left_endmost->label==EMPTY)
					qnode_reversing(node_y2);
				full_sibling=node_y2->link_left_side;
				full_sibling->link_right_side=child_full_endmost;
				child_full_endmost->link_left_side=full_sibling;
				}
			else if((node->right_endmost!=node_y2)&&(node_y2->link_right_side->label==FULL))
				{
				if(node_y2->right_endmost->label==EMPTY)
					qnode_reversing(node_y2);
				full_sibling=node_y2->link_right_side;
				full_sibling->link_left_side=child_full_endmost;
				child_full_endmost->link_right_side=full_sibling;
				}
			     //else //node_y2 non ha sibling con label full
				//{
				//temp_endmost2=child_full_endmost;
				/*if((node->right_endmost!=node_y2)&&(node_y2->link_right_side->label==PARTIAL))
					{
					temp_endmost->link_left_side=child_full_endmost;
					child_full_endmost->link_right_side=temp_endmost;
					}
				else
					{
					temp_endmost->link_right_side=child_full_endmost;
					child_full_endmost->link_left_side=temp_endmost;
					}*/
				//}
				/*{
				if(node->left_endmost==node_y2)
					node->left_endmost=child_full_endmost;
				else
					node->right_endmost=child_full_endmost;
				}*/
			if((node->left_endmost!=node_y2)&&(node_y2->link_left_side->label==EMPTY))
				{
				if(node_y2->left_endmost->label==FULL)
					qnode_reversing(node_y2);
				empty_sibling=node_y2->link_left_side;
				empty_sibling->link_right_side=child_empty_endmost;
				child_empty_endmost->link_left_side=empty_sibling;
				}
			else if((node->right_endmost!=node_y2)&&(node_y2->link_right_side->label==EMPTY))
				{
				if(node_y2->right_endmost->label==FULL)
					qnode_reversing(node_y2);
				empty_sibling=node_y2->link_right_side;
				empty_sibling->link_left_side=child_empty_endmost;
				child_empty_endmost->link_right_side=empty_sibling;
				}
			     else //node_y2 non ha sibling con label empty
				{
				if((node->left_endmost!=node_y2)&&(node_y2->link_left_side->label==PARTIAL))
					{
					if(node_y2->left_endmost->label==EMPTY)
						qnode_reversing(node_y2);
					}
				else if((node->right_endmost!=node_y2)&&(node_y2->link_right_side->label==PARTIAL))
					{
					if(node_y2->right_endmost->label==EMPTY)
						qnode_reversing(node_y2);
					}
				if(node->type==QNODE)
					if(node->left_endmost==node_y2)
						node->left_endmost=child_empty_endmost;
					else
						node->right_endmost=child_empty_endmost;
				else //if(node->type!=QNODE)
					if(node->left_endmost==node_y2)
						{
						child_empty_endmost->link_left_side=node_y2->link_left_side;
						node_y2->link_left_side->link_right_side=child_empty_endmost;
						}
					else
						{
						child_empty_endmost->link_right_side=node_y2->link_right_side;
						node_y2->link_right_side->link_left_side=child_empty_endmost;
						}
				}
			if((node->right_endmost!=node_y2)&&(node->right_endmost!=child_empty_endmost) &&(node_y2->link_right_side->label==PARTIAL))
				{
				temp_endmost->link_left_side=child_full_endmost;
				child_full_endmost->link_right_side=temp_endmost;
				}
			else if((node->left_endmost!=node_y2)&&(node->left_endmost!=child_empty_endmost) &&(node_y2->link_left_side->label==PARTIAL))
				{
				temp_endmost->link_right_side=child_full_endmost;
				child_full_endmost->link_left_side=temp_endmost;
				}
		//child_empty_endmost->parent=node; //già messo sopra per disegnare il grafo
		//node->child_count++;
		/*if(node->full_children.size()==0)
			if(node->left_endmost==node_y2)
				node->left_endmost=child_full_endmost;
			else
				node->right_endmost=child_full_endmost;*/
		//child_full_endmost->parent=node; //già messo sopra per disegnare il grafo
		//node->child_count++;
			node->child_count=node->child_count+node_y2->child_count;
			while(l!=2)
				{
				temp_pq=full_queue.pop();
				if((temp_pq==node_y1)||(temp_pq==node_y2))
					l++;
				else
					full_queue.append(temp_pq);
				}
			l=0;
			delete(node_y2);
			node->child_count--;
			number_of_nodes--;
			}
		if(node->partial_children.size()==1) //??
			{
			temp_pq=full_queue.pop();
			while(temp_pq!=node_y1)
				{
				full_queue.append(temp_pq);
				temp_pq=full_queue.pop();
				}
			}
		delete(node_y1);
		node->child_count--;
		number_of_nodes--;
		/*if(node->type==QNODE) //di questo secondo me non c'è bisogno perchè è il template_Q3
			{
			node->label=PARTIAL;
			if(!is_root(node))
				node->parent->partial_children.append(node);
			full_queue.append(node);
			}*/
		node->partial_children.clear();
		}
	if(node->type!=QNODE)
		{
		assign_parent_to_draw_graph(node,count); //da togliere serve per disegnare il grafo
		delete(node);
		}
	else
		{
		node->label=PARTIAL;//
		if(!is_root(node))//
			node->parent->partial_children.append(node);//
		full_queue.append(node);//
		node->pertinent_child_count=0;//??
		node->pertinent_leaf_count=0;//??
		node->mark=UNMARKED;
		}
	blocked_list.clear();
	//q3++;
	//stop10.pause();
	/*cout<<root->id<<endl;
	forall(temp_pq,root->full_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	forall(temp_pq,root->partial_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	temp_pq=root->partial_children.head()->link_right_side;
	while(temp_pq!=root->partial_children.head())
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_right_side;
		}
	cout<<endl;
	temp_pq=root->partial_children.head()->link_left_side;
	while(temp_pq!=root->partial_children.head())
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_left_side;
		}
	cout<<endl;
	forall(temp_pq,root->partial_children.head()->full_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	temp_pq=root->partial_children.head()->left_endmost;
	while(temp_pq!=NULL)
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_right_side;
		}
	cout<<endl;
	temp_pq=root->partial_children.head()->right_endmost;
	while(temp_pq!=NULL)
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_left_side;
		}
	cout<<endl;
	cout<<root->child_count<<endl;
	cout<<root->partial_children.head()->child_count<<endl;*/
	return true;
	}
	
	
	
	template <class T>
	bool
	gdt::PQ_tree<T>::
make_empty
	()
	{
	PQ_node temp_node,current_parent;
	gdt::gdtqueue<PQ_node> queue_to_make_empty;
	gdt::gdtmap<int,int> queued_for_delete(0);
	forall(temp_node,leaves)
		queue_to_make_empty.append(temp_node);
	while(!(queue_to_make_empty.empty()))
		{
		temp_node=queue_to_make_empty.pop();
		current_parent=temp_node->parent;
		if(current_parent!=root)
			{
			if(temp_node->type!=PSEUDONODE)
				{
				queued_for_delete[current_parent->id]++;
				if(queued_for_delete[current_parent->id]==current_parent->child_count)
					queue_to_make_empty.append(current_parent);
				}
			}
		delete(temp_node);
		}
	leaves.clear();
	queue.clear();
	full_queue.clear();
	if(root!=NULL)
		{
		max_id=root->id;
		number_of_nodes=1;
		}
	else
		{
		max_id=-1;
		number_of_nodes=0;
		}
	return true;
	}
	
	
	template <class T>
	bool
	gdt::PQ_tree<T>::
reduce
	(
	gdt::gdtlist<T> &S
	)
	{
	T temp_i;
	PQ_node temp_pq,current_parent,temp_n;
	queue.clear();//??
	forall(temp_i,S)
		{
		temp_pq=find_leaf(temp_i);
		if(temp_pq==NULL)
			gdt_error("A leaf doesn't exist");
		queue.append(temp_pq);
		temp_pq->pertinent_leaf_count=1;
		}
	while(queue.size()>0)
		{
		temp_pq=queue.pop();
		if(temp_pq->pertinent_leaf_count<S.size())
			{//temp_pq is not root of the pertinent subtree
			current_parent=temp_pq->parent;
			current_parent->pertinent_leaf_count=current_parent->pertinent_leaf_count + temp_pq->pertinent_leaf_count;
			current_parent->pertinent_child_count=current_parent->pertinent_child_count-1;
			/*cout<<current_parent->id<<" "<<current_parent->pertinent_leaf_count<<" "<<current_parent->pertinent_child_count<<endl;*/
			//cout<<current_parent->id<<endl;
			//cout<<current_parent->pertinent_leaf_count<<" "<<current_parent->pertinent_child_count<<endl;
			if(current_parent->pertinent_child_count==0)//all children of current parent have been computed
				queue.append(current_parent);
			if(!template_L1(temp_pq))
				if(!template_P1(temp_pq))
					if(!template_P3(temp_pq))
						if(!template_P5(temp_pq))
							if(!template_Q1(temp_pq))
								if(!template_Q2(temp_pq))
									{
									make_empty();
									std::cout<<"Could not reduce {";
									forall(temp_i,S)
										if(temp_i!=S.tail())
											std::cout<<find_leaf(temp_i)->id<<",";
									std::cout<<S.tail()<<"}"<<std::endl;
									return false;
									}
			}
		else
			{//temp_pq is root of the pertinent subtree
			if(!template_L1(temp_pq))
				if(!template_P1(temp_pq))
					if(!template_P2(temp_pq))
						if(!template_P4(temp_pq))
							if(!template_P6(temp_pq))
								if(!template_Q1(temp_pq))
									if(!template_Q2(temp_pq))
										if(!template_Q3(temp_pq))
											{
											make_empty();
											std::cout<<"Could not reduce {";
											forall(temp_i,S)
											if(temp_i!=S.tail())
												std::cout<<find_leaf (temp_i)->id<<",";
											std::cout<<S.tail()<<"}"<<std::endl;
											return false;
											}
			}
		//temp_pq->pertinent_child_count=0;//??
		//temp_pq->pertinent_leaf_count=0;//??
		//temp_pq->mark=UNMARKED;
		/*if(temp_pq->parent!=NULL)
			cout<<temp_pq->parent->id<<" "<<temp_pq->parent->pertinent_child_count<<endl;*/
		}
	//cout<<endl;
	temp_pq=temp_pq->parent;
	while((temp_pq!=NULL)&&(temp_pq->mark!=UNMARKED))
		{
		temp_pq->mark=UNMARKED;
		temp_pq->pertinent_child_count=0;//??
		temp_pq->pertinent_leaf_count=0;
		temp_pq=temp_pq->parent;
		}
	while(!full_queue.empty())
		{
		temp_n=full_queue.pop();
		/*if(!is_root(temp_n))
			if(temp_n->label==FULL)
				temp_n->parent->full_children.remove(temp_n);
			else if(temp_n->label==PARTIAL)//ovviamente quindi togliere questo
				temp_n->parent->partial_children.remove(temp_n);*/
		temp_n->label=EMPTY;
		temp_n->full_children.clear();
		temp_n->partial_children.clear();
		}
	if(!is_root(temp_n))
		{
		temp_n->parent->full_children.clear();
		temp_n->parent->partial_children.clear();
		}
	/*PQ_node temp_n,temp_n1;
	cout<<"numero di nodi "<<number_of_nodes<<endl;
	cout<<"queue ";
	while(!queue.empty())
		cout<<queue.pop()->id<<" ";
	cout<<endl;
	cout<<"max_id "<<max_id<<endl;
	cout<<"leaves ";
	forall(temp_pq,leaves)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	cout<<"radice "<<root->id<<endl;
	cout<<"     child_count "<<root->child_count<<endl;
	cout<<"     full_children ";
	forall(temp_pq,root->full_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	cout<<"     label "<<root->label<<endl;
	cout<<"     pertinent_child_count "<<root->pertinent_child_count<<endl;
	cout<<"     pertinent_leaf_count "<<root->pertinent_leaf_count<<endl;
	cout<<"     type "<<root->type<<endl;
	cout<<endl;
	temp_n=root->full_children.head();
	cout<<"full_children "<<temp_n->id<<endl;
	cout<<"     child_count "<<temp_n->child_count<<endl;
	cout<<"     link_left_side "<<temp_n->link_left_side->id<<endl;
	cout<<"     link_right_side "<<temp_n->link_right_side->id<<endl;
	cout<<"     circular_link right ";
	temp_pq=temp_n->link_right_side;
	while(temp_pq!=temp_n)
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_right_side;
		}
	cout<<endl;
	cout<<"     circular_link left ";
	temp_pq=temp_n->link_left_side;
	while(temp_pq!=temp_n)
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_left_side;
		}
	cout<<endl;
	cout<<"     full_children ";
	forall(temp_pq,temp_n->full_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	cout<<"     label "<<temp_n->label<<endl;
	cout<<"     parent "<<temp_n->parent->id<<endl;
	cout<<"     pertinent_child_count "<<temp_n->pertinent_child_count<<endl;
	cout<<"     pertinent_leaf_count "<<temp_n->pertinent_leaf_count<<endl;
	cout<<"     type "<<temp_n->type<<endl;
	cout<<endl;
	temp_n1=temp_n->full_children.head();
	cout<<"full_children1 "<<temp_n1->id<<endl;
	cout<<"     child_count "<<temp_n1->child_count<<endl;
	cout<<"     link_left_side "<<temp_n1->link_left_side->id<<endl;
	cout<<"     link_right_side "<<temp_n1->link_right_side->id<<endl;
	cout<<"     circular_link right ";
	temp_pq=temp_n1->link_right_side;
	while(temp_pq!=temp_n1)
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_right_side;
		}
	cout<<endl;
	cout<<"     circular_link left ";
	temp_pq=temp_n1->link_left_side;
	while(temp_pq!=temp_n1)
		{
		cout<<temp_pq->id<<" ";
		temp_pq=temp_pq->link_left_side;
		}
	cout<<endl;
	cout<<"     full_children ";
	forall(temp_pq,temp_n1->full_children)
		cout<<temp_pq->id<<" ";
	cout<<endl;
	cout<<"     label "<<temp_n1->label<<endl;
	cout<<"     parent "<<temp_n1->parent->id<<endl;
	cout<<"     pertinent_child_count "<<temp_n1->pertinent_child_count<<endl;
	cout<<"     pertinent_leaf_count "<<temp_n1->pertinent_leaf_count<<endl;
	cout<<"     type "<<temp_n1->type<<endl;
	cout<<endl;*/
	return true;
	}
	
	
	
	template<class T>
	void
	gdt::PQ_tree<T>::
freeze
	(
	PQ_tree_freezed<T>& freezed_tree
	)
	{
	PQ_node temp_pq,current_node,prec_child,current_child;
	PQ_node_freezed freezed_child,freezed_node,temp_freezed;
	gdt::gdtmap<PQ_node,int> node_visited(0);
	gdt::gdtmap<PQ_node,PQ_node_struct_freezed<T>*> map_PQ;
	gdt::gdtmap<PQ_node_struct_freezed<T>*,PQ_node> map_PQfreezed;
	gdt::gdtqueue<PQ_node_freezed> freezed_queue;
	//All of the nodes have mark UNMARKED
	forall(temp_pq,leaves)
		{
		current_node=temp_pq;
		while((current_node!=NULL)&&(node_visited[current_node]==0))
			{
			freezed_node=new PQ_node_struct_freezed<T>;
			freezed_node->id=current_node->id;
			if(current_node->type!=LEAF)
				freezed_node->children_list.append(map_PQ[prec_child]);
			freezed_node->type=current_node->type;
			map_PQ[current_node]=freezed_node;
			map_PQfreezed[freezed_node]=current_node;
			if((current_node->parent!=NULL)&&(node_visited[current_node->parent]==1))
				map_PQ[current_node->parent]->children_list.append(freezed_node);
			freezed_tree.number_of_nodes++;
			if(freezed_node->type==LEAF)
				freezed_node->value=current_node->value;
			if(current_node==root)
				{
				freezed_tree.root=freezed_node;
				}
			node_visited[current_node]=1;
			prec_child=current_node;
			current_node=current_node->parent;
			}
		}
	freezed_queue.append(freezed_tree.root);
	while(freezed_queue.size()!=0)
		{
		freezed_node=freezed_queue.pop();
		forall(temp_freezed,freezed_node->children_list)
			{
			if(temp_freezed->type!=LEAF)
				freezed_queue.append(temp_freezed);
			}
		if(freezed_node->type==QNODE)
			{
			freezed_child=freezed_node->children_list.head();
			freezed_node->children_list.clear();
			current_child=map_PQfreezed[freezed_child];
			current_node=map_PQfreezed[freezed_node];
			while(!(current_child->is_endmost()))
				{
				current_child=current_child->link_left_side;//link_right_side anche va bene
				}
			if(current_child==current_node->right_endmost)
				while(current_child!=NULL)
					{
					freezed_node->children_list.append(map_PQ[current_child]);
					current_child=current_child->link_left_side;
					}
			else if(current_child==current_node->left_endmost)
				while(current_child!=NULL)
					{
					freezed_node->children_list.append(map_PQ[current_child]);
					current_child=current_child->link_right_side;
					}
			}
		}
	}
	
	
	template<class T>
	undi_graph
	gdt::PQ_tree<T>::
PQ_tree_into_undi_graph
	(
	std::string title
	)
	{
	int n;
	gdt::gdtqueue<PQ_node> pq_queue;
	undi_graph ug;
	gdtnode graph_node,temp_node,root_node;
	PQ_node temp,tree_node;
	//int queued_for_graph[this->number_of_nodes];
	gdt::gdtmap<int,int> queued_for_graph(0);
	gdt::gdtnode_map<std::string> labels(ug);
	gdt::gdtnode_map<PQ_node> parents(ug);
	
	forall(temp,this->leaves)
		pq_queue.append(temp);
	if(number_of_nodes!=1)
		while(!pq_queue.empty())
			{
			tree_node=pq_queue.pop();
			//cout<<tree_node->id<<" "<<tree_node->child_count<<endl;
			graph_node=ug.new_node();
			if(tree_node==root)
				root_node=graph_node;
			n=tree_node->id; //n=tree_node->get_id();
			if(tree_node->type==LEAF) //(tree_node->get_type()==LEAF)
				labels[graph_node]="L"+gdt_itoa(n);
			else if(tree_node->type==PNODE) //(tree_node->get_type()==PNODE)
				labels[graph_node]="P"+gdt_itoa(n);
			else labels[graph_node]="Q"+gdt_itoa(n);
			parents[graph_node]=tree_node->parent;
			if(tree_node->parent!=NULL)
				queued_for_graph[tree_node->parent->id]++;
			if((tree_node->parent!=NULL)&&(queued_for_graph[tree_node->parent->id]==tree_node->parent->child_count))
				pq_queue.append(tree_node->parent);
			if(tree_node->child_count!=0)
				forall_nodes(temp_node,ug)
					if(parents[temp_node]!=NULL)
						if(parents[temp_node]->id==tree_node->id)
							ug.new_edge(temp_node,graph_node);
			}
	else
		{
		graph_node=ug.new_node();
		n=root->id;
		labels[graph_node]="P"+gdt_itoa(n);
		temp_node=ug.new_node();
		labels[temp_node]="0";
		ug.new_edge(temp_node,graph_node);
		}
		tree albero(ug,root_node);
	draw_undi_graph dug(ug);
	forall_nodes(temp_node,dug)
		{
		graph_node=ug.find_node(dug.id(temp_node));
		dug.set_label(temp_node,labels[graph_node]);
		}
	dug.write(title+".gdt");
	draw_undi_graph dug_tree(albero);
	forall_nodes(temp_node,dug_tree)
		{
		graph_node=ug.find_node(dug_tree.id(temp_node));
		dug_tree.set_label(temp_node,labels[graph_node]);
		}
	title=title+"_tree.gdt";
	dug_tree.write(title);
	return ug;
	}
	
	
	
	
	
	
	template<class T>
	gdt::PQ_node_struct_freezed<T>::
PQ_node_struct_freezed
	()
	{
	id=0;
	type=NON_VALID;
	//punt_value=NULL;
	}
	
	
	template<class T>
	gdt::PQ_node_struct_freezed<T>::
~PQ_node_struct_freezed
	()
	{
	children_list.clear();
	}
	
	
	
	template<class T>
	int
	gdt::PQ_node_struct_freezed<T>::
get_id
	()
	{
	return id;
	}
	
	
	
	template<class T>
	T
	gdt::PQ_node_struct_freezed<T>::
get_value
	()
	{
	return value;
	}
	
	
	
	
	
	
	template<class T>
	gdt::PQ_tree_freezed<T>::
PQ_tree_freezed
	()
	{
	root=NULL;
	//max_id=-1;
	number_of_nodes=0;
	}
	
	
	
	template<class T>
	gdt::PQ_tree_freezed<T>::
~PQ_tree_freezed 
	()
	{
	delete(root);
	frontier.clear();
	}
	
	
	
	template<class T>
	gdt::gdtlist<T>
	gdt::PQ_tree_freezed<T>::
tree_search
	()
	{
	PQ_node_freezed freezed_node,freezed_child;
	gdt::gdtqueue<PQ_node_freezed> search_queue;
	gdt::gdtlist<T> search_list;
	search_queue.append(root);
	while(search_queue.size()!=0)
		{
		freezed_node=search_queue.pop();
		forall(freezed_child,freezed_node->children_list)
			search_queue.append(freezed_child);
		search_list.append(freezed_node->id);
		}
	return search_list;
	}
	
	
	
	template<class T>
	void
	gdt::PQ_tree_freezed<T>::
sorted_leaves_list
	(
	gdt::gdtlist<gdt::PQ_node_struct_freezed<T>*> node_list
	)
	{
	PQ_node_freezed temp_node;
	forall(temp_node,node_list)
		if(temp_node->type==LEAF)
			frontier.append(temp_node);
		else
			sorted_leaves_list(temp_node->children_list);
	}
	
	
	
	template<class T>
	void
	gdt::PQ_tree_freezed<T>::
tree_frontier
	()
	{
	gdt::gdtlist<gdt::PQ_node_struct_freezed<T>*> node_list;
	node_list.append(root);
	sorted_leaves_list(node_list);
	}
	
	
	
	template<class T>
	gdt::gdtlist<gdt::PQ_node_struct_freezed<T>*>
	gdt::PQ_tree_freezed<T>::
get_frontier
	()
	{
	return frontier;
	}


#endif	
