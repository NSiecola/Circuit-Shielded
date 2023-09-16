#include <avr/io.h>
#include <avr/interrupt.h>

#define VOLTAGEM_ESTAVEL_BAIXA 110  // 1,1V
#define VOLTAGEM_ESTAVEL_ALTA 130   // 1,3V
#define VOLTAGEM_INSTAVEL_BAIXA 200 // 2,0V
#define VOLTAGEM_INSTAVEL_ALTA 230  // 2,3V
#define LED_VERDE_LIGA (1 << PB1) // PB1 = LED verde
#define LED_VERMELHO_DESLIGA (1 << PB2) // PB2 = LED vermelho
#define RELE_PIN (1 << PB3) // PB3 = Relé

volatile uint8_t voltagemEstavel = 0;
volatile uint8_t faixaSelecionada = 0;

void initADC()
{
  // Define a tensão de referência como AVCC e ajusta o resultado para a esquerda
  ADMUX = (1 << REFS0) | (1 << ADLAR);

  // Habilita o ADC, ativa a interrupção do ADC e define o pré-divisor do ADC como 128
  ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

void iniciarConversaoADC(uint8_t canal)
{
  ADMUX = (ADMUX & 0xF8) | (canal & 0x07); // Seleciona o canal de conversão
  ADCSRA |= (1 << ADSC);                   // Inicia a conversão
}

void initInterrupcaoZeroCross()
{
  // Configura a interrupção externa em INT0 (PD2)
  EICRA |= (1 << ISC01); // Gatilho de borda de descida
  EIMSK |= (1 << INT0);  // Habilita a interrupção externa INT0
}

ISR(ADC_vect)
{
  uint16_t valorADC = ADC;

  if (faixaSelecionada == 1 && valorADC >= VOLTAGEM_ESTAVEL_BAIXA && valorADC <= VOLTAGEM_ESTAVEL_ALTA)
  {
    voltagemEstavel = 1;
  }
  else if (faixaSelecionada == 2 && valorADC >= VOLTAGEM_INSTAVEL_BAIXA && valorADC <= VOLTAGEM_INSTAVEL_ALTA)
  {
    voltagemEstavel = 1;
  }
  else
  {
    voltagemEstavel = 0;
  }
}

ISR(INT0_vect)
{
  // Evento de cruzamento zero, alterna o relé
  PORTB ^= (1 << PB3); // Alterna o estado do relé
}

int main()
{
  // Configura os pinos digitais
  DDRB |= LED_VERDE_LIGA | LED_VERMELHO_DESLIGA | RELE_PIN; // Define como saída
  PORTB &= ~(LED_VERDE_LIGA | LED_VERMELHO_DESLIGA | RELE_PIN); // Desliga os LEDs e o relé inicialmente

  DDRD &= ~(1 << PD4); // Define PD4 como entrada (sensor de voltagem)
  PORTD |= (1 << PD4); // Habilita o pull-up interno

  DDRD &= ~(1 << PD2); // Define PD2 como entrada (detector de cruzamento zero)
  PORTD |= (1 << PD2); // Habilita o pull-up interno

  initADC();
  initInterrupcaoZeroCross();

  sei(); // Habilita as interrupções globais
  while (1)
  {
    // Verifica a estabilidade da voltagem e a faixa selecionada
    if (voltagemEstavel)
    {
      PORTB |= (1 << PB1);  // Liga o LED verde
      PORTB &= ~(1 << PB2); // Desliga o LED vermelho
      PORTB |= (1 << PB3);  // Liga o relé
    }
    else
    {
      PORTB &= ~(1 << PB1); // Desliga o LED verde
      PORTB |= (1 << PB2);  // Liga o LED vermelho
      PORTB &= ~(1 << PB3); // Desliga o relé
    }

    // Verifica o botão de seleção da faixa de voltagem (P1.2)
    if (PIND & (1 << PD2))
    {
      faixaSelecionada = 1;
    }
    else
    {
      faixaSelecionada = 2;
    }

    // Lê a voltagem usando o ADC (P1.4)
    iniciarConversaoADC(4);

    // Seu código principal aqui
  }
  return 0;
}
