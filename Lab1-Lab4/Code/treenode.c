#include "treenode.h"
void insertTreeNode(TreeNode*parent,TreeNode*child){
    /*if (parent == NULL || child == NULL) {
        return false;
    }*/
    if(child==NULL){//当前子女生成了空
        return ;
    }
    if (parent->firstChild == NULL) {
        // 如果父节点没有子节点，直接将子节点作为父节点的第一个子节点
        parent->firstChild = child;
    } else {
        // 否则，遍历父节点的子节点链表，找到最后一个子节点，将新子节点插入到其兄弟节点的位置
        TreeNode *current = parent->firstChild;
        while (current->brother != NULL) {
            current = current->brother;
        }
        current->brother = child;
    }
    return ;
    //return true;
}
TreeNode*createTreeNode(char*tokenName,char*realVal,int lineNumber,int type){
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    if (node != NULL) {
        // 为 tokenName 分配内存并复制字符串
        /*if(tokenName!=NULL){
            node->tokenName = malloc(strlen(tokenName) + 1);
            if (node->tokenName != NULL) {
                strcpy(node->tokenName, tokenName);
            } else {
                free(node);
                return NULL;
            }
        }*/
        node->tokenName = (char *)malloc(strlen(tokenName) + 1);
        strcpy(node->tokenName, tokenName);//node->tokenName=strdup(tokenName);
        
        // 为 realVal 分配内存并复制字符串
        /*if(realVal!=NULL){
            node->realVal = malloc(strlen(realVal) + 1);
            if (node->realVal != NULL) {
                strcpy(node->realVal, realVal);
            } else {
                free(node->tokenName); // 释放先前分配的 tokenName 的内存
                free(node);
                return NULL;
            }
        }*/
        node->type=type;
        if(type==1){
            node->val.val_int=atoi(realVal);
        }else if(type==2){
            node->val.val_float=atof(realVal);
        }else if(type==3){
            if(realVal==NULL){
                 node->val.string=NULL;
            }else{//node->val.string=(realVal==NULL)?NULL:strdup(realVal);
                node->val.string = (char *)malloc(strlen(realVal) + 1);
                strcpy(node->val.string,realVal);
            }
        }

        node->lineNumber = lineNumber;
        node->firstChild = NULL;
        node->brother = NULL;
    }
    return node;
}
void dfs(TreeNode* node,int cnt){//节点与缩进次数
    for(int i=0;i<cnt;i++){//cnt*2个空格
        printf("  ");
    }
    if(node->type==0){
        printf("%s (%d)\n",node->tokenName,node->lineNumber);
    }else if(node->type==1){
        printf("%s: %d\n",node->tokenName,node->val.val_int);
    }else if(node->type==2){
        printf("%s: %f\n",node->tokenName,node->val.val_float);
    }else{
        if(node->val.string==NULL){
            printf("%s\n",node->tokenName);
        }else{
            printf("%s: %s\n",node->tokenName,node->val.string);
        }
    }
    
    if(node->firstChild!=NULL){
        dfs(node->firstChild,cnt+1);
    }
    if(node->brother!=NULL){
        dfs(node->brother,cnt);
    }
    return ;
}