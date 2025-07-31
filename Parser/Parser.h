 #pragma once
#include "..\Utils\Utils.h"

TOKEN CurrToken;
TOKEN NextToken;

extern TokenType AllowedPrimaryTokTypes[];

typedef enum {
	NONE,

	//Primary
	IDENTIFIER_P,
	NUMBER,
	STRING_P,
	TRUE,
	FALSE,
	NUL,
	GROUP_EXPR,
	
	//Unary
	NOT,
	NEGATIVE,
	
	//Factor
	MULT,
	DIV,
	
	//Term
	PLUS,
	MINUS,
	
	//Comparison
	GREATER,
	LESS,
	GREATER_EQUAL,
	LESS_EQUAL,

	//Expression
	NOT_EQUALS,
	EQUALS,
	ASSIGN
} ParserTypes;

typedef struct Expression;

typedef struct {
	ParserTypes Type;
	TOKEN Tok;
	struct Expression* GroupExpression;
} Primary;

typedef struct {
	ParserTypes Type;
	Primary PrimaryValue;
} Unary;

typedef struct Factor{
	Unary Left;
	ParserTypes Type;
	struct Factor* Right;
} Factor;

typedef struct Term{
	Factor Left;
	ParserTypes Type;
	struct Term* Right;
} Term;

typedef struct Comparison{
	Term Left;
	ParserTypes Type;
	struct Comparison* Right;
} Comparison;

typedef struct Expression{
	Comparison Left;
	ParserTypes Type;
	struct Expression* Right;
} Expression;

void Parse();