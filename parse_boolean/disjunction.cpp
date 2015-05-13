/*
 * disjunction.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "disjunction.h"
#include "conjunction.h"
#include <parse/default/symbol.h>

namespace parse_boolean
{
disjunction::disjunction()
{
	debug_name = "disjunction";
}

disjunction::disjunction(tokenizer &tokens, void *data)
{
	debug_name = "disjunction";
	parse(tokens, data);
}

disjunction::~disjunction()
{
}

void disjunction::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(false);
	tokens.expect("|");

	tokens.increment(true);
	tokens.expect<conjunction>();

	if (tokens.decrement(__FILE__, __LINE__, data))
		branches.push_back(conjunction(tokens, data));

	while (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();

		tokens.increment(false);
		tokens.expect("|");

		tokens.increment(true);
		tokens.expect<conjunction>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			branches.push_back(conjunction(tokens, data));
	}

	tokens.syntax_end(this);
}

bool disjunction::is_next(tokenizer &tokens, int i, void *data)
{
	return conjunction::is_next(tokens, i, data);
}

void disjunction::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<disjunction>())
	{
		tokens.register_syntax<disjunction>();
		tokens.register_token<parse::symbol>();
		conjunction::register_syntax(tokens);
	}
}

string disjunction::to_string(string tab) const
{
	string result = "";
	if (branches.size() == 0)
		result = "0";
	else
		for (int i = 0; i < (int)branches.size(); i++)
		{
			if (i != 0)
				result += "|";

			if (branches[i].valid)
				result += branches[i].to_string(tab);
			else
				result += "null";
		}

	return result;
}

string disjunction::to_string(bool nl, string tab) const
{
	string result = "";
	if (branches.size() == 0)
		result = "0";
	else
		for (int i = 0; i < (int)branches.size(); i++)
		{
			if (i != 0)
			{
				result += "|";
				if (nl)
					result += "\n";
			}

			if (branches[i].valid)
				result += branches[i].to_string(tab);
			else
				result += "null";
		}

	return result;
}

parse::syntax *disjunction::clone() const
{
	return new disjunction(*this);
}

}
