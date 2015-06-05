/*
 * guard.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "guard.h"
#include <parse/default/symbol.h>
#include <parse/default/number.h>

namespace parse_boolean
{
guard::guard()
{
	debug_name = "guard";
	operation = OR;
}

guard::guard(tokenizer &tokens, int operation, void *data)
{
	debug_name = "guard";
	this->operation = operation;
	parse(tokens, data);
}

guard::~guard()
{
}

void guard::parse(tokenizer &tokens, void *data)
{
	string opstr[2] = {"&", "|"};
	tokens.syntax_start(this);

	tokens.increment(false);
	tokens.expect(opstr[operation]);

	bool invert = false;

	if (operation == AND)
	{
		tokens.increment(false);
		tokens.expect("~");

		while (tokens.decrement(__FILE__, __LINE__, data))
		{
			tokens.next();
			tokens.increment(false);
			tokens.expect("~");
			invert = !invert;
		}
	}

	if (operation == OR)
	{
		tokens.increment(true);
		tokens.expect<guard>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			guards.push_back(pair<guard, bool>(guard(tokens, 1-operation, data), invert));
	}
	else if (operation == AND)
	{
		tokens.increment(true);
		tokens.expect("(");
		tokens.expect("0");
		tokens.expect("1");
		tokens.expect<variable_name>();

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
				tokens.expect<guard>();

				if (tokens.decrement(__FILE__, __LINE__, data))
					guards.push_back(pair<guard, bool>(guard(tokens, 1-operation, data), invert));

				if (tokens.decrement(__FILE__, __LINE__, data))
					tokens.next();

				if (tokens.decrement(__FILE__, __LINE__, data))
				{
					tokens.next();

					tokens.increment(true);
					tokens.expect<parse::number>();

					if (tokens.decrement(__FILE__, __LINE__, data))
						guards.back().first.region = tokens.next();
				}
			}
			else if ((tokens.found("0") && invert) || (tokens.found("1") && !invert))
			{
				tokens.next();
				constants.push_back("1");
			}
			else if ((tokens.found("1") && invert) || (tokens.found("0") && !invert))
			{
				tokens.next();
				constants.push_back("0");
			}
			else if (tokens.found<variable_name>())
				literals.push_back(pair<variable_name, bool>(variable_name(tokens, data), invert));
		}
	}

	invert = false;

	while (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();

		tokens.increment(false);
		tokens.expect(opstr[operation]);

		if (operation == AND)
		{
			tokens.increment(false);
			tokens.expect("~");

			while (tokens.decrement(__FILE__, __LINE__, data))
			{
				tokens.next();
				tokens.increment(false);
				tokens.expect("~");
				invert = !invert;
			}
		}

		if (operation == OR)
		{
			tokens.increment(true);
			tokens.expect<guard>();

			if (tokens.decrement(__FILE__, __LINE__, data))
				guards.push_back(pair<guard, bool>(guard(tokens, 1-operation, data), invert));
		}
		else if (operation == AND)
		{
			tokens.increment(true);
			tokens.expect("(");
			tokens.expect("0");
			tokens.expect("1");
			tokens.expect<variable_name>();

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
					tokens.expect<guard>();

					if (tokens.decrement(__FILE__, __LINE__, data))
						guards.push_back(pair<guard, bool>(guard(tokens, 1-operation, data), invert));

					if (tokens.decrement(__FILE__, __LINE__, data))
						tokens.next();

					if (tokens.decrement(__FILE__, __LINE__, data))
					{
						tokens.next();

						tokens.increment(true);
						tokens.expect<parse::number>();

						if (tokens.decrement(__FILE__, __LINE__, data))
							guards.back().first.region = tokens.next();
					}
				}
				else if ((tokens.found("0") && invert) || (tokens.found("1") && !invert))
				{
					tokens.next();
					constants.push_back("1");
				}
				else if ((tokens.found("1") && invert) || (tokens.found("0") && !invert))
				{
					tokens.next();
					constants.push_back("0");
				}
				else if (tokens.found<variable_name>())
					literals.push_back(pair<variable_name, bool>(variable_name(tokens, data), invert));
			}
		}

		invert = false;
	}

	tokens.syntax_end(this);
}

bool guard::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next("~", i) || tokens.is_next("(", i) || tokens.is_next("0") || tokens.is_next("1") || variable_name::is_next(tokens, i, data);
}

void guard::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<guard>())
	{
		tokens.register_syntax<guard>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::number>();
		variable_name::register_syntax(tokens);
	}
}

string guard::to_string(string tab) const
{
	string opstr[2] = {"&", "|"};

	string result = "";
	if (guards.size() == 0 && literals.size() == 0 && constants.size() == 0)
		result = operation == OR ? "1" : "0";
	else
	{
		bool first = true;
		for (int i = 0; i < (int)constants.size(); i++)
		{
			if (!first)
				result += opstr[operation];

			result += constants[i];
			first = false;
		}

		for (int i = 0; i < (int)literals.size(); i++)
		{
			if (!first)
				result += opstr[operation];

			if (literals[i].second)
				result += "~";

			if (literals[i].first.valid)
				result += literals[i].first.to_string(tab);
			else
				result += "null";

			first = false;
		}

		for (int i = 0; i < (int)guards.size(); i++)
		{
			if (!first)
				result += opstr[operation];

			if (guards[i].second)
				result += "~";

			if (guards[i].second || (operation == AND && guards[i].first.operation == OR))
				result += "(";

			if (guards[i].first.valid)
				result += guards[i].first.to_string(tab);
			else
				result += "null";

			if (guards[i].second || (operation == AND && guards[i].first.operation == OR))
				result += ")";

			first = false;
		}
	}

	return result;
}

string guard::to_string(int depth, string tab) const
{
	string opstr[2] = {"&", "|"};

	string result = "";
	if (guards.size() == 0 && literals.size() == 0 && constants.size() == 0)
		result = operation == OR ? "1" : "0";
	else
	{
		bool first = true;
		for (int i = 0; i < (int)constants.size(); i++)
		{
			if (!first)
				result += opstr[operation];

			result += constants[i];
			first = false;
		}

		for (int i = 0; i < (int)literals.size(); i++)
		{
			if (!first)
				result += opstr[operation];

			if (literals[i].second)
				result += "~";

			if (literals[i].first.valid)
				result += literals[i].first.to_string(tab);
			else
				result += "null";

			first = false;
		}

		for (int i = 0; i < (int)guards.size(); i++)
		{
			if (!first)
			{
				result += opstr[operation];
				if (operation == OR && depth > 0)
					result += "\\n";
			}

			if (guards[i].second)
				result += "~";

			if (guards[i].second || (operation == AND && guards[i].first.operation == OR))
				result += "(";

			if (guards[i].first.valid)
				result += guards[i].first.to_string(operation == OR ? depth-1 : depth, tab);
			else
				result += "null";

			if (guards[i].second || (operation == AND && guards[i].first.operation == OR))
				result += ")";

			first = false;
		}
	}

	return result;
}

parse::syntax *guard::clone() const
{
	return new guard(*this);
}

}
