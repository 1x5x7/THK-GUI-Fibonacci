#include <windows.h>
#include <tchar.h>
#include <iostream>

#define ERROR_INSTANCE_EXISTS   255
#define ERROR_CLASS             256
#define ERROR_MAIN_WINDOW       257
#define ERROR_DIALOG_WINDOW     258

#define IDC_OPEN_DIALOG         101
#define IDC_LABELX              102
#define IDC_LABELY              103
#define IDC_LABELFIB            104
#define IDC_LABELINDX           105

#define IDC_DIALOG              120
#define IDC_INDEX               122
#define IDC_FIBONACCI           123

#define MINIMAL_INDEX           0
#define MAXIMAL_INDEX           46 // muss festgelegt werden, da der Größenbereich sonst überschritten wird (Overflow)

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogProcedure (HWND, UINT, WPARAM, LPARAM);
int FibVoni(int);
void DrawDiagram(HWND);
void AddToFibArray(int, unsigned long int);
