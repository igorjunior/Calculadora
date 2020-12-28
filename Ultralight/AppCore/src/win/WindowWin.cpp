#include "WindowWin.h"
#include <AppCore/Monitor.h>
#include <Windows.h>
#include <ShellScalingAPI.h>
#include "Windowsx.h"
#include <tchar.h>
#include "AppWin.h"
#include "DIBSurface.h"

namespace ultralight {

#define WINDOWDATA() ((WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA))
#define WINDOW() (WINDOWDATA()->window)

  static HDC g_dc = 0;

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT ps;
  HDC hdc;
  switch (message) {
  case WM_PAINT:
    g_dc = hdc = BeginPaint(hWnd, &ps);
    static_cast<AppWin*>(App::instance())->InvalidateWindow();
    static_cast<AppWin*>(App::instance())->OnPaint();
    EndPaint(hWnd, &ps);
    g_dc = 0;
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_ENTERSIZEMOVE:
    WINDOWDATA()->is_resizing_modal = true;
    break;
  case WM_SIZE:
  {
    if (WINDOWDATA()) {
      WINDOW()->OnResize(WINDOW()->width(), WINDOW()->height());
      // This would normally be called when the message loop is idle
      // but during resize the window consumes all messages so we need
      // to force paints during the operation.
      //static_cast<AppWin*>(App::instance())->OnPaint();
      InvalidateRect(hWnd, nullptr, false);
    }
    break;
  }
  case WM_EXITSIZEMOVE:
    WINDOWDATA()->is_resizing_modal = false;
    WINDOW()->OnResize(WINDOW()->width(), WINDOW()->height());
    InvalidateRect(hWnd, nullptr, false);
    break;
  case WM_KEYDOWN:
    WINDOW()->FireKeyEvent(KeyEvent(KeyEvent::kType_RawKeyDown, (uintptr_t)wParam, (intptr_t)lParam, false));
    break;
  case WM_KEYUP:
    WINDOW()->FireKeyEvent(KeyEvent(KeyEvent::kType_KeyUp, (uintptr_t)wParam, (intptr_t)lParam, false));
    break;
  case WM_CHAR:
    WINDOW()->FireKeyEvent(KeyEvent(KeyEvent::kType_Char, (uintptr_t)wParam, (intptr_t)lParam, false));
    break;
  case WM_MOUSELEAVE:
    WINDOWDATA()->is_mouse_in_client = false;
    WINDOWDATA()->is_tracking_mouse = false;
    break;
  case WM_MOUSEMOVE:
  {
    if (!WINDOWDATA()->is_tracking_mouse) {
      // Need to install tracker to get WM_MOUSELEAVE events.
      WINDOWDATA()->track_mouse_event_data = { sizeof(WINDOWDATA()->track_mouse_event_data) };
      WINDOWDATA()->track_mouse_event_data.dwFlags = TME_LEAVE;
      WINDOWDATA()->track_mouse_event_data.hwndTrack = hWnd;
      TrackMouseEvent(&(WINDOWDATA()->track_mouse_event_data));
      WINDOWDATA()->is_tracking_mouse = true;
    }
    if (!WINDOWDATA()->is_mouse_in_client) {
      // We need to manually set the cursor when mouse enters client area
      WINDOWDATA()->is_mouse_in_client = true;
      WINDOW()->SetCursor(ultralight::kCursor_Pointer);
    }
    WINDOW()->FireMouseEvent(
      { MouseEvent::kType_MouseMoved,
        WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)),
        WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)),
        WINDOWDATA()->cur_btn });
    break;
  }
  case WM_LBUTTONDOWN:
  case WM_LBUTTONDBLCLK:
    SetCapture(WINDOW()->hwnd());
    WINDOWDATA()->cur_btn = MouseEvent::kButton_Left;
    WINDOW()->FireMouseEvent(
      { MouseEvent::kType_MouseDown,
        WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)),
        WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)),
        WINDOWDATA()->cur_btn });
    break;
  case WM_MBUTTONDOWN:
  case WM_MBUTTONDBLCLK:
    SetCapture(WINDOW()->hwnd());
    WINDOWDATA()->cur_btn = MouseEvent::kButton_Middle;
    WINDOW()->FireMouseEvent(
    { MouseEvent::kType_MouseDown,
      WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)),
      WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)),
      WINDOWDATA()->cur_btn });
    break;
  case WM_RBUTTONDOWN:
  case WM_RBUTTONDBLCLK:
    SetCapture(WINDOW()->hwnd());
    WINDOWDATA()->cur_btn = MouseEvent::kButton_Right;
    WINDOW()->FireMouseEvent(
    { MouseEvent::kType_MouseDown,
      WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)),
      WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)),
      WINDOWDATA()->cur_btn });
    break;
  case WM_LBUTTONUP:
  case WM_MBUTTONUP:
  case WM_RBUTTONUP:
    ReleaseCapture();
    WINDOW()->FireMouseEvent(
    { MouseEvent::kType_MouseUp,
      WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)),
      WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)),
      WINDOWDATA()->cur_btn });
	WINDOWDATA()->cur_btn = MouseEvent::kButton_None;
    break;
  case WM_MOUSEWHEEL:
    WINDOW()->FireScrollEvent(
      { ScrollEvent::kType_ScrollByPixel, 0, static_cast<int>(WINDOW()->PixelsToDevice(GET_WHEEL_DELTA_WPARAM(wParam)) * 0.8) });
    break;
  case WM_SETFOCUS:
    WINDOW()->SetWindowFocused(true);
    break;
  case WM_KILLFOCUS:
    WINDOW()->SetWindowFocused(false);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

void CenterHwndOnMainMonitor(HWND hwnd)
{
  RECT rect;
  GetWindowRect(hwnd, &rect);
  LPRECT prc = &rect;

  // Get main monitor
  HMONITOR hMonitor = MonitorFromPoint({ 1,1 }, MONITOR_DEFAULTTONEAREST);

  MONITORINFO mi;
  RECT        rc;
  int         w = prc->right - prc->left;
  int         h = prc->bottom - prc->top;

  mi.cbSize = sizeof(mi);
  GetMonitorInfo(hMonitor, &mi);

  rc = mi.rcMonitor;

  prc->left = rc.left + (rc.right - rc.left - w) / 2;
  prc->top = rc.top + (rc.bottom - rc.top - h) / 2;
  prc->right = prc->left + w;
  prc->bottom = prc->top + h;

  SetWindowPos(hwnd, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

WindowWin::WindowWin(Monitor* monitor, uint32_t width, uint32_t height,
  bool fullscreen, unsigned int window_flags) : monitor_(monitor), is_fullscreen_(fullscreen) {

  TCHAR* class_name = _T("UltralightWindow");

  HINSTANCE hInstance = GetModuleHandle(NULL);

  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
  wcex.hCursor = NULL;
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = class_name;
  wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_APPLICATION);
  if (!RegisterClassEx(&wcex)) {
    MessageBoxW(NULL, (LPCWSTR)L"RegisterClassEx failed", (LPCWSTR)L"Notification", MB_OK);
    exit(-1);
  }

  DWORD style = WS_SYSMENU;
  if (window_flags & kWindowFlags_Borderless)
    style |= WS_POPUP;
  else
    style |= WS_BORDER;
  if (window_flags & kWindowFlags_Titled)
    style |= WS_CAPTION;
  if (window_flags & kWindowFlags_Resizable)
    style |= WS_SIZEBOX;
  if (window_flags & kWindowFlags_Maximizable)
    style |= WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

  // Create window
  RECT rc = { 0, 0, (LONG)DeviceToPixels(width), (LONG)DeviceToPixels(height) };
  AdjustWindowRect(&rc, style, FALSE);
  hwnd_ = ::CreateWindowEx(NULL
    , class_name
    , _T("")
    , fullscreen ? (WS_EX_TOPMOST | WS_POPUP) : style
    , fullscreen ? 0 : CW_USEDEFAULT
    , fullscreen ? 0 : CW_USEDEFAULT
    , fullscreen ? DeviceToPixels(width) : (rc.right - rc.left)
    , fullscreen ? DeviceToPixels(height) : (rc.bottom - rc.top)
    , NULL
    , NULL
    , hInstance
    , NULL);

  if (!hwnd_) {
    MessageBoxW(NULL, (LPCWSTR)L"CreateWindowEx failed", (LPCWSTR)L"Notification", MB_OK);
    exit(-1);
  }

  window_data_.window = this;
  window_data_.cur_btn = ultralight::MouseEvent::kButton_None;
  window_data_.is_resizing_modal = false;
  window_data_.is_mouse_in_client = false;
  window_data_.is_tracking_mouse = false;

  SetWindowLongPtr(hwnd_, GWLP_USERDATA, (LONG_PTR)&window_data_);

  CenterHwndOnMainMonitor(hwnd_);

  ShowWindow(hwnd_, SW_SHOW);

  // Set the thread affinity mask for better clock
  ::SetThreadAffinityMask(::GetCurrentThread(), 1);

  cursor_hand_ = ::LoadCursor(NULL, IDC_HAND);
  cursor_arrow_ = ::LoadCursor(NULL, IDC_ARROW);
  cursor_ibeam_ = ::LoadCursor(NULL, IDC_IBEAM);
  cur_cursor_ = ultralight::kCursor_Hand;

  SetWindowScale(scale());
}

WindowWin::~WindowWin() {
  DestroyCursor(cursor_hand_);
  DestroyCursor(cursor_arrow_);
  DestroyCursor(cursor_ibeam_);
}

uint32_t WindowWin::width() const {
  RECT rc;
  ::GetClientRect(hwnd_, &rc);
  return rc.right - rc.left;
}

uint32_t WindowWin::height() const {
  RECT rc;
  ::GetClientRect(hwnd_, &rc);
  return rc.bottom - rc.top;
}

double WindowWin::scale() const {
  return monitor_->scale();
}

void WindowWin::SetTitle(const char* title) {
  SetWindowTextA(hwnd_, title);
}

void WindowWin::SetCursor(ultralight::Cursor cursor) {
  switch (cursor) {
  case ultralight::kCursor_Hand: {
    ::SetCursor(cursor_hand_);
    break;
  }
  case ultralight::kCursor_Pointer: {
    ::SetCursor(cursor_arrow_);
    break;
  }
  case ultralight::kCursor_IBeam: {
    ::SetCursor(cursor_ibeam_);
    break;
  }
  };

  cur_cursor_ = cursor;
}

void WindowWin::Close() {
  DestroyWindow(hwnd_);
}

/*
void WindowWin::DrawBitmap(int x, int y, Ref<Bitmap> bitmap, IntRect rect) {
  if (bitmap->IsEmpty())
    return;

  //HDC hdc = GetDC(hwnd_);
  if (!g_dc) return;
  HDC hdc = g_dc;
  void* bits = bitmap->LockPixels();
  BITMAPINFO bmi = {};
  bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
  bmi.bmiHeader.biWidth = static_cast<LONG>(bitmap->width());
  bmi.bmiHeader.biHeight = -static_cast<LONG>(bitmap->height());
  bmi.bmiHeader.biBitCount = bitmap->bpp() * 8;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biSizeImage = static_cast<DWORD>(bitmap->size());

  // TODO: Handle double buffering properly. For now ignore dirty rect bounds
  //       and force repaint of entire bitmap.
  rect = { 0, 0, (int)bitmap->width(), (int)bitmap->height() };

  LONG adjustedSourceTop = bitmap->height() - rect.bottom;
  StretchDIBits(hdc, x + rect.left, y + rect.top, rect.width(), rect.height(), 
    rect.left, adjustedSourceTop, rect.width(), rect.height(), bits, &bmi, DIB_RGB_COLORS, SRCCOPY);

  bitmap->UnlockPixels();
}
*/

void WindowWin::DrawSurface(int x, int y, Surface* surface) {
  DIBSurface* dibSurface = static_cast<DIBSurface*>(surface);

  if (!g_dc) return;
  HDC hdc = g_dc;
  BitBlt(hdc, x, y, (int)surface->width(), (int)surface->height(), dibSurface->dc(), 0, 0, SRCCOPY);
}

void* WindowWin::native_handle() const {
  return hwnd_;
}

void WindowWin::OnClose() {
  if (listener_)
    listener_->OnClose();
  if (app_listener_)
    app_listener_->OnClose();
}

void WindowWin::OnResize(uint32_t width, uint32_t height) {
  if (listener_)
    listener_->OnResize(width, height);
  if (app_listener_)
    app_listener_->OnResize(width, height);
}

Ref<Window> Window::Create(Monitor* monitor, uint32_t width, uint32_t height,
  bool fullscreen, unsigned int window_flags) {
  return AdoptRef(*new WindowWin(monitor, width, height, fullscreen, window_flags));
}

Window::~Window() {}

}  // namespace ultralight
