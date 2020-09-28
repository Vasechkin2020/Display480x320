#include <SPI.h>

void setup()
{
	// have to send on master in, *slave out*
	pinMode(MISO, OUTPUT);

	// turn on SPI in slave mode
	SPCR |= _BV(SPE);

	// turn on interrupts
	SPCR |= _BV(SPIE);

	Serial.begin(115200);

}
byte indx = 0;
char* p_c;
int* p_i;
byte mass[100];
byte buffer[6];

byte count = 0;

// SPI interrupt routine

ISR(SPI_STC_vect)
{
	byte data = SPDR;
	switch (count)
	{
		case 0 :
		{
			if (data == 0x1A) //Ждем первый случебный байт при нулевом счетчике
			{
				count++;                     // Увеличиваем счетчик, что значит что пришел первый байт
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
			return;
		}
		case 3:
		{
			buffer[1] = data;
			count++;
			return;
		}
		case 4:
		{
			buffer[2] = data;
			count++;
			return;
		}
		case 5:
		{
			buffer[3] = data;
			count++;
			return;
		}
		case 6:
		{
			buffer[4] = data;
			count++;
			return;
		}
		case 7:
		{
			buffer[5] = data;
			count = 0;             // Приняли все данные, начинаем ждать новый правильный байт
			int *p = (int*)(&buffer);         // Serial.print("buffer: "); Serial.println(*pI); 
			int angle = *p;
			*p++;
			int coord_X = *p;
			*p++;
			int coord_Y = *p;
			return;
		}
	}



	if (count == 0 && c == 0x1A) //Ждем первый случебный байт при нулевом счетчике
	{
		count++;                     // Увеличиваем счетчик, что значит что пришел первый байт
		return;
	}
	else
	{
		if (count == 1 && c == 0x1B)// Сюда когда был приняьт первый байт и пришел второй правильный   или что угодно
		{
			count++;                     // Увеличиваем счетчик, что значит что пришел второй байт
			return;
		}
		else
		{
			count = 0;                     // Пришел второй байт не правильный. сбрасываем счетчик и начинаем все сначала
			return;
		}

	}





	mass[indx] = c;
	indx++;
	if (indx > 1)
	{
		indx = 0;
		data = ((int16_t)mass[0] << 8) | mass[1];             // Сдвигаем старший байт влево и добавляем младший байт  // Возвращаем значение 
	 //   Serial.println(mass[0],BIN);
	 //   Serial.println(mass[1],BIN);
		Serial.println(data, BIN);
		Serial.println("---");
	}

}

void loop()
{
	//    Serial.println(mass[0],BIN);
	//    Serial.println(mass[1],BIN);
	//    Serial.println(data,BIN);
	   // Serial.println(data);
	delay(100);

}