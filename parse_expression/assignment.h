#pragma once

#include <parse/parse.h>

#include "expression.h"

namespace parse_expression {

struct guard : parse::syntax {
	expression expr;

	guard();
	guard(tokenizer &tokens, context ctx);
	~guard();

	void parse(tokenizer &tokens, std::any data=std::any());

	static bool is_next(tokenizer &tokens, int i=1, std::any data=std::any());
	static void register_syntax(tokenizer &tokens);
	string to_string(string tab="") const;

	parse::syntax *clone() const;
};

struct assignment : parse::syntax {
	vector<expression> left;
	expression right;
	string operation;

	assignment();
	assignment(tokenizer &tokens, context ctx);
	~assignment();

	void parse(tokenizer &tokens, std::any data=std::any());

	static bool is_next(tokenizer &tokens, int i=1, std::any data=std::any());
	static void register_syntax(tokenizer &tokens);
	string to_string(string tab="") const;

	parse::syntax *clone() const;
};

}
