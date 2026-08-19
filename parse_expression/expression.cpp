#include "expression.h"

namespace parse_expression {

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
	for (const argument &arg : copy.arguments) {
		if (arg.ptr) {
			arguments.push_back({arg.type, std::shared_ptr<parse::syntax>(arg.ptr->clone())});
		} else {
			arguments.push_back({arg.type, std::shared_ptr<parse::syntax>(nullptr)});
		}
	}
}

expression::expression(tokenizer &tokens, context ctx) {
	debug_name = "expression";
	level = 0;
	type = -1;
	parse(tokens, ctx);
}

expression::~expression() {}

context expression::sub(std::shared_ptr<config> cfg, int nextLevel) {
	return context(cfg, nextLevel < 0 ? level+1 : nextLevel);
}

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

void expression::expectLiteral(tokenizer &tokens, context ctx, std::vector<int> argType) {
	if (ctx.cfg->order.isValidLevel(ctx.level) and argType.empty()) {
		tokens.expect<expression>(ctx);
	} else {
		if (argType.empty()) {
			argType = ctx.cfg->base;
		}

		for (int arg : argType) {
			ctx.cfg->literals[arg].second.expect(tokens);
		}
		tokens.expect("(");
	}
}

void expression::readLiteral(tokenizer &tokens, context ctx, std::vector<int> argType) {
	if (tokens.found<expression>()) {
		arguments.push_back({-1, std::make_shared<expression>(tokens, ctx)});
	} else if (tokens.found("(")) {
		context upCtx = sub(ctx.cfg, 0);

		tokens.next();

		tokens.increment(true);
		tokens.expect(")");

		tokens.increment(true);
		tokens.expect<expression>(upCtx);

		if (tokens.decrement(__FILE__, __LINE__)) {
			arguments.push_back({-1, std::make_shared<expression>(tokens, upCtx)});
		}

		if (tokens.decrement(__FILE__, __LINE__)) {
			tokens.next();
		}
	} else {
		if (argType.empty()) {
			argType = ctx.cfg->base;
		}

		for (int arg : argType) {
			if (ctx.cfg->literals[arg].second.found(tokens)) {
				arguments.push_back({arg, std::shared_ptr<parse::syntax>(ctx.cfg->literals[arg].second.produce(tokens))});
				break;
			}
		}
	}
}

void expression::parse(tokenizer &tokens, std::any data) {
	if (not data.has_value()) {
		tokens.internal("expression has no context", __FILE__, __LINE__);
		return;
	}

	std::shared_ptr<config> cfg = std::any_cast<context>(data).cfg;
	level = std::any_cast<context>(data).level;
	if (cfg->order.isValidLevel(level)) {
		type = cfg->order.type(level);
	}

	tokens.syntax_start(this);

	if (cfg->order.operations.empty()) {
		tokens.internal("operator precedence not initialized", __FILE__, __LINE__);
	} else if (not cfg->order.isValidLevel(level)) {
		tokens.internal("invalid expression level", __FILE__, __LINE__);
	} else if (cfg->order.isTernary(level)) {
		context ctx = sub(cfg);

		tokens.increment(false);
		for (int i = 0; i < (int)cfg->order.at(level).size(); i++) {
			tokens.expect(cfg->order.at(level, i).trigger);
		}

		tokens.increment(true);
		expectLiteral(tokens, ctx);

		if (tokens.decrement(__FILE__, __LINE__)) {
			readLiteral(tokens, ctx);
		}

		if (tokens.decrement(__FILE__, __LINE__)) {
			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)cfg->order.at(level).size(); i++) {
				if (cfg->order.at(level, i).trigger == tok) {
					match.push_back(i);
				}
			}

			tokens.increment(true);
			expectLiteral(tokens, ctx);

			tokens.increment(true);
			for (int i = 0; i < (int)match.size(); i++) {
				tokens.expect(cfg->order.at(level, match[i]).infix);
			}

			tokens.increment(true);
			expectLiteral(tokens, ctx);
			
			if (tokens.decrement(__FILE__, __LINE__)) {
				readLiteral(tokens, ctx);
			}

			if (tokens.decrement(__FILE__, __LINE__)) {
				string tok = tokens.next();
				for (int i = (int)match.size()-1; i >= 0; i--) {
					if (cfg->order.at(level, match[i]).infix != tok) {
						match.erase(match.begin()+i);
					}
				}
			
				if (match.size() != 1u) {
					tokens.internal("ambiguous ternary operators", __FILE__, __LINE__);
					if (match.empty()) {
						return;
					}
				}

				operators.push_back(cfg->order.at(level, match[0]));
			}

			if (tokens.decrement(__FILE__, __LINE__)) {
				readLiteral(tokens, ctx);
			}
		}
	} else if (cfg->order.isBinary(level)) {
		context ctx = sub(cfg);

		tokens.increment(false);
		for (int i = 0; i < (int)cfg->order.at(level).size(); i++) {
			tokens.expect(cfg->order.at(level, i).infix);
		}

		tokens.increment(true);
		expectLiteral(tokens, ctx);

		if (tokens.decrement(__FILE__, __LINE__)) {
			readLiteral(tokens, ctx);
		}

		while (tokens.decrement(__FILE__, __LINE__)) {
			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)cfg->order.at(level).size(); i++) {
				if (cfg->order.at(level, i).infix == tok) {
					match.push_back(i);
				}
			}

			if (match.size() != 1u) {
				tokens.internal("ambiguous binary operators " + ::to_string(level) + ":" + ::to_string(match), __FILE__, __LINE__);
				if (match.empty()) {
					return;
				}
			}

			operators.push_back(cfg->order.at(level, match[0]));

			tokens.increment(false);
			for (int i = 0; i < (int)cfg->order.at(level).size(); i++) {
				tokens.expect(cfg->order.at(level, i).infix);
			}

			tokens.increment(true);
			expectLiteral(tokens, ctx);

			if (tokens.decrement(__FILE__, __LINE__)) {
				readLiteral(tokens, ctx);
			}
		}
	} else if (cfg->order.isUnary(level)) {
		context ctx = sub(cfg);

		tokens.increment(true);
		expectLiteral(tokens, ctx);

		bool hasPrefix = false;
		for (int i = 0; i < (int)cfg->order.at(level).size(); i++) {
			if (not cfg->order.at(level, i).prefix.empty()) {
				if (not hasPrefix) {
					tokens.increment(false);
					hasPrefix = true;
				}
				tokens.expect(cfg->order.at(level, i).prefix);
			}
		}

		while (hasPrefix and tokens.decrement(__FILE__, __LINE__)) {
			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)cfg->order.at(level).size(); i++) {
				if (cfg->order.at(level, i).prefix == tok) {
					match.push_back(i);
				}
			}

			if (match.size() != 1u) {
				tokens.internal("ambiguous unary operators", __FILE__, __LINE__);
				if (match.empty()) {
					return;
				}
			}

			operators.push_back(cfg->order.at(level, match[0]));

			tokens.increment(false);
			for (auto i = 0; i < (int)cfg->order.at(level).size(); i++) {
				if (not cfg->order.at(level, i).prefix.empty()) {
					tokens.expect(cfg->order.at(level, i).prefix);
				}
			}
		}

		if (tokens.decrement(__FILE__, __LINE__)) {
			readLiteral(tokens, ctx);
		}

		bool hasPostfix = false;
		for (int i = 0; i < (int)cfg->order.at(level).size(); i++) {
			if (not cfg->order.at(level, i).postfix.empty()) {
				if (not hasPostfix) {
					tokens.increment(false);
					hasPostfix = true;
				}
				tokens.expect(cfg->order.at(level, i).postfix);
			}
		}

		while (hasPostfix and tokens.decrement(__FILE__, __LINE__)) {
			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)cfg->order.at(level).size(); i++) {
				if (cfg->order.at(level, i).postfix == tok) {
					match.push_back(i);
				}
			}

			if (match.size() != 1u) {
				tokens.internal("ambiguous unary operators", __FILE__, __LINE__);
				if (match.empty()) {
					return;
				}
			}

			operators.push_back(cfg->order.at(level, match[0]));

			tokens.increment(false);
			for (auto i = 0; i < (int)cfg->order.at(level).size(); i++) {
				if (not cfg->order.at(level, i).postfix.empty()) {
					tokens.expect(cfg->order.at(level, i).postfix);
				}
			}
		}
	} else if (cfg->order.isModifier(level)) {
		context ctx = sub(cfg);

		bool hasPrefix = false;
		vector<int> found;

		for (int i = 0; i < (int)cfg->order.at(level).size(); i++) {
			found.push_back(i);
			if (not cfg->order.at(level, i).prefix.empty()) {
				if (not hasPrefix) {
					tokens.increment(false);
					hasPrefix = true;
				}
				tokens.expect(cfg->order.at(level, i).prefix);
			}
		}

		if (hasPrefix) {
			if (tokens.decrement(__FILE__, __LINE__)) {
				for (int i = found.size()-1; i >= 0; i--) {
					if (cfg->order.at(level, found[i]).prefix.empty()
						or not tokens.found(cfg->order.at(level, found[i]).prefix)) {
						found.erase(found.begin()+i);
					}
				}
			} else {
				for (int i = found.size()-1; i >= 0; i--) {
					if (not cfg->order.at(level, found[i]).prefix.empty()) {
						found.erase(found.begin()+i);
					}
				}
			}
		}

		if (not found.empty()) {
			tokens.increment(false);
			for (int i = 0; i < (int)found.size(); i++) {
				tokens.expect(cfg->order.at(level, found[i]).trigger);
			}
		}

		tokens.increment(true);
		expectLiteral(tokens, ctx);

		if (tokens.decrement(__FILE__, __LINE__)) {
			readLiteral(tokens, ctx);
		}

		bool first = true;
		while (not found.empty() and tokens.decrement(__FILE__, __LINE__)) {
			if (not first) {
				expression sub = *this;
				sub.valid = true;
				arguments.clear();
				operators.clear();
				arguments.push_back({-1, std::make_shared<expression>(sub)});
			}
			first = false;

			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)found.size(); i++) {
				if (cfg->order.at(level, found[i]).trigger == tok) {
					match.push_back(found[i]);
				}
			}

			if (match.size() != 1u) {
				tokens.internal("ambiguous modifier operators", __FILE__, __LINE__);
				if (match.empty()) {
					return;
				}
			}

			operators.push_back(cfg->order.at(level, match[0]));

			if (not cfg->order.at(level, match[0]).postfix.empty()) {
				tokens.increment(true);
				tokens.expect(cfg->order.at(level, match[0]).postfix);
			}

			if (not cfg->order.at(level, match[0]).infix.empty()) {
				context upCtx = sub(cfg, 0);

				tokens.increment(false);
				expectLiteral(tokens, upCtx, cfg->order.at(level, match[0]).argType);

				if (tokens.decrement(__FILE__, __LINE__)) {
					readLiteral(tokens, upCtx, cfg->order.at(level, match[0]).argType);

					tokens.increment(false);
					tokens.expect(cfg->order.at(level, match[0]).infix);

					while (tokens.decrement(__FILE__, __LINE__)) {
						tokens.next();

						tokens.increment(false);
						tokens.expect(cfg->order.at(level, match[0]).infix);

						tokens.increment(true);
						expectLiteral(tokens, upCtx, cfg->order.at(level, match[0]).argType);

						if (tokens.decrement(__FILE__, __LINE__)) {
							readLiteral(tokens, upCtx, cfg->order.at(level, match[0]).argType);
						}
					}
				}
			} else {
				context upCtx;
				if (cfg->order.at(level, match[0]).postfix.empty()) {
					upCtx = sub(cfg);
				} else {
					upCtx = sub(cfg, 0);
				}

				std::vector<int> argType = cfg->order.at(level, match[0]).argType;

				tokens.increment(true);
				expectLiteral(tokens, upCtx, argType);

				if (tokens.decrement(__FILE__, __LINE__)) {
					readLiteral(tokens, upCtx, argType);
				}
			}

			if (not cfg->order.at(level, match[0]).postfix.empty() and tokens.decrement(__FILE__, __LINE__)) {
				tokens.next();
			}

			tokens.increment(false);
			for (int i = 0; i < (int)found.size(); i++) {
				tokens.expect(cfg->order.at(level, found[i]).trigger);
			}
		}
	} else if (cfg->order.isGroup(level)) {
		context upCtx = sub(cfg, 0);

		tokens.increment(false);
		for (int i = 0; i < (int)cfg->order.at(level).size(); i++) {
			tokens.expect(cfg->order.at(level, i).prefix);
		}

		if (tokens.decrement(__FILE__, __LINE__)) {
			string tok = tokens.next();
			vector<int> match;
			for (int i = 0; i < (int)cfg->order.at(level).size(); i++) {
				if (cfg->order.at(level, i).prefix == tok) {
					match.push_back(i);
				}
			}

			if (match.size() != 1u) {
				tokens.internal("ambiguous group operators", __FILE__, __LINE__);
				if (match.empty()) {
					return;
				}
			}

			operators.push_back(cfg->order.at(level, match[0]));
	
			tokens.increment(true);
			tokens.expect(cfg->order.at(level, match[0]).postfix);
	
			tokens.increment(false);
			expectLiteral(tokens, upCtx);

			if (tokens.decrement(__FILE__, __LINE__)) {
				readLiteral(tokens, upCtx);

				tokens.increment(false);
				tokens.expect(cfg->order.at(level, match[0]).infix);

				while (tokens.decrement(__FILE__, __LINE__)) {
					tokens.next();

					tokens.increment(false);
					tokens.expect(cfg->order.at(level, match[0]).infix);

					tokens.increment(true);
					expectLiteral(tokens, upCtx);

					if (tokens.decrement(__FILE__, __LINE__)) {
						readLiteral(tokens, upCtx);
					}
				}
			}

			if (tokens.decrement(__FILE__, __LINE__)) {
				tokens.next();
			}
		}	else {
			context ctx = sub(cfg);

			tokens.increment(true);
			expectLiteral(tokens, ctx);

			if (tokens.decrement(__FILE__, __LINE__)) {
				readLiteral(tokens, ctx);
			}
		}
	}

	tokens.syntax_end(this);
}

bool expression::is_next(tokenizer &tokens, int i, std::any data) {
	if (not data.has_value()) {
		tokens.internal("expression has no context", __FILE__, __LINE__);
		return false;
	}

	context ctx = std::any_cast<context>(data);

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

	if (tokens.is_next("(", i)) {
		return true;
	}
	for (auto &literal : ctx.cfg->literals) {
		if (literal.second.is_next(tokens, i)) {
			return true;
		}
	}

	for (int j = ctx.level; j < (int)ctx.cfg->order.size(); j++) {
		for (int k = 0; k < (int)ctx.cfg->order.at(j).size(); k++) {
			if (not ctx.cfg->order.at(j, k).prefix.empty()) {
				if (tokens.is_next(ctx.cfg->order.at(j, k).prefix, i)) {
					return true;
				}
			}
		}
	}

	return false;
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
	if (not arguments[i].ptr) {
		return "undef";
	} else if (arguments[i].ptr->is_a<expression>()) {
		return arguments[i].ptr->get<expression>().to_string(prev_level, prev_group, tab);
	} else {
		return arguments[i].ptr->to_string(tab);
	}
}

string expression::to_string(int prev_level, bool prev_group, string tab) const {
	if (not valid or arguments.empty() or level < 0) {
		return "undef";
	} else if (operators.empty()) {
		return argument_to_string(0, prev_level, prev_group, tab);
	}

	string result = "";
	bool paren = level >= 0 and prev_level > level and not prev_group;
	if (paren) {
		result += "(";
	}

	if (isTernary()) {
		result += argument_to_string(0, level, false, tab);
		result += operators[0].trigger;
		result += argument_to_string(1, level, false, tab);
		result += operators[0].infix;
		result += argument_to_string(2, level, false, tab);
	} else if (isBinary()) {
		for (int i = 0; i < (int)arguments.size(); i++) {
			if (i != 0) {
				if (i-1 < (int)operators.size()) {
					result += operators[i-1].infix;
				} else if (not operators.empty()) {
					result += operators.back().infix;
				}
			}

			result += argument_to_string(i, level, false, tab);
		}
	} else if (isUnary()) {
		for (int i = 0; i < (int)operators.size(); i++) {
			result += operators[i].prefix;
			if (not operators[i].prefix.empty() and isalnum(operators[i].prefix.back())) {
				result += " ";
			}
		}

		result += argument_to_string(0, level, false, tab);

		for (int i = 0; i < (int)operators.size(); i++) {
			result += operators[i].postfix;
		}
	} else if (isModifier()) {
		result += operators[0].prefix + argument_to_string(0, level, false, tab) + operators[0].trigger;
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
