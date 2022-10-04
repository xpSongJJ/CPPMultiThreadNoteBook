#include<iostream>
#include<thread>
#include<future>
#include<functional> // bind

using namespace std;

// 定义一个可调用函数
int mythread(int num){
    cout << "mythread() start ! " << "thread id = " << this_thread::get_id() << endl;
    chrono::milliseconds dura(5000);  // 5000ms
    this_thread::sleep_for(dura);
    cout << "mythread() end ! " << "thread id = " << this_thread::get_id() << endl;
    return num;
}

// 线程入口
auto mythread_bind = bind(mythread, 5); // 绑定传入的可调用对象及参数，生成一个新的可调用对象

int main(){
    cout << "main " << "thread id = " << this_thread::get_id() << endl;
    packaged_task<int(void)> mypt(mythread_bind);  // 可调用对象mypt只能被使用一次
    chrono::milliseconds dura(2000);
    this_thread::sleep_for(dura);
    cout << "main continuing !" << endl;

    thread t(ref(mypt));  // 子线程在这里开始执行
    t.join();  // 等待子线程执行完毕
    future<int> result = mypt.get_future(); // 获取mypt的future
    cout << result.get() << endl;

    // 直接调用mypt
    // mypt();
    return 0;
}