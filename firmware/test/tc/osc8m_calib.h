#ifndef __verification__
#define __verification__
#endif

#include "xosc.h"

/****************************//* osc8m_calib_tc *//****************************/
/* The simplest test case. ever.   Used to check for sanity                 */

/* Parameters in */
struct osc8m_calib_tc_params {
  int dummy;
} osc8m_calib_tc_params;
/* Results out */
struct osc8m_calib_tc_results {
  int result;
  int c_process;
  int c_temp;
} osc8m_calib_tc_results;


uint32_t _result;

void osc8m_tc_xosc_measure_callback(uint32_t result) {
  _result = result;
}

/* Function */
__verification__ void osc8m_calib_tc(void) {
  struct osc8m_calibration_t calib;

  calib.process = 9;
  calib.temperature = 28;
  osc8m_set_calibration(calib);

  _result = 0;
  measure_xosc(XOSC_MEASURE_TIMEPULSE, osc8m_tc_xosc_measure_callback);

  while (_result == 0);
  osc8m_calib_tc_results.result = _result;

  calib = osc8m_get_calibration();
  osc8m_calib_tc_results.c_process = calib.process;
  osc8m_calib_tc_results.c_temp = calib.temperature;
}
