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
                mt1.lock();
                mt2.lock();
                mylist.push_back(num);
                std::cout << "向队尾添加数据：" << num << std::endl;
                mt2.unlock();
                mt1.unlock();
                ++num;
            }
        }

        void out(){
            int i = 0;
            while(i < 10000){
                mt2.lock();
                mt1.lock();
                if(!mylist.empty()){
                    int temp = mylist.front();
                    mylist.pop_front();
                    std::cout << "从队头取出数据：" << temp << std::endl;  // 两个线程的输出会争夺屏幕的使用权，所有会有部分内容消失的现象
                    mt1.unlock();
                    mt2.unlock();
                }
                else{
                    std::cout << "队列为空！！！！！！！！！！！！！！！！！！！！！！！！！！！！" << std::endl;
                    mt1.unlock();
                    mt2.unlock();
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