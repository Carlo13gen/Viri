/*! \file */

#ifndef __gdtstack__
#define __gdtstack__

#include <GDT/gdtlist.h>

namespace gdt {

template<class E>
class gdtstack : private gdtlist<E> {

	private:

		typedef gdtlist<E> base;

	public:

		gdtstack() {}

		gdtstack(const gdtstack<E>& s) : base(s) {}
		
		~gdtstack() { clear(); }

		gdtstack<E>& operator = (const gdtstack<E>& s) 
		{ base::operator=(s); return *this; }

		const E& top() const { return base::head(); }

		void push(const E& x) { base::push(x); }

		const E& pop() { return base::pop(); }

		int size() const { return base::length(); }

		bool empty() const { return base::empty(); }

		void clear() { base::clear(); }
};

}

#endif


