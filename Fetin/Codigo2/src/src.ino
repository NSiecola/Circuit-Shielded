#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <Arduino.h>

#define LED_VERDE_LIGADO (1 << PB1)
#define LED_VERMELHO_DESLIGADO (1 << PB2)
#define SENSOR_CONVERSAO 0

float Leitura_AD;
float tensao;

void configADC()
{

    ADMUX = (1 << REFS0);                                              // Para Aref=AVcc
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler de 128
    ADCSRB = 0;                                                        // Conversão Única
}

void iniciarADC()
{

    ADMUX |= (ADMUX & 0xF8) | SENSOR_CONVERSAO; // Seleciona o canal 0
    ADCSRA |= (1 << ADSC);                      // Inicia conversão
}

int main(void)
{
    Serial.begin(9600);

    DDRB |= LED_VERDE_LIGADO | LED_VERMELHO_DESLIGADO;     // Configura o pino do LED como saída
    PORTB &= ~(LED_VERDE_LIGADO | LED_VERMELHO_DESLIGADO); // Apaga o LED

    configADC(); // Configura o ADC

    while (1)
    {
        iniciarADC(); // Inicia a conversão

        while ((ADCSRA & (1 << ADSC)) == (1 << ADSC))
            ; // Esperar a conversão

        Leitura_AD = ADC; // Armazenamento da leitura

        tensao = (Leitura_AD * 5.00) / 1023.00; // Cálculo da Tensão

        Serial.println(tensao);

        if (tensao > 1.17)
        {
            PORTB |= LED_VERDE_LIGADO;        // Acende o LED
            PORTB &= ~LED_VERMELHO_DESLIGADO; // Apaga o LED
        }
        else
        {
            PORTB &= ~LED_VERDE_LIGADO;      // Apaga o LED
            PORTB |= LED_VERMELHO_DESLIGADO; // Acende o LED
        }
    }
}
