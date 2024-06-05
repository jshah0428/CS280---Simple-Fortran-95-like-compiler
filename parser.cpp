



//#include "parser.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;
//include "lex.cpp"
//#include "prog2.cpp"
#include "parser.h"


map<string, bool> defVar;
//map<string, Token> SymTable;
LexItem j;
namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		cout<<j<<endl;
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







//Prog ::= PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT 

bool Prog(istream& in, int& line){

	j = Parser::GetNextToken(in,line);
	
	if (j!=PROGRAM){
		ParseError(line, "Invalid Start");
		return false;
	} //atp you assume that the first word was PROGRAM

	j = Parser::GetNextToken(in,line);
	
	if (j!=IDENT){
		ParseError(line, "IDENT not given next");
		return false;
	} //atp you know an ident was given as well

	j = Parser::GetNextToken(in,line);
	

	while (true){ //check this one. !!
		bool a = Decl(in,line);
		if(!a){
			break;
		}
		
		j = Parser::GetNextToken(in,line);
	}
	while (true){
		bool b = Stmt(in,line);
		if(!b){
			break;
		}
		//j = Parser::GetNextToken(in,line);
	}
	j = Parser::GetNextToken(in,line);
	
	if(j!=END){
		ParseError(line, "Missing End Statement");
		return false;
	}
	
	j = Parser::GetNextToken(in,line);

	if (j!=PROGRAM){
		ParseError(line, "Missing Program at end");
		return false;
	} //atp you assume that the first word was PROGRAM

	j = Parser::GetNextToken(in,line);

	if (j!=IDENT){
		ParseError(line, "IDENT not given next");
		return false;
	} //atp you know an ident was given as well
	


	return true;
	// while (j!=DONE){
	// 	j= Parser::GetNextToken(in,line);
	// 	cout<<j<<endl;
		
	// }
	
    // exit(0);
	// return true;
}

bool Decl(istream& in, int& line){
	//check over this, especially the recursion part. 
	//bool status = true;

	if (!Type(in,line)){ //still the same j variable from the Program element. 
		//ParseError(line, "Invalid Type Declaration");
		return false;
	} //established that type works
	//LexItem varcheck = j; //checks what type of variable it is, integer, real, Character

	j = Parser::GetNextToken(in,line);

	if(j != DCOLON){
		ParseError(line, "Missing Double Colons");
		return false;
	}//now we assume that the :: exists as well. 

	j = Parser::GetNextToken(in,line);
	if(!VarList(in, line)){ //have already gotten the token, Sending the token in.
		ParseError(line, "Incorrect Variable Declaration");
		return false;
	}
	
	//j = Parser::GetNextToken(in,line);
	// if(Type(in,line)){ //this might be a wrong condition to check. 
	// 	status = Decl(in,line); //check this recursion, could work. this is saying if the type exists, do it again. 
	// }

	return true;

	
	

}//this function should be recursive, can be true for multiple data types, ie real and then characters. Or recursion could probably go in the previous method, idrk.

bool Type(istream& in, int& line){
	if (j != INTEGER && j != REAL&&j!= CHARACTER){
		//ParseError(line,"type issue");
		return false;

	} //not an error, it is valid.


	if (j == CHARACTER){ //to check if they are doing the length thing. //this has to be in declaration. 
		j = Parser::GetNextToken(in,line);
		if (j == LPAREN){ //(LEN = 20)
			j = Parser::GetNextToken(in,line);
			if(j==LEN){
				j = Parser::GetNextToken(in,line);
				if(j == ASSOP){
					j = Parser::GetNextToken(in,line);
					if (j == ICONST){
						j = Parser::GetNextToken(in,line);
						if(j == RPAREN){
							cout<<"Definition of Strings with length of " + j.GetLexeme() + " in declaration statement.";
						}
						else{
							Parser::PushBackToken(j);
						}
					}
					else{

						Parser::PushBackToken(j);
						ParseError(line, "Incorrect length Declaration");
						return false;
					}
				}
				else{
					Parser::PushBackToken(j);
				}
			}
			else{
				Parser::PushBackToken(j);
			}
		}
		else{
			Parser::PushBackToken(j);
		}

	}//may have to include error message stuff here, potentially. 
	return true;

	
}

//VarList ::= Var [= Expr] {, Var [= Expr]}
//a =10,b,c=10
bool VarList(istream& in, int& line){ //r
	bool status = true;
	
	LexItem p = j;//this should be b

	if(!Var(in,line)){
		ParseError(line, "Unusable Variable");
		return false;
	}//if it makes it here, VAR EXISTS. 


	j = Parser::GetNextToken(in,line);
	

	if(j == ASSOP){ //MIGHT HAVE TO DO TYPE CHECKING HERE. X
		j = Parser::GetNextToken(in,line); 
		if (!Expr(in,line)){ //j is already incremented to send to expr
			ParseError(line, "Invalid Expression"); //might have to change this error statement
			return false;
		}//atp assumes that Expr is true.  
		
	}
	else{
		Parser:: PushBackToken(j);

	}

	
	j = Parser::GetNextToken(in,line);//it equals b, and then goes to a
	
	
	if (j == COMMA){
		j = Parser::GetNextToken(in,line);
		status = VarList(in,line);

	}
	else if ((j==IDENT && p.GetLinenum() ==j.GetLinenum()) || j == ERR){
		if(!Var(in,line)){
			return false;
		}
		ParseError(line,"Missing comma");
		return false;

	}
	else{
		Parser:: PushBackToken(j);
	}
	return status;
}

bool Stmt(istream& in, int& line){
	//return false;
	//figure 
	//j = Parser::GetNextToken(in,line); - not necessary because already happens on the top. Confirm one more time. 
	if (!PrintStmt(in,line)&&!AssignStmt(in,line)&&!BlockIfStmt(in,line)){
		
	
			//&& !SimpleIfStmt(in,line) check this in the blockifstmt 
				return false; 
			//make sure to delete this later
				
			
			
		
	}
	return true;
	//j = Parser::GetNextToken(in,line); not necessary comes from prog. 

	//figure out a way to call this multiple times. 

}



//PrintStmt:= PRINT *, ExpreList 
bool PrintStmt(istream& in, int& line) {
	//LexItem t;
	
	//j; //confirm if this is correct. 
	if (j!=PRINT){
		return false;
	}
	j = Parser::GetNextToken(in,line);
	
 	if( j != DEF ) {
		
		ParseError(line, "Print statement syntax error.");
		return false;
	}

	j = Parser::GetNextToken(in, line);
	
	if( j != COMMA ) {
		
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


//BlockIfStmt ::= IF (RelExpr) THEN {Stmt} [ELSE {Stmt}] END IF
bool BlockIfStmt(istream& in, int& line){
	if (j!=IF){
		return false;
	}//assumes if is here, and we are in a 
	
	j = Parser::GetNextToken(in,line);
	if (j!= LPAREN){
		ParseError(line, "Missing left parentheses within if statement ");
	}//Assumes IF (

	j = Parser::GetNextToken(in,line);
	if(!RelExpr(in,line)){ //relexpr will give the parse error so its fine. 
		return false;
	}//now we have IF (RelExpr

	j = Parser::GetNextToken(in,line);
	if (j!= RPAREN){
		ParseError(line, "Missing right parentheses within if statement ");
	}//now we have IF (RelExpr)

	j = Parser::GetNextToken(in,line);
	if (j!=THEN){
		if(!SimpleIfStmt(in,line)){ //parse error will be given here. 
			return false;
		}
		else if (SimpleIfStmt(in,line)){ //means that the simple if statement is right, so this must be right. 
			return true;
		}
	}//now we have IF (RelExpr) THEN

	j = Parser::GetNextToken(in,line);
	//bool status = true;


	while (true){
		if(!Stmt(in,line)){
			Parser::PushBackToken(j);
			break;
		}
		j = Parser::GetNextToken(in,line);
	}//now we have IF (RelExpr) THEN {Stmt}
	j = Parser::GetNextToken(in,line);

	if (j == ELSE){ //now we have IF (RelExpr) THEN {Stmt} or //now we have IF (RelExpr) THEN ELSE
		j = Parser::GetNextToken(in,line);

		while(true){ //this may also work while(Stmt(in, line));
			if(!Stmt(in,line)){
				Parser::PushBackToken(j);
				break;
			}//now we have IF (RelExpr) THEN ELSE {STMT}
			j = Parser::GetNextToken(in,line);
			
		}
	}

	j = Parser::GetNextToken(in,line);
	if(j != END){
		ParseError(line, "missing End Statement");
		return false;
	}

	j = Parser::GetNextToken(in,line);
	if (j!= IF){
		ParseError(line, "Missing Ending If");
		return false;
	}
	return true;

}

bool SimpleIfStmt(istream& in, int& line){
	if(!SimpleStmt(in,line)){
		ParseError(line, "SimpleIFStmt error");
		return false;
	} //means there is a SimpleStmt
	return true;
}

bool SimpleStmt(istream& in, int& line){
	if (!AssignStmt(in,line) && !PrintStmt(in,line)){ //in either of those methods, there is a parse error. 
		ParseError(line, "SimpleStmt error");
		return false;
	}
	return true;
}

//AssignStmt ::= Var = Expr
bool AssignStmt(istream& in, int& line){ //You have to do type conversion here somewhere
	// if (!Var(in,line)){
	// 	ParseError(line, "Assignment problem");	
	// 	return false;
	// }

	j = Parser:: GetNextToken(in,line);
	if (j!=ASSOP){
		ParseError(line, "Missing Assignment operator");
		return false;
	}
	j = Parser:: GetNextToken(in,line);
	if (!Expr(in,line)){
		ParseError(line, "EXPR is wrong in the Assignstmt");
		return false;
	}
	return true;
	



}

bool Var(istream& in, int& line){
	if (j == IDENT){
		if (defVar.find(j.GetLexeme()) == defVar.end()){
			defVar[j.GetLexeme()] = true;
			return true;
		}
		else{
			ParseError(line, "Reusing a variable");
			return false;
		}
	}
	else{
		return false;
	}
}

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
//End of ExprList

//RelExpr ::= Expr [ ( == | < | > ) Expr ]
bool RelExpr(istream& in, int& line){
	if (!(Expr(in,line))){
		ParseError(line,"missing expression statement");
		return false;
	}
	
	j = Parser:: GetNextToken(in,line);
	if(j == EQ || j == LTHAN || j == GTHAN){
		j = Parser::GetNextToken(in,line);
		if (!(Expr(in,line))){
			ParseError(line, "error in expr in rel expr");
			return false;
		}


	}
	else{
		Parser:: PushBackToken(j);
	}
 

	return true;

}

bool Expr(std::istream& in, int& line) { //all of the operator's here will be left associative, so built get ter,inal for left part, and then add the operators. 
	//return true; //change this
	if(!MultExpr(in,line)){ //still using same token as defined in var list 
		ParseError(line, "Incorrect Multexpr");//change this error message
		return false;
	}//means that a mult expr exists here. 
	j = Parser:: GetNextToken(in,line);
	
		while (true){
			if(j == PLUS || j == MINUS || j == CAT){
				j = Parser:: GetNextToken(in,line);
				if (!(MultExpr(in,line))){
					ParseError(line, "Issue in Expr");
					return false;
				}
			}
			else{
				Parser::PushBackToken(j);
				break;
			}
			j = Parser:: GetNextToken(in,line);
		}
	
	return true;


}

bool MultExpr(istream& in, int& line){
	if (!TermExpr(in,line)){
		ParseError(line, "TermExpr issue");//change this error message
		return false;
	}

	j = Parser::GetNextToken(in,line);

	while(true){
		if(j == MULT || j == DIV){
			j = Parser:: GetNextToken(in,line);

			if (!(TermExpr(in,line))){
				ParseError(line, "Issue in MultExpr");
				return false;
			}
		}
		else{
			Parser::PushBackToken(j);
			break;
		}
		j = Parser:: GetNextToken(in,line);
	}

	return true;


}

bool TermExpr(istream& in, int& line){
	if(!SFactor(in,line)){
		ParseError(line, "Sfactor error");
		return false;
	}
	j = Parser::GetNextToken(in,line);

	while (true){
		if (j == POW){
			j = Parser::GetNextToken(in,line);
			if (!SFactor(in,line)){
				ParseError(line, "Issue in TermExpr");
				return false;
			}
		}
		else{
			Parser::PushBackToken(j);
			break;
			
		}
		j = Parser::GetNextToken(in,line);
	}

	return true;
}

bool SFactor(istream& in, int& line){
	if (j == PLUS || j == MINUS){
		if (!Factor(in,line,1)){ //CHANGE FOR THE INTERPRETER.
			ParseError(line, "SFactor error");
			return false;
		}

	}
	else{
		Parser:: PushBackToken(j);
	}
	j = Parser::GetNextToken(in,line);
	
	if (!Factor(in,line,1)){
		ParseError(line, "Sfactor outer if statement error. ");
		return false;
	}
	return true;

}

bool Factor(istream& in, int& line, int sign){
	if (j!= IDENT&&j!= ICONST && j!= RCONST && j!= SCONST){

		if (j == LPAREN){
			j = Parser:: GetNextToken(in,line);
			if (!Expr(in,line)){
				ParseError(line, "problem with expr inside of factor");
				return false;
			}
			j = Parser::GetNextToken(in,line);
			if(j!=RPAREN){
				ParseError(line,"Missing right parentheses in facotor function");
				return false;
			}
			else{
				return true;
			}
		}
		else{
			return false;
		}
	}
	return true;
	

}




// if(!Decl(in,line)){
// 		return false;
// 	}//Note: already sends the get next token character this assumes that all variable declarations were correct. From here, go to variable declaration section.
		
// 	j = Parser::GetNextToken(in,line);
	
// 		if(!Stmt(in,line)){ //token already being sent here.  //how to recurse this.
// 			if (j!=END){
// 				return false;
// 			}
// 			else{
// 				Parser::PushBackToken(j);
// 			}
// 		}
// 		j = Parser::GetNextToken(in,line);

// 	j = Parser::GetNextToken(in,line);

