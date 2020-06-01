#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

/*
 * Part 1 : 创建出整个基本骨架，能够接受用户输入的命令
 * 1:定义存储输入命令的buffer，一个结构体，包括一个字符指针，一个长度标示
 * 2:创建buffer的函数
 * 3：读取用户输入的命令，存入buffer
 */

void Print_Prompt() {
    printf("db >");
}
//1.1
typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t buffer_inputlength;
} InputBuffer;

//1.2
InputBuffer* Create_Input_Buffer() {
    InputBuffer* buffer_pointer = (InputBuffer*)malloc(sizeof(InputBuffer));
    buffer_pointer->buffer = NULL;
    buffer_pointer->buffer_length = 0;
    buffer_pointer->buffer_inputlength = 0;
    return buffer_pointer;
}

//1.3
void Read_Input_Order(InputBuffer* buffer_pointer) {
    ssize_t bytes_read = getline(&(buffer_pointer->buffer),&(buffer_pointer->buffer_length),stdin);
    if(bytes_read <= 0) {
        printf("read input order failed\n");
        exit(EXIT_FAILURE);
    }
    buffer_pointer->buffer_inputlength = bytes_read - 1;
    buffer_pointer->buffer[bytes_read - 1] = 0;
}

void Close_Buffer(InputBuffer* buffer_pointer) {
    free(buffer_pointer->buffer);
    free(buffer_pointer);
}

/*
 * Part2 输入对应的命令能偶进行判断交给对应的函数执行，包括两种命令，.开头的meta_command,和不是.开头的Statement
 * 1：定义出执行meta_command的函数,在其中执行对应的meta_command,然后给出执行的反馈， 成功还是失败
 * 2：定义出转化statement的函数,在其中将用户输入的statement转化成 自定义的internal——representation,然后给出转化反馈
 * 成功还是失败
 * 3: 定义出执行转化后statement的函数
 *
 */
//1
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED
} MetaResult;

//MetaResult Do_Meta_Command(InputBuffer* buffer_pointer,Table* table_pointer) {
//    if(strcmp(buffer_pointer->buffer,".exit") == 0) {
//        Close_Buffer(buffer_pointer);
//        Free_Table(table_pointer);//table定义在part3中，函数整体挪到part3
//
//        exit(EXIT_SUCCESS);
//    } else
//    return META_COMMAND_UNRECOGNIZED;
//}

//2
//Part3-1
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
typedef struct {
    uint32_t ID;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
}Row;
//Part3-1结束

typedef enum {
    CONVERT_STATEMENT_SUCCESS,
    COVERT_STATEMENT_UNRECOGNIZED,
    COVERT_STATEMENT_SYNTAX_ERROR
} StatementCovertResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
}StatementType;

typedef struct {
    StatementType statementType;
    Row row_to_insert;
}Statement;


StatementCovertResult Convert_Input_Statement(InputBuffer* buffer_pointer,Statement* statement_pointer) {
    if(strncmp(buffer_pointer->buffer,"insert",6) == 0) {
        statement_pointer->statementType = STATEMENT_INSERT;
        //Part3-1中添加
        int args_assigned = sscanf(buffer_pointer->buffer,"insert %d %s %s",
                &(statement_pointer->row_to_insert.ID),
                statement_pointer->row_to_insert.username,
                statement_pointer->row_to_insert.email);
        if(args_assigned < 3){
            return COVERT_STATEMENT_SYNTAX_ERROR;
        }
        //Part3-1结束
        return CONVERT_STATEMENT_SUCCESS;
    }
    if(strcmp(buffer_pointer->buffer,"select") == 0) {
        statement_pointer->statementType = STATEMENT_SELECT;
        return CONVERT_STATEMENT_SUCCESS;
    }
    return COVERT_STATEMENT_UNRECOGNIZED;
}

//3：整体移到Part3的4中
//void Execute_Statement_After_Converting(Statement* statement_pointer) {
//    switch(statement_pointer->statementType) {
//        case(STATEMENT_INSERT):
//            break;
//        case(STATEMENT_SELECT):
//            break;
//    }
//}
/*
 * Part3 对Part2中的statement执行函数，添加具体的实现细节，实现两个命令Insert 和select, 其中insert是将行插入到表中，
 * select则是显示出表中所有的行
 * 1：首先定义出使用到的数据结构，包括 表table，表中的每一行Row,并定义出创建table的函数
 * 2: 在对表进行插入和select操作的时候，都需要能够选出表中的某一行，在这里增加对应的函数Row_slot，
 * 同时需要先定义每一行的大小ROW_SIZE，每一页的大小PAGE_SIZE，每一页多少行ROWS_PER_PAGE，
 * 整个表中最多少页，TABLE_MAX_PAGES，整个表中最多多少行TABLE_MAX_ROWS
 * 3:在通过Row_slot函数确定行的位置之后，insert和select命令（这两个都属于statement命令)将数据进行拷贝
 * 也就是从将需要插入的row拷贝到表中的指定位置，将表中指定位置的row数据，拷贝出来进行展示，这两个函数分别是serialize和deserialze
 * 4：有了确定行位置的函数 row—slot， 和 将row插入指定位置的serialize，和将指定位置行展示的deserialize，
 * 现再是将其封装到execute—statement-after-converting函数中修改Part2中3
 */
//1，行定义在Part2的2中，下面是定义table，同时添加创建table的函数，修改Convert_Statement

#define TABLE_MAX_PAGES 100
typedef struct {
    uint32_t row_total;
    void* pages[TABLE_MAX_PAGES];
}Table;

Table* Create_Table() {
    Table* table_pointer = (Table*) malloc(sizeof(Table));
    table_pointer->row_total = 0;
    for(uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        table_pointer->pages[i] = NULL;
    }
    return table_pointer;
}

void Free_Table(Table* table_pointer) {
    for(int i = 0; table_pointer->pages[i]; i++) {
        free(table_pointer->pages[i]);
    }
    free(table_pointer);
}

MetaResult Do_Meta_Command(InputBuffer* buffer_pointer,Table* table_pointer) {
    if(strcmp(buffer_pointer->buffer,".exit") == 0) {
        Close_Buffer(buffer_pointer);
        Free_Table(table_pointer);
        exit(EXIT_SUCCESS);
    } else
        return META_COMMAND_UNRECOGNIZED;
}

//2
#define size_of_attribute(Struct,Attribute) sizeof(((Struct*)0)->Attribute)
const uint32_t ID_SIZE = size_of_attribute(Row,ID);
const uint32_t USERNAME_SIZE = size_of_attribute(Row,username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row,email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;


const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;

void* Row_Slot(Table* table_pointer,uint32_t row_number) {

    uint32_t page_number = row_number / ROWS_PER_PAGE;
    void* page = table_pointer->pages[page_number];
    if(table_pointer->pages[page_number] == 0) {
        page = table_pointer->pages[page_number] = malloc(PAGE_SIZE);
    }
    uint32_t row_offset = row_number % ROWS_PER_PAGE;
    uint32_t bytes_offset = row_offset * ROW_SIZE;
    return page + bytes_offset;
}

//3
void serialize(Row* row_to_insert,void* row_location_in_table) {
    memcpy(row_location_in_table+ID_OFFSET,&(row_to_insert->ID),ID_SIZE);
    memcpy(row_location_in_table+USERNAME_OFFSET,&(row_to_insert->username),USERNAME_SIZE);
    memcpy(row_location_in_table+EMAIL_OFFSET,&(row_to_insert->email),EMAIL_SIZE);
}

void desreialize(void* row_location_in_table,Row* row_to_show) {
    memcpy(&(row_to_show->ID),row_location_in_table+ID_OFFSET,ID_SIZE);
    memcpy(&(row_to_show->username),row_location_in_table+USERNAME_OFFSET,USERNAME_SIZE);
    memcpy(&(row_to_show->email),row_location_in_table+EMAIL_OFFSET,EMAIL_SIZE);
}
//4
typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_FAILED_TABLE_FULL
}ExecuteResult;

const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

ExecuteResult Execute_Insert_Statement(Statement* statement, Table* table_pointer) {
    if(table_pointer->row_total > TABLE_MAX_ROWS) {
        return EXECUTE_FAILED_TABLE_FULL;
    }
    serialize(&(statement->row_to_insert),Row_Slot(table_pointer,table_pointer->row_total));
    table_pointer->row_total += 1;
    return EXIT_SUCCESS;
}

void print_row(Row* row) {
    printf("%d %s %s\n",row->ID,row->username,row->email);
}


ExecuteResult Execute_Select_Statement(Table* table_pointer) {

    Row row_to_show;
    for(uint32_t i = 0; i < table_pointer->row_total; i++) {
        desreialize(Row_Slot(table_pointer,i),&row_to_show);
        print_row(&row_to_show);
    }
    return EXECUTE_SUCCESS;
}



ExecuteResult Execute_Statement_After_Converting(Statement* statement_pointer, Table* table_pointer) {
    switch(statement_pointer->statementType) {
        case(STATEMENT_INSERT):
            return(Execute_Insert_Statement(statement_pointer,table_pointer));
        case(STATEMENT_SELECT):
            return Execute_Select_Statement(table_pointer);
    }
}


int main(int argc, char* argv[]) {
    Table* table_pointer = Create_Table();
    InputBuffer* buffer_pointer = Create_Input_Buffer();

    while(true) {
        Print_Prompt();
        Read_Input_Order(buffer_pointer);
        if(buffer_pointer->buffer[0] == '.') {
            switch (Do_Meta_Command(buffer_pointer,table_pointer)) {
                case (META_COMMAND_SUCCESS) :
                    continue;
                case (META_COMMAND_UNRECOGNIZED) :
                    continue;
            }
        }

        Statement statement;
        switch(Convert_Input_Statement(buffer_pointer,&statement)) {
            case(CONVERT_STATEMENT_SUCCESS):
                break;
            case(COVERT_STATEMENT_SYNTAX_ERROR):
                printf("Syntax error. Could not parse statement\n");
                continue;
            case(COVERT_STATEMENT_UNRECOGNIZED):
                printf("Unrecognized input order %s",buffer_pointer->buffer);
                continue;
        }

//        printf("??input:%s??\n",buffer_pointer->buffer);
//        printf("parse:%d %s %s\n",statement.row_to_insert.ID,statement.row_to_insert.username,statement.row_to_insert.email);
//        printf("%d\n",statement.statementType);
        switch(Execute_Statement_After_Converting(&statement,table_pointer)) {
            case(EXECUTE_SUCCESS):
                printf("Executed.\n");
                break;
            case(EXECUTE_FAILED_TABLE_FULL):
                printf("Error table full");
                break;
        }


    }

}
