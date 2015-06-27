/*
 * helpers.h
 * 
 * Some small helper functions used here and there
 * 
 * Andreas Buttenschoen 
 * 2014
 * 
 */

#ifndef HELPERS_H
#define HELPERS_H

#define HELPERS_H_VERSION 0.1

namespace Helper {

#include "concepts.h"
#include <cmath>
#include <matrix.h>
#include <debug.h>

template<typename T>
Enable_if<Floating_Point<T>() , bool> 
AreSame(T a, T b, T tolerance)
{
	return fabs(a-b) < tolerance;
}

// work for square matrices only!!
template<class Matrix>
Enable_if< Matrix_type<Matrix>() && Floating_Point<typename Matrix::value_type>(), Matrix >
HilbertMatrix(std::size_t size)
{
	DEBUG("HilbertMatrix\n");
	using value_type = typename Matrix::value_type;
	Matrix H (size, size);

	for (int i=0;i<H.rows();i++) {
		for (int j=H.cbegin(i);j<H.cend(i);j++) 
			H(i, j) = 1 / value_type(i + j + 1);
	}

	DEBUG("HilbertMatrix Done\n");
	return H;
}

} // end namespace

#endif