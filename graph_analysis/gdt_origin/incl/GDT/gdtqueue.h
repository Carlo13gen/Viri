/*! \file */

#ifndef __gdtqueue__
#define __gdtqueue__

#include "gdtlist.h"

namespace gdt {

	template<class E>
	class gdtqueue : private gdtlist<E> {

	private:

		typedef gdtlist<E> base;

	public:

		gdtqueue() {}

		gdtqueue(const gdtqueue<E>& q) : base(q) {}

		~gdtqueue() { clear(); }

		gdtqueue<E>& operator=(const gdtqueue<E>& q) 
		{ base::operator=(q); return *this; }

		const E& top() const { return base::head();	}

		const E& pop() { return base::pop(); }

		void append(const E& x) { base::append(x); }

		int  size() const { return base::size(); }

		bool  empty() const { return base::empty(); }

		void clear() { base::clear(); }
	};
}

#endif
