#include<iostream>
#include<atomic>
#include<thread>

using std::cout;
using std::endl;

/* 
该示例模拟的是两个线程对全局变量不停地进行自增操作，分别自增100000次。
如果不加任何措施，最后该全局变量可能不是200000。
加互斥量，则效率较低。
将该全局变量定义为原子变量是一个不错的解决办法。
*/

std::atomic_int count{0};  // 初始化count为0，使用=初始化会出错

// 线程入口函数
void increase(){
    int num = 0;
    while(num < 1000000){
        ++count;
        ++num;
    }
}

int main(){

    std::thread t1(increase);  // thread t1 start to execute
    std::thread t2(increase);  // thread t2 start to execute
    t1.join();
    t2.join();
    cout << count << endl;

    return 0;
}