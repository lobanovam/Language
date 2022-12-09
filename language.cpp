#include "language.h"


int main() {
     struct Text text = {nullptr, 0, 0};
     readFile(&text);
     logprint("text is \n---------------------\n%s\n---------------------\n", text.buff);
     struct Token **tokens = (struct Token**)calloc(text.ch_count, sizeof(struct Token*));
     int a = Tokenize(tokens, &text);     
}

void readFile(struct Text *text) {

    logprint("    ##### in readFile() #####\n\n");
    ASSERT(text != nullptr);

    struct stat file;
    stat(SOURCE, &file);
    size_t buffSize = file.st_size;

    text->buff = (char*)(calloc(buffSize, sizeof(char)));

    ASSERT(text->buff != nullptr);
    logprint("    buff pointer is %p\n", text->buff);

    FILE* source = NULL;
    if ((source = fopen(SOURCE, "r")) == NULL) {
        printf("can't open source file\n");
    }
    text->ch_count = fread(text->buff, sizeof(char), buffSize, source);
    fclose(source);

    trimSpaces(text);
    logprint("    lines_count is %ld\n", text->lines_count);

    logprint("\n    ##### out readFile() #####\n");
}

void trimSpaces(struct Text *text) {
    logprint("\n        ##### in trimSpaces() #####\n");
    ASSERT(text != nullptr);

    char* tmp = (char*)calloc(text->ch_count, sizeof(char));
    ASSERT(tmp != NULL);

    for (int i = 0, j = 0; i < text->ch_count; i++) {
        if (text->buff[i] == '\n') {
            text->lines_count++;
        }
        if (text->buff[i] != ' ') {
            tmp[j++] = text->buff[i];
        }
    }
    free(text->buff);
    text->buff = tmp;
    logprint("        ##### out trimSpaces() #####\n\n");
}


int Tokenize(struct Token** tokens, struct Text* text) {
    logprint("\nin Tokenize\n");
    size_t curLine = 1;
    size_t tokenId = 0;

    for (int i = 0; i < text->ch_count; ) {
        int old_i = i;

        int curCh = text->buff[i];
        logprint("\n\ni is %d, curCH is %c\n", i, curCh);

        if (curCh == '\n') {
            i++;
            curLine++;
            continue;
        }

        if (curCh == '}') {
            logprint("found }, end of function\n");
            tokens[tokenId++] = CreateToken(SPEC_TYPE, EoFOO, 0.0, NULL, curLine);
            logprint("trying to tokenDump\n");
            tokenDump(tokens, tokenId);
            logprint("tokenDump done\n");
            return 0;
        }

        //trying to get Value
        if ('0' <= curCh && curCh <= '9') {
            logprint("found val\n");
            int digits = 0;
            double val = getVal(text->buff+i, &digits);

            if (!digits) {
                logprint("SYNTAX ERROR ON LINE %d\n", curLine);
                return 1;
            }

            i += digits;

            tokens[tokenId++] = CreateToken(VAL_TYPE, -1, val, NULL, curLine);
            continue;
        }
        
        //trying to get trebleKey
        if (curCh == 't') {
            logprint("got t\n");
            if (strncmp(text->buff+i, "trebleKey", 9) == 0) {
                logprint("found trebleKey\n");
                tokens[tokenId++] = CreateToken(SPEC_TYPE, FUNC, 0.0, NULL, curLine);
                i+=9;

                logprint("cur ch is %c\n", text->buff[i]);

                char* funcName = getFuncName(text->buff+i);
                if (!funcName) {
                    logprint("SYNTAX ERROR ON LINE %d\n", curLine);
                    return 1;
                }
                i+=strlen(funcName);

                tokens[tokenId++] = CreateToken(NAME_TYPE, 0, 0.0, funcName, curLine);

                logprint("after func ch is %c\n", text->buff[i]);
                ASSERT(text->buff[i++] == '(');
                ASSERT(text->buff[i++] == ')');

                while (text->buff[i] == '\n') {
                    curLine++;
                    i++;
                }
                    
                ASSERT(text->buff[i++] == '{');
                tokens[tokenId++] = CreateToken(SPEC_TYPE, SoFOO, 0.0, NULL, curLine);
                logprint("afterr asserts ch is %c\n", text->buff[i]);
                continue;
            } else if (strncmp(text->buff+i, "tatto", 5) == 0) {
                logprint("found tatto\n");
                i+=5;
                continue;
            } else {
                logprint("SYNTAX ERROR ON LINE %d\n", curLine);
                return 1;
            }
        }

        if (curCh == 'n') {
            logprint("got n\n");
            if (strncmp(text->buff+i, "note", 4) == 0) {
                logprint("found note\n");
                logprint("cur ch is %c\n", text->buff[i]);
                
                tokens[tokenId++] = CreateToken(SPEC_TYPE, VAR, 0.0, NULL, curLine);
                i+=4;

                char* variable = getVarName(text->buff+i);
                tokens[tokenId++] = CreateToken(VAR_TYPE, 0.0, 0.0, variable, curLine);
                i+=strlen(variable);
                
                ASSERT(text->buff[i++] == '=');
                tokens[tokenId++] = CreateToken(SPEC_TYPE, EQ, 0.0, NULL, curLine);

                if ('0' <= text->buff[i] && text->buff[i] <= '9') {
                    logprint("found val\n");
                    int digits = 0;
                    double val = getVal(text->buff+i, &digits);

                    if (!digits) {
                        logprint("SYNTAX ERROR ON LINE %d\n", curLine);
                        return 1;
                    }
                    i+= digits;

                    tokens[tokenId++] = CreateToken(VAL_TYPE, -1, val, NULL, curLine);

                    continue;
                } else if (text->buff[i] == 'D') {
                    logprint("found D\n");

                    if (strncmp(text->buff+i, "D.S.alcoda", 10) == 0) {
                        logprint("found CALL func\n");
                        tokens[tokenId++] = CreateToken(SPEC_TYPE, CALL, 0.0, NULL, NULL);
                        i+=10;
                        
                        logprint("cur ch is %c\n", text->buff[i]);

                        char* funcName = getFuncName(text->buff+i);
                        if (!funcName) {
                            logprint("SYNTAX ERROR ON LINE %d\n", curLine);
                            return 1;
                        }
                        i+=strlen(funcName);

                        tokens[tokenId++] = CreateToken(NAME_TYPE, 0, 0.0, funcName, curLine);
                        logprint("got call func %s\n", funcName);
                        ASSERT(text->buff[i++] == '(');
                        ASSERT(text->buff[i++] == ')');

                        continue;
                    } else {
                        logprint("SYNTAX ERROR ON LINE %d, CAN'T FIND D.S.AL CODA\n", curLine);
                        return 1;

                    }
                }
            }
        }


        if (old_i == i) {
            logprint("SYNTAX ERROR ON LINE %d\n", curLine);
            return 1;
        }
    }
    
    return 0;
}

double getVal(const char* str, int* digits) {
    double val = 0;
    int i = 0;
    while ('0' <= str[i] && str[i] <= '9') {
            val = val * 10 + str[i] - '0';
            i++;
        }
    logprint("res val is %lf\n", val);

    *digits = i;
    return val;
}

char* getVarName(const char* str) {
    logprint("in getVarName cur ch is %c\n", *str);

    char* varName = (char*)calloc(MAX_VAR_NAME, sizeof(char));
    ASSERT(varName != NULL);

    int i = 0;
    while (str[i] >= 'a' && str[i] <= 'g') {
        varName[i] = str[i];
        if (str[i+1] == '#' || str[i+1] == '?') {
            varName[i+1] = str[i+1];
            i++;
        }
        i++;
    }
    if (!i) varName = NULL;
    logprint("result var name is %s, its len is %d\n", varName, strlen(varName));

    return varName;
}

char* getFuncName(const char* str) {
    logprint("in getFuncName cur ch is %c\n", *str);

    char* funcName = (char*)calloc(MAX_FUNC_NAME, sizeof(char));
    ASSERT(funcName != NULL);

    int i = 0;
    while (str[i] != '(' && isalpha(str[i])) {
        funcName[i] = str[i];
        i++;
    }
    if (!i) funcName = NULL;
    logprint("result func name is %s, its len is %d\n", funcName, strlen(funcName));

    return funcName;
}

struct Token* CreateToken(int type, int LangOp, double dbl, char* name, size_t line) {

    struct Token* newToken = (struct Token*)calloc(1, sizeof(struct Token));
    newToken->node = CreateNode(type, LangOp, dbl, name);
    newToken->type = type;
    newToken->line = line;

    return newToken;
}

struct node* CreateNode(int type, int LangOp, double dbl, char *name) {

    struct node* newNode = (struct node*)(calloc(1, sizeof(struct node)));
    ASSERT(newNode != NULL);

    newNode->value = (union LangUnion*)calloc(1, sizeof(union LangUnion));
    ASSERT(newNode->value != NULL);

    if (type == VAL_TYPE) {
        newNode->value->dbl = dbl;
    } else if (type == VAR_TYPE) {
        newNode->value->Name = name;
    } else if (type == SPEC_TYPE) {
        newNode->value->langType = LangOp;
    } else if (type == NAME_TYPE) {
        newNode->value->Name = name; 
    }

    return newNode;
}

void tokenDump(struct Token **tokens, size_t tokenSize) {
    FILE* DumpToken = openDump(DUMP_TOKEN);

    dumpToken("digraph {\n");
    dumpToken("rankdir=LR;\n");
    dumpToken("node [ shape=record ];\n");

    for (int i = 0; i < tokenSize; i++) {
        switch(tokens[i]->type) {
            case VAL_TYPE:
                dumpToken("struct%d [\nlabel = \"<type>type: VAL_TYPE|<dbl>dbl: %lf\", style = \"filled\", fillcolor = \"grey\" \n];\n", i, tokens[i]->node->value->dbl);
            break;

            case VAR_TYPE:
                dumpToken("struct%d [\nlabel = \"<type>type: VAR_TYPE|<var>var: %s\", style = \"filled\", fillcolor = \"green\" \n];\n", i, tokens[i]->node->value->Name);
            break;

            case NAME_TYPE:
                 dumpToken("struct%d [\nlabel = \"<type>type: NAME_TYPE|<name>name: %s\", style = \"filled\", fillcolor = \"cyan\" \n];\n", i, tokens[i]->node->value->Name);
            break;

            case SPEC_TYPE:
                char color[10] = " ";
                if (tokens[i]->node->value->langType == EoFOO || tokens[i]->node->value->langType == SoFOO) {
                    strcpy(color, "yellow");
                } else if (tokens[i]->node->value->langType == FUNC){
                    strcpy(color, "red");
                } else {
                    strcpy(color, "blue");
                }
                char* name = getSpecName(tokens[i]->node->value->langType);
                dumpToken("struct%d [\nlabel = \"<type>type: SPEC_TYPE|<spec>spec: %s\", style = \"filled\", fillcolor = \"%s\" \n];\n", i, name, color);
            break;
        }

        if (i == 0) continue;

        dumpToken("struct%d -> struct%d [weight=100];\n", i-1, i);
    }
    dumpToken("}\n");

    fclose(DumpToken);

    char cmd[100] = "dot -T png token.dot -o ";
    char name[30] = "graphCode";
    char* num = NULL;

    num = IntToString(tokenImageNum);

    strcat(num, png);
    strcat(name, num);
    strcat(cmd, name);

    logprint("%d\n", system("cd C:\\Users\\alexm\\Documents\\Language"));
    logprint("%d\n", system(cmd));

    logprint("RESULT NAME IS %s\n", name);

    tokenImageNum;
}

char* getSpecName(int spec) {
    char name[10] = " ";
    switch (spec)
    {
    case ADD:
        strcpy(name, "+");
        break;
    
    case SUB:
        strcpy(name, "-");
        break;

    case MUL:
        strcpy(name, "*");
        break;
    
    case DIV:
        strcpy(name, "/");
        break;

    case POW:
        strcpy(name, "^");
        break;

    case AND:
        strcpy(name, "&&");
        break;

    case OR:
        strcpy(name, "||");
        break;

    case EQ:
        strcpy(name, "=");
        break;

    case IS_EE:
        strcpy(name, "==");
        break;

    case IS_BE:
        strcpy(name, "<=");
        break;

    case IS_GE:
        strcpy(name, ">=");
        break;

    case IS_GT:
        strcpy(name, ">");
        break;

    case IS_BT:
        strcpy(name, "<");
        break;

    case IS_NE:
        strcpy(name, "!=");
        break;

    case IF:
        strcpy(name, "if");
        break;

    case ELSE:
        strcpy(name, "else");
        break;

    case NIL:
        strcpy(name, "nil");
        break;

    case VAR:
        strcpy(name, "var");
        break;

    case WHILE:
        strcpy(name, "while");
        break;

    case FUNC:
        strcpy(name, "func");
        break;

    case RET:
        strcpy(name, "RET");
        break;

    case CALL:
        strcpy(name, "call");
        break;

    case PARAM:
        strcpy(name, "param");
        break;
    

    case SoFOO:
        strcpy(name, "SoF");
        break;
    

    case EoFOO:
        strcpy(name, "EoF");
        break;
    }

    return strdup(name);
}
