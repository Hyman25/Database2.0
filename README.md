# Database2.0 更新纪要

推荐使用Typora编辑查看markdown文件。

### 6.6 21:42 updated by zhanghexiao

完成了command类的select，load函数;

修改了table类selectData的接口。

将ALU类接入主程序中，现在如果select语句中不含from关键词，将利用ALU类进行科学计算。例：

```mysql
select 1 + 2 and PI();
1 + 2 and PI( )
1
```
