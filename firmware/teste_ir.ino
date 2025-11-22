/*
 * Teste de Sensor IR - Versão 4.x da biblioteca IRremote
 * 
 * Conexão:
 * - Sensor IR OUT/S → Pino 11 do Arduino
 * - Sensor IR VCC → 5V do Arduino
 * - Sensor IR GND → GND do Arduino
 */

#include <IRremote.hpp>

const int IR_RECEIVE_PIN = 11;  // Pino do sensor IR

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  Serial.println(F("=== TESTE SENSOR IR (v4.x) ==="));
  Serial.print(F("Pino configurado: "));
  Serial.println(IR_RECEIVE_PIN);
  
  // Inicializar receptor IR (nova API)
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  
  Serial.println(F(">>> Sensor IR HABILITADO"));
  Serial.println(F(">>> Aponte o controle e pressione botões"));
  Serial.println(F(""));
}

void loop() {
  // Verificar se recebeu sinal IR (nova API)
  if (IrReceiver.decode()) {
    Serial.println(F(">>> SINAL IR DETECTADO!"));
    
    // Mostrar código recebido
    Serial.print(F("Código HEX: 0x"));
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    
    Serial.print(F("Código DEC: "));
    Serial.println(IrReceiver.decodedIRData.decodedRawData, DEC);
    
    // Mostrar protocolo
    Serial.print(F("Protocolo: "));
    switch(IrReceiver.decodedIRData.protocol) {
      case NEC: Serial.println(F("NEC")); break;
      case SONY: Serial.println(F("SONY")); break;
      case RC5: Serial.println(F("RC5")); break;
      case RC6: Serial.println(F("RC6")); break;
      case SAMSUNG: Serial.println(F("SAMSUNG")); break;
      case LG: Serial.println(F("LG")); break;
      case PANASONIC: Serial.println(F("PANASONIC")); break;
      case JVC: Serial.println(F("JVC")); break;
      case UNKNOWN: Serial.println(F("DESCONHECIDO")); break;
      default: 
        Serial.print(F("OUTRO ("));
        Serial.print(IrReceiver.decodedIRData.protocol);
        Serial.println(F(")"));
        break;
    }
    
    // Mostrar endereço e comando (se disponível)
    if (IrReceiver.decodedIRData.protocol != UNKNOWN) {
      Serial.print(F("Endereço: 0x"));
      Serial.println(IrReceiver.decodedIRData.address, HEX);
      Serial.print(F("Comando: 0x"));
      Serial.println(IrReceiver.decodedIRData.command, HEX);
    }
    
    // Mostrar número de bits
    Serial.print(F("Bits: "));
    Serial.println(IrReceiver.decodedIRData.numberOfBits);
    
    Serial.println(F("---"));
    Serial.println(F(""));
    
    // Continuar recebendo (nova API)
    IrReceiver.resume();
  }
  
  delay(50);
}
