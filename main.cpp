#include "main.h"

using namespace std;

/* --------------------------------------------------------- VORDEKLARATION --------------------------------------------------------- */

struct StateInfo { // globale Nutzung der Fenster
  HWND hWndWindow;
  HWND hWndDialog;
  int nCmd;
};

static TCHAR szClassName[] = _T("WinAPICpp_Fibonacci"); // Windows-Klassenname

char puffer[40]; // Ausgabepuffer

// Höhe/Breite des Hauptfensters
static int width = 800;
static int height = 600;

static int diagram_dimensions = 250; // Dimensionenlänge (x- u. y-Achse des Diagramms)

int current_index = 0; // derzeit ausgewählter Index-Wert
int max_index = 0; // höchster Index-Wert wird als Maßstab für die x-Achse festgelegt
unsigned long int max_Fibonacci = 0; // größter ermittelter Fibonacci-Wert wird als Maßstab für die y-Achse festgelegt

bool changed_values = false; // Verhindert das Flackern/Buggen der Zahlenwerte im Hauptfenster

/* --------------------------------------------------------- HAUPTPROGRAMM --------------------------------------------------------- */
int WINAPI WinMain (HINSTANCE dieseInstanz, HINSTANCE vorherigeInstanz, LPSTR lpszArgument, int nCmdShow) {

    /* Initialize State */
    StateInfo *pState = new (nothrow) StateInfo;
    if (pState == NULL) return 255;
    pState->nCmd = nCmdShow;

    MSG Meldung;                 /* Here messages to the application are saved */
    WNDCLASSEX wincl;           /* Data structure for the windowclass */

    /* ------ INSTANZ DURCH WINDOWS-KLASSE FESTLEGEN ------ */
    if (!vorherigeInstanz) {
        wincl.style = CS_HREDRAW | CS_VREDRAW;
        wincl.lpfnWndProc = (WNDPROC) WindowProcedure;      /* This function is called by windows */
        wincl.cbClsExtra = 0;                               /* No extra bytes after the window class */
        wincl.cbWndExtra = 0;                               /* structure or the window instance */
        wincl.hInstance = dieseInstanz;
        wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
        wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
        wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
        wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
        wincl.lpszMenuName = NULL;
        wincl.lpszClassName = szClassName;
        wincl.cbSize = sizeof (WNDCLASSEX);

        if (!RegisterClassEx(&wincl)) {
            MessageBox(NULL, "Windows-Klasse konnte nicht erstellt werden.", "Fehler bei Klassenerstellung", MB_ICONERROR);
            return ERROR_CLASS;
        }
    } else {
        MessageBox(NULL, "Es existiert bereits eine laufende Instanz. Bitte schließen Sie diese.", "Fehler bei Klassenerstellung", MB_ICONERROR);
        return ERROR_INSTANCE_EXISTS;
    }


    /* ------ ERSTELLUNG DES HAUPTFENSTERS ------ */
    pState->hWndWindow = CreateWindowEx (
           0,                                       /* Extended possibilites for variation */
           szClassName,                             /* Klassenname */
           "Fibonacci-Diagramm",                    /* Titel Text */
           WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,     /* default Fenster + verhindere resizen*/
           CW_USEDEFAULT,                           /* default Fensterposition */
           CW_USEDEFAULT,
           width, height,                           /* Breite + Höhe */
           HWND_DESKTOP,                            /* Parent ist Desktop */
           NULL,
           dieseInstanz,                            /* InstanceHandler */
           pState                                   /* Hauptfenster an StateInfo gebunden */
           );

    /* ------ ERSTELLUNG DES BUTTONS UND DER LABEL FÜR DAS HAUPTFENSTER ------ */
    HWND hwndButton = CreateWindow (
            "BUTTON",                         // Vordefinierte class; Unicode assumed
            "Fibonacci-Zahlen anzeigen",      // Button Text
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles
            width/2 - 100,                      // x position: width/2 - Hälfte der Buttonbreite
            500,                                // y position
            200,                                // Button-Breite
            20,                                 // Button-Höhe
            pState->hWndWindow,                 // Parent window
            (HMENU) IDC_OPEN_DIALOG,            // IDC Zuordnung
            (HINSTANCE) GetWindowLongPtr(pState->hWndWindow, GWLP_HINSTANCE),
            NULL);

    HWND hwndLabelx = CreateWindow(
            "static",
            "Index",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
            width/2 + width/4, height/2,
            37, 18,
            pState->hWndWindow,
            (HMENU)IDC_LABELX,
            (HINSTANCE) GetWindowLong (pState->hWndWindow, GWLP_HINSTANCE), NULL);

    HWND hwndLabely = CreateWindow (
            "static",
            "Fibonacci-Zahl",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
            width/3 + 20,height/8,
            97, 18,
            pState->hWndWindow,
            (HMENU)IDC_LABELY,
            (HINSTANCE) GetWindowLong (pState->hWndWindow, GWLP_HINSTANCE), NULL);

    if(!pState->hWndWindow) {
        MessageBox(NULL, "Hauptfenster konnte nicht erstellt werden.", "Fehler beim Hauptfenster", MB_ICONERROR);
        return ERROR_MAIN_WINDOW;
    }

    /* ------ ERSTELLUNG DES DIALOGFENSTERS ------ */
    pState->hWndDialog = CreateDialog(dieseInstanz, MAKEINTRESOURCE(IDC_DIALOG), HWND_DESKTOP, (DLGPROC) DialogProcedure);

    if (!pState->hWndDialog) {
        MessageBox(NULL, "Dialogfenster konnte nicht erstellt werden.", "Fehler beim Dialogfenster", MB_ICONERROR);
        return ERROR_DIALOG_WINDOW;
    }

    /* Hauptfenster wird sichtbar gemacht */
    ShowWindow (pState->hWndWindow, nCmdShow);
    UpdateWindow(pState->hWndWindow);

    /* Message Loop, bis ein return von 0 gegeben wird */
    while (GetMessage (&Meldung, NULL, 0, 0)) {
        TranslateMessage(&Meldung);
        DispatchMessage(&Meldung);
    }

    // Erfolgreiches Beenden des Programms
    return Meldung.wParam;
}

/* --------------------------------------------------------- CALLBACK-FKT. DES HAUPTFENSTERS --------------------------------------------------------- */
LRESULT CALLBACK WindowProcedure (HWND Fenster, UINT nachricht, WPARAM wParam, LPARAM lParam) {
    StateInfo *pState;

    // globales State muss für das Message-Handling konfiguriert werden
    if (nachricht == WM_CREATE) {
        CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pState = reinterpret_cast<StateInfo*>(pCreate->lpCreateParams);
        SetWindowLongPtr(Fenster, GWLP_USERDATA, (LONG_PTR)pState);
    } else {
        LONG_PTR ptr = GetWindowLongPtr(Fenster, GWLP_USERDATA);
        pState = reinterpret_cast<StateInfo*>(ptr);
    }

    switch (nachricht) { // Message-Handler
        case WM_CREATE:

            // bei Erstellung des Fensters wird verhindert, dass dieses minimiert bzw. maximiert werden kann
            SetWindowLong(Fenster, GWL_STYLE, GetWindowLong(Fenster, GWL_STYLE) & ~WS_MINIMIZEBOX);
            SetWindowLong(Fenster, GWL_STYLE, GetWindowLong(Fenster, GWL_STYLE) & ~WS_MAXIMIZEBOX);

            /* Es werden zusätzliche Labels zur Kennzeichnung der derzeitigen Skalierung erstellt */
            // Label für den größten Fibonacci-Wert
            static HWND hwndLabelFib = CreateWindow (
                    "static",
                    "0",
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                    width/3 + 20,height/8 + 15,
                    97, 18,
                    Fenster,
                    (HMENU)IDC_LABELFIB,
                    (HINSTANCE) GetWindowLong (Fenster, GWLP_HINSTANCE), NULL);

                    ShowWindow (hwndLabelFib, pState->nCmd); // Label wird angezeigt

            // Label für den höchsten Index-Wert
            static HWND hwndLabelIndx = CreateWindow (
                    "static",
                    "0",
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                    width/2 + width/4, height/2 + 15,
                    37, 18,
                    Fenster,
                    (HMENU)IDC_LABELINDX,
                    (HINSTANCE) GetWindowLong (Fenster, GWLP_HINSTANCE), NULL);

                    ShowWindow (hwndLabelIndx, pState->nCmd); // Label wird angezeigt

            return TRUE;
        case WM_COMMAND: // bei Registrierung eines "Befehls" (in diesem Falle: nur der Knopfdruck) öffnet sich das Dialog-Fenster
            ShowWindow(pState->hWndDialog, SW_SHOW);

            return TRUE;
        case WM_DESTROY: // bei Schließen des Fensters wird das Programm beendet
            //MessageBox(Fenster, "Autor: Robert Schmidt | Matrikelnummer: 11131697", "Programmende", MB_ICONINFORMATION | MB_OK);
            PostQuitMessage(0);       /* sendet ein WM_QUIT an die Message-Queue */

            return TRUE;
        case WM_PAINT: // zeichnet ein Koordinatensystem und updated dieses im Verlaufe der Fibonacci-Sequenz

            DrawDiagram(Fenster);

            // Verhindert eine stetige Aktualisierung der Werte (Flackern der Zahlenwerte entfernt)
            if (changed_values) {
                // Höchster Index wird im Label angepasst
                wsprintf(puffer, "%i", max_index);
                SetWindowText(hwndLabelIndx, puffer);

                // Größter Fibonacci-Wert wird im Label angepasst
                wsprintf(puffer, "%u", max_Fibonacci);
                SetWindowText(hwndLabelFib, puffer);

                changed_values = false; // bleibt false und verhindert stetige Aktualisierung, bis sich Zahlenwerte ändern (wird in FibVoni() neu gesetzt)
            }

            return TRUE;
        default: // default für ungenutzte Messages
            return DefWindowProc (Fenster, nachricht, wParam, lParam);
    }

    return FALSE;
}

/* --------------------------------------------------------- CALLBACK-FKT. DES DIALOG-FENSTERS --------------------------------------------------------- */
BOOL CALLBACK DialogProcedure(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch(Message) // Message-Handler
    {
        case WM_INITDIALOG:
            SCROLLINFO si; // erstellt eine SCROLLINFO für die ScrollBar

            // initialisiere Index und Fibonacci-Zahl mit 0
            wsprintf(puffer, "%i", 0);
            SetDlgItemText(hwnd, IDC_INDEX, puffer);
            SetDlgItemText(hwnd, IDC_FIBONACCI, puffer);

            SetScrollRange(hwnd, SB_VERT, MINIMAL_INDEX, MAXIMAL_INDEX, TRUE); // Einstellung der allgemeinen Seiten-ScrollBar

            /* Konfiguriere ScrollBar */
            si.cbSize = sizeof(si);
            si.nPos = 0;
            si.nMax = MAXIMAL_INDEX;
            si.nMin = MINIMAL_INDEX;
            si.fMask = SIF_ALL;
            si.nPage = 1;
            si.nTrackPos = 0;

            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

            break;
        case WM_COMMAND: // bei Registrierung eines "Befehls" (in diesem Falle: nur der Knopfdruck von "Schließen") schließt sich das Dialog-Fenster
            EndDialog(hwnd, IDCANCEL);

            break;
        case WM_VSCROLL: // bei Betätigung der Scrollbar (UP/DOWN) wird inkrementiert/dekrementiert
            si.fMask = SIF_ALL;
            si.cbSize = sizeof(si);
            GetScrollInfo(hwnd, SB_VERT, &si);

            switch(LOWORD(wParam)) {
            case SB_LINEUP:
                if (!current_index <= MINIMAL_INDEX) {  // inkrementiert den Index und passt den dazugehörigen Fibonacci-Wert an
                    current_index--;                    // wenn Index kleiner/gleich 46 ist (Konstante zur Verhinderung eines Overflows)
                    si.nPos--;
                }

                break;
            case SB_LINEDOWN:
                if (current_index < MAXIMAL_INDEX) {    // dekrementiert den Index (aber nicht kleiner 0) und passt den dazugehörigen Fibonacci-Wert an
                    current_index++;                    // wenn Index gleich oder größer 0 ist (Konstante zur Einhaltung des vorgebenen Falls)
                    si.nPos++;
                }

                break;
            case SB_PAGEUP:                             // dekrementiert Index, wenn Page hochgegangen wird
                current_index -= si.nPage;
                si.nPos -= si.nPage;

                break;
            case SB_PAGEDOWN:                           // inkrementiert Index, wenn Page runtergegangen wird
                current_index += si.nPage;
                si.nPos += si.nPage;

                break;
            case SB_THUMBPOSITION:                      // passt Index der ScrollBar-Position an
                current_index = si.nTrackPos;
                si.nPos = si.nTrackPos;

                break;
            }

            // ScrollBar wird auf Positionsänderung konfiguriert
            si.fMask = SIF_POS;
            si.cbSize = sizeof(si);

            /* ScrollBar wird aktualisiert */
            SetScrollInfo (hwnd, SB_VERT, &si, TRUE);
            GetScrollInfo (hwnd, SB_VERT, &si);

            // Index wird im Label angepasst
            wsprintf(puffer, "%i", current_index);
            SetDlgItemText(hwnd, IDC_INDEX, puffer);

            // Fibonacci-Wert wird im Label mittels FibVoni() angepasst
            wsprintf(puffer, "%u", FibVoni(current_index));
            SetDlgItemText(hwnd, IDC_FIBONACCI, puffer);

            break;
        default: // default für ungenutzte Messages
            return FALSE;
    }

    return TRUE;
}

/* --------------------------------------------------------- FUNKTION ZUR ERMITTLUNG DER FIBONACCI-SEQUENZ (mit Index) --------------------------------------------------------- */
int FibVoni(int n) {
    unsigned long int return_value = 0; // Rückgabewert

    if (n == 0) {           // 1. Fall: Index 0 mit Wert 0
        return_value = 0;
    } else if (n == 1) {    // 2. Fall: Erster Index mit Wert 1
        return_value = 1;
    } else {                // 3. Fall: Berechnung aller weiteren Fibonacci-Werte beliebigen Indexes
        unsigned long int saved_fib[n + 1];  /* Größenbereich der Fibonacci-Berechnung wird festgelegt */
        saved_fib[0] = 0;           /* Index 0 und 1 werden manuell festgelegt */
        saved_fib[1] = 1;

        // Schleife berechnet alle entsprechenden Fibonacci-Werte
        for (int i = 2; i <= n; i++) {
            saved_fib[i] = saved_fib[i-1] + saved_fib[i-2];
        }

        return_value = saved_fib[n]; // finaler Wert wird als Rückgabewert festgelegt
    }

    // Für das Koordinatensystem: x- und y-Achse werden nach ihrem maximal erreichten Index bzw. Wert skaliert
    if (n > max_index) {
        max_index = n;
        max_Fibonacci = return_value;

        changed_values = true; // verhindert das Flackern/Buggen der Zahlenwerte im Hauptfenster, indem dieser Wert dort einmal geprüft und neu gesetzt wird
    }

    return return_value; // Fibonacci-Zahl wird zurückgegeben
}

/* --------------------------------------------------------- FUNKTION ZUM ZEICHNEN DES KOORDINATENSYSTEMS --------------------------------------------------------- */
void DrawDiagram(HWND hWnd)
{
    HDC hDC; // DC Handle
    RECT rect; // benötigt zur Ermittlung des Client-Mittelpunkts

    int xPoint, yPoint; // Punktkoordinaten zur Ermittlung der Koordinaten und Kreisauswahl

    // Ermittelt DC Handle durch Fenster
    hDC = GetDC(hWnd);

    SetGraphicsMode(hDC, GM_ADVANCED);
    SetMapMode(hDC, MM_LOENGLISH); // Mapping-Modus wird zu LOENGLISH gesetzt, um Dimensionen auch bei verschiedenen Displays beizubehalten

    // Mittelpunkt des Clients wird ermittelt
    GetClientRect(hWnd, (LPRECT) &rect);
    DPtoLP(hDC, (LPPOINT) &rect, 2);

    // Zum Zeichnen wird eine Hollow Brush gewählt
    SelectObject(hDC, GetStockObject(HOLLOW_BRUSH));

    /* Koordinatensystem:
        setzt sich zusammen aus zwei Linien, die den ersten (bzw. den einzig sinnvollen) Quadranten bilden
        und aus zwei weiteren Linien, die sowohl den 2.-4. Quadranten bilden (optische Entscheidung)

        rect.right/2 widerspiegelt den Mittelpunkt auf horizontaler Ebene
        währen rect.bottom/2 den Mittelpunkt auf vertikaler Ebene repräsentiert.
    */

    // Horizontale Linien des Koordinatensystems werden gezeichnet

    MoveToEx(hDC, (rect.right /2 - 85), (rect.bottom / 2 + 0), NULL); // Länge links / irrelevant
    LineTo(hDC, (rect.right / 2 - 16), (rect.bottom / 2 + 0));

    MoveToEx(hDC, (rect.right / 2 - 13), (rect.bottom / 2 + 0), NULL);
    LineTo(hDC, (rect.right / 2 + diagram_dimensions), (rect.bottom / 2 + 0));

    // Vertikale Linien des Koordinatensystems werden gezeichnet

    MoveToEx(hDC, (rect.right /2 + 0), (rect.bottom / 2 - 85), NULL); // Länge unten / irrelevant
    LineTo(hDC, (rect.right / 2 + 0), (rect.bottom / 2 - 16));

    MoveToEx(hDC, (rect.right / 2 + 0), (rect.bottom / 2 - 13), NULL);
    LineTo(hDC, (rect.right / 2 + 0), (rect.bottom / 2 + diagram_dimensions));

    // Koordinaten:
    /*
        Die einzelnen Koordinaten richten sich nach dem entsprechenden Index und dem Fibonacci-Wert.
        Sobald der erste Index eingetragen wurde, wird stets bis zum maximal erreichten Index
        eine Koordinate eingetragen und das Diagramm wird entsprechend fehlerfrei aktualisiert.

        Die Koordinaten bestimmen sich aus den Abständen zum Mittelpunkt + entsprechende Skalierungsberechnungen.
        Anschließend wird mittels Rectangle() eine kleine Box erstellt, die als Koordinate fungiert.
    */
    if (!(max_index == 0)) {
        for (int i = 0; i <= max_index; i++) {
            xPoint = rect.right / 2 + ((diagram_dimensions/max_index) * i); // entnimmt x-Koordinate des Index

            if (i == 0) { // Sonderfall: Nullpunkt bleibt in der Mitte
                yPoint = rect.bottom / 2;
            } else {
                yPoint = rect.bottom / 2 + (diagram_dimensions/((double) max_Fibonacci/(double) FibVoni(i))); // entnimmt y-Koordinate des Index
            }

            Rectangle(hDC, xPoint - 1, yPoint + 1, xPoint + 1, yPoint - 1); // Zeichnung der Koordinate
        }

    // Kreis:
        /*
            Für den Kreis werden die Koordinaten für den derzeit ausgewählten Index ermittelt
            und mittels Ellipse() ein kleiner Kreis um die Auswahl erzeugt.
        */

        xPoint = rect.right / 2 + ((diagram_dimensions/max_index) * current_index); // entnimmt die x-Koordinate des ausgewählten Index
        if (current_index == 0) { // Sonderfall: Nullpunkt bleibt in der Mitte
            yPoint = rect.bottom / 2;
        } else {
            yPoint = rect.bottom / 2 + (diagram_dimensions/((double) max_Fibonacci/(double) FibVoni(current_index))); // entnimmt die y-Koordinate des ausgewählten Index
        }

        Ellipse(hDC, xPoint - 4, yPoint + 4, xPoint + 4, yPoint - 4); // Zeichnung des Kreises mit leichter Vergrößerung zum Punkt
    }

    ReleaseDC(hWnd, hDC); // DC Handle wird befreit
}
