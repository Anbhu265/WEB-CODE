void uart0_config(void)
{
    PINSEL0 |= 0x00000005; // Configure pins for UART0 (TXD0 and RXD0)
    U0LCR = 0x83;          // Enable DLAB and set 8N1 format (8-bit data, no parity, 1 stop bit)
    U0DLL = 97;            // Set baud rate (assuming PCLK provides the required frequency)
    U0DLM = 0;
    U0LCR = 0x03;          // Disable DLAB
}

void uart0_tx(unsigned char ch)
{
    U0THR = ch;
    while ((U0LSR >> 5 & 1) == 0); // Wait for THR to be empty
}

unsigned char uart0_rx(void)
{
    while ((U0LSR & 1) == 0); // Wait for data to be received
    return U0RBR;
}

void uart0_tx_str(unsigned char *s)
{
    while (*s)
        uart0_tx(*s++);
}

int uart0_rx_ready()
{
    return (U0LSR & 0x01); // Checks if the Receive Data Ready (RDR) bit is set
}

// Function to clear any residual data in UART0 buffer
void uart0_clear_buffer()
{
    while (uart0_rx_ready())
        uart0_rx();
}

// Function to receive a string from UART0
void uart0_rx_str(unsigned char *buffer, int length)
{
    int i = 0;
    while (i < length - 1) {
        char c = uart0_rx(); // Receive a single character
        if (c == '\r' || c == '\n') { // Handle end of line
            break;
        }
        buffer[i++] = c;
    }
    buffer[i] = '\0'; // Null-terminate the string
}


