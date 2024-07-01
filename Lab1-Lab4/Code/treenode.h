#ifndef TREE
#define TREE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct TreeNode{
        char*tokenName;
        union {
            int val_int;
            float val_float;
            char* string;
        } val;
        int lineNumber;
        int type;//0为非终结符，1为int,2为float,3为string string=NULL代表它属于无值的终结符
        struct TreeNode* firstChild;
        struct TreeNode* brother;
}TreeNode;
void insertTreeNode(TreeNode*parent,TreeNode*child);
TreeNode*createTreeNode(char*tokenName,char*realVal,int lineNumber,int type);  
void dfs(TreeNode* node,int cnt);
#endif