/*
 * guard.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include "variable_name.h"

#ifndef parse_boolean_guard_h
#define parse_boolean_guard_h

namespace parse_boolean
{
struct guard : parse::syntax
{
	guard();
	guard(tokenizer &tokens, int operation = OR, void *data = NULL);
	~guard();

	vector<pair<guard, int> > guards;
	vector<pair<variable_name, int> > literals;
	vector<string> constants;
	string region;
	int operation;

	static const int AND = 0;
	static const int OR = 1;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	string to_string(int depth, string tab = "") const;
	parse::syntax *clone() const;
};
}

#endif
