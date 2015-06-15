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
  following basic functions are used in the basic Catalog.c
 */
/*
void catalog_append(catalog_record ** head,int _id, char * Key_name, char * Key_Type, int count, bool dirty);
//void catalog_delete(catalog_record * head, catalog_record * rec);
//bool catalog_inc(catalog_record ** head, char * Key_name);
//void catalog_dec();
*/
#endif

#ifdef Catalog_Interface
#define Catalog_Interface
/*
  bool for functions
  True: operation compelete successfully
  False: operation failed due to some inner process problems
 */
/*
  input: A catalog_record list(CATALOG **), a Key_name String, a Key_Type String
  process: Append the input atr into the CATALOG
  output: A updated catalog_record list(CATALOG **)
          an int (-1 for fail 0 or other numbers mean _id)
 */
int catalog_update(catalog_record ** CATALOG, char * Key_name,
		    char * Key_Type);
/*
  input: An empty CATALOG list
  process: Read the CATALOG FILE and import the records
  output: A built-up CATALOG list
 */
bool catalog_build(catalog_record ** CATALOG);
/*
  input: A CATALOG list and an empty CATALOG_INDEX(array)
  process: Read All the CATALOG list address into the INDEX for direct access
  output: An array(the index)
 */
catalog_record ** catalog_index_build(catalog_record * CATALOG);
/*
  input: A Catalog id and an index
  process: Find the attr from the catalog 
  output: One catalog record or NULL(not found)
 */
bool catalog_find_by_id(int _id,catalog_record * index[],
		  catalog_record ** destination);
/*
  input: catalog keyname and key type
  process: find the id
  output: an id
 */
bool catalog_find_by_key(catalog_record * CATALOG, int * id, char * Key_name, char * Key_Type);
/*
  input: A Catalog list
  process: write it into the file
  output: void
 */
bool catalog_save(catalog_record * CATALOG);
/*
  traversal all the record of the list, use to output
 */
void catalog_traversal(catalog_record * head);
#endif
