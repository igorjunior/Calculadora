#ifndef MAIN_C_INCLUDE
#define MAIN_C_INCLUDE
#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include "Engine/Engine.h"

int main()
{
  SetDevs("Igor<igorjunior@pm.me> | Arthur<arthur-er@pm.me>");
  SetAppName("Calculator");
  SetMenuMinSize(400, 700);
  RunMenu(400, 700);
  ShutDownMenu();
  return 0;
}

#endif