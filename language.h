#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <ctype.h>

#include "log.h"
#include "dump.h"


void readFile(struct Text* text);
void trimSpaces(struct Text* text);

int Tokenize(struct Token **tokens, struct Text* text);
char* getFuncName(const char* str);
char* getVarName(const char* str);
double getVal(const char* str, int* digits);
struct Token* CreateToken(int type, int LangOp, double dbl, char* name, size_t line);
struct node* CreateNode(int type, int LangOp, double dbl, char* name);
void tokenDump(struct Token **tokens, size_t tokenSize);
char* getSpecName(int spec);


const char* SOURCE = "userCode.txt";

enum LENS {
    MAX_FUNC_NAME = 30,
    MAX_VAR_NAME = 30,
};

struct Text {
    char *buff;
    size_t ch_count;
    size_t lines_count;
};

struct node {
    union LangUnion* value;
    struct node*    parent;
    struct node*      left;
    struct node*     right;
};

union LangUnion {
    double         dbl;
    int       langType;
    char*      Name;
};

struct Token {
    int           type;
    struct node * node;
    size_t        line;
};

enum Types {
    VAL_TYPE,
    VAR_TYPE,
    SPEC_TYPE,
    NAME_TYPE,
};

enum LangTypes {
    UNKNOWN,
    ADD,
    SUB,
    DIV,
    MUL,
    POW,
    AND,
    OR,
    EQ,

    // Comparison
    IS_EE,
    IS_GE,
    IS_BE,
    IS_GT,
    IS_BT,
    IS_NE,
    
    // Special nodes 
    ST,
    IF,
    ELSE,
    NIL,
    VAR,
    WHILE,
    FUNC,
    RET,
    CALL,
    PARAM,

    //for Tokenizer
    SoFOO,       //start of func
    EoFOO,       //end of func

};