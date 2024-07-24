#pragma once
#include <string>
class Action
{
	std::string m_name;
	std::string m_type;
public:
	Action() {};
	Action(const std::string& dataName, const std::string& dataType)
		:m_name(dataName), m_type(dataType) {}
	const std::string& name() const;
	const std::string& type() const;
};

