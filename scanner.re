
/*
 +----------------------------------------------------------------------+
 | Zephir Language                                                      |
 +----------------------------------------------------------------------+
 | Copyright (c) 2013 Zephir Team                                       |
 +----------------------------------------------------------------------+
 | This source file is subject to version 1.0 of the Zephir license,    |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.zephir-lang.com/license                                   |
 |                                                                      |
 | If you did not receive a copy of the Zephir license and are unable   |
 | to obtain it through the world-wide-web, please send a note to       |
 | license@zephir-lang.com so we can mail you a copy immediately.       |
 +----------------------------------------------------------------------+
*/

#include "string.h"
#include "scanner.h"

#define YYCTYPE unsigned char
#define YYCURSOR (s->start)
#define YYLIMIT (s->end)
#define YYMARKER q

int xx_get_token(xx_scanner_state *s, xx_scanner_token *token) {

	char next, *q = YYCURSOR, *start = YYCURSOR;
	int status = XX_SCANNER_RETCODE_IMPOSSIBLE;

	while (XX_SCANNER_RETCODE_IMPOSSIBLE == status) {

		/*!re2c
		re2c:indent:top = 2;
		re2c:yyfill:enable = 0;

		INTEGER = [\-]?[0-9]+;
		INTEGER {
			token->opcode = XX_T_INTEGER;
			token->value = strndup(start, YYCURSOR - start);
			token->len = YYCURSOR - start;
			s->active_char += (YYCURSOR - start);
			q = YYCURSOR;
			return 0;
		}

		DOUBLE = ([\-]?[0-9]+[\.][0-9]+);
		DOUBLE {
			token->opcode = XX_T_DOUBLE;
			token->value = strndup(start, YYCURSOR - start);
			token->len = YYCURSOR - start;
			s->active_char += (YYCURSOR - start);
			q = YYCURSOR;
			return 0;
		}

		'null' {
			s->active_char += sizeof("null")-1;
			token->opcode = XX_T_NULL;
			return 0;
		}

		'false' {
			s->active_char += sizeof("false")-1;
			token->opcode = XX_T_FALSE;
			return 0;
		}

		'true' {
			s->active_char += sizeof("true")-1;
			token->opcode = XX_T_TRUE;
			return 0;
		}

		'namespace' {
			s->active_char += sizeof("namespace")-1;
			token->opcode = XX_T_NAMESPACE;
			return 0;
		}

		'abstract' {
			s->active_char += sizeof("abstract")-1;
			token->opcode = XX_T_ABSTRACT;
			return 0;
		}

		'class' {
			s->active_char += sizeof("class")-1;
			token->opcode = XX_T_CLASS;
			return 0;
		}

		'extends' {
			s->active_char += sizeof("extends")-1;
			token->opcode = XX_T_EXTENDS;
			return 0;
		}

		'public' {
			s->active_char += sizeof("public")-1;
			token->opcode = XX_T_PUBLIC;
			return 0;
		}

		'protected' {
			s->active_char += sizeof("protected")-1;
			token->opcode = XX_T_PROTECTED;
			return 0;
		}

		'function' {
			s->active_char += sizeof("function")-1;
			token->opcode = XX_T_FUNCTION;
			return 0;
		}

		'fn' {
			s->active_char += sizeof("fn")-1;
			token->opcode = XX_T_FUNCTION;
			return 0;
		}

		'let' {
			s->active_char += sizeof("let")-1;
			token->opcode = XX_T_LET;
			return 0;
		}

		'echo' {
			s->active_char += sizeof("echo")-1;
			token->opcode = XX_T_ECHO;
			return 0;
		}

		'const' {
			s->active_char += sizeof("const")-1;
			token->opcode = XX_T_CONST;
			return 0;
		}

		'int' {
			s->active_char += sizeof("int")-1;
			token->opcode = XX_T_TYPE_INTEGER;
			return 0;
		}

		'long' {
			s->active_char += sizeof("long")-1;
			token->opcode = XX_T_TYPE_INTEGER;
			return 0;
		}

		'double' {
			s->active_char += sizeof("double")-1;
			token->opcode = XX_T_TYPE_DOUBLE;
			return 0;
		}

		'float' {
			s->active_char += sizeof("float")-1;
			token->opcode = XX_T_TYPE_DOUBLE;
			return 0;
		}

		'bool' {
			s->active_char += sizeof("bool")-1;
			token->opcode = XX_T_TYPE_BOOL;
			return 0;
		}

		'boolean' {
			s->active_char += sizeof("boolean")-1;
			token->opcode = XX_T_TYPE_BOOL;
			return 0;
		}

		'string' {
			s->active_char += sizeof("string")-1;
			token->opcode = XX_T_TYPE_STRING;
			return 0;
		}

		'var' {
			s->active_char += sizeof("var")-1;
			token->opcode = XX_T_TYPE_VAR;
			return 0;
		}

		'if' {
			s->active_char += sizeof("if")-1;
			token->opcode = XX_T_IF;
			return 0;
		}

		'else' {
			s->active_char += sizeof("else")-1;
			token->opcode = XX_T_ELSE;
			return 0;
		}

		'while' {
			s->active_char += sizeof("while")-1;
			token->opcode = XX_T_WHILE;
			return 0;
		}

		'for' {
			s->active_char += sizeof("for")-1;
			token->opcode = XX_T_FOR;
			return 0;
		}

		'in' {
			s->active_char += sizeof("in")-1;
			token->opcode = XX_T_IN;
			return 0;
		}

		'new' {
			s->active_char += sizeof("new")-1;
			token->opcode = XX_T_NEW;
			return 0;
		}

		'return' {
			s->active_char += sizeof("return")-1;
			token->opcode = XX_T_RETURN;
			return 0;
		}

		'loop' {
			token->opcode = XX_T_LOOP;
			s->active_char += sizeof("loop")-1;
			return 0;
		}

		'break' {
			token->opcode = XX_T_BREAK;
			s->active_char += sizeof("break")-1;
			return 0;
		}

		'continue' {
			token->opcode = XX_T_CONTINUE;
			s->active_char += sizeof("continue")-1;
			return 0;
		}

		'typeof' {
			token->opcode = XX_T_TYPEOF;
			s->active_char += sizeof("typeof")-1;
			return 0;
		}

		'instanceof' {
			token->opcode = XX_T_INSTANCEOF;
			s->active_char += sizeof("instanceof")-1;
			return 0;
		}

		'isset' {
			token->opcode = XX_T_ISSET;
			s->active_char += sizeof("isset")-1;
			return 0;
		}

		'unset' {
			token->opcode = XX_T_UNSET;
			s->active_char += sizeof("unset")-1;
			return 0;
		}

		'throw' {
			token->opcode = XX_T_THROW;
			s->active_char += sizeof("throw")-1;
			return 0;
		}

		'fetch' {
			token->opcode = XX_T_FETCH;
			s->active_char += sizeof("fetch")-1;
			return 0;
		}

		STRING = (["] ([\\]["]|[\\].|[\001-\377]\[\\"])* ["])|(['] ([\\][']|[\\].|[\001-\377]\[\\'])* [']);
		STRING {
			token->opcode = XX_T_STRING;
			token->value = strndup(q, YYCURSOR - q - 1);
			token->len = YYCURSOR - q - 1;
			s->active_char += (YYCURSOR - start);
			q = YYCURSOR;
			return 0;
		}

		COMMENT = ("/*"([^*]+|[*]+[^/*])*[*]*"*/"|"//"[^\r\n]*);
		COMMENT {
			token->opcode = XX_T_COMMENT;
			token->value = strndup(q, YYCURSOR - q - 1);
			token->len = YYCURSOR - q - 1;
			{
				int k, ch = s->active_char;
				for (k = 0; k < (token->len - 1); k++) {
					if (token->value[k] == '\n') {
						ch = 1;
						s->active_line++;
					} else {
						ch++;
					}
				}
				s->active_char = ch;
			}
			q = YYCURSOR;
			return 0;
		}

		IDENTIFIER = [\\\_]?[\_a-zA-Z\\][a-zA-Z0-9\_\\]*;
		IDENTIFIER {
			token->opcode = XX_T_IDENTIFIER;
			token->value = strndup(start, YYCURSOR - start);
			token->len = YYCURSOR - start;
			s->active_char += (YYCURSOR - start);
			q = YYCURSOR;
			return 0;
		}

		"(" {
			s->active_char++;
			token->opcode = XX_T_PARENTHESES_OPEN;
			return 0;
		}

		")" {
			s->active_char++;
			token->opcode = XX_T_PARENTHESES_CLOSE;
			return 0;
		}

		"{" {
			s->active_char++;
			token->opcode = XX_T_BRACKET_OPEN;
			return 0;
		}

		"}" {
			s->active_char++;
			token->opcode = XX_T_BRACKET_CLOSE;
			return 0;
		}

		"[" {
			s->active_char++;
			token->opcode = XX_T_SBRACKET_OPEN;
			return 0;
		}

		"]" {
			s->active_char++;
			token->opcode = XX_T_SBRACKET_CLOSE;
			return 0;
		}

		"@" {
			s->active_char++;
			token->opcode = XX_T_AT;
			return 0;
		}

		"!" {
			s->active_char++;
			token->opcode = XX_T_NOT;
			return 0;
		}

		"=" {
			s->active_char++;
			token->opcode = XX_T_ASSIGN;
			return 0;
		}

		"==" {
			s->active_char += 2;
			token->opcode = XX_T_EQUALS;
			return 0;
		}

		"!=" {
			s->active_char += 2;
			token->opcode = XX_T_NOTEQUALS;
			return 0;
		}

		"===" {
			s->active_char += 3;
			token->opcode = XX_T_IDENTICAL;
			return 0;
		}

		"!==" {
			s->active_char += 3;
			token->opcode = XX_T_NOTIDENTICAL;
			return 0;
		}

		"<" {
			s->active_char++;
			token->opcode = XX_T_LESS;
			return 0;
		}

		">" {
			s->active_char++;
			token->opcode = XX_T_GREATER;
			return 0;
		}

		"->" {
			s->active_char += 2;
			token->opcode = XX_T_ARROW;
			return 0;
		}

		"." {
			s->active_char++;
			token->opcode = XX_T_DOT;
			return 0;
		}

		"+" {
			s->active_char++;
			token->opcode = XX_T_ADD;
			return 0;
		}

		"-" {
			s->active_char++;
			token->opcode = XX_T_SUB;
			return 0;
		}

		"++" {
			s->active_char += 2;
			token->opcode = XX_T_INCR;
			return 0;
		}

		"--" {
			s->active_char += 2;
			token->opcode = XX_T_DECR;
			return 0;
		}

		":" {
			s->active_char++;
			token->opcode = XX_T_COLON;
			return 0;
		}

		";" {
			s->active_char++;
			token->opcode = XX_T_DOTCOMMA;
			return 0;
		}

		"," {
			s->active_char++;
			token->opcode = XX_T_COMMA;
			return 0;
		}

		[ \t\r]+ {
			s->active_char += (YYCURSOR - start);
			token->opcode = XX_T_IGNORE;
			return 0;
		}

		[\n] {
			s->active_line++;
			s->active_char = 0;
			token->opcode = XX_T_IGNORE;
			return 0;
		}

		"\000" {
			status = XX_SCANNER_RETCODE_EOF;
			break;
		}

		[^] {
			status = XX_SCANNER_RETCODE_ERR;
			break;
		}

		*/
	}

	return status;
}
