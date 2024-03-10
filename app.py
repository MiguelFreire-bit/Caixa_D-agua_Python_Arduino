from flask import Flask, render_template, request, jsonify
import serial
import time
import json

app = Flask(__name__)

# Inicializa a comunicação serial (ajuste a porta conforme necessário)
ser = serial.Serial('COM7', 9600, timeout=1)
time.sleep(2)  # Aguarda a inicialização da porta serial

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/atualizar_dados', methods=['POST'])
def atualizar_dados():
    # Lê os dados do Arduino
    ser.write(b'r')  # Comando para solicitar dados
    dados = ser.readline().decode('utf-8').strip().split(',')
    print(dados)


    dados_arduino = {
        "nivel_minimo": "True" if "true" in dados[0] else "False",
        "nivel_50": "True" if "true" in dados[1] else "False",
        "nivel_100": "True" if "true" in  dados[2] else "False",
        "bomba_estado": "Ligada" if dados[2] !=  '"nivel_100":true' else "Desligada"
    }

    return jsonify(dados_arduino)

if __name__ == '__main__':
    app.run(debug=True)