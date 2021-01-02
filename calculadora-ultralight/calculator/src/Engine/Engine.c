#ifndef ENGINE_C_INCLUDE
#define ENGINE_C_INCLUDE
#include <Ultralight/CAPI.h>
#include <AppCore/CAPI.h>
#include <JavaScriptCore/JavaScript.h>
#include "Engine.h"
#include <stdlib.h>
#include <stdio.h>

typedef enum
{
  SUM,
  SUB,
  NON
} operations;

operations currentOperation = -1;
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

char *ULStringToC(ULString string);

void OnDOMReady(void *user_data, ULView caller, unsigned long long frame_id,
                bool is_main_frame, ULString url);

void OnUpdate(void *user_data)
{
}

void OnResize(void *user_data, unsigned int width, unsigned int height)
{
  ulOverlayResize(overlay, width, height);
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

JSValueRef NumberClick(JSContextRef ctx, JSObjectRef function,
                       JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[],
                       JSValueRef *exception)
{
  printf("Number Click: ");
  if (argumentCount == 1)
  {
    if (JSValueIsNumber(ctx, arguments[0]))
    {
      double n = JSValueToNumber(ctx, arguments[0], 0);
      printf("%.2lf ", n);
      if (currentOperation == SUM)
      {
        result += n;
        printf("SUM: %.2lf", result);
      }
      else if (currentOperation == SUB)
      {
        result -= n;
        printf("SUB: %.2lf", result);
      }
      else if (currentOperation == -1)
      {
        result = n;
        printf("RES: %.2lf", result);
      }
      currentOperation = NON;
    }
  }
  printf("\n");
}

JSValueRef OperationClick(JSContextRef ctx, JSObjectRef function,
                          JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[],
                          JSValueRef *exception)
{
  printf("Operation click: ");
  if (JSValueIsNumber(ctx, arguments[0]))
  {
    int n = (int)JSValueToNumber(ctx, arguments[0], 0);
    currentOperation = n;
    printf("%d ", currentOperation);
  }
  printf("\n");
}

void OnDOMReady(void *user_data, ULView caller, unsigned long long frame_id,
                bool is_main_frame, ULString url)
{
  JSContextRef ctx = ulViewLockJSContext(view);
  JSStringRef name = JSStringCreateWithUTF8CString("NumberClick");
  JSObjectRef func = JSObjectMakeFunctionWithCallback(ctx, name, NumberClick);
  JSObjectSetProperty(ctx, JSContextGetGlobalObject(ctx), name, func, 0, 0);
  JSStringRelease(name);
  name = JSStringCreateWithUTF8CString("OperationClick");
  func = JSObjectMakeFunctionWithCallback(ctx, name, OperationClick);
  JSObjectSetProperty(ctx, JSContextGetGlobalObject(ctx), name, func, 0, 0);
  JSStringRelease(name);
  ulViewUnlockJSContext(view);
}

#endif