"""
PIFKID 2026 — Web UI Mock Server
Chạy: python simulate.py
Mở: http://localhost:8080
"""

import json
import os
from datetime import datetime
from http.server import HTTPServer, SimpleHTTPRequestHandler

DATA_DIR = os.path.join(os.path.dirname(__file__), '..', 'data')
DB_FILE = os.path.join(os.path.dirname(__file__), 'mock_db.json')

DEFAULT_DATA = {
    "todos": [
        {"id": 1, "title": "Mua sữa", "description": "Sữa tươi 1 lít", "color": "#4fc3f7", "completed": False, "created_at": 1000},
        {"id": 2, "title": "Viết báo cáo", "description": "Báo cáo tuần", "color": "#66bb6a", "completed": True, "created_at": 2000},
    ],
    "alarms": [
        {"id": 1, "hour": 7, "minute": 30, "enabled": True, "sound": "default", "volume": 50, "repeat": [1,2,3,4,5]},
        {"id": 2, "hour": 10, "minute": 0, "enabled": True, "sound": "chime", "volume": 70, "repeat": [0,6]},
    ],
    "schedule": [
        {"id": 1, "day": 1, "start": "08:00", "end": "08:30", "title": "Báo thức", "color": "#ef5350"},
        {"id": 2, "day": 1, "start": "10:00", "end": "10:30", "title": "Ăn cơm", "color": "#66bb6a"},
        {"id": 3, "day": 1, "start": "11:00", "end": "12:00", "title": "Làm việc", "color": "#4fc3f7"},
        {"id": 4, "day": 2, "start": "09:00", "end": "09:30", "title": "Team standup", "color": "#ffa726"},
    ],
    "next_ids": {"todo": 3, "alarm": 3, "schedule": 5}
}

def load_db():
    if os.path.exists(DB_FILE):
        with open(DB_FILE, 'r', encoding='utf-8') as f:
            return json.load(f)
    return json.loads(json.dumps(DEFAULT_DATA))

def save_db(data):
    with open(DB_FILE, 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=2)

db = load_db()
TODOS = db["todos"]
ALARMS = db["alarms"]
SCHEDULE = db["schedule"]
next_todo_id = db["next_ids"]["todo"]
next_alarm_id = db["next_ids"]["alarm"]
next_schedule_id = db["next_ids"]["schedule"]


class MockHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=DATA_DIR, **kwargs)

    def end_headers(self):
        self.send_header('Cache-Control', 'no-store, no-cache, must-revalidate')
        self.send_header('Pragma', 'no-cache')
        self.send_header('Expires', '0')
        super().end_headers()

    def do_GET(self):
        if self.path.startswith('/api/'):
            self.handle_api_get()
        else:
            super().do_GET()

    def do_POST(self):
        if self.path.startswith('/api/'):
            self.handle_api_post()
        else:
            self.send_error(404)

    def do_PUT(self):
        if self.path.startswith('/api/'):
            self.handle_api_put()
        else:
            self.send_error(404)

    def do_DELETE(self):
        if self.path.startswith('/api/'):
            self.handle_api_delete()
        else:
            self.send_error(404)

    def do_OPTIONS(self):
        self.send_response(200)
        self._cors()
        self.end_headers()

    def _cors(self):
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')

    def _json_response(self, data, code=200):
        body = json.dumps(data).encode()
        self.send_response(code)
        self.send_header('Content-Type', 'application/json')
        self._cors()
        self.end_headers()
        self.wfile.write(body)

    def _read_body(self):
        length = int(self.headers.get('Content-Length', 0))
        return json.loads(self.rfile.read(length)) if length > 0 else {}

    def handle_api_get(self):
        global TODOS, ALARMS, SCHEDULE

        if self.path == '/api/status':
            self._json_response({
                "success": True, "code": 200, "message": "OK",
                "data": {
                    "device": {"firmware": "1.0.0", "chip": "ESP32", "flash": "4MB", "heap": "280KB", "mac": "AA:BB:CC:DD:EE:FF", "battery": 85},
                    "time": {"current": datetime.now().strftime("%H:%M"), "date": datetime.now().strftime("%Y-%m-%d"), "timezone": "UTC", "mode": "ntp"},
                    "wifi": {"connected": True, "ssid": "PIFKID-2026", "ip": "192.168.4.1", "rssi": -45},
                    "display": {"brightness": 80, "autoDim": False, "theme": "dark", "timeout": 30, "animation": True},
                    "sound": {"alarmVolume": 50, "alarmSound": "default"},
                    "todos": TODOS, "alarms": ALARMS, "schedule": SCHEDULE,
                    "firmware": {"version": "1.0.0", "build": "Jul 28 2026", "api": "v1"},
                    "weather": {"temp": 32, "feelsLike": 35, "humidity": 75, "condition": "partly_cloudy", "description": "Partly cloudy", "wind": 12, "location": "Hanoi", "country": "VN", "lastUpdate": 0}
                }, "timestamp": 1234567890
            })

        elif self.path == '/api/todo':
            self._json_response({"success": True, "code": 200, "message": "OK", "data": TODOS, "timestamp": 123})

        elif self.path == '/api/alarm':
            self._json_response({"success": True, "code": 200, "message": "OK", "data": ALARMS, "timestamp": 123})

        elif self.path == '/api/schedule':
            self._json_response({"success": True, "code": 200, "message": "OK", "data": SCHEDULE, "timestamp": 123})

        elif self.path == '/api/time':
            now = datetime.now()
            self._json_response({"success": True, "code": 200, "message": "OK",
                "data": {"current": now.strftime("%H:%M"), "date": now.strftime("%Y-%m-%d"), "timezone": "UTC", "mode": "ntp", "hour": now.hour, "minute": now.minute, "second": now.second}, "timestamp": 123})

        elif self.path == '/api/display':
            self._json_response({"success": True, "code": 200, "message": "OK",
                "data": {"brightness": 80, "autoDim": False, "theme": "dark", "timeout": 30, "animation": True}, "timestamp": 123})

        elif self.path == '/api/sound':
            self._json_response({"success": True, "code": 200, "message": "OK",
                "data": {"alarmVolume": 50, "alarmSound": "default"}, "timestamp": 123})

        elif self.path == '/api/wifi':
            self._json_response({"success": True, "code": 200, "message": "OK",
                "data": {"connected": True, "ssid": "PIFKID-2026", "ip": "192.168.4.1", "rssi": -45, "state": "AP_MODE"}, "timestamp": 123})

        elif self.path == '/api/wifi?scan=1':
            self._json_response({"success": True, "code": 200, "message": "OK",
                "data": {"networks": [
                    {"ssid": "PIFKID-2026", "rssi": -30, "channel": 1, "secure": True},
                    {"ssid": "HomeWiFi", "rssi": -55, "channel": 6, "secure": True},
                    {"ssid": "Office-5G", "rssi": -70, "channel": 11, "secure": True},
                ]}, "timestamp": 123})

        elif self.path == '/api/device':
            self._json_response({"success": True, "code": 200, "message": "OK",
                "data": {"firmware": "1.0.0", "chip": "ESP32", "flash": "4MB", "heap": "280KB", "mac": "AA:BB:CC:DD:EE:FF", "battery": 85}, "timestamp": 123})

        elif self.path == '/api/weather':
            import time
            self._json_response({"success": True, "code": 200, "message": "OK",
                "data": {"temp": 32, "feelsLike": 35, "humidity": 75, "condition": "partly_cloudy", "description": "Partly cloudy", "wind": 12, "location": "Hanoi", "country": "VN", "lastUpdate": int(time.time())}, "timestamp": 123})

        else:
            self._json_response({"success": False, "code": 404, "message": "Not found"}, 404)

    def handle_api_post(self):
        global TODOS, ALARMS, SCHEDULE, next_todo_id, next_alarm_id, next_schedule_id
        body = self._read_body()

        if self.path == '/api/todo':
            todo = {"id": next_todo_id, "title": body.get("title", ""), "description": body.get("description", ""),
                    "color": body.get("color", "#4fc3f7"), "completed": False, "created_at": 9999}
            next_todo_id += 1
            TODOS.append(todo)
            self._save()
            self._json_response({"success": True, "code": 201, "message": "Created", "data": todo}, 201)

        elif self.path == '/api/alarm':
            alarm = {"id": next_alarm_id, "hour": body.get("hour", 0), "minute": body.get("minute", 0),
                     "enabled": body.get("enabled", True), "sound": body.get("sound", "default"),
                     "volume": body.get("volume", 50), "repeat": body.get("repeat", [])}
            next_alarm_id += 1
            ALARMS.append(alarm)
            self._save()
            self._json_response({"success": True, "code": 201, "message": "Created", "data": alarm}, 201)

        elif self.path == '/api/schedule':
            entry = {"id": next_schedule_id, "day": body.get("day", 0), "start": body.get("start", "00:00"),
                     "end": body.get("end", "00:00"), "title": body.get("title", ""), "color": body.get("color", "#4fc3f7")}
            next_schedule_id += 1
            SCHEDULE.append(entry)
            self._save()
            self._json_response({"success": True, "code": 201, "message": "Created", "data": entry}, 201)

        elif self.path == '/api/time':
            self._json_response({"success": True, "code": 200, "message": "Time updated"})

        elif self.path == '/api/display':
            self._json_response({"success": True, "code": 200, "message": "Display updated", "data": body})

        elif self.path == '/api/sound':
            self._json_response({"success": True, "code": 200, "message": "Sound updated", "data": body})

        elif self.path == '/api/wifi':
            self._json_response({"success": True, "code": 200, "message": "WiFi connecting"})

        elif self.path == '/api/weather':
            import time, random
            self._json_response({"success": True, "code": 200, "message": "Weather refreshed",
                "data": {"temp": random.randint(25, 38), "feelsLike": random.randint(27, 40), "humidity": random.randint(40, 90), "condition": "partly_cloudy", "description": "Partly cloudy", "wind": random.randint(5, 20), "location": "Hanoi", "country": "VN", "lastUpdate": int(time.time())}})

        else:
            self._json_response({"success": False, "code": 404, "message": "Not found"}, 404)

    def handle_api_put(self):
        global TODOS, ALARMS

        for todo in TODOS:
            if self.path == f'/api/todo/{todo["id"]}':
                body = self._read_body()
                for k in ("title", "description", "color"):
                    if k in body: todo[k] = body[k]
                if "completed" in body: todo["completed"] = body["completed"]
                self._save()
                self._json_response({"success": True, "code": 200, "message": "Updated", "data": todo})
                return

        for alarm in ALARMS:
            if self.path == f'/api/alarm/{alarm["id"]}':
                body = self._read_body()
                for k in ("hour", "minute", "sound", "volume", "repeat", "enabled"):
                    if k in body: alarm[k] = body[k]
                self._save()
                self._json_response({"success": True, "code": 200, "message": "Updated", "data": alarm})
                return

        self._json_response({"success": False, "code": 404, "message": "Not found"}, 404)

    def handle_api_delete(self):
        global TODOS, ALARMS, SCHEDULE

        for i, todo in enumerate(TODOS):
            if self.path == f'/api/todo/{todo["id"]}':
                TODOS.pop(i)
                self._save()
                self._json_response({"success": True, "code": 200, "message": "Deleted"})
                return

        for i, alarm in enumerate(ALARMS):
            if self.path == f'/api/alarm/{alarm["id"]}':
                ALARMS.pop(i)
                self._save()
                self._json_response({"success": True, "code": 200, "message": "Deleted"})
                return

        for i, entry in enumerate(SCHEDULE):
            if self.path == f'/api/schedule/{entry["id"]}':
                SCHEDULE.pop(i)
                self._save()
                self._json_response({"success": True, "code": 200, "message": "Deleted"})
                return

        self._json_response({"success": False, "code": 404, "message": "Not found"}, 404)

    def _save(self):
        save_db({"todos": TODOS, "alarms": ALARMS, "schedule": SCHEDULE,
                 "next_ids": {"todo": next_todo_id, "alarm": next_alarm_id, "schedule": next_schedule_id}})


if __name__ == '__main__':
    PORT = 8080
    print(f"""
=====================================
  PIFKID 2026 - Web UI Simulator
=====================================

  Open browser:  http://localhost:{PORT}
  Press Ctrl+C to stop

  Data file: {DB_FILE}
  (data persists across restarts)
""")
    server = HTTPServer(('localhost', PORT), MockHandler)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nStopped.")
        server.server_close()
