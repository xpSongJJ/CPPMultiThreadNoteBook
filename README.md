# C++多线程学习笔记
该笔记根据B站课程记录，笔记的记录顺序随着课程的顺序，代码示例较课程上的有点改动。课程地址：https://www.bilibili.com/video/BV1Yb411L7ak/?p=6&spm_id_from=pageDriver&vd_source=ec46083ee7ebee9588ca7ef33741b702  
笔记里的‘互斥量’、‘锁’是一个意思，‘加锁’、‘获得互斥量所有权’、’拿到锁‘是一个意思。

## 创建线程
### 主线程与子线程的区别
- 主线程：从main()函数开始，每个程序必须具备一个主线程。

- 子线程：用户自己创建的线程。 

### 创建线程方法 
假设`fun()`为可调用对象。
- `std::thread t(fun)`：  创建线程t，该线程以可调用对象fun为入口，线程在此刻就开始执行。  
- `t.join()`：  阻塞主线程，待该子线程执行完毕后，与主线程汇合。  
- `t.detach()`： 该方法执行后，该子线程主主线程剥离，不再与主线程关联，此时该子线程会驻留在后台，由c++运行时库接管，待执行完后，再由运行时库清理相关资源，即守护线程。值传递，主线程结束后，对象被释放，子线程运行的是对象的拷贝。  
- `t.joinable()`： 判断是否可以成功使用join()或detach()，返回true或false。

## `detach()`方法问题分析 
示例： 
```cpp
thread t(fun, var);
t.detach();
``` 
 调用detach()方法后，子线程t将会独立于主线程运行，主线程结束后，在主线程内创建的资源将会被销毁，假设var是传递的主线程内的资源，子线程是如何处理的。这要依据资源传递方式看待：
 - var是值传递：资源会被子线程复制一份来处理。 
 - var是引用传递：资源依然会被复制一份。如果要实现真正的引用传递，可使用`thread t(fun, std::ref(var))`。  
 - var是指针传递：资源被销毁，子线程出问题。  
**注意**：传递参数时要尽量避免隐式转换，隐式转换需要时间，有可能出现主线程结束，资源还未被转换就被销毁，导致丢失资源。解决方法可以是在传递参数时显式转换，生成临时对象。

## 多线程数据共享
### 多线程创建
示例： 
```cpp
…
void fun(int x){
	cout << "这是线程x" << endl;
}
…
vector<thread> threads;
for(int i=0; i<10; ++i){
	threads.push_back(thread(fun, i));
}
for(auto it=threads.begin(); it!=threads.end(); ++it){
	it->join();
}
```
多个线程的执行顺序与创建先后无关，与操作系统的调度机制有关。所以示例代码输出会比较乱。 

### 数据共享分析
- 只读数据：安全稳定，不需要什么处理手段。
- 有读有写：具体情况具体分析。 
案例：[点这里](code/test1.cpp)  
案例表述的是两个子线程共同操作一个共享队列（list），一个线程负责写，一个线程负责拿。两个线程共同运行时，就会出现一些问题。（该案例好像没出现什么致命问题）

## 互斥量（锁）与死锁 
为保护共享资源（独占共享资源），在使用共享资源前为其加锁，使用过后再解锁。但多个线程操作多个共享资源时就有可能造成死锁现象。 
### 互斥量的用法 
1. `lock()与unlock()`:  
```cpp
...
std::mutex my_mutex; // 声明互斥量对象
...
my_mutex.lock(); // 加锁
// 需要保护的操作
...
my_mutex.lock(); // 解锁
...
```
注意：`lock()`和`unlock()`必须成对出现，所以在条件语句使用`unolck()`时要考虑到条件不成立的情况。 
示例：[点这里](code\test2.cpp)  
该示例是对**test1**示例的两个子线程添加互斥锁，使双线程操作共享数据时不会冲突。

2. `lock_guard`:  
```cpp
...
std::mutex my_mutex // 声明互斥量对象
...
std::lock_guard<std::mutex> guard(my_mutex);
... // 受保护操作
```
`lock_guard`是类模板，可以解决使用`lock()`时，容易遗忘`unlock()`的问题。`lock_guard`之于`lock()`类似于智能指针之于指针，利用了C++的RAII机制，资源获取即初始化（构造函数），离开作用域就销毁资源（虚构函数）。可以使用`{}`收缩作用域。其缺点是不如`lock() & unlock()`灵活。示例：[点这里](code\test3.cpp)  

### 死锁  
死锁至少需要两个互斥量才能产生。死锁只有满足这4个条件才能发生：互斥条件、持有并等待、不可剥夺条件和环路条件。  
- 互斥条件：多个线程不能同时使用同一个资源； 
- 持有并等待：线程在等待其它资源的同时不会释放持有的资源；
- 不可剥夺条件：线程持有的资源在未使用完以前不能被强制剥夺；
- 环路条件：线程获取资源的顺序形成了环路；  
示例：[点这里](code\test4.cpp)  
该示例实际中很难用到，拿两把锁（金锁银锁）来锁同一个共享数据，当出现线程1用金锁锁定共享数据，这是线程2又用银锁锁定了共享数据，谁都没办法打开对方的锁，导致死锁。  
### 死锁的解决办法
死锁需要同时满足4个条件才能发生，破坏其中一个就可以解决，常用的方法是破坏环路。在`test4`中，让两个线程上两把锁的顺序一致就可以避免死锁。这也算是破环环路，但不直观，比较直观的是2个线程2个共享资源：线程1持有A资源正获取B资源，线程2持有B资源正获取A资源，解决该死锁办法是2个线程对2个资源的访问顺序一致，就不会出现环路，从而不会产生死锁。  

### `std::lock()函数模板` 
`std::lock()`一次锁住2个及以上互斥量，因为它对多个互斥量加锁的顺序是一致的，从而避免了因为锁顺序导致的死锁问题。可以结合`lock_guard`(需要传入`std::adopt_lock`参数)实现不用再手动`unlock()`。示例：[点这里](code\test5.cpp)  
视频up主给的建议：谨慎使用该方法，建议一个一个锁。

## `unique_lock`详解  
`unique_lock`是类模板，它是通用互斥包装器，允许延迟锁定、锁定的有时限尝试、递归锁定、所有权转移和与条件变量一同使用。  
### `unique_lock`取代`lock_guard`  
`unique_lock`比`lock_guard`灵活（提供了更多的功能），但效率较差，占用内存较多。构造对象时可额外传入的参数及其含义：  
- `std::adopt_lock`：和`lock_guard`传入的该参数功能相同，表示之前的互斥量已经加锁，`unique_lock`构造对象时不再加锁。  
- `std::try_to_lock`：构造对象时尝试用`mutex`的`lock()`去锁定这个`mutex`，如果没有锁定成功，就立即返回，不会阻塞在那里，这样可以利用该线程去做其它事情。示例：[点这里](code\test6.cpp)   
这个示例不是很完美，有闲心的话可以优化优化。  
- `std::defer_lock`：不获得互斥量的所用权，即推迟加锁。其前提也是不自己`lock()`，针对这个对象（其它`unique_lock`对象也适用）就会有一些成员函数可调用：
	> 1. `lock()`：手动加锁，但可以自动释放。
	> 2. `unlock()`：灵活解锁或提前解锁，使锁更加灵活，优化锁的粒度。示例：[点这里](code\test7.cpp)  
	> 3. `try_lock()`：尝试给互斥量加锁，拿到锁就返回true，否则返回false。 
	> 4. `release()`：打断互斥量与`*this`的关联，返回指向该互斥量的指针，若互斥量还未被解锁，则需要接管者负责互斥量的解锁。示例：[点这里](code\test8.cpp)  
> **粒度**：锁住的代码量被称为粒度，粒度用粗细来衡量。锁住的代码少时，粒度较细，执行效率一般较高。锁住代码量多时，粒度较粗，执行效率一般较低。需要保持合适的粒度。  
### `unique_lock`所有权传递  
`unique_lock`独占一个互斥量的所有权，和`unique_ptr`比较类似。所以`unique_lock`只能转移互斥量的所有权，不能复制互斥量的所有权。所有权转移的方法： 
```cpp
... 
std::unique_lock<mutex> ul1(my_mutex);
std::unique_lock<mutex> ul2(std::move(my_mutex)); // 将my_mutex所有权由ul1转移至ul2
... 
``` 
## 单例设计模式共享数据分析、解决
设计模式是代码的一些写法，这样的写法使程序更加灵活，维护起来更加方便。但这样的代码对于不了解设计模式的人来说晦涩难懂。设计模式出现的初衷是用来管理大型项目，这样时后期的维护更加方便。  
### 单例设计模式 
单例设计模式是指某些特殊的类（单例类）只能创建一个对象。一个简单的单例类：[点这里](code\test9.cpp)  
该类使用的技巧有：私有化构造函数、静态成员指针变量存储对象、嵌套类实现析构。  
### 共享数据分析、解决 
单例类建议在主线程（子线程之前）完成对象的创建与初始化。当需要在子线程中创建单例类时，可能面临GetInstance()成员函数互斥的问题。  
```cpp
... 
static MyCAS *GetInstance(){ 
	if(m_instance == nullptr){ // 线程可能在这里切换，导致多个线程通过这个条件，就会构造多个对象
		m_instance = new MyCAS();
		static GarbageRec gc;  // 静态类对象在程序退出时析构
	}
	return m_instance;
}  
... 
``` 
解决办法：
```cpp 
... 
std::mutex resource_mutex;  // 在类外
... 
static MyCAS *GetInstance(){ 
	if(m_instance == nullptr){  // 双重检查，收紧条件，为了提高效率，这是比较推荐的方法
		std::unique_lock<std::mutex> ul(resource_mutex);  // 自动加锁
		if(m_instance == nullptr){
			m_instance = new MyCAS();
			static GarbageRec gc;  // 静态类对象在程序退出时析构
		}
	}
	return m_instance;
... 
```   
另一种解决方法是使用`std::call_once()`。
### `std::call_once()` 
即使多线程调用可调用对象，依然只执行一次。具备互斥量的功能，但效率比直接使用互斥量高。  
`void call_once(std::once_flag& flag, Callable&& f, Args&&... args)`  
- `flag`：通过这个标记来决定可调用对象`f`是否执行，调用`call_once()`成功后，这个标记就会被标记为已调用状态，如果后续再次调用`call_once()`,就会检查`flag`状态，来决定是否执行可调用对象`f`。  
- `f`：可调用对象。
- `args`：向可调用对象`f`传递的变量。  
示例：[点这里](code\test10.cpp)  
该示例是上面问题的另一个解决办法，这里利用`std::call_once()`只执行一次的特性，来确保多个线程同时进行时只能创建一个对象。需要声明一个全局变量`std::once_flag  g_flag`传入`std::call_once()`用来判断可调用对象`CreateInstance()`是否已被某个线程执行，这个可调用对象定义为私有成员函数，实现了对象的创建。  
> 注意：依然建议在主线程中创建对象

## 条件变量`condition_variable`   
回顾一下前面的双线程例子：有两个子线程，线程A负责往队列（共享数据）写数据，线程B负责从队列取数据，线程B需要不停地访问队列（通过循环实现），判断队列里是否存在数据，如果存在就取出来，否则继续循环判断，这样写显得有些笨拙，不停地加锁解锁，而且很有限制性。现在可以利用条件变量进行优化。  
条件变量的成员函数有： 
-  `void wait(std::unique_lock<std::mutex>& lock)`：传入的参数是`unique_lock`对象，原子地解锁`lock`，阻塞当前线程，并将它添加到于`*this`上等待的线程列表。线程将在执行`notify_all()`或`notify_one()`时被解除阻塞。解阻塞时，无关乎原因，`lock`再次锁定且`wait`退出。
- `void wait(std::unique_lock<std::mutex>& lock, Predicate pred)`：第二个参数是一个[谓词](https://www.apiref.com/cpp-zh/cpp/named_req/Predicate.html)，定义自己的等待条件。这个重载函数相当于：  
> ```cpp
> while(!pred){
> 	wait(lock);
> }  
> // 当wait()不含有第二个参数时，被唤醒的wait()直接通过
> // 含有第二个参数时，则还需判断谓词条件
> ``` 
- `void notify_one()`：唤醒`wait()`，只能通知一个线程。示例：[点这里](code\test11.cpp)   
- `notify_all()`唤醒全部等待于`*this`的线程。 

## `async`、`future`、`packaged_task`、`promise`讲解  
### `std::async`、`std::future`创建后台任务并返回值  
现在假设你的程序对互斥量不敏感，只关系子线程返回的结果，则可以使用`async`。  
`std::future<...> async( Function&& f, Args&&... args )`；  
`std::future<...> async( std::launch policy, Function&& f, Args&&... args )`  
`std::async`是一个函数模板，用来启动一个异步任务，返回一个`std::future`对象，
这个`std::future`对象含有线程入口函数所返回的结果，可以访问该对象的成员函数来获取特定的功能。
但要等待这个异步任务的结束。可以理解为访问异步任务将来的特性。`std::future`的成员函数如下：  
`get()`：获取该异步任务的返回值。如果异步线程还没有结束，则程序会阻塞在这里，直到返回结果。  
`wait()`：无返回值，程序在这里阻塞，等待异步线程的结束。  
示例：[点这里](code\test12.cpp)   
> 注意：如果没有调用`std::future`的任何方法，则效果相当于在主函数`return`处调用`wait()`。  

`std::launch`是枚举类型，可以定制想要的启动策略。  
	1. `std::launch::deferred`：延迟子线程启动，在`get()`或`wait()`调用时启动。如果没有调用这些方法，则子线程从始至终都没有创建。现在有一个疑问，延迟子线程启动，即在在get()或wait()调用时启动并堵塞，和直接将该线程替换成一个函数好像没什么区别。示例：[点这里](code\test13.cpp)  
	2. `std::launch::async`：异步子线程创建并启动。  
	3. `std::launch::deferred | std::launch::async`：默认标记，系统根据需要选择是否延迟启动。  

### `std::packaged_task`  
`packaged_task`是类模板，模板参数是可调用对象，将各种可调用对象（函数、 lambda 表达式、 bind 表达式或其他函数对象）包装起来，生成一个新的可调用对象，方便将来作为线程入口函数。其返回值或所抛异常被存储于能通过`future`对象访问的共享状态中。使用示例：[点这里](code\test14.cpp)   
程序开发过程中，经常将`packaged_task`封装在容器中，方便后续使用。  
```cpp
... 
int fun(int val){
	...
}
...
packaged_task<int(int)> mypt(fun);
vector<packaged_task<int(int)>> mytasks;
mytasks.push_back(mypt);
...
packaged_task<int(int)> mypt2; // 用来接收容器里的对象
auto iter = mytasks.begin();
mypt2 = move(*iter);  // 转移所有权
mytask.erase(iter) // 删除第一个元素，迭代器失效，后续代码不能再使用iter
mypt2(val);
future<int> result = mypt2.get_future();
result.get();
...
```  

### `std::promise`  
类模板`promise`能够存储值或异常，之后通过`promise`对象所创建的`future`对象异步获得结果。
注意`promise`只应当使用一次。示例： [点这里](code\test15.cpp)  

### 建议
学习这些东西的目的不是马上把他们应用在实际开发中，而是为了帮助我们能够读懂大佬的代码，积累经验。
在实际开发中更值得赞赏的是，使用使用更少的东西写出一个稳定高效的多线程程序。  

## `future`其它成员函数、`shared_future`、`atomic`  

### `std::future`其它成员函数  
类模板`future`提供访问异步操作结果的机制，除了提供结果访问(`get()`)，还提供一些状态访问的成员函数。  
	1. `wait_for()`：等待结果，如果在指定的超时间隔后仍然无法得到结果，则返回。
	返回类型为`std::future_status`。  
> `future_status`是个枚举类型，包含三种状态，`timeout`、`ready`和`deferred`。  

### `std::shared_future` 
`shared_future`是一个类模板，不同于`future`，该类的`get()`函数是将值复制给变量，而不是转移给变量，所以可以`get()`多次。[点这里](code\test17.cpp)  
1. `share()`可以将一个`future`对象转移给`shared_future`对象：
	```cpp
	std::shared_future<int> shf{f.share()}; // f为一个future对象
	```  
2. `valid()`可以检查`future`对象是否拥有共享状态，即其值是否被转移，被转移后就失去了共享状态。

### 原子操作`std::atomic`  
如果有一个线程对共享数据进行写操作，另一个线程进行读取操作，则可能出现意想不到的错误。
一种解决办法就是对共享数据加锁，但频繁的访问，不断地加锁解锁，效率势必比较低。
另一种解决办法是将共享数据定义为一个原子类型对象。对该对象进行的一般操作是原子操作。示例：[点这里](code\test18.cpp)    
原子操作：在多线程中不会被打断的程序片段。  
> 对原子变量的原子操作一般是加减乘除赋值等，自定义的原子操作还需要进一步学习。    


## `std::atomic`续谈、`std::async`深入谈  

### 原子操作`std::atomic`续谈  
回顾上面的示例(test18.cpp)，当把自增运算`++count`更换成`count = count + 1`时，运行结果就会出错，这就说明`count = count + 1`并不是原子操作。
一般默认支持的原子操作是`++`,`--`,`+=`,`&=`,`|=`,`^=`。  

### `std::async`深入谈  
回顾之前的内容，`async`创建一个线程时（更应该说创建一个异步任务），可以传入特定的标记：`std::launch::deferred`，延迟启动；`std::launch::async`,强制启动线程。  
`std::thread`如果在系统资源紧张时，创建线程可能会失败，那么继续执行就可能导致程序崩溃。
`async`一般不叫创建线程，而是创建一个异步任务。与`thread`不同的是，当它传入标记`std::launch::deferred`，并不会创建新的线程（因为毫无意义）。  
`async`还可以传入标记`std::launch::deferred | std::launch::async`，默认标记，代表是否延迟启动是不确定的，由系统决定。在系统资源紧张时，可能就会延迟启动。  

#### `std::async`和`std::thread`的区别
`thread`创建线程时如果系统资源紧张，则可能创建线程失败导致程序崩溃。此外，如果想要拿到这种方式创建的线程的返回值也不是一件容易事。  
`async`更加灵活，它创建一个异步任务，该任务根据传入的标志来决定是否执行，如果不传入标志，则根据系统资源情况决定是否延迟执行。
所谓延迟执行实际就是在需要的时候(调用get()或wait()时)直接在所在线程上执行这个异步任务，而不创建新的线程。  
老师的经验：一个程序里，线程数量不宜超过100-200。
#### `std::async`不确定性解决  
在使用`async`创建异步任务时，没有传入任何标志，即任务是否被延迟是不确定的。
这时如果开发者比较关心该异步任务是否被延迟了，并根据情况进行接下来的操作。
可以使用`wait_for()`来返回状态，如：
```cpp
std::future<int> result = std::async(fun);

std::future_status status = result.wait_for(0s);  // 返回的状态有：deferred、ready、timeout
if(status == std::future_status::deferred){
	...
	result.get();  //任务被延迟了，现在启动任务
	...
}
if(status == std::future_status::timeout){
	...
	// 异步任务已经被执行，但还没有执行完
}
if(status == std::future_status::ready){
	...
	result.get(); // 异步任务已经执行完毕，可以直接拿数据
}
```  

## windows临界区、其它各种`mutex`互斥量  

### windows临界区  
windows临界区是面向windows编程的‘互斥量'，类似C++中的`mutex`，
使用时需要包含`windows.h`头文件。示例：[点这里](code\test19.cpp)  

那么window临界区和C++的`mutex`用法有什么异同呢？  
win临界区可以多次进入：在同一个线程中，相同临界区变量代表的临界区的进入(`EnterCriticalSection()`)可以被多次调用。当然进入临界区和离开临界区(`LeaveCriticalSection()`)必须是成对出现的。而C++本身的互斥量是不允许多次加锁的。  

### 自动离开临界区  
C++中有`std::lock_guard`，来实现锁的自动释放，那么win下有没有提供类似的功能？
这里来实现一个这样的功能。示例：[点这里](code\test20.cpp)  

### `recursive_mutex`递归地独占互斥量  
`recursive_mutex`递归地独占互斥量，即允许同一个线程同一个互斥量多次被`lock()`。类似win临界区的`EnterCriticalSection()`。  
先想象一下什么时候会用到多次`lock()`呢？一个场景就是在实际开发中，在已经`lock()`的情况下需要调用另一个方法，但那个方法里有`lock()`语句，重写又太麻烦。
这时就需要能够多次`lock()`。  
```cpp
void test1(){
	std::lock_guard<std::mutex> lg(my_mutex);
	// ... 干各种事
	test2(); // test2里也有lock()
}
void test2(){
	std::lock_guard<std::mutex> lg(my_mutex);
	// ... 干各种事
}
```
`recursive_mutex`用法和`mutex`类似，直接声明一个变量使用就行了。但在使用的过程中要观察程序是否有优化的空间。  

### 带超时的互斥量`std::timed_mutex`和`std::recursive_timed_mutex`  
`std::timed_mutex`:带有超市功能的独占互斥量，与`std::mutex`有什么区别呢？
`std::timed_mutex`含有另外的两个接口：  
`try_lock_for()`：参数是一个时间段，表示等待一段时间，如果时间段内拿到锁或时间段尾未拿到锁，则程序继续走下去。  
`try_lock_until()`：参数是一个时间点，表示该时间点前拿到锁或到时间点未拿到锁时，程序继续走下去。  
```cpp
std::chrono::steady::now();  // 表示当前时间，可以加一个时间段表示未来一个时间点
```
`std::recursive_timed_mutex`是在`std::timed_mutex`的基础上可以多次`lock()`。  

## 补充知识、线程池浅谈、数量谈、总计

### 补充一些知识
#### 虚假唤醒
回顾一下`test11.cpp`，其中的条件变量`cond_var`的`notify_one()`和`wait()`的配合使用，
来实现线程B等待满足条件(`wait()`)，不满足则进入休眠状态来节省资源，线程A生产出资源立马通知(`notify_one()`)线程B，唤醒其休眠状态。  
那么虚假唤醒是指存在一种情况就是，线程A并没有生产出资源就通知了线程A。  
所说为应对虚假唤醒的现象，线程A的`wait()`中应有一个判断语句(第二个参数)，判断是否有资源，再决定是否再次进入休眠。  
#### `atomic`  
原子变量不支持拷贝构造，也不支持拷贝赋值。可以使用`load()`来实现拷贝构造或拷贝赋值。使用`store()`对原子变量写入数据。
```cpp
std::atomic<int> atm1;
atm1.store(1);  // 写入数据
std::atomic<int> atm2(atm1.load());  //用atm1初始化atm2
```

### 线程池浅谈  
现在有一个网络服务器，每来一个访问就为其创建一个线程，如果同时有上万个访问，就要同时创建上万个线程，这显然是不现实的。  
稳定性问题：编写代码中，偶尔创建一个线程，这种写法是不安全的。因为一旦创建失败，就可能产生一系列连锁反应。  
线程池：把一堆线程弄到一起，统一管理。这种统一管理调度，循环利用线程的方式就叫线程池。
#### 实现方式  
在程序启动时，一次性地创建好一定数量的线程。避免临时创建线程，提高程序的稳定性。
### 线程创建数量谈
根据前辈的经验，同一个程序创建线程的极限数量大概是2000个。  
采用某些技术开发程序，api接口供应商建议创建线程的数量为cpu数量，或cpu*2, 或cpu*2+2，应遵循建议和指示来，专业意见  
创建多线程完成业务(如充值业务)，一个线程等于一条执行通路。比如100个堵塞充值，开110个线程是有必要的，留出一些线程干其他事情。  
创建线程的数量控制在200个之内是比较合适的。
### C++11多线程总结
c++11的多线程程序实现了跨平台。  

## 课程总结是展望
老师推荐课程《跨越30K系列之Linux C++网络架师构实战》、《C++对象模型探索》
