//gcc main.c -lkernel32 -luser32 -lgdi32 -lcomdlg32



#include <stdio.h>
#include <windows.h>

#define NEW_FILE 1
#define SAVE_FILE 2
#define OPEN_FILE 3
#define EXIT_FILE 4
#define SAVE_AS 5
#define RUN_FILE 6

HMENU menu;
HWND txwnd;

char fnam[256] = {0};

LRESULT CALLBACK winproc(HWND, UINT, WPARAM, LPARAM);
void menuinit(HWND);
void textinit(HWND);
int open();
int save();
int saveas();
int new();

int WINAPI WinMain(HINSTANCE inst, HINSTANCE pinst, LPSTR args, int cmd)
{
	WNDCLASS wc = {0};
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hInstance = inst;
	wc.lpszClassName = "class";
	wc.lpfnWndProc = winproc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	if(!RegisterClass(&wc))
		return -1;
	CreateWindow(wc.lpszClassName, "Workbench", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
			CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, 
			NULL, NULL, NULL, NULL);
	MSG msg = {0};
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return 0;
}

LRESULT CALLBACK winproc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg)
	{
		case WM_CREATE:
			menuinit(wnd);
			textinit(wnd);
			break;
		case WM_COMMAND:
			switch(wp)
			{
				case NEW_FILE:
					new();
					break;
				case SAVE_FILE:
					if(fnam[0])
					{
						save();
						break;
					}
				case SAVE_AS:
					saveas();
					break;
				case OPEN_FILE:
					open();
					break;
				case EXIT_FILE:
					DestroyWindow(wnd);
					break;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			DefWindowProc(wnd, msg, wp, lp);
	}
}

void menuinit(HWND wnd)
{
	menu = CreateMenu();
	HMENU file = CreateMenu();
	AppendMenu(file, MF_STRING, NEW_FILE, "New");
	AppendMenu(file, MF_STRING, OPEN_FILE, "Open");
	AppendMenu(file, MF_STRING, SAVE_FILE, "Save");
	AppendMenu(file, MF_SEPARATOR, 0, NULL);
	AppendMenu(file, MF_STRING, EXIT_FILE, "Exit");
	AppendMenu(menu, MF_POPUP, (UINT_PTR)file, "File");
	AppendMenu(menu, MF_STRING, RUN_FILE, "Run");
	SetMenu(wnd, menu);
}

void textinit(HWND wnd)
{
	txwnd = CreateWindow("edit", NULL, 
			WS_VISIBLE|WS_CHILD|WS_TABSTOP|
			ES_WANTRETURN|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL, 
			0, 0, 500, 400, wnd, 
			NULL, NULL, NULL);
	SetFocus(txwnd);
}

int new()
{
	//add dialog instd if msg box
	int mb = MessageBox(txwnd, "Do you want to save?", "Workbench",
			MB_YESNO|MB_ICONWARNING);
	if(mb==IDYES) {
		if(fnam[0]) save();
		else saveas();
	}
	SetWindowText(txwnd, "");
	fnam[0] = 0;
	SetFocus(txwnd);
}

int open()
{
	OPENFILENAME file = {sizeof(OPENFILENAME)};
	file.hwndOwner = txwnd;
	file.lpstrFilter = "\0";
	file.lpstrTitle = "Open";
	file.nMaxFile = sizeof(fnam);
	file.lpstrFile = fnam;
	
	
	if(!GetOpenFileName(&file)) return 0;

	int n=0;
	FILE *f = fopen(fnam, "rb");
	if(f==NULL) return 0;
	while(fgetc(f)!=EOF) n++;
	fseek(f, 0, SEEK_SET);
	char buf[n];
	fread(buf, 1, n, f);
	buf[n-1] = '\0';
	fclose(f);

	SetWindowText(txwnd, buf);
	SendMessage(txwnd, EM_SETSEL, n, n);
	
	return 1;
}

int saveas()
{
	OPENFILENAME file = {sizeof(OPENFILENAME)};
	file.hwndOwner = txwnd;
	file.lpstrFilter = "\0";
	file.lpstrTitle = "Save As";
	file.nMaxFile = sizeof(fnam);
	file.lpstrFile = fnam;

	if(!GetSaveFileName(&file)) return 0;
	save();
	return 1;
}

int save()
{
	int n=0;
	FILE *f = fopen(fnam, "wb");
	if(f==NULL) return 0;
	n = GetWindowTextLength(txwnd)+1;
	char buf[n];
	GetWindowText(txwnd, buf, n);
	buf[n-1] = '\0';

	fwrite(buf, 1, n, f);
	fclose(f);

	return 1;
}
