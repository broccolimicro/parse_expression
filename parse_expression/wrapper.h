#pragma once

#include <parse/parse.h>

#include "expression.h"

namespace parse_expression {

template <typename Tag, typename Base=expression>
struct rvalue : Base {
	using super = Base;

	rvalue() {
		super::debug_name = Tag().debug_name+"_"+super::debug_name+"_rvalue";
	}

	rvalue(const Base &copy) : Base(copy) {
		super::debug_name = Tag().debug_name+"_"+super::debug_name+"_rvalue";
	}

	rvalue(tokenizer &tokens, std::any data={}) {
		super::debug_name = Tag().debug_name+"_"+super::debug_name+"_rvalue";
		parse(tokens, data);
	}

	~rvalue() {
	}

	void parse(tokenizer &tokens, std::any data={}) {
		super::parse(tokens, context(Tag::cfg));
	}

	static bool is_next(tokenizer &tokens, int i, std::any data={}) {
		return super::is_next(tokens, i, context(Tag::cfg));
	}

	static void register_syntax(tokenizer &tokens) {
		Tag::cfg->register_syntax(tokens);
		if (not tokens.syntax_registered<rvalue<Tag, Base> >()) {
			tokens.register_syntax<rvalue<Tag, Base> >();
			super::register_syntax(tokens);
		}
	}
};

template <typename Tag, typename Base=expression>
struct lvalue : Base {
	using super = Base;

	lvalue() {
		super::debug_name = Tag().debug_name+"_"+super::debug_name+"_lvalue";
	}

	lvalue(const Base &copy) : Base(copy) {
		super::debug_name = Tag().debug_name+"_"+super::debug_name+"_lvalue";
	}

	lvalue(tokenizer &tokens, std::any data={}) {
		super::debug_name = Tag().debug_name+"_"+super::debug_name+"_lvalue";
		parse(tokens, data);
	}

	~lvalue() {
	}

	void parse(tokenizer &tokens, std::any data={}) {
		super::parse(tokens, context(Tag::cfg, Tag::cfg->lvalueLevel));
	}

	static bool is_next(tokenizer &tokens, int i, std::any data={}) {
		return super::is_next(tokens, i, context(Tag::cfg, Tag::cfg->lvalueLevel));
	}

	static void register_syntax(tokenizer &tokens) {
		Tag::cfg->register_syntax(tokens);
		if (!tokens.syntax_registered<lvalue<Tag, Base> >()) {
			tokens.register_syntax<lvalue<Tag, Base> >();
			super::register_syntax(tokens);
		}
	}
};

}

