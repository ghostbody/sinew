#include <stdio.h>
#include <malloc.h>
#include "Find.h"
#include "Global.h"

/*
typedef struct file_position {
  unsigned int id;
  long position;
  unsigned int size;
  file_position * next;
} file_position;
*/

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
    positioner->attr_ids = (int *)malloc(sizeof(int) * attr_num);
    fread(positioner->attr_ids, sizeof(int), attr_num, storage);
    /*
    for(i = 0; i < positioner->attr_num; i++) {
      printf("%d ", positioner->attr_ids[i]);
      }
    */
    printf("%d", (positioner->attr_ids)[0]);
    printf("\n");
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
