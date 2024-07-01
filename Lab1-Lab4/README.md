当前目录下的代码包括南京大学《编译原理》课程**实验一**至**实验四**的全部内容，具体来说：
+    **词法分析**与**语法分析**模块的实现在[lexical.l](./Code/lexical.l)、[syntax.y](./Code/syntax.y)、[treenode.h](./Code/treenode.h)与[treenode.c](./Code/treenode.c)中，并需要在`main`函数中调用`treenode.c`的`dfs`函数；
+    **语义分析**模块的实现在之前的基础上加入了[symbol_table.h](./Code/symbol_table.h)与[symbol_table.c](./Code/symbol_table.c)，并需要将之前`main`函数中调用的`dfs`函数改为调用`symbol_table.c`的`dfs_init_symbol_table`函数，并需要在调用之前向符号表中预加入`read`和`write`函数；
+    **中间代码生成**模块的实现在之前的基础上加入了[ir_produce.h](./Code/ir_produce.h)与[ir_produce.c](./Code/ir_produce.c)，并需要将`main`函数中调用的函数改为`ir_produce.c`的`dfs_lab3`函数；
+    **目标代码生成**模块的实现在之前的基础上加入了[code_generate.h](./Code/code_generate.h)与[code_generate.c](./Code/code_generate.c)，并需要在`main`函数中额外调用`code_generate.c`的`allIRtoMips`函数。

`Code`目录下的`main`函数的调用结构为`Lab4`的结果，若需要只实现前几个任务阶段的内容，请阅读对应文件进行调整。

另外，本实验中我们小组的额外任务分别是：
+    `Lab1`的选做1.3；
+    `Lab2`的选做2.1；
+    `Lab3`的选做3.2。