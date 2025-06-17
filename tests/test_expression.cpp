#include <gtest/gtest.h>
#include <parse/default/line_comment.h>
#include <parse/default/block_comment.h>
#include <parse_expression/expression.h>
#include <sstream>
#include <string>

#include "helpers.h"

using namespace std;
using namespace parse_expression;

/*class GlobalTestEnvironment : public ::testing::Environment {
public:
	void SetUp() override {
		// Runs once before all tests
		printf("Global setup\n");
	}

	void TearDown() override {
		// Runs once after all tests
		printf("Global teardown\n");
	}
};

// Static initializer to register environment
namespace {
struct GlobalEnvInitializer {
	GlobalEnvInitializer() {
		::testing::AddGlobalTestEnvironment(new GlobalTestEnvironment);
	}
} initializer;
}*/

TEST(ExpressionParser, BasicBooleanOperations) {
	// Test simple AND, OR, NOT operations
	string test_code = "a & b | ~c";

	expression::register_precedence(createPrecedence());

	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	expression::register_syntax(tokens);
	tokens.insert("basic_boolean", test_code);
	
	expression expr(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(expr.valid);
	EXPECT_EQ(expr.to_string(), "a&b|~c");
}

TEST(ExpressionParser, OperatorPrecedence) {
	// Test that operator precedence is correctly handled
	string test_code = "a & b | c & d";
	
	expression::register_precedence(createPrecedence());
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	expression::register_syntax(tokens);
	tokens.insert("precedence_test", test_code);
	
	expression expr(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(expr.valid);
	EXPECT_EQ(expr.to_string(), "a&b|c&d");
}

TEST(ExpressionParser, ParenthesesGrouping) {
	// Test parentheses for grouping
	string test_code = "(a | b) & c";
	
	expression::register_precedence(createPrecedence());
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	expression::register_syntax(tokens);
	tokens.insert("parentheses_test", test_code);
	
	expression expr(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(expr.valid);
	EXPECT_EQ(expr.to_string(), "(a|b)&c");
}

TEST(ExpressionParser, ComplexVariableNames) {
	// Test complex variable names with dots and slices
	string test_code = "module.sub.signal[3] & another.signal[1:5][c::x:a+b]";
	
	expression::register_precedence(createPrecedence());
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	expression::register_syntax(tokens);
	tokens.insert("variable_name_test", test_code);
	
	expression expr(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(expr.valid);
	EXPECT_EQ(expr.to_string(), "module.sub.signal[3]&another.signal[1:5][c::x:a+b]");
}

TEST(ExpressionParser, NestedExpressions) {
	// Test nested expressions with multiple levels
	string test_code = "a & (b | (c & d)) | ~e";
	
	expression::register_precedence(createPrecedence());
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	expression::register_syntax(tokens);
	tokens.insert("nested_expr", test_code);
	
	expression expr(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(expr.valid);
	EXPECT_EQ(expr.to_string(), "a&(b|(c&d))|~e");
}

TEST(ExpressionParser, ComplexBooleanExpressions) {
	// Test complex boolean expressions with multiple operators
	string test_code = "a & ~b | (c & d & ~e) | (f | ~g)";
	
	expression::register_precedence(createPrecedence());
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	expression::register_syntax(tokens);
	tokens.insert("complex_boolean_test", test_code);
	
	expression expr(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(expr.valid);
	EXPECT_EQ(expr.to_string(), "a&~b|(c&d&~e)|(f|~g)");
}

TEST(ExpressionParser, ArithmeticAndComparison) {
	// Test arithmetic and comparison operators
	string test_code = "(a + b) * c < (d - e) & x == y";
	
	expression::register_precedence(createPrecedence());
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	expression::register_syntax(tokens);
	tokens.insert("arithmetic_comparison_test", test_code);
	
	expression expr(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(expr.valid);
	EXPECT_EQ(expr.to_string(), "(a+b)*c<(d-e)&x==y");
}

TEST(ExpressionParser, UnaryOperators) {
	// Test unary operators
	string test_code = "~a & +b & -c";
	
	expression::register_precedence(createPrecedence());
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	expression::register_syntax(tokens);
	tokens.insert("unary_test", test_code);
	
	expression expr(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(expr.valid);
	EXPECT_EQ(expr.to_string(), "~a&+b&-c");
}

TEST(ExpressionParser, Numbers) {
	// Test numbers in expressions
	string test_code = "a & 42 | b & 0";
	
	expression::register_precedence(createPrecedence());
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	expression::register_syntax(tokens);
	tokens.insert("numbers_test", test_code);
	
	expression expr(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(expr.valid);
	EXPECT_EQ(expr.to_string(), "a&42|b&0");
}

TEST(ExpressionParser, Constants) {
	// Test constants like vdd and gnd
	string test_code = "signal & vdd | variable & gnd";
	
	expression::register_precedence(createPrecedence());
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	expression::register_syntax(tokens);
	tokens.insert("constants_test", test_code);
	
	expression expr(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(expr.valid);
	EXPECT_EQ(expr.to_string(), "signal&vdd|variable&gnd");
}

TEST(ExpressionParser, ErrorHandling) {
	// Test how the parser handles malformed expressions
	vector<string> error_cases = {
		"a &",		   // Missing right operand
		"& b",		   // Missing left operand
		"a & (b | c"	 // Missing closing parenthesis
	};
	
	expression::register_precedence(createPrecedence());
	
	for (const auto& error_case : error_cases) {
		tokenizer tokens;
		tokens.register_token<parse::block_comment>(false);
		tokens.register_token<parse::line_comment>(false);
		expression::register_syntax(tokens);
		tokens.insert("error_test", error_case);
		
		expression expr(tokens);
		EXPECT_FALSE(tokens.is_clean());
	}
}

TEST(ExpressionParser, Function) {
	// Test numbers in expressions
	string test_code = "a+y.x[3].myfunc(x, y)[3].z";
	
	expression::register_precedence(createPrecedence());
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	expression::register_syntax(tokens);
	tokens.insert("function_test", test_code);
	
	expression expr(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(expr.valid);
	EXPECT_EQ(expr.to_string(), "a+y.x[3].myfunc(x,y)[3].z");
}

