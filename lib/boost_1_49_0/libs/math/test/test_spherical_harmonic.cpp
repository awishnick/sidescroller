//  (C) Copyright John Maddock 2006.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <pch_light.hpp>

#include <boost/math/concepts/real_concept.hpp>
#include <boost/test/test_exec_monitor.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/math/special_functions/math_fwd.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/array.hpp>
#include "functor.hpp"

#include "handle_test_result.hpp"

//
// DESCRIPTION:
// ~~~~~~~~~~~~
//
// This file tests the Spherical Harmonic Functions.
// There are two sets of tests, spot
// tests which compare our results with selected values computed
// using the online special function calculator at
// functions.wolfram.com, while the bulk of the accuracy tests
// use values generated with NTL::RR at 1000-bit precision
// and our generic versions of these functions.
//
// Note that when this file is first run on a new platform many of
// these tests will fail: the default accuracy is 1 epsilon which
// is too tight for most platforms.  In this situation you will
// need to cast a human eye over the error rates reported and make
// a judgement as to whether they are acceptable.  Either way please
// report the results to the Boost mailing list.  Acceptable rates of
// error are marked up below as a series of regular expressions that
// identify the compiler/stdlib/platform/data-type/test-data/test-function
// along with the maximum expected peek and RMS mean errors for that
// test.
//

void expected_results()
{
   //
   // Define the max and mean errors expected for
   // various compilers and platforms.
   //
   const char* largest_type;
#ifndef BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS
   if(boost::math::policies::digits<double, boost::math::policies::policy<> >() == boost::math::policies::digits<long double, boost::math::policies::policy<> >())
   {
      largest_type = "(long\\s+)?double";
   }
   else
   {
      largest_type = "long double";
   }
#else
   largest_type = "(long\\s+)?double";
#endif
#ifndef BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS
   if((std::numeric_limits<long double>::digits <= 64) &&
      (std::numeric_limits<long double>::digits != std::numeric_limits<double>::digits))
   {
      add_expected_result(
         ".*",                          // compiler
         ".*",                          // stdlib
         ".*",                          // platform
         "double",                      // test type(s)
         ".*",                          // test data group
         ".*", 10, 5);                  // test function
   }
#endif
   //
   // Catch all cases come last:
   //
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      largest_type,                  // test type(s)
      ".*",      // test data group
      ".*", 30000, 1000);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      "real_concept",                  // test type(s)
      ".*",      // test data group
      ".*", 30000, 1000);  // test function
   //
   // Finish off by printing out the compiler/stdlib/platform names,
   // we do this to make it easier to mark up expected error rates.
   //
   std::cout << "Tests run with " << BOOST_COMPILER << ", "
      << BOOST_STDLIB << ", " << BOOST_PLATFORM << std::endl;
}

template <class T>
void do_test_spherical_harmonic(const T& data, const char* type_name, const char* test_name)
{
   typedef typename T::value_type row_type;
   typedef typename row_type::value_type value_type;

   typedef value_type (*pg)(unsigned, int, value_type, value_type);
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   pg funcp = boost::math::spherical_harmonic_r<value_type, value_type>;
#else
   pg funcp = boost::math::spherical_harmonic_r;
#endif

   boost::math::tools::test_result<value_type> result;

   std::cout << "Testing " << test_name << " with type " << type_name
      << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

   //
   // test Spheric Harmonic against data:
   //
   result = boost::math::tools::test(
      data,
      bind_func_int2(funcp, 0, 1, 2, 3),
      extract_result(4));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::spherical_harmonic_r", test_name);

#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   funcp = boost::math::spherical_harmonic_i<value_type, value_type>;
#else
   funcp = boost::math::spherical_harmonic_i;
#endif
   //
   // test Spheric Harmonic against data:
   //
   result = boost::math::tools::test(
      data,
      bind_func_int2(funcp, 0, 1, 2, 3),
      extract_result(5));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::spherical_harmonic_i", test_name);

   std::cout << std::endl;
}

template <class T>
void test_complex_spherical_harmonic(const T& data, const char* /* name */, boost::mpl::true_ const &)
{
   typedef typename T::value_type row_type;
   typedef typename row_type::value_type value_type;

   for(unsigned i = 0; i < sizeof(data) / sizeof(data[0]); ++i)
   {
      //
      // Sanity check that the complex version does the same thing as the real
      // and imaginary versions:
      //
      std::complex<value_type> r = boost::math::spherical_harmonic(
         boost::math::tools::real_cast<unsigned>(data[i][0]),
         boost::math::tools::real_cast<unsigned>(data[i][1]),
         data[i][2],
         data[i][3]);
      value_type re = boost::math::spherical_harmonic_r(
         boost::math::tools::real_cast<unsigned>(data[i][0]),
         boost::math::tools::real_cast<unsigned>(data[i][1]),
         data[i][2],
         data[i][3]);
      value_type im = boost::math::spherical_harmonic_i(
         boost::math::tools::real_cast<unsigned>(data[i][0]),
         boost::math::tools::real_cast<unsigned>(data[i][1]),
         data[i][2],
         data[i][3]);
      BOOST_CHECK_CLOSE_FRACTION(std::real(r), re, value_type(5));
      BOOST_CHECK_CLOSE_FRACTION(std::imag(r), im, value_type(5));
   }
}

template <class T>
void test_complex_spherical_harmonic(const T& /* data */, const char* /* name */, boost::mpl::false_ const &)
{
   // T is not a built in type, can't use std::complex with it...
}

template <class T>
void test_spherical_harmonic(T, const char* name)
{
   //
   // The actual test data is rather verbose, so it's in a separate file
   //
   // The contents are as follows, each row of data contains
   // three items, input value a, input value b and erf(a, b):
   //
#  include "spherical_harmonic.ipp"

   do_test_spherical_harmonic(spherical_harmonic, name, "Spherical Harmonics");

   test_complex_spherical_harmonic(spherical_harmonic, name, boost::is_floating_point<T>());
}

template <class T>
void test_spots(T, const char* t)
{
   std::cout << "Testing basic sanity checks for type " << t << std::endl;
   //
   // basic sanity checks, tolerance is 100 epsilon:
   //
   T tolerance = boost::math::tools::epsilon<T>() * 100;
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(3, 2, static_cast<T>(0.5), static_cast<T>(0)), static_cast<T>(0.2061460599687871330692286791802688341213L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, 10, static_cast<T>(0.75), static_cast<T>(-0.25)), static_cast<T>(0.06197787102219208244041677775577045124092L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, 10, static_cast<T>(0.75), static_cast<T>(-0.25)), static_cast<T>(0.04629885158895932341185988759669916977920L), tolerance);

   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(40, 15, static_cast<T>(-0.75), static_cast<T>(2.25)), static_cast<T>(0.2806904825045745687343492963236868973484L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(40, 15, static_cast<T>(-0.75), static_cast<T>(2.25)), static_cast<T>(-0.2933918444656603582282372590387544902135L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(40, 15, static_cast<T>(-0.75), static_cast<T>(-2.25)), static_cast<T>(0.2806904825045745687343492963236868973484L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(40, 15, static_cast<T>(-0.75), static_cast<T>(-2.25)), static_cast<T>(0.2933918444656603582282372590387544902135L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(40, 15, static_cast<T>(0.75), static_cast<T>(-2.25)), static_cast<T>(-0.2806904825045745687343492963236868973484L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(40, 15, static_cast<T>(0.75), static_cast<T>(-2.25)), static_cast<T>(-0.2933918444656603582282372590387544902135L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(40, 15, static_cast<T>(0.75), static_cast<T>(2.25)), static_cast<T>(-0.2806904825045745687343492963236868973484L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(40, 15, static_cast<T>(0.75), static_cast<T>(2.25)), static_cast<T>(0.2933918444656603582282372590387544902135L), tolerance);

   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, 14, static_cast<T>(-0.75), static_cast<T>(2.25)), static_cast<T>(0.3479218186133435466692822481919867452442L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, 14, static_cast<T>(-0.75), static_cast<T>(2.25)), static_cast<T>(0.0293201066685263879566422194539567289974L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, 14, static_cast<T>(-0.75), static_cast<T>(-2.25)), static_cast<T>(0.3479218186133435466692822481919867452442L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, 14, static_cast<T>(-0.75), static_cast<T>(-2.25)), static_cast<T>(-0.0293201066685263879566422194539567289974L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, 14, static_cast<T>(0.75), static_cast<T>(-2.25)), static_cast<T>(0.3479218186133435466692822481919867452442L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, 14, static_cast<T>(0.75), static_cast<T>(-2.25)), static_cast<T>(-0.0293201066685263879566422194539567289974L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, 14, static_cast<T>(0.75), static_cast<T>(2.25)), static_cast<T>(0.3479218186133435466692822481919867452442L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, 14, static_cast<T>(0.75), static_cast<T>(2.25)), static_cast<T>(0.0293201066685263879566422194539567289974L), tolerance);

   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(39, 15, static_cast<T>(-0.75), static_cast<T>(2.25)), static_cast<T>(0.1757594233240278196989039119899901986211L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(39, 15, static_cast<T>(-0.75), static_cast<T>(2.25)), static_cast<T>(-0.1837126108841860058078729532035715580790L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(39, 15, static_cast<T>(-0.75), static_cast<T>(-2.25)), static_cast<T>(0.1757594233240278196989039119899901986211L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(39, 15, static_cast<T>(-0.75), static_cast<T>(-2.25)), static_cast<T>(0.1837126108841860058078729532035715580790L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(39, 15, static_cast<T>(0.75), static_cast<T>(-2.25)), static_cast<T>(-0.1757594233240278196989039119899901986211L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(39, 15, static_cast<T>(0.75), static_cast<T>(-2.25)), static_cast<T>(-0.1837126108841860058078729532035715580790L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(39, 15, static_cast<T>(0.75), static_cast<T>(2.25)), static_cast<T>(-0.1757594233240278196989039119899901986211L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(39, 15, static_cast<T>(0.75), static_cast<T>(2.25)), static_cast<T>(0.1837126108841860058078729532035715580790L), tolerance);

   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(19, 14, static_cast<T>(-0.75), static_cast<T>(2.25)), static_cast<T>(0.2341701030303444033808969389588343934828L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(19, 14, static_cast<T>(-0.75), static_cast<T>(2.25)), static_cast<T>(0.0197340092863212879172432610952871202640L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(19, 14, static_cast<T>(-0.75), static_cast<T>(-2.25)), static_cast<T>(0.2341701030303444033808969389588343934828L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(19, 14, static_cast<T>(-0.75), static_cast<T>(-2.25)), static_cast<T>(-0.0197340092863212879172432610952871202640L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(19, 14, static_cast<T>(0.75), static_cast<T>(-2.25)), static_cast<T>(0.2341701030303444033808969389588343934828L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(19, 14, static_cast<T>(0.75), static_cast<T>(-2.25)), static_cast<T>(-0.0197340092863212879172432610952871202640L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(19, 14, static_cast<T>(0.75), static_cast<T>(2.25)), static_cast<T>(0.2341701030303444033808969389588343934828L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(19, 14, static_cast<T>(0.75), static_cast<T>(2.25)), static_cast<T>(0.0197340092863212879172432610952871202640L), tolerance);

   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(40, -15, static_cast<T>(-0.75), static_cast<T>(2.25)), static_cast<T>(-0.2806904825045745687343492963236868973484L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(40, -15, static_cast<T>(-0.75), static_cast<T>(2.25)), static_cast<T>(-0.2933918444656603582282372590387544902135L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(40, -15, static_cast<T>(-0.75), static_cast<T>(-2.25)), static_cast<T>(-0.2806904825045745687343492963236868973484L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(40, -15, static_cast<T>(-0.75), static_cast<T>(-2.25)), static_cast<T>(0.2933918444656603582282372590387544902135L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(40, -15, static_cast<T>(0.75), static_cast<T>(-2.25)), static_cast<T>(0.2806904825045745687343492963236868973484L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(40, -15, static_cast<T>(0.75), static_cast<T>(-2.25)), static_cast<T>(-0.2933918444656603582282372590387544902135L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(40, -15, static_cast<T>(0.75), static_cast<T>(2.25)), static_cast<T>(0.2806904825045745687343492963236868973484L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(40, -15, static_cast<T>(0.75), static_cast<T>(2.25)), static_cast<T>(0.2933918444656603582282372590387544902135L), tolerance);

   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, -14, static_cast<T>(-0.75), static_cast<T>(2.25)), static_cast<T>(0.3479218186133435466692822481919867452442L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, -14, static_cast<T>(-0.75), static_cast<T>(2.25)), static_cast<T>(-0.0293201066685263879566422194539567289974L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, -14, static_cast<T>(-0.75), static_cast<T>(-2.25)), static_cast<T>(0.3479218186133435466692822481919867452442L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, -14, static_cast<T>(-0.75), static_cast<T>(-2.25)), static_cast<T>(0.0293201066685263879566422194539567289974L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, -14, static_cast<T>(0.75), static_cast<T>(-2.25)), static_cast<T>(0.3479218186133435466692822481919867452442L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, -14, static_cast<T>(0.75), static_cast<T>(-2.25)), static_cast<T>(0.0293201066685263879566422194539567289974L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, -14, static_cast<T>(0.75), static_cast<T>(2.25)), static_cast<T>(0.3479218186133435466692822481919867452442L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, -14, static_cast<T>(0.75), static_cast<T>(2.25)), static_cast<T>(-0.0293201066685263879566422194539567289974L), tolerance);

   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, 14, static_cast<T>(-4), static_cast<T>(2.25)), static_cast<T>(0.5253373768014719124617844890495875474590L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, 14, static_cast<T>(-4), static_cast<T>(2.25)), static_cast<T>(0.0442712905622650144694916590407495495699L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, 14, static_cast<T>(-4), static_cast<T>(-2.25)), static_cast<T>(0.5253373768014719124617844890495875474590L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, 14, static_cast<T>(-4), static_cast<T>(-2.25)), static_cast<T>(-0.0442712905622650144694916590407495495699L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, 14, static_cast<T>(4), static_cast<T>(-2.25)), static_cast<T>(0.5253373768014719124617844890495875474590L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, 14, static_cast<T>(4), static_cast<T>(-2.25)), static_cast<T>(-0.0442712905622650144694916590407495495699L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, 14, static_cast<T>(4), static_cast<T>(2.25)), static_cast<T>(0.5253373768014719124617844890495875474590L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, 14, static_cast<T>(4), static_cast<T>(2.25)), static_cast<T>(0.0442712905622650144694916590407495495699L), tolerance);

   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, 15, static_cast<T>(-4), static_cast<T>(2.25)), static_cast<T>(-0.2991140325667575801827063718821420263438L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, 15, static_cast<T>(-4), static_cast<T>(2.25)), static_cast<T>(0.3126490678888350710506307405826667514065L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, 15, static_cast<T>(-4), static_cast<T>(-2.25)), static_cast<T>(-0.2991140325667575801827063718821420263438L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, 15, static_cast<T>(-4), static_cast<T>(-2.25)), static_cast<T>(-0.3126490678888350710506307405826667514065L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, 15, static_cast<T>(4), static_cast<T>(-2.25)), static_cast<T>(0.2991140325667575801827063718821420263438L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, 15, static_cast<T>(4), static_cast<T>(-2.25)), static_cast<T>(0.3126490678888350710506307405826667514065L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(20, 15, static_cast<T>(4), static_cast<T>(2.25)), static_cast<T>(0.2991140325667575801827063718821420263438L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(20, 15, static_cast<T>(4), static_cast<T>(2.25)), static_cast<T>(-0.3126490678888350710506307405826667514065L), tolerance);

   BOOST_CHECK_EQUAL(::boost::math::spherical_harmonic_r(10, 15, static_cast<T>(-0.75), static_cast<T>(2.25)), static_cast<T>(0));
   BOOST_CHECK_EQUAL(::boost::math::spherical_harmonic_i(10, 15, static_cast<T>(-0.75), static_cast<T>(2.25)), static_cast<T>(0));
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_r(53, 42, static_cast<T>(-8.75), static_cast<T>(-2.25)), static_cast<T>(-0.0008147976618889536159592309471859037113647L), tolerance);
   BOOST_CHECK_CLOSE_FRACTION(::boost::math::spherical_harmonic_i(53, 42, static_cast<T>(-8.75), static_cast<T>(-2.25)), static_cast<T>(0.0002099802242493057018193798824353982612756L), tolerance);
}

int test_main(int, char* [])
{
   BOOST_MATH_CONTROL_FP;
   test_spots(0.0F, "float");
   test_spots(0.0, "double");
#ifndef BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS
   test_spots(0.0L, "long double");
   test_spots(boost::math::concepts::real_concept(0.1), "real_concept");
#endif

   expected_results();

   test_spherical_harmonic(0.1F, "float");
   test_spherical_harmonic(0.1, "double");
#ifndef BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS
   test_spherical_harmonic(0.1L, "long double");
#ifndef BOOST_MATH_NO_REAL_CONCEPT_TESTS
   test_spherical_harmonic(boost::math::concepts::real_concept(0.1), "real_concept");
#endif
#else
   std::cout << "<note>The long double tests have been disabled on this platform "
      "either because the long double overloads of the usual math functions are "
      "not available at all, or because they are too inaccurate for these tests "
      "to pass.</note>" << std::cout;
#endif
   return 0;
}



