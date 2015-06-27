#include <type_traits>

#ifndef LOOP_H
#define LOOP_H

template<typename T>
class LoopRangeIterator {
public:
	LoopRangeIterator(T val_) : val(val_) {}

	bool operator!=(LoopRangeIterator const & other) const
	{ return val != other.val; }

	T const& operator*() const
	{ return val;  }

	LoopRangeIterator& operator++()
	{ 
		++val;
		return *this;
	}

private:
	T val;
};

template<typename T>
class LoopRange {
public:
	LoopRange(T from_, T to_) : from(from_), to(to_) {}

	LoopRange(T to_) : from(0), to(to_) {}

	LoopRangeIterator<T> begin() const { return { from }; }
	LoopRangeIterator<T> end() const { return { to }; }

private:
	T const from, to;
};

template<typename T>
LoopRange<T> range(T from, T to)
{
	static_assert(std::is_integral<T>::value, "range only accepts integral values");

	return { from, to };
}

template<typename F>
void repeat(unsigned n, F f) {
	while (n--) f();
}

#endif
