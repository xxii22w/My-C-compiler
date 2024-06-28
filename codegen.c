#include "compiler.h"
#include "helpers/vector.h"
#include <stdarg.h>
#include <stdio.h>
static struct compile_process* current_process = NULL;

void codegen_new_scope(int flags)
{
    #warning "The resolver needs to exist for this to work"
}

void codegen_finish_scope()
{
    #warning "You need to invent a resolver for this to work"
}

struct node* codegen_node_next()
{
    return vector_peek_ptr(current_process->node_tree_vec);
}

void asm_push_args(const char* ins,va_list args)
{
    va_list args2;
    va_copy(args2,args);
    vfprintf(stdout,ins,args);
    fprintf(stdout,"\n");
    if(current_process->ofile)
    {
        vfprintf(current_process->ofile,ins,args2);
        fprintf(current_process->ofile,"\n");
    }
}

void asm_push(const char* ins,...)
{
    va_list args;
    va_start(args,ins);
    asm_push_args(ins,args);
    va_end(args);
}

static const char* asm_keyword_for_size(size_t size, char* tmp_buf)
{
    const char* keyword = NULL;
    switch(size)
    {
        case DATA_SIZE_BYTE:
            keyword = "db";
        break;
        case DATA_SIZE_WORD:
            keyword = "dw";
            break;
        case DATA_SIZE_DWORD:
            keyword = "dd";
            break;

        case DATA_SIZE_DDWORD:
            keyword = "dq";
            break;

        default:
            sprintf(tmp_buf, "times %lld db ", (unsigned long)size);
            return tmp_buf;
    }

    strcpy(tmp_buf, keyword);
    return tmp_buf;
}


void codegen_generate_global_variable_for_primitive(struct node* node)
{
    char tmp_buf[256];
    if(node->var.val != NULL)
    {
        // Handle the value
        if (node->var.val->type == NODE_TYPE_STRING)
        {
            #warning "dont forget to handle the string value"
        }
        else
        {
            #warning "dont forget to handle the numeric value"
        }
    }
    asm_push("%s: %s 0", node->var.name, asm_keyword_for_size(variable_size(node), tmp_buf));
}

void codegen_generate_global_variable(struct node* node)
{
    asm_push("; %s %s", node->var.type.type_str, node->var.name);
    switch(node->var.type.type)
    {
        case DATA_TYPE_VOID:
        case DATA_TYPE_CHAR:
        case DATA_TYPE_SHORT:
        case DATA_TYPE_INTEGER:
        case DATA_TYPE_LONG:
            codegen_generate_global_variable_for_primitive(node);
        break;

        case DATA_TYPE_DOUBLE:
        case DATA_TYPE_FLOAT:
            compiler_error(current_process, "Doubles and floats are not supported in our subset of C\n");
        break;
    }
}

void codegen_generate_data_section_part(struct node* node)
{
    // 在这里创建一个开关，用于处理全局数据。
    switch (node->type)
    {
    case NODE_TYPE_VARIABLE:
        codegen_generate_global_variable(node);
        break;
    
    default:
        break;
    }
}

void codegen_generate_data_section()
{
    // section .data 是一个指令，用于定义数据段（data segment）的开始。
    // 数据段是存储初始化数据的地方，这些数据通常是程序启动时就已经确定的值，如全局变量和常量。
    /*
        初始化数据：数据段中的数据在程序启动时被初始化。
        内存位置：在许多架构中，数据段被放置在内存的特定位置，通常是在代码段（text segment）之后。
        读写访问：数据段通常允许读写访问，与只读的代码段不同。
        持久性：数据段中的数据在程序执行期间保持不变，直到程序显式修改它们。
    */
    asm_push("section .data");
    struct node* node = codegen_node_next();
    while(node)
    {
        codegen_generate_data_section_part(node);
        node = codegen_node_next();
    }
}

void codegen_generate_root_node(struct node* node)
{
    // PROCESS ANY FUNCTIONS.
}

void codegen_generate_root()
{
    // section .text 是一个指令，用于指定代码段（text segment）的开始。代码段是存储程序的可执行指令的地方。
    /*
        可执行指令：代码段包含程序的机器指令，这些指令是由处理器执行的。
        内存位置：在许多架构中，代码段通常位于内存的起始部分，紧接着是数据段和堆栈。
        只读访问：出于安全考虑，代码段通常被设置为只读，以防止程序在运行时被篡改。
        持久性：代码段在程序的整个生命周期内保持不变，直到程序结束。
    */
    asm_push("section .text");
    struct node* node = NULL;
    while((node = codegen_node_next()) != NULL)
    {
        codegen_generate_root_node(node);
    }
}

void codegen_write_strings()
{
    #warning "Loop through the string table and write all the strings."
}

void codegen_generate_rod()
{
    // section .rodata（Read-Only Data Segment）用于声明只读数据段的开始。
    // 这个段通常用于存储程序中的只读数据，比如字符串常量、静态数组等。
    /*
        不可变性：.rodata 段中的数据在程序运行时不应该被修改。
        内存访问：数据可以被程序读取，但通常不允许写入。
        性能优化：由于只读数据段可能被缓存或放置在内存的特定区域，访问这些数据可能比其他数据段更快。
    */
    asm_push("section .rodata");
    codegen_write_strings();
}

int codegen(struct compile_process* process)
{
    current_process = process;
    scope_create_root(process);
    vector_set_peek_pointer(process->node_tree_vec,0);
    codegen_new_scope(0);
    // 数据段
    codegen_generate_data_section();
    vector_set_peek_pointer(process->node_tree_vec,0);
    // 代码段
    codegen_generate_root();

    codegen_finish_scope();

    // Generate read only data
    // 只读数据段
    codegen_generate_rod();
    return 0;
}

/*
    // jmp 是无条件跳转指令
    asm_push("jmp %s","label_name");        
*/