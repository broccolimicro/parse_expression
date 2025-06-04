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

namespace parse_expression {

assignment::assignment() {
	debug_name = "assignment";
}

assignment::assignment(tokenizer &tokens, void *data) {
	debug_name = "assignment";
	parse(tokens, data);
}

assignment::~assignment() {
}

void assignment::parse(tokenizer &tokens, void *data) {
	tokens.syntax_start(this);

	tokens.increment(false);
	tokens.expect("=");
	tokens.expect("+");
	tokens.expect("-");
	tokens.expect("~");

	tokens.increment(false);
	tokens.expect(",");

	tokens.increment(true);
	tokens.expect<expression>();

	if (tokens.decrement(__FILE__, __LINE__, data)) {
		lvalue.push_back(expression(tokens, 12, data));
	}

	while (tokens.decrement(__FILE__, __LINE__, data)) {
		tokens.next();

		tokens.increment(false);
		tokens.expect(",");

		tokens.increment(true);
		tokens.expect<expression>();

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			lvalue.push_back(expression(tokens, 12, data));
		}
	}

	if (tokens.decrement(__FILE__, __LINE__, data)) {
		operation = tokens.next();
	}

	if (operation == "=") {
		tokens.increment(true);
		tokens.expect<expression>();

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			rvalue = expression(tokens, 0, data);
		}
	} 

	tokens.syntax_end(this);
}

bool assignment::is_next(tokenizer &tokens, int i, void *data) {
	return expression::is_next(tokens, i, data);
}

void assignment::register_syntax(tokenizer &tokens) {
	if (!tokens.syntax_registered<assignment>()) {
		tokens.register_syntax<assignment>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::number>();
		tokens.register_token<parse::instance>();
		tokens.register_token<parse::white_space>(false);
		expression::register_syntax(tokens);
	}
}

string assignment::to_string(string tab) const {
	if (not valid or (lvalue.empty() and not rvalue.valid))
		return "skip";

	string result = "";

	if (not lvalue.empty()) {
		for (int i = 0; i < (int)lvalue.size(); i++) {
			if (i != 0) {
				result += ",";
			}
			result += lvalue[i].to_string(tab);
		}

		result += operation;
	}

	if (lvalue.empty() or operation == "=") {	
		result += rvalue.to_string(tab);
	}

	return result;
}

parse::syntax *assignment::clone() const
{
	return new assignment(*this);
}

}
