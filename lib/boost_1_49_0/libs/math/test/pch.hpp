//  Copyright John Maddock 2008.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_BUILD_PCH_ENABLED

#define ONE <boost/math/special_functions.hpp>
#define TWO <boost/math/distributions.hpp>

#define BOOST_MATH_OVERFLOW_ERROR_POLICY ignore_error
#include <boost/test/test_exec_monitor.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include ONE
#include TWO
#include <boost/math/tools/test.hpp>
#include <boost/math/tools/stats.hpp>
#include <boost/type_traits.hpp>
#include <boost/array.hpp>
#include "functor.hpp"

#include "handle_test_result.hpp"

#endif
