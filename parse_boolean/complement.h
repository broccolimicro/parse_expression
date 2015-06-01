/*
 * complement.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include "variable_name.h"

#ifndef parse_boolean_complement_h
#define parse_boolean_complement_h

namespace parse_boolean
{
struct disjunction;

struct complement : parse::syntax
{
	complement();
	complement(tokenizer &tokens, void *data = NULL);
	complement(const complement &copy);
	~complement();

	bool invert;
	string value;
	variable_name variable;
	disjunction *expression;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	string to_string(int depth, string tab = "") const;
	parse::syntax *clone() const;

	complement &operator=(const complement &copy);
};
}

#endif
