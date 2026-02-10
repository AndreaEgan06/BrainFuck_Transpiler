#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CODE_SIZE 50000
#define INPUT_SIZE 20000
#define PARENTHESES_STACK_SIZE 512
#define BUFFER_SIZE 1024

/// @brief A tape cell's value is an unsigned 8-bit integer.
typedef unsigned char CellValue;

/// @brief Defined a tape cell for an infinitely long
///        tape in both directions.
typedef struct TapeCell {
    CellValue value;
    struct TapeCell *left;
    struct TapeCell *right;
} TapeCell;

/// @brief Creates a new tape cell not part of an existing tape.
/// @return the tape cell
static TapeCell *newTapeCell(void) {
    TapeCell *result = malloc(sizeof(TapeCell));
    result->value = 0;
    result->left = NULL;
    result->right = NULL;
    return result;
}

/// @brief Adds a new tape cell as the right neighbor of the given tape cell.
/// @param tape the given tape cell
static void addRightTapeCell(TapeCell *tape) {
    tape->right = newTapeCell();
    tape->right->left = tape;
}

/// @brief Adds a new tape cell as the left neighbor of the given tape cell.
/// @param tape the given tape cell
static void addLeftTapeCell(TapeCell *tape) {
    tape->left = newTapeCell();
    tape->left->right = tape;
}

/// @brief Destroys the entire tape that a cell is a part of.
/// @param tape to be destroyed
static void freeTape(TapeCell *tape) {
    TapeCell *leftCell = tape->left;
    TapeCell *current = leftCell;
    while (leftCell != NULL) {
        leftCell = leftCell->left;
        free(current);
        current = leftCell;
    }
    TapeCell *rightCell = tape->right;
    current = rightCell;
    while (rightCell != NULL) {
        rightCell = rightCell->right;
        free(current);
        current = rightCell;
    }
    free(tape);
}

/// The initial value for the tape index.
#define STARTING_TAPE_INDEX 0

/// The hypothetical index into the infinitely long tape.
/// A value of 0 corresponds to the initial tape cell.
/// Negative values correspond to tape cells to the left of the initial cell.
/// Positive values correspond to tape cells to the right of the initial cell.
static int theTapeIndex = STARTING_TAPE_INDEX;

/// The output buffer containing the results of having executed the BF code.
static char outputBuffer[BUFFER_SIZE] = {0};

/// The output buffer index to indicate how much of the buffer has been used up.
static unsigned int outputBufferIndex = 0;

/// @brief Processes the command from the code at the given codeIndex using the given tape and input stream.
/// @param code the BF code string
/// @param codeIndex the address of the index into the BF code string
/// @param tape the tape to execute the code on
/// @param input the input stream
void process(
        char *code,
        int *codeIndex,
        TapeCell **tape,
        CellValue *input) {
    // Static variables
    static int parenthesesStack[PARENTHESES_STACK_SIZE];
    static int parenthesesStackCounter = 0;
    static int inputPtr = 0;
    
    // Main switch statement
    switch (code[*codeIndex]) {
        case '+':
            (*tape)->value++;
            break;
        case '-':
            (*tape)->value--;
            break;
        case '>':
            if ((*tape)->right == NULL) {
                addRightTapeCell(*tape);
            }
            (*tape) = (*tape)->right;
            ++theTapeIndex;
            break;
        case '<':
            if ((*tape)->left == NULL) {
                addLeftTapeCell(*tape);
            }
            (*tape) = (*tape)->left;
            --theTapeIndex;
            break;
        case '.':
            outputBufferIndex += snprintf(&(outputBuffer[outputBufferIndex]), BUFFER_SIZE, "%d ", (*tape)->value);
            break;
        case ',':
            while (isdigit(input[inputPtr])) {
                ++inputPtr;
            }
            while (!isdigit(input[inputPtr]) && input[inputPtr] != '-') {
                ++inputPtr;
            }
            (*tape)->value = atoi((const char *) &input[inputPtr]);
            break;
        case '[':
            if ((*tape)->value != 0) {
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
            if ((*tape)->value != 0) {
                *codeIndex = parenthesesStack[parenthesesStackCounter] - 1;
            } else {
                --parenthesesStackCounter;
            }
            break;
    }
}

/// The length of the visible tape when printed.
#define TAPE_LENGTH 28

/// The distance to the middle of the stream of printed BF code.
#define MID_DISTANCE 60

/// @brief Prints where the BF code is in execution, and where the tape header is on the tape.
/// @param code the BF code
/// @param codePtr points to the current instruction in the code being executed
/// @param tape the current tape cell
void printState(char *code, int codePtr, TapeCell *tape) {
    // Useful to know where the tape was last printed from.
    static int prevTapeStart = STARTING_TAPE_INDEX;

    // The array containing the values that will be
    static CellValue tapeValues[TAPE_LENGTH];

    // Print the results so far
    printf("\nResults: ");
    puts(outputBuffer);

    // Print the code
    printf("Pos: %d\n", codePtr);
    int i = codePtr < MID_DISTANCE ? 0 : codePtr - MID_DISTANCE;
    int j = codePtr < MID_DISTANCE ? 2 * MID_DISTANCE : codePtr + MID_DISTANCE;
    for (; i < j && code[i] && code[i] != '\n'; ++i) {
        putchar(code[i]);
    }
    putchar('\n');
    for (i = 0; i <= (codePtr < MID_DISTANCE ? codePtr : MID_DISTANCE); ++i) {
        putchar(' ');
    }
    printf("^\n");

    // Update the prev tape start if necessary.
    printf("The tape index is: %d and the prevTapeStart is %d\n", theTapeIndex, prevTapeStart);
    if (theTapeIndex < prevTapeStart) {
        prevTapeStart = theTapeIndex;
    } else if (theTapeIndex >= prevTapeStart + TAPE_LENGTH - 1) {
        prevTapeStart = theTapeIndex - TAPE_LENGTH + 1;
    }

    // Fill in the tapeValues array with teh values to be printed.
    TapeCell *cell = tape;
    const int valuesIndex = theTapeIndex - prevTapeStart;
    for (i = valuesIndex; i >= 0; --i) {
        if (cell != NULL) {
            tapeValues[i] = cell->value;
            cell = cell->left;
        } else {
            tapeValues[i] = 0;
        }
    }
    cell = tape->right;
    for (i = valuesIndex + 1; i < TAPE_LENGTH; ++i) {
        if (cell != NULL) {
            tapeValues[i] = cell->value;
            cell = cell->right;
        } else {
            tapeValues[i] = 0;
        }
    }
  
    // Print the tape pointer and the tape.
    for (i = 0; i <= valuesIndex; ++i) {
        printf("    ");
    }
    printf(" v\n... ");
    for (i = 0; i < TAPE_LENGTH; ++i) {
        printf("%.03d ", tapeValues[i]);
    }
    printf("...\n");

    // Prompt the user for the next debug command.
    printf("CMD: ");
}

/// Processes the code until a certain condition is satisfied.
#define DO_UNTIL(condition)                                                   \
    do {                                                                      \
        process(code, &codePtr, &tape, input);                                \
    } while (++codePtr < CODE_SIZE && code[codePtr] && (condition));          \
    --codePtr

/// @brief Reades a file into a given buffer.
/// @param fileName the name of the file to read
/// @param buffer the buffer to read into
/// @return true if succesful
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

/// Wrapper macro around the readFile function that gives an error mesage
/// and crashes the program if there was an error.
#define READ_FILE(fileName, bufferName)                                       \
    if (!readFile((fileName), (char *) (bufferName))) {                       \
        printf("There was an error opening %s\n", (fileName));                \
        return EXIT_FAILURE;                                                  \
    }

/// @brief The main function :)
/// @param argc number of cmd line args (which must be at most 2)
/// @param argv cmd lines args
/// @return EXIT_SUCCESS if nothing goes wrong, otherwise EXIT_FAILURE
int main(int argc, char *argv[]) {
    // Get cmd line args (a potential breakpoint that if reached will
    // make the interpreter go into visual mode displaying the code
    // and the tape).
    if (argc > 2) {
        printf("Usage: ./interpreter [breakpoint]\n");
        return EXIT_FAILURE;
    }
    int breakpoint = argc < 2 ? CODE_SIZE : atoi(argv[1]);

    char code[CODE_SIZE];
    CellValue input[INPUT_SIZE];
    READ_FILE("code.txt", code);
    READ_FILE("input.txt", input);
    char buffer[BUFFER_SIZE];
    TapeCell *tape = newTapeCell();

    // Process the code up until the breakpoint.
    int codePtr = 0;
    DO_UNTIL(codePtr <= breakpoint);

    // If the code is not done being processed, then print the current state.
    bool finish = !code[codePtr + 1];
    if (!finish) {
        printState(code, codePtr, tape);
    }

    // Main "debug" loop for the visual mode of the interpreter.
    // Inputing a valid BF instruction will cause the code to execute up
    // until the first instruction seen of that kind (without executing it).
    // Inputing a lowercase 'f' will cause the interpreter to exit visual mode
    // and finish interpreting the rest of the code.
    // Inputing a number greater than the currently displayed position (Pos: #)
    // will set that number as the new breakpoint and execute up until that breakpoint.
    while (++codePtr < CODE_SIZE && code[codePtr]) {
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
                break;
            default:
                process(code, &codePtr, &tape, input);
                if (isdigit(buffer[0]) && buffer[0] != '0') {
                    breakpoint = atoi(buffer);
                    if (breakpoint > codePtr + 1) {
                        ++codePtr;
                        DO_UNTIL(codePtr <= breakpoint);
                    }
                }
        }
        if (finish) break;
        printState(code, codePtr, tape);
    }

    // Finish interpreting the code
    while (++codePtr < CODE_SIZE && code[codePtr]) {
        process(code, &codePtr, &tape, input);
    }

    printf("Results: ");
    puts(outputBuffer);
    printf("Done!\n");

    // Free the tape
    freeTape(tape);

    return EXIT_SUCCESS;
}
