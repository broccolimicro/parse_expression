/*
 * internal_choice.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include "internal_parallel.h"

#ifndef parse_boolean_internal_choice_h
#define parse_boolean_internal_choice_h

namespace parse_boolean
{
struct internal_choice : parse::syntax
{
	internal_choice();
	internal_choice(tokenizer &tokens, void *data = NULL);
	~internal_choice();

	vector<internal_parallel> branches;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	parse::syntax *clone() const;
};
}

#endif
