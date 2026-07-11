#include "expression.h"

namespace parse_expression {

default_literal::default_literal() {
	this->debug_name = "literal";
}

default_literal::default_literal(tokenizer &tokens, void *data) {
	this->debug_name = "literal";
	parse(tokens, data);
}

default_literal::~default_literal() {
}

void default_literal::parse(tokenizer &tokens, void *data) {
	tokens.syntax_start(this);
	
	tokens.increment(true);
	tokens.expect<parse::instance>();
	if (tokens.decrement(__FILE__, __LINE__)) {
		name = tokens.next();
	}

	tokens.syntax_end(this);
}

bool default_literal::is_next(tokenizer &tokens, int i, void *data) {
	return tokens.is_next<parse::instance>(i)
		and not tokens.is_next("false", i)
		and not tokens.is_next("true", i)
		and not tokens.is_next("vdd", i)
		and not tokens.is_next("gnd", i);
}

void default_literal::register_syntax(tokenizer &tokens) {
	if (!tokens.syntax_registered<default_literal>()) {
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

default_constant::default_constant(tokenizer &tokens, void *data) {
	this->debug_name = "constant";
	parse(tokens, data);
}

default_constant::~default_constant() {
}

void default_constant::parse(tokenizer &tokens, void *data) {
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

bool default_constant::is_next(tokenizer &tokens, int i, void *data) {
	return tokens.is_next<parse::number>(i)
		or tokens.is_next("false", i)
		or tokens.is_next("true", i)
		or tokens.is_next("vdd", i)
		or tokens.is_next("gnd", i);
}

void default_constant::register_syntax(tokenizer &tokens) {
	if (!tokens.syntax_registered<default_constant>()) {
		tokens.register_token<parse::instance>();
	}
}

string default_constant::to_string(string tab) const {
	return value;
}

parse::syntax *default_constant::clone() const {
	return new default_constant(*this);
}

context::context() {
	data = nullptr;
}

context::context(precedence_set precedence, parse::schema constant, parse::schema literal, void *data)
	: constant(constant), literal(literal), precedence(precedence), data(data) {
}

context::~context() {
}

expression::expression() {
	debug_name = "expression";
	level = 0;
	type = -1;
}

expression::expression(const expression &copy) {
	segment_name = copy.segment_name;
	start = copy.start;
	end = copy.end;
	debug_name = copy.debug_name;
	valid = copy.valid;
	operators = copy.operators;
	level = copy.level;
	type = copy.type;
	for (int i = 0; i < (int)copy.arguments.size(); i++) {
		arguments.push_back(std::unique_ptr<parse::syntax>(copy.arguments[i]->clone()));
	}
}

expression::expression(context &ctx, tokenizer &tokens, int level) {
	debug_name = "expression";
	this->level = level;
	this->type = -1;
	if (ctx.precedence.isValidLevel(level)) {
		this->type = ctx.precedence.type(level);
	}
	parse(tokens, &ctx);
}

expression::~expression() {}

bool expression::isTernary() const {
	return type == operation_set::TERNARY;
}

bool expression::isBinary() const {
	return type == operation_set::BINARY;
}

bool expression::isUnary() const {
	return type == operation_set::UNARY;
}

bool expression::isModifier() const {
	return type == operation_set::MODIFIER;
}

bool expression::isGroup() const {
	return type == operation_set::GROUP;
}

void expression::expectLiteral(tokenizer &tokens, int next, context &ctx) {
	if (ctx.precedence.isValidLevel(next < 0 ? level+1 : next)) {
		tokens.expect<expression>(&ctx);
	} else {
		if (not ctx.literal.empty()) {
			tokens.expect(ctx.literal.label, ctx.data);
		}
		if (not ctx.constant.empty()) {
			tokens.expect(ctx.constant.label, ctx.data);
		}
		tokens.expect("(");
	}
}

void expression::readLiteral(tokenizer &tokens, int next, operation::ArgType argType, context &ctx) {
	if (tokens.found<expression>()) {
		arguments.push_back(std::make_unique<expression>(ctx, tokens, (next < 0 ? level+1 : next)));
	} else if (tokens.found(ctx.constant.label)
		or (tokens.found(ctx.literal.label) and argType == operation::LABEL)) {
		arguments.push_back(std::unique_ptr<parse::syntax>(ctx.constant.factory(tokens, ctx.data)));
	} else if (tokens.found(ctx.literal.label) and argType != operation::LABEL) {
		arguments.push_back(std::unique_ptr<parse::syntax>(ctx.literal.factory(tokens, ctx.data)));
	} else if (tokens.found("(")) {
		tokens.next();

		tokens.increment(true);
		tokens.expect(")");

		tokens.increment(true);
		tokens.expect<expression>(&ctx);

		if (tokens.decrement(__FILE__, __LINE__)) {
			arguments.push_back(std::make_unique<expression>(ctx, tokens, 0));
		}

		if (tokens.decrement(__FILE__, __LINE__)) {
			tokens.next();
		}
	}
}

void expression::parse(tokenizer &tokens, void *data) {
	if (data == nullptr) {
		tokens.internal("expression has no context", __FILE__, __LINE__);
		return;
	}
		
	context *ctx = (context*)data;

	tokens.syntax_start(this);

	if (ctx->precedence.operations.empty()) {
		tokens.internal("operator precedence not initialized", __FILE__, __LINE__);
	} else if (not ctx->precedence.isValidLevel(level)) {
		tokens.internal("invalid expression level", __FILE__, __LINE__);
	} else if (ctx->precedence.isTernary(level)) {
		tokens.increment(false);
		for (int i = 0; i < (int)ctx->precedence.at(level).size(); i++) {
			tokens.expect(ctx->precedence.at(level, i).trigger);
		}

		tokens.increment(true);
		expectLiteral(tokens, -1, *ctx);

		if (tokens.decrement(__FILE__, __LINE__)) {
			readLiteral(tokens, -1, operation::LITERAL, *ctx);
		}

		if (tokens.decrement(__FILE__, __LINE__)) {
			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)ctx->precedence.at(level).size(); i++) {
				if (ctx->precedence.at(level, i).trigger == tok) {
					match.push_back(i);
				}
			}

			tokens.increment(true);
			expectLiteral(tokens, -1, *ctx);

			tokens.increment(true);
			for (int i = 0; i < (int)match.size(); i++) {
				tokens.expect(ctx->precedence.at(level, match[i]).infix);
			}

			tokens.increment(true);
			expectLiteral(tokens, -1, *ctx);
			
			if (tokens.decrement(__FILE__, __LINE__)) {
				readLiteral(tokens, -1, operation::LITERAL, *ctx);
			}

			if (tokens.decrement(__FILE__, __LINE__)) {
				string tok = tokens.next();
				for (int i = (int)match.size()-1; i >= 0; i--) {
					if (ctx->precedence.at(level, match[i]).infix != tok) {
						match.erase(match.begin()+i);
					}
				}
			
				if (match.size() != 1u) {
					tokens.internal("ambiguous ternary operators", __FILE__, __LINE__);
					if (match.empty()) {
						return;
					}
				}

				operators.push_back(ctx->precedence.at(level, match[0]));
			}

			if (tokens.decrement(__FILE__, __LINE__)) {
				readLiteral(tokens, -1, operation::LITERAL, *ctx);
			}
		}
	} else if (ctx->precedence.isBinary(level)) {
		tokens.increment(false);
		for (int i = 0; i < (int)ctx->precedence.at(level).size(); i++) {
			tokens.expect(ctx->precedence.at(level, i).infix);
		}

		tokens.increment(true);
		expectLiteral(tokens, -1, *ctx);

		if (tokens.decrement(__FILE__, __LINE__)) {
			readLiteral(tokens, -1, operation::LITERAL, *ctx);
		}

		while (tokens.decrement(__FILE__, __LINE__)) {
			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)ctx->precedence.at(level).size(); i++) {
				if (ctx->precedence.at(level, i).infix == tok) {
					match.push_back(i);
				}
			}

			if (match.size() != 1u) {
				tokens.internal("ambiguous binary operators " + ::to_string(level) + ":" + ::to_string(match), __FILE__, __LINE__);
				if (match.empty()) {
					return;
				}
			}

			operators.push_back(ctx->precedence.at(level, match[0]));

			tokens.increment(false);
			for (int i = 0; i < (int)ctx->precedence.at(level).size(); i++) {
				tokens.expect(ctx->precedence.at(level, i).infix);
			}

			tokens.increment(true);
			expectLiteral(tokens, -1, *ctx);

			if (tokens.decrement(__FILE__, __LINE__)) {
				readLiteral(tokens, -1, operation::LITERAL, *ctx);
			}
		}
	} else if (ctx->precedence.isUnary(level)) {
		tokens.increment(true);
		expectLiteral(tokens, -1, *ctx);

		bool hasPrefix = false;
		for (int i = 0; i < (int)ctx->precedence.at(level).size(); i++) {
			if (not ctx->precedence.at(level, i).prefix.empty()) {
				if (not hasPrefix) {
					tokens.increment(false);
					hasPrefix = true;
				}
				tokens.expect(ctx->precedence.at(level, i).prefix);
			}
		}

		while (hasPrefix and tokens.decrement(__FILE__, __LINE__)) {
			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)ctx->precedence.at(level).size(); i++) {
				if (ctx->precedence.at(level, i).prefix == tok) {
					match.push_back(i);
				}
			}

			if (match.size() != 1u) {
				tokens.internal("ambiguous unary operators", __FILE__, __LINE__);
				if (match.empty()) {
					return;
				}
			}

			operators.push_back(ctx->precedence.at(level, match[0]));

			tokens.increment(false);
			for (auto i = 0; i < (int)ctx->precedence.at(level).size(); i++) {
				if (not ctx->precedence.at(level, i).prefix.empty()) {
					tokens.expect(ctx->precedence.at(level, i).prefix);
				}
			}
		}

		if (tokens.decrement(__FILE__, __LINE__)) {
			readLiteral(tokens, -1, operation::LITERAL, *ctx);
		}

		bool hasPostfix = false;
		for (int i = 0; i < (int)ctx->precedence.at(level).size(); i++) {
			if (not ctx->precedence.at(level, i).postfix.empty()) {
				if (not hasPostfix) {
					tokens.increment(false);
					hasPostfix = true;
				}
				tokens.expect(ctx->precedence.at(level, i).postfix);
			}
		}

		while (hasPostfix and tokens.decrement(__FILE__, __LINE__)) {
			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)ctx->precedence.at(level).size(); i++) {
				if (ctx->precedence.at(level, i).postfix == tok) {
					match.push_back(i);
				}
			}

			if (match.size() != 1u) {
				tokens.internal("ambiguous unary operators", __FILE__, __LINE__);
				if (match.empty()) {
					return;
				}
			}

			operators.push_back(ctx->precedence.at(level, match[0]));

			tokens.increment(false);
			for (auto i = 0; i < (int)ctx->precedence.at(level).size(); i++) {
				if (not ctx->precedence.at(level, i).postfix.empty()) {
					tokens.expect(ctx->precedence.at(level, i).postfix);
				}
			}
		}
	} else if (ctx->precedence.isModifier(level)) {
		bool hasPrefix = false;
		vector<int> found;

		for (int i = 0; i < (int)ctx->precedence.at(level).size(); i++) {
			found.push_back(i);
			if (not ctx->precedence.at(level, i).prefix.empty()) {
				if (not hasPrefix) {
					tokens.increment(false);
					hasPrefix = true;
				}
				tokens.expect(ctx->precedence.at(level, i).prefix);
			}
		}

		if (hasPrefix) {
			if (tokens.decrement(__FILE__, __LINE__)) {
				for (int i = found.size()-1; i >= 0; i--) {
					if (ctx->precedence.at(level, found[i]).prefix.empty()
						or not tokens.found(ctx->precedence.at(level, found[i]).prefix)) {
						found.erase(found.begin()+i);
					}
				}
			} else {
				for (int i = found.size()-1; i >= 0; i--) {
					if (not ctx->precedence.at(level, found[i]).prefix.empty()) {
						found.erase(found.begin()+i);
					}
				}
			}
		}

		if (not found.empty()) {
			tokens.increment(false);
			for (int i = 0; i < (int)found.size(); i++) {
				tokens.expect(ctx->precedence.at(level, found[i]).trigger);
			}
		}

		tokens.increment(true);
		expectLiteral(tokens, -1, *ctx);

		if (tokens.decrement(__FILE__, __LINE__)) {
			readLiteral(tokens, -1, operation::LITERAL, *ctx);
		}

		bool first = true;
		while (not found.empty() and tokens.decrement(__FILE__, __LINE__)) {
			if (not first) {
				expression sub = *this;
				sub.valid = true;
				arguments.clear();
				operators.clear();
				arguments.push_back(std::make_unique<expression>(sub));
			}
			first = false;

			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)found.size(); i++) {
				if (ctx->precedence.at(level, found[i]).trigger == tok) {
					match.push_back(i);
				}
			}

			if (match.size() != 1u) {
				tokens.internal("ambiguous modifier operators", __FILE__, __LINE__);
				if (match.empty()) {
					return;
				}
			}

			operators.push_back(ctx->precedence.at(level, match[0]));

			if (not ctx->precedence.at(level, match[0]).postfix.empty()) {
				tokens.increment(true);
				for (int i = 0; i < (int)found.size(); i++) {
					if (not ctx->precedence.at(level, found[i]).postfix.empty()) {
						tokens.expect(ctx->precedence.at(level, found[i]).postfix);
					}
				}
			}

			if (not ctx->precedence.at(level, match[0]).infix.empty()) {
				tokens.increment(false);
				expectLiteral(tokens, 0, *ctx);

				if (tokens.decrement(__FILE__, __LINE__)) {
					readLiteral(tokens, 0, ctx->precedence.at(level, match[0]).rightType, *ctx);

					tokens.increment(false);
					tokens.expect(ctx->precedence.at(level, match[0]).infix);

					while (tokens.decrement(__FILE__, __LINE__)) {
						tokens.next();

						tokens.increment(false);
						tokens.expect(ctx->precedence.at(level, match[0]).infix);

						tokens.increment(true);
						expectLiteral(tokens, 0, *ctx);

						if (tokens.decrement(__FILE__, __LINE__)) {
							readLiteral(tokens, 0, ctx->precedence.at(level, match[0]).rightType, *ctx);
						}
					}
				}
			} else {
				tokens.increment(true);
				expectLiteral(tokens, -1, *ctx);

				if (tokens.decrement(__FILE__, __LINE__)) {
					readLiteral(tokens, -1, ctx->precedence.at(level, match[0]).rightType, *ctx);
				}
			}

			if (not ctx->precedence.at(level, match[0]).postfix.empty() and tokens.decrement(__FILE__, __LINE__)) {
				tokens.next();
			}

			tokens.increment(false);
			for (int i = 0; i < (int)found.size(); i++) {
				tokens.expect(ctx->precedence.at(level, found[i]).trigger);
			}
		}
	} else if (ctx->precedence.isGroup(level)) {
		tokens.increment(false);
		for (int i = 0; i < (int)ctx->precedence.at(level).size(); i++) {
			tokens.expect(ctx->precedence.at(level, i).prefix);
		}

		if (tokens.decrement(__FILE__, __LINE__)) {
			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)ctx->precedence.at(level).size(); i++) {
				if (ctx->precedence.at(level, i).prefix == tok) {
					match.push_back(i);
				}
			}

			if (match.size() != 1u) {
				tokens.internal("ambiguous group operators", __FILE__, __LINE__);
				if (match.empty()) {
					return;
				}
			}

			operators.push_back(ctx->precedence.at(level, match[0]));
	
			tokens.increment(true);
			tokens.expect(ctx->precedence.at(level, match[0]).postfix);
	
			tokens.increment(false);
			expectLiteral(tokens, 0, *ctx);

			if (tokens.decrement(__FILE__, __LINE__)) {
				readLiteral(tokens, 0, operation::LITERAL, *ctx);

				tokens.increment(false);
				tokens.expect(ctx->precedence.at(level, match[0]).infix);

				while (tokens.decrement(__FILE__, __LINE__)) {
					tokens.next();

					tokens.increment(false);
					tokens.expect(ctx->precedence.at(level, match[0]).infix);

					tokens.increment(true);
					expectLiteral(tokens, 0, *ctx);

					if (tokens.decrement(__FILE__, __LINE__)) {
						readLiteral(tokens, 0, operation::LITERAL, *ctx);
					}
				}
			}

			if (tokens.decrement(__FILE__, __LINE__)) {
				tokens.next();
			}
		}	else {
			tokens.increment(true);
			expectLiteral(tokens, -1, *ctx);

			if (tokens.decrement(__FILE__, __LINE__)) {
				readLiteral(tokens, -1, operation::LITERAL, *ctx);
			}
		}
	}

	tokens.syntax_end(this);
}

bool expression::is_next(tokenizer &tokens, int i, void *data) {
	if (data == nullptr) {
		tokens.internal("expression has no context", __FILE__, __LINE__);
		return false;
	}

	context *ctx = (context*)data;

	if (tokens.is_next("func", i)
		or tokens.is_next("struct", i)
		or tokens.is_next("interface", i)
		or tokens.is_next("context", i)
		or tokens.is_next("await", i) 
		or tokens.is_next("if", i) 
		or tokens.is_next("while", i)
		or tokens.is_next("region", i)
		or tokens.is_next("assume", i)
		or tokens.is_next("var", i)) {
		return false;
	}

	bool result = (tokens.is_next("(", i)
		or tokens.is_next(ctx->constant.label, i)
		or tokens.is_next(ctx->literal.label, i));

	int level = -1;
	if (data != nullptr) {
		level = *(int*)data;
	}

	for (int j = level+1; j < (int)ctx->precedence.size(); j++) {
		for (int k = 0; k < (int)ctx->precedence.at(j).size(); k++) {
			if (not ctx->precedence.at(j, k).prefix.empty()) {
				result = result or tokens.is_next(ctx->precedence.at(j, k).prefix, i);
			}
		}
	}

	return result;
}

void expression::register_syntax(tokenizer &tokens) {
	if (!tokens.syntax_registered<expression>()) {
		tokens.register_syntax<expression>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::white_space>(false);
	}
}

string expression::to_string(string tab) const {
	return to_string(-1, false, tab);
}

string expression::argument_to_string(int i, int prev_level, bool prev_group, string tab) const {
	if (arguments[i]->is_a<expression>()) {
		return arguments[i]->get<expression>().to_string(prev_level, prev_group, tab);
	} else {
		return arguments[i]->to_string(tab);
	}
}

string expression::to_string(int prev_level, bool prev_group, string tab) const {
	if (not valid or arguments.empty())
		return "undef";

	string result = "";
	bool paren = prev_level > level and not prev_group;
	if (paren) {
		result += "(";
	}

	if (level < 0) {
		result += "undef";
	} else if (operators.empty()) {
		result += argument_to_string(0, level, false, tab);
	} else if (isTernary()) {
		result += argument_to_string(0, level, false, tab);
		result += operators[0].trigger;
		result += argument_to_string(1, level, false, tab);
		result += operators[0].infix;
		result += argument_to_string(2, level, false, tab);
	} else if (isBinary()) {
		for (int i = 0; i < (int)arguments.size() and i-1 < (int)operators.size(); i++) {
			if (i != 0) {
				result += operators[i-1].infix;
			}

			result += argument_to_string(i, level, false, tab);
		}
	} else if (isUnary()) {
		for (int i = 0; i < (int)operators.size(); i++) {
			result += operators[i].prefix;
		}

		result += argument_to_string(0, level, false, tab);

		for (int i = 0; i < (int)operators.size(); i++) {
			result += operators[i].postfix;
		}
	} else if (isModifier()) {
		result += argument_to_string(0, level, false, tab) + operators[0].trigger;
		for (int i = 1; i < (int)arguments.size(); i++) {
			if (i != 1) {
				result += operators[0].infix;
			}
			result += argument_to_string(i, -1, false, tab);
		}
		result += operators[0].postfix;
	} else if (isGroup()) {
		result += operators[0].prefix;
		for (int i = 0; i < (int)arguments.size(); i++) {
			if (i != 0) {
				result += operators[0].infix;
			}

			result += argument_to_string(i, level, true, tab);
		}
		result += operators[0].postfix;
	} else {
		result += "undef";
	}

	if (paren)
		result += ")";

	return result;
}

parse::syntax *expression::clone() const {
	return new expression(*this);
}

}
