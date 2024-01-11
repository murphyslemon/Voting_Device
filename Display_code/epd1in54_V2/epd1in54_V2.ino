#include <SPI.h>
#include "epd1in54_V2.h"
#include "imagedata.h"
#include "epdpaint.h"
#include <stdio.h>

Epd epd;
unsigned char image[1024];
Paint paint(image, 0, 0);

unsigned long time_start_ms;
unsigned long time_now_s;
#define COLORED     0
#define UNCOLORED   1

const unsigned char wifilogo[] PROGMEM = {
// 'imresizer-1702234655501', 35x35px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xc0, 0x00, 0x01, 0xff, 0xff, 
0xf0, 0x00, 0x07, 0xf0, 0x01, 0xfc, 0x00, 0x0f, 0x80, 0x00, 0x3e, 0x00, 0x1f, 0x00, 0x00, 0x1f, 
0x00, 0x1c, 0x0f, 0xfe, 0x07, 0x00, 0x08, 0x3f, 0xff, 0x82, 0x00, 0x00, 0x7f, 0x9f, 0xc0, 0x00, 
0x01, 0xf8, 0x01, 0xf0, 0x00, 0x01, 0xe0, 0x00, 0xf0, 0x00, 0x01, 0x80, 0x00, 0x30, 0x00, 0x00, 
0x07, 0xfc, 0x00, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x1c, 
0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 
0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char batterylogo[] PROGMEM = {
// 'imresizer-1702234758351', 35x35px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xe0, 0x00, 0x1f, 0xff, 0xff, 0xf8, 0x00, 0x3f, 0xff, 0xff, 
0xfc, 0x00, 0x38, 0x00, 0x00, 0x1e, 0x00, 0x78, 0x00, 0x00, 0x0e, 0x00, 0x70, 0x00, 0x00, 0x0e, 
0x00, 0x70, 0x00, 0x00, 0x0f, 0x80, 0x70, 0x00, 0x00, 0x0f, 0x80, 0x70, 0x00, 0x00, 0x0f, 0xc0, 
0x70, 0x00, 0x00, 0x0f, 0xc0, 0x70, 0x00, 0x00, 0x0f, 0xc0, 0x70, 0x00, 0x00, 0x0f, 0xc0, 0x70, 
0x00, 0x00, 0x0f, 0xc0, 0x70, 0x00, 0x00, 0x0f, 0x80, 0x70, 0x00, 0x00, 0x0f, 0x80, 0x70, 0x00, 
0x00, 0x0e, 0x00, 0x70, 0x00, 0x00, 0x0e, 0x00, 0x38, 0x00, 0x00, 0x1e, 0x00, 0x3f, 0xff, 0xff, 
0xfc, 0x00, 0x1f, 0xff, 0xff, 0xf8, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void drawImage(int x, int y, int width, int height, const unsigned char *image) {
    int byteWidth = (width + 7) / 8; // Number of bytes in a row
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (pgm_read_byte(image + j * byteWidth + i / 8) & (128 >> (i % 8))) {
                paint.DrawPixel(x + i, y + j, COLORED);
            } else {
                paint.DrawPixel(x + i, y + j, UNCOLORED);
            }
        }
    }
}

bool question_mark(const char *question) {
    int question_len = strlen(question);
    if (question_len > 0 && question[question_len - 1] == '?') {
        return true;
    }
    return false;
}

bool check_question(const char *question, char *line, int *position) {
    if (!question_mark(question)) {
        printf("ERROR: No question mark!");
        return false;
    }
    int count = 0;
    do {
        strncpy(line, question + *position, 15);
        line[15] = '\0';

        int pos = 15;
        int extra = 0;
        while (pos >= 0 && line[pos] != ' ') {
            pos--;
            extra++;
        }
        line[pos] = '\0';

        if (extra == 16) {
            line[14] = '-';
            *position -= 1;  // Update position to after the '-'
        } else {
            *position -= extra;  // Update position to the last space
            *position += 1;  // Update position to after the space
        }
        printf("%s\n", line);
        *position += 15;
        count++;
    } while (!question_mark(line));

    if (count > 6) {
        printf("ERROR: Question too long!");
        return false;
    }
    return true;
}

void display_question(const char *question, char *line, int *position) {
    paint.SetWidth(200);
    paint.SetHeight(16);
    int count = 0;
    do {
        strncpy(line, question + *position, 15);
        line[15] = '\0';

        int pos = 15;
        int extra = 0;
        while (pos >= 0 && line[pos] != ' ') {
            pos--;
            extra++;
        }
        line[pos] = '\0';

        if (extra == 16) {
            line[14] = '-';
            *position -= 1;  // Update position to after the '-'
        } else {
            *position -= extra;  // Update position to the last space
            *position += 1;  // Update position to after the space
        }
        paint.Clear(UNCOLORED); //paints the height and width the given colour
        paint.DrawStringAt(15, 2, line, &Font16, COLORED); //moves text to co-ordinates with-in the set height and width
        epd.SetFrameMemory(paint.GetImage(), 0, (120-(count*16)), paint.GetWidth(), paint.GetHeight()); //moves page to co-ordinate
        *position += 15;
        count++;
    } while (!question_mark(line));
}

void setup() {
    Serial.begin(115200);

    epd.LDirInit();
    epd.Clear(); // clears whole screen to white
    paint.SetRotate(ROTATE_180); //rotate screen 

    //wifilogo
    paint.SetWidth(35);
    paint.SetHeight(35);
    paint.Clear(UNCOLORED); // paints the height and width with the given color
    drawImage(0, 0, 35, 35, wifilogo); // draw the image at (0, 0) coordinates
    epd.SetFrameMemory(paint.GetImage(), 40, (200-35), paint.GetWidth(), paint.GetHeight());

    //battery logo
    paint.SetWidth(35);
    paint.SetHeight(35);
    paint.Clear(UNCOLORED); // paints the height and width with the given color
    drawImage(0, 0, 35, 35, batterylogo); // draw the image at (0, 0) coordinates
    epd.SetFrameMemory(paint.GetImage(), 0, (200-35), paint.GetWidth(), paint.GetHeight());

    //need to finish battery status bar position
    paint.SetWidth(20);
    paint.SetHeight(13);
    //paint.Clear(COLORED); // paints the height and width with the given color
    //paint.DrawFilledRectangle(0, 0, 20, 14, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 16, (200-23), paint.GetWidth(), paint.GetHeight());

    //Bottom bar
    paint.SetWidth(200);
    paint.SetHeight(22);
    paint.Clear(UNCOLORED); //paints the height and width the given colour
    paint.DrawStringAt(2, 7, "YES ABSTAIN NO", &Font20, COLORED); //moves text to co-ordinates with-in the set height and width
    //paint.DrawStringAt(0, 2, "Yes   Abstain   No", &Font16, COLORED);
    paint.DrawFilledRectangle(0, 0, 200, 3, COLORED);
    paint.DrawFilledRectangle(48, 0, 50, 25, COLORED);
    paint.DrawFilledRectangle(160, 0, 162, 25, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight()); //moves page to co-ordinates

    //name
    paint.Clear(UNCOLORED); //paints the height and width the given colour
    paint.DrawStringAt(40, 2, "NAME", &Font20, COLORED); //moves text to co-ordinates with-in the set height and width
    paint.DrawStringAt(41, 2, "NAME", &Font20, COLORED); //moves text to co-ordinates with-in the set height and width
    epd.SetFrameMemory(paint.GetImage(), 0, 140, paint.GetWidth(), paint.GetHeight()); //moves page to co-ordinates

    //vote question
    char question[100] = "How many characters can E-paper fit across?";

    char question2[100] = "Pneumonoultramicroscopicsilicovolcanoconiosis is a long question another another?";
    int position = 0;
    char line[16];

    bool all_good = check_question(question2, line, &position);
    if (all_good) {
        position = 0;
        display_question(question2, line, &position);
    }
    epd.DisplayFrame();
}


void loop()
{

}