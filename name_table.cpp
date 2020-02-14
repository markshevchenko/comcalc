#include <string>

class name_table
{
public:
	void start_scope(std::string name, std::string prefix);

	void end_scope();

	void register_variable(std::string name);

	int get_variable_index(std::string name);

	int get_variable_internal_name(std::string name);
};