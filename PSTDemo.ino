#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>       
#include <SoftwareSerial.h>

#define USE_SSL true

#define FIREBASE_HOST "https://ews-banjir-307e6-default-rtdb.firebaseio.com/"      
#define FIREBASE_AUTH "M4JCiaWtWchDdsXBKTa7Cp00Y6VMH3LivvbODCZk"  

FirebaseData firebaseData;
FirebaseJson json;

const char* ssid = "ssid";
const char* password = "pass";
static const int relay = 16, trigPin = 4, echoPin = 5;
long duration;
int distance;
int tinggi = 25;
int pantul;
String status;

void post_to_firebase(String data);
boolean waitResponse(String expected_answer="OK", unsigned int timeout=2000);

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relay,OUTPUT);
  WiFi.begin(ssid, password);
  if (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
     Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}
void loop() {
  ultra(500);
  displaydata();
}

static void ultra(unsigned long ms)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  pantul = duration*0.034/2;
  distance= tinggi-pantul;
  
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" cm");
}

void print_ok()
{
    Serial.println("------------------------------------");
    Serial.println("OK");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
}

void print_fail()
{
    Serial.println("------------------------------------");
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
}

void displaydata()
{
  if(Firebase.setFloat(firebaseData, "/Peringatanbanjir/Distance", distance))
    {
    print_ok();
    if(distance >= 6 )
      {
        status = "BAHAYA!!!";
        digitalWrite(relay,LOW);
        Firebase.setString(firebaseData, "/Peringatanbanjir/Condition", status);
        print_ok();
      }

    else if ((distance >= 4 )&&(distance <=5 ))
      {
        status = "WASPADA!!";
        digitalWrite(relay,HIGH);
        Firebase.setString(firebaseData, "/Peringatanbanjir/Condition", status);
      }

    else if ((distance >= 2)&&(distance <= 3))
      {
        status = "SIAGA!";
        digitalWrite(relay,HIGH);
        Firebase.setString(firebaseData, "/Peringatanbanjir/Condition", status);
      }
      
    else if (distance <= 1)
      {
        status = "AMAN";
        digitalWrite(relay,HIGH);
        Firebase.setString(firebaseData, "/Peringatanbanjir/Condition", status);
      }
      
    else
      {
      print_fail();
      }
    }
  delay(1000);
}
