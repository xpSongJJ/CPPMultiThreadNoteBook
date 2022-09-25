#include<iostream>
#include<list>
#include<thread>
#include<mutex>

class INOUT{
    private:
        std::list<int> mylist;
        std::mutex mt;

    public:
        void in(){
            int num = 0;
            mt.lock();
            std::chrono::milliseconds wait(2000);  // 2秒
            std::this_thread::sleep_for(wait);  // 模拟处理时常
            while(num < 1000){
                mylist.push_back(num);
                std::cout << "向队尾添加数据：" << num << std::endl;
                ++num;
            }
            mt.unlock();
        }

        void out(){
            int i = 0;
            std::unique_lock<std::mutex> ul(mt, std::try_to_lock);
            if(!ul.owns_lock()){
                std::cout << "去干其它事情------------------------" << std::endl;
            }
            while(i < 10000){
                if(!mylist.empty()){
                    int temp = mylist.front();
                    mylist.pop_front();
                    std::cout << "从队头取出数据：" << temp << std::endl;  // 两个线程的输出会争夺屏幕的使用权，所有会有部分内容消失的现象
                    
                }
                else{
                    std::cout << "队列为空！！！！！！！！！！！！！！！！！！！！！！！！！！！！" << std::endl;
                
                }
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