#include <stdio.h>
#include "Catalog.h"
#include "Global.h"

int main() {

  char Key_name[100];
  char Key_Type[100];

  int n;

  catalog_record * CATALOG = NULL;
  catalog_record ** index = NULL;
  catalog_record * record = NULL;
  
  catalog_build(&CATALOG);
  catalog_traversal(CATALOG);

  scanf("%s %s", Key_name, Key_Type);
  int id = catalog_update(&CATALOG, Key_name, Key_Type);
  printf("_id: %d\n", id);
  catalog_traversal(CATALOG);
  

  catalog_save(CATALOG);
  catalog_traversal(CATALOG);
  
  index = (catalog_record **)catalog_index_build(CATALOG);
  catalog_find(id, index, &record);
  
  
  printf("%d %s %s %d\n", record->_id, record->Key_name,
  	 record->Key_Type, record->count);
  
  return 0;
}
