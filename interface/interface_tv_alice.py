#!/usr/bin/env python3
"""
Interface Web para TV Alice - Controle Sincronizado de Rolos
Acesse http://localhost:5000 no navegador
"""

from flask import Flask, render_template_string, request, jsonify
import serial
import time
import threading
import re

app = Flask(__name__)

# Variáveis globais
ser = None
connected = False
port = '/dev/cu.usbmodem1301'
baudrate = 9600
messages_buffer = []

# HTML da interface
HTML_TEMPLATE = """
<!DOCTYPE html>
<html>
<head>
    <title>TV Alice - Controle de Rolos</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            padding: 20px;
            min-height: 100vh;
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            border-radius: 15px;
            padding: 30px;
            box-shadow: 0 10px 40px rgba(0,0,0,0.2);
        }
        h1 {
            color: #667eea;
            margin-bottom: 10px;
            text-align: center;
        }
        .subtitle {
            text-align: center;
            color: #666;
            margin-bottom: 30px;
        }
        .section {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 20px;
        }
        .section h2 {
            color: #333;
            margin-bottom: 15px;
            font-size: 18px;
        }
        .status-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
            margin-bottom: 20px;
        }
        .status-box {
            background: white;
            border: 2px solid #667eea;
            border-radius: 10px;
            padding: 15px;
        }
        .status-box h3 {
            color: #667eea;
            margin-bottom: 10px;
            font-size: 16px;
        }
        .status-value {
            font-size: 24px;
            font-weight: bold;
            color: #333;
            margin: 5px 0;
        }
        .status-label {
            font-size: 12px;
            color: #666;
        }
        .input-group {
            display: flex;
            gap: 10px;
            margin-bottom: 15px;
            align-items: center;
        }
        input[type="number"] {
            padding: 10px;
            border: 2px solid #ddd;
            border-radius: 5px;
            font-size: 16px;
            width: 150px;
        }
        button {
            padding: 12px 24px;
            border: none;
            border-radius: 5px;
            font-size: 16px;
            cursor: pointer;
            transition: all 0.3s;
            font-weight: 600;
        }
        .btn-primary {
            background: #667eea;
            color: white;
        }
        .btn-primary:hover {
            background: #5568d3;
            transform: translateY(-2px);
        }
        .btn-success {
            background: #28a745;
            color: white;
        }
        .btn-success:hover {
            background: #218838;
        }
        .btn-danger {
            background: #dc3545;
            color: white;
        }
        .btn-danger:hover {
            background: #c82333;
        }
        .btn-warning {
            background: #ffc107;
            color: #333;
        }
        .btn-warning:hover {
            background: #e0a800;
        }
        .quick-buttons {
            display: flex;
            gap: 10px;
            flex-wrap: wrap;
        }
        .quick-buttons button {
            flex: 1;
            min-width: 100px;
        }
        .status {
            padding: 15px;
            border-radius: 5px;
            margin-bottom: 20px;
            text-align: center;
            font-weight: 600;
        }
        .status.connected {
            background: #d4edda;
            color: #155724;
        }
        .status.disconnected {
            background: #f8d7da;
            color: #721c24;
        }
        .page-control {
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 20px;
            padding: 20px;
            background: white;
            border-radius: 10px;
            border: 2px solid #667eea;
        }
        .page-display {
            font-size: 32px;
            font-weight: bold;
            color: #667eea;
            min-width: 150px;
            text-align: center;
        }
        .messages {
            background: #1e1e1e;
            color: #00ff00;
            padding: 15px;
            border-radius: 5px;
            height: 200px;
            overflow-y: auto;
            font-family: 'Courier New', monospace;
            font-size: 12px;
        }
        .messages div {
            margin-bottom: 5px;
        }
        .visualization {
            background: #fff;
            border: 2px solid #667eea;
            border-radius: 10px;
            padding: 20px;
            margin-top: 20px;
        }
        .rolo-visual {
            display: flex;
            align-items: center;
            justify-content: space-around;
            margin: 20px 0;
        }
        .rolo-circle {
            width: 80px;
            height: 80px;
            border-radius: 50%;
            border: 3px solid #667eea;
            display: flex;
            align-items: center;
            justify-content: center;
            font-weight: bold;
            color: #667eea;
            background: #f0f0f0;
        }
        .arrow {
            font-size: 32px;
            color: #667eea;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>📺 TV Alice - Controle de Rolos</h1>
        <div class="subtitle">Sistema Sincronizado de Enrolamento</div>
        
        <div id="status" class="status disconnected">
            Desconectado
        </div>
        
        <div class="section">
            <h2>📡 Conexão</h2>
            <div class="input-group">
                <input type="text" id="port" value="/dev/cu.usbmodem1301" placeholder="Porta serial">
                <button class="btn-primary" onclick="connect()">Conectar</button>
                <button class="btn-danger" onclick="disconnect()">Desconectar</button>
            </div>
        </div>
        
        <div class="section">
            <h2>📊 Status dos Rolos</h2>
            <div class="status-grid">
                <div class="status-box">
                    <h3>Rolo X</h3>
                    <div class="status-label">Comprimento Enrolado</div>
                    <div class="status-value" id="compX">0.0 cm</div>
                    <div class="status-label">Diâmetro</div>
                    <div class="status-value" id="diamX">41.0 mm</div>
                </div>
                <div class="status-box">
                    <h3>Rolo Y</h3>
                    <div class="status-label">Comprimento Enrolado</div>
                    <div class="status-value" id="compY">150.0 cm</div>
                    <div class="status-label">Diâmetro</div>
                    <div class="status-value" id="diamY">41.0 mm</div>
                </div>
            </div>
        </div>
        
        <div class="section">
            <h2>📄 Navegação por Páginas</h2>
            <div class="page-control">
                <button class="btn-danger" onclick="movePage(-1)">◀◀ Anterior</button>
                <div class="page-display" id="pageDisplay">1 / 7</div>
                <button class="btn-success" onclick="movePage(1)">Próxima ▶▶</button>
            </div>
            <div class="quick-buttons" style="margin-top: 15px; justify-content: center;">
                <button class="btn-primary" onclick="goToFirstPage()">⏮ Primeira</button>
                <button class="btn-primary" onclick="goToLastPage()">Última ⏭</button>
                <button class="btn-warning" onclick="movePage(-5)">-5 páginas</button>
                <button class="btn-warning" onclick="movePage(5)">+5 páginas</button>
            </div>
            <div class="input-group" style="justify-content: center; margin-top: 15px;">
                <input type="number" id="pageInput" value="1" min="1" placeholder="Página">
                <button class="btn-primary" onclick="goToPage()">Ir para Página</button>
            </div>
            <div class="input-group" style="justify-content: center; margin-top: 10px;">
                <input type="number" id="pageDelta" value="1" min="-10" max="10" placeholder="Páginas">
                <button class="btn-primary" onclick="movePageDelta()">Mover N Páginas</button>
            </div>
            <div class="quick-buttons" style="margin-top: 15px;">
                <button class="btn-warning" onclick="setPageLength(10)">10cm</button>
                <button class="btn-warning" onclick="setPageLength(15)">15cm</button>
                <button class="btn-warning" onclick="setPageLength(20)">20cm</button>
                <button class="btn-warning" onclick="setPageLength(25)">25cm</button>
                <button class="btn-warning" onclick="setPageLength(30)">30cm</button>
                <button class="btn-warning" onclick="setPageLengthCustom()">Personalizado</button>
            </div>
        </div>
        
        <div class="section">
            <h2>⚙️ Movimento Manual</h2>
            <div class="input-group">
                <input type="number" id="moveCm" value="10" min="0.1" step="0.1" placeholder="cm">
                <button class="btn-success" onclick="moveSync(true)">▶ Frente</button>
                <button class="btn-danger" onclick="moveSync(false)">◀ Trás</button>
            </div>
        </div>
        
        <div class="section">
            <h2>⚡ Velocidade</h2>
            <div class="input-group">
                <input type="number" id="speed" value="2000" min="500" max="10000" placeholder="μs">
                <button class="btn-primary" onclick="setSpeed()">Definir</button>
                <button class="btn-warning" onclick="changeSpeed('+')">+</button>
                <button class="btn-warning" onclick="changeSpeed('-')">-</button>
            </div>
        </div>
        
        <div class="section">
            <h2>🎮 Controles</h2>
            <div class="quick-buttons">
                <button class="btn-danger" onclick="stopMotor()" style="font-size: 18px; padding: 15px 30px;">🛑 PARAR MOTOR</button>
                <button class="btn-primary" onclick="resetPosition()">🔄 Reset</button>
                <button class="btn-primary" onclick="getStatus()">📊 Atualizar Status</button>
            </div>
        </div>
        
        <div class="section">
            <h2>💬 Mensagens</h2>
            <div id="messages" class="messages"></div>
        </div>
    </div>
    
    <script>
        let isConnected = false;
        let currentPage = 1;
        let totalPages = 7;
        
        function addMessage(msg) {
            const messages = document.getElementById('messages');
            const time = new Date().toLocaleTimeString();
            messages.innerHTML += `<div>[${time}] ${msg}</div>`;
            messages.scrollTop = messages.scrollHeight;
        }
        
        function updateStatus(connected) {
            const status = document.getElementById('status');
            isConnected = connected;
            if (connected) {
                status.textContent = '✅ Conectado';
                status.className = 'status connected';
            } else {
                status.textContent = '❌ Desconectado';
                status.className = 'status disconnected';
            }
        }
        
        function updateRoloStatus(data) {
            if (!data) return;
            
            console.log('Atualizando status com:', data);
            
            // Parse status do Arduino - procurar em todas as linhas
            const lines = data.split('\n');
            let compX, diamX, compY, diamY, page;
            
            for (let line of lines) {
                // Rolo X
                const matchX = line.match(/Rolo X: ([\d.]+)cm.*Diâmetro: ([\d.]+)mm/);
                if (matchX) {
                    compX = matchX[1];
                    diamX = matchX[2];
                }
                
                // Rolo Y
                const matchY = line.match(/Rolo Y: ([\d.]+)cm.*Diâmetro: ([\d.]+)mm/);
                if (matchY) {
                    compY = matchY[1];
                    diamY = matchY[2];
                }
                
                // Página
                const matchPage = line.match(/Página: (\d+)\/(\d+)/);
                if (matchPage) {
                    page = {current: parseInt(matchPage[1]), total: parseInt(matchPage[2])};
                }
            }
            
            // Atualizar interface
            if (compX) {
                document.getElementById('compX').textContent = compX + ' cm';
                document.getElementById('diamX').textContent = diamX + ' mm';
            }
            if (compY) {
                document.getElementById('compY').textContent = compY + ' cm';
                document.getElementById('diamY').textContent = diamY + ' mm';
            }
            if (page) {
                currentPage = page.current;
                totalPages = page.total;
                document.getElementById('pageDisplay').textContent = `${currentPage} / ${totalPages}`;
            }
        }
        
        async function connect() {
            const port = document.getElementById('port').value;
            const response = await fetch('/connect', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({port: port})
            });
            const data = await response.json();
            if (data.success) {
                updateStatus(true);
                addMessage('✅ Conectado ao Arduino');
                getStatus();
            } else {
                addMessage('❌ Erro: ' + data.error);
            }
        }
        
        async function disconnect() {
            const response = await fetch('/disconnect', {method: 'POST'});
            const data = await response.json();
            updateStatus(false);
            addMessage('❌ Desconectado');
        }
        
        async function sendCommand(cmd) {
            if (!isConnected) {
                addMessage('⚠️ Arduino não conectado!');
                return;
            }
            try {
                const response = await fetch('/command', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({command: cmd})
                });
                const data = await response.json();
                addMessage(`📤 ${cmd}`);
                
                if (data.error) {
                    addMessage(`❌ Erro: ${data.error}`);
                    return;
                }
                
                if (data.message) {
                    console.log('Resposta do Arduino:', data.message);
                    // Mostrar apenas as últimas linhas relevantes
                    const lines = data.message.split('\n').filter(l => l.trim());
                    const relevantLines = lines.slice(-5).join(' | ');
                    if (relevantLines) {
                        addMessage(`📥 ${relevantLines}`);
                    }
                    updateRoloStatus(data.message);
                }
            } catch (error) {
                addMessage(`❌ Erro ao enviar comando: ${error.message}`);
                console.error('Erro:', error);
            }
        }
        
        function movePage(delta) {
            if (!isConnected) {
                addMessage('⚠️ Arduino não conectado!');
                return;
            }
            console.log('Enviando PAGE:', delta);
            sendCommand(`PAGE:${delta}`);
        }
        
        function movePageDelta() {
            const delta = parseInt(document.getElementById('pageDelta').value);
            if (isNaN(delta) || delta === 0) {
                addMessage('⚠️ Digite um valor válido');
                return;
            }
            console.log('Enviando PAGE:', delta);
            sendCommand(`PAGE:${delta}`);
        }
        
        function goToPage() {
            const page = parseInt(document.getElementById('pageInput').value);
            if (isNaN(page) || page < 1) {
                addMessage('⚠️ Digite um número de página válido');
                return;
            }
            const delta = page - currentPage;
            if (delta === 0) {
                addMessage('ℹ️ Já está na página ' + page);
                return;
            }
            console.log('Indo para página', page, 'delta:', delta);
            sendCommand(`PAGE:${delta}`);
        }
        
        function goToFirstPage() {
            const delta = 1 - currentPage;
            if (delta === 0) {
                addMessage('ℹ️ Já está na primeira página');
                return;
            }
            console.log('Indo para primeira página, delta:', delta);
            sendCommand(`PAGE:${delta}`);
        }
        
        function goToLastPage() {
            const delta = totalPages - currentPage;
            if (delta === 0) {
                addMessage('ℹ️ Já está na última página');
                return;
            }
            console.log('Indo para última página, delta:', delta);
            sendCommand(`PAGE:${delta}`);
        }
        
        function setPageLength(cm) {
            sendCommand(`SETPAGE:${cm}`);
        }
        
        function setPageLengthCustom() {
            const cm = prompt('Digite o comprimento da página em cm:', '20');
            if (cm && !isNaN(cm) && cm > 0) {
                sendCommand(`SETPAGE:${cm}`);
            }
        }
        
        function stopMotor() {
            sendCommand('STOP');
            addMessage('🛑 Comando de parar enviado');
        }
        
        function moveSync(forward) {
            const cm = parseFloat(document.getElementById('moveCm').value);
            if (isNaN(cm) || cm <= 0) {
                addMessage('⚠️ Digite um valor válido em cm');
                return;
            }
            const cmd = forward ? `SYNC:${cm}` : `SYNC:-${cm}`;
            sendCommand(cmd);
        }
        
        function setSpeed() {
            const speed = document.getElementById('speed').value;
            sendCommand('SPEED:' + speed);
        }
        
        function changeSpeed(dir) {
            sendCommand(dir);
        }
        
        function resetPosition() {
            sendCommand('RESET');
        }
        
        function getStatus() {
            sendCommand('STATUS');
        }
        
        // Atualizar status periodicamente
        setInterval(() => {
            if (isConnected) {
                getStatus();
            }
        }, 3000);
    </script>
</body>
</html>
"""

@app.route('/')
def index():
    return render_template_string(HTML_TEMPLATE)

@app.route('/connect', methods=['POST'])
def connect_arduino():
    global ser, connected, port
    try:
        data = request.json
        port = data.get('port', '/dev/cu.usbmodem1301')
        
        if ser:
            ser.close()
        
        ser = serial.Serial(port, baudrate, timeout=1)
        time.sleep(2)
        connected = True
        return jsonify({'success': True})
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})

@app.route('/disconnect', methods=['POST'])
def disconnect_arduino():
    global ser, connected
    if ser:
        ser.close()
        ser = None
    connected = False
    return jsonify({'success': True})

@app.route('/command', methods=['POST'])
def send_command():
    global ser, connected
    if not connected or not ser:
        return jsonify({'success': False, 'error': 'Arduino não conectado'})
    
    try:
        data = request.json
        command = data.get('command', '')
        
        # Limpar buffer antes de enviar
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        
        # Enviar comando
        ser.write(f"{command}\n".encode())
        ser.flush()
        
        # Ler resposta com timeout maior
        time.sleep(0.3)  # Dar tempo para Arduino processar
        message = ""
        start_time = time.time()
        max_wait = 1.0  # 1 segundo máximo
        
        while (time.time() - start_time) < max_wait:
            if ser.in_waiting:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    message += line + "\n"
                    # Se recebeu várias linhas, continuar lendo
                    if ser.in_waiting == 0:
                        time.sleep(0.1)  # Pequeno delay para ver se vem mais
                        if ser.in_waiting == 0:
                            break
            else:
                time.sleep(0.05)
        
        return jsonify({'success': True, 'message': message.strip()})
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})

if __name__ == '__main__':
    print("=" * 50)
    print("Interface Web - TV Alice")
    print("=" * 50)
    print(f"Acesse: http://localhost:5000")
    print("=" * 50)
    app.run(debug=True, host='0.0.0.0', port=5000)

