#include <map>
#include <stack>

#include "generator.h"
#include "expression.h"

class step1 : public visitor
{
private:
	static std::set<std::string> predefined_functions;

	std::set<std::string> _parameters;
	std::set<std::string> _input_variables;
	std::set<std::string> _output_variables;
	std::set<std::string> _standard_functions;
	std::vector<const ast_function*> _functions;
	std::vector<const ast_assignment*> _assignments;

public:
	const std::set<std::string> &input_variables() const { return _input_variables; }

	const std::set<std::string> &output_variables() const { return _output_variables; }

	const std::set<std::string> &standard_functions() const { return _standard_functions; }

	const std::vector<const ast_function*> &functions() const { return _functions; }

	const std::vector<const ast_assignment*> &assignments() const { return _assignments; }

	virtual void visit_function(const ast_function* function) {
		_parameters.clear();
		auto first_parameter = function->parameters().cbegin();
		auto last_parameter = function->parameters().cend();
		_parameters.insert(first_parameter, last_parameter);

		visitor::visit_function(function);

		_functions.push_back(function);
	}

	virtual void visit_assignment(const ast_assignment* assignment) {
		_parameters.clear();
		auto declared_identifier = assignment->name();

		if (_output_variables.find(declared_identifier) != _output_variables.end())
			throw new std::runtime_error("Variable `" + declared_identifier + "` already declared.");

		_output_variables.insert(declared_identifier);

		visitor::visit_assignment(assignment);

		_assignments.push_back(assignment);
	}

	virtual void visit_variable(const ast_variable* variable) {
		_input_variables.insert(variable->name());

		visitor::visit_variable(variable);
	}

	virtual void visit_call(const ast_call* call) {
		if (predefined_functions.find(call->name()) != predefined_functions.end()
			&& _standard_functions.find(call->name()) == _standard_functions.end()) {
			_standard_functions.insert(call->name());
		}

		visitor::visit_call(call);
	}
};

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
	if (get_variable_type(variable_name) == DOUBLE)
		return "c\"" + variable_name + " = %lf\\0A\\00\"";

	return "c\"" + variable_name + " = %ld\\0A\\00\"";
}

class generator : public visitor
{
private:
	std::set<std::string> _input_only_variables;
	std::set<std::string> _output_only_variables;
	std::set<std::string> _all_variables;
	std::set<std::string> _standard_functions;
	std::vector<const ast_function*> _functions;
	std::vector<const ast_assignment*> _assignments;
	std::ostream& _out;
	std::map<std::string, int> _named_variables;
	std::stack<expression_node> _expressions;
	int _last_variable_index = 0;

	void print_declarations() {
		for (auto i = _all_variables.cbegin(); i != _all_variables.cend(); i++) {
			auto variable_name = *i;

			if (get_variable_type(variable_name) == DOUBLE)
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

			if (get_variable_type(variable_name) == DOUBLE) {
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

			if (get_variable_type(variable_name) == DOUBLE)
				_out << "  %" << _named_variables[variable_name] << " = load double, double* @" << variable_name << ", align 8" << std::endl;
			else
				_out << "  %" << _named_variables[variable_name] << " = load i64, i64* @" << variable_name << ", align 8" << std::endl;
		}

		return _named_variables[variable_name];
	}

	void set_named_variable_index(const std::string& variable_name, expression_node node) {
		_named_variables[variable_name] = node.index;

		if (node._type == DOUBLE)
			_out << "  store double %" << node.index << ", double* @" << variable_name << ", align 8" << std::endl;
		else
			_out << "  store i64 %" << node.index << ", i64* @" << variable_name << ", align 8" << std::endl;
	}

	void print_assignments() {
		for (auto i = _assignments.cbegin(); i != _assignments.cend(); i++) {
			auto assignment = *i;

			assignment->accept(*this);
		}
	}

	void print_outputs() {
		for (auto i = _output_only_variables.cbegin(); i != _output_only_variables.cend(); i++) {
			auto variable_name = *i;
			int variable_index = get_named_variable_index(variable_name);

			if (get_variable_type(variable_name) == DOUBLE) {
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
		_out << "declare double @pow(double, double)" << std::endl;
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
		_functions = pregenerator.functions();
		_assignments = pregenerator.assignments();
		_last_variable_index = 0;

		print_declarations();
		print_input_formats();
		print_output_formats();
		print_main_header();
		print_inputs();

		print_assignments();

		print_outputs();
		print_main_footer();
		print_external_functions();
	}

	virtual void visit_assignment(const ast_assignment* assignment) {
		auto declared_identifier = assignment->name();
		visitor::visit_assignment(assignment);

		auto expression = _expressions.top();
		_expressions.pop();

		if (get_variable_type(declared_identifier) != expression._type)
			throw new std::runtime_error("Incompatible type of variable `" + declared_identifier + "`.");

		set_named_variable_index(declared_identifier, expression);
	}

	virtual void visit_integer(const ast_integer* integer) {
		int index = get_next_variable_index();

		_out << "  %" << index << " = add i64 0, " << integer->value() << std::endl;

		_expressions.push(expression_node(index, I64));
	}

	virtual void visit_double(const ast_double* _double) {
		int index = get_next_variable_index();

		_out << "  %" << index << " = fadd double 0.0, " << _double->value() << std::endl;

		_expressions.push(expression_node(index, DOUBLE));
	}

	virtual void visit_variable(const ast_variable* variable) {
		auto variable_name = variable->name();
		int index = get_named_variable_index(variable_name);
		expression_type type = get_variable_type(variable_name);

		_expressions.push(expression_node(index, type));
	}

	virtual void visit_call(const ast_call* call) {
		visitor::visit_call(call);

		if (call->parameters().size() > 1)
			throw new std::runtime_error("Functions with more than 1 parameter are not supported.");

		auto parameter = _expressions.top();
		_expressions.pop();

		int index = get_next_variable_index();
		_out << "  %" << index << " = call double @" << call->name() << "(" << get_node_as_text(parameter) << ")" << std::endl;

		_expressions.push(expression_node(index, DOUBLE));
	}

	expression_node cast_to_double(expression_node node) {
		int index = get_next_variable_index();

		_out << "  %" << index << " = sitofp i64 %" << node.index << " to double" << std::endl;

		return expression_node(index, DOUBLE);
	}

	virtual void visit_unary_operator(const ast_unary_operator* unary_operator) {
		visitor::visit_unary_operator(unary_operator);

		if (unary_operator->operation() == "+")
			return;

		expression_node operand = _expressions.top();
		_expressions.pop();

		int index = get_next_variable_index();
		if (operand._type == DOUBLE)
			_out << "  %" << index << +" = fsub double 0.0, %" << operand.index << std::endl;
		else
			_out << "  %" << index << +" = sub i64 0, %" << operand.index << std::endl;

		_expressions.push(expression_node(index, operand._type));
	}

	virtual void visit_binary_operator(const ast_binary_operator* binary_operator) {
		visitor::visit_binary_operator(binary_operator);

		expression_node right = _expressions.top();
		_expressions.pop();

		expression_node left = _expressions.top();
		_expressions.pop();

		if (left._type == DOUBLE && right._type == I64)
			right = cast_to_double(right);
		else if (left._type == I64 && right._type == DOUBLE)
			left = cast_to_double(left);
		else if (left._type == I64 && right._type == I64 && binary_operator->operation() == "^") {
			left = cast_to_double(left);
			right = cast_to_double(right);
		}

		int index = get_next_variable_index();
		expression_type type = left._type;

		if (type == DOUBLE) {
			if (binary_operator->operation() == "+")
				_out << "  %" << index << " = fadd double %" << left.index << ", %" << right.index << std::endl;
			else if (binary_operator->operation() == "-")
				_out << "  %" << index << " = fsub double %" << left.index << ", %" << right.index << std::endl;
			else if (binary_operator->operation() == "*")
				_out << "  %" << index << " = fmul double %" << left.index << ", %" << right.index << std::endl;
			else if (binary_operator->operation() == "/")
				_out << "  %" << index << " = fdiv double %" << left.index << ", %" << right.index << std::endl;
			else if (binary_operator->operation() == "%")
				_out << "  %" << index << " = frem double %" << left.index << ", %" << right.index << std::endl;
			else if (binary_operator->operation() == "^")
				_out << "  %" << index << " = call double @pow(double %" << left.index << ", double %" << right.index << ")" << std::endl;
		}
		else {
			if (binary_operator->operation() == "+")
				_out << "  %" << index << " = add i64 %" << left.index << ", %" << right.index << std::endl;
			else if (binary_operator->operation() == "-")
				_out << "  %" << index << " = sub i64 %" << left.index << ", %" << right.index << std::endl;
			else if (binary_operator->operation() == "*")
				_out << "  %" << index << " = mul i64 %" << left.index << ", %" << right.index << std::endl;
			else if (binary_operator->operation() == "/")
				_out << "  %" << index << " = sdiv i64 %" << left.index << ", %" << right.index << std::endl;
			else if (binary_operator->operation() == "%")
				_out << "  %" << index << " = srem i64 %" << left.index << ", %" << right.index << std::endl;
		}

		_expressions.push(expression_node(index, type));
	}
};

std::set<std::string> pregenerator::predefined_functions =
{
	"acos",
	"asin",
	"atan",
	"cos",
	"exp",
	"fabs",
	"log",
	"log10",
	"sin",
	"sqrt",
	"tan",
};

void generate(const ast_program* program, std::ostream& out) {
	pregenerator pregenerator;

	program->accept(pregenerator);

	generator generator(pregenerator, out);
}
