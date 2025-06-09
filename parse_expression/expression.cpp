/*
 * expression.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "expression.h"
#include <parse/default/symbol.h>
#include <parse/default/number.h>
#include <parse/default/instance.h>
#include <parse/default/white_space.h>

namespace parse_expression
{

vector<operation_set> expression::precedence;

operation::operation() {
}

operation::operation(string prefix, string trigger, string infix, string postfix) {
	this->prefix = prefix;
	this->trigger = trigger;
	this->infix = infix;
	this->postfix = postfix;
}

operation::~operation() {
}

bool operation::is(string prefix, string trigger, string infix, string postfix) const {
	return this->prefix == prefix and this->trigger == trigger and this->infix == infix and this->postfix == postfix;
}

bool operator==(operation o0, operation o1) {
	if (o0.infix.size() != o1.infix.size()
		or o0.prefix != o1.prefix
		or o0.postfix != o1.postfix) {
		return false;
	}

	for (int i = 0; i < (int)o0.infix.size(); i++) {
		if (o0.infix[i] != o1.infix[i]) {
			return false;
		}
	}
	return true;
}

bool operator!=(operation o0, operation o1) {
	return not (o0 == o1);
}

operation_set::operation_set() {
	type = -1;
}

operation_set::operation_set(int type) {
	this->type = type;
}

operation_set::~operation_set() {
}

void operation_set::push(string prefix, string trigger, string infix, string postfix) {
	push(operation(prefix, trigger, infix, postfix));
}

void operation_set::push(operation op) {
	symbols.push_back(op);
}

int operation_set::find(operation op) {
	for (int i = 0; i < (int)symbols.size(); i++) {
		if (symbols[i] == op) {
			return i;
		}
	}
	return -1;
}

expression::expression()
{
	debug_name = "expression";
	level = 0;
	init();
}

expression::expression(tokenizer &tokens, int level, void *data)
{
	debug_name = "expression";
	this->level = level;
	init();
	parse(tokens, data);
}

expression::~expression()
{
}

void expression::init() {
	if (precedence.size() == 0) {	
		// 0
		precedence.push_back(operation_set(operation_set::TERNARY));
		precedence.back().push("", "?", ":", "");

		// 1
		precedence.push_back(operation_set(operation_set::BINARY));
		precedence.back().push("", "", "|", "");

		// 2
		precedence.push_back(operation_set(operation_set::BINARY));
		precedence.back().push("", "", "&", "");

		// 3
		precedence.push_back(operation_set(operation_set::BINARY));
		precedence.back().push("", "", "^", "");

		// 4
		precedence.push_back(operation_set(operation_set::BINARY));
		precedence.back().push("", "", "==", "");
		precedence.back().push("", "", "~=", "");
		precedence.back().push("", "", "<", "");
		precedence.back().push("", "", ">", "");
		precedence.back().push("", "", "<=", "");
		precedence.back().push("", "", ">=", "");

		// 5
		precedence.push_back(operation_set(operation_set::BINARY));
		precedence.back().push("", "", "||", "");
		
		// 6
		precedence.push_back(operation_set(operation_set::BINARY));
		precedence.back().push("", "", "&&", "");

		// 7
		precedence.push_back(operation_set(operation_set::BINARY));
		precedence.back().push("", "", "^^", "");

		// 8
		precedence.push_back(operation_set(operation_set::BINARY));
		precedence.back().push("", "", "<<", "");
		precedence.back().push("", "", ">>", "");

		// 9
		precedence.push_back(operation_set(operation_set::BINARY));
		precedence.back().push("", "", "+", "");
		precedence.back().push("", "", "-", "");

		// 10
		precedence.push_back(operation_set(operation_set::BINARY));
		precedence.back().push("", "", "*", "");
		precedence.back().push("", "", "/", "");
		precedence.back().push("", "", "%", "");

		// 11
		precedence.push_back(operation_set(operation_set::UNARY));
		precedence.back().push("!", "", "", "");
		precedence.back().push("~", "", "", "");
		precedence.back().push("(bool)", "", "", "");
		precedence.back().push("+", "", "", "");
		precedence.back().push("-", "", "", "");
		precedence.back().push("?", "", "", "");

		// 12
		precedence.push_back(operation_set(operation_set::BINARY));
		precedence.back().push("", "", "'", "");

		// 13
		precedence.push_back(operation_set(operation_set::BINARY));
		precedence.back().push("", "", "::", "");

		// 14
		precedence.push_back(operation_set(operation_set::BINARY));
		precedence.back().push("", "", ".", "");

		// 15
		precedence.push_back(operation_set(operation_set::MODIFIER));
		precedence.back().push("", "(", ",", ")");
		precedence.back().push("", "[", ":", "]");

		// 16	
		precedence.push_back(operation_set(operation_set::GROUP));
		precedence.back().push("[", "", ",", "]");
	}
}

bool expression::isTernary() const {
	return precedence[level].type == operation_set::TERNARY;
}

bool expression::isBinary() const {
	return precedence[level].type == operation_set::BINARY;
}

bool expression::isUnary() const {
	return precedence[level].type == operation_set::UNARY;
}

bool expression::isModifier() const {
	return precedence[level].type == operation_set::MODIFIER;
}

bool expression::isGroup() const {
	return precedence[level].type == operation_set::GROUP;
}

pair<int, int> expression::find(int type, string prefix, string trigger, string infix, string postfix) {
	operation search(prefix, trigger, infix, postfix);
	for (int i = 0; i < (int)precedence.size(); i++) {
		if (type == precedence[i].type) {
			int j = precedence[i].find(search);
			if (j >= 0) {
				return {i, j};
			}
		}
	}

	return {-1, -1};
}

const vector<operation> &expression::symbols() const {
	return precedence[level].symbols;
}

const operation &expression::symbol(int i) const {
	return precedence[level].symbols[i];
}

void expression::expectLiteral(tokenizer &tokens) {
	if (level < (int)precedence.size()-1) {
		tokens.expect<expression>();
	} else {
		tokens.expect<parse::instance>();
		tokens.expect<parse::number>();
		tokens.expect("false");
		tokens.expect("true");
		tokens.expect("(");
	}
}

void expression::readLiteral(tokenizer &tokens, int next, void *data) {
	if (tokens.found<expression>()) {
		arguments.push_back(argument(expression(tokens, (next < 0 ? level+1 : next), data)));
	} else if (tokens.found<parse::instance>()) {
		arguments.push_back(argument::literalOf(tokens.next()));
	} else if (tokens.found<parse::number>()) {
		arguments.push_back(argument::constantOf(tokens.next()));
	} else if (tokens.found("false") or tokens.found("true")) {
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

void expression::parse(tokenizer &tokens, void *data) {
	tokens.syntax_start(this);

	if (precedence[level].type == operation_set::TERNARY) {
		tokens.increment(false);
		for (int i = 0; i < (int)symbols().size(); i++) {
			tokens.expect(symbol(i).trigger);
		}

		tokens.increment(true);
		expectLiteral(tokens);

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			readLiteral(tokens, -1, data);
		}

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)symbols().size(); i++) {
				if (symbol(i).trigger == tok) {
					match.push_back(i);
				}
			}

			tokens.increment(true);
			expectLiteral(tokens);

			tokens.increment(true);
			for (int i = 0; i < (int)match.size(); i++) {
				tokens.expect(symbol(match[i]).infix);
			}

			tokens.increment(true);
			expectLiteral(tokens);
			
			if (tokens.decrement(__FILE__, __LINE__, data)) {
				readLiteral(tokens, -1, data);
			}

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				string tok = tokens.next();
				for (int i = (int)match.size()-1; i >= 0; i--) {
					if (symbol(match[i]).infix != tok) {
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
	} else if (precedence[level].type == operation_set::BINARY) {
		tokens.increment(false);
		for (int i = 0; i < (int)symbols().size(); i++) {
			tokens.expect(symbol(i).infix);
		}

		tokens.increment(true);
		expectLiteral(tokens);

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			readLiteral(tokens, -1, data);
		}

		while (tokens.decrement(__FILE__, __LINE__, data)) {
			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)symbols().size(); i++) {
				if (symbol(i).infix == tok) {
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
			for (int i = 0; i < (int)symbols().size(); i++) {
				tokens.expect(symbol(i).infix);
			}

			tokens.increment(true);
			expectLiteral(tokens);

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				readLiteral(tokens, -1, data);
			}
		}
	} else if (precedence[level].type == operation_set::UNARY) {
		tokens.increment(true);
		expectLiteral(tokens);

		tokens.increment(false);
		for (int i = 0; i < (int)symbols().size(); i++) {
			tokens.expect(symbol(i).prefix);
		}

		while (tokens.decrement(__FILE__, __LINE__, data)) {
			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)symbols().size(); i++) {
				if (symbol(i).prefix == tok) {
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
			for (auto i = 0; i < (int)symbols().size(); i++) {
				tokens.expect(symbol(i).prefix);
			}
		}

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			readLiteral(tokens, -1, data);
		}
	} else if (precedence[level].type == operation_set::MODIFIER) {
		tokens.increment(false);
		for (int i = 0; i < (int)symbols().size(); i++) {
			tokens.expect(symbol(i).trigger);
		}

		tokens.increment(true);
		expectLiteral(tokens);

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
			for (int i = 0; i < (int)symbols().size(); i++) {
				if (symbol(i).trigger == tok) {
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

			if (not symbol(match[0]).postfix.empty()) {
				tokens.increment(true);
				for (int i = 0; i < (int)symbols().size(); i++) {
					if (not symbol(i).postfix.empty()) {
						tokens.expect(symbol(i).postfix);
					}
				}
			}

			if (not symbol(match[0]).infix.empty()) {
				tokens.increment(false);
				tokens.expect(symbol(match[0]).infix);

				tokens.increment(true);
				expectLiteral(tokens);

				if (tokens.decrement(__FILE__, __LINE__, data)) {
					readLiteral(tokens, 0, data);
				}

				while (tokens.decrement(__FILE__, __LINE__, data)) {
					tokens.next();

					tokens.increment(false);
					tokens.expect(symbol(match[0]).infix);

					tokens.increment(true);
					expectLiteral(tokens);

					if (tokens.decrement(__FILE__, __LINE__, data)) {
						readLiteral(tokens, 0, data);
					}
				}
			} else {
				tokens.increment(true);
				expectLiteral(tokens);

				if (tokens.decrement(__FILE__, __LINE__, data)) {
					readLiteral(tokens, 0, data);
				}
			}

			if (not symbol(match[0]).postfix.empty() and tokens.decrement(__FILE__, __LINE__, data)) {
				tokens.next();
			}

			tokens.increment(false);
			for (int i = 0; i < (int)symbols().size(); i++) {
				tokens.expect(symbol(i).trigger);
			}
		}
	} else if (precedence[level].type == operation_set::GROUP) {
		tokens.increment(false);
		for (int i = 0; i < (int)symbols().size(); i++) {
			tokens.expect(symbol(i).prefix);
		}

		if (tokens.decrement(__FILE__, __LINE__, data)) {
			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)symbols().size(); i++) {
				if (symbol(i).prefix == tok) {
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
	
			tokens.increment(true);
			tokens.expect(symbol(match[0]).postfix);
	
			tokens.increment(false);
			expectLiteral(tokens);

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				readLiteral(tokens, 0, data);

				tokens.increment(false);
				tokens.expect(symbol(match[0]).infix);

				while (tokens.decrement(__FILE__, __LINE__, data)) {
					tokens.next();

					tokens.increment(false);
					tokens.expect(symbol(match[0]).infix);

					tokens.increment(true);
					expectLiteral(tokens);

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
			expectLiteral(tokens);

			if (tokens.decrement(__FILE__, __LINE__, data)) {
				readLiteral(tokens, -1, data);
			}
		}
	}

	tokens.syntax_end(this);
}

bool expression::is_next(tokenizer &tokens, int i, void *data) {
	int level = -1;
	if (data != NULL)
		level = *(int*)data;

	bool result = (tokens.is_next("(", i)
		or tokens.is_next<parse::number>(i)
		or tokens.is_next<parse::instance>(i));

	for (int j = level+1; j < (int)precedence.size(); j++) {
		if (precedence[j].type == operation_set::UNARY) {
			for (int k = 0; k < (int)precedence[j].symbols.size(); k++) {
				result = result or tokens.is_next(precedence[j].symbols[k].prefix, i);
			}
		}
	}

	result = result
		and not tokens.is_next("func", i)
		and not tokens.is_next("struct", i)
		and not tokens.is_next("interface", i)
		and not tokens.is_next("context", i)
		and not tokens.is_next("await", i) 
		and not tokens.is_next("while", i)
		and not tokens.is_next("region", i)
		and not tokens.is_next("assume", i);

	return result;
}

void expression::register_syntax(tokenizer &tokens) {
	if (!tokens.syntax_registered<expression>()) {
		tokens.register_syntax<expression>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::number>();
		tokens.register_token<parse::instance>();
		tokens.register_token<parse::white_space>(false);
	}
}

string expression::to_string(string tab) const {
	return to_string(-1, tab);
}

string expression::to_string(int prev_level, string tab) const {
	if (!valid or arguments.size() == 0)
		return "false";

	string result = "";
	bool paren = prev_level > level;
	if (paren) {
		result += "(";
	}

	if (level < 0 or arguments.empty()) {
		result += "???";
	} else if (operators.empty()) {
		result += arguments[0].to_string(level, tab);
	} else if (precedence[level].type == operation_set::TERNARY) {
		result += arguments[0].to_string(level, tab)
			+ symbol(operators[0]).trigger + arguments[1].to_string(level, tab)
			+ symbol(operators[0]).infix + arguments[2].to_string(level, tab);
	} else if (precedence[level].type == operation_set::BINARY) {
		for (int i = 0; i < (int)arguments.size() and i-1 < (int)operators.size(); i++) {
			if (i != 0) {
				result += symbol(operators[i-1]).infix;
			}

			result += arguments[i].to_string(level, tab);
		}
	} else if (precedence[level].type == operation_set::UNARY) {
		for (int i = 0; i < (int)operators.size(); i++) {
			result += symbol(operators[i]).prefix;
		}

		result += arguments[0].to_string(level, tab);
	} else if (precedence[level].type == operation_set::MODIFIER) {
		result += arguments[0].to_string(level, tab) + symbol(operators[0]).trigger;
		for (int i = 1; i < (int)arguments.size(); i++) {
			if (i != 1) {
				result += symbol(operators[0]).infix;
			}
			result += arguments[i].to_string(-1, tab);
		}
		result += symbol(operators[0]).postfix;
	} else if (precedence[level].type == operation_set::GROUP) {
		result += symbol(operators[0]).prefix;
		for (int i = 0; i < (int)arguments.size() and i-1 < (int)operators.size(); i++) {
			if (i != 0) {
				result += symbol(operators[0]).infix;
			}

			result += arguments[i].to_string(level, tab);
		}
		result += symbol(operators[0]).postfix;
	} else {
		result += "???";
	}

	if (paren)
		result += ")";

	return result;
}

parse::syntax *expression::clone() const {
	return new expression(*this);
}

argument::argument() {
}

argument::argument(expression sub) {
	this->sub = sub;
}

argument argument::literalOf(string str) {
	argument result;
	result.literal = str;
	return result;
}

argument argument::constantOf(string str) {
	argument result;
	result.constant = str;
	return result;
}

argument::~argument() {
}

string argument::to_string(int level, string tab) const {
	if (sub.valid) {
		return sub.to_string(level, tab);
	} else if (not literal.empty()) {
		return literal;
	} else if (not constant.empty()) {
		return constant;
	}
	return "true";
}

}
