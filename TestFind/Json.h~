
#ifndef JSONPARSER
#define JSONPARSER
typedef enum {atr = 0, value = 1} json_type;
typedef enum {int32 = 0, bool8 = 1, stringn = 2, array1 = 3, object1 = 4, unknown = 5} data_type;
typedef struct json_parser {
  unsigned int start;
  unsigned int end;
  json_type type;
  data_type dtype;
  struct json_parser * next;
} json_parser;
extern json_parser * ParseJson(char * JsonStr, int * len);
extern void DeleteJson(json_parser * head);
#endif
