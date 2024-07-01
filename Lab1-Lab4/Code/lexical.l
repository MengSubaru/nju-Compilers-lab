%{
    #include <stdio.h>//关于注释的错误类型存疑,RELOP目前缺乏信息
    #include "syntax.tab.h"
    #include "treenode.h"
    int yylex();
    int yycolumn=1;
    extern int numOferror;
    extern int error_line;
    #define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
%}
%option yylineno
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
INT 0|([1-9][0-9]*)
FLOAT {INT}\.[0-9]+
ID [A-Za-z_][A-Za-z_0-9]*
%%
"//" {
    char c = input();
    while (c != '\n') {
        c = input();
    }
}
"/*" {
    char c=input();
    int endNormally=0;
    while(c!='\0'){
        if(c=='*'){
            c=input();
            if(c=='/'){
                endNormally=1;
                break;
            }
        }else{
            c=input();
        }
    }
    if(!endNormally){
        numOferror++;
    	printf("Error type A at Line %d: 注释不符合定义\n",yylineno);
    }
}
{SEMI} {yylval.type_treeNode = createTreeNode("SEMI",NULL,yylineno,3);return SEMI;} 
{COMMA} {yylval.type_treeNode = createTreeNode("COMMA",NULL,yylineno,3);return COMMA;}
{ASSIGNOP} {yylval.type_treeNode = createTreeNode("ASSIGNOP",NULL,yylineno,3);return ASSIGNOP;}
{RELOP} {yylval.type_treeNode = createTreeNode("RELOP",yytext,yylineno,3);return RELOP;}
{PLUS} {yylval.type_treeNode = createTreeNode("PLUS",NULL,yylineno,3);return PLUS;}
{MINUS} {yylval.type_treeNode = createTreeNode("MINUS",NULL,yylineno,3);return MINUS;}
{STAR} {yylval.type_treeNode = createTreeNode("STAR",NULL,yylineno,3);return STAR;}
{DIV} {yylval.type_treeNode = createTreeNode("DIV",NULL,yylineno,3);return DIV;}
{AND} {yylval.type_treeNode = createTreeNode("AND",NULL,yylineno,3);return AND;}
{OR} {yylval.type_treeNode = createTreeNode("OR",NULL,yylineno,3);return OR;}
{DOT} {yylval.type_treeNode = createTreeNode("DOT",NULL,yylineno,3);;return DOT;}
{NOT} {yylval.type_treeNode = createTreeNode("NOT",NULL,yylineno,3);return NOT;}
{TYPE} {yylval.type_treeNode = createTreeNode("TYPE",yytext,yylineno,3);return TYPE;}
{LP} {yylval.type_treeNode = createTreeNode("LP",NULL,yylineno,3);return LP;}
{RP} {yylval.type_treeNode = createTreeNode("RP",NULL,yylineno,3);return RP;}
{LB} {yylval.type_treeNode = createTreeNode("LB",NULL,yylineno,3);return LB;}
{RB} {yylval.type_treeNode = createTreeNode("RB",NULL,yylineno,3);return RB;}
{LC} {yylval.type_treeNode = createTreeNode("LC",NULL,yylineno,3);return LC;}
{RC} {yylval.type_treeNode = createTreeNode("RC",NULL,yylineno,3);return RC;}
{STRUCT} {yylval.type_treeNode = createTreeNode("STRUCT",NULL,yylineno,3);return STRUCT;}
{RETURN} {yylval.type_treeNode = createTreeNode("RETURN",NULL,yylineno,3);return RETURN;}
{IF} {yylval.type_treeNode = createTreeNode("IF",NULL,yylineno,3);return IF;}
{ELSE} {yylval.type_treeNode = createTreeNode("ELSE",NULL,yylineno,3);return ELSE;}
{WHILE} {yylval.type_treeNode = createTreeNode("WHILE",NULL,yylineno,3);return WHILE;}
{INT} {yylval.type_treeNode = createTreeNode("INT",yytext,yylineno,1);return INT;}
{FLOAT} {yylval.type_treeNode = createTreeNode("FLOAT",yytext,yylineno,2);return FLOAT;}
{ID} {yylval.type_treeNode = createTreeNode("ID",yytext,yylineno,3);return ID;}
\n {yycolumn=1;}
[ \t\f\r\v]+ {}
. {
    error_line=yylineno;
    numOferror++;
    printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);
}
%%
