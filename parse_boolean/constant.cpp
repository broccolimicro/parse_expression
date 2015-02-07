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

constant::constant(configuration &config, tokenizer &tokens)
{
	debug_name = "constant";
	parse(config, tokens);
}

constant::~constant()
{

}

void constant::parse(configuration &config, tokenizer &tokens)
{
	valid = true;

	tokens.increment(true);
	tokens.expect<parse::number>();

	if (tokens.decrement(config, __FILE__, __LINE__))
		value = tokens.next();
}

bool constant::is_next(configuration &config, tokenizer &tokens, int i)
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
