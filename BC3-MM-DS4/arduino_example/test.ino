#include <SoftwareSerial.h>
#include <Arduino.h>

SoftwareSerial dongle(D5, D4); // RX, TX

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    dongle.begin(38400); // custom HC05DS4's baud is 38400
}

char buf[79];
int bytes = 0;

// 9 bytes
typedef struct {
    uint8_t L_X;
	uint8_t L_Y;
	uint8_t R_X;
	uint8_t R_Y;
    // DPAD			0b1111
	// SQUARE_PAD	0b10000
	// X_PAD		0b100000
	// O_PAD		0b1000000
	// TRIANGLE_PAD	0b10000000
	uint8_t PAD;
	// L1		0b0001
	// R1		0b0010
	// L2		0b0100
	// R2		0b1000
	// SHARE	0b10000
	// OPTION	0b100000
	// L3		0b1000000
	// R3		0b10000000
	uint8_t BTN;
    uint8_t TOUCHPS;
	uint8_t L2Analogy;
	uint8_t R2Analogy;
} report0x01;

// 77 bytes
typedef struct {
    uint8_t unknow;
    uint8_t ReportID;
    uint8_t L_X;
	uint8_t L_Y;
	uint8_t R_X;
	uint8_t R_Y;
    // DPAD			0b1111
	// SQUARE_PAD	0b10000
	// X_PAD		0b100000
	// O_PAD		0b1000000
	// TRIANGLE_PAD	0b10000000
	uint8_t PAD;
	// L1		0b0001
	// R1		0b0010
	// L2		0b0100
	// R2		0b1000
	// SHARE	0b10000
	// OPTION	0b100000
	// L3		0b1000000
	// R3		0b10000000
	uint8_t BTN;
    uint8_t TOUCHPS;
	uint8_t L2Analogy;
	uint8_t R2Analogy;
    byte Timestamp[2];
	uint8_t Battery;
	uint16_t AngularVelocity[3];
	uint16_t Acceleration[3];
	byte unknow99[51];
} report0x11;

report0x01 *r01;
report0x11 *r11;

void DS4Task( SoftwareSerial dongle ) {
  if (dongle.available()) {
        bytes = dongle.readBytes(buf,2);
        if (bytes!=2 && buf[0] != 0xa1 ) {
            Serial.printf("BUG1, %d!\n", bytes);
            return;
        }
        switch (buf[1]) {
            case 0x01:
                bytes = dongle.readBytes(buf,9);
                if (bytes!=9 ) {
                    Serial.printf("BUG 0x01 %d!\n", bytes);
                    return;
                }
                r01 = (report0x01*)buf;
                Serial.printf("DS4 says: L(%d,%d) R(%d,%d)\n", r01->L_X, r01->L_Y, r01->R_X, r01->R_Y);
                break;
            case 0x11:
                bytes = dongle.readBytes(buf,77);
                if (bytes!=77 ) {
                    Serial.printf("BUG 0x11 %d!\n", bytes);
                    return;
                }
                r11 = (report0x11*)buf;
                Serial.printf("DS4 says: L(%d,%d) R(%d,%d)\n", r11->L_X, r11->L_Y, r11->R_X, r11->R_Y);
                break;
            default:
            Serial.printf("Unknow report type! %d\n", buf[1]);
        }
    }
}

void loop()
{
    if (dongle.available()) {
        bytes = dongle.readBytes(buf,2);
        if (bytes!=2 && buf[0] != 0xa1 ) { // filter it if it's we not handle packet
            Serial.printf("BUG1, %d!\n", bytes);
            return;
        }
        switch (buf[1]) {
            case 0x01:
                bytes = dongle.readBytes(buf,9);
                if (bytes!=9 ) {
                    Serial.printf("BUG 0x01 %d!\n", bytes);
                    return;
                }
                r01 = (report0x01*)buf;
                Serial.printf("DS4 says: L(%d,%d) R(%d,%d)", r01->L_X, r01->L_Y, r01->R_X, r01->R_Y);
                if (r01->PAD&0b10000   )
                    Serial.printf("□ ");
                if (r01->PAD&0b100000  )
                    Serial.printf("X ");
                if (r01->PAD&0b1000000 )
                    Serial.printf("O ");
                if (r01->PAD&0b10000000)
                    Serial.printf("△ ");

                // Other DS4 code define please visit https://github.com/darkautism/gds4/blob/master/DS4.go
                Serial.printf("\n");
                break;
            case 0x11:
                bytes = dongle.readBytes(buf,77);
                if (bytes!=77 ) {
                    Serial.printf("BUG 0x11 %d!\n", bytes);
                    return;
                }
                r11 = (report0x11*)buf;
                Serial.printf("DS4 says: L(%d,%d) R(%d,%d) ", r11->L_X, r11->L_Y, r11->R_X, r11->R_Y);
                if (r11->PAD&0b10000   )
                    Serial.printf("□ ");
                if (r11->PAD&0b100000  )
                    Serial.printf("X ");
                if (r11->PAD&0b1000000 )
                    Serial.printf("O ");
                if (r11->PAD&0b10000000)
                    Serial.printf("△ ");

                // Other DS4 code define please visit https://github.com/darkautism/gds4/blob/master/DS4.go
                Serial.printf("\n");
                break;
            default:
            Serial.printf("Unknow report type! %d\n", buf[1]);
        }
    }
}
