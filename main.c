#include <stdio.h>
#include <stdlib.h>


// Define the longest string length of source code.
#define MAX_LEN 100000

/*
 * @param filePath : Path of file
 * @param str : An empty string array to store file
 *
 * @return The length of string
 */
int readFile(char *filePath, char *str) {
    FILE *fp = fopen(filePath, "r");
    if (fp == NULL) {
        printf("Fail to open the file!\nPlease confirm that the file exists in the current directory!\n");
        exit(0);
    }
    int strLen = fread(str, 1, MAX_LEN, fp);
    if (strLen >= MAX_LEN) {
        printf("This file is too large!\n");
        exit(1);
    }
    while (str[strLen - 1] == '\n') strLen--;
    if (str[strLen - 1] != '.') {
        printf("Error! Please end up with \".\"!\n");
        exit(2);
    }
    str[strLen] = '\0';
    fclose(fp);
    return strLen;
};


#define STATUS_NUM 14
#define ASCII_NUM 256
int Graph[STATUS_NUM][ASCII_NUM];

// Initialize the above Graph
void initGraph(int graph[STATUS_NUM][ASCII_NUM]) {
    for (int i = 0; i < STATUS_NUM; i++) {
        for (int j = 0; j < ASCII_NUM; j++) {
            Graph[i][j] = 0;
        }
    }
    for (int i = 'a'; i <= 'z'; i++) {
        graph[1][i] = 4;
        graph[4][i] = 4;
    }
    for (int i = 'A'; i <= 'Z'; i++) {
        graph[1][i] = 4;
        graph[4][i] = 4;
    }
    for (int i = '0'; i <= '9'; i++) {
        graph[1][i] = 2;
        graph[2][i] = 2;
        graph[3][i] = 3;
        graph[4][i] = 4;
    }
    graph[2]['.'] = 3;
    graph[4]['_'] = 4;
    graph[1]['='] = 5;
    graph[1]['+'] = 6;
    graph[1]['-'] = 7;
    graph[1]['*'] = 8;
    graph[1]['/'] = 9;
    graph[1]['('] = 10;
    graph[1][')'] = 11;
    graph[1][';'] = 12;
    graph[1]['.'] = 13;
};

typedef enum TokenType {
    INT, FLOAT, WRITE, ID, NUM, REAL, EQ, PLUS, MINUS, MUL, DIV, LP, RP, SEMI, END
} TokenType;

#define MAX_NAME 100
typedef struct Token {
    TokenType tokenType;
    char val[MAX_NAME];
} Token;

typedef Token;
#define MAX_TOKENS 100000
Token tokenStream[MAX_TOKENS];
int lineNum[MAX_TOKENS];
int tokenNum;
char *keyword[3];

void strcat(char *dest, char src);

int strcmp(char *p1, char *p2);

int tt_num(int x);

void Scanner(char *str, int strLen) {
    tokenNum = 0;
    int line = 1;
    int pt = 0;
    int status = 1;
    while (pt != strLen) {
        Token *token = (Token *) malloc(sizeof(Token));
        while (Graph[status][str[pt]] != 0) {
            strcat(token->val, str[pt]);
            status = Graph[status][str[pt]];
            pt++;
        }
        token->tokenType = tt_num(status);
        for (int i = 0; i < 3; i++) {
            if (strcmp(token->val, keyword[i]) == 0) {
                token->tokenType = i;
                break;
            }
        }
        lineNum[tokenNum] = line;
        tokenStream[tokenNum++] = *token;
        if (str[pt] == ' ') pt++;
        else if (str[pt] == '\n') {
            pt++;
            line++;
        }
        status = 1;
    }
}

int tt_num(int x) {
    if (x == 2 || x == 3) return x + 2;
    if (x == 4) return 3;
    if (x >= 5) return x + 1;
}

void strcat(char *dest, char src) {
    char *tmp = dest;
    while (*dest)
        dest++;
    *dest = src;
    dest++;
    *dest = '\0';
    dest = tmp;
}

int strcmp(char *p1, char *p2) {
    int i = 0;
    while (p1[i] == p2[i] && p1[i] != '\0') {
        i++;
    }
    int num;
    if (p1[i] == '\0' && p2[i] == '\0') {
        num = 0;
    } else {
        num = p1[i] - p2[i];
    }
    return num;
}

void initGlobal() {
    initGraph(Graph);
    keyword[0] = "int";
    keyword[1] = "float";
    keyword[2] = "write";
}

int token_count;

Token nextToken();

int isTokenEmpty();

#define MAX_EXP 100
Token stack_num[MAX_EXP];
Token stack_op[MAX_EXP];
int num_pt;
int op_pt;

void init_exp_stack() {
    num_pt = -1;
    op_pt = -1;
}

void push(Token stack[MAX_EXP], int *pt, Token tk) {
    (*pt)++;
    stack[*pt] = tk;
}

Token pop(Token stack[MAX_EXP], int *pt) {
    (*pt)--;
    return stack[*pt + 1];
}

Token top(Token stack[MAX_EXP], int *pt) {
    return stack[*pt];
}

int isEmpty(Token stack[MAX_EXP], int *pt) {
    return (*pt) == -1;
}

/*
 * STM -> DEF | ASSGIN | OUT
 * DEF -> TYPE id
 * OUT -> write ( id )
 * ASSIGN -> id = EXP
 * EXP -> TERM | TERM + EXP | TERM - EXP
 * TERM -> FACTOR | FACTOR * TERM | FACTOR / TERM
 * FACTOR -> id | NUMBER | (EXP)
 * NUMBER -> num | real
 */

void strcp(char *p1, char *p2) {
    int i = 0;
    while (p2[i] != '\0') {
        p1[i] = p2[i];
        i++;
    }
}

#define MAX_DEF 100
char *int_name_table[MAX_DEF];
int int_val_table[MAX_DEF];
int int_pt;

char *float_name_table[MAX_DEF];
float float_val_table[MAX_DEF];
int float_pt;

int find_int_table(char *name) {
    for (int i = 0; i < MAX_DEF; i++) {
        if (strcmp(int_name_table[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

int find_float_table(char *name) {
    for (int i = 0; i < MAX_DEF; i++) {
        if (strcmp(float_name_table[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

int priority[15];

void parser() {
    for (int i = 0; i < MAX_DEF; i++) {
        int_name_table[i] = malloc(100 * sizeof(char));
        float_name_table[i] = malloc(100 * sizeof(char));
    }
    priority[LP] = 0;
    priority[PLUS] = 1;
    priority[MINUS] = 1;
    priority[MUL] = 2;
    priority[DIV] = 2;
    int_pt = 0;
    float_pt = 0;
    token_count = 0;
    while (!isTokenEmpty()) {
        Token nt = nextToken();
        if (nt.tokenType == INT || nt.tokenType == FLOAT) {
            // DEF
            Token def_nt = nextToken();
            if (def_nt.tokenType != ID) {
                printf("Line %d: Error! This is not a definition statement!\n", lineNum[token_count - 1]);
                exit(1);
            }
            if (nt.tokenType == INT) {
                strcp(int_name_table[int_pt++], def_nt.val);
            } else if (nt.tokenType == FLOAT) {
                strcp(float_name_table[float_pt++], def_nt.val);
            }
            Token semi_end = nextToken();
            if (semi_end.tokenType != SEMI && !isTokenEmpty()) {
                printf("Line %d: Error! You should end up with \';\'!\n", lineNum[token_count - 1]);
                exit(1);
            } else if (isTokenEmpty() && semi_end.tokenType != END) {
                printf("Line %d: Error! You should end up with \'.\'!\n", lineNum[token_count - 1]);
                exit(1);
            }
        } else if (nt.tokenType == ID) {
            int find_int = find_int_table(nt.val);
            int find_float = find_float_table(nt.val);
            init_exp_stack();
            // ASSIGN
            if (nextToken().tokenType != EQ) {
                printf("Line %d: Error! Missing \'=\'!\n", lineNum[token_count - 1]);
                exit(1);
            }
            nt = nextToken();
            while (nt.tokenType != SEMI && nt.tokenType != END && !isTokenEmpty()) {
                if (nt.tokenType == REAL || nt.tokenType == NUM || nt.tokenType == ID) {
                    push(stack_num, &num_pt, nt);
                } else if (nt.tokenType == PLUS || nt.tokenType == MINUS || nt.tokenType == MUL ||
                           nt.tokenType == DIV) {
                    while (!isEmpty(stack_op, &op_pt) &&
                           priority[top(stack_op, &op_pt).tokenType] >= priority[nt.tokenType]) {
                        push(stack_num, &num_pt, pop(stack_op, &op_pt));
                    }
                    push(stack_op, &op_pt, nt);
                } else if (nt.tokenType == LP) {
                    push(stack_op, &op_pt, nt);
                } else if (nt.tokenType == RP) {
                    while (!isEmpty(stack_op, &op_pt) && top(stack_op, &op_pt).tokenType != LP) {
                        push(stack_num, &num_pt, pop(stack_op, &op_pt));
                    }
                    pop(stack_op, &op_pt);
                }
                nt = nextToken();
            }
            while (!isEmpty(stack_op, &op_pt)) {
                push(stack_num, &num_pt, pop(stack_op, &op_pt));
            }
            for (int i = 0; i <= num_pt; i++) {
                if (stack_num[i].tokenType == NUM || stack_num[i].tokenType == REAL) {
                    push(stack_op, &op_pt, stack_num[i]);
                } else if (stack_num[i].tokenType == ID) {
                    if (find_int_table(stack_num[i].val) != -1) {
                        stack_num[i].tokenType = NUM;
                        sprintf(stack_num[i].val, "%d", int_val_table[find_int_table(stack_num[i].val)]);
                    } else if (find_float_table(stack_num[i].val) != -1) {
                        stack_num[i].tokenType = REAL;
                        sprintf(stack_num[i].val, "%f", float_val_table[find_float_table(stack_num[i].val)]);
                    } else {
                        printf("Error(Line %d): undefined identifier.\n", lineNum[token_count - 1]);
                        exit(1);
                    }
                    push(stack_op, &op_pt, stack_num[i]);
                } else {
                    Token tk1 = pop(stack_op, &op_pt);
                    Token tk2 = pop(stack_op, &op_pt);
                    if (tk1.tokenType == NUM && tk2.tokenType == NUM) {
                        if (stack_num[i].tokenType == PLUS) {
                            sprintf(tk1.val, "%d", atoi(tk1.val) + atoi(tk2.val));
                        } else if (stack_num[i].tokenType == MINUS) {
                            sprintf(tk1.val, "%d", atoi(tk1.val) - atoi(tk2.val));
                        } else if (stack_num[i].tokenType == MUL) {
                            sprintf(tk1.val, "%d", atoi(tk1.val) * atoi(tk2.val));
                        } else {
                            sprintf(tk1.val, "%d", atoi(tk2.val) / atoi(tk1.val));
                        }
                    } else {
                        tk1.tokenType = REAL;
                        if (stack_num[i].tokenType == PLUS) {
                            sprintf(tk1.val, "%f", atof(tk1.val) + atof(tk2.val));
                        } else if (stack_num[i].tokenType == MINUS) {
                            sprintf(tk1.val, "%f", atof(tk1.val) - atof(tk2.val));
                        } else if (stack_num[i].tokenType == MUL) {
                            sprintf(tk1.val, "%f", atof(tk1.val) * atof(tk2.val));
                        } else {
                            sprintf(tk1.val, "%f", atof(tk2.val) / atof(tk1.val));
                        }
                    }
                    push(stack_op, &op_pt, tk1);
                }
            }

            if (find_int != -1) {
                int_val_table[find_int] = (int) atof(pop(stack_op, &op_pt).val);
            } else if (find_float != -1) {
                float_val_table[find_float] = (float) atof(pop(stack_op, &op_pt).val);
            } else {
                printf("Error(Line %d): undefined identifier.\n", lineNum[token_count - 1]);
                exit(1);
            }

            Token semi_end = nt;
            if (semi_end.tokenType != SEMI && !isTokenEmpty()) {
                printf("Line %d: Error! You should end up with \';\'!\n", lineNum[token_count - 1]);
                exit(1);
            } else if (isTokenEmpty() && semi_end.tokenType != END) {
                printf("Line %d: Error! You should end up with \'.\'!\n", lineNum[token_count - 1]);
                exit(1);
            }

        } else if (nt.tokenType == WRITE) {
            // OUT
            if (nextToken().tokenType != LP) {
                printf("Line %d: Error! Missing \'(\'!\n", lineNum[token_count - 1]);
                exit(1);
            }
            Token ID_tk = nextToken();
            if (ID_tk.tokenType != ID) {
                printf("Line %d: Error! Missing variable!\n", lineNum[token_count - 1]);
                exit(1);
            }
            if (nextToken().tokenType != RP) {
                printf("Line %d: Error! Missing \')\'!\n", lineNum[token_count - 1]);
                exit(1);
            }
            Token semi_end = nextToken();
            if (semi_end.tokenType != SEMI && !isTokenEmpty()) {
                printf("Line %d: Error! You should end up with \';\'!\n", lineNum[token_count - 1]);
                exit(1);
            } else if (isTokenEmpty() && semi_end.tokenType != END) {
                printf("Line %d: Error! You should end up with \'.\'!\n", lineNum[token_count - 1]);
                exit(1);
            }
            if (find_int_table(ID_tk.val) != -1) {
                printf("%d\n", int_val_table[find_int_table(ID_tk.val)]);
            } else if (find_float_table(ID_tk.val) != -1) {
                printf("%f\n", float_val_table[find_float_table(ID_tk.val)]);
            }
        }
    }

}

Token nextToken() {
    return tokenStream[token_count++];
}

int isTokenEmpty() {
    return token_count >= tokenNum;
}

int main(int argc, char *const argv[]) {
    initGlobal();
    char *str = malloc(MAX_LEN * sizeof(char));
    int strLen = readFile(argv[1], str);
    Scanner(str, strLen);
    parser();
    return 0;
}
