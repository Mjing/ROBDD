# ROBDD
To compile the program:
```sh
$make main
```
this will create robdd.out which is main executable.

## Syntax
CFG::=
prop_list : prop | prop_list prop
prop : not prop | ( prop and prop) | atom

I have used lex and bison for parser generation:
To generate lexicon parser:
```sh
$ make lex
```
To generate CFG parser:
```sh
make yacc
```
