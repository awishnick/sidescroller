#include <iostream>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/thread/future.hpp>

using namespace boost::posix_time;
using namespace boost;

int foo()
{
  this_thread::sleep(seconds(10));
  return 0;
}


int main(int argc, char** argv)
{
  boost::packaged_task<int> pt(&foo);
  boost::unique_future<int> fi = pt.get_future();
  boost::thread task(boost::move(pt)); // launch task on a thread

  task.interrupt();

  try
  {
    int v = fi.get();
  }
  catch (boost::thread_interrupted& exc)
  {
    std::cout << "OK: " << std::endl;
    return 0;
  }
  catch (boost::exception& exc)
  {
    std::cout << __LINE__ << " ERROR: " << boost::diagnostic_information(exc) << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cout << __LINE__ << " ERROR: " << std::endl;
    return 2;
  }
  std::cout << __LINE__ << " ERROR: " << std::endl;
  return 3;
}
