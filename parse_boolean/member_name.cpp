/*
 * member_name.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "member_name.h"
#include <parse/default/instance.h>

namespace parse_boolean
{
member_name::member_name()
{
	debug_name = "member_name";
}

member_name::member_name(tokenizer &tokens, void *data)
{
	debug_name = "member_name";
	parse(tokens, data);
}

member_name::~member_name()
{

}

void member_name::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(false);
	tokens.expect<slice>();

	tokens.increment(true);
	tokens.expect<parse::instance>();

	if (tokens.decrement(__FILE__, __LINE__, data))
		name = tokens.next();

	while (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.increment(false);
		tokens.expect<slice>();

		slices.push_back(slice(tokens, data));
	}

	tokens.syntax_end(this);
}

bool member_name::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next<parse::instance>(i);
}

void member_name::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<member_name>())
	{
		tokens.register_syntax<member_name>();
		tokens.register_token<parse::instance>();
		slice::register_syntax(tokens);
	}
}

string member_name::to_string(string tab) const
{
	string result = name;
	for (int i = 0; i < (int)slices.size(); i++)
		result += slices[i].to_string(tab);

	return result;
}

parse::syntax *member_name::clone() const
{
	return new member_name(*this);
}
}
