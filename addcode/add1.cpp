#include<iostream>
#include<thread>

using std::cout;
using std::endl;

int count = 0;  // 全局计数器

// 对计数器进行自增运算
void add(){
    int sum = 0;
    while(sum < 100000){
        ++count;
        ++sum;
    }
}

int main(){

    std::thread t1(add);  // 线程t1开始执行
    std::thread t2(add);
    t1.join();  // 线程t1汇入主线程
    t2.join();

    // 计数器的直观输出应该是200000，但却是一个小于200000的随机数
    cout << count << endl; 

    return 0;
}