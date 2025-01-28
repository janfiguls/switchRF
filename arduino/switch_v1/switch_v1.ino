#include <Wire.h>
#include <ArduinoJson.h>

#define VDD 5.12
#define R1 0.3 //MOhms
#define RDIV_ADC float(103.3/3.3)
#define ADC_ADDRESS 0b1001101 //10 bit voltage ADC, 7bit address

#define DAC_ADDRESS 0x48
#define DAC_MEMORY_OUT0 0xF8

float Vout=5; // must be >=MIN_VOUT and <=MAX_VOUT
#define MIN_VOUT 5
#define MAX_VOUT 35
byte adc_data[2];

float IDAC=0;
#define IDAC_MAX 96
#define IDAC_MIN 0
#define POS_IDAC 127
#define IFS 96.5 // 96.5 uA, IDAC full scale current
#define STEP_IDAC float(POS_IDAC/IFS)
float SPEED_IDAC=0.1; // SHOULD be <=1

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output

  if(Vout<MIN_VOUT){
    Vout=MIN_VOUT;
  }
  if(Vout>MAX_VOUT){
    Vout=MAX_VOUT;
  }
  if(SPEED_IDAC>1){
    SPEED_IDAC=1;
  }
}

void loop() {
  Serial.print("Desired Vout(V)= ");
  Serial.print(Vout);
  Serial.print(",");

  // Read the ADC voltage
  float adcread=readADC(ADC_ADDRESS);  
  // Print the read value
  Serial.print("Adcread(V)= ");
  Serial.print(adcread);
  Serial.print(",");

  // Print the old value of IDAC
  Serial.print("Idac(uA)[t-1]= ");
  Serial.print(IDAC);
  Serial.print(",");


  // Compute the new IDAC and send/write it to the DAC
  IDAC=IDAC-SPEED_IDAC*STEP_IDAC*round(((adcread-Vout)/R1)/STEP_IDAC);
  if(IDAC>IDAC_MAX){
    IDAC=IDAC_MAX;
  }
  if(IDAC<IDAC_MIN){
    IDAC=IDAC_MIN;
  }
  writeDAC(byte(IDAC*POS_IDAC/IFS));
  
  // Print the new modified value of IDAC
  Serial.print("Idac(uA)[t]= ");
  Serial.println(IDAC);

  // Wait for 0.5s so that the capacitor has time to charge/discharge
  delay(100); 
}

float readADC(int address) {
  Wire.requestFrom(address, 2);    // request 2 bytes from the ADC

  int j=0;
  while (Wire.available()) { // peripheral may send less than requested
    adc_data[j] = Wire.read(); // receive a byte as an integer
    j++;  
  }
  float adc_voltage=(float(RDIV_ADC*VDD*((adc_data[0]<<6)+(adc_data[1]>>2))/1024));
  return(adc_voltage);

  // this is for debugging... 
    // Serial.print("First byte received: ");  
    // Serial.println(adc_data[0]);
    // Serial.print("Second byte received: ");
    // Serial.println(adc_data[1]);            
    // Serial.print("Adc voltage(V) is ");
    // Serial.print(adc_voltage);
    // Serial.println("V");
}

void writeDAC(byte IDAC){
  Wire.beginTransmission(DAC_ADDRESS);
  Wire.write(DAC_MEMORY_OUT0);
  Wire.write(IDAC);
  Wire.endTransmission();
}

