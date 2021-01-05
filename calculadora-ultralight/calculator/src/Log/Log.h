typedef struct
{
  double n1;
  double n2;
  int o;
  double r;
} Log;
void addLog(double number1, double number2, int operation, double result);
Log *readLogs(int *size);
void clearLog();