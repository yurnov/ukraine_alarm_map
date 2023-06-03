// Обов'зяково прочитай інструкцію перед використанням https://drukarnia.com.ua/articles/bagatofunkcionalna-proshivka-karta-povitryanikh-trivog-rjK3N
// ============ НАЛАШТУЙ МЕНЕ ============
//Налаштування WiFi
char* ssid = ""; //Назва твоєї мережі WiFi
char* password = ""; //Пароль від твого WiFi
char* APSsid = "AlarmMap"; //Назва точки доступу щоб переналаштувати WiFi
char* APPassword = ""; //Пароль від точки доступу щоб переналаштувати WiFi. Пусте - без пароля, рекомендую так і залишити (пароль від 8 симолів)

//Налштування за замовчуванням
bool enabled = true; //Ввімкнена/вимкнена карта
int brightness = 100; //Яскравість %
int alarm_brightness[] = {
  0,
  100,
  20,
  50
};
bool autoBrightness = false; //Ввімкнена/вимкнена авто яскравість
bool autoSwitch = true; //Автоматичне переключення карти на режим тривоги при початку тривоги в вибраній області, після заверешення тривоги в вибраній області режим не повертається на своє місце
static bool greenStates = true; //true - області без тривоги будуть зелені; false - не будуть світитися

int mapModeInit = 1;
int mapMode = 1; //Режим

bool blink = true;
int blinkCount = 2;
int blinkDistricts[] = {
  7,
  8
};

int modulationMode = 1;
int modulationStep = 10;
int modulationTime = 100;
int modulationCount = 2;

int hv = 60000;

//Налаштування авто-яскравості
const int day = 8; //Початок дня
const int night = 23; //Початок ночі
const int dayBrightness = 100; //Денна яскравість %
const int nightBrightness = 20; //Нічна яскравість %

//Для погоди
const char* apiKey = ""; //API погоди
float minTemp = 10.0; // мінімальна температура у градусах Цельсія для налашутвання діапазону кольорів
float maxTemp = 35.0; // максимальна температура у градусах Цельсія для налашутвання діапазону кольорів

//Налаштуванння режимів
int statesIdsCheck[] = { //Вибери області при тривозі в яких буде пермикатися режим на тривоги (1 - область активована; 0 - не активована)
0, //Закарпатська область
0, //Івано-Франківська область
0, //Тернопільська область
0, //Львівська область
0, //Волинська область
0, //Рівненська область
0, //Житомирська область
1, //Київ
0, //Київська область
0, //Чернігівська область
0, //Сумська область
0, //Харківська область
0, //Луганська область
0, //Донецька область
0, //Запорізька область
0, //Херсонська область
0, //Автономна Республіка Крим
0, //Одеська область
0, //Одеська область
0, //Миколаївська область
0, //Дніпропетровська область
0, //Полтавська область
0, //Черкаська область
0, //Кіровоградська область
0, //Вінницька область
0, //Хмельницька область
0  //Чернівецька область
};

static String states[] = {
  "Закарпатська область",
  "Івано-Франківська область",
  "Тернопільська область",
  "Львівська область",
  "Волинська область",
  "Рівненська область",
  "Житомирська область",
  "м. Київ",
  "Київська область",
  "Чернігівська область",
  "Сумська область",
  "Харківська область",
  "Луганська область",
  "Донецька область",
  "Запорізька область",
  "Херсонська область",
  "Автономна Республіка Крим",
  "Одеська область",
  "Одеська область",
  "Миколаївська область",
  "Дніпропетровська область",
  "Полтавська область",
  "Черкаська область",
  "Кіровоградська область",
  "Вінницька область",
  "Хмельницька область",
  "Чернівецька область"
};

int statesIds[] = {
  690548,
  707471,
  691650,
  702550,
  702569,
  695594,
  686967,
  703447,
  703448,
  710735,
  692194,
  706483,
  702658,
  709717,
  687700,
  706448,
  703883,
  698740,
  698740,
  700569,
  709930,
  696643,
  710719,
  705811,
  689558,
  706369,
  710719
};

// =======================================

#include <ArduinoJson.h> //json для аналізу інформації
#include <Adafruit_NeoPixel.h> //neopixel для управління стрічкою
#include <WiFi.h> //для зв'язку
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <UniversalTelegramBot.h> //Telegram бот
#include <WiFiManager.h> //Керування WiFi
#include <NTPClient.h> //Час
#include <HTTPUpdate.h> //Оновлення прошивки через тг бота
#include <WebServer.h>
#define LED_PIN 25
#define LED_COUNT 27

// JSON data buffer
StaticJsonDocument<250> jsonDocument;
char buffer[250];

WebServer server(80);

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
DynamicJsonDocument doc(30000);
//String baseURL = "https://vadimklimenko.com/map/statuses.json";
String baseURL = "https://map.vglskr.net.ua/alarm_map";
WiFiClientSecure client;
WiFiManager wm;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ua.pool.ntp.org", 7200);
static unsigned long times[27];
static int ledColor[27];
static int ledColorBlue[] = { 4,5,6,7,8,9,10,11,12,21,22 };
static int ledColorYellow[] = { 0,1,2,3,12,13,14,15,16,17,18,19,20,23,24,25,26 };
int arrDistrictsSize = sizeof(blinkDistricts) / sizeof(int);
int arrAlarms = sizeof(ledColor) / sizeof(int);
int arrSize = sizeof(states) / sizeof(String);
bool enable = false;
int alarms_period = 15000;
int weather_period = 600000;
unsigned long lastAlarmsTime;
unsigned long lastWeatherTime;
static bool firstAlarmsUpdate = true;
static bool firstWeatherUpdate = true;
int alarmsNowCount = 0;
static bool wifiConnected;


void setup_routing() {	 	 ;
  server.on("/params", HTTP_POST, handlePost);
  server.on("/params", HTTP_GET, getEnv);

  // start server
  server.begin();
}

void handlePost() {
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);

  // Get brightness
  int brightness = jsonDocument["brightness"];
  int auto_brightness = jsonDocument["auto_brightness"];
  int map_mode = jsonDocument["map_mode"];
  int green_states_on = jsonDocument["green_states_on"];
  int green_states_off = jsonDocument["green_states_off"];
  int map_enable = jsonDocument["map_enable"];
  int map_disable = jsonDocument["map_disable"];

  int blink_enable = jsonDocument["blink_enable"];
  int blink_disable = jsonDocument["blink_disable"];
  int modulation_mode = jsonDocument["modulation_mode"];
  int set_hv = jsonDocument["hv"];

  if(brightness) {
    autoBrightness = false;
    strip.setBrightness(brightness * 2.55);
    strip.show();
  }

  if(auto_brightness) {
    autoBrightness = true;
  }

  if(green_states_on) {
    greenStates = true;
  }

  if(green_states_off) {
    greenStates = false;
  }

  if(map_mode) {
    mapModeInit = map_mode;
  }

  if(map_enable) {
    enabled = true;
  }

  if(map_disable) {
    enabled = false;
  }

  if(blink_enable) {
    blink = true;
  }

  if(blink_disable) {
    blink = false;
  }

  if(modulation_mode) {
    modulationMode = modulation_mode;
  }

  if (set_hv) {
    hv = set_hv*1000;
  }

  // Respond to the client
  server.send(200, "application/json", "{}");
}

void getEnv() {
  Serial.println("Get temperature");
  jsonDocument.clear();
  jsonDocument["brightness"] = brightness;
  jsonDocument["autobrightness"] = autoBrightness;
  jsonDocument["mapMode"] = mapMode;
  jsonDocument["mapModeInit"] = mapModeInit;
  jsonDocument["autoSwitch"] = autoSwitch;
  jsonDocument["enabled"] = enabled;
  jsonDocument["greenStates"] = greenStates;
  jsonDocument["alarmsNowCount"] = alarmsNowCount;
  jsonDocument["greenStates"] = greenStates;
  jsonDocument["blink"] = blink;
  jsonDocument["modulationMode"] = modulationMode;
  jsonDocument["hv"] = hv;
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}


void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.status() != WL_CONNECTED) {
    bool res;
    res = wm.autoConnect("AlarmMap", ""); //точка достпу для налаштування WiFi, другі лапки - пароль
    if (!res) {
      Serial.println("Помилка підключення");
      ESP.restart();
    }
    else {
      Serial.println("Підключено :)");
    }
  }
}
void Modulation(int count) {
  int pixel_brightness_start;
  int pixel_brightness_end;
  int pixel_color_full;
  int pixel_color_penta;
  if (alarm_brightness[1] >= alarm_brightness[2]) {
    pixel_brightness_start = static_cast<int>(alarm_brightness[2]*brightness/100);
    pixel_brightness_end = static_cast<int>(alarm_brightness[1]*brightness/100);
  } else {
    pixel_brightness_start = static_cast<int>(alarm_brightness[1]*brightness/100);
    pixel_brightness_end = static_cast<int>(alarm_brightness[2]*brightness/100);
  }
  for (int i = 0; i < count; i++) {
    for (int i = pixel_brightness_end; i >= pixel_brightness_start; i -= modulationStep) {
      pixel_color_full= static_cast<int>(255*i/100);
      pixel_color_penta = static_cast<int>(55*i/100);
      for (int i = 0; i < arrSize; i++)
      {
        switch (ledColor[i]) {
          case 1: strip.setPixelColor(i, strip.Color(255, 0, 0)); break;
          case 2: strip.setPixelColor(i, strip.Color(pixel_color_full, pixel_color_penta, 0)); break;
          case 0: if (greenStates) {} else {strip.setPixelColor(i, strip.Color(0, 0, 0)); break;}
          case 3: strip.setPixelColor(i, strip.Color(0, 255, 0)); break;
        }
      }
      strip.show();
      delay(modulationTime);
    }
    for (int i = pixel_brightness_start; i <= pixel_brightness_end; i += modulationStep) {
      pixel_color_full= static_cast<int>(255*i/100);
      pixel_color_penta = static_cast<int>(55*i/100);
      for (int i = 0; i < arrSize; i++)
      {
        switch (ledColor[i]) {
          case 1: strip.setPixelColor(i, strip.Color(255, 0, 0)); break;
          case 2: strip.setPixelColor(i, strip.Color(pixel_color_full, pixel_color_penta, 0));; break;
          case 0: if (greenStates) {} else {strip.setPixelColor(i, strip.Color(0, 0, 0)); break;}
          case 3: strip.setPixelColor(i, strip.Color(0, 255, 0)); break;
        }
      }
      strip.show();
      delay(modulationTime);
    }
  }
}
void Blink(int count) {
  //BLYNK
  int blinkCounter = count*2+1;
  bool blinkState = false;

  //if (ledColor[1] == 1 || ledColor[1] == 2) { // Якщо 1 лампочка світить червоним або жовтим кольором
    for (int i = 0; i < blinkCounter; i++) {
      blinkState = !blinkState;
      for (int i = 0; i < arrDistrictsSize; i++) {
        if (blinkState) {
          switch (ledColor[blinkDistricts[i]]) {
          case 1: strip.setPixelColor(blinkDistricts[i], strip.Color(255, 0, 0)); break;
          case 2: strip.setPixelColor(blinkDistricts[i], strip.Color(255, 55, 0)); break;
          case 0: if (greenStates) {} else {strip.setPixelColor(blinkDistricts[i], strip.Color(0, 0, 0)); break;}
          case 3: strip.setPixelColor(blinkDistricts[i], strip.Color(0, 255, 0)); break;
          }
        } else {
          strip.setPixelColor(blinkDistricts[i], strip.Color(0, 0, 0)); // Вимкнути 1 лампочку
        }
        strip.show();
      } // Оновити світлодіодну стрічку
      delay(1000); // Затримка 1 секунда
    }
  //}
  //BLYNK
}
void Flag(int wait) {
  //strip.setPixelColor(ledColorBlue[i], strip.Color(0,191,255));
  //strip.setPixelColor(ledColorYellow[i], strip.Color(255,255,51));
  int count = sizeof(ledColorYellow) / sizeof(int);
  for (int i = 0; i < 11; i++) { // For each pixel in strip...
    strip.setPixelColor(ledColorBlue[i], strip.Color(0,255,255));
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
  for (int i = 0; i < 17; i++) { // For each pixel in strip...
    strip.setPixelColor(ledColorYellow[i], strip.Color(255,255,0));
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
void initStrip() {
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(brightness * 2.55);
  Flag(120);
}
void initTime() {
  // Встановлюємо початкове значення літнього часу на false
  bool isDaylightSaving = false;

  // Отримуємо поточну дату та час з сервера NTP
  timeClient.begin();
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();

  // Розбиваємо рядок з форматованим часом на складові
  int day, month, year, hour, minute, second;
  sscanf(formattedTime.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);

  // Перевіряємо, чи поточний місяць знаходиться в інтервалі березень-жовтень
  if (month >= 3 && month <= 10) {
    // Якщо так, встановлюємо літній час на true
    isDaylightSaving = true;
  }

  // Встановлюємо зміщення часового поясу для врахування літнього часу
  if (isDaylightSaving) {
    timeClient.setTimeOffset(14400); // UTC+3 для України
  }
  else {
    timeClient.setTimeOffset(10800); // UTC+2 для України
  }
}

void setup() {
  initStrip();
  initWiFi();
  Serial.begin(115200);
  initTime();
  setup_routing();
}

void loop() {
	wifiConnected = WiFi.status() == WL_CONNECTED;

  if (!wifiConnected) {
    Flag(10);
    delay(10000);
    ESP.restart();
  }

  if (wifiConnected) {
    server.handleClient();
    if (enabled) {
      if (autoBrightness) {
        timeClient.update();
        int currentHour = timeClient.getHours();
        bool isDay = currentHour >= day && currentHour < night;
        brightness = isDay ? dayBrightness : nightBrightness;
        for (int i = 0; i < LED_COUNT; i++) {
          strip.setBrightness(brightness * 2.55);
        }
        strip.show();
      }

      //тривоги
      if (millis() - lastAlarmsTime > alarms_period || firstAlarmsUpdate) {
        firstAlarmsUpdate = false;
        String response;
        HTTPClient http;
        http.begin(baseURL.c_str());
        // Send HTTP GET request
        int httpResponseCode = http.GET();

        if (httpResponseCode == 200) {
          response = http.getString();
        }
        else {
          return;
        }
        // Free resources
        http.end();
        DeserializationError error = deserializeJson(doc, response);
        if (error) {
          return;
        }

        unsigned long  t = millis();
        alarmsNowCount = 0;
        bool return_to_init_mode = true;
        for (int i = 0; i < arrSize; i++) {
          enable = doc["states"][states[i]]["enabled"].as<bool>();
          if (enable && times[i] == 0) {
            times[i] = t;
            ledColor[i] = 2;
            alarmsNowCount++;
          }
          else if (enable && times[i] + hv > t && ledColor[i] != 1) {
            ledColor[i] = 2;
            alarmsNowCount++;

          }
          else if (enable) {
            ledColor[i] = 1;
            times[i] = t;
            alarmsNowCount++;
          }

          if (!enable && times[i] + hv > t && times[i] != 0) {
            ledColor[i] = 3;
          }
          else if (!enable) {
            ledColor[i] = 0;
            times[i] = 0;
          }

          if (autoSwitch && enable && statesIdsCheck[i]==1) {
              mapMode = 1;
              return_to_init_mode = false;
          }

          if (return_to_init_mode) {
            mapMode = mapModeInit;
          }
        }
      }

      if (mapMode == 1) {
        for (int i = 0; i < arrSize; i++)
        {
          switch (ledColor[i]) {
          case 1: strip.setPixelColor(i, strip.Color(255, 0, 0)); break;
          case 2: strip.setPixelColor(i, strip.Color(255, 55, 0)); break;
          case 0: if (greenStates) {} else {strip.setPixelColor(i, strip.Color(0, 0, 0)); break;}
          case 3: strip.setPixelColor(i, strip.Color(0, 255, 0)); break;
          }
        }
        strip.show();
        if (modulationMode > 1) {
          Modulation(modulationCount);
        }
        if (blink) {
          Blink(blinkCount);
        }
      }
      if (mapMode == 2) {
        if (millis() - lastWeatherTime > weather_period || firstWeatherUpdate) {
          // Loop through the city IDs and get the current weather for each city
          firstWeatherUpdate = false;
          for (int i = 0; i < sizeof(statesIds) / sizeof(int); i++) {
            // Construct the URL for the API call
            String apiUrl = "http://api.openweathermap.org/data/2.5/weather?id=" + String(statesIds[i]) + "&units=metric&appid=" + String(apiKey);
            // Make the HTTP request
            HTTPClient http;
            http.begin(apiUrl);
            int httpResponseCode = http.GET();
            Serial.println(httpResponseCode);
            // If the request was successful, parse the JSON response
            JsonObject obj = doc.to<JsonObject>();
            if (httpResponseCode == 200) {
              String payload = http.getString();
              StaticJsonDocument<512> doc;
              deserializeJson(doc, payload);

              // Extract the temperature from the JSON response

              double temp = doc["main"]["temp"];
              double normalizedTemp = static_cast<double>(temp - minTemp) / (maxTemp - minTemp);
              float red, green, blue, t;

              if (normalizedTemp > 0.99){
                normalizedTemp = 0.99;
              }
              if (normalizedTemp < 0.01){
                normalizedTemp = 0.01;
              }
              if (normalizedTemp <= 0.33) {
                red = 0;
                green = 255;
                blue = static_cast<int>(255 - (normalizedTemp/0.33*255));
              } else if (normalizedTemp <= 0.66) {
                red = static_cast<int>(((normalizedTemp -0.33)/0.33*255));
                green = 255;
                blue = 0;
              } else {
                red = 255;
                green = static_cast<int>(255 - ((normalizedTemp-0.66)/0.33*255));
                blue = 0;
              }
              strip.setPixelColor(i, strip.Color(red, green, blue));
            }
            else {
              Serial.print("Error getting weather data for city ID ");
              Serial.println(statesIds[i]);
            }
            // Clean up the HTTP connection
            http.end();
            strip.show();
          }
          lastWeatherTime = millis();
        }
      }
      if (mapMode == 3) {
        Flag(10);
      }
    }
    else {
      strip.clear();
      strip.show();
    }
    delay(1000);
  }
}