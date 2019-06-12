#include "ALU.h"

#define M_PI       3.14159265358979323846   // pi

using std::vector;
using std::string;
using std::regex;
using std::map;
using std::set;
using std::stack;

map<string, int> ALU::priority = {
	//括号和数字函数
	{"(",-1},{")",-1},{"ABS(",-1},{"SIN(",-1},{"COS(",-1},{"EXP(",-1} ,{"PI(",-1},
	//逻辑运算符
	{"||",1},{"OR",1},{"XOR",1},{"&&",2},{"AND",2},{"NOT",3},{"!",3},
	//比较运算符
	{"=",6},{">=",6},{"<=",6},{">",6},{"<",6},{"<>",6},{"!=",6},
	//算术运算符
	{"+",10},{"-",10},{"*",11},{"/",11},{"DIV",11},{"%",11},{"MOD",11},{"SIGN+",13},{"SIGN-",13}
};

set<string> ALU::function = {
	"ABS(","SIN(","EXP(","COS(","PI(","("
};

regex ALU::operators("(\\+|-|\\*|\\/|%|(DIV)|"
"(MOD)|(OR)|(XOR)|(AND)|(NOT)|(&&)|(\\|\\|)|!|"
"(ABS)|(SIN)|(COS)|(EXP)|(PI)|"
">|<|=|(>=)|(<=)|(!=)|(<>))", regex::icase);

std::vector<std::string> ALU::process()
{
	vector<string> result;
	ALUformat(expression);
	vector<string> ex_split = split(expression, " ");
	ex_split = Transfer(ex_split);
	for (int i = 0; i < (int)ex_split.size(); ++i)
		ex_split[i] = toUpper(ex_split[i]);
	result.push_back(Calculate(ex_split));
	return result;
}

vector<string> ALU::process(Table * table,const set<Data>& keys)
{
	ALUformat(expression);
	vector<string> ex_split = split(expression, " ");
	ex_split = Transfer(ex_split);

	set<string> Columns;
	vector<int> isColumns_index;
	for (auto i : table->attr_list)
		Columns.insert(i.name);
	for (int i = 0; i < (int)ex_split.size(); i++) {
		if (Columns.find(ex_split[i]) != Columns.end())
		{
			isColumns_index.push_back(i);
		}
	}
	vector<string> tmp_ex_split(ex_split), result;

	for (auto i : keys) {
		Row& row = table->row_map[i];
		for (int j = 0; j < (int)isColumns_index.size(); j++) {
			string ColumnName = ex_split[isColumns_index[j]];
			string rowData = row.data[ColumnName];
			if (IsDouble(rowData))
				tmp_ex_split[isColumns_index[j]] = rowData;
			else tmp_ex_split[isColumns_index[j]] = "0";
		}
		for (int i = 0; i < (int)tmp_ex_split.size(); ++i)
			tmp_ex_split[i] = toUpper(tmp_ex_split[i]);
		result.push_back(Calculate(tmp_ex_split));
	}
	return result;
}

bool ALU::IsALU(std::string str)
{
	regex reg("(count\\(\\*\\)|^\\*)", regex::icase);
	if (std::regex_search(str, operators) && !std::regex_search(str, reg))
		return true;
	return false;
}

vector<string> ALU::Transfer(vector<string>& str)
{
	vector<string> result;
	stack<string> operators;
	if (str.empty()) return str;

	for (int i = 0; i < (int)str.size(); ++i)
	{
		
		string tmp = toUpper(str[i]);
		//如果是左括号或函数
		if (function.find(tmp) != function.end())
			operators.push(tmp);
		//如果是右括号
		else if (tmp == ")") {
			string op = operators.top();
			while (function.find(op) == function.end()) {
				result.push_back(op);
				operators.pop();
				op = operators.top();
			}
			if (op != "(")
				result.push_back(op);
			operators.pop();
		}
		//如果是算术或逻辑运算符
		else if (priority.find(tmp) != priority.end())
		{
			if (tmp == "+" || tmp == "-")
				if (i == 0 || function.find(str[i - 1]) != function.end()) {
					operators.push("SIGN" + tmp);
					continue;
				}

			int weight = priority[tmp];
			if (operators.empty() || weight > priority[operators.top()])
				operators.push(tmp);
			else
			{
				while (!operators.empty())
				{
					string op = operators.top();
					if (priority[op] >= weight)
					{
						result.push_back(op);
						operators.pop();
					}
					else break;
				}
				operators.push(tmp);
			}
		}
		//如果是数字或列名
		else result.push_back(str[i]);
	}
	while (!operators.empty())
	{
		string tmp = operators.top();
		result.push_back(tmp);
		operators.pop();
	}
	return result;
}

string ALU::Calculate(vector<string>& expression)
{
	stack<double> nums;
	for (int i = 0; i <(int)expression.size(); ++i)
	{
		if (IsDouble(expression[i])) {
			nums.push(stod(expression[i]));
			continue;
		}
		//NULL在所有情况下都返回NULL，不参与运算
		if (priority.find(expression[i]) == priority.end()) {
			return "NULL";
		}
		if (expression[i] == "PI(") {
			nums.push(M_PI);
			continue;
		}

		double num1 = nums.top(), num2 = 0;
		nums.pop();

		if (expression[i] == "NOT" || expression[i] == "!") {
			nums.push(!num1);
			continue;
		}
		else if (expression[i] == "SIGN+") {
			nums.push(num1);
			continue;
		}
		else if (expression[i] == "SIGN-") {
			nums.push(-num1);
			continue;
		}
		else if (expression[i] == "ABS(") {
			nums.push(abs(num1));
			continue;
		}
		else if (expression[i] == "SIN(") {
			nums.push(sin(num1));
			continue;
		}
		else if (expression[i] == "COS(") {
			nums.push(cos(num1));
			continue;
		}
		else if (expression[i] == "EXP(") {
			nums.push(exp(num1));
			continue;
		}

		if (!nums.empty()) {//防止程序崩溃
			num2 = nums.top();
			nums.pop();
		}

		if (expression[i] == "&&" || expression[i] == "AND")
			nums.push(num1 && num2);
		else if (expression[i] == "||" || expression[i] == "OR")
			nums.push(num1 || num2);
		else if (expression[i] == "XOR")
			nums.push((!num1 && num2) || (num1 && !num2));

		else if (expression[i] == "+")
			nums.push(num1 + num2);
		else if (expression[i] == "-")
			nums.push(num2 - num1);
		else if (expression[i] == "*")
			nums.push(num1 * num2);

		else if (expression[i] == ">")
			nums.push((num2 > num1));
		else if (expression[i] == "<")
			nums.push((num2 < num1));
		else if (expression[i] == "=")
			nums.push((num2 == num1));
		else if (expression[i] == ">=")
			nums.push((num2 >= num1));
		else if (expression[i] == "<=")
			nums.push((num2 <= num1));
		else if (expression[i] == "<>" || expression[i] == "!=")
			nums.push((num2 != num1));

		else {
			if (!num1) return "NULL";//除以0报错,退出

			if (expression[i] == "/")
				nums.push(num2 / num1);
			else if (expression[i] == "DIV")//整除
				nums.push((int)(num2 / num1));
			else if (expression[i] == "%" || expression[i] == "MOD")//取余
				nums.push((int)num2 % (int)num1);
		}
	}
	
	double result = nums.top();
	string tmp = DoubleToString(result);
	return tmp;
}

void ALU::ALUformat(string& str)
{
	str = regex_replace(str, operators, " $1 ");
	regex reg("(\\(|\\))");
	str = regex_replace(str, reg, " $1 ");
	reg="(^ +)|( +$)";
	str = regex_replace(str, reg, "");
	reg = " +";
	str = regex_replace(str, reg, " ");
	reg = "< *=";
	str = regex_replace(str, reg, "<=");
	reg = "> *=";
	str = regex_replace(str, reg, ">=");
	reg = "! *=";
	str = regex_replace(str, reg, "!=");
	reg = "< *>";
	str = regex_replace(str, reg, "<>");

	regex reg5("ABS *\\(", regex::icase);
	str = regex_replace(str, reg5, "ABS(");
	regex reg6("SIN *\\(", regex::icase);
	str = regex_replace(str, reg6, "SIN(");
	regex reg7("COS *\\(", regex::icase);
	str = regex_replace(str, reg7, "COS(");
	regex reg8("EXP *\\(", regex::icase);
	str = regex_replace(str, reg8, "EXP(");
	regex reg9("PI *\\(", regex::icase);
	str = regex_replace(str, reg9, "PI(");
}

bool IsDouble(const string& result) {
	if (result.empty()) return false;
	const char* tmp_char = result.c_str();
	for (int j = 0; j < (int)result.size(); ++j)
		if (!isdigit(tmp_char[j]) && tmp_char[j] != '.') {
			return false;
		}
	return true;
}

std::string DoubleToString(const double value)
{
	string res = std::to_string(value);
	auto pos = res.find('.');
	if (pos == std::string::npos)
		return res;

	string tmp = res.substr(pos + 1, res.size() - pos - 1);
	bool mark = false;
	for (auto i : tmp) {
		if (i != '0') {
			mark = true;
			break;
		}
	}
	if (!mark) {
		res = res.substr(0, pos);
		if (res == "-0") res = "0";
	}
	
	return res;
}