#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "Row.h"

struct Data {
	std::string value;
	std::string type;
	bool operator< (const Data d) const;
	bool operator== (const Data d)const;
};


class Table {
private:
	std::string key;     //主键，应为attribute中元素 
	std::string key_type;  
public:
	std::string getkeytype() { return key_type; }
	std::string getkey() { return key; }

	std::map<Data, Row> row_map;
	std::vector<Attribute> attr_list;

	Table() = default;
	Table(std::vector<Attribute> attr, std::string _key);

	void insert(std::vector<std::string> attr_name, std::vector<std::string> value);  //插入行  
	void OutputAttr();   //更新2：按输入顺序输出属性名，格式参考select *
	void find(const std::string key);
	std::set<Data> getallkeys();
	std::set<Data> keywherecluase(const Clause c);

	std::map<Data, int> Count(std::string);
};

