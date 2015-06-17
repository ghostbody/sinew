#include <stdio.h>
#include "Global.h"
#include "Catalog.h"
#include "Json.h"

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

typedef struct result{
  int record_id;
  char * json;
  int attr_position;
  bool match;
  struct result * next;
}result;

int build_data_file_index(file_position *** file_index, FILE * storage);
bool Materializer(file_position ** file_index, result ** head,
		  catalog_record * CATALOG, FILE * storage, int * record_count);

#endif

