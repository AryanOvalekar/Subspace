#include <IRremote.h>

#define MAX7219_CLK  10 
#define MAX7219_CS   9
#define MAX7219_DIN  8
#define IR_RECV_PIN  11

bool inventory[8][8] = {false};
bool selection[8][8] = {false};
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

void displayInventory(){
    displayMatrix(inventory);
}

void displaySelection(unsigned long keycode){

    if (keycode == "18"){
        Serial.println("UP");
    } else if (keycode == "5A") {
        Serial.println("RIGHT");
    } else if (keycode == "8") {
        Serial.println("LEFT");
    } else if (keycode == "52") {
        Serial.println("DOWN");
    }
}

void update(unsigned long keycode){
    //ADD MODE TOGGLE

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
}

void loop()
{
    unsigned long keycode = 1000;
    if (IrReceiver.decode()) {
        keycode = IrReceiver.decodedIRData.command;
        Serial.println(keycode, HEX);
        update(keycode);
        delay(150);
        IrReceiver.resume();
    }
}
