/*
Speeduino - Simple engine management for the Arduino Mega 2560 platform
Copyright (C) Josh Stewart
A full copy of the license may be found in the projects root directory
*/

/*
This is called when a command is received over serial from TunerStudio / Megatune
It parses the command and calls the relevant function
A detailed description of each call can be found at: http://www.msextra.com/doc/ms1extra/COM_RS232.htm
*/
//#include "comms.h"
//#include "globals.h"
//#include "storage.h"

void command()
{
  switch (Serial.read()) 
  {
      case 'A': // send 22 bytes of realtime values
        sendValues(22);
        break;

      case 'B': // Burn current values to eeprom
        writeConfig();
        break;

      case 'C': // test communications. This is used by Tunerstudio to see whether there is an ECU on a given serial port
        testComm();
        break;

      case 'P': // set the current page
        //A 2nd byte of data is required after the 'P' specifying the new page number. 
        //This loop should never need to run as the byte should already be in the buffer, but is here just in case
        while (Serial.available() == 0) { }
        currentPage = Serial.read();
        break; 

      case 'R': // send 39 bytes of realtime values
        sendValues(39);
        break;    

      case 'S': // send code version
        Serial.print(signature);
        break;

      case 'Q': // send code version
        Serial.print(signature);
        //Serial.write("Speeduino_0_2");
        break;

      case 'V': // send VE table and constants
        sendPage();
        break;

      case 'W': // receive new VE or constant at 'W'+<offset>+<newbyte>
        int offset;
        while (Serial.available() == 0) { }
        
        if(currentPage == veMapPage || currentPage == ignMapPage || currentPage == afrMapPage )
        {
        byte offset1, offset2;
        offset1 = Serial.read();
        while (Serial.available() == 0) { }
        offset2 = Serial.read();
        offset = word(offset2, offset1);
        }
        else
        {
          offset = Serial.read();
        }
        while (Serial.available() == 0) { }
        
        receiveValue(offset, Serial.read());
	break;

      case 't': // receive new Calibration info. Command structure: "t", <tble_idx> <data array>. This is an MS2/Extra command, NOT part of MS1 spec
        byte tableID;
        //byte canID;
        
        //The first 2 bytes sent represent the canID and tableID
        while (Serial.available() == 0) { }
          tableID = Serial.read(); //Not currently used for anything
          
        receiveCalibration(tableID); //Receive new values and store in memory
        writeCalibration(); //Store received values in EEPROM

	break;

      case 'Z': //Totally non-standard testing function. Will be removed once calibration testing is completed. This function takes 1.5kb of program space! :S
        
        Serial.println("Coolant");
        for(int x=0; x<CALIBRATION_TABLE_SIZE; x++)
        {
          Serial.print(x);
          Serial.print(", ");
          Serial.println(cltCalibrationTable[x]);
        }
        Serial.println("Inlet temp");
        for(int x=0; x<CALIBRATION_TABLE_SIZE; x++)
        {
          Serial.print(x);
          Serial.print(", ");
          Serial.println(iatCalibrationTable[x]);
        }
        Serial.println("O2");
        for(int x=0; x<CALIBRATION_TABLE_SIZE; x++)
        {
          Serial.print(x);
          Serial.print(", ");
          Serial.println(o2CalibrationTable[x]);
        }
        Serial.println("WUE");
        for(int x=0; x<10; x++)
        {
          Serial.print(configPage2.wueBins[x]);
          Serial.print(", ");
          Serial.println(configPage1.wueValues[x]);
        }
        Serial.flush();
	break;

      case 'T': //Send 256 tooth log entries to Tuner Studios tooth logger
        sendToothLog(false); //Sends tooth log values as ints
	break;

      case 'r': //Send 256 tooth log entries to a terminal emulator
        sendToothLog(true); //Sends tooth log values as chars
	break;

      default:
	break;
  } 
}

/*
This function returns the current values of a fixed group of variables
*/
void sendValues(int length)
{
  byte packetSize = 30;
  byte response[packetSize];
  
  response[0] = currentStatus.secl; //secl is simply a counter that increments each second. Used to track unexpected resets (Which will reset this count to 0)
  response[1] = currentStatus.squirt; //Squirt Bitfield
  response[2] = currentStatus.engine; //Engine Status Bitfield
  response[3] = 0x00; //baro
  response[4] = currentStatus.MAP; //map
  response[5] = (byte)(currentStatus.IAT + CALIBRATION_TEMPERATURE_OFFSET); //mat
  response[6] = (byte)(currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET); //Coolant ADC
  response[7] = currentStatus.tpsADC; //TPS (Raw 0-255)
  response[8] = currentStatus.battery10; //battery voltage
  response[9] = currentStatus.O2; //O2
  response[10] = currentStatus.egoCorrection; //Exhaust gas correction (%)
  response[11] = 0x00; //Air Correction (%)
  response[12] = currentStatus.wueCorrection; //Warmup enrichment (%)
  response[13] = lowByte(currentStatus.RPM); //rpm HB
  response[14] = highByte(currentStatus.RPM); //rpm LB
  response[15] = currentStatus.TAEamount; //acceleration enrichment (%)
  response[16] = 0x00; //Barometer correction (%)
  response[17] = currentStatus.corrections; //Total GammaE (%)
  response[18] = currentStatus.VE; //Current VE 1 (%)
  response[19] = currentStatus.afrTarget;
  response[20] = (byte)(currentStatus.PW / 100); //Pulsewidth 1 multiplied by 10 in ms. Have to convert from uS to mS. 
  response[21] = currentStatus.tpsDOT; //TPS DOT
  response[22] = currentStatus.advance;
  response[23] = currentStatus.TPS; // TPS (0% to 100%)
  //Need to split the int loopsPerSecond value into 2 bytes
  response[24] = lowByte(currentStatus.loopsPerSecond);
  response[25] = highByte(currentStatus.loopsPerSecond);
 
  //The following can be used to show the amount of free memory
  currentStatus.freeRAM = freeRam();
  response[26] = lowByte(currentStatus.freeRAM); //(byte)((currentStatus.loopsPerSecond >> 8) & 0xFF);
  response[27] = highByte(currentStatus.freeRAM);
  
  response[28] = currentStatus.batCorrection; //Battery voltage correction (%)
  response[29] = (byte)(currentStatus.dwell / 100);
  

  Serial.write(response, (size_t)packetSize);
  //Serial.flush();
  return; 
}

void receiveValue(int offset, byte newValue)
{
  
  byte* pnt_configPage;

  switch (currentPage) 
  {
      case veMapPage:
        if (offset < 256) //New value is part of the fuel map
        {
          fuelTable.values[15-offset/16][offset%16] = newValue;
          return;
        }
        else
        {
          //Check whether this is on the X (RPM) or Y (MAP/TPS) axis
          if (offset < 272) 
          { 
            //X Axis
            fuelTable.axisX[(offset-256)] = ((int)(newValue) * 100); //The RPM values sent by megasquirt are divided by 100, need to multiple it back by 100 to make it correct
          }
          else
          { 
            //Y Axis
            offset = 15-(offset-272); //Need to do a translation to flip the order (Due to us using (0,0) in the top left rather than bottom right
            fuelTable.axisY[offset] = (int)(newValue);
          }
          return;
        }
        break;
        
      case veSetPage:
        pnt_configPage = (byte *)&configPage1; //Setup a pointer to the relevant config page
        //For some reason, TunerStudio is sending offsets greater than the maximum page size. I'm not sure if it's their bug or mine, but the fix is to only update the config page if the offset is less than the maximum size
        if( offset < page_size)
        {
          *(pnt_configPage + (byte)offset) = newValue; //Need to subtract 80 because the map and bins (Which make up 80 bytes) aren't part of the config pages
        } 
        break;
        
      case ignMapPage: //Ignition settings page (Page 2)
        if (offset < 256) //New value is part of the ignition map
        {
          ignitionTable.values[15-offset/16][offset%16] = newValue;
          return;
        }
        else
        {
          //Check whether this is on the X (RPM) or Y (MAP/TPS) axis
          if (offset < 272) 
          { 
            //X Axis
            ignitionTable.axisX[(offset-256)] = (int)(newValue) * int(100); //The RPM values sent by megasquirt are divided by 100, need to multiple it back by 100 to make it correct
          }
          else
          { 
            //Y Axis
            offset = 15-(offset-272); //Need to do a translation to flip the order 
            ignitionTable.axisY[offset] = (int)(newValue);
          }
          return;
        }
        
      case ignSetPage:
        pnt_configPage = (byte *)&configPage2;
        //For some reason, TunerStudio is sending offsets greater than the maximum page size. I'm not sure if it's their bug or mine, but the fix is to only update the config page if the offset is less than the maximum size
        if( offset < page_size)
        {
          *(pnt_configPage + (byte)offset) = newValue; //Need to subtract 80 because the map and bins (Which make up 80 bytes) aren't part of the config pages
        }
        break;
        
      case afrMapPage: //Air/Fuel ratio target settings page
        if (offset < 256) //New value is part of the afr map
        {
          afrTable.values[15-offset/16][offset%16] = newValue;
          return;
        }
        else
        {
          //Check whether this is on the X (RPM) or Y (MAP/TPS) axis
          if (offset < 272) 
          { 
            //X Axis
            afrTable.axisX[(offset-256)] = int(newValue) * int(100); //The RPM values sent by megasquirt are divided by 100, need to multiply it back by 100 to make it correct
          }
          else
          { 
            //Y Axis
            offset = 15-(offset-272); //Need to do a translation to flip the order 
            afrTable.axisY[offset] = int(newValue);
            
          }
          return;
        }
      
      case afrSetPage:
        pnt_configPage = (byte *)&configPage3;
        //For some reason, TunerStudio is sending offsets greater than the maximum page size. I'm not sure if it's their bug or mine, but the fix is to only update the config page if the offset is less than the maximum size
        if( offset < page_size)
        {
          *(pnt_configPage + (byte)offset) = newValue; //Need to subtract 80 because the map and bins (Which make up 80 bytes) aren't part of the config pages
        }
        break;
      
      case iacPage: //Idle Air Control settings page (Page 4)
        pnt_configPage = (byte *)&configPage4;
        //For some reason, TunerStudio is sending offsets greater than the maximum page size. I'm not sure if it's their bug or mine, but the fix is to only update the config page if the offset is less than the maximum size
        if( offset < page_size)
        {
          *(pnt_configPage + (byte)offset) = newValue;
        }
        break;
      
      default:
	break;
  }
}

/*
sendPage() packs the data within the current page (As set with the 'P' command) 
into a buffer and sends it.
Note that some translation of the data is required to lay it out in the way Megasqurit / TunerStudio expect it
*/
void sendPage()
{
  byte* pnt_configPage;
  
  switch (currentPage) 
  {
      case veMapPage:
      {
        //Need to perform a translation of the values[MAP/TPS][RPM] into the MS expected format
        //MS format has origin (0,0) in the bottom left corner, we use the top left for efficiency reasons
        byte response[map_page_size];
        
        for(int x=0;x<256;x++) { response[x] = fuelTable.values[15-x/16][x%16]; } //This is slightly non-intuitive, but essentially just flips the table vertically (IE top line becomes the bottom line etc). Columns are unchanged
        for(int x=256;x<272;x++) { response[x] = byte(fuelTable.axisX[(x-256)] / 100); } //RPM Bins for VE table (Need to be dvidied by 100)
        for(int y=272;y<288;y++) { response[y] = byte(fuelTable.axisY[15-(y-272)]); } //MAP or TPS bins for VE table 
        Serial.write((byte *)&response, sizeof(response));
        break;
      }
        
      case veSetPage:
      {
        //All other bytes can simply be copied from the config table
        byte response[page_size];
        
        pnt_configPage = (byte *)&configPage1; //Create a pointer to Page 1 in memory
        for(byte x=0; x<page_size; x++)
        { 
          response[x] = *(pnt_configPage + x); //Each byte is simply the location in memory of configPage1 + the offset + the variable number (x)
        }
        Serial.write((byte *)&response, sizeof(response));
        break;
      }
        
      case ignMapPage:
      {
        //Need to perform a translation of the values[MAP/TPS][RPM] into the MS expected format
        byte response[map_page_size];
        
        for(int x=0;x<256;x++) { response[x] = ignitionTable.values[15-x/16][x%16]; }
        for(int x=256;x<272;x++) { response[x] = byte(ignitionTable.axisX[(x-256)] / 100); }
        for(int y=272;y<288;y++) { response[y] = byte(ignitionTable.axisY[15-(y-272)]); }
        Serial.write((byte *)&response, sizeof(response));
        break;
      }
        
      case ignSetPage:
      {
        //All other bytes can simply be copied from the config table
        byte response[page_size];
        
        pnt_configPage = (byte *)&configPage2; //Create a pointer to Page 2 in memory
        for(byte x=0; x<page_size; x++)
        { 
          response[x] = *(pnt_configPage + x); //Each byte is simply the location in memory of configPage2 + the offset + the variable number (x)
        }
        Serial.write((byte *)&response, sizeof(response)); 
        break;
      }
        
      case afrMapPage:
      {
        //Need to perform a translation of the values[MAP/TPS][RPM] into the MS expected format
        byte response[map_page_size];
        
        for(int x=0;x<256;x++) { response[x] = afrTable.values[15-x/16][x%16]; }
        for(int x=256;x<272;x++) { response[x] = byte(afrTable.axisX[(x-256)] / 100); }
        for(int y=272;y<288;y++) { response[y] = byte(afrTable.axisY[15-(y-272)]); }
        Serial.write((byte *)&response, sizeof(response));
        break;
      }
        
      case afrSetPage:
      {
        //All other bytes can simply be copied from the config table
        byte response[page_size];
        
        pnt_configPage = (byte *)&configPage3; //Create a pointer to Page 2 in memory
        for(byte x=0; x<page_size; x++)
        { 
          response[x] = *(pnt_configPage + x); //Each byte is simply the location in memory of configPage2 + the offset + the variable number (x)
        }
        Serial.write((byte *)&response, sizeof(response)); 
        break;
      }
        
      case iacPage:
      {
        byte response[page_size];
      
        pnt_configPage = (byte *)&configPage4; //Create a pointer to Page 2 in memory
        for(byte x=0; x<page_size; x++)
        { 
          response[x] = *(pnt_configPage + x); //Each byte is simply the location in memory of configPage2 + the offset + the variable number (x)
        }
        Serial.write((byte *)&response, sizeof(response)); 
        break;
      }
        
      default:
	break;
  }

  return; 
}


/*
This function is used to store calibration data sent by Tuner Studio. 
*/
void receiveCalibration(byte tableID)
{
  byte* pnt_TargetTable; //Pointer that will be used to point to the required target table
  int OFFSET, DIVISION_FACTOR, BYTES_PER_VALUE;

  switch (tableID)
  {
    case 0:
      //coolant table
      pnt_TargetTable = (byte *)&cltCalibrationTable;
      OFFSET = CALIBRATION_TEMPERATURE_OFFSET; //
      DIVISION_FACTOR = 10;
      BYTES_PER_VALUE = 2;
      break;
    case 1:
      //Inlet air temp table
      pnt_TargetTable = (byte *)&iatCalibrationTable;
      OFFSET = CALIBRATION_TEMPERATURE_OFFSET;
      DIVISION_FACTOR = 10;
      BYTES_PER_VALUE = 2;
      break;
    case 2:
      //O2 table
      pnt_TargetTable = (byte *)&o2CalibrationTable;
      OFFSET = 0;
      DIVISION_FACTOR = 1;
      BYTES_PER_VALUE = 1;
      break;

    default:
      return; //Should never get here, but if we do, just fail back to main loop
      //pnt_TargetTable = (table2D *)&o2CalibrationTable;
      //break;
  }

  //1024 value pairs are sent. We have to receive them all, but only use every second one (We only store 512 calibratino table entries to save on EEPROM space)
  //The values are sent as 2 byte ints, but we convert them to single bytes. Any values over 255 are capped at 255.
  int tempValue;
  byte tempBuffer[2];
  bool every2nd = true;
  int x;
  int counter = 0;
  pinMode(13, OUTPUT);
  
  digitalWrite(13, LOW);
  for (x = 0; x < 1024; x++)
  {
    //UNlike what is listed in the protocol documentation, the O2 sensor values are sent as bytes rather than ints
    if(BYTES_PER_VALUE == 1)
    {
      while ( Serial.available() < 1 ) {}
      tempValue = Serial.read();
    }
    else
    {
      while ( Serial.available() < 2 ) {}
      tempBuffer[0] = Serial.read();
      tempBuffer[1] = Serial.read();
      
      tempValue = div(int(word(tempBuffer[1], tempBuffer[0])), DIVISION_FACTOR).quot; //Read 2 bytes, convert to word (an unsigned int), convert to signed int. These values come through * 10 from Tuner Studio
      tempValue = ((tempValue - 32) * 5) / 9; //Convert from F to C
    }
    tempValue = tempValue + OFFSET;

    if (every2nd) //Only use every 2nd value
    {
      if (tempValue > 255) { tempValue = 255; } // Cap the maximum value to prevent overflow when converting to byte
      if (tempValue < 0) { tempValue = 0; }
      
      pnt_TargetTable[(x / 2)] = (byte)tempValue;
      int y = EEPROM_CALIBRATION_O2 + counter;

      every2nd = false;
      analogWrite(13, (counter % 50) );
      counter++;
    }
    else { every2nd = true; }

  }

}

/*
Send 256 tooth log entries
 * if useChar is true, the values are sent as chars to be printed out by a terminal emulator
 * if useChar is false, the values are sent as a 2 byte integer which is readable by TunerStudios tooth logger
*/
void sendToothLog(bool useChar)
{

      //We need 256 records to send to TunerStudio. If there aren't that many in the buffer (Buffer is 512 long) then we just return and wait for the next call
      if (toothHistoryIndex < 256) { return; } //Don't believe this is the best way to go. Just display whatever is in the buffer
      int tempToothHistory[512]; //Create a temporary array that will contain a copy of what is in the main toothHistory array
      
      //Copy the working history into the temporary buffer array. This is done so that, if the history loops whilst the values are being sent over serial, it doesn't affect the values
      memcpy( (void*)tempToothHistory, (void*)toothHistory, sizeof(tempToothHistory) );
      toothHistoryIndex = 0; //Reset the history index

      //Loop only needs to go to 256 (Even though the buffer is 512 long) as we only ever send 256 entries at a time
      if (useChar)
      {
        for(int x=0; x<256; x++)
        {
          Serial.println(tempToothHistory[x]);
        }
      }
      else
      {
        for(int x=0; x<256; x++)
        {
          Serial.write(highByte(tempToothHistory[x]));
          Serial.write(lowByte(tempToothHistory[x]));
        }
      }
      Serial.flush();
}
  

void testComm()
{
  Serial.write(1);
  return; 
}
