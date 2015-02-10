/*
 * variable_name.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "variable_name.h"
#include <parse/default/instance.h>
#include <parse/default/symbol.h>

namespace parse_boolean
{
variable_name::variable_name()
{
	debug_name = "variable_name";
}

variable_name::variable_name(tokenizer &tokens, void *data)
{
	debug_name = "variable_name";
	parse(tokens, data);
}

variable_name::~variable_name()
{

}

void variable_name::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(false);
	tokens.expect(".");

	tokens.increment(true);
	tokens.expect<parse::instance>();

	if (tokens.decrement(__FILE__, __LINE__, data))
		names.push_back(tokens.next());

	while (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();

		tokens.increment(false);
		tokens.expect(".");

		tokens.increment(true);
		tokens.expect<parse::instance>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			names.push_back(tokens.next());
	}

	tokens.syntax_end(this);
}

bool variable_name::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next<parse::instance>(i);
}

void variable_name::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<variable_name>())
	{
		tokens.register_syntax<variable_name>();
		tokens.register_token<parse::instance>();
		tokens.register_token<parse::symbol>();
	}
}

string variable_name::to_string(string tab) const
{
	string result;
	if (names.size() > 0)
		result = names[0];

	for (int i = 1; i < (int)names.size(); i++)
		result += "." + names[i];
	return result;
}
}
