#include "lex.h"
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <map>
using namespace std;

ostream& operator<<(ostream& out, const LexItem& tok){
    map<Token, string> t = {{IF, "IF"}, {ELSE, "ELSE"},{PRINT, "PRINT"}, {INTEGER, "INTEGER"}, {REAL, "REAL"}, 
    {CHARACTER, "CHARACTER"}, {END, "END"}, 
    {THEN, "THEN"}, {PROGRAM, "PROGRAM"},{LEN, "LEN"}, 
    {IDENT, "IDENT"}, {ICONST, "ICONST"}, {RCONST, "RCONST"}, 
    {SCONST, "SCONST"}, {PLUS, "PLUS"}, {MINUS, "MINUS"}, {MULT, "MULT"}, {DIV, "DIV"}, {ASSOP, "ASSOP"}, 
    {EQ, "EQ"}, {POW, "POW"}, {GTHAN, "GTHAN"}, {LTHAN,"LTHAN"}, {CAT, "CAT"}, {COMMA, "COMMA"}, {LPAREN, "LPAREN"}, 
    {RPAREN, "RPAREN"}, {DOT, "DOT"}, {DCOLON, "DCOLON"}, {DEF, "DEF"}, {ERR, "ERR"}, {DONE, "DONE"}};


    if (tok.GetToken() == ICONST||tok.GetToken() == RCONST|| tok.GetToken() == BCONST){
        out << t[tok.GetToken()]<<": ("<<tok.GetLexeme()<<")";
    }
    else if (tok.GetToken() == IDENT){
        out << t[tok.GetToken()]<<": \'"<<tok.GetLexeme()<<"\'";

    }
    else if(tok.GetToken() == SCONST){
        out << t[tok.GetToken()]<<": \""<<tok.GetLexeme()<<"\""; 
    }
    else{
        out<<t[tok.GetToken()];
    }
    return out;

}   
LexItem id_or_kw(const string& lexeme, int linenum){
/*It searches for the lexeme in a directory that maps a string value of a keyword to its
corresponding Token value, and it returns a LexItem object containing the lexeme, the keyword
Token, and the line number, if it is found, or IDENT Token otherwise.    --> maps a string value to a token value */

    map<string,Token> kw = {{"IF", IF}, {"ELSE", ELSE}, {"PRINT", PRINT}, {"INTEGER", INTEGER}, {"REAL", REAL}, {"CHARACTER", CHARACTER}, {"END", END}, {"THEN", THEN}, {"PROGRAM", PROGRAM}, {"LEN", LEN}};

    if (kw.find(lexeme)!=kw.end()){ //find does not equal end, ie this exists in the lexitem method. 
        return LexItem(kw[lexeme], lexeme, linenum);
    }
    else{
        return LexItem(IDENT, lexeme, linenum);

    }

}
LexItem getNextToken(istream& in, int& linenum){

    //entire file goes into getnexttoken.

    
    enum TokState{START, INID, ININT, INSTRING, INREAL} lexstate = START;
    string lexeme; //you have to build this!!
    string lexeme1;
    char ch; //takes in each individual character
    //LexItem lex;
    string test;


    while(in.get(ch)){//each word from stream, indicated by a space delimmite, goes into word. 3+3

        switch(lexstate){
            case START:{
                if (ch == ' ' || ch == '\t'){
                    continue;
                    /*check if this is correct*/
                }
                if(ch=='\n'){
                    linenum++;
                    continue;

                }
                if (ch == '+'){
                    return LexItem(PLUS, "+", linenum);
                    //continue;
                }  
                else if (ch == '-'){
                    return LexItem(MINUS, "-", linenum);
                    //continue;
                }
                else if (ch == '*'){
                    if (in.peek() == '*'){
                        in.get(ch);
                        return LexItem(POW, "**", linenum);    
                    }
                    else if(in.peek() ==','){
                        return LexItem(DEF, "*", linenum);
                    }
                    else{
                        return LexItem(MULT, "*", linenum);
                    }
                    //continue;
                }   
                else if (ch == '/'){
                    if(in.peek() == '/'){
                        in.get(ch);
                        return LexItem(CAT, "//", linenum);
                    }
                    else{
                        return LexItem(DIV, "/", linenum);

                    }
                    //continue;
                } 
                else if (ch == '='){
                    if(in.peek() == '='){
                        in.get(ch);
                        return LexItem(EQ, "==", linenum);
                    }
                    else{
                        return LexItem(ASSOP, "=", linenum);
                    }
                    //continue;
                }     
                else if (ch == '<'){
                    return LexItem(LTHAN, "<", linenum);
                    //continue;
                }
                else if (ch == '>'){
                    return LexItem(GTHAN, ">", linenum);
                    //continue;
                }
                else if (ch == ','){
                    return LexItem(COMMA, ",", linenum);
                    //continue;
                }
                else if (ch == '('){
                    return LexItem(LPAREN, "(", linenum);
                    //continue;
                }
                else if (ch == ')'){
                    return LexItem(RPAREN, ")", linenum);
                    //continue;
                }
                else if (ch == ':'){
                    if(in.peek() == ':'){
                        in.get(ch);
                        return LexItem(DCOLON, "::", linenum);
                        //continue;
                    }

                }
                else if (ch == '.'){
                    if (!isdigit(in.peek())){
                        return LexItem(DOT, ".", linenum);
                        
                    }
                    else{
                        in.putback(ch); //puts the . backinto the stream
                        lexstate = INREAL;
                       // continue;
                    }
                }  //MAKE SURE YOU DO delimiter as well. 
                else if (ch == '!'){ //if the entire line is a comment, skip all characters. 
                    while (in.get(ch)){
                        if (ch =='\n'){
                            linenum++;
                            break;
                        }
                    }
                    /*I have no clue if this works, but lets hope. */
                }
            

                else if (isdigit(ch)){ //example you have 55, 5=, 5a
                    
                    lexstate = ININT; //still reading the 5
                    in.putback(ch);
                }
                else if(isalpha(ch)){ //a5
                    
                    lexstate  = INID;
                    in.putback(ch);
                    
                    //continue;

                }
                else if((ch == '\'') || (ch == '\"')){
                    in.putback(ch);
                    lexstate = INSTRING;
                    //continue;
                }
                else if (ch == '$'){
                    linenum++; //check this against the 
                    return LexItem(ERR, "$", linenum);
                }


                break;
            }

            case INID: {//only mapped to a token if they are in a directory of keywords.
                if((isalnum(ch)) ||(ch == '_')){
                    lexeme += ch;//for example a //ADDS 'a' to
                    test += toupper(ch);
                }
                else{
                    in.putback(ch);
                    LexItem lex = id_or_kw(test, linenum);
                    return LexItem(lex.GetToken(), lexeme, linenum);  
                }
                
            
            break;
            }

            case ININT: {//there might be a case of 5a or something like that, but make sure. 
                if(isdigit(ch)){//two cases, 55, or 5.5, 5+5=10, 5a anything else is an error
                    lexeme+=ch; //lexeme +=5
                } 


                /*
                different cases after reading first int:
                1. could be a decimal
                2. could be a space or a new line
                3. could be a
                */


                if(in.peek() == '.'){ //after 5, .
                    in.get(ch);//this now has the decimal point
                    
                    if (isdigit(in.peek())){ //5.5
                        
                        lexstate = INREAL;//sees that next character is a decimal point, means 5.5 or 5.
                        in.putback(ch); //decimal goes back into the stream.
                    } 

                    else{ //if 5.
                        in.putback(ch); //decimal back into the stream.
                        return LexItem(ICONST, lexeme, linenum);

                    }
                }
                
                else if(!isdigit(in.peek())){
                    return LexItem(ICONST, lexeme, linenum);
                }

                    

              
            break;
            }


            case INSTRING:{
                char h = ch; // h is either ' or "
                lexeme1+=ch;
                while (in.get(ch)){
                    
                    if (ch == '\n'){
                        linenum++;
                        return LexItem(ERR, lexeme1, linenum);
                    }
                    else if(ch=='\''){
                        if (h == '\''){
                            return LexItem(SCONST, lexeme, linenum);
                        }
                        else if (h == '\"'){
                            linenum++;
                            lexeme1+=ch;
                            return LexItem(ERR, lexeme1, linenum);
                           
                        }
                    }
                    else if(ch=='\"'){
                        if (h == '\"'){
                            return LexItem(SCONST, lexeme, linenum);
                        }
                        else if (h == '\''){
                            linenum++;
                            return LexItem(ERR, lexeme1, linenum);
                           
                        }
                    }
                    else{
                        lexeme1+=ch;
                        lexeme+=ch;
                    }
                    
                }

            break;
            }

            case INREAL:
            {
                lexeme += ch;
                if (!isdigit(in.peek())) {
                    if (in.peek() == '.') {
                        in.get(ch);
                        lexeme+=ch;
                        linenum++;
                        return LexItem(ERR, lexeme, linenum); // Error: Two consecutive decimal points
                    }
                    else {
                        return LexItem(RCONST, lexeme, linenum);
                    }
                }
            break;
            }

        }

    }
    return LexItem(DONE, "", linenum);
    
}


