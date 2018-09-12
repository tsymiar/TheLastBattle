#pragma once

#include <vector>
template<typename T>
class ref_ptr
{
public:
	ref_ptr():m_ptr(NULL){}
	ref_ptr(T *ptr):m_ptr(ptr)
	{
		add_ref();
	}
	ref_ptr(const ref_ptr<T>& ref):m_ptr(ref.m_ptr)
	{
		add_ref();
	}
	virtual ~ref_ptr()
	{
		remove_ref();
	}

	ref_ptr<T>& operator = (const ref_ptr<T>& ref)
	{
		if(this == &ref) return *this;
		remove_ref();
		m_ptr = ref.m_ptr;
		add_ref();
		return *this;
	}
	ref_ptr<T>& operator = (T *ptr)
	{
		if(m_ptr != ptr)
		{
			remove_ref();
			m_ptr = ptr;
			add_ref();
		}
		return *this;
	}
	T* get(){
		return m_ptr;
	}
	
public:
	T*  operator->() const { return m_ptr; }
	T&  operator*() const { return *m_ptr; }
	operator T*() const { return m_ptr; }
	operator bool() const { return m_ptr; }
	bool operator < (const ref_ptr<T>& ref){
		return m_ptr < ref.get();
	}
private:
	void add_ref()
	{
		if(m_ptr) ((referable<T>*)m_ptr)->on_reg((referable<T>**)&m_ptr);
	}
	void remove_ref()
	{
		if(m_ptr) ((referable<T>*)m_ptr)->on_unreg((referable<T>**)&m_ptr);
		m_ptr = NULL;
	}

private:
	T* m_ptr;
};

template<typename T>
class referable
{
public:    
	typedef referable** RefAddress;
	virtual ~referable()
	{
		if (m_references.empty()){
			return;
		}
		for (container_iter iter = m_references.begin();
			iter != m_references.end();
			++iter)
		{
			**iter = NULL; 
		}
	}
private:
	typedef typename std::vector<RefAddress> container_type;
	typedef typename container_type::iterator container_iter;

	friend class ref_ptr<T>;
	void on_reg(RefAddress addr)
	{
		for (container_iter iter = m_references.begin();
			iter != m_references.end();
			++iter)
		{
			if(*iter == addr) return;
		}
		m_references.push_back(addr);
	}
	void on_unreg(RefAddress addr)
	{
		for (container_iter iter = m_references.begin();
			iter != m_references.end();)
		{
			if(*iter == addr)
				iter = m_references.erase(iter);
			else
				++iter;
		}
	}
private:
	container_type m_references;
};