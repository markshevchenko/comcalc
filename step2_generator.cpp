#include "step2_generator.h"

std::map<std::string, expression_type> set_except(const std::map<std::string, expression_type>& from, const std::map<std::string, expression_type>& what) {
	auto result = from;

	result.erase(what.cbegin(), what.cend());

	return result;
}

std::map<std::string, expression_type> set_union(const std::map<std::string, expression_type>& what, const std::map<std::string, expression_type>& with) {
	auto result = what;

	result.insert(with.cbegin(), with.cend());

	return result;
}

step2_generator::step2_generator(const table_registry& table_registry, std::ostream& out) : _out(out) {
	auto input_variables = table_registry.input_variables();
	auto output_variables = table_registry.output_variables();
	_input_only_static_variables = set_except(input_variables, output_variables);
	_output_only_static_variables = set_except(output_variables, input_variables);
	_all_static_variables = set_union(input_variables, output_variables);
	_standard_functions = table_registry.standard_functions();
	_functions = table_registry.functions();
	_assignments = table_registry.assignments();
}

void step2_generator::print_code() {
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

void step2_generator::print_declarations() {
	for (auto i = _all_static_variables.cbegin(); i != _all_static_variables.cend(); i++) {
		auto name = i->first;
		auto type = i->second;

		if (type == expression_type::Double)
			_out << "@" << name << " = common global double 0.0e+0, align 8" << std::endl;
		else
			_out << "@" << name << " = common global i64 0, align 8" << std::endl;
	}
}

size_t get_input_format_length(const std::string& variable_name) {
	return variable_name.length() + 3;
}

std::string get_input_format(const std::string& variable_name) {
	return "c\"" + variable_name + ": \\00\"";
}

void step2_generator::print_input_formats() {
	for (auto i = _input_only_static_variables.cbegin(); i != _input_only_static_variables.cend(); i++) {
		auto name = i->first;
		auto type = i->second;

		_out << "@" << name << ".format = private constant [" << get_input_format_length(name)
			<< " x i8] " << get_input_format(name) << std::endl;
	}

	_out << "@i8_input = private constant [4 x i8] c\"%ld\\00\"" << std::endl;
	_out << "@double_input = private constant [4 x i8] c\"%lf\\00\"" << std::endl;
}

size_t get_output_format_length(const std::string& variable_name) {
	return variable_name.length() + 8;
}

std::string get_output_format(const std::string& name, expression_type type) {
	if (type == expression_type::Double)
		return "c\"" + name + " = %lf\\0A\\00\"";

	return "c\"" + name + " = %ld\\0A\\00\"";
}

void step2_generator::print_output_formats() {
	for (auto i = _output_only_static_variables.cbegin(); i != _output_only_static_variables.cend(); i++) {
		auto name = i->first;
		auto type = i->second;

		_out << "@" << name << ".format = private constant [" << get_output_format_length(name)
			<< " x i8] " << get_output_format(name, type) << std::endl;
	}
}

void step2_generator::print_main_header() {
	_out << "define i32 @main() {" << std::endl;
	_out << "entry:" << std::endl;
}

void step2_generator::print_inputs() {
	for (auto i = _input_only_static_variables.cbegin(); i != _input_only_static_variables.cend(); i++) {
		auto name = i->first;
		auto type = i->second;

		_out << "  %" << _last_variable_index++ << " = call i32 (i8*, ...) @printf(i8* getelementptr (["
			<< name.length() + 3 << " x i8], [" << name.length() + 3 << " x i8]* @"
			<< name << ".format, i32 0, i32 0))" << std::endl;

		if (type == expression_type::Double) {
			_out << "  %" << _last_variable_index++
				<< " = call i32 (i8*, ...) @scanf(i8* getelementptr ([4 x i8], [4 x i8]* @double_input, i32 0, i32 0), double* nonnull @"
				<< name << ")" << std::endl;
		}
		else {
			_out << "  %" << _last_variable_index++
				<< " = call i32 (i8*, ...) @scanf(i8* getelementptr ([4 x i8], [4 x i8]* @i64_input, i32 0, i32 0), i64* nonnull @"
				<< name << ")" << std::endl;
		}
	}
}

int step2_generator::get_next_variable_index() {
	return _last_variable_index++;
}

int step2_generator::get_named_variable_index(const std::string& variable_name) {
	if (_named_variables.find(variable_name) == _named_variables.end()) {
		_named_variables[variable_name] = get_next_variable_index();

		if (get_variable_type(variable_name) == expression_type::Double)
			_out << "  %" << _named_variables[variable_name] << " = load double, double* @" << variable_name << ", align 8" << std::endl;
		else
			_out << "  %" << _named_variables[variable_name] << " = load i64, i64* @" << variable_name << ", align 8" << std::endl;
	}

	return _named_variables[variable_name];
}

void step2_generator::set_named_variable_index(const std::string& variable_name, expression_node node) {
	_named_variables[variable_name] = node.index;

	if (node.type() == expression_type::Double)
		_out << "  store double %" << node.index << ", double* @" << variable_name << ", align 8" << std::endl;
	else
		_out << "  store i64 %" << node.index << ", i64* @" << variable_name << ", align 8" << std::endl;
}

void step2_generator::print_assignments() {
	for (auto i = _assignments.cbegin(); i != _assignments.cend(); i++) {
		auto assignment = *i;

		assignment->accept(*this);
	}
}

void step2_generator::print_outputs() {
	for (auto i = _output_only_static_variables.cbegin(); i != _output_only_static_variables.cend(); i++) {
		auto name = i->first;
		auto type = i->second;
		int variable_index = get_named_variable_index(name);

		if (type == expression_type::Double) {
			_out << "  %" << get_next_variable_index() << " = call i32 (i8*, ...) @printf(i8* getelementptr (["
				<< name.length() + 8 << " x i8], [" << name.length() + 8 << " x i8]* @"
				<< name << ".format, i32 0, i32 0), double %" << variable_index << ")" << std::endl;
		}
		else {
			_out << "  %" << get_next_variable_index() << " = call i32 (i8*, ...) @printf(i8* getelementptr (["
				<< name.length() + 8 << " x i8], [" << name.length() + 8 << " x i8]* @"
				<< name << ".format, i32 0, i32 0), i64 %" << variable_index << ")" << std::endl;
		}
	}
}

void step2_generator::print_main_footer() {
	_out << "  ret i32 0" << std::endl;
	_out << "}" << std::endl;
	_out << "declare i32 @printf(i8*, ...)" << std::endl;
	_out << "declare i32 @scanf(i8*, ...)" << std::endl;
	_out << "declare double @pow(double, double)" << std::endl;
}

void step2_generator::print_external_functions() {
	for (auto i = _standard_functions.cbegin(); i != _standard_functions.cend(); i++) {
		auto function_name = *i;

		_out << "declare double @" << function_name << "(double)" << std::endl;
	}
}

expression_node step2_generator::cast_to_double(expression_node node) {
	int index = get_next_variable_index();

	_out << "  %" << index << " = sitofp i64 %" << node.index << " to double" << std::endl;

	return expression_node(index, expression_type::Double);
}

void step2_generator::visit_assignment(const ast_assignment* assignment) {
	auto declared_identifier = assignment->name();
	visitor::visit_assignment(assignment);

	auto expression = _expressions.top();
	_expressions.pop();

	if (get_variable_type(declared_identifier) != expression._type)
		throw new std::runtime_error("Incompatible type of variable `" + declared_identifier + "`.");

	set_named_variable_index(declared_identifier, expression);
}

void step2_generator::visit_long(const ast_long* _long) {
	int index = get_next_variable_index();

	_out << "  %" << index << " = add i64 0, " << _long->value() << std::endl;

	_expressions.push(expression_node(index, expression_type::Long));
}

void step2_generator::visit_double(const ast_double* _double) {
	int index = get_next_variable_index();

	_out << "  %" << index << " = fadd double 0.0, " << _double->value() << std::endl;

	_expressions.push(expression_node(index, expression_type::Double));
}

void step2_generator::visit_variable(const ast_variable* variable) {
	auto name = variable->name();
	expression_type type = variable->type();
	int index = get_named_variable_index(name);

	_expressions.push(expression_node(index, type));
}

void step2_generator::visit_call(const ast_call* call) {
	visitor::visit_call(call);

	if (call->parameters().size() > 1)
		throw new std::runtime_error("Functions with more than 1 parameter are not supported.");

	auto parameter = _expressions.top();
	_expressions.pop();

	int index = get_next_variable_index();
	_out << "  %" << index << " = call double @" << call->name() << "(" << get_node_as_text(parameter) << ")" << std::endl;

	_expressions.push(expression_node(index, expression_type::Double));
}

void step2_generator::visit_unary_operator(const ast_unary_operator* unary_operator) {
	visitor::visit_unary_operator(unary_operator);

	if (unary_operator->operation() == unary_operation::Positive)
		return;

	expression_node operand = _expressions.top();
	_expressions.pop();

	int index = get_next_variable_index();
	if (operand.type() == expression_type::Double)
		_out << "  %" << index << +" = fsub double 0.0, %" << operand.index << std::endl;
	else
		_out << "  %" << index << +" = sub i64 0, %" << operand.index << std::endl;

	_expressions.push(expression_node(index, operand._type));
}

void step2_generator::visit_binary_operator(const ast_binary_operator* binary_operator) {
	visitor::visit_binary_operator(binary_operator);

	expression_node right = _expressions.top();
	_expressions.pop();

	expression_node left = _expressions.top();
	_expressions.pop();

	if (left._type == expression_type::Double && right._type == expression_type::Long)
		right = cast_to_double(right);
	else if (left._type == expression_type::Long && right._type == expression_type::Double)
		left = cast_to_double(left);
	else if (left._type == expression_type::Long && right._type == expression_type::Long && binary_operator->operation() == "^") {
		left = cast_to_double(left);
		right = cast_to_double(right);
	}

	int index = get_next_variable_index();
	expression_type type = left._type;

	if (type == expression_type::Double) {
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
