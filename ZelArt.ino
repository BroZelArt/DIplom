
// ************************** НАСТРОЙКИ ***********************
#define CURRENT_LIMIT 2000  // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит
#define AUTOPLAY_TIME 30    // время между сменой режимов в секундах


#define WIDTH 16              // ширина матрицы
#define HEIGHT 16             // высота матрицы

#define NUM_LEDS WIDTH * HEIGHT        // количсетво светодиодов в одном отрезке ленты
#define NUM_STRIPS 16        // количество отрезков ленты (в параллели)
#define LED_PIN D6           // пин ленты
#define BTN_PIN D0           // пин кнопки/сенсора
#define buzzer D4          // подключаем пьезоэлемент к контакту 5 ШИМ
#define MIN_BRIGHTNESS 5  // минимальная яркость при ручной настройке
#define BRIGHTNESS 60      // начальная яркость

// -------- РАССВЕТ -------
#define DAWN_BRIGHT 200       // макс. яркость рассвета
#define DAWN_TIMEOUT 1        // сколько рассвет светит после времени будильника, минут
#define alarmTimer  30         // за сколько минут до будильника включать рассвет
#define MCTime  10*1000         // частота обновления данных о микроклимате в мс

// -------- ВРЕМЯ -------
#define GMT 3              // смещение (москва 3)
#define NTP_ADDRESS  "europe.pool.ntp.org"    // сервер времени
#define NTP_INTERVAL 60 * 1000    // обновление (1 минута)  

//
#define MQTT_client "ZelArtem" // произвольное название MQTT клиента, иногда требуется уникальное.

char ssid[] = "Zelen";
char pass[] = "9050104241";


// настройки для MQTT брокера
const char *mqtt_server = "broker.hivemq.com"; // адрес сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT
const char *mqtt_user = "ZelArt"; // Логин от сервера MQTT
const char *mqtt_pass = "ZelArt911"; // Пароль от сервера MQTT
const char *led_topic = "Zelen/led"; // топик для включения светильника
const char *autoplay_topic = "Zelen/autoplay"; // топик авторежима
const char *temp_topic = "Zelen/temp"; // топик для данных температуры
const char *pres_topic = "Zelen/pres"; // топик для данных давления
const char *co2_topic = "Zelen/co2"; // топик для данных СО2
const char *humidity_topic = "Zelen/humidity"; // топик для данных влажности
const char *brightness_command_topic = "Zelen/brightness_command"; // топик для выставления яркости
const char *brightness_state_topic = "Zelen/brightness_state"; // топик для отправки данных яркости
const char *speed_command_topic = "Zelen/speed_command"; // топик для данных скорости анимации
const char *speed_state_topic = "Zelen/speed_state"; // топик для отправки данных скорости анимации
const char *scale_command_topic = "Zelen/scale_command"; // топик для данных плотности анимации
const char *scale_state_topic = "Zelen/scale_state"; // топик для отправки данных плотности
const char *mode_command_topic = "Zelen/mode_command"; // топик для данных режима анимции
const char *mode_state_topic = "Zelen/mode_state"; // топик для отправки режима анимации
const char *minutes_command_topic = "Zelen/minutes_command"; // топик для минут
const char *hours_command_topic = "Zelen/hours_command"; // топик для часов
const char *alarm_command_topic = "Zelen/alarm_command"; // топик для включения будильника
const char *alarm_state_topic = "Zelen/alarm_state"; // топик для включения будильника
String one = "1";
String zero = "0";
// ************************** ДЛЯ РАЗРАБОТЧИКОВ ***********************
#define MODES_AMOUNT 8 //клдичеств орежимов подсветки
#include <EEPROM.h>
#include <FastLED.h>
#include "GyverButton.h"
#include "GyverTimer.h"
#include <WiFiManager.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <typeinfo>

WiFiClient wclient;
PubSubClient client(wclient);
//****************Датчик температуры и давления**********************
#include <GyverBME280.h>
GyverBME280 bme;
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);
CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;

GTimer_ms effectTimer(80);
GTimer_ms autoplayTimer((long)AUTOPLAY_TIME * 1000);
GTimer_ms brightTimer(20);

////  NTP Client для получения времени
WiFiUDP Udp;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, GMT * 3600, NTP_INTERVAL);

// Переменные для хранения времени
uint8_t  myTimeH; // час будильника
uint8_t  myTimeM; // минута будильника
uint8_t  TimeH;
uint8_t  TimeM;
int thisTime;  //реальное время
int alarmTime; //время будильника
int mcTimer;   // частота обновления данных о микроклимате
int mcTime = MCTime;
int brightness = BRIGHTNESS;
int currentMode = 0;
boolean autoplay = true;
boolean brightDirection = true;
boolean dawnFlag = false;
boolean alarmOFF = false;
boolean ONflag = false;
unsigned char matrixValue[8][16];

//параметры для каждого режима
struct {
  byte brightness = 50;
  byte speed = 80;
  byte scale = 40;
} modes[MODES_AMOUNT];

void setup() {
  ESP.wdtDisable();
  Serial.begin(115200); //подключение к порту Serial для отладки
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); //инициализация светодиодной матрицы
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT); //создаём лимит по миллиамперам
  FastLED.setBrightness(brightness); //выствляем яркость матрицы
  FastLED.show();  //передаём данные на матрицу

  randomSeed(analogRead(0)); //создаём генератор случайных чисел для режимов свечения
  touch.setTimeout(300); // установка таймаута удержания (по умолчанию 300 мс)
  touch.setStepTimeout(50); // установка таймаута между инкрементами (по умолчанию 400 мс)
  WiFi.begin(ssid, pass); //подключаемся к сети wifi

  Serial.print("Connecting to ");  //  "Подключение к "
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) //сидимв цикле, пока не подключимся к интернету
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  timeClient.begin();  //подключаемся к серверу с реальным временем

  //EEPROM
  EEPROM.begin(3); //забираем из постоянной памяти данные о будильнике
  delay(50);
  dawnFlag = (uint8_t)EEPROM.read(0);
  myTimeH = (uint8_t)EEPROM.read(1);
  myTimeM = (uint8_t)EEPROM.read(2);

  // запуск датчика и проверка на работоспособность
  if (!bme.begin(0x76)) Serial.println("Error!");

  //подключение к mqtt
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) { // если соединения нет
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);// подключаемся к wi-fi

    if (WiFi.waitForConnectResult() != WL_CONNECTED) // ждем окончания подключения
      return;
    Serial.println("WiFi connected");
  }

  // подключаемся к MQTT серверу
    if (!client.connected()) { // если нет подключения к серверу MQTT
   
      if (client.connect(MQTT_client, mqtt_user, mqtt_pass)) {
        client.setCallback(callback);
        client.subscribe(led_topic);                  // подписка на топик led
        client.subscribe(temp_topic);
        client.subscribe(pres_topic); // подписка на топик data
        client.subscribe(brightness_command_topic);
        client.subscribe(autoplay_topic);
        client.subscribe(scale_command_topic);
        client.subscribe(speed_command_topic);
        client.subscribe(mode_command_topic);
        client.subscribe(minutes_command_topic);
        client.subscribe(hours_command_topic);
        client.subscribe(alarm_command_topic);
        Serial.println(MQTT_client);
      } else {
        Serial.println("MQTT - error");   // если не удалось подключиться сообщаем в порт
      }
    }
    if (client.connected()) { // если есть соединение с MQTT
      client.loop();
      refreshData();
      effectsTick();
      timeTick();
      buttonTick();
      ESP.wdtFeed();
      yield();
    }
}
