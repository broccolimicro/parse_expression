/*
 * complement.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "complement.h"
#include "disjunction.h"
#include <parse/default/symbol.h>
#include <parse/default/number.h>

namespace parse_boolean
{
complement::complement()
{
	debug_name = "complement";
	value = "";
	invert = false;
	expression = NULL;
}

complement::complement(tokenizer &tokens, void *data)
{
	debug_name = "complement";
	value = "";
	invert = false;
	expression = NULL;
	parse(tokens, data);
}

complement::complement(const complement &copy) : parse::syntax(copy)
{
	invert = copy.invert;
	value = copy.value;
	variable = copy.variable;
	expression = NULL;
	if (copy.expression != NULL)
		expression = new disjunction(*copy.expression);
}

complement::~complement()
{
	if (expression != NULL)
		delete expression;
	expression = NULL;
}

void complement::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(true);
	tokens.expect<parse::number>();
	tokens.expect<variable_name>();
	tokens.expect("(");

	tokens.increment(false);
	tokens.expect("~");

	while (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();
		tokens.increment(false);
		tokens.expect("~");
		invert = !invert;
	}

	if (tokens.decrement(__FILE__, __LINE__, data))
	{
		if (tokens.found("("))
		{
			tokens.next();

			tokens.increment(true);
			tokens.expect(")");

			tokens.increment(true);
			tokens.expect<disjunction>();

			if (tokens.decrement(__FILE__, __LINE__, data))
				expression = new disjunction(tokens, data);

			if (tokens.decrement(__FILE__, __LINE__, data))
				tokens.next();
		}
		else if (tokens.found<variable_name>())
			variable.parse(tokens, data);
		else if (tokens.found<parse::number>())
			value = tokens.next();
	}

	tokens.syntax_end(this);
}

bool complement::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next("~", i) || tokens.is_next<parse::number>(i) || variable_name::is_next(tokens, i, data) || tokens.is_next("(", i);
}

void complement::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<complement>())
	{
		tokens.register_syntax<complement>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::number>();
		variable_name::register_syntax(tokens);
		disjunction::register_syntax(tokens);
	}
}

string complement::to_string(string tab) const
{
	string result = invert ? "~" : "";

	if (value != "")
		result += value;
	else if (variable.valid)
		result += variable.to_string(tab);
	else if (expression != NULL && expression->valid)
		result += "(" + expression->to_string(tab) + ")";
	else
		result += "null";

	return result;
}

parse::syntax *complement::clone() const
{
	return new complement(*this);
}

complement &complement::operator=(const complement &copy)
{
	parse::syntax::operator=(copy);
	invert = copy.invert;
	value = copy.value;
	variable = copy.variable;
	if (copy.expression != NULL)
	{
		if (expression != NULL )
			*expression = *copy.expression;
		else
			expression = new disjunction(*copy.expression);
	}
	else if (expression != NULL)
	{
		delete expression;
		expression = NULL;
	}

	return *this;
}

}
