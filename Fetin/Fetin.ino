#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LED (1 << PD1)
#define SENSOR_CONVERSAO 0

unsigned int Leitura_AD;
float tensao;

void configADC()
{
    ADMUX = (1 << REFS0);                                // Para Aref=AVcc
    ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler de 128
}

void ativaADC()
{
    ADCSRA |= (1 << ADEN);                      // Ativa o ADC
    ADMUX |= (ADMUX & 0xF8) | SENSOR_CONVERSAO; // Seleciona o canal 0
    ADCSRA |= (1 << ADSC);                      // Inicia conversão
}

int main(void)
{
    DDRD |= LED; // Configura o pino do LED como saída

    configADC(); // Configura o ADC

    while (1)
    {
        ativaADC(); // Ativa o ADC
		
		while((ADCSRA & (1<<ADSC)) == (1<<ADSC)); //Esperar a conversão
		
		Leitura_AD = ADC; //Armazenamento da leitura
		
		tensao = (Leitura_AD * 5) / 1023.0; //Cálculo da Tensão
		
        if(tensao > 2.5)
            PORTD |= LED; // Acende o LED
        else
            PORTD &= ~LED; // Apaga o LED
    }
}
