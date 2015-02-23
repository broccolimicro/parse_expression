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
struct internal_choice;

struct assignment : parse::syntax
{
	assignment();
	assignment(tokenizer &tokens, void *data = NULL);
	assignment(const assignment &copy);
	~assignment();

	bool value;
	variable_name variable;
	internal_choice *expression;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	parse::syntax *clone() const;

	assignment &operator=(const assignment &copy);
};
}

#endif
