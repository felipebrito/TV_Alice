/*
 * TV Alice - Calibração Simplificada
 * Comandos Serial Monitor:
 * 
 * RESET - Resetar para ponto inicial (0 passos)
 * STATUS - Mostrar passo atual e páginas
 * CIMA - Mover 1 passo para cima (frente)
 * BAIXO - Mover 1 passo para baixo (trás)
 * CIMA10 - Mover 10 passos para cima
 * CIMA100 - Mover 100 passos para cima
 * CIMA400 - Mover 400 passos para cima
 * BAIXO10 - Mover 10 passos para baixo
 * BAIXO100 - Mover 100 passos para baixo
 * BAIXO400 - Mover 400 passos para baixo
 * MARK - Marcar página atual
 * GOTO:0 - Ir para página 0 (HOME)
 * GOTO:1 - Ir para página 1
 * GOTO:N - Ir para página N
 * SAVE - Salvar mapeamento na EEPROM
 * LOAD - Carregar mapeamento da EEPROM
 * CLEAR - Limpar mapeamento
 */

#include <EEPROM.h>

// Pinos CNC Shield V3 - Eixo X e Y
const int StepX = 2;
const int DirX = 5;
const int StepY = 3;
const int DirY = 6;

// Configurações
#define MAX_PAGINAS 30
int delayUs = 5000;  // Velocidade (microsegundos) - mais lento para calibração

// Estado
int pagina_atual = 0;
int passos_atual = 0;
int total_paginas_definidas = 0;
bool motor_movendo = false;
bool parar_motor = false;
bool direcao_atual = HIGH;

// Estrutura de página
struct Pagina {
  int passos_acumulados;
  bool definida;
};

Pagina paginas[MAX_PAGINAS];

// Endereços EEPROM
#define EEPROM_MAGIC 0xAA55
#define EEPROM_MAGIC_ADDR 0
#define EEPROM_DATA_ADDR 2

void setup() {
  Serial.begin(115200);
  
  // Configurar pinos
  pinMode(StepX, OUTPUT);
  pinMode(DirX, OUTPUT);
  pinMode(StepY, OUTPUT);
  pinMode(DirY, OUTPUT);
  
  digitalWrite(StepX, LOW);
  digitalWrite(DirX, LOW);
  digitalWrite(StepY, LOW);
  digitalWrite(DirY, LOW);
  
  // Inicializar páginas
  for (int i = 0; i < MAX_PAGINAS; i++) {
    paginas[i].passos_acumulados = 0;
    paginas[i].definida = false;
  }
  
  // Tentar carregar da EEPROM
  loadFromEEPROM();
  
  Serial.println(F("\n=== TV ALICE - CALIBRAÇÃO ==="));
  Serial.println(F("Comandos disponíveis:"));
  Serial.println(F("  RESET - Resetar para ponto inicial"));
  Serial.println(F("  STATUS - Mostrar passo atual e páginas"));
  Serial.println(F("  CIMA - Mover 1 passo para cima"));
  Serial.println(F("  BAIXO - Mover 1 passo para baixo"));
  Serial.println(F("  CIMA10, CIMA100, CIMA400 - Mover N passos para cima"));
  Serial.println(F("  BAIXO10, BAIXO100, BAIXO400 - Mover N passos para baixo"));
  Serial.println(F("  MARK - Marcar página atual"));
  Serial.println(F("  GOTO:0, GOTO:1, ... - Ir para página N"));
  Serial.println(F("  SAVE - Salvar mapeamento"));
  Serial.println(F("  LOAD - Carregar mapeamento"));
  Serial.println(F("  CLEAR - Limpar mapeamento"));
  Serial.println(F("\n>>> Aguardando comandos...\n"));
  
  mostrar_status();
}

void loop() {
  // Verificar comandos serial
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();
    
    if (cmd.length() > 0) {
      Serial.print(F(">>> Comando: ["));
      Serial.print(cmd);
      Serial.println(F("]"));
      processarComando(cmd);
    }
  }
}

void processarComando(String cmd) {
  // RESET
  if (cmd == "RESET") {
    motor_movendo = false;
    parar_motor = true;
    passos_atual = 0;
    pagina_atual = 0;
    paginas[0].passos_acumulados = 0;
    paginas[0].definida = false;
    total_paginas_definidas = 0;
    Serial.println(F(">>> RESET: Posição definida como início (0 passos)"));
    mostrar_status();
  }
  // STATUS
  else if (cmd == "STATUS" || cmd == "S") {
    mostrar_status();
  }
  // CIMA (1 passo)
  else if (cmd == "CIMA") {
    mover_passos(1, true);
  }
  // BAIXO (1 passo)
  else if (cmd == "BAIXO") {
    mover_passos(1, false);
  }
  // CIMA10
  else if (cmd == "CIMA10") {
    mover_passos(10, true);
  }
  // CIMA100
  else if (cmd == "CIMA100") {
    mover_passos(100, true);
  }
  // CIMA400
  else if (cmd == "CIMA400") {
    mover_passos(400, true);
  }
  // BAIXO10
  else if (cmd == "BAIXO10") {
    mover_passos(10, false);
  }
  // BAIXO100
  else if (cmd == "BAIXO100") {
    mover_passos(100, false);
  }
  // BAIXO400
  else if (cmd == "BAIXO400") {
    mover_passos(400, false);
  }
  // MARK
  else if (cmd == "MARK") {
    marcar_pagina();
  }
  // GOTO:P
  else if (cmd.startsWith("GOTO:")) {
    int pagina = cmd.substring(5).toInt();
    ir_para_pagina(pagina);
  }
  // SAVE
  else if (cmd == "SAVE") {
    saveToEEPROM();
  }
  // LOAD
  else if (cmd == "LOAD") {
    loadFromEEPROM();
    mostrar_status();
  }
  // CLEAR
  else if (cmd == "CLEAR") {
    limpar_mapeamento();
  }
  else {
    Serial.print(F(">>> Comando inválido: "));
    Serial.println(cmd);
    Serial.println(F(">>> Digite STATUS para ver ajuda"));
  }
}

void executarPasso() {
  digitalWrite(StepX, HIGH);
  digitalWrite(StepY, HIGH);
  delayMicroseconds(delayUs);
  digitalWrite(StepX, LOW);
  digitalWrite(StepY, LOW);
  delayMicroseconds(delayUs);
}

void mover_passos(int passos, bool frente) {
  if (passos <= 0) {
    Serial.println(F(">>> Número de passos inválido!"));
    return;
  }
  
  // Parar qualquer movimento anterior
  motor_movendo = false;
  parar_motor = false;
  
  // Configurar direção
  direcao_atual = frente ? HIGH : LOW;
  digitalWrite(DirX, direcao_atual);
  digitalWrite(DirY, direcao_atual);
  delayMicroseconds(100);
  
  Serial.print(F(">>> Movendo "));
  Serial.print(passos);
  Serial.print(F(" passos "));
  Serial.println(frente ? F("(CIMA)") : F("(BAIXO)"));
  
  motor_movendo = true;
  
  // Executar passos com feedback
  for (int i = 0; i < passos; i++) {
    executarPasso();
    passos_atual += (frente ? 1 : -1);
    
    // Feedback a cada 10 passos ou no início/fim
    if (i == 0 || (i + 1) % 10 == 0 || i == passos - 1) {
      Serial.print(F("  ["));
      Serial.print(i + 1);
      Serial.print(F("/"));
      Serial.print(passos);
      Serial.print(F("] Passos acumulados: "));
      Serial.println(passos_atual);
    }
    
    // Verificar se deve parar
    if (parar_motor) {
      Serial.println(F(">>> Movimento interrompido!"));
      break;
    }
  }
  
  motor_movendo = false;
  
  Serial.print(F(">>> Concluído. Passos acumulados: "));
  Serial.println(passos_atual);
}

void marcar_pagina() {
  // Verificar se passos_atual está negativo
  if (passos_atual < 0) {
    Serial.println(F(">>> ERRO: Passos acumulados estão negativos!"));
    Serial.println(F(">>> Use GOTO:0 para voltar ao início antes de marcar."));
    Serial.println(F(">>> Ou execute RESET para recalibrar do zero."));
    return;
  }
  
  // Marcar posição atual
  paginas[pagina_atual].passos_acumulados = passos_atual;
  paginas[pagina_atual].definida = true;
  
  // Atualizar total
  if (pagina_atual >= total_paginas_definidas) {
    total_paginas_definidas = pagina_atual + 1;
  }
  
  Serial.print(F(">>> Página "));
  Serial.print(pagina_atual);
  Serial.print(F(" marcada com "));
  Serial.print(passos_atual);
  Serial.println(F(" passos acumulados"));
  
  // Calcular passos desta página
  if (pagina_atual > 0 && paginas[pagina_atual - 1].definida) {
    int passos_pagina = passos_atual - paginas[pagina_atual - 1].passos_acumulados;
    Serial.print(F("    Passos da página: "));
    Serial.println(passos_pagina);
  } else if (pagina_atual == 0) {
    Serial.println(F("    (Página inicial)"));
  }
  
  // Avançar para próxima página
  pagina_atual++;
  if (pagina_atual >= MAX_PAGINAS) {
    pagina_atual = MAX_PAGINAS - 1;
    Serial.println(F(">>> Aviso: Limite de páginas atingido!"));
  }
  
  Serial.print(F(">>> Próxima página: "));
  Serial.print(pagina_atual);
  Serial.println(F(" (mova o papel e marque quando estiver na posição)"));
  
  mostrar_status();
}

void ir_para_pagina(int pagina_destino) {
  if (pagina_destino < 0 || pagina_destino >= MAX_PAGINAS) {
    Serial.print(F(">>> Página inválida: "));
    Serial.println(pagina_destino);
    return;
  }
  
  // Se for página 0 (HOME)
  if (pagina_destino == 0) {
    Serial.println(F(">>> Indo para HOME (Página 0)"));
    
    if (passos_atual == 0) {
      Serial.println(F(">>> Já está no HOME"));
      pagina_atual = 0;
      mostrar_status();
      return;
    }
    
    // Mover de volta para 0
    int passos_para_mover = abs(passos_atual);
    if (passos_para_mover > 0) {
      Serial.print(F(">>> Movendo "));
      Serial.print(passos_para_mover);
      Serial.print(F(" passos "));
      Serial.println(passos_atual < 0 ? F("(CIMA)") : F("(BAIXO)"));
      mover_passos(passos_para_mover, passos_atual < 0);
    }
    pagina_atual = 0;
    passos_atual = 0;
    Serial.println(F(">>> HOME alcançado (0 passos)"));
    mostrar_status();
    return;
  }
  
  // Verificar se página está definida
  if (!paginas[pagina_destino].definida) {
    Serial.print(F(">>> Página "));
    Serial.print(pagina_destino);
    Serial.println(F(" ainda não foi marcada!"));
    return;
  }
  
  int passos_destino = paginas[pagina_destino].passos_acumulados;
  int passos_para_mover = passos_destino - passos_atual;
  
  if (passos_para_mover == 0) {
    Serial.println(F(">>> Já está na posição desta página"));
    pagina_atual = pagina_destino;
    return;
  }
  
  Serial.print(F(">>> Indo para página "));
  Serial.print(pagina_destino);
  Serial.print(F(" ("));
  Serial.print(passos_destino);
  Serial.println(F(" passos)"));
  
  bool frente = passos_para_mover > 0;
  mover_passos(abs(passos_para_mover), frente);
  
  pagina_atual = pagina_destino;
  Serial.print(F(">>> Página "));
  Serial.print(pagina_destino);
  Serial.println(F(" alcançada!"));
  
  mostrar_status();
}

void mostrar_status() {
  Serial.println(F("\n--- STATUS ---"));
  Serial.print(F("Página atual: "));
  Serial.println(pagina_atual);
  Serial.print(F("Passos acumulados: "));
  Serial.println(passos_atual);
  Serial.print(F("Total de páginas definidas: "));
  Serial.println(total_paginas_definidas);
  
  Serial.println(F("\nMapeamento:"));
  Serial.println(F("Pág | Passos Acum | Passos Pág | Status"));
  Serial.println(F("----|-------------|------------|--------"));
  
  for (int i = 0; i < MAX_PAGINAS; i++) {
    if (i < total_paginas_definidas || paginas[i].definida) {
      int passos_pagina = 0;
      if (i > 0 && paginas[i - 1].definida) {
        passos_pagina = paginas[i].passos_acumulados - paginas[i - 1].passos_acumulados;
      }
      
      Serial.print(F("  "));
      Serial.print(i);
      Serial.print(F("  |  "));
      if (paginas[i].definida) {
        Serial.print(paginas[i].passos_acumulados);
      } else {
        Serial.print(F("-"));
      }
      Serial.print(F("  |  "));
      if (paginas[i].definida && i > 0) {
        Serial.print(passos_pagina);
      } else {
        Serial.print(F("-"));
      }
      Serial.print(F("  |  "));
      Serial.println(paginas[i].definida ? F("✓") : F("⏳"));
    }
  }
  Serial.println(F("-------------\n"));
}

void limpar_mapeamento() {
  for (int i = 0; i < MAX_PAGINAS; i++) {
    paginas[i].passos_acumulados = 0;
    paginas[i].definida = false;
  }
  total_paginas_definidas = 0;
  pagina_atual = 0;
  passos_atual = 0;
  Serial.println(F(">>> Mapeamento limpo!"));
  mostrar_status();
}

void saveToEEPROM() {
  Serial.println(F(">>> Salvando mapeamento na EEPROM..."));
  
  // Salvar magic number
  EEPROM.put(EEPROM_MAGIC_ADDR, (uint16_t)EEPROM_MAGIC);
  
  // Salvar dados
  int addr = EEPROM_DATA_ADDR;
  EEPROM.put(addr, total_paginas_definidas);
  addr += sizeof(int);
  
  for (int i = 0; i < MAX_PAGINAS; i++) {
    EEPROM.put(addr, paginas[i].passos_acumulados);
    addr += sizeof(int);
    EEPROM.put(addr, paginas[i].definida);
    addr += sizeof(bool);
  }
  
  EEPROM.put(addr, pagina_atual);
  addr += sizeof(int);
  EEPROM.put(addr, passos_atual);
  
  Serial.println(F(">>> Mapeamento salvo com sucesso!"));
}

void loadFromEEPROM() {
  // Verificar magic number
  uint16_t magic;
  EEPROM.get(EEPROM_MAGIC_ADDR, magic);
  
  if (magic != EEPROM_MAGIC) {
    Serial.println(F(">>> Nenhum dado salvo encontrado na EEPROM"));
    return;
  }
  
  Serial.println(F(">>> Carregando mapeamento da EEPROM..."));
  
  // Carregar dados
  int addr = EEPROM_DATA_ADDR;
  EEPROM.get(addr, total_paginas_definidas);
  addr += sizeof(int);
  
  for (int i = 0; i < MAX_PAGINAS; i++) {
    EEPROM.get(addr, paginas[i].passos_acumulados);
    addr += sizeof(int);
    EEPROM.get(addr, paginas[i].definida);
    addr += sizeof(bool);
  }
  
  EEPROM.get(addr, pagina_atual);
  addr += sizeof(int);
  EEPROM.get(addr, passos_atual);
  
  Serial.print(F(">>> Carregado: "));
  Serial.print(total_paginas_definidas);
  Serial.println(F(" páginas definidas"));
}

