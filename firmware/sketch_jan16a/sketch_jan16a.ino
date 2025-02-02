#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#define debug

#define LEAD 0
#define SLAVE 1
#define ADDITIONAL 2

const char* ssid = "ESP32";               // Имя сети
const char* password = "1234567890";      // Пароль сети

WebServer server(80); // Создаем сервер на порту 80

// Структура для хранения данных
struct ProtocolData {
  uint8_t end : 8;     // 8 бит
  uint8_t start : 8;   // 8 бит
  uint8_t command : 4; // 4 бита
  uint8_t len : 4;     // 4 бита
};

// Структура для хранения целевых значений
struct AimData {
  uint16_t speed : 10;
  uint16_t brake : 10;
  uint8_t left : 1;
  uint8_t right : 1;
};

struct Controller {
  uint8_t id;
  uint8_t type;
};

// Переменная для хранения выбранного протокола
ProtocolData protocolData;
AimData aimData;

// Массив с контроллерами (пока что бусть будет 16)
Controller controllers[16];
uint8_t kol = 0;

// HTML-код страницы
const char* htmlPage = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>NikoPostman</title>
    <style>
        body { font-family: Arial, sans-serif; }
        h1 { text-align: center; }
        input, button { width: 80%; margin: 10px auto; display: block; }
        textarea { width: 80%; height: 200px; margin: 10px auto; display: block; }
    </style>
    <script>
        function updateMessages() {
            fetch('/serialMessages')
                .then(response => response.text())
                .then(data => {
                    document.getElementById('messages').value += data;
                });
        }

        setInterval(updateMessages, 1000000); // Обновляем каждую секунду
    </script>
</head>
<body>
    <img src='/niko.png' alt='Logo' style='width:100px;height:100px;float:left;margin-right:10px;'>
    <h1>NikoPostman</h1>
    <button onclick="requestState()">Запросить состояние</button>
    <h2>Целевые значения</h2>
    <label for="speed">Целевое значение скорости (0-1023):</label>
    <input type="number" id="speed" min="0" max="1023">
    <label for="brake">Целевое значение тормозного усилия (0-1023):</label>
    <input type="number" id="brake" min="0" max="1023">
    <label for="leftTurnSignal">Левый поворотник:</label>
    <input type="checkbox" id="leftTurnSignal">
    <label for="rightTurnSignal">Правый поворотник:</label>
    <input type="checkbox" id="rightTurnSignal">
    <button onclick="sendTargetValues()">Отправить целевые значения</button>
    <textarea id="log" readonly></textarea>
    <script>
        function requestState() {
            var log = document.getElementById("log");
            log.value += "Запрос состояния отправлен.\n"; // Добавляем сообщение в лог
            fetch('/requestState'); // Отправляем запрос состояния на сервер
        }

        function sendTargetValues() {
            var speed = document.getElementById("speed").value;
            var brake = document.getElementById("brake").value;
            var leftTurnSignal = document.getElementById("leftTurnSignal").checked ? 1 : 0;
            var rightTurnSignal = document.getElementById("rightTurnSignal").checked ? 1 : 0;

            var log = document.getElementById("log");
            log.value += "Отправка целевых значений: Скорость = " + speed + ", Тормоз = " + brake +
                         ", Левый поворотник = " + leftTurnSignal + ", Правый поворотник = " + rightTurnSignal + "\n"; // Добавляем сообщение в лог
            fetch('/sendTargets?speed=' + speed + '&brake=' + brake + '&leftTurnSignal=' + leftTurnSignal + '&rightTurnSignal=' + rightTurnSignal, {method: 'POST'}); // Отправляем целевые значения на сервер
        }
    </script>
</body>
</html>
)rawliteral";

void setup() {
    controllers[0] = {0, 0};
    controllers[1] = {1, 1};
    kol += 2;
    Serial.begin(115200);

    Serial.println(ssid);
    
    // Устанавливаем точку доступа
    #if defined(_home) || defined(_phone)
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting...");
    }
    Serial.print("IP адрес: ");
    Serial.println(WiFi.localIP());
    
    #else
    WiFi.softAP(ssid, password);
    Serial.println("Точка доступа создана");
    Serial.print("IP адрес: ");
    Serial.println(WiFi.softAPIP());
    #endif

    // Обработчик корневого URL
    server.on("/", handleRoot);
    server.on("/niko.png", HTTP_GET, handleImage); // Обработчик для изображения
    server.on("/serialMessages", handleSerialMessages); // Обрабатываем запрос на получение сообщений
    
    server.on("/requestState", handleRequestState); // Обработчик для запроса состояния
    
    server.on("/sendTargets", HTTP_POST, handleSendTargets); // Обработчик для отправки целевых значений
    server.on("/getStatuses", HTTP_GET, handleGetStatuses); // Обрабатываем запрос на получение массива статусов
    server.on("/getControllers", HTTP_GET, handleGetControllers); // Обрабатываем запрос на получение массива контроллеров
    server.on("/putController", HTTP_PUT, handlePutController); // Обрабатываем запрос на добавление контроллера в сеть
    server.on("/deleteController", HTTP_DELETE, handleDeleteController); // Обрабатываем запрос на удаление контролера из сети
    
    // Запускаем сервер
    server.begin();
    Serial.println("HTTP сервер запущен");
}

void loop() {
    server.handleClient(); // Обработка входящих клиентов
}

// Функция для обработки запроса к корневому URL
void handleRoot() {
    Serial.println("!");
    server.sendHeader("Content-Type", "text/html; charset=utf-8"); // Устанавливаем правильную кодировку
    server.send(200, "text/html", htmlPage); // Отправляем HTML-код
    #if defined(debug)
    Serial.println("Connected!");
    #endif
}

// Функция для обработки запроса состояния
void handleRequestState() {
    protocolData.end = 0xFF;
    protocolData.start = 0x00;
    protocolData.command = 0x4;
    protocolData.len = 0x0;
    // Здесь можно добавить логику для обработки запроса состояния
    Serial.write((byte*)&protocolData, sizeof(protocolData));
    server.send(200, "text/plain", "Запрос состояния обработан"); // Ответ клиенту
    #if defined(debug)
    Serial.println("handleRequestState");
    #endif
}

// Функция для обработки отправки целевых значений
void handleSendTargets() {
    if (server.hasArg("speed") && server.hasArg("brake") && server.hasArg("leftTurnSignal") && server.hasArg("rightTurnSignal")) {
        int speed = server.arg("speed").toInt(); // Получаем значение скорости
        int brake = server.arg("brake").toInt(); // Получаем значение тормозного усилия
        int leftTurnSignal = server.arg("leftTurnSignal").toInt(); // Получаем значение левого поворотника
        int rightTurnSignal = server.arg("rightTurnSignal").toInt(); // Получаем значение правого поворотника

        protocolData.end = 0xFF;
        protocolData.start = 0x00;
        protocolData.command = 0x6;
        protocolData.len = 0x4;

        aimData.speed = speed;
        aimData.brake = brake;
        aimData.left = leftTurnSignal;
        aimData.right = rightTurnSignal;
        
        // Здесь можно добавить логику для обработки целевых значений
        Serial.write((byte*)&protocolData, sizeof(protocolData));
        Serial.write((byte*)&aimData, sizeof(aimData));

        // Отправляем ответ клиенту
        server.send(200, "text/plain", "Целевые значения отправлены"); 
    } else {
        server.send(400, "text/plain", "Ошибка: недостающие параметры"); // Ответ об ошибке
    }
    #if defined(debug)
    Serial.println("handleSendTargets");
    #endif
}

// Функция для обработки запроса изображения
void handleImage() {
    File file = SPIFFS.open("/niko.png", "r"); // Открываем файл изображения
    if (!file) {
//        Serial.println("Ошибка открытия файла изображения");
        server.send(404, "text/plain", "Файл не найден");
        return;
    }
    server.streamFile(file, "image/png"); // Отправляем файл как изображение
    file.close(); // Закрываем файл после отправки
}

// Функция для обработки запроса на получение сообщений из Serial
void handleSerialMessages() {
    String serialMessages = "";
    if (Serial.available() >= 3) {
        serialMessages = "Подключился контроллер: " + '\n';
        Serial.readBytes((byte*)&protocolData, sizeof(protocolData));
    
        while (Serial.available() < 1) {}
        Serial.read();

        serialMessages += protocolData.end + '|' + protocolData.start + '|' + protocolData.command + '|' + protocolData.len;
    
        protocolData.end = 0xFF;
        protocolData.start = 0x00;
        protocolData.command = 0x2;
        protocolData.len = 0x1;
        Serial.write((byte*)&protocolData, sizeof(protocolData));
    
        Serial.write(1);
        server.send(200, "text/plain", serialMessages); // Отправляем сообщения
    }
    server.send(200, "text/plain", serialMessages); // Отправляем сообщения
}

void handleGetControllers() {
    // Создание JSON-объекта
    StaticJsonDocument<200> doc; // Размер документа зависит от ваших данных
    JsonArray array = doc.to<JsonArray>();

    // Заполнение JSON-массива из массива структур
    for (int i = 0; i < kol; i++) {
      Controller& controller = controllers[i];
      JsonObject obj = array.createNestedObject(); // Создание нового объекта в массиве
      obj["id"] = controller.id;   // Добавление поля id
      obj["type"] = controller.type; // Добавление поля type
    }

    // Отправка JSON-ответа
    String response;
    serializeJson(doc, response); // Сериализация JSON-объекта в строку
    server.send(200, "application/json", response); // Отправка ответа с типом "application/json"
    #if defined(debug)
    Serial.println("handleGetControllers");
    #endif
}

void handleGetStatuses() {
    // Создание JSON-объекта
    StaticJsonDocument<200> doc; // Размер документа зависит от ваших данных
    JsonArray array = doc.to<JsonArray>();

    // Заполнение JSON-массива из массива структур
    for (int i = 0; i < kol; i++) {
        Controller& controller = controllers[i];
        if (controller.type == LEAD) continue;
        
        JsonObject obj = array.createNestedObject(); // Создание нового объекта в массиве
        obj["id"] = controller.id;   // Добавление поля id
        obj["speed"] = aimData.speed;
        obj["brake"] = aimData.brake;
        obj["left"] = aimData.left;
        obj["right"] = aimData.right;
    }

    // Отправка JSON-ответа
    String response;
    serializeJson(doc, response); // Сериализация JSON-объекта в строку
    server.send(200, "application/json", response); // Отправка ответа с типом "application/json"
}

void handlePutController() {
    if (server.hasArg("id") && server.hasArg("type")) {
        int id = server.arg("id").toInt(); // Получаем значение скорости
        int type = server.arg("type").toInt(); // Получаем значение тормозного усилия

        controllers[kol++] = {id, type};
        StaticJsonDocument<200> doc; // Размер документа зависит от ваших данных
        JsonArray array = doc.to<JsonArray>();

        JsonObject obj = array.createNestedObject(); // Создание нового объекта в массиве
        obj["id"] = id;   // Добавление поля id
        obj["type"] = type; // Добавление поля type

        // Отправляем ответ клиенту
        String response;
        serializeJson(doc, response); // Сериализация JSON-объекта в строку
        server.send(200, "application/json", response); // Отправка ответа с типом "application/json"
    } else {
        server.send(400, "text/plain", "Ошибка: недостающие параметры"); // Ответ об ошибке
    }
}

void handleDeleteController() {
    if (!server.hasArg("id")) {
        server.send(400, "text/plain", "Ошибка: недостающие параметры"); // Ответ об ошибке
        return;
    }
    
    if (kol < 2) {
        server.send(400, "text/plain", "Ошибка: удалять нечего (надо оставить хотя бы 1 контроллер)"); // Ответ об ошибке
        return;
    }
    
    int id = server.arg("id").toInt(); // Получаем значение скорости

    uint8_t pos = findElement(id);
    if (pos == -1) {
        server.send(400, "text/plain", "Ошибка: контроллер с таким id не найден"); // Ответ об ошибке
        return;
    }
    Controller controller = controllers[pos];

    for (int i = pos; i < kol - 1; i++) {
        controllers[i] = controllers[i + 1];
    }
    kol--;
    
    StaticJsonDocument<200> doc; // Размер документа зависит от ваших данных
    JsonArray array = doc.to<JsonArray>();

    JsonObject obj = array.createNestedObject(); // Создание нового объекта в массиве
    obj["id"] = controller.id;   // Добавление поля id
    obj["type"] = controller.type; // Добавление поля type

    // Отправляем ответ клиенту
    String response;
    serializeJson(doc, response); // Сериализация JSON-объекта в строку
    server.send(200, "application/json", response); // Отправка ответа с типом "application/json"
}

uint8_t findElement(uint8_t id) {
    for (int i = 0; i < kol; i++) {
        if (controllers[i].id == id) return i;
    }
    return -1;
}
