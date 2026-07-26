#pragma once

#include <parse/parse.h>
#include <parse/factory.h>

namespace parse_expression {

struct operation {
	operation();
	operation(string prefix, string trigger, string infix, string postfix, std::vector<int> leftType=std::vector<int>(), std::vector<int> rightType=std::vector<int>());
	~operation();

	// DESIGN(edward.bingham) only used by modifiers
	// TODO(edward.bingham) should I try to support the other types?
	std::vector<int> leftType; // not used
	std::vector<int> rightType;

	string prefix;
	string trigger;
	string infix;
	string postfix;

	bool is(string prefix, string trigger, string infix, string postfix) const;
	bool empty() const;

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

	void push(string prefix, string trigger, string infix, string postfix, std::vector<int> leftType=std::vector<int>(), std::vector<int> rightType=std::vector<int>());
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

	bool isTernary(int level) const;
	bool isBinary(int level) const;
	bool isUnary(int level) const;
	bool isModifier(int level) const;
	bool isGroup(int level) const;

	index find(int type, string prefix, string trigger, string infix, string postfix) const;
	index find(int type, operation op) const;
	const vector<operation> &at(int level) const;
	const operation &at(int level, int idx) const;
	const operation &at(index i) const;

	void push(int type);
	void push_back(string prefix, string trigger, string infix, string postfix, std::vector<int> leftType=std::vector<int>(), std::vector<int> rightType=std::vector<int>());

	bool isValidLevel(int level) const;

	size_t size() const;
	bool empty() const;
};

ostream &operator<<(ostream &os, const precedence_set &s);

struct config {
	vector<pair<std::string, parse::factory> > literals;
	vector<int> base;
	precedence_set order;
	int lvalueLevel;

	config(std::initializer_list<pair<std::string, parse::factory> > literals={});
	~config();

	template <typename T>
	int push(std::string label, std::any data=std::any()) {
		int result = (int)literals.size();
		literals.push_back({label, parse::factory(parse::schema::from<T>(), data)});
		return result;
	}

	void set_lvalue();

	void register_syntax(tokenizer &tokens);
};

struct context {
	std::shared_ptr<config> cfg;
	int level;

	context();
	context(std::shared_ptr<config> cfg, int level=0);
	~context();
};

}

