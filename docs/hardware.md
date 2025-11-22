# Especificações de Hardware - TV Alice

## Componentes Principais

### Microcontrolador
- **Arduino Uno** (ATmega328P)
- **CNC Shield V3** (compatível com Arduino Uno)

### Motores
- **2x NEMA 17 Stepper Motors**
  - 200 passos por volta
  - 1.8° por passo
  - Alimentação: 12V

### Drivers
- **2x DRV8825 Stepper Motor Driver**
  - Microstepping: 1/1, 1/2, 1/4, 1/8, 1/16, 1/32
  - Corrente máxima: 2.5A
  - Vref ajustável via potenciômetro

### Sensor IR
- **TSOP4838** ou similar
- Protocolo: NEC (padrão)
- Frequência: 38kHz

### Alimentação
- **Fonte externa 12V** (mínimo 2A por motor)
- **GND comum** obrigatório entre Arduino e fonte

## Conexões Detalhadas

### CNC Shield V3 - Eixo X
| Shield Pin | Arduino Pin | Função |
|------------|-------------|--------|
| Step | 2 | Pulso de passo |
| Dir | 5 | Direção |
| Enable | 8 | Habilitar driver |

### CNC Shield V3 - Eixo Y
| Shield Pin | Arduino Pin | Função |
|------------|-------------|--------|
| Step | 3 | Pulso de passo |
| Dir | 6 | Direção |
| Enable | 8 | Habilitar driver (comum) |

### Motor NEMA 17
| Motor Wire | Shield Connector |
|------------|------------------|
| A1 (Vermelho) | A1 |
| A2 (Azul) | A2 |
| B1 (Amarelo) | B1 |
| B2 (Laranja) | B2 |

### Sensor IR
| Sensor Pin | Arduino |
|------------|---------|
| VCC | 5V |
| GND | GND |
| OUT | Pino 11 |

## Configuração do Driver

### Ajuste de Corrente (Vref)

1. **Medir Vref**:
   - Multímetro entre GND e potenciômetro do driver
   - Fórmula: `Corrente (A) = Vref (V) / 0.5`
   - Recomendado: **1.0V = 2.0A**

2. **Ajuste**:
   - Gire potenciômetro no sentido horário para aumentar
   - Gire no sentido anti-horário para diminuir

### Microstepping

Configurado via jumpers no shield:
- **M0, M1, M2**: Controlam resolução
- Padrão: 1/16 microstepping (3200 passos/volta)

## Especificações Mecânicas

### Papel
- **Comprimento total**: 150cm
- **Espessura**: ~0.1mm (ajustável no código)

### Rolos
- **Diâmetro inicial**: 41mm
- **Diâmetro máximo**: Calculado dinamicamente
- **Material**: Tubo de papelão ou similar

### Cálculo de Diâmetro

```
Diâmetro_atual = Diâmetro_inicial + (2 × comprimento_enrolado / π)
```

## Requisitos de Alimentação

### Fonte Externa
- **Tensão**: 12V DC
- **Corrente**: Mínimo 4A (2A por motor)
- **Conector**: Compatível com CNC Shield

### Arduino
- **USB**: 5V para alimentação do Arduino
- **GND**: Deve ser comum com fonte externa

## Pinos Disponíveis

### Usados pelo Shield
- Pinos 2, 3, 4, 5, 6, 7, 8

### Disponíveis para outros usos
- Pinos 9, 10, 11, 12, 13
- Pinos analógicos A0-A5

## Troubleshooting Hardware

### Motor não move
1. Verificar alimentação 12V
2. Verificar GND comum
3. Ajustar Vref (1.0V)
4. Verificar conexões dos fios do motor

### Motor vibra mas não gira
- Vref muito baixo (aumentar)
- Velocidade muito alta (diminuir delayUs)

### Motor esquenta muito
- Vref muito alto (diminuir)
- Reduzir corrente no código

### Sensor IR não funciona
- Verificar alimentação 5V
- Testar pino diferente
- Verificar se sensor está recebendo sinal

