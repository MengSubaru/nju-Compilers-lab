#include<stdio.h>
#include "treenode.h"
#include "symbol_table.h"
#include "ir_produce.h"
#include "code_generate.h"
int yyparse (void);
void yyrestart ( FILE *input_file  );
TreeNode *root=NULL;
int numOferror=0;
int error_line=-1;
int tmp_cnt=1;//当前可供使用的临时变量后缀
int label_cnt=1;//当前可用的标签后缀
const int SIZE_TABLE=0x3fff;
FILE* out_ir;//输出文件名
char* file_name;
int main(int argc, char** argv){
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if(argc == 3){
        out_ir=fopen(argv[2],"w");
        file_name=mystrdup(argv[2]);
    }else{
        printf("the number of args is not right!\n");
        return 1;
    }
    if (!f){
        perror(argv[1]);
        return 1;
    }   
    yyrestart(f);
    yyparse();
    if(numOferror==0){//没有错误
        //打印
        if(root!=NULL){
            init_hash();//加入read和write函数
            dfs_lab3(root);
            //printf_irs();   
            //printf("------------------------\n");
            allIRtoMips(out_ir);      
        }        
    }
    fclose(out_ir);
    fclose(f);
    return 0;
}
