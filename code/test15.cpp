#include<iostream>
#include<thread>
#include<future>


void mythread(std::promise<int>& tmpp, int calc){
    calc++;
    calc *= 10;
    int result = calc;
    std::cout << std::this_thread::get_id() << " thread promise a result: " << result << std::endl;

    tmpp.set_value(result);
    return;
}

void mythread2(std::future<int>& tmpf){
    // 使用线程1得到的值
    auto result = tmpf.get(); // 只能够get()一次，因为get()是一个移动语义，值被移动给result。如果想要get()多次，使用shared_future，参考test17.cpp
    std::cout << std::this_thread::get_id() << " thread get the result: " << result << std::endl;

    return;
}

int main(){
    std::promise<int> myprom; // 声明一个promise对象，保存的值类型为int
    std::thread t1(mythread, std::ref(myprom), 180);
    t1.join();

    std::future<int> ful = myprom.get_future(); // future和promise绑定
    // auto result = ful.get(); // get()只能调用一次
    
    std::thread t2(mythread2, std::ref(ful));
    t2.join();

    return 0;
}