#include <stdio.h>
#include <stdlib.h>
#include "Global.h"
#include "Find.h"

int comp(const void*a,const void*b) {
  return (*(file_position **)a)->id - (*(file_position **)b)->id;
}

void test_build_data_file_index() {
  FILE * f = fopen(DATA_FILE, "rb");
  file_position **file_index;
  int count;
  
  count = build_data_file_index(&file_index, f);
  
  int i, j;

  qsort(file_index, count, sizeof(file_position *), comp);
  
  for(i = 0; i < count; i++) {
    printf("id:%d position:%ld size:%d attr_num:%d ",
	   file_index[i]->id,
	   file_index[i]->position,
	   file_index[i]->size,
	   file_index[i]->attr_num
	   );
    for(j = 0; j < (file_index[i]->attr_num); j++) {
      printf("%d ", (file_index[i]->attr_ids)[j]);
    }
    printf("\n");
  }
}

int main() {
  test_build_data_file_index();
  return 0;
}
