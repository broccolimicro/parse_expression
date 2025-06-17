#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>

#include "precedence.h"

namespace parse_expression {

struct argument;

struct expression : parse::syntax {
	expression();
	expression(tokenizer &tokens, int level = 0, void *data = NULL);
	~expression();

	vector<argument> arguments;
	vector<int> operators;

	int level;

	static precedence_set precedence;

	static void register_precedence(const precedence_set &p);
	
	void expectLiteral(tokenizer &tokens, int next);
	void readLiteral(tokenizer &tokens, int next, void *data = NULL);
	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	string to_string(int prev_level, string tab = "") const;
	parse::syntax *clone() const;
};

struct argument {
	argument();
	argument(expression sub);
	~argument();

	expression sub;
	string literal;
	string constant;

	static argument literalOf(string str);
	static argument constantOf(string str);

	string to_string(int level = -1, string tab = "") const;
};

}

