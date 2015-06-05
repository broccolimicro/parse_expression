/*
 * assignment.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "assignment.h"
#include <parse/default/symbol.h>
#include <parse/default/number.h>

namespace parse_boolean
{
assignment::assignment()
{
	debug_name = "assignment";
	operation = CHOICE;
}

assignment::assignment(tokenizer &tokens, int operation, void *data)
{
	debug_name = "assignment";
	this->operation = operation;
	parse(tokens, data);
}

assignment::~assignment()
{
}

void assignment::parse(tokenizer &tokens, void *data)
{
	string opstr[2] = {",", ":"};
	tokens.syntax_start(this);

	tokens.increment(false);
	tokens.expect(opstr[operation]);

	if (operation == CHOICE)
	{
		tokens.increment(true);
		tokens.expect<assignment>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			assignments.push_back(assignment(tokens, 1-operation, data));
	}
	else if (operation == PARALLEL)
	{
		tokens.increment(true);
		tokens.expect("(");
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
				tokens.expect<assignment>();

				if (tokens.decrement(__FILE__, __LINE__, data))
					assignments.push_back(assignment(tokens, 1-operation, data));

				if (tokens.decrement(__FILE__, __LINE__, data))
					tokens.next();

				if (tokens.decrement(__FILE__, __LINE__, data))
				{
					tokens.next();

					tokens.increment(true);
					tokens.expect<parse::number>();

					if (tokens.decrement(__FILE__, __LINE__, data))
						assignments.back().region = tokens.next();
				}
			}
			else if (tokens.found<variable_name>())
			{
				literals.push_back(pair<variable_name, bool>(variable_name(tokens, data), false));

				tokens.increment(true);
				tokens.expect("+");
				tokens.expect("-");

				if (tokens.decrement(__FILE__, __LINE__, data))
				{
					if (tokens.found("-"))
						literals.back().second = true;

					tokens.next();
				}
			}
		}
	}

	while (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();

		tokens.increment(false);
		tokens.expect(opstr[operation]);

		if (operation == CHOICE)
		{
			tokens.increment(true);
			tokens.expect<assignment>();

			if (tokens.decrement(__FILE__, __LINE__, data))
				assignments.push_back(assignment(tokens, 1-operation, data));
		}
		else if (operation == PARALLEL)
		{
			tokens.increment(true);
			tokens.expect("(");
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
					tokens.expect<assignment>();

					if (tokens.decrement(__FILE__, __LINE__, data))
						assignments.push_back(assignment(tokens, 1-operation, data));

					if (tokens.decrement(__FILE__, __LINE__, data))
						tokens.next();

					if (tokens.decrement(__FILE__, __LINE__, data))
					{
						tokens.next();

						tokens.increment(true);
						tokens.expect<parse::number>();

						if (tokens.decrement(__FILE__, __LINE__, data))
							assignments.back().region = tokens.next();
					}
				}
				else if (tokens.found<variable_name>())
				{
					literals.push_back(pair<variable_name, bool>(variable_name(tokens, data), false));

					tokens.increment(true);
					tokens.expect("+");
					tokens.expect("-");

					if (tokens.decrement(__FILE__, __LINE__, data))
					{
						if (tokens.found("-"))
							literals.back().second = true;

						tokens.next();
					}
				}
			}
		}
	}

	tokens.syntax_end(this);
}

bool assignment::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next("(", i) || variable_name::is_next(tokens, i, data);
}

void assignment::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<assignment>())
	{
		tokens.register_syntax<assignment>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::number>();
		variable_name::register_syntax(tokens);
	}
}

string assignment::to_string(string tab) const
{
	if (literals.size() == 0 && assignments.size() == 0)
		return "1";

	string opstr[2] = {",", ":"};

	string result = "";
	bool first = true;
	for (int i = 0; i < (int)literals.size(); i++)
	{
		if (!first)
			result += opstr[operation];

		if (literals[i].first.valid)
			result += literals[i].first.to_string(tab);
		else
			result += "null";

		if (literals[i].second)
			result += "-";
		else
			result += "+";

		first = false;
	}

	for (int i = 0; i < (int)assignments.size(); i++)
	{
		if (!first)
			result += opstr[operation];

		if (operation == PARALLEL && assignments[i].operation == CHOICE)
			result += "(";

		if (assignments[i].valid)
			result += assignments[i].to_string(tab);
		else
			result += "null";

		if (operation == PARALLEL && assignments[i].operation == CHOICE)
			result += ")";

		first = false;
	}

	return result;
}

string assignment::to_string(int depth, string tab) const
{
	if (literals.size() == 0 && assignments.size() == 0)
		return "1";

	string opstr[2] = {"&", "|"};

	string result = "";
	bool first = true;
	for (int i = 0; i < (int)literals.size(); i++)
	{
		if (!first)
			result += opstr[operation];

		if (literals[i].first.valid)
			result += literals[i].first.to_string(tab);
		else
			result += "null";

		if (literals[i].second)
			result += "-";
		else
			result += "+";

		first = false;
	}

	for (int i = 0; i < (int)assignments.size(); i++)
	{
		if (!first)
		{
			result += opstr[operation];
			if (operation == CHOICE && depth > 0)
				result += "\\n";
		}

		if (operation == PARALLEL && assignments[i].operation == CHOICE)
			result += "(";

		if (assignments[i].valid)
			result += assignments[i].to_string(operation == CHOICE ? depth-1 : depth, tab);
		else
			result += "null";

		if (operation == PARALLEL && assignments[i].operation == CHOICE)
			result += ")";

		first = false;
	}

	return result;
}

parse::syntax *assignment::clone() const
{
	return new assignment(*this);
}

}
