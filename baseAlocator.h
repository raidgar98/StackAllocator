#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include <map>

class baseAlocator
{
private:

	inline static bool isExist(void * ptr) noexcept;

protected:
	
	static std::map<std::shared_ptr<void*>, size_t> _allocatedStuff;

public:

	baseAlocator(const baseAlocator&) = delete;
	baseAlocator(baseAlocator&&) = delete;

	///<summary>Standard (default) constructor</summary>
	///<param> This function doesn't have params</param>
	///<returns> This function doesn't return anything</returns>
	explicit baseAlocator() = default;

	///<summary>Standard (default) destructor</summary>
	///<param> This function doesn't have params</param>
	///<returns> This function doesn't return anything</returns>
	~baseAlocator() = default;

	/*
	///<summary>Standard (pure) virtual destructor, because of this class will be inherted</summary>
	///<param> This function doesn't have params</param>
	///<returns> This function doesn't return anything</returns>
	virtual ~baseAlocator();
	*/

	///<summary>This function allocate <i>bytesToAllocate</i> bytes, and returns pointer to this</summary>
	///<param name="bytesToAllocate">Amount od bytes user want to allocate</param>
	///<returns type="void*">Pointer to allocated memory</returns>
	///<example>void* ptr = alloc(24);</example>
	void* alloc(const size_t bytesToAllocate);

	///<summary>This is upgraded version of <i>alloc()</i> function created in standards of C++ 11 </summary>
	///<param name="bytesToAllocate" type="const size_t">Amount od bytes user want to allocate</param>
	///<returns type="std::shared_ptr[void*]">Inteligent pointer to allocated memory</returns>
	///<example>std::shared_ptr[void*] = sAlloc(24);</example>
	std::shared_ptr<void*> sAlloc(const size_t bytesToAllocate);

	///<summary>Free allocated space</summary>
	///<param name="ptr" type="void*">Pointer to allocated memory</param>
	///<returns>This function doesn't return anythig</returns>
	///<example id="1">dealloc(myPtr, 24);</example>
	///<example id="2">dealloc(memAllocatedByThisAllocator);</example>
	void dealloc(void * ptr);

	///<summary>Upgraded 'dealloc' function, compatibile with C++ 11 or higher</summary>
	///<param name="ptr" type="void*">Pointer to allocated memory</param>
	///<returns>This function doesn't return anythig</returns>
	///<example id="1">dealloc(myPtr, 24);</example>
	///<example id="2">dealloc(memAllocatedByThisAllocator);</example>
	void dealloc(std::shared_ptr<void*> ptr) noexcept;

	///<summary>Returns amount of space allocated by this allocator</summary>
	///<param>No parameters</param>
	///<returns type="size_t">Size of actually allocated memory</returns>
	size_t allocatedSize() const;

	inline static size_t getSize(void * ptr);
};