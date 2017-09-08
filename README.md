## Програма для "электронного ящика" ##
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
