#include <LPC21XX.H>
#include <string.h>
#include "uart1.h"
#include "Lcd.h"
#include "variable.h"

#define LED1_ON '@'          // Command for LED ON
#define LED1_OFF '%'         // Command for LED OFF
#define LED2_ON '!'          // Command for LED ON
#define LED2_OFF '~'         // Command for LED OFF
#define LED3_ON '$'          // Command for LED ON
#define LED3_OFF '&'         // Command for LED OFF
#define LED1_PIN (1 << 17)  // Pin 17 for LED (Active Low)
#define LED2_PIN (1 << 18)  // Pin 18 for LED (Active Low)
#define LED3_PIN (1 << 19)  // Pin 19 for LED (Active Low)
#define BUFFER_SIZE 200

unsigned char RX_Buffer[BUFFER_SIZE];
unsigned int RX_INDEX = 0;  // Index to track the next position in the buffer
unsigned char COMMAND[1];
char ip_address[16];        // Buffer to hold the extracted IP address
volatile int reset_flag = 0; // Variable to handle reset flag


// Function to extract IP address from the RX buffer
void Get_IP_Address(char *src, char *dest) {
    char *start, *end;

    // Look for "+CIFSR:STAIP," tag
    start = strstr(src, "+CIFSR:STAIP,\"");
    if (start) {
        start += 14; // Skip past "+CIFSR:STAIP,\""
        end = strchr(start, '"');  // Find the closing quote
        if (end) {
            // Copy the IP address between the quotes
            while (start < end) {
                *dest++ = *start++;
            }
            *dest = '\0'; // Null-terminate the string
        } else {
            dest[0] = '\0'; // No end quote found, set dest to empty string
        }
    } else {
        dest[0] = '\0'; // "+CIFSR:STAIP," tag not found, set dest to empty string
    }
}

// Function to initialize the ESP8266 and display the IP address
void ESP_Initial(void) {
    uart1_tx_str("AT+CWQAP\r\n"); // Disconnect from WiFi
    delay_ms(3000);
    uart1_tx_str("AT+RST\r\n"); // Reset ESP8266
    delay_ms(5000);
    uart1_tx_str("AT\r\n"); // Test communication
    delay_ms(3000);
    uart1_tx_str("AT+CWMODE=1\r\n"); // Set mode
    delay_ms(3000);
    uart1_tx_str("AT+CWJAP=\"Sriprasanna\",\"qwertyui\"\r\n"); // Connect to WiFi
    delay_ms(9000);  // Wait for connection
    uart1_tx_str("AT+CIPMUX=1\r\n"); // Enable multiple connections
    delay_ms(3000);
    uart1_tx_str("AT+CIPSERVER=1,80\r\n"); // Start server
    delay_ms(3000);

    uart1_tx_str("AT+CIFSR\r\n"); // Get IP address
    delay_ms(8000); // Wait for response

    // Read the response from UART
    Get_IP_Address((char *)RX_Buffer, ip_address);

    // Debug: Display buffer content
    LCD_command(0x01);  // Clear LCD
    LCD_str("Buffer: ");
    LCD_command(0xC0);
    LCD_str((unsigned char *)RX_Buffer);  // Display the buffer contents
    delay_ms(5000);  // Wait to see the output

    if (strlen(ip_address) > 0) {
        // Display IP address on LCD
        LCD_command(0x01);
        LCD_str("IP Address: ");
        LCD_command(0xC0);
        LCD_str((unsigned char *)ip_address);
    } else {
        LCD_command(0x01);
        LCD_str("IP Retrieval Failed");
    }
}

int main() {
    PINSEL0 |= 0;
    IODIR0 |= LED1_PIN|LED2_PIN|LED3_PIN; // Configure pin 17 as output for LED
    IOSET0  = LED1_PIN|LED2_PIN|LED3_PIN; // Set pin 17 HIGH (LED OFF initially)
    delay_ms(1000);
    uart1_config();
    LCD_init();
    LCD_command(0x01);
    LCD_str("Home Automation");
    LCD_command(0xC0);
    LCD_str("   Using IOT");
    delay_ms(1000);
    LCD_command(0x01);
		LCD_command(0x80);
    LCD_str("Wi-Fi Connection");
    LCD_command(0xC0);
    LCD_str("Initializing....");
    ESP_Initial(); // Initialize ESP8266 and fetch IP address

    while (1) 
		{
        // Poll UART for received data
		if (U1LSR & 0x01) 
		{ // Check if data is available in the UART receive register
    char receivedChar = U1RBR; // Read the received character
    if (RX_INDEX < BUFFER_SIZE - 1)
		{
        RX_Buffer[RX_INDEX++] = receivedChar; // Store character in buffer
        RX_Buffer[RX_INDEX] = '\0'; // Null-terminate for safety
    }
		}

		if (RX_INDEX != 0) 
		{ // Process if there is data in the buffer
    COMMAND[0] = RX_Buffer[RX_INDEX - 1]; // Get the last received character

    // Handle LED ON or OFF based on the received command
    if (COMMAND[0] == LED1_ON)
		{
        IOCLR0 = LED1_PIN; // Turn LED ON (Active Low)
				LCD_command(0x01);
				LCD_str("LAMP 1 ON");
        //uart1_tx_str("AT+CIPSEND=0,27\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("HTTP/1.1 200 OK\r\n\r\nLED is ON\r\n");
    } 
		else if (COMMAND[0] == LED1_OFF) 
		{
        IOSET0 = LED1_PIN; // Turn LED OFF (Active Low)
				LCD_command(0x01);
				LCD_str("LAMP 1 OFF");
        uart1_tx_str("AT+CIPSEND=0,28\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("HTTP/1.1 200 OK\r\n\r\nLED is OFF\r\n");
    }
		else if (COMMAND[0] == LED2_ON)
		{
        IOCLR0 = LED2_PIN; // Turn LED ON (Active Low)
				LCD_command(0x01);
				LCD_str("LAMP 2 ON");
        uart1_tx_str("AT+CIPSEND=0,27\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("HTTP/1.1 200 OK\r\n\r\nLED is ON\r\n");
    } 
		else if (COMMAND[0] == LED2_OFF) 
		{
        IOSET0 = LED2_PIN; // Turn LED OFF (Active Low)
				LCD_command(0x01);
				LCD_str("LAMP 2 OFF");
        uart1_tx_str("AT+CIPSEND=0,28\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("HTTP/1.1 200 OK\r\n\r\nLED is OFF\r\n");
    }
		else if (COMMAND[0] == LED3_ON)
		{
        IOCLR0 = LED3_PIN; // Turn LED ON (Active Low)
				LCD_command(0x01);
				LCD_str("LAMP 3 ON");
        uart1_tx_str("AT+CIPSEND=0,27\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("HTTP/1.1 200 OK\r\n\r\nLED is ON\r\n");
    } 
		else if (COMMAND[0] == LED3_OFF) 
		{
        IOSET0 = LED3_PIN; // Turn LED OFF (Active Low)
				LCD_command(0x01);
				LCD_str("LAMP 3 OFF");
        uart1_tx_str("AT+CIPSEND=0,28\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("HTTP/1.1 200 OK\r\n\r\nLED is OFF\r\n");
    }  /*
		else if (COMMAND[0] == MOTOR_ON)
		{
        IOSET0 = M1;
				IOCLR0 = M2;			// Turn LED ON (Active Low)
				LCD_command(0x01);
				LCD_str("FAN ON");
        uart1_tx_str("AT+CIPSEND=0,27\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("HTTP/1.1 200 OK\r\n\r\nLED is ON\r\n");
    } 
		else if (COMMAND[0] == MOTOR_OFF) 
		{
        IOCLR0 = M1;
				IOCLR0 = M2;
				LCD_command(0x01);
				LCD_str("FAN OFF");
        uart1_tx_str("AT+CIPSEND=0,28\r\n"); // Prepare to send response
        delay_ms(100);
        uart1_tx_str("HTTP/1.1 200 OK\r\n\r\nLED is OFF\r\n");
    }  */

    // Reset RX_INDEX and buffer for the next command
    RX_INDEX = 0; // Reset RX index to start fresh for the next command	
	  }

	}
    
}
