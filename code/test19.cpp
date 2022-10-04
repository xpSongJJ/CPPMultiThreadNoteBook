#include<iostream>
#include<list>
#include<thread>
#include<mutex>
#include<windows.h>

/*
该示例是在test2.cpp的基础上，引入windows临界区，
观察其用法
*/

#define __WINDOWSJQ_  // 定义一个开关


class INOUT{
    private:
        std::list<int> mylist;
        std::mutex my_mutex;
#ifdef __WINDOWSJQ_
    CRITICAL_SECTION my_winsec;  // windows下的临界区        
#endif
    public:
    INOUT(){
#ifdef __WINDOWSJQ_
        InitializeCriticalSection(&my_winsec);  // win临界区用之前需要初始化
#endif
    }
        void in(){
            int num = 0;
            while(num < 10000){
#ifdef __WINDOWSJQ_
                EnterCriticalSection(&my_winsec);  // 进入临界区
                mylist.push_back(num);
                std::cout << "向队尾添加数据：" << num << "--(面向win编程)" << std::endl;
                LeaveCriticalSection(&my_winsec);  // 离开临界区
#else
                my_mutex.lock();
                mylist.push_back(num);
                std::cout << "向队尾添加数据：" << num << std::endl;
                my_mutex.unlock();
#endif
                ++num;
            }
        }

        void out(){
            int i = 0;
            while(i < 10000){
#ifdef __WINDOWSJQ_
                EnterCriticalSection(&my_winsec);  // 进入临界区
                if(!mylist.empty()){
                    int temp = mylist.front();
                    mylist.pop_front();
                    std::cout << "从队头取出数据：" << temp << "--(面向win编程)" << std::endl;  // 两个线程的输出会争夺屏幕的使用权，所有会有部分内容消失的现象
                    LeaveCriticalSection(&my_winsec);  // 离开临界区
                }
                else{
                    std::cout << "队列为空！！！！！！！！！！！！！！！！！！！！！！！！！！！！" << "--(面向win编程)" << std::endl;
                    LeaveCriticalSection(&my_winsec);  // 离开临界区
                }
#else

                my_mutex.lock();
                if(!mylist.empty()){
                    int temp = mylist.front();
                    mylist.pop_front();
                    std::cout << "从队头取出数据：" << temp << std::endl;  // 两个线程的输出会争夺屏幕的使用权，所有会有部分内容消失的现象
                    my_mutex.unlock();
                }
                else{
                    std::cout << "队列为空！！！！！！！！！！！！！！！！！！！！！！！！！！！！" << std::endl;
                    my_mutex.unlock();  // 保证必有一个unlock()匹配clock()
                }
#endif
                ++i;
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