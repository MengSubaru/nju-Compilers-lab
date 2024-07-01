#include "symbol_table.h"
void error_print(int idx,char *msg,int line){
    if(line!=error_line){
        printf("Error type %d at Line %d: %s\n",idx,line,msg);
        error_line=line;
    }
}
char* mystrdup(char* string){
    char* res=(char *)malloc(strlen(string) + 1);
    strcpy(res, string);
    return res;
}
int child_num(TreeNode*root){
    int num=0;
    root=root->firstChild;
    while(root!=NULL){
        num++;
        root=root->brother;
    }    
    return num;
}
void init_hash(){
    for(int i=0;i<SIZE_TABLE;i++){
        hash_table[i]=NULL;
    }
    returnType=NULL;

    char* read_func=(char*)malloc(5);
    strcpy(read_func,"read");
    int ele=hash_insert(read_func);
    hash_table[ele]=(Type)malloc(sizeof(struct Type_));
    hash_table[ele]->name=read_func;
    hash_table[ele]->kind=FUNC;
    hash_table[ele]->u.func.def=1;
    hash_table[ele]->u.func.input=NULL;
    hash_table[ele]->u.func.line=0;
    hash_table[ele]->u.func.output=(Type)malloc(sizeof(struct Type_));
    hash_table[ele]->u.func.output->kind=BASIC;
    hash_table[ele]->u.func.output->name=NULL;
    hash_table[ele]->u.func.output->u.basic=0;

    char* write_func=(char*)malloc(6);
    strcpy(write_func,"write");
    ele=hash_insert(write_func);
    hash_table[ele]=(Type)malloc(sizeof(struct Type_));
    hash_table[ele]->name=write_func;
    hash_table[ele]->kind=FUNC;
    hash_table[ele]->u.func.def=1;
    hash_table[ele]->u.func.input=(FieldList)malloc(sizeof(struct FieldList_));
    hash_table[ele]->u.func.input->name=NULL;//write函数的参数只有一个，但名字实际上不存在
    hash_table[ele]->u.func.input->tail=NULL;
    hash_table[ele]->u.func.input->type=(Type)malloc(sizeof(struct Type_));
    hash_table[ele]->u.func.input->type->name=NULL;
    hash_table[ele]->u.func.input->type->kind=BASIC;
    hash_table[ele]->u.func.input->type->u.basic=0;
    hash_table[ele]->u.func.output=(Type)malloc(sizeof(struct Type_));
    hash_table[ele]->u.func.output->kind=BASIC;
    hash_table[ele]->u.func.output->name=NULL;
    hash_table[ele]->u.func.output->u.basic=0;
}
int hash_pjw(char* name){
    unsigned int val = 0, i;
    for (; *name; ++name){
        val = (val << 2) + *name;
        if (i = val & ~SIZE_TABLE){
            val = (val ^ (i >> 12)) & SIZE_TABLE;
        }       
    }
    return val;
}
int hash_insert(char* name){//找到一个空位插入
    int val=hash_pjw(name);
    if(hash_table[val]==NULL){
        return val;
    }else{
        for(int i=val;i<SIZE_TABLE;){//循环一遍的情况？
            if(hash_table[i]==NULL){//默认是一定能找到空位的
                return i;
            }
            i=(i+1)%SIZE_TABLE;
        }
    }  
}
char *get_VarDecName(TreeNode* root){//传入VarDec,返回它的名字
    TreeNode* IDnode=root;//寻找varDec的标识符
    while(child_num(IDnode)!=1){
        IDnode=IDnode->firstChild;
    }//退出循环时IDnode必定指向标识符
    return IDnode->firstChild->val.string;
}
FieldList deal_with_Def(TreeNode *root,const FieldList head){//传入一个不为空的Def,以及当前结构体的第一个成员变量（加入新成员时要遍历头来确保无重名），返回一个或多个构建好的属于结构体的成员变量的类型单元（如果出错就返回NULL？）
    TreeNode* type=root->firstChild->firstChild;//TYPE或StructSpecifier
    struct Type_ tmp;
    tmp.name=NULL;
    deal_with_type(type,&tmp);//处理结束后tmp里面的u和kind存放有类型信息，如果类型定义了struct，deal_with_type就会自动检查并填表

    root=root->firstChild->brother;//DecList

    //依次处理后续的Dec

    FieldList res=(FieldList)malloc(sizeof(struct FieldList_));
    res->name=NULL;

    FieldList iter_field=res;//迭代器

    FieldList last=NULL;//指向倒数第二个成员变量
    TreeNode* iter_node=root->firstChild;//指向Dec,Dec的迭代器

    //每定义一个成员变量都要看是否与之前的成员冲突
    while(1){
        if(child_num(iter_node)!=1){//该成员变量试图对结构体成员进行定义         ---------------错误类型15------------------
            error_print(15,"try to init in struct!",iter_node->lineNumber);
            //即使此成员变量试图赋初值也依然加入结构体的成员中
            //因此后续代码继续执行
        }

        TreeNode* varDec=iter_node->firstChild;//指向VarDec

        //遍历一遍已经加入的成员变量，看看是否有重名
        FieldList pnt=head;
        int canDo=1;//bool变量，为1代表可以加入成员变量
        while(pnt!=NULL){
            if(strcmp(pnt->name,get_VarDecName(varDec))==0){//重复定义
                canDo=0;
                break;
            }
            pnt=pnt->tail;
        }
        if(canDo){
            iter_field->type=(Type)malloc(sizeof(struct Type_));
            iter_field->type->kind=tmp.kind;
            iter_field->type->name=NULL;
            iter_field->type->u=tmp.u;//如果是指针的赋值也无需复制，因为要保证指向的是hash_table的同一个元素
            //iter_field->name=NULL;//不赋值似乎也可以
            iter_field->tail=NULL;
            deal_with_VarDec_Struct(varDec,iter_field);
        }else{
            error_print(15,"define repeatedly!",iter_node->lineNumber);
        }
        
        if(iter_node->brother==NULL){
            break;
        }else{//后面还有Dec
            if(canDo){//本次遍历在尾部加入了成员变量
                iter_field->tail=(FieldList)malloc(sizeof(struct FieldList_));
                last=iter_field;
                iter_field=iter_field->tail;//在tail后面继续加结构体成员
                iter_field->name=NULL;
            }
            iter_node=iter_node->brother->brother->firstChild;//指向下一个Dec
        }
    }

    //无需释放tmp的空间
    if(last==NULL){//只有一个元素
        if(res->name==NULL){//仅有的一个元素有问题，name并未赋值，释放空间并返回空
            free(res);
            return NULL;
        }else{//仅有一个元素，填入数据后返回
            return res;
        }
    }else{//不止一个元素
        if(iter_field->name==NULL){//最后一个的空间未填入元素，代表出现问题，释放最后的空间
            free(iter_field);
            last->tail=NULL;
        }
        return res;
    }
}
void deal_with_VarDec_Struct(TreeNode* root,FieldList cont){//处理结构体成员变量的VarDec以及函数参数的VarDec
    if(child_num(root)==1){//该成员不是数组
        cont->name=mystrdup(root->firstChild->val.string);
    }else{        
        Type array=(Type)malloc(sizeof(struct Type_));
        array->name=NULL;//数组的最后一个元素应该是用于告知类型，不属于变量，不具备名称

        //cont的type存有元素的类型信息
        array->kind=cont->type->kind;
        array->u=cont->type->u;//可以直接拷贝

        TreeNode* iter=root;
        while(1){
            if(child_num(iter)==1){//到头
                break;
            }
            TreeNode* num=iter->firstChild->brother->brother;//INT对应节点

            Type tmp=(Type)malloc(sizeof(struct Type_));
            tmp->kind=ARRAY;
            tmp->name=NULL;
            tmp->u.array.size=num->val.val_int;
            tmp->u.array.elem=array;

            array=tmp;
            iter=iter->firstChild;//下一个VarDec
        }

        iter=iter->firstChild;//VarDec->ID
        //iter现在指向的是这个数组的标识符
        cont->name=mystrdup(iter->val.string);
        cont->type=array;    
    }
}
int hash_find_struct(char *name){//给StructSpecifier->STRUCT Tag用，用于找该Tag对应的struct是否已经定义过
    int val=hash_pjw(name);
    if(hash_table[val]!=NULL){//找
        for(;val<SIZE_TABLE;){//循环一遍的情况？假设总能找到空位
            if(hash_table[val]==NULL){//找到头
                return val;
            }else if(hash_table[val]->kind==STRUCTURE&&strcmp(name,hash_table[val]->u.structure->name)==0){//找到该结构体
                return val;
            }
            val=(val+1)%SIZE_TABLE;
        }
    }
    return val;
}
void free_FieldList(FieldList var){//释放这个FieldList
    if(var->name!=NULL){
        free(var->name);
    }
    if(var->type==NULL){//无可释放元素
        free(var);
        return ;
    }

    //var->type不为NULL
    if(var->type->kind==ARRAY){
        Type type=var->type;
        Type iter;//迭代器
        while(type->kind==ARRAY){
            iter=type;
            type=type->u.array.elem;//指向下一个元素
            free(iter);
        }
        //type现在指向这个数组元素的类型
        free(type);
    }else{
        free(var->type);
    }//类型只有三种，basic代表基本类型，需要释放基本类型这个节点；
    //struct代表一个存在符号表中的结构体，该成员变量的释放不应该影响作用域为全局的结构体的定义，即同样只释放类型节点，但不释放它指向的结构体
    //array代表要逐个释放
    free(var);
}
FieldList deal_with_Struct(char* name,TreeNode* DefList,int line){//处理StructSpecifier : STRUCT OptTag LC DefList RC，传入结构体的名字以及DefList以及该结构体的行号
    FieldList tmp=(FieldList)malloc(sizeof(struct FieldList_));
    int error_struct=0;//判断该结构体是否与其他变量重名的bool变量
    if(name==NULL){//代表这个结构体仅供当前变量使用
        tmp->name=NULL;
    }else{//代表最后要把它赋值给两个元素
        //有名字的结构体应该检查是否与其他结构体或变量重名     ----------------------错误类型16-----------------------
        if(check_same_name(name)){
            error_struct=1;
            error_print(16,"struct duplicates name!",line);
            tmp->name=NULL;
        }else{//未重名，填入结构体的name字段
            tmp->name=mystrdup(name);//结构体名字
        }     
    }   
    tmp->type=NULL;//结构体头部弃用type,tail指向第一个成员变量
    
    tmp->tail=NULL;
    FieldList iter=tmp;//迭代用
    while(DefList!=NULL){
        DefList=DefList->firstChild;//指向DefList的第一个Def
        iter->tail=deal_with_Def(DefList,tmp->tail);//可能在里面遇见错误类型？传入Def与该结构体的第一个成员变量
        DefList=DefList->brother;//指向下一个DefList

        while(1){//不能简单地取tail
            if(iter->tail==NULL){
                break;
            }
            iter=iter->tail;
        }
    }
    if(error_struct){//释放tmp的空间
        iter=tmp;
        tmp=tmp->tail;//指向第一个成员变量，结构体头的name，type均为NULL
        free(iter);
        while(tmp!=NULL){
            iter=tmp;
            tmp=tmp->tail;
            free_FieldList(iter);
        }
        return NULL;
    }
    //没有重名，正常
    return tmp;
}
int deal_with_type(TreeNode* type,Type tar){//type为TYPE OR StructSpecifier，tar为要存放信息的指针,返回值为-1代表类型有误
    if(strcmp("TYPE",type->tokenName)==0){//基本类型
        tar->kind=BASIC;
        if(strcmp(type->val.string,"int")==0){//int               
            tar->u.basic=0;
        }else{//float
            tar->u.basic=1;
        }
    }else{//结构体，type->tokenname=StructSpecifier
        //如果不带括号就去前面找，否则就缓存结构体类型以及当前类型
        tar->kind=STRUCTURE;
        if(child_num(type)==2){//STRUCT Tag
            type=type->firstChild->brother->firstChild;//Tag->ID
            int val=hash_find_struct(type->val.string);
            if(hash_table[val]==NULL){//未找到此结构体的定义                                                -------------错误类型17---------------
                error_print(17,"struct is not defined!",type->lineNumber);
                //该类型所定义的变量无效
                return -1;
            }else{//该结构体被定义过，可以正常使用
                tar->u.structure=hash_table[val]->u.structure;//确保指向同一个元素
            }       
        }else{//STRUCT OptTag LC DefList RC  注意在OptTag不为空时该定义同时声明了一种类型
            TreeNode* OptTag_LC=type->firstChild->brother;//这个孩子为OptTag或LC
            if(strcmp(OptTag_LC->tokenName,"OptTag")==0){//OptTag_LC->tokenname=OptTag同时声明了一种类型
                TreeNode *DefList=NULL;
                if(strcmp(OptTag_LC->brother->brother->tokenName,"RC")){//DefList不为空
                    DefList=OptTag_LC->brother->brother;
                }
                FieldList tmp=deal_with_Struct(OptTag_LC->firstChild->val.string,DefList,OptTag_LC->lineNumber);//最后要把它赋值给两个元素
                if(tmp==NULL){//代表定义的新类型与已有类型或变量重名，该类型定义的变量无效
                    tar->u.structure=NULL;
                    return -1;
                }

                tar->u.structure=tmp;

                //在deal_with_Struct里已经检查过结构体重名问题了
                int eleOfStruct=hash_insert(tmp->name);//找空位放置这个结构体类型
                hash_table[eleOfStruct]=(Type)malloc(sizeof(struct Type_));
                hash_table[eleOfStruct]->u.structure=tmp;
                hash_table[eleOfStruct]->kind=STRUCTURE;
                hash_table[eleOfStruct]->name=NULL;//结构体不应该具有名字
            }else{//该结构体只对当前变量有用
                TreeNode *DefList=NULL;
                if(strcmp(OptTag_LC->brother->tokenName,"RC")){//DefList不为空
                    DefList=OptTag_LC->brother;
                }
                tar->u.structure=deal_with_Struct(NULL,DefList,OptTag_LC->lineNumber);//这里因为结构体无名，所以line无意义
            }
        }
    }
    return 0;
}
int deal_with_VarDec(TreeNode* root,Type type){//root为VarDec,type为存放有类型信息的指针,返回该符号在符号表的位置
    //进入此函数前需要考虑重名问题
    if(child_num(root)==1){//不是数组
        int ele=hash_insert(root->firstChild->val.string);//在符号表插入的位置
        hash_table[ele]=(Type)malloc(sizeof(struct Type_));
        hash_table[ele]->name=mystrdup(root->firstChild->val.string);
        //类型处理部分

        hash_table[ele]->u=type->u;//可以直接拷贝
        hash_table[ele]->kind=type->kind;
        return ele;
    }else{//数组
        TreeNode* iter=root;
        while(1){
            if(child_num(iter)==1){//到头
                break;
            }
            iter=iter->firstChild;
        }
        iter=iter->firstChild;//VarDec->ID
        //iter现在指向的是这个数组的标识符
        int ele=hash_insert(iter->val.string);
        hash_table[ele]=create_type_array(root,type);

        return ele;
    }
}
Type create_type_array(TreeNode* root, Type type){
    //先处理类型
    Type array=(Type)malloc(sizeof(struct Type_));
    array->name=NULL;//数组的最后一个元素应该是用于告知类型，不属于变量，不具备名称

    array->u=type->u;
    array->kind=type->kind;

    TreeNode* iter=root;
    while(1){
        if(child_num(iter)==1){//到头
            break;
        }
        TreeNode* num=iter->firstChild->brother->brother;//INT对应节点

        Type tmp=(Type)malloc(sizeof(struct Type_));
        tmp->kind=ARRAY;
        tmp->name=NULL;
        tmp->u.array.size=num->val.val_int;
        tmp->u.array.elem=array;

        array=tmp;
        iter=iter->firstChild;
    }

    iter=iter->firstChild;//VarDec->ID
    //iter现在指向的是这个数组的标识符

    Type res=(Type)malloc(sizeof(struct Type_));
    res->kind=ARRAY;
    res->name=mystrdup(iter->val.string);
    res->u.array.elem=array;
    res->u.array.size=0;//数组头的size无意义
    return res;
}

FieldList deal_Func(TreeNode* root,int* error_happen){//处理函数的参数,传入FunDec节点以及一个记录参数是否有问题的指针，返回值为参数的FieldList串
    char* nameOfFunc=root->firstChild->val.string;//函数的名字
    //处理函数的传入参数
    if(child_num(root)==3){//空参数列表
        return NULL;
    }else{
        TreeNode* ParaDec=root->firstChild->brother->brother->firstChild;//第一个ParaDec

        FieldList res=(FieldList)malloc(sizeof(struct FieldList_));
        res->name=NULL;
        res->tail=NULL;
        res->type=NULL;

        int idx=1;//记录当前处理到第几个参数
        FieldList iter=res;//指向同一片单元
        while(1){
            struct Type_ type_now;
            type_now.name=NULL;
            int status=deal_with_type(ParaDec->firstChild->firstChild,&type_now);//拿到第一个参数的类型信息

            if(status==-1){//当前参数无效，该函数也无效
                *error_happen=1;
            }else{
                //即使error_happen=1，仍然要将其他没有错误的参数作为全局变量插入符号表中，所以不能直接退出循环
                //检测重名
                int checkValue;
                if(child_num(ParaDec->firstChild->brother)==4){//形参为数组类型
                    Type true_type=create_type_array(ParaDec->firstChild->brother,&type_now);
                    checkValue=checkArgDuplicate(get_VarDecName(ParaDec->firstChild->brother),idx,nameOfFunc,true_type,ParaDec->lineNumber);
                    free(true_type->name);
                    free_Type(true_type);                   
                }else{
                    checkValue=checkArgDuplicate(get_VarDecName(ParaDec->firstChild->brother),idx,nameOfFunc,&type_now,ParaDec->lineNumber);
                }
               
                if(checkValue==1){
                    //存在重名或类型不一致情况，当前变量不处理，标记函数有问题
                    status=-1;
                    *error_happen=1;
                }else{//等于0或2
                    if(!checkValue){////不重名并且需要插入
                        int loc=deal_with_VarDec(ParaDec->firstChild->brother,&type_now);//需要检查重名，这个函数负责向全局符号表中插入元素
                        hash_table[loc]->arg=idx;
                        hash_table[loc]->func=mystrdup(nameOfFunc);
                    }
                    //如果checkValue==2代表与某一个函数声明或定义的形参一致，符号表中只保留一个所以不需要插入

                    iter->type=(Type)malloc(sizeof(struct Type_));
                    iter->type->kind=type_now.kind;
                    iter->type->u=type_now.u;//如果是指针的赋值也可以直接复制，因为要保证指向的是hash_table的同一个元素
                    iter->type->name=NULL;//类型的名字无意义

                    deal_with_VarDec_Struct(ParaDec->firstChild->brother,iter);//负责填写name字段
                }               
            }
            if(ParaDec->brother==NULL){//无后续参数
                break;
            }
            ParaDec=ParaDec->brother->brother->firstChild;//下一个ParaDec
            idx++;
            if(status!=-1){//这个参数无误，可以申请下一片空间
                iter->tail=(FieldList)malloc(sizeof(struct FieldList_));//为下一个参数申请空间
                iter=iter->tail;//指向下一个参数
                iter->tail=NULL;
                iter->name=NULL;
                iter->type=NULL;
            }                
        }
        if(*error_happen==1){//当前函数有问题，释放空间
            while(res!=NULL){
                FieldList tmp_iter=res;
                res=res->tail;
                free_FieldList(tmp_iter);
            }
            return NULL;
        }
        return res;
    }
}
void insert_table(TreeNode *root){//root为ExtDef或Def
    struct Type_ tmp;//申请一个临时变量，用tmp记录当前Def语句的类型信息
    tmp.name=NULL;//类型的name无意义，具体的name归每个声明的变量或函数定义
    int status=deal_with_type(root->firstChild->firstChild,&tmp);//可以用返回值记录是否出错？
    //status==-1时，tmp中无信息，由此类型定义的变量和函数不插入符号表

    //处理完之后tmp.u和tmp.kind存放有类型的信息,如果类型定义了struct，deal_with_type就会自动检查并填表

    if(strcmp("ExtDef",root->tokenName)==0){//ExtDef
        TreeNode* child_1=root->firstChild->brother;//拿到ExtDef的第一个孩子
        if(strcmp(child_1->tokenName,"ExtDecList")==0){//全局变量
            if(status!=-1){//由错误类型声明的全局变量不应该加入符号表
                TreeNode* VarDec=child_1->firstChild;//第一个VarDec一定存在         
                while(1){
                    if(check_same_name(get_VarDecName(VarDec))){//重名
                        error_print(3,"var duplicates name!",VarDec->lineNumber);
                    }else{//不重名就加入符号表
                        int loc=deal_with_VarDec(VarDec,&tmp);
                        hash_table[loc]->arg=0;
                        hash_table[loc]->func=NULL;
                    }                
                    if(VarDec->brother==NULL){
                        break;
                    }
                    VarDec=VarDec->brother->brother->firstChild;//指向下一个VarDec
                }
            }           
        }else if(strcmp(child_1->tokenName,"FunDec")==0){//函数
        //返回值似乎应该存为全局变量供return语句使用？
            int error_happen=0;//假设无错误
            FieldList input_func=deal_Func(child_1,&error_happen);//必须调用一次函数来将参数代表的全局变量加入符号表
            if(status==-1){//返回类型有误
                //需要检查函数名吗？             

                //按照一行只有一个错误的原则，error_happen应该为0，但如果有错误也会自动处理
                //由于返回类型有误，函数不加入符号表，下面释放参数链表串
                while(input_func!=NULL){
                    FieldList tmp_iter=input_func;
                    input_func=input_func->tail;
                    free_FieldList(tmp_iter);
                }
                free_Type(returnType);
                returnType=NULL;
            }else{//返回类型无误
                free_Type(returnType);
                returnType=copy_Type(&tmp);
                int val=hash_pjw(child_1->firstChild->val.string);
                for(;val<SIZE_TABLE;){
                    if(hash_table[val]==NULL){
                        break;
                    }else if(hash_table[val]->kind==FUNC&&strcmp(child_1->firstChild->val.string,hash_table[val]->name)==0){
                        break;
                    }
                    val=(val+1)%SIZE_TABLE;
                }
                if(hash_table[val]==NULL){//可插入
                    if(!error_happen){
                        hash_table[val]=(Type)malloc(sizeof(struct Type_));
                        hash_table[val]->name=mystrdup(child_1->firstChild->val.string);
                        hash_table[val]->kind=FUNC;
                        hash_table[val]->u.func.output=(Type)malloc(sizeof(struct Type_));
                        hash_table[val]->u.func.output->kind=tmp.kind;
                        hash_table[val]->u.func.output->name=NULL;//函数返回值没有名字
                        hash_table[val]->u.func.output->u=tmp.u;//即使涉及到指针也可以直接拷贝
                        hash_table[val]->u.func.input=input_func;//如果参数列表为空但error_happen为0，返回为NULL，不影响

                        if(strcmp(child_1->brother->tokenName,"SEMI")==0){//函数声明             
                            hash_table[val]->u.func.def=0;//只是声明
                            hash_table[val]->u.func.line=child_1->lineNumber;
                        }else{//函数定义
                            hash_table[val]->u.func.def=1;//函数定义
                        }
                      
                    }//error_happen=1时参数中存在错误,没有错误的参数已经加入到符号表中，但该函数不应该加入符号表              
                }else{//找到同名函数，可能是声明可能是定义
                    if(!error_happen){//形参无误，下面检查是否函数重复定义
                        if(strcmp(child_1->brother->tokenName,"CompSt")==0){//当前为函数定义
                            if(hash_table[val]->u.func.def){
                                //已经有过一次定义且当前的也为函数定义                                         
                                error_print(4,"function redefined!",child_1->lineNumber); 
                            }else{//当前为函数定义且之前为声明
                                FieldList last_list=hash_table[val]->u.func.input;//取得之前声明的串
                                hash_table[val]->u.func.input=input_func;//换为函数定义的串
                                hash_table[val]->u.func.def=1;
                                hash_table[val]->u.func.output=(Type)malloc(sizeof(struct Type_));
                                hash_table[val]->u.func.output->kind=tmp.kind;
                                hash_table[val]->u.func.output->name=NULL;//函数返回值没有名字
                                hash_table[val]->u.func.output->u=tmp.u;//即使涉及到指针也可以直接拷贝
                                input_func=last_list;//将input指向需要释放的形参串
                            }                    
                        }
                        //当前是函数声明
                        //如果已经定义过了，那当前的声明只起到了定义了一些冗余全局变量的作用，符号表中的函数形参仍以符号表中的形参串为准;
                        //如果符号表中的函数也为声明，那形参表以谁为准都无所谓
                                                            
                        //已知新的函数的参数列表没有错误，现在比对函数的类型是否一致
                        FieldList iter1=input_func;//指向第一个参数
                        FieldList iter2=hash_table[val]->u.func.input;
                        if(iter1==NULL){
                            if(iter2!=NULL){//参数数量不一致
                                error_print(19,"the number of args of function collision!",child_1->lineNumber);
                            }
                        }else if(iter2==NULL){//参数数量不一致
                            error_print(19,"the number of args of function collision!",child_1->lineNumber);
                        }else{//都至少有一个形参
                            while(1){
                                if(check_same_type(iter1->type,iter2->type)){//类型不一致
                                    error_print(19,"the args of function collision!",child_1->lineNumber);
                                    break;
                                }

                                if(iter1->tail==NULL){
                                    if(iter2->tail!=NULL){//数量不一致
                                        error_print(19,"the number of args of function collision!",child_1->lineNumber);
                                    }
                                    break;
                                }else if(iter2->tail==NULL){
                                    error_print(19,"the number of args of function collision!",child_1->lineNumber);
                                    break;
                                }else{
                                    iter1=iter1->tail;
                                    iter2=iter2->tail;
                                }
                            }
                        }

                        while(input_func!=NULL){//释放当前形参串空间
                            FieldList tmp_iter=input_func;
                            input_func=input_func->tail;
                            free_FieldList(tmp_iter);
                        }
                    }
                    if(check_same_type(&tmp,hash_table[val]->u.func.output)){//返回值类型冲突
                        error_print(19,"the type of return collision!",child_1->lineNumber);
                    }
                    //形参有误，函数不应加入符号表，也无必要和已有的正确函数比对            
                }
            }
        }else{//只是定义了一个结构体，或者什么也没有定义
            //do nothing
        }
    }else{//Def
        if(status!=-1){//类型有问题时变量无需加入符号表，如果涉及到表达式也无需检查（左值的类型有误，无论右值类型是什么都是错误）
            TreeNode* iter_node=root->firstChild->brother->firstChild;//指向第一个Dec，Dec的迭代器
            //依次处理Dec

            while(1){
                if(check_same_name(get_VarDecName(iter_node->firstChild))){//重名,不应该加入符号表
                    error_print(3,"var duplicates name!",iter_node->lineNumber);
                }else{//不重名，加入符号表                   
                    if(child_num(iter_node)==3){//VarDec ASSIGNOP Exp此为赋值语句
                        //注意先处理EXP再加入符号表，考虑int a=a+3;这种情况
                        int LRvalue;
                        Type RValue=deal_with_EXP(&LRvalue,iter_node->firstChild->brother->brother);//记录右边的EXP的返回类型

                        int ele=deal_with_VarDec(iter_node->firstChild,&tmp);
                        hash_table[ele]->arg=0;
                        hash_table[ele]->func=NULL;
                        if(RValue!=NULL){//代表表达式有唯一类型，与左值进行比对
                            if(check_same_type(RValue,hash_table[ele])){
                                error_print(5,"types on either side of assign don't match!",iter_node->lineNumber);
                            }
                            free_Type(RValue);                         
                        }//如果为NULL在内部处理时已经报过错

                    //注意这里传入的类型不是tmp而是符号表中该元素的类型，因为如果该元素为数组，它的类型应该还包含它的维度以及大小之类的信息
                    //处理表达式右侧，内部有错误自己报，不影响左值在符号表中的情况
                    }else{
                        int ele=deal_with_VarDec(iter_node->firstChild,&tmp);
                        hash_table[ele]->arg=0;
                        hash_table[ele]->func=NULL;
                    }
                }
                
                if(iter_node->brother==NULL){//后续没有别的Dec
                    break;
                }
                iter_node=iter_node->brother->brother->firstChild;//指向下一个Dec
            }
        }
    }
    //无需释放临时变量tmp的空间
}
int check_same_name(char* name){//用于检查该名称是否与其他结构体或变量重名，重名返回1，否则返回0
    int val=hash_pjw(name);
    if(hash_table[val]!=NULL){//
        for(;val<SIZE_TABLE;){
            if(hash_table[val]==NULL){
                return 0;
            }else if(hash_table[val]->kind==BASIC||hash_table[val]->kind==ARRAY){//比对name字段
                if(strcmp(name,hash_table[val]->name)==0){//与其他某个基本类型变量或数组变量重名
                    return 1;
                }
            }else if(hash_table[val]->kind==STRUCTURE){
                if(hash_table[val]->name==NULL){//该元素为结构体的定义
                    if(strcmp(name,hash_table[val]->u.structure->name)==0){
                        return 1;
                    }
                }else{//该变量为类型为结构体的变量
                    if(strcmp(name,hash_table[val]->name)==0){
                        return 1;
                    }
                }
            }//函数不考虑

            val=(val+1)%SIZE_TABLE;
        }
    }
    return 0;
}
int checkArgDuplicate(char* name,int idx,char* func,Type type,int line){//检测作为函数的参数是否与其他变量重名，如果与自身重名则返回2,与其他变量重名返回1，否则可插入返回0
    //name为变量名，idx为变量在函数中是第几个参数，func为函数名,type为该变量的类型
    int val=hash_pjw(name);
    for(;val<SIZE_TABLE;){
        if(hash_table[val]==NULL){
            return 0;//无重名变量，插入                      
        }else if(hash_table[val]->kind==BASIC||hash_table[val]->kind==ARRAY){//比对name字段
            if(strcmp(name,hash_table[val]->name)==0){//与其他某个基本类型变量或数组变量重名
                if(hash_table[val]->arg==idx&&strcmp(func,hash_table[val]->func)==0){//两者都是函数参数且位置相同且所属函数相同
                    //还要比对参数类型
                    if(check_same_type(hash_table[val],type)){//类型不一致
                        error_print(19,"the args of function collision!",line);//声明与定义或者声明与声明之间存在参数类型冲突
                        return 1;
                    }else{
                        return 2;
                    }                   
                }else{//与其他变量重名
                    error_print(3,"redefine!",line);//与其他变量重名
                }
                return 1;
            }
            /*else{//不同名
                if(hash_table[val]->arg==idx&&strcmp(func,hash_table[val]->func)==0){//是同一个参数位置
                    if(check_same_type(hash_table[val],type)){//类型冲突
                        error_print(19,"the args of function collision!",line);//声明与定义或者声明与声明之间存在参数类型冲突
                        wrong=1;
                    }//不可以直接退出，还需检查重名
                }
            }*/
        }else if(hash_table[val]->kind==STRUCTURE){
            if(hash_table[val]->name==NULL){//该元素为结构体的定义
                if(strcmp(name,hash_table[val]->u.structure->name)==0){
                    error_print(3,"arg has same name as another struct!",line);
                    return 1;
                }
            }else{//该变量为类型为结构体的变量
                if(strcmp(name,hash_table[val]->name)==0){
                    if(hash_table[val]->arg==idx&&strcmp(func,hash_table[val]->func)==0){//两者都是函数参数且位置相同且所属函数相同
                        if(type->kind==STRUCTURE&&type->u.structure==hash_table[val]->u.structure){
                            return 2;
                        }
                        error_print(19,"the args of function collision!",line);
                    }else{
                        error_print(3,"redefine!",line);
                    }
                    return 1;
                }
                /*else{//不同名
                    if(hash_table[val]->arg==idx&&strcmp(func,hash_table[val]->func)==0){//是同一个参数位置
                        if(check_same_type(hash_table[val],type)){//类型冲突
                            error_print(19,"the args of function collision!",line);//声明与定义或者声明与声明之间存在参数类型冲突
                            wrong=1;
                        }//不可以直接退出，还需检查重名
                    }
                }*/
            }
        }//函数不考虑
        val=(val+1)%SIZE_TABLE;
    }
}
int check_same_type(Type type1,Type type2){//检测两种类型是否一致，一致返回0，不一致返回1
    if(type1->kind==type2->kind){
        if(type1->kind==BASIC){
            if(type1->u.basic==type2->u.basic){
                return 0; 
            }else{
                return 1;
            }
        }else if(type1->kind==STRUCTURE){
            if(type1->u.structure==type2->u.structure){
                return 0;
            }else{
                return 1;
            }
        }else if(type1->kind==ARRAY){
            int depth=0;
            Type iter1=type1->u.array.elem;
            Type iter2=type2->u.array.elem;
            while(iter1->kind==ARRAY){
                iter1=iter1->u.array.elem;
                depth++;
            }
            while(iter2->kind==ARRAY){
                depth--;
                iter2=iter2->u.array.elem;
            }

            if(depth==0){//两个指针同时指向最后的基本类型
                return check_same_type(iter1,iter2);//递归调用
            }else{//数组深度不一致
                return 1;
            }
        }else{//不应该到达
            assert(0);
        }
    }else{
        return 1;
    }
}

Type copy_Type(Type tar){//复制一份type返回，供deal_with_EXP使用
    Type res=(Type)malloc(sizeof(struct Type_));
    res->kind=tar->kind;
    if(tar->kind==BASIC||tar->kind==STRUCTURE){
        res->u=tar->u;
    }else if(tar->kind==ARRAY){
        Type tmp=tar;
        Type iter=res;
        while(tmp->kind==ARRAY){
            iter->kind=ARRAY;
            iter->u.array.size=tmp->u.array.size;
            iter->u.array.elem=(Type)malloc(sizeof(struct Type_));
            iter=iter->u.array.elem;
            tmp=tmp->u.array.elem;
        }
        iter->kind=tmp->kind;
        iter->u=tmp->u;
    }else{//函数不应该在这里处理
        assert(0);
    }
    return res;
}
void free_Type(Type tar){//与copy_Type配合使用
    if(tar==NULL){
        return;
    }
    if(tar->kind==ARRAY){
        Type iter=tar->u.array.elem;
        while(iter->kind==ARRAY){
            Type tmp=iter;
            iter=iter->u.array.elem;
            free(tmp);
        }
        free(iter);
    }
    free(tar);
}
int hash_find_name(char* name){//传入一个函数或变量的标识符，返回这个标识符在符号表中的位置
    int val=hash_pjw(name);
    for(;val<SIZE_TABLE;){
        if(hash_table[val]==NULL){//未找到函数定义
            return val;
        }else if(hash_table[val]->name!=NULL&&strcmp(name,hash_table[val]->name)==0){//如果hash_table[val].name==NULL代表这是个结构体（类型），不是变量
            return val;
        }
        val=(val+1)%SIZE_TABLE;
    }
}
void dfs_init_symbol_table(TreeNode* root){

    if(strcmp(root->tokenName,"ExtDef")==0){
        insert_table(root);
        if(child_num(root)==3&&strcmp(root->firstChild->brother->brother->tokenName,"CompSt")==0){
            //returnType里已经存放了函数返回值的类型
            dfs_init_symbol_table(root->firstChild->brother->brother);
        }
    }else if(strcmp(root->tokenName,"Def")==0){
        insert_table(root);
    }else if(strcmp(root->tokenName,"Exp")==0){
        int tmp;
        Type res=deal_with_EXP(&tmp,root);
        free_Type(res);
    }else if(strcmp(root->tokenName,"Stmt")==0){
        if(child_num(root)==3){
            int tmp;
            Type res=deal_with_EXP(&tmp,root->firstChild->brother);

            if(returnType!=NULL){//如果为NULL代表返回类型本就不确定，也不能对return的类型苛求什么
                if(res==NULL||check_same_type(res,returnType)){
                    error_print(8,"type of return doesn't match the type of function!",root->lineNumber);
                }
            }
            free_Type(res);
        }else{
            if(root->firstChild!=NULL){
                dfs_init_symbol_table(root->firstChild);
            }
        }
    }else{//其他的语句正常递归
        if(root->firstChild!=NULL){
            dfs_init_symbol_table(root->firstChild);
        }
    }

    if(root->brother!=NULL){
        dfs_init_symbol_table(root->brother);
    }
    return ;
}

Type deal_with_EXP(int* LRValue,TreeNode* exp){//传入需要处理的EXP以及存放该EXP是左值还是右值类型的int指针,返回exp的类型，返回NULL代表在处理时已经出现类型不一致的问题
    int numOfExp=child_num(exp);
    if(numOfExp==3){//EXP为EXP OP EXP
        char* op=exp->firstChild->brother->tokenName;
        if(strcmp(op,"ASSIGNOP")==0){//为形如a=b=3;这种表达式,其中exp为b=3
            Type t2=deal_with_EXP(LRValue,exp->firstChild->brother->brother);//需要这个孩子同为int型

            Type t1=deal_with_EXP(LRValue,exp->firstChild);//需要这个孩子为int型
            //接下来处理operand1,需要它为左值
            if(*LRValue!=1){//为0代表不是表达式左值
                error_print(6,"the left of assign is RValue!",exp->lineNumber);
            }
            if(t1==NULL||t2==NULL){//在内层处理时已经出错，此时比对类型已经无意义
                free_Type(t1); 
                free_Type(t2);              
                return NULL;
            }
            
            if(check_same_type(t1,t2)){//类型不一致
                error_print(5,"types on either side of assign don't match!",exp->lineNumber);
                free_Type(t1);  
                free_Type(t2); 
                return NULL;//类型不匹配时无法确认返回的类型
            }
            free_Type(t2);
            return t1;
        }else if(strcmp(op,"AND")==0||strcmp(op,"OR")==0){//逻辑运算
            Type t1=deal_with_EXP(LRValue,exp->firstChild);//需要这个孩子为int型

            Type t2=deal_with_EXP(LRValue,exp->firstChild->brother->brother);//需要这个孩子同为int型

            *LRValue=0;//当前表达式的结果是右值
            if(t1==NULL||t2==NULL){//内层处理出错
                free_Type(t1); 
                free_Type(t2);
                return NULL;
            }
            
            if(check_same_type(t1,t2)){//操作数类型不匹配
                error_print(7,"operands don't match!",exp->lineNumber);
                free_Type(t1);   
                free_Type(t2);
                return NULL;//类型不匹配时返回NULL代表出现未定义行为，后续不再考虑类型
            }else if(t1->kind!=BASIC||t1->u.basic){//操作数类型匹配但操作数不是int类型
                free_Type(t1);  
                free_Type(t2);
                error_print(7,"types of operand don't match operator!",exp->lineNumber);//操作数类型不与操作符匹配
                return NULL;
            }
            free_Type(t2);
            return t1;//虽然返回的是t1的类型，但是实际的值应该是由这个表达式产生的不存在的bool变量
        }else if(strcmp(op,"RELOP")==0){//关系运算
            
            Type t1=deal_with_EXP(LRValue,exp->firstChild);

            Type t2=deal_with_EXP(LRValue,exp->firstChild->brother->brother);

            *LRValue=0;
   
            if(t1==NULL||t2==NULL){  
                free_Type(t1);
                free_Type(t2);
                return NULL;
            }

            if(check_same_type(t1,t2)){
                error_print(7,"operands don't match!",exp->lineNumber); 
                free_Type(t1);
                free_Type(t2);
                return NULL;//类型不匹配时返回NULL代表出现未定义行为，后续不再考虑类型
            }else if(t1->kind!=BASIC){//虽然两个操作数类型匹配，但是不是基本类型
                free_Type(t1);
                free_Type(t2);
                error_print(7,"types of operand don't match operator!",exp->lineNumber);//操作数类型不与操作符匹配 
                return NULL;
            }

            free_Type(t1);
            free_Type(t2);

            //返回一个int类型
            Type tmp=(Type)malloc(sizeof(struct Type_));
            tmp->kind=BASIC;
            tmp->u.basic=0;
            return tmp;
        }else if(strcmp(op,"PLUS")==0||strcmp(op,"MINUS")==0||strcmp(op,"STAR")==0||strcmp(op,"DIV")==0){//算术运算符
            Type t1=deal_with_EXP(LRValue,exp->firstChild);

            Type t2=deal_with_EXP(LRValue,exp->firstChild->brother->brother);

            *LRValue=0;
           
            if(t1==NULL||t2==NULL){
                free_Type(t2);
                free_Type(t1);
                return NULL;
            }

            if(check_same_type(t1,t2)){
                error_print(7,"operands don't match!",exp->lineNumber);
                free_Type(t1); 
                free_Type(t2);
                return NULL;//类型不匹配时返回NULL代表出现未定义行为，后续不再考虑类型
            }else if(t1->kind!=BASIC){//虽然两个操作数类型匹配，但是不是基本类型
                error_print(7,"types of operand don't match operator!",exp->lineNumber);//操作数类型不与操作符匹配 
                free_Type(t1);
                free_Type(t2);
                return NULL;
            }
            free_Type(t2);
            //进行运算的操作数是什么类型就返回什么类型
            return t1;
        }else if(strcmp(op,"Exp")==0){//(EXP)
            Type t1=deal_with_EXP(LRValue,exp->firstChild->brother);

            if(t1==NULL){
                return NULL;
            }

            //进行运算的操作数是什么类型就返回什么类型
            return t1;
        }else if(strcmp(op,"LP")==0){//无参数调用函数
            int ele=hash_find_name(exp->firstChild->val.string);
            *LRValue=0;
            if(hash_table[ele]==NULL){
                error_print(2,"use undefined function!",exp->lineNumber);
                return NULL;
            }else if(hash_table[ele]->kind!=FUNC){
                error_print(11,"use \"()\" for variable which is not function!",exp->lineNumber);
                return NULL;
            }

            FieldList arg=hash_table[ele]->u.func.input;

            if(arg!=NULL){
                error_print(9,"args of function don't match!",exp->lineNumber);               
            }

            return copy_Type(hash_table[ele]->u.func.output);            
        }else{//DOT
            Type t1=deal_with_EXP(LRValue,exp->firstChild);

            *LRValue=1;
            if(t1==NULL){
                return NULL;
            }
            
            if(t1->kind!=STRUCTURE){//对非结构体变量使用.操作符
                error_print(13,"use \".\" for variable which is not struct!",exp->lineNumber);               
                free_Type(t1);
                return NULL;
            }else{//查看该结构体的成员变量
                int have=0;//bool 变量
                char* IDname=exp->firstChild->brother->brother->val.string;
                FieldList iter=t1->u.structure->tail;//指向第一个成员变量
                while(iter!=NULL){
                    if(strcmp(iter->name,IDname)==0){//发现确实存在这个成员变量
                        have=1;
                        break;
                    }
                    iter=iter->tail;
                }
                if(!have){//没有这个成员变量
                    error_print(14,"access inexistent filed of struct!",exp->lineNumber);
                    free_Type(t1);
                    return NULL;
                }else{
                    free_Type(t1);
                    return copy_Type(iter->type);
                }
            }
        }
    }else if(numOfExp==2){
        char* op=exp->firstChild->tokenName;
        if(strcmp(op,"MINUS")==0){//取反
            Type t1=deal_with_EXP(LRValue,exp->firstChild->brother);

            *LRValue=0;

            if(t1==NULL){
                return NULL;
            }

            if(t1->kind!=BASIC){//不是基本类型，取反无意义
                error_print(7,"types of operand don't match operator!",exp->lineNumber);
                free_Type(t1); 
                return NULL;//类型不匹配时返回NULL代表出现未定义行为，后续不再考虑类型
            }else{
                return t1;
            }
        }else{//逻辑非
            Type t1=deal_with_EXP(LRValue,exp->firstChild->brother);//需要这个孩子为int型

            *LRValue=0;//当前表达式的结果是右值

            if(t1==NULL){//内层处理出错
                return NULL;
            }

            if(t1->kind!=BASIC||t1->u.basic){//操作数不是int类型
                free_Type(t1);
                error_print(7,"types of operand don't match operator!",exp->lineNumber);
                return NULL;
            }else{
                return t1;
            }
        }
    }else if(numOfExp==4){
        char *ID=exp->firstChild->tokenName;//为ID（函数调用）或EXP（数组）
        if(strcmp(ID,"ID")==0){//ID LP Args RP
            int ele=hash_find_name(exp->firstChild->val.string);           

            if(hash_table[ele]==NULL){
                *LRValue=0;
                error_print(2,"use undefined function!",exp->lineNumber);
                return NULL;
            }else if(hash_table[ele]->kind!=FUNC){
                *LRValue=0;
                error_print(11,"use \"(...)\" for variable which is not function!",exp->lineNumber);
                return NULL;
            }

            FieldList formal_arg=hash_table[ele]->u.func.input;//指向第一个形参

            int consistence=1;//bool 变量，如果为1代表形参与实参完全相同，否则就是形参与实参的数目或类型不匹配

            TreeNode* arg=exp->firstChild->brother->brother->firstChild;//指向第一个实参EXP

            if(formal_arg==NULL){//形参0个，实参却不止一个
                consistence=0;
            }else{
                while(1){
                    Type typeOfArg=deal_with_EXP(LRValue,arg);//取得当前参数的类型
                    if(typeOfArg==NULL){
                        consistence=0;
                        break;
                    }else if(check_same_type(typeOfArg,formal_arg->type)){//参数类型不一致
                        consistence=0;
                        free_Type(typeOfArg);
                        break;
                    }

                    //当前形参与实参的类型一致
                    free_Type(typeOfArg);//释放这个表达式的类型

                    if(arg->brother==NULL){//后面没有别的实参了
                        if(formal_arg->tail!=NULL){//还有多余的形参                 
                            consistence=0;
                        }
                        break;
                    }else{
                        if(formal_arg->tail==NULL){
                            consistence=0;
                            break;
                        }else{
                            formal_arg=formal_arg->tail;
                            arg=arg->brother->brother->firstChild;//指向下一个EXP
                        }
                    }
                }
            }
            *LRValue=0;
            if(!consistence){//存在参数类型不匹配
                error_print(9,"args of function don't match!",exp->lineNumber);
                return NULL;
            }
            
            return copy_Type(hash_table[ele]->u.func.output);   
        }else{//Exp LB Exp RB
            Type idx=deal_with_EXP(LRValue,exp->firstChild->brother->brother);
            if(idx==NULL||idx->kind!=BASIC||idx->u.basic){//不为基本类型或者是基本类型的float
                error_print(12,"non-integer is in \"[...]\"!",exp->lineNumber);
            }

            free_Type(idx);

            Type head=deal_with_EXP(LRValue,exp->firstChild);//处理数组头

            //将数组头与当前的下标访问结合起来
            *LRValue=1;//数组元素为左值
            if(head==NULL){//在递归过程中已经出现问题
                return NULL;
            }else if(head->kind!=ARRAY){//拿到的不是数组类型
                error_print(10,"use \"[...]\" for variable which is not array!",exp->lineNumber);
                free_Type(head);
                return NULL;
            }else{//在符号表中一定是存在的

                Type res=head->u.array.elem;

                free(head);
                return res;
            }
        }
    }else{//numOfExp==1，检测变量在使用时是否定义过
        char* name=exp->firstChild->tokenName;
        if(strcmp(name,"ID")==0){
            *LRValue=1;

            int val=hash_find_name(exp->firstChild->val.string);
            if(hash_table[val]==NULL){
                error_print(1,"use undefined variable!",exp->lineNumber);
                return NULL;
            }else {
                if(hash_table[val]->kind==BASIC||hash_table[val]->kind==STRUCTURE){
                    return copy_Type(hash_table[val]);
                }else{//应该是array
                    return copy_Type(hash_table[val]->u.array.elem);
                }
            }
        }else if(strcmp(name,"INT")==0){
            *LRValue=0;
            Type res=(Type)malloc(sizeof(struct Type_));
            res->kind=BASIC;
            res->u.basic=0;
            return res;
        }else{
            *LRValue=0;
            Type res=(Type)malloc(sizeof(struct Type_));
            res->kind=BASIC;
            res->u.basic=1;
            return res;
        }
    }
}