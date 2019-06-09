#include "Command.h"
using std::vector;
using std::string;
using std::regex;

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

void trim(std::string& s)
{
	if (!s.empty())
	{
		s.erase(0, s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
	}
}

inline string getFirstSubstr(string& str, const string& sep) {
	string::size_type pos = str.find(sep);
	if (pos != string::npos) {
		string tmp = str.substr(0, pos);
		str.erase(0, pos + 1);
		return tmp;
	}
	else {//将输出字符串清空，返回原字符串
		string tmp = str;
		str.clear();
		return tmp;
	}
}

inline string toUpper(const string& str) {
	string tmp(str);
	for (int i = 0; i < (int)tmp.size(); ++i)
		tmp[i] = toupper(tmp[i]);
	return tmp;
}

void Command::operate() {
	FormatSQL();

	string order = toUpper(getFirstSubstr(buffer, " "));

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
	else if (order == "LOAD")
		Load();
	else if (order == "INTERNET")
		LinkAsServer();
	else if (order == "LINK")
		LinkAsClient();
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
	/*将,()左右的空格去掉*/
	reg = " ?(\\)|\\(|,) ?";
	buffer = regex_replace(buffer, reg, "$1");
}

void Command::Create()
{
	string order = toUpper(getFirstSubstr(buffer, " "));

	if (order == "DATABASE") {
		string dbname = getFirstSubstr(buffer, " ");
		if (!dbname.empty()) DB.CreateDatabase(dbname);
	}
	else if (order == "TABLE") {
		string tableName = getFirstSubstr(buffer, "(");

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

			string PRIMARY = toUpper(ColumnAttr[0]), KEY = toUpper(ColumnAttr[1]);

			if (PRIMARY == "PRIMARY" && KEY == "KEY" && ColumnAttr.size() == 3) {
				PrimaryKey = ColumnAttr[2];
				continue;
			}
			Attribute tmp;
			tmp.name = ColumnAttr[0];
			string type = toUpper(ColumnAttr[1]);
		
			if (type == "INT")	tmp.type = "int";
			else if (type == "CHAR") tmp.type = "char";
			else if (type == "DOUBLE") tmp.type = "double";
			else return;

			tmp.Null = !(((ColumnAttr.size() == 4) && 
				(toUpper(ColumnAttr[2]) + " " + toUpper(ColumnAttr[3]) == "NOT NULL")));
			
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
	string dbname = getFirstSubstr(buffer, " ");
	DB.UseDatabase(dbname);
}

void Command::Drop() {
	vector<string> orders = split(buffer, " ");
	if (orders.size() == 2) {
		orders[0] = toUpper(orders[0]);
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
		orders[0] = toUpper(orders[0]);
		if (orders[0] == "DATABASES")
			DB.ShowDatabases();
		else if (orders[0] == "TABLES") {
			DB.ShowTables();
		}
	}
	else if (orders.size() == 3) {
		orders[0] = toUpper(orders[0]);
		orders[1] = toUpper(orders[1]);
		if (orders[0] + " " + orders[1] == "COLUMNS FROM") {
			DB.ShowColumns(orders[2]);
		}
	}
	else {
		std::cout << "ERROR!\n";
	}
}

void Command::Update() {
	string table_name = getFirstSubstr(buffer, " ");

	string SET = toUpper(getFirstSubstr(buffer, " "));
	if (SET != "SET") return;  //输入异常

	vector<std::pair<string, string> > UpdateAttr;

	vector<string> UpdateTmp = split(getFirstSubstr(buffer, " "), ",");
	for (int i = 0; i < (int)UpdateTmp.size(); i++)
	{
		vector<string> Attr_tmp = split(UpdateTmp[i], "=");
		if (Attr_tmp.size() != 2) return;

		std::pair<string, string> tmp(Attr_tmp[0], Attr_tmp[1]);
		UpdateAttr.push_back(tmp);
	}

	string WHERE = toUpper(getFirstSubstr(buffer, " "));
	if (WHERE != "WHERE") return;

	std::set<Data> key_of_rows = where_clause(table_name, buffer);
	int k = UpdateAttr.size();
	for (auto i = key_of_rows.begin(); i != key_of_rows.end(); i++)
		for (int j = 0; j < k; j++)
			DB.Set(table_name, UpdateAttr[j].first, UpdateAttr[j].second, (*i).value);
}

void Command::Insert() {
	string INTO = toUpper(getFirstSubstr(buffer, " "));
	if (INTO != "INTO") return;  //输入异常

	string table_name = getFirstSubstr(buffer, "(");

	vector<string> attr_list = split(getFirstSubstr(buffer, ")"), ",");

	string VALUES = toUpper(getFirstSubstr(buffer, "("));
	if (VALUES != "VALUES") return; //输入异常

	vector<string> value_list = split(getFirstSubstr(buffer, ")"), ",");

	DB.InsertInto(table_name, attr_list, value_list);
}

void Command::Delete() {
	string FROM = toUpper(getFirstSubstr(buffer, " "));
	if (FROM != "FROM") return;  //输入异常

	string table_name = getFirstSubstr(buffer, " ");

	string WHERE = toUpper(getFirstSubstr(buffer, " "));
	if (WHERE != "WHERE") return;

	std::set<Data> key_of_rows = where_clause(table_name, buffer);
	for (auto i = key_of_rows.begin(); i != key_of_rows.end(); i++) {
		DB.DeleteRow(table_name, (*i).value);
	}
}

void Command::Select() {
	string CountAttr = "", FileName = "", orderby = "", orderbyCount = "", whereclause = "";
	vector<string> Columns, groupbyAttr;
	int countpos = -1;

	string tmp = toUpper(buffer), str;
	string::size_type pos = tmp.find("FROM");//将select语句按照from关键词分开
	if (pos != string::npos && tmp[pos - 1] == ' ')
		str = buffer.substr(0, pos - 1);
	else str = buffer.substr(0, pos);
	buffer.erase(0, pos);

	if (buffer.empty()) {//没有form，必定是算术表达式
		vector<string> expressions = split(str, ",");
		vector<vector<string> > results;
		for (auto i : expressions) {
			ALU expression(i);
			results.push_back(expression.process());
			std::cout << i << "\t";
		}
		std::cout << std::endl;

		for (auto j : results) {
			std::cout << j[0] << "\t";
		}
		std::cout << std::endl;
		return;
	}

	string FROM = toUpper(getFirstSubstr(buffer, " "));
	if (FROM != "FROM") return;  //输入异常
	string table_name = getFirstSubstr(buffer, " ");

	if (!DB.existTable(table_name)) return;
	Table& table = DB.getDatabase().getTable(table_name);

	regex reg(" ?INTO OUTFILE ?", regex::icase);//不区分大小写

	if (regex_search(str, reg)) {
		str = regex_replace(str, reg, " ");
		pos = str.find_last_of(" ");
		FileName = str.substr(pos + 1, str.size() - pos - 1);
		str.erase(pos, str.size() - pos);
	}
	string tmpColumns = str;

	vector<string> AttrNewName;
	if (tmpColumns == "*") {
		for (int i = 0; i < (int)table.attr_list.size(); ++i) {
			Columns.push_back(table.attr_list[i].name);
		}
	}

	else {
		Columns = split(tmpColumns, ",");
		AttrNewName = Columns;
		for (int i = 0; i < (int)Columns.size(); ++i) {
			//判断count(expression)
			string tmp = Columns[i];
			string COUNT = getFirstSubstr(tmp, "(");
			if (toUpper(COUNT) == "COUNT") {
				countpos = i;
				CountAttr = getFirstSubstr(tmp, ")");
			}
			//判断有没有as

			regex as(" ?(as) ? ", regex::icase);
			if (regex_search(Columns[i], as)) {
				Columns[i] = regex_replace(Columns[i], as, " $1 ");
				as = " +";
				Columns[i] = regex_replace(Columns[i], as, " ");

				string tmp = toUpper(Columns[i]);
				auto pos = tmp.find(" AS");
				AttrNewName[i] = Columns[i].substr(pos + 4, Columns[i].size() - (pos + 4));
				Columns[i] = Columns[i].substr(0, pos);
			}
		}
	}
	//提取where子句
	tmp = toUpper(buffer);
	pos = tmp.find("GROUP");
	if (pos == string::npos)
		pos = tmp.find("ORDER");
	whereclause = buffer.substr(0, pos); trim(whereclause);
	buffer.erase(0, pos);
	if (!whereclause.empty()) {
		string WHERE = toUpper(getFirstSubstr(whereclause, " "));
		if (WHERE != "WHERE") return;
	}

	//提取group子句
	tmp = toUpper(buffer);
	pos = tmp.find("ORDER");
	string tmp_group = buffer.substr(0, pos); trim(tmp_group);
	buffer.erase(0, pos);
	if (!tmp_group.empty()) {
		string GROUP = toUpper(getFirstSubstr(tmp_group, " "));
		if (GROUP != "GROUP") return;
		string BY = toUpper(getFirstSubstr(tmp_group, " "));
		if (BY != "BY") return;
	}
	groupbyAttr = split(tmp_group, ",");

	//提取order子句
	if (!buffer.empty()) {
		string ORDER = toUpper(getFirstSubstr(buffer, " "));
		if (ORDER != "ORDER") return;
		string BY = toUpper(getFirstSubstr(buffer, " "));
		if (BY != "BY") return;
	}
	orderby = buffer; trim(orderby);
	tmp = orderby;
	string COUNT = getFirstSubstr(tmp, "(");
	if (toUpper(COUNT) == "COUNT")
		orderbyCount = getFirstSubstr(tmp, ")");


	table.SelectData(Columns, AttrNewName, countpos, CountAttr, groupbyAttr, orderby, orderbyCount, whereclause, FileName);
}


void Command::Load()
{
	vector<string> orders = split(buffer, " ");
	string fileName;
	bool error = true;
	if (orders.size() == 6 &&
		toUpper(orders[0]) == "DATA" &&
		toUpper(orders[1]) == "INFILE" &&
		toUpper(orders[3]) == "INTO" &&
		toUpper(orders[4]) == "TABLE") {
		fileName = orders[2];
		error = false;
	}
	else if (orders.size() == 7 &&
		toUpper(orders[0]) == "DATA" &&
		toUpper(orders[1]) == "LOCAL" &&
		toUpper(orders[2]) == "INFILE" &&
		toUpper(orders[4]) == "INTO" &&
		toUpper(orders[5]) == "TABLE") {
		fileName = orders[3];
		error = false;
	}
	if (error) return; //报错模块

	string tableAttr = orders[orders.size() - 1],
		tableName = getFirstSubstr(tableAttr, "(");

	if (!DB.existTable(tableName))	return;

	Table& table = DB.getDatabase().getTable(tableName);

	vector<string> Columns;
	if (!tableAttr.empty()) {
		Columns = split(getFirstSubstr(tableAttr, ")"), ",");
	}
	else {
		for (auto i : table.attr_list)
			Columns.push_back(i.name);
	}
	table.LoadFile(fileName, Columns);
}

std::set<Data> where_clause(std::string table_name, std::string clause) {
	//注意：这里默认每个条件句之内没有空格
	regex reg(" ?(>|<|=) ?");
	clause = regex_replace(clause, reg, "$1");

	std::stringstream ss(clause);
	std::vector<Clause> conditions;
	std::vector<std::string> operators;
	std::vector<std::set<Data>> keys;
	while (true) {
		Clause c;
		std::string str;
		ss >> str;
		int i = 0;
		for (; i < (int)str.length(); i++) {
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