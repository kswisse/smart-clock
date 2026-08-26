function detectMode() {
  const h = location.hostname;
  if (h === '192.168.4.1' || h === 'clock.local' || h === 'localhost') {
    return h === 'localhost' ? 'demo' : 'esp32';
  }
  return 'demo';
}

export const CONFIG = {
  MODE: detectMode(),
  API_BASE: '',
  API_TIMEOUT: 5000,
  API_RETRY: 1,
  CACHE_TTL: {
    STATUS: 10000,
    TODOS: 30000,
    ALARMS: 30000,
    SCHEDULE: 30000,
    DISPLAY: 30000,
    SOUND: 30000,
    DEVICE: 60000,
    WIFI: 0,
    WEATHER: 300000
  },
  PAGES: {
    DASHBOARD: 'dashboard',
    TODO: 'todo',
    ALARM: 'alarm',
    SCHEDULE: 'schedule',
    CLOCK_SETTINGS: 'clock-settings',
    DISPLAY_SETTINGS: 'display-settings',
    SOUND_SETTINGS: 'sound-settings',
    WIFI: 'wifi',
    DEVICE: 'device'
  },
  ROUTES: {
    dashboard: { title: 'Dashboard', page: 'dashboard' },
    todo: { title: 'Todos', page: 'todo' },
    alarm: { title: 'Alarms', page: 'alarm' },
    schedule: { title: 'Schedule', page: 'schedule' },
    'clock-settings': { title: 'Clock', page: 'clock-settings' },
    'display-settings': { title: 'Display', page: 'display-settings' },
    'sound-settings': { title: 'Sound', page: 'sound-settings' },
    wifi: { title: 'WiFi', page: 'wifi' },
    device: { title: 'Device', page: 'device' }
  },
  ENDPOINTS: {
    STATUS: '/api/status',
    TIME: '/api/time',
    TODO: '/api/todo',
    ALARM: '/api/alarm',
    SCHEDULE: '/api/schedule',
    DISPLAY: '/api/display',
    SOUND: '/api/sound',
    WIFI: '/api/wifi',
    DEVICE: '/api/device',
    SYNC: '/api/sync',
    WEATHER: '/api/weather'
  },
  COLORS: [
    '#4fc3f7', '#66bb6a', '#ffa726', '#ef5350',
    '#ab47bc', '#26c6da', '#ffca28', '#ec407a'
  ],
  DAYS: ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'],
  DAYS_FULL: ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'],
  ALARM_SOUNDS: ['default', 'beep', 'chime', 'gentle', 'urgent'],
  DEFAULT_TODO: { title: '', description: '', color: '#4fc3f7' },
  DEFAULT_ALARM: { hour: 7, minute: 0, repeat: [], sound: 'default', volume: 50, enabled: true },
  DEFAULT_SCHEDULE: { day: 1, start: '09:00', end: '10:00', title: '', color: '#4fc3f7' },
  DEFAULT_DISPLAY: { brightness: 80, autoDim: false, theme: 'dark', timeout: 30, animation: true },
  DEFAULT_SOUND: { alarmVolume: 50, alarmSound: 'default' }
};
