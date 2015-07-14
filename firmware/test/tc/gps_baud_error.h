#ifndef __verification__
#define __verification__
#endif

/****************************//* gps_baud_error_tc *//****************************/
/**
 * Calculates the real hardware baud rate for gps serial
 */
#include "hw_config.h"
#include "sercom/sercom.h"
#include "sercom/usart.h"

/* Parameters in */
struct gps_baud_error_tc_params {

  /* Input paramters to your test case go here */
  uint32_t dummy;

} gps_baud_error_tc_params;
/* Results out */
struct gps_baud_error_tc_results {

  /* Result values should be populated here */
  uint32_t intended_baud;
  uint32_t peripheral_clock;
  uint16_t calculated_baud;

} gps_baud_error_tc_results;
/* Function */
__verification__ void gps_baud_error_tc(void) {

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */

  uint32_t sercom_index = _sercom_get_sercom_inst_index((Sercom*)GPS_SERCOM);
  uint32_t gclk_index   = sercom_index + SERCOM0_GCLK_ID_CORE;
  uint32_t baudrate = GPS_BAUD_RATE;
  uint16_t baud;                /* The actual register value */

  enum sercom_asynchronous_operation_mode mode = SERCOM_ASYNC_OPERATION_MODE_ARITHMETIC;
  enum sercom_asynchronous_sample_num sample_num = SERCOM_ASYNC_SAMPLE_NUM_16;

  uint32_t peripheral_clock = system_gclk_chan_get_hz(gclk_index);

  _sercom_get_async_baud_val(baudrate,
                             peripheral_clock, &baud, mode, sample_num);

  gps_baud_error_tc_results.intended_baud = baudrate;
  gps_baud_error_tc_results.peripheral_clock = peripheral_clock;
  gps_baud_error_tc_results.calculated_baud = baud;
}
