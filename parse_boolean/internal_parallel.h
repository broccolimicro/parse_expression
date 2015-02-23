/*
 * internal_parallel.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include "assignment.h"

#ifndef parse_boolean_internal_parallel_h
#define parse_boolean_internal_parallel_h

namespace parse_boolean
{
struct internal_parallel : parse::syntax
{
	internal_parallel();
	internal_parallel(tokenizer &tokens, void *data = NULL);
	~internal_parallel();

	vector<assignment> branches;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	parse::syntax *clone() const;
};
}

#endif
