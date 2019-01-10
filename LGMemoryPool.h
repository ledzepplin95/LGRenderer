#pragma once

template<int grow_size>
class LGMemoryPool
{
	struct big_buf
	{
		big_buf*m_next;
		char m_mem[4];
	};
private:
	big_buf* m_big_buf;
	char*    m_buf;
	int      m_pos;

public:
	LGMemoryPool()
	{
		m_big_buf=NULL;
		m_buf=NULL;
		m_pos=0;
	}
	void*mem_malloc(size_t size)
	{
		if(size>grow_size)throw;
		if(m_buf==NULL || m_pos+size>grow_size)new_buf();
		char*buf=&m_buf[m_pos];
		m_pos+=size;
		return buf;
	}
	void mem_free(void*p)
	{
	}

	~LGMemoryPool()
	{
		big_buf*buf=m_big_buf;
		while(buf)
		{
			big_buf*tmp=buf;
			buf=buf->m_next;
			free(tmp);
		}
	}

	void mem_reset()
	{
		big_buf*buf=m_big_buf;
		int i=0;
		while(buf)
		{
			big_buf*tmp=buf;
			buf=buf->m_next;
			if(i>0)free(tmp);
			++i;
		}
		if(m_big_buf)
		{
			m_big_buf->m_next=0;
			m_buf=m_big_buf->m_mem;
		}
		m_pos=0;
	}

private:
	void new_buf()
	{
		big_buf*buf=(big_buf*)malloc(sizeof(big_buf)+grow_size-4);
		if(!buf)throw;
		m_buf=buf->m_mem;
		buf->m_next=m_big_buf;
		m_big_buf=buf;
		m_pos=0;
	}	
};
