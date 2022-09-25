#include<iostream>

class MyCAS{
private:
    MyCAS() {} //私有化构造函数，无法直接构造

private:
    static MyCAS *m_instance;  //静态成员变量

public:
    static MyCAS *GetInstance(){  // 间接构造对象
        if(m_instance == nullptr){
            m_instance = new MyCAS();
            static GarbageRec gc;  // 静态类对象在程序退出时析构
        }

        return m_instance;
    }

    void fun(){
        std::cout << &m_instance << std::endl;
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

};

MyCAS *MyCAS::m_instance = nullptr; // 静态成员变量只能在类外初始化（分配内存）

int main(){

    MyCAS *p_a = MyCAS::GetInstance();
    MyCAS *p_b = MyCAS::GetInstance();

    p_a->fun();
    p_b->fun();


    return 0;
}