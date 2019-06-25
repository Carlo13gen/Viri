/*! \file */

#ifndef __gdtbinary_heap2__
#define __gdtbinary_heap2__

#include <assert.h>
#include "ptr.h"
#include "gdtlist.h"
#include <iostream>
#include <stdio.h>  // needed to avoid a preprocessing error while parsing the CHECK macro (titto 20/3/2016)
#include <vector>

#define CHECK(x) if (!(x)) {printf(#x" returned false\n"); return false;};

namespace gdt {

	template <class T>
	struct stdless {
		bool operator () (const T& x, const T& y) {
			return x < y;
		}
	};

	typedef list_item heap_item;

	template<class P, class I, class less = stdless<P> >
	class gdtbinary_heap2 {

		private:

			typedef unsigned long Index;			
			typedef unsigned long Size;
			Ptr buffer;
			
			struct Data {
				Index heap_index;
				P prio;
				Ptr ptr;

				Data() {}

				Data(Index _heap_index, P _prio, Ptr _ptr) : 
					heap_index(_heap_index), prio(_prio), ptr(_ptr) {}		
			};

			//list_item *heap;
			std::vector <list_item> heap;
			gdtlist<Data> data;

			Size heap_size;
			Size length;

			//*****************************************************
			// Utility 
			//*****************************************************

			inline bool less_op(list_item x, list_item y) const {
				less op;
				return op(data.inf(x).prio, data.inf(y).prio);
			}			

			//***************************************************
			// Gestione memoria
			//***************************************************

			void init(Size new_size) {
				assert(heap.size()==0 && data.empty());
				//heap = new list_item[new_size];
				heap= std::vector<list_item>(new_size);
				length = new_size;
			}

			void double_table() {
				assert(heap.size()>0 && !data.empty() && length >= 1);
				length = length * 2 + 1;
				heap.resize(length * sizeof(list_item));
				//heap = (list_item*) realloc(heap, length * sizeof(list_item));
				assert(heap.size()!=0);
			}			

			void half_table() {
				assert(heap.size()>0 && !data.empty() && length >= 15);
				length = ((length + 1) / 2) - 1;
				assert(heap_size < length);
				heap.resize(length * sizeof(list_item));
				//heap = (list_item*) realloc(heap, length * sizeof(list_item));
				assert(heap.size()!=0);
			}

			inline void swap(Index x, Index y) {
				assert(consistent_list());
				list_item aux = heap[x];
				heap[x] = heap[y];
				heap[y] = aux;
				data[heap[x]].heap_index = x;
				data[heap[y]].heap_index = y;								
				assert(consistent_list());
			}

			inline void assign_buffer(list_item x) {
				if (buffer) PTR_DELETE(I, buffer);
				buffer = data.inf(x).ptr;
			}		

			//****************************************************
			// Gestione dello heap
			//****************************************************

			inline void heapify(Index i) {
				Index l = left_child(i);
				Index r = right_child(i);
				Index largest;
				if (l < heap_size && less_op(heap[i], heap[l])) 
					largest = l;
				else 
					largest = i;
				if (r < heap_size && less_op(heap[largest], heap[r]))
					largest = r;
				if (largest != i) {
					swap(largest, i);
					heapify(largest); //Togliere la ricorsione
				}				
			}

			inline Index parent(Index i) const {				
				return (i + 1) / 2 - 1;
			}

			inline Index left_child(Index i) const {
				return i*2 + 1;
			}

			inline Index right_child(Index i) const {
				return i*2 + 2;
			}

			inline Index last() const {
				return heap_size - 1;
			}	

		public:

			const P& prio(list_item i) const {
				assert(i);
				return data[i].prio;
			}

			const I& inf(list_item i) const {
				assert(i);
				return PTR_CONST_ACCESS(I, data[i].ptr);				
			}

			gdtbinary_heap2() : buffer(NULL), heap_size(0) {
				init(1);
			}
			
			~gdtbinary_heap2() {
				heap.clear();
				//free(heap);
			}

			void increase_key(heap_item it, const P& p) {
				assert(it);
				less op;
				assert(op (prio(it),p));
				data[it].prio = p;				
				Index i = data[it].heap_index;
				while (i > 0 && op (prio(heap[parent(i)]), prio(heap[i]))) {
					Index p = parent(i);
					swap(i, p);
					i = p;
				}
				heapify(i);
				assert(consistency_check());
			}

			//
			list_item insert(const P& pr, const I& info) {

				if (heap_size == length) double_table();
				++heap_size;
				Index i = last();
				less op;
				while (i > 0 && ( op( prio( heap[parent(i)] ), pr ) ) ) {
					heap[i] = heap[parent(i)];
					data[heap[i]].heap_index = i;
					i = parent(i);
				}					
				list_item it = data.append(Data(i, pr, ptr_copy(info)));
				heap[i] = it;

				assert(consistency_check());

				return it;
			}		

			void remove(list_item it) {
				assert(consistency_check());
				//Erase item and assign to buffer
				assert(heap_size > 0);
				Index i = data.inf(it).heap_index;
				assert(i < heap_size && i >= 0);
				data.erase(it);

				//
				if (i == last()) {
					--heap_size;
				}
				else { //i != last()
					while (i != 0) {
						Index p = parent(i);
						heap[i] = heap[p];
						assert(data[heap[i]].heap_index == p);
						data[heap[i]].heap_index = i;
						assert(data[heap[i]].heap_index == i);
						i = p;
					}
					heap[0] = heap[heap_size-1];
					data[heap[0]].heap_index = 0;
					--heap_size;					
					heapify(0);					
				}		
				
				if (length >= 15 && heap_size == (length + 1) / 4) half_table();
				assert(consistency_check());				
			}

			//Restituisce un riferimento ad un buffer
			const I& extract_max() {
				assert(heap_size > 0);
				assign_buffer(heap[0]);
				data.erase(heap[0]);
				
				if (heap_size>1) {
					heap[0] = heap[heap_size-1];
					data[heap[0]].heap_index = 0;
				}	
				--heap_size;		
				heapify(0);

				assert(consistency_check());
				if (length >= 15 && heap_size == (length + 1) / 4) half_table();
				assert(consistency_check());
				return PTR_CONST_ACCESS(I, buffer);
			}

			const I& get_max() const {
				assert(heap_size > 0);
				return PTR_CONST_ACCESS(I, heap[0]);
			}		
			
			heap_item find_max() const {				
				return heap_size > 0 ? heap[0] : NULL;
			}

			Size size() const {
				return heap_size;
			}

			//****************************************************
			//* Debug
			//****************************************************			

			bool consistency_check() const {				
				CHECK(heap_size <= length);
				CHECK(heap_size == (unsigned long) data.size());
				CHECK(is_heap(0));	
				CHECK(consistent_list());
				return true;
			}

			bool consistent_list() const {				
				list_item it;
				forall_items(it, data) {
					if (heap[data.inf(it).heap_index] != it) return false;
				}
				return true;
			}

			bool is_heap(Index i) const {
				less op;
				bool res = true;
				if (right_child(i) < heap_size) {
					res = res && 
							!(op (prio(heap[i]), prio(heap[right_child(i)]))) &&	
							is_heap(right_child(i));
				}
				if (res && (left_child(i) < heap_size) ) {
					res = res && 
							!(op (prio(heap[i]), prio(heap[left_child(i)]))) &&
							is_heap(left_child(i));
				}				
				return res;
			}

			void show_list() const {
				std::cout << "List contents " << std::endl;
				list_item it = data.first();
				while (it) {	
					std::cout << "("<< PTR_CONST_ACCESS(I, data[it].ptr)<< ",i" << data[it].heap_index <<")" << " --> ";
					it = data.succ(it);
				}
				std::cout <<"NULL\n";
			}

			void show_heap() const {
				std::cout << "Heap contents " << std::endl;
				if (heap_size == 0) std::cout << "empty" << std::endl;
				for (int i = 0; i < heap_size; ++i) {					
					std::cout << PTR_CONST_ACCESS(I, data[heap[i]].ptr) << " ";
				}
				std::cout << "\n";
			}

			void show() const {
				show_list();				
				show_heap();			
			}

			/*********************
			 *                   *
			 * Iteration methods *
			 *                   * 
			 *********************/

			typedef heap_item item;

			heap_item first_item() const { return data.first(); }
			heap_item last_item() const { return data.last(); }
			heap_item next_item(heap_item p) const { return data.next_item(p); }
			heap_item pred_item(heap_item p) const { return data.pred_item(p); }

	};
}

#endif
