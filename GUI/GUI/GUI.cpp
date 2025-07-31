#include <windows.h>
#include <iostream>

// Fonction de gestion des événements
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_CLOSE:
        if (MessageBoxW(hwnd, L"QUITTER L'APPLICATION ?", L"APP", MB_OKCANCEL) == IDOK) {
            DestroyWindow(hwnd);
        }
        return 0;

    case WM_LBUTTONDOWN:
        std::wcout << L"CLIC GAUCHE\n";
        return 0;

    case WM_RBUTTONDOWN:
        std::wcout << L"CLIC DROIT\n";
        return 0;

    case WM_SIZE: {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        // À implémenter : ajuster les widgets
        std::wcout << L"Redimensionnement : " << width << L"x" << height << L"\n";
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
        return 0;
    }

    default:
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Enregistrement de la classe de fenêtre
    const wchar_t CLASS_NAME[] = L"Sample Window Class";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassW(&wc)) {
        MessageBoxW(nullptr, L"Échec de l'enregistrement de la classe", L"Erreur", MB_OK | MB_ICONERROR);
        return -1;
    }

    // Création de la fenêtre
    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Création d'une GUI", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, hInstance, nullptr
    );

    if (hwnd == nullptr) {
        MessageBoxW(nullptr, L"Échec de la création de la fenêtre", L"Erreur", MB_OK | MB_ICONERROR);
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);

    // Boucle principale
    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}