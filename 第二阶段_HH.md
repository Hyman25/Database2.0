## 第二阶段

### 一、Table类新增内容

第二阶段的基本要求包括读写文件以及select中功能的增加。在保留原来代码的接口的同时，我们将Load、select操作放入Table类内，接受语句解析后的成分（例如列名等）作为参数，统一处理。

#### 1.从文件读入数据操作

```c++
void Table::LoadFile(std::string filename, std::vector<std::string> attrname)
```

接受文件名和列名作为参数，调用原有的 insert 接口储存数据。

#### 2.select操作

```c++
void Table::SelectData(const std::vector<std::string>& attrName,//列名
	const std::vector<std::string>& attrNewName,//select ... as 新的列名
	int countpos, //count所在的位置
	const std::string& countAttr, //count后面的列名
	const std::vector<std::string>& groupby, //group后面的列名
	const std::string& orderbyAttr, //order后面的列名
	const std::string& orderbyCount, //order by count 后面的列名
	const std::string& Where, //where字句
	const std::string& filename)//输出文件名
```

##### （1）参数说明

```sql
SELECT student_num AS name, COUNT(score_id) 
FROM score 
INTO OUTFILE 'out'
WHERE score = 90
GROUP BY student_num 
ORDER BY COUNT(score_id);
```

此时 attrName 中 储存 stu_name 和 COUNT(score_id),

 attrNewName 中储存 name 和 COUNT(score_id)，

countpos 的值为 1， countAttr 中储存 score_id ,

groupby 储存 student_num，

orderbyAttr 为空，orderbyCount 储存 score_id，

Where 中 储存 score = 90， filename 中储存 out。

*（若语句为ORDER BY score_id，则orderbyAttr 中储存 score_id，orderbyCount 为空。）*

##### （2）实现细节

**为了使程序每部分承担功能不致过于繁杂，实现时将 count、 group by、order by 分离出来。**

-----------

```c++
std::map<Data, int> Table::Count(std::string name)
```

Count 接受列名作为参数，返回 map<主键, 次数>。

------------

```c++
void Table::Group(std::vector<Data>& SelectResult, 
                  std::vector<std::string> groupby, 
                  std::map<Data, int>& countResult, 
                  std::string orderbyCount)
```

Group 接受筛选出的结果 SelectResult、分组的依据 groupby 作为参数。

同一组的数据只留下一个，其于删除；并记录该组原来有几行数据。

由于删除了一些数据，所以若 order by 子句中有 count，同样在这里处理。

countResult 是之前计数结果，在这里更新。orderbyCount 是排序的依据。

------------------

```c++
void Table::OrderAttr(std::vector<Data>& SelectResult, std::string orderbyAttr)
```

如果order by 后面没有 count 语句，则为列名，由此函数处理。

-------------

**SelectData实现**

* select后面支持 sin(stu_id*3+1) 之类的运算式，因此，首先调用 ALU 中的接口判断，并进行运算，将表达式以及运算结果作为临时的一列数据加入表中。

* 接下来调用原来的接口处理where语句，并将结果转存入SelectResult。

* 调用 count运算。

* 调用 group by 分组。

* 调用 order by 排序。

* 如果 filename 不为空则把输出定向到文件。

* 输出表头、内容。

* 关闭文件，删除临时列。

-------------------



### 二、网络连接

调用 winsocket2.h 库，服务端显示 ip 地址与端口号，客户端输入相应 ip 与端口号后连接。

客户端输入命令，服务端将执行结果发送到客户端。

  