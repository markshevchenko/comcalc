#ifndef __PRINTER_H__
#define __PRINTER_H__

#include <ostream>

#include "ast.h"

void print(const ast_program* program, std::ostream& out);

#endif