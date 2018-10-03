#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include "pregenerator.h"

#include <map>

static std::string integer_first_letters = "ijklmnIJKLMN";

bool is_double_variable(std::string variable_name) {
	auto first_variable_letter = variable_name[0];

	return integer_first_letters.find(first_variable_letter) == std::string::npos;
}

std::set<std::string> operator -(const std::set<std::string> &a, const std::set<std::string> &b) {
	std::set<std::string> result;

	for (auto i = a.cbegin(); i != a.cend(); i++) {
		if (b.find(*i) == b.end())
			result.insert(*i);
	}

	return result;
}

std::set<std::string> operator +(const std::set<std::string> &a, const std::set<std::string> &b) {
	std::set<std::string> result = a;

	for (auto i = b.cbegin(); i != b.cend(); i++) {
		if (a.find(*i) == a.end())
			result.insert(*i);
	}

	return result;
}

size_t get_input_format_length(const std::string& variable_name) {
	return variable_name.length() + 3;
}

std::string get_input_format(const std::string& variable_name) {
	return "c\"" + variable_name + ": \\00\"";
}

size_t get_output_format_length(const std::string& variable_name) {
	return variable_name.length() + 8;
}

std::string get_output_format(const std::string& variable_name) {
	if (is_double_variable(variable_name))
		return "c\"" + variable_name + " = %lf\\0A\\00\"";

	return "c\"" + variable_name + " = %ld\\0A\\00\"";
}

class generator : public iterating_visitor
{
private:
	std::set<std::string> _input_only_variables;
	std::set<std::string> _output_only_variables;
	std::set<std::string> _all_variables;
	std::set<std::string> _standard_functions;
	std::ostream& _out;
	std::map<std::string, int> _named_variables;
	int _last_variable_index = 0;

	void print_declarations() {
		for (auto i = _all_variables.cbegin(); i != _all_variables.cend(); i++) {
			auto variable_name = *i;

			if (is_double_variable(variable_name))
				_out << "@" << variable_name << " = common global double 0.0e+0, align 8" << std::endl;
			else
				_out << "@" << variable_name << " = common global i64 0, align 8" << std::endl;
		}
	}

	void print_input_formats() {
		for (auto i = _input_only_variables.cbegin(); i != _input_only_variables.cend(); i++) {
			auto variable_name = *i;

			_out << "@" << variable_name << ".format = private constant [" << get_input_format_length(variable_name)
				<< " x i8] " << get_input_format(variable_name) << std::endl;
		}

		_out << "@i8_input = private constant [4 x i8] c\"%ld\\00\"" << std::endl;
		_out << "@double_input = private constant [4 x i8] c\"%lf\\00\"" << std::endl;
	}

	void print_output_formats() {
		for (auto i = _output_only_variables.cbegin(); i != _output_only_variables.cend(); i++) {
			auto variable_name = *i;

			_out << "@" << *i << ".format = private constant [" << get_output_format_length(variable_name)
				<< " x i8] " << get_output_format(variable_name) << std::endl;
		}
	}

	void print_main_header() {
		_out << "define i32 @main() {" << std::endl;
		_out << "entry:" << std::endl;
	}

	void print_inputs() {
		for (auto i = _input_only_variables.cbegin(); i != _input_only_variables.cend(); i++) {
			auto variable_name = *i;

			_out << "  %" << _last_variable_index++ << " = call i32 (i8*, ...) @printf(i8* getelementptr (["
				<< variable_name.length() + 3 << " x i8], [" << variable_name.length() + 3 << " x i8]* @"
				<< variable_name << ".format, i32 0, i32 0))" << std::endl;

			if (is_double_variable(variable_name)) {
				_out << "  %" << _last_variable_index++
					<< " = call i32 (i8*, ...) @scanf(i8* getelementptr ([4 x i8], [4 x i8]* @double_input, i32 0, i32 0), double* nonnull @"
					<< variable_name << ")" << std::endl;
			}
			else {
				_out << "  %" << _last_variable_index++
					<< " = call i32 (i8*, ...) @scanf(i8* getelementptr ([4 x i8], [4 x i8]* @i64_input, i32 0, i32 0), i64* nonnull @"
					<< variable_name << ")" << std::endl;
			}
		}
	}

	int get_next_variable_index() {
		return _last_variable_index++;
	}

	int get_named_variable_index(const std::string& variable_name) {
		if (_named_variables.find(variable_name) == _named_variables.end()) {
			_named_variables[variable_name] = get_next_variable_index();

			if (is_double_variable(variable_name))
				_out << "  %" << _named_variables[variable_name] << " = load double, double* @" << variable_name << ", align 8" << std::endl;
			else
				_out << "  %" << _named_variables[variable_name] << " = load i64, i64* @" << variable_name << ", align 8" << std::endl;
		}

		return _named_variables[variable_name];
	}

	void print_outputs() {
		for (auto i = _output_only_variables.cbegin(); i != _output_only_variables.cend(); i++) {
			auto variable_name = *i;
			int variable_index = get_named_variable_index(variable_name);

			if (is_double_variable(variable_name)) {
				_out << "  %" << get_next_variable_index() << " = call i32 (i8*, ...) @printf(i8* getelementptr (["
					<< variable_name.length() + 8 << " x i8], [" << variable_name.length() + 8 << " x i8]* @"
					<< variable_name << ".format, i32 0, i32 0), double %" << variable_index << ")" << std::endl;
			}
			else {
				_out << "  %" << get_next_variable_index() << " = call i32 (i8*, ...) @printf(i8* getelementptr (["
					<< variable_name.length() + 8 << " x i8], [" << variable_name.length() + 8 << " x i8]* @"
					<< variable_name << ".format, i32 0, i32 0), i64 %" << variable_index << ")" << std::endl;
			}
		}

	}

	void print_main_footer() {
		_out << "  ret i32 0" << std::endl;
		_out << "}" << std::endl;
		_out << "declare i32 @printf(i8*, ...)" << std::endl;
		_out << "declare i32 @scanf(i8*, ...)" << std::endl;
	}

	void print_external_functions() {
		for (auto i = _standard_functions.cbegin(); i != _standard_functions.cend(); i++) {
			auto function_name = *i;

			_out << "declare double @" << function_name << "(double)" << std::endl;
		}
	}

public:
	generator(const pregenerator& pregenerator, std::ostream& out) : _out(out) {
		_input_only_variables = pregenerator.input_variables() - pregenerator.output_variables();
		_output_only_variables = pregenerator.output_variables() - pregenerator.input_variables();
		_all_variables = pregenerator.input_variables() + pregenerator.output_variables();
		_standard_functions = pregenerator.standard_functions();
		_last_variable_index = 0;

		print_declarations();
		print_input_formats();
		print_output_formats();
		print_main_header();
		print_inputs();

		print_outputs();
		print_main_footer();
		print_external_functions();
	}
};

#endif
