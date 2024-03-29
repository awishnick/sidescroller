/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Olaf Krzikalla 2004-2006.
// (C) Copyright Ion Gaztanaga  2006-2009.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////
#include <boost/intrusive/detail/config_begin.hpp>
#include <boost/intrusive/unordered_set.hpp>
#include <boost/intrusive/pointer_traits.hpp>
#include "itestvalue.hpp"
#include "smart_ptr.hpp"
#include "common_functors.hpp"
#include <vector>
#include <algorithm> //std::sort std::find
#include <set>
#include <boost/detail/lightweight_test.hpp>
#include "test_macros.hpp"
#include "test_container.hpp"

namespace boost { namespace intrusive { namespace test {

#if !defined (BOOST_INTRUSIVE_VARIADIC_TEMPLATES)
template<class T, class O1, class O2, class O3, class O4, class O5, class O6>
#else
template<class T, class ...Options>
#endif
struct is_unordered<boost::intrusive::unordered_multiset<T,
   #if !defined (BOOST_INTRUSIVE_VARIADIC_TEMPLATES)
   O1, O2, O3, O4, O5, O6
   #else
   Options...
   #endif
> >
{
   static const bool value = true;
};

}}}

using namespace boost::intrusive;

struct my_tag;

template<class VoidPointer>
struct hooks
{
   typedef unordered_set_base_hook<void_pointer<VoidPointer> >    base_hook_type;
   typedef unordered_set_base_hook
      < link_mode<auto_unlink>
      , void_pointer<VoidPointer>
      , tag<my_tag>
      , store_hash<true>
      >                                                           auto_base_hook_type;

   typedef unordered_set_member_hook
      < void_pointer<VoidPointer>
      , optimize_multikey<true>
      >                                                           member_hook_type;
   typedef unordered_set_member_hook
      < link_mode<auto_unlink>, void_pointer<VoidPointer>
      , store_hash<true>
      , optimize_multikey<true>
      >                                                           auto_member_hook_type;
};

static const std::size_t BucketSize = 8;

template<class ValueTraits, bool CacheBegin, bool CompareHash, bool Incremental>
struct test_unordered_multiset
{
   typedef typename ValueTraits::value_type value_type;
   static void test_all (std::vector<value_type>& values);
   static void test_sort(std::vector<value_type>& values);
   static void test_insert(std::vector<value_type>& values);
   static void test_swap(std::vector<value_type>& values);
   static void test_rehash(std::vector<value_type>& values, detail::true_);
   static void test_rehash(std::vector<value_type>& values, detail::false_);
   static void test_find(std::vector<value_type>& values);
   static void test_impl();
   static void test_clone(std::vector<value_type>& values);
};

template<class ValueTraits, bool CacheBegin, bool CompareHash, bool Incremental>
void test_unordered_multiset<ValueTraits, CacheBegin, CompareHash, Incremental>::
   test_all (std::vector<typename ValueTraits::value_type>& values)
{
   typedef typename ValueTraits::value_type value_type;
   typedef unordered_multiset
      < value_type
      , value_traits<ValueTraits>
      , constant_time_size<value_type::constant_time_size>
      , cache_begin<CacheBegin>
      , compare_hash<CompareHash>
      , incremental<Incremental>
      > unordered_multiset_type;
   {
      typedef typename unordered_multiset_type::bucket_traits bucket_traits;
      typename unordered_multiset_type::bucket_type buckets [BucketSize];
      unordered_multiset_type testset
         (bucket_traits(pointer_traits<typename unordered_multiset_type::bucket_ptr>::
            pointer_to(buckets[0]), BucketSize));
      testset.insert(values.begin(), values.end());
      test::test_container(testset);
      testset.clear();
      testset.insert(values.begin(), values.end());
      test::test_common_unordered_and_associative_container(testset, values);
      testset.clear();
      testset.insert(values.begin(), values.end());
      test::test_unordered_associative_container(testset, values);
      testset.clear();
      testset.insert(values.begin(), values.end());
      test::test_non_unique_container(testset, values);
   }
   test_sort(values);
   test_insert(values);
   test_swap(values);
   test_rehash(values, detail::bool_<Incremental>());
   test_find(values);
   test_impl();
   test_clone(values);
}

//test case due to an error in tree implementation:
template<class ValueTraits, bool CacheBegin, bool CompareHash, bool Incremental>
void test_unordered_multiset<ValueTraits, CacheBegin, CompareHash, Incremental>
   ::test_impl()
{
   typedef typename ValueTraits::value_type value_type;
   typedef unordered_multiset
      <value_type
      , value_traits<ValueTraits>
      , constant_time_size<value_type::constant_time_size>
      , cache_begin<CacheBegin>
      , compare_hash<CompareHash>
      , incremental<Incremental>
      > unordered_multiset_type;
   typedef typename unordered_multiset_type::bucket_traits bucket_traits;

   std::vector<value_type> values (5);
   for (int i = 0; i < 5; ++i)
      values[i].value_ = i; 

   typename unordered_multiset_type::bucket_type buckets [BucketSize];
   unordered_multiset_type testset(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets[0]), BucketSize));

   for (int i = 0; i < 5; ++i)
      testset.insert (values[i]);

   testset.erase (testset.iterator_to (values[0]));
   testset.erase (testset.iterator_to (values[1]));
   testset.insert (values[1]);
     
   testset.erase (testset.iterator_to (values[2]));
   testset.erase (testset.iterator_to (values[3]));
}

//test: constructor, iterator, clear, reverse_iterator, front, back, size:
template<class ValueTraits, bool CacheBegin, bool CompareHash, bool Incremental>
void test_unordered_multiset<ValueTraits, CacheBegin, CompareHash, Incremental>
   ::test_sort(std::vector<typename ValueTraits::value_type>& values)
{
   typedef typename ValueTraits::value_type value_type;
   typedef unordered_multiset
      <value_type
      , value_traits<ValueTraits>
      , constant_time_size<value_type::constant_time_size>
      , cache_begin<CacheBegin>
      , compare_hash<CompareHash>
      , incremental<Incremental>
      > unordered_multiset_type;
   typedef typename unordered_multiset_type::bucket_traits bucket_traits;

   typename unordered_multiset_type::bucket_type buckets [BucketSize];
   unordered_multiset_type testset1
      (values.begin(), values.end(), bucket_traits
         (pointer_traits<typename unordered_multiset_type::bucket_ptr>::
            pointer_to(buckets[0]), BucketSize));

   if(Incremental){
      {  int init_values [] = { 4, 5, 1, 2, 2, 3 };
         TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }
   }
   else{
      {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
         TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }
   }
   testset1.clear();
   BOOST_TEST (testset1.empty());
}  

//test: insert, const_iterator, const_reverse_iterator, erase, iterator_to:
template<class ValueTraits, bool CacheBegin, bool CompareHash, bool Incremental>
void test_unordered_multiset<ValueTraits, CacheBegin, CompareHash, Incremental>
   ::test_insert(std::vector<typename ValueTraits::value_type>& values)
{
   typedef typename ValueTraits::value_type value_type;
   typedef unordered_multiset
      <value_type
      , value_traits<ValueTraits>
      , constant_time_size<value_type::constant_time_size>
      , cache_begin<CacheBegin>
      , compare_hash<CompareHash>
      , incremental<Incremental>
      > unordered_multiset_type;
   typedef typename unordered_multiset_type::bucket_traits bucket_traits;
   typedef typename unordered_multiset_type::iterator iterator;
   typename unordered_multiset_type::bucket_type buckets [BucketSize];
   unordered_multiset_type testset(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
            pointer_to(buckets[0]), BucketSize));

   testset.insert(&values[0] + 2, &values[0] + 5);

   const unordered_multiset_type& const_testset = testset;

   if(Incremental){
      {
         {  int init_values [] = { 4, 5, 1 };
            TEST_INTRUSIVE_SEQUENCE( init_values, const_testset.begin() );  }

         typename unordered_multiset_type::iterator i = testset.begin();
         BOOST_TEST (i->value_ == 4);

         i = testset.insert (values[0]);
         BOOST_TEST (&*i == &values[0]);
            
         i = testset.iterator_to (values[2]);
         BOOST_TEST (&*i == &values[2]);
         testset.erase(i);

         {  int init_values [] = { 5, 1, 3 };
            TEST_INTRUSIVE_SEQUENCE( init_values, const_testset.begin() );  }
         testset.clear();
         testset.insert(&values[0], &values[0] + values.size());

         {  int init_values [] = { 4, 5, 1, 2, 2, 3 };
            TEST_INTRUSIVE_SEQUENCE( init_values, const_testset.begin() );  }

         BOOST_TEST (testset.erase(1) == 1);
         BOOST_TEST (testset.erase(2) == 2);
         BOOST_TEST (testset.erase(3) == 1);
         BOOST_TEST (testset.erase(4) == 1);
         BOOST_TEST (testset.erase(5) == 1);
         BOOST_TEST (testset.empty() == true);

         //Now with a single bucket
         typename unordered_multiset_type::bucket_type single_bucket[1];
         unordered_multiset_type testset2(bucket_traits(
            pointer_traits<typename unordered_multiset_type::bucket_ptr>::
               pointer_to(single_bucket[0]), 1));
         testset2.insert(&values[0], &values[0] + values.size());
         BOOST_TEST (testset2.erase(5) == 1);
         BOOST_TEST (testset2.erase(2) == 2);
         BOOST_TEST (testset2.erase(1) == 1);
         BOOST_TEST (testset2.erase(4) == 1);
         BOOST_TEST (testset2.erase(3) == 1);
         BOOST_TEST (testset2.empty() == true);
      }
   }
   else{
      {
         {  int init_values [] = { 1, 4, 5 };
            TEST_INTRUSIVE_SEQUENCE( init_values, const_testset.begin() );  }

         typename unordered_multiset_type::iterator i = testset.begin();
         BOOST_TEST (i->value_ == 1);

         i = testset.insert (values[0]);
         BOOST_TEST (&*i == &values[0]);
            
         i = testset.iterator_to (values[2]);
         BOOST_TEST (&*i == &values[2]);
         testset.erase(i);

         {  int init_values [] = { 1, 3, 5 };
            TEST_INTRUSIVE_SEQUENCE( init_values, const_testset.begin() );  }
         testset.clear();
         testset.insert(&values[0], &values[0] + values.size());

         {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
            TEST_INTRUSIVE_SEQUENCE( init_values, const_testset.begin() );  }

         BOOST_TEST (testset.erase(1) == 1);
         BOOST_TEST (testset.erase(2) == 2);
         BOOST_TEST (testset.erase(3) == 1);
         BOOST_TEST (testset.erase(4) == 1);
         BOOST_TEST (testset.erase(5) == 1);
         BOOST_TEST (testset.empty() == true);

         //Now with a single bucket
         typename unordered_multiset_type::bucket_type single_bucket[1];
         unordered_multiset_type testset2(bucket_traits(
            pointer_traits<typename unordered_multiset_type::bucket_ptr>::
               pointer_to(single_bucket[0]), 1));
         testset2.insert(&values[0], &values[0] + values.size());
         BOOST_TEST (testset2.erase(5) == 1);
         BOOST_TEST (testset2.erase(2) == 2);
         BOOST_TEST (testset2.erase(1) == 1);
         BOOST_TEST (testset2.erase(4) == 1);
         BOOST_TEST (testset2.erase(3) == 1);
         BOOST_TEST (testset2.empty() == true);
      }
   }
   {
      //Now erase just one per loop
      const int random_init[] = { 3, 2, 4, 1, 5, 2, 2 };
      const unsigned int random_size = sizeof(random_init)/sizeof(random_init[0]);
      typename unordered_multiset_type::bucket_type single_bucket[1];
      for(unsigned int i = 0, max = random_size; i != max; ++i){
         std::vector<typename ValueTraits::value_type> data (random_size);
         for (unsigned int j = 0; j < random_size; ++j)
            data[j].value_ = random_init[j]; 
         unordered_multiset_type testset_new(bucket_traits(
            pointer_traits<typename unordered_multiset_type::bucket_ptr>::
               pointer_to(single_bucket[0]), 1));
         testset_new.insert(&data[0], &data[0]+max);
         testset_new.erase(testset_new.iterator_to(data[i]));
         BOOST_TEST (testset_new.size() == (max -1));
      }
   }
   {
      typename unordered_multiset_type::bucket_type buckets [BucketSize];
      const unsigned int NumBucketSize = BucketSize;
      const unsigned int LoadFactor    = 3;
      const unsigned int NumIterations = NumBucketSize*LoadFactor;
      std::vector<value_type> random_init(NumIterations);//Preserve memory
      std::vector<value_type> set_tester;
      set_tester.reserve(NumIterations);

      //Initialize values
      for (unsigned int i = 0; i < NumIterations; ++i){
         random_init[i].value_ = i*2;//(i/LoadFactor)*LoadFactor;
      }

      for(unsigned int initial_pos = 0; initial_pos != (NumIterations+1); ++initial_pos){
         for(unsigned int final_pos = initial_pos; final_pos != (NumIterations+1); ++final_pos){

            //Create intrusive container inserting values
            unordered_multiset_type testset
               ( &random_init[0]
               , &random_init[0] + random_init.size()
               , bucket_traits(pointer_traits<typename unordered_multiset_type::bucket_ptr>::
                  pointer_to(buckets[0]), NumBucketSize));

            BOOST_TEST (testset.size() == random_init.size());

            //Obtain the iterator range to erase
            iterator it_beg_pos = testset.begin();
            for(unsigned int it_beg_pos_num = 0; it_beg_pos_num != initial_pos; ++it_beg_pos_num){
               ++it_beg_pos;
            }
            iterator it_end_pos(it_beg_pos);
            for(unsigned int it_end_pos_num = 0; it_end_pos_num != (final_pos - initial_pos); ++it_end_pos_num){
               ++it_end_pos;
            }

            //Erase the same values in both the intrusive and original vector
            std::size_t erased_cnt = std::distance(it_beg_pos, it_end_pos);

            //Erase values from the intrusive container
            testset.erase(it_beg_pos, it_end_pos);

            BOOST_TEST (testset.size() == (random_init.size()-(final_pos - initial_pos)));

            //Now test...
            BOOST_TEST ((random_init.size() - erased_cnt) == testset.size());

            //Create an ordered copy of the intrusive container
            set_tester.insert(set_tester.end(), testset.begin(), testset.end());
            std::sort(set_tester.begin(), set_tester.end());
            {
               typename std::vector<value_type>::iterator it = set_tester.begin(), itend = set_tester.end();
               typename std::vector<value_type>::iterator random_init_it(random_init.begin());
               for( ; it != itend; ++it){
                  while(!random_init_it->is_linked())
                     ++random_init_it;
                  BOOST_TEST(*it == *random_init_it);
                  ++random_init_it;
               }
            }
            set_tester.clear();
         }
      }
   }
}

//test: insert (seq-version), swap, erase (seq-version), size:
template<class ValueTraits, bool CacheBegin, bool CompareHash, bool Incremental>
void test_unordered_multiset<ValueTraits, CacheBegin, CompareHash, Incremental>::
   test_swap(std::vector<typename ValueTraits::value_type>& values)
{
   typedef typename ValueTraits::value_type value_type;
   typedef unordered_multiset
      <value_type
      , value_traits<ValueTraits>
      , constant_time_size<value_type::constant_time_size>
      , cache_begin<CacheBegin>
      , compare_hash<CompareHash>
      , incremental<Incremental>
      > unordered_multiset_type;
   typedef typename unordered_multiset_type::bucket_traits bucket_traits;
   typename unordered_multiset_type::bucket_type buckets [BucketSize];

   typename unordered_multiset_type::bucket_type buckets2 [BucketSize];
   unordered_multiset_type testset1(&values[0], &values[0] + 2,
      bucket_traits(pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets[0]), BucketSize));
   unordered_multiset_type testset2(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
                  pointer_to(buckets2[0]), BucketSize));

   testset2.insert (&values[0] + 2, &values[0] + 6);
   testset1.swap (testset2);

   if(Incremental){
      {  int init_values [] = { 4, 5, 1, 2 };
         TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

      {  int init_values [] = { 2, 3 };
         TEST_INTRUSIVE_SEQUENCE( init_values, testset2.begin() );  }
      testset1.erase (testset1.iterator_to(values[4]), testset1.end());
      BOOST_TEST (testset1.size() == 1);
      //  BOOST_TEST (&testset1.front() == &values[3]);
      BOOST_TEST (&*testset1.begin() == &values[2]);
   }
   else{
      {  int init_values [] = { 1, 2, 4, 5 };
         TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

      {  int init_values [] = { 2, 3 };
         TEST_INTRUSIVE_SEQUENCE( init_values, testset2.begin() );  }
      testset1.erase (testset1.iterator_to(values[5]), testset1.end());
      BOOST_TEST (testset1.size() == 1);
      //  BOOST_TEST (&testset1.front() == &values[3]);
      BOOST_TEST (&*testset1.begin() == &values[3]);
   }
}  



//test: rehash:

template<class ValueTraits, bool CacheBegin, bool CompareHash, bool Incremental>
void test_unordered_multiset<ValueTraits, CacheBegin, CompareHash, Incremental>
   ::test_rehash(std::vector<typename ValueTraits::value_type>& values, detail::true_)
{
   typedef typename ValueTraits::value_type value_type;
   typedef unordered_multiset
      <value_type
      , value_traits<ValueTraits>
      , constant_time_size<value_type::constant_time_size>
      , cache_begin<CacheBegin>
      , compare_hash<CompareHash>
      , incremental<Incremental>
      > unordered_multiset_type;
   typedef typename unordered_multiset_type::bucket_traits bucket_traits;
   //Build a uset
   typename unordered_multiset_type::bucket_type buckets1 [BucketSize];
   typename unordered_multiset_type::bucket_type buckets2 [BucketSize*2];
   unordered_multiset_type testset1(&values[0], &values[0] + values.size(),
      bucket_traits(pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets1[0]), BucketSize));
   //Test current state
   BOOST_TEST(testset1.split_count() == BucketSize/2);
   {  int init_values [] = { 4, 5, 1, 2, 2, 3 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }
   //Incremental rehash step
   BOOST_TEST (testset1.incremental_rehash() == true);
   BOOST_TEST(testset1.split_count() == (BucketSize/2+1));
   {  int init_values [] = { 5, 1, 2, 2, 3, 4 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }
   //Rest of incremental rehashes should lead to the same sequence
   for(std::size_t split_bucket = testset1.split_count(); split_bucket != BucketSize; ++split_bucket){
      BOOST_TEST (testset1.incremental_rehash() == true);
      BOOST_TEST(testset1.split_count() == (split_bucket+1));
      {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }
   }
   //This incremental rehash should fail because we've reached the end of the bucket array
   BOOST_TEST (testset1.incremental_rehash() == false);
   BOOST_TEST(testset1.split_count() == BucketSize);
   {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
   TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

   //
   //Try incremental hashing specifying a new bucket traits pointing to the same array
   //
   //This incremental rehash should fail because the new size is not twice the original
   BOOST_TEST(testset1.incremental_rehash(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
                              pointer_to(buckets1[0]), BucketSize)) == false);
   BOOST_TEST(testset1.split_count() == BucketSize);
   {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
   TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

   //This incremental rehash should success because the new size is twice the original
   //and split_count is the same as the old bucket count
   BOOST_TEST(testset1.incremental_rehash(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets1[0]), BucketSize*2)) == true);
   BOOST_TEST(testset1.split_count() == BucketSize);
   {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
   TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

   //This incremental rehash should also success because the new size is half the original
   //and split_count is the same as the new bucket count
   BOOST_TEST(testset1.incremental_rehash(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets1[0]), BucketSize)) == true);
   BOOST_TEST(testset1.split_count() == BucketSize);
   {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
   TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

   //
   //Try incremental hashing specifying a new bucket traits pointing to the same array
   //
   //This incremental rehash should fail because the new size is not twice the original
   BOOST_TEST(testset1.incremental_rehash(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
               pointer_to(buckets2[0]), BucketSize)) == false);
   BOOST_TEST(testset1.split_count() == BucketSize);
   {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
   TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

   //This incremental rehash should success because the new size is twice the original
   //and split_count is the same as the old bucket count
   BOOST_TEST(testset1.incremental_rehash(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
                     pointer_to(buckets2[0]), BucketSize*2)) == true);
   BOOST_TEST(testset1.split_count() == BucketSize);
   {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
   TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

   //This incremental rehash should also success because the new size is half the original
   //and split_count is the same as the new bucket count
   BOOST_TEST(testset1.incremental_rehash(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
                           pointer_to(buckets1[0]), BucketSize)) == true);
   BOOST_TEST(testset1.split_count() == BucketSize);
   {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
   TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

   //Full shrink rehash
   testset1.rehash(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets1[0]), 4));
   BOOST_TEST (testset1.size() == values.size());
   BOOST_TEST (testset1.incremental_rehash() == false);
   {  int init_values [] = { 4, 5, 1, 2, 2, 3 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }
   //Full shrink rehash again
   testset1.rehash(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets1[0]), 2));
   BOOST_TEST (testset1.size() == values.size());
   BOOST_TEST (testset1.incremental_rehash() == false);
   {  int init_values [] = { 2, 2, 4, 3, 5, 1 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }
   //Full growing rehash
   testset1.rehash(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets1[0]), BucketSize));
   BOOST_TEST (testset1.size() == values.size());
   BOOST_TEST (testset1.incremental_rehash() == false);
   {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }
   //Incremental rehash shrinking
   //First incremental rehashes should lead to the same sequence
   for(std::size_t split_bucket = testset1.split_count(); split_bucket > 6; --split_bucket){
      BOOST_TEST (testset1.incremental_rehash(false) == true);
      BOOST_TEST(testset1.split_count() == (split_bucket-1));
      {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }
   }
   //Incremental rehash step
   BOOST_TEST (testset1.incremental_rehash(false) == true);
   BOOST_TEST(testset1.split_count() == (BucketSize/2+1));
   {  int init_values [] = { 5, 1, 2, 2, 3, 4 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }
   //Incremental rehash step 2
   BOOST_TEST (testset1.incremental_rehash(false) == true);
   BOOST_TEST(testset1.split_count() == (BucketSize/2));
   {  int init_values [] = { 4, 5, 1, 2, 2, 3 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }
   //This incremental rehash should fail because we've reached the half of the bucket array
   BOOST_TEST(testset1.incremental_rehash(false) == false);
   BOOST_TEST(testset1.split_count() == BucketSize/2);
   {  int init_values [] = { 4, 5, 1, 2, 2, 3 };
   TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }
}
template<class ValueTraits, bool CacheBegin, bool CompareHash, bool Incremental>
void test_unordered_multiset<ValueTraits, CacheBegin, CompareHash, Incremental>
   ::test_rehash(std::vector<typename ValueTraits::value_type>& values, detail::false_)
{
   typedef typename ValueTraits::value_type value_type;
   typedef unordered_multiset
      <value_type
      , value_traits<ValueTraits>
      , constant_time_size<value_type::constant_time_size>
      , cache_begin<CacheBegin>
      , compare_hash<CompareHash>
      , incremental<Incremental>
      > unordered_multiset_type;
   typedef typename unordered_multiset_type::bucket_traits bucket_traits;

   typename unordered_multiset_type::bucket_type buckets1 [BucketSize];
   typename unordered_multiset_type::bucket_type buckets2 [2];
   typename unordered_multiset_type::bucket_type buckets3 [BucketSize*2];

   unordered_multiset_type testset1(&values[0], &values[0] + 6, bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets1[0]), BucketSize));
   BOOST_TEST (testset1.size() == values.size());
   {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

   testset1.rehash(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets2[0]), 2));
   BOOST_TEST (testset1.size() == values.size());
   {  int init_values [] = { 4, 2, 2, 5, 3, 1 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

   testset1.rehash(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets3[0]), BucketSize*2));
   BOOST_TEST (testset1.size() == values.size());
   {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

   //Now rehash reducing the buckets
   testset1.rehash(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets3[0]), 2));
   BOOST_TEST (testset1.size() == values.size());
   {  int init_values [] = { 4, 2, 2, 5, 3, 1 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

   //Now rehash increasing the buckets
   testset1.rehash(bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets3[0]), BucketSize*2));
   BOOST_TEST (testset1.size() == values.size());
   {  int init_values [] = { 1, 2, 2, 3, 4, 5 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }
}  

//test: find, equal_range (lower_bound, upper_bound):
template<class ValueTraits, bool CacheBegin, bool CompareHash, bool Incremental>
void test_unordered_multiset<ValueTraits, CacheBegin, CompareHash, Incremental>::
   test_find(std::vector<typename ValueTraits::value_type>& values)
{
   typedef typename ValueTraits::value_type value_type;
   typedef unordered_multiset
      <value_type
      , value_traits<ValueTraits>
      , constant_time_size<value_type::constant_time_size>
      , cache_begin<CacheBegin>
      , compare_hash<CompareHash>
      , incremental<Incremental>
      > unordered_multiset_type;
   typedef typename unordered_multiset_type::bucket_traits bucket_traits;

   typename unordered_multiset_type::bucket_type buckets[BucketSize];
   unordered_multiset_type testset(values.begin(), values.end(), bucket_traits(
      pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets[0]), BucketSize));

   typedef typename unordered_multiset_type::iterator iterator;

   value_type cmp_val;
   cmp_val.value_ = 2;
   iterator i = testset.find (cmp_val);
   BOOST_TEST (i->value_ == 2);
   BOOST_TEST ((++i)->value_ == 2);
   std::pair<iterator,iterator> range = testset.equal_range (cmp_val);
     
   BOOST_TEST (range.first->value_ == 2);
   BOOST_TEST (range.second->value_ == 3);
   BOOST_TEST (std::distance (range.first, range.second) == 2);

   cmp_val.value_ = 7;
   BOOST_TEST (testset.find (cmp_val) == testset.end());
} 


template<class ValueTraits, bool CacheBegin, bool CompareHash, bool Incremental>
void test_unordered_multiset<ValueTraits, CacheBegin, CompareHash, Incremental>
   ::test_clone(std::vector<typename ValueTraits::value_type>& values)
{
   typedef typename ValueTraits::value_type value_type;
   typedef unordered_multiset
      <value_type
      , value_traits<ValueTraits>
      , constant_time_size<value_type::constant_time_size>
      , cache_begin<CacheBegin>
      , compare_hash<CompareHash>
      , incremental<Incremental>
      > unordered_multiset_type;
   typedef typename unordered_multiset_type::bucket_traits bucket_traits;
   {
      //Test with equal bucket arrays
      typename unordered_multiset_type::bucket_type buckets1 [BucketSize];
      typename unordered_multiset_type::bucket_type buckets2 [BucketSize];
      unordered_multiset_type testset1 (values.begin(), values.end(), bucket_traits(
         pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets1[0]), BucketSize));
      unordered_multiset_type testset2 (bucket_traits(
         pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets2[0]), BucketSize));

      testset2.clone_from(testset1, test::new_cloner<value_type>(), test::delete_disposer<value_type>());
      //Ordering is not guarantee in the cloning so insert data in a set and test
      std::multiset<typename ValueTraits::value_type>
         src(testset1.begin(), testset1.end());
      std::multiset<typename ValueTraits::value_type>
         dst(testset2.begin(), testset2.end());
      BOOST_TEST (src.size() == dst.size() && std::equal(src.begin(), src.end(), dst.begin()));
      testset2.clear_and_dispose(test::delete_disposer<value_type>());
      BOOST_TEST (testset2.empty());
   }
   {
      //Test with bigger source bucket arrays
      typename unordered_multiset_type::bucket_type buckets1 [BucketSize*2];
      typename unordered_multiset_type::bucket_type buckets2 [BucketSize];
      unordered_multiset_type testset1 (values.begin(), values.end(), bucket_traits(
         pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets1[0]), BucketSize*2));
      unordered_multiset_type testset2 (bucket_traits(
         pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets2[0]), BucketSize));

      testset2.clone_from(testset1, test::new_cloner<value_type>(), test::delete_disposer<value_type>());
      //Ordering is not guarantee in the cloning so insert data in a set and test
      std::multiset<typename ValueTraits::value_type>
         src(testset1.begin(), testset1.end());
      std::multiset<typename ValueTraits::value_type>
         dst(testset2.begin(), testset2.end());
      BOOST_TEST (src.size() == dst.size() && std::equal(src.begin(), src.end(), dst.begin()));
      testset2.clear_and_dispose(test::delete_disposer<value_type>());
      BOOST_TEST (testset2.empty());
   }
   {
      //Test with smaller source bucket arrays
      typename unordered_multiset_type::bucket_type buckets1 [BucketSize];
      typename unordered_multiset_type::bucket_type buckets2 [BucketSize*2];
      unordered_multiset_type testset1 (values.begin(), values.end(), bucket_traits(
         pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets1[0]), BucketSize));
      unordered_multiset_type testset2 (bucket_traits(
         pointer_traits<typename unordered_multiset_type::bucket_ptr>::
         pointer_to(buckets2[0]), BucketSize*2));

      testset2.clone_from(testset1, test::new_cloner<value_type>(), test::delete_disposer<value_type>());
      //Ordering is not guaranteed in the cloning so insert data in a set and test
      std::multiset<typename ValueTraits::value_type>
         src(testset1.begin(), testset1.end());
      std::multiset<typename ValueTraits::value_type>
         dst(testset2.begin(), testset2.end());
      BOOST_TEST (src.size() == dst.size() && std::equal(src.begin(), src.end(), dst.begin()));
      testset2.clear_and_dispose(test::delete_disposer<value_type>());
      BOOST_TEST (testset2.empty());
   }
}

template<class VoidPointer, bool constant_time_size, bool Incremental>
class test_main_template
{
   public:
   int operator()()
   {
      typedef testvalue<hooks<VoidPointer> , constant_time_size> value_type;
      static const int random_init[6] = { 3, 2, 4, 1, 5, 2 };
      std::vector<testvalue<hooks<VoidPointer> , constant_time_size> > data (6);
      for (int i = 0; i < 6; ++i)
         data[i].value_ = random_init[i]; 

      test_unordered_multiset < typename detail::get_base_value_traits
                  < value_type
                  , typename hooks<VoidPointer>::base_hook_type
                  >::type
                , true
                , false
                , Incremental
                >::test_all(data);

      test_unordered_multiset < typename detail::get_member_value_traits
                  < value_type
                  , member_hook< value_type
                               , typename hooks<VoidPointer>::member_hook_type
                               , &value_type::node_
                               >
                  >::type
                , false
                , false
                , Incremental
                >::test_all(data);
      return 0;
   }
};

template<class VoidPointer, bool Incremental>
class test_main_template<VoidPointer, false, Incremental>
{
   public:
   int operator()()
   {
      typedef testvalue<hooks<VoidPointer> , false> value_type;
      static const int random_init[6] = { 3, 2, 4, 1, 5, 2 };
      std::vector<testvalue<hooks<VoidPointer> , false> > data (6);
      for (int i = 0; i < 6; ++i)
         data[i].value_ = random_init[i]; 

      test_unordered_multiset < typename detail::get_base_value_traits
                  < value_type
                  , typename hooks<VoidPointer>::base_hook_type
                  >::type
                , false
                , false
                , Incremental
                >::test_all(data);

      test_unordered_multiset < typename detail::get_member_value_traits
                  < value_type
                  , member_hook< value_type
                               , typename hooks<VoidPointer>::member_hook_type
                               , &value_type::node_
                               >
                  >::type
                , true
                , false
                , Incremental
                >::test_all(data);

      test_unordered_multiset < typename detail::get_base_value_traits
                  < value_type
                  , typename hooks<VoidPointer>::auto_base_hook_type
                  >::type
                , false
                , false
                , Incremental
                >::test_all(data);

      test_unordered_multiset < typename detail::get_member_value_traits
                  < value_type
                  , member_hook< value_type
                               , typename hooks<VoidPointer>::auto_member_hook_type
                               , &value_type::auto_node_
                               >
                  >::type
                , false
                , true
                , Incremental
                >::test_all(data);
      return 0;
   }
};

int main( int, char* [] ) 
{
   test_main_template<void*, false, true>()();
   test_main_template<smart_ptr<void>, false, true>()();
   test_main_template<void*, true, true>()();
   test_main_template<smart_ptr<void>, true, true>()();
   test_main_template<void*, false, false>()();
   test_main_template<smart_ptr<void>, false, false>()();
   test_main_template<void*, true, true>()();
   test_main_template<smart_ptr<void>, true, false>()();
   return boost::report_errors();
}

#include <boost/intrusive/detail/config_end.hpp>
