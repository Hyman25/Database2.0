# Database2.0 更新纪要
### 6.11 18:30 updated by zhanghx
现在可以通过oj了，但是也暴露出一些bug(已修复).

### 6.11 16:50 updated by zhanghx

增加了编译选项，只有在windows下才编译网络连接模块。

将DB.saving函数中输出的提示信息注释掉了。

### 6.9 14:45 updated by zhanghx
做了一些架构调整。将没有from时ALU模块的调用从command类内移到了databaseMap类中，保证command类只负责处理输入，不负责输出；DataBaseMap类中增加了判断某个表或数据库是否存在的函数。

现在网络连接的指令有变化。

客户端:

```mysql
link server;
192.168.0.100 6000
CREATE DATABASE OOP;
USE OOP;
CREATE TABLE oop_info(stu_id INT NOT NULL, PRIMARY KEY(stu_id), stu_name CHAR);
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011243, "a");
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011344, "b");
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011445, "c");
SELECT COUNT(*) from oop_info;
DROP DATABASE OOP;
exit;
```

```mysql
————————————————
输入要连接的Ip地址与端口号：
Connected!
————连接服务器————
CREATE DATABASE OOP Done!
USE OOP Done!
CREATE TABLE oop_info(stu_id INT NOT NULL, PRIMARY KEY(stu_id), stu_name CHAR) Done!
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011243, "a") Done!
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011344, "b") Done!
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011445, "c") Done!
SELECT COUNT(*) from oop_info Done!
COUNT(*)
3
DROP DATABASE OOP Done!
————已断开连接————
```
服务器：

```mysql
link client;
```

```mysql
————————————————
本机IP[1]:192.168.0.100
使用端口：6000
————————————————
服务器已启动:
监听中...
————连接客户端————
CREATE DATABASE OOP
USE OOP
CREATE TABLE oop_info(stu_id INT NOT NULL, PRIMARY KEY(stu_id), stu_name CHAR)
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011243, "a")
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011344, "b")
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011445, "c")
SELECT COUNT(*) from oop_info
COUNT(*)
3
DROP DATABASE OOP
exit
————已断开连接————
```



### 6.9 12:15 updated by HH
修复select输出的bug

### 6.9 11:34 updated by HH
修复order by的排序bug

### 6.9 11:10 updated by zhanghx

现在可以使用select (列名 as 别名, ...) from tablename;的语法了。别名只影响输出，不能用在groupby和orderby中。

```mysql
CREATE DATABASE OOP;
USE OOP;
CREATE TABLE oop_info(stu_id INT NOT NULL, stu_name CHAR, PRIMARY KEY(stu_id));
insert into oop_info(stu_id, stu_name) values (1,"a");
insert into oop_info(stu_id, stu_name) values (2,"b");
insert into oop_info(stu_id, stu_name) values (3,"c");
select stu_id as id from oop_info;
select 1+ stu_id as 1+id from oop_info;
select stu_name as name, Sin(stu_id) as sin(id) from oop_info;
DROP DATABASE OOP;
```

```mysql
id
1
2
3
1+id
2
3
4
name    sin(id)
a       0.841471
b       0.909297
c       0.141120
```

### 6.8 16:15 updated by zhanghx

select函数改了一堆bug；现在急需更多测试样例。

### 6.8 14:49 updated by HH
SelectData中，select支持运算式，group by支持运算式，order by支持运算式。

### 6.8 12:20 updated by zhanghx

现在不带列名的算术逻辑运算支持同时进行传入多个算式了。

```mysql
select 1+2,2+3;
1+2     2+3
3       5
```

更新了ALU类，现在算术逻辑运算的结果将不在ALU类中输出，而是作为一个vector<string>对象返回。

更新了command类select函数调用ALU类的方式。下例作为参考：

```mysql
insert into oop_info(stu_id, stu_name) values (1,"a");
insert into oop_info(stu_id, stu_name) values (2,"b");
insert into oop_info(stu_id, stu_name) values (3,"c");
select 1+stu_id from oop_info;
```

调用时：

```c++
ALU obj("1+std");
obj.process(table*, vector<string> keys)//oop_info的指针,假如keys={"1","2","3"}
    //将返回{”2“，”3“，”4“}
```

### 6.8 10:40 updated by HH
修复了Table中SelectData的bug：即当select结果为空仍然输出表头。

修复CreateTable中的bug：即null值与标准输出相反。

### 6.8 10:26 updated by HH
修复了Table中SelectData的bug

### 6.8 9:54 updated by zhanghx

修复了前端select函数的bug。

Select前端传入数据说明:

接口：

```c++
	void SelectData(const std::vector<std::string>& attrName,
		const std::string& countAttr,
		int countpos,
		const std::vector<std::string>& groupby,
		const std::string& orderbyAttr,
		const std::string& orderbyCount,
		const std::string& Where,
		const std::string& filename);
```

```mysql
SELECT stu_name, COUNT(*) from oop_info GROUP BY stu_name ORDER BY COUNT(*);
#attrname={"stu_name","COUNT(*)"},countAttr="*",countpos=1,groupby={"stu_name"}，orderbucount="*"
#其他项都为空
```

```mysql
SELECT * INTO OUTFILE 'output_file' FROM oop_info;
#attrname={"stu_id","stu_name"},countpos=-1,filename="output_file"
```

### 6.8 9:38 updated by HH
**select** 基础功能模块完成

### 6.7 22:00 updated by zhanghexiao
ALU模块现在可以支持比较运算符了，包括:=,<,>,>=,<=,<>,!=；

### 6.7 16:50 updated by HH

Internet的sever端通过recv返回值判断client端是否断开，防止client端意外关闭导致的sever端死循环。

### 6.7 12:00 updated by zhanghexiao

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
select Sin(stu_id*Pi()) from oop_info;
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

### 6.7 9:30 updated by zhanghexiao

现在远程访问模块已经调通了，在本机上比较稳定。

### 6.6 21:42 updated by zhanghexiao

完成了command类的select，load函数;

修改了table类selectData的接口。

将ALU类接入主程序中，现在如果select语句中不含from关键词，将利用ALU类进行科学计算。例：

```mysql
select 1 + 2 and PI();
1 + 2 and PI( )
1
```
