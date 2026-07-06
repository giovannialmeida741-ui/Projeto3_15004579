/*
 Giovanni Costa de Almeida - 15004579 
 Entrega 3 - Parte 1
 */

const int PINO_VERMELHO = 25; //aqui definino os pinos dos leds
const int PINO_VERDE = 26;
const int PINO_AZUL = 27;

const int FREQUENCIA_PWM = 5000;   // 5 kHz
const int RESOLUCAO_PWM = 8;       // 8 bits
const int VALOR_MAX_PWM = 255;

// Incrementos do duty cycle, escolhi esses valores pra todos ficarem em zero em algum momento, ai o led apaga
const int PASSO_VERDE = 5;
const int PASSO_AZUL = 10;
const int PASSO_VERMELHO = 20;

// Duty cycle atual de cada cor, começa zerad obviamente
int dutyVermelho = 0;   
int dutyVerde = 0;
int dutyAzul = 0;

unsigned long ultimoTempo = 0;
const unsigned long INTERVALO_ATUALIZACAO = 200; // ms

// Converte porcentagem para valor PWM (0 a 255)
int porcentagemParaPWM(int porcentagem) {
  return (int)((porcentagem / 100.0) * VALOR_MAX_PWM);
}

// Atualiza o duty cycle de um canal
void atualizarCanal(int pino, int &dutyAtual, int incremento, const char *cor) {

  dutyAtual += incremento;

  if (dutyAtual > 100) {
    dutyAtual = 0;
  }

  int valorPWM = porcentagemParaPWM(dutyAtual);

  ledcWrite(pino, valorPWM);

  Serial.print("[");
  Serial.print(cor);
  Serial.print("] Incremento: ");
  Serial.print(incremento);
  Serial.print("% | Duty Cycle: ");
  Serial.print(dutyAtual);
  Serial.print("% | Valor PWM: ");
  Serial.println(valorPWM);
}

void setup() {

  Serial.begin(115200);
  delay(300);

  Serial.println("Projeto 3 - Parte 1 ");
  Serial.println("Controle PWM de LED RGB");
  Serial.println("Frequencia: 5 kHz");
  Serial.println("Resolucao: 8 bits");
  Serial.println("-----------------------------------------");

  ledcAttach(PINO_VERMELHO, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcAttach(PINO_VERDE, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcAttach(PINO_AZUL, FREQUENCIA_PWM, RESOLUCAO_PWM);
}

void loop() {

  unsigned long tempoAtual = millis();

  if (tempoAtual - ultimoTempo >= INTERVALO_ATUALIZACAO) {

    ultimoTempo = tempoAtual;

    atualizarCanal(PINO_VERDE, dutyVerde, PASSO_VERDE, "VERDE");
    atualizarCanal(PINO_AZUL, dutyAzul, PASSO_AZUL, "AZUL");
    atualizarCanal(PINO_VERMELHO, dutyVermelho, PASSO_VERMELHO, "VERMELHO");

    Serial.println("-----------------------------------------");
  }
}