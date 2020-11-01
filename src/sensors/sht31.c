/*!
 *  ADAPTED FROM:
 *  @mainpage Adafruit SHT31 Digital Humidity & Temp Sensor
 *
 *  @section intro_sec Introduction
 *
 *  This is a library for the SHT31 Digital Humidity & Temp Sensor
 *
 *  Designed specifically to work with the SHT31 Digital sensor from Adafruit
 *
 *  Pick one up today in the adafruit shop!
 *  ------> https://www.adafruit.com/product/2857
 *
 *  These sensors use I2C to communicate, 2 pins are required to interface
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit andopen-source hardware by purchasing products
 *  from Adafruit!
 *
 *  @section author Author
 *
 *  Limor Fried/Ladyada (Adafruit Industries).
 *
 *  @section license License
 *
 *  BSD license, all text above must be included in any redistribution
 */

#include "src/sensors/sht31.h"
/* #include "nrfx_twim.h" */
#include "../../SDK_17/modules/nrfx/drivers/include/nrfx_twim.h"
#include "nrf_log.h"
#include "nrf_delay.h"
#include "stdbool.h"
#include <stdint.h>

const int SHT31_ADDR = 0x44;
#define TWI_INSTANCE_ID 0
static const nrfx_twim_t twi = NRFX_TWIM_INSTANCE(TWI_INSTANCE_ID);
static bool twi_done;

static void twi_handler(nrfx_twim_evt_t const * p_event, void * p_context);

void twi_init() {
    uint32_t err_code;

    const nrfx_twim_config_t twi_config = {
       .scl = 3,
       .sda = 4,
       .frequency = NRFX_TWIM_DEFAULT_CONFIG_FREQUENCY,
       .interrupt_priority = NRFX_TWIM_DEFAULT_CONFIG_IRQ_PRIORITY,
       .hold_bus_uninit = false
    };

    err_code = nrfx_twim_init(&twi, &twi_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrfx_twim_enable(&twi);
}

static void twi_handler(nrfx_twim_evt_t const * p_event, void * p_context)
{
    switch (p_event->type)
    {
        case NRFX_TWIM_EVT_DONE:
            twi_done = true;
            break;
        default:
            break;
    }
}

static void sht31_write_command(uint16_t command) {
    uint8_t cmd[2];

    cmd[0] = command >> 8;
    cmd[1] = command & 0xFF;

    uint32_t err_code = nrfx_twim_tx(&twi, SHT31_ADDR, cmd, sizeof(cmd), false);
    APP_ERROR_CHECK(err_code);
}


//////////////////////////////////////// sensor specific logic /////////////////////////////////

///Thanks adafruit
static uint8_t crc8(const uint8_t *data, int len) {
  const uint8_t POLYNOMIAL = 0x31;
  uint8_t crc = 0xFF;

  for (int j = len; j; --j) {
    crc ^= *data++;

    for (int i = 8; i; --i) {
      crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
    }
  }
  return crc;
}

// allow for a 10 ms delay before reading again after reset
void sht31_reset() {
    const int16_t SHT31_SOFTRESET = 0x30A2;
    sht31_write_command(SHT31_SOFTRESET);
}

void sht31_request_temphum() {
    NRF_LOG_INFO("request temphum");
    const int SHT31_MEAS_HIGHREP = 0x2400;
    sht31_write_command(SHT31_MEAS_HIGHREP);
    //delay(20);
}

// should be run 20ms after request temphum
void sht31_read_temphum(float* temp, float* hum) {
    NRF_LOG_INFO("read temphum");
    /* uint8_t readbuffer[6]; */
    /* twi_done = false; */
    /* uint32_t err_code = nrfx_twim_rx(&twi, SHT31_ADDR, (uint8_t*)&readbuffer, sizeof(readbuffer)); */
    /* APP_ERROR_CHECK(err_code); */
    /* while (twi_done == false); //block till read buffer is filled */ 

    /* if (readbuffer[2] != crc8(readbuffer, 2) || readbuffer[5] != crc8(readbuffer + 3, 2)){ */
    /*     NRF_LOG_ERROR("CRC ERROR IN SHT31"); */
    /* } */

    /* int32_t stemp = (int32_t)(((uint32_t)readbuffer[0] << 8) | readbuffer[1]); */
    /* stemp = ((4375 * stemp) >> 14) - 4500; */
    /* *temp = (float)stemp / 100.0f; */

    /* uint32_t shum = ((uint32_t)readbuffer[3] << 8) | readbuffer[4]; */
    /* shum = (625 * shum) >> 12; */
    /* *hum = (float)shum / 100.0f; */
}
