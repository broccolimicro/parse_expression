/*
 * assignment.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "assignment.h"
#include <parse/default/symbol.h>
#include <parse/default/number.h>
#include <parse/default/white_space.h>
#include <parse/default/instance.h>

namespace parse_expression
{

assignment::assignment()
{
	debug_name = "assignment";
}

assignment::assignment(tokenizer &tokens, void *data)
{
	debug_name = "assignment";
	parse(tokens, data);
}

assignment::~assignment()
{
}

void assignment::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(false);
	tokens.expect("=");
	tokens.expect("+");
	tokens.expect("-");
	tokens.expect("~");
	tokens.expect("(");
	tokens.expect("?");
	tokens.expect("!");

	tokens.increment(true);
	tokens.expect<variable_name>();

	if (tokens.decrement(__FILE__, __LINE__, data)) {
		names.push_back(variable_name(tokens, data));
	}

	if (tokens.decrement(__FILE__, __LINE__, data)) {
		operation = tokens.next();
	}

	if (operation == "=") {
		tokens.increment(true);
		tokens.expect<expression>();

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			expressions.push_back(expression(tokens, 0, data));
		}
	} else if (operation == "?") {
		tokens.increment(false);
		tokens.expect("!");

		tokens.increment(false);
		tokens.expect<variable_name>();

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			names.push_back(variable_name(tokens, data));
		}

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			operation += tokens.next();

			tokens.increment(false);
			tokens.expect<expression>();

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				expressions.push_back(expression(tokens, 0, data));
			}
		}
	} else if (operation == "!") {
		tokens.increment(false);
		tokens.expect("?");

		tokens.increment(false);
		tokens.expect<expression>();

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			expressions.push_back(expression(tokens, 0, data));
		}

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			operation += tokens.next();

			tokens.increment(false);
			tokens.expect<variable_name>();

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				names.push_back(variable_name(tokens, data));
			}
		}
	} else if (operation == "(") {
		tokens.increment(true);
		tokens.expect(")");

		tokens.increment(false);
		tokens.expect<expression>();

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			expressions.push_back(expression(tokens, 0, data));

			tokens.increment(false);
			tokens.expect(",");

			while (tokens.decrement(__FILE__, __LINE__, data)) {
				tokens.next();

				tokens.increment(false);
				tokens.expect(",");

				tokens.increment(true);
				tokens.expect<expression>();

				if (tokens.decrement(__FILE__, __LINE__, data)) {
					expressions.push_back(expression(tokens, 0, data));
				}
			}
		}

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			tokens.next();
		}
	}

	tokens.syntax_end(this);
}

bool assignment::is_next(tokenizer &tokens, int i, void *data)
{
	return not tokens.is_next("while") and not tokens.is_next("await") and not tokens.is_next("region") and not tokens.is_next("assume") and variable_name::is_next(tokens, i, data);
}

void assignment::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<assignment>())
	{
		tokens.register_syntax<assignment>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::number>();
		tokens.register_token<parse::instance>();
		tokens.register_token<parse::white_space>(false);
		variable_name::register_syntax(tokens);
		expression::register_syntax(tokens);
	}
}

string assignment::to_string(string tab) const
{
	if (!valid || (names.size() == 0 && expressions.size() == 0))
		return "skip";

	string result = "";

	if (names.size() == 0)
		result += expressions[0].to_string(tab);
	else
	{
		result += names[0].to_string(tab);

		if (operation == "+" || operation == "-" || operation == "~") {
			result += operation;
		} else if (operation == "=" && expressions.size() > 0) {
			result += "=" + expressions[0].to_string(tab);
		} else if (operation == "=" && expressions.size() == 0) {
			return "skip";
		} else if (operation == "?" || operation == "?!") {
			result += "?";
			if (names.size() > 1) {
				result += names[1].to_string(tab);
			}

			if (operation == "?!") {
				result += "!";
				if (expressions.size() > 0)
					result += expressions[0].to_string(tab);
			}
		} else if (operation == "!" || operation == "!?") {
			result += "!";
			if (expressions.size() > 0) {
				result += expressions[0].to_string(tab);
			}

			if (operation == "!?") {
				result += "?";
				if (names.size() > 1)
					result += names[1].to_string(tab);
			}
		} else if (operation == "(") {
			result += "(";
			for (auto i = expressions.begin(); i != expressions.end(); i++) {
				if (i != expressions.begin()) {
					result += ", ";
				}
				result += i->to_string(tab);
			}
			result += ")";
		} else if (operation == "~") {
			result += "~";
		}
	}

	return result;
}

parse::syntax *assignment::clone() const
{
	return new assignment(*this);
}

}
