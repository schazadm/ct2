/* ------------------------------------------------------------------
 * --  _____       ______  _____                                    -
 * -- |_   _|     |  ____|/ ____|                                   -
 * --   | |  _ __ | |__  | (___    Institute of Embedded Systems    -
 * --   | | | '_ \|  __|  \___ \   Zuercher Hochschule Winterthur   -
 * --  _| |_| | | | |____ ____) |  (University of Applied Sciences) -
 * -- |_____|_| |_|______|_____/   8401 Winterthur, Switzerland     -
 * ------------------------------------------------------------------
 * --
 * -- Project     : CT2 lab - SPI Display
 * -- Description : Contains the implementations of the functions
 * --               to write and read frames from and to the TFT-LCD
 * --               display EAeDIPTFT43-A.
 * --
 * -- $Id: lcd_io.c 4720 2019-03-04 10:11:31Z akdi $
 * ------------------------------------------------------------------
 */
#include "lcd_io.h"
#include "hal_spi.h"
#include "hal_sbuf.h"

#define ACK_CHAR         (uint8_t)0x06
#define DC1_CHAR         (uint8_t)0x11
#define DC2_CHAR         (uint8_t)0x12
#define ESC_CHAR         (uint8_t)0x1B
#define ONE_CHAR         (uint8_t)0x01

#define NOTHING_RECEIVED (uint8_t)0
enum { SUCCESS = 0, ERRORCODE = 1 };

/* ------------------------------------------------------------------
 * -- Function prototypes
 * ------------------------------------------------------------------
 */
static void send_read_display_buffer_request(void);

/* ------------------------------------------------------------------
 * -- Function implementations
 * ------------------------------------------------------------------
 */

/*
 * according to description in header file
 */
void init_display_interface(void)
{
    hal_spi_init();
    hal_sbuf_init();
}

/*
 * according to description in header file
 */
uint8_t read_display_buffer(uint8_t *readBuffer)
{
    /// STUDENTS: To be programmed
	
		uint8_t length;
    uint8_t x;
    uint8_t bcc = 0;

    send_read_display_buffer_request(); //

    if (hal_spi_read_write(0x00) == ACK_CHAR) { // if cant write
        return 0; // return 0
    }
    if(hal_sbuf_get_state() == 0) { // check if data to be sent
        return 0; // if no data to be sent, return 0
    }
    bcc += hal_spi_read_write(0x00); //
    length = hal_spi_read_write(0x00); // write character
    bcc += length;
    for(x = 0; x < length; x++) {
        readBuffer[x] = hal_spi_read_write(0x00);
        bcc += readBuffer[x];
    }

    if (readBuffer[3] == 1) {
        readBuffer[3] = 2;
    } else {
        readBuffer[3] = 1;
    }

    // Check if bcc correct
    if(bcc % 256 != hal_spi_read_write(0x00)) {
        return 0;
    }
    // return amount of characters written
    return length;


    /// END: To be programmed
}

/*
 * according to description in header file
 */
uint8_t write_cmd_to_display(const uint8_t *cmdBuffer, uint8_t length)
{
    /// STUDENTS: To be programmed

		//  Setze das Sendepaket gem?ss der Praktikumsanleitung S.2mit dem  *cmdBuffer
    //	zusammen, siehe auch lcd.io.h
    uint8_t x;
    uint8_t response;
    uint8_t bcc = 0;

    // Sendepaket Overhead

    // Plus DC1 = 0X1
    hal_spi_read_write(DC1_CHAR); // DC1 = 0X11
    bcc += DC1_CHAR; // Plus erster Character

    // Plus Length = ESC + L?nge Befehlszeichenfolgenl?nge (*cmdBuffer)
    hal_spi_read_write(length + 1);
    bcc += (length + 1);

    //Plus  ESC = 0X1B
    hal_spi_read_write(ESC_CHAR);
    bcc += ESC_CHAR;

    for(x = 0; x < length; x++) {
        hal_spi_read_write(cmdBuffer[x]);
        bcc += cmdBuffer[x];
    }

    // Sende Pr?fsumme
    hal_spi_read_write(bcc % 256);
    response = hal_spi_read_write(0x00);
    // Wenn ACK = 0X06
    if(response == ACK_CHAR) {
        return SUCCESS;
    }
    // Wenn ACK != 0X06
    return ERRORCODE;


    /// END: To be programmed
}

/*
 * Assemble and send a packet to trigger the reading of the display buffer
 * Uses the sequence "<DC2>, 0x01, 0x53, checksum" according to datasheet
 */
static void send_read_display_buffer_request()
{
    /// STUDENTS: To be programmed

		hal_spi_read_write(DC2_CHAR);
    hal_spi_read_write(ONE_CHAR);
    hal_spi_read_write(0x53);
    hal_spi_read_write(0x66 % 256);


    /// END: To be programmed
}
