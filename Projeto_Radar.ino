//Inclusao de bibliotecas
#include <NewPing.h> //Sensor
#include <Servo.h> //Servo
#include <Wire.h> 
#include <SoftwareSerial.h> //Saida TXRX

//Definicoes
#define TRIG_PIN A3  //define o pino A3 para sensor de distancia 
#define ECHO_PIN A2  //define o pino A2 para sensor de distancia
#define MAX_DISTANCE 450 //distancia maxima de 45cm

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);
Servo servosensor;
SoftwareSerial BT(3,2); //pinos D3 e D2 como TX e Rx, respectivamente
                         //com estas definicoes nao precisa desconectar o bluetooth para carregar o Arduino/Garagino

//Variavel para enviar dados via bluetooth
char buffer[5]; //utilização de Arrays e Buffer
int cmdbt;

//Declaracao de variaveis para sensor de distancia
int cm;//distancia do obstaculo em centimetros
int pos;//posição em graus, do sensor 
int uS;//variável do sensor de distancia

//Declaração de variáveis do calculo de distância
int angsensor[170];//Array dos angulos do sensor com 170 posicoes
int dist;

//------------
void setup(){
  
  BT.begin(9600); //Saida para o Bluetooth 
  servosensor.attach(9); //Define o pino D9 para a leitura do servo
  
}

//------------
void loop(){
  
  pos=90;
  servosensor.write(pos); //Coloca o servo para frente
  
  while(BT.available()>0){
    cmdbt = BT.read();//Faz a leitura do bluetooth
    if(cmdbt == '7'){
      entdadossensor();//Recebe o comando '7' definido no App - botão RADAR - e ativa o sensor
    }
    else if(cmdbt == '1'){
      pararsensor();//Parar o sensor
    }
  }
}

//----------------------
void entdadossensor(){

  // Varredura do sensor, do angulo 10 ao 170, colhendo as distancias
  for(pos=10;pos<=170;pos++){
    servosensor.write(pos);
    unsigned uS=sonar.ping()/US_ROUNDTRIP_CM;
    cm=uS;
    
    if(cm>45){ 
      cm=45; //Estabelece 45cm como a maior distancia
    }
    angsensor[pos]=cm;
    dist=angsensor[pos];
    
    //organiza as variaveis diferentes para enviar para o App
    sprintf(buffer,"%d,%d",pos,dist); //Gera saidas formatadas, interpolando variáveis a uma string de formatação
    BT.println(buffer); //Utilizacao de buffer para armazenamento
   
    delay(50); // Faz a leitura a cada 100 milisegundos -> mesma do temporizador do App
  }
}
//------------
void pararsensor(){
  pos=90;
  servosensor.write(pos); //Coloca o servo para frente
}

