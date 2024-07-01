#include "ir_produce.h"
IRInstruction* ir_head = NULL;
IRInstruction* ir_tail = NULL;
char buffer[IR_BUFFER_SIZE];
int offset = 72;
//copy to provide repeat
char* mynamestrdup(char*p){
    if(strcmp(p,"main")==0){
        char* res = (char*)malloc(strlen(p)+1);
        strcpy(res,p);
        return res;
    }


    char* res = (char*)malloc(strlen(p)+5);
    strcpy(res,"NAME");
    strcpy(res+4,p);
    return res;
}
OperandPointer newOperand(int kind,...){
    OperandPointer p = (Operand*)malloc(sizeof(Operand));
    assert(p!=NULL);
    p->kind = kind;
    va_list args;
    va_start(args,kind);
    p->kind = kind;
    switch (kind)
    {
    case OP_CONSTANT:
        p->element.val = va_arg(args,int);
        break;
    case OP_RELOP:
        p->element.name = mystrdup(va_arg(args,char*));
        break;
    default:
        
        p->element.name = mynamestrdup(va_arg(args,char*));
        break;
    }
    va_end(args);
    return p;
}

void printf_irs(){
    IRInstruction* current = ir_head;
    while (current != NULL) {
        printf("%d %s %d",current->kind, current->instruction,current->debug);
        // 
        current = current->next;
        continue;
        // debug use printfIr
        switch (current->kind)
        {
        case IR_LABEL:
        case IR_FUNCTION:
        case IR_GOTO:
        case IR_READ:
        case IR_WRITE:
        case IR_ARG:
        case IR_PARAM:
        case IR_RETURN:
            /* code */
            printf(" OP %s\n",current->operands.oneOp.op->element.name);
            break;
        case IR_ASSIGN:
        case IR_CALL:
        case IR_GET_ADDR://x = &y
        case IR_READ_ADDR://x = *y
        case IR_WRITE_ADDR://*x = y
            if(current->operands.twoOp.src->kind==OP_CONSTANT)
                printf(" OP1 %s OP2 %d \n",current->operands.twoOp.des->element.name,current->operands.twoOp.src->element.val);
            else 
                printf(" OP1 %s OP2 %s \n",current->operands.twoOp.des->element.name,current->operands.twoOp.src->element.name);
            break;
        case IR_ADD:
        case IR_SUB:
        case IR_MUL:
        case IR_DIV:
            printf(" OP1 %s",current->operands.threeOp.des->element.name);
            if(current->operands.threeOp.src1->kind==OP_CONSTANT){
                printf(" OP2 %d",current->operands.threeOp.src1->element.val);
            }
            else{
                printf(" OP2 %s",current->operands.threeOp.src1->element.name);
            }
            if(current->operands.threeOp.src2->kind==OP_CONSTANT){
                printf(" OP3 %d\n",current->operands.threeOp.src2->element.val);
            }
            else{
                printf(" OP3 %s\n",current->operands.threeOp.src2->element.name);
            }
            break;
        case IR_DEC:
            printf(" OP1 %s OP2 %d\n",current->operands.dec.op->element.name,current->operands.dec.size);
            break;
        case IR_IF_GOTO:
            if(current->operands.ifGoto.x->kind==OP_CONSTANT){
                printf(" OP1 %d",current->operands.ifGoto.x->element.val);
            }
            else{
                printf(" OP1 %s",current->operands.ifGoto.x->element.name);
            }
            printf(" OP2 %s",current->operands.ifGoto.relop->element.name);
            if(current->operands.ifGoto.y->kind==OP_CONSTANT){
                printf(" OP3 %d",current->operands.ifGoto.y->element.val);
            }
            else{
                printf(" OP3 %s",current->operands.ifGoto.y->element.name);
            }
            printf(" OP4 %s\n",current->operands.ifGoto.z->element.name);
            break;
        default:
            assert(0);
            break;
        }
        current = current->next;
    }
}
void printf_ir(IRInstruction *ir){
    IRInstruction* current = ir;
    printf("%d %s %d",current->kind,current->instruction,current->debug);
    switch (current->kind)
    {
    case IR_LABEL:
    case IR_FUNCTION:
    case IR_GOTO:
    case IR_READ:
    case IR_WRITE:
    case IR_ARG:
    case IR_PARAM:
    case IR_RETURN:
        /* code */
        printf(" OP %s\n",current->operands.oneOp.op->element.name);
        break;
    case IR_ASSIGN:
    case IR_CALL:
    case IR_GET_ADDR://x = &y
    case IR_READ_ADDR://x = *y
    case IR_WRITE_ADDR://*x = y
        if(current->operands.twoOp.src->kind==OP_CONSTANT)
            printf(" OP1 %s OP2 %d \n",current->operands.twoOp.des->element.name,current->operands.twoOp.src->element.val);
        else 
            printf(" OP1 %s OP2 %s \n",current->operands.twoOp.des->element.name,current->operands.twoOp.src->element.name);
        break;
    case IR_ADD:
    case IR_SUB:
    case IR_MUL:
    case IR_DIV:
        printf(" OP1 %s",current->operands.threeOp.des->element.name);
        if(current->operands.threeOp.src1->kind==OP_CONSTANT){
            printf(" OP2 %d",current->operands.threeOp.src1->element.val);
        }
        else{
            printf(" OP2 %s",current->operands.threeOp.src1->element.name);
        }
        if(current->operands.threeOp.src2->kind==OP_CONSTANT){
            printf(" OP3 %d\n",current->operands.threeOp.src2->element.val);
        }
        else{
            printf(" OP3 %s\n",current->operands.threeOp.src2->element.name);
        }
        break;
    case IR_DEC:
        printf(" OP1 %s OP2 %d\n",current->operands.dec.op->element.name,current->operands.dec.size);
        break;
    case IR_IF_GOTO:
        if(current->operands.ifGoto.x->kind==OP_CONSTANT){
            printf(" OP1 %d",current->operands.ifGoto.x->element.val);
        }
        else{
            printf(" OP1 %s",current->operands.ifGoto.x->element.name);
        }
        printf(" OP2 %s",current->operands.ifGoto.relop->element.name);
        if(current->operands.ifGoto.y->kind==OP_CONSTANT){
            printf(" OP3 %d",current->operands.ifGoto.y->element.val);
        }
        else{
            printf(" OP3 %s",current->operands.ifGoto.y->element.name);
        }
        printf(" OP4 %s\n",current->operands.ifGoto.z->element.name);
        break;
    default:
        assert(0);
        break;
    }
}

void clear_buffer(){
    buffer[0] = '\0';
}
void append_ir_instruction(const char* format, ...) {
    va_list args;
    va_start(args, format);

    size_t currentLength = strlen(buffer);

    
    vsnprintf(buffer+currentLength, IR_BUFFER_SIZE, format, args);

    

    va_end(args);
}
void generate_ir_instruction(IRKind irKind,int line){
    IRInstruction* new_instruction = (IRInstruction*)malloc(sizeof(IRInstruction));
    if (!new_instruction) {
        perror("Failed to allocate memory for new IR instruction");
        return;
    }

    new_instruction->instruction = mystrdup(buffer);
    new_instruction->kind = irKind;
    new_instruction->debug = line+offset;
    clear_buffer();
    if (!new_instruction->instruction) {
        perror("Failed to duplicate IR instruction string");
        free(new_instruction);
        return;
    }

    new_instruction->next = NULL;

    if (ir_tail == NULL) {
        ir_head = ir_tail = new_instruction;
    } else {
        ir_tail->next = new_instruction;
        ir_tail = new_instruction;
    }
}
void fill_in_op(int kind,...){
    if(ir_tail==NULL)return;
    va_list args;
    va_start(args,kind);
    
    switch (kind)
    {
    case IR_LABEL:
    case IR_FUNCTION:
    case IR_GOTO:
    case IR_READ:
    case IR_WRITE:
    case IR_ARG:
    case IR_PARAM:
    case IR_RETURN:
        /* code */
        ir_tail->operands.oneOp.op = va_arg(args,OperandPointer);
        break;
    case IR_ASSIGN:
    case IR_CALL:
    case IR_GET_ADDR://x = &y
    case IR_READ_ADDR://x = *y
    case IR_WRITE_ADDR://*x = 
        {
        ir_tail->operands.twoOp.des = va_arg(args,OperandPointer);
        ir_tail->operands.twoOp.src = va_arg(args,OperandPointer);
        break;
        }
    case IR_ADD:
    case IR_SUB:
    case IR_MUL:
    case IR_DIV:
        ir_tail->operands.threeOp.des = va_arg(args,OperandPointer);
        ir_tail->operands.threeOp.src1 = va_arg(args,OperandPointer);
        ir_tail->operands.threeOp.src2 = va_arg(args,OperandPointer);
        break;
    case IR_DEC:
        ir_tail->operands.dec.op = va_arg(args,OperandPointer);
        ir_tail->operands.dec.size = va_arg(args,int);
        break;
    case IR_IF_GOTO:
        ir_tail->operands.ifGoto.x = va_arg(args,OperandPointer);
        ir_tail->operands.ifGoto.relop = va_arg(args,OperandPointer);
        ir_tail->operands.ifGoto.y = va_arg(args,OperandPointer);
        ir_tail->operands.ifGoto.z = va_arg(args,OperandPointer);
        break;
    default:
        assert(0);
        break;
    }
    
    va_end(args); 
}
void write_ir_to_file(FILE* file) {
    IRInstruction* current = ir_head;
    while (current != NULL) {
        fprintf(file, "%s", current->instruction);
        current = current->next;
    }
}
void free_ir_instructions() {
    IRInstruction* current = ir_head;
    while (current != NULL) {
        IRInstruction* next = current->next;
        free(current->instruction);
        free(current);
        current = next;
    }
    ir_head = ir_tail = NULL;
}
void exit_my_code(){//以写入方式重新打开以清空文件
    fclose(out_ir);
    out_ir=fopen(file_name,"w");
    exit(0);
}
int check_tmp(char* tmp){//检查该变量是否为临时变量，为临时变量则返回1，否则返回0
    int len=strlen(tmp);
    if(len>1&&tmp[0]=='t'){
        for(int i=1;i<len;i++){
            if(tmp[i]<'0'||tmp[i]>'9'){
                return 0;
            }
        }
        //确认为tx的形式
        int loc=hash_find_name(tmp);
        if(hash_table[loc]!=NULL){//虽为tx形式，但是不是临时变量
            return 0;
        }else{
            return 1;
        }
    }
    return 0;
}
char* new_tmp() {
	int tmp = tmp_cnt;
	int num_malloc = 0;
	while (tmp > 0) {
		num_malloc++;
		tmp /= 10;
	}
	char* res = (char*)malloc(32);
    res[0]='t';
	tmp = tmp_cnt;
	for (int i = 0; i < num_malloc; i++) {
		int now = tmp % 10;
		res[num_malloc - i] = '0' + now;//考虑LSB和HSB
		tmp /= 10;
	}
	res[num_malloc+1] = '\0';
    tmp_cnt++;
    int loc=hash_find_name(res);
    if(hash_table[loc]!=NULL){//存在tx，不应该把该临时变量分配出去
        free(res);
        res=new_tmp();
    }
	return res;
}
char *new_label(){
    int tmp = label_cnt;
	int num_malloc = 0;
	while (tmp > 0) {
		num_malloc++;
		tmp /= 10;
	}
	char* res = (char*)malloc(32);
    res[0]='l';
    res[1]='a';
    res[2]='b';
    res[3]='e';
    res[4]='l';
	tmp = label_cnt;
	for (int i = 0; i < num_malloc; i++) {
		int now = tmp % 10;
		res[num_malloc - i +4] = '0' + now;//考虑LSB和HSB
		tmp /= 10;
	}
	res[num_malloc+5] = '\0';
	label_cnt++;
	return res;
}
void preprocess_exp_before_cond(TreeNode* exp,char* place){//在exp中遇到cond是转到cond之前的预处理部分
    //label似乎不需要释放，后续lab可能需要将它作为综合属性放入某些节点中
    char* label1=new_label();
    char* label2=new_label();
    if(place!=NULL){
        append_ir_instruction("%s := #0\n",place);
        generate_ir_instruction(IR_ASSIGN,280);
        fill_in_op(IR_ASSIGN,newOperand(OP_VARIABLE,place),newOperand(OP_CONSTANT,0));

        //fprintf(out_ir,"%s := #0\n",place);//origin
    }   
    translate_Cond(exp,label1,label2);
    append_ir_instruction("LABEL %s :\n",label1);
    generate_ir_instruction(IR_LABEL,287);
    fill_in_op(IR_LABEL,newOperand(OP_LABEL,label1));
    //fprintf(out_ir,"LABEL %s :\n",label1);//origin
    if(place!=NULL){
        append_ir_instruction("%s := #1\n",place);
        generate_ir_instruction(IR_ASSIGN,292);
        fill_in_op(IR_ASSIGN,newOperand(OP_VARIABLE,place),newOperand(OP_CONSTANT,1));
        //fprintf(out_ir,"%s := #1\n",place);//origin
    }
    append_ir_instruction("LABEL %s :\n",label2);
    generate_ir_instruction(IR_LABEL,297);
    fill_in_op(IR_LABEL,newOperand(OP_LABEL,label2));     
    //fprintf(out_ir,"LABEL %s :\n",label2);//origin
}
void operand_op_operand(TreeNode *exp,char* place,int operator){
    char* t1=new_tmp();
    char* t2=new_tmp();
    translate_Exp(exp->firstChild,t1);
    translate_Exp(exp->firstChild->brother->brother,t2);
    if(place!=NULL){
        //fprintf(out_ir,"%s := %s ",place,t1);//+ %s\n // origin
        append_ir_instruction("%s := %s ",place,t1);
        IRKind irKindTemp;
        switch(operator){
            case 0:{
                //fprintf(out_ir,"+");//origin
                append_ir_instruction("+");
                irKindTemp = IR_ADD;
                break;
            }
            case 1:{
                //fprintf(out_ir,"-");//origin
                append_ir_instruction("-");
                irKindTemp = IR_SUB;
                break;
            }
            case 2:{
                //fprintf(out_ir,"*");//origin
                append_ir_instruction("*");
                irKindTemp = IR_MUL;
                break;
            }
            default:{
                //fprintf(out_ir,"/");//origin
                irKindTemp = IR_DIV;
                append_ir_instruction("/");
            }
        }
        //fprintf(out_ir," %s\n",t2);//origin
        append_ir_instruction(" %s\n",t2);
        
        generate_ir_instruction(irKindTemp,338);
        fill_in_op(irKindTemp,newOperand(OP_VARIABLE,place)
            ,newOperand(OP_VARIABLE,t1),newOperand(OP_VARIABLE,t2));
    }
    free(t1);
    free(t2);
}
int translate_array(TreeNode* exp,char* place){//exp：exp lb exp rb; place为数组元素的地址,返回1代表此次是访问元素，否则代表仍为数组地址
    //place一定不为NULL
    int ele=hash_find_name(get_VarDecName(exp));//虽然调用的是get var dec name但是底层逻辑一样
    if(hash_table[ele]->kind==STRUCTURE){//结构体
        exit_my_code();
    }
    Type it=hash_table[ele]->u.array.elem;//指向数组链表的第一个元素
    int length=0;//数组维数
    while(it->kind==ARRAY){
        length++;
        it=it->u.array.elem;
    }
    it=hash_table[ele]->u.array.elem;
    int *size=(int*)malloc(sizeof(int)*length);
    for(int i=0;it->kind==ARRAY;i++){
        size[length-1-i]=it->u.array.size;
        it=it->u.array.elem;
    }

    //数组从低下标到高下标为低维度的大小到高纬度的大小
    TreeNode *it_node=exp;
    int real_dim=0;//实际的维度
    while(child_num(it_node)==4){
        real_dim++;
        it_node=it_node->firstChild;
    }
    int i=0;
    int now_size=1;
    for(;i<length-real_dim;i++){
        now_size*=size[i];
    }
    char *last=NULL;
    it_node=exp;   
    while(child_num(it_node)==4){
        char* t=new_tmp();
        translate_Exp(it_node->firstChild->brother->brother,t);
        if(last!=NULL){
            char* tmp=new_tmp();
            append_ir_instruction("%s := %s * #%d\n",tmp,t,now_size);
            generate_ir_instruction(IR_MUL,384);
            fill_in_op(IR_MUL,newOperand(OP_VARIABLE,tmp),
                newOperand(OP_VARIABLE,t),newOperand(OP_CONSTANT,now_size));
            //fprintf(out_ir,"%s := %s * #%d\n",tmp,t,now_size);//origin
            char* tmp2=new_tmp();
            append_ir_instruction("%s := %s + %s\n",tmp2,tmp,last);
            generate_ir_instruction(IR_ADD,390);
            fill_in_op(IR_ADD,newOperand(OP_VARIABLE,tmp2),
                newOperand(OP_VARIABLE,tmp),newOperand(OP_VARIABLE,last));
            //fprintf(out_ir,"%s := %s + %s\n",tmp2,tmp,last);//origin
            free(last);
            last=tmp2;
            free(t);
            free(tmp);
        }else{
            if(now_size!=1){
                char* tmp=new_tmp();
                append_ir_instruction("%s := %s * #%d\n",tmp,t,now_size);
                generate_ir_instruction(IR_MUL,402);
                fill_in_op(IR_MUL,newOperand(OP_VARIABLE,tmp),
                    newOperand(OP_VARIABLE,t),newOperand(OP_CONSTANT,now_size));
                //fprintf(out_ir,"%s := %s * #%d\n",tmp,t,now_size);//origin
                last=tmp;
                free(t);
            }else{
                last=t; 
            }                     
        }
        now_size*=size[i];
        it_node=it_node->firstChild;
        i++;
    }
    //it_node->EXP: ID
    char* tmp=new_tmp();
    append_ir_instruction("%s := %s * #4\n",tmp,last);
    generate_ir_instruction(IR_MUL,419);
    fill_in_op(IR_MUL,newOperand(OP_VARIABLE,tmp),newOperand(OP_VARIABLE,last),newOperand(OP_CONSTANT,4));
    //fprintf(out_ir,"%s := %s * #4\n",tmp,last);//只考虑int // origin
    
    free(last);
    
    if(hash_table[ele]->arg){//reach not
        append_ir_instruction("%s := %s + %s\n",place,it_node->firstChild->val.string,tmp);
        generate_ir_instruction(IR_ADD,427);
        fill_in_op(IR_ADD,newOperand(OP_VARIABLE,place),
            newOperand(OP_VARIABLE,it_node->firstChild->val.string),newOperand(OP_VARIABLE,tmp));
        //fprintf(out_ir,"%s := %s + %s\n",place,it_node->firstChild->val.string,tmp);//origin

    }else{//变量而非参数
        append_ir_instruction("%s := &%s + %s\n",place,it_node->firstChild->val.string,tmp);
        generate_ir_instruction(IR_ADD,434);
        fill_in_op(IR_ADD,newOperand(OP_VARIABLE,place),newOperand(OP_QUOTE,it_node->firstChild->val.string),
            newOperand(OP_VARIABLE,tmp));
        //fprintf(out_ir,"%s := &%s + %s\n",place,it_node->firstChild->val.string,tmp);//origin

    }  
    free(tmp);
    free(size);
    return real_dim==length;
}
int translate_Exp(TreeNode* exp,char *place){//记得回收place,返回值为1代表为普通变量，为0代表为数组
    int num=child_num(exp);
    if(num==1){//只有一个子女
        if(strcmp(exp->firstChild->tokenName,"INT")==0){
            int value=exp->firstChild->val.val_int;
            if(place!=NULL){
                append_ir_instruction("%s := #%d\n",place,value);
                generate_ir_instruction(IR_ASSIGN,451);
                fill_in_op(IR_ASSIGN,newOperand(OP_VARIABLE,place),newOperand(OP_CONSTANT,value));
                //fprintf(out_ir,"%s := #%d\n",place,value);//origin
            }        
        }else if(strcmp(exp->firstChild->tokenName,"ID")==0){//ID
            if(place!=NULL){
                if(check_tmp(place)){//为临时变量:=ID的形式,无需赋值，直接将place改为对应ID
                    strcpy(place,exp->firstChild->val.string);
                }else{
                    append_ir_instruction("%s := %s\n",place,exp->firstChild->val.string);
                    generate_ir_instruction(IR_ASSIGN,461);
                    fill_in_op(IR_ASSIGN,newOperand(OP_VARIABLE,place),newOperand(OP_VARIABLE,exp->firstChild->val.string));
                    //fprintf(out_ir,"%s := %s\n",place,exp->firstChild->val.string);//origin
                }            
            }
            int ele=hash_find_name(exp->firstChild->val.string);
            if(hash_table[ele]->kind==ARRAY&&!hash_table[ele]->arg){
                return 0;
            }
        }else{
            printf("float occurs!\n");
            fflush(stdout);
            exit_my_code();
        }
    }else if(num==2){
        if(strcmp(exp->firstChild->tokenName,"MINUS")==0){
            if(place!=NULL){
                if(child_num(exp->firstChild->brother)==1&&strcmp(exp->firstChild->brother->firstChild->tokenName,"INT")==0&&exp->firstChild->brother->firstChild->val.val_int!=0){
                    append_ir_instruction("%s := #-%d\n",place,exp->firstChild->brother->firstChild->val.val_int);
                    generate_ir_instruction(IR_ASSIGN,480);
                    fill_in_op(IR_ASSIGN,newOperand(OP_VARIABLE,place),newOperand(OP_CONSTANT,-(exp->firstChild->brother->firstChild->val.val_int)));
                    //fprintf(out_ir,"%s := #-%d\n",place,exp->firstChild->brother->firstChild->val.val_int);//origin
                }else{
                    char* next_place=new_tmp();
                    translate_Exp(exp->firstChild->brother,next_place);
                    append_ir_instruction("%s := #0 - %s\n",place,next_place);
                    generate_ir_instruction(IR_SUB,487);
                    fill_in_op(IR_SUB,newOperand(OP_VARIABLE,place),newOperand(OP_CONSTANT,0),newOperand(OP_VARIABLE,next_place));
                    //fprintf(out_ir,"%s := #0 - %s\n",place,next_place);//origin
                    free(next_place);
                }
            }else{
                char* next_place=new_tmp();
                translate_Exp(exp->firstChild->brother,next_place);
                free(next_place);
            }
            /*char* next_place=new_tmp();
            translate_Exp(exp->firstChild->brother,next_place);
            if(place!=NULL){
                if(child_num(exp->firstChild->brother)==1&&strcmp(exp->firstChild->brother->firstChild->tokenName,"INT")==0&&exp->firstChild->brother->firstChild->val.val_int!=0){
                    fprintf(out_ir,"%s := #-%d\n",place,exp->firstChild->brother->firstChild->val.val_int);
                }else{
                    fprintf(out_ir,"%s := #0 - %s\n",place,next_place);
                }        
            }
            free(next_place);*/    
        }else{//NOT exp
            preprocess_exp_before_cond(exp,place);
        }
    }else if(num==3){
        TreeNode* mid_node=exp->firstChild->brother;
        if(strcmp(mid_node->tokenName,"ASSIGNOP")==0){
            //int变量与数组变量(ID或EXP LB EXP RB)
            if(child_num(exp->firstChild)==1){//ID
                //char* t1=new_tmp();
                translate_Exp(exp->firstChild->brother->brother,exp->firstChild->firstChild->val.string);
                //fprintf(out_ir,"%s := %s\n",exp->firstChild->firstChild->val.string,t1);
                //free(t1);
                if(place!=NULL){
                    append_ir_instruction("%s := %s\n",place,exp->firstChild->firstChild->val.string);
                    generate_ir_instruction(IR_ASSIGN,521);
                    fill_in_op(IR_ASSIGN,newOperand(OP_VARIABLE,place),newOperand(OP_VARIABLE,exp->firstChild->firstChild->val.string));
                    //fprintf(out_ir,"%s := %s\n",place,exp->firstChild->firstChild->val.string);//origin
                }
            }else{
                char* array_addr=new_tmp();
                translate_array(exp->firstChild,array_addr);
                char* exp2=new_tmp();
                translate_Exp(exp->firstChild->brother->brother,exp2);
                append_ir_instruction("*%s := %s\n",array_addr,exp2);
                generate_ir_instruction(IR_WRITE_ADDR,531);
                fill_in_op(IR_WRITE_ADDR,newOperand(OP_ADDRESS,array_addr),newOperand(OP_VARIABLE,exp2));
                //fprintf(out_ir,"*%s := %s\n",array_addr,exp2);//origin//因为没有数组的赋值，所以此处无需检测translate_array的返回值
                if(place!=NULL){
                    append_ir_instruction("%s := %s\n",place,exp2);
                    generate_ir_instruction(IR_ASSIGN,536);
                    fill_in_op(IR_ASSIGN,newOperand(OP_VARIABLE,place),newOperand(OP_VARIABLE,exp2));
                   //fprintf(out_ir,"%s := %s\n",place,exp2);//origin
                }
                free(array_addr);
                free(exp2);
            }
        }else if(strcmp(mid_node->tokenName,"AND")==0||strcmp(mid_node->tokenName,"OR")==0||strcmp(mid_node->tokenName,"RELOP")==0){
            preprocess_exp_before_cond(exp,place);
        }else if(strcmp(mid_node->tokenName,"PLUS")==0){
            operand_op_operand(exp,place,0);
        }else if(strcmp(mid_node->tokenName,"MINUS")==0){
            operand_op_operand(exp,place,1);
        }else if(strcmp(mid_node->tokenName,"STAR")==0){
            operand_op_operand(exp,place,2);
        }else if(strcmp(mid_node->tokenName,"DIV")==0){
            operand_op_operand(exp,place,3);
        }else if(strcmp(mid_node->tokenName,"LP")==0){//无参数调用函数
            if(strcmp("read",exp->firstChild->val.string)==0){//读函数
                if(place!=NULL){
                    //printf("implement READ function but no variable to store the read data!\n");
                    //fflush(stdout);
                    //assert(0);
                    append_ir_instruction("READ %s\n",place);
                    generate_ir_instruction(IR_READ,560);
                    fill_in_op(IR_READ,newOperand(OP_VARIABLE,place));
                    //fprintf(out_ir,"READ %s\n",place);//origin
                }//需要考虑place==NULL吗？
            }else{
                if(place!=NULL){//应该都有返回语句
                    append_ir_instruction("%s := CALL %s\n",place,exp->firstChild->val.string);
                    generate_ir_instruction(IR_CALL,567);
                    fill_in_op(IR_CALL,newOperand(OP_VARIABLE,place),newOperand(OP_FUNCTION,exp->firstChild->val.string));
                    //fprintf(out_ir,"%s := CALL %s\n",place,exp->firstChild->val.string);//origin
                }else{
                    char *tmp=new_tmp();
                    append_ir_instruction("%s := CALL %s\n",tmp,exp->firstChild->val.string);
                    generate_ir_instruction(IR_CALL,573);
                    fill_in_op(IR_CALL,newOperand(OP_VARIABLE,tmp),newOperand(OP_FUNCTION,exp->firstChild->val.string));
                    //fprintf(out_ir,"%s := CALL %s\n",tmp,exp->firstChild->val.string);//origin
                    free(tmp);//启用一个临时变量存放返回值
                }
            }
        }else if(strcmp(mid_node->tokenName,"Exp")==0){//表达式加括号
            char* t=new_tmp();
            translate_Exp(exp->firstChild->brother,t);
            if(place!=NULL){
                append_ir_instruction("%s := %s\n",place,t);
                generate_ir_instruction(IR_ASSIGN,584);
                fill_in_op(IR_ASSIGN,newOperand(OP_VARIABLE,place),newOperand(OP_VARIABLE,t));
                //fprintf(out_ir,"%s := %s\n",place,t);//origin
            }
            free(t);
        }else{//结构体
            printf("struct variable occurs!\n");
            fflush(stdout);
            exit_my_code();
        }
    }else{//num==4
        if(strcmp(exp->firstChild->tokenName,"ID")==0){//调用参数
            arg_list** para=(arg_list**)malloc(sizeof(arg_list*));
            *para=NULL;
            translate_Args(exp->firstChild->brother->brother,para);
            arg_list* arg=*para;
            if(strcmp("write",exp->firstChild->val.string)==0){//write函数
                append_ir_instruction("WRITE %s\n",arg->arg);
                generate_ir_instruction(IR_WRITE,602);
                fill_in_op(IR_WRITE,newOperand(OP_VARIABLE,arg->arg));
                //fprintf(out_ir,"WRITE %s\n",arg->arg);//origin
                if(place!=NULL){
                    append_ir_instruction("%s := #0\n",place);
                    generate_ir_instruction(IR_ASSIGN,607);
                    fill_in_op(IR_ASSIGN,newOperand(OP_VARIABLE,place),newOperand(OP_CONSTANT,0));
                    //fprintf(out_ir,"%s := #0\n",place);/origin
                }
            }else{
                arg_list* it=arg;
                while(it!=NULL){
                    if(it->type){
                        append_ir_instruction("ARG %s\n",it->arg);
                        generate_ir_instruction(IR_ARG,616);
                        fill_in_op(IR_ARG,newOperand(OP_VARIABLE,it->arg));
                        //fprintf(out_ir,"ARG %s\n",it->arg);//origin
                    }else{//数组传参
                        append_ir_instruction("ARG &%s\n",it->arg);
                        generate_ir_instruction(IR_ARG,621);
                        fill_in_op(IR_ARG,newOperand(OP_QUOTE,it->arg));
                        //fprintf(out_ir,"ARG &%s\n",it->arg);//origin
                    }                
                    it=it->next;
                }
                if(place!=NULL){
                    append_ir_instruction("%s := CALL %s\n",place,exp->firstChild->val.string);
                    generate_ir_instruction(IR_CALL,629);
                    fill_in_op(IR_CALL,newOperand(OP_VARIABLE,place),newOperand(OP_FUNCTION,exp->firstChild->val.string));
                    //fprintf(out_ir,"%s := CALL %s\n",place,exp->firstChild->val.string);//origin
                }else{
                    char* tmp=new_tmp();
                    append_ir_instruction("%s := CALL %s\n",tmp,exp->firstChild->val.string);
                    generate_ir_instruction(IR_CALL,635);
                    fill_in_op(IR_CALL,newOperand(OP_VARIABLE,tmp),newOperand(OP_FUNCTION,exp->firstChild->val.string));
                    //fprintf(out_ir,"%s := CALL %s\n",tmp,exp->firstChild->val.string);//origin
                    free(tmp);
                }
            }
            while(arg!=NULL){
                arg_list* tmp=arg;
                arg=arg->next;
                free(tmp->arg);
                free(tmp);
            }
            free(para);
            //释放链表
        }else{//数组
            char* array_addr=new_tmp();
            int state=translate_array(exp,array_addr);
            if(place!=NULL){
                if(state){//访问元素
                    append_ir_instruction("%s := *%s\n",place,array_addr);
                    generate_ir_instruction(IR_READ_ADDR,655);
                    fill_in_op(IR_READ_ADDR,newOperand(OP_VARIABLE,place),newOperand(OP_ADDRESS,array_addr));
                    //fprintf(out_ir,"%s := *%s\n",place,array_addr);//origin
                }else{
                    append_ir_instruction("%s := %s\n",place,array_addr);
                    generate_ir_instruction(IR_ASSIGN,660);
                    fill_in_op(IR_ASSIGN,newOperand(OP_VARIABLE,place),newOperand(OP_VARIABLE,array_addr));
                    //fprintf(out_ir,"%s := %s\n",place,array_addr);
                }     
            }
            free(array_addr);
        }
    }
    return 1;
}
void else_translate_Cond(TreeNode* exp,char* label1 ,char* label2){
    char* t=new_tmp();
    translate_Exp(exp,t);
    append_ir_instruction("IF %s != #0 GOTO %s\n",t,label1);
    generate_ir_instruction(IR_IF_GOTO,674);
    fill_in_op(IR_IF_GOTO,newOperand(OP_VARIABLE,t),newOperand(OP_RELOP,"!="),newOperand(OP_CONSTANT,0),newOperand(OP_LABEL,label1));
    //fprintf(out_ir,"IF %s != #0 GOTO %s\n",t,label1);//origin
    append_ir_instruction("GOTO %s\n",label2);
    generate_ir_instruction(IR_GOTO,678);
    fill_in_op(IR_GOTO,newOperand(OP_LABEL,label2));
    //fprintf(out_ir,"GOTO %s\n",label2);//origin
    free(t);
}
void translate_Cond(TreeNode* exp,char *label1,char* label2){
    int num=child_num(exp);
    if(num==2){
        if(strcmp(exp->firstChild->tokenName,"NOT")==0){
            translate_Cond(exp->firstChild->brother,label2,label1);
        }else{
            else_translate_Cond(exp,label1,label2);
        }
    }else if(num==3){
        TreeNode* mid=exp->firstChild->brother;
        if(strcmp(mid->tokenName,"RELOP")==0){
            char* t1=new_tmp(),*t2=new_tmp();
            translate_Exp(exp->firstChild,t1);
            translate_Exp(exp->firstChild->brother->brother,t2);
            append_ir_instruction("IF %s %s %s GOTO %s\n",t1,exp->firstChild->brother->val.string,t2,label1);
            generate_ir_instruction(IR_IF_GOTO,698);
            fill_in_op(IR_IF_GOTO,newOperand(OP_VARIABLE,t1),newOperand(OP_RELOP,exp->firstChild->brother->val.string),
                newOperand(OP_VARIABLE,t2),newOperand(OP_LABEL,label1));
            //fprintf(out_ir,"IF %s %s %s GOTO %s\n",t1,exp->firstChild->brother->val.string,t2,label1);//origin
            append_ir_instruction("GOTO %s\n",label2);
            generate_ir_instruction(IR_GOTO,703);
            fill_in_op(IR_GOTO,newOperand(OP_LABEL,label2));
            //fprintf(out_ir,"GOTO %s\n",label2);//origin
            free(t1);
            free(t2);
        }else if(strcmp(mid->tokenName,"AND")==0){
            char* label=new_label();
            translate_Cond(exp->firstChild,label,label2);
            append_ir_instruction("LABEL %s :\n",label);
            generate_ir_instruction(IR_LABEL,712);
            fill_in_op(IR_LABEL,newOperand(OP_LABEL,label));
            //fprintf(out_ir,"LABEL %s :\n",label);//origin
            translate_Cond(exp->firstChild->brother->brother,label1,label2);
        }else if(strcmp(mid->tokenName,"OR")==0){
            char* label=new_label();
            translate_Cond(exp->firstChild,label1,label);
            append_ir_instruction("LABEL %s :\n",label);
            generate_ir_instruction(IR_LABEL,720);
            fill_in_op(IR_LABEL,newOperand(OP_LABEL,label));
            //fprintf(out_ir,"LABEL %s :\n",label);//origin
            translate_Cond(exp->firstChild->brother->brother,label1,label2);
        }else{
            else_translate_Cond(exp,label1,label2);
        }
    }else{
        else_translate_Cond(exp,label1,label2);
    }
}
void translate_Stmt(TreeNode* stmt){
    int num=child_num(stmt);
    if(num==1){//CompSt
        dfs_lab3(stmt->firstChild);        
    }else if(num==2){//Exp SEMI
        translate_Exp(stmt->firstChild,NULL);
    }else if(num==3){//RETURN Exp SEMI
        char* t=new_tmp();
        translate_Exp(stmt->firstChild->brother,t);
        append_ir_instruction("RETURN %s\n",t);
        generate_ir_instruction(IR_RETURN,741);
        fill_in_op(IR_RETURN,newOperand(OP_VARIABLE,t));
        //fprintf(out_ir,"RETURN %s\n",t);//origin
        free(t);
    }else if(num==7){//IF LP Exp RP Stmt1 ELSE Stmt2
        char* label1=new_label(),*label2=new_label(),*label3=new_label();
        translate_Cond(stmt->firstChild->brother->brother,label1,label2);
        append_ir_instruction("LABEL %s :\n",label1);
        generate_ir_instruction(IR_LABEL,749);
        fill_in_op(IR_LABEL,newOperand(OP_LABEL,label1));
        //fprintf(out_ir,"LABEL %s :\n",label1);//origin
        translate_Stmt(stmt->firstChild->brother->brother->brother->brother);

        append_ir_instruction("GOTO %s\n",label3);
        generate_ir_instruction(IR_GOTO,755);
        fill_in_op(IR_GOTO,newOperand(OP_LABEL,label3));
        //fprintf(out_ir,"GOTO %s\n",label3);//origin

        append_ir_instruction("LABEL %s :\n",label2);
        generate_ir_instruction(IR_LABEL,760);
        fill_in_op(IR_LABEL,newOperand(OP_LABEL,label2));
        //fprintf(out_ir,"LABEL %s :\n",label2);//origin
        translate_Stmt(stmt->firstChild->brother->brother->brother->brother->brother->brother);

        append_ir_instruction("LABEL %s :\n",label3);
        generate_ir_instruction(IR_LABEL,766);
        fill_in_op(IR_LABEL,newOperand(OP_LABEL,label3));
        //fprintf(out_ir,"LABEL %s :\n",label3);//origin
    }else{
        if(strcmp(stmt->firstChild->tokenName,"IF")==0){//IF LP Exp RP Stmt1
            char* label1=new_label(),*label2=new_label();//不应该释放
            translate_Cond(stmt->firstChild->brother->brother,label1,label2);

            append_ir_instruction("LABEL %s :\n",label1);
            generate_ir_instruction(IR_LABEL,775);
            fill_in_op(IR_LABEL,newOperand(OP_LABEL,label1));
            //fprintf(out_ir,"LABEL %s :\n",label1);//origin
            translate_Stmt(stmt->firstChild->brother->brother->brother->brother);

            append_ir_instruction("LABEL %s :\n",label2);
            generate_ir_instruction(IR_LABEL,781);
            fill_in_op(IR_LABEL,newOperand(OP_LABEL,label2));
            //fprintf(out_ir,"LABEL %s :\n",label2);//origin
        }else{//WHILE LP Exp RP Stmt1
            char* label1=new_label(),*label2=new_label(),*label3=new_label();
            append_ir_instruction("LABEL %s :\n",label1);
            generate_ir_instruction(IR_LABEL,787);
            fill_in_op(IR_LABEL,newOperand(OP_LABEL,label1));
            //fprintf(out_ir,"LABEL %s :\n",label1);//origin
            translate_Cond(stmt->firstChild->brother->brother,label2,label3);
            append_ir_instruction("LABEL %s :\n",label2);
            generate_ir_instruction(IR_LABEL,792);
            fill_in_op(IR_LABEL,newOperand(OP_LABEL,label2));
            //fprintf(out_ir,"LABEL %s :\n",label2);//origin
            translate_Stmt(stmt->firstChild->brother->brother->brother->brother);
            append_ir_instruction("GOTO %s\n",label1);
            generate_ir_instruction(IR_GOTO,797);
            fill_in_op(IR_GOTO,newOperand(OP_LABEL,label1));
            //fprintf(out_ir,"GOTO %s\n",label1);//origin

            append_ir_instruction("LABEL %s :\n",label3);
            generate_ir_instruction(IR_LABEL,802);
            fill_in_op(IR_LABEL,newOperand(OP_LABEL,label3));
            //fprintf(out_ir,"LABEL %s :\n",label3);//origin
        }
    }
}
void translate_Args(TreeNode* args,arg_list** arg){//arg指向 存放链表的首指针
    char* t=new_tmp();
    arg_list* new_ele=(arg_list*)malloc(sizeof(struct _arg));
    new_ele->arg=t;
    new_ele->next=*arg;
    new_ele->type= translate_Exp(args->firstChild,t);
    *arg=new_ele;
    if(child_num(args)!=1){
        translate_Args(args->firstChild->brother->brother,arg);
    }
}
void translate_FunDec(TreeNode* FunDec){
    int ele=hash_find_name(FunDec->firstChild->val.string);
    append_ir_instruction("FUNCTION %s :\n",FunDec->firstChild->val.string);
    generate_ir_instruction(IR_FUNCTION,822);
    fill_in_op(IR_FUNCTION,newOperand(OP_FUNCTION,FunDec->firstChild->val.string));
    //fprintf(out_ir,"FUNCTION %s :\n",FunDec->firstChild->val.string);//origin
    FieldList para=hash_table[ele]->u.func.input;
    while(para!=NULL){
        append_ir_instruction("PARAM %s\n",para->name);
        generate_ir_instruction(IR_PARAM,828);
        fill_in_op(IR_PARAM,newOperand(OP_VARIABLE,para->name));
        //fprintf(out_ir,"PARAM %s\n",para->name);//origin
        para=para->tail;
    }
}
void translate_var_dec(TreeNode* root){//检测VarDec中是否存在数组，若存在需要添加dec语句
    if(root->firstChild->brother!=NULL){//数组
        int ele=hash_find_name(get_VarDecName(root));
        int size=1;
        Type it=hash_table[ele]->u.array.elem;
        while(it->kind==ARRAY){
            size*=it->u.array.size;
            it=it->u.array.elem;
        }
        append_ir_instruction("DEC %s %d\n",get_VarDecName(root),size*4);
        generate_ir_instruction(IR_DEC,844);
        fill_in_op(IR_DEC,newOperand(OP_VARIABLE,get_VarDecName(root)),size*4);
        //fprintf(out_ir,"DEC %s %d\n",get_VarDecName(root),size*4);//origin
    }
}
void dfs_lab3(TreeNode *root){
    if(strcmp(root->tokenName,"ExtDef")==0){
        insert_table(root);
        if(strcmp(root->firstChild->brother->tokenName,"ExtDecList")==0){
            TreeNode* it=root->firstChild->brother->firstChild;//指向第一个VarDec
            while(1){
                translate_var_dec(it);
                if(it->brother==NULL){
                    break;
                }
                it=it->brother->brother->firstChild;//下一个VarDec
            }
        }
        if(child_num(root)==3&&strcmp(root->firstChild->brother->brother->tokenName,"CompSt")==0){
            translate_FunDec(root->firstChild->brother);
            dfs_lab3(root->firstChild->brother->brother);
        }
    }else if(strcmp(root->tokenName,"Def")==0){
        insert_table(root);
        TreeNode* it=root->firstChild->brother->firstChild;//指向第一个Dec
        while(1){
            translate_var_dec(it->firstChild);
            if(it->firstChild->brother!=NULL){//赋值语句
                if(strcmp(it->firstChild->firstChild->tokenName,"ID")){//为数组
                    printf("array assign!\n");
                    fflush(stdout);
                    exit_my_code();
                }
                translate_Exp(it->firstChild->brother->brother,it->firstChild->firstChild->val.string);
            }
            if(it->brother==NULL){
                break;
            }
            it=it->brother->brother->firstChild;//下一个Dec
        }
    }else if(strcmp(root->tokenName,"Stmt")==0){       
        //不需要检测return错误
        if(strcmp(root->firstChild->tokenName,"CompSt")==0){
            dfs_lab3(root->firstChild);
        }else{
            translate_Stmt(root);
        }
    }else{//其他的语句正常递归
        if(root->firstChild!=NULL){
            dfs_lab3(root->firstChild);
        }
    }
    /*else if(strcmp(root->tokenName,"Exp")==0){//EXP在Def和Stmt中已被处理
        int tmp;
        Type res=deal_with_EXP(&tmp,root);
        free_Type(res);
    }*/

    if(root->brother!=NULL){
        dfs_lab3(root->brother);
    }
    return ;
}
