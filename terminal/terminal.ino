#define MAX7219_CLK  10 
#define MAX7219_CS   9
#define MAX7219_DIN  8

bool ledDisplayPattern[8][8] = {false};
bool ledDisplayPattern2[8][8] = {
    {1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
};

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
            if (matrix[col][7 - row]) { 
                rowData |= (1 << (row));
            }
        }
        writeLEDMatrix(row + 1, rowData);
    }
}

void setup() {
    pinMode(MAX7219_CLK, OUTPUT);
    pinMode(MAX7219_CS, OUTPUT);
    pinMode(MAX7219_DIN, OUTPUT);
    initLEDMatrix();
    displayMatrix(ledDisplayPattern2);
}

void loop() {
    delay(200);
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            ledDisplayPattern[i][j] = true;
        }
    }

    displayMatrix(ledDisplayPattern);

}
