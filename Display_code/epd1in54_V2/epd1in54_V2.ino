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

void process_string(const char *question, char *string, int *position) {
  strncpy(string, question + *position, 15);
  string[15] = '\0';
  int string_len = 15;
  while (string[string_len] != ' ') {
    string_len--;
  }
  int remainder = *position - string_len - 1;
  *position -= remainder;
  if (string_len > 0) {
    string[string_len] = '\0';  // Null-terminate at the space
  }
}

void display_question(const char *question) {
  char string[16];
  if (!question_mark(question)) {
    printf("ERROR: no question mark!");
    return;
  }

  paint.SetWidth(200);
  paint.SetHeight(16);

  int position = 0;
  int count = 0;
  char buffer[20];
  process_string(question, string, &position);
  sprintf(buffer, "%d", position);
  paint.Clear(UNCOLORED); //paints the height and width the given colour
  paint.DrawStringAt(15, 2, string, &Font16, COLORED); //moves text to co-ordinates with-in the set height and width
  epd.SetFrameMemory(paint.GetImage(), 0, (100-(count*16)), paint.GetWidth(), paint.GetHeight()); //moves page to co-ordinate
  
  count = 1;
  process_string(question, string, &position);
  sprintf(buffer, "%d", position);
  paint.Clear(UNCOLORED); //paints the height and width the given colour
  paint.DrawStringAt(15, 2, string, &Font16, COLORED); //moves text to co-ordinates with-in the set height and width
  epd.SetFrameMemory(paint.GetImage(), 0, (100-(count*16)), paint.GetWidth(), paint.GetHeight()); //moves page to co-ordinate
   
  count = 2;
  process_string(question, string, &position);
  paint.Clear(UNCOLORED); //paints the height and width the given colour
  sprintf(buffer, "%d", position);
  paint.DrawStringAt(15, 2, string, &Font16, COLORED); //moves text to co-ordinates with-in the set height and width
  epd.SetFrameMemory(paint.GetImage(), 0, (100-(count*16)), paint.GetWidth(), paint.GetHeight()); //moves page to co-ordinate

  count = 3;
  process_string(question, string, &position);
  paint.Clear(UNCOLORED); //paints the height and width the given colour
  sprintf(buffer, "%d", position);
  paint.DrawStringAt(15, 2, string, &Font16, COLORED); //moves text to co-ordinates with-in the set height and width
  epd.SetFrameMemory(paint.GetImage(), 0, (100-(count*16)), paint.GetWidth(), paint.GetHeight()); //moves page to co-ordinate

  /*
  do {
    process_string(question, string, &position);
    paint.Clear(UNCOLORED); //paints the height and width the given colour
    paint.DrawStringAt(15, 2, string, &Font16, COLORED); //moves text to co-ordinates with-in the set height and width
    epd.SetFrameMemory(paint.GetImage(), 0, (100-(count*16)), paint.GetWidth(), paint.GetHeight()); //moves page to co-ordinate
    count++;
  } while (strchr(string, '?') == NULL);
  */
  
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

    display_question(question);

    //paint.SetWidth(200);
    //paint.SetHeight(16);
    //paint.Clear(UNCOLORED); //paints the height and width the given colour
    //paint.DrawStringAt(15, 2, "How many", &Font16, COLORED); //moves text to co-ordinates with-in the set height and width
    //epd.SetFrameMemory(paint.GetImage(), 0, 100, paint.GetWidth(), paint.GetHeight()); //moves page to co-ordinates
    //paint.Clear(UNCOLORED);
    //paint.DrawStringAt(15, 2, "characters can", &Font16, COLORED);
    //epd.SetFrameMemory(paint.GetImage(), 0, (100-16), paint.GetWidth(), paint.GetHeight());
    //paint.Clear(UNCOLORED);
    //paint.DrawStringAt(15, 2, "Epaper fit", &Font16, COLORED);
    //epd.SetFrameMemory(paint.GetImage(), 0, (100-16-16), paint.GetWidth(), paint.GetHeight());
    //paint.Clear(UNCOLORED);
    //paint.DrawStringAt(15, 2, "across?", &Font16, COLORED);
    //epd.SetFrameMemory(paint.GetImage(), 0, (100-16-16-16), paint.GetWidth(), paint.GetHeight());

    epd.DisplayFrame();
}


void loop()
{

}