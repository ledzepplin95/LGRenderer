#include "StdAfx.h"

#include "LGMemoryAllocator.h"
#include "LGObject.h"
#include "LGMemoryPool.h"
#include "TriangleMesh.h"

static LGMemoryPool<1024*1024*50> lg_mem_pool;
static vector<LGObject*> ps;
static vector<TriangleMesh*> tris;

void* LGMemoryAllocator::lg_malloc(size_t t)
{
	return lg_mem_pool.mem_malloc(t);
}

void LGMemoryAllocator::lg_mem_reset()
{
	for(int i=0; i<ps.size(); i++)
	{
		delete ps[i];
	}
	ps.clear();
	for(int i=0; i<tris.size(); i++)
	{
		tris[i]->~TriangleMesh();
	}
	tris.clear();

	lg_mem_pool.mem_reset();
}

void LGMemoryAllocator::lg_mem_pointer(LGObject*p)
{
	ps.push_back(p);
}

void LGMemoryAllocator::lg_mem_triangle_pointer(TriangleMesh*p)
{
	tris.push_back(p);
}