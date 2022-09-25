#include<iostream>
#include<list>
#include<thread>
#include<mutex>


class INOUT{
    private:
        std::list<int> mylist;
        std::mutex my_mutex;

    public:
        void in(){
            int num = 0;
            while(num < 10000){
                my_mutex.lock();
                mylist.push_back(num);
                std::cout << "向队尾添加数据：" << num << std::endl;
                my_mutex.unlock();
                ++num;
            }
        }

        void out(){
            int i = 0;
            while(i < 10000){
                my_mutex.lock();
                if(!mylist.empty()){
                    int temp = mylist.front();
                    mylist.pop_front();
                    std::cout << "从队头取出数据：" << temp << std::endl;  // 两个线程的输出会争夺屏幕的使用权，所有会有部分内容消失的现象
                    my_mutex.unlock();
                }
                else{
                    std::cout << "队列为空！！！！！！！！！！！！！！！！！！！！！！！！！！！！" << std::endl;
                    my_mutex.unlock();  // 保证必有一个unlock()匹配clock()
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