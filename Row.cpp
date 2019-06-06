#include"Row.h"
#include"Table.h"

void Row::into(Table* ta, std::vector<std::string> attr_name, std::vector<std::string> value) {

	table = ta;
	for (auto i = 0; i < ta->attr_list.size(); i++)  //构建横向为空的一行数据
	{
		std::string a = ta->attr_list[i].name;
		data.insert(std::pair<std::string, std::string>(a, "NULL"));
	}

	for (auto i = 0; i < attr_name.size(); i++) //为对应空格赋值；
	{
		std::string a = attr_name[i], b = value[i];
		data[a] = b;
	}
}

void Row::outputrow(Table* ta)
{
	table = ta;
	for (auto i = 0; i <ta->attr_list.size(); i++) 
	{
		std::string a = ta->attr_list[i].name;
		std::string type = ta->attr_list[i].type;
		OutputData(data[a], type);
		if (i < ta->attr_list.size() - 1)
			std::cout << "\t";
	}
	std::cout << "\n";
}

void OutputData(std::string value, std::string type) {
	std::stringstream ss(value);
	if (value == "NULL") {
		std::cout << "NULL";
	}
	else if (type == "int" || type == "INT") {
		int tmp;
		ss >> tmp;
		std::cout << tmp;
	}
	else if (type == "double" || type == "DOUBLE") {
		double tmp;
		ss >> tmp;
		std::cout << tmp;
	}
	else if (type == "char" || type == "CHAR") {
		char tmp;
		ss >> tmp;
		std::cout << tmp;
	}
	else {
		std::cout << "ERROR!\n";
	}
}