#include<iostream>
#include<thread>
#include<future>

int mythread(int val){
    std::cout << std::this_thread::get_id();
    std::cout << " thread starting ... " << std::endl;
    std::chrono::seconds dura(5);
    std::this_thread::sleep_for(dura);  // 休眠5s
    std::cout << std::this_thread::get_id();
    std::cout << " thread ending ! " << std::endl;

    return val;
}

int main(){
    std::cout << std::this_thread::get_id();
    std::cout << " thread starting ... " << std::endl;

    std::future<int> result = std::async(std::launch::deferred, mythread, 43);
    // 等待2s后返回状态
    // 但不妨碍子线程继续执行
    // 如果async()传入了deferred，则不会等待，直接返回状态
    std::future_status status = result.wait_for(std::chrono::seconds(2));
    if(status == std::future_status::timeout){
        std::cout << "the child thread timeout !!!" << std::endl;
    }
    // 等待时间即为阻塞时间，不依赖于子线程是否执行完
    status = result.wait_for(std::chrono::seconds(4));
    if(status == std::future_status::ready){
        std::cout << "the child thread have return !" << std::endl;
        std::cout << result.get() << std::endl;
    }
    if(status == std::future_status::deferred){
        std::cout << "child thread have been deferred !" << std::endl;
        std::cout << result.get() << std::endl;
    }
    std::cout << std::this_thread::get_id();
    std::cout << " thread end !" << std::endl;
    return 0;
}