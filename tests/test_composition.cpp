#include <gtest/gtest.h>
#include <parse/default/line_comment.h>
#include <parse/default/block_comment.h>
#include <parse_expression/assignment.h>
#include <parse_expression/composition.h>
#include <sstream>
#include <string>

using namespace std;
using namespace parse_expression;

//==============================================================================
// Assignment Tests
//==============================================================================

TEST(AssignmentParser, BasicAssignment) {
	// Test basic assignment with + operation
	string test_code = "a+";
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	assignment::register_syntax(tokens);
	tokens.insert("assignment_test", test_code);
	
	assignment assign(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(assign.valid);
	EXPECT_EQ(assign.lvalue.size(), 1u);
	EXPECT_EQ(assign.operation, "+");
	EXPECT_EQ(assign.to_string(), "a+");
}

TEST(AssignmentParser, RemovalOperation) {
	string test_code = "a-";
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	assignment::register_syntax(tokens);
	tokens.insert("removal_test", test_code);
	
	assignment assign(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(assign.valid);
	EXPECT_EQ(assign.to_string(), "a-");
}

TEST(AssignmentParser, ComplexVariableName) {
	string test_code = "module.signal[3]+";
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	assignment::register_syntax(tokens);
	tokens.insert("complex_var_test", test_code);
	
	assignment assign(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(assign.valid);
	EXPECT_EQ(assign.to_string(), "module.signal[3]+");
}

TEST(AssignmentParser, AssignmentWithExpression) {
	// Test assignment with expression
	string test_code = "d = a & b | c";
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	assignment::register_syntax(tokens);
	tokens.insert("assignment_expr_test", test_code);
	
	assignment assign(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(assign.valid);
	EXPECT_EQ(assign.to_string(), "d=a&b|c");
}

TEST(AssignmentParser, FunctionCall) {
	// Test multiple assignments with expressions
	string test_code = "v1=f0(a & b, c | d)";
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	assignment::register_syntax(tokens);
	tokens.insert("function_call", test_code);
	
	assignment assign(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(assign.valid);
	EXPECT_EQ(assign.to_string(), "v1=f0(a&b,c|d)");
}

//==============================================================================
// Composition Tests
//==============================================================================

TEST(CompositionParser, ParallelComposition) {
	// Test parallel composition (,)
	string test_code = "a+, b+, c-";
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	composition::register_syntax(tokens);
	tokens.insert("parallel_test", test_code);
	
	composition comp(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(comp.valid);
	EXPECT_EQ(comp.to_string(), "a+,b+,c-");
}

TEST(CompositionParser, InternalChoice) {
	// Test internal choice (:)
	string test_code = "(a+) : (b-)";
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	composition::register_syntax(tokens);
	tokens.insert("choice_test", test_code);
	
	composition comp(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(comp.valid);
	EXPECT_EQ(comp.to_string(), "(a+):(b-)");
}

TEST(CompositionParser, NestedComposition) {
	// Test nested compositions
	string test_code = "(a+, b+) : (c-, (d+ : e+))";
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	composition::register_syntax(tokens);
	tokens.insert("nested_test", test_code);
	
	composition comp(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(comp.valid);
	EXPECT_EQ(comp.to_string(), "(a+,b+):(c-,(d+):(e+))");
}

TEST(CompositionParser, ComplexComposition) {
	// Test a complex composition structure
	string test_code = "(a = x & y, b-) : (e = c & d ? f : g)";
	
	tokenizer tokens;
	tokens.register_token<parse::block_comment>(false);
	tokens.register_token<parse::line_comment>(false);
	composition::register_syntax(tokens);
	tokens.insert("complex_composition", test_code);
	
	composition comp(tokens);
	EXPECT_TRUE(tokens.is_clean());
	EXPECT_TRUE(comp.valid);
	EXPECT_EQ(comp.to_string(), "(a=x&y,b-):(e=c&d?f:g)");
}
