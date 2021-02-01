//#include <SPI.h>

long time_start = 0;
volatile byte buffer[7]; // 6 байт данных и один контрольная сумма
volatile byte count = 0;
volatile byte data = 0;
volatile byte flag = 0;

struct Koordinat			  // структура для хранения данных получаемых с лидара 
{
	int x;		   // координата по х для экрана 480*320  которые передаем в экран
	int y;		    // координата по У для экрана 480*320 которые передаем в экран
};

volatile Koordinat Massiv[720];    //Создаем массив из расчета в 0,5 градуса


volatile int angle_okrugl;		   //Угол для которого пришли координаты. становится номером ячейки в массиве
volatile int coord_X;
volatile int coord_Y;
volatile byte chek_sum = 0;   // Байт контрольной суммы
//test

//=================================================================================================================
// This program requires the UTFT library.
//

//################################################
// GLUE class that implements the UTFT API
// replace UTFT include and constructor statements
// remove UTFT font declaration e.g. SmallFont
//################################################

#include <UTFTGLUE.h>              //use GLUE class and constructor
UTFTGLUE myGLCD(0, A2, A1, A3, A4, A0); //all dummy args

void setup()
{
	// have to send on master in, *slave out*
	pinMode(MISO, OUTPUT);

	// turn on SPI in slave mode
	SPCR |= 1<<(SPE);

	// turn on interrupts
	SPCR |= 1<<(SPIE);

	Serial.begin(115200);
	time_start = millis(); // Время старта программы

		// Setup the LCD
	myGLCD.InitLCD();
	myGLCD.setFont(SmallFont);
	randomSeed(analogRead(0));
	myGLCD.clrScr();
	myGLCD.setColor(255, 255, 0);
	myGLCD.fillRoundRect(226, 153, 254, 167);
	
	for (int i = 0; i < 720; i++)	 //Обнуляем массив 
	{
		Massiv[angle_okrugl].x = 0;	 
		Massiv[angle_okrugl].y = 0;
	}


}

// SPI interrupt routine


ISR(SPI_STC_vect)
{
	data = SPDR;
	switch (count)
	{
		case 0 :
		{
			if (data == 0x1A) //Ждем первый случебный байт при нулевом счетчике
			{
				count++;                     // Увеличиваем счетчик, что значит что пришел первый байт
				SPDR = flag;        // Записываем в регистр для передачи статус обработки . Успели ли обработать предыдущую партию
				return;
			}
		}
		case 1 :
		{
			if (data == 0x1B) // Сюда когда был приняьт первый байт и пришел второй правильный
			{
				count++;                     // Увеличиваем счетчик, что значит что пришел второй байт
				return;
			}
			else
			{
				count = 0;                         // Пришел второй байт не правильный. сбрасываем счетчик и начинаем все сначала
				return;
			}
		}
		case 2:
		{
			buffer[0] = data;
			count++;
			chek_sum += data;			 // Суммирем все байты при перевышении остается младший бакт его и сравниваем
			return;
		}
		case 3:
		{
			buffer[1] = data;
			count++;
			chek_sum += data;			 // Суммирем все байты при перевышении остается младший бакт его и сравниваем
			return;
		}
		case 4:
		{
			buffer[2] = data;
			count++;
			chek_sum += data;			 // Суммирем все байты при перевышении остается младший бакт его и сравниваем
			return;
		}
		case 5:
		{
			buffer[3] = data;
			count++;
			chek_sum += data;			 // Суммирем все байты при перевышении остается младший бакт его и сравниваем
			return;
		}
		case 6:
		{
			buffer[4] = data;
			count++;
			chek_sum += data;			 // Суммирем все байты при перевышении остается младший бакт его и сравниваем
			return;
		}
		case 7:
		{
			buffer[5] = data;
			count++;            
			chek_sum += data;			 // Суммирем все байты при перевышении остается младший бакт его и сравниваем
			return;
		}
		case 8:
		{
			buffer[6] = data;
			count = 0;             // Приняли все данные, начинаем ждать новый правильный байт
			Serial.print(data);
			Serial.print(" ");
			Serial.println(chek_sum);

			flag = 1;				//Включаем обработку в лупе
			return;
		}

	}






	//mass[indx] = c;
	//indx++;
	//if (indx > 1)
	//{
	//	indx = 0;
	//	data = ((int16_t)mass[0] << 8) | mass[1];             // Сдвигаем старший байт влево и добавляем младший байт  // Возвращаем значение 
	// //   Serial.println(mass[0],BIN);
	// //   Serial.println(mass[1],BIN);
	//	Serial.println(data, BIN);
	//	Serial.println("---");
	//}

}

void loop()
{
	if (flag == 1)
	{
		//if (buffer[6] != chek_sum)
		//{
		//	Serial.println(".");
		//}
		//else
		//{
		//	Serial.println("+");
		//}
		int* p = (int*)(&buffer);        // обьявляем переменную
		angle_okrugl = *p++;
		coord_X = *p++;
		coord_Y = *p;

		// Clear the screen and draw the frame
		long a = micros();
		myGLCD.setColor(0, 0, 0); 
		myGLCD.drawPixel(Massiv[angle_okrugl].x, Massiv[angle_okrugl].y);	  //Стираем старую точку
		long b = micros();
		if (coord_X >= 0 && coord_X <= 480 && coord_Y >= 0 && coord_Y <= 320 )	// Если координаты не выходят за границы 
		{
			Massiv[angle_okrugl].x = coord_X;	   //  Сохраняем координаты для этого угла чтобы потом когда придую снова координаты знать какую точку стирать
			Massiv[angle_okrugl].y = coord_Y;

			myGLCD.setColor(255, 0, 255);
			myGLCD.drawPixel(Massiv[angle_okrugl].x, Massiv[angle_okrugl].y);	  //Рисуем новую точку для этого угла
		}

		chek_sum = 0;
		flag = 0 ;

		//long c = micros();
		//Serial.print(b-a);
		//Serial.print(" ");
		//Serial.print(c - b);
		//
		//Serial.print(" ");
		//Serial.print(Massiv[angle_okrugl].x);
		//Serial.print(" ");
		//Serial.print(Massiv[angle_okrugl].y);
		//Serial.println("=");

		//for (int i = 0; i < 720; i++)
		//{

		//	Serial.print("angle ");
		//	Serial.print(i);
		//	Serial.print(" coord_X ");
		//	Serial.print(Massiv[i].x);
		//	Serial.print(" coord_Y ");
		//	Serial.print(Massiv[i].y);

		//	Serial.println(" = ");
		//}

		//delay(9999999999999);
	}
}


