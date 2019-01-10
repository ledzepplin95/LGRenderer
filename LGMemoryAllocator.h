#pragma once

class LGObject;
class TriangleMesh;
class LGMemoryAllocator
{
public:	
	static void* lg_malloc(size_t size);	
	static void lg_mem_pointer(LGObject*p);
	static void lg_mem_triangle_pointer(TriangleMesh*p);
	static void lg_mem_reset();
};
