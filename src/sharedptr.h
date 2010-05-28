#ifndef _SHAREDPTR_H_
#define _SHAREDPTR_H_

#include <assert.h>

template<class T>
class SharedPtr {
	public:
	T* value;
	int* ref;
	void releaseRef() {
		--(*ref);
		if(*ref < 1) {
			delete ref;
			delete value;
		}
	}
	public:
		SharedPtr():ref(new int),value(0) {
			*ref = 1;
		}
		SharedPtr(T* src):ref(new int),value(src) {
			*ref = 1;
		}
		SharedPtr(SharedPtr<T>& src):ref(src.ref),value(src.value) {
			++(*ref);
		}
		void operator =(T* src) {
			releaseRef();
			value = src;
			ref = new int;
			*ref = 1;
		}
		void operator =(const SharedPtr<T>& src) {
			if(this != &src) {
				releaseRef();
				ref = src.ref;
				value = src.value;
				++(*ref);
			}
		}
		T* operator ->()
		{
			assert(value != 0);
			return value;
		}
		T& operator *()
		{
			assert(value != 0);
			return *value;
		}
		~SharedPtr() {
			releaseRef();
		}
};


#endif