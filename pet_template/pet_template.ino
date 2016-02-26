#include <EEPROM.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Wire.h>
#include <Adafruit_FT6206.h>

#define TFT_DC 9
#define TFT_CS 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_FT6206 ts = Adafruit_FT6206();

#define MAX_STRING 140
char stringBuffer[MAX_STRING];

const bool TESTING = false;

const char INTROSTRING[] PROGMEM = "HELLO FIXME!";
const char TITLESTRING[] PROGMEM = "SNO!";
const char AUTHORSTRING[] PROGMEM = "a short game by Canruo Ying";
const char LEVEL0TEXT1[] PROGMEM = "Winter is here. The city is covered in a shining white blanket.";
const char LEVEL0TEXT2[] PROGMEM = "You are walking alone on a field of snow.";
const char LEVEL0TEXT3[] PROGMEM = "\"Oh my, what is that?\"";
const char LEVEL0TEXT4[] PROGMEM = "You found a golden egg in the middle of a circle of snowballs...";
const char LEVEL1TEXT1[] PROGMEM = "The egg cracks open.";
const char LEVEL1TEXT2[] PROGMEM = "A tiny yellow being slowly crawled out.";
const char LEVEL1TEXT3[] PROGMEM = "\"How cute!\"";
const char LEVEL1TEXT4[] PROGMEM = "You decided to feed it with some milk...";
const char LEVEL2TEXT1[] PROGMEM = "\"What should I name this little guy?\"";
const char LEVEL2TEXT2[] PROGMEM = "\"How about 'Sno'... That's a good name.\"";
const char LEVEL2TEXT3[] PROGMEM = "You built a tiny shelter for Sno. It looks like Sno is hungry.";
const char LEVEL2TEXT4[] PROGMEM = "Well, you do have some food in your pocket...";
const char LEVEL3TEXT1[] PROGMEM = "\"Mommy!\"";
const char LEVEL3TEXT2[] PROGMEM = "\"Wow! You can talk!\"";
const char LEVEL3TEXT3[] PROGMEM = "Sno is growing bigger. You expanded the shelter.";
const char LEVEL3TEXT4[] PROGMEM = "\"I want to play with Mommy!\"";
const char LEVEL4TEXT1[] PROGMEM = "That was a fun day.";
const char LEVEL4TEXT2[] PROGMEM = "You have found a new friend.";
const char LEVEL4TEXT3[] PROGMEM = "Sno will depend on you for all her needs.";
const char LEVEL4TEXT4[] PROGMEM = "Please take good care of her.";
const char LEVEL5TEXT1[] PROGMEM = "Hey FIXME!";
const char LEVEL5TEXT2[] PROGMEM = "It's me, the person who made this for you. ^_^";
const char LEVEL5TEXT3[] PROGMEM = "I hope Sno has brought joy to you.";
const char LEVEL5TEXT4[] PROGMEM = "And I think you make a great mom.";
const char LEVEL6TEXT1[] PROGMEM = "Hey FIXME!";
const char LEVEL6TEXT2[] PROGMEM = "I hope everything is well with you.";
const char LEVEL6TEXT3[] PROGMEM = "Just wanna tell you,";
const char LEVEL6TEXT4[] PROGMEM = "I'm always here to help.";
const char LEVEL7TEXT1[] PROGMEM = "Wow, you made it! Congrats!";
const char LEVEL7TEXT2[] PROGMEM = "It's been quite a journey.";
const char LEVEL7TEXT3[] PROGMEM = "FIXME";
const char LEVEL7TEXT4[] PROGMEM = "FIXME";

const char FLAVORTEXT0[] PROGMEM = "Don't be too stressed!";
const char FLAVORTEXT1[] PROGMEM = "Let's take a break?";
const char FLAVORTEXT2[] PROGMEM = "I love you Mommy!";
const char FLAVORTEXT3[] PROGMEM = "Spicy strips are the best!";
const char FLAVORTEXT4A[] PROGMEM = "Can we do some exercise?";
const char FLAVORTEXT4B[] PROGMEM = "Time for some more exercise!";
const char FLAVORTEXT5A[] PROGMEM = "Can we go play?";
const char FLAVORTEXT5B[] PROGMEM = "Let's go play some more!";
const char FLAVORTEXTL4A[] PROGMEM = "I love spending time with you!";
const char FLAVORTEXTL4B[] PROGMEM = "I've grown so big!";
const char FLAVORTEXTL4C[] PROGMEM = "Life is so exciting!";
const char FLAVORTEXTL5A[] PROGMEM = "What a beautiful view!";
const char FLAVORTEXTL5B[] PROGMEM = "How is work?";
const char FLAVORTEXTL5C[] PROGMEM = "Let's be more healthy.";
const char FLAVORTEXTL6A[] PROGMEM = "I hope you have a great day!";
const char FLAVORTEXTL6B[] PROGMEM = "Can I help you with anything?";
const char FLAVORTEXTL6C[] PROGMEM = "Did you quit spicy strips?";
const char FLAVORTEXTL7A[] PROGMEM = "I'm so glad to know you.";
const char FLAVORTEXTL7B[] PROGMEM = "What's on your mind?";
const char FLAVORTEXTL7C[] PROGMEM = "What's the meaning of life?";
const char FLAVORTEXTDEBUG[] PROGMEM = "Tell my Daddy to debug this.";

const int UPDATERATE = 10; // higher is slower
const int BUTTONWIDTH = 80;
const int BUTTONHEIGHT = 40;
const uint16_t BUTTONCOLOR = 0x0fbc;
const uint16_t BUTTONBORDERCOLOR = ILI9341_WHITE;
const int PETHEIGHT = 130;

const int MAXLEVEL = 7;
const long LVLUPCOUNT[7] = {0, 0, 300, 86400, 172800, 604800, 2592000};

const long MAXHUNGER = 1000;
const long MAXENERGY = 1000;
const long MAXHAPPINESS = 1000;
const long MAXFITNESS = 1000;
const long MAXLOCATION = 170;
const long MINLOCATION = 110;
const int SPEED = 10;

long hunger;
long energy;
long happiness;
long fitness;
long state; // 0: neutral, 1: sleeping, 2: playing, 3: exercising, 4: eating
long level;
long updateCounter;
long petLocation;
long petDirection;
long fitnessUnlock;
long happinessUnlock;

bool hungerChange = true;
bool energyChange = true;
bool statusChange = true;
bool talkChange = true;
bool animationFlag = true;
bool touched = false;

int flavor = 0; // current flavor texts
int talk = 1; // 0: status specific, 1: state specific, 2: flavor

void setup() {

  if (TESTING) {
    for ( int i = 0 ; i < EEPROM.length() ; i++ )
      EEPROM.write(i, 0);
  }

  tft.begin();
  tft.setRotation(1);
  ts.begin();
  
  intro();
  loadData();

  tft.fillScreen(ILI9341_BLACK);
  drawButtons();
  drawBackground();
  randomSeed(updateCounter);
}

void loop() {
  updateCounter++;
  if (updateCounter % UPDATERATE == 0) {
    updateStatus();
  }
  saveData();
  updateGraphics();

  unsigned long t = millis();
  touched = false;
  while (abs(millis() - t) < 1000 && !touched) {
    handleTouch();
  }
}

void intro() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
  tft.setCursor(85, 100);
  tft.println(getString(INTROSTRING));
  delay(1000);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_YELLOW);  tft.setTextSize(3);
  tft.setCursor(140, 90);
  tft.println(getString(TITLESTRING));
  tft.setTextColor(ILI9341_RED);  tft.setTextSize(1);
  tft.setCursor(85, 150);
  tft.println(getString(AUTHORSTRING));
  delay(2000);
}

void drawButtons() {
  if (level >= 1) {
    drawMilkButton();
    if (level >= 2) {
      drawSpicystripButton();
      drawCarrotButton();
      drawRiceballButton();
      if (level >= 3) {
        drawRestButton();
        drawPlayButton();
        drawExerciseButton();
        drawTalkButton();
      }
    }
  }
}

void drawMilkButton() {
  tft.fillRect(0, 0, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  tft.fillRect(0, 0, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_CYAN);
  tft.drawRect(0, 0, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  drawMilk(40, 20);
}

void drawMilk(int x, int y) {
  tft.drawLine(x - 6, y - 15, x + 6, y - 15, ILI9341_WHITE);
  tft.drawLine(x - 7, y - 8, x - 5, y - 13, ILI9341_WHITE);
  tft.drawLine(x + 7, y - 8, x + 5, y - 13, ILI9341_WHITE);
  tft.drawRect(x - 5, y - 15, 11, 3, ILI9341_WHITE);
  tft.fillRoundRect(x - 10, y - 8, 21, 23, 4, ILI9341_WHITE);  
}

void drawSpicystripButton() {
  tft.fillRect(80, 0, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  tft.fillRect(80, 0, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_RED);
  tft.drawRect(80, 0, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  drawSpicystrip(120, 20);
}

void drawSpicystrip(int x, int y) {
  tft.fillTriangle(x + 20, y - 15, x + 30, y - 12, x - 10, y + 3, 0xE426);
  tft.fillTriangle(x - 30, y + 8, x - 24, y + 15, x - 6, y + 1, 0xE426);
  tft.fillCircle(x + 14, y - 10, 1, ILI9341_RED);
  tft.fillCircle(x + 9, y - 6, 1, ILI9341_RED);
  tft.fillCircle(x - 23, y + 10, 1, ILI9341_RED);
}

void drawCarrotButton() {
  tft.fillRect(160, 0, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  tft.fillRect(160, 0, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_GREEN);
  tft.drawRect(160, 0, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  drawCarrot(200, 20);
}

void drawCarrot(int x, int y) {
  tft.fillRoundRect(x - 5, y - 14, 12, 7, 2, 0x258A);
  tft.fillRoundRect(x - 11, y - 11, 26, 10, 3, 0xFBE8);
  tft.fillRoundRect(x - 9, y - 4, 21, 8, 3, 0xFBE8);
  tft.drawLine(x - 8, y - 3, x + 6, y - 3, 0xB240);
  tft.fillRoundRect(x - 7, y + 1, 16, 8, 3, 0xFBE8);
  tft.drawLine(x - 6, y + 2, x + 3, y + 2, 0xB240);
  tft.fillRoundRect(x - 5, y + 5, 11, 10, 3, 0xFBE8);
}

void drawRiceballButton() {
  tft.fillRect(240, 0, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  tft.fillRect(240, 0, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_BLUE);
  tft.drawRect(240, 0, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  drawRiceball(280, 20);
}

void drawRiceball(int x, int y) {
  tft.fillCircle(x, y - 2, 11, ILI9341_WHITE);
  tft.fillRoundRect(x - 13, y - 6, 27, 18, 4, ILI9341_WHITE);
  tft.fillRect(x - 7, y - 3, 15, 15, ILI9341_BLACK);
}

void drawRestButton() {
  tft.fillRect(0, 200, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  tft.fillRect(0, 200, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_BLUE);
  tft.drawRect(0, 200, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  tft.setCursor(20, 210);
  tft.println("Z");
  tft.setTextSize(2);
  tft.setCursor(40, 210);
  tft.println("Z");
  tft.setTextSize(1);
  tft.setCursor(56, 210);
  tft.println("Z");
}

void drawPlayButton() {
  tft.fillRect(80, 200, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  tft.fillRect(80, 200, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_GREEN);
  tft.drawRect(80, 200, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  tft.fillCircle(120, 220, 13, ILI9341_RED);
  tft.fillCircle(115, 216, 6, 0xFC10);
}

void drawExerciseButton() {
  tft.fillRect(160, 200, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  tft.fillRect(160, 200, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_RED);
  tft.drawRect(160, 200, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  tft.fillRoundRect(175, 215, 50, 10, 4, ILI9341_BLACK);
  tft.fillRoundRect(175, 210, 10, 20, 4, ILI9341_BLACK);
  tft.fillRoundRect(215, 210, 10, 20, 4, ILI9341_BLACK);
  tft.drawLine(177, 213, 177, 220, ILI9341_WHITE);
  tft.drawLine(217, 213, 217, 220, ILI9341_WHITE);
}

void drawTalkButton() {
  tft.fillRect(240, 200, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  tft.fillRect(240, 200, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_CYAN);
  tft.drawRect(240, 200, BUTTONWIDTH, BUTTONHEIGHT, ILI9341_WHITE);
  tft.fillRoundRect(260, 207, 45, 18, 4, ILI9341_WHITE);
  tft.fillTriangle(262, 225, 273, 225, 255, 232, ILI9341_WHITE);
}

void loadData() {
  if (EEPROMReadlong(1020) == 0) {
    hunger = 0;
    energy = 0;
    happiness = 500;
    fitness = 500;
    level = 0;
    state = 0;
    petLocation = 160;
    petDirection = -1;
    fitnessUnlock = 0;
    happinessUnlock = 0;
    updateCounter = 0;
    lvlupGraphics();
    EEPROMWritelong(1020, 1);
    saveData();
  } else {
    hunger = EEPROMReadlong(0);
    energy = EEPROMReadlong(4);
    happiness = EEPROMReadlong(8);
    fitness = EEPROMReadlong(12);
    level = EEPROMReadlong(16);
    state = EEPROMReadlong(20);
    petLocation = EEPROMReadlong(24);
    petDirection = EEPROMReadlong(28);
    fitnessUnlock = EEPROMReadlong(32);
    happinessUnlock = EEPROMReadlong(36);
    updateCounter = EEPROMReadlong(200);
  }
}

void updateGraphics() {
  if (energyChange == true && level >= 1) {
    energyChange = false;
    drawEnergybar();
  }
  if (hungerChange == true && level >= 2) {
    hungerChange = false;
    drawhungerbar();
  }
  if (statusChange == true) {
    statusChange = false;
    drawStatusbar();
  }
  if (talkChange == true && level >= 3) {
    talkChange = false;
    drawTalk();
  }
  drawPet();
}

void drawPet() {
  if (state == 0 || state == 2) {
    tft.fillRect(60, 90, 150, 70, ILI9341_BLACK);
  } else {
    tft.fillRect(130, 90, 80, 70, ILI9341_BLACK);
  }
  if (state != 0 && level >= 3) {
    petLocation = 160;
    petDirection = -1;
    drawStateAccessories();
  }
  if (level == 0) {
    int y = (animationFlag) ? 0 : 3;
    tft.fillCircle(petLocation, PETHEIGHT + 3 + y, 11, ILI9341_YELLOW);
    tft.fillCircle(petLocation, PETHEIGHT - 3 + y, 11, ILI9341_YELLOW);
    tft.drawLine(petLocation - 11, PETHEIGHT + 3 + y, petLocation - 6, PETHEIGHT - 3 + y, ILI9341_BLACK);
    tft.drawLine(petLocation - 6, PETHEIGHT - 3 + y, petLocation, PETHEIGHT + 3 + y, ILI9341_BLACK);
    tft.drawLine(petLocation, PETHEIGHT + 3 + y, petLocation + 6, PETHEIGHT - 3 + y, ILI9341_BLACK);
  } else if (level >= 1) {
    int y = 0;
    if (level == 1) {
      y = (animationFlag) ? 10 : 0;
    } else if (level == 2) {
      y = (animationFlag) ? 10 : -5;
    } else if (level >= 3) {
      y = (animationFlag) ? -5 : -20;
    }
    int x = (animationFlag) ? 0 : 5;
    int h = (animationFlag) ? 0 : 6;
    int w = (animationFlag) ? 6 : 0;
    int r = (animationFlag) ? 0 : 3;
    tft.fillRoundRect(petLocation - 10 + x, PETHEIGHT - 10 + y, 25 + w, 20 + h, 4 + r, ILI9341_YELLOW);
    if (level >= 3) {
      if (happinessUnlock == 1 && state == 2) {
        tft.fillRoundRect(petLocation - 12 + x, PETHEIGHT - 10 + y, 29 + w, 6, 2, ILI9341_BLUE);
        tft.drawLine(petLocation - 11 + x + petDirection * 5, PETHEIGHT - 5 + y, petLocation + 15 + x + w + petDirection * 5, PETHEIGHT - 5 + y, ILI9341_BLUE);
      }
      y = (animationFlag) ? 3 : -5;
      x = (animationFlag) ? 0 : 5;
      h = (animationFlag) ? 4 : 10;
      w = (animationFlag) ? 6 : 0;
      r = (animationFlag) ? 0 : 3;
      if (fitness >= 700) {
        x += 5;
        w -= 10;
      } else if (fitness < 200) {
        x -= 7;
        w += 14;
        r += 2;
      }
      tft.fillRoundRect(petLocation - 10 + x, PETHEIGHT - 2 + y, 25 + w, 15 + h, 5 + r, ILI9341_YELLOW);
      if (fitnessUnlock == 1 && state == 3) {
        tft.fillRect(petLocation - 10 + x, PETHEIGHT + 10 + y, 25 + w, 5, ILI9341_RED);
      }
    }
    drawFace();
  }
  if (animationFlag == true) {
    animationFlag = false;
  } else {
    animationFlag = true;
  }
  movePet();
}

void drawStateAccessories() {
  if (state == 1) {
    tft.fillCircle(petLocation - 64, PETHEIGHT - 22, 11, ILI9341_YELLOW);
    tft.fillCircle(petLocation - 70, PETHEIGHT - 25, 8, ILI9341_BLACK);
  } else if (state == 2) {
    if (animationFlag) {
      tft.fillCircle(petLocation - 60, PETHEIGHT - 21, 10, ILI9341_RED);
      tft.fillCircle(petLocation - 60, PETHEIGHT - 27, 10, ILI9341_RED);
    } else {
      tft.fillCircle(petLocation - 57, PETHEIGHT + 15, 10, ILI9341_RED);
      tft.fillCircle(petLocation - 63, PETHEIGHT + 15, 10, ILI9341_RED);        
    }
  } else if (state == 3) {
    if (animationFlag) {
      tft.drawRoundRect(petLocation - 25, PETHEIGHT - 30, 60, 35, 13, ILI9341_RED);
      tft.fillRect(petLocation - 20, PETHEIGHT, 50, 10, ILI9341_BLACK);
      tft.fillCircle(petLocation - 20, PETHEIGHT + 2, 5, ILI9341_YELLOW);
      tft.fillCircle(petLocation + 25, PETHEIGHT + 2, 5, ILI9341_YELLOW);
    } else {
      tft.drawRoundRect(petLocation - 15, PETHEIGHT, 50, 20, 10, ILI9341_RED);
      tft.fillRect(petLocation - 10, PETHEIGHT - 5, 40, 10, ILI9341_BLACK);
      tft.fillCircle(petLocation - 10, PETHEIGHT + 2, 5, ILI9341_YELLOW);
      tft.fillCircle(petLocation + 25, PETHEIGHT + 2, 5, ILI9341_YELLOW);        
    }
  }
}

void movePet() {
  if (state == 0 && level >= 2 && fitness > 100) {
    petLocation = petLocation + petDirection * SPEED;
    if (petLocation > MAXLOCATION) {
      petLocation = MAXLOCATION;
      petDirection = -1;
    } else if (petLocation < MINLOCATION) {
      petLocation = MINLOCATION;
      petDirection = 1;
    }
  }
}

void drawFace() {
  int x = 0;
  int y = 0;
  String face = "";
  if (level == 1) {
    if (animationFlag) {
      x = -6;
      y = 9;
    } else {
      x = -4;
      y = 2;
    }    
    face = "-_-";
  } else if (level == 2) {
    if (animationFlag) {
      x = petDirection * 3 - 3;
      y = 9;
    } else {
      x = petDirection * 3 - 1;
      y = -3;
    }
    face = "o_o";
  } else if (level >= 3) {
    if (animationFlag) {
      x = petDirection * 3 - 3;
      y = -6;
    } else {
      x = petDirection * 3 - 1;
      y = -18;
    }
    if (happiness >= 500) {
      face = "^w^";
    } else if (happiness >= 200) {
      face = "o_o";
    } else {
      face = "Q_Q";
    }
    if (hunger < 200) {
      face = "x_x";
    } else if (energy < 200) {
      face = "=_=";
    }
  }
  if (state == 1) {
    face = "-_-";
  } else if (state == 2) {
    face = "^o^";
  } else if (state == 3) {
    face = ">w<";
  } else if (state == 4) {
    face = (animationFlag) ? "^_^)" : "^O^";
  }
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(1);
  tft.setCursor(petLocation + x, PETHEIGHT + y);
  tft.println(face);
}

void drawEnergybar() {
  tft.fillRect(20, 50, 10, 100, ILI9341_BLUE);
  tft.fillRect(20, 50, 10, (MAXENERGY - energy) * 100 / MAXENERGY, ILI9341_BLACK);
  tft.drawRect(20, 50, 10, 100, ILI9341_WHITE);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.setCursor(8, 152);
  tft.println("ENERGY");
}

void drawhungerbar() {
  tft.fillRect(290, 50, 10, 100, ILI9341_RED);
  tft.fillRect(290, 50, 10, (MAXHUNGER - hunger) * 100 / MAXHUNGER, ILI9341_BLACK);
  tft.drawRect(290, 50, 10, 100, ILI9341_WHITE);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.setCursor(278, 152);
  tft.println("HUNGER");
}

void drawStatusbar() {
  tft.fillRect(0, 180, 320, 20, ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.setCursor(5, 180);
  if (TESTING) {
    tft.print("TEST ");    
  } else {
    tft.print("LVL ");
  }
  tft.println(level);
  tft.setCursor(5, 190);
  tft.println(updateCounter);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.setCursor(255, 180);
  tft.println("STATE");
  tft.setCursor(255, 190);
  switch (state) {
    case 0:
      tft.println("none");
      break;
    case 1:
      tft.println("sleeping");
      break;
    case 2:
      tft.println("playing");
      break;
    case 3:
      tft.println("exercising");
      break;
    case 4:
      tft.println("eating");
      break;
    default:
      tft.println("error");
      break;
  }
  if (level >= 3) {
    tft.setCursor(95, 180);
    tft.println("HAPPINESS");
    tft.setCursor(95, 190);
    tft.print(happiness);
    tft.println("/1000");
    tft.setCursor(175, 180);
    tft.println("FITNESS");
    tft.setCursor(175, 190);
    tft.print(fitness);
    tft.println("/1000");    
  }
}

void drawTalk() {
  String text = "";
  if (talk == 0) { // status specific
    if (hunger == 0) {
      text = "I'm starving... Food please..."; 
    } else if (energy == 0) {
      text = "Tired... Need rest...";
    } else if (happiness == 0) {
      text = "Where's mommy?";
    } else if (fitness == 0) {
      text = "I need to exercise...";
    } else if (hunger < 200) {
      text = "When's dinner mommy?";
    } else if (energy < 200) {
      text = "I'm a little sleepy...";
    } else if (happiness < 200) {
      text = "I wanna play...";
    } else if (fitness < 200) {
      text = "Do I look fat?";
    } else if (hunger >= 900) {
      text = "I'm so full...";
    } else if (energy >= 900) {
      text = "I'm so refreshed!";
    } else if (happiness >= 900) {
      text = "I'm having so much fun!";
    } else if (fitness >= 900) {
      text = "Do I look sexy?";
    } else if (happiness >= 500) {
      text = "What a great day!";
    } else if (fitness >= 500) {
      text = "Do I look good?";
    } else {
      text = "What do we do next?";
    }
  } else if (talk == 1) { // state specific
    if (state == 0) {
      text = "Let's do something together!";
    } else if (state == 1) {
      text = "zzz... zzz...";
    } else if (state == 2) {
      text = "Yay! I love playing!";
    } else if (state == 3) {
      text = "1... 2... 3... 4...";
    } else if (state == 4) {
      text = "Nom nom nom...";
    }
  } else if (talk == 2) { // flavor
    switch (flavor) {
      case 0:
        text = getString(FLAVORTEXT0);
        break;
      case 1:
        text = getString(FLAVORTEXT1);
        break;
      case 2:
        text = getString(FLAVORTEXT2);
        break;
      case 3:
        text = getString(FLAVORTEXT3);
        break;
      case 4: // unlocked with fitness achievement
        if (fitnessUnlock == 0) {
          text = getString(FLAVORTEXT4A);
        } else {
          text = getString(FLAVORTEXT4B);
        }
        break;
      case 5: // unlocked with happiness achievement
        if (happinessUnlock == 0) {
          text = getString(FLAVORTEXT5A);
        } else {
          text = getString(FLAVORTEXT5B);
        }
        break;
      case 6: // unlocked with level 4
        text = getString(FLAVORTEXTL4A);
        break;
      case 7: // unlocked with level 4
        text = getString(FLAVORTEXTL4B);
        break;
      case 8: // unlocked with level 4
        text = getString(FLAVORTEXTL4C);
        break;
      case 9: // unlocked with level 5
        text = getString(FLAVORTEXTL5A);
        break;
      case 10: // unlocked with level 5
        text = getString(FLAVORTEXTL5B);
        break;
      case 11: // unlocked with level 5
        text = getString(FLAVORTEXTL5C);
        break;
      case 12: // unlocked with level 6
        text = getString(FLAVORTEXTL6A);
        break;
      case 13: // unlocked with level 6
        text = getString(FLAVORTEXTL6B);
        break;
      case 14: // unlocked with level 6
        text = getString(FLAVORTEXTL6C);
        break;
      case 15: // unlocked with level 7
        text = getString(FLAVORTEXTL7A);
        break;
      case 16: // unlocked with level 7
        text = getString(FLAVORTEXTL7B);
        break;
      case 17: // unlocked with level 7
        text = getString(FLAVORTEXTL7C);
        break;
      default:
        text = getString(FLAVORTEXTDEBUG);
        break;
    }
  }
  tft.fillRoundRect(60, 50, 200, 30, 4, ILI9341_WHITE);
  tft.fillTriangle(170, 80, 185, 80, 165, 88, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(1);
  tft.setCursor(62, 60);
  tft.println(text);
}

void drawBackground() {
  for (int i = 0; i <= 32; i++) {
    tft.fillCircle(i * 10, 170, 5, ILI9341_WHITE);
  }
  if (level >= 3) {
    tft.fillCircle(240, 120, 25, ILI9341_WHITE);
    tft.fillRect(215, 120, 51, 26, ILI9341_BLACK);
  }
  if (level >= 2) {
    tft.fillRoundRect(213, 119, 55, 20, 4, ILI9341_WHITE);
    tft.fillRect(225, 122, 10, 16, ILI9341_BLACK);
    tft.drawLine(250, 123, 250, 143, ILI9341_RED);
    tft.fillTriangle(250, 123, 250, 131, 257, 127, ILI9341_RED);
  }
}

void updateStatus() {
  autoTransition();
  switch (state) {
    case 0: // neutral
      happiness--;
      energy--;
      fitness--;
      hunger--;
      break;
    case 1: // sleeping
      energy++;
      if (hunger >= 500) {
        hunger--;
      }
      break;
    case 2: // playing
      happiness += 30;
      hunger -= 20;
      energy -= 100;
      break;
    case 3: // exercising
      fitness += 30;
      hunger -= 20;
      energy -= 100;
      break;
    default:
      break;
  }
  if (level < MAXLEVEL) {
    if (updateCounter >= LVLUPCOUNT[level]) {
      if (level == 1 && energy > 900) {
        level++;
        updateCounter = 0;
        lvlupGraphics();
        drawBackground();
        drawButtons();
      } else if (level > 1 && hunger > 900) {
        level++;
        updateCounter = 0;
        lvlupGraphics();
        drawBackground();
        drawButtons();
      } else if (level == 0) {
        level++;
        updateCounter = 0;
        lvlupGraphics();
        drawBackground();
        drawButtons();
      }
    }
  } else {
    if (updateCounter >= LVLUPCOUNT[MAXLEVEL - 1]) {
      if (hunger > 900) {
        level++;
        updateCounter = 0;      
        lvlupGraphics();
        drawButtons();
        drawBackground();
      }
    }
  }  
  cleanupStatus();
}

void lvlupGraphics() {
  String text1, text2, text3, text4;
  uint16_t text1Color, text2Color, text3Color, text4Color;
  if (level == 0) {
    text1 = getString(LEVEL0TEXT1);
    text2 = getString(LEVEL0TEXT2);
    text3 = getString(LEVEL0TEXT3);
    text4 = getString(LEVEL0TEXT4);
    text1Color = ILI9341_WHITE;
    text2Color = ILI9341_WHITE;
    text3Color = ILI9341_WHITE;
    text4Color = ILI9341_YELLOW;
  } else if (level == 1) {
    text1 = getString(LEVEL1TEXT1);
    text2 = getString(LEVEL1TEXT2);
    text3 = getString(LEVEL1TEXT3);
    text4 = getString(LEVEL1TEXT4);
    text1Color = ILI9341_YELLOW;
    text2Color = ILI9341_YELLOW;
    text3Color = ILI9341_WHITE;
    text4Color = ILI9341_WHITE;
  } else if (level == 2) {
    text1 = getString(LEVEL2TEXT1);
    text2 = getString(LEVEL2TEXT2);
    text3 = getString(LEVEL2TEXT3);
    text4 = getString(LEVEL2TEXT4);
    text1Color = ILI9341_WHITE;
    text2Color = ILI9341_WHITE;
    text3Color = ILI9341_WHITE;
    text4Color = ILI9341_WHITE;
  } else if (level == 3) {
    text1 = getString(LEVEL3TEXT1);
    text2 = getString(LEVEL3TEXT2);
    text3 = getString(LEVEL3TEXT3);
    text4 = getString(LEVEL3TEXT4);
    text1Color = ILI9341_YELLOW;
    text2Color = ILI9341_WHITE;
    text3Color = ILI9341_WHITE;
    text4Color = ILI9341_YELLOW;
  } else if (level == 4) {
    text1 = getString(LEVEL4TEXT1);
    text2 = getString(LEVEL4TEXT2);
    text3 = getString(LEVEL4TEXT3);
    text4 = getString(LEVEL4TEXT4);
    text1Color = ILI9341_GREEN;
    text2Color = ILI9341_GREEN;
    text3Color = ILI9341_GREEN;
    text4Color = ILI9341_GREEN;
  } else if (level == 5) {
    text1 = getString(LEVEL5TEXT1);
    text2 = getString(LEVEL5TEXT2);
    text3 = getString(LEVEL5TEXT3);
    text4 = getString(LEVEL5TEXT4);
    text1Color = ILI9341_GREEN;
    text2Color = ILI9341_GREEN;
    text3Color = ILI9341_GREEN;
    text4Color = ILI9341_GREEN;
  } else if (level == 6) {
    text1 = getString(LEVEL6TEXT1);
    text2 = getString(LEVEL6TEXT2);
    text3 = getString(LEVEL6TEXT3);
    text4 = getString(LEVEL6TEXT4);
    text1Color = ILI9341_GREEN;
    text2Color = ILI9341_GREEN;
    text3Color = ILI9341_GREEN;
    text4Color = ILI9341_GREEN;
  } else if (level == 7) {
    text1 = getString(LEVEL7TEXT1);
    text2 = getString(LEVEL7TEXT2);
    text3 = getString(LEVEL7TEXT3);
    text4 = getString(LEVEL7TEXT4);
    text1Color = ILI9341_GREEN;
    text2Color = ILI9341_GREEN;
    text3Color = ILI9341_GREEN;
    text4Color = ILI9341_GREEN;
  } else {
    return;
  }
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(text1Color);  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.println(text1);
  tft.println();
  delay(1000);
  while (!ts.touched());
  tft.setTextColor(text2Color);
  tft.println(text2);
  tft.println();
  delay(1000);
  while (!ts.touched());
  tft.setTextColor(text3Color);
  tft.println(text3);
  tft.println();
  delay(1000);
  while (!ts.touched());
  tft.setTextColor(text4Color);
  tft.println(text4);
  tft.println();
  delay(1000);
  while (!ts.touched());
  tft.fillScreen(ILI9341_BLACK);
}

void cleanupStatus() {
  energy = (energy > MAXENERGY) ? MAXENERGY : energy;
  energy = (energy < 0) ? 0 : energy;
  hunger = (hunger > MAXHUNGER) ? MAXHUNGER : hunger;
  hunger = (hunger < 0) ? 0 : hunger;
  fitness = (fitness > MAXFITNESS) ? MAXFITNESS : fitness;
  fitness = (fitness < 0) ? 0 : fitness;
  happiness = (happiness > MAXHAPPINESS) ? MAXHAPPINESS : happiness;
  happiness = (happiness < 0) ? 0 : happiness;
  hungerChange = true;
  energyChange = true;
  statusChange = true;
  talkChange = true;
  if (fitness >= MAXFITNESS) {
    fitnessUnlock = 1;
  }
  if (happiness >= MAXHAPPINESS) {
    happinessUnlock = 1;
  }
}

void autoTransition() {
  if ((energy == 0 || hunger == 0) && state != 1) {
    state = 0;
    talk = 0;
  }
}

void saveData() {
    EEPROMWritelong(0, hunger);
    EEPROMWritelong(4, energy);
    EEPROMWritelong(8, happiness);
    EEPROMWritelong(12, fitness);
    EEPROMWritelong(16, level);
    EEPROMWritelong(20, state);
    EEPROMWritelong(24, petLocation);
    EEPROMWritelong(28, petDirection);
    EEPROMWritelong(32, fitnessUnlock);
    EEPROMWritelong(36, happinessUnlock);
    EEPROMWritelong(200, updateCounter);
}

void handleTouch() {
  TS_Point p;
  if (ts.touched()) {
    touched = true;
    p = ts.getPoint();
    int y = map(p.x, 0, 240, 0, 240);
    int x = map(p.y, 0, 320, 320, 0);
    if (y < 40) {
      if (x < 80 && level >= 1) {
        drinkMilk();
      } else if (x < 160 && level >= 2) {
        eatSpicystrip();
      } else if (x < 240 && level >= 2) {
        eatCarrot();
      } else if (level >= 2){
        eatRiceball();
      }
    } else if (y >= 200 && level >= 3) {
      if (x < 80) {
        rest();
      } else if (x < 160 && level >= 3) {
        play();
      } else if (x < 240 && level >= 3) {
        exercise();
      } else if (level >= 3) {
        handleTalk();
      }
    } else {
      handlePoke();
    }
  }
}

void drinkMilk() {
  if (energy <= 990) {
    drawMilkButton();
    eatingAnimation(1);
    energy += 200;
    state = 0;
    talk = 0;
    cleanupStatus();
  } else {
    touched = false;
  }
}

void eatSpicystrip() {
  if (hunger <= 990) {
    drawSpicystripButton();
    eatingAnimation(2);
    hunger += 200;
    happiness += 50;
    fitness -= 100;
    state = 0;
    talk = 0;
    cleanupStatus();
  } else {
    touched = false;
  }
}

void eatCarrot() {
  if (hunger <= 990) {
    drawCarrotButton();
    eatingAnimation(3);
    hunger += 200;
    happiness -= 50;
    fitness += 50;
    state = 0;
    talk = 0;
    cleanupStatus();
  } else {
    touched = false;
  }
}

void eatRiceball() {
  if (hunger <= 990) {
    drawRiceballButton();
    eatingAnimation(4);
    hunger += 200;
    fitness -= 25;
    state = 0;
    talk = 0;
    cleanupStatus();
  } else {
    touched = false;
  }
}

void eatingAnimation(int item) {
  tft.fillRect(60, 90, 150, 70, ILI9341_BLACK);
  state = 4;
  talk = 1;
  petLocation = 160;
  petDirection = -1;
  drawStatusbar();
  if (level >= 3) {
    drawTalk();
  }
  animationFlag = 0;
  switch (item) {
    case 1:
      drawMilk(petLocation - 60, PETHEIGHT);
      break;
    case 2:
      drawSpicystrip(petLocation - 60, PETHEIGHT);
      break;
    case 3:
      drawCarrot(petLocation - 60, PETHEIGHT);
      break;
    case 4:
      drawRiceball(petLocation - 60, PETHEIGHT);
      break;
    default:
      break;
  }
  for (int i = 0; i < 8; i++) {
    drawPet();
    if (i >= 1) {
      tft.fillCircle(petLocation - 50, PETHEIGHT - 10, 15, ILI9341_BLACK);
    }
    if (i >= 3) {
      tft.fillCircle(petLocation - 70, PETHEIGHT - 5, 15, ILI9341_BLACK);
    }
    if (i >= 5) {
      tft.fillCircle(petLocation - 80, PETHEIGHT + 5, 15, ILI9341_BLACK);
    }
    if (i >= 7) {
      tft.fillCircle(petLocation - 60, PETHEIGHT, 15, ILI9341_BLACK);
    }
    delay(500);
  }
  drawPet();
  state = 0;
}

void rest() {
  if (state != 1) {
    drawRestButton();
    tft.fillRect(60, 90, 150, 70, ILI9341_BLACK);
    state = 1;
    talk = 1;
    cleanupStatus();
  } else {
    touched = false;
  }
}

void play() {
  if (state != 2) {
    drawPlayButton();
    tft.fillRect(60, 90, 150, 70, ILI9341_BLACK);
    state = 2;
    talk = 1;
    cleanupStatus();
  } else {
    touched = false;
  }
}

void exercise() {
  if (state != 3) {
    drawExerciseButton();
    tft.fillRect(60, 90, 150, 70, ILI9341_BLACK);
    state = 3;
    talk = 1;
    cleanupStatus();
  } else {
    touched = false;
  }
}

void handleTalk() {
  drawTalkButton();
  int oldFlavor = flavor;
  state = 0;
  talk = 2;
  while (oldFlavor == flavor) {
    flavor = random(min((level - 3) * 3 + 6, (MAXLEVEL - 3) * 3 + 6));
  }
  cleanupStatus();
}

void handlePoke() {
  if (state != 0 || talk != 0) {
    state = 0;
    talk = 0;
    cleanupStatus();    
  } else {
    touched = false;
  }
}

// Kevin Elsenberger
void EEPROMWritelong(int address, long value) {
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
  
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

// Kevin Elsenberger
long EEPROMReadlong(long address) {
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

// Scott Penrose
char* getString(const char* str) {
  strcpy_P(stringBuffer, (char*)str);
  return stringBuffer;
}
