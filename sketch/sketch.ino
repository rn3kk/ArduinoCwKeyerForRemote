
#include <EEPROM.h>

const int EE_ADR_WPM = 1;

const int out_cw = 8;
const int dashPin = 5; 
const int ditPin = 3; 
const int ledPin = 13; 

int wpm = 24;

int dot_len;
int dash_len = dot_len*3;

const char DASH = '-';
const char DOT = '.';
const char CHANR_SEPARATOR = ']';
const char WORD_SEPARATOR = ' ';

unsigned long last_dash_or_dot_time_send = millis();
int count_empty_separator_sended = 0;

void setup() {
  pinMode(ledPin, OUTPUT);  
  pinMode(out_cw, OUTPUT);  

  pinMode(dashPin, INPUT_PULLUP);   
  pinMode(ditPin, INPUT_PULLUP);

  wpm = EEPROM.read(EE_ADR_WPM);
  dot_len = 1.2 / wpm*1000;
  dash_len = dot_len*3;
  Serial.begin(9600);
  send_params_to_serial("wpm", wpm);
  send_params_to_serial("dot", dot_len);
  send_params_to_serial("dash", dash_len);
}

void loop(){
  if (Serial.available() > 0)
  {
    String str = Serial.readString();       
    if(str.indexOf("get") >= 0)
    {
      send_params_to_serial("wpm", wpm);
      send_params_to_serial("dot", dot_len);
      send_params_to_serial("dash", dash_len);
    }
    else if(str.indexOf("wpm") >= 0){
      int i = str.indexOf("wpm");
      wpm = str.substring(i+3, i+3+2).toInt();
      if (wpm == 0)
      {
        wpm = 10;
      }
      dot_len = 1.2 / float(wpm)*1000;
      dash_len = dot_len*3;
      EEPROM.write(EE_ADR_WPM, wpm);
      send_params_to_serial("wpm", wpm);
      send_params_to_serial("dot", dot_len);
      send_params_to_serial("dash", dash_len);
    }
  }
  else if (digitalRead(dashPin) == LOW) //Тире
  {
    Serial.print(DASH);
    Serial.flush();
    last_dash_or_dot_time_send = millis();
    count_empty_separator_sended = 0;

    digitalWrite(out_cw, HIGH);
    delay(3*dot_len);
    digitalWrite(out_cw, LOW);
    delay(dot_len);

  }
  else if (digitalRead(ditPin) == LOW) //Тире
  {
    Serial.print(DOT);
    Serial.flush();
    last_dash_or_dot_time_send = millis();
    count_empty_separator_sended = 0;

    digitalWrite(out_cw, HIGH);
    delay(dot_len);
    digitalWrite(out_cw, LOW);
    delay(dot_len);
  }
  else
  {    
    if(millis() - last_dash_or_dot_time_send > dash_len && count_empty_separator_sended < 3)
    {
      Serial.print(CHANR_SEPARATOR);
      last_dash_or_dot_time_send = millis();
      count_empty_separator_sended ++;      
    }
  }
}

void send_params_to_serial(String cmd, int value)
{
  Serial.print(cmd);
  Serial.print(value);
  Serial.print("\n");
  Serial.flush();
}
