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

struct expression : parse::syntax {
	struct argument {
		int type;
		std::shared_ptr<parse::syntax> ptr;
	};

	vector<argument> arguments;
	vector<operation> operators;

	int level;
	int type;

	expression();
	expression(const expression &copy);
	expression(tokenizer &tokens, context ctx);
	~expression();

	context sub(std::shared_ptr<config> cfg, int nextLevel=-1);

	bool isTernary() const;
	bool isBinary() const;
	bool isUnary() const;
	bool isModifier() const;
	bool isGroup() const;

	void expectLiteral(tokenizer &tokens, context ctx, std::vector<int> argType=std::vector<int>());
	void readLiteral(tokenizer &tokens, context ctx, std::vector<int> argType=std::vector<int>());

	void parse(tokenizer &tokens, std::any data);
	static bool is_next(tokenizer &tokens, int i=1, std::any data=std::any());
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab="") const;
	string argument_to_string(int i, int prev_level, bool prev_group, string tab) const;
	string to_string(int prev_level, bool prev_group, string tab="") const;

	parse::syntax *clone() const;
};

template <typename tag>
struct wrapper : expression {
	using super = expression;

	static std::shared_ptr<config> cfg;

	static void expect(tokenizer &tokens) {
		tokens.expect<expression>(context(cfg));
	}

	static void expectl(tokenizer &tokens) {
		tokens.expect<expression>(context(cfg, cfg->lvalueLevel));
	}

	void parse(tokenizer &tokens, std::any data={}) {
		super::parse(tokens, context(cfg));
	}

	void parsel(tokenizer &tokens, std::any data={}) {
		super::parse(tokens, context(cfg, cfg->lvalueLevel));
	}

	static bool is_next(tokenizer &tokens, int i, std::any data={}) {
		return super::is_next(tokens, i, context(cfg));
	}

	static bool is_nextl(tokenizer &tokens, int i, std::any data={}) {
		return super::is_next(tokens, i, context(cfg, cfg->lvalueLevel));
	}

	static void register_syntax(tokenizer &tokens) {
		cfg->register_syntax(tokens);
		super::register_syntax(tokens);
	}
};

}

