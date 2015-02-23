/*
 * slice.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>

#ifndef parse_boolean_slice_h
#define parse_boolean_slice_h

namespace parse_boolean
{
struct slice : parse::syntax
{
	slice();
	slice(tokenizer &tokens, void *data = NULL);
	~slice();

	string lower;
	string upper;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	parse::syntax *clone() const;
};
}

#endif
