# 数据库 第二阶段 实验报告

### 一、概述

本程序在第一阶段代码的基础上继续开发，保留了前一组已有的大部分代码，并在此基础上增加了网络连接模块和计算模块，在Command类和Table类中增加了一些新的接口以实现新的功能。

设计时尽可能将地降低了模块之间的耦合度。Command类只负责解析字符串，ALU类只负责处理算式，文件操纵和屏幕输出交由各个数据结构类处理。在字符串解释时大量使用了正则表达式，大大提高了语法解析的鲁棒性，降低了程序扩展的难度。

调用Socket完成了网络连接模块，并借助宏定义判断操作系统，实现了网络模块的跨平台运行。

### 二、基础功能

改进了原第一阶段代码不支持大关键字小写混用的问题。本程序对关键字大小写不敏感。通过大量使用正则表达式，可以处理用户不规范的输入。

group by支持同时输入多列，如下例：

```mysql
CREATE DATABASE SCHOOL;
USE SCHOOL;
CREATE TABLE score (score_id int NOT NULL,course_num int NOT NULL, score int NOT NULL, PRIMARY KEY ( score_id ));
INSERT Into score(score_id, course_num, score) VALUES (1, 1, 80);
INSERT iNto score(score_id, course_num, score) VALUES (2, 1, 80);
INSERT INTO score(score_id, course_num, score) VALUES (3, 1, 80);
INSERT INTO score(score_id, course_num, score) VALUES (4, 1, 78);
INSERT INTO score(score_id, course_num, score) VALUES (5, 2, 78);
INSERT INTO score(score_id, course_num, score) VALUES (6, 2, 78);
INSERT INTO score(score_id, course_num, score) VALUES (7, 2, 78);
INSERT INTO score(score_id, course_num, score) VALUES (8, 2, 80);
SELECT course_num, score FROM score GROUP BY course_num, score;
DROP DATABASE SCHOOL;
```

```mysql
course_num      score
1       80
1       78
2       78
2       80
```

### 三、附加功能

| 功能                              |
| --------------------------------- |
| 网络连接(win&mac)                 |
| 算术&逻辑&比较运算                |
| 数字函数(sin,cos,exp,abs,pi,etc.) |
| 存档(每次操作对应到文件)          |
| select...as...                    |

#### 网络连接

```mysql
Link Server; #客户端连接服务器
Link Client；#服务器连接客户端
```

本程序使用Socket实现了局域网内部的网络连接。通过宏定义控制判断操作系统(Win&Mac)，控制编译内容。开始运行后，服务端将显示 ip 地址与端口号，客户端输入相应 ip 与端口号后连接。由客户端输入命令，服务端会将执行结果发送至客户端。样例：

客户端输入：
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
客户端输出：
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
服务器输入：

```mysql
link client;
```
服务器输出：
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

如果连接失败，将停止连接并报错：

```mysql
link server;
————————————————
输入要连接的Ip地址与端口号：
123.456.7.89 8000
LINK ERROR!断开连接!
```

#### 算术&逻辑&比较运算和数字函数

在select语句中实现了**算术，逻辑，比较运算符**和六个**数字函数**。并与其它基础功能有机地结合在一起。通过正则表达式保证处理的鲁棒性，可以处理用户不规范的输入。

算术运算符包括四则运算(+,-,*,/,DIV),取余(MOD,%). 其中“/”表示浮点数除法，DIV表示整除。执行取余运算时，会自动将小数截断为整数。

逻辑运算符包括与(and,&&)、或(or,||)，非(not,!)，异或(xor). 

比较运算符包括大于(>)、小于(<)、等于(=)、大于等于(>=)、小于等于(<=)、不等于(!=,<>).

逻辑运算和比较运算的返回值只有0(True)或1(False);

实现了数字函数Sin(),Cos(),Exp(),Abs(),PI(),LN()共6个数字函数。

如果在运算中出现了除以0、超出数字函数定义域的错误或无法解释的字符，将返回Null; char类型的数据全部视为0。最多输出6位小数。

一般来说，算式和数字函数不能直接出现在where子句中。但如果将算式**整体视作列名**，则可在where子句中有限制的使用。

样例1：

```mysql
CREATE DATABASE SCHOOL;
USE SCHOOL;
CREATE TABLE score (score_id int NOT NULL,course_num int NOT NULL,student_num int NOT NULL, score int NOT NULL, PRIMARY KEY ( score_id ));
INSERT INTO score(score_id, course_num, student_num, score) VALUES (3, 1, 15003, 80);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (8, 2, 16004, 89);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (10, 3, 15002, 77);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (13, 4, 15001, 90);
SELECT score_id, score_id+6, score_id-5 FROM score; 
SELECT course_num, (course_num *  5)/  4 FROM score;
SELECT student_num, student_num Mod 10 FROM score;
SELECT score, score+ score DIV 10 +score Mod 9 FROM score;
SELECT student_num, (student_num MOd 10)= 4 FROM score;
SELECT score, (score >80 && score < = 90) FROM score;
SELECT course_num, course_num <> 2 or course_num >=4 FROM score;
SELECT score, score+exP(sin(score)), score+abs(-1*5) FROM score;
SELECT course_num, course_num*pi() From score;
SELECT score/0 from score;
SELECT student_num, score, score+5 FROM score WHERE score+5 >85;
DROP DATABASE SCHOOL;
```

输出如下：

```mysql
#SELECT score_id, score_id+6, score_id-5 FROM score;
score_id        score_id+6      score_id-5
3       9       -2
8       14      3
10      16      5
13      19      8
#SELECT course_num, (course_num *  5)/  4 FROM score;
course_num      (course_num * 5)/ 4
1       1.25
2       2.5
3       3.75
4       5
#SELECT student_num, student_num Mod 10 FROM score;
student_num     student_num Mod 10
15003   3
16004   4
15002   2
15001   1
#SELECT score, score+ score DIV 10 +score Mod 9 FROM score;
score   score+ score DIV 10 +score Mod 9
80      96
89      105
77      89
90      99
#SELECT student_num, (student_num MOd 10)= 4 FROM score;
student_num     (student_num MOd 10)=4
15003   0
16004   1
15002   0
15001   0
#SELECT score, (score >80 && score < = 90) FROM score;
score   (score >80 && score <=90)
80      0
89      1
77      0
90      1
#SELECT course_num, course_num <> 2 or course_num >=4 FROM score;
course_num      course_num <> 2 or course_num >=4
1       1
2       0
3       1
4       1
#SELECT score, score+exP(sin(score)), score+abs(-1*5) FROM score;
score   score+exP(sin(score))   score+abs(-1*5)
80      80.370135       85
89      91.363325       94
77      79.716978       82
90      92.444882       95
#SELECT course_num, course_num*pi() From score;
course_num      course_num*pi()
1       3.141593
2       6.283185
3       9.424778
4       12.566371
#SELECT score/0 from score;
score/0
NULL
NULL
NULL
NULL
#SELECT student_num, score, score+5 FROM score WHERE score+5 >85;
student_num     score   score+5
16004   89      94
15001   90      95
```

样例2：

```mysql
CREATE DATABASE SCHOOL;
USE SCHOOL;
CREATE TABLE score (score_id int NOT NULL,course_num int NOT NULL,student_num int NOT NULL, score int NOT NULL, PRIMARY KEY ( score_id ));
INSERT INTO score(score_id, course_num, student_num, score) VALUES (1, 1, 15001, 89);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (3, 1, 15003, 80);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (5, 2, 15001, 85);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (6, 2, 15002, 78);
INSERT INTO score(score_id, course_num, student_num, score) VALUES (8, 2, 16004, 89);
SELECT student_num, course_num, score FROM score GROUP BY student_num, score;
SELECT student_num AS name, COUNT(score_id) INTO OUTFILE 'out' FROM score GROUP BY student_num ORDER BY COUNT(score_id);
SELECT score+  4, score  -9, score * 5, score/2, score Div 8, score Mod 9 FROM score;
SELECT student_num, course_num, score> =85 AND score< 95 AND score <> 90 FROM score ORDER BY student_num;
SELECT student_num,abs(-1*score),count(*) FROM score WHERE score>80 GROUP BY student_num;
SELECT score_id, student_num AS name, score>80 && score<95, COUNT(score_id) FROM score GROUP BY student_num ORDER BY COUNT(score_id);
SELECT score_id, student_num AS name, COS(score)+80, COUNT(score_id) FROM score GROUP BY student_num ORDER BY COUNT(score_id);
SELECT score_id, student_num AS name, (100-EXP(SIN(score)))/2, COUNT(score_id) FROM score GROUP BY student_num ORDER BY COUNT(score_id);
DROP DATABASE SCHOOL;
```
输出如下：
```mysql
#SELECT student_num, course_num, score FROM score GROUP BY student_num, score;
student_num     course_num      score
15001   1       89
15003   1       80
15001   2       85
15002   2       78
16004   2       89
#SELECT score+  4, score  -9, score * 5, score/2, score Div 8, score Mod 9 FROM score;
score+ 4        score -9        score * 5       score/2 score Div 8     score Mod 9
93      80      445     44.5    11      8
84      71      400     40      10      8
89      76      425     42.5    10      4
82      69      390     39      9       6
93      80      445     44.5    11      8
#SELECT student_num, course_num, score> =85 AND score< 95 AND score <> 90 FROM score ORDER BY student_num;
student_num     course_num      score>=85 AND score< 95 AND score <> 90
15001   1       1
15001   2       1
15002   2       0
15003   1       0
16004   2       1
#SELECT student_num,abs(-1*score),count(*) FROM score WHERE score>80 GROUP BY student_num;
student_num     abs(-1*score)   count(*)
15001   89      2
16004   89      1
#SELECT score_id, student_num AS name, score>80 && score<95, COUNT(score_id) FROM score GROUP BY student_num ORDER BY COUNT(score_id);
score_id        name    score>80 && score<95    COUNT(score_id)
3       15003   0       1
6       15002   0       1
8       16004   1       1
1       15001   1       2
#SELECT score_id, student_num AS name, COS(score)+80, COUNT(score_id) FROM score GROUP BY student_num ORDER BY COUNT(score_id);
score_id        name    COS(score)+80   COUNT(score_id)
3       15003   79.889613       1
6       15002   79.142197       1
8       16004   80.510177       1
1       15001   80.510177       2
#SELECT score_id, student_num AS name, (100-EXP(SIN(score)))/2, COUNT(score_id) FROM score GROUP BY student_num ORDER BY COUNT(score_id);
score_id        name    (100-EXP(SIN(score)))/2 COUNT(score_id)
3       15003   49.814933       1
6       15002   49.164035       1
8       16004   48.818338       1
1       15001   48.818338       2
```
文件输出：

```mysql
#SELECT student_num AS name, COUNT(score_id) INTO OUTFILE 'out' FROM score GROUP BY student_num ORDER BY COUNT(score_id);
#out
15003	1
15002	1
16004	1
15001	2
```
如果select语句中不含from关键词，将进行科学计算：
```mysql
select 1 + 2 and PI();
select 1+2,2+3;
select ln(2),ln(- 1);
```

```mysql
1 + 2 and PI()
1
1+2     2+3
3       5
ln(2)	ln(- 1)
0.693147	NULL
```

#### 存档

存档实现了每次操作对应到文件。退出程序后，数据仍然存在。再次运行时可以继续使用。

样例1：

```
CREATE DATABASE OOP;
USE OOP;
CREATE TABLE poi(lkj INT, mnb INT, bvc INT, dfj CHAR, PRIMARY KEY(mnb));
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (1, 2, 3, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (2, 3, 4, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (3, 4, 5, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (4, 5, 6, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (5, 6, 7, "a");
INSERT INTO poi(mnb, bvc, dfj) VALUES (7, 7, "a");
#退出,重新启动
USE OOP;
DELETE FROM poi  WHERE lkj>3 OR mnb=7 AND lkj=10;
SELECT * from poi;
DROP DATABASE OOP;
```

```mysql
lkj	mnb	bvc	dfj
1	2	3	a
2	3	4	a
3	4	5	a
NULL	7	7	a
```

样例2：

```mysql
CREATE DATABASE OOP;
USE OOP;
CREATE TABLE qwe(asd DOUBLE, rty INT, zxc CHAR, PRIMARY KEY(asd));
INSERT INTO qwe(rty, zxc, asd) VALUES (1, "a", 2.1233242398479238098230948329084347897878);
INSERT INTO qwe(zxc, asd, rty) VALUES ("b", 1.1, 3);
INSERT INTO qwe(asd, rty, zxc) VALUES (4.2, 5, "c");
select * from qwe;
#退出,重新启动
USE OOP;
UPDATE qwe SET asd=5 WHERE zxc="a";
select * from qwe;
DROP DATABASE OOP;
```

```mysql
asd	rty	zxc
1.1	3	b
2.1233242398479238098230948329084347897878	1	a
4.2	5	c
#退出,重新启动
asd	rty	zxc
1.1	3	b
4.2	5	c
5	1	a
```

#### select...as...

```mysql
select (列名 as 别名, ...) from tablename (where ...) (group by ...) (order by...);
```

实现了select (列名 as 别名, ...) 的语法，如下例所示。

```mysql
CREATE DATABASE OOP;
USE OOP;
CREATE TABLE oop_info(stu_id INT NOT NULL, stu_name CHAR, PRIMARY KEY(stu_id));
insert into oop_info(stu_id, stu_name) values (2,"b");
insert into oop_info(stu_id, stu_name) values (3,"c");
insert into oop_info(stu_id, stu_name) values (4,"c");
select stu_id as id from oop_info;
select 1+stu_id as 1+id from oop_info where 1+stu_id >3;
select stu_name as name, Sin(stu_id) as sin(id) from oop_info group by stu_name order by Sin(stu_id);
DROP DATABASE OOP;
```

```mysql
id
2
3
4
1+id
4
5
name    sin(id)
c       0.14112
b       0.909297
```

### 四、反思

第二阶段开始之前，我们经过多次讨论，选定了相对较好的E组进行二次开发。E组数据组织结构比较清晰，但是仍然因为层层调用接口造成代码阅读上的困难。经过仔细分析与阅读，基于对E组代码的理解，我们选定了第二基础功能的开发方式，以及适合二次的拓展功能。

由于E组原有代码将部分功能的实现放在处理输入的函数中，造成代码职责不明确，并且难以拓展开发，我们决定重构这部分代码，将输入处理与功能实现解耦，使代码易于阅读与维护。第二阶段绝大部分功能于select中，因此，我们对select语句进行妥善处理。我们首先通过语句解析出重要成分，然后将这些成分作为参数，传递到下一层的函数中处理。由此，输入处理与功能实现得以分离。原有where接口难以调整，因此我们选择不开发多表的where字句，而是选择新增数字函数等拓展功能。

阅读他人的代码也给我们带来了经验与教训。每个人编写代码的风格与思路不尽相同，例如E组代码where字句的实现使用分别判断每一行记录是否符合where字句中的某一条件，先以此筛选出多个集合，再根据and、or取交集或并集，最后筛选出结果。我们组一开始虽然同样采用这一思路，但是在随后采用分别判断每一行记录是否符合整个where语句的方式。在阅读过程中，E组的这一不同实现给我们带来了一些启发。当然，E组存在的缺陷，例如代码冗余量大、程序鲁棒性有待提升等，也警醒我们在未来的代码中避免类似情况。

此次OOP大作业是我们第一次以小组方式完成代码，是一次难得的体验，让我们深有感想。在小组合作写代码时，首先需要定出代码的基本框架，确定基本的接口名称及参数，同时尽可能使得每部分功能不重叠，这样才便于分工与维护。由于合理地分工，我们各司其职，一旦发现bug，可以迅速定位，并且由执行相应部分开发的组员快速修复。

大作业是一项工程，不同于OJ上输入十分规整的题目，因此难度大、完成周期长。在大作业过程中，我们需要自学能力，例如一位成员学习正则表达式并应用于工程中，保障输入处理的效率与准确性。面对层出不穷、各式各样的错误，我们需要信息检索能力，上网搜索遇到的各种“奇葩”bug。并且，我们学习了如何利用Github这一有力工具进行代码管理，解决了第一阶段在微信群内传输文件，代码难以同步等问题。

我们也有仍待改进的问题，例如在程序中使用较多的容器使得程序效率有所下降、编程时较难合理使用各类设计模式等等。这些会是我们需要提高的地方。当然，总体而言，我们在这次大作业付出很多，也收获很多。汲取这些经验教训，我们将更好地成长。