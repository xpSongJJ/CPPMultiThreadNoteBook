#include<iostream>
#include<thread>
#include<future>

using namespace std;

// 线程入口函数
int mythread(){
    cout << "mythread() start ! " << "thread id = " << this_thread::get_id() << endl;
    chrono::milliseconds dura(5000);  // 5000ms
    this_thread::sleep_for(dura);
    cout << "mythread() end ! " << "thread id = " << this_thread::get_id() << endl;
    return 5;
}

int main(){
    cout << "main " << "thread id = " << this_thread::get_id() << endl;
    future<int> result = async(launch::deferred, mythread);  // 延迟子线程启动
    chrono::milliseconds dura(2000);
    this_thread::sleep_for(dura);
    cout << "main continuing !" << endl;
    
    cout << result.get() << endl;  // 子线程在这里启动
    // 子线程和主线程的线程id是一样的
    // 说明子线程没有被创建

    return 0;
}