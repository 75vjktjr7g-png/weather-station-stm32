from flask import Flask, render_template, jsonify
import serial, threading, time

app = Flask(__name__)

SERIAL_PORT = "COM3"   # change if needed
BAUD_RATE = 115200

latest_data = {
    "temperature": "--",
    "humidity": "--",
    "pressure": "--"
}

def read_serial():
    global latest_data
    ser = None

    while ser is None:
        try:
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
            print("✅ Serial connected on", SERIAL_PORT)
        except Exception as e:
            print("❌ Serial open failed:", e)
            time.sleep(2)

    while True:
        try:
            line = ser.readline().decode(errors="ignore").strip()
            if not line:
                continue

            print("RAW:", line)

            # Expected: T=17 C  H=82 %  P=994 hPa
            if line.startswith("T="):
                try:
                    parts = line.split()

                    temp = parts[0].replace("T=", "")
                    hum  = parts[2].replace("H=", "")
                    pres = parts[4].replace("P=", "")

                    latest_data = {
                        "temperature": temp,
                        "humidity": hum,
                        "pressure": pres
                    }

                    print("✅ Updated:", latest_data)

                except Exception as parse_err:
                    print("❌ Parse error:", parse_err)

        except Exception as e:
            print("Serial read error:", e)
            time.sleep(1)

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/data")
def data():
    return jsonify(latest_data)

if __name__ == "__main__":
    threading.Thread(target=read_serial, daemon=True).start()
    app.run(debug=True, use_reloader=False)
