#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define RED_PIN 15
#define GREEN_PIN 12
#define BLUE_PIN 13

#define LDR_PIN A0

const char* ssid = "TP-LINK_44";         // Назва Wi-Fi мережі
const char* password = "0936461022"; // Пароль


ESP8266WebServer server(80);  // Створюємо сервер на порті 80

//html Змінна для відправки на сторінку

const char* html = R"====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>RGB & Joystick Controller</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
            margin: 0;
            padding: 0;
        }
        .container {
            max-width: 600px;
            margin: auto;
            padding: 20px;
            text-align: center;
        }
        h1 {
            color: #333;
        }
        a {
            display: inline-block;
            margin: 10px 5px;
            padding: 10px 20px;
            text-decoration: none;
            color: white;
            background-color: #007BFF;
            border-radius: 5px;
        }
        a:hover {
            background-color: #0056b3;
        }
        input[type="color"] {
            padding: 5px;
            margin-top: 10px;
            border: none;
            outline: none;
            border-radius: 5px;
            width: 100%;
            height: 40px;
            cursor: pointer;
            background-color: #f0f0f0;
            box-shadow: inset 0 0 5px rgba(0, 0, 0, 0.2);
        }
        button {
            padding: 10px 20px;
            margin-top: 10px;
            background-color: #28a745;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }
        button:hover {
            background-color: #218838;
        }
        .sensor {
            margin-top: 20px;
            color: #555;
        }
        .joystick-container {
            display: flex;
            justify-content: space-around;
            flex-wrap: wrap;
            margin-top: 20px;
        }
        .joystick {
            width: 150px;
            height: 150px;
            position: relative;
            margin: 10px;
        }
        .color-label {
            margin-top: 10px;
            font-size: 16px;
            color: #333;
        }
    </style>
    <!-- Підключаємо бібліотеку nipple.js -->
    <script src="https://cdn.jsdelivr.net/npm/nipplejs@0.7.3/dist/nipplejs.min.js"></script>
</head>
<body>
    <div class="container">
        <h1>RGB & Joystick Controller</h1>
        <div>
            <a href='/led1/on'>Turn LED ON</a>
            <a href='/led1/off'>Turn LED OFF</a>
        </div>
        <div>
            <label class="color-label" for="colorPicker">Pick a color:</label>
            <input type="color" id="colorPicker" value="#ff0000">
        </div>
        <div class="sensor">
            <h2>Light Sensor</h2>
            <p>Value: <span id="ldr-value"></span></p>
        </div>

        <!-- Joystick Controls -->
        <p id='coordinates'>X: 0, Y: 0</p> 
        <div id='joystick' style='margin-right: 40px; width: 300px; height: 300px; background-color: #ccc; position: relative; border-radius: 50%; overflow: hidden;'>
        <div id='stick' style='width: 80px; height: 80px; background-color: rgb(0, 0, 0); position: absolute; top: 110px; left: 110px; border-radius: 50%;'></div>
   
    </div>

    <script>
            let j1x = 0;
                let j1y = 0;
                let j2x = 0;
                let j2y = 0;
    window.addEventListener('DOMContentLoaded', () => {



                const openButton = document.getElementById('openButton');
                const closeButton = document.getElementById('closeButton');
                const slider = document.getElementById('slider');
            const joystick = document.getElementById('joystick');
            const stick = document.getElementById('stick');
            const coordinates = document.getElementById('coordinates');
            let isDragging = false;
            let interactionType;
        
        
        if ('ontouchstart' in window || navigator.maxTouchPoints > 0 || navigator.msMaxTouchPoints > 0) {
            interactionType = 'touch';
            stick.addEventListener('touchstart', handleStart);
            document.addEventListener('touchmove', handleMove);
            document.addEventListener('touchend', handleEnd);
            } else {
            interactionType = 'mouse';
            stick.addEventListener('mousedown', handleStart);
            document.addEventListener('mousemove', handleMove);
            document.addEventListener('mouseup', handleEnd);
            }
            const joystickRadius = joystick.clientWidth / 2;
            const stickRadius = stick.clientWidth / 2;
            function handleStart(event) {
            event.preventDefault();
            isDragging = true;
            disableScroll();
            }
        
        
        let moveCounter = 0;

        function handleMove(event) {
        if (isDragging) {
            const touch = interactionType === 'touch' ? event.touches[0] : event;
            const rect = joystick.getBoundingClientRect();
            const x = touch.clientX - rect.left - joystickRadius;
            const y = touch.clientY - rect.top - joystickRadius;
            const distanceFromCenter = Math.sqrt(x * x + y * y);
            const angle = Math.atan2(y, x);
            const maxDistance = joystickRadius - stickRadius;
            const clampedDistance = Math.min(distanceFromCenter, maxDistance);
            const offsetX = Math.round((Math.cos(angle) * clampedDistance / maxDistance) * 255);
            const offsetY = Math.round((Math.sin(angle) * clampedDistance / maxDistance) * 255);
            stick.style.left = `${Math.round(offsetX / 255 * maxDistance) + joystickRadius - stickRadius}px`;
            stick.style.top = `${Math.round(offsetY / 255 * maxDistance) + joystickRadius - stickRadius}px`;
            coordinates.textContent = `X: ${offsetX}, Y: ${offsetY}`;

            // Increment the move counter
            moveCounter++;
            // Send coordinates every second time
            if (moveCounter % 2 === 0) {
            j1x = offsetX;
            j1y = offsetY;
            }
        }
        }



            function handleEnd() {
            if (isDragging) {
                isDragging = false;
                stick.style.left = `${joystickRadius - stickRadius}px`;
                stick.style.top = `${joystickRadius - stickRadius}px`;
                coordinates.textContent = `X: 0, Y: 0`;
                enableScroll();
            }
            }
            function disableScroll() {
            document.body.style.overflow = 'hidden';
            }
            function enableScroll() {
            document.body.style.overflow = 'auto';
            }
            console.log(`Interaction Type: ${interactionType}`);
            setInterval(() => {
            const currentX = parseInt(coordinates.textContent.split(',')[0].split('X:')[1]) || 0;
            const currentY = parseInt(coordinates.textContent.split(',')[1].split('Y:')[1]) || 0;
            j1x = currentX;
            j1y = currentY;
            }, 200); // Send coordinates every 200 ms
        
        
                openButton.addEventListener('click', () => {
                sendButtonValue(1);
                });
        
                closeButton.addEventListener('click', () => {
                sendButtonValue(0);
                });
        
                slider.addEventListener('input', () => {
                const sliderValue = slider.value;
                sendSliderValue(sliderValue);
                });
        
        
            });


        setInterval(() => {
            fetch('/joystick?id=1&x=' + j1x + '&y=' + j1y, { method: 'GET' });
            // fetch('/joystick?id=2&x=' + j2x + '&y=' + j2y, { method: 'GET' });
        }, 300);

      // Обробка кольорів з колірного пікера
      const colorPicker = document.getElementById('colorPicker');

      colorPicker.addEventListener('input', () => {
        const color = colorPicker.value;
        const r = parseInt(color.substr(1, 2), 16);
        const g = parseInt(color.substr(3, 2), 16);
        const b = parseInt(color.substr(5, 2), 16);
        sendColorData(r, g, b);
      });

      // Функція для відправки кольору на сервер
      function sendColorData(r, g, b) {
        const url = `/rgb?r=${r}&g=${g}&b=${b}`;
        fetch(url, { method: 'GET' })
          .then(response => response.text())
          .then(data => console.log('Color updated:', data))
          .catch(error => console.error('Error:', error));
      }

    //   Автоматичне оновлення значення сенсора світла
      setInterval(() => {
        fetch('/ldr')
          .then(response => response.json())
          .then(data => {
            document.getElementById('ldr-value').innerText = data.value;
          });
      }, 500);

   
    </script>
</body>
</html>
)====";



int pinsMotor1[] = {14, 16};
int pinsMotor2[] = {4, 5};

// Задача контрювати двигуни відповідно до значень x та y від джойстика
// x, y - значення від -255 до 255
// Y - вперед/назад
// X - ліво/право
// Якщо x або y дорівнює 0, то двигуни не рухаються
void controlMotors(int x, int y){
  int speed1 = y + x;
  int speed2 = y - x;
  speed1 = constrain(speed1, -255, 255);
  speed2 = constrain(speed2, -255, 255);
  if(speed1 > 0){
    analogWrite(pinsMotor1[0], speed1);
    analogWrite(pinsMotor1[1], 0);
  }else{
    analogWrite(pinsMotor1[0], 0);
    analogWrite(pinsMotor1[1], -speed1);
  }
  if(speed2 > 0){
    analogWrite(pinsMotor2[0], speed2);
    analogWrite(pinsMotor2[1], 0);
  }else{
    analogWrite(pinsMotor2[0], 0);
    analogWrite(pinsMotor2[1], -speed2);
  }
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Встановлюємо пін LED_BUILTIN як вихідний
  pinMode(RED_PIN, OUTPUT);  // Встановлюємо пін RED_PIN як вихідний
  pinMode(GREEN_PIN, OUTPUT);  // Встановлюємо пін GREEN_PIN як вихідний
  pinMode(BLUE_PIN, OUTPUT);  // Встановлюємо пін BLUE_PIN як вихідний

  Serial.begin(115200);  // Починаємо серійну комунікацію
  WiFi.begin(ssid, password);  // Підключаємось до Wi-Fi

  // Чекаємо підключення
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Виводимо IP-адресу, яку отримала плата ESP8266
  Serial.println("");
  Serial.print("Connected to WiFi. IP Address: ");
  Serial.println(WiFi.localIP());

  // Встановлюємо обробник для шляху /
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", html);
  });
  
  // Встановлюємо обробник для шляху /led/on
  server.on("/led1/on", HTTP_GET, []() {
    digitalWrite(LED_BUILTIN, HIGH);  // Вмикаємо світлодіод
    // Перенаправляємо користувача на головну сторінку
    server.sendHeader("Location", "/");
    server.send(303);
  });

  // Встановлюємо обробник для шляху /led/off
  server.on("/led1/off", HTTP_GET, []() {
    digitalWrite(LED_BUILTIN, LOW);  // Вимикаємо світлодіод
    // Перенаправляємо користувача на головну сторінку
    server.sendHeader("Location", "/");
    server.send(303);
  });

  // Встановлюємо обробник для шляху /rgb
  server.on("/rgb", HTTP_GET, []() {
    // Отримуємо значення параметрів r, g, b
    int r = server.arg("r").toInt();
    int g = server.arg("g").toInt();
    int b = server.arg("b").toInt();
    // Виводимо значення параметрів
    Serial.printf("r: %d, g: %d, b: %d\n", r, g, b);
    // Вмикаємо світлодіоди з відповідними кольорами
    r = constrain(r, 0, 255);// обмежуємо значення від 0 до 255
    g = constrain(g, 0, 255);// обмежуємо значення від 0 до 255
    b = constrain(b, 0, 255);// обмежуємо значення від 0 до 255
    analogWrite(RED_PIN, r);
    analogWrite(GREEN_PIN, g);
    analogWrite(BLUE_PIN, b);

    // Перенаправляємо користувача на головну сторінку
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.on("/ldr", HTTP_GET, []() {
    int value = analogRead(LDR_PIN);
    server.send(200, "application/json", "{\"value\": " + String(value) + "}");
  });

  // Встановлюємо обробник для шляху /joystick
  server.on("/joystick", HTTP_GET, []() {
    // Отримуємо значення параметрів id, x, y
    int id = server.arg("id").toInt();
    int x = server.arg("x").toInt();
    int y = server.arg("y").toInt();
    // Виводимо значення параметрів
    Serial.printf("Joystick %d: x: %d, y: %d\n", id, x, y);
    // Контролюємо двигуни відповідно до значень x та y
    controlMotors(x, y);
    // Відправляємо відповідь
    server.send(200, "application/json", "{\"status\": \"ok\"}");
  });

  server.begin();  // Запускаємо сервер

}

void loop() {
  server.handleClient();  // Обробляємо 
}


