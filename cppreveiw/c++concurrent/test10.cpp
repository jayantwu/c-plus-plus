// https://en.cppreference.com/w/cpp/thread/lock_tag_t

#include <mutex>
#include <thread>
#include <iostream>
 
struct bank_account {
    explicit bank_account(int balance) : balance{balance} {}
    int balance;
    std::mutex m;
};
 

// 转账
void transfer(bank_account &from, bank_account &to, int amount)
{
    if(&from == &to) return; // avoid deadlock in case of self transfer
 
    // lock both mutexes without deadlock
    std::lock(from.m, to.m);
    // make sure both already-locked mutexes are unlocked at the end of scope
    std::lock_guard<std::mutex> lock1{from.m, std::adopt_lock}; // 为了释放锁， 调用lock时， 已经把两个锁锁住， 保证这两个锁在离开作用域时自动释放
    std::lock_guard<std::mutex> lock2{to.m, std::adopt_lock};
 
// equivalent approach:
//    std::unique_lock<std::mutex> lock1{from.m, std::defer_lock}; // 推迟锁住这两个锁
//    std::unique_lock<std::mutex> lock2{to.m, std::defer_lock};
//    std::lock(lock1, lock2); //在这里把两个锁锁住， 离开作用域时会自动释放
 
    from.balance -= amount;
    to.balance += amount;
}
 
int main()
{
    bank_account my_account{100};
    bank_account your_account{50};
 /*
 多线程的 std::thread 也是必须显式通过 std::ref 来绑定引用进行传参，否则，形参的引用声明是无效的。
 */
    std::thread t1{transfer, std::ref(my_account), std::ref(your_account), 10};
    std::thread t2{transfer, std::ref(your_account), std::ref(my_account), 5};

 
    t1.join();
    t2.join();
 
    std::cout << "my_account.balance = " << my_account.balance << "\n"
                 "your_account.balance = " << your_account.balance << '\n';
}