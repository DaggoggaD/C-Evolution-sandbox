#include "Parser.h"

//Extern of Parser.h
TokenType AllowedPrimaryTokTypes[] = { IDENTIFIER, INT, DOUBLE, STRING, CHAR, UNKNOWN };

ParserTypes FindParserTokType(TokenType TokType) {
	switch (TokType)
	{
		case(IDENTIFIER):
			return IDENTIFIER_P;
			break;

		case(INT): case(DOUBLE):
			return NUMBER;
			break;

		case(STRING):
			return STRING_P;
			break;
		default:
			//ADD ERROR PATTERNS
			return;
			break;
	}
}

bool IsPrimaryTok(TOKEN Tok, ParserTypes* Type) {
	int i = 0;
	while (AllowedPrimaryTokTypes[i] != UNKNOWN) {
		if (Tok.Type == AllowedPrimaryTokTypes) {
			*Type = FindParserTokType(Tok.Type);
			return true;
		}
		i++;
	}
	return false;
}

void Analyze() {
	
	ParserTypes OutType;
	Expression CurrExpr;

	if (IsPrimaryTok(CurrToken, &OutType)) {

	}

}

void Advance() {
	CurrToken = NextToken;
	if (TokensFirst->next == NULL) return; //ADD ERROR PATTERNS
	
	TokensFirst = TokensFirst->next;
	NextToken = TokensFirst->next->Tok;
}

void Parse() {
	
	CurrToken = TokensFirst->Tok;
	if (TokensFirst->next != NULL) NextToken = TokensFirst->next->Tok;
	
	while (TokensFirst != NULL) {
		Analyze();
	}

}