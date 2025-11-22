/*
 * Sistema de Calibração e Mapeamento de Páginas - TV Alice
 * 
 * Comandos:
 * RESET - Define posição atual como ponto inicial (0 passos)
 * MARK - Marca posição atual como fim de página atual
 * NEXT - Avança para próxima página mapeada
 * PREV - Retrocede para página anterior
 * F:N - Move N passos para frente
 * B:N - Move N passos para trás
 * GOTO:P - Vai para página P
 * SAVE - Salva mapeamento na EEPROM
 * LOAD - Carrega mapeamento da EEPROM
 * STATUS - Mostra estado completo
 * CLEAR - Limpa mapeamento (reset completo)
 */

#include <EEPROM.h>

// Pinos do Shield CNC V3
const int StepX = 2;
const int DirX = 5;
const int StepY = 3;
const int DirY = 6;
const int EnablePin = 8;

// Configuração
const int MAX_PAGINAS = 30;
const int EEPROM_START = 0;
const int EEPROM_MAGIC = 0xABCD;  // Magic number para validação

// Estrutura de dados
struct Pagina {
  int passos_acumulados;  // Passos desde o início até esta página
  bool definida;          // Se a página foi marcada
};

// Variáveis globais
Pagina paginas[MAX_PAGINAS];
int pagina_atual = 0;
int passos_atual = 0;
int total_paginas_definidas = 0;
int delayUs = 2000;  // Velocidade (microsegundos)
bool motor_movendo = false;
bool parar_motor = false;
int passos_restantes = 0;
bool direcao_atual = HIGH;

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  // Configurar pinos do motor
  pinMode(EnablePin, OUTPUT);
  pinMode(StepX, OUTPUT);
  pinMode(DirX, OUTPUT);
  pinMode(StepY, OUTPUT);
  pinMode(DirY, OUTPUT);
  
  // Habilitar motores
  digitalWrite(EnablePin, LOW);
  digitalWrite(DirX, HIGH);
  digitalWrite(DirY, HIGH);
  digitalWrite(StepX, LOW);
  digitalWrite(StepY, LOW);
  
  // Inicializar estrutura de páginas
  for (int i = 0; i < MAX_PAGINAS; i++) {
    paginas[i].passos_acumulados = 0;
    paginas[i].definida = false;
  }
  
  // Tentar carregar da EEPROM
  if (loadFromEEPROM()) {
    Serial.println(F(">>> Mapeamento carregado da EEPROM"));
  } else {
    Serial.println(F(">>> Nenhum mapeamento encontrado na EEPROM"));
  }
  
  Serial.println(F("=== SISTEMA DE CALIBRAÇÃO - TV ALICE ==="));
  Serial.println(F(""));
  Serial.println(F("Comandos disponíveis:"));
  Serial.println(F("  RESET      - Define posição atual como início"));
  Serial.println(F("  MARK       - Marca fim da página atual"));
  Serial.println(F("  F:N        - Move N passos para frente"));
  Serial.println(F("  B:N        - Move N passos para trás"));
  Serial.println(F("  NEXT       - Vai para próxima página"));
  Serial.println(F("  PREV       - Vai para página anterior"));
  Serial.println(F("  GOTO:P     - Vai para página P"));
  Serial.println(F("  SAVE       - Salva mapeamento na EEPROM"));
  Serial.println(F("  LOAD       - Carrega mapeamento da EEPROM"));
  Serial.println(F("  STATUS     - Mostra estado completo"));
  Serial.println(F("  CLEAR      - Limpa mapeamento"));
  Serial.println(F(""));
  mostrar_status();
}

void loop() {
  // Verificar comandos serial
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();
    
    processarComando(cmd);
  }
  
  // Executar movimento se necessário (síncrono - bloqueia até completar)
  if (motor_movendo && !parar_motor && passos_restantes > 0) {
    executarPasso();
    passos_restantes--;
    passos_atual += (direcao_atual == HIGH ? 1 : -1);
    
    // Se terminou, atualizar status
    if (passos_restantes == 0) {
      motor_movendo = false;
      Serial.println(F(">>> Movimento completo!"));
    }
  }
  
  delay(10);
}

void processarComando(String cmd) {
  // Comando RESET
  if (cmd == "RESET") {
    // Parar qualquer movimento
    motor_movendo = false;
    parar_motor = true;
    passos_restantes = 0;
    
    // Resetar posição
    passos_atual = 0;
    pagina_atual = 0;
    
    // Limpar mapeamento (opcional - comentado para não perder dados)
    // limpar_mapeamento();
    
    Serial.println(F(">>> RESET: Posição definida como início (0 passos)"));
    Serial.println(F(">>> Página atual resetada para 0"));
    mostrar_status();
  }
  // Comando MARK
  else if (cmd == "MARK") {
    marcar_pagina();
  }
  // Comando F:N - Mover N passos frente
  else if (cmd.startsWith("F:")) {
    int passos = cmd.substring(2).toInt();
    if (passos > 0) {
      mover_passos(passos, true);
    }
  }
  // Comando B:N - Mover N passos trás
  else if (cmd.startsWith("B:")) {
    int passos = cmd.substring(2).toInt();
    if (passos > 0) {
      mover_passos(passos, false);
    }
  }
  // Comando NEXT
  else if (cmd == "NEXT") {
    ir_para_pagina(pagina_atual + 1);
  }
  // Comando PREV
  else if (cmd == "PREV") {
    ir_para_pagina(pagina_atual - 1);
  }
  // Comando GOTO:P
  else if (cmd.startsWith("GOTO:")) {
    int pagina = cmd.substring(5).toInt();
    ir_para_pagina(pagina);
  }
  // Comando HOME
  else if (cmd == "HOME") {
    ir_para_pagina(0);
  }
  // Comando SAVE
  else if (cmd == "SAVE") {
    saveToEEPROM();
  }
  // Comando LOAD
  else if (cmd == "LOAD") {
    loadFromEEPROM();
    mostrar_status();
  }
  // Comando STATUS
  else if (cmd == "STATUS" || cmd == "S") {
    mostrar_status();
  }
  // Comando CLEAR
  else if (cmd == "CLEAR") {
    limpar_mapeamento();
  }
  else if (cmd.length() > 0) {
    Serial.print(F(">>> Comando inválido: "));
    Serial.println(cmd);
  }
}

void marcar_pagina() {
  // Marcar posição atual como fim da página atual
  paginas[pagina_atual].passos_acumulados = passos_atual;
  paginas[pagina_atual].definida = true;
  
  // Atualizar total de páginas definidas
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
  }
  
  // Avançar para próxima página
  pagina_atual++;
  if (pagina_atual >= MAX_PAGINAS) {
    pagina_atual = MAX_PAGINAS - 1;
    Serial.println(F(">>> Aviso: Limite de páginas atingido!"));
  }
  
  mostrar_status();
}

void mover_passos(int passos, bool frente) {
  if (passos <= 0) {
    Serial.println(F(">>> Número de passos inválido!"));
    return;
  }
  
  // Parar qualquer movimento anterior
  motor_movendo = false;
  passos_restantes = 0;
  
  // Configurar direção
  direcao_atual = frente ? HIGH : LOW;
  digitalWrite(DirX, direcao_atual);
  digitalWrite(DirY, direcao_atual);
  delayMicroseconds(100);  // Pequeno delay para direção estabilizar
  
  // Executar passos de forma síncrona (bloqueante)
  Serial.print(F(">>> Movendo "));
  Serial.print(passos);
  Serial.print(F(" passos "));
  Serial.println(frente ? F("(frente)") : F("(trás)"));
  
  motor_movendo = true;
  passos_restantes = passos;
  parar_motor = false;
  
  // Executar todos os passos agora (síncrono)
  for (int i = 0; i < passos; i++) {
    executarPasso();
    passos_atual += (frente ? 1 : -1);
    
    // Verificar se deve parar
    if (parar_motor) {
      Serial.println(F(">>> Movimento interrompido!"));
      break;
    }
  }
  
  motor_movendo = false;
  passos_restantes = 0;
  
  Serial.print(F(">>> Concluído. Passos acumulados: "));
  Serial.println(passos_atual);
}

void ir_para_pagina(int pagina_destino) {
  if (pagina_destino < 0 || pagina_destino >= MAX_PAGINAS) {
    Serial.print(F(">>> Página inválida: "));
    Serial.println(pagina_destino);
    return;
  }
  
  // Se for página 0 e não estiver definida, assumir 0 passos
  if (pagina_destino == 0) {
    Serial.println(F(">>> Indo para HOME (Página 0)"));
    mover_passos(abs(passos_atual), passos_atual < 0);
    pagina_atual = 0;
    passos_atual = 0;
    Serial.println(F(">>> HOME alcançado (0 passos)"));
    mostrar_status();
    return;
  }
  
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
  Serial.print(abs(passos_para_mover));
  Serial.println(F(" passos)"));
  
  mover_passos(abs(passos_para_mover), passos_para_mover > 0);
  pagina_atual = pagina_destino;
  passos_atual = passos_destino;  // Atualizar passos atual para a posição da página
}

void executarPasso() {
  // Motor X
  digitalWrite(StepX, HIGH);
  delayMicroseconds(10);  // Pulso mínimo
  digitalWrite(StepX, LOW);
  
  // Motor Y (mesma direção) - simultâneo
  digitalWrite(StepY, HIGH);
  delayMicroseconds(10);  // Pulso mínimo
  digitalWrite(StepY, LOW);
  
  // Delay entre passos (velocidade)
  delayMicroseconds(delayUs);
}

void mostrar_status() {
  Serial.println(F("--- STATUS ---"));
  Serial.print(F("Página atual: "));
  Serial.println(pagina_atual);
  Serial.print(F("Passos acumulados: "));
  Serial.println(passos_atual);
  Serial.print(F("Total de páginas definidas: "));
  Serial.println(total_paginas_definidas);
  Serial.println(F(""));
  Serial.println(F("Mapeamento:"));
  Serial.println(F("Pág | Passos Acum | Passos Pág | Status"));
  Serial.println(F("----|-------------|------------|--------"));
  
  for (int i = 0; i < total_paginas_definidas + 1 && i < MAX_PAGINAS; i++) {
    Serial.print(F(" "));
    if (i < 10) Serial.print(F(" "));
    Serial.print(i);
    Serial.print(F("  | "));
    
    if (paginas[i].definida) {
      Serial.print(paginas[i].passos_acumulados);
      int len = String(paginas[i].passos_acumulados).length();
      for (int j = len; j < 11; j++) Serial.print(F(" "));
      Serial.print(F("| "));
      
      // Calcular passos da página
      if (i > 0 && paginas[i - 1].definida) {
        int passos_pag = paginas[i].passos_acumulados - paginas[i - 1].passos_acumulados;
        Serial.print(passos_pag);
        int len2 = String(passos_pag).length();
        for (int j = len2; j < 10; j++) Serial.print(F(" "));
      } else {
        Serial.print(F("     -    "));
      }
      Serial.print(F("|  ✓"));
    } else {
      Serial.print(F("     -     |     -    |  ⏳"));
    }
    Serial.println();
  }
  Serial.println(F("-------------"));
}

void limpar_mapeamento() {
  for (int i = 0; i < MAX_PAGINAS; i++) {
    paginas[i].passos_acumulados = 0;
    paginas[i].definida = false;
  }
  pagina_atual = 0;
  passos_atual = 0;
  total_paginas_definidas = 0;
  Serial.println(F(">>> Mapeamento limpo!"));
  mostrar_status();
}

void saveToEEPROM() {
  int addr = EEPROM_START;
  
  // Escrever magic number
  EEPROM.put(addr, EEPROM_MAGIC);
  addr += sizeof(int);
  
  // Escrever total de páginas
  EEPROM.put(addr, total_paginas_definidas);
  addr += sizeof(int);
  
  // Escrever página atual e passos atual
  EEPROM.put(addr, pagina_atual);
  addr += sizeof(int);
  EEPROM.put(addr, passos_atual);
  addr += sizeof(int);
  
  // Escrever array de páginas
  for (int i = 0; i < MAX_PAGINAS; i++) {
    EEPROM.put(addr, paginas[i].passos_acumulados);
    addr += sizeof(int);
    EEPROM.put(addr, paginas[i].definida);
    addr += sizeof(bool);
  }
  
  Serial.println(F(">>> Mapeamento salvo na EEPROM!"));
}

bool loadFromEEPROM() {
  int addr = EEPROM_START;
  int magic;
  
  // Ler magic number
  EEPROM.get(addr, magic);
  addr += sizeof(int);
  
  if (magic != EEPROM_MAGIC) {
    return false;  // Dados inválidos
  }
  
  // Ler total de páginas
  EEPROM.get(addr, total_paginas_definidas);
  addr += sizeof(int);
  
  // Ler página atual e passos atual
  EEPROM.get(addr, pagina_atual);
  addr += sizeof(int);
  EEPROM.get(addr, passos_atual);
  addr += sizeof(int);
  
  // Ler array de páginas
  for (int i = 0; i < MAX_PAGINAS; i++) {
    EEPROM.get(addr, paginas[i].passos_acumulados);
    addr += sizeof(int);
    EEPROM.get(addr, paginas[i].definida);
    addr += sizeof(bool);
  }
  
  return true;
}

