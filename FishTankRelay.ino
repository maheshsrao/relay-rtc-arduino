///////////////////////////////////////////////////////////////////////////////////////
//Terms of use
///////////////////////////////////////////////////////////////////////////////////////
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.
///////////////////////////////////////////////////////////////////////////////////////

/*
    FishTankRelay
    made by
    Mahesh S Rao ( mahesh.srao@gmail.com ) 27-09-2018
*/

#include <DS3231.h>
//#define DEBUG
// ASCII chars for 0 & 1
#define OFF 48
#define ON  49

// Total number of schedules
#define NUMBER_OF_SCHEDULES  11

void switchon(void);
void switchoff(void);

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
Time  current_time;

int chour;
int cmin;

// Schedule Variables
int state;
int schedule_num;

int date_counter;
int mon_counter;
int year_counter;

struct times {
  int shour;
  int smin;
};

struct schedules {
  struct times on_sch;
  struct times off_sch;
};

// Schedule timings: {ON hour, ON Minute, OFF Hour, OFF Minute}
struct schedules stime[NUMBER_OF_SCHEDULES] = {
  {{7, 0}, {7, 5}},         //0.5
  {{7, 15}, {7, 45}},       //0.30
  {{8, 0}, {9, 15}},        //1.15
  {{9, 50}, {10, 10}},      //0.20
  {{11, 0}, {12, 0}},       //1.00
  {{13, 0}, {14, 0}},       //1.00 (1PM - 2PM)
  {{15, 45}, {16, 5}},      //0.20 (3:45PM - 4:05PM)
  {{16, 45}, {17, 0}},      //0.15 (4:45PM - 5PM)
  {{18, 0}, {18, 30}},      //0.30 (6PM - 6:30PM)
  {{19, 45}, {20, 0}},      //0.15 (7:45PM - 8PM)
  {{21, 0}, {22, 5}}        //1.30 (9PM - 10:05PM)
};

// Relay is connected to pins 5 and 6
int relay_1 = 5;
int relay_2 = 6;

// Bluetooth Command
char    command;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  rtc.begin();

  // pins for relay module set as output
  pinMode(relay_1, OUTPUT);
  digitalWrite(relay_1, HIGH);
  pinMode(relay_2, OUTPUT);
  digitalWrite(relay_2, HIGH);

  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(12, 13, 1);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(25, 9, 2019);   // Set the date to January 1st, 2014

  // Initial state is OFF.
  switchoff();

  current_time = rtc.getTime();
  chour = (int)current_time.hour;
  cmin = (int)current_time.min;

  date_counter = (int)current_time.date;
  mon_counter = (int)current_time.mon;
  year_counter = (int)current_time.year;
  schedule_num = 0;
  delay(5000);

  while (1) {
    if (schedule_num >= (NUMBER_OF_SCHEDULES - 1)) {
      schedule_num = (NUMBER_OF_SCHEDULES - 1);
      break;
    }
      
    if (chour < stime[schedule_num].off_sch.shour)
      break;
    else if (chour > stime[schedule_num].off_sch.shour) {
      schedule_num++;
      continue;
    }
    else if (cmin > stime[schedule_num].off_sch.smin) {
      schedule_num++;
      continue;
    }
    else 
      break;
  }

  // If System ON time falls within ON schedule - switchon.
  if (chour > stime[schedule_num].on_sch.shour && chour < stime[schedule_num].off_sch.shour)
    switchon();
  else if (chour == stime[schedule_num].off_sch.shour && cmin < stime[schedule_num].off_sch.smin)
    switchon();
  else if (chour == stime[schedule_num].on_sch.shour && cmin > stime[schedule_num].on_sch.smin)
    switchon();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void loop()
{
  if (Serial1.available() > 0)     // Checks whether data is comming from the serial port
    command = Serial1.read();      // Reads the data from the serial port

  // Get data from the DS3231
  current_time = rtc.getTime();
  chour = (int)current_time.hour;
  cmin = (int)current_time.min;

#ifdef DEBUG
  Serial.print("[Scheduled Number:");
  Serial.print(schedule_num);
  Serial.print("] [Lights: ");
  Serial.print(state);
  Serial.print("] [BL cmd: ");
  Serial.println(command);

  Serial.print("[ Current time: ");
  Serial.print(current_time.hour, DEC);
  Serial.print("] [hour(s), ");
  Serial.print(current_time.min, DEC);
  Serial.print(" minute(s)] [");
  Serial.print(current_time.sec, DEC);
  Serial.println(" second(s) ]");
  Serial.println("---------------------");
#endif

  if (chour == stime[schedule_num].on_sch.shour && cmin == stime[schedule_num].on_sch.smin)
    switchon();
  else if (chour == stime[schedule_num].off_sch.shour && cmin == stime[schedule_num].off_sch.smin) {
    switchoff();
    // One schedule is compleated on OFF sequence.
    if (schedule_num < (NUMBER_OF_SCHEDULES - 1))
        schedule_num++; 
  }

  if (command == ON) {
    command = 0;
    switchon();
  }
  else if (command == OFF) {
    command = 0;
    switchoff();
  }

  // New Day/Month/Year Start over.
  if (current_time.date > date_counter || current_time.mon > mon_counter || current_time.year > year_counter) {
    schedule_num = 0;
    date_counter = (int)current_time.date;
    mon_counter = (int)current_time.mon;
    year_counter = (int)current_time.year;
  }

  // Wait 5 second before repeating :)
  delay (5000);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void switchon(void) {
  if (state != ON) {
#ifdef DEBUG
    Serial.println("Lights should be turned on");
#endif
    // Switch on the relay and update the state of relay
    digitalWrite(relay_2, LOW);
    state = ON;
  }
  return;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void switchoff(void) {
  if (state != OFF) {
#ifdef DEBUG
    Serial.println("Lights should be turned off");
#endif
    // Switch off the relay and update the state of relay
    digitalWrite(relay_2, HIGH);
    state = OFF;
  }
  return;
}
