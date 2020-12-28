#include <AppCore/CAPI.h>
#include <JavaScriptCore/JavaScript.h>
#include <stdlib.h>
#include <stdio.h>

ULApp app = 0;
ULWindow window = 0;
ULOverlay overlay = 0;
ULView view = 0;

ULString devs;
ULString appName;
char *app_Name;

void RunMenu();
void OnUpdate(void *user_data);
void OnResize(void *user_data, unsigned int width, unsigned int height);
void OnDOMReady(void *user_data, ULView caller, unsigned long long frame_id,
                bool is_main_frame, ULString url);
void Shutdown();

char *ULStringToC(ULString string);

int main()
{
  RunMenu();
  Shutdown();

  return 0;
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

void RunMenu()
{
  devs = ulCreateString("Igor <igorjunior@protonmail.com & Arthur<>");
  appName = ulCreateString("Calculator");
  app_Name = ULStringToC(appName);

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
  ulSetWindowMinSize(window, 350, 500);

  overlay = ulCreateOverlay(window, ulWindowGetWidth(window), ulWindowGetHeight(window), 0, 0);
  view = ulOverlayGetView(overlay);
  ulViewSetDOMReadyCallback(view, OnDOMReady, 0);
  ULString url = ulCreateString("file:///app.html");
  ulViewLoadURL(view, url);
  ulDestroyString(url);
  ulAppRun(app);
}

void OnUpdate(void *user_data)
{
}

void OnResize(void *user_data, unsigned int width, unsigned int height)
{
  ulOverlayResize(overlay, width, height);
}

JSValueRef GetMessage(JSContextRef ctx, JSObjectRef function,
                      JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[],
                      JSValueRef *exception)
{

  JSStringRef str = JSStringCreateWithUTF8CString("Hello from C!");
  JSValueRef value = JSValueMakeString(ctx, str);
  JSStringRelease(str);
  return value;
}

void OnDOMReady(void *user_data, ULView caller, unsigned long long frame_id,
                bool is_main_frame, ULString url)
{
  JSContextRef ctx = ulViewLockJSContext(view);
  JSStringRef name = JSStringCreateWithUTF8CString("GetMessage");
  JSObjectRef func = JSObjectMakeFunctionWithCallback(ctx, name, GetMessage);
  JSObjectSetProperty(ctx, JSContextGetGlobalObject(ctx), name, func, 0, 0);
  JSStringRelease(name);
  ulViewUnlockJSContext(view);
}

void Shutdown()
{
  ulDestroyString(devs);
  ulDestroyString(appName);
  free(app_Name);
  ulDestroyOverlay(overlay);
  ulDestroyWindow(window);
  ulDestroyApp(app);
}