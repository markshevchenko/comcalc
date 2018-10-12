#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <ostream>

#include "ast.h"

void generate(const ast_program* program, std::ostream& out);

#endif
