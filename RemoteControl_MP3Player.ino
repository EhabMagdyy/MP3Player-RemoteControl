/*****************************************************************************************************************************
**********************************    Author  : Ehab Magdy Abdullah                      *************************************
**********************************    Linkedin: https://www.linkedin.com/in/ehabmagdyy/  *************************************
**********************************    Youtube : https://www.youtube.com/@EhabMagdyy      *************************************
******************************************************************************************************************************/

#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <IRremote.hpp>

#define IR_RECEIVER_PIN         22
#define MP3_TX_PIN              17
#define MP3_RX_PIN              16
#define POTENTIOMETER_PIN       4

#define NO_OF_AUDIO             4

unsigned char audioNumber = 1;
unsigned char isPaused = 0;
unsigned int ADCReadCounter = 0;
unsigned int analogValue = 0;
unsigned char volume = 0;

SoftwareSerial softwareSerial(MP3_RX_PIN, MP3_TX_PIN);

DFRobotDFPlayerMini player;

void setup()
{
    Serial.begin(115200);
    softwareSerial.begin(9600);
    player.begin(softwareSerial);
    IrReceiver.begin(IR_RECEIVER_PIN, ENABLE_LED_FEEDBACK);   // Start IR Receiver, Enable IRreceiver led
    player.volume(20);
    player.play(1);
}

void loop() 
{
    ADCReadCounter++;
    /* Reading Potentiometer analog value to set the volume */
    if(ADCReadCounter > 1000)
    {
        analogValue = analogRead(POTENTIOMETER_PIN);
        volume = map(analogValue, 0, 4095, 0, 30);      /* Speed Range: 0 -> 30 */
        player.volume(volume);
        ADCReadCounter = 0;
    }
    /* if current played audio is finished, play it again */
    if(DFPlayerPlayFinished == player.readType())
    {
        player.play(audioNumber);
    }

    if (IrReceiver.decode())        // Check if there is a received code
    {
        Serial.print("Received Data: ");
        Serial.print(IrReceiver.decodedIRData.decodedRawData, HEX);  // Print Received code on Serial Monitor in Hexadecimal
        Serial.print("  -  Received Command: ");
        Serial.println(IrReceiver.decodedIRData.command, HEX);  // Print Received command on Serial Monitor in Hexadecimal

        /* play the next audio */
        if(IrReceiver.decodedIRData.command == 0x1){        // if you press a button with command number '1'
            /* You can use the function player.previous(). i used this to play just 4 specific audios from SD card */
            audioNumber = ((audioNumber + 1) > NO_OF_AUDIO)? 1 : (audioNumber + 1);
            player.play(audioNumber);
        }

        /* Pause/Resume next audio */
        else if(IrReceiver.decodedIRData.command == 0x2){   // if you press a button with command number '2'
            if(isPaused)
            {
                player.start();
                isPaused = 0;
            }
            else
            {
                player.pause();
                isPaused = 1;
            }
        }

        /* play the previous audio */
        else if(IrReceiver.decodedIRData.command == 0x3){   // if you press a button with command number '3'
            /* You can use the function player.previous(). i used this to play just 43 specific audios from SD card */
            audioNumber = ((audioNumber - 1) == 0)? NO_OF_AUDIO : (audioNumber - 1); 
            player.play(audioNumber);
        }

        delay(1000);
        IrReceiver.resume();       // Enable receiving of the next value
        ADCReadCounter = 1001;
    }
}
