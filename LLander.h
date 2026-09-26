#pragma string name LLander
#pragma output nostreams

void DrawCursor(void);
void DrawCell(unsigned char pos);
void RevealAllMines(void);
void SetupLevel(void);
void DrawBoard(void);
char Gamekeys(void);
void MoveCursor(char plusx, char plusy);
void ToggleFlag(unsigned char pos);
char RevealCell(unsigned char pos);
void FloodReveal(unsigned char start);
void PlaceMines(void);
unsigned char CountAdjacentMines(unsigned char pos);
char CheckWin(void);
void DrawStatus(void);
void DrawTitle(void);
void DrawFlagCount(void);
void WinMessage(void);

extern unsigned char mineBoard[144];
extern unsigned char visibleBoard[144];
extern unsigned char cursorPos;        /* current cursor position */
extern int totalMines;                 /* number of mines on the board */
extern unsigned char gameOver;         /* 1 = game over */
extern unsigned char gameWon;          /* 1 = player won */
extern int startMines;                 /* starting mine count for next level */
extern unsigned char flagsLeft;        /* remaining flags to place */


#define STARTMINECOUNT 12

#define N_COLS    16
#define N_ROWS     9
#define N_CELLS  144

#define TILE_HIDDEN   0
#define TILE_REVEALED 1
#define TILE_FLAGGED  2

#define MINE          9

#define MAXMINES     24

#define TRUE         1
#define FALSE        0

#define K_NEXTLEV '+'
#define K_PREVLEV '-'
#define K_FLAG     'F'

#define K_UP       'q'  /* arrow up     */
#define K_DOWN     'a' /* arrow down   */
#define K_LEFT     'o' /* arrow left   */
#define K_RIGHT    'p' /* arrow right  */
#define K_SWITCH   ' '  /* [SPACE]      */
#define K_EXIT     'g' /* [Esc]/[Quit] */
#define K_CLEAR    'h'

extern char levels[];
extern char sprites[];

#asm
._sprites
 defb    16,16
 defw    0,0,0,0,0,0,0,0	; empty sprite
 defw    0,0,0,0,0,0,0,0
 
 defb    16,16
 defb    @01111111, @11111110	;1=edge,
 defb    @10101010, @10101001
 defb    @11010101, @01000001
 defb    @10101000, @00000001
 defb    @11010000, @00000001
 defb    @10100000, @00000001
 defb    @11000000, @00000001
 defb    @10000000, @00000001
 defb    @10000000, @00000001
 defb    @10000000, @00000001
 defb    @10000000, @00000001
 defb    @10000000, @00000001
 defb    @10000000, @00000001
 defb    @10000000, @00000001
 defb    @10000000, @00000001
 defb    @01111111, @11111110

 defb    16,16
 defb    @00000000, @00000000	;2=bubble,
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000011, @11000000
 defb    @00000100, @00100000
 defb    @00001000, @10010000
 defb    @00001000, @01010000
 defb    @00001000, @00010000
 defb    @00001000, @00010000
 defb    @00000100, @00100000
 defb    @00000011, @11000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000

 defb    16,16
 defb    @00000000, @00000000	;3=moveable ball
 defb    @00000000, @00000000
 defb    @00000011, @11000000
 defb    @00001111, @00110000
 defb    @00011111, @11011000
 defb    @00011111, @11101000
 defb    @00111111, @11101100
 defb    @00111111, @11111100
 defb    @00111111, @11111100
 defb    @00111111, @11111100
 defb    @00011111, @11111000
 defb    @00011111, @11111000
 defb    @00001111, @11110000
 defb    @00000011, @11000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000

 defb    16,16
 defb    @00000000, @00000000	;4=moveable block
 defb    @01111111, @11111110
 defb    @01001101, @10110010
 defb    @01011111, @11111010
 defb    @01110000, @00001110
 defb    @01110000, @00001110
 defb    @01010000, @00001010
 defb    @01110000, @00001110
 defb    @01110000, @00001110
 defb    @01010000, @00001010
 defb    @01110000, @00001110
 defb    @01110000, @00001110
 defb    @01011111, @11111010
 defb    @01001101, @10110010
 defb    @01111111, @11111110
 defb    @00000000, @00000000

 defb    16,16
 defb    @00000000, @00000000	;5=number 1
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000001, @11000000
 defb    @00000110, @11000000
 defb    @00000000, @11000000
 defb    @00000000, @11000000
 defb    @00000000, @11000000
 defb    @00000000, @11000000
 defb    @00000000, @11000000
 defb    @00000000, @11000000
 defb    @00000000, @11000000
 defb    @00000111, @11100000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000

 defb    16,16
 defb    @00000000, @00000000	;6=number 2
 defb    @00000000, @00000000
 defb    @00000111, @00000000
 defb    @00001001, @10000000
 defb    @00000001, @10000000
 defb    @00000001, @10000000
 defb    @00000111, @10000000
 defb    @00001100, @00000000
 defb    @00011000, @00000000
 defb    @00011000, @00000000
 defb    @00011000, @00000000
 defb    @00011111, @10000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000

 defb    16,16
 defb    @00000000, @00000000	;7=number 3
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000111, @11000000
 defb    @00000100, @01100000
 defb    @00000000, @01100000
 defb    @00000001, @11000000
 defb    @00000000, @01100000
 defb    @00000000, @01100000
 defb    @00000111, @10000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000

 defb    16,16
 defb    @00000000, @00000000	;8=number 4
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00001100, @11000000
 defb    @00001100, @11000000
 defb    @00001100, @11000000
 defb    @00001111, @11000000
 defb    @00000000, @11000000
 defb    @00000000, @11000000
 defb    @00000000, @11000000
 defb    @00000000, @11000000
 defb    @00000000, @11000000
 defb    @00000000, @11000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000

 defb    16,16
 defb    @00000000, @00000000	;9=number 5
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000111, @10000000
 defb    @00000100, @00000000
 defb    @00000100, @00000000
 defb    @00000111, @10000000
 defb    @00000001, @10000000
 defb    @00000001, @10000000
 defb    @00000111, @10000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000

 defb    16,16
 defb    @00000000, @00000000	;10=number 6
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000111, @10000000
 defb    @00000100, @00000000
 defb    @00000100, @00000000
 defb    @00000111, @10000000
 defb    @00000101, @10000000
 defb    @00000101, @10000000
 defb    @00000111, @10000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000

 defb    16,16
 defb    @00000000, @00000000	;11=number 7
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000111, @10000000
 defb    @00000001, @10000000
 defb    @00000001, @10000000
 defb    @00000001, @10000000
 defb    @00000001, @10000000
 defb    @00000001, @10000000
 defb    @00000001, @10000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000

 defb    16,16
 defb    @00000000, @00000000	;12=number 8
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000111, @10000000
 defb    @00000101, @10000000
 defb    @00000111, @10000000
 defb    @00000101, @10000000
 defb    @00000101, @10000000
 defb    @00000111, @10000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
#endasm
