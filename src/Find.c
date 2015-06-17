#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "Find.h"
#include "Global.h"
#include "Catalog.h"
#include "Json.h"
#include "Serializer.h"

/*
typedef struct file_position {
  unsigned int id;
  long position;
  unsigned int size;
  file_position * next;
} file_position;
*/

#ifndef DATA_TYPES
#define DATA_TYPES
const char * dataTypes[] = {
  "int32",
  "bool",
  "string",
  "array",
  "object",
  "unknown"
};
#endif


int build_data_file_index(file_position *** file_index, FILE * storage) {
  if(storage == NULL) {
    printf("build_data_file_index : invalid file!\n");
    return 0;
  }
  
  file_position * head = NULL;
  int id;
  int attr_num;
  int seek_flag = 0;
  long current_position = 0;

  int count = 0;
  int i;

  while(head == NULL) {
    head = (file_position *)malloc(sizeof(file_position));
  }
  count++;
  // read first id
  fread(&(head->id), sizeof(int), 1, storage);
  head->position = current_position;
  // read first attr_num
  fseek(storage, (long)sizeof(int), SEEK_SET);
  fread(&attr_num, sizeof(int), 1, storage);
  head->attr_num = attr_num;
  head->attr_ids = (int *)malloc(sizeof(int) * attr_num);
  fread(head->attr_ids, sizeof(int), attr_num, storage);
  fseek(storage, (long)((1+1+attr_num+2) * sizeof(int)), SEEK_SET);
  fread(&(head->size), sizeof(int), 1, storage);
  head->next = NULL;
  current_position += head->size;
  file_position * positioner = head;

  // printf("%d %ld %d\n", head->id, head->position, head->size);

  while(true) {
    // printf("current : %ld\n", current_position);
    fseek(storage, (long)(current_position), SEEK_SET);
    seek_flag = fread(&id, sizeof(int), 1, storage);
    
    // printf("seek_flag : %d\n", seek_flag);
    if(!seek_flag) {
      break;
    }
    
    while(positioner->next == NULL) {
      positioner->next = (file_position *)malloc(sizeof(file_position));
    }
    count++;
    positioner = positioner->next;
    positioner->id = id;
    positioner->position = current_position;
    fread(&attr_num, sizeof(int), 1, storage);
    // printf("attr_num: %d\n", attr_num);
    positioner->attr_num = attr_num;
    positioner->attr_ids = NULL;
    if(attr_num > 0) {
      while(positioner->attr_ids == NULL) {
	positioner->attr_ids = (int *)malloc(sizeof(int) * attr_num);
      }
      fread(positioner->attr_ids, sizeof(int), attr_num, storage);
    }
    /*
    for(i = 0; i < positioner->attr_num; i++) {
      printf("%d ", positioner->attr_ids[i]);
      }
    */
    // printf("%d", (positioner->attr_ids)[0]);
    // printf("\n");
    fseek(storage, (long)((1+1+attr_num*2) * sizeof(int) + current_position), SEEK_SET);
    fread(&(positioner->size), sizeof(int), 1, storage);
    positioner->next = NULL;
    current_position += positioner->size;
    // printf("%d %ld %d\n", positioner->id, positioner->position, positioner->size);
  }

  (*file_index) = NULL;

  while((*file_index) == NULL) {
    (*(file_index)) = (file_position **)malloc(sizeof(file_position *) * count);
  }

  positioner = head;
  for(i = 0; i < count; i++) {
    (*file_index)[i] = positioner;
    positioner = positioner->next;
  }
  
  return count;
}

int compare(const void *p, const void *q) {
    return (*(int *)p - *(int *)q);
}

void find_attr_in_file(result ** head, result ** tail, file_position ** file_index, int count, int attr_id) {
  int i;
  int * p;

  // int now = 0;
  
  for(i = 0; i < count; i++) {
    if(file_index[i]->attr_ids == NULL) {
      continue;
    }
    p = (int *)bsearch(&attr_id, file_index[i]->attr_ids, file_index[i]->attr_num, sizeof(int), compare);
    if(p != NULL) {
      // printf("found an matched attr %d!\n", now++);
      if((*head) == NULL) {
	while((*head) == NULL) {
	  (*head) = (result *)malloc(sizeof(result));
	}
	(*tail) = (*head);
	(*head)->record_id = file_index[i]->id;
	(*head)->json = NULL;
	(*head)->attr_position = p - (int *)file_index[i]->attr_ids;
	(*head)->match = true;
	(*head)->next = NULL;
      } else {
	while((*tail)->next == NULL) {
	  (*tail)->next = (result *)malloc(sizeof(result));
	}
	(*tail) = (*tail)->next;
	(*tail)->attr_position = p - (int *)file_index[i]->attr_ids;
	(*tail)->record_id = file_index[i]->id;
	(*tail)->match = true;
	(*tail)->json = NULL;
	(*tail)->next = NULL;
      }
    }
  }
}

char * Array_Materializer(file_position * array_record, FILE * storage) {
  char * Array = (char * )malloc(sizeof(char) * 1000);
  int i;

  const char * left_bracket = "[";
  const char * right_bracket = "]";
  const char * quote = "\"";
  const char * comma = ",";

  strcat(Array, left_bracket);
    
  for(i = 0; i < array_record->attr_num; i++) {
    int offset1;
    int offset2;
    fseek(storage, array_record->position + (2 + array_record->attr_num + i) * sizeof(int), SEEK_SET);
    fread(&offset1, sizeof(int), 1, storage);
    fread(&offset2, sizeof(int), 1, storage);
    fseek(storage, array_record->position + offset1, SEEK_SET);
    int length = offset2-offset1;
    char * data_string = (char *)malloc(sizeof(char) * (length + 1));
    fread(data_string, sizeof(char), length, storage);
    // printf("data_string: %s\n", data_string);

    strcat(Array, quote);
    strcat(Array, data_string);
    strcat(Array, quote);
    free(data_string);

    if(i != array_record->attr_num - 1)
      strcat(Array, comma);
  }
  strcat(Array, right_bracket);
  return Array;
}

char * Object_Materializer(int id, file_position ** file_index,
			   catalog_record ** catalog_index, FILE * storage) {
  catalog_record * catalog_res;
  file_position * destination_record;
  destination_record = file_index[id-1];
  
  char ** Key_names = (char **)malloc(sizeof(char *) * destination_record->attr_num);
  char ** values = (char **)malloc(sizeof(char *) * destination_record->attr_num);
  char * Json = NULL;
  while(Json == NULL) {
    Json = (char *)malloc(sizeof(char) * (destination_record->size) * 2);
  }
  int i;
  for(i = 0; i < destination_record->attr_num; i++) {
    catalog_find_by_id(destination_record->attr_ids[i],
		       catalog_index,
		       &catalog_res);
    //
    // printf("%d\n", strlen(catalog_res->Key_name));
    Key_names[i] = catalog_res->Key_name;
    //
    // puts(catalog_res->Key_Type); // dsad
    // values
    int offset1;
    int offset2;
    fseek(storage, destination_record->position + (2 + destination_record->attr_num + i) * sizeof(int), SEEK_SET);
    fread(&offset1, sizeof(int), 1, storage);
    // printf("catalog_res : %d %s ",destination_record->attr_ids[i], catalog_res->Key_Type);
    if (strcmp(catalog_res->Key_Type, dataTypes[2]) == 0)
      fread(&offset2, sizeof(int), 1, storage);
    fseek(storage, destination_record->position + offset1, SEEK_SET);
    if(strcmp(catalog_res->Key_Type, dataTypes[0]) == 0) {
      int data_int32;
      fread(&data_int32, sizeof(int), 1, storage);
      //printf("data_int32: %d\n", data_int32);
      values[i] = (char *)malloc(sizeof(char) * 20);
      sprintf(values[i], "%d", data_int32);
      // tostring
    } else if (strcmp(catalog_res->Key_Type, dataTypes[1]) == 0) {
      bool data_bool;
      fread(&data_bool, sizeof(int), 1, storage);
      //printf("data_bool: %d\n", data_bool);
      if(data_bool == 0) {
	values[i] = (char *)malloc(sizeof(char) * 6);
	strncpy(values[i], "false", sizeof("false"));
      } else {
	values[i] = (char *)malloc(sizeof(char) * 5);
	strncpy(values[i], "true", sizeof("true"));
      }
    } else if (strcmp(catalog_res->Key_Type, dataTypes[2]) == 0) {
      int length = offset2-offset1;
      char * data_string = (char *)malloc(sizeof(char) * (length + 3));

      fread(data_string, sizeof(char), length, storage);
      int j;
      for(j = length; j >= 0; j--) {
	data_string[j] = data_string[j-1];
      }
      data_string[0] = '"';
      strcat(data_string, "\"");
      //printf("data_string: %s\n", data_string);
      values[i] = data_string;
    } else if (strcmp(catalog_res->Key_Type, dataTypes[3]) == 0) {
      int data_array = NULL;
      fread(&data_array, sizeof(int), 1, storage);
      values[i] = Array_Materializer(file_index[data_array-1], storage);
    } else if (strcmp(catalog_res->Key_Type, dataTypes[4]) == 0) {
      int data_Obj;
      fread(&(data_Obj), sizeof(int), 1, storage);
      values[i] = Object_Materializer(data_Obj, file_index, catalog_index, storage);
    } else {
      printf("Materializer Type Error!\n");
    }
  }

  to_json(Json, Key_names, values, destination_record->attr_num);

  for(i = 0; i < destination_record->attr_num; i++) {
    free(values[i]);
    // note that Key_names[i] is just pointers point to Catalog
  }

  free(values);
  free(Key_names);
  
  return Json;
}

bool Materializer(file_position ** file_index, result ** head,
		  catalog_record * CATALOG, FILE * storage, int * record_count) {
  result * positioner = (*head);
  catalog_record ** catalog_index;
  catalog_record * catalog_res;
  catalog_index_build(CATALOG, &catalog_index);
  // int now = 0;
  while(positioner != NULL) {
    // printf("Materializer %d !\n", now++);
    if(positioner->match == false) {
      positioner = positioner->next;
      continue;
    }
    char * json = NULL;
    json = Object_Materializer(positioner->record_id, file_index, catalog_index, storage);
    printf("%s\n", json);
    (*record_count)++;
    // printf("position id: %d\n", positioner->record_id);
    free(json);
    positioner = positioner->next;
  }
}

bool find_value_in_file(file_position ** file_index, result ** head,const char * value, int attr_id,
			int typei, FILE * storage, catalog_record * CATALOG) {
  result * positioner = (*head);
  catalog_record ** catalog_index;
  catalog_index_build(CATALOG, &catalog_index);
  while(positioner != NULL) {
    file_position * destination_check_record = file_index[positioner->record_id-1];
    fseek(storage, destination_check_record->position
	  + (2 + destination_check_record->attr_num + positioner->attr_position) * sizeof(int), SEEK_SET);
    int offset, offset1;
    fread(&offset, sizeof(int), 1, storage);
    if(typei == 2) { 
      fread(&offset1, sizeof(int), 1, storage);
    }
    // printf("%d\n", offset);
    fseek(storage, destination_check_record->position+offset, SEEK_SET);
    if(typei == 0) {
      int data_int32;
      //printf("data_int32: %d\n", data_int32);
      char * s = (char *)malloc(sizeof(char) * 20);   
      fread(&data_int32, sizeof(int), 1, storage);
      sprintf(s, "%d", data_int32);
      if(strcmp(s, value) != 0) {
	// printf("value not match! %s %s\n", s, value);
	positioner->match = false;
      } else {
	// printf("value match! %s %s %d\n", s, value, positioner->record_id);
      }
      free(s);
    } else if(typei == 1) {
      char * s;
      bool data_bool;
      fread(&data_bool, sizeof(int), 1, storage);
      if(data_bool == 0) {
	s = (char *)malloc(sizeof(char) * 6);
	strncpy(s, "false", sizeof("false"));
      } else {
	s = (char *)malloc(sizeof(char) * 5);
	strncpy(s, "true", sizeof("true"));
      }
      if(strcmp(s, value) != 0) {
	positioner->match = false;
	// printf("value not match %s %s\n", s, value);
      }
      free(s);
    } else if(typei == 2) {
      int length = offset1 - offset;
      char * data_string = (char *)malloc(sizeof(char) * (length + 3));

      fread(data_string, sizeof(char), length, storage);
      int j;
      for(j = length; j >= 0; j--) {
	data_string[j] = data_string[j-1];
      }
      data_string[0] = '"';
      strcat(data_string, "\"");
      if(strcmp(data_string, value) != 0) {
	positioner->match = false;
	// printf("value not match %s %s\n", data_string, value);
      }
      free(data_string);
    } else if (typei == 3) {
      int data_array = NULL;
      fread(&data_array, sizeof(int), 1, storage);
      char * s = Array_Materializer(file_index[data_array-1], storage);
      if(strcmp(s, value) != 0) {
	positioner->match = false;
	// printf("value not match %s %s\n", s, value);
      }
      free(s);
    } else if (typei == 4) {
      int data_Obj;
      fread(&data_Obj, sizeof(int), 1, storage);
      char * s = Object_Materializer(data_Obj, file_index, catalog_index, storage);
      if(strcmp(s, value) != 0) {
	positioner->match = false;
	// printf("value not match %s %s\n", s, value);
      }
      free(s);
    } else {
      printf("Match Value Type Error!\n");
    }
    positioner = positioner->next;
  }
}

bool find(const char * Key_name, const char * value, catalog_record * CATALOG,
	  file_position ** file_index, int count, FILE * storage) {
  int attr_id;
  int typei;
  int record_count = 0;
  result * result_head = NULL;
  result * result_tail = result_head;
  // not completely!

  if(value == NULL || strlen(value) == 0) {
    printf("no value input \n");
    return false;
  } else if (value[0] == 't' || value[0] == 'T' ||
	     value[0] == 'f' || value[0] == 'F') {
    typei = 1;
  } else if (value[0] > '0' && value[0] < '9') {
    typei = 0;
  } else if (value[0] == '\"') {
    typei = 2;
  } else if (value[0] == '[') {
    typei = 3;
  } else if (value[0] == '{') {
    typei = 4;
  } else {
    printf("type error\n");
    return false; 
  }
  
  if(catalog_find_by_key(CATALOG, &attr_id, Key_name, dataTypes[typei])) {
    find_attr_in_file(&result_head, &result_tail, file_index, count, attr_id);
    find_value_in_file(file_index, &result_head,
		       value, attr_id, typei, storage, CATALOG);
    Materializer(file_index, &result_head, CATALOG, storage, &record_count);
  }
  printf("\nrecord find: %d\n", record_count);
}

