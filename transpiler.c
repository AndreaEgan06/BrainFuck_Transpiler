#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DA_INIT_CAP 256
#define da_append(da, item)                                                    \
  do {                                                                         \
    if ((da)->count >= (da)->capacity) {                                       \
      (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity * 2; \
      (da)->items =                                                            \
          realloc((da)->items, (da)->capacity * sizeof(*(da)->items));         \
      assert((da)->items != NULL && "Could not append to dynamic array");      \
    }                                                                          \
    (da)->items[(da)->count++] = (item);                                       \
  } while (0)

#define da_free(da)                                                            \
  do {                                                                         \
    if ((da).count > 0)                                                        \
      free((da).items);                                                        \
    (da).count = 0;                                                            \
    (da).capacity = 0;                                                         \
  } while (0)

#define DA_DECLARATION                                                         \
  items;                                                                       \
  size_t count;                                                                \
  size_t capacity;

#define return_(defer, value)                                                  \
  do {                                                                         \
    result = (value);                                                          \
    goto defer;                                                                \
  } while (0)

typedef enum {
  EOL = 10,
  VAR_DEC,
  PROC_NAME,
  LABEL,
  STR,
  NUM,
  OPEN_BRACKET,
  CLOSE_BRACKET,
  FSET,
  FINC,
  FDEC,
  FADD,
  FSUB,
  FMUL,
  FDIVMOD,
  FDIV,
  FMOD,
  FCMP,
  FA2B,
  FB2A,
  FLSET,
  FLGET,
  IFEQ,
  IFNEQ,
  WNEQ,
  PROC,
  END,
  CALL,
  READ,
  MSG,
  DOUBLE,
  SQUARE,
  DUPLICATE
} Token;

typedef struct {
  Token *DA_DECLARATION
} TokenList;

typedef struct {
  char *DA_DECLARATION size_t index;
} Data;

typedef struct {
  int *DA_DECLARATION size_t index;
} StringLengths;

#define da_cur(da) (da)->items[(da)->index]

#define BUFFER_SIZE 8

void printToken(Token token) {
  switch (token) {
  case EOL:
    printf("EOL");
    break;
  case VAR_DEC:
    printf("VAR_DEC");
    break;
  case PROC_NAME:
    printf("PROC_NAME");
    break;
  case LABEL:
    printf("LABEL");
    break;
  case STR:
    printf("STR");
    break;
  case NUM:
    printf("NUM");
    break;
  case OPEN_BRACKET:
    printf("OPEN_BRACKET");
    break;
  case CLOSE_BRACKET:
    printf("CLOSE_BRACKET");
    break;
  case FSET:
    printf("FSET");
    break;
  case FINC:
    printf("FINC");
    break;
  case FDEC:
    printf("FDEC");
    break;
  case FADD:
    printf("FADD");
    break;
  case FSUB:
    printf("FSUB");
    break;
  case FMUL:
    printf("FMUL");
    break;
  case FDIVMOD:
    printf("FDIVMOD");
    break;
  case FDIV:
    printf("FDIV");
    break;
  case FMOD:
    printf("FMOD");
    break;
  case FCMP:
    printf("FCMP");
    break;
  case FA2B:
    printf("FA2B");
    break;
  case FB2A:
    printf("FB2A");
    break;
  case FLSET:
    printf("FLSET");
    break;
  case FLGET:
    printf("FLGET");
    break;
  case IFEQ:
    printf("IFEQ");
    break;
  case IFNEQ:
    printf("IFNEQ");
    break;
  case WNEQ:
    printf("WNEQ");
    break;
  case PROC:
    printf("PROC");
    break;
  case END:
    printf("END");
    break;
  case CALL:
    printf("CALL");
    break;
  case READ:
    printf("READ");
    break;
  case MSG:
    printf("MSG");
    break;
  case DOUBLE:
    printf("DOUBLE");
    break;
  case SQUARE:
    printf("SQUARE");
    break;
  case DUPLICATE:
    printf("DUPLICATE");
    break;
  }
}

#define IS_COMMENTPREFIX                                                       \
  ((code[i] == '/' && code[i + 1] == '/') ||                                   \
   (code[i] == '-' && code[i + 1] == '-') || code[i] == '#')
#define IS_VARPREFIX (code[i] == '$' || code[i] == '_' || isalpha(code[i]))
#define IS_VARSUFFIX (IS_VARPREFIX || isdigit(code[i]))
#define CHECK_SPECIAL_CHAR(charToCheck)                                        \
  do {                                                                         \
    switch ((charToCheck)) {                                                   \
    case '\\':                                                                 \
    case '\'':                                                                 \
    case '\"':                                                                 \
      da_append(data, (charToCheck));                                          \
      break;                                                                   \
    case 'n':                                                                  \
      da_append(data, '\n');                                                   \
      break;                                                                   \
    case 'r':                                                                  \
      da_append(data, '\r');                                                   \
      break;                                                                   \
    case 't':                                                                  \
      da_append(data, '\t');                                                   \
      break;                                                                   \
    default:                                                                   \
      return -1;                                                               \
    }                                                                          \
  } while (0)

int tokenize(TokenList *tokens, Data *data, StringLengths *symbolLengths,
             const char *code) {
  char buffer[BUFFER_SIZE] = {0};
  int bufIndex = 0;
  bool foundStatement = false;
  for (int i = 0; code[i] > 0; ++i) {
    if (code[i] == '\n') {
      da_append(tokens, EOL);
      foundStatement = false;
      continue;
    }
    if (!foundStatement) {
      while (isspace(code[i]))
        ++i;
      while (!isspace(code[i])) {
        if IS_COMMENTPREFIX
          break;
        if (!isalnum(code[i]) || bufIndex == BUFFER_SIZE - 1) {
          buffer[bufIndex] = '\0';
          break;
        }
        buffer[bufIndex++] = code[i++];
      }
      bool isBlank = true;
      for (int j = 0; j <= bufIndex; ++j)
        isBlank = isBlank && isspace(buffer[j]);
      if (isBlank || bufIndex == 0) {
        while (code[i + 1] != '\n' && code[i + 1] > 0)
          ++i;
        continue;
      }
      --i;
      for (int j = 0; j < BUFFER_SIZE; ++j)
        buffer[j] = tolower(buffer[j]);
      if (strcmp(buffer, "set") == 0) {
        da_append(tokens, FSET);
      } else if (strcmp(buffer, "var") == 0) {
        da_append(tokens, VAR_DEC);
      } else if (strcmp(buffer, "inc") == 0) {
        da_append(tokens, FINC);
      } else if (strcmp(buffer, "dec") == 0) {
        da_append(tokens, FDEC);
      } else if (strcmp(buffer, "add") == 0) {
        da_append(tokens, FADD);
      } else if (strcmp(buffer, "sub") == 0) {
        da_append(tokens, FSUB);
      } else if (strcmp(buffer, "mul") == 0) {
        da_append(tokens, FMUL);
      } else if (strcmp(buffer, "divmod") == 0) {
        da_append(tokens, FDIVMOD);
      } else if (strcmp(buffer, "div") == 0) {
        da_append(tokens, FDIV);
      } else if (strcmp(buffer, "mod") == 0) {
        da_append(tokens, FMOD);
      } else if (strcmp(buffer, "cmp") == 0) {
        da_append(tokens, FCMP);
      } else if (strcmp(buffer, "a2b") == 0) {
        da_append(tokens, FA2B);
      } else if (strcmp(buffer, "b2a") == 0) {
        da_append(tokens, FB2A);
      } else if (strcmp(buffer, "lset") == 0) {
        da_append(tokens, FLSET);
      } else if (strcmp(buffer, "lget") == 0) {
        da_append(tokens, FLGET);
      } else if (strcmp(buffer, "ifeq") == 0) {
        da_append(tokens, IFEQ);
      } else if (strcmp(buffer, "ifneq") == 0) {
        da_append(tokens, IFNEQ);
      } else if (strcmp(buffer, "wneq") == 0) {
        da_append(tokens, WNEQ);
      } else if (strcmp(buffer, "proc") == 0 || strcmp(buffer, "call") == 0) {
        int nameLength = 0;
        da_append(tokens, buffer[0] == 'p' ? PROC : CALL);
        while (isspace(code[++i]))
          ;
        while (!isspace(code[i])) {
          if (isalpha(code[i]) || code[i] == '_' || code[i] == '$' ||
              (isdigit(code[i]) && isspace(code[i + 1]))) {
            da_append(data, code[i]);
            ++nameLength;
          } else
            return -1; // Invalid proc name
          ++i;
        }
        --i;
        da_append(tokens, PROC_NAME);
        da_append(symbolLengths, nameLength);
      } else if (strcmp(buffer, "end") == 0) {
        da_append(tokens, END);
      } else if (strcmp(buffer, "read") == 0) {
        da_append(tokens, READ);
      } else if (strcmp(buffer, "msg") == 0) {
        da_append(tokens, MSG);
      } else if (strcmp(buffer, "rem") == 0) {
        while (code[i + 1] != '\n' && code[i + 1] > 0)
          ++i;
      } else
        return -1; // Invalid statement
      for (int j = 0; j < BUFFER_SIZE; ++j)
        buffer[j] = '\0';
      bufIndex = 0;
      foundStatement = true;
      continue;
    }
    if IS_COMMENTPREFIX {
      while (code[i + 1] != '\n' && code[i + 1] > 0)
        ++i;
      continue;
    }
    size_t stringSize = 0;
    switch (code[i]) {
    case '[':
      da_append(tokens, OPEN_BRACKET);
      break;
    case ']':
      if (code[++i] != ' ' && !IS_COMMENTPREFIX && code[i] != '\n' &&
          code[i] > 0)
        return -1; // Invalid declaration of list
      --i;
      da_append(tokens, CLOSE_BRACKET);
      break;
    case '\'':
      if (code[++i] == '\\') {
        CHECK_SPECIAL_CHAR(code[++i]);
      } else
        da_append(data, code[i]);
      if (code[++i] != '\'')
        return -1; // Invalid char literal
      da_append(tokens, NUM);
      da_append(symbolLengths, 1);
      break;
    case '\"':
      while (code[++i] != '\"' && code[i] != '\n') {
        ++stringSize;
        if (code[i] == '\\') {
          CHECK_SPECIAL_CHAR(code[++i]);
        } else
          da_append(data, code[i]);
      }
      if (code[i] == '\n')
        return -1; // Invalid string literal
      da_append(tokens, STR);
      da_append(symbolLengths, stringSize);
      break;
    case ' ':
      break;
    default:
      if (IS_VARPREFIX) {
        size_t length = 0;
        do {
          da_append(data, toupper(code[i++]));
          ++length;
        } while IS_VARSUFFIX;
        if (code[i] != ' ' && code[i] != '\n' && code[i] != '[' &&
            code[i] != '\"' && !IS_COMMENTPREFIX && code[i] > 0)
          return -1; // Invalid variable name
        da_append(tokens, LABEL);
        da_append(symbolLengths, length);
      } else if (isdigit(code[i]) || (code[i] == '-' && isdigit(code[i + 1]))) {
        da_append(tokens, NUM);
        da_append(data, atoi(&code[i]) % 256);
        da_append(symbolLengths, 1);
        if (code[i] == '-')
          ++i;
        while (isdigit(code[i]))
          ++i;
        if (code[i] != ']' && code[i] != ' ' && !IS_COMMENTPREFIX &&
            code[i] != '\n' && code[i] > 0)
          return -1; // Invalid number literal
      } else
        return -1; // Invalid variable name or number literal
      --i;
    }
  }
  return 0;
}

typedef enum { NUMBER, STRING, VARIABLE, LIST, PROCEDURE, INDEX } ArgType;

typedef struct {
  char *start;
  size_t length;
} StringView;

bool strViewCmp(const StringView one, const StringView two) {
  if (one.length != two.length)
    return false;
  for (size_t i = 0; i < one.length; ++i)
    if (one.start[i] != two.start[i])
      return false;
  return true;
}

typedef struct {
  ArgType type;
  union {
    long index;
    struct {
      union {
        StringView name;
        StringView string;
      } alpha;
      union {
        unsigned char ber;
        unsigned char size;
      } num;
    } data;
  } val;
} Argument;

typedef struct {
  Argument *DA_DECLARATION
} ArgList;

typedef struct Statement {
  Token type;
  ArgList args;
  struct Statement *next;
  struct Statement *jump;
  struct Statement *prev;
} Statement;

typedef struct {
  Statement *DA_DECLARATION
} StatementList;

int makeStatement(StatementList *statements, TokenList tokens, size_t *i,
                  Data *restrict data, StringLengths *restrict symbolLengths) {
  Statement statement = {0};
  switch (tokens.items[*i]) {
  case VAR_DEC:
  case FSET:
  case FINC:
  case FDEC:
  case FADD:
  case FSUB:
  case FMUL:
  case FDIVMOD:
  case FDIV:
  case FMOD:
  case FCMP:
  case FA2B:
  case FB2A:
  case FLSET:
  case FLGET:
  case IFEQ:
  case IFNEQ:
  case WNEQ:
  case PROC:
  case END:
  case CALL:
  case READ:
  case MSG:
    statement.type = tokens.items[*i];
    break;
  default:
    return -1; // Unrecognized or invalid token
  }
  while (++(*i) < tokens.count && tokens.items[*i] != EOL) {
    Argument arg = {0};
    switch (tokens.items[*i]) {
    case PROC_NAME:
      arg.type = PROCEDURE;
      arg.val.data.alpha.name =
          (StringView){&da_cur(data), da_cur(symbolLengths)};
      break;
    case LABEL:
      if (tokens.items[*i + 1] == OPEN_BRACKET && tokens.items[*i + 2] == NUM &&
          tokens.items[*i + 3] == CLOSE_BRACKET) {
        arg.type = LIST;
        arg.val.data.num.size = tokens.items[*i + 2];
        *i += 3;
      } else
        arg.type = VARIABLE;
      arg.val.data.alpha.name =
          (StringView){&da_cur(data), da_cur(symbolLengths)};
      arg.val.data.num.size = 1;
      break;
    case STR:
      arg.type = STRING;
      arg.val.data.alpha.string =
          (StringView){da_cur(symbolLengths) == 0 ? NULL : &da_cur(data),
                       da_cur(symbolLengths)};
      break;
    case NUM:
      arg.type = NUMBER;
      arg.val.data.num.ber = da_cur(data);
      break;
    default:
      return -1; // Unrecognized or invalid token
    }
    data->index += symbolLengths->items[(symbolLengths->index)++];
    da_append(&statement.args, arg);
  }
  da_append(statements, statement);
  return 0;
}

int makeConditionalBlock(StatementList *stmtList, size_t *index) {
  Statement *ogStmt = &stmtList->items[*index];
  ArgList ogArgs = ogStmt->args;
  if (ogArgs.count != 2)
    return -1; // Incorrect number of arguments for conditional block
  for (int i = 0; i < 2; ++i)
    if (ogArgs.items[i].type != VARIABLE && ogArgs.items[i].type != NUMBER)
      return -1; // Incorrect argument types for conditional block
  while (++(*index) < stmtList->count) {
    Statement *stmt = &stmtList->items[*index];
    if (*index + 1 < stmtList->count)
      stmt->next = &stmtList->items[*index + 1];
    if (*index > 0)
      stmt->prev = &stmtList->items[*index - 1];
    if (stmt->type == END) {
      ogStmt->jump = stmt;
      if (ogStmt->type == WNEQ)
        stmt->jump = ogStmt;
      return 0;
    }
    switch (stmt->type) {
    case IFEQ:
    case IFNEQ:
    case WNEQ:
      if (makeConditionalBlock(stmtList, index) == 0)
        break; // In case of an invalid conditional block, it fallsthrough
    case PROC:
      return -1; // Invalid proc declaration inside a conditional block
    default:
      break;
    }
  }
  return -1; // Missing end
}

typedef struct {
  StringView *DA_DECLARATION
} ParamList;

typedef struct {
  StringView name;
  ParamList parameters;
  Statement *start;
} Procedure;

typedef struct {
  Procedure *DA_DECLARATION
} ProcedureList;

int makeProcedure(ProcedureList *procs, StatementList *stmtList,
                  size_t *index) {
  Procedure proc = {0};
  Statement *stmt = &stmtList->items[*index];
  Argument *args = stmt->args.items;
  if (stmt->args.count < 1 || args[0].type != PROCEDURE)
    return -1; // Procedure declaration missing name
  proc.name = args[0].val.data.alpha.name;
  for (size_t i = 1; i < stmt->args.count; ++i) {
    if (args[i].type != VARIABLE)
      return -1; // Invalid procedure declaration
    for (size_t j = 0; j < proc.parameters.count; ++j)
      if (strViewCmp(args[i].val.data.alpha.name, proc.parameters.items[j]))
        return -1; // Duplicate parameter names in procedure declaration
    da_append(&proc.parameters, args[i].val.data.alpha.name);
  }

  if (*index >= stmtList->count - 1)
    return -1; // Invalid procedure

  stmt = &stmtList->items[++(*index)];
  if (stmt->type == END) {
    da_append(procs, proc);
    return 0;
  }

  proc.start = stmt;

  do {
    stmt = &stmtList->items[*index];
    if (*index < stmtList->count - 1)
      stmt->next = &stmtList->items[++(*index)];
    if (stmt != proc.start)
      stmt->prev = &stmtList->items[*index - 2];
    switch (stmt->type) {
    case IFEQ:
    case IFNEQ:
    case WNEQ:
      if (!makeConditionalBlock(stmtList, index))
        break; // In case of an invalid conditional block, it fallsthrough
    case PROC:
    case VAR_DEC:
      return -1; // Invalid procedure or variable declaration inside a procedure
    default:
      break;
    }
    if (stmtList->items[(*index)].type == END) {
      da_append(procs, proc);
      return 0;
    }
  } while (*index < stmtList->count);
  return -1; // Missing end
}

int buildAST(StatementList *stmts, ProcedureList *procList, Statement **stmt) {
  bool needToFindStartingPoint = false;
  size_t prev, i;
  for (i = 0; i < stmts->count - 1; ++i) {
    bool wasProc = false;
    if (stmts->items[i].type != PROC) {
      stmts->items[i].next = &stmts->items[i + 1];
      if (i > 0)
        stmts->items[i].prev = &stmts->items[i - 1];
    }
    switch (stmts->items[i].type) {
    case IFEQ:
    case IFNEQ:
    case WNEQ:
      if (makeConditionalBlock(stmts, &i))
        return -1; // Invalid conditional block
      break;
    case PROC:
      needToFindStartingPoint = i == 0 || needToFindStartingPoint;
      if (makeProcedure(procList, stmts, &i))
        return -1; // Invalid procedure
      if (!needToFindStartingPoint)
        stmts->items[prev].next =
            i < stmts->count - 1 ? &stmts->items[i + 1] : NULL;
      wasProc = true;
      break;
    case END:
      return -1; // Invalid end before starting a block
    default:
      break;
    }
    prev = wasProc ? prev : i;
  }
  stmts->items[i].prev = &stmts->items[i - 1];
  *stmt = &stmts->items[0];
  if (needToFindStartingPoint) {
    size_t i = 0;
    bool notFound = false;
    while (stmts->items[i].type == PROC && !notFound)
      while (stmts->items[i++].type != END)
        if (i >= stmts->count) {
          notFound = true;
          break;
        }
    *stmt = notFound ? NULL : &stmts->items[i];
  }
  return 0;
}

typedef struct {
  unsigned char *DA_DECLARATION
} Values;

typedef struct {
  StringView name;
  ArgType type;
  long index;
  bool known;
  bool reads;
  Values values;
  bool tbd;
} Reg;

typedef struct {
  Reg *DA_DECLARATION long index;
} Memory;

typedef struct {
  Procedure **DA_DECLARATION;
} CallStack;

#define CHECK_ARG_COUNT(num)                                                   \
  if (stmt->args.count != (num))                                               \
  return -1

void printSV(StringView sv) {
  for (size_t i = 0; i < sv.length; ++i)
    printf("%c", sv.start[i]);
}
bool findInMemory(Memory *memory, Argument *var, Reg **reg, ArgType type) {
  if (var->type == type) {
    for (size_t i = 0; i < memory->count; ++i) {
      if (strViewCmp(memory->items[i].name, var->val.data.alpha.name)) {
        *reg = &memory->items[i];
        return true;
      }
    }
  }
  return false;
}

#define VARVAL(register) (register)->values.items[0]
#define NUMVAL(argIndex) args[(argIndex)].val.data.num.ber

#define REPLACE_WITH(argType, value, argIndex)                                 \
  do {                                                                         \
    if ((argType) != INDEX && (argType) != NUMBER)                             \
      return -1;                                                               \
    args[(argIndex)].type = (argType);                                         \
    if ((argType) == INDEX) {                                                  \
      args[(argIndex)].val.index = (value);                                    \
    } else                                                                     \
      NUMVAL((argIndex)) = (value);                                            \
  } while (0)

#define REMOVE_STMT                                                            \
  if (stmt->prev)                                                              \
    stmt->prev->next = stmt->next;                                             \
  if (stmt->next)                                                              \
    stmt->next->prev = stmt->prev;                                             \
  if (stmt == *start)                                                          \
    *start = stmt->next;                                                       \
  break

#define TYPE stmt->type

int checkAST(StatementList *stmts, Statement **start, ProcedureList *procList,
             Memory *memory, CallStack *callStack) {
  Statement *stmt = *start;
  while (stmt) {
    Argument *args = stmt->args.items;
    Reg *dest[3] = {0};
    Reg *orig[3] = {0};
    bool isKnown[3] = {0};
    unsigned char val[3] = {0};
    switch (TYPE) {
    case VAR_DEC:
      for (size_t i = 0; i < stmt->args.count; ++i) {
        Argument *arg = &args[i];
        if (findInMemory(memory, arg, dest, VARIABLE) ||
            findInMemory(memory, arg, dest, LIST))
          assert(0 && "Duplicate var names");
        Values values = {0};
        for (size_t count = 0; count < arg->val.data.num.size; ++count)
          da_append(&values, 0);
        da_append(memory, ((Reg){
                              .name = arg->val.data.alpha.name,
                              .type = arg->type,
                              .index = memory->index,
                              .known = true,
                              .reads = false,
                              .values = values,
                              .tbd = true,
                          }));
        memory->index += arg->val.data.num.size;
        memory->index += arg->val.data.num.size == 1 ? 3 : 0;
      }
      REMOVE_STMT;
    case FSET:
      CHECK_ARG_COUNT(2);
      if (!findInMemory(memory, &args[0], dest, VARIABLE))
        assert(0 && "Invalid argument passed to set");
      findInMemory(memory, &args[1], orig, VARIABLE);
      if (orig[0] == dest[0]) {
        REMOVE_STMT;
      }
      dest[0]->known = (orig[0] && orig[0]->known) || args[0].type == NUMBER;
      if (!dest[0]->known && orig[0])
        assert(0 && "Invalid argumnt passed to set");
      val[0] = orig[0] ? VARVAL(orig[0]) : NUMVAL(1);
      REPLACE_WITH(INDEX, dest[0]->index, 0);
      if (dest[0]->known && orig[0])
        REPLACE_WITH(NUMBER, VARVAL(orig[0]), 1);
      if (args[1].type == NUMBER) {
        VARVAL(dest[0]) = NUMVAL(1);
      } else if (findInMemory(memory, &args[1], orig, VARIABLE)) {
        VARVAL(dest[0]) = VARVAL(orig[0]);
        if (orig[0]->known) {
          REPLACE_WITH(NUMBER, VARVAL(orig[0]), 1);
        } else
          REPLACE_WITH(INDEX, orig[0]->index, 1);
      } else
        assert(0 && "Invalid argument passed to set");
      dest[0]->known = args[1].type == NUMBER ? true : orig[0]->known;
      break;
    case FINC:
    case FDEC:
      CHECK_ARG_COUNT(2);
      if (!findInMemory(memory, &args[0], dest, VARIABLE))
        assert(0 && "Invalid argument passed to inc/dec");
      findInMemory(memory, &args[1], orig, VARIABLE);
      isKnown[0] = (orig[0] && orig[0]->known) || args[1].type == NUMBER;
      if (!isKnown[0] && !orig[0])
        assert(0 && "Invalid argumnt passed to inc/dec");
      dest[0]->known = dest[0]->known && isKnown[0];
      val[0] = orig[0] ? VARVAL(orig[0]) : NUMVAL(1);
      REPLACE_WITH(INDEX, dest[0]->index, 0);
      if (dest[0]->known) {
        VARVAL(dest[0]) += val[0] * (2 * (TYPE == FINC) - 1);
        REPLACE_WITH(NUMBER, VARVAL(dest[0]), 1);
        TYPE = FSET;
      } else if (orig[0] && isKnown[0]) {
        REPLACE_WITH(NUMBER, val[0], 1);
      } else if (orig[0])
        REPLACE_WITH(INDEX, orig[0]->index, 1);
      break;
    case FADD:
    case FSUB:
    case FMUL:
      CHECK_ARG_COUNT(3);
      if (!findInMemory(memory, &args[2], dest, VARIABLE))
        assert(0 && "Invalid argument passed to add/sub/mul");
      for (int i = 0; i < 2; ++i) {
        findInMemory(memory, &args[i], &orig[i], VARIABLE);
        isKnown[i] = (orig[i] && orig[i]->known) || args[i].type == NUMBER;
        if (!isKnown[i] && !orig[i])
          assert(0 && "Invalid argument passed to add/sub/mul");
        val[i] = orig[i] ? VARVAL(orig[i]) : NUMVAL(i);
      }
      dest[0]->known = isKnown[0] && isKnown[1];
      if (dest[0]->known) {
        VARVAL(dest[0]) = val[0];
        if (TYPE == FMUL) {
          VARVAL(dest[0]) *= val[1];
        } else
          VARVAL(dest[0]) += (2 * (TYPE == FADD) - 1) * val[1];
        TYPE = FSET;
        REPLACE_WITH(INDEX, dest[0]->index, 0);
        REPLACE_WITH(NUMBER, VARVAL(dest[0]), 1);
        stmt->args.count = 2;
        break;
      } else if (orig[0] == orig[1] && orig[1] == dest[0]) {
        REPLACE_WITH(INDEX, dest[0]->index, 0);
        stmt->args.count = 1 + (TYPE == FSUB);
        if (TYPE == FSUB) {
          TYPE = FSET;
          REPLACE_WITH(NUMBER, 0, 1);
          VARVAL(dest[0]) = 0;
          dest[0]->known = true;
        } else
          TYPE = TYPE == FADD ? DOUBLE : SQUARE;
        break;
      }
      bool wasEqual = false;
      for (int i = 0; i < 2; ++i) {
        if (orig[i] != dest[0])
          continue;
        int other = (i + 1) % 2;
        bool isRegister = orig[other];
        TYPE = TYPE == FMUL ? DUPLICATE : (TYPE == FADD ? FINC : FDEC);
        REPLACE_WITH(INDEX, dest[0]->index, 0);
        if (isRegister && !isKnown[other]) {
          REPLACE_WITH(INDEX, orig[other]->index, 1);
        } else
          REPLACE_WITH(NUMBER, val[other], 1);
        stmt->args.count = 2;
        wasEqual = true;
        break;
      }
      if (wasEqual)
        break;
      REPLACE_WITH(INDEX, dest[0]->index, 2);
      for (int i = 0; i < 2; ++i)
        if (orig[i])
          REPLACE_WITH(INDEX, orig[i]->index, i);
      break;
    case FDIVMOD:
    case FDIV:
    case FMOD:
      CHECK_ARG_COUNT(TYPE == FDIVMOD ? 4 : 3);
      if (TYPE == FMOD && !findInMemory(memory, &args[2], &dest[1], VARIABLE))
        assert(0 && "Invalid argument passed to mod");
      for (int i = 0; i < 2; ++i) {
        if ((TYPE == FDIVMOD || (i == 0 && TYPE == FDIV)) &&
            !findInMemory(memory, &args[i + 2], &dest[i], VARIABLE))
          assert(0 && "Invalid argument passed to divmod/div");
        findInMemory(memory, &args[i], &orig[i], VARIABLE);
        isKnown[i] = (orig[i] && orig[i]->known) || args[i].type == NUMBER;
        if (!isKnown[i] && !orig[i])
          assert(0 && "Invalid argument passed to divmod/div/mod");
        val[i] = orig[i] ? VARVAL(orig[i]) : NUMVAL(i);
      }
      if (dest[0] == dest[1])
        assert(0 && "Invalid argument passed to divmod/div/mod");
      if (isKnown[1] && val[1] == 0)
        assert(0 && "Tried to divide by zero");
      if (isKnown[0] && isKnown[1]) {
        if (TYPE != FMOD) {
          dest[0]->known = true;
          VARVAL(dest[0]) = val[0] / val[1];
        }
        if (TYPE != FDIV) {
          dest[1]->known = true;
          VARVAL(dest[1]) = val[0] % val[1];
        }
        REPLACE_WITH(INDEX, dest[TYPE == FMOD ? 1 : 0]->index, 0);
        REPLACE_WITH(NUMBER, VARVAL(dest[TYPE == FMOD ? 1 : 0]), 1);
        if (TYPE != FDIVMOD)
          goto endDivMod;
        Argument newArgs[] = {{.type = INDEX, .val.index = dest[1]->index},
                              {.type = NUMBER, .val.data.num.ber = 0}};
        da_append(stmts, ((Statement){
                             .type = FSET,
                             .args = {0},
                             .next = stmt->next,
                             .jump = NULL,
                             .prev = stmt,
                         }));
        da_append(&(stmts->items[stmts->count - 1].args), newArgs[0]);
        da_append(&(stmts->items[stmts->count - 1].args), newArgs[1]);
        stmt->next = &stmts->items[stmts->count - 1];
      endDivMod:
        stmt->args.count = 2;
        if (TYPE == FDIVMOD) {
          TYPE = FSET;
          stmt = stmt->next;
        } else
          TYPE = FSET;
      } else
        for (int i = 0; i < 2; ++i) {
          if (TYPE == FDIVMOD || (i == 0 && TYPE == FDIV))
            REPLACE_WITH(INDEX, dest[i]->index, i + 2);
          if (orig[i])
            REPLACE_WITH(INDEX, orig[i]->index, i);
        }
      break;
    case FCMP:
      CHECK_ARG_COUNT(3);
      if (!findInMemory(memory, &args[2], dest, VARIABLE))
        assert(0 && "Invalid argument passed to cmp");
      for (int i = 0; i < 2; ++i) {
        findInMemory(memory, &args[i], &orig[i], VARIABLE);
        isKnown[i] = (orig[i] && orig[i]->known) || args[i].type == NUMBER;
        if (!isKnown[i] && !orig[i])
          assert(0 && "Invalid argument passed to cmp");
        val[i] = orig[i] ? VARVAL(orig[i]) : NUMVAL(i);
      }
      dest[0]->known = (isKnown[0] && isKnown[1]) || (orig[0] == orig[1]);
      if (dest[0]->known) {
        VARVAL(dest[0]) = val[0] < val[1] ? 255 : (val[0] == val[1] ? 0 : 1);
        if (orig[0] == orig[1])
          VARVAL(dest[0]) = 0;
        REPLACE_WITH(INDEX, dest[0]->index, 0);
        REPLACE_WITH(NUMBER, VARVAL(dest[0]), 1);
        TYPE = FSET;
        stmt->args.count = 2;
      } else
        for (int i = 0; i < 2; ++i)
          if (orig[i])
            REPLACE_WITH(INDEX, orig[i]->index, i);
      break;
    case FA2B:
      CHECK_ARG_COUNT(4);
      for (int i = 0; i < 3; ++i) {
        findInMemory(memory, &args[i], &orig[i], VARIABLE);
        isKnown[i] = (orig[i] && orig[i]->known) || args[i].type == NUMBER;
        if (!isKnown[i] && !orig[i])
          assert(0 && "Invalid argument passed to a2b");
        val[i] = orig[i] ? VARVAL(orig[i]) : NUMVAL(i);
      }
      if (!findInMemory(memory, &args[3], dest, VARIABLE))
        assert(0 && "Invalid argument passed to a2b");
      dest[0]->known = isKnown[0] && isKnown[1] && isKnown[2];
      if (dest[0]->known) {
        REPLACE_WITH(INDEX, dest[0]->index, 0);
        REPLACE_WITH(NUMBER,
                     100 * (val[0] - 48) + 10 * (val[1] - 48) + val[2] - 48, 1);
        TYPE = FSET;
        stmt->args.count = 2;
      } else
        for (int i = 0; i < 4; ++i)
          if (args[i].type != NUMBER)
            REPLACE_WITH(INDEX, args[i].val.index, i);
      break;
    case FB2A:
      CHECK_ARG_COUNT(4);
      for (int i = 1; i < 4; ++i)
        if (!findInMemory(memory, &args[i], &dest[i], VARIABLE))
          assert(0 && "Invalid argument passed to b2a");
      findInMemory(memory, args, orig, VARIABLE);
      isKnown[0] = (orig[0] && orig[0]->known) || args[0].type == NUMBER;
      if (!isKnown[0] && !orig[0])
        assert(0 && "Invalid argument passed to b2a");
      if (isKnown[0]) {
        TYPE = FSET;
        stmt->args.count = 2;
        REPLACE_WITH(INDEX, dest[0]->index, 0);
        REPLACE_WITH(NUMBER, 48 + VARVAL(orig[0]) / 100, 1);
        Argument firstArgs[] = {
            {.type = INDEX, .val.index = dest[1]->index},
            {.type = NUMBER,
             .val.data.num.ber = 48 + ((VARVAL(orig[0]) / 10) % 10)}};
        Statement firstStatement = {
            .type = FSET,
            .args = {0},
            .jump = NULL,
            .prev = stmt,
        };
        Argument secondArgs[] = {
            {.type = INDEX, .val.index = dest[2]->index},
            {.type = NUMBER, .val.data.num.ber = 48 + (VARVAL(orig[0]) % 10)}};
        Statement secondStatement = {
            .type = FSET,
            .args = {0},
            .next = stmt->next,
            .jump = NULL,
        };
        da_append(stmts, firstStatement);
        da_append(stmts, secondStatement);
        da_append(&stmts->items[stmts->count - 2].args, firstArgs[0]);
        da_append(&stmts->items[stmts->count - 2].args, firstArgs[1]);
        da_append(&stmts->items[stmts->count - 1].args, secondArgs[0]);
        da_append(&stmts->items[stmts->count - 1].args, secondArgs[1]);
        if (stmt->next)
          stmt->next->prev = &stmts->items[stmts->count - 1];
        stmt->next = &stmts->items[stmts->count - 2];
        stmts->items[stmts->count - 2].next = &stmts->items[stmts->count - 1];
        stmts->items[stmts->count - 1].prev = &stmts->items[stmts->count - 2];
        for (int i = 0; i < 3; ++i)
          dest[i]->known = true;
        stmt = stmt->next->next;
      } else
        for (int i = 0; i < 4; ++i)
          REPLACE_WITH(INDEX, args[i].val.index, i);
      break;
    case FLSET:
    case FLGET:
      CHECK_ARG_COUNT(3);
      if (!findInMemory(memory, args, TYPE == FLSET ? dest : orig, LIST))
        assert(0 && "Invalid argument passed to lset/lget");
      findInMemory(memory, &args[1], TYPE == FLSET ? &dest[1] : &orig[1],
                   VARIABLE);
      findInMemory(memory, &args[2], TYPE == FLSET ? orig : dest, VARIABLE);
      if ((TYPE == FLGET) && !dest[0])
        assert(0 && "Invalid argument passed to lget");
      isKnown[2] = args[2].type == NUMBER || (orig[0] && orig[0]->known);
      isKnown[1] = args[1].type == NUMBER
                       ? true
                       : (TYPE == FLSET ? (dest[1] && dest[1]->known)
                                        : (orig[1] && orig[1]->known));
      if (!isKnown[1] && (TYPE == FLSET ? !dest[1] : !orig[1]))
        assert(0 && "Invalid argument passed to lset/lget");
      if (isKnown[1]) {
        REPLACE_WITH(INDEX, dest[0]->index, 0);
        long listIndex =
            args[1].type == NUMBER ? (long)NUMVAL(1) : args[1].val.index;
        if (TYPE == FLSET) {
          args[0].val.index += listIndex;
          if (isKnown[2]) {
            dest[0]->values.items[listIndex] =
                args[2].type == NUMBER ? NUMVAL(2) : VARVAL(orig[0]);
          } else
            dest[0]->known = false;
        }
        if (isKnown[2]) {
          REPLACE_WITH(NUMBER, VARVAL(orig[0]), 1);
        } else
          REPLACE_WITH(INDEX, orig[0]->index, 1);
        if (TYPE == FLGET)
          dest[0]->known = true;
        TYPE = FSET;
        stmt->args.count = 2;
      } else {
        if (TYPE == FLSET)
          dest[0]->known = false;
        for (int i = 0; i < 3; ++i)
          if (args[i].type != NUMBER)
            REPLACE_WITH(INDEX, args[i].val.index, i);
      }
      break;
    case IFEQ:
      CHECK_ARG_COUNT(2);
      puts("TODO");
      break;
    case IFNEQ:
      CHECK_ARG_COUNT(2);
      puts("TODO");
      break;
    case WNEQ:
      CHECK_ARG_COUNT(2);
      puts("TODO");
      break;
    case END:
      CHECK_ARG_COUNT(0);
      puts("TODO");
      break;
    case CALL:
      if (stmt->args.count < 1 || false)
        return -1; // Invalid procedure call
      puts("TODO");
      break;
    case READ:
      CHECK_ARG_COUNT(1);
      if (!findInMemory(memory, &args[0], dest, VARIABLE))
        assert(0 && "Invalid argument passed to read");
      REPLACE_WITH(INDEX, dest[0]->index, 0);
      dest[0]->known = false;
      dest[0]->reads = true;
      break;
    case MSG:
      if (stmt->args.count < 1)
        assert(0 && "Invalid msg");
      for (size_t i = 0; i < stmt->args.count; ++i) {
        if (findInMemory(memory, &args[i], orig, VARIABLE)) {
          if (orig[0]->known) {
            REPLACE_WITH(NUMBER, VARVAL(orig[0]), i);
          } else
            REPLACE_WITH(INDEX, orig[0]->index, i);
        } else if (args[i].type != STRING)
          assert(0 && "Invalid argument passed to msg");
      }
      break;
    default:
      assert(0 && "Invalid statement");
    }
    stmt = stmt->next;
  }
  stmt = *start;
  while (stmt) {
    if (TYPE != FSET)
      for (size_t t = 0; t < stmt->args.count; ++t)
        if (stmt->args.items[t].type == INDEX)
          for (size_t u = 0; u < memory->count; ++u)
            if (memory->items[u].index == stmt->args.items[t].val.index)
              memory->items[u].tbd = false;
    stmt = stmt->next;
  }
  stmt = *start;
  while (stmt) {
    if (TYPE == FSET)
      for (size_t t = 0; t < memory->count; ++t)
        if (stmt->args.items[0].val.index == memory->items[t].index &&
            memory->items[t].tbd) {
          REMOVE_STMT;
        }
    stmt = stmt->next;
  }

  memory->index = 0;
  return 0;
}

#define LAST_OUTPUT (output->count > 0 ? output->items[output->count - 1] : ']')

#define OUTPUT(string)                                                         \
  do {                                                                         \
    for (size_t stringIndex = 0; (string)[stringIndex];)                       \
      da_append(output, (string)[stringIndex++]);                              \
  } while (0)

#define INC da_append(output, '+')
#define DEC da_append(output, '-')

#define GOTO(ind)                                                              \
  while (memory->index != (ind)) {                                             \
    OUTPUT(memory->index < (ind) ? ">" : "<");                                 \
    memory->index += memory->index < (ind) ? 1 : -1;                           \
  }

#define SET_TO_ZERO(ind)                                                       \
  do {                                                                         \
    GOTO((ind));                                                               \
    if (LAST_OUTPUT != ']' &&                                                  \
        !(LAST_OUTPUT == '.' && output->items[output->count - 2] == ']')) {    \
      OUTPUT("[-]");                                                           \
    }                                                                          \
  } while (0)

#define ADDSUB(ind, amount, sign)                                              \
  do {                                                                         \
    GOTO((ind));                                                               \
    for (unsigned char t = 0; t < (amount); ++t)                               \
      if ((sign) == '+') {                                                     \
        INC;                                                                   \
      } else if ((sign) == '-') {                                              \
        DEC;                                                                   \
      }                                                                        \
  } while (0)

#define SET_TO(amount, ind)                                                    \
  do {                                                                         \
    SET_TO_ZERO((ind));                                                        \
    for (size_t count = 0; count < (amount); ++count)                          \
      da_append(output, '+');                                                  \
  } while (0)

bool distribute(Memory *memory, long origin, long *indeces, size_t indecesCount,
                char *signs, Data *output) {
  for (size_t count = 0; count < indecesCount; ++count)
    if (signs[count] != '+' && signs[count] != '-')
      return false;
  GOTO(origin);
  if (LAST_OUTPUT == ']')
    return true;
  OUTPUT("[-");
  for (size_t counter = 0; counter < indecesCount; ++counter) {
    GOTO(indeces[counter]);
    da_append(output, signs[counter]);
  }
  GOTO(origin);
  da_append(output, ']');
  return true;
}

#define INDEX(argNum) args[(argNum)].val.index

#define EXTRACT(argNum)                                                        \
  GOTO(INDEX(argNUM));                                                         \
  distribute(memory, INDEX(argNum), ram[argNum], 2, plus, output);             \
  distribute(memory, INDEX(argNum) + 2, ram[0], 1, plus, output);

void interpretStatement(Statement *restrict stmt, Memory *memory,
                        Data *output) {
  Argument *args = stmt->args.items;
  char plus[] = {'+', '+', '+', '+', '+'};
  long ram[3][4] = {0};
  for (int i = 0; i < 3; ++i)
    if (args[i].type == INDEX)
      for (int j = 0; j < 4; ++j)
        ram[i][j] = INDEX(i) + j;
  switch (TYPE) {
  case FSET:
    SET_TO_ZERO(INDEX(0));
    if (args[1].type == VARIABLE) {
      GOTO(INDEX(1));
      long temp = INDEX(1) - 1;
      long locations[] = {INDEX(0), INDEX(1)};
      distribute(memory, INDEX(1), &temp, 1, plus, output);
      distribute(memory, temp, locations, 2, plus, output);
    } else
      SET_TO(NUMVAL(1), INDEX(0));
    break;
  case FINC:
  case FDEC:
    if (args[1].type == INDEX) {
      char signs[] = {'+', TYPE == FADD ? '+' : '-'};
      long locations[] = {INDEX(1) - 1, INDEX(0)};
      distribute(memory, INDEX(1), locations, 2, signs, output);
      distribute(memory, locations[0], &INDEX(1), 1, signs, output);
    } else
      ADDSUB(INDEX(0), NUMVAL(1), (TYPE == FINC ? '+' : '-'));
    break;
  case FADD:
  case FSUB:
    SET_TO_ZERO(INDEX(2));
    long first[] = {INDEX(0) + 1, INDEX(2)};
    distribute(memory, INDEX(0), first, 2, plus, output);
    char secondSigns[] = {'+', TYPE == FADD ? '+' : '-'};
    long second[] = {INDEX(1) + 1, INDEX(2)};
    distribute(memory, INDEX(1), second, 2, secondSigns, output);
    distribute(memory, INDEX(0) + 1, &INDEX(0), 1, plus, output);
    distribute(memory, INDEX(1) + 1, &INDEX(1), 1, plus, output);
    break;
  case FMUL:
    SET_TO_ZERO(INDEX(2));
    long locations[] = {INDEX(0) + 1, INDEX(1) + 1, INDEX(0), INDEX(2)};
    distribute(memory, INDEX(0), locations, 1, plus, output);
    distribute(memory, INDEX(1), &locations[1], 1, plus, output);
    GOTO(INDEX(1) + 1);
    OUTPUT("[-");
    ADDSUB(INDEX(1), 1, *plus);
    distribute(memory, INDEX(0) + 1, &locations[2], 2, plus, output);
    distribute(memory, INDEX(0), locations, 1, plus, output);
    GOTO(INDEX(1) + 1);
    da_append(output, ']');
    distribute(memory, INDEX(0) + 1, &locations[2], 1, plus, output);
    break;
  case FDIVMOD:
    GOTO(INDEX(0));
    distribute(memory, INDEX(0), ram[0], 2, plus, output);
    distribute(memory, INDEX(0) + 2, ram[0], 1, plus, output);

    break;
  case FDIV:
    puts("TODO FDIV");
    break;
  case FMOD:
    puts("TODO FMOD");
    break;
  case FCMP:
    puts("TODO FCMP");
    break;
  case FA2B:
    puts("TODO FA2B");
    break;
  case FB2A:
    puts("TODO FB2A");
    break;
  case FLSET:
    puts("TODO FLSET");
    break;
  case FLGET:
    puts("TODO FLGET");
    break;
  case IFEQ:
    puts("TODO IFEQ");
    break;
  case IFNEQ:
    puts("TODO IFNEQ");
    break;
  case WNEQ:
    puts("TODO WNEQ");
    break;
  case END:
    puts("TODO END");
    break;
  case READ:
    GOTO(args[0].val.index);
    da_append(output, ',');
    break;
  case MSG:
    for (size_t i = 0; i < stmt->args.count; ++i) {
      long ind = memory->index / 4;
      unsigned char prev = 0;
      switch (args[i].type) {
      case STRING:
        SET_TO_ZERO(ind + 1);
        for (size_t j = 0; j < args[i].val.data.alpha.string.length; ++j) {
          int diff = (int)args[i].val.data.alpha.string.start[j] - (int)prev;
          for (int k = 0; k < abs(diff); ++k)
            da_append(output, (diff < 0 ? '-' : '+'));
          da_append(output, '.');
          prev = args[i].val.data.alpha.string.start[j];
        }
        break;
      case INDEX:
        GOTO(args[i].val.index);
        da_append(output, '.');
        prev = 0;
        break;
      default:
        SET_TO(NUMVAL(i), ind + 1);
        da_append(output, '.');
        prev = NUMVAL(i);
        break;
      }
    }
    break;
  case DOUBLE:;
    long doubleInds[] = {INDEX(0) + 1, INDEX(0) + 2};
    distribute(memory, INDEX(0), doubleInds, 2, plus, output);
    distribute(memory, INDEX(0) + 1, &INDEX(0), 1, plus, output);
    distribute(memory, INDEX(0) + 2, &INDEX(0), 1, plus, output);
    break;
  case SQUARE:;
    long squareInds[] = {INDEX(0), INDEX(0) + 1, INDEX(0) + 2, INDEX(0) + 3};
    distribute(memory, squareInds[0], &squareInds[2], 2, plus, output);
    GOTO(squareInds[3]);
    OUTPUT("[-");
    distribute(memory, squareInds[2], squareInds, 2, plus, output);
    distribute(memory, squareInds[1], &squareInds[2], 1, plus, output);
    GOTO(squareInds[3]);
    da_append(output, ']');
    break;
  case DUPLICATE:;
    long dupInds[] = {INDEX(0), INDEX(0) + 1, INDEX(0) + 2, INDEX(0) + 3};
    distribute(memory, squareInds[0], &dupInds[2], 1, plus, output);
    if (args[1].type == NUMBER) {
      SET_TO(NUMVAL(1), dupInds[3]);
    } else
      distribute(memory, INDEX(1), &dupInds[3], 1, plus, output);
    GOTO(squareInds[2]);
    OUTPUT("[-");
    distribute(memory, dupInds[2], dupInds, 2, plus, output);
    distribute(memory, dupInds[1], &dupInds[2], 1, plus, output);
    GOTO(squareInds[3]);
    da_append(output, ']');
    break;
  default:
    puts("THIS SHOULD NEVER HAPPEN");
    break;
  }
}

int kcuf(char **output, const char *code) {
  int result = 0;
  TokenList tokens = {0};
  Data data = {0};
  StringLengths symbolLengths = {0};

  result = tokenize(&tokens, &data, &symbolLengths, code);

  if (result)
    return_(defer1, result);

  StatementList statements = {0};

  for (size_t i = 0; i < tokens.count;) {
    while (tokens.items[i] == EOL)
      ++i;
    if (i >= tokens.count)
      break;
    result = makeStatement(&statements, tokens, &i, &data, &symbolLengths);
    if (result)
      return_(defer2, result);
  }

  ProcedureList procList = {0};
  Statement *stmt;
  result = buildAST(&statements, &procList, &stmt);

  if (result || !stmt)
    return_(defer3, result); // Invalid AST

  Memory memory = {0};
  CallStack callStack = {0};

  result = checkAST(&statements, &stmt, &procList, &memory,
                    &callStack); // Need to finish making function

  if (result)
    return_(defer4, result);

  printf("\n");
  Statement *blah = stmt;
  while (blah) {
    printf("[0x%.04x] ", (unsigned int)((unsigned long)blah % 8192));
    printToken(blah->type);
    printf(" prev: 0x%.04x next: 0x%.04x",
           (unsigned int)((unsigned long)blah->prev % 8192),
           (unsigned int)((unsigned long)blah->next % 8192));
    for (size_t u = 0; u < blah->args.count; ++u) {
      printf(" [ ");
      if (blah->args.items[u].type == INDEX) {
        printf("INDEX: %ld", blah->args.items[u].val.index);
      } else if (blah->args.items[u].type == NUMBER) {
        printf("NUMBER: %d", blah->args.items[u].val.data.num.ber);
      } else if (blah->args.items[u].type == STRING) {
        printf("STRING: ");
        printSV(blah->args.items[u].val.data.alpha.string);
      }
      printf(" ]");
    }
    printf("\n");
    blah = blah->next;
  }

  return_(defer4, 100);

  Data outputStr = {0};

  while (stmt != NULL) {
    interpretStatement(stmt, &memory, &outputStr);
    stmt = stmt->next;
  }

  da_append(&outputStr, '\0');
  *output = realloc(outputStr.items, outputStr.count);

  // printf("\n");
  // for (size_t t = 0; t < procList.count; ++t) {
  //   printf("Proc Name: ");
  //   for (size_t u = 0; u < procList.items[t].name.length; ++u)
  //     printf("%c", procList.items[t].name.start[u]);
  //   printf(" Parameters:");
  //   for (size_t u = 0; u < procList.items[t].parameters.count; ++u) {
  //     printf(" ");
  //     for (size_t v = 0; v < procList.items[t].parameters.items[u].length;
  //     ++v)
  //       printSV(procList.items[t].parameters.items[u]);
  //   }
  //   printf("\n");
  //   printf("Start: 0x%.04x\n",
  //          (unsigned int)((unsigned long)procList.items[t].start % 8192));
  // }

  // printf("MEMORY: ");
  // for (size_t u = 0; u < memory.count; ++u) {
  //   for (size_t v = 0; v < memory.items[u].name.length; ++v)
  //     printf("%c", memory.items[u].name.start[v]);
  //   if (u < memory.count - 1)
  //     printf(", ");
  // }

defer4:
  da_free(memory);
  da_free(callStack);
defer3:
  for (size_t x = 0; x < procList.count; ++x)
    da_free(procList.items[x].parameters);
  da_free(procList);
defer2:
  for (size_t x = 0; x < statements.count; ++x)
    da_free(statements.items[x].args);
  da_free(statements);
defer1:
  da_free(tokens);
  da_free(data);
  da_free(symbolLengths);

  return result;
}

// Because there are no throw catch infrastructures in C
// We are here using the return value to tell if there are errors
// If no error occured
//     0 should be returned
// If any error occured
//     non-0 should be returned.
//     And to help debug, output will be printed if it is not null

/*

In the checking of the AST, treat functions like macros by expanding them
out and keep track of the call stack to avoid recursion.

TODO: Go through compilation warnings

*/

int main(void) {
  const char *program = "var A B C D\n"
                        "set A 79\n"
                        "set B 13\n"
                        "divmod A B C D\n"
                        "msg A B C D\n"
                        "div C D C\n"
                        "msg A B C D\n"
                        "mod A D A\n"
                        "msg A B C D";
  char *output = NULL;
  int returnValue = kcuf(&output, program);

  printf("\nThe return value was %d\n", returnValue);
  if (output != NULL) {
    printf("Output: %s\n", output);
    FILE *codeTxt = fopen("code.txt", "w");
    if (!codeTxt) {
      puts("Could not save output to file.");
    } else {
      fputs(output, codeTxt);
      fclose(codeTxt);
      puts("Output saved to code.txt");
    }
    free(output);
  }

  return 0;
}
