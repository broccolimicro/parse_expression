/*
 * assignment.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include <parse_ucs/variable_name.h>
#include "expression.h"

#ifndef parse_expression_assignment_h
#define parse_expression_assignment_h

namespace parse_expression
{
using parse_ucs::variable_name;

struct assignment : parse::syntax
{
	assignment();
	assignment(tokenizer &tokens, void *data = NULL);
	~assignment();

	vector<expression> lvalue;
	expression rvalue;
	string operation;
	string region;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	string to_string(int depth, string tab = "") const;
	parse::syntax *clone() const;
};
}

#endif
