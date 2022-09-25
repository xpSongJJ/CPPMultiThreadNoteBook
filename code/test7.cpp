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
                std::chrono::milliseconds wait(1000);  // 2秒
                std::this_thread::sleep_for(wait);  // 模拟处理时长
                mylist.push_back(num);
                std::cout << "线程A向队尾添加数据: " << num << std::endl;
                ++num;
                mt.unlock();
                std::this_thread::sleep_for(wait/500); // 稍微延迟一下，保证线程B立即拿到数据。
            }
        }

        void out(){
            int i = 0;
            while(i < 10){
                std::unique_lock<std::mutex> ul(mt, std::defer_lock); // 推迟加锁
                ul.lock(); 
                if(!mylist.empty()){
                    int temp = mylist.front();
                    mylist.pop_front();
                    std::cout << "线程B从队头取出数据: " << temp << std::endl;  
                    ++i;
                    ul.unlock();  // 拿到数据就解锁
                }
                else{
                    std::cout << "线程B未取到数据！！！" << std::endl;
                }
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