#pragma once

#include <stack>
#include <string>
#include <regex>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <cmath>

class ALU
{
public:
	ALU(std::string str) :expression(str) {}
	~ALU() = default;

	std::string process();

	friend std::vector<std::string> split(const std::string& str, const std::string& sep);
	friend bool IsDouble(const std::string& result);

private:
	std::string expression;
	static std::map<std::string, int> priority;
	static std::set<std::string> function;

	std::vector<std::string> Transfer(const std::vector<std::string>&);
	std::string Calculate(const std::vector<std::string>&);

	void ALUformat();
};

bool IsDouble(const std::string& result);
std::vector<std::string> split(const std::string& str, const std::string& sep);
