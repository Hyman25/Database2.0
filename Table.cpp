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
		for (auto i = SelectResult.begin(); i < SelectResult.end(); i++)
		{
			std::vector<Data>::iterator cur = i;
			for (auto j = i + 1; j < SelectResult.end(); j++)
			{
				if (orderCount[*j]<orderCount[*cur])
				{
					Data tmp = *cur;
					*cur = *j;
					*j = tmp;
					cur = j;
				}
			}
		}
	}
}



void Table::OrderAttr(std::vector<Data>& SelectResult, std::string orderbyAttr)
{
	for (auto i = SelectResult.begin(); i < SelectResult.end(); i++)
	{
		std::vector<Data>::iterator cur = i;
		for (auto j = i + 1; j < SelectResult.end(); j++)
		{
			if (row_map[*j].data[orderbyAttr] < row_map[*cur].data[orderbyAttr])
			{
				Data tmp = *cur;
				*cur = *j;
				*j = tmp;
				cur = j;
			}
		}
	}
}


void Table::SelectData(const std::vector<std::string>& attrName, 
	int countpos, 
	const std::string& countAttr, 
	const std::vector<std::string>& groupby, 
	const std::string& orderbyAttr, 
	const std::string& orderbyCount, 
	const std::string& Where, 
	const std::string& filename)
{
	std::map<Data, int> countResult;
	if (!countAttr.empty())
		countResult = Count(countAttr);

	std::set<Data> tmp;
	if (!Where.empty())
		tmp = where_clause(name, Where);
	else
		tmp = getallkeys();

	std::vector<Data> SelectResult;
	for (auto x : tmp)
		SelectResult.push_back(x);
	
	if (!groupby.empty()) // groupby非空
		Group(SelectResult, groupby, countResult, orderbyCount);

	if (!orderbyAttr.empty())
		OrderAttr(SelectResult, orderbyAttr);
	
	std::streambuf* coutBuf = std::cout.rdbuf();
	std::ofstream of;
	if (!filename.empty())//把输出定向到文件
	{	
		of = std::ofstream(filename);
		std::streambuf* fileBuf = of.rdbuf();
		std::cout.rdbuf(fileBuf);
	}
	
	if (filename.empty())
		for (auto i = attrName.begin(); i < attrName.end(); i++) 
		{
			std::cout << (*i) << (i == attrName.end() - 1 ? "\n" : "\t");
		}

	if ((int)attrName.size() == 1 && countpos == 0)
	{
		int num = 0;
		for (auto x : SelectResult)
			if (countResult[x] == 1)
				num++;
		std::cout << num << "\n";
	}
	else
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

	

	if (!filename.empty())
	{
		of.flush();
		of.close();
		std::cout.rdbuf(coutBuf);
	}
}
