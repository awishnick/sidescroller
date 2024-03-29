#include <iostream>
#include <boost/thread.hpp>

class Worker
{
public:

    Worker()
    {
        // the thread is not-a-thread until we call start()
    }

    void start(int N)
    {
      std::cout << "start\n";
        m_Thread = boost::thread(&Worker::processQueue, this, N);
        std::cout << "started\n";
    }

    void join()
    {
        m_Thread.join();
    }

    void processQueue(unsigned N)
    {
        float ms = N * 1e3;
        boost::posix_time::milliseconds workTime(ms);

        std::cout << "Worker: started, will work for "
                  << ms << "ms"
                  << std::endl;

        // We're busy, honest!
        boost::this_thread::sleep(workTime);

        std::cout << "Worker: completed" << std::endl;
    }

private:

    boost::thread m_Thread;
};

int main(int argc, char* argv[])
{
    std::cout << "main: startup" << std::endl;

    Worker worker;

    std::cout << "main: create worker" << std::endl;

    worker.start(3);

    std::cout << "main: waiting for thread" << std::endl;

    worker.join();

    std::cout << "main: done" << std::endl;

    return 0;
}
