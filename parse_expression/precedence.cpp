/*
 * expression.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "expression.h"
#include <parse/default/symbol.h>
#include <parse/default/number.h>
#include <parse/default/instance.h>
#include <parse/default/white_space.h>

namespace parse_expression
{

operation::operation() {
}

operation::operation(string prefix, string trigger, string infix, string postfix) {
	this->prefix = prefix;
	this->trigger = trigger;
	this->infix = infix;
	this->postfix = postfix;
}

operation::~operation() {
}

bool operation::is(string prefix, string trigger, string infix, string postfix) const {
	return this->prefix == prefix and this->trigger == trigger and this->infix == infix and this->postfix == postfix;
}

string operation::to_string() const {
	string result;
	if (not trigger.empty()) {
		result = "a" + trigger;
		if (not infix.empty()) {
			result += infix + "b";
		}
		result += postfix;
	} else {
		result += prefix + "a";
		if (not infix.empty()) {
			result += infix + "b";
		}
		result += postfix;
	}
	return result;
}

ostream &operator<<(ostream &os, operation &o) {
	os << o.to_string();
	return os;
}

bool operator==(operation o0, operation o1) {
	return o0.prefix == o1.prefix and o0.trigger == o1.trigger and o0.infix == o1.infix and o0.postfix == o1.postfix;
}

bool operator!=(operation o0, operation o1) {
	return not (o0 == o1);
}

operation_set::operation_set() {
	type = -1;
}

operation_set::operation_set(int type) {
	this->type = type;
}

operation_set::~operation_set() {
}

void operation_set::push(string prefix, string trigger, string infix, string postfix) {
	push(operation(prefix, trigger, infix, postfix));
}

void operation_set::push(operation op) {
	symbols.push_back(op);
}

int operation_set::find(operation op) const {
	for (int i = 0; i < (int)symbols.size(); i++) {
		if (symbols[i] == op) {
			return i;
		}
	}
	return -1;
}

precedence_set::precedence_set() {
}

precedence_set::~precedence_set() {
}

bool precedence_set::isTernary(int level) const {
	return operations[level].type == operation_set::TERNARY;
}

bool precedence_set::isBinary(int level) const {
	return operations[level].type == operation_set::BINARY;
}

bool precedence_set::isUnary(int level) const {
	return operations[level].type == operation_set::UNARY;
}

bool precedence_set::isModifier(int level) const {
	return operations[level].type == operation_set::MODIFIER;
}

bool precedence_set::isGroup(int level) const {
	return operations[level].type == operation_set::GROUP;
}

precedence_set::index precedence_set::find(int type, string prefix, string trigger, string infix, string postfix) const {
	operation search(prefix, trigger, infix, postfix);
	for (int i = 0; i < (int)operations.size(); i++) {
		if (type == operations[i].type) {
			int j = operations[i].find(search);
			if (j >= 0) {
				return {i, j};
			}
		}
	}

	return {-1, -1};
}

const vector<operation> &precedence_set::at(int level) const {
	return operations[level].symbols;
}

const operation &precedence_set::at(int level, int idx) const {
	return operations[level].symbols[idx];
}

const operation &precedence_set::at(precedence_set::index i) const {
	return operations[i.level].symbols[i.index];
}

void precedence_set::push(int type) {
	operations.push_back(operation_set(type));
}

void precedence_set::push_back(string prefix, string trigger, string infix, string postfix) {
	operations.back().push(prefix, trigger, infix, postfix);
}

bool precedence_set::isValidLevel(int level) const {
	return level >= 0 and level < (int)operations.size();
}

size_t precedence_set::size() const {
	return operations.size();
}

}
