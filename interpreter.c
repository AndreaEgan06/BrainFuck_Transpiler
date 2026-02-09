#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CODE_SIZE 50000
#define TAPE_SIZE 30000
#define INPUT_SIZE 20000
#define PARENTHESES_STACK_SIZE 512
#define BUFFER_SIZE 256
#define MID_DISTANCE 60
#define TAPE_END 30
#define TAPE_START 0

void process(
        char *code,
        int *codeIndex,
        unsigned char *tape,
        int *index,
        char *input) {
    // Static variables
    static int parenthesesStack[PARENTHESES_STACK_SIZE];
    static int parenthesesStackCounter = 0;
    static int inputPtr = 0;

    // Main switch statement
    switch (code[*codeIndex]) {
        case '+':
            tape[*index]++;
            break;
        case '-':
            tape[*index]--;
            break;
        case '>':
            ++(*index);
            break;
        case '<':
            --(*index);
            break;
        case '.':
            printf("%d ", tape[*index]);
            break;
        case ',':
            while (isdigit(input[inputPtr])) {
                ++inputPtr;
            }
            while (!isdigit(input[inputPtr]) && input[inputPtr] != '-') {
                ++inputPtr;
            }
            tape[*index] = atoi(&input[inputPtr]);
            break;
        case '[':
            if (tape[*index]) {
                if (parenthesesStack[parenthesesStackCounter] != *codeIndex) {
                    parenthesesStack[++parenthesesStackCounter] = *codeIndex;
                }
            } else {
                if (parenthesesStack[parenthesesStackCounter] == *codeIndex) {
                    --parenthesesStackCounter;
                }
                int parenCounter = 0;
                while (code[++(*codeIndex)] != ']' || parenCounter > 0) {
                    if (code[*codeIndex] == '[') {
                        ++parenCounter;
                    } else if (code[*codeIndex] == ']') {
                        --parenCounter;
                    }
                }
            }
            break;
        case ']':
            if (tape[*index]) {
                *codeIndex = parenthesesStack[parenthesesStackCounter] - 1;
            } else {
                --parenthesesStackCounter;
            }
            break;
    }
}

void printCode(char *code, int codePtr, unsigned char *tape, int index) {
    putchar('\n');
    int i = codePtr < MID_DISTANCE ? 0 : codePtr - MID_DISTANCE;
    int j = codePtr < MID_DISTANCE ? 2 * MID_DISTANCE : codePtr + MID_DISTANCE;
    for (; i < j && code[i] && code[i] != '\n'; ++i) {
        putchar(code[i]);
    }
    putchar('\n');
    for (i = 0; i <= (codePtr < MID_DISTANCE ? codePtr : MID_DISTANCE); ++i) {
        putchar(' ');
    }
    printf("^\nPos: %d\n ", codePtr);
    for (i = 0; i <= index; ++i) {
        printf(i == index ? "v" : "    ");
        if (i == index) break;
    }
    putchar('\n');
    for (i = TAPE_START; i < TAPE_END; ++i) {
        printf("%.03d ", tape[i]);
    }
    printf("\nCMD: ");
}

#define DO_UNTIL(condition)                                                   \
    do {                                                                      \
        process(code, &codePtr, tape, &index, input);                         \
    } while (++codePtr < CODE_SIZE && code[codePtr] && (condition));          \
    --codePtr

bool readFile(char *fileName, char *buffer) {
    bool result = true;
    FILE *filePtr = fopen(fileName, "r");
    if (!fgets(buffer, CODE_SIZE, filePtr)) {
        result = false;
    }
    if (filePtr) {
        fclose(filePtr);
    }
    return result;
}

#define READ_FILE(fileName, bufferName)                                       \
    if (!readFile((fileName), (bufferName))) {                                \
        printf("There was an error opening %s\n", (fileName));                \
        return -1;                                                            \
    }

int main(int argc, char *argv[]) {
    if (argc > 2) {
        printf("Usage: ./interpreter code_file [breakpoint]\n");
        return -1;
    }
    int breakpoint = argc < 2 ? CODE_SIZE : atoi(argv[1]);
    char code[CODE_SIZE], input[INPUT_SIZE];
    READ_FILE("code.txt", code);
    READ_FILE("input.txt", input);
    char buffer[BUFFER_SIZE];
    unsigned char tape[TAPE_SIZE] = {0};
    int index = 0, codePtr = 0;
    DO_UNTIL(codePtr <= breakpoint);
    bool finish = !code[codePtr + 1];
    if (!finish) {
        printCode(code, codePtr, tape, index);
    }
    while (!finish && ++codePtr < CODE_SIZE && code[codePtr]) {
        fgets(buffer, BUFFER_SIZE, stdin);
        switch (buffer[0]) {
            case '+':
            case '-':
            case '>':
            case '<':
            case '[':
            case ']':
            case '.':
            case ',':
                DO_UNTIL(code[codePtr] != buffer[0]);
                break;
            case 'f':
                finish = true;
            default:
                process(code, &codePtr, tape, &index, input);
                if (isdigit(buffer[0])) {
                    breakpoint = atoi(buffer);
                    if (breakpoint > codePtr++)
                    DO_UNTIL(codePtr <= breakpoint);
                }
        }
        printCode(code, codePtr, tape, index);
    }
    while (++codePtr < CODE_SIZE && code[codePtr]) {
        process(code, &codePtr, tape, &index, input);
    }
    printf("\nDone!\n");
    return 0;
}
