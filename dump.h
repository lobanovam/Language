#ifndef DUMP_H_INCLUDED
#define DUMP_H_INCLUDED

#define dumpToken(str...)                                                 \
    fprintf(DumpToken, str);                                               \
    fflush(DumpToken)

#ifndef DUMP_TOKEN
    #define DUMP_TOKEN "token.dot"
#endif

const char png[5] = ".png";
static int tokenImageNum = 1;

FILE *openDump();
char* IntToString(int num);

FILE* openDump(const char* name) {
    FILE *Dump = fopen(name, "w");
    if (Dump == NULL)
        printf("ERROR, CAN'T OPEN DUMP FILE\n");
    return Dump;
}

char* IntToString(int num) {
    char* str = (char*)(calloc(10, sizeof(char)));
    int tmp = num;
    int decimal = 1;
    int digits =  0;
    while (tmp > 0) {
        tmp/=10;
        digits++;
        decimal*=10;
    }
    decimal/=10;
    for (int i = 0; i < digits; i++) {
        str[i] = num / decimal + '0';
        num = num%decimal;
        decimal/=10;
    }
    return str;
}

#endif // DUMP_H_INCLUDED
