/*
weather_station.ino
Michael Morrissey
16 Dec 2023

Minimal weather station based around hobbiest-grade sensors and cadalogger mini logging platform.

https://github.com/mbmorrissey/minimalweatherstation
*/

#include "cadalogger.h"
cadalogger weather(CADALOGGERMINI);

#define tempHumSensor 0x44     // I2C address of temperature and humidity sensor

#define RTC_pin 22             // hard-wired pin assignment in cadalogger mini
#define anem_pin 18            // D14/A2 and D18/A6 are fully asynchronous
#define raga_pin 14            // D14/A2 and D18/A6 are fully asynchronous
#define wind_vane_pin A4       // wind vane in voltage divider measured on this pin
#define wind_vane_power_pin 17 // ground switch pin for wind vane voltage divider

#define write_interval 12      // number of 5s intervals between data writes

File file;                     // file object to record events
char df[80];                   // data file name (to include date)

byte x;                        // counter for intervals between data writes

int wind_pulses[write_interval]; // store pulses over 5s interval to calculate mean and max every minute

double wind_sin_sum;           // sum sin and cos of win direction over each interval
double wind_cos_sum;           // to calculate circular mean

int anem;       // anemometer pulses over every 5s interval
int raga;       // rain gauge pulses over every data recording (minute) interval
byte RTC_int;   // whether RTC fired interrupt that woke weather station (counts 5s interval)


void setup() {

  // initialise with 5s RTC wake interval watchdog timer (default 8.1s)
  weather.initialise(5);  
  wind.enable_watchdog();

  // necessary initial values of counter variables
  anem=0;
  raga=0;
  RTC_int=0;
  x=0;

  // set up low-side switch pin for wind vane and turn off
  pinMode(wind_vane_power_pin,OUTPUT);
  digitalWrite(wind_vane_power_pin, HIGH);
  
  // probably redundant with setting of all pints to INPUT_PULLUP in cadalogger::initialise()
  pinMode(anem_pin,INPUT_PULLUP);
  pinMode(raga_pin,INPUT_PULLUP);
  pinMode(RTC_pin,INPUT_PULLUP);

  weather.feed_watchdog();


  // attach interrupts; probably don't need to repeatedly enable and disable interrupts to do so
  cli();  
  attachInterrupt(digitalPinToInterrupt(anem_pin), anem_interrupt, FALLING);
  sei();
  cli(); 
  attachInterrupt(digitalPinToInterrupt(raga_pin), raga_interrupt, FALLING);
  sei();
  cli(); 
  attachInterrupt(digitalPinToInterrupt(RTC_pin), RTC_interrupt, FALLING);
  sei();

}
  

void loop() {

  if(RTC_int==1){

    // reset watchdog timer
    weather.feed_watchdog();

    // flash led for heartbeat and re-sent RTC interrupt indicator variable
    weather.flash(1);
    RTC_int=0;

    // store anemometer pulse count for subsequent calculation of mean and max
    wind_pulses[x] = anem;
    anem=0;

    // record wind direction and store sin and cos for subsequent calculation of circular mean
    int wind_direct = read_wind_direction();
    wind_sin_sum = wind_sin_sum + sin(((double) wind_direct)*3.14159/180.0);
    wind_cos_sum = wind_cos_sum + cos(((double) wind_direct)*3.14159/180.0);

    x++; // increment counter for number of 5s intervals
  }

  if(x==write_interval){

    // calculate mean and max wind speed in units of anemometer pulses
    float mean_wind = 0;
    int max_wind = 0;
    for(int i=0; i<write_interval;i++){
      mean_wind = mean_wind + wind_pulses[i]/((double) write_interval);
      max_wind = max(max_wind,wind_pulses[i]);
    }

    // calculate circular mean from saved sin and cos sums of wind vane direction
    float circ_wind_mean = atan2( (double) (wind_sin_sum)/((double) write_interval) , (double) (wind_cos_sum)/((double) write_interval) );        
    wind_sin_sum=0;
    wind_cos_sum=0;
    
    // read temperature and humidity from sensor
    double tempHum[2];
    read_temp_humid(tempHum);
    double temperature= tempHum[0];
    double humidity = tempHum[1];

    // write minute summary weather data to SD card   
    weather.power_up_sd();
    weather.update_time();
    sprintf(df, "weather_%d_%d.csv", weather.time[3], weather.time[4]);
    file = weather.SD.open(df, FILE_WRITE);

    write_time();
    file.print(',');
    file.print(mean_wind);
    file.print(',');
    file.print(max_wind);
    file.print(',');
    file.print(circ_wind_mean*180.0/3.14159);
    file.print(',');
    file.print(raga);
    file.print(',');
    file.print(temperature);
    file.print(',');
    file.println(humidity);
    file.flush();
    file.close();
    weather.power_down_sd();

    // re-set relevant counters
    x=0;
    raga=0;
  }

  // go to sleep until woken by anemometer, rain gauge, or 5 second RTC wake
  go_to_sleep();

}

// sleep mode while waiting for anemometer, rain gauge or RTC interrupt
void go_to_sleep(){
  cli();
  set_sleep_mode(SLEEP_MODE_STANDBY);  //  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sei();
  sleep_cpu(); 
  // sleeps here until the interrupt V falls from anemometer, rain gauge or RTC
  sleep_disable();
}


// ISRs for anemometer and rain gauge interrupts
static void anem_interrupt(){
  anem++;
}

static void raga_interrupt(){
  raga++;
}

// ISR for RTC 5s interrupt
static void RTC_interrupt(){
  RTC_int = 1;
}

void read_temp_humid(double *data){
  // read temperature and humidity from SHT31 sensor
  // datasheet: https://sensirion.com/media/documents/213E6A3B/63A5A569/Datasheet_SHT3x_DIS.pdf

  //  useage:
  //  double tempHum[2];
  //  read_temp_humid(tempHum);
  //  temperature= tempHum[0];
  //  humidity = tempHum[1];

  Wire.beginTransmission(tempHumSensor);
  if(Wire.endTransmission()==0){
    Wire.beginTransmission(tempHumSensor);
    Wire.write(0x24);  // clock stretching disabled
    Wire.write(0x0B);  // medium repeatability
    Wire.endTransmission(); 
    // to to wait for conversion - could possibly make this more efficient using the clock 
    // stretching option (page 10 of datasheet). From p7 of datasheet, max measurement duration
    // is 6 ms at medium repeatability and 15 ms at high repeatability
    delay(20);
    byte buffer[6];
    Wire.requestFrom(0x44, 6);
    for(int i = 0; i<6; i++) buffer[i] = Wire.read();  
    Wire.endTransmission();  

    // bytes 2 and 5 are checksum bytes - could work out how to check them - there is a function
    // within the ADS1X15 library that looks pretty straightforward and relates well to the 
    // datasheet
    uint16_t rawTemperature = (buffer[0] << 8) + buffer[1];
    uint16_t rawHumidity    = (buffer[3] << 8) + buffer[4];

    // conversions as given on page 13 of datasheet
    data[0] = rawTemperature * (175.0 / 65535) - 45;
    data[1] = rawHumidity    * (100.0 / 65535);    
  } else{
    data[0] = -50;
    data[1] = 0;  
  }
}


int read_wind_direction(){
// returns wind direction as an integer angle in degrees
// assumes that wind vane is the first resistor in a voltage divider where the second is 100k Ohms

// resistance between black and green wires
// W  120k         465
// NW 64.9k        621
// N  43k          770
// SW 16k          883
// NE 8.2k         947
// S  3.9k         986
// SE 2.2k         1002
// E  1k           1014

  digitalWrite(wind_vane_power_pin, LOW);
  pinMode(wind_vane_pin, INPUT);
  delay(2);
  int val = analogRead(wind_vane_pin);
  pinMode(wind_vane_pin, INPUT_PULLUP);
  digitalWrite(wind_vane_power_pin, HIGH);

  
  int windDir = -99;
  switch(val){
    case 0 ... 520:        // W
      windDir = 270; 
      break;
    case 521 ... 700:      // NW
      windDir = 315; 
      break;
    case 701 ... 820:      // N
      windDir = 0; 
      break;
    case 821 ... 910:      // SW
      windDir = 225;
      break;
    case 911 ... 965:      // NE
      windDir = 45;
      break;
    case 966 ... 995:      // S
      windDir = 180;
      break;
    case 996 ... 1009:     // SE
      windDir = 135; 
      break;
    case 1010 ... 1023:    // E
      windDir = 90; 
      break;
  }

  return windDir;
}


// function to write current time to data file
void write_time() {
  file.print(weather.time[0]);
  file.print(",");
  file.print(weather.time[1]);
  file.print(",");
  file.print(weather.time[2]);
  file.print(",");
  file.print(weather.time[3]);
  file.print(",");
  file.print(weather.time[4]);
  file.print(",");
  file.print(weather.time[5]);
}
