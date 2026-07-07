/*
 Giovanni Costa de Almeida - 15004579
 
  biblioteca so servo "ESP32Servo" (Kevin Harrington / madhephaestus)
 */

#include <ESP32Servo.h>

const int PINO_POTENCIOMETRO = 34;  // Entrada ADC, usei potenciometro
const int PINO_SERVO = 18;          // Saída controle para o servomotor

Servo servoMotor;

const int ADC_MAXIMO = 4095;     // resolução de 12 bits di ESP
const int ANGULO_MINIMO = 0;
const int ANGULO_MAXIMO = 180;

unsigned long ultimoTempoSerial = 0;
const unsigned long INTERVALO_SERIAL = 300; // ms entre mensagens no serial

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("=== Projeto 3 - Parte 2 (Ex.1): Servo + Potenciometro ===");

  // Configuração do timer PWM utilizado pela biblioteca ESP32Servo
  ESP32PWM::allocateTimer(0);
  servoMotor.setPeriodHertz(50);                   // servos padrão trabalham em 50 Hz
  servoMotor.attach(PINO_SERVO, 500, 2400);        // pulso min/max em microssegundos

  analogReadResolution(12); // garante leitura de 0 a 4095
}

void loop() {
  int leituraPotenciometro = analogRead(PINO_POTENCIOMETRO);
  int anguloServo = map(leituraPotenciometro, 0, ADC_MAXIMO, ANGULO_MINIMO, ANGULO_MAXIMO);
  anguloServo = constrain(anguloServo, ANGULO_MINIMO, ANGULO_MAXIMO);

  servoMotor.write(anguloServo);

  unsigned long tempoAtual = millis();
  if (tempoAtual - ultimoTempoSerial >= INTERVALO_SERIAL) {
    ultimoTempoSerial = tempoAtual;

    int dutyCycle = map(anguloServo, 0, 180, 0, 100);

    Serial.print("Potenciometro (ADC): ");
    Serial.print(leituraPotenciometro);
    Serial.print(" | Angulo: ");
    Serial.print(anguloServo);
    Serial.print(" graus | Duty cycle equivalente: ");
    Serial.print(dutyCycle);
    Serial.println("%");
  }

  delay(15); // pequena pausa para estabilidade da leitura/atuação
}