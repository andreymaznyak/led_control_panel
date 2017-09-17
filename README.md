## Програма для "электронного ящика" ##
Система состоит из 3 частей:
1. Микроконтроллер, на базе esp8266 в формате nodemcu amica  
Цепочки микросхем Maxim7219 подключены с помощью SPI интерфейса
к контроллеру, по 3 SPI пина на каждую цепочку  
Распиновка:  
![](https://github.com/andreymaznyak/led_control_panel/blob/master/docs/img/NodeMCU-amica-pinout.png)
1. Две цепочки подследовательно подключенных микросхем Maxim7219  
Одна цепочка включает в себя 4 платы -> 8 микросхем Maxim7219 -> 16 цифровых табло,
другая 2 платы -> 4 микросхемы -> 8 цифровых табло.
На одной плате 2 микросхемы, каждая микросхема управляет 2
семисегметными 4х циферными табло, с общим катодом.
Я распаял печатные платы по 2 микросхемы Maxim7219 для последовательного подключения.  
Схема плат:  
![](https://github.com/andreymaznyak/led_control_panel/blob/master/docs/img/MAX7219-circuit.GIF)  
1. Матрица кнопок

Конфиги пинов для матрицы кнопок, матрица кнопок управляется помощью
расширителя портов PCF8574AP и сдвигового регистра SN72HC595

Для управления светодиодными табло используется 2 последовательно подключенных линии
микросхем Maxim7219, в одной линии 8 микросхем, в другой 4.

Всего 24 7-сегметных, 4х-циферных светодиодных табло  
распиновка:  
__ __PCF  
A0    [| U |] ++++  
A1    [|   |] sda <->  
A2    [|   |] scl <->  
IO-0  [|   |] INT ->  
IO-1  [|   |] IO-7  
IO-2  [|   |] IO-6  
IO-3  [|   |] IO-5  
GND   [|___|] IO-4  

SN72HC595  
OUT 1  [|  U  |] ++++  
OUT 2  [|     |] OUT 0  
OUT 3  [|     |] DATA <->  
OUT 4  [|     |] ENABLE -> GND  
OUT 5  [|     |] LATCH <->  
OUT 6  [|     |] CLOCK <->  
OUT 7  [|     |] RESET -> GND  
GND   [|_____|] OVERFLOW  
