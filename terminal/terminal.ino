#include <IRremote.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>

#define MAX7219_CLK  10 
#define MAX7219_CS   9
#define MAX7219_DIN  8
#define IR_RECV_PIN  11
#define RH_TRANSMITTER_PIN  12

#define UP     24
#define RIGHT  90
#define LEFT   8
#define DOWN   82

#define DESPOSIT   7
#define CHANGEMODE 21
#define WITHDRAW   9

#define INPUTSPEED 150
#define ANIMATIONSPEED 100
#define ERRORTIME 1000

RH_ASK driver(2000);

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool inventory[8][8] = {false};
bool selection[8][8] = {false};
int selectX = 0, selectY = 0, lastX = 0, lastY = 0;
bool mode = 0; // False for inventory mode, true for selection mode

// Sends byte for row data
void writeLEDMatrixByte(byte data) {
    for (byte i = 0; i < 8; i++) {
        digitalWrite(MAX7219_CLK, LOW);
        digitalWrite(MAX7219_DIN, (data & 0x80) ? HIGH : LOW);
        data <<= 1;
        digitalWrite(MAX7219_CLK, HIGH);
    }
}

// Writes byte to address
void writeLEDMatrix(byte address, byte data) {
    digitalWrite(MAX7219_CS, LOW);
    writeLEDMatrixByte(address);
    writeLEDMatrixByte(data);
    digitalWrite(MAX7219_CS, HIGH);
}

void initLEDMatrix() {
    writeLEDMatrix(0x09, 0x00);  // No decoding mode
    writeLEDMatrix(0x0A, 0x03);  // Set brightness (0x00 to 0x0F)
    writeLEDMatrix(0x0B, 0x07);  // Scan limit: all 8 rows
    writeLEDMatrix(0x0C, 0x01);  // Normal operation mode
    writeLEDMatrix(0x0F, 0x00);  // Disable test mode
}

// Function to display an 8x8 boolean matrix
void displayMatrix(bool matrix[8][8]) {
    for (byte row = 0; row < 8; row++) {
        byte rowData = 0;
        for (byte col = 0; col < 8; col++) {
            if (matrix[col][row]) { 
                rowData |= (1 << (col));
            }
        }
        writeLEDMatrix(row + 1, rowData);
    }
}

void printToScreen(char * line1, char * line2){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print(line2);
}

void errorAnimation(){
    //illegal messaging shape
    bool animation[8][8] = {
        {1, 0, 0, 0, 0, 0, 0, 1},
        {0, 1, 0, 0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0, 1, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0, 0, 1, 0},
        {1, 0, 0, 0, 0, 0, 0, 1}
    };
    displayMatrix(animation);
    delay(ERRORTIME);
    displayMatrix(selection);
}

void loadingAnimation() {
    // Arrow shape
    bool animation[8][8] = {
        {0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 1, 1, 1, 1, 1, 1, 0 },
        {0, 0, 1, 1, 1, 1, 0, 0 },
        {0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 0, 0, 0, 0, 0, 0, 0 }
    };

    bool frame[8][8] = {false};

    for (int offset = -8; offset <= 8; offset++) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                frame[i][j] = false;
            }
        }
        for (int i = 0; i < 8; i++) {
            int srcRow = i - offset;
            if (srcRow >= 0 && srcRow < 8) {
                for (int j = 0; j < 8; j++) {
                    frame[i][j] = animation[srcRow][j];
                }
            }
        }
        displayMatrix(frame);
        delay(ANIMATIONSPEED);
    }
}

void loadingAnimation2() {
    bool animation[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0 },
        {0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 0, 1, 1, 1, 1, 0, 0 },
        {0, 1, 1, 1, 1, 1, 1, 0 },
        {0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 0, 0, 1, 1, 0, 0, 0 }
    };

    bool frame[8][8] = {false};

    for (int offset = -8; offset <= 8; offset++) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                frame[i][j] = false;
            }
        }

        for (int i = 0; i < 8; i++) {
            int srcRow = i + offset;
            if (srcRow >= 0 && srcRow < 8) {
                for (int j = 0; j < 8; j++) {
                    frame[i][j] = animation[srcRow][j];
                }
            }
        }

        displayMatrix(frame);
        delay(ANIMATIONSPEED);
    }
}

void sendCommand(bool isDeposit){
    char msg[4];
    if(isDeposit){
        sprintf(msg, "d%d%d", selectX, selectY);
    } else {
        sprintf(msg, "w%d%d", selectX, selectY);
    }

    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
}

void deposit(){
    if (inventory[selectY][selectX]){
        printToScreen("Slot Full!", "");
        errorAnimation();
        printToScreen("Selecting Item", "Slot...");
    } else {
        inventory[selectY][selectX] = true;
        sendCommand(true);
        printToScreen("Depositing", "Item...");
        loadingAnimation();
        mode = false;
        displayInventory();
        printToScreen("Displaying", "Inventory");
        selectX = 0;
        selectY = 0;
    }
}

void withdraw(){
    if (!inventory[selectY][selectX]){
        printToScreen("Slot Empty!", "");
        errorAnimation();
        printToScreen("Selecting Item", "Slot...");
    } else {
        inventory[selectY][selectX] = false;
        sendCommand(false);
        printToScreen("Withdrawing", "Item...");
        loadingAnimation2();
        mode = false;
        displayInventory();
        printToScreen("Displaying", "Inventory");
        selectX = 0;
        selectY = 0;
    }
}



void displayInventory(){
    displayMatrix(inventory);
}

void displaySelection(unsigned long keycode){
    if (keycode == UP){
        if (selectY == 0)
            return;
        selectY = selectY - 1;
    } else if (keycode == RIGHT) {
        if (selectX == 7)
            return;
        selectX = selectX + 1;
    } else if (keycode == LEFT) {
        if (selectX == 0)
            return;
        selectX = selectX - 1;
    } else if (keycode == DOWN) {
        if (selectY == 7)
            return;
        selectY = selectY + 1;
    } else if (keycode == DESPOSIT) {
        deposit();
        return;
    } else if (keycode == WITHDRAW) {
        withdraw();
        return;
    }

    selection[lastY][lastX] = false;
    selection[selectY][selectX] = true;
    lastX = selectX;
    lastY = selectY;
    displayMatrix(selection);
}

void update(unsigned long keycode){
    if (keycode == CHANGEMODE) {
        mode = !mode;
    }

    if (!mode) { // Inventory Mode
        displayInventory();
    } else { // Selection Mode
        displaySelection(keycode);
    }

    if (!mode && keycode == CHANGEMODE){
        printToScreen("Displaying", "Inventory");
    }
    if (mode && keycode == CHANGEMODE){
        printToScreen("Selecting Item", "Slot...");
    }
}

void setup() {
    pinMode(MAX7219_CLK, OUTPUT);
    pinMode(MAX7219_CS, OUTPUT);
    pinMode(MAX7219_DIN, OUTPUT);

    initLEDMatrix();
    displayMatrix(inventory);

    Serial.begin(115200);
    IrReceiver.begin(IR_RECV_PIN, ENABLE_LED_FEEDBACK);
    driver.init();
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Inventory Ready");
    lcd.setCursor(0,1);
    lcd.print("Awaiting Input..");
}

void loop()
{
    unsigned long keycode = 1000;
    if (IrReceiver.decode()) {
        keycode = IrReceiver.decodedIRData.command;
        update(keycode);
        delay(INPUTSPEED);
        IrReceiver.resume();
    }
}
