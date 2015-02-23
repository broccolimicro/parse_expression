/*
 * member_name.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include "slice.h"

#ifndef parse_boolean_member_name_h
#define parse_boolean_member_name_h

namespace parse_boolean
{
struct member_name : parse::syntax
{
	member_name();
	member_name(tokenizer &tokens, void *data = NULL);
	~member_name();

	string name;
	vector<slice> slices;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	parse::syntax *clone() const;
};
}

#endif
