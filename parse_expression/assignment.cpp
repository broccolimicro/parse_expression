#include "assignment.h"

namespace parse_expression {

guard::guard() {
	debug_name = "guard";
}

guard::guard(tokenizer &tokens, context ctx) {
	debug_name = "guard";
	parse(tokens, ctx);
}

guard::~guard() {
}

void guard::parse(tokenizer &tokens, std::any data) {
	if (not data.has_value()) {
		tokens.internal("guard has no context", __FILE__, __LINE__);
		return;
	}

	std::shared_ptr<config> cfg = std::any_cast<context>(data).cfg;

	context ctx(cfg);

	tokens.syntax_start(this);

	tokens.increment(true);
	tokens.expect("]");

	tokens.increment(true);
	tokens.expect<expression>(ctx);

	tokens.increment(true);
	tokens.expect("[");

	if (tokens.decrement(__FILE__, __LINE__)) {
		tokens.next();
	}

	if (tokens.decrement(__FILE__, __LINE__)) {
		expr = expression(tokens, ctx);
	}

	if (tokens.decrement(__FILE__, __LINE__)) {
		tokens.next();
	}

	tokens.syntax_end(this);
}

bool guard::is_next(tokenizer &tokens, int i, std::any data) {
	return tokens.is_next("[", i);
}

void guard::register_syntax(tokenizer &tokens) {
	if (not tokens.syntax_registered<guard>()) {
		tokens.register_syntax<guard>();
		tokens.register_token<parse::white_space>(false);
		expression::register_syntax(tokens);
	}
}

string guard::to_string(string tab) const {
	return "[" + expr.to_string(tab) + "]";
}

parse::syntax *guard::clone() const {
	return new guard(*this);
}

assignment::assignment() {
	debug_name = "assignment";
}

assignment::assignment(tokenizer &tokens, context ctx) {
	debug_name = "assignment";
	parse(tokens, ctx);
}

assignment::~assignment() {
}

void assignment::parse(tokenizer &tokens, std::any data) {
	if (not data.has_value()) {
		tokens.internal("assignment has no context", __FILE__, __LINE__);
		return;
	}

	std::shared_ptr<config> cfg = std::any_cast<context>(data).cfg;

	context lCtx(cfg, cfg->lvalueLevel);
	context rCtx(cfg);

	tokens.syntax_start(this);

	tokens.increment(false);
	tokens.expect("=");
	tokens.expect("+");
	tokens.expect("-");
	tokens.expect("~");

	tokens.increment(false);
	tokens.expect(",");

	tokens.increment(true);
	tokens.expect<expression>(lCtx);

	if (tokens.decrement(__FILE__, __LINE__)) {
		lvalue.push_back(expression(tokens, lCtx));
	}

	while (tokens.decrement(__FILE__, __LINE__)) {
		tokens.next();

		tokens.increment(false);
		tokens.expect(",");

		tokens.increment(true);
		tokens.expect<expression>(lCtx);

		if (tokens.decrement(__FILE__, __LINE__)) {
			lvalue.push_back(expression(tokens, lCtx));
		}
	}

	if (tokens.decrement(__FILE__, __LINE__)) {
		operation = tokens.next();
	}

	if (operation == "=") {
		tokens.increment(true);
		tokens.expect<expression>(rCtx);

		if (tokens.decrement(__FILE__, __LINE__)) {
			rvalue = expression(tokens, rCtx);
		}
	} 

	tokens.syntax_end(this);
}

bool assignment::is_next(tokenizer &tokens, int i, std::any data) {
	return expression::is_next(tokens, i, data);
}

void assignment::register_syntax(tokenizer &tokens) {
	if (not tokens.syntax_registered<assignment>()) {
		tokens.register_syntax<assignment>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::number>();
		tokens.register_token<parse::instance>();
		tokens.register_token<parse::white_space>(false);
		expression::register_syntax(tokens);
	}
}

string assignment::to_string(string tab) const {
	if (not valid or (lvalue.empty() and not rvalue.valid)) {
		return "skip";
	}

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

parse::syntax *assignment::clone() const {
	return new assignment(*this);
}

std::shared_ptr<config> defaultCompConfig(context ctx) {
	std::shared_ptr<config> cfg = std::make_shared<config>();

	int GUARD = cfg->push<guard>(ctx);
	int ASSIGN = cfg->push<assignment>(ctx);

	cfg->base = {GUARD, ASSIGN};

	cfg->order.push(operation_set::BINARY);
	cfg->order.push_back("", "", ":", "");

	cfg->order.push(operation_set::BINARY);
	cfg->order.push_back("", "", ",", "");

	cfg->lvalueLevel = 2;

	return cfg;
}

}
