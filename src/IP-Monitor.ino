#define MON_LIGHT 10
#define MON_BTN A1
#define MON_LED A2

#define MB_LED  11
#define MB_BTN  12
#define MB_RES   6

#define FAN   9
#define TEMP A0

#define MON_START 0
#define MON_OFF 1
#define MON_SLEEP 2
#define MON_RUN 3
#define MON_CBTN 4
#define MON_HZ 100

#define MB_START 0
#define MB_OFF 1
#define MB_ON 2
#define MB_HZ 100

#define MIN_FAN 127
#define MAX_FAN 0
#define OFF_FAN 255

#define MIN_TEMP 600
#define MAX_TEMP 660

#define MAX_TIME 4294967295

#define START_DELAY 500
#define MON_BTN_DELAY 1000
#define MON_OFF_DELAY 3000

bool is_mon_light(){
  pinMode(MON_LIGHT, INPUT_PULLUP);
  return (!digitalRead(MON_LIGHT));  
}
bool is_mon_btn(){
  pinMode(MON_BTN, INPUT_PULLUP);
  return (!digitalRead(MON_BTN));  
}
bool is_mon_led(){
  pinMode(MON_LED, INPUT_PULLUP);
  return (!digitalRead(MON_LED));  
}
bool is_mb_led(){
  pinMode(MB_LED, INPUT_PULLUP);
  return (!digitalRead(MB_LED));
}
bool is_mb_res(){
  pinMode(MB_RES, INPUT_PULLUP);
  return (!digitalRead(MB_RES));
}

uint16_t cur_temp(){
  pinMode(TEMP, INPUT);
  uint16_t t = analogRead(TEMP);
  return t;
}
uint8_t percentage_fan(uint8_t percent){
  return ( MIN_FAN - abs(MAX_FAN - MIN_FAN)*percent/100 );
}
uint8_t fan_from_temp(uint16_t temp){
  return ( MIN_FAN - (temp - MIN_TEMP)*(abs(MAX_FAN-MIN_FAN)-1)/(abs(MAX_TEMP-MIN_TEMP)) );
}
void set_fan(uint8_t pwm){
  pinMode(FAN, OUTPUT);
  analogWrite(FAN, pwm);
}
void do_fan(){
  uint16_t temp = cur_temp();
  if (temp > MAX_TEMP) set_fan(MAX_FAN); 
  else if (temp < MIN_TEMP) set_fan(MIN_FAN);
  else set_fan(fan_from_temp( temp ));
}

void mb_btn_up(){
  pinMode(MB_BTN,  OUTPUT);
  digitalWrite(MB_BTN, HIGH);
}
void mb_btn_down(){
  pinMode(MB_BTN,  OUTPUT);
  digitalWrite(MB_BTN, LOW);
}

uint16_t get_temp(){
  pinMode(TEMP, INPUT);
  return (analogRead(TEMP));
}

uint16_t b_t;
uint16_t res;

uint8_t mon_state=MON_START;
uint8_t prev_mon_state=0;

uint8_t mb_state=MB_START;
uint8_t prev_mb_state=0;

bool is_on = false;

void setup() {
  set_fan(0);
  Serial.begin(115200);
  b_t = millis();
}

void loop() {
  switch (mon_state) {
    case MON_START:
          if(!is_mon_btn()){
            if (is_mon_light()){
              mon_state = MON_RUN;
            }
            else{
              if(is_mon_led()){
                mon_state = MON_SLEEP;
                b_t = millis();
              }
              else{
                res = millis()-b_t;
                if(res > MON_OFF_DELAY){
                  mon_state = MON_OFF;
                }
              }
            }
          }
          else {
            prev_mon_state = mon_state;
            mon_state = MON_CBTN;
          }
          break;
    case MON_OFF:
          if(!is_mon_btn()){
            if (is_mon_light()){
              mon_state = MON_RUN;
            }
            else{
              if(is_mon_led()){
                mon_state = MON_SLEEP;
                b_t = millis();
              }
            }
          }
          else {
            prev_mon_state = mon_state;
            mon_state = MON_CBTN;
          }
          break;
    case MON_SLEEP:
          if(!is_mon_btn()){
            if(is_mon_light()){
              mon_state = MON_RUN;
            }
            else{
              if(is_mon_led()){
                b_t = millis();
              }
              else{
                res = millis()-b_t;
                if(res > MON_OFF_DELAY){
                  mon_state = MON_OFF;
                }
              }
            }
          }
          else{
            prev_mon_state = mon_state;
            mon_state = MON_CBTN;
          }
          break;
    case MON_CBTN:
          if(!is_mon_btn()){
            mon_state = prev_mon_state;
          } 
          else{
            if(prev_mon_state==MON_RUN){
              is_on = false;
            }
            else{
              is_on = true;
            }
          }
          break;       
    case MON_RUN:
          if(!is_mon_btn()){
            if(!is_mon_light()){
              mon_state = MON_SLEEP;
              b_t = millis();
            }
          }
          else{
            prev_mon_state = mon_state;
            mon_state = MON_CBTN;
          }
          break;
    case MON_HZ:
          break;
    default:
        mon_state=MON_HZ;
  }

  switch (mb_state) {
    case MB_START:
          if( is_mb_led() ){
            mb_state = MB_ON;
          }
          else {
            mb_state = MB_OFF;
          }
          break;
    case MB_OFF:
          if( is_mb_led() ){
            mb_state = MB_ON;
          }
          break;
    case MB_ON:
          if( !is_mb_led() ){
            mb_state = MB_OFF;
          }
          break;
    case MB_HZ:
          break;
    default:
        mb_state=MB_HZ;
  }
  
  if(mb_state==MB_OFF){
    if(mon_state==MON_CBTN && is_on){
      mb_btn_down();
    }
    else{
      mb_btn_up();
    }
  }
  else{
    do_fan();
    is_on = false;
    if(is_mb_res()){
      mb_btn_down();
    }
    else{
      mb_btn_up();
    }
  }
}




