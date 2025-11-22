# Guia de Calibração de Páginas - TV Alice

## Objetivo

Calibrar o sistema para mapear todas as páginas do papel, medindo quantos passos são necessários para mover de uma página para outra.

## Pré-requisitos

1. **Firmware de calibração carregado** (`calibracao.ino`)
2. **Interface web rodando** (`calibracao.py`)
3. **Papel posicionado corretamente** nos rolos
4. **Arduino conectado** via USB

## Processo de Calibração

### Passo 1: Conectar e Resetar

1. Abra a interface web: `http://localhost:5001`
2. Conecte ao Arduino (porta serial)
3. Posicione o papel no **ponto inicial** (início da primeira página)
4. Clique em **RESET** para definir esta posição como início (0 passos)

### Passo 2: Marcar Primeira Página

1. Verifique que está na posição inicial
2. Clique em **MARCAR PÁGINA**
3. A página 0 será marcada com 0 passos acumulados

### Passo 3: Mover para Próxima Página

1. Use os botões de movimento manual para mover o papel até o **fim da página 1**
   - Use botões pequenos (1, 10 passos) para ajuste fino
   - Use botões grandes (100 passos) para movimento rápido
   - Ou digite um valor customizado
2. Quando estiver na posição correta, clique em **MARCAR PÁGINA**
3. O sistema calculará automaticamente quantos passos foram dados da página 0 para a página 1

### Passo 4: Repetir para Todas as Páginas

1. Continue movendo e marcando cada página
2. Para cada página:
   - Mova até o fim da página
   - Clique em **MARCAR PÁGINA**
   - O sistema calcula os passos automaticamente

### Passo 5: Salvar Mapeamento

1. Após mapear todas as páginas, clique em **SALVAR**
2. O mapeamento será salvo na EEPROM do Arduino
3. Você pode também **EXPORTAR JSON** para backup

## Comandos Disponíveis

### Movimento Manual

- **◀◀◀ 100**: Move 100 passos para trás
- **◀◀ 10**: Move 10 passos para trás
- **◀ 1**: Move 1 passo para trás
- **1 ▶**: Move 1 passo para frente
- **10 ▶▶**: Move 10 passos para frente
- **100 ▶▶▶**: Move 100 passos para frente
- **Campo customizado**: Digite quantos passos deseja mover

### Marcação

- **RESET**: Define posição atual como início (0 passos)
- **MARCAR PÁGINA**: Marca fim da página atual
- **IR PARA PÁGINA**: Vai para uma página já mapeada
- **Próxima/Anterior**: Navega entre páginas mapeadas

### Persistência

- **SALVAR**: Salva mapeamento na EEPROM do Arduino
- **CARREGAR**: Carrega mapeamento da EEPROM
- **LIMPAR**: Limpa todo o mapeamento (cuidado!)
- **EXPORTAR JSON**: Baixa arquivo JSON com os dados

## Visualização do Mapeamento

A tabela mostra:
- **Página**: Número da página
- **Passos Acumulados**: Total de passos desde o início até esta página
- **Passos da Página**: Quantos passos foram dados nesta página específica
- **Status**: ✓ Definida ou ⏳ Pendente

## Dicas

1. **Ajuste fino**: Use movimentos pequenos (1-10 passos) quando estiver perto da posição correta
2. **Verificação visual**: Sempre verifique visualmente se está na posição correta antes de marcar
3. **Salvar frequentemente**: Salve o mapeamento periodicamente para não perder progresso
4. **Exportar backup**: Exporte o JSON como backup antes de limpar ou fazer grandes mudanças
5. **Navegação**: Use "Ir para Página" para verificar se o mapeamento está correto

## Comandos Serial (Alternativa)

Se preferir usar comandos serial diretamente:

```bash
# Reset
echo "RESET" > /dev/cu.usbmodem1301

# Mover 100 passos frente
echo "F:100" > /dev/cu.usbmodem1301

# Mover 50 passos trás
echo "B:50" > /dev/cu.usbmodem1301

# Marcar página
echo "MARK" > /dev/cu.usbmodem1301

# Ir para página 5
echo "GOTO:5" > /dev/cu.usbmodem1301

# Salvar
echo "SAVE" > /dev/cu.usbmodem1301

# Status
echo "STATUS" > /dev/cu.usbmodem1301
```

## Estrutura de Dados

### No Arduino (EEPROM)

- Magic number para validação
- Número total de páginas definidas
- Página atual
- Passos atuais
- Array de páginas (até 30 páginas)

### No JSON Exportado

```json
{
  "total_paginas": 10,
  "paginas": [
    {"numero": 0, "passos": 0, "definida": true},
    {"numero": 1, "passos": 1500, "definida": true},
    {"numero": 2, "passos": 3000, "definida": true}
  ],
  "ultima_atualizacao": "2025-01-XX",
  "pagina_atual": 2,
  "passos_atual": 3000
}
```

## Troubleshooting

### Página não marca
- Verifique se está conectado ao Arduino
- Verifique se o comando foi enviado (veja no log)
- Tente novamente

### Movimento não funciona
- Verifique conexão serial
- Verifique se motores estão habilitados
- Verifique alimentação 12V

### Dados não salvam
- Verifique se há espaço na EEPROM
- Tente salvar novamente
- Exporte JSON como backup

### Perdeu mapeamento
- Tente carregar da EEPROM (LOAD)
- Se tiver exportado JSON, pode importar manualmente
- Caso contrário, recalibre

## Próximos Passos

Após calibrar:
1. Exporte o JSON com os dados
2. Use esses dados no firmware final (`motor_com_ir.ino` ou versão futura)
3. O sistema poderá navegar automaticamente entre páginas usando os passos medidos

