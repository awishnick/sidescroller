// Copyright John Maddock 2006.
// Copyright Paul A. Bristow 2007, 2009
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/math/concepts/real_concept.hpp>
#include <boost/test/test_exec_monitor.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/math/special_functions/math_fwd.hpp>
#include <boost/array.hpp>
#include "functor.hpp"

#include "handle_test_result.hpp"

#ifndef SC_
#define SC_(x) static_cast<T>(BOOST_JOIN(x, L))
#endif

template <class T>
void do_test_digamma(const T& data, const char* type_name, const char* test_name)
{
   typedef typename T::value_type row_type;
   typedef typename row_type::value_type value_type;

   typedef value_type (*pg)(value_type);
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   pg funcp = boost::math::digamma<value_type>;
#else
   pg funcp = boost::math::digamma;
#endif

   boost::math::tools::test_result<value_type> result;

   std::cout << "Testing " << test_name << " with type " << type_name
      << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

   //
   // test digamma against data:
   //
   result = boost::math::tools::test(
      data, 
      bind_func(funcp, 0), 
      extract_result(1));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::digamma", test_name);
   std::cout << std::endl;
}

template <class T>
void test_digamma(T, const char* name)
{
   //
   // The actual test data is rather verbose, so it's in a separate file
   //
   // The contents are as follows, each row of data contains
   // three items, input value a, input value b and erf(a, b):
   // 
#  include "digamma_data.ipp"

   do_test_digamma(digamma_data, name, "Digamma Function: Large Values");

#  include "digamma_root_data.ipp"

   do_test_digamma(digamma_root_data, name, "Digamma Function: Near the Positive Root");

#  include "digamma_small_data.ipp"

   do_test_digamma(digamma_small_data, name, "Digamma Function: Near Zero");

#  include "digamma_neg_data.ipp"

   do_test_digamma(digamma_neg_data, name, "Digamma Function: Negative Values");

}

