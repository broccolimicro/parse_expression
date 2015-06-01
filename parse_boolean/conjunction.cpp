/*
 * conjunction.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "conjunction.h"
#include "complement.h"
#include <parse/default/symbol.h>

namespace parse_boolean
{
conjunction::conjunction()
{
	debug_name = "conjunction";
}

conjunction::conjunction(tokenizer &tokens, void *data)
{
	debug_name = "conjunction";
	parse(tokens, data);
}

conjunction::~conjunction()
{
}

void conjunction::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(false);
	tokens.expect("&");

	tokens.increment(true);
	tokens.expect<complement>();

	if (tokens.decrement(__FILE__, __LINE__, data))
		branches.push_back(complement(tokens, data));

	while (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();

		tokens.increment(false);
		tokens.expect("&");

		tokens.increment(true);
		tokens.expect<complement>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			branches.push_back(complement(tokens, data));
	}

	tokens.syntax_end(this);
}

bool conjunction::is_next(tokenizer &tokens, int i, void *data)
{
	return complement::is_next(tokens, i, data);
}

void conjunction::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<conjunction>())
	{
		tokens.register_syntax<conjunction>();
		tokens.register_token<parse::symbol>();
		complement::register_syntax(tokens);
	}
}

string conjunction::to_string(string tab) const
{
	string result = "";
	if (branches.size() == 0)
		result = "1";
	else
		for (int i = 0; i < (int)branches.size(); i++)
		{
			if (i != 0)
				result += "&";

			if (branches[i].valid)
				result += branches[i].to_string(tab);
			else
				result += "null";
		}
	return result;
}

string conjunction::to_string(int depth, string tab) const
{
	string result = "";
	if (branches.size() == 0)
		result = "1";
	else
		for (int i = 0; i < (int)branches.size(); i++)
		{
			if (i != 0)
				result += "&";

			if (branches[i].valid)
				result += branches[i].to_string(depth, tab);
			else
				result += "null";
		}
	return result;
}

parse::syntax *conjunction::clone() const
{
	return new conjunction(*this);
}

}
