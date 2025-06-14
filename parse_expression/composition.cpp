/*
 * composition.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "composition.h"
#include <parse/default/symbol.h>
#include <parse/default/number.h>
#include <parse/default/white_space.h>

namespace parse_expression
{

vector<string> composition::precedence;

composition::composition()
{
	debug_name = "composition";
	level = 0;
	init();
}

composition::composition(tokenizer &tokens, int level, void *data)
{
	debug_name = "composition";
	this->level = level;
	init();
	parse(tokens, data);
}

composition::~composition()
{
}

void composition::init()
{
	if (precedence.size() == 0)
	{
		precedence.push_back(":");
		precedence.push_back(",");
	}
}

int composition::get_level(string operation)
{
	for (int i = 0; i < (int)precedence.size(); i++)
		if (precedence[i] == operation)
			return i;

	return -1;
}

void composition::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	bool first = true;
	do
	{
		if (first)
			first = false;
		else
			tokens.next();

		tokens.increment(false);
		tokens.expect(precedence[level]);

		if (level < (int)precedence.size()-1)
		{
			tokens.increment(true);
			tokens.expect<composition>();

			if (tokens.decrement(__FILE__, __LINE__, data))
				compositions.push_back(composition(tokens, level+1, data));
		}
		else
		{
			tokens.increment(true);
			tokens.expect("(");
			tokens.expect<assignment>();
			tokens.expect("[");

			if (tokens.decrement(__FILE__, __LINE__, data))
			{
				if (tokens.found("("))
				{
					tokens.next();

					tokens.increment(false);
					tokens.expect("'");

					tokens.increment(true);
					tokens.expect(")");

					tokens.increment(true);
					tokens.expect<composition>();

					if (tokens.decrement(__FILE__, __LINE__, data))
						compositions.push_back(composition(tokens, 0, data));

					if (tokens.decrement(__FILE__, __LINE__, data))
						tokens.next();

					if (tokens.decrement(__FILE__, __LINE__, data))
					{
						tokens.next();

						tokens.increment(true);
						tokens.expect<parse::number>();

						if (tokens.decrement(__FILE__, __LINE__, data))
							compositions.back().region = tokens.next();
					}
				}
				else if (tokens.found<assignment>())
					literals.push_back(assignment(tokens, data));
				else if (tokens.found("["))
				{
					tokens.next();

					tokens.increment(true);
					tokens.expect("]");

					tokens.increment(true);
					tokens.expect<expression>();

					if (tokens.decrement(__FILE__, __LINE__, data))
						guards.push_back(expression(tokens, 0, data));

					if (tokens.decrement(__FILE__, __LINE__, data))
						tokens.next();
				}
			}
		}
	} while (tokens.decrement(__FILE__, __LINE__, data));

	tokens.syntax_end(this);
}

bool composition::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next("(", i) || tokens.is_next("[", i) || assignment::is_next(tokens, i, data);
}

void composition::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<composition>())
	{
		tokens.register_syntax<composition>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::number>();
		tokens.register_token<parse::white_space>(false);
		assignment::register_syntax(tokens);
		expression::register_syntax(tokens);
	}
}

string composition::to_string(string tab) const
{
	return to_string(-1, tab);
}

string composition::to_string(int prev_level, string tab) const
{
	if (!valid or level < 0 or (literals.empty() and compositions.empty() and guards.empty())) {
		if (level == 0) {
			return "null";
		} else if (level == 1) {
			return "skip";
		}
		return "error";
	}

	string result = "";
	bool first = true;
	for (int i = 0; i < (int)guards.size(); i++) {
		if (!first) {
			result += precedence[level];
		}
		result += "[" + guards[i].to_string(tab) + "]";
		first = false;
	}

	for (int i = 0; i < (int)literals.size(); i++) {
		if (!first) {
			result += precedence[level];
		}

		result += literals[i].to_string(tab);
		first = false;
	}

	for (int i = 0; i < (int)compositions.size(); i++)
	{
		if (!first)
			result += precedence[level];

		if (level < prev_level)
			result += "(";

		result += compositions[i].to_string(level, tab);

		if (level < prev_level)
			result += ")";

		first = false;
	}

	return result;
}

parse::syntax *composition::clone() const
{
	return new composition(*this);
}

}
