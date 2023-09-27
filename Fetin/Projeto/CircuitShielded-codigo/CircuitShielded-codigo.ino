#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <Arduino.h>

#define LED_VERDE_LIGADO (1 << PB1)       // Define o pino do LED para o estado de ligado como PB1
#define LED_VERMELHO_DESLIGADO (1 << PB2) // Define o pino do LED para o estado de desligado como PB2
#define SENSOR_CONVERSAO 0                // Define o pino do sensor de conversão como PC0
#define LEITURA_AD (1 << PB3)             // Define o pino de leitura do ADC como PB3
#define RELAY (1 << PB4)                  // Define o pino do Relay como PB4
#define ZERO_CROSS (1 << PB5)             // Define o pino de entrada do cruzamento por zero como P1.5

float Leitura_AD;   // Variável para armazenar a leitura do ADC
float tensao = 0;   // Variável para armazenar a tensão
float media;        // Variável para armazenar a média da tensão
float tensao_int;   // Variável para armazenar a tensão instantânea
int amostras = 100; // Variável para definir a quantidade de amostras

// Função para configurar o ADC
void configADC()
{
    ADMUX = (1 << REFS0);                                              // Para Aref=AVcc
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler de 128
    ADCSRB = 0;                                                        // Conversão Única
}

// Função para iniciar a conversão
void iniciarADC()
{
    ADMUX |= (ADMUX & 0xF8) | SENSOR_CONVERSAO; // Seleciona o canal 0
    ADCSRA |= (1 << ADSC);                      // Inicia conversão
}

int main(void)
{
    // Serial.begin(9600); // Inicia a comunicação serial

    DDRB |= LED_VERDE_LIGADO | LED_VERMELHO_DESLIGADO | RELAY;     // Configura o pino do LED como saída
    PORTB &= ~(LED_VERDE_LIGADO | LED_VERMELHO_DESLIGADO | RELAY); // Apaga o LED

    configADC(); // Configura o ADC

    // Configura o pino do zero crossing como entrada
    DDRB &= ~ZERO_CROSS;

    // Ativa o resistor de pull-up para a entrada do zero crossing
    PORTB |= ZERO_CROSS;

    while (1)
    {

        // Aguarda o zero crossing
        while (PINB & ZERO_CROSS)
            ;

        tensao = 0;

        for (int i = 0; i < amostras; i++)
        {

            iniciarADC(); // Inicia a conversão

            while ((ADCSRA & (1 << ADSC)) == (1 << ADSC))
                ; // Esperar a conversão

            Leitura_AD = ADC; // Armazenamento da leitura

            tensao_int = (Leitura_AD * 3.00) / 1023.00; // Cálculo da Tensão

            tensao += tensao_int;
        }

        media = tensao / amostras;

        // Serial.println(media);
        // _delay_ms(500);

        if (PINB & LEITURA_AD)
        {
            if (media > 0.410 && media < 0.520)
            {
                //     Serial.println("Luz Ligada");
                PORTB |= LED_VERDE_LIGADO;        // Acende o LED
                PORTB &= ~LED_VERMELHO_DESLIGADO; // Apaga o LED
                _delay_ms(5);
                PORTB |= RELAY; // Liga o Relay
            }
            else
            {
                // Serial.println("Luz Desligada");
                PORTB &= ~LED_VERDE_LIGADO;      // Apaga o LED
                PORTB |= LED_VERMELHO_DESLIGADO; // Acende o LED
                PORTB &= ~RELAY;                 // Desliga o Relay
            }
        }
        else
        {
            if (media > 1.040 && media < 1.290)
            {
                // Serial.println("Luz Ligada");
                PORTB |= LED_VERDE_LIGADO;        // Acende o LED
                PORTB &= ~LED_VERMELHO_DESLIGADO; // Apaga o LED
                _delay_ms(5);
                PORTB |= RELAY; // Liga o Relay
            }
            else
            {
                // Serial.println("Luz Desligada");
                PORTB &= ~LED_VERDE_LIGADO;      // Apaga o LED
                PORTB |= LED_VERMELHO_DESLIGADO; // Acende o LED
                PORTB &= ~RELAY;                 // Desliga o Relay
            }
        }
    }
}
