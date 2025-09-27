#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>
#include <parse/default/symbol.h>
#include <parse/default/number.h>
#include <parse/default/white_space.h>
#include <parse/default/instance.h>
#include <parse/default/new_line.h>

#include "precedence.h"

namespace parse_expression {

template <int group=0, typename number_t=parse::number, typename instance_t=parse::instance>
struct argument_t;

template <int group=0, typename number_t=parse::number, typename instance_t=parse::instance>
struct expression_t : parse::syntax {
	using argument=parse_expression::argument_t<group, number_t, instance_t>;
	using expression=parse_expression::expression_t<group, number_t, instance_t>;

	vector<argument> arguments;
	vector<int> operators;

	int level;

	static precedence_set precedence;

	expression_t() {
		debug_name = "expression_" + ::to_string(group);
		level = 0;
	}

	expression_t(tokenizer &tokens, int level=0, void *data=nullptr) {
		debug_name = "expression_" + ::to_string(group);
		this->level = level;
		parse(tokens, data);
	}

	~expression_t() {}

	static void register_precedence(const precedence_set &p) {
		expression::precedence = p;
	}

	void expectLiteral(tokenizer &tokens, int next) {
		if (precedence.isValidLevel(next < 0 ? level+1 : next)) {
			tokens.expect<expression>();
		} else {
			tokens.expect<instance_t>();
			tokens.expect<number_t>();
			tokens.expect("false");
			tokens.expect("true");
			tokens.expect("gnd");
			tokens.expect("vdd");
			tokens.expect("(");
		}
	}

	void readLiteral(tokenizer &tokens, int next, void *data=nullptr) {
		if (tokens.found<expression>()) {
			arguments.push_back(argument(expression(tokens, (next < 0 ? level+1 : next), data)));
		} else if (tokens.found<instance_t>()) {
			arguments.push_back(argument::literalOf(tokens.next()));
		} else if (tokens.found<number_t>()) {
			arguments.push_back(argument::constantOf(tokens.next()));
		} else if (tokens.found("false") or tokens.found("true") or tokens.found("vdd") or tokens.found("gnd")) {
			arguments.push_back(argument::constantOf(tokens.next()));
		} else if (tokens.found("(")) {
			tokens.next();

			tokens.increment(true);
			tokens.expect(")");

			tokens.increment(true);
			tokens.expect<expression>();

			if (tokens.decrement(__FILE__, __LINE__, NULL)) {
				arguments.push_back(argument(expression(tokens, 0, data)));
			}

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				tokens.next();
			}
		}
	}

	void parse(tokenizer &tokens, void *data=nullptr) {
		tokens.syntax_start(this);

		if (precedence.operations.empty()) {
			tokens.internal("operator precedence not initialized", __FILE__, __LINE__);
		} else if (not precedence.isValidLevel(level)) {
			tokens.internal("invalid expression level", __FILE__, __LINE__);
		} else if (precedence.isTernary(level)) {
			tokens.increment(false);
			for (int i = 0; i < (int)precedence.at(level).size(); i++) {
				tokens.expect(precedence.at(level, i).trigger);
			}

			tokens.increment(true);
			expectLiteral(tokens, -1);

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				readLiteral(tokens, -1, data);
			}

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				string tok = tokens.next();
				vector<int> match;
				for (int i = 0; i < (int)precedence.at(level).size(); i++) {
					if (precedence.at(level, i).trigger == tok) {
						match.push_back(i);
					}
				}

				tokens.increment(true);
				expectLiteral(tokens, -1);

				tokens.increment(true);
				for (int i = 0; i < (int)match.size(); i++) {
					tokens.expect(precedence.at(level, match[i]).infix);
				}

				tokens.increment(true);
				expectLiteral(tokens, -1);
				
				if (tokens.decrement(__FILE__, __LINE__, data)) {
					readLiteral(tokens, -1, data);
				}

				if (tokens.decrement(__FILE__, __LINE__, data)) {
					string tok = tokens.next();
					for (int i = (int)match.size()-1; i >= 0; i--) {
						if (precedence.at(level, match[i]).infix != tok) {
							match.erase(match.begin()+i);
						}
					}
				
					if (match.size() != 1u) {
						tokens.internal("ambiguous ternary operators", __FILE__, __LINE__);
						if (match.empty()) {
							return;
						}
					}

					operators.push_back(match[0]);
				}

				if (tokens.decrement(__FILE__, __LINE__, data)) {
					readLiteral(tokens, -1, data);
				}
			}
		} else if (precedence.isBinary(level)) {
			tokens.increment(false);
			for (int i = 0; i < (int)precedence.at(level).size(); i++) {
				tokens.expect(precedence.at(level, i).infix);
			}

			tokens.increment(true);
			expectLiteral(tokens, -1);

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				readLiteral(tokens, -1, data);
			}

			while (tokens.decrement(__FILE__, __LINE__, data)) {
				string tok = tokens.next();
				vector<int> match;
				for (int i = 0; i < (int)precedence.at(level).size(); i++) {
					if (precedence.at(level, i).infix == tok) {
						match.push_back(i);
					}
				}

				if (match.size() != 1u) {
					tokens.internal("ambiguous binary operators " + ::to_string(level) + ":" + ::to_string(match), __FILE__, __LINE__);
					if (match.empty()) {
						return;
					}
				}

				operators.push_back(match[0]);

				tokens.increment(false);
				for (int i = 0; i < (int)precedence.at(level).size(); i++) {
					tokens.expect(precedence.at(level, i).infix);
				}

				tokens.increment(true);
				expectLiteral(tokens, -1);

				if (tokens.decrement(__FILE__, __LINE__, data)) {
					readLiteral(tokens, -1, data);
				}
			}
		} else if (precedence.isUnary(level)) {
			tokens.increment(true);
			expectLiteral(tokens, -1);

			bool hasPrefix = false;
			for (int i = 0; i < (int)precedence.at(level).size(); i++) {
				if (not precedence.at(level, i).prefix.empty()) {
					if (not hasPrefix) {
						tokens.increment(false);
						hasPrefix = true;
					}
					tokens.expect(precedence.at(level, i).prefix);
				}
			}

			while (hasPrefix and tokens.decrement(__FILE__, __LINE__, data)) {
				string tok = tokens.next();
				vector<int> match;
				for (int i = 0; i < (int)precedence.at(level).size(); i++) {
					if (precedence.at(level, i).prefix == tok) {
						match.push_back(i);
					}
				}

				if (match.size() != 1u) {
					tokens.internal("ambiguous unary operators", __FILE__, __LINE__);
					if (match.empty()) {
						return;
					}
				}

				operators.push_back(match[0]);

				tokens.increment(false);
				for (auto i = 0; i < (int)precedence.at(level).size(); i++) {
					if (not precedence.at(level, i).prefix.empty()) {
						tokens.expect(precedence.at(level, i).prefix);
					}
				}
			}

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				readLiteral(tokens, -1, data);
			}

			bool hasPostfix = false;
			for (int i = 0; i < (int)precedence.at(level).size(); i++) {
				if (not precedence.at(level, i).postfix.empty()) {
					if (not hasPostfix) {
						tokens.increment(false);
						hasPostfix = true;
					}
					tokens.expect(precedence.at(level, i).postfix);
				}
			}

			while (hasPostfix and tokens.decrement(__FILE__, __LINE__, data)) {
				string tok = tokens.next();
				vector<int> match;
				for (int i = 0; i < (int)precedence.at(level).size(); i++) {
					if (precedence.at(level, i).postfix == tok) {
						match.push_back(i);
					}
				}

				if (match.size() != 1u) {
					tokens.internal("ambiguous unary operators", __FILE__, __LINE__);
					if (match.empty()) {
						return;
					}
				}

				operators.push_back(match[0]);

				tokens.increment(false);
				for (auto i = 0; i < (int)precedence.at(level).size(); i++) {
					if (not precedence.at(level, i).postfix.empty()) {
						tokens.expect(precedence.at(level, i).postfix);
					}
				}
			}
		} else if (precedence.isModifier(level)) {
			tokens.increment(false);
			for (int i = 0; i < (int)precedence.at(level).size(); i++) {
				tokens.expect(precedence.at(level, i).trigger);
			}

			tokens.increment(true);
			expectLiteral(tokens, -1);

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				readLiteral(tokens, -1, data);
			}

			bool first = true;
			while (tokens.decrement(__FILE__, __LINE__, data)) {
				if (not first) {
					expression sub = *this;
					sub.valid = true;
					arguments.clear();
					operators.clear();
					arguments.push_back(sub);
				}
				first = false;

				string tok = tokens.next();
				vector<int> match;
				for (int i = 0; i < (int)precedence.at(level).size(); i++) {
					if (precedence.at(level, i).trigger == tok) {
						match.push_back(i);
					}
				}

				if (match.size() != 1u) {
					tokens.internal("ambiguous modifier operators", __FILE__, __LINE__);
					if (match.empty()) {
						return;
					}
				}

				operators.push_back(match[0]);

				if (not precedence.at(level, match[0]).postfix.empty()) {
					tokens.increment(true);
					for (int i = 0; i < (int)precedence.at(level).size(); i++) {
						if (not precedence.at(level, i).postfix.empty()) {
							tokens.expect(precedence.at(level, i).postfix);
						}
					}
				}

				if (not precedence.at(level, match[0]).infix.empty()) {
					tokens.increment(false);
					expectLiteral(tokens, 0);

					if (tokens.decrement(__FILE__, __LINE__, data)) {
						readLiteral(tokens, 0, data);

						tokens.increment(false);
						tokens.expect(precedence.at(level, match[0]).infix);

						while (tokens.decrement(__FILE__, __LINE__, data)) {
							tokens.next();

							tokens.increment(false);
							tokens.expect(precedence.at(level, match[0]).infix);

							tokens.increment(true);
							expectLiteral(tokens, 0);

							if (tokens.decrement(__FILE__, __LINE__, data)) {
								readLiteral(tokens, 0, data);
							}
						}
					}
				} else {
					tokens.increment(true);
					expectLiteral(tokens, -1);

					if (tokens.decrement(__FILE__, __LINE__, data)) {
						readLiteral(tokens, -1, data);
					}
				}

				if (not precedence.at(level, match[0]).postfix.empty() and tokens.decrement(__FILE__, __LINE__, data)) {
					tokens.next();
				}

				tokens.increment(false);
				for (int i = 0; i < (int)precedence.at(level).size(); i++) {
					tokens.expect(precedence.at(level, i).trigger);
				}
			}
		} else if (precedence.isGroup(level)) {
			tokens.increment(false);
			for (int i = 0; i < (int)precedence.at(level).size(); i++) {
				tokens.expect(precedence.at(level, i).prefix);
			}

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				string tok = tokens.next();
				vector<int> match;
				for (int i = 0; i < (int)precedence.at(level).size(); i++) {
					if (precedence.at(level, i).prefix == tok) {
						match.push_back(i);
					}
				}

				if (match.size() != 1u) {
					tokens.internal("ambiguous group operators", __FILE__, __LINE__);
					if (match.empty()) {
						return;
					}
				}

				operators.push_back(match[0]);
		
				tokens.increment(true);
				tokens.expect(precedence.at(level, match[0]).postfix);
		
				tokens.increment(false);
				expectLiteral(tokens, 0);

				if (tokens.decrement(__FILE__, __LINE__, data)) {
					readLiteral(tokens, 0, data);

					tokens.increment(false);
					tokens.expect(precedence.at(level, match[0]).infix);

					while (tokens.decrement(__FILE__, __LINE__, data)) {
						tokens.next();

						tokens.increment(false);
						tokens.expect(precedence.at(level, match[0]).infix);

						tokens.increment(true);
						expectLiteral(tokens, 0);

						if (tokens.decrement(__FILE__, __LINE__, data)) {
							readLiteral(tokens, 0, data);
						}
					}
				}

				if (tokens.decrement(__FILE__, __LINE__, data)) {
					tokens.next();
				}
			}	else {
				tokens.increment(true);
				expectLiteral(tokens, -1);

				if (tokens.decrement(__FILE__, __LINE__, data)) {
					readLiteral(tokens, -1, data);
				}
			}
		}

		tokens.syntax_end(this);
	}

	static bool is_next(tokenizer &tokens, int i=1, void *data=nullptr) {
		if (tokens.is_next("func", i)
			or tokens.is_next("struct", i)
			or tokens.is_next("interface", i)
			or tokens.is_next("context", i)
			or tokens.is_next("await", i) 
			or tokens.is_next("if", i) 
			or tokens.is_next("while", i)
			or tokens.is_next("region", i)
			or tokens.is_next("assume", i)
			or tokens.is_next("var", i)) {
			return false;
		}

		bool result = (tokens.is_next("(", i)
			or tokens.is_next<number_t>(i)
			or tokens.is_next<instance_t>(i));

		int level = -1;
		if (data != nullptr) {
			level = *(int*)data;
		}

		for (int j = level+1; j < (int)precedence.size(); j++) {
			for (int k = 0; k < (int)precedence.at(j).size(); k++) {
				if (not precedence.at(j, k).prefix.empty()) {
					result = result or tokens.is_next(precedence.at(j, k).prefix, i);
				}
			}
		}

		return result;
	}

	static void register_syntax(tokenizer &tokens) {
		if (!tokens.syntax_registered<expression>()) {
			tokens.register_syntax<expression>();
			tokens.register_token<parse::symbol>();
			tokens.register_token<number_t>();
			tokens.register_token<instance_t>();
			tokens.register_token<parse::white_space>(false);
		}
	}

	string to_string(string tab="") const {
		return to_string(-1, tab);
	}

	string to_string(int prev_level, string tab="") const {
		if (!valid or arguments.size() == 0)
			return "gnd";

		string result = "";
		bool paren = prev_level > level and not precedence.isGroup(prev_level);
		if (paren) {
			result += "(";
		}

		if (level < 0 or arguments.empty()) {
			result += "???";
		} else if (operators.empty()) {
			result += arguments[0].to_string(level, tab);
		} else if (precedence.isTernary(level)) {
			result += arguments[0].to_string(level, tab)
				+ precedence.at(level, operators[0]).trigger + arguments[1].to_string(level, tab)
				+ precedence.at(level, operators[0]).infix + arguments[2].to_string(level, tab);
		} else if (precedence.isBinary(level)) {
			for (int i = 0; i < (int)arguments.size() and i-1 < (int)operators.size(); i++) {
				if (i != 0) {
					result += precedence.at(level, operators[i-1]).infix;
				}

				result += arguments[i].to_string(level, tab);
			}
		} else if (precedence.isUnary(level)) {
			for (int i = 0; i < (int)operators.size(); i++) {
				result += precedence.at(level, operators[i]).prefix;
			}

			result += arguments[0].to_string(level, tab);

			for (int i = 0; i < (int)operators.size(); i++) {
				result += precedence.at(level, operators[i]).postfix;
			}
		} else if (precedence.isModifier(level)) {
			result += arguments[0].to_string(level, tab) + precedence.at(level, operators[0]).trigger;
			for (int i = 1; i < (int)arguments.size(); i++) {
				if (i != 1) {
					result += precedence.at(level, operators[0]).infix;
				}
				result += arguments[i].to_string(-1, tab);
			}
			result += precedence.at(level, operators[0]).postfix;
		} else if (precedence.isGroup(level)) {
			result += precedence.at(level, operators[0]).prefix;
			for (int i = 0; i < (int)arguments.size(); i++) {
				if (i != 0) {
					result += precedence.at(level, operators[0]).infix;
				}

				result += arguments[i].to_string(level, tab);
			}
			result += precedence.at(level, operators[0]).postfix;
		} else {
			result += "???";
		}

		if (paren)
			result += ")";

		return result;
	}

	parse::syntax *clone() const {
		return new expression(*this);
	}
};

template <int group, typename number_t, typename instance_t>
struct argument_t {
	using expression=parse_expression::expression_t<group, number_t, instance_t>;
	using argument=parse_expression::argument_t<group, number_t, instance_t>;

	expression sub;
	string literal;
	string constant;

	argument_t() {}
	argument_t(expression sub) : sub(sub) {}
	~argument_t() {}

	static argument literalOf(string str) {
		argument result;
		result.literal = str;
		return result;
	}

	static argument constantOf(string str) {
		argument result;
		result.constant = str;
		return result;
	}

	string to_string(int level=-1, string tab="") const {
		if (sub.valid) {
			return sub.to_string(level, tab);
		} else if (not literal.empty()) {
			return literal;
		} else if (not constant.empty()) {
			return constant;
		}
		return "vdd";
	}
};

template <int group, typename number_t, typename instance_t>
precedence_set expression_t<group, number_t, instance_t>::precedence;

}

