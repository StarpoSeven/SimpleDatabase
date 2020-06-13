#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


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
    char username[COLUMN_USERNAME_SIZE + 1];//C语言字符串结尾，with a null character
    char email[COLUMN_EMAIL_SIZE + 1];
}Row;
//Part3-1结束

typedef enum {
    CONVERT_STATEMENT_SUCCESS,
    COVERT_STATEMENT_UNRECOGNIZED,
    COVERT_NEGATIVE_ID,
    COVERT_STATEMENT_SYNTAX_ERROR,
    COVERT_STATEMENT_STRING_TOO_LONG
} StatementCovertResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
}StatementType;

typedef struct {
    StatementType statementType;
    Row row_to_insert;
}Statement;

//Part4-1将Convert_Input_Statementh中 if(strncmp(buffer,"insert",6）修改成函数，去掉sscanf，换成stroke
StatementCovertResult Prepare_Insert(InputBuffer* buffer_pointer,Statement* statement_pointer) {
    statement_pointer->statementType = STATEMENT_INSERT;
    char* keyword = strtok(buffer_pointer->buffer," ");
    char* id_string = strtok(NULL," ");
    char* username = strtok(NULL," ");
    char* email = strtok(NULL," ");
    if(id_string == NULL || username == NULL || email == NULL) {
        return COVERT_STATEMENT_SYNTAX_ERROR;
    }
    int id = atoi(id_string);
    if(id < 0) {
        return COVERT_NEGATIVE_ID;
    }
    if(strlen(username) > COLUMN_USERNAME_SIZE) {
        return COVERT_STATEMENT_STRING_TOO_LONG;
    }
    if(strlen(email) > COLUMN_EMAIL_SIZE) {
        return COVERT_STATEMENT_STRING_TOO_LONG;
    }

    statement_pointer->row_to_insert.ID = id;
    strcpy(statement_pointer->row_to_insert.username,username);
    strcpy(statement_pointer->row_to_insert.email,email);
    return CONVERT_STATEMENT_SUCCESS;
}

StatementCovertResult Convert_Input_Statement(InputBuffer* buffer_pointer,Statement* statement_pointer) {
    if(strncmp(buffer_pointer->buffer,"insert",6) == 0) {
        //Part4-1将下面部分写成Prepare_Insert函数
//        statement_pointer->statementType = STATEMENT_INSERT;
//        //Part3-1中添加
//        int args_assigned = sscanf(buffer_pointer->buffer,"insert %d %s %s",
//                &(statement_pointer->row_to_insert.ID),
//                statement_pointer->row_to_insert.username,
//                statement_pointer->row_to_insert.email);
//        if(args_assigned < 3){
//            return COVERT_STATEMENT_SYNTAX_ERROR;
//        }
//        //Part3-1结束
//        return CONVERT_STATEMENT_SUCCESS;
            return Prepare_Insert(buffer_pointer,statement_pointer);
        //Part4-1结束
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


//5-1中添加新的数据结构
typedef struct {
    int file_descriptor;
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGES];
}Pager;
//5-1结束

typedef struct {
    uint32_t row_total;
    Pager* pager;//5-1中添加
}Table;






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


//5-1
Pager* Create_pager(const char* filename) {
    int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    if(fd == -1) {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    off_t file_length = lseek(fd,0,SEEK_END);
    Pager* pager_pointer = malloc(sizeof(Pager));
    pager_pointer->file_descriptor = fd;
    pager_pointer->file_length = file_length;

    for(uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager_pointer->pages[i] = NULL;
    }
    return pager_pointer;
}
Table* Create_Table(const char* filename) {
    Table* table_pointer = (Table*) malloc(sizeof(Table));
    table_pointer->pager = Create_pager(filename);
    table_pointer->row_total = table_pointer->pager->file_length / ROW_SIZE;
    return table_pointer;
}
void* Get_page(Pager* pager, uint32_t page_num) {
    if(page_num > TABLE_MAX_PAGES) {
        printf("Tried to fetch page number out of bounds. %d > %d",page_num,TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }
    if(pager->pages[page_num] == NULL) {
        uint32_t page_num_file = pager->file_length / PAGE_SIZE;
        if(pager->file_length % PAGE_SIZE) {
            page_num_file += 1;
        }
        void* page_load_data = malloc(PAGE_SIZE);
        if(page_num <= page_num_file) {
            lseek(pager->file_descriptor,page_num*PAGE_SIZE,SEEK_SET);
            ssize_t bytes_read = read(pager->file_descriptor,page_load_data,PAGE_SIZE);
            if(bytes_read == -1) {
                printf("Error reading file:%d\n",errno);
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[page_num] = page_load_data;
    }
    return pager->pages[page_num];
}
//5-1结束

void* Row_Slot(Table* table_pointer,uint32_t row_number) {

    uint32_t page_number = row_number / ROWS_PER_PAGE;
    void* page = Get_page(table_pointer->pager,page_number);
    uint32_t row_offset = row_number % ROWS_PER_PAGE;
    uint32_t bytes_offset = row_offset * ROW_SIZE;
    return page + bytes_offset;
}

//3
void serialize(Row* row_to_insert,void* row_location_in_table) {
    memcpy(row_location_in_table+ID_OFFSET,&(row_to_insert->ID),ID_SIZE);
//    memcpy(row_location_in_table+USERNAME_OFFSET,&(row_to_insert->username),USERNAME_SIZE);
//    memcpy(row_location_in_table+EMAIL_OFFSET,&(row_to_insert->email),EMAIL_SIZE);
    strncpy(row_location_in_table+USERNAME_OFFSET,row_to_insert->username,USERNAME_SIZE);
    strncpy(row_location_in_table+EMAIL_OFFSET,row_to_insert->email,EMAIL_SIZE);
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

//5-2
void page_flush(Pager* pager,uint32_t page_num, uint32_t size ) {
    if(pager->pages[page_num] == NULL) {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->file_descriptor,page_num*PAGE_SIZE,SEEK_SET);
    if(offset == -1) {
        printf("Error seeking: %d\n",errno);
        exit(EXIT_FAILURE);
    }
    ssize_t bytes_written = write(pager->file_descriptor,pager->pages[page_num],size);
    if(bytes_written == -1) {
        printf("Error writing:%d\n",errno);
        exit(EXIT_FAILURE);
    }
}

void db_close(Table* table) {
    uint32_t pages_full = table->row_total / ROWS_PER_PAGE;
    Pager* pager = table->pager;
    for(int i = 0; i < pages_full; i++) {
        if(pager->pages[i] == NULL) {
            continue;
        }
        page_flush(pager,i,PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }
    uint32_t additional_rows = table->row_total % ROWS_PER_PAGE;
    if(additional_rows > 0) {
        uint32_t page_num = pages_full;
        if(pager->pages[page_num] != NULL) {
            page_flush(pager,page_num,additional_rows * ROW_SIZE);
            free(pager->pages[page_num]);
            pager->pages[page_num] = NULL;
        }
    }
    if(close(pager->file_descriptor) == -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }

    for(uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void* page = pager->pages[i];
        if(page != NULL) {
            free(page);
            pager->pages[i] = NULL;
        }
    }
    free(pager);
    free(table);
}
//5-2结束
MetaResult Do_Meta_Command(InputBuffer* buffer_pointer,Table* table_pointer) {
    if(strcmp(buffer_pointer->buffer,".exit") == 0) {
        Close_Buffer(buffer_pointer);
        db_close(table_pointer);
        exit(EXIT_SUCCESS);
    } else
        return META_COMMAND_UNRECOGNIZED;
}

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
/*
 * Part4在Convert_Input_Statement函数中使用的是scanf函数，如果输入的参数超过了buffer长度，会造成溢出，
 * 在拷贝进Row中时，需要对string的长度进行确认，因此需要通过space划分input，使用strtok（）
 *
 */

//1:修改Part2中的Convert_Input_Statement函数

/*
 * Part5 将内存的页输出到硬盘的file中持久化存储
 * 1：将原来的void* pages[] , 增加file_describe file_length,修改对应Table,3-2中增加新的函数
 * 2: 关闭数据库时，将数据库中的页输出到file文件
 */
//1 在3-2中增加函数三个 create——table create——pager db——close
//2 在3-4中增加函数两个 page——flush db——close
//3 修改3-3中的serialize memcpy -->strncpy




int main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("Must supply a database filename");
        exit(EXIT_FAILURE);
    }
    Table* table_pointer = Create_Table(argv[1]);
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
            case(COVERT_STATEMENT_STRING_TOO_LONG):
                printf("String is too long\n");
                continue;
            case(COVERT_STATEMENT_SYNTAX_ERROR):
                printf("Syntax error. Could not parse statement\n");
                continue;
            case(COVERT_STATEMENT_UNRECOGNIZED):
                printf("Unrecognized input order %s",buffer_pointer->buffer);
                continue;
        }
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
