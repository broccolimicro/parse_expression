#include "config.h"
#include <parse_expression/literal.h>
#include <parse_expression/assignment.h>
#include <parse/wrapper.h>

parse_expression::config makeExprConfig() {
	using operation_set = parse_expression::operation_set;

	parse_expression::config cfg;
	int CONSTANT = cfg.push<parse_expression::default_constant>("constant");
	int LITERAL = cfg.push<parse_expression::default_literal>("literal");
	int LABEL = cfg.push<parse::wrapper<parse::instance> >("label");

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
	cfg.order.push_back("", "", "!=", "");
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

	cfg.set_lvalue();

	cfg.order.push(operation_set::MODIFIER);
	cfg.order.push_back("", "!", "", "");

	cfg.order.push(operation_set::UNARY);
	cfg.order.push_back("#", "", "", "");
	cfg.order.push_back("", "", "", "?");

	cfg.order.push(operation_set::MODIFIER);
	cfg.order.push_back("", "'", "", "", {LABEL});

	cfg.order.push(operation_set::MODIFIER);
	cfg.order.push_back("", "(", ",", ")");
	cfg.order.push_back("", ".", "", "", {LABEL});
	cfg.order.push_back("", "[", ":", "]");
	cfg.order.push_back("", "::", "", "", {LABEL});
	
	return cfg;
}

parse_expression::config makeCompConfig(parse_expression::context ctx) {
	using operation_set = parse_expression::operation_set;

	parse_expression::config cfg;

	int GUARD = cfg.push<parse_expression::guard>("guard", ctx);
	int ASSIGN = cfg.push<parse_expression::assignment>("assign", ctx);

	cfg.base = {GUARD, ASSIGN};

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", ":", "");

	cfg.order.push(operation_set::BINARY);
	cfg.order.push_back("", "", ",", "");

	cfg.set_lvalue();

	return cfg;
}
