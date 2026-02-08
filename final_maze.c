#include <stdio.h>
#include <stdlib.h>

// --- Cross-Platform input handling (Windows/Linux/Mac) ---
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    void setup_console() {
        // Enable ANSI colors for Windows 10/11
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
        SetConsoleMode(hOut, dwMode);
    }
    void clear_screen() { system("cls"); }
#else
    #include <termios.h>
    #include <unistd.h>
    // Custom getch implementation for Linux/Mac
    int getch(void) {
        struct termios oldattr, newattr;
        int ch;
        tcgetattr(STDIN_FILENO, &oldattr);
        newattr = oldattr;
        newattr.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
        return ch;
    }
    void setup_console() { }
    void clear_screen() { system("clear"); }
#endif

// --- Constants & Config ---
#define WIDTH 15

#define HEIGHT 10

// ANSI Colors
#define C_RESET  "\033[0m"
#define C_WALL   "\033[48;5;236m\033[38;5;236m" // Slate-600 look
#define C_PATH   "\033[48;5;240m\033[38;5;240m" // Lighter slate
#define C_PLAYER "\033[48;5;206m\033[38;5;206m" // Pink-500
#define C_GOAL   "\033[48;5;46m\033[38;5;46m"   // Green-500
#define C_TEXT   "\033[37m\033[49m"             // White text

// The Maze Grid (1=Wall, 0=Path)
const int GRID[HEIGHT][WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

int px = 1, py = 1;    // Player Start
int gx = 13, gy = 1;   // Goal
int moves = 0;

void draw_game() {
    // Move cursor to top-left rather than clearing to reduce flicker
    printf("\033[H"); 

    printf(C_TEXT "\n  == CYBER MAZE ==\n");
    printf("  Use WASD to Move | Moves: %d\n\n", moves);

    for (int y = 0; y < HEIGHT; y++) {
        printf("  "); // Margin
        for (int x = 0; x < WIDTH; x++) {
            if (x == px && y == py) {
                printf(C_PLAYER "  " C_RESET);
            } else if (x == gx && y == gy) {
                printf(C_GOAL "  " C_RESET);
            } else if (GRID[y][x] == 1) {
                printf(C_WALL "  " C_RESET);
            } else {
                printf(C_PATH "  " C_RESET);
            }
        }
        printf("\n");
    }
    printf("\n" C_RESET);
}

void show_win_screen() {
    clear_screen();
    printf("\n\n");
    printf(C_GOAL "  * * * * * * * * * * * *  " C_RESET "\n");
    printf(C_GOAL "  *     I   W O N !     *  " C_RESET "\n");
    printf(C_GOAL "  * * * * * * * * * * * *  " C_RESET "\n\n");
    
    //printf("     [Confetti Image Placeholder]\n\n");
}

int main() {
    setup_console();
    clear_screen();

    char input;
    int running = 1;

    while (running) {
        draw_game();

        // Check Win
        if (px == gx && py == gy) {
            show_win_screen();
            break;
        }

        // Get Input (WASD)
        input = getch();
        
        int dx = 0, dy = 0;
        if (input == 'w' || input == 'W') dy = -1;
        if (input == 's' || input == 'S') dy = 1;
        if (input == 'a' || input == 'A') dx = -1;
        if (input == 'd' || input == 'D') dx = 1;
        if (input == 'q') running = 0; // Quit option

        // Attempt Move
        if (dx != 0 || dy != 0) {
            int nx = px + dx;
            int ny = py + dy;

            // Bounds check
            if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
                // Wall check
                if (GRID[ny][nx] == 0) {
                    px = nx;
                    py = ny;
                    moves++;
                }
            }
        }
    }

    return 0;
}