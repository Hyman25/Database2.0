# Database2.0 测试样例

我们需要为附加功能编写测试样例，样例需要展示出功能的正确性和鲁棒性。

| 功能                              | 分数    |
| --------------------------------- | ------- |
| 网络连接(win&mac)                 | 15%     |
| 算术&逻辑&比较运算                | 5%+5%+? |
| 存档                              | 5%+5%   |
| 数字函数(sin,cos,exp,abs,pi,etc.) | 10%     |
| select..as..                      | ?       |

每组一到两个样例，我们需要大概10个样例。

---

```mysql
CREATE DATABASE SCHOOL;
USE SCHOOL;
CREATE TABLE score (score_id int NOT NULL,course_num int NOT NULL,student_num int NOT NULL, score int NOT NULL, PRIMARY KEY ( score_id ));
INSERT INTO score(score_id, course_num, student_num, score) VALUES (1, 1, 15001, 89);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (2, 1, 15002, 78);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (3, 1, 15003, 80);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (4, 1, 16004, 78);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (5, 2, 15001, 85);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (6, 2, 15002, 78);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (7, 2, 15003, 75);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (8, 2, 16004, 89);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (9, 3, 15001, 87);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (10, 3, 15002, 77);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (11, 3, 15003, 88);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (12, 3, 16004, 90);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (13, 4, 15001, 90);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (14, 4, 15002, 98);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (15, 4, 15003, 89);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (16, 4, 16004, 88);
SELECT student_num AS name, COUNT(score_id) INTO OUTFILE 'out' FROM score GROUP BY student_num ORDER BY COUNT(score_id);
DROP DATABASE SCHOOL;
```
---

网络连接

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
---

select (列名 as 别名, ...) from tablename;别名只影响输出，不能用在groupby和orderby中。

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


select (逻辑&算术&比较表达式) from tablename; 在这些运算中，char类型的数据均视为0.

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
---

如果select语句中不含from关键词，将利用ALU类进行科学计算

```mysql
select 1 + 2 and PI();
1 + 2 and PI( )
1
select 1+2,2+3;
1+2     2+3
3       5
```


