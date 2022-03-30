/** \file advance.cpp
 * @brief Functions for calculating degrees of ignition advance
 * 
 */

#include "globals.h"
#include "advance.h"
#include "corrections.h"

/** 
 * @brief Returns the calculated advance
 * 
 * @details Calculates the advance based on configuration and current state. Determines how the output from the primary and secondary spark table is used.
 * 
 * @return The current target advance value in degrees
 */

int8_t getAdvance() {
  
  int16_t correction = 0;
  if (correctionsIgn(correction) == true) { // If there is a fixed correction we are done
    currentStatus.advance1 = 0;
    currentStatus.advance2 = 0;
    return correction;
  }

  int16_t tempAdvance = getAdvance1() + correction; // Advance from table 1 + corrections

  if( sparkTable2Enabled() == true ) //Spark table 2
  {
    currentStatus.advance1 = 0; // Since this isn't valid anymore reset it
    BIT_SET(currentStatus.spark2, BIT_SPARK2_SPARK2_ACTIVE); //Set the bit indicating that the 2nd spark table is in use.

    int16_t tempAdvance2 = getAdvance2(); // Advance from table 2

    if(configPage10.spark2Mode == SPARK2_MODE_MULTIPLY || configPage10.spark2Mode == SPARK2_MODE_ADD)
    {
      if(configPage10.spark2correctedMultiplyAddedAdvance == false) { //The old code applies the advance corrections to both tables before adding or multiplying
        tempAdvance2 += correction;
      }

      if(configPage10.spark2Mode == SPARK2_MODE_MULTIPLY) {
        if(tempAdvance2 < 0) { tempAdvance2 = 0; } //Negative values not supported
        tempAdvance2 = (tempAdvance * tempAdvance2) / 100; //Spark 2 table is treated as a % value. Table 1 and 2 are multiplied together and divided by 100
      }
      else { // SPARK_MODE_ADD
        tempAdvance2 = tempAdvance + tempAdvance2;
      }

    }
    else { // All spark table 2 modes except MULTIPLY and ADD
      tempAdvance2 += correction;
    }

    currentStatus.advance2 = tempAdvance = constrain(tempAdvance2, -128, 127);
  }
  else { //Spark table 1
    currentStatus.advance2 = 0; // Since this isn't valid anymore reset it
    BIT_CLEAR(currentStatus.spark2, BIT_SPARK2_SPARK2_ACTIVE); //Clear the bit indicating that the 2nd spark table is in use.

    currentStatus.advance1 = tempAdvance = constrain(tempAdvance, -128, 127);
  }

  return tempAdvance;
}

/**
 * @brief Queries the Primary spark table based on RPM and the user configured load source
 * 
 * @return Primary spark table result in degrees
 */
int16_t getAdvance1()
{
  int16_t tempAdvance = 0;
  if (configPage2.ignAlgorithm == LOAD_SOURCE_MAP) //Check which fuelling algorithm is being used
  {
    //Speed Density
    currentStatus.ignLoad = currentStatus.MAP;
  }
  else if(configPage2.ignAlgorithm == LOAD_SOURCE_TPS)
  {
    //Alpha-N
    currentStatus.ignLoad = currentStatus.TPS * 2;

  }
  else if (configPage2.fuelAlgorithm == LOAD_SOURCE_IMAPEMAP)
  {
    //IMAP / EMAP
    currentStatus.ignLoad = (currentStatus.MAP * 100) / currentStatus.EMAP;
  }

  tempAdvance = get3DTableValue(&ignitionTable, currentStatus.ignLoad, currentStatus.RPM) - OFFSET_IGNITION; //As above, but for ignition advance

  return tempAdvance;
}

/**
 * @brief Queries the secondary spark table based on RPM and the user configured load source
 * 
 * @return Secondary spark table result in degrees
 */
int16_t getAdvance2()
{
  int16_t tempAdvance = 0;
  if (configPage10.spark2Algorithm == LOAD_SOURCE_MAP) //Check which fuelling algorithm is being used
  {
    //Speed Density
    currentStatus.ignLoad2 = currentStatus.MAP;
  }
  else if(configPage10.spark2Algorithm == LOAD_SOURCE_TPS)
  {
    //Alpha-N
    currentStatus.ignLoad2 = currentStatus.TPS * 2;
  }
  else if (configPage10.spark2Algorithm == LOAD_SOURCE_IMAPEMAP)
  {
    //IMAP / EMAP
    currentStatus.ignLoad2 = (currentStatus.MAP * 100) / currentStatus.EMAP;
  }
  else { currentStatus.ignLoad2 = currentStatus.MAP; }

  tempAdvance = get3DTableValue(&ignitionTable2, currentStatus.ignLoad2, currentStatus.RPM) - OFFSET_IGNITION; //As above, but for ignition advance

  return tempAdvance;
}

/**
 * @brief Checks if we should use spark table 2
 * 
 * @details Uses the secondary spark table configuration to check wether the secondary spark table is enabled. Any conditional switches are also checked.
 * 
 * @retval true The configuration and current state enables spark table 2
 * 
 * @retval false The configuration and current state enables spark table 2
 */
bool sparkTable2Enabled() {
  if (configPage10.spark2Mode <= 0)
  { return false; }

  if (configPage10.spark2Mode == SPARK2_MODE_MULTIPLY || configPage10.spark2Mode == SPARK2_MODE_ADD )
  { return true; }

  if (configPage10.spark2Mode == SPARK2_MODE_CONDITIONAL_SWITCH) {
    if ( (configPage10.spark2SwitchVariable == SPARK2_CONDITION_RPM && currentStatus.RPM > configPage10.spark2SwitchValue       ) ||
         (configPage10.spark2SwitchVariable == SPARK2_CONDITION_MAP && currentStatus.MAP > configPage10.spark2SwitchValue       ) ||
         (configPage10.spark2SwitchVariable == SPARK2_CONDITION_TPS && currentStatus.TPS > configPage10.spark2SwitchValue       ) ||
         (configPage10.spark2SwitchVariable == SPARK2_CONDITION_ETH && currentStatus.ethanolPct > configPage10.spark2SwitchValue) )
    { return true; }
  }

  else if ( configPage10.spark2Mode == SPARK2_MODE_INPUT_SWITCH && digitalRead(pinSpark2Input) == configPage10.spark2InputPolarity)
  { return true; }

  //Default
  return false;
}