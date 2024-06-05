/* Implementation of Interpreter for the SFort95 Language
 * parserInterp.cpp
 * Programming Assignment 3
 * Spring 2024
 * 
 * Note: Based on assignment specifications, I had to modify my professors parser for the interpreter part of this assignment
*/
#include <stack>

#include "parserInterp.h"
#include "val.cpp" //probably have to get rid of this later.

map<string, bool> defVar;
map<string, Token> SymTable;

map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects
using namespace std;

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
	ValQue = new queue<Value>;
	
	
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
	
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;
	return ex;
}//End of PrintStmt


//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value retVal;
	
	status = Expr(in, line, retVal);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(retVal);
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


bool Prog(istream& in, int& line)
{
	bool dl = false, sl = false;
	LexItem tok = Parser::GetNextToken(in, line);
		
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			dl = Decl(in, line);
			if( !dl  )
			{
				ParseError(line, "Incorrect Declaration in Program");
				return false;
			}
			sl = Stmt(in, line);
			if( !sl  )
			{
				ParseError(line, "Incorrect Statement in program");
				return false;
			}	
			tok = Parser::GetNextToken(in, line);
			
			if (tok.GetToken() == END) {
				tok = Parser::GetNextToken(in, line);
				
				if (tok.GetToken() == PROGRAM) {
					tok = Parser::GetNextToken(in, line);
					
					if (tok.GetToken() == IDENT) {
						cout << "(DONE)" << endl;
						return true;
					}
					else
					{
						ParseError(line, "Missing Program Name");
						return false;
					}	
				}
				else
				{
					ParseError(line, "Missing PROGRAM at the End");
					return false;
				}	
			}
			else
			{
				ParseError(line, "Missing END of Program");
				return false;
			}	
		}
		else
		{
			ParseError(line, "Missing Program name");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	
	else
	{
		ParseError(line, "Missing Program keyword");
		return false;
	}
}


bool Decl(istream& in, int& line) {
	bool status = false;
	LexItem tok;
	string strLen;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	if(t == INTEGER || t == REAL || t == CHARACTER ) { //character comes and is pushed through
        //at this point, t should equal one of these. 
		tok = t; 
		
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == DCOLON) {
			status = VarList(in, line, t, 1); //changed so that I can remember its data type. 
			
			if (status)
			{
				status = Decl(in, line);
				if(!status)
				{
					ParseError(line, "Declaration Syntactic Error.");
					return false;
				}
				return status;
			}
			else
			{
				ParseError(line, "Missing Variables List.");
				return false;
			}
		}
		else if(t == CHARACTER && tok.GetToken() == LPAREN)
		{
			tok = Parser::GetNextToken(in, line);
			
			if(tok.GetToken() == LEN)
			{
				tok = Parser::GetNextToken(in, line);
				
				if(tok.GetToken() == ASSOP)
				{
					tok = Parser::GetNextToken(in, line);
					
					if(tok.GetToken() == ICONST)
					{ 
						strLen = tok.GetLexeme();
						
						tok = Parser::GetNextToken(in, line);
						if(tok.GetToken() == RPAREN)
						{
							tok = Parser::GetNextToken(in, line);
							if(tok.GetToken() == DCOLON)
							{
								status = VarList(in, line,t , stoi(strLen));
								
								if (status)
								{
									//cout << "Definition of Strings with length of " << strLen << " in declaration statement." << endl;
									status = Decl(in, line);
									if(!status)
									{
										ParseError(line, "Declaration Syntactic Error.");
										return false;
									}
									return status;
								}
								else
								{
									ParseError(line, "Missing Variables List.");
									return false;
								}
							}
						}
						else
						{
							ParseError(line, "Missing Right Parenthesis for String Length definition.");
							return false;
						}
					
					}
					else
					{
						ParseError(line, "Incorrect Initialization of a String Length");
						return false;
					}
				}
			}
		}
		else
		{
			ParseError(line, "Missing Double Colons");
			return false;
		}
			
	}
		
	Parser::PushBackToken(t);
	return true;
}//End of Decl

//Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
bool Stmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case PRINT:
		status = PrintStmt(in, line);
		
		if(status)
			status = Stmt(in, line);
		break;

	case IF:
		status = BlockIfStmt(in, line);
		if(status)
			status = Stmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		if(status)
			status = Stmt(in, line);
		break;
		
	
	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}//End of Stmt

bool SimpleStmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case PRINT:
		status = PrintStmt(in, line);
		
		if(!status)
		{
			ParseError(line, "Incorrect Print Statement");
			return false;
		}	
		//cout << "Print statement in a Simple If statement." << endl;
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		if(!status)
		{
			ParseError(line, "Incorrect Assignent Statement");
			return false;
		}
		//cout << "Assignment statement in a Simple If statement." << endl;
			
		break;
		
	
	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}//End of SimpleStmt

//VarList ::= Var [= Expr] {, Var [= Expr]}
bool VarList(istream& in, int& line, LexItem & idtok, int strlen){
	Value retval;

	bool status = false, exprstatus = false;
	string identstr;
	
	//for test 1, last item was a dcolon
	LexItem tok = Parser::GetNextToken(in, line); //'a'
	if(tok == IDENT) //new token is the r
	{
		
		identstr = tok.GetLexeme();
		if (!(defVar.find(identstr)->second)) //this checks if it is in defvar. 
		{ //executes if NOT in defvar.

            SymTable[identstr] = idtok.GetToken(); //this tells us what the data type of the variable is. 
			defVar[identstr] = true;
			if (idtok == CHARACTER){
				retval.SetType(VSTRING);
				retval.SetstrLen(strlen);
				string n;
				for (int i = 0; i <strlen; i++){
					n+=' ';
				}
				retval.SetString(n);
				TempsResults[identstr]= retval;	
			}
			else if (idtok == INTEGER){
				retval.SetType(VINT);
			}
			else if(idtok == REAL){
				retval.SetType(VREAL);
			}
		}	
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
		
	}
	else
	{
		
		ParseError(line, "Missing Variable Name");
		return false;
	}

	LexItem variable_tok = tok;	
	tok = Parser::GetNextToken(in, line);
	if(tok == ASSOP) //test 1 -> = sign
	{

		
		exprstatus = Expr(in, line, retval);
		
		if(!exprstatus)
		{
			ParseError(line, "Incorrect initialization for a variable.");
			return false;
		}
		if (retval.GetType() == VSTRING && idtok== CHARACTER){
			if (retval.GetstrLen()<retval.GetString().length()){
				string n = retval.GetString();
				n = n.substr(0,strlen);
				retval.SetString(n);
			}
			else if(retval.GetstrLen()>retval.GetString().length()){
				int counter = retval.GetString().length();
				string n = retval.GetString();
				while (counter != retval.GetstrLen()){
					n+=' ';
					counter++;
				}
				retval.SetString(n);
			}

		}
		TempsResults[variable_tok.GetLexeme()]= Value(retval);
		
		//cout<< "Initialization of the variable " << identstr <<" in the declaration statement." << endl;
		tok = Parser::GetNextToken(in, line);
		
		if (tok == COMMA) {
			
			status = VarList(in, line, idtok, strlen);
			
		}
		else
		{
			Parser::PushBackToken(tok);
			return true;
		}
	}
	else if (tok == COMMA) {
		
		status = VarList(in, line,idtok, strlen);
	}
	else if(tok == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		
		return false;
	}
	else {
		
		Parser::PushBackToken(tok);
		return true;
	}
	
	return status;
	
}//End of VarList
	
bool BlockIfStmt(istream& in, int& line) {
	bool ex=false, status ; 
	static int nestlevel = 0;
	//int level;
	LexItem t;
	
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	Value retVal;
	ex = RelExpr(in, line, retVal);
	if( !ex ) {
		ParseError(line, "Missing if statement condition");
		return false;
	}

	if(retVal.GetBool() == true){
		
		t = Parser::GetNextToken(in, line);
		if(t != RPAREN ) {
			
			ParseError(line, "Missing Right Parenthesis");
			return false;
		}
		
		t = Parser::GetNextToken(in, line);
		if(t != THEN)
		{
			Parser::PushBackToken(t);
			
			status = SimpleStmt(in, line);
			if(status)
			{
				return true;
			}
			else
			{
				ParseError(line, "If-Stmt Syntax Error");
				return false;
			}
			
		}
		nestlevel++;
		//level = nestlevel;
		status = Stmt(in, line);
		if(!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Then-Part");
			return false;
		}

		while (t!=END){
			t = Parser::GetNextToken(in,line);
		}
	}

	else if (retVal.GetBool() == false){
		while(t!=END && t != ELSE){
			t = Parser::GetNextToken(in,line);
		} //may not be necessary
		if( t == ELSE ) {
			status = Stmt(in, line);
			if(!status)
			{
				ParseError(line, "Missing Statement for If-Stmt Else-Part");
				return false;
			}
			else
			t = Parser::GetNextToken(in, line);
			
		}
	
	}
	
	
	if(t != END ) {
		
		ParseError(line, "Missing END of IF");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if(t == IF ) {
		//cout << "End of Block If statement at nesting level " << level << endl;
		return true;
	}	
	
	Parser::PushBackToken(t);
	ParseError(line, "Missing IF at End of IF statement");
	return false;
}//End of IfStmt function

//Var:= ident
bool Var(istream& in, int& line, LexItem & idtok)
{
	string identstr;
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == IDENT){
		identstr = tok.GetLexeme();
		
		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}
		idtok = tok;	
		return true;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}//End of Var

//AssignStmt:= Var = Expr
bool AssignStmt(istream& in, int& line) {
	//make sure that the value of the operation = the type of the variable. 
	
	bool varstatus = false, status = false;
	LexItem t;
	
    Value retval;

	LexItem idtok; //gives me the id of the token passed
	
	varstatus = Var(in, line, idtok);
	//now, idtok = ident, but idtok.GetLexeme() = the variable name. 
	
	int templine = line;
	if (varstatus){
		
		t = Parser::GetNextToken(in, line);
		
		
		if (t == ASSOP){
			status = Expr(in, line, retval);
			if(!status) {
				ParseError(templine, "Missing Expression in Assignment Statment");
				return status;
			}
			
		}
		else if(t.GetToken() == ERR){
			ParseError(templine, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(templine, "Missing Assignment Operator");
			return false;
		}
	}
	else {
		ParseError(templine, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}


	//real --> real
	//real --> int 
	//int --> real
	//int --> int 



	
	if(((SymTable[idtok.GetLexeme()] == REAL && retval.GetType() == VINT))){
		retval.SetReal(double(retval.GetInt()));
		retval.SetType(VREAL);
	}
	else if(((SymTable[idtok.GetLexeme()] == INTEGER && retval.GetType() == VREAL))){
		retval.SetInt(int(retval.GetReal()));
		retval.SetType(VINT);
	}

	if ((SymTable[idtok.GetLexeme()] == REAL && retval.GetType() == VREAL) || ((SymTable[idtok.GetLexeme()] == INTEGER && retval.GetType() == VINT)) ){
		TempsResults[idtok.GetLexeme()] = retval;
	}
	else if (((SymTable[idtok.GetLexeme()] == CHARACTER && retval.GetType() == VSTRING))){
		Value temp = TempsResults[idtok.GetLexeme()]; //this should be where str1 is stored. 
			if (temp.GetstrLen()<retval.GetString().length()){
				string n = retval.GetString();
				n = n.substr(0,temp.GetstrLen());
				retval.SetString(n);
			}
			else if(temp.GetstrLen()>retval.GetString().length()){
				int counter = retval.GetString().length();
				string n = retval.GetString();
				while (counter != temp.GetstrLen()){
					n+=' ';
					counter++;
				}
				retval.SetString(n);
			}

		
		TempsResults[idtok.GetLexeme()]= Value(retval);


	}
	else{
		status = false;
		ParseError(templine, "Incorrect assignment");
	}


	


	return status;	
}//End of AssignStmt

bool RelExpr(istream& in, int& line, Value & retVal){
	
	bool t1 = Expr(in, line, retVal);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	Value val2;
	
	if ( tok == EQ || tok == LTHAN || tok == GTHAN) 
	{
		LexItem rel = tok;
		t1 = Expr(in, line, val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (val2.GetType() == VSTRING || retVal.GetType() == VSTRING){
			ParseError(line, "Incorrect types");
		}
		
		if (rel == LTHAN){
			
			retVal = retVal<val2;
			retVal.SetType(VBOOL);
		}
		else if (rel == GTHAN){
			
			retVal = retVal>val2;
			retVal.SetType(VBOOL);
		}
		else if(rel == EQ){
			
			retVal = retVal == val2;
			retVal.SetType(VBOOL);
		}

		
	}
	else{
		ParseError(line, "Missing relational expression");
		return false;
	}
	
	
	return true;
}//End of RelExpr

//Expr ::= MultExpr { ( + | - | // ) MultExpr }
bool Expr(istream& in, int& line, Value & retVal) {
	
   
	bool t1 = MultExpr(in, line, retVal); //retVal has the first value
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}

	Value Val2;
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == PLUS || tok == MINUS || tok == CAT) 
	{
		LexItem op = tok;
		t1 = MultExpr(in, line, Val2); //gets you the second value you are working with
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (op == PLUS){
			retVal = retVal +Val2;
		}
		else if (op == MINUS){
			retVal = retVal - Val2;
		}
		else if (op==CAT){
			retVal = retVal.Catenate(Val2);
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
	}

	Parser::PushBackToken(tok);
	return true;
}//End of Expr

//MultExpr ::= TermExpr { ( * | / ) TermExpr }
bool MultExpr(istream& in, int& line, Value & retVal){
	
	bool t1 = TermExpr(in, line, retVal);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}

	Value val2;
	
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == MULT || tok == DIV  )
	{
		LexItem op = tok;
		t1 = TermExpr(in, line, val2);

		
		if( !t1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (op == MULT){
			retVal = retVal*val2;
		}
		else if (op == DIV){
			if((val2.GetType() == VINT && val2.GetInt() == 0) || (val2.GetReal() == 0.0 && val2.GetType()==VREAL)){
				ParseError(line-1, "Division by 0");
				return false;
			}
			retVal = retVal/val2;
		}

		
		
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of MultExpr

//TermExpr ::= SFactor { ** SFactor }
bool TermExpr(istream& in, int& line, Value & retVal){
	stack<Value> el;
	
	bool t1 = SFactor(in, line, retVal);
	LexItem tok;

	if( !t1 ) {
		return false;
	}

	 //retval gives me the first value I am working with
	 Value val2;
	
	tok	= Parser::GetNextToken(in, line); 
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	el.push(retVal);
	while ( tok == POW )
	{
		t1 = SFactor(in, line, val2);
		
		
		if( !t1 ) {
			ParseError(line, "Missing exponent operand");
			return false;
		}
		el.push(val2);

		
		
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
		
	}

	while(el.size()>1){
		Value val1 = el.top();
		el.pop();
		Value val2 = el.top();
		el.pop();
		retVal = val2.Power(val1);
		el.push(retVal);
	}
	retVal = el.top();
	el.pop();

	Parser::PushBackToken(tok); 
	return true;
}//End of TermExpr

//SFactor = Sign Factor | Factor
bool SFactor(istream& in, int& line, Value & retVal)
{
	LexItem t = Parser::GetNextToken(in, line); //NOW, IT IS THE 5
	
	bool status;
	int sign = 0;
	if(t == MINUS )
	{
		sign = -1;
	}
	else if(t == PLUS)
	{
		sign = 1;
	}
	else
		Parser::PushBackToken(t);//5 IS PUSHED BACK
		
	status = Factor(in, line, sign, retVal);
	return status;
}//End of SFactor

//Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream& in, int& line, int sign, Value & retVal){
	
	LexItem tok = Parser::GetNextToken(in, line); //GET 5 AGAIN.
	
	//cout << tok.GetLexeme() << endl;

	

	
	if( tok == IDENT) { //retrieve the value of the variable in temps result
		
		string lexeme = tok.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			return false;	
		}
		retVal = TempsResults[tok.GetLexeme()];

		if(sign ==-1 && retVal.GetType() == VSTRING){
			ParseError(line,"Illegal Operand Type for Sign Operator" );
			return false;
		}

		if ((retVal.GetType() == VINT && sign ==-1) || (retVal.GetType() == VREAL && sign ==-1)){
			retVal = retVal*-1;
			
		}
		
		
        
		return true;
	}
	
	else if( tok == ICONST ) { //you have to do checking for negative
		if (sign == -1){
			
			retVal.SetInt(stoi(tok.GetLexeme())*-1);
		}
		else{
			retVal.SetInt(stoi(tok.GetLexeme()));
		}
		//cout<<retVal.GetInt()<<endl;

		if (retVal.GetType() == VREAL){ //id = int
			retVal.SetReal(stod(tok.GetLexeme()));

		}
		else if(retVal.GetType() == VERR){
			retVal.SetType(VINT);
		}
		

		return true;
	}
	else if( tok == SCONST ) {
		retVal.SetType(VSTRING);
		retVal.SetString(tok.GetLexeme());
		return true;
	}
	else if( tok == RCONST ) { //you have to do checking for positive
		
		if (sign == -1){
			
			retVal.SetReal(stod(tok.GetLexeme())*-1.0);
		}
		else{
			retVal.SetReal(stod(tok.GetLexeme()));
		}
		if (retVal.GetType() == VINT){ //id = int
			retVal.SetInt(stoi(tok.GetLexeme()));
			
		}
		else if(retVal.GetType() == VERR){
			retVal.SetType(VREAL);
		}
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line, retVal);
		if( !ex ) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;
		else 
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	
	return false;
}
