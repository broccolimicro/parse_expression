/*
 * assignment.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "assignment.h"
#include "internal_choice.h"
#include <parse/default/instance.h>
#include <parse/default/symbol.h>

namespace parse_boolean
{
assignment::assignment()
{
	debug_name = "assignment";
	value = false;
	expression = NULL;
}

assignment::assignment(tokenizer &tokens, void *data)
{
	debug_name = "assignment";
	value = false;
	expression = NULL;
	parse(tokens, data);
}

assignment::assignment(const assignment &copy) : parse::syntax(copy)
{
	value = copy.value;
	variable = copy.variable;
	expression = NULL;
	if (copy.expression != NULL)
		expression = new internal_choice(*copy.expression);
}

assignment::~assignment()
{
	if (expression != NULL)
		delete expression;
	expression = NULL;
}

void assignment::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokenizer::level level = tokens.increment(true);
	tokens.expect<variable_name>();
	tokens.expect("(");

	if (tokens.decrement(__FILE__, __LINE__, data))
	{
		if (tokens.found<variable_name>())
		{
			tokens.increment(level, true);
			tokens.expect(level, "+");
			tokens.expect(level, "-");

			variable.parse(tokens, data);

			if (tokens.decrement(__FILE__, __LINE__, data))
			{
				if (tokens.found("+"))
					value = true;
				else if (tokens.found("-"))
					value = false;

				tokens.next();
			}
		}
		else if (tokens.found("("))
		{
			tokens.next();

			tokens.increment(true);
			tokens.expect(")");

			tokens.increment(true);
			tokens.expect<internal_choice>();

			if (tokens.decrement(__FILE__, __LINE__, data))
				expression = new internal_choice(tokens, data);

			if (tokens.decrement(__FILE__, __LINE__, data))
				tokens.next();
		}
	}

	tokens.syntax_end(this);
}

bool assignment::is_next(tokenizer &tokens, int i, void *data)
{
	return variable_name::is_next(tokens, i, data) || tokens.is_next("(", i);
}

void assignment::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<assignment>())
	{
		tokens.register_syntax<assignment>();
		tokens.register_token<parse::instance>();
		tokens.register_token<parse::symbol>();
		variable_name::register_syntax(tokens);
		internal_choice::register_syntax(tokens);
	}
}

string assignment::to_string(string tab) const
{
	if (expression != NULL && expression->valid)
		return "(" + expression->to_string(tab) + ")";
	else
		return variable.to_string(tab) + (value ? "+" : "-");
}

parse::syntax *assignment::clone() const
{
	return new assignment(*this);
}

assignment &assignment::operator=(const assignment &copy)
{
	parse::syntax::operator=(copy);
	value = copy.value;
	variable = copy.variable;
	if (copy.expression != NULL)
	{
		if (expression != NULL)
			*expression = *copy.expression;
		else
			expression = new internal_choice(*copy.expression);
	}
	else if (expression != NULL)
	{
		delete expression;
		expression = NULL;
	}

	return *this;
}
}
