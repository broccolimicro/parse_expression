/*
 * internal_parallel.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "internal_parallel.h"
#include <parse/default/symbol.h>

namespace parse_boolean
{
internal_parallel::internal_parallel()
{
	debug_name = "internal_parallel";
}

internal_parallel::internal_parallel(tokenizer &tokens, void *data)
{
	debug_name = "internal_parallel";
	parse(tokens, data);
}

internal_parallel::~internal_parallel()
{
}

void internal_parallel::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(false);
	tokens.expect(",");

	tokens.increment(true);
	tokens.expect<assignment>();

	if (tokens.decrement(__FILE__, __LINE__, data))
		branches.push_back(assignment(tokens, data));

	while (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();

		tokens.increment(false);
		tokens.expect(",");

		tokens.increment(true);
		tokens.expect<assignment>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			branches.push_back(assignment(tokens, data));
	}

	tokens.syntax_end(this);
}

bool internal_parallel::is_next(tokenizer &tokens, int i, void *data)
{
	return assignment::is_next(tokens, i, data);
}

void internal_parallel::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<internal_parallel>())
	{
		tokens.register_syntax<internal_parallel>();
		tokens.register_token<parse::symbol>();
		assignment::register_syntax(tokens);
	}
}

string internal_parallel::to_string(string tab) const
{
	string result = "";
	for (int i = 0; i < (int)branches.size(); i++)
	{
		if (i != 0)
			result += ",";

		if (branches[i].valid)
			result += branches[i].to_string(tab);
		else
			result += "null";
	}
	return result;
}

parse::syntax *internal_parallel::clone() const
{
	return new internal_parallel(*this);
}

}
