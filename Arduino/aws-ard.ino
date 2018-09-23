#include <AWS_IOT.h>
#include <WiFi.h>
#include <Servo.h>

AWS_IOT hornbill;

char WIFI_SSID[]="Me";
char WIFI_PASSWORD[]="a1b2c3d4";
char HOST_ADDRESS[]="a2h0ivz19359nc.iot.us-east-1.amazonaws.com";
char CLIENT_ID[]= "myTestThing";
char TOPIC_NAME[]="CommChannel";
char TOPIC_SHADOW_GET[]= "$aws/things/myTestThing/shadow/get";
char TOPIC_SHADOW_GET_ACCEPTED[]= "$aws/things/myTestThing/shadow/get/accepted";
char TOPIC_SHADOW_UPDATE[]= "$aws/things/myTestThing/shadow/update";
char TOPIC_SHADOW_UPDATE_ACCEPTED[]= "$aws/things/myTestThing/shadow/update/accepted";

int status = WL_IDLE_STATUS;
int tick=0,msgCount=0,msgReceived = 0, hasFailed = 0;
char payload[512];
char rcvdPayload[512];

Servo myservo;

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
    strncpy(rcvdPayload,payLoad,payloadLen);
    rcvdPayload[payloadLen] = 0;
    msgReceived = 1;
}

void setup() {
    myservo.attach(13); 
    
    Serial.begin(115200);
    delay(2000);

    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(WIFI_SSID);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        // wait 5 seconds for connection
        delay(5000);
    }
    Serial.println("Connected to wifi");

    if(hornbill.connect(HOST_ADDRESS,CLIENT_ID)== 0)
    {
        Serial.println("Connected to AWS");
        delay(1000);

//        if(0==hornbill.subscribe(TOPIC_NAME,mySubCallBackHandler) && 0==hornbill.subscribe(TOPIC_SHADOW_GET,mySubCallBackHandler) && 0==hornbill.subscribe(TOPIC_SHADOW_GET_ACCEPTED,mySubCallBackHandler))
        if(0==hornbill.subscribe(TOPIC_SHADOW_UPDATE,mySubCallBackHandler))
        {
            Serial.println("Subscribe Successfull");
        }
        else
        {
            Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
            while(1);
        }
    }
    else
    {
        Serial.println("AWS connection failed, Check the HOST Address");
        while(1);
    }

    delay(2000);

}

void loop() {

    if(msgReceived == 1)
    {
        msgReceived = 0;
        Serial.print("Received:\t");
        Serial.println(rcvdPayload);
        String str(rcvdPayload);

        if(str.indexOf("\"action\" : \"OpenDoor\"") > 0){
          Serial.println("Command: Open");
          myservo.write(100);
        }
        else if (str.indexOf("\"action\" : \"CloseDoor\"") > 0){
          Serial.println("Command: Close");
          myservo.write(0);
        }
    }

//    if(tick >= 5)   // publish to topic every 5 seconds
//    {
//        tick = 0;
//        sprintf(payload,"ESP32: \t%d",msgCount);
//
//        hasFailed = hornbill.publish(TOPIC_NAME,payload);
//        if(hasFailed == 0)
//        {
//            Serial.print("Publishing:\t");
//            Serial.println(payload);
//            msgCount++;
//        }
//        else
//        {
//            Serial.print("Resending:\t");
//            Serial.println(payload);
//        }
//    }
    vTaskDelay(1000 / portTICK_RATE_MS);
    tick++;
}
