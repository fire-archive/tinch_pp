#ifndef BOOST_PP_IS_ITERATING
#if !defined(MAKE_ERL_TUPLE)
#define MAKE_ERL_TUPLE

#include <boost/preprocessor/iterate.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/fusion/tuple/tuple.hpp>
#include "erl_tuple.h"

namespace tinch_pp {
namespace erl {

inline tuple<boost::fusion::tuple<> > make_tuple()
{
  typedef boost::fusion::tuple<> Tuple;
  
  return erl::tuple<Tuple>(boost::fusion::tuple<>());
}

#define BOOST_PP_FILENAME_1 "tinch_pp/make_erl_tuple.h"

// TODO: Document the constant!
#if !defined(MAX_ERL_TUPLE_SIZE)
  #define MAX_ERL_TUPLE_SIZE 10
#endif
#define BOOST_PP_ITERATION_LIMITS (1, MAX_ERL_TUPLE_SIZE)
#include BOOST_PP_ITERATE()

}}

#endif
#else // defined(BOOST_PP_IS_ITERATING)
///////////////////////////////////////////////////////////////////////////////
//
//  Preprocessor vertical repetition code
//
///////////////////////////////////////////////////////////////////////////////

#define N BOOST_PP_ITERATION()

template <BOOST_PP_ENUM_PARAMS(N, typename T)>
inline tuple<boost::fusion::tuple<BOOST_PP_ENUM_PARAMS(N, T)> >
make_tuple(BOOST_PP_ENUM_BINARY_PARAMS(N, T, const& t))
{
  typedef boost::fusion::tuple<BOOST_PP_ENUM_PARAMS(N, T)> Tuple;
  
  return erl::tuple<Tuple>(boost::fusion::tuple<BOOST_PP_ENUM_PARAMS(N, T)>(
                           BOOST_PP_ENUM_PARAMS(N, t)));
}

#undef N
#endif // defined(BOOST_PP_IS_ITERATING)

