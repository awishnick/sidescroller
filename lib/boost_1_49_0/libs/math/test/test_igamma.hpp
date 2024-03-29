// Copyright John Maddock 2006.
// Copyright Paul A. Bristow 2007, 2009
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_MATH_OVERFLOW_ERROR_POLICY ignore_error

#include <boost/math/concepts/real_concept.hpp>
#include <boost/math/special_functions/math_fwd.hpp>
#include <boost/test/test_exec_monitor.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/math/tools/stats.hpp>
#include <boost/math/tools/test.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/array.hpp>
#include "functor.hpp"

#include "test_gamma_hooks.hpp"
#include "handle_test_result.hpp"

#ifndef SC_
#define SC_(x) static_cast<T>(BOOST_JOIN(x, L))
#endif

template <class T>
void do_test_gamma_2(const T& data, const char* type_name, const char* test_name)
{
   typedef typename T::value_type row_type;
   typedef typename row_type::value_type value_type;

   typedef value_type (*pg)(value_type, value_type);
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   pg funcp = boost::math::tgamma<value_type, value_type>;
#else
   pg funcp = boost::math::tgamma;
#endif

   boost::math::tools::test_result<value_type> result;

   std::cout << "Testing " << test_name << " with type " << type_name
      << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

   //
   // test tgamma(T, T) against data:
   //
   if(data[0][2] > 0)
   {
      result = boost::math::tools::test(
         data,
         bind_func(funcp, 0, 1),
         extract_result(2));
      handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::tgamma", test_name);
      //
      // test tgamma_lower(T, T) against data:
      //
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
      funcp = boost::math::tgamma_lower<value_type, value_type>;
#else
      funcp = boost::math::tgamma_lower;
#endif
      result = boost::math::tools::test(
         data,
         bind_func(funcp, 0, 1),
         extract_result(4));
      handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::tgamma_lower", test_name);
   }
   //
   // test gamma_q(T, T) against data:
   //
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   funcp = boost::math::gamma_q<value_type, value_type>;
#else
   funcp = boost::math::gamma_q;
#endif
   result = boost::math::tools::test(
      data,
      bind_func(funcp, 0, 1),
      extract_result(3));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::gamma_q", test_name);
#if defined(TEST_OTHER)
   //
   // test other gamma_q(T, T) against data:
   //
   if(boost::is_floating_point<value_type>::value)
   {
      funcp = other::gamma_q;
      result = boost::math::tools::test(
         data,
         bind_func(funcp, 0, 1),
         extract_result(3));
      print_test_result(result, data[result.worst()], result.worst(), type_name, "other::gamma_q");
   }
#endif
   //
   // test gamma_p(T, T) against data:
   //
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   funcp = boost::math::gamma_p<value_type, value_type>;
#else
   funcp = boost::math::gamma_p;
#endif
   result = boost::math::tools::test(
      data,
      bind_func(funcp, 0, 1),
      extract_result(5));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::gamma_p", test_name);
#if defined(TEST_OTHER)
   //
   // test other gamma_p(T, T) against data:
   //
   if(boost::is_floating_point<value_type>::value)
   {
      funcp = other::gamma_p;
      result = boost::math::tools::test(
         data,
         bind_func(funcp, 0, 1),
         extract_result(5));
      print_test_result(result, data[result.worst()], result.worst(), type_name, "other::gamma_p");
   }
#endif
   std::cout << std::endl;
}

template <class T>
void test_gamma(T, const char* name)
{
   //
   // The actual test data is rather verbose, so it's in a separate file
   //
   // First the data for the incomplete gamma function, each
   // row has the following 6 entries:
   // Parameter a, parameter z,
   // Expected tgamma(a, z), Expected gamma_q(a, z)
   // Expected tgamma_lower(a, z), Expected gamma_p(a, z)
   //
#  include "igamma_med_data.ipp"

   do_test_gamma_2(igamma_med_data, name, "tgamma(a, z) medium values");

#  include "igamma_small_data.ipp"

   do_test_gamma_2(igamma_small_data, name, "tgamma(a, z) small values");

#  include "igamma_big_data.ipp"

   do_test_gamma_2(igamma_big_data, name, "tgamma(a, z) large values");

#  include "igamma_int_data.ipp"

   do_test_gamma_2(igamma_int_data, name, "tgamma(a, z) integer and half integer values");
}

template <class T>
void test_spots(T)
{
   //
   // basic sanity checks, tolerance is 10 epsilon expressed as a percentage:
   //
   T tolerance = boost::math::tools::epsilon<T>() * 1000;
#if (defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__))
   tolerance *= 10;
#endif
   BOOST_CHECK_CLOSE(::boost::math::tgamma(static_cast<T>(5), static_cast<T>(1)), static_cast<T>(23.912163676143750903709045060494956383977723517065L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::tgamma(static_cast<T>(5), static_cast<T>(5)), static_cast<T>(10.571838841565097874621959975919877646444998907920L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::tgamma(static_cast<T>(5), static_cast<T>(10)), static_cast<T>(0.70206451384706574414638719662835463671916532623256L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::tgamma(static_cast<T>(5), static_cast<T>(100)), static_cast<T>(3.8734332808745531496973774140085644548465762343719e-36L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::tgamma(static_cast<T>(0.5), static_cast<T>(0.5)), static_cast<T>(0.56241823159440712427949495730204306902676756479651L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::tgamma(static_cast<T>(0.5), static_cast<T>(9)/10), static_cast<T>(0.31853210360412109873859360390443790076576777747449L), tolerance*10);
   BOOST_CHECK_CLOSE(::boost::math::tgamma(static_cast<T>(0.5), static_cast<T>(5)), static_cast<T>(0.0027746032604128093194908357272603294120210079791437L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::tgamma(static_cast<T>(0.5), static_cast<T>(100)), static_cast<T>(3.7017478604082789202535664481339075721362102520338e-45L), tolerance);

   BOOST_CHECK_CLOSE(::boost::math::tgamma_lower(static_cast<T>(5), static_cast<T>(1)), static_cast<T>(0.087836323856249096290954939505043616022276482935091L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::tgamma_lower(static_cast<T>(5), static_cast<T>(5)), static_cast<T>(13.428161158434902125378040024080122353555001092080L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::tgamma_lower(static_cast<T>(5), static_cast<T>(10)), static_cast<T>(23.297935486152934255853612803371645363280834673767L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::tgamma_lower(static_cast<T>(5), static_cast<T>(100)), static_cast<T>(23.999999999999999999999999999999999996126566719125L), tolerance);

   BOOST_CHECK_CLOSE(::boost::math::gamma_q(static_cast<T>(5), static_cast<T>(1)), static_cast<T>(0.99634015317265628765454354418728984933240514654437L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::gamma_q(static_cast<T>(5), static_cast<T>(5)), static_cast<T>(0.44049328506521241144258166566332823526854162116334L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::gamma_q(static_cast<T>(5), static_cast<T>(10)), static_cast<T>(0.029252688076961072672766133192848109863298555259690L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::gamma_q(static_cast<T>(5), static_cast<T>(100)), static_cast<T>(1.6139305336977304790405739225035685228527400976549e-37L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::gamma_q(static_cast<T>(1.5), static_cast<T>(2)), static_cast<T>(0.26146412994911062220282207597592120190281060919079L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::gamma_q(static_cast<T>(20.5), static_cast<T>(22)), static_cast<T>(0.34575332043467326814971590879658406632570278929072L), tolerance);

   BOOST_CHECK_CLOSE(::boost::math::gamma_p(static_cast<T>(5), static_cast<T>(1)), static_cast<T>(0.0036598468273437123454564558127101506675948534556288L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::gamma_p(static_cast<T>(5), static_cast<T>(5)), static_cast<T>(0.55950671493478758855741833433667176473145837883666L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::gamma_p(static_cast<T>(5), static_cast<T>(10)), static_cast<T>(0.97074731192303892732723386680715189013670144474031L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::gamma_p(static_cast<T>(5), static_cast<T>(100)), static_cast<T>(0.9999999999999999999999999999999999998386069466302L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::gamma_p(static_cast<T>(1.5), static_cast<T>(2)), static_cast<T>(0.73853587005088937779717792402407879809718939080921L), tolerance);
   BOOST_CHECK_CLOSE(::boost::math::gamma_p(static_cast<T>(20.5), static_cast<T>(22)), static_cast<T>(0.65424667956532673185028409120341593367429721070928L), tolerance);

   // naive check on derivative function:
   using namespace std;  // For ADL of std functions
   tolerance = boost::math::tools::epsilon<T>() * 5000; // 50 eps
   BOOST_CHECK_CLOSE(::boost::math::gamma_p_derivative(static_cast<T>(20.5), static_cast<T>(22)), 
      exp(static_cast<T>(-22)) * pow(static_cast<T>(22), static_cast<T>(19.5)) / boost::math::tgamma(static_cast<T>(20.5)), tolerance);

}

