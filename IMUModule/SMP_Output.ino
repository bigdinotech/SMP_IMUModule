//Dino Tinitigan
//IMU9 Output for Smart Modular Platform

byte imu9ID = 38;        //ID for gyro
//byte accID = 40;         //ID for accelerometer
byte dataChecksum = 0;       //checksum byte
byte header = 170;       //Header 0b10101010
byte imuLength = 6;
//byte accLength = 3;
byte compassLength = 2;
int imuPriority = 10;
int imuWait = 200;

byte rollMSB;
byte rollLSB;
byte pitchMSB;
byte pitchLSB;
byte yawMSB;
byte yawLSB;

unsigned int prevRoll_16 = 0;
unsigned int prevPitch_16 = 0;
unsigned int prevYaw_16 = 0;
  
const double MAP360IN16BIT = 0.0054932;


void IMUModule(void)
{
  
  float froll = ToDeg(roll);
  float fpitch = ToDeg(pitch);
  float fyaw = ToDeg(yaw);
  
  
  
  //remap values to 360 degree range
  if(froll < 0)
  {
    froll = 360.0 + froll;
  }
  if(fyaw <0)
  {
    fyaw = 360.0 + fyaw;
  }
  fpitch += 90.0;
  
  unsigned int Roll_16 = froll/MAP360IN16BIT;
  unsigned int Pitch_16 = fpitch/MAP360IN16BIT;
  unsigned int Yaw_16 = fyaw/MAP360IN16BIT;
  
  if((prevRoll_16 == Roll_16) && (prevPitch_16 == Pitch_16) && (prevYaw_16 == Yaw_16))
  {
    //no changes do not transmit anything
  }
  
  else
  {
    rollMSB = (Roll_16/256);
    Roll_16 = Roll_16 & 0b0000000011111111;
    rollLSB = Roll_16;
    pitchMSB = (Pitch_16/256);
    Pitch_16 = Pitch_16 & 0b0000000011111111;
    pitchLSB = Pitch_16;
    yawMSB = (Yaw_16/256);
    Yaw_16 = Yaw_16 & 0b0000000011111111;
    yawLSB = Yaw_16;
    
    //calculate checksum
    dataChecksum = 0;
    dataChecksum += imu9ID;
    dataChecksum += imuLength;
    dataChecksum += rollMSB;
    dataChecksum += rollLSB;
    dataChecksum += pitchMSB;
    dataChecksum += pitchLSB;
    dataChecksum += yawMSB;
    dataChecksum += yawLSB;
  
    int test = digitalRead(bPin);
    while(test)
    {
      //BUS STOP
      delay(random(imuPriority));	//wait in the BUS STOP sometime before reading
      imuPriority--;
      if(imuPriority <=1)
      {
        imuPriority = 1;
      }
      test = digitalRead(bPin);	//read the bPin
    }
    transmit();
    //delay(100);
  }
}

void transmit()
{
  digitalWrite(sPin,LOW);     //pull LOW to signify sending
  Serial.write(header);       //Header Byte
  Serial.write(imu9ID);       //ID Byte  
  Serial.write(imuLength);    //Length Byte
  Serial.write(rollMSB);      //Data Bytes
  Serial.write(rollLSB);
  Serial.write(pitchMSB);
  Serial.write(pitchLSB);
  Serial.write(yawMSB);
  Serial.write(yawLSB);
  Serial.write(dataChecksum); //Checksum Byte
  Serial.flush();
  digitalWrite(sPin, HIGH);   //pull LOW to signify done sending
}
