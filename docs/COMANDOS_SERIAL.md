# Comandos Serial Monitor - TV Alice

## Comandos Básicos

### Reset e Status
- `RESET` - **Define a posição ATUAL como ponto inicial (Página 0, 0 passos)**
  - ⚠️ **IMPORTANTE**: Não move o motor, apenas redefine a contagem
  - A partir daqui, apenas movimentos CIMA são permitidos
  - Movimentos BAIXO que resultariam em negativos são bloqueados
- `STATUS` ou `S` - Mostrar passo atual e páginas mapeadas

### Movimento - Cima (Frente)
- `CIMA` - Mover 1 passo para cima
- `CIMA10` - Mover 10 passos para cima
- `CIMA100` - Mover 100 passos para cima
- `CIMA400` - Mover 400 passos para cima

### Movimento - Baixo (Trás)
- `BAIXO` - Mover 1 passo para baixo
- `BAIXO10` - Mover 10 passos para baixo
- `BAIXO100` - Mover 100 passos para baixo
- `BAIXO400` - Mover 400 passos para baixo
- ⚠️ **BLOQUEADO** se resultar em passos negativos (use RESET para novo ponto inicial)

### Páginas
- `MARK` - Marcar página atual (e avança para próxima)
- `MARK:0` - Marcar página 0 na posição atual
- `MARK:1` - Marcar página 1 na posição atual
- `MARK:N` - Marcar página N na posição atual
- `GOTO:0` - Ir para página 0 (HOME)
- `GOTO:1` - Ir para página 1
- `GOTO:N` - Ir para página N

### Persistência
- `SAVE` - Salvar mapeamento na EEPROM
- `LOAD` - Carregar mapeamento da EEPROM
- `CLEAR` - Limpar todo o mapeamento

## Exemplo de Uso

### Calibração Básica (420 passos por página)
```
1. RESET                    # Define posição ATUAL como ponto inicial (Página 0)
2. STATUS                   # Ver passo atual (deve ser 0)
3. CIMA420                  # Mover 420 passos para cima
4. STATUS                   # Ver passo atual (deve ser 420)
5. MARK                     # Marcar página 0 (com 420 passos)
6. CIMA420                  # Mover mais 420 passos
7. STATUS                   # Ver passo atual (deve ser 840)
8. MARK                     # Marcar página 1 (com 840 passos)
9. SAVE                     # Salvar mapeamento
10. GOTO:0                  # Voltar para página 0 (HOME)
11. GOTO:1                  # Ir para página 1
```

### ⚠️ Importante sobre RESET
- `RESET` **NÃO move o motor**, apenas redefine a contagem
- A posição física atual vira o "zero"
- Após RESET, apenas movimentos CIMA são permitidos
- Se tentar BAIXO e resultar em negativo, será bloqueado

## Observações

- Todos os comandos são **case-insensitive** (maiúsculas/minúsculas)
- O movimento mostra progresso a cada 10 passos
- Use `STATUS` frequentemente para verificar a posição atual
- Sempre use `SAVE` após marcar páginas importantes
- `RESET` limpa o mapeamento - use com cuidado!

