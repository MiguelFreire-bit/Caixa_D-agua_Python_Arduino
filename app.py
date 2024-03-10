from flask import Flask, render_template, request, jsonify
import serial
import time
import json

app = Flask(__name__)

# Inicializa a comunicação serial (ajuste a porta conforme necessário)
ser = serial.Serial('COM7', 9600, timeout=1)
time.sleep(2)  # Aguarda a inicialização da porta serial

# Variável para armazenar os dados recebidos
dados_buffer = []

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/atualizar_dados', methods=['POST'])
def atualizar_dados():
    global dados_buffer

    # Lê os dados do Arduino
    ser.write(b'r')  # Comando para solicitar dados
    dados_recebidos  = ser.readline().decode('utf-8').strip()
    print(dados_recebidos )

    # Concatena os dados recebidos no buffer
    dados_buffer.append(dados_recebidos)

# Verifica se o último caractere é '}' (fim do JSON)
    if dados_recebidos.endswith('}'):
        # Concatena todos os dados no buffer
        dados_completos = ''.join(dados_buffer)

        # Limpa o buffer
        dados_buffer = []

        try:
            # Parseia os dados JSON
            dados_json = json.loads(dados_completos)

            # Processa os dados normalmente
            dados_arduino = {
                "nivel_minimo": str(dados_json.get("nivel_minimo", False)),
                "nivel_50": str(dados_json.get("nivel_50", False)),
                "nivel_100": str(dados_json.get("nivel_100", False)),
                "bomba_estado": "Ligada" if dados_json.get("bomba_estado", False) else "Desligada"
            }

            return jsonify(dados_arduino)
        except json.JSONDecodeError as e:
            return jsonify({"error": str(e)}), 400  # Retorna um erro 400 se houver problema no JSON

    return jsonify({"status": "Dados recebidos parcialmente, aguarde mais dados."})

if __name__ == '__main__':
    app.run(debug=True)