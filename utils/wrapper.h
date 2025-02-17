// wrapper.h

#ifndef WRAPPER_H
#define WRAPPER_H

template<class T>
class Wrapper
{
public:
    Wrapper() 
	: DataPtr(nullptr)
	{}

    Wrapper(const T& inner) 
	: DataPtr(inner.clone())
	{}
	
	Wrapper(const T* iptr)
	: DataPtr(iptr->clone())
	{}
	
    ~Wrapper()
    {
        if (DataPtr != nullptr)
            delete DataPtr;
    }
    
    Wrapper(const Wrapper<T>& original)
	: DataPtr(nullptr)
    {
        if (original.DataPtr!=nullptr)
            DataPtr = original.DataPtr->clone();
    }
    
    Wrapper& operator=(const Wrapper<T>& original)
    {
        if (this != &original)
        {
            if (DataPtr != nullptr)
                delete DataPtr;
            
            DataPtr = (original.DataPtr != nullptr) ?
                original.DataPtr->clone() : nullptr;
        }
        
        return *this;
    }
    
    T& operator*() { return *DataPtr; }
    const T& operator*() const { return *DataPtr; }
    T* operator->() { return DataPtr; }
    const T* operator->() const { return DataPtr; }

private:
        T* DataPtr;
};

#endif