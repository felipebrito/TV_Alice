/*
 * Controle de Motor com Sensor IR
 * 
 * Comandos IR:
 * - Esquerda (0x5) → Gira trás
 * - Direita (0x1B) → Gira frente
 * - Menos (0x7) → Diminuir velocidade
 * - Mais (0x9) → Aumentar velocidade
 * - Parar (0x1E) → Parar motor
 * 
 * Comandos Serial (também funcionam):
 * F - Gira frente
 * B - Gira trás
 * S - Parar
 * R:200 - Rotacionar 200 passos
 * V:2000 - Velocidade
 */

#include <IRremote.hpp>

// Pinos do Shield CNC V3
const int StepX = 2;
const int DirX = 5;
const int StepY = 3;
const int DirY = 6;
const int EnablePin = 8;

// Sensor IR
const int IR_RECEIVE_PIN = 11;

// Variáveis
int delayUs = 2000;  // Velocidade (microsegundos)
bool motor_movendo = false;
bool parar_motor = false;
int passos_restantes = 0;
bool direcao_atual = HIGH;  // HIGH = frente, LOW = trás

// Códigos IR mapeados
const uint16_t IR_ESQUERDA = 0x5;    // Botão esquerda
const uint16_t IR_DIREITA = 0x1B;   // Botão direita
const uint16_t IR_MENOS = 0x7;      // Botão menos (-)
const uint16_t IR_MAIS = 0x9;       // Botão mais (+)
const uint16_t IR_PARAR = 0x1E;     // Botão parar

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
  
  // Inicializar receptor IR
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  
  Serial.println(F("=== MOTOR COM CONTROLE IR ==="));
  Serial.println(F(""));
  Serial.println(F("Comandos IR:"));
  Serial.println(F("  Esquerda → Gira trás"));
  Serial.println(F("  Direita → Gira frente"));
  Serial.println(F("  Menos (-) → Diminuir velocidade"));
  Serial.println(F("  Mais (+) → Aumentar velocidade"));
  Serial.println(F("  Parar → Parar motor"));
  Serial.println(F(""));
  Serial.println(F("Comandos Serial também funcionam: F, B, S, R:200, V:2000"));
  Serial.println(F(""));
  Serial.print(F("Velocidade inicial: "));
  Serial.print(delayUs);
  Serial.println(F("us"));
  Serial.println(F(">>> Pronto!"));
}

void loop() {
  // Verificar comandos IR
  if (IrReceiver.decode()) {
    uint16_t comando = IrReceiver.decodedIRData.command;
    
    Serial.print(F("[IR] Comando: 0x"));
    Serial.print(comando, HEX);
    Serial.print(F(" ("));
    
    if (comando == IR_ESQUERDA) {
      Serial.println(F("ESQUERDA)"));
      girarTras();
    }
    else if (comando == IR_DIREITA) {
      Serial.println(F("DIREITA)"));
      girarFrente();
    }
    else if (comando == IR_MENOS) {
      Serial.println(F("MENOS)"));
      diminuirVelocidade();
    }
    else if (comando == IR_MAIS) {
      Serial.println(F("MAIS)"));
      aumentarVelocidade();
    }
    else if (comando == IR_PARAR) {
      Serial.println(F("PARAR)"));
      pararMotor();
    }
    else {
      Serial.print(F("DESCONHECIDO)"));
      Serial.print(F(" | HEX: 0x"));
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    }
    
    IrReceiver.resume();
  }
  
  // Verificar comandos serial
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();
    
    Serial.print(F("[SERIAL] Comando: "));
    Serial.println(cmd);
    
    processarComandoSerial(cmd);
  }
  
  // Executar movimento se necessário
  if (motor_movendo && !parar_motor) {
    if (passos_restantes == -1) {
      // Movimento contínuo
      executarPasso();
    } else if (passos_restantes > 0) {
      // Movimento com número de passos
      executarPasso();
    } else {
      // Terminou movimento
      motor_movendo = false;
      Serial.println(F(">>> Movimento completo!"));
    }
  }
  
  delay(10);
}

void girarFrente() {
  direcao_atual = HIGH;
  digitalWrite(DirX, direcao_atual);
  digitalWrite(DirY, LOW);  // Y gira em direção oposta
  motor_movendo = true;
  passos_restantes = -1;  // -1 = movimento contínuo
  parar_motor = false;
  Serial.println(F(">>> Girando FRENTE (direita) - AMBOS MOTORES"));
}

void girarTras() {
  direcao_atual = LOW;
  digitalWrite(DirX, direcao_atual);
  digitalWrite(DirY, HIGH);  // Y gira em direção oposta
  motor_movendo = true;
  passos_restantes = -1;  // -1 = movimento contínuo
  parar_motor = false;
  Serial.println(F(">>> Girando TRÁS (esquerda) - AMBOS MOTORES"));
}

void pararMotor() {
  parar_motor = true;
  motor_movendo = false;
  passos_restantes = 0;
  Serial.println(F(">>> PARAR"));
}

void aumentarVelocidade() {
  if (delayUs > 500) {
    delayUs -= 200;
    Serial.print(F(">>> Velocidade aumentada: "));
    Serial.print(delayUs);
    Serial.println(F("us"));
  } else {
    Serial.println(F(">>> Velocidade já está no máximo!"));
  }
}

void diminuirVelocidade() {
  if (delayUs < 5000) {
    delayUs += 200;
    Serial.print(F(">>> Velocidade diminuída: "));
    Serial.print(delayUs);
    Serial.println(F("us"));
  } else {
    Serial.println(F(">>> Velocidade já está no mínimo!"));
  }
}

void processarComandoSerial(String cmd) {
  // Comando F - Frente
  if (cmd == "F") {
    girarFrente();
  }
  // Comando B - Trás
  else if (cmd == "B") {
    girarTras();
  }
  // Comando S - Parar
  else if (cmd == "S" || cmd == "STOP") {
    pararMotor();
  }
  // Comando R:200 - Rotacionar N passos
  else if (cmd.startsWith("R:")) {
    int passos = cmd.substring(2).toInt();
    if (passos > 0) {
      direcao_atual = HIGH;
      digitalWrite(DirX, direcao_atual);
      digitalWrite(DirY, LOW);
      motor_movendo = true;
      passos_restantes = passos;
      parar_motor = false;
      Serial.print(F(">>> Rotacionando "));
      Serial.print(passos);
      Serial.println(F(" passos (frente)"));
    }
  }
  // Comando R:-200 - Rotacionar N passos para trás
  else if (cmd.startsWith("R:-")) {
    int passos = abs(cmd.substring(3).toInt());
    if (passos > 0) {
      direcao_atual = LOW;
      digitalWrite(DirX, direcao_atual);
      digitalWrite(DirY, HIGH);
      motor_movendo = true;
      passos_restantes = passos;
      parar_motor = false;
      Serial.print(F(">>> Rotacionando "));
      Serial.print(passos);
      Serial.println(F(" passos (trás)"));
    }
  }
  // Comando V:2000 - Velocidade
  else if (cmd.startsWith("V:")) {
    int novaVelocidade = cmd.substring(2).toInt();
    if (novaVelocidade >= 500 && novaVelocidade <= 10000) {
      delayUs = novaVelocidade;
      Serial.print(F(">>> Velocidade: "));
      Serial.print(delayUs);
      Serial.println(F("us"));
    } else {
      Serial.println(F(">>> Velocidade inválida (500-10000)"));
    }
  }
  // Comando STATUS
  else if (cmd == "STATUS") {
    mostrarStatus();
  }
}

void executarPasso() {
  // Motor X
  digitalWrite(StepX, HIGH);
  delayMicroseconds(10);
  digitalWrite(StepX, LOW);
  
  // Motor Y (sincronizado)
  digitalWrite(StepY, HIGH);
  delayMicroseconds(10);
  digitalWrite(StepY, LOW);
  
  delayMicroseconds(delayUs);
  
  if (passos_restantes > 0) {
    passos_restantes--;
  }
}

void mostrarStatus() {
  Serial.println(F("--- STATUS ---"));
  Serial.print(F("Velocidade: "));
  Serial.print(delayUs);
  Serial.println(F("us"));
  Serial.print(F("Motor: "));
  Serial.println(motor_movendo ? F("MOVENDO") : F("PARADO"));
  if (motor_movendo) {
    Serial.print(F("Direção: "));
    Serial.println(direcao_atual == HIGH ? F("FRENTE") : F("TRÁS"));
    if (passos_restantes > 0) {
      Serial.print(F("Passos restantes: "));
      Serial.println(passos_restantes);
    } else if (passos_restantes == -1) {
      Serial.println(F("Movimento: CONTÍNUO"));
    }
  }
  Serial.println(F("-------------"));
}

