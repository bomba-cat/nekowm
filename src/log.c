#include "headers/neko.h"

FILE *log_file = NULL;

#ifdef LOG
static char log_tag[4][25] =
{
  "[INFO]",
  "[WARNING]",
  "[ERROR]",
  "[SEVERE]",
};
#endif

void neko_log_init()
{
  #ifdef LOG
  log_file = fopen(LOG_PATH, "w");
  #else
  return;
  #endif
}

void neko_log(char* message, neko_log_flag flag)
{
  #ifdef LOG
  if (log_file)
  {
    fprintf(log_file, "%s %s\r\n", log_tag[flag], message);
    fflush(log_file);
  }
  #else
  UNUSED(message);
  UNUSED(flag);
  return;
  #endif
}
