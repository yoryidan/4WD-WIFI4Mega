/**
 * /////////////////////////////
 * //////ENGLISGH VERSION///////
 * /////////////////////////////

 * This program runs in an Arduino board (in this case in a Mega 2560), attached to an 4WD vehicle and a Wifi-ESP8266 module to communicate the intructions to the vehicle. You can equip any shield or other mechanism to calculate the position.
 * 
 * Firstly, the program initialice the devices.
 * You have to load manually the adjusts of your own Wifi network (name and password) to connect the ESP8266 to your network.
 * 
 * After the initialization if you connect another devices to the same network and send a message to the IP of the vehicle you can give it instructions to move and to get its position. 
 * (you can see the IP and MAC address of the vehicle in the Serial if you connect the board to the computer)
 * 
 * At the moment, you can ask the device for its position or to move in any direction thru the next commands:
 * 
 *      - To get the position: http://IP/getPos
 *      - To set movements: http://IP/setMove?direccion=ADELANTE&tiempo=1000    The direccion parameter can be ADELANTE/ATRAS/IZQUIERDA/DERECHA, the time is a value in ms(miliseconds), if you do not insert it will be 0 by default
 *      
 * @author J. Garcia Paredes for Rari AUTO|TECH
 * 
 * /////////////////////////////
 * //////SPANISH VERSION///////
 * /////////////////////////////
 * 
 * Este programa funciona sobre una placa Arduino (en nuestro caso UNO/MEGA). Está equipada con un chasis con 4 motores y su controladora (en  nuestro caso IRF3205), un shield para obtener la posición y con conexión a un dispositivo Wifi ESP8266. 
 * Al iniciarse el programa arranca los dispositivos y los configura.
 * 
 * En el setup hay que ajustar el comando AT+CWJAP con el nombre de la red Wifi a la que queremos conectarnos, seguido de la contraseña.
 * 
 * Una vez conectado desde otro dispositivo a la misma red Wifi se puede acceder al dispositivo Wifi desde un navegador, introduciendo la dirección IP de este (dicha dirección podemos encontrarla en el Serial, después de realizarse el setup)
 *
 * Actualmente está configurado para devolver la información sobre la posición actual al navegador cuando se le añade a la petición IP, getPos.
 * También se puede ordenar al vehículo moverse en cualquier dirección durante determinado tiempo. Mediante las siguientes instrucciones:
 * 
 *      - Para solicitar la posicion: http://IP/getPos
 *      - Para ordenar movimientos: http://IP/setMove?direccion=ADELANTE&tiempo=1000  El parámetro de la dirección puede ser ADELANTE/ATRAS/IZQUIERDA/DERECHA,el tiempo será un valor en ms(milisegundos), si no se inserta ningún valor, por defecto será 0.
 * 
 * @author J. Garcia Paredes para Rari AUTO|TECH
 * 
 */

//#include <SoftwareSerial.h>
 
#define DEBUG true
 
//SoftwareSerial esp8266(5,6);  // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                                // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                                // and the RX line from the esp to the Arduino's pin 3

////////////////////////////////////////////////
////////////////// PARAMETERS //////////////////
////////////////////////////////////////////////

// connect motor controller pins to Arduino digital pins
// motor one
int pwm1 = 10;
int dir1 = A1;

int pwm2 = 9;
int dir2 = A2;
int speed = 100;

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600); // your esp's baud rate might be different

  Serial.println(F("Starting positioning: "));

  //Initial Wifi Settings
  //sendData("AT+CIOBAUD=9600\r\n",2000,DEBUG);  // baud rate
  sendData("AT+RST\r\n",2000,DEBUG);             // reset module
  sendData("AT+CWMODE=3\r\n",1000,DEBUG);        // configure as access point and server

  sendData("AT+CWJAP=\"HUAWEI-E5172-863F\",\"DARIO2004\"\r\n",5000,DEBUG);                  //Change this parameters with the settings of your Wifi-network//
 
  delay(4000);
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG);         // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG);   // turn on server on port 80
  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  
  // set all the motor control pins to outputs
  pinMode(pwm1, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  pinMode(dir2, OUTPUT);
}
 
void loop()
{
  //int status; 
  String recibido = "";
  int posicionREST = -1;

  if(Serial1.available()) // check if the esp is sending a message 
  {

    if(Serial1.find("+IPD,"))
    {
      delay(1000);
 
     int connectionId = Serial1.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48

     //Serial info it's received (GET  from the internet-navi)
      while(Serial1.available())
      {
        // The ESP has data so display its output to the serial window 
        char c = Serial1.read(); // read the next character.
        recibido+=c;
      }

     posicionREST = recibido.indexOf("get");
     if(posicionREST >= 0){
       posicionREST = recibido.indexOf("Pos", posicionREST);
       if(posicionREST >= 0){
          //CalculatePosition
          //if (status == SUCCESS){
            //Serial.println();
            //printCoordinates(position);
            //web(connectionId, position);
         //}
       }
     }
     else{ 

      posicionREST = recibido.indexOf("set");
      if(recibido.indexOf("set")>=0){

        //Check if the REST it is "setMove" and if it has all the parameters setMove?direccion=ADELATE/ATRAS/IZQUIERDA/DERECHA&tiempo=****
        //Comprueba si el REST es de la forma setMove y si así fuera, comprueba si están todos lo parámetros setMove?direccion=ADELATE/ATRAS/IZQUIERDA/DERECHA&tiempo=****
        posicionREST = recibido.indexOf("Move", posicionREST);
        if(posicionREST >= 0){
            //webDefault(connectionId, true); 
            checkDirection(connectionId, recibido, posicionREST);
        }
        
     }else{
         Serial.print("HOLA\r\n");
         webDefault(connectionId, false); 
     }
    }
   }
  }
}


String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    Serial1.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(Serial1.available())
      {
        
        // The ESP has data so display its output to the serial window 
        char c = Serial1.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}

void webDirection(int connectionId, String direccion, String tiempo){
         String webpage="";

         webpage = "El dispositivo se desplazara en direccion ";
         webpage += direccion;
         webpage += "  durante: ";
         webpage += tiempo;
         webpage += " ms";
         String cipSend = "AT+CIPSEND=";
         cipSend += connectionId;
         cipSend += ",";
         cipSend +=webpage.length();
         cipSend +="\r\n";
         
         sendData(cipSend,1000,DEBUG);
         sendData(webpage,1000,DEBUG);
        
         String closeCommand = "AT+CIPCLOSE="; 
         closeCommand+=connectionId; // append connection id
         closeCommand+="\r\n";
         
         sendData(closeCommand,3000,DEBUG);
}

//Check if the REST it is "setMove" and if it has all the parameters setMove?direccion=ADELATE/ATRAS/IZQUIERDA/DERECHA&tiempo=****
void checkDirection(int connectionId, String recibido, int posicionSetMove){
    String movimiento = "";
    int tiempoInt = 0;
    String tiempo = "";

    posicionSetMove = recibido.indexOf("tiempo");
    if(posicionSetMove >= 0){
      
        //////////////////////coger tiempo;
        tiempo = getSubString(recibido , posicionSetMove, 1);

        if(tiempo != "")
            tiempoInt = tiempo.toInt();
        else
            tiempoInt = 0;

        posicionSetMove = recibido.indexOf("direccion");
        if(posicionSetMove >= 0){
          posicionSetMove = recibido.indexOf("ADELANTE");
          if(posicionSetMove >= 0){
           
            movimiento = "ADELANTE";
            //mover hacia adelante, x tiempo
            forwards();
            delay(tiempoInt);
            off();

          }else{
            posicionSetMove = recibido.indexOf("ATRAS");
            if(posicionSetMove >= 0){     
              movimiento = "ATRAS";
              //mover hacia detrás, x tiempo
              backwards();
              delay(tiempoInt);
              off();
            }else{
              posicionSetMove = recibido.indexOf("DERECHA");
              if(posicionSetMove >= 0){     
                movimiento = "DERECHA";
                //mover hacia detrás, x tiempo
                right();
                delay(tiempoInt);
                off();
              }else{
                  posicionSetMove = recibido.indexOf("IZQUIERDA");
                  if(posicionSetMove >= 0){     
                    movimiento = "IZQUIERDA";
                    //mover hacia detrás, x tiempo
                    left();
                    delay(tiempoInt);
                    off();
                  }else{
                      movimiento = " Direccion no introducida. ";
                    }
                  }
              }
          }
        }else{
          movimiento = " Direccion no introducida. ";
        }
    }else {
      movimiento = " Tiempo no introducido. ";
    }

    webDirection(connectionId, movimiento, tiempo);
  
}

void web(int connectionId, /*position coor*/){
    
     String webpage = "{\"x\":";
     webpage+= coor.x;
     webpage+= ",\"y\":";
     webpage+= coor.y;
     webpage+= ",\"z\":";
     webpage+= coor.z;
     webpage+="}";
     
     String cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
     cipSend +=webpage.length();
     cipSend +="\r\n";
     
     sendData(cipSend,1000,DEBUG);
     sendData(webpage,1000,DEBUG);
 
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     
     sendData(closeCommand,3000,DEBUG);  
}

void webDefault(int connectionId, boolean error){
         String webpage="";

         if(error){
            //webpage = "Error en el cálculo de la posición";
            webpage = "Set Move";
         }
         else{
            webpage = "Envie \"getPos\" .";
         }
         String cipSend = "AT+CIPSEND=";
         cipSend += connectionId;
         cipSend += ",";
         cipSend +=webpage.length();
         cipSend +="\r\n";
         
         sendData(cipSend,1000,DEBUG);
         sendData(webpage,1000,DEBUG);
        
         String closeCommand = "AT+CIPCLOSE="; 
         closeCommand+=connectionId; // append connection id
         closeCommand+="\r\n";
         
         sendData(closeCommand,3000,DEBUG);
}

//Check the value of the time in the REST, it can be used in the future to get othe values as the position coordinates
String getSubString(String recibido, int posicionSetMove, int mode){    //mode=1 for time reading
  String cadena = "";
  int posicionFin = 0;
  
  switch(mode){
    case 1:
    {
      posicionSetMove = posicionSetMove + 7;    //desplazamos hacia la derecha la posicion de la cadena "tiempo=" para empezar a recibir el tiempo
      Serial.print("modo 1 : ");
      posicionFin = recibido.indexOf(" HTTP/");

      if(posicionFin >= 0){
        cadena = recibido.substring(posicionSetMove, posicionFin);
      }
    }
    break;
  }

  return cadena;
}

///////////////////////////////////////////////////////////////
// prints the coordinates
/*void printCoordinates(position coor){
    Serial.print("{\"x\":");
    Serial.print(coor.x);
    Serial.print(",\"y\":");
    Serial.print(coor.y);
    Serial.print(",\"z\":}");
    Serial.println(coor.z);
}

*/
/////////////////////////MOTORES//////////////////////////////////////
void forwards(){
  // turn on motor A&B
  digitalWrite(dir1, HIGH);
  digitalWrite(dir2, HIGH);

  // set speed to 200 out of possible range 0~255
  analogWrite(pwm1, speed);
  analogWrite(pwm2, speed);

}

void backwards(){
  // turn on motor A&B
  // now change motor directions
  digitalWrite(dir1, LOW); 
  digitalWrite(dir2, LOW); 

  // set speed to 200 out of possible range 0~255
  analogWrite(pwm1, speed);
  analogWrite(pwm2, speed);
  
}

void left(){
   digitalWrite(dir1, LOW); 
  digitalWrite(dir2, HIGH); 

  // set speed to 200 out of possible range 0~255
  analogWrite(pwm1, speed);
  analogWrite(pwm2, speed);
}

void right(){
   digitalWrite(dir1, HIGH); 
  digitalWrite(dir2, LOW); 

  // set speed to 200 out of possible range 0~255
  analogWrite(pwm1, speed);
  analogWrite(pwm2, speed);
}


void off(){
    // now turn off motors
  digitalWrite(pwm1, LOW);  
  digitalWrite(pwm2, LOW);  
}
