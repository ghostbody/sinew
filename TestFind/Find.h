#include <stdio.h>
#include "Global.h"

#ifndef FIND
#define FIND
typedef struct file_position {
  unsigned int id;
  unsigned int attr_num;
  int * attr_ids;
  long position;
  unsigned int size;
  struct file_position * next;
} file_position;

int build_data_file_index(file_position *** file_index, FILE * storage);
bool find(char * Key_name, char * value, char ** json);
bool parse_serializer(file_position file_position[], int id);

#endif

