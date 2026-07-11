#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>
#include <parse/default/symbol.h>
#include <parse/default/number.h>
#include <parse/default/white_space.h>

#include "assignment.h"

namespace parse_expression {

template <int group=0, typename number_t=parse::number, typename instance_t=parse::instance>
struct composition_t : parse::syntax {
	using assignment = parse_expression::assignment_t<group, number_t, instance_t>;
	using expression = parse_expression::expression_t<group, number_t, instance_t>;
	using composition = parse_expression::composition_t<group, number_t, instance_t>;

	vector<composition> compositions;
	vector<assignment> literals;
	vector<expression> guards;
	int level;

	static vector<string> precedence;

	composition_t() {
		debug_name = "composition_" + ::to_string(group);
		level = 0;
		init();
	}

	composition_t(tokenizer &tokens, int level=0, void *data=nullptr) {
		debug_name = "composition_" + ::to_string(group);
		this->level = level;
		init();
		parse(tokens, data);
	}

	~composition_t() {
	}

	static void init() {
		if (precedence.size() == 0)
		{
			precedence.push_back(":");
			precedence.push_back(",");
		}
	}

	static int get_level(string operation) {
		for (int i = 0; i < (int)precedence.size(); i++)
			if (precedence[i] == operation)
				return i;

		return -1;
	}

	void parse(tokenizer &tokens, void *data=nullptr) {
		tokens.syntax_start(this);

		bool first = true;
		do {
			if (first) {
				first = false;
			} else {
				tokens.next();
			}

			tokens.increment(false);
			tokens.expect(precedence[level]);

			if (level < (int)precedence.size()-1) {
				tokens.increment(true);
				tokens.expect<composition>();

				if (tokens.decrement(__FILE__, __LINE__, data)) {
					compositions.push_back(composition(tokens, level+1, data));
				}
			} else {
				tokens.increment(true);
				tokens.expect("(");
				tokens.expect<assignment>();
				tokens.expect("[");

				if (tokens.decrement(__FILE__, __LINE__, data)) {
					if (tokens.found("(")) {
						tokens.next();

						tokens.increment(true);
						tokens.expect(")");

						tokens.increment(true);
						tokens.expect<composition>();

						if (tokens.decrement(__FILE__, __LINE__, data)) {
							compositions.push_back(composition(tokens, 0, data));
						}

						if (tokens.decrement(__FILE__, __LINE__, data)) {
							tokens.next();
						}
					} else if (tokens.found<assignment>()) {
						literals.push_back(assignment(tokens, data));
					} else if (tokens.found("[")) {
						tokens.next();

						tokens.increment(true);
						tokens.expect("]");

						tokens.increment(true);
						tokens.expect<expression>();

						if (tokens.decrement(__FILE__, __LINE__, data)) {
							guards.push_back(expression(tokens, 0, data));
						}

						if (tokens.decrement(__FILE__, __LINE__, data)) {
							tokens.next();
						}
					}
				}
			}
		} while (tokens.decrement(__FILE__, __LINE__, data));

		tokens.syntax_end(this);
	}

	static bool is_next(tokenizer &tokens, int i=1, void *data=nullptr) {
		return tokens.is_next("(", i) or tokens.is_next("[", i) or assignment::is_next(tokens, i, data);
	}

	static void register_syntax(tokenizer &tokens) {
		if (!tokens.syntax_registered<composition>()) {
			tokens.register_syntax<composition>();
			tokens.register_token<parse::symbol>();
			tokens.register_token<parse::number>();
			tokens.register_token<parse::white_space>(false);
			assignment::register_syntax(tokens);
			expression::register_syntax(tokens);
		}
	}

	string to_string(string tab="") const {
		return to_string(-1, tab);
	}

	string to_string(int prev_level, string tab="") const {
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

	parse::syntax *clone() const {
		return new composition(*this);
	}
};

template <int group, typename number_t, typename instance_t>
vector<string> composition_t<group, number_t, instance_t>::precedence;

}
