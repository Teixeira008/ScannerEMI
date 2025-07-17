
#include <Arduino.h>
                

#define stepY 3 // y3
#define dirY  6// y6

#define stepX 2 // x
#define dirX 5 // x

#define stepZ 4 // z4
#define dirZ 7 // z7

#define stepA 12 // a
#define dirA 13 // a

#define enPin 8

#define fimdcursoY 10
#define fimdcursoX 9
#define fimdcursoZ 11






void passoY(int direcao, int velocidade)
{       
    digitalWrite(dirY,direcao);
    digitalWrite(stepY,HIGH);
    delayMicroseconds(velocidade);
    digitalWrite(stepY,LOW);
    delayMicroseconds(velocidade);
}
void passoX(int direcao, int velocidade ){
    digitalWrite(dirX,direcao);
    digitalWrite(stepX,HIGH);
    delayMicroseconds(velocidade);
    digitalWrite(stepX, LOW);
    delayMicroseconds(velocidade);

}
void passoZ(int direcao, int velocidade ){
    digitalWrite(dirZ,direcao);
    digitalWrite(stepZ,HIGH);
    delayMicroseconds(velocidade);
    digitalWrite(stepZ, LOW);
    delayMicroseconds(velocidade);

    digitalWrite(dirA, direcao);
    digitalWrite(stepA, HIGH);
    delayMicroseconds(velocidade);
    digitalWrite(stepA, LOW);
    delayMicroseconds(velocidade);

}

void zeraY(void)
{
    while(digitalRead(fimdcursoY) == LOW)
    {
        passoY(1,1000);
    }

    for(int y = 0; y < 50; y++)
    {
        passoY(0,1000);
    }

    while(digitalRead(fimdcursoY) == LOW)
    {
        passoY(1,30000);
    }
}

void zeraX(void){
  while (digitalRead(fimdcursoX) == LOW)
  {
    passoX(0, 500);
  }
  for(int x = 0; x < 50; x++){
    passoX(1, 500);
  }
  while (digitalRead(fimdcursoX) == LOW)
  {
    passoX(0, 30000);
  }
  
}
void zeraZ(void){
  while (digitalRead(fimdcursoZ) == LOW)
  {
    passoZ(1, 200);
    for(int z; z < 50; z++){
      passoZ(0, 200);
    }
    while (digitalRead(fimdcursoZ) == LOW)
    {
      passoZ(1, 20000);
    }
    
  }
  

}


 void setup() {
 
 pinMode(stepY,OUTPUT); 
 pinMode(dirY,OUTPUT);

 pinMode(stepX,OUTPUT); 
 pinMode(dirX,OUTPUT);

 pinMode(stepZ,OUTPUT); 
 pinMode(dirZ,OUTPUT);
 
 pinMode(stepA,OUTPUT); 
 pinMode(dirA,OUTPUT);

 pinMode(enPin, OUTPUT);

 digitalWrite(enPin, LOW);

 pinMode(fimdcursoY, INPUT_PULLUP);
 pinMode(fimdcursoX, INPUT_PULLUP);
 pinMode(fimdcursoZ, INPUT_PULLUP);
 

 delay(100);
 zeraY();
 zeraX();

 Serial.begin(9600);
 }


  
 void loop() {

  /*byte leitura;
  if(Serial.available()){
    leitura = Serial.read();
    Serial.println(leitura);

 }*/
 }