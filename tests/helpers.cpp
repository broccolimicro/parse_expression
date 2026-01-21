#include "helpers.h"

using namespace parse_expression;

precedence_set createPrecedence() {
	precedence_set result;
	result.push(operation_set::GROUP);
	result.push_back("[", "", ",", "]");

	result.push(operation_set::TERNARY);
	result.push_back("", "@", ":", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "|", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "&", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "^", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "==", "");
	result.push_back("", "", "~=", "");
	result.push_back("", "", "<", "");
	result.push_back("", "", ">", "");
	result.push_back("", "", "<=", "");
	result.push_back("", "", ">=", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "||", "");
	
	result.push(operation_set::BINARY);
	result.push_back("", "", "&&", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "^^", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "<<", "");
	result.push_back("", "", ">>", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "+", "");
	result.push_back("", "", "-", "");

	result.push(operation_set::BINARY);
	result.push_back("", "", "*", "");
	result.push_back("", "", "/", "");
	result.push_back("", "", "%", "");

	result.push(operation_set::UNARY);
	result.push_back("!", "", "", "");
	result.push_back("~", "", "", "");
	result.push_back("+", "", "", "");
	result.push_back("-", "", "", "");

	result.push(operation_set::MODIFIER);
	result.push_back("", "!", "", "");

	result.push(operation_set::UNARY);
	result.push_back("#", "", "", "");
	result.push_back("", "", "", "?");

	result.push(operation_set::MODIFIER);
	result.push_back("", "'", "", "", operation::LITERAL, operation::LABEL);

	result.push(operation_set::MODIFIER);
	result.push_back("", "(", ",", ")");
	result.push_back("", ".", "", "", operation::LITERAL, operation::LABEL);
	result.push_back("", "[", ":", "]");
	result.push_back("", "::", "", "", operation::LITERAL, operation::LABEL);
	return result;
}

