# Projeto3_15004579
Entrega do projeto 3, todas as partes obrigatórias estão aqui, são 3 partes, a primeira é usando pwm para os leds, a segunda utilizando o potenciometro para controlar o servo motor, e a terceira é a aplicação, abaixo uma explicação mais detalhada de cada uma.

-------------- SEL0433 - Projeto 3: Controle PWM e Comunicação=----------------------pg---------------

## Estrutura

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
└── parte2_ex2_mcpwm_motor/              -> Parte 2, Ex.2: motor DC com MCPWM
    ├── sketch.ino
    ├── diagram.json
    └── wokwi.toml
```

Cada pasta é um projeto Wokwi independente, em formato Arduino padrão (arquivo
`sketch.ino`). Para simular: abra https://wokwi.com/, crie um novo projeto ESP32
e cole o conteúdo do `sketch.ino` e do `diagram.json` correspondentes (ou use a
extensão Wokwi do VS Code apontando para a pasta). (obs: não fiz com esp idf pois não estava conseguindo exibir a interface UART)

Parte 1 — LEDs com LEDC

- 3 canais PWM independentes (LEDC), 8 bits, 5 kHz.
- Duty cycle de cada canal varia de 0% a 100% em loop, com incrementos diferentes:
  - Verde: passo de 5% (base)
  - Azul: passo de 10% (2x o verde)
  - Vermelho: passo de 15% (3x o verde)
- A cada atualização, o programa envia pela UART (115200 baud) o incremento e o
  duty cycle aplicado em cada canal.
- Ligações: Pino 25 (led vermelho), pin 26 (Led verde), Pino 27 (led azul), cada um com resistor de 220 Ω em
  série até o respectivo terminal do LED RGB de catodo comum; o catodo comum vai
  aterrado.

## Parte 2.1, Servo + potenciômetro

- Biblioteca `ESP32Servo`.
- Potenciômetro no GPIO34 (ADC), servo (fio laranja) no GPIO18 (PWM).
- A leitura do ADC (0–4095) é convertida em ângulo (0–180°), controlando
  diretamente a posição do servo conforme o usuário gira o potenciômetro.
- Mensagens periódicas via serial informando leitura do ADC, ângulo e duty cycle
  equivalente.
  
## Parte 2.2, Aplicação própria com MCPWM (motor DC)

Aplicação: controle de velocidade e sentido de rotação de um motor de
passo, usando a biblioteca nativa **MCPWM** para gerar o trem de pulsos do
pino `STEP` (a frequência do MCPWM define a velocidade/passos por segundo).

Recursos incorporados (além do PWM via MCPWM):
- ADC (potenciômetro) define a velocidade (frequência de pulsos de STEP).
- Botão com interrupção externa (GPIO4) alterna o sentido de rotação
  (pino `DIR` do A4988).
- Buzzer emite um bipe curto sempre que o sentido é alterado.
- Display OLED (I2C, SSD1306) mostra velocidade atual e sentido de rotação.
- Comunicação serial (115200 baud) para monitoramento contínuo do sistema.

Bibliotecas necessárias no Arduino IDE / PlatformIO:
- `Adafruit_GFX`
- `Adafruit_SSD1306`
- `driver/mcpwm.h` (nativa do core ESP32, não precisa instalar)

Ligações:
- GPIO27 → `STEP` do A4988 (pulsos gerados via MCPWM)
- GPIO14 → `DIR` do A4988 (sentido de rotação)
- GPIO34 → potenciômetro (velocidade)
- GPIO4 → chave slide switch — troca de sentido
- GPIO25 → buzzer
- GPIO21 (SDA) / GPIO22 (SCL) → display OLED
- `SLEEP` e `RESET` do A4988 ligados entre si (mantém o driver ativo)
- `1A`/`1B`/`2A`/`2B` do A4988 → bobinas do motor de passo (`wokwi-stepper-motor`)

Obs: a parte 2.2 nao funcionou adequadamente, pelo que li MCPWM não funciona direito com o driver A4988 que vem no wokwi, que eu tentie usar para controlar o motr de passo, eu deixei aqui um codigo que teste que mostra que as ligações estao corretas, e uma versao sem mcpwm, com ela o motor gira, mas com a versão final tudo funciona, interrupções, buzzer, display, menos o motor, o display mostra os valores do adc sendo processados corretamente, e exibe a troca de sentido com a chave slide switch
