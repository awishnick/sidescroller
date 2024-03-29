//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2004-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_TEST_NAMED_RESOURCE_TEMPLATE_HEADER
#define BOOST_INTERPROCESS_TEST_NAMED_RESOURCE_TEMPLATE_HEADER

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/exceptions.hpp>
#include "boost_interprocess_check.hpp"
#include <iostream>
#include <typeinfo>
#include <boost/interprocess/creation_tags.hpp>

namespace boost { namespace interprocess { namespace test {

template <class NamedResource>
inline void create_then_open_then_open_or_create()
{  
   try{
      //Create it and open it twice
      NamedResource nresource1(create_only);
      NamedResource nresource2(open_only);
      NamedResource nresource3(open_or_create);
   }
   catch(...){
      //This shouldn't throw so show the error
      BOOST_INTERPROCES_CHECK( false );
   }
}

template <class NamedResource>
inline void open_or_create_then_create()
{  
   //Create it with open_or_create and try to create it twice
   NamedResource nresource1(open_or_create);
   try{
      NamedResource nresource2(create_only);
   }
   catch(interprocess_exception &err){
      BOOST_INTERPROCES_CHECK(err.get_error_code() == already_exists_error);
   }
}

template <class NamedResource>
inline void dont_create_and_open()
{  
   //Try to open it without creating
   try{
      NamedResource nresource1(open_only);
   }
   catch(interprocess_exception &err){
      BOOST_INTERPROCES_CHECK(err.get_error_code() == not_found_error);
      return;
   }
   //The mutex should not exist
   BOOST_INTERPROCES_CHECK(false);
}

template <class NamedResource>
inline void test_named_creation()
{
   std::cout   << "create_then_open_then_open_or_create<"
               << typeid(NamedResource).name() << ">" << std::endl;
   create_then_open_then_open_or_create<NamedResource>();
   std::cout   << "open_or_create_then_create<"
               << typeid(NamedResource).name() << ">" << std::endl;
   open_or_create_then_create<NamedResource>();
   std::cout   << "dont_create_and_open<" 
               << typeid(NamedResource).name() << ">" << std::endl;
   dont_create_and_open<NamedResource>();
}

}}}   //namespace boost { namespace interprocess { namespace test {

#include <boost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_TEST_NAMED_RESOURCE_TEMPLATE_HEADER
