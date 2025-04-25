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
#include <parse/default/new_line.h>

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

		expression expr;
		int level = 0;
		for (level = 0; expression::precedence[level].type != operation_set::call; level++);
		if (level < (int)expression::precedence.size()) {
			expr.valid = true;
			expr.level = level;
			expr.arguments.push_back(argument(names[0]));
			expr.operations.push_back(expression::precedence[level].symbols[0]);
			for (int i = 0; i < (int)expressions.size(); i++) {
				if (i != 0) {
					expr.operations.push_back(expression::precedence[level].symbols[1]);
				}
				expr.arguments.push_back(argument(expressions[i]));
			}
			expr.operations.push_back(expression::precedence[level].symbols[2]);
			names.clear();
			expressions.clear();
			expressions.push_back(expr);
			operation = "=";
		}
	}

	tokens.syntax_end(this);
}

bool assignment::is_next(tokenizer &tokens, int i, void *data)
{
	return variable_name::is_next(tokens, i, data)
		and not tokens.is_next<parse::instance>(i+1)
		and not tokens.is_next("func", i)
		and not tokens.is_next("struct", i)
		and not tokens.is_next("interface", i)
		and not tokens.is_next("context", i)
		and not tokens.is_next("await", i) 
		and not tokens.is_next("while", i)
		and not tokens.is_next("region", i)
		and not tokens.is_next("assume", i);
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
