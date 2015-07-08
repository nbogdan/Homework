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

%class NFALexer
%unicode
%cup
%line
%column

%{
  
  StringBuffer string = new StringBuffer();
  int contor = 0;
  int start = 0;
  
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
upperCaseLetter = [A-NP-Z]
digit           	= [0-9]
other 				= [!#$%&-./:;"<"">""="@"[""]""^"'~]
symbol  			= {lowerCaseLetter}|{digit}|{other}
alphabet			= {symbol}(,{symbol})*
word 				= e|{symbol}+



name= ({upperCaseLetter}|_)({lowerCaseLetter}|{digit}|_)*
state				= {name}
states				= {state}(,{state})*
transition			= "d("{state}","{symbol}")""="{state}
relation			= {state},{word},{state}
relations			= O|{relation}(,{relation})*

%state STATES
%state ALFAB
%state RELS
%state STARTS

%%
/**
 * LEXICAL RULES:
 */

<YYINITIAL>{
	"("					{ 
							yybegin(STATES);
						}
	

	","					{
							switch(contor)
							{
							
								case 1:
										yybegin(ALFAB);
										break;			
								case 2:
										yybegin(RELS);
										break;
								case 3:
										yybegin(STARTS);
										break;
								case 4:
										yybegin(STATES);
										break;
							} 
						}
	")"					{
						}
}

<STATES>{
	
	"O"					{
							contor++;
							return symbol(STATES, "NaS");
						}
						
	")"					{ yybegin(YYINITIAL); }
	
	"{"					{ }
	","					{ }
	
	{state} 			{
							string.setLength(0);
							string.append( yytext());
							String aux = string.toString();
							
							string.setLength(0);
							return symbol(STATES, aux);
						}

	"}"					{
							contor++;
							yybegin(YYINITIAL); 
						}

}

<STARTS>{

	{state} 			{
							string.setLength(0);
							string.append( yytext());
							String aux = string.toString();
							string.setLength(0);
							contor++;
							yybegin(YYINITIAL); 
							return symbol(STARTS, aux);
						}

}


<ALFAB>{
	
	"{"					{ }

	{alphabet} 			{
							string.setLength(0);
							string.append( yytext());

						}

	"}"					{
							String aux = string.toString();
							string.setLength(0);
							yybegin(YYINITIAL); 
							contor++;
                            return symbol(ALFAB, 
                            aux);
						}

}

<RELS>{
	
	"{"					{ }
	[)(]				{ }

	{state}		 		{
							string.setLength(0);
							string.append( yytext());
						}
						
	","					{
							start = 0;
							return symbol(RELS, 
                            string.toString());
						}
						
	{symbol}|e 			{
							if(start == 0)
							{
								string.setLength(0);
								start = 1;
							}
							string.append( yytext());
						}
	


	"}"					{
							String aux = string.toString();
							string.setLength(0);
							yybegin(YYINITIAL); 
							contor++;
                            return symbol(RELS, 
                            aux);
						}

}



.               { System.out.println("Illegal char, '" + yytext() +
                    "' line: " + yyline + ", column: " + yychar); }
<<EOF>>
{
	return symbol(YYEOF);
}