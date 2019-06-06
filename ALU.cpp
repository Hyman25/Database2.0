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
	{"(",-1},{")",-1},{"ABS(",-1},{"SIN(",-1},{"EXP(",-1},{"COS(",-1},{"PI(",-1},
	//逻辑运算符
	{"||",1},{"OR",1},{"XOR",1},{"&&",2},{"AND",2},{"NOT",3},{"!",3},
	//算术运算符
	{"+",4},{"-",4},{"*",5},{"/",5},{"DIV",5},{"%",5},{"MOD",5},{"sign+",6},{"sign-",6}
};

set<string> ALU::function = {
	"ABS(","SIN(","EXP(","COS(","PI(","("
};

string ALU::process()
{
	std::transform(expression.begin(), expression.end(), expression.begin(), ::toupper);

	ALUformat();
	vector<string> ex_split = split(expression, " ");
	ex_split = Transfer(ex_split);

	string result = Calculate(ex_split);
	return result;
}

vector<string> ALU::Transfer(const vector<string>& str)
{
	vector<string> result;
	stack<string> operators;
	if (str.empty()) return str;

	for (int i = 0; i < str.size(); ++i)
	{	//如果是数字
		if (priority.find(str[i]) == priority.end())
			result.push_back(str[i]);
		//如果是左括号标记
		else if (function.find(str[i]) != function.end())
			operators.push(str[i]);
		//如果是右括号
		else if (str[i] == ")") {
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
		else
		{
			if (str[i] == "+" || str[i] == "-")
				if (i == 0 || function.find(str[i - 1]) != function.end()) {
					operators.push("sign" + str[i]);
					continue;
				}

			int weight = priority[str[i]];
			if (operators.empty() || weight > priority[operators.top()])
				operators.push(str[i]);
			else {
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
				operators.push(str[i]);
			}
		}
	}
	while (!operators.empty())
	{
		string tmp = operators.top();
		result.push_back(tmp);
		operators.pop();
	}
	return result;
}

string ALU::Calculate(const vector<string>& expression)
{
	stack<double> nums;
	for (int i = 0; i < expression.size(); ++i)
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
		else if (expression[i] == "sign+") {
			nums.push(num1);
			continue;
		}
		else if (expression[i] == "sign-") {
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
	std::ostringstream stream;
	double result = nums.top();
	stream << result;
	string tmp = stream.str();
	return tmp;
}

void ALU::ALUformat()
{
	regex reg(" ?(\\+|-|\\*|/|%|(DIV)|(MOD)|\\(|\\)) ?");
	expression = regex_replace(expression, reg, " $1 ");
	reg = " ?((OR)|(XOR)|(AND)|(NOT)|(&&)|(\\|\\|)|!) ?";
	expression = regex_replace(expression, reg, " $1 ");
	reg = " ?((ABS)|(SIN)|(COS)|(EXP)|(PI)) ?";
	expression = regex_replace(expression, reg, " $1 ");
	reg = "(^ +)|( +$)";
	expression = regex_replace(expression, reg, "");
	reg = " +";
	expression = regex_replace(expression, reg, " ");
	reg = "ABS *\\(";
	expression = regex_replace(expression, reg, "ABS(");
	reg = "SIN *\\(";
	expression = regex_replace(expression, reg, "SIN(");
	reg = "COS *\\(";
	expression = regex_replace(expression, reg, "COS(");
	reg = "EXP *\\(";
	expression = regex_replace(expression, reg, "EXP(");
	reg = "PI *\\(";
	expression = regex_replace(expression, reg, "PI(");
}

bool IsDouble(const string& result) {
	if (result.empty()) return false;
	const char* tmp_char = result.c_str();
	for (int j = 0; j < result.size(); ++j)
		if (!isdigit(tmp_char[j]) && tmp_char[j] != '.') {
			return false;
		}
	return true;
}