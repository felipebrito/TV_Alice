# Comandos Serial Monitor - TV Alice

## Comandos Básicos

### Reset e Status
- `RESET` - Resetar para ponto inicial (0 passos)
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

### Páginas
- `MARK` - Marcar página atual
- `GOTO:0` - Ir para página 0 (HOME)
- `GOTO:1` - Ir para página 1
- `GOTO:N` - Ir para página N

### Persistência
- `SAVE` - Salvar mapeamento na EEPROM
- `LOAD` - Carregar mapeamento da EEPROM
- `CLEAR` - Limpar todo o mapeamento

## Exemplo de Uso

```
1. RESET                    # Define ponto inicial
2. STATUS                   # Ver passo atual (deve ser 0)
3. CIMA100                  # Mover 100 passos para cima
4. STATUS                   # Ver passo atual (deve ser 100)
5. MARK                     # Marcar página 0
6. CIMA200                  # Mover mais 200 passos
7. STATUS                   # Ver passo atual (deve ser 300)
8. MARK                     # Marcar página 1
9. SAVE                     # Salvar mapeamento
10. GOTO:0                  # Voltar para página 0 (HOME)
11. GOTO:1                  # Ir para página 1
```

## Observações

- Todos os comandos são **case-insensitive** (maiúsculas/minúsculas)
- O movimento mostra progresso a cada 10 passos
- Use `STATUS` frequentemente para verificar a posição atual
- Sempre use `SAVE` após marcar páginas importantes
- `RESET` limpa o mapeamento - use com cuidado!

