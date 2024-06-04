# parse_expression

This library provides a parser for boolean expressions, either represented as an 
expression with operators &, |, and ~ or as an assignment with operators + or -.

Literal characters and strings are given in single quotes. Parentheses indicate 
grouping when needed. Square Brackets enclose an optional group of items. Vertical 
bars separate alternatives. Star indicates that the group should be repeated.

```
disjunction: conjunction ['|' conjunction ]*
conjunction: complement ['&'  complement ]*
complement: ['~']* (number | variable_name | '(' disjunction ')')
internal_choice: internal_parallel [':' internal_parallel]*
internal_parallel: assignment [',' assignment ]*
assignment: variable_name '+' | variable_name '-' | '(' internal_choice ')'
variable_name: member_name [ '.' member_name ]*
member_name: instance [ slice ]*
slice: '[' number [ '..' number ] ']'
```

**Dependencies**:

 - parse
   - common

## License

Licensed by Cornell University under GNU GPL v3.

Written by Ned Bingham.
Copyright © 2020 Cornell University.

Haystack is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Haystack is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License may be found in COPYRIGHT.
Otherwise, see <https://www.gnu.org/licenses/>.

