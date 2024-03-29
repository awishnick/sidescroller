// Copyright John Maddock 2006.
// Copyright Paul A. Bristow 2007, 2009
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

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

#include "test_beta_hooks.hpp"
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
         BOOST_CHECK_EQUAL(boost::math::ibeta_inv(data[i][0], data[i][1], data[i][5]), value_type(0));
      else if((1 - data[i][5] > 0.001) 
         && (fabs(data[i][5]) > 2 * boost::math::tools::min_value<value_type>()) 
         && (fabs(data[i][5]) > 2 * boost::math::tools::min_value<double>()))
      {
         value_type inv = boost::math::ibeta_inv(data[i][0], data[i][1], data[i][5]);
         BOOST_CHECK_CLOSE(data[i][2], inv, precision);
      }
      else if(1 == data[i][5])
         BOOST_CHECK_EQUAL(boost::math::ibeta_inv(data[i][0], data[i][1], data[i][5]), value_type(1));

      if(data[i][6] == 0)
         BOOST_CHECK_EQUAL(boost::math::ibetac_inv(data[i][0], data[i][1], data[i][6]), value_type(1));
      else if((1 - data[i][6] > 0.001) 
         && (fabs(data[i][6]) > 2 * boost::math::tools::min_value<value_type>()) 
         && (fabs(data[i][6]) > 2 * boost::math::tools::min_value<double>()))
      {
         value_type inv = boost::math::ibetac_inv(data[i][0], data[i][1], data[i][6]);
         BOOST_CHECK_CLOSE(data[i][2], inv, precision);
      }
      else if(data[i][6] == 1)
         BOOST_CHECK_EQUAL(boost::math::ibetac_inv(data[i][0], data[i][1], data[i][6]), value_type(0));
   }
}

template <class T>
void test_inverses2(const T& data, const char* type_name, const char* test_name)
{
   typedef typename T::value_type row_type;
   typedef typename row_type::value_type value_type;

   typedef value_type (*pg)(value_type, value_type, value_type);
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   pg funcp = boost::math::ibeta_inv<value_type, value_type, value_type>;
#else
   pg funcp = boost::math::ibeta_inv;
#endif

   boost::math::tools::test_result<value_type> result;

   std::cout << "Testing " << test_name << " with type " << type_name
      << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

   //
   // test ibeta_inv(T, T, T) against data:
   //
   result = boost::math::tools::test(
      data,
      bind_func(funcp, 0, 1, 2),
      extract_result(3));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::ibeta_inv", test_name);
   //
   // test ibetac_inv(T, T, T) against data:
   //
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   funcp = boost::math::ibetac_inv<value_type, value_type, value_type>;
#else
   funcp = boost::math::ibetac_inv;
#endif
   result = boost::math::tools::test(
      data,
      bind_func(funcp, 0, 1, 2),
      extract_result(4));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::ibetac_inv", test_name);
}


template <class T>
void test_beta(T, const char* name)
{
   (void)name;
   //
   // The actual test data is rather verbose, so it's in a separate file
   //
   // The contents are as follows, each row of data contains
   // five items, input value a, input value b, integration limits x, beta(a, b, x) and ibeta(a, b, x):
   //
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

#if !defined(TEST_DATA) || (TEST_DATA == 4)
#  include "ibeta_inv_data.ipp"

   test_inverses2(ibeta_inv_data, name, "Inverse incomplete beta");
#endif
}

template <class T>
void test_spots(T)
{
   //
   // basic sanity checks, tolerance is 100 epsilon expressed as a percentage:
   //
   T tolerance = boost::math::tools::epsilon<T>() * 10000;
   BOOST_CHECK_CLOSE(
      ::boost::math::ibeta_inv(
         static_cast<T>(1),
         static_cast<T>(2),
         static_cast<T>(0.5)),
      static_cast<T>(0.29289321881345247559915563789515096071516406231153L), tolerance);
   BOOST_CHECK_CLOSE(
      ::boost::math::ibeta_inv(
         static_cast<T>(3),
         static_cast<T>(0.5),
         static_cast<T>(0.5)),
      static_cast<T>(0.92096723292382700385142816696980724853063433975470L), tolerance);
   BOOST_CHECK_CLOSE(
      ::boost::math::ibeta_inv(
         static_cast<T>(20.125),
         static_cast<T>(0.5),
         static_cast<T>(0.5)),
      static_cast<T>(0.98862133312917003480022776106012775747685870929920L), tolerance);
   BOOST_CHECK_CLOSE(
      ::boost::math::ibeta_inv(
         static_cast<T>(40),
         static_cast<T>(80),
         static_cast<T>(0.5)),
      static_cast<T>(0.33240456430025026300937492802591128972548660643778L), tolerance);
}

