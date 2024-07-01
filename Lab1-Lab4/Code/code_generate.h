#ifndef CODE
#define CODE

#include "ir_produce.h"
#define FRAME_CODE_SIZE 256
extern FILE* out_ir;
extern IRInstruction* ir_head;
extern IRInstruction* ir_tail;
extern char codeBuffer[FRAME_CODE_SIZE];
typedef struct _code{
    char* code;
    struct _code*next;
    IRInstruction* ir; 
}CodeList;

typedef struct _varDesList
{
    OperandPointer op;
    int ifinMemory;//0 不在 // 1 在
    int regNo;//
    int fpoffset;//内存相对栈位移
    struct _varDesList *next;
}VarDesList;


typedef struct {
    char*name;
    int ifUse;

    VarDesList*varDes;
    // 1 use 0 free
}Reg;
typedef enum {
    ZERO,
    AT,
    V0,
    V1,
    A0,
    A1,
    A2,
    A3,
    T0,
    T1,
    T2,
    T3,
    T4,
    T5,
    T6,
    T7,
    S0,
    S1,
    S2,
    S3,
    S4,
    S5,
    S6,
    S7,
    T8,
    T9,
    K0,
    K1,
    GP,
    SP,
    FP,
    RA,
}RegNo;
typedef struct _frame
{
    VarDesList * varDesList;
    int size;
    char* funcName;
    struct _frame* next;
    int max_size;
    CodeList*head;
    CodeList*tail;
}Frame;

typedef struct _stack
{
    Frame* frameList;
    Frame*curr;
    
}Stack;

void allIRtoMips(FILE*fp);
void pushVartoFrame(FILE*fp,VarDesList*vp,Frame*frame,int offset);
void IRtoMips(FILE*fp,IRInstruction*ir,Frame*frame);
void addVartoReg(int no,OperandPointer op,Frame*frame,FILE*fp);

#endif
