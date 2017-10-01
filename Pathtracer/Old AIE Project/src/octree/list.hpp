#pragma once
#include <memory>
#include <assert.h>

template <typename T>
class ptr_list final
{
public:
	ptr_list(unsigned capacity = 1) :
		m_capacity(capacity),
		m_count(0)
	{
		assert(capacity >= 1);
		m_data = static_cast<T*>(malloc(m_capacity * sizeof(T)));
	}

	ptr_list(const ptr_list<T> &other)
	{
		*this = other;
	}

	~ptr_list()
	{
		free(m_data);
		m_capacity = m_count = 0;
	}

	void add(const T elem)
	{
		if (m_count + 1 > m_capacity)
		{
			// re-allocate and expand array capacity
			T* oldData = m_data;
			unsigned newCapacity = (m_capacity == 0) ? (1) : (m_capacity << 1);
			m_data = static_cast<T*>(malloc(newCapacity * sizeof(T)));
			memcpy(m_data, oldData, m_capacity * sizeof(T));
			m_capacity = newCapacity;
			free(oldData);
		}

		m_data[m_count] = elem;
		++m_count;
	}

	int find(const T elem)
	{
		if (m_count == 0)
			return -1;

		int t = 0;
		for (int s = 0; s < m_count; ++s)
		{
			if (m_data[s] == elem)
				return t;

			++t;
		}

		return -1;
	}

	void remove(unsigned index)
	{
		if (m_count == 0 || index >= m_count)
			return;

		T &last = m_data[m_count];
		T &elem = m_data[index];

		elem = last;

		--m_count;
	}

	T operator[](unsigned index) const
	{
		return *(m_data + index);
	}

	ptr_list<T> &operator=(const ptr_list<T> &other)
	{
		free(m_data);
		m_data = static_cast<T*>(malloc(other.m_capacity * sizeof(T)));
		memcpy(m_data, other.m_data, other.m_capacity * sizeof(T));
		m_capacity = other.m_capacity;
		m_count = other.m_count;
	}

	unsigned count() const
	{
		return m_count;
	}

	T *data()
	{
		return m_data;
	}

private:
	unsigned m_count, m_capacity;
	T *m_data;
};