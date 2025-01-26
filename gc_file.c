#include "gc_file.h"

bool gc_file_exists (char *filename)
{
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}