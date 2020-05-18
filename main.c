#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//Par1数据结构
typedef struct//1.2)设定数据结构,用于接受输入的字符串
{
    char* buffer;
    size_t buffer_length;
    ssize_t Input_length;
} InputBuffer;

//Part2数据结构
typedef enum
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
}MetaCommandResult;

typedef enum
{
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
}PrepareResult;

typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
}StatementType;

typedef struct
{
    StatementType type;
}Statement;
/*
 * Part1中的函数
 */
void print_prompt();//1.1)输出提示符 1.2）第二步设置数据结构InputBuffer）
InputBuffer* new_input_buffer();//1.3)创建接受输入字符串的数据结构
void read_input(InputBuffer* input_buffer);//1.4)开始读取输入,关键是getline函数
void close_input_buffer(InputBuffer* input_buffer);//1.5)关闭开辟出来的内存空间

/*
 * Part2中的函数
 * 数据库中 '.' 开头的命令，叫做meta-command，需要单独处理
 */
MetaCommandResult do_meta_command(InputBuffer* inputBuffer);//2.1）接受'.'开头的命令，然后判断执行
PrepareResult prepare_statement(InputBuffer* input_Buffer,Statement* statement);//2.2)接受不是'.'开头的命令
void execute_statement(Statement* statement);//2.3)执行对应的命令



int main(int argc,char* argv[]) {

    InputBuffer* input_buffer = new_input_buffer();

    while(true)
    {
        print_prompt();
        read_input(input_buffer);
        if(input_buffer->buffer[0] == '.'){
            switch(do_meta_command(input_buffer)){
                case(META_COMMAND_SUCCESS):
                    continue;
                case(META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s'\n",input_buffer->buffer);
                    continue;
                }
            }

        Statement statement;
        switch(prepare_statement(input_buffer,&statement)) {
            case(PREPARE_SUCCESS):
                break;
            case(PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n",input_buffer->buffer);
                continue;
        }
        execute_statement(&statement);
        printf("Executed.\n");
    }
}
//Par1
void print_prompt()//1)输出提示符 2）第二步设置数据结构InputBuffer
{
    printf("db > ");
}
InputBuffer* new_input_buffer()//3)创建接受输入字符串的数据结构
{
    InputBuffer* input_buffer = (InputBuffer*) malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->Input_length = 0;
    return input_buffer;
}

void read_input(InputBuffer* input_buffer)//4)开始读取输入,关键是getline函数
{
    ssize_t bytes_read = getline(
            &(input_buffer->buffer),
            &(input_buffer->buffer_length),
            stdin);
    if(bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    input_buffer->buffer_length = bytes_read - 1;
    input_buffer->buffer[bytes_read-1] = 0;
}

void close_input_buffer(InputBuffer* input_buffer) {//5)关闭开辟出来的内存空间
    free(input_buffer->buffer);//getlline函数中用于接收
    free(input_buffer);//malloc开辟
}
//Part1 End

//Part2
MetaCommandResult do_meta_command(InputBuffer* inputBuffer )//2.1执行'.'开头的命令
{
    if(strcmp(inputBuffer->buffer,".exit") == 0) {
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

PrepareResult prepare_statement(InputBuffer* input_Buffer,Statement* statement)//2.2执行不是'.'开头的命令
{
    if(strncmp(input_Buffer->buffer,"insert",6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }

    if(strcmp(input_Buffer->buffer,"select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement)//2.3接受2.2中的状态，并且执行对应的状态。
{
    switch(statement->type){
        case(STATEMENT_INSERT):
            printf("This is where we would do an insert.\n");
            break;
        case(STATEMENT_SELECT):
            printf("This is where we would do an select.\n");
            break;
    }
}


