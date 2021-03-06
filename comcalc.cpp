#include <exception>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

#include "parser.h"
#include "printer.h"
#include "generator.h"

#if defined WIN32 || defined _WIN32 || defined __CYGWIN__
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

void compile(const std::string& infile, const std::string& outfile);
void compile(const ast_program* program, const std::string& outfile);
std::string replace_extension(const std::string& filename, const std::string& extension);

int main(int argc, const char* const* argv) {
    std::cout << "COMpiling CALCulator" << std::endl;

    if(argc < 2 || argc > 3) {
        std::cerr << "  Usage: comcalc in.cc [out.ll]    -- generate LLVM IR" << std::endl;
        std::cerr << "         comcalc in.cc --ast       -- print AST" << std::endl;

        return 2;
    }

    try {
        std::string infile = argv[1];
        std::string outfile = argc == 3 ? std::string(argv[2]) : replace_extension(infile, ".ll");
        
        compile(infile, outfile);
    }
    catch(std::exception &exception) {
        std::cerr << exception.what() << std::endl;

        return 1;
    }

    return 0;
}

void compile(const std::string& infile, const std::string& outfile) {
	std::ifstream in;
	in.open(infile);

	try {
		parser parser(in);
		const ast_program* program = parser.parse_program();

		if (outfile == "--ast")
			print(program, std::cout);
		else
			compile(program, outfile);
	}
	catch (std::exception&) {
		in.close();

		throw;
	}
}

void compile(const ast_program* program, const std::string& outfile) {
	std::ofstream out;
	out.open(outfile);

	try {
		generate(program, out);
	}
	catch (std::exception&) {
		out.close();

		throw;
	}
}


std::string replace_extension(const std::string& filename, const std::string& extension) {
	size_t separator_position = filename.rfind(PATH_SEPARATOR);
	size_t point_position = filename.rfind('.');

	if (separator_position == std::string::npos) {
		if (point_position == std::string::npos || point_position == 0)
			return filename + extension;
	}
	else {
		if (point_position == std::string::npos || point_position < separator_position + 2)
			return filename + extension;
	}

	return filename.substr(0, point_position) + extension;
}

