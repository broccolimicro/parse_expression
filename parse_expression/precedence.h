#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>

namespace parse_expression {

struct operation {
	enum ArgType {
		MODULE = 0,
		TYPE = 1,
		TERM = 2,
		LABEL = 3,
		LITERAL = 4
	};

	operation();
	operation(string prefix, string trigger, string infix, string postfix, ArgType leftType=LITERAL, ArgType rightType=LITERAL);
	~operation();

	ArgType leftType;
	ArgType rightType;

	string prefix;
	string trigger;
	string infix;
	string postfix;

	bool is(string prefix, string trigger, string infix, string postfix) const;

	string to_string() const;
};

ostream &operator<<(ostream &os, const operation &o);

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
		UNARY    =  2, // Uses prefix or postfix
		// array index, call, isochronic region
		MODIFIER =  3, // Uses prefix, trigger, infix, and postfix
		// array
		GROUP    =  4  // Uses prefix, infix, and postfix
	};

	int type;
	vector<operation> symbols;

	void push(string prefix, string trigger, string infix, string postfix, operation::ArgType leftType=operation::LITERAL, operation::ArgType rightType=operation::LITERAL);
	void push(operation op);
	int find(operation op) const;
};

ostream &operator<<(ostream &os, const operation_set &s);

struct precedence_set {
	precedence_set();
	~precedence_set();

	struct index {
		int level;
		int index;
	};

	vector<operation_set> operations;

	int type(int level) const;
	bool typeIs(int level, int t) const;

	bool isTernary(int level) const;
	bool isBinary(int level) const;
	bool isUnary(int level) const;
	bool isModifier(int level) const;
	bool isGroup(int level) const;

	index find(int type, string prefix, string trigger, string infix, string postfix) const;
	const vector<operation> &at(int level) const;
	const operation &at(int level, int idx) const;
	const operation &at(index i) const;

	void push(int type);
	void push_back(string prefix, string trigger, string infix, string postfix, operation::ArgType leftType=operation::LITERAL, operation::ArgType rightType=operation::LITERAL);

	bool isValidLevel(int level) const;

	size_t size() const;
	bool empty() const;
};

ostream &operator<<(ostream &os, const precedence_set &s);

}

