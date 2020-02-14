#include "generator.h"
#include "step1_tables_builder.h"
#include "step2_generator.h"

void generate(const ast_program* program, std::ostream& out) {
	step1_tables_builder builder;
	auto table_registry = builder.build(program);

	step2_generator generator(table_registry, out);
	generator.print_code();
}
