
#include <Arduino.h>
                

#define stepY 3 // y
#define dirY  6// y

#define stepX 2 // x
#define dirX 5 // x

#define stepZ 4 // z
#define dirZ 7 // z

#define stepA 12 // a
#define dirA 13 // a

#define enPin 8

#define fimdcursoY 10
#define fimdcursoX 9
#define fimdcursoZ 11



void passo(int direcao, int velocidade)
{       
    digitalWrite(dirY,direcao);
    digitalWrite(stepY,HIGH);
    delayMicroseconds(velocidade);
    digitalWrite(stepY,LOW);
    delayMicroseconds(velocidade);
}

void zeraY(void)
{
    while(digitalRead(fimdcursoY) == LOW)
    {
        passo(1,500);
    }

    for(int x = 0; x < 50; x++)
    {
        passo(0,500);
    }

    while(digitalRead(fimdcursoY) == LOW)
    {
        passo(1,30000);
    }

}
 void setup() {
 
 pinMode(stepY,OUTPUT); 
 pinMode(dirY,OUTPUT);

 pinMode(enPin, OUTPUT);
 digitalWrite(enPin, LOW);

 pinMode(fimdcursoY, INPUT_PULLUP);

 delay(100);
 zeraY();
 
 }

 void loop() {


 }