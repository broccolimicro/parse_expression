/*
 * constant.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "constant.h"
#include <parse/default/number.h>

namespace parse_boolean
{
constant::constant()
{
	debug_name = "constant";
}

constant::constant(tokenizer &tokens, void *data)
{
	debug_name = "constant";
	parse(tokens, data);
}

constant::~constant()
{

}

void constant::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(true);
	tokens.expect<parse::number>();

	if (tokens.decrement(__FILE__, __LINE__, data))
		value = tokens.next();

	tokens.syntax_end(this);
}

bool constant::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next<parse::number>(i);
}

void constant::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<constant>())
	{
		tokens.register_syntax<constant>();
		tokens.register_token<parse::number>();
	}
}

string constant::to_string(string tab) const
{
	return value;
}
}
