
#include <EEPROM.h>

const int EE_ADR_WPM = 1;

const int out_cw = 8;
const int dashPin = 5; 
const int dotPin = 3; 
const int ledPin = 13; 

int wpm = 24;

int dot_len;
int dash_len = dot_len*3;

int pause_beetween_char = dot_len * 3;
int pause_beetween_word = dot_len * 6;

const char DASH = '-';
const char DOT = '.';
const char CHANR_SEPARATOR = ']';
const char WORD_SEPARATOR = ' ';

String one_char = "";
bool end_of_char_sended = false;
unsigned long last_dash_or_dot_time = millis();

void setup() {
  pinMode(ledPin, OUTPUT);  
  pinMode(out_cw, OUTPUT);  

  pinMode(dashPin, INPUT_PULLUP);   
  pinMode(dotPin, INPUT_PULLUP);

  wpm = EEPROM.read(EE_ADR_WPM);
  calculate_timings(wpm);

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
      calculate_timings(wpm);
      EEPROM.write(EE_ADR_WPM, wpm);
      send_params_to_serial("wpm", wpm);
      send_params_to_serial("dot", dot_len);
      send_params_to_serial("dash", dash_len);
    }
  }

  if (digitalRead(dashPin) == LOW) //Тире
  {
    send_word_separator();
    one_char = one_char + "-";    
    end_of_char_sended = false;
    digitalWrite(out_cw, HIGH);
    delay(3*dot_len);
    digitalWrite(out_cw, LOW);
    last_dash_or_dot_time = millis();
    delay(dot_len);
  }

  if (digitalRead(dotPin) == LOW) //Тире
  {
    send_word_separator();
    one_char = one_char + ".";    
    end_of_char_sended = false;
    digitalWrite(out_cw, HIGH);
    delay(dot_len);
    digitalWrite(out_cw, LOW);
    last_dash_or_dot_time = millis();
    delay(dot_len);    
  }
  if((millis() - last_dash_or_dot_time) > dot_len*3 && !end_of_char_sended)
  {
    Serial.println(one_char+"]");
    Serial.flush();
    one_char = "";    
    end_of_char_sended = true;
  }
}

void send_word_separator()
{
  if (end_of_char_sended)
  {
    unsigned long paus = millis() - last_dash_or_dot_time;
    if ( paus >= pause_beetween_word )
    {
      one_char = one_char + " ";
      //Serial.println(' ');
      //Serial.flush();      
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

void calculate_timings(int wpm)
{
  dot_len = 1.2 / wpm*1000;
  dash_len = dot_len*3;
  pause_beetween_char = dot_len * 3;
  pause_beetween_word = dot_len * 6 ;
}
