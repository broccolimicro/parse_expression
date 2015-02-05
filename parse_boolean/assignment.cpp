/*
 * assignment.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "assignment.h"
#include <parse/default/instance.h>
#include <parse/default/symbol.h>

namespace parse_boolean
{
assignment::assignment()
{
	debug_name = "assignment";
	value = false;
}

assignment::assignment(configuration &config, tokenizer &tokens)
{
	debug_name = "assignment";
	value = false;
	parse(config, tokens);
}

assignment::~assignment()
{

}

void assignment::parse(configuration &config, tokenizer &tokens)
{
	valid = true;

	tokens.increment(true);
	tokens.expect("+");
	tokens.expect("-");

	tokens.increment(true);
	tokens.expect<variable_name>();

	if (tokens.decrement(config, __FILE__, __LINE__))
		variable.parse(config, tokens);

	if (tokens.decrement(config, __FILE__, __LINE__))
	{
		if (tokens.found("+"))
			value = true;
		else if (tokens.found("-"))
			value = false;

		tokens.next();
	}
}

bool assignment::is_next(configuration &config, tokenizer &tokens, int i)
{
	return variable_name::is_next(config, tokens, i);
}

void assignment::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<assignment>())
	{
		tokens.register_syntax<assignment>();
		tokens.register_token<parse::instance>();
		tokens.register_token<parse::symbol>();
		variable_name::register_syntax(tokens);
	}
}

string assignment::to_string(string tab)
{
	return variable.to_string(tab) + (value ? "+" : "-");
}
}
