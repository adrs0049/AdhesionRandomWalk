
#pragma once

#include <utility>
#include <vector>
#include <exception>
#include <stdexcept>
#include <sstream>

template<class K>
std::out_of_range make_out_of_range(const std::string& message, const K& key)
{
	std::ostringstream ss;
	ss << message << key;
	return std::out_of_range(ss.str());
}

template <class V, class K = unsigned>
class flat_index_map
{
	public:
		using value_type = std::pair<K, V>;
		using key_type = K;
		using mapped_type = V;

	private:
		std::vector<value_type> m_data;
		std::vector<bool> m_bits;

	struct compare_elems
	{
		bool operator()(const value_type& x, const value_type& y)
		{
			return x.first < y.first;
		}

		bool operator()(const value_type& x, key_type y)
		{
			return x.first < y;
		}

		bool operator()(key_type x, const value_type& y)
		{
			return x < y.first;
		}
	};

	public:
		using iterator = typename std::vector<value_type>::iterator;
		using const_iterator = typename std::vector<value_type>::const_iterator;
		using reverse_iterator = typename std::vector<value_type>::reverse_iterator;
		using const_reverse_iterator = typename std::vector<value_type>::const_reverse_iterator;

		flat_index_map(std::size_t n = 0) : m_bits(n) {}
		void resize(std::size_t n) { m_bits.resize(n); }

		bool insert(const value_type& value)
		{
			if (m_bits[value.first])
				return false;
			iterator it = std::lower_bound(m_data.begin(), m_data.end(), value.first, compare_elems());
			iterator it2 = m_data.insert(it, value);
			m_bits[value.first] = true;
			return true;
		}

		V& operator[](const K& key)
		{
			iterator it = std::lower_bound(m_data.begin(), m_data.end(), key, compare_elems());
			if (!m_bits[key])
			{
				iterator it2 = m_data.insert(it, value_type(key, V()));
				m_bits[key] = true;
				return it2->second;
			}
			else
			{
				return it->second;
			}
		}

		V& at(const K& key)
		{
			if (m_bits[key])
			{
				iterator it = std::lower_bound(m_data.begin(), m_data.end(), key, compare_elems());
				return it->second;
			}
			throw make_out_of_range("flat_index_map invalid key: ", key);
		}

		const V& at(const K& key) const
		{
			if (m_bits[key])
			{
				const_iterator it = std::lower_bound(m_data.begin(), m_data.end(), key, compare_elems());
				return it->second;
			}
			throw make_out_of_range("flat_index_map invalid key: ", key);
		}

		void swap(flat_index_map& m)
		{
			m_data.swap(m.m_data);
			m_bits(m.m_bits);
		}

		iterator find(const K& key)
		{
			if (m_bits[key])
			{
				iterator it = std::lower_bound(m_data.begin(), m_data.end(), key, compare_elems());
				return it;
			}
			else
			{
				return m_data.end();
			}
		}

		const_iterator find(const K& key) const
		{
			if (m_bits[key])
			{
				const_iterator it = std::lower_bound(m_data.begin(), m_data.end(), key, compare_elems());
				return it;
			}
			else
			{
				return m_data.cend();
			}
		}

		void erase(const K& key)
		{
			if (m_bits[key])
			{
				iterator it = std::lower_bound(m_data.begin(), m_data.end(), key, compare_elems());
				m_data.erase(it);
				m_bits[key] = false;
			}
		}

		void clear()
		{
			m_data.clear();
			std::fill(m_bits.begin(), m_bits.end(), false);
		}

		bool empty() const
		{
			return m_data.empty();
		}

		std::size_t size() const { return m_data.size(); }

		iterator begin() { return m_data.begin(); }
		iterator end() { return m_data.end(); }

		const_iterator begin() const { return m_data.begin(); }
		const_iterator end() const { return m_data.end(); }

		reverse_iterator rbegin() { return m_data.rbegin(); }
		reverse_iterator rend() { return m_data.rend(); }

		const_reverse_iterator rbegin() const { return m_data.rbegin(); }
		const_reverse_iterator rend() const { return m_data.rend(); }
};


