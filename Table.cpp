#include"Table.h"
std::set<Data> where_clause(std::string table_name, std::string clause);

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
Table::Table(std::string _name, std::vector<Attribute> attr, std::string _key) : name(_name), attr_list(attr), key(_key)
{
	for (auto it = attr.begin(); it < attr.end(); it++) {
		if ((*it).Key == true) {
			key_type = (*it).type;
			break;
		}
	}
}
//Not Null关键字缺乏对应实现！！
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

void Table::LoadFile(std::string filename, std::vector<std::string> attrname)
{
	std::ifstream fin;
	fin.open(filename);
	
	while (!fin.eof())
	{
		std::vector<std::string> value;
		std::string tmp;
		std::getline(fin, tmp);
		std::stringstream ss(tmp);
		while (ss >> tmp)
			value.push_back(tmp);
		if(!tmp.empty())
			insert(attrname, value);
	}
	fin.close();
}

std::map<Data, int> Table::Count(std::string name)//count函数，返回值为 <keyvalue, 1 or 0>
{
	std::map<Data, int> countResult;
	
	if (name != "*")
	{	
		for (auto itr : row_map)
		{
			if (itr.second.data[name] != "NULL")
				countResult[itr.first] = 1;
			else
				countResult[itr.first] = 0;
		}
	}
	else // name == "*"
	{
		for (auto itr : row_map)
		{
			bool nul = true;
			for (auto rowval : itr.second.data)
			{
				if (rowval.second != "NULL")
				{
					nul = false;
					countResult[itr.first] = 1;
					break;
				}
				if (nul)
					countResult[itr.first] = 0;
			}
		}
	}

	return countResult;
}

void Table::Group(std::vector<Data>& SelectResult, std::vector<std::string> groupby, std::map<Data, int>& countResult, std::string orderbyCount)
{
	std::map<Data, int> orderCount;
	if (!orderbyCount.empty())
	{
		orderCount = Count(orderbyCount);
	}
	for (auto key = SelectResult.begin(); key < SelectResult.end(); key++)
	{
		for (auto x = key + 1; x < SelectResult.end(); )
		{
			bool samegroup = true;
			for (auto attr : groupby) 
			{
				if (row_map[*key].data[attr] != row_map[*x].data[attr])
				{
					samegroup = false;
					break;
				}
			}
			if (samegroup)
			{
				countResult[*key] += countResult[*x];
				if (!orderCount.empty())
					orderCount[*key] += orderCount[*x];
				x = SelectResult.erase(x);
			}
			else
				x++;
		}
	}

	if (!orderbyCount.empty())
	{
		for (int i = 0; i < (int)SelectResult.size(); i++)
		{
			for (auto cur = SelectResult.begin(); cur < SelectResult.end() - i; cur++)
			{
				auto j = cur + 1;
				if (orderCount[*j]<orderCount[*cur])
				{
					Data tmp = *cur;
					*cur = *j;
					*j = tmp;
				}
			}
		}
	}
}



void Table::OrderAttr(std::vector<Data>& SelectResult, std::string orderbyAttr)
{
	std::string type;
	for (auto x : attr_list)
	{
		if (x.name == orderbyAttr)
		{
			type = x.type;
			break;
		}
	}

	for (int i = 1; i < (int)SelectResult.size(); i++)
	{
		for (auto cur = SelectResult.begin(); cur < SelectResult.end() - i; cur++)
		{
			auto j = cur + 1;
			if (type == "int")
			{
				int tmp1 = 0, tmp2 = 0;
				if(row_map[*j].data[orderbyAttr]!="NULL") tmp2 = stoi(row_map[*j].data[orderbyAttr]);
				if (row_map[*cur].data[orderbyAttr] != "NULL") tmp1 = stoi(row_map[*cur].data[orderbyAttr]);
				if (tmp2 < tmp1)
				{
					Data tmp = *cur;
					*cur = *j;
					*j = tmp;
				}
			}
			else if (type == "double")
			{
				double tmp1 = 0, tmp2 = 0;
				if (row_map[*j].data[orderbyAttr] != "NULL") tmp2 = stod(row_map[*j].data[orderbyAttr]);
				if (row_map[*cur].data[orderbyAttr] != "NULL") tmp1 = stod(row_map[*cur].data[orderbyAttr]);
				if (tmp2 < tmp1)
				{
					Data tmp = *cur;
					*cur = *j;
					*j = tmp;
				}
			}
			else
				if (row_map[*j].data[orderbyAttr] < row_map[*cur].data[orderbyAttr])
				{
					Data tmp = *cur;
					*cur = *j;
					*j = tmp;
				}
		}
	}
}


void Table::SelectData(const std::vector<std::string>& attrName,
	const std::vector<std::string>& attrNewName,
	int countpos, 
	const std::string& countAttr, 
	const std::vector<std::string>& groupby, 
	const std::string& orderbyAttr, 
	const std::string& orderbyCount, 
	const std::string& Where, 
	const std::string& filename)
{
	std::set<Data> tmp;
	if (!Where.empty())
		tmp = where_clause(name, Where);
	else
		tmp = getallkeys();

	std::vector<Data> SelectResult;
	for (auto x : tmp)
		SelectResult.push_back(x);//结果转存到SelectResult中
	
	int tmpAttrNum = 0;
	std::vector<Data> tmpAttrKey = SelectResult;
	for (auto x : attrName)//判断列名中是否含有运算式，若有，作为临时列加入表中，最后删除
	{
		ALU tmp(x);
		if (tmp.IsALU(x)) 
		{
			tmpAttrNum++;
			std::vector<std::string> value = tmp.process(this, tmpAttrKey);
			Attribute tmpAttr;
			tmpAttr.name = x;
			tmpAttr.type = "double";
			attr_list.push_back(tmpAttr);
			int valueNum = 0;
			for (auto key:SelectResult)
			{
				row_map[key].data[x] = value[valueNum++];
			}
		}
	}

	std::map<Data, int> countResult;
	if (!countAttr.empty())
		countResult = Count(countAttr);//count运算

	if (!groupby.empty()) // groupby非空
		Group(SelectResult, groupby, countResult, orderbyCount);//对结果分组

	if (!orderbyAttr.empty())//结果按照列名排序
		OrderAttr(SelectResult, orderbyAttr);
	
	//以下开始输出
	std::streambuf* coutBuf = std::cout.rdbuf();
	std::ofstream of;
	if (!filename.empty())//把输出定向到文件
	{	
		of = std::ofstream(filename);
		std::streambuf* fileBuf = of.rdbuf();
		std::cout.rdbuf(fileBuf);
	}
	
	if (filename.empty() && !SelectResult.empty())//输出表头
		for (auto i = attrNewName.begin(); i < attrNewName.end(); i++) 
		{
			std::cout << (*i) << (i == attrNewName.end() - 1 ? "\n" : "\t");
		}

	if ((int)attrName.size() == 1 && countpos == 0)//select后面只有count
	{
		int num = 0;
		for (auto x : SelectResult)
			if (countResult[x] == 1)
				num++;
		std::cout << num << "\n";
	}
	else//select后面不只是count
	{
		for (auto key : SelectResult)
		{
			int curpos = 0, countPrinted = 0;
			if (countAttr.empty())
				countPrinted = 1;

			for (auto attr = attrName.begin(); attr < attrName.end(); attr++)
			{
				if (!countAttr.empty() && !countPrinted && curpos == countpos)
				{
					std::cout << countResult[key] << "\t";
					countPrinted = 1;
				}
				std::cout << row_map[key].data[*attr] << ((attr == attrName.end() - 1 && countPrinted) ? "\n" : "\t");
				curpos++;
			}

			if (!countAttr.empty() && !countPrinted)
				std::cout << countResult[key] << "\n";
		}
	}

	if (!filename.empty())//关闭文件
	{
		of.flush();
		of.close();
		std::cout.rdbuf(coutBuf);
	}
	//输出结束

	//若有临时列，删除
	if (tmpAttrNum)
	{
		for (auto key : tmpAttrKey)
		{
			for (auto x = attr_list.end() - tmpAttrNum; x < attr_list.end(); x++)
			{
				row_map[key].data.erase(x->name);
			}
		}
		attr_list.erase(attr_list.end() - tmpAttrNum, attr_list.end());
	}

}
