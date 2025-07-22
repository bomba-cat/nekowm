#include "headers/neko.h"

FILE *log_file = NULL;

static char log_tag[4][25] =
{
  "[INFO]",
  "[WARNING]",
  "[ERROR]",
  "[SEVERE]",
};

void neko_log_init()
{
  log_file = fopen(LOG_PATH, "w");
}

void neko_log(char* message, neko_log_flag flag)
{ 
  fprintf(log_file, "%s %s\r\n", log_tag[flag], message);
}
