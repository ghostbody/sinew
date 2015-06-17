#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Global.h"
#include "Catalog.h"
#include "Json.h"
#include "Serializer.h"
#include "Find.h"

int comp(const void*a,const void*b) {
  return (*(file_position **)a)->id - (*(file_position **)b)->id;
}

/* commands for the project
   -h : help
   -i <file> : insert file
   -cc : checkout the catalog
   -f <A> = <B> : find <key, value>
 */

const char * command[] = {
  "-h","-i","-cc","-f"
};

void help() {
  printf("usage:\n");
  printf("-h : help\n");
  printf("-i <file> : insert file\n");
  printf("-cc : checkout the catalog\n");
  printf("-f <A> = <B> : find <key,value>\n");
  printf("\n");
}

void CheckoutCatolog() {
  catalog_record * CATALOG = NULL;
  if(!catalog_build(&CATALOG)) {
    printf("CheckoutCatolog : CATALOG LOADED FAILED!\n");
    return;
  }

  if(CATALOG == NULL) {
    printf("CheckoutCatolog : EMPTY!\n");
    return;
  }
  catalog_traversal(CATALOG);
}

void Insert(const char * file) {
  FILE * toInsert = fopen(file, "r");
  if(toInsert == NULL) {
    printf("Insert FILE : '%s', no such file\n", file);
  }
  time_t clock_start, clock_end;
  char json[10000];
  catalog_record * CATALOG = NULL;
  int counter = 1;
  catalog_build(&CATALOG);
  FILE * storage = fopen(DATA_FILE, "ab+");

  clock_start = clock();

  static char IOBUFFER[PAGE_SIZE];
  
  if (setvbuf(storage, IOBUFFER, _IOFBF, PAGE_SIZE) != 0) {
    printf("failed to set up buffer for qurey\n");
  } else {
    printf("buffer(8KB) set up for input file\n"); 
  }

  while(fgets(json, 10000, toInsert) != NULL) {
    {
      json_parser * parser;
      int len = 0;
      parser = ParseJson(json, &len);
      static int max_id = 0;
      JsonSerializer(json, parser, len, &max_id, &CATALOG, storage);
    }
    if (counter % 1000 == 0)
      printf("process at %d\n", counter);
    counter++;
  }
  clock_end = clock();

  printf("insert file done!\n");
  printf("time waste: %.2fs\n", (double)(clock_end-clock_start)/CLOCKS_PER_SEC);
  
  catalog_save(CATALOG);
  fclose(storage);
}

void Find(const char * A, const char * B) {
  catalog_record * CATALOG = NULL;
  FILE * storage = fopen(DATA_FILE, "rb");
  file_position **file_index;
  int count;

  printf("\n note that:\n");
  printf("\t use raw_int for int values\n");
  printf("\t use 'string' for string values\n");
  printf("\t use true or false for bool values\n");
  printf("\t use [] for array\n");
  printf("\t use {} for json\n\n");

  static char IOBUFFER[PAGE_SIZE];
  if (setvbuf(storage, IOBUFFER, _IOFBF, PAGE_SIZE) != 0) {
    printf("failed to set up buffer for input file\n");
  } else {
    printf("buffer(8KB) set up for input file\n"); 
  }

  count = build_data_file_index(&file_index, storage);
  catalog_build(&CATALOG);
  qsort(file_index, count, sizeof(file_position *), comp);
  find(A, B, CATALOG, file_index, count, storage);
}

int main(int argc, char * argv[]) {

  printf("Simple Sinew System\n\n");
  
  if(argc <= 1) {
    printf("parameter -h for usage\n");
    exit(EXIT_FAILURE);
  }

  const char * Argument = argv[1];
  
  if(!strcmp(Argument, command[0])) {
    help();
    exit(EXIT_SUCCESS);
  } else if(!strcmp(Argument, command[1])) {
    const char * file = argv[2];
    if(file == NULL) {
      printf("No input file!\n");
      printf("parameter -h for usage\n");
      exit(EXIT_FAILURE);
    }
    printf("insert file '%s'\n", file);
    Insert(file);
    exit(EXIT_SUCCESS);
  } else if(!strcmp(Argument, command[2])) {
    CheckoutCatolog();
    exit(EXIT_SUCCESS);
  } else if(!strcmp(Argument, command[3])) {
    const char * A = argv[2];
    // argv[3] is '='
    const char * B = argv[4];
    if(A == NULL || B == NULL) {
      printf("Incomplete expression!\n");
      printf("parameter -h for usage\n");
      exit(EXIT_FAILURE);
    }
    printf("find %s = %s\n", A, B);
    Find(A, B);
    exit(EXIT_SUCCESS);
  } else {
    printf("No such Command!\n");
    printf("parameter -h for usage\n");
    exit(EXIT_FAILURE);
  }
}
