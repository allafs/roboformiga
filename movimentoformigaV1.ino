#include <DualMotor.h>
#include <NewPing.h>
#include <Servo.h>
#include <Wire.h>
#include <LSM303.h>

#define TRIG_PIN A3  //define o pino A3 para sensor de distancia 
#define ECHO_PIN A2  //define o pino A2 para sensor de distancia
#define MAX_DISTANCE 500 //distancia maxima de 50cm

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);
DualMotor dualmotor;
LSM303 compass;//instancia da bussola/acelerometro. define pinos A4(SDA) e A5(SCL)
Servo servosensor;

int lf=0; 
int ld45=0; 
int ld90=0;  
int le45=0; 
int le90=0;
int angi;
int angf;

//------------
void setup(){
  
  Serial.begin(9600);//saida para visor 

  //Inicializacao e calibracao da bussola
  Wire.begin();
  compass.init();
  compass.enableDefault();
  compass.m_min = (LSM303::vector<int16_t>){-337, +271,+296};
  compass.m_max = (LSM303::vector<int16_t>){+64, +648, +550};

  //Motores parados
  dualmotor.M1parar();
  dualmotor.M2parar();

  //coloca o sensor para frente
  servosensor.attach(9);
  servosensor.write(90); 
}

//------------
void loop(){
  int cm;
  servosensor.write(90);
  delay(500);
  lerdistancia(); //leitura da distancia
  while (cm == 0 or cm < 1){
   lerdistancia();
  }
  //Para frente enquanto nao houver obstaculo a 7cm
  while(cm >= 7){
    lerdistancia();
    parafrente();
  }
  parar();
  
  //Ao encontrar obstaculo calcula o mais distante e gira o robo para a posicao
  movsensor(); 
  calcposicao();
  
  delay(3000);
}

//---------------
void lerdistancia(){
  
  int cm;
  unsigned int uS=sonar.ping()/US_ROUNDTRIP_CM;
  
  cm=uS;
  
  Serial.print("obstaculo a ");
  Serial.print(cm);
  Serial.print("cm de distancia");
  Serial.println();
 
  delay(50);//leitura em metade de um microsegundo
}

//----------------------
void movsensor(){
 
 int cm;
 int pos;//(10)-direita90g,(45)-direita45g,(90)-frente,(135)-esquerda45g,(170)-esquerda90g 
 
 pos=10;
 servosensor.write(pos);
 delay(500);
 lerdistancia();
 delay(500);//10 leituras da distÃ¢ncia em 5 microsegundos
 ld90=cm;
 delay(2000);
 
 pos=45;
 servosensor.write(pos);
 delay(500);
 lerdistancia();
 delay(500);
 ld45=cm;
 delay(2000);
 
 pos=90;
 servosensor.write(pos);
 delay(500);
 lerdistancia();
 delay(500);
 lf=cm;
 delay(2000);
 
 pos=135;
 servosensor.write(pos);
 delay(500);
 lerdistancia();
 delay(500);
 le45=cm;
 delay(2000);
 
 pos=170;
 servosensor.write(pos);
 delay(500);
 lerdistancia();
 ld90=cm;
 delay(2000);
}

//-----------
void leiturabussola(){
 
 float declinationAngle = 12.117; 
 float heading;
 int int_heading;
 
 compass.read();
 heading = compass.heading();
 heading += declinationAngle;
 int_heading = int(heading);
 int_heading = map(int_heading,12,372,0,360);
 angi = int_heading;
 delay(50);//leitura em metade de um microsegundo
}

//-----------
void calcposicao(){
  
  if(ld45>=ld90 && ld45>=le45 && ld45>=le90){
    leiturabussola();
    angf = 0;
    
    if(angi>=0 && angi<314){  
      angf = angi+45;
    }
    else{ 
      angf = (angi+45)-360;
    }
    while (angf<=314 && angi>=314 || angi<=angf){
      dualmotor.M1move(180,1);
      dualmotor.M2move(180,1);
      leiturabussola();
    }
    parar();
  }
  
  if(ld90>=ld45 && ld90>=le45 && ld90>=le90){
    leiturabussola();
    angf = 0;
    
    if(angi>=0 && angi<269){  
    angf = angi+90;
    }
    else{ 
      angf = (angi+90)-360;
    }
    while (angf<=269 && angi>=269 || angi<=angf){
      dualmotor.M1move(180,1);
      dualmotor.M2move(180,1);
      leiturabussola();
    }
    parar();
  }
  
  if(le45>=ld45 && le45>=ld90 && le45>=le90){
    leiturabussola();
    angf = 0;
    
    if(angi<=359 && angi>45){  
      angf = angi-45;
      delay(1000);
      if(angf==0){angf=360;}
    }
    else{
      angf = 360+(angi-45);
      delay(1000);
      if(angf==0){angf=360;}
    }
    while (angf<angi || angi<=44 && angf>=44){
      dualmotor.M1move(180,0);
      dualmotor.M2move(180,0);
      leiturabussola();
      if(angi==0){angi=360;}
    }
    parar();
  }
  
  if(le90>=ld45 && le90>=ld90 && le90>=le45){
    leiturabussola();
    angf = 0;
    
    if(angi<=359 && angi>90){  
      angf = angi-90;
      delay(1000);
      if(angf==0){angf=360;}
    }
    else{
      angf = 360+(angi-90);
      delay(1000);
      if(angf==0){angf=360;}
    }
    while (angf<angi || angi<=89 && angf>=89){
      dualmotor.M1move(180,0);
      dualmotor.M2move(180,0);
      leiturabussola();
      if(angi==0){angi=360;}
    }
    parar();
  }
  
  if(le90<=10 && ld45<=10 && ld90<=10 && le45<=10){
    leiturabussola();
    angf = 0;
    
    if(angi>=0 && angi<179){  
      angf = angi+180;
    }
    else{ 
      angf = (angi+180)-360;
    }
    while (angf<=179 && angi>=179 || angi<=angf){
      dualmotor.M1move(180,1);
      dualmotor.M2move(180,1);
      leiturabussola();
    }
    parar();
  }
  
}

//--------------
void parar(){
  
  Serial.println("parar");
  delay(500);
  dualmotor.M1parar();
  dualmotor.M2parar();
}

//-----------
void parafrente(){
  
  int pos=90;
  servosensor.write(pos);
  delay(500);
  Serial.print("para frente");
  Serial.println();
  delay(500);
  
  //Parametro dualmotor.M1move (velocidade,sentido)
  //velocidade (0 a 255) e sentido (0 - HorÃ¡rio ou 1 - Anti-horÃ¡rio)
  dualmotor.M1move(200,0);
  dualmotor.M2move(200,1);
}

//------------------
void paratras(){
  
  Serial.println("para tras");
  delay(500);
  dualmotor.M1move(180,1);
  dualmotor.M2move(180,0);
  delay(2000);
}
