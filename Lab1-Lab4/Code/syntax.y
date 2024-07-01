%{
    #include "lex.yy.c"
    #include <stdio.h>
    #include "treenode.h"
    int yylex();
    void yyerror(const char* msg);

    extern TreeNode *root;
    extern int numOferror;
    extern int error_line;
%}
%union {
    struct TreeNode* type_treeNode;
}
%token <type_treeNode> INT FLOAT ID RELOP TYPE SEMI COMMA ASSIGNOP PLUS MINUS STAR DIV AND OR DOT NOT LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE
%type <type_treeNode> Program ExtDefList ExtDef Specifier ExtDecList StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Args Exp

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left MINUS PLUS
%left STAR DIV
%right NEG NOT
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%locations
%%

//High-level Definitions
Program : ExtDefList {$$=createTreeNode("Program",NULL,@$.first_line,0);insertTreeNode($$,$1);root=$$;}
;
ExtDefList : ExtDef ExtDefList {$$=createTreeNode("ExtDefList",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);}
| /* empty */ {$$=NULL;}//| ExtDef error ExtDefList
;
ExtDef : Specifier ExtDecList SEMI {$$=createTreeNode("ExtDef",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}//全局变量//| Specifier ExtDecList error SEMI
| Specifier SEMI {$$=createTreeNode("ExtDef",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);}//结构体，但int;也合法
| Specifier FunDec CompSt {$$=createTreeNode("ExtDef",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}//函数//| Specifier FunDec CompSt error//| Specifier FunDec SEMI {$$=createTreeNode("ExtDef",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}//函数声明
| Specifier error SEMI {yyerrok;}//似乎多余？
| Specifier FunDec error CompSt {yyerrok;}
| Specifier FunDec error {yyerrok;}
| Specifier error CompSt {yyerrok;}
| error FunDec CompSt {yyerrok;}
| error CompSt {yyerrok;}
| error FunDec {yyerrok;}
| error SEMI {yyerrok;}
;
ExtDecList : VarDec {$$=createTreeNode("ExtDecList",NULL,@$.first_line,0);insertTreeNode($$,$1);}
| VarDec COMMA ExtDecList {$$=createTreeNode("ExtDecList",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}//| VarDec error COMMA ExtDecList
;

//Specifiers
Specifier : TYPE {$$=createTreeNode("Specifier",NULL,@$.first_line,0);insertTreeNode($$,$1);}//类型：包括INT FLOAT和STRUCT
| StructSpecifier {$$=createTreeNode("Specifier",NULL,@$.first_line,0);insertTreeNode($$,$1);}
;
StructSpecifier : STRUCT OptTag LC DefList RC {$$=createTreeNode("StructSpecifier",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);insertTreeNode($$,$4);insertTreeNode($$,$5);}//结构体类型
| STRUCT Tag {$$=createTreeNode("StructSpecifier",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);}
| STRUCT OptTag LC DefList error RC {yyerrok;}
| STRUCT error LC DefList RC {yyerrok;}//不可删除
;
OptTag : ID {$$=createTreeNode("OptTag",NULL,@$.first_line,0);insertTreeNode($$,$1);}//结构体的名字，可以为空
| /* empty */ {$$=NULL;}
;
Tag : ID {$$=createTreeNode("Tag",NULL,@$.first_line,0);insertTreeNode($$,$1);}
;

//Declarators
VarDec : ID {$$=createTreeNode("VarDec",NULL,@$.first_line,0);insertTreeNode($$,$1);}//变量定义，可以为数组
| VarDec LB INT RB {$$=createTreeNode("VarDec",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);insertTreeNode($$,$4);}
| VarDec LB error RB {yyerrok;}//数组的长度不为常量
| VarDec LB INT error {yyerrok;}//缺少右括号
| VarDec LB error {yyerrok;}//只有左括号
;
FunDec : ID LP VarList RP {$$=createTreeNode("FunDec",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);insertTreeNode($$,$4);}//函数头，可以为空
| ID LP RP {$$=createTreeNode("FunDec",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| ID LP VarList error {yyerrok;}
| ID LP error {yyerrok;}
| ID LP error RP {yyerrok;}
;
VarList : ParamDec COMMA VarList {$$=createTreeNode("VarList",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| ParamDec {$$=createTreeNode("VarList",NULL,@$.first_line,0);insertTreeNode($$,$1);}
;
ParamDec : Specifier VarDec {$$=createTreeNode("ParamDec",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);}//| Specifier VarDec error
;

//Statements
CompSt : LC DefList StmtList RC {$$=createTreeNode("CompSt",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);insertTreeNode($$,$4);}
;
StmtList : Stmt StmtList {$$=createTreeNode("StmtList",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);}
| /* empty */ {$$=NULL;}
;
Stmt : Exp SEMI {$$=createTreeNode("Stmt",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);}
| CompSt {$$=createTreeNode("Stmt",NULL,@$.first_line,0);insertTreeNode($$,$1);}
| RETURN Exp SEMI {$$=createTreeNode("Stmt",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=createTreeNode("Stmt",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);insertTreeNode($$,$4);insertTreeNode($$,$5);}//调整if_else的优先级
| IF LP Exp RP Stmt ELSE Stmt {$$=createTreeNode("Stmt",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);insertTreeNode($$,$4);insertTreeNode($$,$5);insertTreeNode($$,$6);insertTreeNode($$,$7);}
| WHILE LP Exp RP Stmt {$$=createTreeNode("Stmt",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);insertTreeNode($$,$4);insertTreeNode($$,$5);}
| error SEMI {yyerrok;}
| RETURN Exp error { yyerrok; }
| RETURN error SEMI { yyerrok; }//不接受void
| RETURN error { yyerrok; }
| IF LP error RP Stmt %prec LOWER_THAN_ELSE { yyerrok; }
| IF LP error RP Stmt ELSE Stmt { yyerrok; }
| IF LP error Stmt %prec LOWER_THAN_ELSE { yyerrok; }
| IF LP error Stmt ELSE Stmt { yyerrok; }
| IF error RP Stmt %prec LOWER_THAN_ELSE { yyerrok; }
| IF error RP Stmt ELSE Stmt { yyerrok; }
| IF error Stmt %prec LOWER_THAN_ELSE { yyerrok; }
| IF error Stmt ELSE Stmt { yyerrok; }
| WHILE LP error RP Stmt { yyerrok; }
| WHILE LP error Stmt { yyerrok; }//不可删除
| WHILE error RP Stmt { yyerrok; }
| WHILE error Stmt { yyerrok; }
| Exp error {}
;

//Local Definitions
DefList : Def DefList {$$=createTreeNode("DefList",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);}
| /* empty */ {$$=NULL;}
;
Def : Specifier DecList SEMI {$$=createTreeNode("Def",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| Specifier DecList error SEMI { yyerrok; }//| Specifier DecList error { yyerrok; }//error DecList SEMI应该属于stmtlist部分
| Specifier error SEMI { yyerrok; }
;
DecList : Dec {$$=createTreeNode("DecList",NULL,@$.first_line,0);insertTreeNode($$,$1);}
| Dec COMMA DecList {$$=createTreeNode("DecList",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
;
Dec : VarDec {$$=createTreeNode("Dec",NULL,@$.first_line,0);insertTreeNode($$,$1);}
| VarDec ASSIGNOP Exp {$$=createTreeNode("Dec",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}//| VarDec ASSIGNOP error
;

//Expressions
Exp : Exp ASSIGNOP Exp {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| Exp AND Exp {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| Exp OR Exp {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| Exp RELOP Exp {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| Exp PLUS Exp {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| Exp MINUS Exp {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| Exp STAR Exp {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| Exp DIV Exp {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| LP Exp RP {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| MINUS Exp %prec NEG {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);}
| NOT Exp {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);}
| ID LP Args RP {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);insertTreeNode($$,$4);}
| ID LP RP {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| Exp LB Exp RB {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);insertTreeNode($$,$4);}
| Exp DOT ID {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| ID {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);}
| INT {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);}
| FLOAT {$$=createTreeNode("Exp",NULL,@$.first_line,0);insertTreeNode($$,$1);}
| Exp ASSIGNOP error { yyerrok; }
| Exp AND error { yyerrok; }
| Exp OR error { yyerrok; }
| Exp RELOP error { yyerrok; }
| Exp PLUS error { yyerrok; }
| Exp MINUS error { yyerrok; }
| Exp STAR error { yyerrok; }
| Exp DIV error { yyerrok; }
| LP error RP { yyerrok; }
| MINUS error %prec NEG { yyerrok; }
| NOT error { yyerrok; }
| ID LP error RP { yyerrok; }//| ID LP error
| Exp LB error RB { yyerrok; }//| Exp LB error
| Exp DOT error { yyerrok; }
;
Args : Exp COMMA Args {$$=createTreeNode("Args",NULL,@$.first_line,0);insertTreeNode($$,$1);insertTreeNode($$,$2);insertTreeNode($$,$3);}
| Exp {$$=createTreeNode("Args",NULL,@$.first_line,0);insertTreeNode($$,$1);}
| Exp error Args { yyerrok; }
| Exp COMMA error { yyerrok; }
| error COMMA Args { yyerrok; }
;
%%
void yyerror(const char *msg) {
    if(yylineno!=error_line){
        numOferror++;
        error_line=yylineno;
        printf("Error type B at Line %d: %s, near \"%s\".\n",yylineno,msg,yytext);
    }  
}
