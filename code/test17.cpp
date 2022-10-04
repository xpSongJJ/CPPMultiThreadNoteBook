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

void mythread2(std::shared_future<int>& tmpf){
    // 使用线程1得到的值
    auto result = tmpf.get();
    auto result1 = tmpf.get(); 
    std::cout << std::this_thread::get_id() << " thread get the result: " << result << " " << result1 << std::endl;

    return;
}

int main(){
    std::promise<int> myprom; // 声明一个promise对象，保存的值类型为int
    std::thread t1(mythread, std::ref(myprom), 180);
    t1.join();

    // std::shared_future<int> shf{myprom.get_future()};
    std::future<int> ful = myprom.get_future(); // future和promise绑定
    bool ifcanget = ful.valid(); // 测试ful储存值的变化
    std::cout << "ful is valid ? " << ifcanget << std::endl;
    std::shared_future<int> sh_ful{ful.share()}; // 转移共享状态，ful变为空
    ifcanget = ful.valid();
    std::cout << "ful is valid ? " << ifcanget << std::endl;
    std::thread t2(mythread2, std::ref(sh_ful));
    t2.join();

    return 0;
}