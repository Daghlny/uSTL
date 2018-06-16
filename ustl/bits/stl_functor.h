
#pragma once

namespace ustl
{

/* unarg_function and binary_function are both base classes to inherit
 */
template<class Arg, class Result>
struct unarg_function {
    typedef Arg argument_type;
    typedef Result result_type;
};

template<class Arg1, class Arg2, class Result>
struct binary_function {
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};

/*
 * All built-in functors are divided into six parts:
 * 1. Arithmetic
 * 2. Logistic
 * 3. Relational
 * 4. Identity
 * 5. Select
 * 6. Project
 */
////////////////////////////////////////////////////
//////// Arithmetic ////////////////////////////////
////////////////////////////////////////////////////

template<class T>
struct plus: public binary_function<T, T, T>
{
    T operator()(const T& x, const T& y) const { return x+y; }
};

template<class T>
struct minus: public binary_function<T, T, T>
{
    T operator()(const T& x, const T& y) const { return x-y; }
};

template<class T>
struct negate : public unary_function<T, T>
{
    T operator()(const T& x) const { return -x; }
};

template<class T>
struct multiplies: public binary_function<T, T, T>
{
    T operator()(const T& x, const T& y) const { return x*y; }
};

template<class T>
struct divides: public binary_function<T, T, T>
{
    T operator()(const T& x, const T& y) const { return x/y; }
};

////////////////////////////////////////////////////
//////// Relational ////////////////////////////////
////////////////////////////////////////////////////

template<class T>
struct equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x == y; }
};

template<class T>
struct less : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x < y; }
};

template<class T>
struct greater : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x > y; }
};

////////////////////////////////////////////////////
//////// Identity   ////////////////////////////////
////////////////////////////////////////////////////

template<class T>
struct Identity : public unary_function<T, T> {
    const T& operator()(const T& x) const { return x; }
}

////////////////////////////////////////////////////
//////// Select     ////////////////////////////////
////////////////////////////////////////////////////

template<class Pair>
struct _Select1st: public unary_function<Pair, typename Pair::first_type> {
    const typename Pair::first_type& operator() (const Pair& x) const { return x.first; }
};

template<class Pair>
struct _Select2nd: public unary_function<Pair, typename Pair::second_type> {
    const typename Pair::second_type& operator() (const Pair& x) const { return x.second; }
};


}
