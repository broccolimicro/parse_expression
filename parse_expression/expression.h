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

template <typename T>
struct rvalue : expression {
	using super = expression;

	rvalue() {
		debug_name = T().debug_name+"_rvalue";
	}

	rvalue(tokenizer &tokens, std::any data={}) {
		debug_name = T().debug_name+"_rvalue";
		parse(tokens, data);
	}

	~rvalue() {
	}

	void parse(tokenizer &tokens, std::any data={}) {
		super::parse(tokens, context(T::cfg));
	}

	static bool is_next(tokenizer &tokens, int i, std::any data={}) {
		return super::is_next(tokens, i, context(T::cfg));
	}

	static void register_syntax(tokenizer &tokens) {
		T::cfg->register_syntax(tokens);
		if (!tokens.syntax_registered<rvalue<T> >()) {
			tokens.register_syntax<rvalue<T> >();
			tokens.register_token<parse::symbol>();
			tokens.register_token<parse::white_space>(false);
			super::register_syntax(tokens);
		}
	}
};

template <typename T>
struct lvalue : expression {
	using super = expression;

	lvalue() {
		debug_name = T().debug_name+"_lvalue";
	}

	lvalue(tokenizer &tokens, std::any data={}) {
		debug_name = T().debug_name+"_lvalue";
		parse(tokens, data);
	}

	~lvalue() {
	}

	void parse(tokenizer &tokens, std::any data={}) {
		super::parse(tokens, context(T::cfg, T::cfg->lvalueLevel));
	}

	static bool is_next(tokenizer &tokens, int i, std::any data={}) {
		return super::is_next(tokens, i, context(T::cfg, T::cfg->lvalueLevel));
	}

	static void register_syntax(tokenizer &tokens) {
		T::cfg->register_syntax(tokens);
		if (!tokens.syntax_registered<lvalue<T> >()) {
			tokens.register_syntax<lvalue<T> >();
			tokens.register_token<parse::symbol>();
			tokens.register_token<parse::white_space>(false);
			super::register_syntax(tokens);
		}
	}
};

}

