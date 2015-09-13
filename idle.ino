/*
Speeduino - Simple engine management for the Arduino Mega 2560 platform
Copyright (C) Josh Stewart
A full copy of the license may be found in the projects root directory
*/

/*
These functions over the PWM and stepper idle control
*/

/*
Idle Control
Currently limited to on/off control and open loop PWM and stepper drive
*/
void initialiseIdle()
{
  //Initialising comprises of setting the 2D tables with the relevant values from the config pages
  switch(configPage4.iacAlgorithm)
  {
    case 0:
      //Case 0 is no idle control ('None')
      break;
      
    case 1:
      //Case 1 is on/off idle control
      if (currentStatus.coolant < configPage4.iacFastTemp)
      {
        digitalWrite(pinIdle1, HIGH);
      }
      break;
      
    case 2:
      //Case 2 is PWM open loop
      iacPWMTable.xSize = 10;
      iacPWMTable.values = configPage4.iacOLPWMVal;
      iacPWMTable.axisX = configPage4.iacBins;
      
      iacCrankDutyTable.xSize = 4;
      iacCrankDutyTable.values = configPage4.iacCrankDuty;
      iacCrankDutyTable.axisX = configPage4.iacCrankBins;
      break;
    
    case 3:
      //Case 3 is PWM closed loop
      iacClosedLoopTable.xSize = 10;
      iacClosedLoopTable.values = configPage4.iacCLValues;
      iacClosedLoopTable.axisX = configPage4.iacBins;
      
      iacCrankDutyTable.xSize = 4;
      iacCrankDutyTable.values = configPage4.iacCrankDuty;
      iacCrankDutyTable.axisX = configPage4.iacCrankBins;
      break;
      
    case 4:
      //Case 2 is Stepper open loop
      iacStepTable.xSize = 10;
      iacStepTable.values = configPage4.iacOLStepVal;
      iacStepTable.axisX = configPage4.iacBins;
      
      iacCrankStepsTable.xSize = 4;
      iacCrankStepsTable.values = configPage4.iacCrankSteps;
      iacCrankStepsTable.axisX = configPage4.iacCrankBins;
      
      homeStepper(); //Returns the stepper to the 'home' position
      break;
      
    case 5:
      //Case 5 is Stepper closed loop
      iacClosedLoopTable.xSize = 10;
      iacClosedLoopTable.values = configPage4.iacCLValues;
      iacClosedLoopTable.axisX = configPage4.iacBins;
      
      iacCrankStepsTable.xSize = 4;
      iacCrankStepsTable.values = configPage4.iacCrankSteps;
      iacCrankStepsTable.axisX = configPage4.iacCrankBins;
      break;
  }
  
}

void idleControl()
{
  switch(configPage4.iacAlgorithm)
  {
    case 0:       //Case 0 is no idle control ('None')
      break;
      
    case 1:      //Case 1 is on/off idle control
      if ( (currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET) < configPage4.iacFastTemp) //All temps are offset by 40 degrees
      {
        digitalWrite(pinIdle1, HIGH);
        idleOn = true;
      }
      else if (idleOn) { digitalWrite(pinIdle1, LOW); idleOn = false; }
      break;
      
    case 2:      //Case 2 is PWM open loop
      //Check for cranking pulsewidth
      if( BIT_CHECK(currentStatus.engine, BIT_ENGINE_CRANK) )
      {
        //Currently cranking. Use the cranking table
        analogWrite(pinIdle1, table2D_getValue(&iacCrankDutyTable, currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET)); //All temps are offset by 40 degrees
        idleOn = true;
      }
      else if( (currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET) < iacPWMTable.values[IDLE_TABLE_SIZE-1])
      {
        //Standard running
        analogWrite(pinIdle1, table2D_getValue(&iacPWMTable, currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET)); //All temps are offset by 40 degrees
        idleOn = true;
      }
      else if (idleOn) { digitalWrite(pinIdle1, LOW); idleOn = false; }
      break;
      
    case 3:    //Case 3 is PWM closed loop (Not currently implemented)
      break;
      
    case 4:    //Case 4 is open loop stepper control
      //First thing to check is whether there is currently a step going on and if so, whether it needs to be turned off
      if(idleStepper.stepperStatus == STEPPING)
      {
        if(micros() > (idleStepper.stepStartTime + DRV8825_STEP_TIME) )
        {
          //Means we're currently in a step, but it needs to be turned off
          digitalWrite(pinStepperStep, LOW); //Turn off the step
          idleStepper.stepperStatus = SOFF;
        }
        else
        {
          //Means we're in a step, but it doesn't need to turn off yet. No further action at this time
          return;
        }
      }
      
      //Check for cranking pulsewidth
      if( BIT_CHECK(currentStatus.engine, BIT_ENGINE_CRANK) )
      {
        //Currently cranking. Use the cranking table
        idleStepper.targetIdleStep = table2D_getValue(&iacCrankStepsTable, (currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET)); //All temps are offset by 40 degrees
        if (idleStepper.targetIdleStep == idleStepper.curIdleStep) { return; } //No action required
        else if(idleStepper.targetIdleStep < idleStepper.curIdleStep) { digitalWrite(pinStepperDir, STEPPER_BACKWARD); }//Sets stepper direction to backwards
        else if (idleStepper.targetIdleStep > idleStepper.curIdleStep) { digitalWrite(pinStepperDir, STEPPER_FORWARD); }//Sets stepper direction to forwards
        
        digitalWrite(pinStepperStep, HIGH);
        idleStepper.stepStartTime = micros();
        idleStepper.stepperStatus = STEPPING;
        idleOn = true; 
      }
      else if( (currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET) < iacStepTable.values[IDLE_TABLE_SIZE-1])
      {
        //Standard running
        idleStepper.targetIdleStep = table2D_getValue(&iacStepTable, (currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET)); //All temps are offset by 40 degrees
        if (idleStepper.targetIdleStep == idleStepper.curIdleStep) { return; } //No action required
        else if(idleStepper.targetIdleStep < idleStepper.curIdleStep) { digitalWrite(pinStepperDir, STEPPER_BACKWARD); }//Sets stepper direction to backwards
        else if (idleStepper.targetIdleStep > idleStepper.curIdleStep) { digitalWrite(pinStepperDir, STEPPER_FORWARD); }//Sets stepper direction to forwards
        
        digitalWrite(pinStepperStep, HIGH);
        idleStepper.stepStartTime = micros();
        idleStepper.stepperStatus = STEPPING;
        idleOn = true; 
      }
      
      break;
  }
}

/*
A simple function to home the stepper motor (If in use)
*/
void homeStepper()
{
   //Need to 'home' the stepper on startup
   digitalWrite(pinStepperDir, STEPPER_BACKWARD); //Sets stepper direction to backwards
   for(int x=0; x < configPage4.iacStepHome; x++)
   {
     digitalWrite(pinStepperStep, HIGH);
     delayMicroseconds(DRV8825_STEP_TIME);
   }
   digitalWrite(pinStepperDir, STEPPER_FORWARD);
   idleStepper.curIdleStep = 0; 
   idleStepper.targetIdleStep = 0;
   idleStepper.stepperStatus = SOFF;
}
