#pragma once

#include <stack>
#include <string>
#include <regex>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>

class ALU
{
public:
	ALU(std::string str) :expression(str) {}
	~ALU() = default;

	void process();

private:
	std::string expression;
	static std::map<std::string, int> priority;
	static std::set<std::string> function;

	std::vector<std::string> Transfer(const std::vector<std::string>&);
	std::string Calculate(const std::vector<std::string>&);
	void output(const std::string& result);
	void ALUformat(std::string& expression);
};

inline std::string toUpper(const std::string& str);
bool IsDouble(const std::string& result);
std::vector<std::string> split(const std::string& str, const std::string& sep);
