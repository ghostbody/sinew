#include "Global.h"

#ifndef Catalog_Basic
#define Catalog_Basic
typedef struct catalog_record {
  int _id;
  char Key_name[100];
  char Key_Type[100];
  int count;
  bool dirty;
  struct catalog_record * next;
} catalog_record;

/*
void catalog_append(catalog_record ** head,int _id, char * Key_name, char * Key_Type, int count, bool dirty);
//void catalog_delete(catalog_record * head, catalog_record * rec);
//bool catalog_inc(catalog_record ** head, char * Key_name);
//void catalog_dec();
void catalog_traversal(catalog_record * head);
*/
#endif

#ifdef Catalog_Interface
#define Catalog_Interface
bool catalog_update(catalog_record ** CATALOG, char * Key_name, char * Key_Type);
bool catalog_build(catalog_record ** CATALOG);
bool catalog_save(catalog_record * CATALOG);
#endif