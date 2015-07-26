/*
 * expression.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include <parse_ucs/variable_name.h>

#ifndef parse_expression_expression_h
#define parse_expression_expression_h

namespace parse_expression
{
using parse_ucs::variable_name;

struct operation_set
{
	operation_set();
	operation_set(int type);
	~operation_set();

	vector<string> symbols;
	int type;

	enum
	{
		left_unary = 0,
		right_unary = 1,
		binary = 2
	};
};

struct argument;

struct expression : parse::syntax
{
	expression();
	expression(tokenizer &tokens, int level = 0, void *data = NULL);
	~expression();

	void init();

	vector<argument> arguments;
	vector<string> operations;

	string region;
	int level;

	static vector<operation_set> precedence;
	static int get_level(string operation);
	static bool level_has(int level, string operation);

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	string to_string(int prev_level, string tab = "") const;
	parse::syntax *clone() const;
};

struct argument
{
	argument();
	argument(expression sub);
	argument(variable_name literal);
	argument(string constant);
	~argument();

	expression sub;
	variable_name literal;
	string constant;

	string to_string(int level = -1, string tab = "") const;
};
}

#endif
