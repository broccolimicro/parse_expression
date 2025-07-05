/*
 * composition.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include "assignment.h"

#ifndef parse_expression_composition_h
#define parse_expression_composition_h

namespace parse_expression
{
struct composition : parse::syntax
{
	composition();
	composition(tokenizer &tokens, int level = 0, void *data = NULL);
	~composition();

	static void init();

	vector<composition> compositions;
	vector<assignment> literals;
	vector<expression> guards;
	string region;
	int level;

	static vector<string> precedence;
	static int get_level(string operation);

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	string to_string(int prev_level, string tab = "") const;
	parse::syntax *clone() const;
};
}

#endif
