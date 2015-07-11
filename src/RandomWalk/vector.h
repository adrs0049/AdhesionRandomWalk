

#pragma once

#include <vector>
#include <AlignmentAllocator.h>

template<typename T>
//using AligndVector = typename std::vector<T>;
using AligndVector = typename std::vector<T, AlignedAllocator<T, Alignment::AVX>>;
//struct AlignVector
//{ typedef std::vector<T, AlignedAllocator<T, Alignment::AVX> type; };

