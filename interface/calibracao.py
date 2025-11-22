#!/usr/bin/env python3
"""
Interface Web para Calibração de Páginas - TV Alice
Acesse http://localhost:5001 no navegador
"""

from flask import Flask, render_template_string, request, jsonify
import serial
import time
import json
import re
from datetime import datetime

app = Flask(__name__)

# Variáveis globais
ser = None
connected = False
port = '/dev/cu.usbmodem1301'
baudrate = 9600
log_messages = []

# HTML da interface
HTML_TEMPLATE = """
<!DOCTYPE html>
<html>
<head>
    <title>TV Alice - Calibração de Páginas</title>
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
            max-width: 1400px;
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
        .status-info {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin-bottom: 20px;
        }
        .info-box {
            background: white;
            border: 2px solid #667eea;
            border-radius: 10px;
            padding: 15px;
            text-align: center;
        }
        .info-label {
            font-size: 12px;
            color: #666;
            margin-bottom: 5px;
        }
        .info-value {
            font-size: 24px;
            font-weight: bold;
            color: #667eea;
        }
        .btn {
            padding: 12px 24px;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            cursor: pointer;
            transition: all 0.3s;
            margin: 5px;
        }
        .btn-primary { background: #667eea; color: white; }
        .btn-primary:hover { background: #5568d3; }
        .btn-success { background: #28a745; color: white; }
        .btn-success:hover { background: #218838; }
        .btn-danger { background: #dc3545; color: white; }
        .btn-danger:hover { background: #c82333; }
        .btn-warning { background: #ffc107; color: #333; }
        .btn-warning:hover { background: #e0a800; }
        .btn-group {
            display: flex;
            flex-wrap: wrap;
            gap: 10px;
            margin: 10px 0;
        }
        .input-group {
            display: flex;
            gap: 10px;
            margin: 10px 0;
            align-items: center;
        }
        input[type="number"] {
            padding: 10px;
            border: 2px solid #ddd;
            border-radius: 5px;
            font-size: 16px;
            width: 120px;
        }
        .mapping-table {
            width: 100%;
            border-collapse: collapse;
            margin: 20px 0;
            background: white;
            border-radius: 10px;
            overflow: hidden;
        }
        .mapping-table th,
        .mapping-table td {
            padding: 12px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }
        .mapping-table th {
            background: #667eea;
            color: white;
            font-weight: bold;
        }
        .mapping-table tr:hover {
            background: #f8f9fa;
        }
        .status-badge {
            padding: 4px 8px;
            border-radius: 4px;
            font-size: 12px;
            font-weight: bold;
        }
        .status-defined {
            background: #28a745;
            color: white;
        }
        .status-pending {
            background: #ffc107;
            color: #333;
        }
        .log-container {
            background: #1e1e1e;
            color: #d4d4d4;
            padding: 15px;
            border-radius: 10px;
            max-height: 300px;
            overflow-y: auto;
            font-family: 'Courier New', monospace;
            font-size: 12px;
            margin-top: 20px;
        }
        .log-entry {
            margin: 5px 0;
            padding: 5px;
        }
        .log-time {
            color: #858585;
        }
        .status {
            padding: 10px;
            border-radius: 8px;
            margin-bottom: 20px;
            text-align: center;
            font-weight: bold;
        }
        .status.connected {
            background: #d4edda;
            color: #155724;
        }
        .status.disconnected {
            background: #f8d7da;
            color: #721c24;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>📐 TV Alice - Calibração de Páginas</h1>
        <div class="subtitle">Sistema de Mapeamento e Medição de Passos</div>
        
        <div id="status" class="status disconnected">
            Desconectado
        </div>
        
        <div class="section">
            <h2>📡 Conexão</h2>
            <div class="input-group">
                <input type="text" id="port" value="/dev/cu.usbmodem1301" placeholder="Porta serial">
                <button class="btn btn-primary" onclick="connect()">Conectar</button>
                <button class="btn btn-danger" onclick="disconnect()">Desconectar</button>
            </div>
        </div>
        
        <div class="section">
            <h2>📊 Status Atual</h2>
            <div class="status-info">
                <div class="info-box">
                    <div class="info-label">Página Atual</div>
                    <div class="info-value" id="currentPage">0</div>
                </div>
                <div class="info-box">
                    <div class="info-label">Passos Acumulados</div>
                    <div class="info-value" id="currentSteps">0</div>
                </div>
                <div class="info-box">
                    <div class="info-label">Páginas Definidas</div>
                    <div class="info-value" id="totalDefined">0</div>
                </div>
            </div>
        </div>
        
        <div class="section">
            <h2>🎮 Movimento Manual</h2>
            <div class="btn-group">
                <button class="btn btn-danger" onclick="moveSteps(-100)">◀◀◀ 100</button>
                <button class="btn btn-danger" onclick="moveSteps(-10)">◀◀ 10</button>
                <button class="btn btn-danger" onclick="moveSteps(-1)">◀ 1</button>
                <button class="btn btn-success" onclick="moveSteps(1)">1 ▶</button>
                <button class="btn btn-success" onclick="moveSteps(10)">10 ▶▶</button>
                <button class="btn btn-success" onclick="moveSteps(100)">100 ▶▶▶</button>
            </div>
            <div class="input-group">
                <input type="number" id="customSteps" value="50" min="1" placeholder="Passos">
                <button class="btn btn-primary" onclick="moveCustomSteps()">Mover</button>
            </div>
        </div>
        
        <div class="section">
            <h2>📍 Marcação de Páginas</h2>
            <div class="btn-group">
                <button class="btn btn-warning" onclick="resetPosition()">🔄 RESET</button>
                <button class="btn btn-success" onclick="markPage()">✓ MARCAR PÁGINA</button>
                <button class="btn btn-primary" onclick="goToPage()">➡️ IR PARA PÁGINA</button>
            </div>
            <div class="input-group">
                <input type="number" id="gotoPage" value="0" min="0" placeholder="Página">
                <button class="btn btn-primary" onclick="goToPage()">Ir</button>
                <button class="btn btn-primary" onclick="nextPage()">Próxima ▶</button>
                <button class="btn btn-primary" onclick="prevPage()">◀ Anterior</button>
            </div>
        </div>
        
        <div class="section">
            <h2>💾 Persistência</h2>
            <div class="btn-group">
                <button class="btn btn-success" onclick="saveMapping()">💾 SALVAR</button>
                <button class="btn btn-primary" onclick="loadMapping()">📂 CARREGAR</button>
                <button class="btn btn-danger" onclick="clearMapping()">🗑️ LIMPAR</button>
                <button class="btn btn-warning" onclick="exportData()">📤 EXPORTAR JSON</button>
            </div>
        </div>
        
        <div class="section">
            <h2>📋 Mapeamento de Páginas</h2>
            <div id="mappingTable">
                <table class="mapping-table">
                    <thead>
                        <tr>
                            <th>Página</th>
                            <th>Passos Acumulados</th>
                            <th>Passos da Página</th>
                            <th>Status</th>
                        </tr>
                    </thead>
                    <tbody id="mappingBody">
                        <tr><td colspan="4" style="text-align: center;">Carregando...</td></tr>
                    </tbody>
                </table>
            </div>
        </div>
        
        <div class="section">
            <h2>📝 Log de Ações</h2>
            <div class="log-container" id="logContainer">
                <div class="log-entry">Aguardando conexão...</div>
            </div>
        </div>
    </div>
    
    <script>
        let isConnected = false;
        let currentPage = 0;
        let currentSteps = 0;
        let totalDefined = 0;
        let mappingData = [];
        
        function addLog(message) {
            const logContainer = document.getElementById('logContainer');
            const time = new Date().toLocaleTimeString();
            const entry = document.createElement('div');
            entry.className = 'log-entry';
            entry.innerHTML = `<span class="log-time">[${time}]</span> ${message}`;
            logContainer.appendChild(entry);
            logContainer.scrollTop = logContainer.scrollHeight;
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
        
        async function sendCommand(cmd) {
            if (!isConnected) {
                addLog('⚠️ Arduino não conectado!');
                return;
            }
            try {
                const response = await fetch('/command', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({command: cmd})
                });
                const data = await response.json();
                addLog(`📤 ${cmd}`);
                if (data.message) {
                    addLog(`📥 ${data.message}`);
                }
                if (data.error) {
                    addLog(`❌ Erro: ${data.error}`);
                }
                // Atualizar status após comando
                setTimeout(getStatus, 500);
            } catch (error) {
                addLog(`❌ Erro: ${error.message}`);
            }
        }
        
        function moveSteps(steps) {
            const cmd = steps > 0 ? `F:${steps}` : `B:${Math.abs(steps)}`;
            sendCommand(cmd);
        }
        
        function moveCustomSteps() {
            const steps = parseInt(document.getElementById('customSteps').value);
            if (steps > 0) {
                moveSteps(steps);
            }
        }
        
        function resetPosition() {
            if (confirm('Deseja realmente resetar a posição? Isso definirá a posição atual como início (0 passos).')) {
                sendCommand('RESET');
            }
        }
        
        function markPage() {
            sendCommand('MARK');
        }
        
        function goToPage() {
            const page = parseInt(document.getElementById('gotoPage').value);
            sendCommand(`GOTO:${page}`);
        }
        
        function nextPage() {
            sendCommand('NEXT');
        }
        
        function prevPage() {
            sendCommand('PREV');
        }
        
        function saveMapping() {
            sendCommand('SAVE');
        }
        
        function loadMapping() {
            sendCommand('LOAD');
        }
        
        function clearMapping() {
            if (confirm('Deseja realmente limpar todo o mapeamento? Esta ação não pode ser desfeita!')) {
                sendCommand('CLEAR');
            }
        }
        
        async function exportData() {
            if (!isConnected) {
                addLog('⚠️ Conecte ao Arduino primeiro');
                return;
            }
            try {
                const response = await fetch('/export');
                const data = await response.json();
                const blob = new Blob([JSON.stringify(data, null, 2)], {type: 'application/json'});
                const url = URL.createObjectURL(blob);
                const a = document.createElement('a');
                a.href = url;
                a.download = `calibracao_${new Date().toISOString().split('T')[0]}.json`;
                a.click();
                URL.revokeObjectURL(url);
                addLog('✅ Dados exportados!');
            } catch (error) {
                addLog(`❌ Erro ao exportar: ${error.message}`);
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
                addLog('✅ Conectado ao Arduino');
                getStatus();
            } else {
                addLog(`❌ Erro: ${data.error}`);
            }
        }
        
        async function disconnect() {
            const response = await fetch('/disconnect', {method: 'POST'});
            const data = await response.json();
            updateStatus(false);
            addLog('❌ Desconectado');
        }
        
        async function getStatus() {
            if (!isConnected) return;
            try {
                const response = await fetch('/status');
                const data = await response.json();
                if (data.success) {
                    updateMappingDisplay(data.mapping);
                    document.getElementById('currentPage').textContent = data.current_page || 0;
                    document.getElementById('currentSteps').textContent = data.current_steps || 0;
                    document.getElementById('totalDefined').textContent = data.total_defined || 0;
                }
            } catch (error) {
                console.error('Erro ao obter status:', error);
            }
        }
        
        function updateMappingDisplay(mapping) {
            const tbody = document.getElementById('mappingBody');
            if (!mapping || mapping.length === 0) {
                tbody.innerHTML = '<tr><td colspan="4" style="text-align: center;">Nenhuma página mapeada ainda</td></tr>';
                return;
            }
            
            let html = '';
            for (let i = 0; i < mapping.length; i++) {
                const p = mapping[i];
                const prevPassos = i > 0 && mapping[i-1].definida ? mapping[i-1].passos : 0;
                const passosPagina = p.definida ? (p.passos - prevPassos) : '-';
                
                html += `
                    <tr>
                        <td>${p.numero}</td>
                        <td>${p.definida ? p.passos : '-'}</td>
                        <td>${passosPagina}</td>
                        <td>
                            <span class="status-badge ${p.definida ? 'status-defined' : 'status-pending'}">
                                ${p.definida ? '✓ Definida' : '⏳ Pendente'}
                            </span>
                        </td>
                    </tr>
                `;
            }
            tbody.innerHTML = html;
        }
        
        // Atualizar status periodicamente
        setInterval(() => {
            if (isConnected) {
                getStatus();
            }
        }, 2000);
        
        // Inicializar
        addLog('Interface de calibração carregada');
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
        
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        
        ser.write(f"{command}\n".encode())
        ser.flush()
        
        time.sleep(0.3)
        message = ""
        start_time = time.time()
        max_wait = 1.0
        
        while (time.time() - start_time) < max_wait:
            if ser.in_waiting:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    message += line + "\n"
                    if ser.in_waiting == 0:
                        time.sleep(0.1)
                        if ser.in_waiting == 0:
                            break
            else:
                time.sleep(0.05)
        
        return jsonify({'success': True, 'message': message.strip()})
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})

@app.route('/status')
def get_status():
    global ser, connected
    if not connected or not ser:
        return jsonify({'success': False, 'error': 'Arduino não conectado'})
    
    try:
        ser.reset_input_buffer()
        ser.write(b"STATUS\n")
        ser.flush()
        
        time.sleep(0.5)
        response = ""
        start_time = time.time()
        
        while (time.time() - start_time) < 2.0:
            if ser.in_waiting:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    response += line + "\n"
                    if "-------------" in line:
                        break
            else:
                time.sleep(0.1)
        
        # Parse do status
        mapping = []
        current_page = 0
        current_steps = 0
        total_defined = 0
        
        lines = response.split('\n')
        for line in lines:
            # Página atual
            if 'Página atual:' in line:
                match = re.search(r'(\d+)', line)
                if match:
                    current_page = int(match.group(1))
            # Passos acumulados
            elif 'Passos acumulados:' in line:
                match = re.search(r'(\d+)', line)
                if match:
                    current_steps = int(match.group(1))
            # Total definidas
            elif 'Total de páginas definidas:' in line:
                match = re.search(r'(\d+)', line)
                if match:
                    total_defined = int(match.group(1))
            # Mapeamento (linhas da tabela)
            elif re.match(r'\s+\d+\s+\|', line):
                parts = [p.strip() for p in line.split('|')]
                if len(parts) >= 3:
                    try:
                        pagina_num = int(parts[0].strip())
                        passos_str = parts[1].strip()
                        definida = '✓' in parts[3] if len(parts) > 3 else False
                        
                        if passos_str != '-' and passos_str:
                            passos = int(passos_str)
                            mapping.append({
                                'numero': pagina_num,
                                'passos': passos,
                                'definida': definida
                            })
                    except:
                        pass
        
        return jsonify({
            'success': True,
            'current_page': current_page,
            'current_steps': current_steps,
            'total_defined': total_defined,
            'mapping': mapping
        })
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})

@app.route('/export')
def export_data():
    global ser, connected
    if not connected or not ser:
        return jsonify({'error': 'Arduino não conectado'})
    
    try:
        # Obter status atual
        status_response = get_status()
        status_data = status_response.get_json()
        
        if not status_data.get('success'):
            return jsonify({'error': 'Não foi possível obter dados'})
        
        export_data = {
            'total_paginas': status_data.get('total_defined', 0),
            'paginas': status_data.get('mapping', []),
            'ultima_atualizacao': datetime.now().isoformat(),
            'pagina_atual': status_data.get('current_page', 0),
            'passos_atual': status_data.get('current_steps', 0)
        }
        
        return jsonify(export_data)
    except Exception as e:
        return jsonify({'error': str(e)})

if __name__ == '__main__':
    print("=" * 50)
    print("Interface de Calibração - TV Alice")
    print("=" * 50)
    print(f"Acesse: http://localhost:5001")
    print("=" * 50)
    app.run(debug=True, host='0.0.0.0', port=5001)

