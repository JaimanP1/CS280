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
	t = Parser::GetNextToken(in, line);
	while(t.GetToken() == INTEGER || t.GetToken() == REAL || t.GetToken() == CHARACTER){
		Parser::PushBackToken(t);
		cout << t << line << "first part of decl while call in prog" << endl;	
		bool decl = Decl(in, line);
		if(!decl){
			ParseError(line, "Program syntax error, Decl is false.");
			return false;
		}
		t = Parser::GetNextToken(in, line); ////////checkpoint to see if decl are calling
		cout << t << line << "second part of decl while call in prog" << endl;
	}
    while(t != END){
		if(t == PROGRAM){
			ParseError(line, "Program syntax error, missing END token.");
			return false;
		}
		Parser::PushBackToken(t);
		cout << t << "first part of stmt while call in prog" << endl;
		bool stmt = Stmt(in, line);
		if(!stmt){
			ParseError(line, "Program syntax error, Stmt is false.");
			return false;
		}
		t = Parser::GetNextToken(in, line);
		cout << t << "second part of stmt while call in prog" << endl;
	}
	t = Parser::GetNextToken(in, line);
	if(t != PROGRAM){
		ParseError(line, "Program syntax error, missing (second) PROGRAM token.");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if(t != IDENT){
		ParseError(line, "Program syntax error, missing IDENT token.");
		return false;
	}
	cout << "(DONE)" << endl;
    return true;
}
//Decl ::= Type :: VarList
bool Decl(istream& in, int& line){
	bool type = Type(in, line);
	if(!type){
		ParseError(line, "Decl syntax error, Type is false.");
		return false;
	}
	LexItem t;
	t = Parser::GetNextToken(in, line);
	cout << t << line << "in decl method" << endl;
	if(t != DCOLON){
		ParseError(line, "Decl syntax error, missing DCOLON token.");
		return false;
	}
	bool varlist = VarList(in, line);
	if(!varlist){
		ParseError(line, "Decl syntax error, VarList is false.");
		return false;
	}
	return true;
}
//Type ::= INTEGER | REAL | CHARARACTER [(LEN = ICONST)]
bool Type(istream& in, int& line){
	LexItem t;
	t = Parser::GetNextToken(in, line);
	if(t != INTEGER && t != REAL && t != CHARACTER){
		ParseError(line, "Type syntax error, missing INTEGER/REAL/CHARACTER token.");
		//cout << t << line << endl;
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
//VarList ::= Var [= Expr] {, Var [= Expr]}
bool VarList(istream& in, int& line){
	int lineIndex = line;
	bool var = Var(in, line);
	if(!var){
		ParseError(line, "VarList syntax error, Var is false.");
		return false;
	}
	LexItem t;
	t = Parser::GetNextToken(in, line);
	cout << t << line << "Varlist method" << endl;
	if(t == ASSOP){
		bool expr = Expr(in, line);
		if(!expr){
			ParseError(line, "VarList syntax error, Expr is false.");
			return false;
		}
		t = Parser::GetNextToken(in, line);
	}
	if(t == COMMA){
		bool varlist = VarList(in, line);
		if(!varlist){
			ParseError(line, "VarList syntax error, VarList is false.");
			return false;
		}
		return true;   /////////////do I need this return statement??
	}
	if(t == IDENT && t.GetLinenum() == lineIndex){
		ParseError(line, "VarList syntax error, missing COMMA token.");
		return false;
	}
	else{
		Parser::PushBackToken(t);
	}
	//cout << t << line << "VarList test" << endl;
	//Parser::PushBackToken(t); /////////////////////////////////////////PLEASE DEAR GOD
	//cout << t << line << "VarList end" << endl;
	return true;
}
//Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
bool Stmt(istream& in, int& line){
	LexItem t;
	t = Parser::GetNextToken(in, line);
	cout << t << line << "stmt method" << endl;
	if(t == IDENT){
		Parser::PushBackToken(t);
		bool assignstmt = AssignStmt(in, line);
		if(!assignstmt){
			ParseError(line, "Stmt syntax error, AssignStmt is false.");
			return false;
		}
		cout << "AssignStmt successfully called." << endl;
	}
	if(t == IF){
		cout << "SimpleIfStmt called." << endl;
		return true;
	}
	if(t == LPAREN){
		cout << "BlockIfStmt called." << endl;
		return true;
	}
	if(t == PRINT){
		cout << "PrintStmt called." << endl;
		return true;
	}
	else{
		ParseError(line, "Stmt syntax error, AssignStmt/BlockIfStmt/SimpleIfStmt/PrintStmt is false.");
		return false;
	}
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
//AssignStmt ::= Var = Expr
bool AssignStmt(istream& in, int& line){
	bool var = Var(in, line);
	if(!var){
		ParseError(line, "AssignStmt syntax error, Var is false.");
		return false;
	}
	LexItem t;
	t = Parser::GetNextToken(in, line);
	cout << t << line << "assignstmt method" << endl;
	if(t != ASSOP){
		ParseError(line, "AssignStmt syntax error, missing ASSOP token.");
		return false;
	}
	bool expr = Expr(in, line);
	if(!expr){
		ParseError(line, "AssignStmt syntax error, Expr is false.");
		return false;
	}
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
	cout << t << line << "Var method" << endl;
	return true;
}
//bool ExprList(istream& in, int& line);
//RelExpr ::= Expr [ ( == | < | > ) Expr ]
bool RelExpr(istream& in, int& line){
	bool expr = Expr(in, line);
	if(!expr){
		ParseError(line, "RelExpr syntax error, Expr is false.");
		return false;
	}
	LexItem t;
	t = Parser::GetNextToken(in, line);
	if(t == EQ || t == GTHAN || t == LTHAN){
		expr = Expr(in, line);
		if(!expr){
			ParseError(line, "RelExpr syntax error, (second) Expr is false");
			return false;
		}
	}
	else{
		Parser::PushBackToken(t);
	}
	return true;
}
//Expr ::= MultExpr { ( + | - | // ) MultExpr }
bool Expr(istream& in, int& line){
	bool multexpr = MultExpr(in, line);
	if(!multexpr){
		ParseError(line, "Expr syntax error, MultExpr is false.");
		return false;
	}
	LexItem t;
	t = Parser::GetNextToken(in, line);
	if(t == PLUS || t == MINUS || t == CAT){
		return Expr(in, line);
	}
	else{
		Parser::PushBackToken(t);
	}
	return true;
}
//MultExpr ::= TermExpr { ( * | / ) TermExpr }
bool MultExpr(istream& in, int& line){
	bool termexpr = TermExpr(in, line);
	if(!termexpr){
		ParseError(line, "MultExpr syntax error, TermExpr is false.");
		return false;
	}
	LexItem t;
	t = Parser::GetNextToken(in, line);
	if(t == MULT || t == DIV){
		return MultExpr(in, line);
	}
	else{
		Parser::PushBackToken(t);
	}
	return true;
}
//TermExpr ::= SFactor { ** SFactor }
bool TermExpr(istream& in, int& line){
	bool sfactor = SFactor(in, line);
	if(!sfactor){
		ParseError(line, "TermExpr syntax error, no SFactor.");
		return false;
	}
	LexItem t;
	t = Parser::GetNextToken(in, line);
	if(t == POW){
		t = Parser::GetNextToken(in, line);
		sfactor = SFactor(in, line);
		if(!sfactor){
			ParseError(line, "TermExpr syntax error, no (second) SFactor.");
			return false;
		}
		return true;
	}
	else{
		Parser::PushBackToken(t);
	}
	return true;
}
//SFactor ::= [+ | -] Factor
bool SFactor(istream& in, int& line){
	LexItem t;
	t = Parser::GetNextToken(in, line);
	if(t == PLUS || t == MINUS){
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
//Factor ::= IDENT | ICONST | RCONST | SCONST | (Expr)
bool Factor(istream& in, int& line, int sign){
	LexItem t;
	t = Parser::GetNextToken(in, line);
	if(t == IDENT || t == ICONST || t == RCONST || t == SCONST){
		return true;
	}
	bool expr = Expr(in, line);
	if(expr){
		return true;
	}
	else{
		ParseError(line, "Factor syntax error, missing IDENT/ICONST/RCONST/SCONST token or Expr is false.");
		return false;
	}
	return true;
}
//int ErrCount();