/**
  ******************************************************************************
  * @file    USER/main_functions.h
  * @author  SBS@DataTellIt Inc.
  * @version V1.0.1
  * @date    May 2, 2021
  * @brief   This is the define of main functions of Alcoho Sensor Application
  ******************************************************************************
  *
  * <h2><center>&copy; COPYRIGHT 2016-2021 DataTellIt Inc.</center></h2>
  ******************************************************************************
  */

#ifndef DTIT_ALCOHO_SENSOR_MAIN_FUNCTIONS_H_
#define DTIT_ALCOHO_SENSOR_MAIN_FUNCTIONS_H_

// Expose a C friendly interface for main functions.
#ifdef __cplusplus
extern "C" {
#endif
  
// Initializes all data needed for the example. The name is important, and needs
// to be setup() for Arduino compatibility.
void setup();

// Runs one iteration of data gathering and inference. This should be called
// repeatedly from the application code. The name needs to be loop() for Arduino
// compatibility.
int loop();

#ifdef __cplusplus
}
#endif

#endif  // DTIT_ALCOHO_SENSOR_MAIN_FUNCTIONS_H_

