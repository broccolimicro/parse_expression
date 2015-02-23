libparse_boolean
This library provides a parser for boolean expressions, either represented as an 
expression with operators &, |, and ~ or as an assignment with operators + or -.

Literal characters and strings are given in single quotes. Parentheses indicate 
grouping when needed. Square Brackets enclose an optional group of items. Vertical 
bars separate alternatives. Star indicates that the group should be repeated.

disjunction: conjunction ['|' conjunction ]*
conjunction: complement ['&'  complement ]*
complement: ['~']* (number | variable_name | '(' disjunction ')')
internal_choice: internal_parallel [':' internal_parallel]*
internal_parallel: assignment [',' assignment ]*
assignment: variable_name '+' | variable_name '-' | '(' internal_choice ')'
variable_name: member_name [ '.' member_name ]*
member_name: instance [ slice ]*
slice: '[' number [ '..' number ] ']'

Dependencies:
parse
	common