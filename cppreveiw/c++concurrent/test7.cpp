#include <iostream>
#include <thread>
#include "help.h"

using namespace std;

class scoped_thread {
    std::thread t;
public:
    explicit scoped_thread(std::thread t_): t(std::move(t_)){
        if (!t.joinable())
            throw std::logic_error("No thread");
    }
    ~scoped_thread(){
        t.join();
    }
    scoped_thread(scoped_thread const &)=delete;
    scoped_thread& operator=(scoped_thread const&)=delete;

};


struct func {
    void operator()() {
        for (auto i = 0; i < 10; i++)
            cout << "do task ....." << i << endl;
            this_thread::sleep_for(std::chrono::microseconds(100));
    }
};

void do_my_current_task() {

    cout << "do_current_task...." << endl;
    for (auto i = 0; i < 10; i++) {
        cout << "current task " << i << endl;
        this_thread::sleep_for(std::chrono::microseconds(100));
    }

    throw MyException();
}

void fun() {
    func my_func;
    scoped_thread t{std::thread(my_func)};
    do_my_current_task();
}
thread::id master_thread;

void f1()
{
    if (std::this_thread::get_id() == master_thread)
        cout << "this is master thread." << endl;
    else {
        cout << "other thread to something" << endl;
    }
    cout << "master and other do something" << endl;
}

int main() {
    
    master_thread = std::this_thread::get_id();
    f1();
    thread t1(f1);
    t1.join();
    try
    {
        fun();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;

}










