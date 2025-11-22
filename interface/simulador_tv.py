#!/usr/bin/env python3
"""
Simulador TV Alice - Visualização do Sistema de Rolos
Simula movimento dos rolos e mostra como diâmetros mudam
"""

import math

class SimuladorTV:
    def __init__(self, diametro_inicial_mm=41, comprimento_total_cm=150):
        self.diametro_inicial_mm = diametro_inicial_mm
        self.comprimento_total_cm = comprimento_total_cm
        self.comprimento_enrolado_X = 0  # cm
        self.comprimento_enrolado_Y = comprimento_total_cm  # Todo papel no Y inicialmente
        
    def calcular_diametro(self, comprimento_enrolado_cm):
        """Calcula diâmetro atual baseado no comprimento enrolado"""
        # Aproximação: considerando espessura do papel
        # Fator de espessura (ajustar conforme papel real)
        espessura_papel_mm = 0.1
        fator = espessura_papel_mm * 2.0
        
        # Diâmetro aumenta proporcionalmente ao comprimento enrolado
        # Fórmula simplificada: diâmetro = inicial + (comprimento × fator / perímetro_inicial)
        perimetro_inicial_mm = math.pi * self.diametro_inicial_mm
        diametro_atual = self.diametro_inicial_mm + (comprimento_enrolado_cm * 10.0 * fator / perimetro_inicial_mm)
        
        # Limite mínimo
        if diametro_atual < self.diametro_inicial_mm:
            diametro_atual = self.diametro_inicial_mm
            
        return diametro_atual
    
    def calcular_perimetro(self, diametro_mm):
        """Calcula perímetro em cm"""
        return (math.pi * diametro_mm) / 10  # mm para cm
    
    def calcular_passos_necessarios(self, comprimento_cm, diametro_mm):
        """Calcula passos necessários para mover comprimento dado"""
        perimetro_cm = self.calcular_perimetro(diametro_mm)
        voltas = comprimento_cm / perimetro_cm
        passos = voltas * 200  # NEMA 17: 200 passos/volta
        return passos
    
    def calcular_relacao_velocidade(self, comprimento_cm):
        """Calcula relação de velocidade entre X e Y para mover comprimento"""
        diametro_X = self.calcular_diametro(self.comprimento_enrolado_X)
        diametro_Y = self.calcular_diametro(self.comprimento_enrolado_Y)
        
        passos_X = self.calcular_passos_necessarios(comprimento_cm, diametro_X)
        passos_Y = self.calcular_passos_necessarios(comprimento_cm, diametro_Y)
        
        return {
            'diametro_X': diametro_X,
            'diametro_Y': diametro_Y,
            'passos_X': passos_X,
            'passos_Y': passos_Y,
            'relacao': passos_Y / passos_X if passos_X > 0 else 0
        }
    
    def mover_papel(self, comprimento_cm, direcao='frente'):
        """Simula movimento do papel"""
        if direcao == 'frente':
            # X enrola, Y desenrola
            self.comprimento_enrolado_X += comprimento_cm
            self.comprimento_enrolado_Y -= comprimento_cm
        else:
            # X desenrola, Y enrola
            self.comprimento_enrolado_X -= comprimento_cm
            self.comprimento_enrolado_Y += comprimento_cm
        
        # Limites
        self.comprimento_enrolado_X = max(0, min(self.comprimento_enrolado_X, self.comprimento_total_cm))
        self.comprimento_enrolado_Y = self.comprimento_total_cm - self.comprimento_enrolado_X
    
    def get_status(self):
        """Retorna status atual"""
        diametro_X = self.calcular_diametro(self.comprimento_enrolado_X)
        diametro_Y = self.calcular_diametro(self.comprimento_enrolado_Y)
        
        return {
            'comprimento_X': self.comprimento_enrolado_X,
            'comprimento_Y': self.comprimento_enrolado_Y,
            'diametro_X': diametro_X,
            'diametro_Y': diametro_Y,
            'perimetro_X': self.calcular_perimetro(diametro_X),
            'perimetro_Y': self.calcular_perimetro(diametro_Y)
        }
    
    def simular_movimento(self, comprimento_cm, direcao='frente'):
        """Simula movimento e retorna informações"""
        antes = self.get_status()
        relacao_antes = self.calcular_relacao_velocidade(comprimento_cm)
        
        self.mover_papel(comprimento_cm, direcao)
        
        depois = self.get_status()
        
        return {
            'antes': antes,
            'depois': depois,
            'relacao': relacao_antes,
            'comprimento_movido': comprimento_cm
        }
    
    def visualizar_sistema(self, comprimento_pagina_cm=20):
        """Visualiza o sistema atual"""
        status = self.get_status()
        
        print("=" * 60)
        print("SIMULADOR TV ALICE - Estado Atual")
        print("=" * 60)
        print(f"\n📊 Rolos:")
        print(f"  Rolo X (enrola):")
        print(f"    Comprimento enrolado: {status['comprimento_X']:.2f} cm")
        print(f"    Diâmetro atual: {status['diametro_X']:.2f} mm")
        print(f"    Perímetro: {status['perimetro_X']:.2f} cm/volta")
        print(f"  Rolo Y (desenrola):")
        print(f"    Comprimento enrolado: {status['comprimento_Y']:.2f} cm")
        print(f"    Diâmetro atual: {status['diametro_Y']:.2f} mm")
        print(f"    Perímetro: {status['perimetro_Y']:.2f} cm/volta")
        
        # Calcular páginas
        total_paginas = int(self.comprimento_total_cm / comprimento_pagina_cm)
        pagina_atual = int(status['comprimento_X'] / comprimento_pagina_cm) + 1
        
        print(f"\n📄 Páginas:")
        print(f"  Comprimento por página: {comprimento_pagina_cm} cm")
        print(f"  Total de páginas: {total_paginas}")
        print(f"  Página atual: {pagina_atual}/{total_paginas}")
        
        # Teste de movimento
        print(f"\n🔧 Teste: Mover 10cm para frente")
        resultado = self.simular_movimento(10, 'frente')
        print(f"  Passos necessários X: {resultado['relacao']['passos_X']:.0f}")
        print(f"  Passos necessários Y: {resultado['relacao']['passos_Y']:.0f}")
        print(f"  Relação Y/X: {resultado['relacao']['relacao']:.2f}x")
        print(f"  (Y precisa girar {resultado['relacao']['relacao']:.2f}x mais rápido que X)")
        
        # Reverter movimento para manter estado
        self.mover_papel(10, 'tras')
        
        print("=" * 60)

def main():
    print("Simulador TV Alice - Sistema de Rolos Sincronizados\n")
    
    # Criar simulador
    sim = SimuladorTV(diametro_inicial_mm=41, comprimento_total_cm=150)
    
    # Estado inicial
    print("\n📍 Estado Inicial:")
    sim.visualizar_sistema(comprimento_pagina_cm=20)
    
    # Simular alguns movimentos
    print("\n\n🎬 Simulando Movimentos:\n")
    
    movimentos = [
        (10, 'frente', 'Mover 10cm para frente'),
        (20, 'frente', 'Mover 20cm (1 página) para frente'),
        (50, 'frente', 'Mover 50cm para frente'),
        (30, 'tras', 'Mover 30cm para trás'),
    ]
    
    for comprimento, direcao, descricao in movimentos:
        print(f"\n{descricao}:")
        resultado = sim.simular_movimento(comprimento, direcao)
        status = sim.get_status()
        
        print(f"  Após movimento:")
        print(f"    Rolo X: {status['comprimento_X']:.1f}cm enrolado, {status['diametro_X']:.1f}mm diâmetro")
        print(f"    Rolo Y: {status['comprimento_Y']:.1f}cm enrolado, {status['diametro_Y']:.1f}mm diâmetro")
        print(f"    Relação Y/X: {resultado['relacao']['relacao']:.2f}x")

if __name__ == '__main__':
    main()

