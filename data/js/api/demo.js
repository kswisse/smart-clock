import { CONFIG } from '../core/config.js';

const STORAGE_KEY = 'pifkid_demo_data';

function getDefaultData() {
  return {
    todos: [
      { id: 1, title: "Mua sữa", description: "Sữa tươi 1 lít", color: "#4fc3f7", completed: false, created_at: 1000 },
      { id: 2, title: "Viết báo cáo", description: "Báo cáo tuần", color: "#66bb6a", completed: true, created_at: 2000 }
    ],
    alarms: [
      { id: 1, hour: 7, minute: 30, enabled: true, sound: "default", volume: 50, repeat: [1,2,3,4,5] },
      { id: 2, hour: 10, minute: 0, enabled: true, sound: "chime", volume: 70, repeat: [0,6] }
    ],
    schedule: [
      { id: 1, day: 1, start: "08:00", end: "08:30", title: "Báo thức", color: "#ef5350" },
      { id: 2, day: 1, start: "10:00", end: "10:30", title: "Ăn cơm", color: "#66bb6a" },
      { id: 3, day: 1, start: "11:00", end: "12:00", title: "Làm việc", color: "#4fc3f7" },
      { id: 4, day: 2, start: "09:00", end: "09:30", title: "Team standup", color: "#ffa726" }
    ],
    nextIds: { todo: 3, alarm: 3, schedule: 5 },
    display: { brightness: 80, autoDim: false, theme: "dark", timeout: 30, animation: true },
    sound: { alarmVolume: 50, alarmSound: "default" },
    wifi: { connected: true, ssid: "PIFKID-2026", ip: "192.168.4.1", rssi: -45, state: "AP_MODE" },
    device: { firmware: "1.0.0", chip: "ESP32", flash: "4MB", heap: "280KB", mac: "AA:BB:CC:DD:EE:FF", battery: 85 },
    weather: { temp: 32, feelsLike: 35, humidity: 75, condition: "partly_cloudy", description: "Partly cloudy", wind: 12, location: "Hanoi", country: "VN", lastUpdate: 0 }
  };
}

function loadData() {
  try {
    const stored = localStorage.getItem(STORAGE_KEY);
    if (stored) return JSON.parse(stored);
  } catch (e) {}
  return getDefaultData();
}

function saveData(data) {
  try {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(data));
  } catch (e) {}
}

function makeResponse(data, code = 200) {
  return {
    success: true,
    code,
    message: code === 201 ? "Created" : "OK",
    data,
    timestamp: Math.floor(Date.now() / 1000)
  };
}

function makeError(message, code = 404) {
  return { success: false, code, message };
}

function handleGet(endpoint) {
  const data = loadData();
  const now = new Date();

  switch (endpoint) {
    case '/api/status':
      return makeResponse({
        device: data.device,
        time: {
          current: now.toTimeString().slice(0, 5),
          date: now.toISOString().slice(0, 10),
          timezone: "UTC",
          mode: "ntp"
        },
        wifi: data.wifi,
        display: data.display,
        sound: data.sound,
        todos: data.todos,
        alarms: data.alarms,
        schedule: data.schedule,
        firmware: { version: "1.0.0", build: "Jul 28 2026", api: "v1" },
        weather: data.weather
      });

    case '/api/time':
      return makeResponse({
        current: now.toTimeString().slice(0, 5),
        date: now.toISOString().slice(0, 10),
        timezone: "UTC",
        mode: "ntp",
        hour: now.getHours(),
        minute: now.getMinutes(),
        second: now.getSeconds()
      });

    case '/api/todo':
      return makeResponse(data.todos);

    case '/api/alarm':
      return makeResponse(data.alarms);

    case '/api/schedule':
      return makeResponse(data.schedule);

    case '/api/display':
      return makeResponse(data.display);

    case '/api/sound':
      return makeResponse(data.sound);

    case '/api/wifi':
      return makeResponse(data.wifi);

    case '/api/device':
      return makeResponse(data.device);

    case '/api/weather':
      data.weather.lastUpdate = Math.floor(Date.now() / 1000);
      saveData(data);
      return makeResponse(data.weather);

    default:
      if (endpoint.startsWith('/api/todo/')) {
        const id = parseInt(endpoint.split('/').pop());
        const todo = data.todos.find(t => t.id === id);
        return todo ? makeResponse(todo) : makeError("Not found");
      }
      if (endpoint.startsWith('/api/alarm/')) {
        const id = parseInt(endpoint.split('/').pop());
        const alarm = data.alarms.find(a => a.id === id);
        return alarm ? makeResponse(alarm) : makeError("Not found");
      }
      if (endpoint.startsWith('/api/schedule/')) {
        const id = parseInt(endpoint.split('/').pop());
        const entry = data.schedule.find(s => s.id === id);
        return entry ? makeResponse(entry) : makeError("Not found");
      }
      return makeError("Not found", 404);
  }
}

function handlePost(endpoint, body) {
  const data = loadData();

  switch (endpoint) {
    case '/api/todo': {
      const todo = {
        id: data.nextIds.todo++,
        title: body.title || '',
        description: body.description || '',
        color: body.color || '#4fc3f7',
        completed: false,
        created_at: Date.now()
      };
      data.todos.push(todo);
      saveData(data);
      return makeResponse(todo, 201);
    }

    case '/api/alarm': {
      const alarm = {
        id: data.nextIds.alarm++,
        hour: body.hour || 0,
        minute: body.minute || 0,
        enabled: body.enabled !== undefined ? body.enabled : true,
        sound: body.sound || 'default',
        volume: body.volume || 50,
        repeat: body.repeat || []
      };
      data.alarms.push(alarm);
      saveData(data);
      return makeResponse(alarm, 201);
    }

    case '/api/schedule': {
      const entry = {
        id: data.nextIds.schedule++,
        day: body.day || 0,
        start: body.start || '00:00',
        end: body.end || '00:00',
        title: body.title || '',
        color: body.color || '#4fc3f7'
      };
      data.schedule.push(entry);
      saveData(data);
      return makeResponse(entry, 201);
    }

    case '/api/time':
      return makeResponse({ message: "Time updated" });

    case '/api/sync':
      if (body.todos) data.todos = body.todos;
      if (body.alarms) data.alarms = body.alarms;
      saveData(data);
      return makeResponse({ todos_synced: (body.todos || []).length, alarms_synced: (body.alarms || []).length });

    case '/api/display':
      for (const key of ['brightness', 'autoDim', 'theme', 'timeout', 'animation']) {
        if (body[key] !== undefined) data.display[key] = body[key];
      }
      saveData(data);
      return makeResponse(data.display);

    case '/api/sound':
      for (const key of ['alarmVolume', 'alarmSound']) {
        if (body[key] !== undefined) data.sound[key] = body[key];
      }
      saveData(data);
      return makeResponse(data.sound);

    case '/api/wifi':
      return makeResponse({ message: "WiFi connecting" });

    case '/api/weather':
      data.weather.temp = Math.floor(Math.random() * 14) + 25;
      data.weather.feelsLike = data.weather.temp + Math.floor(Math.random() * 5);
      data.weather.humidity = Math.floor(Math.random() * 51) + 40;
      data.weather.lastUpdate = Math.floor(Date.now() / 1000);
      saveData(data);
      return makeResponse(data.weather);

    default:
      return makeError("Not found", 404);
  }
}

function handlePut(endpoint, body) {
  const data = loadData();

  if (endpoint.startsWith('/api/todo/')) {
    const id = parseInt(endpoint.split('/').pop());
    const todo = data.todos.find(t => t.id === id);
    if (!todo) return makeError("Not found");
    for (const key of ['title', 'description', 'color', 'completed']) {
      if (body[key] !== undefined) todo[key] = body[key];
    }
    saveData(data);
    return makeResponse(todo);
  }

  if (endpoint.startsWith('/api/alarm/')) {
    const id = parseInt(endpoint.split('/').pop());
    const alarm = data.alarms.find(a => a.id === id);
    if (!alarm) return makeError("Not found");
    for (const key of ['hour', 'minute', 'sound', 'volume', 'repeat', 'enabled']) {
      if (body[key] !== undefined) alarm[key] = body[key];
    }
    saveData(data);
    return makeResponse(alarm);
  }

  if (endpoint.startsWith('/api/schedule/')) {
    const id = parseInt(endpoint.split('/').pop());
    const entry = data.schedule.find(s => s.id === id);
    if (!entry) return makeError("Not found");
    for (const key of ['day', 'start', 'end', 'title', 'color']) {
      if (body[key] !== undefined) entry[key] = body[key];
    }
    saveData(data);
    return makeResponse(entry);
  }

  return makeError("Not found", 404);
}

function handleDelete(endpoint) {
  const data = loadData();

  if (endpoint.startsWith('/api/todo/')) {
    const id = parseInt(endpoint.split('/').pop());
    const index = data.todos.findIndex(t => t.id === id);
    if (index === -1) return makeError("Not found");
    data.todos.splice(index, 1);
    saveData(data);
    return makeResponse({ message: "Deleted" });
  }

  if (endpoint.startsWith('/api/alarm/')) {
    const id = parseInt(endpoint.split('/').pop());
    const index = data.alarms.findIndex(a => a.id === id);
    if (index === -1) return makeError("Not found");
    data.alarms.splice(index, 1);
    saveData(data);
    return makeResponse({ message: "Deleted" });
  }

  if (endpoint.startsWith('/api/schedule/')) {
    const id = parseInt(endpoint.split('/').pop());
    const index = data.schedule.findIndex(s => s.id === id);
    if (index === -1) return makeError("Not found");
    data.schedule.splice(index, 1);
    saveData(data);
    return makeResponse({ message: "Deleted" });
  }

  return makeError("Not found", 404);
}

export const demoApi = {
  async request(method, endpoint, body = null) {
    await new Promise(r => setTimeout(r, 50));

    switch (method) {
      case 'GET': return handleGet(endpoint);
      case 'POST': return handlePost(endpoint, body || {});
      case 'PUT': return handlePut(endpoint, body || {});
      case 'DELETE': return handleDelete(endpoint);
      default: return makeError("Method not allowed", 405);
    }
  },

  isDemoMode() {
    return CONFIG.MODE === 'demo';
  }
};
