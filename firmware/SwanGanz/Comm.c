

// 80 MHz CPU clock, creates 115200 baud rate
void UART_Init() {
    UART0->GPRCM.RSTCTL = 0xB1000003; // reset UART0
    UART0->GPRCM.PWREN = 0x26000001; // activate UART0
    Clock_Delay(24); // time for uart to activate

    // configure PA11 PA10 as alternate UART0 function
    IOMUX->SECCFG.PINCM[PA10INDEX] = 0x00000082;

    //bit 7 PC connected
    //bits 5-0=2 for UART0_Tx
    IOMUX->SECCFG.PINCM[PA11INDEX] = 0x00040082;

    //bit 18 INENA input enable
    //bit 7 PC connected
    //bits 5-0=2 for UART0_Rx
    UART0->CLKSEL = 0x08; // bus clock
    UART0->CLKDIV = 0x00; // no divide
    UART0->CTL0 &= ~0x01; // disable UART0
    UART0->CTL0 = 0x00020018; // enable fifos, tx and rx
    // 40000000/16 = 2,500,000, 2,500,000/115200 = 21.70139
    UART0->IBRD = 21; // divider = 21+45/64 = 21.703125
    UART0->FBRD = 45;
    UART0->LCRH = 0x00000030; // 8bit, 1 stop, no parity
    UART0->CTL0 |= 0x01; // enable UART0
}