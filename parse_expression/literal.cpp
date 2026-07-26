#include "literal.h"

namespace parse_expression {

default_literal::default_literal() {
	this->debug_name = "literal";
}

default_literal::default_literal(tokenizer &tokens, std::any data) {
	this->debug_name = "literal";
	parse(tokens, data);
}

default_literal::~default_literal() {
}

void default_literal::parse(tokenizer &tokens, std::any data) {
	tokens.syntax_start(this);
	
	tokens.increment(true);
	tokens.expect<parse::instance>();
	if (tokens.decrement(__FILE__, __LINE__)) {
		name = tokens.next();
	}

	tokens.syntax_end(this);
}

bool default_literal::is_next(tokenizer &tokens, int i, std::any data) {
	return tokens.is_next<parse::instance>(i)
		and not tokens.is_next("false", i)
		and not tokens.is_next("true", i)
		and not tokens.is_next("vdd", i)
		and not tokens.is_next("gnd", i);
}

void default_literal::register_syntax(tokenizer &tokens) {
	if (!tokens.syntax_registered<default_literal>()) {
		tokens.register_syntax<default_literal>();
		tokens.register_token<parse::instance>();
	}
}

string default_literal::to_string(string tab) const {
	return name;
}

parse::syntax *default_literal::clone() const {
	return new default_literal(*this);
}

default_constant::default_constant() {
	this->debug_name = "constant";
}

default_constant::default_constant(tokenizer &tokens, std::any data) {
	this->debug_name = "constant";
	parse(tokens, data);
}

default_constant::~default_constant() {
}

void default_constant::parse(tokenizer &tokens, std::any data) {
	tokens.syntax_start(this);
	
	tokens.increment(true);
	tokens.expect<parse::number>();
	tokens.expect("false");
	tokens.expect("true");
	tokens.expect("vdd");
	tokens.expect("gnd");

	if (tokens.decrement(__FILE__, __LINE__)) {
		value = tokens.next();
	}

	tokens.syntax_end(this);
}

bool default_constant::is_next(tokenizer &tokens, int i, std::any data) {
	return tokens.is_next<parse::number>(i)
		or tokens.is_next("false", i)
		or tokens.is_next("true", i)
		or tokens.is_next("vdd", i)
		or tokens.is_next("gnd", i);
}

void default_constant::register_syntax(tokenizer &tokens) {
	if (!tokens.syntax_registered<default_constant>()) {
		tokens.register_syntax<default_constant>();
		tokens.register_token<parse::number>();
	}
}

string default_constant::to_string(string tab) const {
	return value;
}

parse::syntax *default_constant::clone() const {
	return new default_constant(*this);
}

config defaultExprConfig() {
	config cfg;
	int CONSTANT = cfg.push<default_constant>("constant");
	int LITERAL = cfg.push<default_literal>("literal");
	int LABEL = cfg.push<default_literal>("label");

	cfg.base = {CONSTANT, LITERAL};

	cfg.order.push(operation_set::GROUP);
	cfg.order.push_back("[", "", ",", "]");

	cfg.order.push(operation_set::TERNARY);
	cfg.order.push_back("", "@", ":", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "|", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "&", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "^", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "==", "");
	cfg.order.push_back("", "", "~=", "");
	cfg.order.push_back("", "", "<", "");
	cfg.order.push_back("", "", ">", "");
	cfg.order.push_back("", "", "<=", "");
	cfg.order.push_back("", "", ">=", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "||", "");
	
	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "&&", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "^^", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "<<", "");
	cfg.order.push_back("", "", ">>", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "+", "");
	cfg.order.push_back("", "", "-", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", "*", "");
	cfg.order.push_back("", "", "/", "");
	cfg.order.push_back("", "", "%", "");

	cfg.order.push(operation_set::UNARY);
	cfg.order.push_back("!", "", "", "");
	cfg.order.push_back("~", "", "", "");
	cfg.order.push_back("+", "", "", "");
	cfg.order.push_back("-", "", "", "");

	cfg.order.push(operation_set::MODIFIER);
	cfg.order.push_back("", "!", "", "");

	cfg.order.push(operation_set::UNARY);
	cfg.order.push_back("#", "", "", "");
	cfg.order.push_back("", "", "", "?");

	cfg.order.push(operation_set::MODIFIER);
	cfg.order.push_back("", "'", "", "", cfg.base, {LABEL});

	cfg.order.push(operation_set::MODIFIER);
	cfg.order.push_back("", "(", ",", ")");
	cfg.order.push_back("", ".", "", "", cfg.base, {LABEL});
	cfg.order.push_back("", "[", ":", "]");
	cfg.order.push_back("", "::", "", "", cfg.base, {LABEL});
	
	cfg.lvalueLevel = 15;

	return cfg;
}

}
