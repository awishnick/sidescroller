// Copyright John Maddock 2006.
// Copyright Paul A. Bristow 2007, 2009
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_MATH_OVERFLOW_ERROR_POLICY ignore_error

#include <boost/math/concepts/real_concept.hpp>
#include <boost/test/test_exec_monitor.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/math/special_functions/math_fwd.hpp>
#include <boost/math/tools/stats.hpp>
#include <boost/math/tools/test.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/array.hpp>
#include "functor.hpp"

#ifdef TEST_GSL
#include <gsl/gsl_errno.h>
#include <gsl/gsl_message.h>
#endif

#include "handle_test_result.hpp"

#ifndef SC_
#define SC_(x) static_cast<T>(BOOST_JOIN(x, L))
#endif

template <class T>
void test_inverses(const T& data)
{
   using namespace std;
   typedef typename T::value_type row_type;
   typedef typename row_type::value_type value_type;

   value_type precision = static_cast<value_type>(ldexp(1.0, 1-boost::math::policies::digits<value_type, boost::math::policies::policy<> >()/2)) * 100;
   if(boost::math::policies::digits<value_type, boost::math::policies::policy<> >() < 50)
      precision = 1;   // 1% or two decimal digits, all we can hope for when the input is truncated

   for(unsigned i = 0; i < data.size(); ++i)
   {
      //
      // These inverse tests are thrown off if the output of the
      // incomplete beta is too close to 1: basically there is insuffient
      // information left in the value we're using as input to the inverse
      // to be able to get back to the original value.
      //
      if(data[i][5] == 0)
      {
         BOOST_CHECK_EQUAL(boost::math::ibeta_inva(data[i][1], data[i][2], data[i][5]), boost::math::tools::max_value<value_type>());
         BOOST_CHECK_EQUAL(boost::math::ibeta_invb(data[i][0], data[i][2], data[i][5]), boost::math::tools::min_value<value_type>());
      }
      else if((1 - data[i][5] > 0.001) 
         && (fabs(data[i][5]) > 2 * boost::math::tools::min_value<value_type>()) 
         && (fabs(data[i][5]) > 2 * boost::math::tools::min_value<double>()))
      {
         value_type inv = boost::math::ibeta_inva(data[i][1], data[i][2], data[i][5]);
         BOOST_CHECK_CLOSE(data[i][0], inv, precision);
         inv = boost::math::ibeta_invb(data[i][0], data[i][2], data[i][5]);
         BOOST_CHECK_CLOSE(data[i][1], inv, precision);
      }
      else if(1 == data[i][5])
      {
         BOOST_CHECK_EQUAL(boost::math::ibeta_inva(data[i][1], data[i][2], data[i][5]), boost::math::tools::min_value<value_type>());
         BOOST_CHECK_EQUAL(boost::math::ibeta_invb(data[i][0], data[i][2], data[i][5]), boost::math::tools::max_value<value_type>());
      }

      if(data[i][6] == 0)
      {
         BOOST_CHECK_EQUAL(boost::math::ibetac_inva(data[i][1], data[i][2], data[i][6]), boost::math::tools::min_value<value_type>());
         BOOST_CHECK_EQUAL(boost::math::ibetac_invb(data[i][0], data[i][2], data[i][6]), boost::math::tools::max_value<value_type>());
      }
      else if((1 - data[i][6] > 0.001) 
         && (fabs(data[i][6]) > 2 * boost::math::tools::min_value<value_type>()) 
         && (fabs(data[i][6]) > 2 * boost::math::tools::min_value<double>()))
      {
         value_type inv = boost::math::ibetac_inva(data[i][1], data[i][2], data[i][6]);
         BOOST_CHECK_CLOSE(data[i][0], inv, precision);
         inv = boost::math::ibetac_invb(data[i][0], data[i][2], data[i][6]);
         BOOST_CHECK_CLOSE(data[i][1], inv, precision);
      }
      else if(data[i][6] == 1)
      {
         BOOST_CHECK_EQUAL(boost::math::ibetac_inva(data[i][1], data[i][2], data[i][6]), boost::math::tools::max_value<value_type>());
         BOOST_CHECK_EQUAL(boost::math::ibetac_invb(data[i][0], data[i][2], data[i][6]), boost::math::tools::min_value<value_type>());
      }
   }
}

template <class T>
void test_inverses2(const T& data, const char* type_name, const char* test_name)
{
   typedef typename T::value_type row_type;
   typedef typename row_type::value_type value_type;

   typedef value_type (*pg)(value_type, value_type, value_type);
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   pg funcp = boost::math::ibeta_inva<value_type, value_type, value_type>;
#else
   pg funcp = boost::math::ibeta_inva;
#endif

   boost::math::tools::test_result<value_type> result;

   std::cout << "Testing " << test_name << " with type " << type_name
      << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

   //
   // test ibeta_inva(T, T, T) against data:
   //
   result = boost::math::tools::test(
      data,
      bind_func(funcp, 0, 1, 2),
      extract_result(3));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::ibeta_inva", test_name);
   //
   // test ibetac_inva(T, T, T) against data:
   //
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   funcp = boost::math::ibetac_inva<value_type, value_type, value_type>;
#else
   funcp = boost::math::ibetac_inva;
#endif
   result = boost::math::tools::test(
      data,
      bind_func(funcp, 0, 1, 2),
      extract_result(4));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::ibetac_inva", test_name);
   //
   // test ibeta_invb(T, T, T) against data:
   //
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   funcp = boost::math::ibeta_invb<value_type, value_type, value_type>;
#else
   funcp = boost::math::ibeta_invb;
#endif
   result = boost::math::tools::test(
      data,
      bind_func(funcp, 0, 1, 2),
      extract_result(5));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::ibeta_invb", test_name);
   //
   // test ibetac_invb(T, T, T) against data:
   //
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   funcp = boost::math::ibetac_invb<value_type, value_type, value_type>;
#else
   funcp = boost::math::ibetac_invb;
#endif
   result = boost::math::tools::test(
      data,
      bind_func(funcp, 0, 1, 2),
      extract_result(6));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::ibetac_invb", test_name);
}

template <class T>
void test_beta(T, const char* name)
{
   //
   // The actual test data is rather verbose, so it's in a separate file
   //
   // The contents are as follows, each row of data contains
   // five items, input value a, input value b, integration limits x, beta(a, b, x) and ibeta(a, b, x):
   //
   std::cout << "Running sanity checks for type " << name << std::endl;

#if !defined(TEST_DATA) || (TEST_DATA == 1)
#  include "ibeta_small_data.ipp"

   test_inverses(ibeta_small_data);
#endif

#if !defined(TEST_DATA) || (TEST_DATA == 2)
#  include "ibeta_data.ipp"

   test_inverses(ibeta_data);
#endif

#if !defined(TEST_DATA) || (TEST_DATA == 3)
#  include "ibeta_large_data.ipp"

   test_inverses(ibeta_large_data);
#endif

#if !defined(TEST_REAL_CONCEPT) || defined(FULL_TEST) || (TEST_DATA == 4)
#ifndef FULL_TEST
   if(boost::is_floating_point<T>::value){
#endif
   //
   // This accuracy test is normally only enabled for "real"
   // floating point types and not for class real_concept.
   // The reason is that these tests are exceptionally slow
   // to complete when T doesn't have Lanczos support defined for it.
   //
#  include "ibeta_inva_data.ipp"

   test_inverses2(ibeta_inva_data, name, "Inverse incomplete beta");
#ifndef FULL_TEST
   }
#endif
#endif
}

