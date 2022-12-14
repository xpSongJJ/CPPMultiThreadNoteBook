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
    future<int> result = async(mythread);  // 创建异步任务，并从这里已经开始执行
    chrono::milliseconds dura(2000);
    this_thread::sleep_for(dura);
    cout << "main continuing !" << endl;
    
    cout << result.get() << endl;
    // cout << result.get() << endl;  // 不能重复调用
    // result.wait();  // 会报错：No associated state

    return 0;
}