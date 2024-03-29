// Copyright (C) 2001-2003
// William E. Kempf
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  William E. Kempf makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.
//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_TEST_SHARABLE_MUTEX_TEST_TEMPLATE_HEADER
#define BOOST_INTERPROCESS_TEST_SHARABLE_MUTEX_TEST_TEMPLATE_HEADER

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include "boost_interprocess_check.hpp"
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>
#include <cassert>
#include "util.hpp"

namespace boost { namespace interprocess { namespace test {

template<typename SM>
void plain_exclusive(void *arg, SM &sm)
{
   data<SM> *pdata = static_cast<data<SM>*>(arg);
   boost::interprocess::scoped_lock<SM> l(sm);
   boost::thread::sleep(xsecs(3*BaseSeconds));
   shared_val += 10;
   pdata->m_value = shared_val;
}

template<typename SM>
void plain_shared(void *arg, SM &sm)
{
   data<SM> *pdata = static_cast<data<SM>*>(arg);
   boost::interprocess::sharable_lock<SM> l(sm);
   if(pdata->m_secs){
      boost::thread::sleep(xsecs(pdata->m_secs*BaseSeconds));
   }
   pdata->m_value = shared_val;
}

template<typename SM>
void try_exclusive(void *arg, SM &sm)
{
   data<SM> *pdata = static_cast<data<SM>*>(arg);
   boost::interprocess::scoped_lock<SM> l(sm, boost::interprocess::defer_lock);
   if (l.try_lock()){
      boost::thread::sleep(xsecs(3*BaseSeconds));
      shared_val += 10;
      pdata->m_value = shared_val;
   }
}

template<typename SM>
void try_shared(void *arg, SM &sm)
{
   data<SM> *pdata = static_cast<data<SM>*>(arg);
   boost::interprocess::sharable_lock<SM> l(sm, boost::interprocess::defer_lock);
   if (l.try_lock()){
      if(pdata->m_secs){
         boost::thread::sleep(xsecs(pdata->m_secs*BaseSeconds));
      }
      pdata->m_value = shared_val;
   }
}

template<typename SM>
void timed_exclusive(void *arg, SM &sm)
{
   data<SM> *pdata = static_cast<data<SM>*>(arg);
   boost::posix_time::ptime pt(delay(pdata->m_secs));
   boost::interprocess::scoped_lock<SM> 
      l (sm, boost::interprocess::defer_lock);
   if (l.timed_lock(pt)){
      boost::thread::sleep(xsecs(3*BaseSeconds));
      shared_val += 10;
      pdata->m_value = shared_val;
   }
}

template<typename SM>
void timed_shared(void *arg, SM &sm)
{
   data<SM> *pdata = static_cast<data<SM>*>(arg);
   boost::posix_time::ptime pt(delay(pdata->m_secs));
   boost::interprocess::sharable_lock<SM> 
      l(sm, boost::interprocess::defer_lock);
   if (l.timed_lock(pt)){
      if(pdata->m_secs){
         boost::thread::sleep(xsecs(pdata->m_secs*BaseSeconds));
      }
      pdata->m_value = shared_val;
   }
}

template<bool SameObject, typename SM>
void test_plain_sharable_mutex()
{
   {
      shared_val = 0;
      SM m1, m2, m3, m4;
      SM *pm1, *pm2, *pm3, *pm4;

      if(SameObject){
         pm1 = pm2 = pm3 = pm4 = &m1;
      }
      else{
         pm1 = &m1;
         pm2 = &m2;
         pm3 = &m3;
         pm4 = &m4;
      }
      data<SM> s1(1);
      data<SM> s2(2);
      data<SM> e1(1);
      data<SM> e2(2);

      // Writer one launches, holds the lock for 3*BaseSeconds seconds.
      boost::thread tw1(thread_adapter<SM>(plain_exclusive, &e1, *pm1));

      // Writer two launches, tries to grab the lock, "clearly"
      //  after Writer one will already be holding it.
      boost::thread::sleep(xsecs(1*BaseSeconds));
      boost::thread tw2(thread_adapter<SM>(plain_exclusive, &e2, *pm2));

      // Reader one launches, "clearly" after writer two, and "clearly"
      //   while writer 1 still holds the lock
      boost::thread::sleep(xsecs(1*BaseSeconds));
      boost::thread thr1(thread_adapter<SM>(plain_shared,&s1, *pm3));
      boost::thread thr2(thread_adapter<SM>(plain_shared,&s2, *pm4));

      thr2.join();
      thr1.join();
      tw2.join();
      tw1.join();

      //We can only assure that the writer will be first
      BOOST_INTERPROCES_CHECK(e1.m_value == 10);
      //A that we will execute all
      BOOST_INTERPROCES_CHECK(s1.m_value == 20 || s2.m_value == 20 || e2.m_value == 20);
   }

   {
      shared_val = 0;
      SM m1, m2, m3, m4;
      SM *pm1, *pm2, *pm3, *pm4;

      if(SameObject){
         pm1 = pm2 = pm3 = pm4 = &m1;
      }
      else{
         pm1 = &m1;
         pm2 = &m2;
         pm3 = &m3;
         pm4 = &m4;
      }
      data<SM> s1(1, 3);
      data<SM> s2(2, 3);
      data<SM> e1(1);
      data<SM> e2(2);

      //We launch 2 readers, that will block for 3*BaseTime seconds
      boost::thread thr1(thread_adapter<SM>(plain_shared,&s1,*pm1));
      boost::thread thr2(thread_adapter<SM>(plain_shared,&s2,*pm2));

      //Make sure they try to hold the sharable lock
      boost::thread::sleep(xsecs(1*BaseSeconds));

      // We launch two writers, that should block until the readers end
      boost::thread tw1(thread_adapter<SM>(plain_exclusive,&e1,*pm3));
      boost::thread tw2(thread_adapter<SM>(plain_exclusive,&e2,*pm4));

      thr2.join();
      thr1.join();
      tw2.join();
      tw1.join();

      //We can only assure that the shared will finish first...
      BOOST_INTERPROCES_CHECK(s1.m_value == 0 || s2.m_value == 0);
      //...and writers will be mutually excluded after readers
      BOOST_INTERPROCES_CHECK((e1.m_value == 10 && e2.m_value == 20) || 
             (e1.m_value == 20 && e2.m_value == 10) );
   }
}

template<bool SameObject, typename SM>
void test_try_sharable_mutex()
{
   SM m1, m2, m3;
   SM *pm1, *pm2, *pm3;

   if(SameObject){
      pm1 = pm2 = pm3 = &m1;
   }
   else{
      pm1 = &m1;
      pm2 = &m2;
      pm3 = &m3;
   }
   data<SM> s1(1);
   data<SM> e1(2);
   data<SM> e2(3);

   // We start with some specialized tests for "try" behavior

   shared_val = 0;

   // Writer one launches, holds the lock for 3*BaseSeconds seconds.

   boost::thread tw1(thread_adapter<SM>(try_exclusive,&e1,*pm1));

   // Reader one launches, "clearly" after writer #1 holds the lock
   //   and before it releases the lock.
   boost::thread::sleep(xsecs(1*BaseSeconds));
   boost::thread thr1(thread_adapter<SM>(try_shared,&s1,*pm2));

   // Writer two launches in the same timeframe.
   boost::thread tw2(thread_adapter<SM>(try_exclusive,&e2,*pm3));

   tw2.join();
   thr1.join();
   tw1.join();

   BOOST_INTERPROCES_CHECK(e1.m_value == 10);
   BOOST_INTERPROCES_CHECK(s1.m_value == -1);        // Try would return w/o waiting
   BOOST_INTERPROCES_CHECK(e2.m_value == -1);        // Try would return w/o waiting
}

template<bool SameObject, typename SM>
void test_timed_sharable_mutex()
{
   SM m1, m2, m3, m4;
   SM *pm1, *pm2, *pm3, *pm4;

   if(SameObject){
      pm1 = pm2 = pm3 = pm4 = &m1;
   }
   else{
      pm1 = &m1;
      pm2 = &m2;
      pm3 = &m3;
      pm4 = &m4;
   }
   data<SM> s1(1,1*BaseSeconds);
   data<SM> s2(2,3*BaseSeconds);
   data<SM> e1(3,3*BaseSeconds);
   data<SM> e2(4,1*BaseSeconds);

   // We begin with some specialized tests for "timed" behavior

   shared_val = 0;

   // Writer one will hold the lock for 3*BaseSeconds seconds.
   boost::thread tw1(thread_adapter<SM>(timed_exclusive,&e1,*pm1));

   boost::thread::sleep(xsecs(1*BaseSeconds));
   // Writer two will "clearly" try for the lock after the readers
   //  have tried for it.  Writer will wait up 1*BaseSeconds seconds for the lock.
   //  This write will fail.
   boost::thread tw2(thread_adapter<SM>(timed_exclusive,&e2,*pm2));

   // Readers one and two will "clearly" try for the lock after writer
   //   one already holds it.  1st reader will wait 1*BaseSeconds seconds, and will fail
   //   to get the lock.  2nd reader will wait 3*BaseSeconds seconds, and will get
   //   the lock.

   boost::thread thr1(thread_adapter<SM>(timed_shared,&s1,*pm3));
   boost::thread thr2(thread_adapter<SM>(timed_shared,&s2,*pm4));

   tw1.join();
   thr1.join();
   thr2.join();
   tw2.join();

   BOOST_INTERPROCES_CHECK(e1.m_value == 10);
   BOOST_INTERPROCES_CHECK(s1.m_value == -1);
   BOOST_INTERPROCES_CHECK(s2.m_value == 10);
   BOOST_INTERPROCES_CHECK(e2.m_value == -1);
}

template<bool SameObject, typename SM>
void test_all_sharable_mutex()
{
   std::cout << "test_plain_sharable_mutex<" << SameObject << ", " << typeid(SM).name() << ">" << std::endl;
   test_plain_sharable_mutex<SameObject, SM>();

   std::cout << "test_try_sharable_mutex<" << SameObject << ", " << typeid(SM).name() << ">" << std::endl;
   test_try_sharable_mutex<SameObject, SM>();

   std::cout << "test_timed_sharable_mutex<" << SameObject << ", " << typeid(SM).name() << ">" << std::endl;
   test_timed_sharable_mutex<SameObject, SM>();
}


}}}   //namespace boost { namespace interprocess { namespace test {

#include <boost/interprocess/detail/config_end.hpp>

#endif   //#ifndef BOOST_INTERPROCESS_TEST_SHARABLE_MUTEX_TEST_TEMPLATE_HEADER
