#include <windows.h>
#include <iostream>
#include <commdlg.h> // Ajout pour GetOpenFileName
#include <fstream>
#include <string>

// Déclaration de macros

#define FILE_MENU_NEW 1
#define FILE_MENU_OPEN 2
#define FILE_MENU_SAVE 3
#define FILE_MENU_EXIT 4

// Déclaration de la variable globale pour le menu
HMENU hMenu;
TCHAR szFileName[MAX_PATH] = { 0 }; // Buffer pour le chemin du fichier (MAX_PATH = 260)
std::wstring textBuffer; // Buffer pour stocker le texte
const int MAX_TEXT_LENGTH = 4294967296; // Limite du texte
HBITMAP hBitmap = nullptr; // Handle pour l'image


// Déclaration des fonctions
void AddMenu(HWND hwnd);
void AddControls(HWND hwnd);




// Fonction de gestion des événements
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hEdit = nullptr; // Handle du contrôle EDIT

    switch (uMsg) {
    case WM_CREATE:
        AddMenu(hwnd);
        AddControls(hwnd); // Ajoute les contrôles
        hEdit = GetDlgItem(hwnd, 100); // Récupère le handle du contrôle EDIT (ID 100)
        if (hEdit) {
            SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)textBuffer.c_str()); // Initialise avec textBuffer
        }
        return 0;

    case WM_COMMAND:
    {
        switch (wParam)
        {
        case FILE_MENU_NEW:
        {
            if (hEdit) {
                SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)L""); // Efface le texte
                textBuffer.clear();
            }
            MessageBeep(MB_OK);
            break;
        }

        case FILE_MENU_OPEN:
        {
            OPENFILENAME ofn = { 0 };
            TCHAR szFileName[MAX_PATH] = { 0 };

            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = L"Tous les fichiers (*.*)\0*.*\0Textes (*.txt)\0*.txt\0Images (*.bmp)\0*.bmp\0\0";
            ofn.lpstrFile = szFileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

            if (GetOpenFileName(&ofn)) {
                std::wstring filePath(szFileName);
                if (filePath.length() >= 4 && filePath.substr(filePath.length() - 4) == L".txt") {
                    std::wifstream file(szFileName, std::ios::in);
                    if (file.is_open()) {
                        textBuffer.assign((std::istreambuf_iterator<wchar_t>(file)), std::istreambuf_iterator<wchar_t>());
                        file.close();
                        if (hEdit) {
                            SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)textBuffer.c_str()); // Mets à jour le contrôle
                        }
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                    else {
                        MessageBoxW(hwnd, L"Erreur lors de l'ouverture du fichier texte", L"Erreur", MB_OK | MB_ICONERROR);
                    }
                }
                else if (filePath.length() >= 4 && filePath.substr(filePath.length() - 4) == L".bmp") {
                    if (hBitmap) DeleteObject(hBitmap);
                    hBitmap = (HBITMAP)LoadImageW(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
                    if (hBitmap) {
                        InvalidateRect(hwnd, NULL, TRUE);
                        MessageBoxW(hwnd, L"Image chargée avec succès !", L"Info", MB_OK | MB_ICONINFORMATION);
                    }
                    else {
                        MessageBoxW(hwnd, L"Erreur lors du chargement de l'image", L"Erreur", MB_OK | MB_ICONERROR);
                    }
                }
            }
            else {
                if (CommDlgExtendedError() == 0) {
                    MessageBoxW(hwnd, L"Annulé par l'utilisateur", L"Info", MB_OK);
                }
                else {
                    MessageBoxW(hwnd, L"Erreur lors de l'ouverture du dialogue", L"Erreur", MB_OK | MB_ICONERROR);
                }
            }
            break;
        }

        case FILE_MENU_SAVE:
        {
            OPENFILENAME ofn = { 0 };
            TCHAR szFileName[MAX_PATH] = { 0 };

            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = L"Tous les fichiers (*.*)\0*.*\0Textes (*.txt)\0*.txt\0\0";
            ofn.lpstrFile = szFileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
            ofn.lpstrDefExt = L"txt";

            if (GetSaveFileName(&ofn)) {
                if (hEdit) {
                    int len = SendMessageW(hEdit, WM_GETTEXTLENGTH, 0, 0);
                    if (len > 0) {
                        wchar_t* buffer = new wchar_t[len + 1];
                        SendMessageW(hEdit, WM_GETTEXT, len + 1, (LPARAM)buffer);
                        textBuffer = buffer;
                        delete[] buffer;
                    }
                }
                std::wofstream file(szFileName, std::ios::out);
                if (file.is_open()) {
                    file << textBuffer;
                    file.close();
                    MessageBoxW(hwnd, L"Fichier sauvegardé avec succès !", L"Sauvegarde", MB_OK | MB_ICONINFORMATION);
                }
                else {
                    MessageBoxW(hwnd, L"Erreur lors de l'ouverture du fichier", L"Erreur", MB_OK | MB_ICONERROR);
                }
            }
            else {
                if (CommDlgExtendedError() == 0) {
                    MessageBoxW(hwnd, L"Annulé par l'utilisateur", L"Info", MB_OK);
                }
                else {
                    MessageBoxW(hwnd, L"Erreur lors de la sauvegarde du dialogue", L"Erreur", MB_OK | MB_ICONERROR);
                }
            }
            break;
        }

        case FILE_MENU_EXIT:
        {
            if (MessageBoxW(hwnd, L"Voulez-vous vraiment quitter l'application ?", L"Quitter", MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
                DestroyWindow(hwnd);
            }
            break;
        }
        return 0;
}
    }

    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        if (hEdit) {
            // Redimensionne le contrôle EDIT pour s'adapter à la fenêtre
            MoveWindow(hEdit, 10, 10, width - 20, height - 60, TRUE); // Laisse une marge
        }
        std::wcout << L"Redimensionnement : " << width << L"x" << height << L"\n";
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        // Affiche l'image si chargée (en dessous du contrôle EDIT)  
        if (hBitmap) {
            HDC memDC = CreateCompatibleDC(hdc);
            SelectObject(memDC, hBitmap);
            BITMAP bm;
            GetObject(hBitmap, sizeof(bm), &bm);
            BitBlt(hdc, 10, 590, bm.bmWidth, bm.bmHeight, memDC, 0, 0, SRCCOPY); // Ajuste la position  
            DeleteDC(memDC);
        }
        EndPaint(hwnd, &ps); // Correction : passez hwnd comme premier argument et &ps comme second  
        return 0;
    }

    case WM_DESTROY:
    {
        if (hBitmap) DeleteObject(hBitmap);
        PostQuitMessage(0);
        return 0;
    }

    default:
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}


void AddMenu(HWND hwnd)  
{  
   hMenu = CreateMenu();
   HMENU hFileMenu = CreateMenu();
   HMENU hWindowMenu = CreateMenu();
   HMENU hSubMenu = CreateMenu();

   // Menu déroulant
   // Window
   AppendMenuW(hSubMenu, MF_STRING, 1, L"Submenu Item 1");

   // Menu Fichier
   AppendMenuW(hFileMenu, MF_STRING, FILE_MENU_NEW, L"New");  
   AppendMenuW(hFileMenu, MF_STRING, FILE_MENU_OPEN, L"Open");  
   AppendMenuW(hFileMenu, MF_STRING, FILE_MENU_SAVE, L"Save");
   AppendMenuW(hFileMenu, MF_SEPARATOR, 0, nullptr);
   AppendMenuW(hFileMenu, MF_STRING, FILE_MENU_EXIT, L"Exit");

   // Menu Window
   AppendMenuW(hWindowMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"Window Item 1");

   // Menu principal
   AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");  
   AppendMenuW(hMenu, MF_STRING, 5, L"Edit");  
   AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hWindowMenu, L"Window");
   AppendMenuW(hMenu, MF_STRING, 6, L"Help");  

   SetMenu(hwnd, hMenu);  
}

void AddControls(HWND hwnd)
{
    // Crée un champ de texte multiligne avec défilement
    HWND hEdit = CreateWindowW(L"EDIT", L"",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_BORDER,
        10, 10, 780, 580, // Ajuste les dimensions pour presque toute la fenêtre (laisse de la marge)
        hwnd, (HMENU)100, nullptr, nullptr); // ID 100 pour le contrôle

    if (hEdit) {
        // Définit la police par défaut (facultatif, améliore la lisibilité)
        SendMessageW(hEdit, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
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