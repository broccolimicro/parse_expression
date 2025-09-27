#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>
#include <parse/default/symbol.h>
#include <parse/default/number.h>
#include <parse/default/white_space.h>
#include <parse/default/instance.h>
#include <parse/default/new_line.h>

#include "expression.h"

namespace parse_expression {

template <int group=0, typename number_t=parse::number, typename instance_t=parse::instance>
struct assignment_t : parse::syntax {
	using expression = parse_expression::expression_t<group, number_t, instance_t>;
	using assignment = parse_expression::assignment_t<group, number_t, instance_t>;

	static int lvalueLevel;

	vector<expression> lvalue;
	expression rvalue;
	string operation;
	string region;

	assignment_t() {
		debug_name = "assignment";
	}

	assignment_t(tokenizer &tokens, void *data=nullptr) {
		debug_name = "assignment";
		parse(tokens, data);
	}

	~assignment_t() {
	}

	void parse(tokenizer &tokens, void *data=nullptr) {
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
			lvalue.push_back(expression(tokens, lvalueLevel, data));
		}

		while (tokens.decrement(__FILE__, __LINE__, data)) {
			tokens.next();

			tokens.increment(false);
			tokens.expect(",");

			tokens.increment(true);
			tokens.expect<expression>();

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				lvalue.push_back(expression(tokens, lvalueLevel, data));
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

	static bool is_next(tokenizer &tokens, int i=1, void *data=nullptr) {
		return expression::is_next(tokens, i, data);
	}

	static void register_syntax(tokenizer &tokens) {
		if (!tokens.syntax_registered<assignment>()) {
			tokens.register_syntax<assignment>();
			tokens.register_token<parse::symbol>();
			tokens.register_token<parse::number>();
			tokens.register_token<parse::instance>();
			tokens.register_token<parse::white_space>(false);
			expression::register_syntax(tokens);
		}
	}

	string to_string(string tab="") const {
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

	parse::syntax *clone() const {
		return new assignment(*this);
	}
};

template <int group, typename number_t, typename instance_t>
int assignment_t<group, number_t, instance_t>::lvalueLevel = 0;

}
