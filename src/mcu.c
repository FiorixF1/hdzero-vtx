#include "camera.h"
#include "common.h"
#include "dm6300.h"
#include "global.h"
#include "hardware.h"
#include "i2c.h"
#include "i2c_device.h"
#include "isr.h"
#include "lifetime.h"
#include "monitor.h"
#include "msp_displayport.h"
#include "print.h"
#include "rom.h"
#include "runcam.h"
#include "sfr_ext.h"
#include "smartaudio_protocol.h"
#include "tramp_protocol.h"
#include "uart.h"
#include "version.h"

uint8_t UNUSED = 0;
uint8_t rf_delay_init_done = 0;

BIT_TYPE int0_req = 0;
BIT_TYPE int1_req = 0;

void Timer0_isr(void) INTERRUPT(1) {
    TH0 = 138;

#ifdef USE_SMARTAUDIO_SW
    if (SA_config) {
        if (suart_tx_en) {
            // TH0 = 139;
            suart_txint();
        } else
            suart_rxint();
    }
#endif

    timer_ms10x++;
}

void Timer1_isr(void) INTERRUPT(3) {
}

void Ext0_isr(void) INTERRUPT(0) {
    int0_req = 1;
}

void Ext1_isr(void) INTERRUPT(2) {
    int1_req = 1;
}

void UART0_isr() INTERRUPT(4) {
    if (RI && (tr_tx_busy == 0)) { // RX int
        RI = 0;
        RS_buf[RS_in++] = SBUF0;
        if (RS_in >= BUF_MAX)
            RS_in = 0;
        if (RS_in == RS_out)
            RS0_ERR = 1;
    }

    if (TI) { // TX int
        TI = 0;
        RS_Xbusy = 0;
    }
}
#ifdef USE_SMARTAUDIO_HW
void UART1_isr() INTERRUPT(6) {

    if (RI1) { // RX int
        RI1 = 0;
        if (sa_status == SA_ST_IDLE) {
            uart_set_baudrate(3);
            sa_status = SA_ST_RX;
            sa_start_ms = timer_ms10x;
        }
        if (sa_status == SA_ST_TX)
            return;

        RS_buf1[RS_in1++] = SBUF1;
        if (RS_in1 >= BUF1_MAX)
            RS_in1 = 0;
    }

    if (TI1) { // TX int
        TI1 = 0;
        RS_Xbusy1 = 0;
    }
}
#else
void UART1_isr() INTERRUPT(6) {

    if (RI1 && (tr_tx_busy == 0)) { // RX int
        RI1 = 0;
        RS_buf1[RS_in1++] = SBUF1;
        if (RS_in1 >= BUF1_MAX)
            RS_in1 = 0;
    }

    if (TI1) { // TX int
        TI1 = 0;
        RS_Xbusy1 = 0;
    }
}
#endif

void version_info(void) {
#ifdef _DEBUG_MODE
    debugf("\r\n");
    debugf("\r\nVtx        : %s", VTX_NAME);
    debugf("\r\nVersion    : %s", VTX_VERSION_STRING);
    debugf("\r\nBuild time : " __DATE__ " " __TIME__);
#endif
}

uint8_t I2C_EN = 0;

void main(void) {
    I2C_EN = 0;
#if (1)
    uint16_t i;

    WAIT(600);
    I2C_EN = 1;

    // init
    CPU_init();
    WriteReg(0, 0xB0, 0x3E);
    WriteReg(0, 0xB2, 0x03);
    WriteReg(0, 0x80, 0xC8);
    check_eeprom();
#endif
#if (0)
    for (i = 0; i < 256; i++) {
        I2C_Write8_Wait(10, ADDR_EEPROM, (uint8_t)i, 0x11);
    }
#endif
#if (0)
    I2C_Write8_Wait(10, ADDR_EEPROM, 0x80, 0x11);
#endif
#if (1)
    for (i = 0; i < 256; i++) {
        //_outchar(I2C_Read8_Wait(10, ADDR_EEPROM, i));
        I2C_Read8_Wait(10, ADDR_EEPROM, i);
    }
#endif
    // main loop
    while (1) {
    }
}