//  (C) Copyright John Maddock 2006.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <pch_light.hpp>
#include "test_gamma.hpp"

//
// DESCRIPTION:
// ~~~~~~~~~~~~
//
// This file tests the functions tgamma and lgamma, and the 
// function tgamma1pm1.  There are two sets of tests, spot
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
   //
   // G++ on Darwin: results are just slightly worse than we might hope for
   // but still pretty good:
   //
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "Mac OS",                      // platform
      largest_type,                  // test type(s)
      "factorials",                  // test data group
      "boost::math::tgamma", 100, 15); // test function

   //
   // G++ on Linux, result vary a bit by processor type,
   // on Itanium results are *much* better than listed here,
   // but x86 appears to have much less accurate std::pow
   // that throws off the results:
   //
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "linux",                       // platform
      largest_type,                  // test type(s)
      "factorials",                  // test data group
      "boost::math::tgamma", 400, 200); // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "linux",                          // platform
      largest_type,                  // test type(s)
      "factorials",                  // test data group
      "boost::math::lgamma", 30, 10);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "linux",                       // platform
      largest_type,                  // test type(s)
      "near (1|2|-10)",              // test data group
      "boost::math::tgamma", 10, 5);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "linux",                       // platform
      largest_type,                  // test type(s)
      "near (1|2|-10)",              // test data group
      "boost::math::lgamma", 50, 50);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "linux",                       // platform
      largest_type,                  // test type(s)
      "tgamma1pm1.*",                // test data group
      "boost::math::tgamma1pm1", 50, 15);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "linux",                       // platform
      "real_concept",                // test type(s)
      "factorials",                  // test data group
      "boost::math::tgamma", 220, 70);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "linux",                       // platform
      "real_concept",                // test type(s)
      "near (0|-55)",                // test data group
      "boost::math::(t|l)gamma", 130, 80);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "linux",                       // platform
      "real_concept",                // test type(s)
      "tgamma1pm1.*",                // test data group
      "boost::math::tgamma1pm1", 40, 10);  // test function
   //
   // HP-UX results:
   //
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "HP-UX",                          // platform
      largest_type,                  // test type(s)
      "factorials",                  // test data group
      "boost::math::tgamma", 5, 4);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "HP-UX",                       // platform
      largest_type,                  // test type(s)
      "near (0|-55)",                // test data group
      "boost::math::tgamma", 10, 5);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "HP-UX",                       // platform
      largest_type,                  // test type(s)
      "near (1|2|-10)",              // test data group
      "boost::math::lgamma", 250, 200);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "HP-UX",                          // platform
      "real_concept",                // test type(s)
      "factorials",                  // test data group
      "boost::math::lgamma", 50, 20);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "HP-UX",                          // platform
      "real_concept",                // test type(s)
      "tgamma1pm1.*",                // test data group
      "boost::math::tgamma1pm1", 200, 80);  // test function
   //
   // Tru64:
   //
   add_expected_result(
      ".*Tru64.*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      "real_concept",                // test type(s)
      "factorials",                  // test data group
      "boost::math::lgamma", 50, 20);  // test function
   //
   // Sun OS:
   //
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "Sun.*",                       // platform
      largest_type,                  // test type(s)
      "factorials",                  // test data group
      "boost::math::tgamma", 300, 50); // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      "Sun.*",                       // platform
      "real_concept",                // test type(s)
      "factorials",                  // test data group
      "boost::math::tgamma", 300, 50); // test function

   //
   // Catch all cases come last:
   //
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      largest_type,                  // test type(s)
      "factorials",                  // test data group
      "boost::math::tgamma", 4, 1);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      largest_type,                  // test type(s)
      "factorials",                  // test data group
      "boost::math::lgamma", 9, 1);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      largest_type,                  // test type(s)
      "near (0|-55)",                // test data group
      "boost::math::(t|l)gamma", 200, 100);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      largest_type,                  // test type(s)
      "near (1|2|-10)",              // test data group
      "boost::math::tgamma", 10, 5);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      largest_type,                  // test type(s)
      "near (1|2|-10)",              // test data group
      "boost::math::lgamma", 14, 7);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      largest_type,                  // test type(s)
      "tgamma1pm1.*",                // test data group
      "boost::math::tgamma1pm1", 30, 9);  // test function

   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      "real_concept",                // test type(s)
      "factorials",                  // test data group
      "boost::math::tgamma", 70, 25);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      "real_concept",                // test type(s)
      "factorials",                  // test data group
      "boost::math::lgamma", 40, 4);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      "real_concept",                // test type(s)
      "near.*",                      // test data group
      "boost::math::tgamma", 80, 60);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      "real_concept",                // test type(s)
      "near.*",                      // test data group
      "boost::math::lgamma", 10000000, 10000000);  // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      "real_concept",                // test type(s)
      "tgamma1pm1.*",                // test data group
      "boost::math::tgamma1pm1", 20, 5);  // test function

   //
   // Finish off by printing out the compiler/stdlib/platform names,
   // we do this to make it easier to mark up expected error rates.
   //
   std::cout << "Tests run with " << BOOST_COMPILER << ", " 
      << BOOST_STDLIB << ", " << BOOST_PLATFORM << std::endl;
}

int test_main(int, char* [])
{
   expected_results();
   BOOST_MATH_CONTROL_FP;

#ifndef BOOST_MATH_BUGGY_LARGE_FLOAT_CONSTANTS
   test_spots(0.0F);
#endif
   test_spots(0.0);
#ifndef BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS
   test_spots(0.0L);
   test_spots(boost::math::concepts::real_concept(0.1));
#endif

#ifndef BOOST_MATH_BUGGY_LARGE_FLOAT_CONSTANTS
   test_gamma(0.1F, "float");
#endif
   test_gamma(0.1, "double");
#ifndef BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS
   test_gamma(0.1L, "long double");
#ifndef BOOST_MATH_NO_REAL_CONCEPT_TESTS
#if !BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x582))
   test_gamma(boost::math::concepts::real_concept(0.1), "real_concept");
#endif
#endif
#else
   std::cout << "<note>The long double tests have been disabled on this platform "
      "either because the long double overloads of the usual math functions are "
      "not available at all, or because they are too inaccurate for these tests "
      "to pass.</note>" << std::cout;
#endif
   return 0;
}



