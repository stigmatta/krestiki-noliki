#include <Windows.h>
#include "resource.h"

HINSTANCE hInstance;

enum class Cell {
    Empty,
    Cross,
    Circle
};

Cell board[3][3] = { {Cell::Empty, Cell::Empty, Cell::Empty},
                          {Cell::Empty, Cell::Empty, Cell::Empty},
                          {Cell::Empty, Cell::Empty, Cell::Empty} };

BOOL CheckWin(Cell player) {
    for (int i = 0; i < 3; ++i) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player)
            return TRUE;
    }

    for (int j = 0; j < 3; ++j) {
        if (board[0][j] == player && board[1][j] == player && board[2][j] == player)
            return TRUE;
    }

    if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
        (board[0][2] == player && board[1][1] == player && board[2][0] == player))
        return TRUE;

    return FALSE;
}

BOOL CheckDraw() {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == Cell::Empty)
                return FALSE; 
        }
    }
    return TRUE; 
}

void DrawSymbol(HWND hButton, Cell symbol) {
    HBITMAP hBitmap = NULL;
    switch (symbol) {
        case Cell::Cross:
            hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
            break;
        case Cell::Circle:
            hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
            break;
    }
    SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);
}

void MakeMove(HWND hButton, int row, int col, Cell player) {
    board[row][col] = player;
    DrawSymbol(hButton, player);
}

void BotTurn(HWND hwnd) {
    int row, col;
    do {
        row = rand() % 3;
        col = rand() % 3;
    } while (board[row][col] != Cell::Empty);
    HWND hButton = GetDlgItem(hwnd, 1014 + row * 3 + col);
    MakeMove(hButton, row, col, Cell::Circle);
}


INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hButtons[9];
    static HWND hStartNew, hFirstTurn;
    static Cell currentPlayer = Cell::Cross; 
    static BOOL startWithCross = TRUE;

    switch (uMsg) {
    case WM_INITDIALOG:
    {
        for (int i = 0; i < 9; ++i) {
            hButtons[i] = GetDlgItem(hwnd, 1014 + i);
            SendMessage(hButtons[i], BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)NULL);
        }
        hStartNew = GetDlgItem(hwnd, IDC_STARTNEW);
        hFirstTurn = GetDlgItem(hwnd, IDC_FIRSTTURN);
        break;
    }
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == IDC_STARTNEW && HIWORD(wParam) == BN_CLICKED) {
            MessageBox(hwnd, L"Game Started", L"Good Luck", MB_OK | MB_ICONINFORMATION);
            if (SendMessage(hFirstTurn, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                for (int i = 0; i < 9; ++i) {
                    EnableWindow(hButtons[i], TRUE);
                }
            }
            for (int i = 0; i < 9; ++i) {
                SendMessage(hButtons[i], BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)NULL);
            }
            currentPlayer = Cell::Cross; // Начинаем с крестика
        }
        else if (LOWORD(wParam) >= 1014 && LOWORD(wParam) <= 1022) {
            HWND hButton = GetDlgItem(hwnd, LOWORD(wParam));
            if (hButton != NULL && IsWindowEnabled(hButton)) {
                int index = LOWORD(wParam) - 1014;
                int row = index / 3;
                int col = index % 3;
                MakeMove(hButton, row, col, currentPlayer);
                if (CheckWin(currentPlayer)) {
                    if (currentPlayer == Cell::Cross)
                        MessageBox(hwnd, L"Congratulations! You win!", L"Game Over", MB_OK | MB_ICONINFORMATION);
                    else
                        MessageBox(hwnd, L"Sorry, you lost!", L"Game Over", MB_OK | MB_ICONINFORMATION);
                    break;
                }
                else if (CheckDraw()) {
                    MessageBox(hwnd, L"It's a draw!", L"Game Over", MB_OK | MB_ICONINFORMATION);
                    break;
                }
                currentPlayer = (currentPlayer == Cell::Cross) ? Cell::Circle : Cell::Cross;
                BotTurn(hwnd);
                // После хода бота, меняем текущего игрока на игрока
                currentPlayer = (currentPlayer == Cell::Cross) ? Cell::Circle : Cell::Cross;
                if (CheckWin(currentPlayer)) {
                    if (currentPlayer == Cell::Cross)
                        MessageBox(hwnd, L"Congratulations! You win!", L"Game Over", MB_OK | MB_ICONINFORMATION);
                    else
                        MessageBox(hwnd, L"Sorry, you lost!", L"Game Over", MB_OK | MB_ICONINFORMATION);
                    break;
                }
                else if (CheckDraw()) {
                    MessageBox(hwnd, L"It's a draw!", L"Game Over", MB_OK | MB_ICONINFORMATION);
                    break;
                }
            }
        }
        return TRUE;
    }
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
    }

    return FALSE;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
    srand(GetTickCount());
    ::hInstance = hInstance;
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}
