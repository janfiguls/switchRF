#include <Wire.h>
#include <ArduinoJson.h>

JsonDocument doc,rec; 
JsonArray switches=doc["switches"].to<JsonArray>(); 
JsonArray switchNames=doc["switchNames"].to<JsonArray>();
#define N_SWITCHES 2

#define VDD 5.12
#define R1 0.3 //MOhms
#define RDIV_ADC float(103.3/3.3)
#define ADC_ADDRESS 0b1001101 //10 bit voltage ADC, 7bit address

#define DAC_ADDRESS 0x48
#define DAC_MEMORY_OUT0 0xF8

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

unsigned long t=millis();

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  Serial.setTimeout(3);
  switchNames.add("s1");
  switchNames.add("s3");
  initSwitches();
}
void initSwitches() {
    for(int i=0;i<N_SWITCHES;i++){
      JsonObject s=switches[i].to<JsonObject>();
      // s1['whatever'] throws out an error, make sure to use ""!!!
      s["Vout"]=MIN_VOUT;
      s["adcread"]=999;
    }
  }

void loop(){
  String data=Serial.readStringUntil('\r');
  String send;
  data.trim();
  if(data!=0){
    DeserializationError error = deserializeJson(rec, data);
    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.print(error.f_str());
      Serial.print("\n\r");
      return;
    }
    if(rec["action"]=="getNames"){
      sendNames();
    }
    else if(rec["action"]=="getStatus"){
      serializeJson(switches,Serial);
    }
    else if(rec["action"]=="setVout"){
      for(int i=0;i<N_SWITCHES;i++){
        if(switchNames[i]==rec["switch"]["name"]){
          switches[i]["Vout"]=rec["switch"]["Vout"];
          i=N_SWITCHES;
        }
      }
    }
    
  }
  switchRW();
}
void sendNames(){
  serializeJson(switchNames,Serial);
  Serial.print('\r');
}

void switchRW() {
  for (int i=0;i<N_SWITCHES;i++){
    JsonObject s=switches[i];
    float Vout=s["Vout"];

    // Read the ADC voltage
    float adcread=readADC(ADC_ADDRESS);  
    s["adcread"]=adcread;
    
    // Compute the new IDAC and send/write it to the DAC
    IDAC=IDAC-SPEED_IDAC*STEP_IDAC*round(((adcread-Vout)/R1)/STEP_IDAC);
    if(IDAC>IDAC_MAX){
      IDAC=IDAC_MAX;
    }
    if(IDAC<IDAC_MIN){
      IDAC=IDAC_MIN;
    }

    writeDAC(byte(IDAC*POS_IDAC/IFS));
    if(millis()-t> 1000){
      // serializeJson(s,Serial);
        //Serial.print('\r'); 
        t=millis();
      }
  }
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
}

void writeDAC(byte IDAC){
  Wire.beginTransmission(DAC_ADDRESS);
  Wire.write(DAC_MEMORY_OUT0);
  Wire.write(IDAC);
  Wire.endTransmission();
}

bool checkVout(float Vout_in){
  return(Vout_in>=MIN_VOUT && Vout_in<=MAX_VOUT);
}
void sendError(String error_msg){
  JsonDocument error;
  error["error"]=error_msg;
  serializeJson(error,Serial);
  Serial.print('\r');
}