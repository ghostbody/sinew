#include "Catalog.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "Global.h"

int catalog_inc(catalog_record ** head, char * Key_name, char * Key_Type) {
  typedef catalog_record node;
  node * positioner = *head;
  while(positioner != NULL) {
    if(strcmp(positioner->Key_name, Key_name) == 0 && strcmp(positioner->Key_Type, Key_Type) == 0)
      break;
    positioner = positioner->next;
  }

  if(positioner == NULL) {
    return -1;
  }

  positioner->count++;
  positioner->dirty = true;
  return positioner->_id;
}

int catalog_append(catalog_record ** head, int _id, char * Key_name,
		    char * Key_Type, int count, bool dirty) {
  typedef catalog_record node;		   
  
  if((*head) == NULL) {
    (*head) = (node *)malloc(sizeof(node));
    (*head)->_id = (_id == -1 ? 1 : _id);
    strcpy((*head)->Key_name, Key_name);
    strcpy((*head)->Key_Type, Key_Type);
    (*head)->count = count;
    (*head)->dirty = dirty;
    (*head)->next = NULL;
    return (*head)->_id;
  }

  int temp = catalog_inc(head, Key_name, Key_Type);
  if(temp != -1) {
    return temp;
  }
    
  node * positioner = *head;
  while(positioner->next != NULL) {
    positioner = positioner->next;
  }
  positioner->next = (node *)malloc(sizeof(node));
  positioner->next->_id = (_id == -1 ? positioner->_id+1 : _id);
  strcpy(positioner->next->Key_name, Key_name);
  strcpy(positioner->next->Key_Type, Key_Type);
  positioner->next->count = count;
  positioner->next->dirty = dirty;
  positioner->next->next = NULL;
  return positioner->next->_id;
}

void catalog_traversal(catalog_record * head) {
  typedef catalog_record node;
  node * positioner = head;
  if(head == NULL) {
    return;
  }

  printf("_id\tKey_name\tKey_type\tcount\n");

  while(positioner != NULL) {
    printf("%d\t%s\t\t%s\t\t%d\n", positioner->_id, positioner->Key_name,
	   positioner->Key_Type, positioner->count);
    positioner = positioner->next;
  }
  
}

int catalog_update(catalog_record ** CATALOG, char * Key_name, char * Key_Type) {
  if(Key_name == NULL || Key_Type == NULL) {
    return false;
  }

  int ret = catalog_append(CATALOG, -1, Key_name, Key_Type, 1, true);

  if(*CATALOG == NULL) {
    return false;
  }

  return ret;
}

bool catalog_build(catalog_record ** CATALOG) {
  FILE * F_Catalog = fopen(CATALOG_FILE, "r");
  catalog_record temp;
  
  if(F_Catalog == NULL) {
    return false;
  }

  while(fscanf(F_Catalog, "%d%s%s%d", &temp._id, temp.Key_name, temp.Key_Type, &temp.count) != EOF) {
    catalog_append(CATALOG, temp._id, temp.Key_name, temp.Key_Type, temp.count, false);
  }
  
  fclose(F_Catalog);
  return true;
}

bool catalog_save(catalog_record * CATALOG) {
  FILE * F_Catalog = fopen(CATALOG_FILE, "w+");

  if(F_Catalog == NULL) {
    return false;
  }

  catalog_record * positioner = CATALOG;
  
  while(positioner != NULL) {
    fprintf(F_Catalog, "%d %s %s %d\n",positioner->_id, positioner->Key_name,
	    positioner->Key_Type, positioner->count);
    positioner = positioner->next;
  }

  return true;
}

catalog_record ** catalog_index_build(catalog_record * CATALOG) {

  if (CATALOG == NULL) {
    return NULL;
  }

  catalog_record * positioner = CATALOG;

  int count = 0;

  while(positioner != NULL) {
    count++;
    positioner = positioner->next;
  }

  catalog_record ** CATALOG_INDEX = (catalog_record **)malloc(sizeof(catalog_record *) * (count + 1));

  if(CATALOG_INDEX == NULL) {
    return NULL;
  }

  int i;
  
  for(positioner = CATALOG, i = 0; positioner != NULL; positioner = positioner->next, i++) {
    CATALOG_INDEX[i] = positioner;
  }
  
  return CATALOG_INDEX;
}

bool catalog_find_by_id(int _id, catalog_record * index[],
		  catalog_record ** destination) {
  if(index == NULL) {
    return false;
  }

  int length = sizeof(index)/sizeof(index[0]);

  if(_id-1 > length) {
    return false;
  }
  
  if(index[_id-1]->_id == _id) {
    (*destination) = index[_id-1];
      return true;
  }

  destination = NULL;
  return false;
}

bool catalog_find_by_key(catalog_record * CATALOG, int * id, char * Key_name, char * Key_Type) {
  if(CATALOG == NULL || Key_name == NULL || id == NULL || Key_Type == NULL) {
    printf("NULL input @ bool catalog_find_by_key(catalog_record * CATALOG, int * id, char * Key_name, char * Key_Type)");
    return;
  }

  catalog_record * positioner = CATALOG;

  while(positioner != NULL) {
    if(strcmp(positioner->Key_name, Key_name) == 0 && strcmp(positioner->Key_Type, Key_Type) == 0)
      break;
    positioner = positioner->next;
  }
  
  if(positioner == NULL) {
    return false;
  }

  *id = positioner->_id;
  return true;
}
