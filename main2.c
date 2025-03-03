#include <LPC21XX.H>
#include <string.h>
#include "uart1.h"
#include "Lcd.h"
#include "variable.h"

#define LED1_PIN (1 << 17) // Pin 17 for LED (Active Low)
#define LED2_PIN (1 << 18) // Pin 17 for LED (Active Low)
#define LED3_PIN (1 << 19)
#define BUZZ_PIN (1 << 21)
#define BUFFER_SIZE 200    // Buffer size for storing incoming data

unsigned char RX_Buffer[BUFFER_SIZE];
unsigned int RX_INDEX = 0; // Index to track the buffer

// Function to process received commands
void process_command(char *buffer) {
    // Look for specific HTTP GET commands in the received buffer
    if (strstr(buffer, "GET /lamp1on")) {
        IOCLR0 = LED1_PIN; // Turn LED ON (Active Low)
        LCD_command(0x01);
        LCD_str("LAMP 1 ON");
        uart1_tx_str("AT+CIPSEND=0,12\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("LED is ON\r\n");
    } else if (strstr(buffer, "GET /lamp1off")) {
        IOSET0 = LED1_PIN; // Turn LED OFF (Active Low)
        LCD_command(0x01);
        LCD_str("LAMP 1 OFF");
        uart1_tx_str("AT+CIPSEND=0,13\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("LED is OFF\r\n");
    }

	if (strstr(buffer, "GET /lamp2on")) {
        IOCLR0 = LED2_PIN; // Turn LED ON (Active Low)
        LCD_command(0x01);
        LCD_str("LAMP 2 ON");
        uart1_tx_str("AT+CIPSEND=0,12\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("LED is ON\r\n");
    } else if (strstr(buffer, "GET /lamp2off")) {
        IOSET0 = LED2_PIN; // Turn LED OFF (Active Low)
        LCD_command(0x01);
        LCD_str("LAMP 2 OFF");
        uart1_tx_str("AT+CIPSEND=0,13\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("LED is OFF\r\n");
    }

	if (strstr(buffer, "GET /lamp3on")) {
        IOCLR0 = LED3_PIN; // Turn LED ON (Active Low)
        LCD_command(0x01);
        LCD_str("LAMP 3 ON");
        uart1_tx_str("AT+CIPSEND=0,12\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("LED is ON\r\n");
    } else if (strstr(buffer, "GET /lamp3off")) {
        IOSET0 = LED3_PIN; // Turn LED OFF (Active Low)
        LCD_command(0x01);
        LCD_str("LAMP 3 OFF");
        uart1_tx_str("AT+CIPSEND=0,13\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("LED is OFF\r\n");
    }

	if (strstr(buffer, "GET /buzzon")) {
        IOSET0 = BUZZ_PIN; // Turn LED ON (Active Low)
        LCD_command(0x01);
        LCD_str("Calling bell ON");
        uart1_tx_str("AT+CIPSEND=0,12\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("LED is ON\r\n");
    } else if (strstr(buffer, "GET /buzzoff")) {
        IOCLR0 = BUZZ_PIN; // Turn LED OFF (Active Low)
        LCD_command(0x01);
        LCD_str("calling bell OFF");
        uart1_tx_str("AT+CIPSEND=0,13\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("LED is OFF\r\n");
    }
	if (strstr(buffer, "GET /allon")) {
        IOSET0 = BUZZ_PIN; // Turn LED ON (Active Low)
		IOCLR0=LED1_PIN|LED2_PIN|LED3_PIN;
        LCD_command(0x01);
        LCD_str("Calling bell ON");
        uart1_tx_str("LED is ON\r\n");
    } else if (strstr(buffer, "GET /alloff")) {
        IOCLR0 = BUZZ_PIN; // Turn LED ON (Active Low)
		IOSET0=LED1_PIN|LED2_PIN|LED3_PIN;
        LCD_command(0x01);
        LCD_str("calling bell OFF");
     
        uart1_tx_str("LED is OFF\r\n");
    }
}

// ESP8266 Initialization
void ESP_Initial(void) {
    uart1_tx_str("AT+CWQAP\r\n");       // Disconnect from Wi-Fi
    delay_ms(3000);
    uart1_tx_str("AT+RST\r\n");         // Reset ESP8266
    delay_ms(5000);
    uart1_tx_str("AT\r\n");             // Test communication
    delay_ms(3000);
	uart1_tx_str("OK\n");
    uart1_tx_str("AT+CWMODE=1\r\n");    // Set mode to station
    delay_ms(3000);
    uart1_tx_str("AT+CWJAP=\"Sriprasanna\",\"qwertyui\"\r\n"); // Connect to Wi-Fi
    delay_ms(9000); // Wait for connection
    uart1_tx_str("AT+CIPMUX=1\r\n");    // Enable multiple connections
    delay_ms(3000);
    uart1_tx_str("AT+CIPSERVER=1,80\r\n"); // Start server on port 80
    delay_ms(3000);
}

int main() {
    PINSEL0 |= 0; // Configure pins as GPIO
    IODIR0 |= LED1_PIN|LED2_PIN|LED3_PIN|BUZZ_PIN; // Set pin 17 as output for LED
    IOSET0 = LED1_PIN|LED2_PIN|LED3_PIN;
	IOCLR0=BUZZ_PIN; // Turn LED OFF (Active Low)
    delay_ms(1000);

    uart1_config(); // Initialize UART1
    LCD_init();     // Initialize LCD

    LCD_command(0x01);
    LCD_str("LED Control Panel");
    LCD_command(0xC0);
    LCD_str("Initializing...");
    delay_ms(2000);

    ESP_Initial(); // Initialize ESP8266

    LCD_command(0x01);
    LCD_str("Wi-Fi Ready");

    while (1) {
        if (U1LSR & 0x01) { // Check if data is available in the UART receive register
            char receivedChar = U1RBR; // Read the received character

            if (RX_INDEX < BUFFER_SIZE - 1) {
                RX_Buffer[RX_INDEX++] = receivedChar; // Store character in buffer
                RX_Buffer[RX_INDEX] = '\0'; // Null-terminate for safety
            }

            // Check if a complete command is received (ends with \n)
            if (receivedChar == '\n') {
                process_command((char *)RX_Buffer); // Process the command
                RX_INDEX = 0; // Reset buffer index
                memset(RX_Buffer, 0, BUFFER_SIZE); // Clear buffer
            }
        }
    }
}

/*
#include <LPC21XX.H>
#include <string.h>
#include <stdlib.h>
#include "uart0.h"
#include "Lcd.h"
#include "variable.h"

#define LED1_PIN (1 << 17)
#define LED2_PIN (1 << 18)
#define LED3_PIN (1 << 19)
#define BUZZ_PIN (1 << 21)
#define BUFFER_SIZE 200

unsigned char RX_Buffer[BUFFER_SIZE];
unsigned int RX_INDEX = 0;

void process_command(char *buffer);

void ESP_Initial(void) {
    uart0_tx_str("AT+CWQAP\r\n");
    delay_ms(3000);
    uart0_tx_str("AT+RST\r\n");
    delay_ms(5000);
    uart0_tx_str("AT\r\n");
    delay_ms(3000);
    uart0_tx_str("AT+CWMODE=1\r\n");
    delay_ms(3000);
	LCD_command(0x01);
	LCD_str("Connecting to");
	LCD_command(0xC0);
	LCD_str("a Network");
    uart0_tx_str("AT+CWJAP=\"Thanu\",\"12345678\"\r\n");
    delay_ms(9000);
    uart0_tx_str("AT+CIPMUX=1\r\n");
    delay_ms(3000);
    uart0_tx_str("AT+CIPSERVER=1,80\r\n");
    delay_ms(3000);
    uart0_tx_str("AT+CIFSR\r\n");
    delay_ms(3000);
}

int main() {
    PINSEL0 |= 0;
    IODIR0 |= LED1_PIN | LED2_PIN | LED3_PIN | BUZZ_PIN;
    IOSET0 = LED1_PIN | LED2_PIN | LED3_PIN;
    IOCLR0 = BUZZ_PIN;
    delay_ms(1000);

    uart0_config();
    LCD_init();

    LCD_command(0x01);
    LCD_str((unsigned char *)"Home Automation Panel");
    LCD_command(0xC0);
    LCD_str((unsigned char *)"Initializing...");
    delay_ms(2000);

    ESP_Initial();

    LCD_command(0x01);
    LCD_str((unsigned char *)"Wi-Fi Ready");

    while (1) {
        if (U0LSR & 0x01) {
            char receivedChar = U0RBR;

            if (RX_INDEX < BUFFER_SIZE - 1) {
                RX_Buffer[RX_INDEX++] = receivedChar;
                RX_Buffer[RX_INDEX] = '\0';
            }

            if (receivedChar == '\n') {
                process_command((char *)RX_Buffer);
                RX_INDEX = 0;
                memset(RX_Buffer, 0, BUFFER_SIZE);
            }
        }
    }
}



#include <LPC21XX.H>
#include <string.h>
#include <stdlib.h>
#include "uart0.h"
#include "Lcd.h"
#include "variable.h"

#define LED1_PIN (1 << 17)
#define LED2_PIN (1 << 18)
#define LED3_PIN (1 << 19)
#define BUZZ_PIN (1 << 21)
#define BUFFER_SIZE 200

unsigned char RX_Buffer[BUFFER_SIZE];
unsigned int RX_INDEX = 0;

void process_command(char *buffer);

void ESP_Initial(void) {
    uart0_tx_str("AT+CWQAP\r\n");
    delay_ms(3000);
    uart0_tx_str("AT+RST\r\n");
    delay_ms(5000);
    uart0_tx_str("AT\r\n");
    delay_ms(3000);
    uart0_tx_str("AT+CWMODE=1\r\n");
    delay_ms(3000);
	LCD_command(0x01);
	LCD_str("Connecting to");
	LCD_command(0xC0);
	LCD_str("a Network");
    uart0_tx_str("AT+CWJAP=\"Thanu\",\"12345678\"\r\n");
    delay_ms(9000);
    uart0_tx_str("AT+CIPMUX=1\r\n");
    delay_ms(3000);
    uart0_tx_str("AT+CIPSERVER=1,80\r\n");
    delay_ms(3000);
    uart0_tx_str("AT+CIFSR\r\n");
    delay_ms(3000);
}

int main() {
    PINSEL0 |= 0;
    IODIR0 |= LED1_PIN | LED2_PIN | LED3_PIN | BUZZ_PIN;
    IOSET0 = LED1_PIN | LED2_PIN | LED3_PIN;
    IOCLR0 = BUZZ_PIN;
    delay_ms(1000);

    uart0_config();
    LCD_init();

    LCD_command(0x01);
    LCD_str((unsigned char *)"Home Automation Panel");
    LCD_command(0xC0);
    LCD_str((unsigned char *)"Initializing...");
    delay_ms(2000);

    ESP_Initial();
        LCD_str((unsigned char *)"LAMP 1 ON");
    } else if (strstr(buffer, "GET /lamp1off")) {
        IOSET0 = LED1_PIN;
        LCD_command(0x01);
        LCD_str((unsigned char *)"LAMP 1 OFF");
    }

    if (strstr(buffer, "GET /lamp2on")) {
        IOCLR0 = LED2_PIN;
        LCD_command(0x01);
        LCD_str((unsigned char *)"LAMP 2 ON");
    } else if (strstr(buffer, "GET /lamp2off")) {
        IOSET0 = LED2_PIN;
        LCD_command(0x01);
        LCD_str((unsigned char *)"LAMP 2 OFF");
    }

    if (strstr(buffer, "GET /lamp3on")) {
        IOCLR0 = LED3_PIN;
        LCD_command(0x01);
        LCD_str((unsigned char *)"LAMP 3 ON");
    } else if (strstr(buffer, "GET /lamp3off")) {
        IOSET0 = LED3_PIN;
        LCD_command(0x01);
        LCD_str((unsigned char *)"LAMP 3 OFF");
    }

    if (strstr(buffer, "GET /buzzon")) {
        IOSET0 = BUZZ_PIN;
        LCD_command(0x01);
        LCD_str((unsigned char *)"Buzz ON");
    } else if (strstr(buffer, "GET /buzzoff")) {
        IOCLR0 = BUZZ_PIN;
        LCD_command(0x01);
        LCD_str((unsigned char *)"Buzz OFF");
    }
}
   */
