#pragma once

#include <parse/parse.h>
#include <parse/default/number.h>
#include <parse/default/instance.h>

#include "precedence.h"

namespace parse_expression {

struct default_literal : parse::syntax {
	std::string name;

	default_literal();
	default_literal(tokenizer &tokens, std::any data=std::any());
	~default_literal();

	void parse(tokenizer &tokens, std::any data=std::any());
	static bool is_next(tokenizer &tokens, int i=1, std::any data=std::any());
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab="") const;

	parse::syntax *clone() const;
};

struct default_constant : parse::syntax {
	std::string value;

	default_constant();
	default_constant(tokenizer &tokens, std::any data=std::any());
	~default_constant();

	void parse(tokenizer &tokens, std::any data=std::any());
	static bool is_next(tokenizer &tokens, int i=1, std::any data=std::any());
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab="") const;

	parse::syntax *clone() const;
};

}
