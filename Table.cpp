#include"Table.h"

bool Data::operator< (const Data d) const {
	if (type == "int") {
		int left, right;
		left = stoi(this->value);
		right = stoi(d.value);
		return left < right;
	}
	else if (type == "double") {
		double left, right;
		left = stod(this->value);
		right = stod(d.value);
		return left < right;
	}
	else {
		return this->value < d.value;
	}
}

bool Data::operator== (const Data d) const {
	if (type == "int") {
		int left, right;
		left = stoi(this->value);
		right = stoi(d.value);
		return left == right;
	}
	else if (type == "double") {
		double left, right;
		left = stod(this->value);
		right = stod(d.value);
		return left == right;
	}
	else {
		return this->value == d.value;
	}
}
//Not Null关键字缺乏对应实现
void Table::insert(std::vector<std::string> attr_name, std::vector<std::string> value) {
	Row new_row;
	new_row.into(this, attr_name, value);
	Data a;
	a.type = key_type;
	a.value = new_row.data[key];
	row_map.insert(std::pair<Data, Row>(a, new_row));
}

void Table::OutputAttr()
{
	for (auto it = attr_list.begin() ; it != attr_list.end(); it++)
	{
		std::cout << it->name ;
		if (it != attr_list.end()-1)
			std::cout << "\t";
	}
	std::cout << "\n";
}

void Table::find(const std::string key)
{
	Data a;
	a.type = key_type;
	a.value = key;
	row_map[a].outputrow(this);
}

std::set<Data> Table::getallkeys()
{
	std::set<Data> keys;
	for (auto iter = row_map.begin(); iter != row_map.end(); iter++)
	{
		keys.insert(iter->first);
	}
	return keys;
}

std::set<Data> Table::keywherecluase(const Clause c)
{
	Clause d;
	d = c;
	std::set<Data> keys;
	std::string a = "", b = "";
	bool leftattr = 0, rightattr = 0;
	for (auto iter = attr_list.begin(); iter != attr_list.end(); iter++)  //循环判断左右是否为属性名
	{
		if (c.left == iter->name)
		{
			leftattr = 1;
			a = iter->type;
		}
		if (c.right == iter->name)
		{
			rightattr = 1;
			b = iter->type;
		}

	}


	if((!rightattr && leftattr)|| rightattr && !leftattr)   //只有一个属性名的情况
	{
	
		//if (!rightattr && leftattr) //左边是值 右边是属性名,将两者交换
		if (!leftattr && rightattr) //左边是值 右边是属性名,将两者交换
	{
		std::string p = d.left;
		d.left = d.right;
		d.right = p;
		if (d.cmp_op == "<")
		{
			d.cmp_op = ">";
		}
		else if (d.cmp_op == ">")
		{
			d.cmp_op = "<";
		}
		a = b;

	}

	for (auto iter = row_map.begin(); iter != row_map.end(); iter++) //左边为属性名 右边为值的情况
	{
		if (iter->second.data[d.left] == "NULL")
			continue;
		if (a == "int" || a == "INT")
		{
			int aa = stoi(iter->second.data[d.left]);
			int bb = stoi(d.right);
			if (d.cmp_op == ">" && (aa > bb))
				keys.insert(iter->first);
			else if (d.cmp_op == "=" && (aa == bb))
				keys.insert(iter->first);
			else if (d.cmp_op == "<" && (aa < bb))
				keys.insert(iter->first);
		}

		else if (a == "double" || a == "DOUBLE")
		{
			double aa = stof(iter->second.data[d.left]);
			double bb = stof(d.right);
			if (d.cmp_op == ">" && (aa > bb))
				keys.insert(iter->first);
			else if (d.cmp_op == "=" && (aa == bb))
				keys.insert(iter->first);
			else if (d.cmp_op == "<" && (aa < bb))
				keys.insert(iter->first);
		}
		else
		{
			std::string aa = iter->second.data[d.left];
			std::string bb = d.right;
			if (d.cmp_op == ">" && (aa > bb))
				keys.insert(iter->first);
			else if (d.cmp_op == "=" && (aa == bb))
				keys.insert(iter->first);
			else if (d.cmp_op == "<" && (aa < bb))
				keys.insert(iter->first);
		}


	}
}
	else                //两边均为属性名的情况
	{
		for (auto iter = row_map.begin(); iter != row_map.end(); iter++) 
		{
			if (((iter->second.data[d.left] == "NULL")|| (iter->second.data[d.left] == "NULL"))&& !((iter->second.data[d.left] == "NULL") && (iter->second.data[d.left] == "NULL")))//当左右两边只有一个为空时 不进行比较
				continue;

			if (d.cmp_op == ">" && (iter->second.data[d.left] > iter->second.data[d.right]))
				keys.insert(iter->first);
			else if (d.cmp_op == "=" && (iter->second.data[d.left] == iter->second.data[d.right]))
				keys.insert(iter->first);
			else if (d.cmp_op == "<" && (iter->second.data[d.left] < iter->second.data[d.right]))
				keys.insert(iter->first);
			
		}
	}
	return keys;
}

std::map<Data, int> Table::Count(std::string)
{
	return std::map<Data, int>();
}
