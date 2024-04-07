#include "parser.h"

map<string, bool> defVar;
map<string, Token> SymTable;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);

//PrintStmt:= PRINT *, ExpreList 
bool PrintStmt(istream& in, int& line) {
	LexItem t;
	
	t = Parser::GetNextToken(in, line);
	
 	if( t != DEF ) {
		
		ParseError(line, "Print statement syntax error.");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	
	if( t != COMMA ) {
		
		ParseError(line, "Missing Comma.");
		return false;
	}
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}
	return ex;
}//End of PrintStmt


//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	
	status = Expr(in, line);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		status = ExprList(in, line);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of ExprList

//Prog ::= PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line){
    LexItem t;
    t = Parser::GetNextToken(in, line);
    if(t != PROGRAM){
        ParseError(line, "Program syntax error, missing PROGRAM token.");
        return false;
    }
	t = Parser::GetNextToken(in, line);
    if(t != IDENT){
        ParseError(line, "Program syntax error, missing IDENT token.");
        return false;
    }
    bool decl = Decl(in, line);
    if(!decl){
        ParseError(line, "Program syntax error, missing declaration.");
        return false;
    }
    //How to handle optional stmts
    bool stmt = Stmt(in, line);
    if(!stmt){
        ParseError(line, "Program syntax error, missing statment.");
        return false;
    }
    while(t != END){
        t = Parser::GetNextToken(in, line);
    }
    t = Parser::GetNextToken(in, line);
    if(t != PROGRAM){
        ParseError(line, "Program syntax error, missing (final) PROGRAM token.");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if(t != IDENT){
        ParseError(line, "Program syntax error, missing (final) IDENT token.");
        return false;
    }
    return true;
}
bool Decl(istream& in, int& line){
	return true;
}
//Type ::= INTEGER | REAL | CHARARACTER [(LEN = ICONST)]
bool Type(istream& in, int& line){
	LexItem t;
	t = Parser::GetNextToken(in, line);
	if(t != INTEGER | t != REAL | t != CHARACTER){
		ParseError(line, "Type syntax error, missing INTEGER/REAL/CHARACTER token.");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if(t == LPAREN){
		t = Parser::GetNextToken(in, line);
		if(t != LEN){
			ParseError(line, "Type syntax error, missing LEN token.");
			return false;
		}
		t = Parser::GetNextToken(in, line);
		if(t != ASSOP){
			ParseError(line, "Type syntax error, missing ASSOP token.");
			return false;
		}
		t = Parser::GetNextToken(in, line);
		if(t != ICONST){
			ParseError(line, "Type syntax error, missing ICONST token.");
			return false;
		}
		t = Parser::GetNextToken(in, line);
		if(t != RPAREN){
			ParseError(line, "Type syntax error, missing RPAREN token.");
			return false;
		}
	}
	else{
		Parser::PushBackToken(t);
	}
	return true;
}
bool VarList(istream& in, int& line){
	return true;
}
bool Stmt(istream& in, int& line){
	return true;
}
bool SimpleStmt(istream& in, int& line){
	return true;
}
//bool PrintStmt(istream& in, int& line);
bool BlockIfStmt(istream& in, int& line){
	return true;
}
bool SimpleIfStmt(istream& in, int& line){
	return true;
}
bool AssignStmt(istream& in, int& line){
	return true;
}
//Var ::= IDENT
bool Var(istream& in, int& line){
	LexItem t;
	t = Parser::GetNextToken(in, line);
	if(t != IDENT){
		ParseError(line, "Var syntax error, missing IDENT token.");
		return false;
	}
	return true;
}
//bool ExprList(istream& in, int& line);
bool RelExpr(istream& in, int& line){
	return true;
}
bool Expr(istream& in, int& line){
	return true;
}
bool MultExpr(istream& in, int& line){
	return true;
}
bool TermExpr(istream& in, int& line){
	return true;
}
//SFactor ::= [+ | -] Factor
bool SFactor(istream& in, int& line){
	LexItem t;
	t = Parser::GetNextToken(in, line);
	if(t == PLUS | t == MINUS){
		t = Parser::GetNextToken(in, line);
	}
	else{
		Parser::PushBackToken(t);
	}
	bool factor = Factor(in, line, 0);
	if(!factor){
		ParseError(line, "SFactor syntax error, missing factor.");
		return false;
	}
	return true;
}
bool Factor(istream& in, int& line, int sign){

	return true;
}
//int ErrCount();