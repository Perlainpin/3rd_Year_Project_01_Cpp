#include "pch.h"
#include "main.h"

// make sure WIN32_LEAN_AND_MEAN is commented out in all code (usually stdafx.h)
//#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <string.h>
#include <bitset>
#include <vector>
#include <Commdlg.h>

#include "BitmapImage.h"

#define MAX_LOADSTRING 100

using namespace std;

// Variables globales :
HINSTANCE hInst;                                // instance actuelle
WCHAR szTitle[MAX_LOADSTRING];                  // Texte de la barre de titre
WCHAR szWindowClass[MAX_LOADSTRING];            // nom de la classe de fenêtre principale

// Déclarations anticipées des fonctions incluses dans ce module de code :
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

#define BTN_OPEN 1
#define BTN_COLOR 2
#define BTN_ENCRYPT 3
#define BTN_CLEAR 4
#define BTN_SAVE 5
#define BTN_DECRYPT 6

BitmapImage img;
HWND ImageWindow;
HWND TextArea;
HWND ImageSize;
HWND DebugWindow;
string DebugText;
HDC hdc;
LPWSTR filePath;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Placez le code ici.

    // Initialise les chaînes globales
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_STEGANOGRAPHIE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Effectue l'initialisation de l'application :
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    // HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_STEGANOGRAPHIE));

    MSG msg;

    // Boucle de messages principale :
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        // if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  FONCTION : MyRegisterClass()
//
//  OBJECTIF : Inscrit la classe de fenêtre.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_STEGANOGRAPHIE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_STEGANOGRAPHIE);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FONCTION : InitInstance(HINSTANCE, int)
//
//   OBJECTIF : enregistre le handle d'instance et crée une fenêtre principale
//
//   COMMENTAIRES :
//
//        Dans cette fonction, nous enregistrons le handle de l'instance dans une variable globale, puis
//        nous créons et affichons la fenêtre principale du programme.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Stocke le handle d'instance dans la variable globale

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, 0, 1180, 666, nullptr, nullptr, hInstance, nullptr);


    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

void ReadPixels() {
    vector<vector<int>> pixels;
    for (long i = 0; i <= img.bgrSize - 4; i += 4)
    {
        vector<int> rgb = { img._bgr[i + 2], img._bgr[i + 1], img._bgr[i], img._bgr[i + 3] };
        pixels.push_back(rgb);
    }
    string debug;
    for (int i = 0; i < pixels.size(); i++)
    {
        debug.append("(");
        debug.append(to_string(pixels[i][0]));
        debug.append(",");
        debug.append(to_string(pixels[i][1]));
        debug.append(",");
        debug.append(to_string(pixels[i][2]));
        debug.append(",");
        debug.append(to_string(pixels[i][3])); // Alpha
        debug.append(")");
    }
    DebugText.append(debug);
    SetWindowTextA(TextArea, "Replace this with text in ASCII to encrypt");
    SetWindowTextA(DebugWindow, DebugText.c_str());
}

string DecimalToBinary(int num)
{
    string str;
    while (num) {
        if (num & 1) // 1
            str += '1';
        else // 0
            str += '0';
        num >>= 1; // Right Shift by 1  
    }

    while (str.length() < 8) {
        str += '0';
    }
    return str;
}

// Function to convert binary to decimal
int BinaryToDecimal(string binary)
{
    int temp = 0;
    int mult = 1;

    while (binary.length() > 0)
    {
        int a = binary.back() - '0';
        temp += a * mult;
        mult *= 2;
        binary.pop_back();
    }

    return temp;
}

void InsertMessage(HWND hWnd) {
    DebugText.clear();
    DebugText.append("\nEncoding letters in image:\n");

    // Get message from text area
    char text[256];
    if (GetWindowTextA(TextArea, text, 256) == 0) {
        MessageBox(hWnd, L"Text lenght must be > 0", L"No text found", MB_OK | MB_ICONWARNING);
        return;
    }
    SetWindowTextA(DebugWindow, DebugText.c_str());


    // Convert to string
    string msg = text;
    string binaryMsg = "";
    for (char& c : msg) {
        binaryMsg += DecimalToBinary(c);
    }

    //int binaryMsg = 0;
    //vector<byte> characters;
    //// for each letter of the msg
    //for (char& c : msg) {  
    //    characters.push_back((int)c-32); // when decrypt add 32 
    //}

    //SetWindowTextA(DebugWindow, to_string(binaryMsg).c_str());

    // Modifier certains pixels(exemple : passer au negatif de l'image)

    for (int y = 0; y < img._infoHeader.biHeight; ++y) {
        for (int x = 0; x < img._infoHeader.biWidth; ++x) {
            int pixelIndex = (y * img._infoHeader.biWidth + x) * 3; // 3 car on a 3 octets par pixel (BGR) euh enfaite il y en a 4 avec l'alpha 

            // convert rgb to binary
            string b = DecimalToBinary(img._bgr[pixelIndex]);

            string g = DecimalToBinary(img._bgr[pixelIndex + 1]);

            string r = DecimalToBinary(img._bgr[pixelIndex + 2]);



            if (binaryMsg.size() > 0) {
                b.pop_back();
                b += binaryMsg[binaryMsg.size() - 1];
                binaryMsg.pop_back();
            }
            else {
                b.pop_back();
                b += '0';
            }
            if (binaryMsg.size() > 0) {
                g.pop_back();
                g += binaryMsg[binaryMsg.size() - 1];
                binaryMsg.pop_back();
            }
            else {
                g.pop_back();
                g += '0';
            }
            if (binaryMsg.size() > 0) {
                r.pop_back();
                r += binaryMsg[binaryMsg.size() - 1];
                binaryMsg.pop_back();
            }
            else {
                r.pop_back();
                r += '0';
            }

            img._bgr[pixelIndex] = BinaryToDecimal(b);
            img._bgr[pixelIndex + 1] = BinaryToDecimal(g);
            img._bgr[pixelIndex + 2] = BinaryToDecimal(r);
        }
    }
}

void DecryptMessage(HWND hWnd) {
    // read all image pixel

    string encryptedMessage;

    for (int y = 0; y < img._infoHeader.biHeight; ++y) {
        for (int x = 0; x < img._infoHeader.biWidth; ++x) {
            int pixelIndex = (y * img._infoHeader.biWidth + x) * 3;

            string b = DecimalToBinary(img._bgr[pixelIndex]);
            string g = DecimalToBinary(img._bgr[pixelIndex + 1]);
            string r = DecimalToBinary(img._bgr[pixelIndex + 2]);

            reverse(b.begin(), b.end());
            reverse(g.begin(), g.end());
            reverse(r.begin(), r.end());
            // get only last bit
            encryptedMessage += b.back();
            encryptedMessage += g.back();
            encryptedMessage += r.back();
        }
    }
    // turn into ascii
    // 1 character = 8 bits so split the encrypted message
    // convert to decimal and cast to char
    vector<string> lettersBinary;
    string newString;
    for (int i = 0; i < encryptedMessage.length(); i++)
    {

        newString += encryptedMessage[i];

        // when the 8th bit is set, put into vector
        if (i == 7) {
            // if the string is 00000000, stop here
            if (newString != "00000000") {
                lettersBinary.push_back(newString);
                encryptedMessage.erase(encryptedMessage.begin(), encryptedMessage.begin() + 8);
                i = -1;
                newString.clear();
            }
            else {
                encryptedMessage.clear();
            }
        }
    }

    string decryptedMessage;
    for (int i = 0; i < lettersBinary.size(); i++)
    {
        // convert to ascii
        decryptedMessage += BinaryToDecimal(lettersBinary[i]);
    }

    reverse(decryptedMessage.begin(), decryptedMessage.end());
    SetWindowTextA(TextArea, decryptedMessage.c_str());
}

void SaveBitmapFile(HWND hWnd) {
    // Utiliser IFileSaveDialog
    OPENFILENAME ofn = { 0 };
    TCHAR szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = L"Bitmap image (*.bmp)\0*.bmp\0";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = L"bmp";

    if (GetSaveFileName(&ofn))
    {
        // Save new file
        img.Save(ofn.lpstrFile);
    }
}

void OpenBitmapFile(HWND hWnd) {
    OPENFILENAME ofn = { 0 };
    TCHAR szFile[260] = { 0 };
    // Initialize remaining fields of OPENFILENAME structure
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    //ofn.lpstrFilter = _T("All\0*.*\0Text\0*.TXT\0");
    ofn.lpstrFilter = _T("Supported Files(*.bmp)\0*.bmp\0");
    //ofn.lpstrFilter = _T("Supported Files(*.bmp)\0*.bmp\0All files(*.*)\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Recreate the window to clear old image
    ImageWindow = CreateWindow(
        L"STATIC",
        L"",
        WS_VISIBLE | WS_CHILD,
        700,
        100,
        400,
        400,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);

    EnableWindow(DebugWindow, true);

    if (GetOpenFileName(&ofn) == TRUE)
    {
        // use ofn.lpstrFile here
        filePath = ofn.lpstrFile;

        SetWindowText(DebugWindow, L"File path");

        if (img.Load(ofn.lpstrFile) != 0) {
            SetWindowText(DebugWindow, L"Error loading image");
            return;
        }

        if (img.Read() != 0) {
            SetWindowText(DebugWindow, L"Error reading image");
            return;
        }

        if (img._infoHeader.biBitCount == 32) {
            MessageBox(hWnd, L"This image is in 32bits format, please select a 24bits image", L"Error loading image", MB_OK | MB_ICONERROR);
            OpenBitmapFile(hWnd);
        }
        if (img._infoHeader.biWidth % 2 != 0 || img._infoHeader.biHeight % 2 != 0) {
            MessageBox(hWnd, L"The Image width and Height must be multiples of 2", L"Error loading image", MB_OK | MB_ICONERROR);
            OpenBitmapFile(hWnd);
        }

        SetWindowText(DebugWindow, L"Image Loaded");
        EnableWindow(TextArea, true);
        // Show the widht and the height of the image
        wstring msg = L"Image size: " + to_wstring(img._infoHeader.biWidth) + L"x" + to_wstring(img._infoHeader.biHeight) + L" pixels";
        SetWindowText(ImageSize, msg.c_str());

        //ReadPixels();
    }
}

void ColorPicker(HWND hWnd) {
    CHOOSECOLOR cc;                 // common dialog box structure 
    static COLORREF acrCustClr[16]; // array of custom colors
    HBRUSH hbrush;                  // brush handle
    static DWORD rgbCurrent;        // initial color selection

    // Initialize CHOOSECOLOR 
    ZeroMemory(&cc, sizeof(cc));
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = hWnd;
    cc.lpCustColors = (LPDWORD)acrCustClr;
    cc.rgbResult = rgbCurrent;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;

    if (ChooseColor(&cc) == TRUE)
    {
        hbrush = CreateSolidBrush(cc.rgbResult);
        rgbCurrent = cc.rgbResult;
    }
}

void DrawImage(HWND hWnd) {

    HDC Imagehdc = GetDC(ImageWindow);
    HDC CompatibleDC = CreateCompatibleDC(Imagehdc);

    HBITMAP Drawing = CreateDIBitmap(Imagehdc, &img._infoHeader, CBM_INIT, img._bgr, (BITMAPINFO*)&img._infoHeader, DIB_RGB_COLORS);
    SelectObject(CompatibleDC, Drawing);

    //BitBlt(Imagehdc, 0, 0, img._infoHeader.biWidth, img._infoHeader.biHeight, CompatibleDC, 0, 0, SRCCOPY);
    StretchBlt(Imagehdc, 0, 0, 400, 400, CompatibleDC, 0, 0, img._infoHeader.biWidth, img._infoHeader.biHeight, SRCCOPY);

    ReleaseDC(hWnd, Imagehdc);
}

//
//  FONCTION : WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  OBJECTIF : Traite les messages pour la fenêtre principale.
//
//  WM_COMMAND  - traite le menu de l'application
//  WM_PAINT    - Dessine la fenêtre principale
//  WM_DESTROY  - génère un message d'arrêt et retourne
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {

        // Text area
        TextArea = CreateWindow(
            L"EDIT",
            L"Load an image first",
            WS_VISIBLE | WS_CHILD | ES_LEFT | WS_DLGFRAME | WS_DISABLED, // Will be enabled once image is loaded
            15,
            20,
            530,
            240,
            hWnd,
            NULL,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        HWND DecryptButton = CreateWindow(
            L"BUTTON",
            L"Decrypt text",
            WS_VISIBLE | WS_CHILD,
            15,
            265,
            140,
            30,
            hWnd,
            (HMENU)BTN_DECRYPT,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        HWND EncryptButton = CreateWindow(
            L"BUTTON",
            L"Encrypt text",
            WS_VISIBLE | WS_CHILD,
            405,
            265,
            140,
            30,
            hWnd,
            (HMENU)BTN_ENCRYPT,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);


        // Image area
        ImageSize = CreateWindow(
            L"EDIT",
            L"Load an image first",
            WS_VISIBLE | WS_CHILD | ES_LEFT | WS_DISABLED, // Will be enabled once image is loaded
            700,
            75,
            400,
            15,
            hWnd,
            NULL,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        ImageWindow = CreateWindow(
            L"STATIC",
            L"",
            WS_VISIBLE | WS_CHILD,
            700,
            100,
            400,
            400,
            hWnd,
            NULL,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        HWND OpenButton = CreateWindow(
            L"BUTTON",
            L"Open image",
            WS_VISIBLE | WS_CHILD,
            700,
            505,
            140,
            30,
            hWnd,
            (HMENU)BTN_OPEN,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        HWND SaveButton = CreateWindow(
            L"BUTTON",
            L"Save image",
            WS_VISIBLE | WS_CHILD,
            960,
            505,
            140,
            30,
            hWnd,
            (HMENU)BTN_SAVE,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        /*
        // Color picker
        HWND ColorButton = CreateWindow(
            L"BUTTON",
            L"Color",
            WS_VISIBLE | WS_CHILD,
            15,
            305,
            140,
            30,
            hWnd,
            (HMENU)BTN_COLOR,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        HWND ModificationList = CreateWindow(
            L"LISTBOX",
            L"List",
            WS_VISIBLE | WS_CHILD,
            15,
            405,
            140,
            30,
            hWnd,
            (HMENU)BTN_COLOR,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);
         */

         // Error box
        DebugWindow = CreateWindow(
            L"STATIC",
            L"Error Zone",
            WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_RIGHT,
            700,
            0,
            470,
            25,
            hWnd,
            NULL,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        HWND ClearDebugButton = CreateWindow(
            L"BUTTON",
            L"Clear",
            WS_VISIBLE | WS_CHILD,
            405,
            565,
            140,
            30,
            hWnd,
            (HMENU)BTN_CLEAR,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        //ShowWindow(DebugWindow, SW_HIDE);
        //return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Analyse les sélections de menu :
        switch (wmId)
        {
        case BTN_OPEN:
            OpenBitmapFile(hWnd);
            DrawImage(hWnd);
            break;
        case BTN_ENCRYPT:
            // Get text from text area and encrypt it in the image
            InsertMessage(hWnd);
            break;
        case BTN_CLEAR:
            DebugText.clear();
            SetWindowTextA(DebugWindow, DebugText.c_str());
            break;
        case BTN_COLOR:
            //ColorPicker(hWnd);
            break;
        case BTN_SAVE:
            SaveBitmapFile(hWnd);
            break;
        case BTN_DECRYPT:
            DecryptMessage(hWnd);
            break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
