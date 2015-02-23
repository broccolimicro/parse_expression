/*
 * internal_choice.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "internal_choice.h"
#include <parse/default/symbol.h>

namespace parse_boolean
{
internal_choice::internal_choice()
{
	debug_name = "internal_choice";
}

internal_choice::internal_choice(tokenizer &tokens, void *data)
{
	debug_name = "internal_choice";
	parse(tokens, data);
}

internal_choice::~internal_choice()
{
}

void internal_choice::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(false);
	tokens.expect(":");

	tokens.increment(true);
	tokens.expect<internal_parallel>();

	if (tokens.decrement(__FILE__, __LINE__, data))
		branches.push_back(internal_parallel(tokens, data));

	while (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();

		tokens.increment(false);
		tokens.expect(":");

		tokens.increment(true);
		tokens.expect<internal_parallel>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			branches.push_back(internal_parallel(tokens, data));
	}

	tokens.syntax_end(this);
}

bool internal_choice::is_next(tokenizer &tokens, int i, void *data)
{
	return internal_parallel::is_next(tokens, i, data);
}

void internal_choice::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<internal_choice>())
	{
		tokens.register_syntax<internal_choice>();
		tokens.register_token<parse::symbol>();
		internal_parallel::register_syntax(tokens);
	}
}

string internal_choice::to_string(string tab) const
{
	string result = "";
	for (int i = 0; i < (int)branches.size(); i++)
	{
		if (i != 0)
			result += ":";

		if (branches[i].valid)
			result += branches[i].to_string(tab);
		else
			result += "null";
	}
	return result;
}

parse::syntax *internal_choice::clone() const
{
	return new internal_choice(*this);
}

}
