/*
 * slice.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "slice.h"
#include <parse/default/number.h>
#include <parse/default/symbol.h>

namespace parse_boolean
{
slice::slice()
{
	debug_name = "slice";
	lower = "";
	upper = "";
}

slice::slice(tokenizer &tokens, void *data)
{
	debug_name = "slice";
	lower = "";
	upper = "";
	parse(tokens, data);
}

slice::~slice()
{

}

void slice::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(true);
	tokens.expect("]");

	tokens.increment(false);
	tokens.expect("..");

	tokens.increment(true);
	tokens.expect<parse::number>();

	tokens.increment(true);
	tokens.expect("[");

	if (tokens.decrement(__FILE__, __LINE__, data))
		tokens.next();

	if (tokens.decrement(__FILE__, __LINE__, data))
	{
		lower = tokens.next();
		upper = lower;
	}

	if (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();

		tokens.increment(true);
		tokens.expect<parse::number>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			upper = tokens.next();
	}

	if (tokens.decrement(__FILE__, __LINE__, data))
		tokens.next();

	tokens.syntax_end(this);
}

bool slice::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next("[", i);
}

void slice::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<slice>())
	{
		tokens.register_syntax<slice>();
		tokens.register_token<parse::number>();
		tokens.register_token<parse::symbol>();
	}
}

string slice::to_string(string tab) const
{
	string result;
	result += "[" + lower;

	if (upper != lower)
		result += ".." + upper;

	result += "]";
	return result;
}

parse::syntax *slice::clone() const
{
	return new slice(*this);
}
}
