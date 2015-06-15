#include <stdio.h>
#include <string.h>
#include "Json.h"
#include <malloc.h>

json_parser * ParseJson(char * JsonStr, int * len) {

  (*len) = 0;
  
  if(JsonStr == NULL)
    return NULL;

  // check_json();
  
  int i = 2;
  int length = strlen(JsonStr);

  json_parser * head;

  if(JsonStr[0] == '{' && JsonStr[1] == '"') {
    head = (json_parser *)malloc(sizeof(json_parser));
    head->start = 1;
    while(JsonStr[i] != '"')
      i++;
    head->end = i;
    head->next = NULL;
    head->type = 0;
    head->dtype = 2;
    (*len)++;
  } else {
    return NULL;
  }

  json_parser * positioner = head;
  json_parser * temp = NULL;
  
  for(i; i < length; i++) {
    if(JsonStr[i] == ':') {
      i++;
      while(JsonStr[i] == ' ') {
	i++;
      }
      
      temp = (json_parser *)malloc(sizeof(json_parser));
      temp->start = i;
      
      if(JsonStr[i] == '{') {
	while(JsonStr[i] != '}')
	  i++;
	i++;
      } else if (JsonStr[i] == '[') {
	while(JsonStr[i] != ']')
	  i++;
	i++;
      } else {
	while(JsonStr[i] != ',' && JsonStr[i] != '}')
	  i++;
      }

      if(JsonStr[temp->start] >= '0' && JsonStr[temp->start] <= '9') {
	temp->dtype = 0;
      } else if (JsonStr[temp->start] == '"') {
	temp->dtype = 2;
      } else if (JsonStr[temp->start] == '[') {
	temp->dtype = 3;
      } else if (JsonStr[temp->start] == '{') {
	temp->dtype = 4;
      } else if (JsonStr[temp->start] == 't' || JsonStr[temp->start] == 'T'
		 || JsonStr[temp->start] == 'f' || JsonStr[temp->start] == 'F') {
	temp->dtype = 1;
      } else {
	temp->dtype = 5;
      }
      
      temp->end = i - 1;
      temp->type = 1;
      temp->next = NULL;
      positioner->next = temp;
      positioner = temp;
      
      i--;
      (*len)++;
    } else if(JsonStr[i] == ',') {
      i++;
      while(JsonStr[i] == ' ') {
	i++;
      }
      
      temp = (json_parser *)malloc(sizeof(json_parser));
      temp->start = i;
      temp->dtype = 2;
      while(JsonStr[i] != ':')
	i++;
      temp->end = i - 1;
      temp->type = 0;
      temp->next = NULL;
      positioner->next = temp;
      positioner = temp;
      i--;
      (*len)++;
    }
  }
  return head;
}

extern char * to_json(char ** Key_names, char ** values, int property_amount) {
  int position = 0;

  const char * comma = ",";
  const char * quote = "\"";
  const char * colon = ":";
  const char * left_brace = "{";
  const char * right_brace = "}";

  char * json[10000];
  
  int i;
  strcat(json, left_brace);
  
  for(i = 0; i < property_amount; i++) {
    strcat(json, quote);
    strcat(json, Key_names[i]);
    strcat(json, quote);
    strcat(json, colon);
    strcat(json, quote);
    strcat(json, values[i]);
    strcat(json, quote);
    if(i != property_amount-1)
      strcat(json, comma);
  }

  strcat(json, right_brace);
}

void DeleteJson(json_parser * head) {
  json_parser * positioner = head;
  json_parser * to_delete;

  while(positioner != NULL) {
    to_delete = positioner;
    positioner = positioner->next;
    free(to_delete);
  }
}
