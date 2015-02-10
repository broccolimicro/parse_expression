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

assignment::assignment(tokenizer &tokens, void *data)
{
	debug_name = "assignment";
	value = false;
	parse(tokens, data);
}

assignment::~assignment()
{

}

void assignment::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(true);
	tokens.expect("+");
	tokens.expect("-");

	tokens.increment(true);
	tokens.expect<variable_name>();

	if (tokens.decrement(__FILE__, __LINE__, data))
		variable.parse(tokens, data);

	if (tokens.decrement(__FILE__, __LINE__, data))
	{
		if (tokens.found("+"))
			value = true;
		else if (tokens.found("-"))
			value = false;

		tokens.next();
	}

	tokens.syntax_end(this);
}

bool assignment::is_next(tokenizer &tokens, int i, void *data)
{
	return variable_name::is_next(tokens, i, data);
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

string assignment::to_string(string tab) const
{
	return variable.to_string(tab) + (value ? "+" : "-");
}
}
