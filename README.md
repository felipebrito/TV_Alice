# 📺 TV Alice - Sistema de Controle de Rolos Sincronizados

Sistema mecatrônico para controlar dois motores NEMA 17 que enrolam/desenrolam papel de forma sincronizada, permitindo navegação página por página. O sistema pode ser controlado via controle remoto IR ou interface web.

## 🎯 Status Atual

✅ **Funcionando:**
- Controle de dois motores NEMA 17 sincronizados
- Controle via controle remoto IR (NEC protocol)
- Controle via serial (comandos F, B, S, R, V)
- Interface web Flask para monitoramento
- Simulador Python para cálculos de sincronização

## 📋 Componentes

### Hardware
- **Arduino Uno** com **CNC Shield V3**
- **2x Motores NEMA 17** (200 passos/volta)
- **2x Drivers DRV8825**
- **Sensor IR** (TSOP4838 ou similar)
- **Fonte 12V externa** para alimentação dos motores
- **Papel**: 150cm de comprimento total
- **Rolos**: Diâmetro inicial de 41mm

### Software
- **Firmware Arduino** (motor_com_ir.ino)
- **Interface Web Flask** (interface_tv_alice.py)
- **Simulador Python** (simulador_tv.py)

## 🔌 Conexões

### Motores (CNC Shield V3)
- **Motor X (Eixo X):**
  - A1 → Pino 3
  - A2 → Pino 6
  - B1 → Pino 1
  - B2 → Pino 4
- **Motor Y (Eixo Y):**
  - A1 → Pino 3
  - A2 → Pino 6
  - B1 → Pino 1
  - B2 → Pino 4
- **Enable Pin**: Pino 8 (comum para ambos)

### Sensor IR
- **VCC** → 5V do Arduino
- **GND** → GND do Arduino
- **OUT/S** → Pino 11 do Arduino

### Alimentação
- **12V externa** → Entrada do CNC Shield
- **GND comum** entre Arduino e fonte externa

## 🎮 Controles

### Controle Remoto IR

| Botão | Ação |
|-------|------|
| **Direita (→)** | Gira motores para frente |
| **Esquerda (←)** | Gira motores para trás |
| **Mais (+)** | Aumenta velocidade |
| **Menos (-)** | Diminui velocidade |
| **Parar** | Para os motores |

**Códigos IR mapeados:**
- Esquerda: `0x5`
- Direita: `0x1B`
- Menos: `0x7`
- Mais: `0x9`
- Parar: `0x1E`

### Comandos Serial

| Comando | Descrição |
|---------|-----------|
| `F` | Gira frente (contínuo) |
| `B` | Gira trás (contínuo) |
| `S` | Parar motor |
| `R:200` | Rotacionar 200 passos para frente |
| `R:-200` | Rotacionar 200 passos para trás |
| `V:2000` | Ajustar velocidade (500-10000 us) |
| `STATUS` | Mostrar status do sistema |

## 🚀 Instalação

### 1. Firmware Arduino

```bash
# Instalar biblioteca IRremote
arduino-cli lib install IRremote

# Compilar e enviar
cd firmware
arduino-cli compile --fqbn arduino:avr:uno motor_com_ir.ino
arduino-cli upload -p /dev/cu.usbmodem1301 --fqbn arduino:avr:uno motor_com_ir.ino
```

### 2. Interface Web

```bash
cd interface
pip3 install -r requirements.txt
python3 interface_tv_alice.py
```

Acesse: `http://localhost:5000`

## 📁 Estrutura do Projeto

```
TV_Alice/
├── firmware/           # Códigos Arduino
│   ├── motor_com_ir.ino      # Firmware principal (IR + Serial)
│   ├── motor_simples.ino     # Versão apenas serial
│   └── teste_ir.ino          # Teste do sensor IR
├── interface/          # Interface web e scripts Python
│   ├── interface_tv_alice.py # Interface Flask principal
│   ├── simulador_tv.py       # Simulador de sincronização
│   └── requirements.txt      # Dependências Python
├── docs/              # Documentação
│   ├── hardware.md          # Especificações de hardware
│   └── CALCULOS_SINCRONIZACAO.md
├── desenhos/          # Arquivos de design
│   ├── box-all.svg
│   └── illustrator/
└── README.md          # Este arquivo
```

## 🔧 Configuração

### Ajuste do Driver DRV8825

1. **Medir Vref** (tensão de referência):
   - Conectar multímetro entre GND e potenciômetro
   - Ajustar para **~1.0V** (2A de corrente)

2. **Verificar alimentação**:
   - 12V externa conectada no shield
   - GND comum entre Arduino e fonte

### Calibração

- **Velocidade padrão**: 2000 microsegundos
- **Ajuste**: Use botões +/- no controle IR ou comando `V:XXXX`

## 📊 Funcionalidades

### Sincronização de Motores

Os dois motores giram sincronizados:
- **Motor X**: Gira em uma direção
- **Motor Y**: Gira em direção oposta (para sincronizar papel)
- Ambos executam passos simultaneamente

### Cálculo de Sincronização

O sistema calcula dinamicamente:
- Diâmetro atual de cada rolo baseado no comprimento enrolado
- Relação de velocidade entre motores
- Passos necessários para mover comprimento específico

Fórmula: `Diâmetro = 41mm + (2 × comprimento_enrolado / π)`

## 🐛 Troubleshooting

### Motor não move
1. Verificar alimentação 12V externa
2. Verificar GND comum
3. Ajustar Vref do driver (~1.0V)
4. Verificar conexões dos motores

### Sensor IR não responde
1. Verificar conexão (VCC, GND, OUT)
2. Testar pino diferente (9, 10, 11, 12)
3. Verificar se sensor está recebendo sinal (LED pisca)

### Motores não sincronizados
1. Verificar se ambos estão conectados
2. Verificar direções (Y deve girar oposto a X)
3. Ajustar velocidade se necessário

## 📝 Licença

Este projeto é de código aberto.

## 👤 Autor

Felipe Brito

## 🔗 Links

- [Repositório GitHub](https://github.com/felipebrito/TV_Alice.git)

---

**Última atualização**: Janeiro 2025
