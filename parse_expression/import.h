#pragma once

#include <common/standard.h>

#include <parse/tokenizer.h>

#include "expression.h"

namespace parse_expression {

template <typename T>
struct Importer {
	virtual T import_term(const expression::argument &syntax, tokenizer *tokens) const = 0;
	virtual void push_properties(operation op, const vector<expression::argument> &args, tokenizer *tokens) {
		// default behavior is NOP
	}

	virtual void pop_properties(operation op) {
		// default behavior is NOP
	}

	virtual T import_unary(operation op, T expr, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("unary operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "unary operations not supported by interpreter", __FILE__, __LINE__);
		}
		return expr;
	}

	virtual T import_binary(operation op, T left, T right, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("binary operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "binary operations not supported by interpreter", __FILE__, __LINE__);
		}
		return left;
	}

	virtual T import_ternary(operation op, vector<T> args, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("binary operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "binary operations not supported by interpreter", __FILE__, __LINE__);
		}
		if (args.empty()) {
			return T();
		}
		return args[0];
	}

	virtual T import_group(operation op, vector<T> args, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("group operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "group operations not supported by interpreter", __FILE__, __LINE__);
		}
		if (not args.empty()) {
			return args[0];
		}
		return T();
	}

	virtual T import_modifier(operation op, vector<T> args, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("modifier operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "modifier operations not supported by interpreter", __FILE__, __LINE__);
		}
		if (not args.empty()) {
			return args[0];
		}
		return T();
	}

	// These probably don't need an overide
	virtual T import_argument(const expression::argument &syntax, tokenizer *tokens) {
		if (tokens != NULL) {
			tokens->load(syntax.ptr.get());
		}

		if (syntax.type >= 0) {
			return import_term(syntax, tokens);
		} else {
			return import_expression(syntax.ptr->get<expression>(), tokens);
		}
	}

	virtual vector<T> import_arguments(operation op, const vector<expression::argument> &syntax, tokenizer *tokens) {
		vector<T> result;
		for (size_t i = 0; i < syntax.size(); i++) {
			result.push_back(import_argument(syntax[i], tokens));
		}
		return result;
	}

	T import_expression(const expression &syntax, tokenizer *tokens) {
		if (tokens != NULL) {
			tokens->load(&syntax);
		}

		if (syntax.type < 0) {
			if (tokens != NULL) {
				tokens->internal("unrecognized operation", __FILE__, __LINE__);
			} else {
				internal("", "unrecognized operation", __FILE__, __LINE__);
			}
			return T();
		}

		if (syntax.operators.empty()) {
			if (syntax.arguments.size() == 1u) {
				return import_argument(syntax.arguments[0], tokens);
			} else {
				internal("", "malformed expression", __FILE__, __LINE__);
			}
		}

		T result;

		operation op = syntax.operators[0];
		push_properties(op, syntax.arguments, tokens);
		if (syntax.isGroup()) {
			result = import_group(op, import_arguments(op, syntax.arguments, tokens), tokens);
		} else if (syntax.isModifier()) {
			result = import_modifier(op, import_arguments(op, syntax.arguments, tokens), tokens);
		} else if (syntax.isTernary()) {
			result = import_ternary(op, import_arguments(op, syntax.arguments, tokens), tokens);
		} else if (syntax.isBinary() or syntax.isUnary()) {
			if (not syntax.arguments.empty()) {
				result = import_argument(syntax.arguments[0], tokens);
			}

			if (syntax.arguments.size() == 1u) {
				if (syntax.isUnary()) {
					for (int i = (int)syntax.operators.size()-1; i >= 0; i--) {
						result = import_unary(syntax.operators[i], result, tokens);
					}
				}
			} else {
				for (size_t i = 1; i < syntax.arguments.size(); i++) {
					T sub = import_argument(syntax.arguments[i], tokens);
					result = import_binary(syntax.operators[i-1], result, sub, tokens);
				}
			}
		}
		pop_properties(op);
		return result;
	}
};

template <typename T, typename L>
struct LValuedImporter {
	virtual T L_to_T(L lval, tokenizer *tokens) const = 0;
	virtual L T_to_L(T expr, tokenizer *tokens) const = 0;

	virtual bool is_lvalue(const expression &syntax) const = 0;

	virtual L import_term(const expression::argument &syntax, tokenizer *tokens) const = 0;

	virtual void push_properties(operation op, const vector<expression::argument> &args, tokenizer *tokens) {
		// default behavior is NOP
	}

	virtual void pop_properties(operation op) {
		// default behavior is NOP
	}

	virtual L import_unary(operation op, L expr, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("unary operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "unary operations not supported by interpreter", __FILE__, __LINE__);
		}
		return expr;
	}

	virtual L import_binary(operation op, L left, L right, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("binary operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "binary operations not supported by interpreter", __FILE__, __LINE__);
		}
		return left;
	}

	virtual L import_ternary(operation op, vector<L> args, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("binary operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "binary operations not supported by interpreter", __FILE__, __LINE__);
		}
		if (args.empty()) {
			return L();
		}
		return args[0];
	}

	virtual L import_group(operation op, vector<L> args, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("group operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "group operations not supported by interpreter", __FILE__, __LINE__);
		}
		if (not args.empty()) {
			return args[0];
		}
		return L();
	}

	virtual L import_modifier(operation op, vector<L> args, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("modifier operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "modifier operations not supported by interpreter", __FILE__, __LINE__);
		}
		if (not args.empty()) {
			return args[0];
		}
		return L();
	}

	virtual T import_unary(operation op, T expr, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("unary operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "unary operations not supported by interpreter", __FILE__, __LINE__);
		}
		return expr;
	}

	virtual T import_binary(operation op, T left, T right, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("binary operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "binary operations not supported by interpreter", __FILE__, __LINE__);
		}
		return left;
	}

	virtual T import_ternary(operation op, vector<T> args, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("binary operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "binary operations not supported by interpreter", __FILE__, __LINE__);
		}
		if (args.empty()) {
			return T();
		}
		return args[0];
	}

	virtual T import_group(operation op, vector<T> args, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("group operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "group operations not supported by interpreter", __FILE__, __LINE__);
		}
		if (not args.empty()) {
			return args[0];
		}
		return T();
	}

	virtual T import_modifier(operation op, vector<T> args, tokenizer *tokens) const {
		if (tokens != nullptr) {
			tokens->internal("modifier operations not supported by interpreter", __FILE__, __LINE__);
		} else {
			internal("", "modifier operations not supported by interpreter", __FILE__, __LINE__);
		}
		if (not args.empty()) {
			return args[0];
		}
		return T();
	}

	// These probably don't need an overide
	virtual L import_L_argument(const expression::argument &syntax, tokenizer *tokens) {
		if (tokens != NULL) {
			tokens->load(syntax.ptr.get());
		}

		if (syntax.type >= 0) {
			return import_term(syntax, tokens);
		} else {
			const expression &expr = syntax.ptr->get<expression>();
			if (not expr.operators.empty() and not is_lvalue(expr)) {
				return T_to_L(import_expression(expr, tokens), tokens);
			}
			return import_lvalue(expr, tokens);
		}
	}

	virtual vector<L> import_L_arguments(operation op, const vector<expression::argument> &syntax, tokenizer *tokens) {
		vector<L> result;
		for (size_t i = 0; i < syntax.size(); i++) {
			result.push_back(import_L_argument(syntax[i], tokens));
		}
		return result;
	}

	L import_lvalue(const expression &syntax, tokenizer *tokens) {
		if (tokens != NULL) {
			tokens->load(&syntax);
		}

		if (syntax.type < 0) {
			if (tokens != NULL) {
				tokens->internal("unrecognized operation", __FILE__, __LINE__);
			} else {
				internal("", "unrecognized operation", __FILE__, __LINE__);
			}
			return L();
		}

		if (syntax.operators.empty()) {
			if (syntax.arguments.size() == 1u) {
				return import_L_argument(syntax.arguments[0], tokens);
			} else {
				internal("", "malformed expression", __FILE__, __LINE__);
			}
		}

		L result;

		operation op = syntax.operators[0];
		push_properties(op, syntax.arguments, tokens);
		if (syntax.isGroup()) {
			result = import_group(op, import_L_arguments(op, syntax.arguments, tokens), tokens);
		} else if (syntax.isModifier()) {
			result = import_modifier(op, import_L_arguments(op, syntax.arguments, tokens), tokens);
		} else if (syntax.isTernary()) {
			result = import_ternary(op, import_L_arguments(op, syntax.arguments, tokens), tokens);
		} else if (syntax.isBinary() or syntax.isUnary()) {
			if (not syntax.arguments.empty()) {
				result = import_L_argument(syntax.arguments[0], tokens);
			}

			if (syntax.arguments.size() == 1u) {
				if (syntax.isUnary()) {
					for (int i = (int)syntax.operators.size()-1; i >= 0; i--) {
						result = import_unary(syntax.operators[i], result, tokens);
					}
				}
			} else {
				for (size_t i = 1; i < syntax.arguments.size(); i++) {
					L sub = import_L_argument(syntax.arguments[i], tokens);
					result = import_binary(syntax.operators[i-1], result, sub, tokens);
				}
			}
		}
		pop_properties(op);
		return result;
	}

	virtual T import_T_argument(const expression::argument &syntax, tokenizer *tokens) {
		if (tokens != NULL) {
			tokens->load(syntax.ptr.get());
		}

		if (syntax.type >= 0) {
			return L_to_T(import_term(syntax, tokens), tokens);
		} else {
			const expression &expr = syntax.ptr->get<expression>();
			if (not expr.operators.empty() and is_lvalue(expr)) {
				return L_to_T(import_lvalue(expr, tokens), tokens);
			}
			return import_expression(expr, tokens);
		}
	}

	virtual vector<T> import_T_arguments(operation op, const vector<expression::argument> &syntax, tokenizer *tokens) {
		vector<T> result;
		for (size_t i = 0; i < syntax.size(); i++) {
			result.push_back(import_T_argument(syntax[i], tokens));
		}
		return result;
	}

	T import_expression(const expression &syntax, tokenizer *tokens) {
		if (tokens != NULL) {
			tokens->load(&syntax);
		}

		if (syntax.type < 0) {
			if (tokens != NULL) {
				tokens->internal("unrecognized operation", __FILE__, __LINE__);
			} else {
				internal("", "unrecognized operation", __FILE__, __LINE__);
			}
			return T();
		}

		if (syntax.operators.empty()) {
			if (syntax.arguments.size() == 1u) {
				return import_T_argument(syntax.arguments[0], tokens);
			} else {
				internal("", "malformed expression", __FILE__, __LINE__);
			}
		}

		T result;

		operation op = syntax.operators[0];
		push_properties(op, syntax.arguments, tokens);
		if (syntax.isGroup()) {
			result = import_group(op, import_T_arguments(op, syntax.arguments, tokens), tokens);
		} else if (syntax.isModifier()) {
			result = import_modifier(op, import_T_arguments(op, syntax.arguments, tokens), tokens);
		} else if (syntax.isTernary()) {
			result = import_ternary(op, import_T_arguments(op, syntax.arguments, tokens), tokens);
		} else if (syntax.isBinary() or syntax.isUnary()) {
			if (not syntax.arguments.empty()) {
				result = import_T_argument(syntax.arguments[0], tokens);
			}

			if (syntax.arguments.size() == 1u) {
				if (syntax.isUnary()) {
					for (int i = (int)syntax.operators.size()-1; i >= 0; i--) {
						result = import_unary(syntax.operators[i], result, tokens);
					}
				}
			} else {
				for (size_t i = 1; i < syntax.arguments.size(); i++) {
					T sub = import_T_argument(syntax.arguments[i], tokens);
					result = import_binary(syntax.operators[i-1], result, sub, tokens);
				}
			}
		}
		pop_properties(op);
		return result;
	}
};


}
