/*
 * Minesweeper for ZX Spectrum / Z88DK
 *
 * Uses some sprites from the DStar demo - otherwise all original code.
 * 
 * Ambrose Clarke - July 2026
 * 
 */

#include <stdio.h>
#include <games.h>
#include <stdlib.h>
#include <graphics.h>
#include <conio.h>
#include <input.h>

#define SCREEN_ATTRIBUTES 22528
#define ATTR_BRIGHT 64
#define ATTR_NORMAL_WHITE 7
#define ATTR_WHITE (ATTR_BRIGHT | 7)
#define ATTR_BLACK (7 << 3)
#define SCREEN_CENTER_X 128
#define SCREEN_CENTER_Y 96
#define FRAME_COUNT 28
#define MAX_SPRITE_BYTES 242
#define ROTATION_STEP 3
#define FIXED_SCALE 16
#define FIX15_SHIFT 15
#define GRAVITY_ACCEL 1
#define THRUST_ACCEL 4
#define THRUST_FUEL_RATE 2
#define STARTING_FUEL 1000
#define MAX_LANDING_SPEED_TENTHS 12
#define MAX_LANDING_ANGLE 10
#define ENABLE_HORIZONTAL_DAMPING 1
#define HORIZONTAL_DAMPING_RETAINED 63
#define HORIZONTAL_DAMPING_DIVISOR 64
#define LANDER_SOLID_ROWS 3
#define SPEED_DISPLAY_SCALE 10
#define TERRAIN_COLUMNS 33
#define TERRAIN_START_X 0
#define TERRAIN_START_Y 176
#define TERRAIN_MIN_Y 144
#define TERRAIN_MAX_Y 176
#define TERRAIN_FILL_BOTTOM 181
#define TERRAIN_FILL_MASK 3
#define LANDING_PAD_FIRST 16
#define LANDING_PAD_LAST 23
#define TEXT_SCREEN_WIDTH 60

typedef signed int fix15;

#define FIX15_MULT(a, b) ((fix15)(((signed long)(a) * (signed long)(b)) >> FIX15_SHIFT))
#define FIX15_DIV(a, b) ((fix15)(((signed long)(a) * (1L << FIX15_SHIFT)) / (signed long)(b)))
#define FIX15_ADD(a, b) ((fix15)((a) + (b)))
#define FIX15_SUB(a, b) ((fix15)((a) - (b)))

extern unsigned char lander_frame_q0_00[];
extern unsigned char lander_frame_q0_15[];
extern unsigned char lander_frame_q0_30[];
extern unsigned char lander_frame_q0_45[];
extern unsigned char lander_frame_q0_60[];
extern unsigned char lander_frame_q0_75[];
extern unsigned char lander_frame_q0_90[];
extern unsigned char lander_frame_q1_00[];
extern unsigned char lander_frame_q1_15[];
extern unsigned char lander_frame_q1_30[];
extern unsigned char lander_frame_q1_45[];
extern unsigned char lander_frame_q1_60[];
extern unsigned char lander_frame_q1_75[];
extern unsigned char lander_frame_q1_90[];
extern unsigned char lander_frame_q2_00[];
extern unsigned char lander_frame_q2_15[];
extern unsigned char lander_frame_q2_30[];
extern unsigned char lander_frame_q2_45[];
extern unsigned char lander_frame_q2_60[];
extern unsigned char lander_frame_q2_75[];
extern unsigned char lander_frame_q2_90[];
extern unsigned char lander_frame_q3_00[];
extern unsigned char lander_frame_q3_15[];
extern unsigned char lander_frame_q3_30[];
extern unsigned char lander_frame_q3_45[];
extern unsigned char lander_frame_q3_60[];
extern unsigned char lander_frame_q3_75[];
extern unsigned char lander_frame_q3_90[];
extern unsigned char lander_no_flame_q0_00[];
extern unsigned char lander_no_flame_q0_15[];
extern unsigned char lander_no_flame_q0_30[];
extern unsigned char lander_no_flame_q0_45[];
extern unsigned char lander_no_flame_q0_60[];
extern unsigned char lander_no_flame_q0_75[];
extern unsigned char lander_no_flame_q0_90[];
extern unsigned char lander_no_flame_q1_00[];
extern unsigned char lander_no_flame_q1_15[];
extern unsigned char lander_no_flame_q1_30[];
extern unsigned char lander_no_flame_q1_45[];
extern unsigned char lander_no_flame_q1_60[];
extern unsigned char lander_no_flame_q1_75[];
extern unsigned char lander_no_flame_q1_90[];
extern unsigned char lander_no_flame_q2_00[];
extern unsigned char lander_no_flame_q2_15[];
extern unsigned char lander_no_flame_q2_30[];
extern unsigned char lander_no_flame_q2_45[];
extern unsigned char lander_no_flame_q2_60[];
extern unsigned char lander_no_flame_q2_75[];
extern unsigned char lander_no_flame_q2_90[];
extern unsigned char lander_no_flame_q3_00[];
extern unsigned char lander_no_flame_q3_15[];
extern unsigned char lander_no_flame_q3_30[];
extern unsigned char lander_no_flame_q3_45[];
extern unsigned char lander_no_flame_q3_60[];
extern unsigned char lander_no_flame_q3_75[];
extern unsigned char lander_no_flame_q3_90[];

static unsigned char *lander_frames[FRAME_COUNT] = {
    lander_frame_q0_00, lander_frame_q0_15, lander_frame_q0_30,
    lander_frame_q0_45, lander_frame_q0_60, lander_frame_q0_75,
    lander_frame_q0_90, lander_frame_q1_00, lander_frame_q1_15,
    lander_frame_q1_30, lander_frame_q1_45, lander_frame_q1_60,
    lander_frame_q1_75, lander_frame_q1_90, lander_frame_q2_00,
    lander_frame_q2_15, lander_frame_q2_30, lander_frame_q2_45,
    lander_frame_q2_60, lander_frame_q2_75, lander_frame_q2_90,
    lander_frame_q3_00, lander_frame_q3_15, lander_frame_q3_30,
    lander_frame_q3_45, lander_frame_q3_60, lander_frame_q3_75,
    lander_frame_q3_90
};

static unsigned char *lander_no_flame_frames[FRAME_COUNT] = {
    lander_no_flame_q0_00, lander_no_flame_q0_15, lander_no_flame_q0_30,
    lander_no_flame_q0_45, lander_no_flame_q0_60, lander_no_flame_q0_75,
    lander_no_flame_q0_90, lander_no_flame_q1_00, lander_no_flame_q1_15,
    lander_no_flame_q1_30, lander_no_flame_q1_45, lander_no_flame_q1_60,
    lander_no_flame_q1_75, lander_no_flame_q1_90, lander_no_flame_q2_00,
    lander_no_flame_q2_15, lander_no_flame_q2_30, lander_no_flame_q2_45,
    lander_no_flame_q2_60, lander_no_flame_q2_75, lander_no_flame_q2_90,
    lander_no_flame_q3_00, lander_no_flame_q3_15, lander_no_flame_q3_30,
    lander_no_flame_q3_45, lander_no_flame_q3_60, lander_no_flame_q3_75,
    lander_no_flame_q3_90
};

static unsigned int key_space;
static unsigned int key_p;
static unsigned int key_q;
extern unsigned int ReadDigitKeys(void);
static unsigned char thrust_active;
static unsigned char paused;
static int lander_x_fp;
static int lander_y_fp;
static int velocity_x_fp;
static int velocity_y_fp;
static int thrust_x_remainder;
static int thrust_y_remainder;
static unsigned int fuel;
static unsigned char flight_state;
static unsigned int elapsed_frames;

static unsigned char terrain_y[TERRAIN_COLUMNS];

static fix15 sine_table[1024];

static unsigned char lander_frame_width[FRAME_COUNT] = {
    24, 32, 32, 40, 40, 48, 48,
    24, 32, 32, 40, 40, 48, 48,
    24, 32, 32, 40, 40, 48, 48,
    24, 32, 32, 40, 40, 48, 48
};

static unsigned char lander_frame_height[FRAME_COUNT] = {
    48, 48, 48, 40, 32, 32, 24,
    48, 48, 48, 40, 32, 32, 24,
    48, 48, 48, 40, 32, 32, 24,
    48, 48, 48, 40, 32, 32, 24
};

static unsigned char lander_no_flame_frame_width[FRAME_COUNT] = {
    24, 32, 32, 32, 32, 32, 24,
    24, 32, 32, 32, 32, 32, 24,
    24, 32, 32, 32, 32, 32, 24,
    24, 32, 32, 32, 32, 32, 24
};

static unsigned char lander_no_flame_frame_height[FRAME_COUNT] = {
    24, 32, 32, 32, 32, 32, 24,
    24, 32, 32, 32, 32, 32, 24,
    24, 32, 32, 32, 32, 32, 24,
    24, 32, 32, 32, 32, 32, 24
};

static unsigned char previous_x;
static unsigned char previous_y;
static unsigned char previous_width;
static unsigned char previous_height;
static unsigned char has_previous_sprite;
static unsigned char *previous_sprite;
extern unsigned char success_flag_0[];
extern unsigned char success_flag_1[];
extern unsigned char crashed_lander_row[];
extern unsigned char moonlander_wordmark[];
extern unsigned char moon_art[];

static unsigned char normal_stars[40] = {
    18, 22, 47, 35, 78, 20, 112, 31, 151, 18, 188, 28,
    224, 21, 241, 45, 31, 67, 61, 83, 199, 72, 232, 94,
    16, 118, 54, 137, 207, 126, 239, 151, 28, 171, 92, 163,
    178, 174, 221, 183
};

static unsigned char bright_stars[20] = {
    29, 14, 96, 42, 175, 36, 216, 58, 8, 91, 247, 111,
    43, 151, 189, 151, 116, 178, 154, 119
};

void PrintAt(int y, int x, const char *text)
{
    char buf[4];
    buf[0] = '\x16';
    buf[1] = (char)(0x20 + y);
    buf[2] = (char)(0x20 + x);
    buf[3] = '\0';
    fputs(buf, stdout);
    fputs(text, stdout);
}

#define STATUS_FIELD_WIDTH 20
void PrintCenteredAt(int y, const char *text)
{
    int len = 0;
    const char *p = text;
    int fieldWidth = STATUS_FIELD_WIDTH;
    int startX;
    char pad[STATUS_FIELD_WIDTH + 1];
    int i;

    while (*p++) len++;

    startX = (64 - fieldWidth) / 2;
    if (startX < 0) startX = 0;

    for (i = 0; i < STATUS_FIELD_WIDTH; i++) pad[i] = ' ';
    pad[STATUS_FIELD_WIDTH] = '\0';

    PrintAt(y, startX, pad);
    PrintAt(y, startX + (fieldWidth - len) / 2, text);
}

static void SetAttributeBlock(unsigned char x, unsigned char y,
                              unsigned char width, unsigned char height,
                              unsigned char attribute)
{
    unsigned char *attributes = (unsigned char *)SCREEN_ATTRIBUTES;
    unsigned char row;
    unsigned char column;

    for (row = 0; row < height; row++)
    {
        for (column = 0; column < width; column++)
        {
            attributes[(y + row) * 32 + x + column] = attribute;
        }
    }
}

static void SetSpriteAttributeBlock(int x, int y,
                                    unsigned char width,
                                    unsigned char height,
                                    unsigned char attribute)
{
    unsigned char cell_x = (unsigned char)(x / 8);
    unsigned char cell_y = (unsigned char)(y / 8);
    unsigned char cell_width =
        (unsigned char)(((x & 7) + width + 7) / 8);
    unsigned char cell_height =
        (unsigned char)(((y & 7) + height + 7) / 8);

    SetAttributeBlock(cell_x, cell_y, cell_width, cell_height, attribute);
}

static void DrawStars(void)
{
    unsigned char i;

    SetAttributeBlock(0, 0, 32, 24, 0);

    for (i = 0; i < 20; i++)
    {
        plot(normal_stars[i * 2], normal_stars[i * 2 + 1]);
        SetAttributeBlock(normal_stars[i * 2] / 8,
                          normal_stars[i * 2 + 1] / 8, 1, 1,
                          ATTR_NORMAL_WHITE);
    }
    for (i = 0; i < 10; i++)
    {
        plot(bright_stars[i * 2], bright_stars[i * 2 + 1]);
        SetAttributeBlock(bright_stars[i * 2] / 8,
                          bright_stars[i * 2 + 1] / 8, 1, 1,
                          ATTR_WHITE);
    }
}

static void GenerateTerrain(void)
{
    unsigned char column;
    static unsigned char mountain_profile[TERRAIN_COLUMNS] = {
        176, 172, 164, 172, 176, 172, 160, 148, 156, 172, 176,
        174, 162, 150, 158, 172, 168, 168, 168, 168, 168, 168,
        168, 168, 172, 158, 144, 152, 168, 176, 174, 160, 150
    };

    for (column = 0; column < TERRAIN_COLUMNS; column++)
    {
        terrain_y[column] = mountain_profile[column];
        if (terrain_y[column] < TERRAIN_MIN_Y)
            terrain_y[column] = TERRAIN_MIN_Y;
        if (terrain_y[column] > TERRAIN_MAX_Y)
            terrain_y[column] = TERRAIN_MAX_Y;
    }
}

static void PlotTerrainLine(int x1, int y1, int x2, int y2,
                            int clip_left, int clip_top,
                            int clip_right, int clip_bottom)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int step_x = dx < 0 ? -1 : 1;
    int step_y = dy < 0 ? -1 : 1;
    int error = (dx < 0 ? -dx : dx) - (dy < 0 ? -dy : dy);
    int error_twice;

    while (1)
    {
        if (x1 >= 0 && x1 < 256 && y1 >= 0 && y1 < 192 &&
            x1 >= clip_left && x1 < clip_right &&
            y1 >= clip_top && y1 < clip_bottom)
        {
            SetAttributeBlock((unsigned char)(x1 / 8),
                              (unsigned char)(y1 / 8), 1, 1,
                              ATTR_WHITE);
            plot(x1, y1);
        }
        if (x1 == x2 && y1 == y2) break;
        error_twice = error * 2;
        if (error_twice > -(dy < 0 ? -dy : dy))
        {
            error -= (dy < 0 ? -dy : dy);
            x1 += step_x;
        }
        if (error_twice < (dx < 0 ? -dx : dx))
        {
            error += (dx < 0 ? -dx : dx);
            y1 += step_y;
        }
    }
}

static void DrawTerrainFillInRect(int clip_x, int clip_y,
                                 int clip_width, int clip_height)
{
    int x_start = clip_x < 0 ? 0 : clip_x;
    int x_end = clip_x + clip_width;
    int y_end = clip_y + clip_height;
    int x;
    int column;
    int offset;
    int surface_y;
    int y_start;
    int y;

    if (x_end > 256) x_end = 256;
    if (y_end > TERRAIN_FILL_BOTTOM + 1) y_end = TERRAIN_FILL_BOTTOM + 1;
    if (clip_y < 0) clip_y = 0;

    for (x = x_start; x < x_end; x++)
    {
        column = x / 8;
        if (column >= TERRAIN_COLUMNS - 1) column = TERRAIN_COLUMNS - 2;
        offset = x - column * 8;
        surface_y = terrain_y[column] +
            ((terrain_y[column + 1] - terrain_y[column]) * offset) / 8;
        y_start = surface_y + 1;
        if (y_start < clip_y) y_start = clip_y;

        y = y_start;
        while (y < y_end && ((x + y * 3) & TERRAIN_FILL_MASK) != 0)
        {
            y++;
        }
        for (; y < y_end; y += TERRAIN_FILL_MASK + 1)
        {
            SetAttributeBlock((unsigned char)(x / 8),
                              (unsigned char)(y / 8), 1, 1,
                              ATTR_WHITE);
            plot(x, y);
        }
    }
}

static void RestoreStarsInRect(unsigned char x, unsigned char y,
                               unsigned char width, unsigned char height)
{
    unsigned char i;
    unsigned char star_x;
    unsigned char star_y;

    for (i = 0; i < 20; i++)
    {
        star_x = normal_stars[i * 2];
        star_y = normal_stars[i * 2 + 1];
        if (star_x >= x && star_x < x + width &&
            star_y >= y && star_y < y + height)
        {
            SetAttributeBlock(star_x / 8, star_y / 8, 1, 1,
                              ATTR_NORMAL_WHITE);
            plot(star_x, star_y);
        }
    }
    for (i = 0; i < 10; i++)
    {
        star_x = bright_stars[i * 2];
        star_y = bright_stars[i * 2 + 1];
        if (star_x >= x && star_x < x + width &&
            star_y >= y && star_y < y + height)
        {
            SetAttributeBlock(star_x / 8, star_y / 8, 1, 1,
                              ATTR_WHITE);
            plot(star_x, star_y);
        }
    }
}

static void DrawTerrainInRect(unsigned char x, unsigned char y,
                              unsigned char width, unsigned char height)
{
    unsigned char column;
    unsigned char left = x / 8;
    unsigned char right = (unsigned char)((x + width - 1) / 8);

    DrawTerrainFillInRect(x, y, width, height);
    if (right >= TERRAIN_COLUMNS - 1) right = TERRAIN_COLUMNS - 2;
    for (column = left; column <= right; column++)
    {
        PlotTerrainLine(column * 8, terrain_y[column],
                column == TERRAIN_COLUMNS - 2 ? 255 : (column + 1) * 8,
                terrain_y[column + 1], x, y, x + width, y + height);
    }
}

static void RestoreSpriteBackground(unsigned char x, unsigned char y,
                                    unsigned char width,
                                    unsigned char height)
{
    unsigned char cell_x = (unsigned char)(x / 8);
    unsigned char cell_y = (unsigned char)(y / 8);
    unsigned char cell_width =
        (unsigned char)(((x & 7) + width + 7) / 8);
    unsigned char cell_height =
        (unsigned char)(((y & 7) + height + 7) / 8);
    unsigned char restore_x = (unsigned char)(cell_x * 8);
    unsigned char restore_y = (unsigned char)(cell_y * 8);
    unsigned char restore_width = (unsigned char)(cell_width * 8);
    unsigned char restore_height = (unsigned char)(cell_height * 8);

    SetAttributeBlock(cell_x, cell_y, cell_width, cell_height, 0);
    RestoreStarsInRect(restore_x, restore_y, restore_width, restore_height);
    DrawTerrainInRect(restore_x, restore_y, restore_width, restore_height);
}

static void DrawTerrain(void)
{
    unsigned char column;

    DrawTerrainFillInRect(0, 0, 256, TERRAIN_FILL_BOTTOM + 1);
    for (column = 0; column < TERRAIN_COLUMNS - 1; column++)
    {
        SetAttributeBlock(column, terrain_y[column] / 8, 1, 1,
                          ATTR_WHITE);
        PlotTerrainLine(column * 8, terrain_y[column],
                column == TERRAIN_COLUMNS - 2 ? 255 : (column + 1) * 8,
            terrain_y[column + 1], 0, 0, 256, 192);
    }
    SetAttributeBlock(TERRAIN_START_X / 8, TERRAIN_START_Y / 8,
                      32, 1, ATTR_WHITE);
}

static int GetLanderSolidBottom(unsigned int angle, int pivot_y);

static void DrawTriangleLogoCell(unsigned char cell_x, unsigned char cell_y,
                                 unsigned char ink, unsigned char paper)
{
    unsigned char px;
    unsigned char py;

    SetAttributeBlock(cell_x, cell_y, 1, 1,
                      (unsigned char)(ink | (paper << 3)));
    for (py = 0; py < 8; py++)
    {
        for (px = 0; px < 8; px++)
        {
            /* Two triangular halves make neighboring colour cells read as
               diagonal bands rather than solid attribute blocks. */
            if (px + py >= 7)
                plot(cell_x * 8 + px, cell_y * 8 + py);
        }
    }
}

static void DrawTitle(void)
{
    static unsigned char logo_ink[14] = {
        2, 6, 4, 5, 0, 0, 0,
        6, 4, 5, 0, 0, 0, 0
    };
    static unsigned char logo_paper[14] = {
        0, 2, 6, 4, 0, 0, 0,
        2, 6, 4, 5, 0, 0, 0
    };
    unsigned char cell;

    /* White title text on black ground; logo cells override this with the
       Spectrum ink/paper colour pairs that form diagonal rainbow triangles. */
    clga(0, 0, 144, 16);
    SetAttributeBlock(0, 0, 32, 1, ATTR_WHITE);
    for (cell = 0; cell < 4; cell++)
    {
        DrawTriangleLogoCell(cell, 0, logo_ink[cell], logo_paper[cell]);
        DrawTriangleLogoCell(cell, 1, logo_ink[cell + 7], logo_paper[cell + 7]);
    }
    SetAttributeBlock(5, 0, 25, 2, ATTR_WHITE);
    putsprite(spr_or, 64-20, 4, moonlander_wordmark);
    //text upper right corner
    PrintAt(0, 40, "         ZX SPECTRUM 48K");
    PrintAt(1, 40, "Ambrose - Cambridge 2026");
    SetAttributeBlock(15, 0, 17, 2, (ATTR_BRIGHT | 5));
    //draw moon
    #define CYAN 5
    SetAttributeBlock(19, 7, 3, 3, CYAN);
    putsprite(spr_or, 152, 56, moon_art);
}

static void DrawHudLabels(void)
{
    DrawTitle();
    PrintAt(2, 1, "TIME");
    PrintAt(3, 1, "ALT");
    PrintAt(4, 1, "H.SPD");
    PrintAt(5, 1, "V.SPD");
    PrintAt(6, 1, "FUEL");
    PrintAt(2, 21+25, "THRUST");
    PrintAt(3, 21+25, "ANGLE");
    PrintAt(5, 21+25, "SCORE");
    PrintAt(23, 1, "1-9 ANGLE+THRUST P-PAUSE");
    //instructions
    SetAttributeBlock(0, 23, 15, 1, 6<<3);
    //the status panels
    SetAttributeBlock(0, 2, 10, 5, 7+(1<<3));
    SetAttributeBlock(22, 2, 10, 5, 7+(1<<3));

}

static void UpdateSpeedField(int row, int speed_fp)
{
    char value[12];
    int speed_tenths;
    int absolute_speed;
    char sign = '+';

    speed_tenths = (speed_fp * SPEED_DISPLAY_SCALE) / FIXED_SCALE;
    if (speed_tenths < 0)
    {
        sign = '-';
        absolute_speed = -speed_tenths;
    }
    else
    {
        absolute_speed = speed_tenths;
    }
    sprintf(value, "%c%d.%d ", sign, absolute_speed / 10,
            absolute_speed % 10);
    PrintAt(row, 10, value);
}

static void UpdateHud(unsigned int angle)
{
    char value[12];
    int altitude;
    int thrust_percent;
    int score;
    int terrain_column;

    terrain_column = (lander_x_fp / FIXED_SCALE) / 8;
    if (terrain_column < 0) terrain_column = 0;
    if (terrain_column >= TERRAIN_COLUMNS - 1)
        terrain_column = TERRAIN_COLUMNS - 2;
    altitude = terrain_y[terrain_column] -
               GetLanderSolidBottom(angle, lander_y_fp / FIXED_SCALE);
    if (altitude < 0) altitude = 0;
    thrust_percent = thrust_active && fuel > 0 ? 100 : 0;
    score = fuel;

    sprintf(value, "%u ", elapsed_frames / 50);
    PrintAt(2, 10, value);
    sprintf(value, "%d ", altitude);
    PrintAt(3, 10, value);
    UpdateSpeedField(4, velocity_x_fp);
    UpdateSpeedField(5, velocity_y_fp);
    sprintf(value, "%u ", fuel);
    PrintAt(6, 10, value);
    sprintf(value, "%d%%  ", thrust_percent);
    PrintAt(2, 29+25, value);
    sprintf(value, "%d  ", angle > 180 ? angle - 360 : angle);
    PrintAt(3, 29+25, value);
    sprintf(value, "%d ", score);
    PrintAt(5, 28+25, value);
}

static unsigned char RoundAngleToFrame(unsigned int angle)
{
    unsigned char frame = (unsigned char)((angle + 7) / 15);
    if (frame > 6) frame = 6;
    return frame;
}

static void GetLanderSpriteBounds(unsigned int angle, int pivot_x, int pivot_y,
                                  unsigned char flame_on,
                                  unsigned char *frame_out,
                                  unsigned char *width_out,
                                  unsigned char *height_out,
                                  int *x_out, int *y_out)
{
    unsigned int normalized = angle % 360;
    unsigned char acute;
    unsigned char quadrant = (unsigned char)(normalized / 90);
    unsigned char pivot_cell_x;
    unsigned char pivot_cell_y;

    if (normalized <= 90) acute = (unsigned char)normalized;
    else if (normalized <= 180) acute = (unsigned char)(180 - normalized);
    else if (normalized <= 270) acute = (unsigned char)(normalized - 180);
    else acute = (unsigned char)(360 - normalized);

    *frame_out = (unsigned char)(quadrant * 7 + RoundAngleToFrame(acute));
    if (flame_on)
    {
        *width_out = lander_frame_width[*frame_out];
        *height_out = lander_frame_height[*frame_out];
    }
    else
    {
        *width_out = lander_no_flame_frame_width[*frame_out];
        *height_out = lander_no_flame_frame_height[*frame_out];
    }
    pivot_cell_x = (unsigned char)(*width_out / 8 - 2);
    pivot_cell_y = (unsigned char)(*height_out / 8 - 2);
    if (quadrant < 2) pivot_cell_x = 1;
    if (quadrant == 0 || quadrant == 3) pivot_cell_y = 1;
    *x_out = pivot_x - pivot_cell_x * 8;
    *y_out = pivot_y - pivot_cell_y * 8;
}

static void GetLanderBottom(unsigned int angle, int pivot_y,
                            int *bottom)
{
    unsigned int normalized = angle % 360;
    unsigned char acute;
    unsigned char quadrant = (unsigned char)(normalized / 90);
    unsigned char frame;
    unsigned char pivot_cell_y;

    if (normalized <= 90) acute = (unsigned char)normalized;
    else if (normalized <= 180) acute = (unsigned char)(180 - normalized);
    else if (normalized <= 270) acute = (unsigned char)(normalized - 180);
    else acute = (unsigned char)(360 - normalized);

    frame = (unsigned char)(quadrant * 7 + RoundAngleToFrame(acute));
    pivot_cell_y = (unsigned char)(lander_frame_height[frame] / 8 - 2);
    if (quadrant == 0 || quadrant == 3) pivot_cell_y = 1;
    *bottom = pivot_y + lander_frame_height[frame] - pivot_cell_y * 8;
}

static int GetLanderSolidBottom(unsigned int angle, int pivot_y)
{
    unsigned int normalized = angle % 360;
    unsigned char quadrant = (unsigned char)(normalized / 90);
    unsigned char acute;
    unsigned char frame;
    unsigned char pivot_cell_y;
    int sprite_y;

    if (normalized <= 90) acute = (unsigned char)normalized;
    else if (normalized <= 180) acute = (unsigned char)(180 - normalized);
    else if (normalized <= 270) acute = (unsigned char)(normalized - 180);
    else acute = (unsigned char)(360 - normalized);

    frame = (unsigned char)(quadrant * 7 + RoundAngleToFrame(acute));
    pivot_cell_y = (unsigned char)(lander_frame_height[frame] / 8 - 2);
    if (quadrant == 0 || quadrant == 3) pivot_cell_y = 1;
    sprite_y = pivot_y - pivot_cell_y * 8;

    if (quadrant == 1 || quadrant == 2)
    {
        /* Vertical mirroring places the three solid rows at the lower edge. */
        return sprite_y + lander_frame_height[frame];
    }
    return sprite_y + LANDER_SOLID_ROWS * 8;
}

static void ApplyThrust(unsigned int sine_index)
{
    fix15 thrust_x = (fix15)-sine_table[sine_index & 1023];
    fix15 thrust_y = (fix15)-sine_table[(sine_index + 256) & 1023];
    signed long x_numerator;
    signed long y_numerator;
    int x_delta;
    int y_delta;

    x_numerator = (signed long)thrust_x * THRUST_ACCEL +
                  thrust_x_remainder;
    y_numerator = (signed long)thrust_y * THRUST_ACCEL +
                  thrust_y_remainder;
    x_delta = (int)(x_numerator >> FIX15_SHIFT);
    y_delta = (int)(y_numerator >> FIX15_SHIFT);
    velocity_x_fp += x_delta;
    velocity_y_fp += y_delta;
    thrust_x_remainder = (int)(x_numerator -
                               (signed long)x_delta * 32768L);
    thrust_y_remainder = (int)(y_numerator -
                               (signed long)y_delta * 32768L);
}

static void InitializeSineTable(void)
{
    fix15 current_sine = 0;
    fix15 current_cosine = 32767;
    fix15 step_sine = 201;
    fix15 step_cosine = 32767;
    fix15 next_sine;
    fix15 next_cosine;
    unsigned int index;

    for (index = 0; index <= 256; index++)
    {
        sine_table[index] = current_sine;
        if (index < 256)
        {
            sine_table[512 - index] = current_sine;
            sine_table[512 + index] = (fix15)-current_sine;
        }
        if (index > 0)
            sine_table[1024 - index] = (fix15)-current_sine;

        next_sine = FIX15_ADD(FIX15_MULT(current_sine, step_cosine),
                              FIX15_MULT(current_cosine, step_sine));
        next_cosine = FIX15_SUB(FIX15_MULT(current_cosine, step_cosine),
                                FIX15_MULT(current_sine, step_sine));
        current_sine = next_sine;
        current_cosine = next_cosine;
    }
}

static unsigned char UpdatePhysics(unsigned int angle,
                                   unsigned int thrust_index)
{
    int bottom_lander_legs;
    unsigned char terrain_column;
    int abs_horizontal;
    int abs_vertical;
    unsigned char safe_angle;
    unsigned char frame;
    unsigned char width;
    unsigned char height;
    int sprite_x;
    int sprite_y;

    if (flight_state != 0) return 0;

    elapsed_frames++;

    velocity_y_fp += GRAVITY_ACCEL;
    if (thrust_active && fuel > 0)
    {
        ApplyThrust(thrust_index);
        if (fuel > THRUST_FUEL_RATE) fuel -= THRUST_FUEL_RATE;
        else fuel = 0;
    }
    else if (fuel == 0)
    {
        thrust_active = 0;
    }
#if ENABLE_HORIZONTAL_DAMPING
    if (!thrust_active)
    {
        velocity_x_fp = (velocity_x_fp * HORIZONTAL_DAMPING_RETAINED) /
                        HORIZONTAL_DAMPING_DIVISOR;
    }
#endif

    lander_x_fp += velocity_x_fp;
    lander_y_fp += velocity_y_fp;

    GetLanderSpriteBounds(angle, lander_x_fp / FIXED_SCALE,
                          lander_y_fp / FIXED_SCALE, thrust_active,
                          &frame, &width,
                          &height, &sprite_x, &sprite_y);
    if (sprite_x + width <= 0 || sprite_x >= 256 ||
        sprite_y + height <= 0 || sprite_y >= 192)
    {
        flight_state = 3;
        thrust_active = 0;
        velocity_x_fp = 0;
        velocity_y_fp = 0;
        return 1;
    }
    if (sprite_x < 0 || sprite_x + width > 256 ||
        sprite_y < 0 || sprite_y + height > 192)
        return 1;

    bottom_lander_legs = GetLanderSolidBottom(angle,
                                               lander_y_fp / FIXED_SCALE);
    terrain_column = (unsigned char)(lander_x_fp / FIXED_SCALE / 8);
    if (terrain_column >= TERRAIN_COLUMNS - 1)
        terrain_column = TERRAIN_COLUMNS - 2;

    if (bottom_lander_legs >= terrain_y[terrain_column])
    {
        abs_horizontal = velocity_x_fp < 0 ? -velocity_x_fp : velocity_x_fp;
        abs_vertical = velocity_y_fp < 0 ? -velocity_y_fp : velocity_y_fp;
        safe_angle = (unsigned char)(angle <= MAX_LANDING_ANGLE ||
                                     angle >= 360 - MAX_LANDING_ANGLE);
        if (terrain_column >= LANDING_PAD_FIRST &&
            terrain_column <= LANDING_PAD_LAST && safe_angle &&
            abs_horizontal * SPEED_DISPLAY_SCALE <=
                MAX_LANDING_SPEED_TENTHS * FIXED_SCALE &&
            abs_vertical * SPEED_DISPLAY_SCALE <=
                MAX_LANDING_SPEED_TENTHS * FIXED_SCALE)
        {
            flight_state = 1;
        }
        else
        {
            flight_state = 2;
        }
        velocity_x_fp = 0;
        velocity_y_fp = 0;
    }
    return 1;
}

static void DrawLander(unsigned int angle, int pivot_x, int pivot_y)
{
    unsigned char frame;
    unsigned char width;
    unsigned char height;
    unsigned char *current_sprite;
    int x;
    int y;

    GetLanderSpriteBounds(angle, pivot_x, pivot_y, thrust_active, &frame,
                          &width, &height, &x, &y);
    current_sprite = thrust_active ? lander_frames[frame] :
                                     lander_no_flame_frames[frame];
    if ((x < 0 || x + width > 256 || y < 0 || y + height > 192) &&
        !(x + width <= 0 || x >= 256 || y + height <= 0 || y >= 192))
        return;
    if (has_previous_sprite && previous_sprite == current_sprite &&
        previous_x == (unsigned char)x && previous_y == (unsigned char)y)
    {
        return;
    }
    if (has_previous_sprite)
    {
        putsprite(spr_xor, previous_x, previous_y, previous_sprite);
        RestoreSpriteBackground(previous_x, previous_y, previous_width,
                                previous_height);
    }
    if (x + width <= 0 || x >= 256 || y + height <= 0 || y >= 192)
    {
        has_previous_sprite = 0;
        return;
    }
    SetSpriteAttributeBlock(x, y, width, height, ATTR_WHITE);
    putsprite(spr_or, x, y, current_sprite);
    previous_x = (unsigned char)x;
    previous_y = (unsigned char)y;
    previous_width = width;
    previous_height = height;
    previous_sprite = current_sprite;
    has_previous_sprite = 1;
}

static void InitialiseKeys(void)
{
    key_space = in_LookupKey(' ');
    key_p = in_LookupKey('p');
    key_q = in_LookupKey('q');
}

static void StartFlight(unsigned int angle)
{
    clg();
    DrawStars();
    GenerateTerrain();
    DrawTerrain();

    lander_x_fp = 128 * FIXED_SCALE;
    lander_y_fp = 64 * FIXED_SCALE;
    velocity_x_fp = 0;
    velocity_y_fp = 0;
    thrust_x_remainder = 0;
    thrust_y_remainder = 0;
    fuel = STARTING_FUEL;
    elapsed_frames = 0;
    flight_state = 0;
    thrust_active = 0;
    paused = 0;
    has_previous_sprite = 0;
    DrawHudLabels();
    UpdateHud(angle);
    DrawLander(angle, lander_x_fp / FIXED_SCALE,
               lander_y_fp / FIXED_SCALE);
}

static void ShowFlightResult(unsigned int angle)
{
    if (flight_state == 2 && has_previous_sprite)
    {
        putsprite(spr_xor, previous_x, previous_y, previous_sprite);
        RestoreSpriteBackground(previous_x, previous_y, previous_width,
                                previous_height);
          SetAttributeBlock(previous_x / 8, (unsigned char)((previous_y + 16) / 8),
                          3, 1, ATTR_WHITE);
          putsprite(spr_or, previous_x, (unsigned char)(previous_y + 16),
                  crashed_lander_row);
          /* Restore any HUD/control text whose attributes were under the old
              sprite rectangle before printing the crash message. */
          DrawHudLabels();
          UpdateHud(angle);
    }
    has_previous_sprite = 0;
    /*clg();
    DrawStars();
    DrawTerrain();*/
    if (flight_state == 1) PrintCenteredAt(10, "SUCCESS");
    else if (flight_state == 2) PrintCenteredAt(10, "CRASH");
    else PrintCenteredAt(10, "LOST IN SPACE");
    PrintCenteredAt(12, "PRESS SPACE");
}

static void WaitForRestart(void)
{
    unsigned char flag_frame = 0;
    unsigned char flag_tick = 0;
    unsigned char flag_x = 0;
    unsigned char flag_y = 0;
    unsigned char *flag_sprite;

    if (flight_state == 1)
    {
        flag_x = (unsigned char)(previous_x + previous_width + 8);
        if (flag_x > 240) flag_x = 240;
        /* Align the flag with the solid legs, not the three flame rows. */
        flag_y = (unsigned char)(previous_y + previous_height - 16 - 24);
        if (flag_y > 176) flag_y = 176;
        SetAttributeBlock(flag_x / 8, flag_y / 8, 2, 2, ATTR_WHITE);
        putsprite(spr_or, flag_x, flag_y, success_flag_0);
    }

    in_WaitForNoKey();
    while (!in_KeyPressed(key_space))
    {
        asm("halt");
        if (flight_state == 1)
        {
            flag_tick++;
            if (flag_tick >= 8)
            {
                flag_tick = 0;
                flag_sprite = flag_frame ? success_flag_1 : success_flag_0;
                putsprite(spr_xor, flag_x, flag_y, flag_sprite);
                flag_frame = (unsigned char)!flag_frame;
                flag_sprite = flag_frame ? success_flag_1 : success_flag_0;
                putsprite(spr_or, flag_x, flag_y, flag_sprite);
            }
        }
    }
    in_WaitForNoKey();
}

static unsigned int StepAngleTowards(unsigned int angle,
                                     unsigned int target_angle)
{
    unsigned int clockwise_distance =
        (target_angle + 360 - angle) % 360;
    unsigned int counterclockwise_distance;

    if (clockwise_distance == 0) return angle;
    if (clockwise_distance <= 180)
    {
        if (clockwise_distance <= ROTATION_STEP) return target_angle;
        return (angle + ROTATION_STEP) % 360;
    }

    counterclockwise_distance = 360 - clockwise_distance;
    if (counterclockwise_distance <= ROTATION_STEP) return target_angle;
    return (angle + 360 - ROTATION_STEP) % 360;
}

static unsigned char PollFlightInput(unsigned int *target_angle,
                                     unsigned int *thrust_index)
{
    unsigned int digit_keys;
    unsigned char digit_thrust = 0;

    if (in_KeyPressed(key_q)) return 0;

    if (in_KeyPressed(key_p))
    {
        paused = (unsigned char)!paused;
        in_WaitForNoKey();
    }
    if (paused) return 1;

    digit_keys = ReadDigitKeys();
    if (digit_keys & 0x0001)
    {
        *target_angle = 315;
        *thrust_index = 896;
        digit_thrust = 1;
    }
    else if (digit_keys & 0x0002)
    {
        *target_angle = 330;
        *thrust_index = 939;
        digit_thrust = 1;
    }
    else if (digit_keys & 0x0004)
    {
        *target_angle = 345;
        *thrust_index = 981;
        digit_thrust = 1;
    }
    else if (digit_keys & 0x0008)
    {
        *target_angle = 355;
        *thrust_index = 1010;
        digit_thrust = 1;
    }
    else if (digit_keys & 0x0010)
    {
        *target_angle = 0;
        *thrust_index = 0;
        digit_thrust = 1;
    }
    else if (digit_keys & 0x1000)
    {
        *target_angle = 5;
        *thrust_index = 14;
        digit_thrust = 1;
    }
    else if (digit_keys & 0x0800)
    {
        *target_angle = 15;
        *thrust_index = 43;
        digit_thrust = 1;
    }
    else if (digit_keys & 0x0400)
    {
        *target_angle = 30;
        *thrust_index = 85;
        digit_thrust = 1;
    }
    else if (digit_keys & 0x0200)
    {
        *target_angle = 45;
        *thrust_index = 128;
        digit_thrust = 1;
    }
    else if (digit_keys & 0x0100)
    {
        *target_angle = 0;
    }

    thrust_active = (unsigned char)(digit_thrust ||
                                    in_KeyPressed(key_space));
    return 1;
}

static void WaitOneFrame(void)
{
    static unsigned char waitAccumulator;

    /* 300 HALT intervals over 360 degree steps gives approximately six
       seconds on a 50 Hz Spectrum frame clock. */
    waitAccumulator = (unsigned char)(waitAccumulator + 5);
    if (waitAccumulator >= 2)
    {
        waitAccumulator = (unsigned char)(waitAccumulator - 6);
        asm("halt");
    }
}

void main(void)
{
    unsigned int angle;
    unsigned int target_angle;
    unsigned int thrust_index;

#ifdef TestDrawLander
    clg();
    DrawStars();
    GenerateTerrain();
    DrawTerrain();
    DrawHudLabels();
    has_previous_sprite = 0;
    for (angle = 0; angle < 360; angle+=5)
    {
        asm("halt");
        DrawLander(angle, SCREEN_CENTER_X, SCREEN_CENTER_Y);
        //WaitOneFrame();
    }
#endif

    InitializeSineTable();
    InitialiseKeys();
    angle = 0;
    target_angle = 0;
    thrust_index = 0;
    StartFlight(angle);
    while (PollFlightInput(&target_angle, &thrust_index))
    {
        asm("halt");
        if (!paused)
        {
            angle = StepAngleTowards(angle, target_angle);
            if (UpdatePhysics(angle, thrust_index))
            {
                if ((elapsed_frames & 3) == 0 || flight_state != 0)
                    UpdateHud(angle);
                DrawLander(angle, lander_x_fp / FIXED_SCALE,
                           lander_y_fp / FIXED_SCALE);
                if (flight_state != 0)
                {
                    ShowFlightResult(angle);
                    WaitForRestart();
                    angle = 0;
                    target_angle = 0;
                    thrust_index = 0;
                    StartFlight(angle);
                }
            }
        }
    }

    while (1) { }
}

#asm
._ReadDigitKeys
    ld bc,0xf7fe
    in a,(c)
    cpl
    and 0x1f
    ld l,a
    ld bc,0xeffe
    in a,(c)
    cpl
    and 0x1f
    ld h,a
    ret
#endasm

#include "lander_frames.inc"
#include "lander_frames_no_flame.inc"

#asm
._moonlander_wordmark
 defb 80,8
 defb 99,60,121,141,128,99,27,227,247,192
 defb 119,126,253,205,128,243,155,243,247,224
 defb 127,102,205,205,128,179,155,51,6,96
 defb 107,102,205,237,129,155,219,19,199,224
 defb 99,102,205,189,129,251,123,19,199,192
 defb 99,102,205,157,129,251,59,51,6,192
 defb 99,126,253,157,249,155,59,243,246,96
 defb 99,60,121,141,249,155,27,227,246,96

._moon_art
 defb 24,24
 defb 0,0,0
 defb 0,0,0
 defb 0,188,0
 defb 3,239,0
 defb 5,87,192
 defb 26,253,96
 defb 20,87,224
 defb 59,189,216
 defb 42,187,120
 defb 86,213,168
 defb 85,42,188
 defb 46,213,108
 defb 68,50,212
 defb 86,141,108
 defb 7,98,220
 defb 40,25,40
 defb 36,165,152
 defb 2,21,112
 defb 20,138,176
 defb 8,5,224
 defb 2,131,128
 defb 0,62,0
 defb 0,0,0
 defb 0,0,0

._success_flag_0
 defb 16,16
 defb @00000000,@00000000
 defb @00000000,@11111110
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11111110
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000

._success_flag_1
 defb 16,16
 defb @00000000,@00000000
 defb @00000000,@11110000
 defb @00000000,@11111110
 defb @00000000,@11000000
 defb @00000000,@11111110
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
 defb @00000000,@11000000
#endasm

#asm
._crashed_lander_row
 defb 24,8
 defb 3,62,96
 defb 2,62,32
 defb 0,54,0
 defb 6,255,176
 defb 4,255,144
 defb 4,54,16
 defb 5,62,80
 defb 9,127,72
#endasm


