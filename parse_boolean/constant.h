/*
 * constant.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>

#ifndef parse_boolean_constant_h
#define parse_boolean_constant_h

namespace parse_boolean
{
struct constant : parse::syntax
{
	constant();
	constant(tokenizer &tokens, void *data = NULL);
	~constant();

	string value;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
};
}

#endif
