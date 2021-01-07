#include <Ultralight/CAPI.h>
#include <AppCore/CAPI.h>
#include <JavaScriptCore/JavaScript.h>
#include "Engine.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../Log/Log.h"
//ENUMS
typedef enum
{
  SUM,
  SUB,
  MULT,
  DIV,
  INVERT,
  POWER,
  CLEAR,
  NON
} operations;

//VARS
double result = 0.0;
ULApp app = 0;
ULWindow window = 0;
ULOverlay overlay = 0;
ULView view = 0;
ULString devs;
ULString appName;
char *app_Name = "";
int minX = 0;
int minY = 0;
int maxX = 0;
int maxY = 0;

//DECLARATIONS
char *ULStringToC(ULString string);
double power(double n1, double n2);
void OnDOMReady(void *user_data, ULView caller, unsigned long long frame_id,
                bool is_main_frame, ULString url);
void OnUpdate(void *user_data);
void OnResize(void *user_data, unsigned int width, unsigned int height);
JSValueRef Operation(JSContextRef ctx, JSObjectRef function,
                     JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[],
                     JSValueRef *exception);
JSValueRef FlushLog(JSContextRef ctx, JSObjectRef function,
                    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[],
                    JSValueRef *exception);
JSValueRef SetResult(JSContextRef ctx, JSObjectRef function,
                     JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[],
                     JSValueRef *exception);

//UL FUNCTIONS
void OnUpdate(void *user_data)
{
}
void OnResize(void *user_data, unsigned int width, unsigned int height)
{
  ulOverlayResize(overlay, width, height);
}
void OnDOMReady(void *user_data, ULView caller, unsigned long long frame_id,
                bool is_main_frame, ULString url)
{
  JSContextRef ctx = ulViewLockJSContext(view);
  ULString script = ulCreateString("clearLog()");
  ulViewEvaluateScript(view, script, 0);
  ulDestroyString(script);

  int size, i;
  Log *logs = readLogs(&size);
  for (i = 0; i < size; i++)
  {
    char out[999];
    memset(out, 0, sizeof(out));
    strcat(out, "addLog(");
    char resultNumber[80];
    sprintf(resultNumber, "%.2lf", logs[i].n1);
    strcat(out, resultNumber);
    strcat(out, ",");
    sprintf(resultNumber, "%.2lf", logs[i].n2);
    strcat(out, resultNumber);
    strcat(out, ",");
    sprintf(resultNumber, "%d", logs[i].o);
    strcat(out, resultNumber);
    strcat(out, ",");
    sprintf(resultNumber, "%.2lf", logs[i].r);
    strcat(out, resultNumber);
    strcat(out, ")");
    printf("%s\n", out);
    script = ulCreateString(out);
    ulViewEvaluateScript(view, script, 0);
    ulDestroyString(script);
  }

  JSStringRef name = JSStringCreateWithUTF8CString("Operation");
  JSObjectRef func = JSObjectMakeFunctionWithCallback(ctx, name, Operation);
  JSObjectSetProperty(ctx, JSContextGetGlobalObject(ctx), name, func, 0, 0);
  JSStringRelease(name);

  name = JSStringCreateWithUTF8CString("FlushLog");
  func = JSObjectMakeFunctionWithCallback(ctx, name, FlushLog);
  JSObjectSetProperty(ctx, JSContextGetGlobalObject(ctx), name, func, 0, 0);
  JSStringRelease(name);

  name = JSStringCreateWithUTF8CString("SetResult");
  func = JSObjectMakeFunctionWithCallback(ctx, name, SetResult);
  JSObjectSetProperty(ctx, JSContextGetGlobalObject(ctx), name, func, 0, 0);
  JSStringRelease(name);
  ulViewUnlockJSContext(view);
}

//JS FUNCTIONS
JSValueRef Operation(JSContextRef ctx, JSObjectRef function,
                     JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[],
                     JSValueRef *exception)
{
  if (argumentCount == 2)
  {
    int operation = (int)JSValueToNumber(ctx, arguments[0], 0);
    double number = JSValueToNumber(ctx, arguments[1], 0);
    double old_result = result;
    char out[999];
    memset(out, 0, sizeof(out));
    strcat(out, "addLog(");
    char resultNumber[80];
    sprintf(resultNumber, "%.2lf", result);
    strcat(out, resultNumber);
    strcat(out, ",");
    sprintf(resultNumber, "%.2lf", number);
    strcat(out, resultNumber);
    if (operation != NON)
    {
      if (operation == SUM)
      {
        if (result != 0)
        {
          result += number;
          strcat(out, ",0,");
          sprintf(resultNumber, "%.2lf", result);
          strcat(out, resultNumber);
          strcat(out, ")");
          ULString script = ulCreateString(out);
          ulViewEvaluateScript(view, script, 0);
          ulDestroyString(script);
          addLog(old_result, number, operation, result);
        }
        else
        {
          result += number;
        }
      }
      else if (operation == SUB)
      {
        if (result != 0)
        {
          result -= number;
          strcat(out, ",1,");
          sprintf(resultNumber, "%.2lf", result);
          strcat(out, resultNumber);
          strcat(out, ")");
          ULString script = ulCreateString(out);
          ulViewEvaluateScript(view, script, 0);
          ulDestroyString(script);
          addLog(old_result, number, operation, result);
        }
        else
        {
          result -= number;
        }
      }
      else if (operation == MULT)
      {
        result *= number;
        strcat(out, ",2,");
        sprintf(resultNumber, "%.2lf", result);
        strcat(out, resultNumber);
        strcat(out, ")");
        ULString script = ulCreateString(out);
        ulViewEvaluateScript(view, script, 0);
        ulDestroyString(script);
        addLog(old_result, number, operation, result);
      }
      else if (operation == DIV)
      {
        result /= number;
        strcat(out, ",3,");
        sprintf(resultNumber, "%.2lf", result);
        strcat(out, resultNumber);
        strcat(out, ")");
        ULString script = ulCreateString(out);
        ulViewEvaluateScript(view, script, 0);
        ulDestroyString(script);
        addLog(old_result, number, operation, result);
      }
      else if (operation == POWER)
      {
        result = power(result, number);
        strcat(out, ",5,");
        sprintf(resultNumber, "%.2lf", result);
        strcat(out, resultNumber);
        strcat(out, ")");
        ULString script = ulCreateString(out);
        ulViewEvaluateScript(view, script, 0);
        ulDestroyString(script);
        addLog(old_result, number, operation, result);
      }
      else if (operation == INVERT)
      {
        result *= -1;
      }
      else if (operation == CLEAR)
      {
        result = 0.0f;
      }
    }
    JSValueRef returnValue = JSValueMakeNumber(ctx, result);
    return returnValue;
  }
}

JSValueRef FlushLog(JSContextRef ctx, JSObjectRef function,
                    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[],
                    JSValueRef *exception)
{
  clearLog();
  ULString script = ulCreateString("clearLog()");
  ulViewEvaluateScript(view, script, 0);
  ulDestroyString(script);
}

JSValueRef SetResult(JSContextRef ctx, JSObjectRef function,
                     JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[],
                     JSValueRef *exception)
{
  result = JSValueToNumber(ctx, arguments[0], 0);
}

//UTILS FUNCTIONS
double power(double n1, double n2)
{
  double result = 1;
  while (n2 != 0)
  {
    result *= n1;
    --n2;
  }
  return result;
}
void RunMenu(int width, int height)
{
  devs = ulCreateString("");
  appName = ulCreateString("");
  ULSettings settings = ulCreateSettings();
  ulSettingsSetForceCPURenderer(settings, true);
  ulSettingsSetDeveloperName(settings, devs);
  ulSettingsSetAppName(settings, appName);
  ULConfig config = ulCreateConfig();
  app = ulCreateApp(settings, config);
  ulAppSetUpdateCallback(app, OnUpdate, 0);
  ulDestroySettings(settings);
  ulDestroyConfig(config);

  window = ulCreateWindow(ulAppGetMainMonitor(app), appName, 350, 500, false,
                          kWindowFlags_Titled | kWindowFlags_Borderless);
  ulWindowSetTitle(window, app_Name);
  ulWindowSetResizeCallback(window, OnResize, 0);
  ulAppSetWindow(app, window);
  if (minX || minY)
    ulSetWindowMinSize(window, minX, minY);
  if (maxX || maxY)
    ulSetWindowMaxSize(window, maxX, maxY);

  overlay = ulCreateOverlay(window, ulWindowGetWidth(window), ulWindowGetHeight(window), 0, 0);
  view = ulOverlayGetView(overlay);
  ulViewSetDOMReadyCallback(view, OnDOMReady, 0);
  ULString url = ulCreateString("file:///app.html");
  ulViewLoadURL(view, url);
  ulDestroyString(url);
  ulAppRun(app);
}
void ShutDownMenu()
{
  ulDestroyString(devs);
  ulDestroyString(appName);
  free(app_Name);
  ulDestroyOverlay(overlay);
  ulDestroyWindow(window);
  ulDestroyApp(app);
}
char *ULStringToC(ULString string)
{
  char *out = malloc(sizeof(char) * ulStringGetLength(string));
  ULChar16 *data = ulStringGetData(string);
  int i;
  for (i = 0; i < ulStringGetLength(string); i++)
  {
    out[i] = data[i];
  }
  return out;
}
void SetDevs(const char *devs_)
{
  devs = ulCreateString(devs_);
}
void SetAppName(const char *appName_)
{
  appName = ulCreateString(appName_);
  app_Name = ULStringToC(appName);
}
void SetMenuMinSize(int width, int height)
{
  minX = width;
  minY = height;
}
void SetMenuMaxSize(int width, int height)
{
  maxX = width;
  maxY = height;
}