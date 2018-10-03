#include "llvm.h"
#include "pregenerator.h"
#include "generator.h"

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
