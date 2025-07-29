#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#define USE_TIMER_1     true

#if ( defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)  || \
        defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI) ||    defined(ARDUINO_AVR_ETHERNET) || \
        defined(ARDUINO_AVR_FIO) || defined(ARDUINO_AVR_BT)   || defined(ARDUINO_AVR_LILYPAD) || defined(ARDUINO_AVR_PRO)      || \
        defined(ARDUINO_AVR_NG) || defined(ARDUINO_AVR_UNO_WIFI_DEV_ED) || defined(ARDUINO_AVR_DUEMILANOVE) || defined(ARDUINO_AVR_FEATHER328P) || \
        defined(ARDUINO_AVR_METRO) || defined(ARDUINO_AVR_PROTRINKET5) || defined(ARDUINO_AVR_PROTRINKET3) || defined(ARDUINO_AVR_PROTRINKET5FTDI) || \
        defined(ARDUINO_AVR_PROTRINKET3FTDI) )
  #define USE_TIMER_2     true
  #warning Using Timer1, Timer2
#else          
  #define USE_TIMER_3     true
  #warning Using Timer1, Timer3
#endif

#define TIMER_FREQ_HZ        4000.0

#include <Arduino.h>
#include <TimerInterrupt.h>


#define stepY 3 // y3
#define dirY  6 // y6

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

#define PDBG 0 // habilita prints de debug


struct{
  bool posicaoZerada;
  uint16_t XposicaoAtual;
  uint16_t YposicaoAtual;
  uint16_t ZposicaoAtual;
  uint16_t XposicaoDesejada;
  uint16_t YposicaoDesejada;
  uint16_t ZposicaoDesejada;
  uint8_t comando;
  unsigned long tUltimaPiscada;
  uint16_t tempoPiscada;


}motores;


void processoSerial();

bool verificaTempoPassado(uint16_t tempoEspera, unsigned long * tUltimaExecucao){
  unsigned long tAtual = millis();
  unsigned long tProxExecucao = *tUltimaExecucao + tempoEspera;
  if(tAtual > tProxExecucao){
    *tUltimaExecucao = tAtual;
    return 1;
  }
  return 0;
}


//TimerInterrupt timer1; 

volatile bool stopFlag = false; 

void meioPassoSoltoX(bool direcao) {
  static bool lastState = 0;
  digitalWrite(dirX, direcao);
  if(lastState){
    digitalWrite(stepX, LOW);
    lastState = 0;
  }
  else {
    digitalWrite(stepX, HIGH);
    lastState = 1;
  }
}

void meioPassoSoltoY(bool direcao) {
  static bool lastState = 0;
  digitalWrite(dirY, !direcao);
  if(lastState){
    digitalWrite(stepY, LOW);
    lastState = 0;
  }
  else {
    digitalWrite(stepY, HIGH);
    lastState = 1;
  }
}

void meioPassoSoltoZ(bool direcao) {
  static bool lastState = 0;
  digitalWrite(dirZ, direcao);
  digitalWrite(dirA, direcao);
  if(lastState){
    digitalWrite(stepZ, LOW);
    digitalWrite(stepA, LOW);
    lastState = 0;
  }
  else {
    digitalWrite(stepZ, HIGH);
    digitalWrite(stepA, HIGH);
    lastState = 1;
  }
}

void zeraSolto(bool forcaZera){
  static uint16_t xZerado = 600;
  static uint16_t yZerado = 600;
  static uint16_t zZerado = 600;

  if(forcaZera == 1){
    xZerado = 0;
    yZerado = 0;
    zZerado = 0;
    motores.posicaoZerada = 0;
  }

  if(motores.posicaoZerada == 0)
  {
    if(xZerado < 10000){
      if(xZerado == 0){
        if(digitalRead(fimdcursoX) == HIGH) xZerado++;
        else xZerado = 1000;
      }

      if((xZerado < 500)&&(xZerado > 0)){
          meioPassoSoltoX(1);
          xZerado++;
          if(xZerado == 500) xZerado = 1000;
      }

      if(xZerado == 1000){
        if(digitalRead(fimdcursoX) == LOW) meioPassoSoltoX(0);
        else xZerado = 2000;
      }

      if((xZerado >= 2000) && (xZerado < 3000)){
        meioPassoSoltoX(1);
        xZerado++;
        if(xZerado == 2600) xZerado = 3000;
      }

      if((xZerado >= 3000) && (xZerado < 9000)){
        if((xZerado%3) == 1){
          meioPassoSoltoX(0);
        }
        xZerado++;
        if(digitalRead(fimdcursoX) == HIGH){
          xZerado = 10000;
          motores.XposicaoAtual = 0;
          if(PDBG) Serial.println("X zerado");
        }
      }
    }
      
    if(yZerado < 10000){
      if(yZerado == 0){
        if(digitalRead(fimdcursoY) == HIGH) yZerado++;
        else yZerado = 1000;
      }

      if((yZerado < 500)&&(yZerado > 0)){
          meioPassoSoltoY(1);
          yZerado++;
          if(yZerado == 500) yZerado = 1000;
      }

      if(yZerado == 1000){
        if(digitalRead(fimdcursoY) == LOW) meioPassoSoltoY(0);
        else yZerado = 2000;
      }

      if((yZerado >= 2000) && (yZerado < 3000)){
        meioPassoSoltoY(1);
        yZerado++;
        if(yZerado == 2600) yZerado = 3000;
      }

      if((yZerado >= 3000) && (yZerado < 9000)){
        if((yZerado%3) == 1){
          meioPassoSoltoY(0);
        }
        yZerado++;
        if(digitalRead(fimdcursoY) == HIGH){
          yZerado = 10000;
          motores.YposicaoAtual = 0;
          if(PDBG) Serial.println("Y zerado");
        }
      }
    }

      
    if(zZerado < 10000){
      if(zZerado == 0){
        if(digitalRead(fimdcursoZ) == HIGH) zZerado++;
        else zZerado = 1000;
      }

      if((zZerado < 500)&&(zZerado > 0)){
          meioPassoSoltoZ(1);
          zZerado++;
          if(zZerado == 500) zZerado = 1000;
      }

      if(zZerado == 1000){
        if(digitalRead(fimdcursoZ) == LOW) meioPassoSoltoZ(0);
        else zZerado = 2000;
      }

      if((zZerado >= 2000) && (zZerado < 3000)){
        meioPassoSoltoZ(1);
        zZerado++;
        if(zZerado == 2600) zZerado = 3000;
      }

      if((zZerado >= 3000) && (zZerado < 9000)){
        if((zZerado%3) == 1){
          meioPassoSoltoZ(0);
        }
        zZerado++;
        if(digitalRead(fimdcursoZ) == HIGH){
          zZerado = 10000;
          motores.ZposicaoAtual = 0;
          if(PDBG) Serial.println("Z zerado");
        }
      }
    }

    if((xZerado == 10000)&&(yZerado == 10000)&&(zZerado == 10000)){
      motores.XposicaoDesejada = 0;
      motores.YposicaoDesejada = 0;
      motores.ZposicaoDesejada = 0;
      motores.posicaoZerada = 1;
      if(PDBG) Serial.println("todos os eixos zerados");
    } 
  }
}

void passoY(int direcao, int velocidade) {
    digitalWrite(dirY, direcao);
    digitalWrite(stepY, HIGH);
    delayMicroseconds(velocidade);
    digitalWrite(stepY, LOW);
    delayMicroseconds(velocidade);
}

void passoX(int direcao, int velocidade) {
    digitalWrite(dirX, direcao);
    digitalWrite(stepX, HIGH);
    delayMicroseconds(velocidade);
    digitalWrite(stepX, LOW);
    delayMicroseconds(velocidade);
}

void passoZ(int direcao, int velocidade) {
    digitalWrite(dirZ, direcao);
    digitalWrite(dirA, direcao);
    digitalWrite(stepZ, HIGH);
    digitalWrite(stepA, HIGH);
    delayMicroseconds(velocidade);
    digitalWrite(stepZ, LOW);
    digitalWrite(stepA, LOW);
    delayMicroseconds(velocidade);
}

void zeraY() {
    while (digitalRead(fimdcursoY) == LOW) {
        passoY(1, 2000);
    }

    for (int y = 0; y < 200; y++) {
        passoY(0, 2000);
    }

    while (digitalRead(fimdcursoY) == LOW) {
        passoY(1, 6000);
    }
}

void zeraX() {
    while (digitalRead(fimdcursoX) == LOW) {
        passoX(0, 2000);
    }
    for (int x = 0; x < 200; x++) {
        passoX(1, 2000);
    }
    while (digitalRead(fimdcursoX) == LOW) {
        passoX(0, 6000);
    }
}

void zeraZ() {
    while (digitalRead(fimdcursoZ) == LOW) {
        passoZ(0, 500);
    }
    for (int x = 0; x < 200; x++) {
        passoZ(1, 500);
    }
    while (digitalRead(fimdcursoZ) == LOW) {
        passoZ(0, 1500);
    }
}


void stopMotor() {
    stopFlag = true;  
}

void trataMotores(void)
{
  /*static uint8_t timeout = 0;
  timeout ++;
  if(timeout > 50){
    if(PDBG) Serial.print("dentro do processo MOTORES, tempo: ");
    if(PDBG) Serial.println(millis());
    timeout = 0;
  }*/
  if(motores.posicaoZerada){
    if(motores.XposicaoAtual < motores.XposicaoDesejada){
      meioPassoSoltoX(1);
      motores.XposicaoAtual ++;
    }
    else if(motores.XposicaoAtual > motores.XposicaoDesejada){
      meioPassoSoltoX(0);
      motores.XposicaoAtual --;
    }

    if(motores.YposicaoAtual < motores.YposicaoDesejada){
      meioPassoSoltoY(1);
      motores.YposicaoAtual ++;
    }
    else if(motores.YposicaoAtual > motores.YposicaoDesejada){
      meioPassoSoltoY(0);
      motores.YposicaoAtual --;
    }

    if(motores.ZposicaoAtual < motores.ZposicaoDesejada){
      meioPassoSoltoZ(1);
      motores.ZposicaoAtual ++;
    }
    else if(motores.ZposicaoAtual > motores.ZposicaoDesejada){
      meioPassoSoltoZ(0);
      motores.ZposicaoAtual --;
    }
  }
  else{
    zeraSolto(0);
  }
}

bool toggle1 = false;

void setup() {
    Serial.begin(9600);
    if(PDBG) Serial.println("inicia samerda");

    ITimer1.init();
    ITimer2.init();


    pinMode(stepY, OUTPUT);
    pinMode(dirY, OUTPUT);

    pinMode(stepX, OUTPUT);
    pinMode(dirX, OUTPUT);

    pinMode(stepZ, OUTPUT);
    pinMode(dirZ, OUTPUT);

    pinMode(stepA, OUTPUT);
    pinMode(dirA, OUTPUT);

    pinMode(enPin, OUTPUT);
    digitalWrite(enPin, LOW);

    pinMode(fimdcursoY, INPUT_PULLUP);
    pinMode(fimdcursoX, INPUT_PULLUP);
    pinMode(fimdcursoZ, INPUT_PULLUP);

    if(PDBG) Serial.println("Pinos configurados");
    
    //delay(2000);

    if (ITimer1.attachInterruptInterval(1, processoSerial))
    {
      if(PDBG) Serial.print(F("Starting  ITimer1 OK, millis() = ")); 
      if(PDBG) Serial.println(millis());
    }
    else
      if(PDBG) Serial.println(F("Can't set ITimer1. Select another freq. or timer"));

      
    if (ITimer2.setFrequency(TIMER_FREQ_HZ, trataMotores))
    {
      if(PDBG) Serial.print(F("Starting  ITimer2 OK, millis() = ")); 
      if(PDBG) Serial.println(millis());
    }
    else
      if(PDBG) Serial.println(F("Can't set ITimer2. Select another freq. or timer"));


/*
      delay(3000);
      zeraSolto(1);

      if(PDBG) Serial.println("aguardando os motores chegarem na posicao 0");
      while(motores.posicaoZerada == 0){
        delay(500);
        if(PDBG) Serial.println("aguardando...");
      }
      if(PDBG) Serial.println("motores em posicao zero");

      while(1){
        delay(3000);
        if(PDBG) Serial.println("movendo para posicao 1000 1000 1000");

        motores.XposicaoDesejada = 1000;
        motores.YposicaoDesejada = 1000;
        motores.ZposicaoDesejada = 1000;

        delay(3000);
        if(PDBG) Serial.println("movendo para posicao 100 100 100");

        motores.XposicaoDesejada = 100;
        motores.YposicaoDesejada = 100;
        motores.ZposicaoDesejada = 100;

      }
*/

/*
    while(1)
    {
      digitalWrite(13,1);
      digitalWrite(12,1);
      delay(1000);
      digitalWrite(13,0);
      digitalWrite(12,0);
      delay(1000);
    }*/

    /*motores.tempoPiscada = 500;

    while(1){
      if(verificaTempoPassado(motores.tempoPiscada, &motores.tUltimaPiscada)){
      
        digitalWrite(13, toggle1);
        toggle1 = !toggle1;
      }
    }*/


/*
    if(PDBG) Serial.println("iniciando zeragem de posicao");
    delay(2000);
    zeraY();
    if(PDBG) Serial.println("Zerou o Y");
    zeraX();
    if(PDBG) Serial.println("Zerou o X");
    zeraZ();
    if(PDBG) Serial.println("Zerou o Z");

    uint8_t k = 10;
    while(k)
    {
      if(PDBG) Serial.print("aguardando ");
      if(PDBG) Serial.print(k);
      if(PDBG) Serial.println(" segundos para mexer");
      delay(1000);
      k--;
    }

    motores.XposicaoDesejada = 1000;
    motores.YposicaoDesejada = 1000;
    motores.ZposicaoDesejada = 1000;*/


/*
    zeraY();
    if(PDBG) Serial.println("Zerou o Y");
    zeraX();
    if(PDBG) Serial.println("Zerou o X");
    zeraZ();
    if(PDBG) Serial.println("Zerou o Z");*/
}

void loop() {
    /*if (stopFlag) {
        
        digitalWrite(stepY, LOW);
        digitalWrite(stepX, LOW);
        digitalWrite(stepZ, LOW);
        if(PDBG) Serial.println("Motores parados");
        stopFlag = false;  
    }*/

    
  
}
union{
  uint16_t valor16;
  uint8_t valor8[2];
}converte16;


void salvaMensagem(uint8_t * mensagem)
{
  //if(PDBG) Serial.print("Mensagem recebida: ");
  //faz o que a mensagem pedir
  if(mensagem[2] == 1){
    zeraSolto(1);
    //motores.tempoPiscada = 100;
    //zerar posicionamento motor
  }

  if(mensagem[2] == 2){
    converte16.valor8[0] = mensagem[3];
    converte16.valor8[1] = mensagem[4];
    if(converte16.valor16 >= 4800) motores.XposicaoDesejada = 4800;
    else motores.XposicaoDesejada = converte16.valor16;

    motores.XposicaoDesejada *= 4;

    converte16.valor8[0] = mensagem[5];
    converte16.valor8[1] = mensagem[6];
    if(converte16.valor16 >= 4700) motores.YposicaoDesejada = 4700;
    else motores.YposicaoDesejada = converte16.valor16;
    motores.YposicaoDesejada *= 4;

    converte16.valor8[0] = mensagem[7];
    converte16.valor8[1] = mensagem[8];
    if(converte16.valor16 > 13000) converte16.valor16 = 13000;
    converte16.valor16 = converte16.valor16 * 10;
    if(converte16.valor16 >= 36600) motores.ZposicaoDesejada = 36600;
    else motores.ZposicaoDesejada = converte16.valor16;
  }
}

bool validaChecksum(uint8_t * mensagem)
{
  //faz calculo e retorna se o checksum tá valido
  // 1 = válido
  return 1;
}

void processoSerial() {
  static uint8_t estado = 0;
  static uint8_t dados[11];
  static uint8_t index = 0;
  static uint8_t timeout = 0;
  uint8_t byteRecebido = 0;

  timeout ++;
  if(timeout > 50){
    //if(PDBG) Serial.print("dentro do processo serial, tempo: ");
    //if(PDBG) Serial.println(millis());
    timeout = 0;
    estado = 0;
    index = 0;
    for(uint8_t k = 0; k < 11;k++){
      dados[k] = 0;
    }
  }

  if(Serial.available() > 0){
    byteRecebido = Serial.read();
    timeout = 0;
    switch(estado){
      case 0:
        if(byteRecebido == 0xAC){
          estado = 1;
          dados[0] = byteRecebido;
        }
        break;

        case 1:
        if(byteRecebido == 0x33){
          estado = 2;
          dados[1] = byteRecebido;
          index = 2;
        }
        break;

        case 2:
        if(index < 11){
          dados[index] = byteRecebido;
          index ++;
          if(index >= 11){
            estado = 0;
            index = 0;
            if(validaChecksum(dados)) salvaMensagem(dados);
            delay(10);            
            for(uint8_t k = 0; k < 11;k++){
              Serial.write(dados[k]);

              dados[k] = 0;
            }
          }
        }
        break;
        
        default:
        estado = 0;
        index = 0;
        for(uint8_t k = 0; k < 11;k++){
          dados[k] = 0;
        }
        break;
    }
  }
}
