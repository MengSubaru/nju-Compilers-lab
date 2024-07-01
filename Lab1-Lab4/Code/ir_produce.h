#ifndef IR
#define IR
#include <stdio.h>
#include <stdarg.h>
#include "symbol_table.h"
#define IR_BUFFER_SIZE 256
extern char buffer[IR_BUFFER_SIZE];
extern FILE* out_ir;
extern int tmp_cnt;//当前可供使用的临时变量后缀
extern int label_cnt;//当前可用的标签后缀
extern char* file_name;

typedef struct _arg{//链表
    char* arg;
    int type;//0为（一维）数组，1为普通变量
    struct _arg* next;
} arg_list;

typedef enum{
        OP_VARIABLE,
        OP_CONSTANT,
        OP_ADDRESS,
        OP_LABEL,
        OP_FUNCTION,
        OP_RELOP,
        OP_QUOTE,
}OperandKind;
typedef struct {
    OperandKind kind;
    struct 
    {
        int val;
        char* name;
    
    } element;
}Operand;
typedef Operand* OperandPointer;
typedef enum {
        IR_LABEL,
        IR_FUNCTION,
        IR_ASSIGN,
        IR_ADD,
        IR_SUB,
        IR_MUL,
        IR_DIV,
        IR_GET_ADDR,
        IR_READ_ADDR,
        IR_WRITE_ADDR,
        IR_GOTO,
        IR_IF_GOTO,
        IR_RETURN,
        IR_DEC,
        IR_ARG,
        IR_CALL,
        IR_PARAM,
        IR_READ,
        IR_WRITE,
} IRKind;
typedef struct IRInstruction {
    IRKind kind;
    char* instruction;
    struct IRInstruction* next;
    int debug;
    union {
        struct {
            OperandPointer op;
        } oneOp;
        //GOTO RETURN ARG 
        struct {
            OperandPointer des, src;
        } twoOp;// ASSIGN CALL 单个取地址or取引用
        struct {
            OperandPointer des, src1, src2;
        } threeOp;// +-*/ 允许操作数取地址取引用
        struct {
            OperandPointer x, relop, y, z;
        } ifGoto;
        struct {
            OperandPointer op;
            int size;
        } dec;
    }operands;
    
} IRInstruction;

extern IRInstruction* ir_head;
extern IRInstruction* ir_tail;
void exit_my_code();
int check_tmp(char* tmp);
char* new_tmp();
char *new_label();
void preprocess_exp_before_cond(TreeNode* exp,char* place);
void operand_op_operand(TreeNode *exp,char* place,int operator);
int translate_array(TreeNode* exp,char* place);
int translate_Exp(TreeNode* exp,char *place);
void else_translate_Cond(TreeNode* exp,char* label1 ,char* label2);
void translate_Cond(TreeNode* exp,char *label1,char* label2);
void translate_Stmt(TreeNode* stmt);
void translate_Args(TreeNode* args,arg_list** arg);
void translate_FunDec(TreeNode* FunDec);
void translate_var_dec(TreeNode* root);
void dfs_lab3(TreeNode *root);

void clear_buffer();
void append_ir_instruction(const char* format, ...) ;
void write_ir_to_file(FILE* file);
void free_ir_instructions();
void generate_ir_instruction(IRKind irKind,int debug);
void printf_irs();
void printf_ir(IRInstruction *ir);
#endif
