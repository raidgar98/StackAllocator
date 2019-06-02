#include "baseAlocator.h"
#include <algorithm>

std::map<std::shared_ptr<void*>, size_t> baseAlocator::_allocatedStuff = std::map<std::shared_ptr<void*>, size_t>();

inline bool baseAlocator::isExist(void * ptr) noexcept
{
	for (auto var : _allocatedStuff)
		if (*(var.first) == ptr) return true;

	return false;
}

void * baseAlocator::alloc(const size_t bytesToAllocate)
{
	void* ptr = nullptr;

	try
	{
		ptr = malloc(bytesToAllocate);
	}
	catch (std::bad_alloc& e)
	{ throw e; }

	_allocatedStuff[std::make_shared<void*>(ptr)] = bytesToAllocate;
	return ptr;
}

std::shared_ptr<void*> baseAlocator::sAlloc(const size_t bytesToAllocate)
{
	void* ptr = nullptr;

	try
	{
		ptr = alloc(bytesToAllocate);
	}
	catch (std::bad_alloc& e)
	{ throw e; }

	return std::make_shared<void*>(ptr);
}

void baseAlocator::dealloc(void * ptr)
{	
	if (isExist(ptr))
		_allocatedStuff.erase(std::make_shared<void*>(ptr));
	else 
		free(ptr);
				
	ptr = nullptr;
}

void baseAlocator::dealloc(std::shared_ptr<void*> ptr) noexcept
{
	if (isExist(*ptr))
	{
		_allocatedStuff.erase(ptr);
	}

	void * temp = *ptr;
	(*ptr) = nullptr;
	free(temp);
	temp = nullptr;
}

size_t baseAlocator::allocatedSize() const
{
	size_t sum = 0;
	for (auto var : _allocatedStuff)
		sum += var.second;
	return sum;
}
inline size_t baseAlocator::getSize(void * ptr)
{
	if (!isExist(ptr))
	{
		return _allocatedStuff.find(std::make_shared<void*>(ptr))->second;
	}
	else
	{
		try
		{
			throw std::invalid_argument("Not found");
		}
		catch (...) { throw; }
	}
}