/*
 * guard.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "guard.h"
#include "variable_name.h"
#include "constant.h"
#include <parse/default/symbol.h>

namespace parse_boolean
{
guard::guard()
{
	debug_name = "guard";
	level = -1;
}

guard::guard(configuration &config, tokenizer &tokens, int i)
{
	debug_name = "guard";
	level = -1;
	parse(config, tokens, i);
}

guard::guard(const guard &g)
{
	debug_name = "guard";
	valid = g.valid;
	level = g.level;
	functions = g.functions;
	for (int i = 0; i < (int)g.operands.size(); i++)
	{
		if (g.operands[i]->is_a<guard>())
			operands.push_back(new guard(*(guard*)g.operands[i]));
		else if (g.operands[i]->is_a<variable_name>())
			operands.push_back(new variable_name(*(variable_name*)g.operands[i]));
		else if (g.operands[i]->is_a<constant>())
			operands.push_back(new constant(*(constant*)g.operands[i]));
	}
}

guard::~guard()
{
	for (int i = 0; i < (int)operands.size(); i++)
	{
		if (operands[i] != NULL)
			delete operands[i];
		operands[i] = NULL;
	}

	operands.clear();
	functions.clear();
}

void guard::parse(configuration &config, tokenizer &tokens, int i)
{
	vector<vector<string> > binary;
	binary.push_back(vector<string>());
	binary[0].push_back("|");
	binary.push_back(vector<string>());
	binary[1].push_back("&");
	vector<string> unary;
	unary.push_back("~");
	level = i;

	valid = true;

	// Binary Operators
	if (i < (int)binary.size())
	{
		tokens.increment(false);
		for (int j = 0; j < (int)binary[i].size(); j++)
			tokens.expect(binary[i][j]);

		tokens.increment(true);
		tokens.expect<guard>();

		if (tokens.decrement(config, __FILE__, __LINE__))
			operands.push_back(new guard(config, tokens, i+1));

		while (tokens.decrement(config, __FILE__, __LINE__))
		{
			functions.push_back(tokens.next());

			tokens.increment(false);
			for (int j = 0; j < (int)binary[i].size(); j++)
				tokens.expect(binary[i][j]);

			tokens.increment(true);
			tokens.expect<guard>();

			if (tokens.decrement(config, __FILE__, __LINE__))
				operands.push_back(new guard(config, tokens, i+1));
		}
	}
	// Unary operators
	else if (i == (int)binary.size())
	{
		tokens.increment(true);
		tokens.expect<guard>();

		tokens.increment(false);
		for (int j = 0; j < (int)unary.size(); j++)
			tokens.expect(unary[j]);

		while (tokens.decrement(config, __FILE__, __LINE__))
		{
			functions.push_back(tokens.next());

			tokens.increment(false);
			for (int j = 0; j < (int)unary.size(); j++)
				tokens.expect(unary[j]);
		}

		if (tokens.decrement(config, __FILE__, __LINE__))
			operands.push_back(new guard(config, tokens, i+1));
	}
	// Parens and Variable names
	else
	{
		tokens.increment(true);
		tokens.expect<variable_name>();
		tokens.expect<constant>();
		tokens.expect("(");

		if (tokens.decrement(config, __FILE__, __LINE__))
		{
			if (tokens.found("("))
			{
				tokens.next();
				tokens.increment(true);
				tokens.expect(")");

				tokens.increment(true);
				tokens.expect<guard>();

				if (tokens.decrement(config, __FILE__, __LINE__))
					operands.push_back(new guard(config, tokens));

				if (tokens.decrement(config, __FILE__, __LINE__))
					tokens.next();
			}
			else if (tokens.found<variable_name>())
				operands.push_back(new variable_name(config, tokens));
			else if (tokens.found<constant>())
				operands.push_back(new constant(config, tokens));
		}
	}
}

bool guard::is_next(configuration &config, tokenizer &tokens, int i)
{
	return variable_name::is_next(config, tokens, i) || constant::is_next(config, tokens, i) || tokens.is_next("(", i) || tokens.is_next("~", i);
}

void guard::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<guard>())
	{
		tokens.register_syntax<guard>();
		tokens.register_token<parse::symbol>();
		constant::register_syntax(tokens);
		variable_name::register_syntax(tokens);
	}
}

string guard::to_string(string tab) const
{
	string result = "";
	for (int i = 0; i < (int)operands.size(); i++)
	{
		if (level < 2 && i > 0)
			result += functions[i-1];
		else if (level == 2 && i == 0)
			for (int j = 0; j < (int)functions.size(); j++)
				result += functions[j];

		if (operands[i] != NULL && operands[i]->valid)
		{
			if (operands[i]->is_a<guard>() && ((guard*)operands[i])->level < level)
				result += "(" + operands[i]->to_string(tab) + ")";
			else
				result += operands[i]->to_string(tab);
		}
		else
			result += "null";
	}
	return result;
}

guard &guard::operator=(const guard &g)
{
	debug_name = "guard";
	valid = g.valid;
	level = g.level;
	functions = g.functions;
	for (int i = 0; i < (int)g.operands.size(); i++)
	{
		if (g.operands[i]->is_a<guard>())
			operands.push_back(new guard(*(guard*)g.operands[i]));
		else if (g.operands[i]->is_a<variable_name>())
			operands.push_back(new variable_name(*(variable_name*)g.operands[i]));
		else if (g.operands[i]->is_a<constant>())
			operands.push_back(new constant(*(constant*)g.operands[i]));
	}
	return *this;
}

}
