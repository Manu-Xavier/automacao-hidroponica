# Automação Hidropônica

Sistema de automação para cultivo hidropônico baseado em Arduino Nano, que monitora a condutividade elétrica (EC) da solução nutritiva e controla bombas dosadoras e irrigação automaticamente.

## Funcionalidades

- Leitura de condutividade elétrica via sensor EC-TDS (protocolo Modbus serial, ver `EC-TDS_485 manual.pdf`).
- Exibição em display LCD (I2C) dos valores lidos e dos limites configurados.
- Controle proporcional de duas bombas dosadoras (`bomba1`, `bomba2`) com base em limites (setpoints) lidos de entradas analógicas.
- Acionamento de relé de irrigação por entrada digital.
- Watchdog timer (8s) para reiniciar o sistema em caso de travamento.

## Hardware

- **Placa:** Arduino Nano (ATmega328)
- **Display:** LCD 20x4 via módulo I2C (endereço `0x20`)
- **Sensor:** condutivímetro EC-TDS via `SoftwareSerial` (pinos 10/11)

| Pino | Função |
|---|---|
| 13 | Botão (leitura manual do sensor) |
| 9  | Enable RX/TX do sensor (RS485) |
| A0 | Setpoint bomba 1 (potenciômetro) |
| A1 | Setpoint bomba 2 (potenciômetro) |
| A2 | Saída bomba 1 |
| A3 | Saída bomba 2 |
| 12 | Habilita bombeamento (entrada) |
| 7  | Habilita irrigação (entrada) |
| 6  | Relé de irrigação (saída) |

## Como compilar e gravar

Projeto gerenciado via [PlatformIO](https://platformio.org/):

```bash
pio run              # compila
pio run --target upload   # grava no Arduino Nano
pio device monitor   # abre o monitor serial (9600 baud)
```

## Status / pendências

- A aquisição de dados do sensor de EC ainda apresenta leituras incoerentes — revisar comandos e valores em hexadecimal no manual do sensor (`EC-TDS_485 manual.pdf`).
- O controle de equilíbrio das substâncias funciona apenas por controle proporcional por enquanto.
