#include <windows.h>  

// Déclaration de la fonction WindowProc  
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)  
{  
  switch (uMsg)  
  {  
      case WM_DESTROY:  
          PostQuitMessage(0);  
          return 0;  

      default:  
          return DefWindowProc(hwnd, uMsg, wParam, lParam);  
  }  
}  

int main()  
{  
  // Enregistrement de la classe de fenêtre.  
  const wchar_t CLASS_NAME[] = L"Sample Window Class";  

  WNDCLASS wc = {};  

  wc.lpfnWndProc = WindowProc;  
  wc.hInstance = GetModuleHandle(NULL);  
  wc.lpszClassName = CLASS_NAME;  

  if (!RegisterClass(&wc))  
  {  
      return -1;  
  }

  HINSTANCE hInstance = GetModuleHandle(NULL); // Définition de hInstance

  HWND hwnd = CreateWindowEx(
      0,                              // Optional window styles.
      CLASS_NAME,                     // Window class
      L"Learn to Program Windows",    // Window text
      WS_OVERLAPPEDWINDOW,            // Window style

      // Size and position
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

      NULL,       // Parent window    
      NULL,       // Menu
      hInstance,  // Instance handle
      NULL        // Additional application data
  );

  if (hwnd == NULL)
  {
      return 0;
  }

  ShowWindow(hwnd, SW_SHOW);

  MSG msg = {};
  while (GetMessage(&msg, NULL, 0, 0))
  {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
  }

  return 0;
}