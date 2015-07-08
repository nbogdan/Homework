/*-***
 *
 * This file defines a stand-alone lexical analyzer for a subset of the Pascal
 * programming language.  This is the same lexer that will later be integrated
 * with a CUP-based parser.  Here the lexer is driven by the simple Java test
 * program in ./PascalLexerTest.java, q.v.  See 330 Lecture Notes 2 and the
 * Assignment 2 writeup for further discussion.
 *
 */


/* JFlex example: part of Java language lexer specification */
import java_cup.runtime.*;

/**
 * This class is a simple example lexer.
 */
%%

%class RELexer
%unicode
%cup
%line
%column

%{
  boolean concat_flag = false;
  StringBuffer string = new StringBuffer();

  private Symbol symbol(int type) {
    return new Symbol(type, yyline, yycolumn);
  }
  private Symbol symbol(int type, Object value) {
    return new Symbol(type, yyline, yycolumn, value);
  }
%}



/*-*
 * PATTERN DEFINITIONS:
 */
lowerCaseLetter	= [a-df-z]
digit           	= [0-9]
other 				= "!"|"#"|"$"|"%"|"&"|"-"|"."|"/"|":"|";"|"<"|">"|"="|"@"|"["|"]"|"^"|"'"|"~"
symbol  			= {lowerCaseLetter}|{digit}|{other}
alphabet			= {symbol}(,{symbol})*
unaryOp				= "*"|"+"|"?"
binaryOp			= "|"
symbolS				= {symbol}|"O"|"e"

%state EXPR
%state ALFAB

%%
/**
 * LEXICAL RULES:
 */

<YYINITIAL>{

	"{"					{
							yybegin(ALFAB); 
						}

	":"					{
							yybegin(EXPR);
						}
}

<ALFAB>{

	{alphabet} 			{
							string.setLength(0);
							string.append( yytext());

						}

	"}"					{
							String aux = string.toString();
							string.setLength(0);
							yybegin(YYINITIAL); 
                            return symbol(ALFAB, 
                            aux);
						}

}

<EXPR>{

	"("					{
							if(concat_flag)
								string.append("_" + yytext());
							else
								string.append( yytext());
							concat_flag = false;
						}
						
	")"					{
							string.append( yytext());
							concat_flag = true;
						}

	{unaryOp}			{
							concat_flag = true;
							string.append( yytext());
						}
	
	{binaryOp}			{
							concat_flag = false;
							string.append( yytext());
						}

	{symbolS}			{
							if(concat_flag)
								string.append("_" + yytext());
							else
								string.append( yytext());
							concat_flag = true;
						}
	

	



	<<EOF>>					{
							
							yybegin(YYINITIAL); 
                            return symbol(EXPR, 
                            string.toString());
						}
}


.               { System.out.println("Illegal char, '" + yytext() +
                    "' line: " + yyline + ", column: " + yychar); }

<<EOF>>
{
	return symbol(YYEOF);
}