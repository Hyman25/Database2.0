#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>

class Table;
struct Clause {
	//bool identical = false;   //若为true则表示恒等式，不需要判断，直接返回true即可（对应 * ）
	std::string left;   //被比较的属性,即stu_id
	std::string right;   //给定值，一律做表达式的右侧值，即2018000000
	std::string cmp_op;    //比较运算符，">","<","="
};

struct Attribute {
	std::string name;    //属性名
	std::string type;    //属性类型，应为：int/char/double 
	bool Null = false;    //属性标志，0表示NOT NULL ，1表示非NOT NULL
	bool Key = false;   //属性标志，0表示不是主键，1表示是主键
	std::string Default = "";    //可能是默认值
	std::string Extra = "";
};

class Row {
private:
	Table* table = nullptr;
public:
	Row() {}
	std::map<std::string, std::string> data;
	void into(Table* ta, std::vector<std::string> attr_name, std::vector<std::string> value);
	void outputrow(Table* ta);
};

void OutputData(std::string, std::string);
