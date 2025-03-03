void uart1_config(void)
{
    PINSEL0 |= 0x00050000; // Configure pins for UART1 (TXD1 and RXD1)
    U1LCR = 0x83;          // Enable DLAB and set 8N1 format (8-bit data, no parity, 1 stop bit)
    U1DLL = 97;            // Set baud rate (assuming PCLK provides the required frequency)
    U1DLM = 0;
    U1LCR = 0x03;          // Disable DLAB
}

void uart1_tx(unsigned char ch)
{
    U1THR = ch;
    while ((U1LSR >> 5 & 1) == 0); // Wait for THR to be empty
}

unsigned char uart1_rx(void)
{
    while ((U1LSR & 1) == 0); // Wait for data to be received
    return U1RBR;
}

void uart1_tx_str(unsigned char *s)
{
    while (*s)
        uart1_tx(*s++);
}

int uart1_rx_ready()
{
    return (U1LSR & 0x01); // Checks if the Receive Data Ready (RDR) bit is set
}

// Function to clear any residual data in UART1 buffer
void uart1_clear_buffer()
{
    while (uart1_rx_ready())
        uart1_rx();
}

// Function to receive a string from UART1
void uart1_rx_str(unsigned char *buffer, int length)
{
    int i = 0;
    while (i < length - 1) {
        char c = uart1_rx(); // Receive a single character
        if (c == '\r' || c == '\n') { // Handle end of line
            break;
        }
        buffer[i++] = c;
    }
    buffer[i] = '\0'; // Null-terminate the string
}
