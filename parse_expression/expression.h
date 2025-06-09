#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>

namespace parse_expression {

struct operation {
	operation();
	operation(string prefix, string trigger, string infix, string postfix);
	~operation();

	string prefix;
	string trigger;
	string infix;
	string postfix;

	bool is(string prefix, string trigger, string infix, string postfix) const;
};

bool operator==(operation o0, operation o1);
bool operator!=(operation o0, operation o1);

struct operation_set {
	operation_set();
	operation_set(int type);
	~operation_set();

	// The types determine how the operators in a level should be parsed
	enum {
		UNDEF    = -1, // This is an error case
		TERNARY  =  0, // Uses trigger and infix
		BINARY   =  1, // Uses infix
		UNARY    =  2, // Uses prefix
		// array in dex, call, isochronic region
		MODIFIER =  3, // Uses trigger, infix, and postfix
		// array
		GROUP    =  4  // Uses prefix, infix, and postfix
	};

	int type;
	vector<operation> symbols;

	void push(string prefix, string trigger, string infix, string postfix);
	void push(operation op);
	int find(operation op);
};

struct argument;

struct expression : parse::syntax {
	expression();
	expression(tokenizer &tokens, int level = 0, void *data = NULL);
	~expression();

	void init();

	vector<argument> arguments;
	vector<int> operators;

	int level;
	
	static vector<operation_set> precedence;

	bool isTernary() const;
	bool isBinary() const;
	bool isUnary() const;
	bool isModifier() const;
	bool isGroup() const;

	static pair<int, int> find(int type, string prefix, string trigger, string infix, string postfix);
	const vector<operation> &symbols() const;
	const operation &symbol(int i) const;

	void expectLiteral(tokenizer &tokens);
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

