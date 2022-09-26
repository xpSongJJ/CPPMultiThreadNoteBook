#include<iostream>
#include<mutex>
#include<thread>

std::once_flag  g_flag;

class MyCAS{
private:
    MyCAS() {} //私有化构造函数，无法直接构造

    static void CreateInstance(){  // 只被调用一次
        std::chrono::milliseconds dura(1000);
        std::this_thread::sleep_for(dura);
        std::cout << "Function CreateInstance() is executed !" << std::endl;
        m_instance = new MyCAS();
        static GarbageRec gc;  // 静态类对象在程序退出时析构
        
    }
private:
    static MyCAS *m_instance;  //静态成员变量

public:
    static MyCAS *GetInstance(){  // 间接构造对象
        
        std::call_once(g_flag, CreateInstance);  // 实现多线程仅执行一次GetInstance()，未执行代表没有创建任何对象
        std::cout << std::this_thread::get_id() << "thread have executed call_once() !" << std::endl;
        return m_instance;
    }

    class GarbageRec{  // 使用嵌套类来释放对象
    public:
        ~GarbageRec(){
            if(MyCAS::m_instance){
                delete MyCAS::m_instance;
                MyCAS::m_instance = nullptr;
            }
        }
    };

    void fun(){
        std::cout << "Hello world ! " << std::endl;
        return;
    }

};

MyCAS *MyCAS::m_instance = nullptr; // 静态成员变量只能在类外初始化（分配内存）
// 线程入口函数
void mythread(){
    std::cout << "thread starting to execute: " << std::this_thread::get_id() << std::endl; // 两个线程都会输出这句话，可能有点乱，毕竟争着进入输出缓冲区
    MyCAS *p_a = MyCAS::GetInstance();
    p_a->fun();
    return;
}
int main(){

    std::thread t1(mythread);
    std::thread t2(mythread);
    t1.join();
    t2.join();

    return 0;
}