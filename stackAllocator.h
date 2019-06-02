#pragma once
#include "baseAlocator.h"
#include <limits>
#include <stack>

using lluint = long long unsigned int;

template<typename T>
class stackAllocator :
	private  baseAlocator
{
private:

	template<typename U> friend class stackAllocator;
	baseAlocator* that;// = this;

	///<summary name="__stackSize" type="lluint" permissions="private">Actual stack size</summary>
	lluint __stackSize = 0;

	///<summary name="__begin" type="void*" permissions="private"> Holds ( First - 1 ) position of whole stack</summary>
	void * __begin = nullptr;

	///<summary name="__actualPos" type="std::shared_ptr[char*]" permissions="private"> Holds position for next allocation</summary>
	char* __actualPos = nullptr;

	///<summary name="__mamStack" type="std::stack[std::pair[std::shared_ptr[void*], lluint]]" permissions="private"> Keeps order of allocation with information about width of allocated memory</summary>
	std::stack<std::pair<std::shared_ptr<void*>, lluint>> __memStack;

	///<summary name="__prepared" type="bool" permissions="private"> Inform is everythink ok (<b>__begin</b> is set?)</summary>
	bool __prepared = false;

public:

	///<summary permissions="public">Basic constructor</summary>
	///<param name="iStackSize" type="lluint" default="1 GB">Size (in bytes) to pre-reserve for stack</param>
	///<returns>Returning itself</returns>
    explicit stackAllocator(const lluint iStackSize = std::numeric_limits<size_t>().max() / 4) noexcept;

	///<summary permissions="public">Copy constructor require by stl</summary>
	///<param name="src" type="stackAllocator[U]"> Source for constructor</param>
	///<returns>Returning itself</returns>
	template<typename U>
	stackAllocator(const stackAllocator<U>& src) noexcept
		:baseAlocator{}, that{ const_cast<stackAllocator<U>*>(&src) } {}

	///<summary permissions="public">STL requirement</summary>
	template<typename U>
	stackAllocator(stackAllocator<U>&&) = delete;
	
	/*
	template<typename T>
	stackAllocator(stackAllocator<T>&&) = delete;
	*/

	///<summary permissions="public">Destructor - take care for everythink. After using, use <i>try_again</i> function, because this one will destroy everythink</summary>
	~stackAllocator() noexcept
	{

		while (__memStack.size() != 0)
			__memStack.pop();

		__actualPos = nullptr;
		void* ptr = __begin;
		__begin = nullptr;
		that->dealloc(ptr);
		__prepared = false;
		__stackSize = 0;
	}

	///<summary permissions="public">Function for allocation</summary>
	///<param name="amountOfObjects" type="lluint" default="1">Amount of objects (for arrays) to allocate. By default single objects.</param>
	///<returns type="T*">Pointer to allocated memory space</returns>
	T* allocate(const lluint amountOfObjects = 1)
	{
		if (!__prepared)
			try { std::invalid_argument("Allocator not initialized correctly!"); }
		catch (...) { throw; }

		lluint totSize = sizeof(T) * amountOfObjects;
		if (totSize > freeSpace())
		{
			try
			{
				throw std::invalid_argument("Too many object!");
			}
			catch (std::invalid_argument& e)
			{
				throw;
			}
		}
		void* ptrToReturn = (void*)__actualPos;
		__memStack.push(std::pair<std::shared_ptr<void*>, lluint>(std::make_shared<void*>(ptrToReturn), totSize));
		__actualPos += totSize;
		return (T*)ptrToReturn;
	}

	///<summary permissions="public">Deallocate latest if user use correct pointer</summary>
	///<param name="src" type="T*" default="nullptr"> Pointer to check and release. On nullptr release latest (very unsafe).</param>
	///<returns>Nothing.</returns>
	void deallocate(T* src = nullptr)
	{
		if (!__prepared)
			try { std::invalid_argument("Allocator not initialized correctly!"); }
		catch (...) { throw; }

		void * ptrToDealloc = (void*)src;
		if ( ptrToDealloc == *(__memStack.top().first) || src == nullptr)
		{
			*__memStack.top().first = nullptr;
			__actualPos -= __memStack.top().second;
			__memStack.top().second = 0;
			__memStack.pop();
		}
		else
		{
			try
			{
				throw std::invalid_argument("This pointer is not latest allocated object. Cannot deallocate.");
			}
			catch (std::invalid_argument& e) { throw; }
		}
		
	}

	///<summary permissions="public">Returns actual state of object</summary>
	///<returns type="bool">Only if true allocate, and deallocate function will work</returns>
	bool ready() const noexcept { return __prepared; }

	///<summary permissions="public">Informs about actual avaiable freeSpace</summary>
	///<param name="inBytes" type="bool" default="true">Define is return type should be in bytes or amount of objects 'T'</param>
	///<returns type="lluint">Amount of avaiable space</returns>
	lluint freeSpace(const bool inBytes = true) const
	{
		if (!__prepared)
			try { std::invalid_argument("Allocator not initialized correctly!"); }
		catch (...) { throw; }

		lluint sum = 0;

		auto temp{ __memStack };
		while (temp.size() != 0)
		{
			sum += temp.top().second;
			temp.pop();
		}

		return (__stackSize - sum)/ (inBytes ? 1 : sizeof(T));
	}

	///<summary permissions="public">Function for another try if stack doesn't allocate correctly</summary>
	///<param name="iStackSize" type="lluint" default="1 GB">Size (in bytes) to pre-reserve for stack</param>
	///<returns type="bool">Resignation from exception, for better (Eg. in loops) searching for optimal stack size</returns>
	bool try_again(const lluint iStackSize) const noexcept;

	typedef T value_type;
};

template<typename T>
stackAllocator<T>::stackAllocator(const lluint iStackSize) noexcept
	:that(this)
{
	void* temp = nullptr;

	try
	{
		temp = that->alloc(iStackSize + 1);
	}
	catch (...) { return; }

	__prepared = temp != nullptr ? true : false;

	if (__prepared)
	{
		__begin = temp;
		__actualPos = ((char*)temp)+1;
		temp = nullptr;
		__stackSize = iStackSize;
	}
}

template<typename T>
bool stackAllocator<T>::try_again(const lluint iStackSize) const noexcept
{
	if (__prepared) return false;

	void * temp = nullptr;

	try
	{
		temp = that->alloc(iStackSize + 1);
	}
	catch (std::invalid_argument& e) { return false; }

	__prepared = true;
	__begin = temp;
	__actualPos = ((char*)temp) + 1;
	temp = nullptr;
	__stackSize = iStackSize;
}
