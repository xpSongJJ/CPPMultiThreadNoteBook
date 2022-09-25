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
            while(num < 10){
                mt.lock();
                std::chrono::milliseconds wait(2000);  // 2秒
                std::this_thread::sleep_for(wait);  // 模拟处理时长
                mylist.push_back(num);
                std::cout << "向队尾添加数据：";
                for(int i = 0; i < 20; ++i) std::cout << num << "    ";
                std::cout << std::endl;
                ++num;
                mt.unlock();
                std::this_thread::sleep_for(wait/10);  // 暂停200毫秒，不然另一个线程总是抢不到加锁
            }
        }

        void out(){
            // 加入互斥量已加锁，该线程就去做其它事情
            // 同时不断监视互斥量，若未被加锁就访问共享资源
            int i = 0;
            while(i < 1000){
                std::unique_lock<std::mutex> ul(mt, std::try_to_lock);
                if(ul.owns_lock()){
                    if(!mylist.empty()){
                        int temp = mylist.front();
                        mylist.pop_front();
                        std::cout << "从队头取出数据：";
                        for(int j = 0; j < 30; ++j) std::cout << temp << "    ";
                        std::cout << std::endl;  
                        
                    }
                    else{
                        std::cout << "队列为空！！！！！！！！！！！！！！！！！！！！！！！！！！！！" << std::endl;

                    }
                }
                else{
                    std::cout << "去干其它事情了-------------------------" << std::endl;
                    std::chrono::milliseconds wait(100); 
                    std::this_thread::sleep_for(wait);
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