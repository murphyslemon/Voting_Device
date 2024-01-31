#include "display.h"

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

Epd epd;
unsigned char image[1024];
Paint paint(image, 0, 0);

void initDisplay(){
   //E-paper init
  epd.LDirInit();
  epd.Clear(); // clears whole screen to white
  paint.SetRotate(ROTATE_180); //rotate screen 
}

void paintDragon() {
  //wifilogo
    paint.SetWidth(35);
    paint.SetHeight(35);
    paint.Clear(COLORED); // paints the height and width with the given color
    drawImage(0, 0, 35, 35, wifilogo); // draw the image at (0, 0) coordinates
    epd.SetFrameMemory(paint.GetImage(), 40, (200-35), paint.GetWidth(), paint.GetHeight());
}

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

void displayWifi(){
  //wifilogo
    paint.SetWidth(35);
    paint.SetHeight(35);
    paint.Clear(UNCOLORED); // paints the height and width with the given color
    drawImage(0, 0, 35, 35, wifilogo); // draw the image at (0, 0) coordinates
    epd.SetFrameMemory(paint.GetImage(), 40, (200-35), paint.GetWidth(), paint.GetHeight());
}

