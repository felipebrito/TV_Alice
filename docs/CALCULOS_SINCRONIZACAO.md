# Cálculos de Sincronização - TV Alice

## Dados do Sistema

- **Diâmetro inicial dos rolos**: 41mm = 0.041m
- **Raio inicial**: 20.5mm = 0.0205m
- **Comprimento total do papel**: 150cm = 1.5m
- **Motor NEMA 17**: 200 passos/volta
- **Páginas**: Comprimento fixo (a definir)

## Cálculos Fundamentais

### Perímetro Inicial
```
Perímetro = π × diâmetro
Perímetro = π × 41mm = 128.8mm = 12.88cm por volta
```

### Passos por Centímetro (inicial)
```
Passos/cm = 200 passos / 12.88cm = 15.53 passos/cm
```

### Relação de Sincronização

Quando um rolo enrola e outro desenrola:

**Rolo que ENROLA (diâmetro aumenta):**
- Diâmetro atual = D_enrola
- Voltas necessárias = comprimento / (π × D_enrola)
- Menos voltas necessárias (rolo fica maior)

**Rolo que DESENROLA (diâmetro diminui):**
- Diâmetro atual = D_desenrola  
- Voltas necessárias = comprimento / (π × D_desenrola)
- Mais voltas necessárias (rolo fica menor)

**Relação:**
```
Voltas_enrola / Voltas_desenrola = D_desenrola / D_enrola
```

## Exemplo Prático

### Situação Inicial (ambos com 41mm)
- Para mover 10cm de papel:
  - Ambos precisam: 10cm / 12.88cm = 0.776 voltas
  - Passos: 0.776 × 200 = 155 passos
  - **Relação 1:1** (mesma velocidade)

### Após Enrolar 50cm no Rolo X
- Rolos X tem ~91mm de diâmetro (enrolou 50cm)
- Rolos Y tem ~41mm de diâmetro (desenrolou 50cm)

Para mover 10cm:
- Rolos X: 10cm / (π × 91mm) = 0.35 voltas = 70 passos
- Rolos Y: 10cm / (π × 41mm) = 0.776 voltas = 155 passos
- **Relação: Y precisa girar 2.21x mais rápido que X**

## Fórmula Geral

Para mover comprimento L de papel:

```
Passos_X = (L / (π × Dx)) × 200
Passos_Y = (L / (π × Dy)) × 200

Relação = Passos_Y / Passos_X = Dx / Dy
```

## Implementação

Precisamos rastrear:
1. Diâmetro atual de cada rolo
2. Comprimento enrolado em cada rolo
3. Calcular relação de velocidade dinamicamente

