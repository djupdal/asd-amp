#include "asd_pre_1.h"
#include "ff.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "microsd.h"
#include "diskio.h"

struct configLine {
  char id[82];
  char value[82];
  struct configLine *next;
};

struct configLine *config = NULL;
struct configLine *lastConfig = NULL;

static void printConfig(void) {
  printf("Current config:\n");
  
  struct configLine *c = config;

  while(c) {
    if(c->id[0]) {
      printf("%s", c->id);
      if(c->value[0]) {
        printf(" %s", c->value);
      }
    }
    printf("\n");
    c = c->next;
  }
}

void configInit(void) {
  FIL fil;
  FRESULT fr;
  char line[82];

  fr = f_open(&fil, "config.txt", FA_READ);
  if (fr) return;

  struct configLine **c = &config;

  while(f_gets(line, 82, &fil)) {
    struct configLine *cLine = malloc(sizeof(struct configLine));
    cLine->next = NULL;
    *c = cLine;
    lastConfig = cLine;
    c = &cLine->next;

    if(line[0] == '#') {
      // comment, store it all
      strcpy(cLine->id, line);
      *strchr(cLine->id, '\n') = 0;
      cLine->value[0] = 0;
    } else if(isspace(line[0])) {
      // white space, just insert blank line
      cLine->id[0] = 0;
    } else {
      // id
      char *token = strtok(line, " \t\n");
      if(token) strcpy(cLine->id, token);
      else cLine->id[0] = 0;

      // value
      token = strtok(NULL, " \t\n");
      if(token) strcpy(cLine->value, token);
      else cLine->value[0] = 0;
    }
  }

  printConfig();

  f_close(&fil);
}

void flushConfig(void) {
  FIL fil;
  FRESULT fr;

  //printConfig();

  fr = f_open(&fil, "config.txt", FA_WRITE | FA_CREATE_ALWAYS);
  if (fr) return;

  struct configLine *c = config;

  while(c) {
    if(c->id[0]) {
      f_printf(&fil, "%s", c->id);
      if(c->value[0]) {
        f_printf(&fil, " %s", c->value);
      }
    }
    f_printf(&fil, "\n");
    c = c->next;
  }

  f_close(&fil);
}

struct configLine *getLine(char *id) {
  struct configLine *c = config;

  while(c) {
    if(!strcmp(id, c->id)) {
      return c;
    } else c = c->next;
  }
  return NULL;
}

uint32_t getUint32(char *id) {
  struct configLine *c = getLine(id);
  if(c) return strtol(c->value, NULL, 0);
  return 0;
}

char *getString(char *id) {
  struct configLine *c = getLine(id);
  if(c) return c->value;
  return NULL;
}

void setUint32(char *id, uint32_t val) {
  char buf[82];
  sprintf(buf, "%u", (unsigned int)val);
  setString(id, buf);
}

void setString(char *id, char *s) {
  struct configLine *c = getLine(id);
  if(c) {
    strcpy(c->value, s);
  } else {
    c = malloc(sizeof(struct configLine));
    c->next = NULL;
    strcpy(c->id, id);
    strcpy(c->value, s);
    lastConfig->next = c;
    lastConfig = c;
  }
}
