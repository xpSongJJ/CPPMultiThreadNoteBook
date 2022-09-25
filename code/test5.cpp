#include<iostream>
#include<list>
#include<thread>
#include<mutex>

class INOUT{
    private:
        std::list<int> mylist;
        std::mutex mt1;
        std::mutex mt2;

    public:
        void in(){
            int num = 0;
            while(num < 10000){
                std::lock(mt1, mt2);
                std::lock_guard<std::mutex> guard1(mt1, std::adopt_lock);
                std::lock_guard<std::mutex> guard2(mt2, std::adopt_lock);
                mylist.push_back(num);
                std::cout << "向队尾添加数据：" << num << std::endl;
                
                ++num;
            }
        }

        void out(){
            int i = 0;
            while(i < 10000){
                std::lock(mt1, mt2);
                std::lock_guard<std::mutex> guard1(mt1, std::adopt_lock);
                std::lock_guard<std::mutex> guard2(mt2, std::adopt_lock);
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