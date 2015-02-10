/*
 * guard.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>

#ifndef parse_boolean_guard_h
#define parse_boolean_guard_h

namespace parse_boolean
{
struct guard : parse::syntax
{
	guard();
	guard(tokenizer &tokens, int i = 0, void *data = NULL);
	guard(const guard &g);
	~guard();

	vector<parse::syntax*> operands;
	vector<string> functions;
	int level;

	void clear();

	void parse(tokenizer &tokens, int i = 0, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;

	guard &operator=(const guard &g);
};
}

#endif
