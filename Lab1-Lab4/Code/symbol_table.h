#ifndef SYMBOL
#define SYMBOL
#include <stdio.h>
#include <assert.h>
//#include "syntax.tab.h"
#include "treenode.h"
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

extern const int SIZE_TABLE;//符号表的大小
Type hash_table[0x3fff];//传入标识符，找到对应的类型
extern int error_line;//上一次出错行号

Type returnType;//记录当前函数的返回类型，为NULL代表无返回类型或者已经出错
struct Type_{
    enum { BASIC, ARRAY, STRUCTURE, FUNC } kind;//函数也记录在此符号表中
    union{
        int basic;// 基本类型，0为int 1为float
        struct { Type elem; int size; } array;// 数组类型信息包括元素类型与数组大小构成//数组头的size无意义
        FieldList structure;// 结构体类型用链表维护
        struct {
            FieldList input;//指向输入元素的指针，输入元素用链表维护
            Type output;//返回值类型
            int def;//def==0代表仅声明未定义，def==1代表已定义但声明情况未知
            int line;
        } func;//注意检查重名时函数与变量（结构体）的重名时可以允许的
    } u;
    char* name;//该变量或结构体或函数的名字，对于结构体来说为空（去u.structure->name找），但是变量和函数需要这个进行标识
    int arg;//arg为0代表只是普通的变量，为大于1的数则代表是函数的第arg个参数
    char* func;//当arg=1时，func指向该变量所属的函数名
};
struct FieldList_{
    char* name; // 域的名字
    Type type; // 域的类型
    FieldList tail; // 下一个域
    //注意：对于结构体头部来说，弃用type类型，tail直接连接结构体的第一个成员变量
};
void init_hash();//check
void error_print(int idx,char *msg,int line);//check
int hash_pjw(char* name);//check
int hash_insert(char* name);//check
void insert_table(TreeNode *root);//基本处理完了，只剩下Def对应的Dec是赋值语句且右值出现问题的情况处理，但是也不需要处理，错误在dealEXP里处理，且左值的符号在符号表中位置不受影响
int child_num(TreeNode*root);//check
int hash_find_struct(char *name);//check
int deal_with_VarDec(TreeNode* root,Type type);//check 但是进入前需要确认不重名，退出之后还要手动为Type_的arg和func赋值,里面会自动申请空间
FieldList deal_with_Def(TreeNode *root,const FieldList head);//check
char* mystrdup(char* string);//check
FieldList deal_with_Struct(char* name,TreeNode* DefList,int line);//check
int deal_with_type(TreeNode* type,Type tar);//check
void deal_with_VarDec_Struct(TreeNode* root,FieldList cont);//check
FieldList deal_Func(TreeNode* root,int* error_happen);//check
int check_same_name(char* name);//check
void free_FieldList(FieldList var);//check
int checkArgDuplicate(char* name,int idx,char* func,Type type,int line);//check
char *get_VarDecName(TreeNode* root);//check
int check_same_type(Type type1,Type type2);//check
Type copy_Type(Type tar);//check
void free_Type(Type tar);//check
int hash_find_name(char* name);//check
Type create_type_array(TreeNode* root, Type type);
Type deal_with_EXP(int* LRValue,TreeNode* exp);//判断exp是否有问题
void dfs_init_symbol_table(TreeNode* root);
#endif