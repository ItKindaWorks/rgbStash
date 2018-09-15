#include <ESP8266WiFi.h>

#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"
#include "Metro.h"

// How many leds are in the strip?
#define NUM_LEDS 24
CRGB mainRoof[NUM_LEDS];
CRGB mainFloor[NUM_LEDS];
CRGB upperRoof[NUM_LEDS];
CRGB mainSide[NUM_LEDS];

CRGB mainSideNext[NUM_LEDS];


const int MAX_ACTIVE = 5;
uint8_t activeLights[MAX_ACTIVE];

const int MAX_BRIGHTNESS = 255;

Metro mainFadeTimer = Metro(35);
Metro sideTwinkleTimer = Metro(15);

// Data pin that led data will be written out over
#define DATA_PIN 2

void setup() {

		//initially disconnect from wifi to prevent issues with ESP8266 ADC
    WiFi.disconnect();

		//start serial line for debugging
    Serial.begin(115200);

		//set the ADC pin to be an input

    FastLED.addLeds<WS2812B, D1, GRB>(mainRoof, NUM_LEDS);
    FastLED.addLeds<WS2812B, D3, GRB>(mainFloor, NUM_LEDS);
    FastLED.addLeds<WS2812B, D5, GRB>(upperRoof, NUM_LEDS);
    FastLED.addLeds<WS2812B, D7, GRB>(mainSide, NUM_LEDS);

    FastLED.show();

    delay(1000);

}

int currentVal = 0;

void loop(){

    if(mainFadeTimer.check()){
        if(currentVal == 255){currentVal = 0;}

        for(int led = 0; led < NUM_LEDS; led++){
            mainRoof[led] = CHSV(currentVal, 255, MAX_BRIGHTNESS);
            mainFloor[led] = CHSV(currentVal, 255, MAX_BRIGHTNESS);
            upperRoof[led] = CHSV(255 - currentVal, 255, MAX_BRIGHTNESS);
        }

        currentVal++;
    }

    if(sideTwinkleTimer.check()){
        sideTwinkle();
    }


    FastLED.show();
    yield();
}





void sideTwinkle(){
    for(int i = 0; i < MAX_ACTIVE; i++){
        //grab the current index of the light being accessed
        int ledIndex = activeLights[i];

        //light is off
        if(mainSide[ledIndex] == CRGB(0, 0, 0)){
            //generate new index for a light to control
            genNewIndex(i);

            //update ledIndex to match the newly generated value
            ledIndex = activeLights[i];

            //generate a random light value for the new LED
            mainSideNext[ledIndex] = CHSV(random(255), random(100, 255), random(100, 200));
        }

        //if the light has gotten to it's set value, reset back to 0
        else if(mainSide[ledIndex] == mainSideNext[ledIndex]){
            mainSideNext[ledIndex] = CHSV(0, 0, 0);
        }

        //update red value
        if(mainSide[ledIndex].red > mainSideNext[ledIndex].red){
            mainSide[ledIndex].red--;
        }
        else if(mainSide[ledIndex].red < mainSideNext[ledIndex].red){
            mainSide[ledIndex].red++;
        }

        //update blue value
        if(mainSide[ledIndex].blue > mainSideNext[ledIndex].blue){
            mainSide[ledIndex].blue--;
        }
        else if(mainSide[ledIndex].blue < mainSideNext[ledIndex].blue){
            mainSide[ledIndex].blue++;
        }

        //update green value
        if(mainSide[ledIndex].green > mainSideNext[ledIndex].green){
            mainSide[ledIndex].green--;
        }
        else if(mainSide[ledIndex].green < mainSideNext[ledIndex].green){
            mainSide[ledIndex].green++;
        }
    }
}


//finds an LED on the side panel that is not being
//used right now and sets the random index to the 
//activelight at the input index. (input index val is the activeLights index)
void genNewIndex(int index){
    int possibleIndex = 0;
    bool safeIndex = false;

    while(safeIndex == false){
        possibleIndex = random(25);
        safeIndex = true;
        for(int i = 0; i < MAX_ACTIVE; i++){
            if(activeLights[i] == possibleIndex){
                safeIndex = false;
            }
        }
    }

    activeLights[index] = possibleIndex;
}