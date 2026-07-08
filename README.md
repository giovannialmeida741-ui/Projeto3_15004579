SEL0433 - Projeto 3: Controle PWM e Comunicação

Estrutura

```
projeto3/
├── parte1_rgb_pwm/                      -> Parte 1: LED RGB com LEDC
│   ├── sketch.ino
│   ├── diagram.json
│   └── wokwi.toml
├── parte2_ex1_servo_potenciometro/      -> Parte 2.1  servo e potenciômetro
│   ├── sketch.ino
│   ├── diagram.json
│   └── libraries.txt
│   └── wokwi.toml
└── parte2_ex2_mcpwm_motor/              -> Parte 2, Ex.2: motor de passo (PWM/LEDC)
    ├── sketch.ino
    ├── diagram.json
    └── wokwi.toml
    └── libraries.txt

```

Cada pasta é um projeto Wokwi independente, em formato Arduino padrão (arquivo `sketch.ino`). Para simular: abra https://wokwi.com/, crie um novo projeto ESP32 e cole o conteúdo do `sketch.ino` e do `diagram.json` correspondentes (ou use a extensão Wokwi do VS Code apontando para a pasta). (obs: não fiz com esp idf pois não estava conseguindo exibir a interface UART)

Parte 1 — LEDs com LEDC

* 3 canais PWM independentes (LEDC), 8 bits, 5 kHz.
* Duty cycle de cada canal varia de 0% a 100% em loop, com incrementos diferentes:
   * Verde: passo de 5% (base)
   * Azul: passo de 10% (2x o verde)
   * Vermelho: passo de 15% (3x o verde)
* A cada atualização, o programa envia pela UART (115200 baud) o incremento e o duty cycle aplicado em cada canal.
* Ligações: Pino 25 (led vermelho), pin 26 (Led verde), Pino 27 (led azul), cada um com resistor de 220 Ω em série até o respectivo terminal do LED RGB de catodo comum; o catodo comum vai aterrado.

Parte 2.1, Servo + potenciômetro

* Biblioteca `ESP32Servo`.
* Potenciômetro no GPIO34 (ADC), servo (fio laranja) no GPIO18 (PWM).
* A leitura do ADC (0–4095) é convertida em ângulo (0–180°), controlando diretamente a posição do servo conforme o usuário gira o potenciômetro.
* Mensagens periódicas via serial informando leitura do ADC, ângulo e duty cycle equivalente.

Parte 2.2, Aplicação própria: controle de motor de passo (via PWM/LEDC)

Aplicação: controle de velocidade e sentido de rotação de um motor de passo, usando PWM (LEDC) para gerar o trem de pulsos do pino `STEP` (a frequência do PWM define a velocidade/passos por segundo).

Recursos incorporados (além do PWM):

* ADC (potenciômetro) define a velocidade (frequência de pulsos de STEP).
* Botão com interrupção externa (GPIO4) alterna o sentido de rotação (pino `DIR` do A4988).
* Buzzer emite um bipe curto sempre que o sentido é alterado.
* Display OLED (I2C, SSD1306) mostra velocidade atual e sentido de rotação.
* Comunicação serial (115200 baud) para monitoramento contínuo do sistema.

Bibliotecas necessárias no Arduino IDE / PlatformIO:

* `Adafruit_GFX`
* `Adafruit_SSD1306`
* driver LEDC (nativo do core ESP32, incluso em `ledcAttach`/`ledcWrite`/`ledcChangeFrequency` — não precisa instalar nada)

Ligações:

* GPIO27 → `STEP` do A4988 (pulsos gerados via PWM/LEDC)
* GPIO14 → `DIR` do A4988 (sentido de rotação)
* GPIO34 → potenciômetro (velocidade)
* GPIO4 → chave slide switch — troca de sentido
* GPIO25 → buzzer
* GPIO21 (SDA) / GPIO22 (SCL) → display OLED
* `SLEEP` e `RESET` do A4988 ligados entre si (mantém o driver ativo)
* `1A`/`1B`/`2A`/`2B` do A4988 → bobinas do motor de passo (`wokwi-stepper-motor`)

A implementação original desta etapa utilizava o driver nativo mcpwm do ESP-IDF para gerar o trem de pulsos do pino STEP. O código compilava e executava sem apresentar erros. Além disso, as interrupções do botão funcionavam corretamente, o buzzer emitia o sinal sonoro na troca de sentido e o display OLED era atualizado em tempo real com o valor do ADC e o sentido de rotação. Porem, mesmo com todas essas funcionalidades operando normalmente e com as ligações idênticas às da versão funcional, o motor de passo não girava quando os pulsos eram gerados pelo MCPWM.

Após analisar o comportamento da aplicação, a explicação mais provável é que o simulador Wokwi não implemente completamente o periférico MCPWM do ESP32, os timers são simulados corretamente, mas o MCPWM é um periférico mais complexo, feito por múltiplos timers, operadores, comparadores e geradores independentes, além de recursos de sincronização, captura e proteção contra falhas. Dessa forma, embora o firmware configure o periférico normalmente e a API não retorne erros, o pino de saída provavelmente não altera seu estado na simulação, ou gera um sinal que o componente wokwi-stepper-motor não reconhece como um pulso válido de STEP, testando com um led no pino de onde deveria sair o step fica comprovado q nn sai nada, o led fica apagado, isso explica por que os demais recursos da aplicação, como ADC, interrupções, buzzer, display OLED e comunicação I2C do display, continuam funcionando normalmente, enquanto apenas a geração de pulsos pelo MCPWM não produz o efeito esperado.

Com isso substitui a geração de pulsos pelo módulo LEDC, que ja tinha funcionado nas outras parttes, utilizando as funções ledcAttach, ledcWrite e ledcChangeFrequency. Como o LEDC é totalmente suportado pelo Wokwi, o motor passou a funcionar corretamente, respeitando a velocidade definida pelo potenciômetro e o sentido de rotação determinado pela chave, sem que fosse necessário alterar a lógica da aplicação. As interrupções, o buzzer, o display OLED e a leitura do ADC permaneceram exatamente como na implementação original. Para fins de comparação, foram incluídos na pasta desta etapa tanto os arquivos de teste utilizando MCPWM, nos quais o motor não gira, quanto a versão final utilizando LEDC, que apresenta o funcionamento esperado.
