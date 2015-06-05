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
	assignment(tokenizer &tokens, int operation = CHOICE, void *data = NULL);
	~assignment();

	vector<assignment> assignments;
	vector<pair<variable_name, bool> > literals;
	string region;
	int operation;

	static const int PARALLEL = 0;
	static const int CHOICE = 1;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	string to_string(int depth, string tab = "") const;
	parse::syntax *clone() const;
};
}

#endif
