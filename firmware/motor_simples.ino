/*
 * Controle Simples de Motor NEMA 17
 * Comandos via Serial:
 * F - Gira para frente (direita)
 * B - Gira para trás (esquerda)
 * S - Parar
 * R:200 - Rotacionar 200 passos
 * V:2000 - Velocidade (microsegundos)
 */

// Pinos do Shield CNC V3
const int StepX = 2;
const int DirX = 5;
const int StepY = 3;
const int DirY = 6;
const int EnablePin = 8;

// Variáveis
int delayUs = 2000;  // Velocidade (microsegundos)
bool motor_movendo = false;
bool parar_motor = false;
int passos_restantes = 0;
bool direcao_atual = HIGH;  // HIGH = frente, LOW = trás

void setup() {
  Serial.begin(9600);
  delay(2000);  // Aguardar mais tempo
  
  // Configurar pinos
  pinMode(EnablePin, OUTPUT);
  pinMode(StepX, OUTPUT);
  pinMode(DirX, OUTPUT);
  pinMode(StepY, OUTPUT);
  pinMode(DirY, OUTPUT);
  
  // Habilitar motores (LOW = habilitado)
  digitalWrite(EnablePin, LOW);
  digitalWrite(DirX, HIGH);
  digitalWrite(DirY, HIGH);
  
  // Estado inicial
  digitalWrite(StepX, LOW);
  digitalWrite(StepY, LOW);
  
  Serial.println(F("=== MOTOR SIMPLES - PRONTO ==="));
  Serial.println(F("Comandos:"));
  Serial.println(F("  F - Gira frente (direita)"));
  Serial.println(F("  B - Gira trás (esquerda)"));
  Serial.println(F("  S - Parar"));
  Serial.println(F("  R:200 - Rotacionar 200 passos"));
  Serial.println(F("  V:2000 - Velocidade (us)"));
  Serial.println(F(""));
  Serial.println(F(">>> Aguardando comandos..."));
}

void loop() {
  // Verificar comandos serial
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();
    
    Serial.print(F(">>> Comando recebido: ["));
    Serial.print(cmd);
    Serial.println(F("]"));
    
    processarComando(cmd);
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
}

void processarComando(String cmd) {
  // Comando F - Frente (direita)
  if (cmd == "F") {
    direcao_atual = HIGH;
    digitalWrite(DirX, direcao_atual);
    digitalWrite(DirY, LOW);  // Y gira em direção oposta para sincronizar
    motor_movendo = true;
    passos_restantes = -1;  // -1 = movimento contínuo
    parar_motor = false;
    Serial.println(F(">>> Girando FRENTE (direita) - contínuo - AMBOS MOTORES"));
  }
  // Comando B - Trás (esquerda)
  else if (cmd == "B") {
    direcao_atual = LOW;
    digitalWrite(DirX, direcao_atual);
    digitalWrite(DirY, HIGH);  // Y gira em direção oposta para sincronizar
    motor_movendo = true;
    passos_restantes = -1;  // -1 = movimento contínuo
    parar_motor = false;
    Serial.println(F(">>> Girando TRÁS (esquerda) - contínuo - AMBOS MOTORES"));
  }
  // Comando S - Parar
  else if (cmd == "S" || cmd == "STOP") {
    parar_motor = true;
    motor_movendo = false;
    passos_restantes = 0;
    Serial.println(F(">>> PARAR"));
  }
  // Comando R:200 - Rotacionar N passos
  else if (cmd.startsWith("R:")) {
    int passos = cmd.substring(2).toInt();
    if (passos > 0) {
      direcao_atual = HIGH;
      digitalWrite(DirX, direcao_atual);
      digitalWrite(DirY, LOW);  // Y gira em direção oposta
      motor_movendo = true;
      passos_restantes = passos;
      parar_motor = false;
      Serial.print(F(">>> Rotacionando "));
      Serial.print(passos);
      Serial.println(F(" passos (frente) - AMBOS MOTORES"));
    }
  }
  // Comando R:-200 - Rotacionar N passos para trás
  else if (cmd.startsWith("R:-")) {
    int passos = abs(cmd.substring(3).toInt());
    if (passos > 0) {
      direcao_atual = LOW;
      digitalWrite(DirX, direcao_atual);
      digitalWrite(DirY, HIGH);  // Y gira em direção oposta
      motor_movendo = true;
      passos_restantes = passos;
      parar_motor = false;
      Serial.print(F(">>> Rotacionando "));
      Serial.print(passos);
      Serial.println(F(" passos (trás) - AMBOS MOTORES"));
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
  else if (cmd == "STATUS" || cmd == "S") {
    mostrarStatus();
  }
  else if (cmd.length() > 0) {
    Serial.print(F(">>> Comando inválido: "));
    Serial.println(cmd);
  }
}

void executarPasso() {
  // Motor X
  digitalWrite(StepX, HIGH);
  delayMicroseconds(10);  // Pulso mínimo
  digitalWrite(StepX, LOW);
  
  // Motor Y (sincronizado)
  digitalWrite(StepY, HIGH);
  delayMicroseconds(10);  // Pulso mínimo
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
  Serial.print(F("Enable Pin: "));
  Serial.println(digitalRead(EnablePin) == LOW ? F("HABILITADO") : F("DESABILITADO"));
  Serial.println(F("-------------"));
}
