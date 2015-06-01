/*
 * conjunction.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include "complement.h"

#ifndef parse_boolean_conjunction_h
#define parse_boolean_conjunction_h

namespace parse_boolean
{
struct conjunction : parse::syntax
{
	conjunction();
	conjunction(tokenizer &tokens, void *data = NULL);
	~conjunction();

	vector<complement> branches;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	string to_string(int depth, string tab = "") const;
	parse::syntax *clone() const;
};
}

#endif
