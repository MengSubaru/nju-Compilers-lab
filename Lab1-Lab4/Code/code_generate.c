#include "code_generate.h"

Reg regs[32];
Stack stack;
// 指定T9 作为 立即数
int arg_count = 0;

int para_count = 0;
char codeBuffer[FRAME_CODE_SIZE];
void clearCodeBuffer(){
	codeBuffer[0] = '\0';
}
void writeCodetoBuffer(FILE* fp,const char*format,...){
	va_list args;
	va_start(args,format);
	size_t curr_length = strlen(codeBuffer);
	vsnprintf(codeBuffer+curr_length,FRAME_CODE_SIZE,format,args);
	va_end(args); 
}
void appendCode(Frame *frame,IRInstruction* ir){
	
	CodeList*cp = (CodeList*)malloc(sizeof(CodeList));
	cp->code = mystrdup(codeBuffer);
	cp->ir = ir;
	clearCodeBuffer();
	
	if(frame->head==NULL){
	
		frame->head = cp;
		frame->tail = cp;
	}
	else{
		frame->tail->next = cp;
		frame->tail = cp;
	}
}
void init_regs(){
    //初始化;
	//寄存器的名字和状态初始化;
	for(int i=0;i<32;++i){

		regs[i].ifUse = 0;
	}
	//别名初始化;
	regs[0].name="$zero";
	regs[1].name="$at";
	regs[2].name="$v0";
	regs[3].name="$v1";

	regs[4].name="$a0";
	regs[5].name="$a1";
	regs[6].name="$a2";
	regs[7].name="$a3";

	regs[8].name="$t0";
	regs[9].name="$t1";
	regs[10].name="$t2";
	regs[11].name="$t3";

	regs[12].name="$t4";
	regs[13].name="$t5";
	regs[14].name="$t6";
	regs[15].name="$t7";

	regs[16].name="$s0";
	regs[17].name="$s1";
	regs[18].name="$s2";
	regs[19].name="$s3";

	regs[20].name="$s4";
	regs[21].name="$s5";
	regs[22].name="$s6";
	regs[23].name="$s7";

	regs[24].name="$t8";
	regs[25].name="$t9";

	regs[26].name="$k0";
	regs[27].name="$k1";

	regs[28].name="$gp";
	regs[29].name="$sp";

	regs[30].name="$fp";
	regs[31].name="$ra";
}
void showRegState(){
	for(int i = 0;i<32;i++){
		printf("reg %s ifuse %d ",regs[i].name,regs[i].ifUse);
	}
	printf("\n");
}
// T8 use 
void init_data(FILE *fp){
	//数据段初始化;
	fprintf(fp, ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n");
}
void freeReg(int no){
	regs[no].ifUse = 0;
	regs[no].varDes = NULL;
}
VarDesList* searchVar(OperandPointer op,Frame *frame){
	
	VarDesList* curr = frame->varDesList;
	
	while(curr!=NULL){
		OperandPointer temp = curr->op;
		if(op->kind==OP_CONSTANT&&temp->kind==OP_CONSTANT&&op->element.val == temp->element.val){
			return curr;
		}
		else if(op->kind!=OP_CONSTANT&&temp->kind!=OP_CONSTANT&&strcmp(op->element.name,temp->element.name)==0){
			return curr;
		}
		// if(op->kind==OP_CONSTANT){
		// 	printf("val %d\n",op->element.val);
		// }
		// else{
		// 	printf("name %s\n",op->element.name);
		// }
		curr = curr->next;

	}
	
	return NULL;
}
void init_write_read(FILE *fp){
	fprintf(fp, ".data\n");
    fprintf(fp, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(fp, "_ret: .asciiz \"\\n\"\n");
    fprintf(fp, ".globl main\n");
	// read function //copy pdf
    fprintf(fp, ".text\n");
    fprintf(fp, "read:\n");
    fprintf(fp, "  li $v0, 4\n");
    fprintf(fp, "  la $a0, _prompt\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  li $v0, 5\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  jr $ra\n\n");

    // write function //copy pdf
    fprintf(fp, "write:\n");
    fprintf(fp, "  li $v0, 1\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  li $v0, 4\n");
    fprintf(fp, "  la $a0, _ret\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  move $v0, $0\n");
    fprintf(fp, "  jr $ra\n\n");
}
void addVartoReg(int no,OperandPointer op,Frame*frame,FILE*fp){
	VarDesList *vp;

	vp = searchVar(op,frame);
	// if(op->kind==OP_CONSTANT){
	// 	printf("val %d vp==NULL?:%d\n",op->element.val,vp==NULL);
	// }
	// else{
	// 	printf("name %s vp==NULL?:%d\n",op->element.name,vp==NULL);
	// }
	if(vp==NULL){
		vp = (VarDesList*)malloc(sizeof(VarDesList));
		vp->next = NULL;
		vp->op = op;
		vp->regNo = no;
		pushVartoFrame(fp,vp,frame,4);

	}
	else{
		vp->regNo = no;
	}
	
	switch (op->kind)
	{
	case OP_QUOTE:
	case OP_VARIABLE:
		writeCodetoBuffer(fp,"  lw %s, %d($fp)\n",regs[no].name,vp->fpoffset);
		break;
	// 貌似不用引用
	// case OP_QUOTE:
	// 	writeCodetoBuffer(fp,"  la %s, %d($fp)\n",regs[no].name,vp->fpoffset);
	// 	break;
	case OP_ADDRESS:
		writeCodetoBuffer(fp,"  lw %s, %d($fp)\n",regs[no].name,vp->fpoffset);
		writeCodetoBuffer(fp,"  lw %s, 0(%s)\n",regs[no].name,regs[no].name);
		break;
	case OP_CONSTANT:
		writeCodetoBuffer(fp,"  li %s, %d\n",regs[no].name,op->element.val);
		break;
	default:
		printf(" %d addvartoreg seems wrong\n",op->kind);
		break;
	}
	regs[no].varDes = vp;
}


int allocate(FILE*fp,OperandPointer var,Frame*frame){
	int res = 0;
	
	for(int i = T0;i<T8;i++){
		if(regs[i].ifUse == 0){
			regs[i].ifUse = 1;
			
			addVartoReg(i,var,frame,fp);
			return i;
		}
	}
	showRegState();
	printf("should not reach here reg can't be useout\n");
	//assert(0);
	return T9;
}

void pushVartoFrame(FILE*fp,VarDesList*vp,Frame*frame,int offset){

	if(vp->op!=NULL&&vp->op->kind!=OP_CONSTANT)
	{	
		//固定栈帧，开始一次性开辟好
		//fprintf(fp,"  addi $sp, $sp, %d\n",-offset);
		frame->size-=offset;
		frame->max_size+=offset;
	}
	
	if(frame->varDesList==NULL){
		frame->varDesList = vp;
		
	}
	else{
		vp->next = frame->varDesList;
        frame->varDesList = vp;
		
	}
	
	vp->fpoffset = frame->size;
}
void storeRegToVar(int no,OperandPointer op,Frame*frame,FILE*fp){

	VarDesList*vp;
	
	vp = searchVar(op,frame);
	if(vp==NULL){
		vp = (VarDesList*)malloc(sizeof(VarDesList));
		vp->op = op;
		pushVartoFrame(fp,vp,frame,4);
		
	}
	
	switch (op->kind)
	{
	case OP_VARIABLE:
		writeCodetoBuffer(fp,"  sw %s, %d($fp)\n",regs[no].name,vp->fpoffset);
		
		break;
	case OP_ADDRESS:
		
		//对于形如*x 的左值，将寄存器中的右值存入需要先取x的值到寄存器中，然后用他的值作为地址寻址并存入
		writeCodetoBuffer(fp,"  lw %s, %d($fp)\n",regs[T8].name,vp->fpoffset);

		writeCodetoBuffer(fp,"  sw %s, 0(%s)\n",regs[no].name,regs[T8].name);
		break;
	default:
		printf("%d storeregtovar wrong\n",op->kind);
		break;
	}
	
	freeReg(no);
	freeReg(T8);
}


void writeRelop(FILE*fp,char*relop,char*x,char*y,char*z){
	assert(relop!=NULL&&x!=NULL&&y!=NULL&&z!=NULL);
	if(strcmp(relop,"==")==0){
		writeCodetoBuffer(fp,"  beq %s, %s, %s\n",x,y,z);
	}
	else if(strcmp(relop,"!=")==0){
		writeCodetoBuffer(fp,"  bne %s, %s, %s\n",x,y,z);
	}
	else if(strcmp(relop,">")==0){
		writeCodetoBuffer(fp,"  bgt %s, %s, %s\n",x,y,z);
	}
	else if(strcmp(relop,"<")==0){
		writeCodetoBuffer(fp,"  blt %s, %s, %s\n",x,y,z);
	}
	else if(strcmp(relop,">=")==0){
		writeCodetoBuffer(fp,"  bge %s, %s, %s\n",x,y,z);
	}
	else if(strcmp(relop,"<=")==0){
		writeCodetoBuffer(fp,"  ble %s, %s, %s\n",x,y,z);
	}
	else{
		printf("relop wrong \n");
		assert(0);
	}
}
void writeCodeToFile(FILE*fp,Frame*frame){
	
	fprintf(fp,"  addi $sp,$sp,%d\n",-frame->max_size);
	CodeList *curr = frame->head;
	while(curr!=NULL){
		
		fprintf(fp,"\n# %s\n",curr->ir->instruction);
		fputs(curr->code,fp);
		//fprintf(fp,curr->code);
		curr=curr->next;
	}
}
void IRtoMips(FILE*fp,IRInstruction*ir,Frame*frame){

	if(ir->kind == IR_LABEL){
		writeCodetoBuffer(fp,"%s:\n",ir->operands.oneOp.op->element.name);
		appendCode(frame,ir);
		return;
	}
	if(ir->kind == IR_FUNCTION){
		//处理上一个结束
		if(frame!=NULL)
			writeCodeToFile(fp,frame);
		//开始时栈帧在这里确定，因而写入buffer的不局限
		fprintf(fp,"%s:\n",ir->operands.oneOp.op->element.name);

		fprintf(fp,"  addi $sp, $sp, -8\n");
		fprintf(fp,"  sw $ra, 4($sp)\n");
		fprintf(fp,"  sw $fp, 0($sp)\n");
		Frame*f = (Frame*)malloc(sizeof(Frame));
		f->size = 0;
		f->max_size = 0;
		f->varDesList =NULL;
		f->next = NULL;
		f->head = NULL;
		f->tail = NULL;
		f->funcName = mystrdup(ir->operands.oneOp.op->element.name);
		if(stack.frameList==NULL){
			stack.frameList = f;
		}
		else{
			f->next = stack.frameList;
			stack.frameList = f;
		}
		
		
		
		fprintf(fp,"  move $fp, $sp\n");
		f->size = 0;
		stack.curr = f;
		//这里需要确定栈帧大小了
		appendCode(stack.curr,ir);
		return;

	}
	if(ir->kind == IR_ASSIGN){
		
		OperandPointer des = ir->operands.twoOp.des;
		OperandPointer src = ir->operands.twoOp.src;
		
		int srcNo = allocate(fp,src,frame);
		
		
		storeRegToVar(srcNo,des,frame,fp);
		
		appendCode(frame,ir);
		freeReg(srcNo);
	
		return;
	}
	if(ir->kind == IR_ADD){
		OperandPointer des = ir->operands.threeOp.des;
		OperandPointer src1 = ir->operands.threeOp.src1;
		OperandPointer src2 = ir->operands.threeOp.src2;
		int desNo = allocate(fp,des,frame);
		int src1No = 0;
		int src2No = 0;
		if(src1->kind==OP_CONSTANT){
			src2No = allocate(fp,src2,frame);
			writeCodetoBuffer(fp,"  addi %s, %s, %d\n",regs[desNo].name,regs[src2No].name,src1->element.val);
		}
		else if(src2->kind==OP_CONSTANT){
			src1No = allocate(fp,src1,frame);
			writeCodetoBuffer(fp,"  addi %s, %s, %d\n",regs[desNo].name,regs[src1No].name,src2->element.val); 
		}
		else {
			src1No = allocate(fp,src1,frame);
			src2No = allocate(fp,src2,frame);
			writeCodetoBuffer(fp,"  add %s, %s, %s\n",regs[desNo].name,regs[src1No].name,regs[src2No].name);
		}
		storeRegToVar(desNo,des,frame,fp);
		freeReg(src1No);
		freeReg(src2No);
		freeReg(desNo);
		appendCode(frame,ir);
		return;
	}
	if(ir->kind == IR_SUB){
		OperandPointer des = ir->operands.threeOp.des;
		OperandPointer src1 = ir->operands.threeOp.src1;
		OperandPointer src2 = ir->operands.threeOp.src2;
		int desNo =allocate(fp,des,frame);
		int src1No = 0;
		int src2No = 0;
		/*
		if(src1->kind==OP_CONSTANT){
			src2No = allocate(fp,src2,frame);
			writeCodetoBuffer(fp,"  addi %s, %s, %d\n",regs[desNo].name,regs[src2No].name,-src1->element.val);
		}*/
		if(src2->kind==OP_CONSTANT){
			src1No = allocate(fp,src1,frame);
			writeCodetoBuffer(fp,"  addi %s, %s, %d\n",regs[desNo].name,regs[src1No].name,-src2->element.val); 
		}
		else {
			src1No = allocate(fp,src1,frame);
			src2No = allocate(fp,src2,frame);
			writeCodetoBuffer(fp,"  sub %s, %s, %s\n",regs[desNo].name,regs[src1No].name,regs[src2No].name);
		}
		storeRegToVar(desNo,des,frame,fp);
		freeReg(src1No);
		freeReg(src2No);
		freeReg(desNo);
		appendCode(frame,ir);
		return;
	}
	if(ir->kind == IR_MUL){
		OperandPointer des = ir->operands.threeOp.des;
		OperandPointer src1 = ir->operands.threeOp.src1;
		OperandPointer src2 = ir->operands.threeOp.src2;
		int desNo =allocate(fp,des,frame);
		int src1No = 0;
		int src2No = 0;
		src1No = allocate(fp,src1,frame);
		src2No = allocate(fp,src2,frame);
		writeCodetoBuffer(fp,"  mul %s, %s, %s\n",regs[desNo].name,regs[src1No].name,regs[src2No].name);
		storeRegToVar(desNo,des,frame,fp);
		freeReg(src1No);
		freeReg(src2No);
		freeReg(desNo);
		appendCode(frame,ir);
		return;
	}
	if(ir->kind == IR_DIV){
		OperandPointer des = ir->operands.threeOp.des;
		OperandPointer src1 = ir->operands.threeOp.src1;
		OperandPointer src2 = ir->operands.threeOp.src2;
		int desNo =allocate(fp,des,frame);
		int src1No = 0;
		int src2No = 0;
		src1No = allocate(fp,src1,frame);
		src2No = allocate(fp,src2,frame);
		writeCodetoBuffer(fp,"  div %s, %s\n",regs[src1No].name,regs[src2No].name);
		writeCodetoBuffer(fp,"  mflo %s\n",regs[desNo].name);
		storeRegToVar(desNo,des,frame,fp);
		freeReg(src1No);
		freeReg(src2No);
		freeReg(desNo);
		appendCode(frame,ir);
		return;
	}
	//x = &y
	if(ir->kind == IR_GET_ADDR){
		OperandPointer des = ir->operands.twoOp.des;
		OperandPointer src = ir->operands.twoOp.src;
		int srcNo = allocate(fp,src,frame);
		storeRegToVar(srcNo,des,frame,fp);
		freeReg(srcNo);
		appendCode(frame,ir);
		return;
	}
	//x = *y
	if(ir->kind == IR_READ_ADDR){
		OperandPointer des = ir->operands.twoOp.des;
		OperandPointer src = ir->operands.twoOp.src;
		int srcNo = allocate(fp,src,frame);
		storeRegToVar(srcNo,des,frame,fp);
		freeReg(srcNo);
		appendCode(frame,ir);
		return;
	}
	//*x = y
	if(ir->kind == IR_WRITE_ADDR){
		OperandPointer des = ir->operands.twoOp.des;
		OperandPointer src = ir->operands.twoOp.src;
		
	
		int srcNo = allocate(fp,src,frame);
	
		storeRegToVar(srcNo,des,frame,fp);

		freeReg(srcNo);
		appendCode(frame,ir);
		return;
	}
	if(ir->kind == IR_GOTO){
		
		OperandPointer des = ir->operands.oneOp.op;
		writeCodetoBuffer(fp,"  j %s\n",des->element.name);
		appendCode(frame,ir);
		return;
	}
	// if x op y goto z
	if(ir->kind == IR_IF_GOTO){
		OperandPointer x = ir->operands.ifGoto.x;
		OperandPointer relop = ir->operands.ifGoto.relop;
		OperandPointer y = ir->operands.ifGoto.y;
		OperandPointer z = ir->operands.ifGoto.z;
		int xNo = allocate(fp,x,frame);
		int yNo = allocate(fp,y,frame);
		writeRelop(fp,relop->element.name,regs[xNo].name,regs[yNo].name,z->element.name);
		freeReg(xNo);
		freeReg(yNo);
		appendCode(frame,ir);
		return;
	}
	if(ir->kind == IR_RETURN){
		OperandPointer x = ir->operands.oneOp.op;
		int xNo = allocate(fp,x,frame);
		//no no no 一个函数可能有好几个return 
		appendCode(frame,ir);
		//writeCodeToFile(fp,frame);
		//放到函数入口处理
		writeCodetoBuffer(fp,"  move $v0, %s\n",regs[xNo].name);
		writeCodetoBuffer(fp,"  move $sp ,$fp\n");// 有待商榷
		writeCodetoBuffer(fp,"  lw $ra, 4($fp)\n");
		writeCodetoBuffer(fp,"  lw $fp, 0($fp)\n");
		writeCodetoBuffer(fp,"  add $sp, $sp, 8\n");
		writeCodetoBuffer(fp,"  jr $ra\n");
		appendCode(frame,ir);
		freeReg(xNo);
		para_count = 0;
		return;
	}
	if(ir->kind == IR_DEC){
		OperandPointer x = ir->operands.dec.op;
		int size = ir->operands.dec.size;
		
		//fprintf(fp," addi $sp, $sp, %d\n",-size+4);
		//frame->size-=(size-4);
		//frame->max_size +=(size-4);
		//只需要知道数组的offset,后续调用数组用&x 
		//不对还是得存取值
		frame->size -= size;
		frame->max_size += size;
		int xNo = allocate(fp,x,frame);
		writeCodetoBuffer(fp,"  la %s, %d($fp)\n",regs[xNo].name,frame->size+4);
		storeRegToVar(xNo,x,frame,fp);
		
		//int xNo = allocate(fp,x,frame);
		freeReg(xNo);
		appendCode(frame,ir);
		return;
	}
	if(ir->kind == IR_ARG){
		//函数参数直接存栈底，记得要恢复
		OperandPointer arg = ir->operands.oneOp.op;
		arg_count++;
		writeCodetoBuffer(fp,"  addi $sp, $sp, -4\n");
		// frame->size-=4;
		// frame->max_size+=4;
		int argNo = allocate(fp,arg,frame);
		writeCodetoBuffer(fp,"  sw %s, 0($sp)\n",regs[argNo].name);
		appendCode(frame,ir);
		freeReg(argNo);
		return;
	}
	if(ir->kind == IR_CALL){
		OperandPointer res = ir->operands.twoOp.des;
		OperandPointer func = ir->operands.twoOp.src;
		writeCodetoBuffer(fp,"  jal %s\n",func->element.name);
		//恢复栈帧
		writeCodetoBuffer(fp,"  addi $sp, $sp,%d\n",4*arg_count);
		// frame->size+=4*arg_count;
		// 这些是会变不统计
 		arg_count = 0;
		storeRegToVar(V0,res,frame,fp);
		
		appendCode(frame,ir);
		return;
	}
	if(ir->kind == IR_PARAM){
		OperandPointer para = ir->operands.oneOp.op;
		int paraOp = allocate(fp,para,frame);
		writeCodetoBuffer(fp,"  lw %s, %d($fp)\n",regs[paraOp].name,8+4*para_count);
		storeRegToVar(paraOp,para,frame,fp);
		para_count++;
		appendCode(frame,ir);
		freeReg(paraOp);
		return;

	}
	if(ir->kind == IR_READ){
		
		writeCodetoBuffer(fp,"  jal read\n");
		OperandPointer x = ir->operands.oneOp.op;
		storeRegToVar(V0,x,frame,fp);
		appendCode(frame,ir);
		return;
	}
	if(ir->kind == IR_WRITE){
		OperandPointer x = ir->operands.oneOp.op;
		int xNo = allocate(fp,x,frame);
		writeCodetoBuffer(fp,"  move $a0, %s\n",regs[xNo].name);
		writeCodetoBuffer(fp,"  jal write\n");
		freeReg(xNo);
		appendCode(frame,ir);
		return;
	}

}
void showFrame(){
	Frame *curr;
	curr = stack.frameList;
	while (curr!=NULL)
	{
		printf("func %s size%d max_size%d\n",curr->funcName,curr->size,curr->max_size);
		VarDesList *temp = curr->varDesList;
		while (temp!=NULL)
		{
			if(temp->op->kind==OP_CONSTANT)
				printf("cons %d, offset %d\n",temp->op->element.val,temp->fpoffset);
			else{
				printf("var %s, offset %d\n",temp->op->element.name,temp->fpoffset);
			}
			temp=temp->next;
		}
		curr=curr->next;
		printf("\n");
	}
	
}
void allIRtoMips(FILE*fp){
	init_regs();
	init_write_read(fp);
	stack.curr = NULL;
	stack.frameList = NULL;
	IRInstruction* ir = ir_head;
	// printf("***********************\n");
	// printf_irs();
	// printf("***********************\n");
	while(ir!=NULL){
		//printf_ir(ir);
		
		
		IRtoMips(fp,ir,stack.curr);
		ir = ir->next;
	}
	writeCodeToFile(fp,stack.curr);
	//showFrame();
}


