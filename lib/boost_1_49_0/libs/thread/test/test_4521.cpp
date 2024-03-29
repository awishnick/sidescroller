#include <boost/thread.hpp>

int calculate_the_answer_to_life_the_universe_and_everything()
{
    return 42;
}

int main() {
boost::packaged_task<int> pt(calculate_the_answer_to_life_the_universe_and_everything);
boost::unique_future<int> fi=pt.get_future();

boost::thread task(boost::move(pt)); // launch task on a thread

fi.wait(); // wait for it to finish

//assert(fi.is_ready());
//assert(fi.has_value());
//assert(!fi.has_exception());
//assert(fi.get_state()==boost::future_state::ready);
//assert(fi.get()==42);
}
