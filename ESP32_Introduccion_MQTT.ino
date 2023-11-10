#include <WiFi.h>
#include <PubSubClient.h>
WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

const char* ssid     = "Nombre_de_la_red";
const char* password = "Contraseña";

char *server = "broker.emqx.io"; //nombre la pagina donde vamos a trabajar
int port = 1883; //Puerto

int ledpin = 2;

int var = 0;
int ledval = 0;
char datos[40];
String resultS = "";

long lastmillis = 0; //Para el  delay

void wifiInit() {
  Serial.print("Conectándose a ");
  Serial.println(ssid);  //Indica la reda la que se está conectando

  WiFi.begin(ssid, password); // Le manda los datos al módulo para que se conecte

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("Conectado a WiFi");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {  //Toda la información recibida
  Serial.print("Mensaje recibido [");
  Serial.print(topic);//Imprime el tópico
  Serial.print("] ");

  char payload_string[length + 1]; // El tamaño + un espacio
  int resultI;

  memcpy(payload_string, payload, length); //memcpy para pasar la informacion de una variable a otra
  payload_string[length] = '\0'; // Se imprime el \0 para indicar el final del mensaje
  resultI = atoi(payload_string);//Cambia la variable
  Serial.print(payload_string); //Imprimen en el monitor serial el mensaje
  var = resultI;
  resultS = "";

  for (int i = 0; i < length; i++) {
    resultS = resultS + (char)payload[i];
  }
  Serial.println();
}



void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Intentando conectarse MQTT...");

    if (mqttClient.connect("arduinoClient34")) {
      Serial.println("Conectado");

      mqttClient.subscribe("entrada01");

    } else {
      Serial.print("Fallo, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" intentar de nuevo en 5 segundos");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(ledpin, OUTPUT);
  Serial.begin(115200);
  delay(10);
  wifiInit();
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);
}

void loop()
{
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();


  if (var == 0) //si hay un 0; apague el led
  {
    digitalWrite(ledpin, LOW);
  }
  else if (var == 1) // si hay un 1, enciende el led
  {
    digitalWrite(ledpin, HIGH);
  }


  if (var == 2) //si hay un 2, Haga la siguiente secuencia
  {
    digitalWrite(ledpin, LOW);
    delay (2000);
    digitalWrite(ledpin, HIGH);
    delay (2000);
    digitalWrite(ledpin, LOW);
    delay (2000);
  }
  else if (var == 1)
  {
    digitalWrite(ledpin, HIGH);
  }

  if (millis() - lastmillis > 5000) {
    Serial.println("enviando data...");

    mqttClient.publish("salida01", "hola...123");//Envia un mensaje a la pagina del Broker, primero va el nombre de la salida y despues el mensaje
    lastmillis = millis();
  }
}