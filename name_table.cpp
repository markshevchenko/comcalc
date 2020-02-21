#include <map>
#include <stdexcept>
#include <string>

#include "expression.h"

class scope
{
public:
	scope(char prefix) {
		_prefix = prefix;
		_index = 0;
	}

	int index() const {
		return _index;
	}

	void next_index() {
		_index++;
	}

	std::string fullname(std::string name) const {
		return _prefix + name;
	}

	bool has_variable(std::string name) const {
		return _variables.find(name) != _variables.end();
	}

private:
	struct variable
	{
		expression_type type;
		int index;
	};

	char _prefix;
	int _index;
	std::map<std::string, variable> _variables;
};

class name_table
{
public:
	void start_scope(std::string name, std::string prefix);

	void end_scope();

	void register_variable(std::string name);

	int get_variable_index(std::string name);

	int get_variable_internal_name(std::string name);
};