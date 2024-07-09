#include <stdio.h>
#include "helpers/vector.h"
#include "compiler.h"
int main(int argc, char** argv)
{
    const char* input_file = "./test.c";
    const char* output_file = "./test";
    const char* option = "exec";

    if (argc > 1)
    {
        input_file = argv[1];
    }

    if (argc > 2)
    {
        output_file = argv[2];
    }

    if (argc > 3)
    {
        option = argv[3];
    }

    int compile_flags = COMPILE_PROCESS_EXECUTE_MASM;
    // 表示编译输出应该是一个对象文件而不是可执行文件
    if (S_EQ(option, "object"))
    {
        compile_flags |= COMPILE_PROCESS_EXPORT_AS_OBJECT;
    }

    int res = compile_file(input_file, output_file, compile_flags);
    if (res == COMPILER_FILE_COMPILED_OK)
    {
        printf("everything compiled file\n");
    }
    else if(res == COMPILER_FAILED_WITH_ERRORS)
    {
        printf("Compile failed\n");
    }
    else
    {
        printf("Unknown response for compile time\n");
    }

    // 如果compile_flags包含COMPILE_PROCESS_EXECUTE_MASM标志，说明需要使用NASM汇编器和GCC链接器进一步处理文件
    if (compile_flags & COMPILE_PROCESS_EXECUTE_MASM)
    {
        char nasm_output_file[40];
        char nasm_cmd[512];
        sprintf(nasm_output_file, "%s.o", output_file);
        if (compile_flags & COMPILE_PROCESS_EXPORT_AS_OBJECT)
        {
            sprintf(nasm_cmd, "nasm -f elf32 %s -o %s", output_file, nasm_output_file);
        }
        else
        {
            // 使用NASM生成对象文件。否则，使用NASM生成对象文件后，再使用GCC链接生成可执行文件
            sprintf(nasm_cmd, "nasm -f elf32 %s -o %s && gcc -m32 %s -o %s", output_file, nasm_output_file, nasm_output_file, output_file);
        }

        printf("%s", nasm_cmd);
        int res = system(nasm_cmd);
        if (res < 0)
        {
            printf("Issue assemblign the assembly file with NASM and linking with gcc");
            return res;
        }

    }
    return 0;
}