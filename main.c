#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>



void print_prompt()//1)输出提示符
{
    printf("db > ");
}

typedef struct//2)设定数据结构,用于接受输入的字符串
{
    char* buffer;
    size_t buffer_length;
    ssize_t Input_length;
} InputBuffer;

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
            stdin
    );
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


int main(int argc,char* argv[]) {

    InputBuffer* input_buffer = new_input_buffer();

    while(true)
    {
        print_prompt();
        read_input(input_buffer);
        if(strcmp(input_buffer->buffer,".exit") == 0) {
            close_input_buffer(input_buffer);
            exit(EXIT_SUCCESS);
        } else
        {
            printf("Unrecongnized command '%s'.\n",input_buffer->buffer);
        }
    }
}

