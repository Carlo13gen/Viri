/*! \file */
#ifndef __ptr__

#define __ptr__


inline void* operator new(size_t, void* where, int) { return where; }   //GDT


inline void operator delete(void*, size_t) {return;};
inline void operator delete(void*, size_t, int) {return;};
inline void operator delete(void*, void*, int) {return;};
namespace gdt {


	typedef void* Ptr;

	//Copia il contenuto di un oggetto di dimensioni minori di un puntatore
	//in un puntatore (Ptr)

	//inline void* operator new(size_t s) { return ::operator new(s); }

    //inline void* operator new(size_t, void* where) { return where; }        //PJ

//	#define COPY_INTO_PTR(T,x,p) (new((void*)(&p),0) T(x))
	#define COPY_INTO_PTR(T,x,p) (new((void*)(&p),0) T(x))

	//Copia un oggetto x in modo diverso a seconda della sua dimensione
	template <class T>
	inline Ptr ptr_copy(T x) {
		Ptr p = 0;
		if (sizeof(T) <= sizeof(Ptr))
			COPY_INTO_PTR(T,x,p);
		else
			p = (Ptr) new T(x);
		return p;
	}

	//Accede ad un Ptr
	#define PTR_ACCESS(T,p) ptr_access((T*)0, p)

	template <class T>
	inline T& ptr_access(T*, Ptr& p) {
		T* t;
		if (sizeof(T) <= sizeof(Ptr))
			t = (T*)&p;
		else
			t = (T*)p;
		return *t;
	}

	#define PTR_DEFAULT_INIT(T,p) ptr_default_init((T*)0,p)

	template <class T>
	inline void ptr_default_init(T*, Ptr& p) {
		if (sizeof(T) > sizeof(Ptr))
			p = ((void*) new T());
	}

	#define PTR_CONST_ACCESS(T,p) ptr_const_access((T*)0, p)

	template <class T>
	inline const T& ptr_const_access(T*, const Ptr& p) {
		const T* t;
		if (sizeof(T) <= sizeof(Ptr))
			t = (const T*)&p;
		else
			t = (const T*)p;
		return *t;
	}

	#define PTR_DELETE(T,p) ptr_delete((T*)0, p)

	template <class T>
	inline void ptr_delete(T*, Ptr p) {
		if (sizeof(T) > sizeof(Ptr))
			delete ((T*) p);
		//else do nothing
	}
}

#endif
