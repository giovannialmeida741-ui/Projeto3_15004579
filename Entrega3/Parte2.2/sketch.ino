/*
 * SEL0433 - Projeto 3 - Parte 2 (Exercício 2.2
 Giovanni Costa de Almeida - 15004579
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// --- NÃO FUNCIONA adequuadamente
// #include "driver/mcpwm_timer.h"
// #include "driver/mcpwm_oper.h"
// #include "driver/mcpwm_cmpr.h"
// #include "driver/mcpwm_gen.h"

//pinos
const int PINO_STEP = 27;      // Gerado pelo MCPWM -> STEP do A4988
const int PINO_DIR = 14;       // Sentido de rotação -> DIR do A4988
const int PINO_POT = 34;       // Potenciômetro
const int PINO_BOTAO = 4;      // Botão (troca de sentido) e uma interrupção
const int PINO_BUZZER = 25;

//faixa d velocidade
const int FREQUENCIA_MINIMA = 50;    // velocidade mínima (Hz de pulsos STEP)
const int FREQUENCIA_MAXIMA = 2000;  // velocidade máxima

// --- NÃO funciona adequadamente
// // Resolução do timer MCPWM
// const uint32_t RESOLUCAO_TIMER_HZ = 1000000; // 1 tick = 1 us
//
// //Handles do MCPWM
// mcpwm_timer_handle_t temporizadorMcpwm = NULL;
// mcpwm_oper_handle_t operadorMcpwm = NULL;
// mcpwm_cmpr_handle_t comparadorMcpwm = NULL;
// mcpwm_gen_handle_t geradorMcpwm = NULL;

// --- FUNCIONAusando o ledc no lugar do mcpwm-
const int RESOLUCAO_LEDC_BITS = 8; // 8 bits -> duty de 0 a 255 (usaremos 128 = 50%)

// display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 tela(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// estado (relacionado ao sentido do motor)
volatile bool sentidoHorario = true;
volatile bool sentidoAlterado = false; // sinalizado pela ISR e tratado no loop
volatile unsigned long ultimoDebounce = 0;
const unsigned long TEMPO_DEBOUNCE = 250;

unsigned long ultimaAtualizacao = 0;
const unsigned long INTERVALO_ATUALIZACAO = 200; // ms entre atualizações de tela/serial

// ISR da chave
void IRAM_ATTR botaoPressionado() {
  unsigned long tempoAtual = millis();

  if (tempoAtual - ultimoDebounce > TEMPO_DEBOUNCE) {
    sentidoHorario = !sentidoHorario;
    sentidoAlterado = true;
    ultimoDebounce = tempoAtual;
  }
}

void tocarBuzzer() {
  tone(PINO_BUZZER, 2000, 120); // bipe curto de 2 kHz por 120 ms
}

// --- nn pega adequadamente
// // Configura o MCPWM pra dar um trem de pulsos no pino step do CI A4988
// void configurarStepMcpwm(uint32_t frequenciaInicial) {
//
//   uint32_t periodoTicks = RESOLUCAO_TIMER_HZ / frequenciaInicial;
//
//   mcpwm_timer_config_t configuracaoTimer = {};
//   configuracaoTimer.group_id = 0;
//   configuracaoTimer.clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT;
//   configuracaoTimer.resolution_hz = RESOLUCAO_TIMER_HZ;
//   configuracaoTimer.period_ticks = periodoTicks;
//   configuracaoTimer.count_mode = MCPWM_TIMER_COUNT_MODE_UP;
//   mcpwm_new_timer(&configuracaoTimer, &temporizadorMcpwm);
//
//   mcpwm_operator_config_t configuracaoOperador = {};
//   configuracaoOperador.group_id = 0;
//   mcpwm_new_operator(&configuracaoOperador, &operadorMcpwm);
//   mcpwm_operator_connect_timer(operadorMcpwm, temporizadorMcpwm);
//
//   mcpwm_comparator_config_t configuracaoComparador = {};
//   configuracaoComparador.flags.update_cmp_on_tez = true;
//   mcpwm_new_comparator(operadorMcpwm, &configuracaoComparador, &comparadorMcpwm);
//   mcpwm_comparator_set_compare_value(comparadorMcpwm, periodoTicks / 2); // 50%
//
//   mcpwm_generator_config_t configuracaoGerador = {};
//   configuracaoGerador.gen_gpio_num = PINO_STEP;
//   mcpwm_new_generator(operadorMcpwm, &configuracaoGerador, &geradorMcpwm);
//
//   // Nível alto no início do período (contagem para cima, evento "vazio")
//   mcpwm_generator_set_action_on_timer_event(
//       geradorMcpwm,
//       MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
//
//   // Nível baixo quando bater no valor do comparador (50% do período)
//   mcpwm_generator_set_action_on_compare_event(
//       geradorMcpwm,
//       MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparadorMcpwm, MCPWM_GEN_ACTION_LOW));
//
//   mcpwm_timer_enable(temporizadorMcpwm);
//   mcpwm_timer_start_stop(temporizadorMcpwm, MCPWM_TIMER_START_NO_STOP);
// }
//
// // Atualiza a frequência dos pulsos de STEP (velocidade), mantendo 50% de duty
// void definirFrequenciaStep(uint32_t frequencia) {
//
//   uint32_t periodoTicks = RESOLUCAO_TIMER_HZ / frequencia;
//
//   mcpwm_timer_set_period(temporizadorMcpwm, periodoTicks);
//   mcpwm_comparator_set_compare_value(comparadorMcpwm, periodoTicks / 2);
// }

// --- FUNCIONA  LEDC (PWM normal)
// Configura o LEDC pra dar um trem de pulsos no pino step do CI A4988
void configurarStepPwm(uint32_t frequenciaInicial) {
  ledcAttach(PINO_STEP, frequenciaInicial, RESOLUCAO_LEDC_BITS);
  ledcWrite(PINO_STEP, 128); // duty de 50% (128 de 255)
}

// Atualiza a frequência dos pulsos de STEP (velocidade), mantendo 50% de duty
void definirFrequenciaStep(uint32_t frequencia) {
  ledcChangeFrequency(PINO_STEP, frequencia, RESOLUCAO_LEDC_BITS);
  ledcWrite(PINO_STEP, 128); // garante duty de 50%
}

void setup() {

  Serial.begin(115200);
  delay(300);
  Serial.println("=== Projeto 3 - Parte 2 (Ex.2): PWM (LEDC) - Motor de Passo (A4988) ===");

  pinMode(PINO_DIR, OUTPUT);
  pinMode(PINO_BOTAO, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PINO_BOTAO), botaoPressionado, FALLING);

  pinMode(PINO_BUZZER, OUTPUT);
  analogReadResolution(12);

  // configurarStepMcpwm(FREQUENCIA_MINIMA); // NÃO FUNCIONA NO WOKWI
  configurarStepPwm(FREQUENCIA_MINIMA); // FUNCIONA NO WOKWI

  digitalWrite(PINO_DIR, sentidoHorario ? HIGH : LOW);

  // Display OLED
  Wire.begin(21, 22);

  if (!tela.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {

    Serial.println("Falha ao iniciar o display OLED");

  } else {

    tela.clearDisplay();
    tela.setTextSize(1);
    tela.setTextColor(SSD1306_WHITE);
    tela.setCursor(0, 0);
    tela.println("Motor de Passo");
    tela.println("PWM + A4988");
    tela.display();
  }
}

void loop() {

  int leituraPotenciometro = analogRead(PINO_POT);

  int frequenciaPassos = map(leituraPotenciometro, 0, 4095,
                             FREQUENCIA_MINIMA, FREQUENCIA_MAXIMA);

  digitalWrite(PINO_DIR, sentidoHorario ? HIGH : LOW);

  if (sentidoAlterado) {

    sentidoAlterado = false;
    tocarBuzzer();

    Serial.println("___________________Sentido de rotacao alterado!________");
  }

  unsigned long tempoAtual = millis();

  if (tempoAtual - ultimaAtualizacao >= INTERVALO_ATUALIZACAO) {

    ultimaAtualizacao = tempoAtual;

    definirFrequenciaStep(frequenciaPassos);

    int porcentagemVelocidade = map(frequenciaPassos,
                                    FREQUENCIA_MINIMA,
                                    FREQUENCIA_MAXIMA,
                                    0,
                                    100);

    Serial.print("Velocidade: ");
    Serial.print(frequenciaPassos);
    Serial.print(" passos/s (");
    Serial.print(porcentagemVelocidade);
    Serial.print("%) | Sentido: ");
    Serial.println(sentidoHorario ? "Horario" : "Anti-horario");

    tela.clearDisplay();
    tela.setCursor(0, 0);
    tela.println("Motor de Passo");
    tela.print("Vel: ");
    tela.print(frequenciaPassos);
    tela.println(" pps");
    tela.print("(");
    tela.print(porcentagemVelocidade);
    tela.println(" %)");
    tela.print("Sentido: ");
    tela.println(sentidoHorario ? "Horario" : "Anti-hor.");
    tela.display();
  }
}
