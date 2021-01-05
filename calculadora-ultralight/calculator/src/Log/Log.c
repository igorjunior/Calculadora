#include <stdio.h>
#include <stdlib.h>
#include "Log.h"

void addLog(double number1, double number2, int operation, double result)
{
  FILE *file;
  file = fopen("calculator.log", "a");
  if (file == NULL)
  {
    fprintf(stderr, "\nError opening file\n");
    exit(1);
  }
  Log new_log;
  new_log.n1 = number1;
  new_log.n2 = number2;
  new_log.o = operation;
  new_log.r = result;
  fwrite(&new_log, sizeof(Log), 1, file);
  fclose(file);
}

Log *readLogs(int *size)
{
  Log *inputs = NULL;
  Log input;
  int counter = 0;
  FILE *file;
  file = fopen("calculator.log", "rw");
  if (file == NULL)
  {
    fprintf(stderr, "\nError opening file\n");
    exit(1);
  }
  while (fread(&input, sizeof(Log), 1, file))
  {
    inputs = realloc(inputs, (counter + 1) * sizeof(Log));
    inputs[counter] = input;
    counter++;
  }
  fclose(file);
  *size = counter;
  return inputs;
}

void clearLog()
{
  fclose(fopen("calculator.log", "w"));
}