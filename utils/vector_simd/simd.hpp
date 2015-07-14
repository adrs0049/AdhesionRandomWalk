#include "simd_traits.h"

template<class T, class V>
struct simd_functions_invoker
{
    inline static V
    set1(const T& a) { return V(a); }

    inline static V
    load_a(const T* src) { V res; res.load_a(src); return res;}

    inline static V
    load_u(const T* src) { V res; res.load_u(src); return res; }

    inline static void
    store_a(T* dst, const V& src) { src.store_a(dst); }

    inline static void
    store_u(T* dst, const V& src) { src.store_u(dst); }
};

template<class T>
struct simd_functions_invoker<T, T>
{
    inline static T
    set1(const T& a) { return T(a); }

    inline static T
    load_a(const T* src) { return *src; }

    inline static T
    load_u(const T* src) { return *src; }

    inline static void
    store_a(T* dst, const T& src) { *dst = src; }

    inline static void
    store_u(T* dst, const T& src) { *dst = src; }
};

template<class T> inline typename simd_traits<T>::type
set1(const T& a)
{ return simd_functions_invoker<T, typename simd_traits<T>::type>::set1(a); }

template<class T> inline typename simd_traits<T>::type
load_a(const T* src)
{ return simd_functions_invoker<T, typename simd_traits<T>::type>::load_a(src); }

template<class T> inline typename simd_traits<T>::type
load_u(const T* src)
{ return simd_functions_invoker<T, typename simd_traits<T>::type>::load_u(src); }

template<class T> inline void
store_a(T* dst, const typename simd_traits<T>::type& src)
{ simd_functions_invoker<T, typename simd_traits<T>::type>::store_a(dst, src); }

template<class T> inline void
store_u(T* dst, const typename simd_traits<T>::type& src)
{ simd_functions_invoker<T, typename simd_traits<T>::type>::store_u(dst, src); }

