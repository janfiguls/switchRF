#include <Wire.h>
#include <ArduinoJson.h>

JsonDocument doc,rec;  
// de moment només hi ha s1, però n'hi haurà més 
JsonObject s1=doc["s1"].to<JsonObject>();
#define N_SWITCHS 1

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

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  
  if(SPEED_IDAC>1){
    SPEED_IDAC=1;
  }
  initSwitch(s1);
  //initSwitch(s2,s3...);
}
void initSwitch(JsonObject s) {
    // s1['whatever'] throws out an error, make sure to use ""!!!
    s["Vout"]=15;
    s["adcread"]=999;
    s["ready"]="starting...";
    switchRW(s);
  }

void loop(){
  String data=Serial.readString();
  data.trim();
  if(data!=0){
    DeserializationError error = deserializeJson(rec, data);
    // Test if parsing succeeds.
    if (error) {
      //Serial.print("ERROr");
      //Serial.print(F("deserializeJson() failed: "));
      //Serial.print(error.f_str());
      //Serial.print("\n\r");
      return;
    }
    
   /* if(rec["action"]=="view"){
      serializeJson(doc,Serial);
    }*/

  
    /*else if(rec["action"]=="modify"){
      float voutin=rec["switch"]["Vout"];
      if (!checkVout(voutin)){
        sendError("Invalid Vout!");
        return;
      }
      String switch_name=rec["switch"]["name"];
      JsonObject s=doc[switch_name].to<JsonObject>();
      doc[switch_name]["Vout"]=voutin;
      serializeJson(s,Serial);
    }
    else{
      // Important aquí també escriure "" i no ''
      sendError("Invalid action!");
      return;
    }
    Serial.print('\r');*/

  }
  else{
    //Serial.print("Waiting for you to send me something...");
  }
  //isSteady(s1);

  switchRW(s1);
  delay(1000);
  //isSteady(s1);
}

void switchRW(JsonObject s) {
  unsigned long tmax=millis(),t=millis();
  float idac=0;
  while(s["ready"]!="done"){
    // 1s
    if(millis()-t> 1000){
      Serial.flush();
      serializeJson(s,Serial);
      Serial.print('\r'); 
      t=millis();
    }
    // 10s
    if(millis()-tmax> 10000){
      s["ready"]="aborted";
      s["Vout"]=5;
      return;
    }
    // Read the ADC voltage
    float adcread=readADC(ADC_ADDRESS);  
    float Vout=s["Vout"];
    
    s["adcread"]=adcread;
    
    // Compute the new IDAC and send/write it to the DAC
    idac=idac-SPEED_IDAC*STEP_IDAC*round(((adcread-Vout)/R1)/STEP_IDAC);
    if(idac>IDAC_MAX){
      idac=IDAC_MAX;
    }
    if(idac<IDAC_MIN){
      idac=IDAC_MIN;
    }
  
    writeDAC(byte(idac*POS_IDAC/IFS));
  }
}

void isSteady(JsonObject obj){
  String ready;
  float Vout=obj["Vout"];
  float adcread=obj["adcread"];
  for(int i=0;i<3;i++){
    if(abs(Vout-adcread)<0.25){
      ready="done";
    }
    else{
      ready="modifying";
    }
    delay(50);
  }
  obj["ready"]=ready;
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