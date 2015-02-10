/*
 * assignment.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include "variable_name.h"

#ifndef parse_boolean_assignment_h
#define parse_boolean_assignment_h

namespace parse_boolean
{
struct assignment : parse::syntax
{
	assignment();
	assignment(tokenizer &tokens, void *data = NULL);
	~assignment();

	variable_name variable;
	bool value;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
};
}

#endif
