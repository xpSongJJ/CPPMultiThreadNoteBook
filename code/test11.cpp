#include<iostream>
#include<list>
#include<thread>
#include<mutex>
#include<condition_variable>

class INOUT{
    private:
        std::list<int> mylist;
        std::mutex mt;
        std::condition_variable cond_var;  // 条件变量对象

    public:
        void in(){
            int num = 0;
            while(num < 10){
                std::unique_lock<std::mutex> ul(mt);
                std::chrono::milliseconds wait(100);  // 100毫秒
                std::this_thread::sleep_for(wait);  // 模拟处理时长
                mylist.push_back(num);
                cond_var.notify_one();  // 唤醒wait()，该线程仍然可能拥有互斥量的所有权
                std::cout << "向队尾添加数据：" << num << "\n";
                ul.unlock(); // 先notify_one，再释放锁，让另一个线程唤醒后等着这个锁，以便迅速抢到锁的所有权
                ++num;
                std::this_thread::sleep_for(wait/10); // 稍微等待一下，保证另一个线程抢到互斥量所有权，这里只是为了让输出更好看
            }
        }

        void out(){
            int command = 0;
            while(true){
                std::unique_lock<std::mutex> ul(mt);  // 创建ul对象即加锁
                // 若不满足条件（lambda表达式的条件），则解锁互斥量并堵塞
                // 直到cond_var调用notify_one()成员函数
                // wait()被唤醒后，会不断地尝试获取互斥量的所有权
                cond_var.wait(ul, [this]{  // lambda表达式
                    if(!mylist.empty()) return true;
                    return false;
                });
                // -----------拥有了互斥量的所有权
                command = mylist.front();
                mylist.pop_front();
                ul.unlock(); // 及时释放互斥量的所有权，接下来的操作不需要操作共享数据
                std::cout << "从队头取出数据：  " << command << "\n";
            }
        }
};

int main(){

    INOUT inout;

    std::thread t1(&INOUT::in, &inout);
    std::thread t2(&INOUT::out, &inout);
    t1.join();
    t2.join();

    return 0;
}