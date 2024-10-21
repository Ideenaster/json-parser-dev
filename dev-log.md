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

