/*
 * disjunction.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include "conjunction.h"

#ifndef parse_boolean_disjunction_h
#define parse_boolean_disjunction_h

namespace parse_boolean
{
struct disjunction : parse::syntax
{
	disjunction();
	disjunction(tokenizer &tokens, void *data = NULL);
	~disjunction();

	vector<conjunction> branches;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	string to_string(bool nl, string tab = "") const;
	parse::syntax *clone() const;
};
}

#endif
