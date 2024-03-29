//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2004-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#include <boost/container/detail/config_begin.hpp>
#include <algorithm>
#include <memory>
#include <vector>
#include <iostream>
#include <functional>
#include <list>

#include <boost/move/move.hpp>
#include <boost/container/detail/mpl.hpp>
#include "print_container.hpp"
#include "check_equal_containers.hpp"
#include "movable_int.hpp"
#include <string>
#include <vector>
#include "emplace_test.hpp"

namespace boost{
namespace container {
namespace test{

template<class V1, class V2>
bool vector_copyable_only(V1 *, V2 *, boost::container::container_detail::false_type)
{
   return true;
}

//Function to check if both sets are equal
template<class V1, class V2>
bool vector_copyable_only(V1 *boostvector, V2 *stdvector, boost::container::container_detail::true_type)
{
   typedef typename V1::value_type IntType;
   std::size_t size = boostvector->size();
   boostvector->insert(boostvector->end(), 50, IntType(1));
   stdvector->insert(stdvector->end(), 50, 1);
   if(!test::CheckEqualContainers(boostvector, stdvector)) return false;

   {
      IntType move_me(1);
      boostvector->insert(boostvector->begin()+size/2, 50, boost::move(move_me));
      stdvector->insert(stdvector->begin()+size/2, 50, 1);
      if(!test::CheckEqualContainers(boostvector, stdvector)) return false;
   }
   {
      IntType move_me(2);
      boostvector->assign(boostvector->size()/2, boost::move(move_me));
      stdvector->assign(stdvector->size()/2, 2);
      if(!test::CheckEqualContainers(boostvector, stdvector)) return false;
   }
   {
      IntType move_me(3);
      boostvector->assign(boostvector->size()*3-1, boost::move(move_me));
      stdvector->assign(stdvector->size()*3-1, 3);
      if(!test::CheckEqualContainers(boostvector, stdvector)) return false;
   }

   {
      IntType copy_me(3);
      const IntType ccopy_me(3);
      boostvector->push_back(copy_me);
      stdvector->push_back(int(3));
      boostvector->push_back(ccopy_me);
      stdvector->push_back(int(3));
      if(!test::CheckEqualContainers(boostvector, stdvector)) return false;
   }

   return true;
}

template<class MyBoostVector>
int vector_test()
{
   typedef std::vector<int>                     MyStdVector;
   typedef typename MyBoostVector::value_type     IntType;
   const int max = 100;

   {
      try{
         MyBoostVector *boostvector = new MyBoostVector;
         MyStdVector *stdvector = new MyStdVector;
         boostvector->resize(100);
         stdvector->resize(100);
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;         

         boostvector->resize(200);
         stdvector->resize(200);
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;         

         boostvector->resize(0);
         stdvector->resize(0);
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;         

         for(int i = 0; i < max; ++i){
            IntType new_int(i);
            boostvector->insert(boostvector->end(), boost::move(new_int));
            stdvector->insert(stdvector->end(), i);
            if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;
         }
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;

         typename MyBoostVector::iterator boostit(boostvector->begin());
         typename MyStdVector::iterator stdit(stdvector->begin());
         typename MyBoostVector::const_iterator cboostit = boostit;
         (void)cboostit;
         ++boostit; ++stdit;
         boostvector->erase(boostit);
         stdvector->erase(stdit);
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;

         boostvector->erase(boostvector->begin());
         stdvector->erase(stdvector->begin());
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;

         {
            //Initialize values
            IntType aux_vect[50];
            for(int i = 0; i < 50; ++i){
               IntType new_int(-1);
               BOOST_STATIC_ASSERT((boost::container::test::is_copyable<boost::container::test::movable_int>::value == false));
               aux_vect[i] = boost::move(new_int);
            }
            int aux_vect2[50];
            for(int i = 0; i < 50; ++i){
               aux_vect2[i] = -1;
            }

            boostvector->insert(boostvector->end()
                              ,boost::make_move_iterator(&aux_vect[0])
                              ,boost::make_move_iterator(aux_vect + 50));
            stdvector->insert(stdvector->end(), aux_vect2, aux_vect2 + 50);
            if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;

            for(int i = 0, j = static_cast<int>(boostvector->size()); i < j; ++i){
               boostvector->erase(boostvector->begin());
               stdvector->erase(stdvector->begin());
            }
            if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;
         }
         {
            IntType aux_vect[50];
            for(int i = 0; i < 50; ++i){
               IntType new_int(-1);
               aux_vect[i] = boost::move(new_int);
            }
            int aux_vect2[50];
            for(int i = 0; i < 50; ++i){
               aux_vect2[i] = -1;
            }
            boostvector->insert(boostvector->begin()
                              ,boost::make_move_iterator(&aux_vect[0])
                              ,boost::make_move_iterator(aux_vect + 50));
            stdvector->insert(stdvector->begin(), aux_vect2, aux_vect2 + 50);
            if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;
         }
/*
         boostvector->reserve(boostvector->size()*2);
         stdvector->reserve(stdvector->size()*2);
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;
*/
         boostvector->shrink_to_fit();
         MyStdVector(*stdvector).swap(*stdvector);
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;

         boostvector->shrink_to_fit();
         MyStdVector(*stdvector).swap(*stdvector);
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;

         IntType push_back_this(1);
         boostvector->push_back(boost::move(push_back_this));
         stdvector->push_back(int(1));
         boostvector->push_back(IntType(1));
         stdvector->push_back(int(1));

         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;

         if(!vector_copyable_only(boostvector, stdvector
                        ,container_detail::bool_<boost::container::test::is_copyable<IntType>::value>())){
            return 1;
         }

         boostvector->erase(boostvector->begin());
         stdvector->erase(stdvector->begin());
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;

         for(int i = 0; i < max; ++i){
            IntType insert_this(i);
            boostvector->insert(boostvector->begin(), boost::move(insert_this));
            stdvector->insert(stdvector->begin(), i);
            boostvector->insert(boostvector->begin(), IntType(i));
            stdvector->insert(stdvector->begin(), int(i));
         }
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;

         //Test insertion from list
         {
            std::list<int> l(50, int(1));
            boostvector->insert(boostvector->begin(), l.begin(), l.end());
            stdvector->insert(stdvector->begin(), l.begin(), l.end());
            if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;
            boostvector->assign(l.begin(), l.end());
            stdvector->assign(l.begin(), l.end());
            if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;
         }
/*
         std::size_t cap = boostvector->capacity();
         boostvector->reserve(cap*2);
         stdvector->reserve(cap*2);
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;
         boostvector->resize(0);
         stdvector->resize(0);
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;

         boostvector->resize(cap*2);
         stdvector->resize(cap*2);
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;

         boostvector->clear();
         stdvector->clear();
         boostvector->shrink_to_fit();
         MyStdVector(*stdvector).swap(*stdvector);
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;

         boostvector->resize(cap*2);
         stdvector->resize(cap*2);
         if(!test::CheckEqualContainers(boostvector, stdvector)) return 1;
*/

         delete stdvector;
         delete boostvector;
      }
      catch(std::exception &ex){
         std::cout << ex.what() << std::endl;
         return 1;
      }
   }
   std::cout << std::endl << "Test OK!" << std::endl;
   return 0;
}

}  //namespace test{
}  //namespace container {
}  //namespace boost{

#include <boost/container/detail/config_end.hpp>
