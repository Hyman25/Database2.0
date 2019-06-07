# Database2.0 更新纪要

推荐使用Typora编辑查看markdown文件。

### 6.7 16:50 updated by HH

Internet的sever端通过recv返回值判断client端是否断开，防止client端意外关闭导致的sever端死循环。

### 6.7 12：00 updated by zhanghexiao

现在可以使用“select (逻辑&算术表达式） from tablename;”的语法了，一个测试样例。

```mysql
CREATE DATABASE OOP;
USE OOP;
CREATE TABLE oop_info(stu_id INT NOT NULL, stu_name CHAR, PRIMARY KEY(stu_id));
insert into oop_info(stu_id, stu_name) values (1,"a");
insert into oop_info(stu_id, stu_name) values (2,"b");
insert into oop_info(stu_id, stu_name) values (3,"c");
select 1+stu_id from oop_info;
select 1 and not stu_id from oop_info;
select Sin(std_id*Pi()) from oop_info;
DROP DATABASE OOP;
```

输出：

```mysql
1 + stu_id
2
3
4
1 and not stu_id
0
0
0
SIN( stu_id * PI( ) )
1.22465e-16
-2.44929e-16
3.67394e-16
```

### 6.7 9：30 updated by zhanghexiao

现在远程访问模块已经调通了，在本机上比较稳定。
大家可以尽情测试。

### 6.6 21:42 updated by zhanghexiao

完成了command类的select，load函数;

修改了table类selectData的接口。

将ALU类接入主程序中，现在如果select语句中不含from关键词，将利用ALU类进行科学计算。例：

```mysql
select 1 + 2 and PI();
1 + 2 and PI( )
1
```
