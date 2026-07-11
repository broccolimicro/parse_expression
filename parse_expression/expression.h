#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>
#include <parse/schema.h>
#include <parse/default/symbol.h>
#include <parse/default/number.h>
#include <parse/default/white_space.h>
#include <parse/default/instance.h>
#include <parse/default/new_line.h>

#include "precedence.h"

namespace parse_expression {

struct default_literal : parse::syntax {
	std::string name;

	default_literal();
	default_literal(tokenizer &tokens, void *data);
	~default_literal();

	void parse(tokenizer &tokens, void *data);
	static bool is_next(tokenizer &tokens, int i=1, void *data=nullptr);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab="") const;

	parse::syntax *clone() const;
};

struct default_constant : parse::syntax {
	std::string value;

	default_constant();
	default_constant(tokenizer &tokens, void *data);
	~default_constant();

	void parse(tokenizer &tokens, void *data);
	static bool is_next(tokenizer &tokens, int i=1, void *data=nullptr);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab="") const;

	parse::syntax *clone() const;
};

struct context {
	parse::schema constant;
	parse::schema literal;
	precedence_set precedence;
	void *data;

	context();
	context(precedence_set precedence, parse::schema constant=parse::schema::from<default_constant>(), parse::schema literal=parse::schema::from<default_literal>(), void *data=nullptr);
	~context();
};

struct expression : parse::syntax {
	vector<std::unique_ptr<parse::syntax> > arguments;
	vector<operation> operators;

	int level;
	int type;

	expression();
	expression(const expression &copy);
	expression(context &ctx, tokenizer &tokens, int level=0);
	~expression();

	bool isTernary() const;
	bool isBinary() const;
	bool isUnary() const;
	bool isModifier() const;
	bool isGroup() const;

	void expectLiteral(tokenizer &tokens, int next, context &ctx);
	void readLiteral(tokenizer &tokens, int next, operation::ArgType argType, context &ctx);

	void parse(tokenizer &tokens, void *data);
	static bool is_next(tokenizer &tokens, int i=1, void *data=nullptr);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab="") const;
	string argument_to_string(int i, int prev_level, bool prev_group, string tab) const;
	string to_string(int prev_level, bool prev_group, string tab="") const;

	parse::syntax *clone() const;
};

}

