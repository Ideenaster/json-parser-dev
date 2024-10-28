# dev-log
## day1
### what`s Json
- 数字（Number）
- 整数
- 浮点数
- 字符串（String）
- 布尔值（Boolean）
- 空值（Null）
- 对象（Object）
- 数组（Array）
### 数据结构
1. 使用union存放单独json其值，实现类似于动态类型的效果
2. 使用map映射到另一个json对象来实现json::object
3. vector存放数组
   
### 编码规范
1. 使用命名空间进行隔绝
2. 使用下划线前缀表示内部变量

### 设计友好api
编写多类型入口的构造函数，运算符重载

### TODO
注意浅拷贝问题

## day2

### 尝试解决浅拷贝相关问题
#### 问题
诚然，对于一个复杂的json结构，如果直接进行深拷贝会耗费大量的时间,但是采用浅拷贝方法遇到许多难以解决的问题
- 直接浅拷贝，析构函数进行delete和nullptr，会导致二次free错误，很难保证不进行临时对象的构建，如果坚持采用该方案，必须使用移动语义等。但尽管如此，仍然无法解决原有的指针（string，map）所指向对象可能在其他场所发生析构的可能，完全不安全。
- 采用shared_ptr，即引用计数方式。刚尝试采用该方案就遇到一个相当难发现的bug。原因是shared_ptr是对象而非简单的指针，由于该json项目采用union进行value的存放，导致每个shared_ptr都是初始化即为野指针状态，而shared_ptr的operator = 的默认行为为原先的指针所指对象进行析构引发了析构异常。所以所有对原指针进行赋值的操作也变得不安全，极大增加了开发代码的心智负担。
#### 结论
转而采用深拷贝方式，优化深拷贝的开销可通过其他方法进行支持，比如额外编写一个move方法，但是默认的copy方法必须是深拷贝，否则bug的产生难以通过调试发觉。

## day3
### 编写parse方法的细则
#### 采用递归方式进行解析
- 为每种 JSON 数据类型创建单独的解析函数
- 主解析函数根据当前字符决定调用哪个具体的解析函数
- 每个解析函数负责处理自己的数据类型，并在需要时调用其他解析函数

#### 性能考虑
- 对于非常深的嵌套结构，可能需要考虑栈溢出的问题
- 后续可以考虑实现一个基于栈的非递归版本作为备选

## day4
## 移动语义问题
由于没有相关的开发经验，在设计数组API和对象API时考虑过于简单，只实现了基于const左值引用的参数传入，而在进行parse_arrary和parse_object等parse子函数实现时相当自然地将函数返回值直接传入对应的push_back,append等函数，右值对象传入时构造了临时对象，不仅需要负担深拷贝的开销，同时在函数调用结束后会对这些临时对象进行析构，造成析构错误（同样是同一对象资源析构两次的问题），解决方案是实现完整的移动语义相关函数，右值引用operator，右值引用参数输入等等。


## day5
## 非递归实现
由于使用递归函数下降的方式实现parse会导致大量的栈空间占用，在Json结构相对复杂的情况下，可能会因为递归深度导致栈空间不足。所以采用模拟栈的方式将原先的递归parse改为非递归parse
### AST状态维护
```cpp
enum ParseState {
    PARSE_VALUE,
    PARSE_OBJECT_KEY,
    PARSE_OBJECT_VALUE,
    PARSE_ARRAY_VALUE
};

// 存储解析上下文
struct ParseContext {
    ParseState state;
    Json* json;
    std::string key;  // 用于对象的键
    ParseContext(ParseState s, Json* j) : state(s), json(j) {}
};
```
每次解析的上下文由ParseContext进行管理
### 解决问题
- 一开始出于代码复用的考虑将PARSE_OBJECT_VALUE状态重定向到PARSE_VALUE(该状态负责基本类型的解析)，增加该重定向过程引发一系列问题，比如无法从嵌套object或者array中跳出。
- 当前方案为使用重复的PARSE_VALUE段进行解析，解析过程中PARSE_OBJECT_VALUE和PARSE_OBJECT_KEY等不会构建下层解析状态，直接进switch作基本类型解析。

## day6
### XML序列化
和`to_pretty_string`的实现逻辑没有太大的区别

### 解析错误时的提示
#### 思路
如何规范输出错误，我认为作为一个parser的解析错误提示，应当至少具备以下输出：

- 输出当前上下文（context）（由状态决定，比如PARSE_OBJECT_KEY是指parser”认为“自己在解析对象的键）
- 语法错误，由上层的异常给出（如parse_number给出的int溢出等）
- 当前位置, 亦即当前无法解析的字符的偏移位置
所以，我将异常对象的字符串设定以下格式
```cpp
throw std::logic_error(std::string(e.what()) + 
    "\nContext: " + "context.state" + 
    "\nPosition: " + std::to_string(_pos));   
```
`e.what()`是上层简单类型解析函数抛出的异常字符串。

### 测试
唉，最后测试一个benchmark发现没处理parse_string的转义字符问题
