#include "Command.h"

using std::vector;
using std::string;
using std::regex;
void Link();
vector<string> split(const string& str, const string& sep)
{
	vector<string> tmp;
	string::size_type pos[2] = { 0, str.find(sep) };
	while (string::npos != pos[1])
	{
		tmp.push_back(str.substr(pos[0], pos[1] - pos[0]));
		pos[0] = pos[1] + sep.size();
		pos[1] = str.find(sep, pos[0]);
	}
	if (pos[0] != str.length())
		tmp.push_back(str.substr(pos[0]));

	return tmp;
}

inline string getFirstSubstr(string& str, const char& sep) {
	string::size_type pos = str.find(sep);
	if (pos != string::npos) {
		string tmp = str.substr(0, pos);
		str.erase(0, pos + 1);
		return tmp;
	}
	else {
		string tmp = str;
		str.clear();
		return tmp;
	}
}

inline void toUpper(string& str) {
	for (int i = 0; i < str.size(); i++)
		str[i] = toupper(str[i]);
}

void Command::operate() {
	FormatSQL();

	string order = getFirstSubstr(buffer, ' ');
	toUpper(order);

	if (order == "CREATE")
		Create();
	else if (order == "DROP")
		Drop();
	else if (order == "DELETE")
		Delete();
	else if (order == "SELECT")
		Select();
	else if (order == "INSERT")
		Insert();
	else if (order == "UPDATE")
		Update();
	else if (order == "SHOW")
		Show();
	else if (order == "USE")
		Use();
	else if (order == "INTERNET")
		Link();
}

void Command::FormatSQL()
{
	/*换行,tab,",'回车用" "代替*/
	regex reg("[\r\n\t\"']");
	buffer = regex_replace(buffer, reg, " ");
	/*去除分号之后的内容*/
	reg = ";.*$";
	buffer = regex_replace(buffer, reg, "");
	//去除最前和最后的" "
	reg = "(^ +)|( +$)";
	buffer = regex_replace(buffer, reg, "");
	/*去除重复的空格*/
	reg = " +";
	buffer = regex_replace(buffer, reg, " ");
	/*将=,()左右的空格去掉*/
	reg = " ?(\\)|\\(|,|=) ?";
	buffer = regex_replace(buffer, reg, "$1");
}

void Command::Create()
{
	string order = getFirstSubstr(buffer, ' ');
	toUpper(order);

	if (order == "DATABASE") {
		string dbname = getFirstSubstr(buffer, ' ');
		if (!dbname.empty()) DB.CreateDatabase(dbname);
	}
	else if (order == "TABLE") {
		string tableName = getFirstSubstr(buffer, '(');

		//将( )转换成空格
		regex reg("(\\(|\\))");
		buffer = regex_replace(buffer, reg, " ");
		//去除首尾的" "
		reg = "(^ +)|( +$)";
		buffer = regex_replace(buffer, reg, "");
		//去除,周围的空格
		reg = " ?, ?";
		buffer = regex_replace(buffer, reg, ",");

		vector<string> TableAttr = split(buffer, ",");
		vector<Attribute> attr;
		string PrimaryKey;

		for (int i = 0; i < (int)TableAttr.size(); i++) {
			vector<string> ColumnAttr = split(TableAttr[i], " ");

			string PRIMARY = ColumnAttr[0], KEY = ColumnAttr[1];
			toUpper(PRIMARY);
			toUpper(KEY);
			if (PRIMARY == "PRIMARY" && KEY == "KEY" && ColumnAttr.size() == 3) {
				PrimaryKey = ColumnAttr[2];
				continue;
			}
			Attribute tmp;
			tmp.name = ColumnAttr[0];
			string& type = ColumnAttr[1];
			toUpper(type);
			if (type == "INT")	tmp.type = "int";
			else if (type == "CHAR") tmp.type = "char";
			else if (type == "DOUBLE") tmp.type = "double";
			else return;

			tmp.Null = ((ColumnAttr.size() == 4) && (ColumnAttr[2] + " " + ColumnAttr[3] == "NOT NULL"));
			
			attr.push_back(tmp);
		}

		for (int i = 0; i < (int)attr.size(); i++)
			if (PrimaryKey == attr[i].name) {
				attr[i].Key = true;
				break;
			}

		DB.CreateTable(tableName, attr, PrimaryKey);
	}
}

void Command::Use() {
	string dbname = getFirstSubstr(buffer, ' ');
	DB.UseDatabase(dbname);
}

void Command::Drop() {
	vector<string> orders = split(buffer, " ");
	if (orders.size() == 2) {
		toUpper(orders[0]);
		string name = orders[1];

		if (orders[0] == "DATABASE")
			DB.DropDatabase(name);
		else if (orders[0] == "TABLE") {
			DB.DropTable(name);
		}
	}
}

void Command::Show() {
	vector<string> orders = split(buffer, " ");
	if (orders.size() == 1) {
		toUpper(orders[0]);
		if (orders[0] == "DATABASES")
			DB.ShowDatabases();
		else if (orders[0] == "TABLES") {
			DB.ShowTables();
		}
	}
	else if (orders.size() == 3) {
		toUpper(orders[0]);
		toUpper(orders[1]);
		if (orders[0] + " " + orders[1] == "COLUMNS FROM") {
			DB.ShowColumns(orders[2]);
		}
	}
	else {
		std::cout << "ERROR!\n";
	}
}

void Command::Update() {
	string table_name = getFirstSubstr(buffer, ' ');

	string SET = getFirstSubstr(buffer, ' '); toUpper(SET);
	if (SET != "SET") return;  //输入异常

	vector<std::pair<string, string> > UpdateAttr;

	vector<string> UpdateTmp = split(getFirstSubstr(buffer, ' '), ",");
	for (int i = 0; i < UpdateTmp.size(); i++)
	{
		vector<string> Attr_tmp = split(UpdateTmp[i], "=");
		if (Attr_tmp.size() != 2) return;

		std::pair<string, string> tmp(Attr_tmp[0], Attr_tmp[1]);
		UpdateAttr.push_back(tmp);
	}

	string WHERE = getFirstSubstr(buffer, ' '); toUpper(WHERE);
	if (WHERE != "WHERE") return;

	std::set<Data> key_of_rows = where_clause(table_name, buffer);
	int k = UpdateAttr.size();
	for (auto i = key_of_rows.begin(); i != key_of_rows.end(); i++)
		for (int j = 0; j < k; j++)
			DB.Set(table_name, UpdateAttr[j].first, UpdateAttr[j].second, (*i).value);
}

void Command::Insert() {
	string INTO = getFirstSubstr(buffer, ' '); toUpper(INTO);
	if (INTO != "INTO") return;  //输入异常

	string table_name = getFirstSubstr(buffer, '(');

	vector<string> attr_list = split(getFirstSubstr(buffer, ')'), ",");

	string VALUES = getFirstSubstr(buffer, '('); toUpper(VALUES);

	if (VALUES != "VALUES") return; //输入异常

	vector<string> value_list = split(getFirstSubstr(buffer, ')'), ",");

	DB.InsertInto(table_name, attr_list, value_list);
}

void Command::Delete() {
	string FROM = getFirstSubstr(buffer, ' '); toUpper(FROM);
	if (FROM != "FROM") return;  //输入异常

	string table_name = getFirstSubstr(buffer, ' ');

	string WHERE = getFirstSubstr(buffer, ' '); toUpper(WHERE);
	if (WHERE != "WHERE") return;  //输入异常

	std::set<Data> key_of_rows = where_clause(table_name, buffer);
	for (auto i = key_of_rows.begin(); i != key_of_rows.end(); i++) {
		DB.DeleteRow(table_name, (*i).value);
	}
}

void Command::Select() {
	string attr_name = getFirstSubstr(buffer, ' ');

	string FROM = getFirstSubstr(buffer, ' '); toUpper(FROM);
	if (FROM != "FROM") return;  //输入异常

	string table_name = getFirstSubstr(buffer, ' ');
	std::set<Data> key_of_rows;

	if (attr_name == "*") {
		if (buffer.empty()) {
			key_of_rows = DB.GetAllKeys(table_name);
		}
		else {
			string WHERE = getFirstSubstr(buffer, ' '); toUpper(WHERE);
			if (WHERE != "WHERE") return;  //输入异常

			key_of_rows = where_clause(table_name, buffer);
		}

		if (!key_of_rows.empty()) {
			DB.OutputAttr(table_name);
			for (auto i = key_of_rows.begin(); i != key_of_rows.end(); i++)
				DB.OutputRow(table_name, (*i).value);
		}
	}

	else {
		vector<string> attr_names = split(attr_name, ",");
		for (auto i = attr_names.begin(); i < attr_names.end() - 1; i++)
			std::cout << (*i) << "\t";
		std::cout << *(attr_names.end() - 1) << "\n";

		if (buffer.empty()) {
			key_of_rows = DB.GetAllKeys(table_name);
		}
		else {
			string WHERE = getFirstSubstr(buffer, ' '); toUpper(WHERE);
			if (WHERE != "WHERE") return;  //输入异常

			key_of_rows = where_clause(table_name, buffer);
		}
		int k = attr_names.size();
		for (auto i = key_of_rows.begin(); i != key_of_rows.end(); i++) {
			for (int j = 0; j < k; j++) {
				string value = DB.GetValue(table_name, attr_names[j], (*i).value);
				string type = DB.GetType(table_name, attr_names[j]);
				OutputData(value, type);
				if (j != k - 1)
					std::cout << "\t";
			}
			std::cout << "\n";
		}
	}
}

std::set<Data> where_clause(std::string table_name, std::string clause) {
	//注意：这里默认每个条件句之内没有空格
	std::stringstream ss(clause);
	std::vector<Clause> conditions;
	std::vector<std::string> operators;
	std::vector<std::set<Data>> keys;
	while (true) {
		Clause c;
		std::string str;
		ss >> str;
		int i = 0;
		for (; i < str.length(); i++) {
			if (str[i] == '=' || str[i] == '<' || str[i] == '>') {
				c.cmp_op = str[i];
				break;
			}

		}
		c.left = str.substr(0, i);
		if (c.left[0] == '"' || c.left[0] == '\'') {
			c.left = c.left.substr(1, c.left.length() - 2);
		}
		std::string right = str.substr(i + 1);
		//c.right = str.substr(i + 1);//错误，右值字符串为"\"\a""
		if (right[0] == '"' || right[0] == '\'') {
			c.right = right.substr(1, right.length() - 2);
		}
		else
			c.right = str.substr(i + 1);
		conditions.push_back(c);
		keys.push_back(DB.KeyWhereCluase(table_name, c));
		std::string op;
		if (ss >> op) {
			operators.push_back(op);
		}
		else {
			break;
		}
	}
	auto it1 = keys.begin();
	auto it2 = operators.begin();
	while (it2 < operators.end()) {
		if ((*it2) == "AND" || (*it2) == "and") {
			std::set<Data> s;
			std::set_intersection((*it1).begin(), (*it1).end(), (*(it1 + 1)).begin(), (*(it1 + 1)).end(), std::inserter(s, s.begin()));
			*(it1 + 1) = s;
			it1 = keys.erase(it1);
			it2 = operators.erase(it2);
		}
		else if ((*it2) == "OR" || (*it2) == "or") {
			it1++;
			it2++;
		}
		else {
			std::cout << "ERROR!\n";
		}
	}
	it1 = keys.begin();
	while (keys.size() > 1) {
		std::set<Data> s;
		std::set_union((*it1).begin(), (*it1).end(), (*(it1 + 1)).begin(), (*(it1 + 1)).end(), std::inserter(s, s.begin()));
		(*it1) = s;
		keys.erase(it1 + 1);
	}
	return keys[0];
}