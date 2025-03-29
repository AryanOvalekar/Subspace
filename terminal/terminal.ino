#include <IRremote.h>
#include <RH_ASK.h>
#include <SPI.h>

#define MAX7219_CLK  10 
#define MAX7219_CS   9
#define MAX7219_DIN  8
#define IR_RECV_PIN  11

#define UP     24
#define RIGHT  90
#define LEFT   8
#define DOWN   82

#define DESPOSIT   7
#define CHANGEMODE 21
#define WITHDRAW   9

#define INPUTSPEED 150
#define ANIMATIONSPEED 50

RH_ASK driver;

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

void loadingAnimation(){
    bool animation[8][8] = {false};
    displayMatrix(animation);
    bool animation[8][8] = {
        {0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 1, 1, 1, 1, 1, 1, 0 },
        {0, 0, 1, 1, 1, 1, 0, 0 },
        {0, 0, 0, 1, 1, 0, 0, 0 },
        {0, 0, 0, 0, 0, 0, 0, 0 }
    }

    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            animation[i][j] = true;
            displayMatrix(animation);
            delay(ANIMATIONSPEED);
        }
    }
}

void deposit(){
    if (inventory[selectY][selectX]){
        //PRINT ERROR MESSAGE: SLOT FULL
        return;
    }
    inventory[selectY][selectX] = true;
    loadingAnimation();
    mode = false;
    displayInventory();
    selectX = 0;
    selectY = 0;
}

void withdraw(){
    if (!inventory[selectY][selectX]){
        //PRINT ERROR MESSAGE: SLOT EMPTY
        return;
    }
    inventory[selectY][selectX] = false;
    loadingAnimation();
    mode = false;
    displayInventory();
    selectX = 0;
    selectY = 0;
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
}

void setup() {
    pinMode(MAX7219_CLK, OUTPUT);
    pinMode(MAX7219_CS, OUTPUT);
    pinMode(MAX7219_DIN, OUTPUT);

    initLEDMatrix();
    displayMatrix(inventory);

    Serial.begin(115200);
    IrReceiver.begin(IR_RECV_PIN, ENABLE_LED_FEEDBACK);
    driver.init()
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

    const char *msg = "Hello World!";
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    delay(1000);
}
