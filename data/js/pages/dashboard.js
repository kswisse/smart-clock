import { el, clearChildren } from '../utils/dom.js';
import { store } from '../core/store.js';
import { statusApi } from '../api/status.js';
import { timeApi } from '../api/time.js';
import { weatherApi } from '../api/weather.js';
import { showToast } from '../components/toast.js';
import { router } from '../core/router.js';

let container = null;
let unsubscribers = [];

const GUIDE_KEY = 'pifkid_dashboard_guide_hidden';

function getGreeting() {
  const hour = new Date().getHours();
  if (hour < 12) return 'Good Morning';
  if (hour < 18) return 'Good Afternoon';
  return 'Good Evening';
}

function getGreetingIcon() {
  const hour = new Date().getHours();
  if (hour < 12) return '🌅';
  if (hour < 18) return '☀️';
  return '🌙';
}

function getConnectionStatus() {
  const wifi = store.get('wifi');
  return wifi.connected ? { text: 'Online', color: 'var(--success)', icon: '📶' } : { text: 'Offline', color: 'var(--danger)', icon: '📵' };
}

function getDeviceStatus() {
  const device = store.get('device');
  if (!device || !device.firmware) return { text: 'Unknown', color: 'var(--text-muted)' };
  return { text: `v${device.firmware}`, color: 'var(--accent)' };
}

function getWeatherInfo() {
  const weather = store.get('weather');
  if (!weather || !weather.condition) return null;
  return weather;
}

function renderHeroSection() {
  const time = store.get('time');
  const connection = getConnectionStatus();
  const device = getDeviceStatus();
  const weather = getWeatherInfo();

  const weatherInfo = weather
    ? el('div', { class: 'hero-weather' }, [
        el('span', { class: 'hero-weather-icon', text: getWeatherIcon(weather.condition) }),
        el('span', { class: 'hero-weather-temp', text: `${weather.temp}°C` }),
        el('span', { class: 'hero-weather-desc', text: weather.description })
      ])
    : null;

  return el('div', { class: 'hero-section' }, [
    el('div', { class: 'hero-greeting' }, [
      el('span', { class: 'hero-greeting-icon', text: getGreetingIcon() }),
      el('h1', { class: 'hero-greeting-text', text: getGreeting() })
    ]),
    el('div', { class: 'hero-time', text: time.current || '--:--' }),
    el('div', { class: 'hero-date', text: time.date || '---' }),
    weatherInfo,
    el('div', { class: 'hero-status' }, [
      el('span', { class: 'hero-status-item', style: { color: connection.color } }, [
        el('span', { text: connection.icon }),
        el('span', { text: connection.text })
      ]),
      el('span', { class: 'hero-status-item', style: { color: device.color } }, [
        el('span', { text: '🔧' }),
        el('span', { text: device.text })
      ])
    ])
  ]);
}

function renderSummarySection() {
  const alarms = store.get('alarms') || [];
  const todos = store.get('todos') || [];
  const schedule = store.get('schedule') || [];
  const wifi = store.get('wifi');

  const today = new Date().getDay();
  const todaySchedule = schedule.filter(s => s.day === today);
  const activeAlarms = alarms.filter(a => a.enabled);
  const pendingTodos = todos.filter(t => !t.completed);

  return el('div', { class: 'summary-section' }, [
    el('h2', { class: 'section-title', text: "Today's Summary" }),
    el('div', { class: 'summary-grid' }, [
      createSummaryCard('⏰', 'Alarms', `${activeAlarms.length} active`, 'var(--accent)', () => router.navigateTo('alarm')),
      createSummaryCard('📋', 'Todos', `${pendingTodos.length} pending`, 'var(--warning)', () => router.navigateTo('todo')),
      createSummaryCard('📅', 'Schedule', `${todaySchedule.length} events`, 'var(--success)', () => router.navigateTo('schedule')),
      createSummaryCard('📶', 'WiFi', wifi.connected ? wifi.ssid : 'Disconnected', wifi.connected ? 'var(--success)' : 'var(--danger)', () => router.navigateTo('wifi'))
    ])
  ]);
}

function createSummaryCard(icon, title, value, color, onClick) {
  return el('div', { class: 'summary-card', onClick }, [
    el('div', { class: 'summary-card-icon', text: icon }),
    el('div', { class: 'summary-card-content' }, [
      el('div', { class: 'summary-card-title', text: title }),
      el('div', { class: 'summary-card-value', text: value, style: { color } })
    ]),
    el('div', { class: 'summary-card-arrow', text: '→' })
  ]);
}

function renderQuickActions() {
  const actions = [
    { icon: '⏰', label: 'Alarms', page: 'alarm', color: '#ef5350' },
    { icon: '📋', label: 'Todos', page: 'todo', color: '#ffa726' },
    { icon: '📅', label: 'Schedule', page: 'schedule', color: '#66bb6a' },
    { icon: '🕐', label: 'Clock', page: 'clock-settings', color: '#4fc3f7' },
    { icon: '🖼️', label: 'Display', page: 'display-settings', color: '#ab47bc' },
    { icon: '🔊', label: 'Sound', page: 'sound-settings', color: '#26c6da' },
    { icon: '📶', label: 'WiFi', page: 'wifi', color: '#ffca28' },
    { icon: '📱', label: 'Device', page: 'device', color: '#ec407a' }
  ];

  return el('div', { class: 'actions-section' }, [
    el('h2', { class: 'section-title', text: 'Quick Actions' }),
    el('div', { class: 'actions-grid' }, actions.map(action =>
      el('button', {
        class: 'action-card',
        onClick: () => router.navigateTo(action.page)
      }, [
        el('div', { class: 'action-icon', text: action.icon, style: { background: action.color + '20', color: action.color } }),
        el('div', { class: 'action-label', text: action.label })
      ])
    ))
  ]);
}

function renderTimeline() {
  const alarms = store.get('alarms') || [];
  const schedule = store.get('schedule') || [];
  const today = new Date().getDay();

  const todayAlarms = alarms.filter(a => a.enabled && a.repeat?.includes(today));
  const todaySchedule = schedule.filter(s => s.day === today);

  const events = [
    ...todayAlarms.map(a => ({
      time: `${String(a.hour).padStart(2, '0')}:${String(a.minute).padStart(2, '0')}`,
      title: `Alarm - ${a.sound || 'default'}`,
      type: 'alarm',
      color: 'var(--accent)'
    })),
    ...todaySchedule.map(s => ({
      time: s.start,
      title: s.title,
      type: 'schedule',
      color: s.color || 'var(--success)'
    }))
  ].sort((a, b) => a.time.localeCompare(b.time));

  if (events.length === 0) {
    return el('div', { class: 'timeline-section' }, [
      el('h2', { class: 'section-title', text: 'Today Timeline' }),
      el('div', { class: 'timeline-empty' }, [
        el('p', { text: 'No events scheduled for today' })
      ])
    ]);
  }

  return el('div', { class: 'timeline-section' }, [
    el('h2', { class: 'section-title', text: 'Today Timeline' }),
    el('div', { class: 'timeline-list' }, events.map(event =>
      el('div', { class: 'timeline-item' }, [
        el('div', { class: 'timeline-time', text: event.time }),
        el('div', { class: 'timeline-dot', style: { background: event.color } }),
        el('div', { class: 'timeline-content' }, [
          el('div', { class: 'timeline-title', text: event.title }),
          el('div', { class: 'timeline-type', text: event.type })
        ])
      ])
    ))
  ]);
}

function renderSetupGuide() {
  if (localStorage.getItem(GUIDE_KEY)) return null;

  const alarms = store.get('alarms') || [];
  const todos = store.get('todos') || [];
  const wifi = store.get('wifi');
  const device = store.get('device');

  const steps = [
    { label: 'Connect WiFi', done: wifi.connected },
    { label: 'Configure Time', done: !!store.get('time')?.current },
    { label: 'Create Alarm', done: alarms.length > 0 },
    { label: 'Add Todo', done: todos.length > 0 },
    { label: 'Review Device Settings', done: !!device?.firmware }
  ];

  const completedCount = steps.filter(s => s.done).length;
  const allDone = completedCount === steps.length;

  const hideGuide = () => {
    localStorage.setItem(GUIDE_KEY, '1');
    render();
  };

  return el('div', { class: 'guide-section' }, [
    el('div', { class: 'guide-header' }, [
      el('h2', { class: 'section-title', text: 'Setup Guide' }),
      el('span', { class: 'guide-progress', text: `${completedCount}/${steps.length}` })
    ]),
    el('div', { class: 'guide-steps' }, steps.map(step =>
      el('div', { class: `guide-step ${step.done ? 'completed' : ''}` }, [
        el('div', { class: 'guide-step-check', text: step.done ? '✓' : '○' }),
        el('div', { class: 'guide-step-label', text: step.label })
      ])
    )),
    allDone ? el('div', { class: 'guide-complete' }, [
      el('p', { text: '🎉 Setup complete! You\'re ready to go.' }),
      el('button', { class: 'btn btn-ghost btn-sm', text: 'Hide Guide', onClick: hideGuide })
    ]) : el('button', { class: 'btn btn-ghost btn-sm guide-hide', text: 'Hide Guide', onClick: hideGuide })
  ]);
}

function getWeatherIcon(condition) {
  const icons = {
    clear: '☀️', partly_cloudy: '⛅', cloudy: '☁️',
    rain: '🌧️', heavy_rain: '⛈️', thunderstorm: '⛈️',
    snow: '❄️', fog: '🌫️', wind: '💨'
  };
  return icons[condition] || '🌡️';
}

function render() {
  if (!container) return;
  clearChildren(container);

  const page = el('div', { class: 'page-content dashboard-page' }, [
    renderHeroSection(),
    renderSummarySection(),
    renderQuickActions(),
    renderTimeline(),
    renderSetupGuide()
  ].filter(Boolean));

  container.appendChild(page);
}

async function load() {
  try {
    await statusApi.get();
    await weatherApi.get();
  } catch (e) {
    showToast('Failed to load status', 'error');
  }
}

function init(c) {
  container = c;
  unsubscribers.push(
    store.subscribe('time', () => render()),
    store.subscribe('wifi', () => render()),
    store.subscribe('device', () => render()),
    store.subscribe('alarms', () => render()),
    store.subscribe('todos', () => render()),
    store.subscribe('schedule', () => render()),
    store.subscribe('weather', () => render())
  );
}

function destroy() {
  unsubscribers.forEach(unsub => unsub());
  unsubscribers = [];
  container = null;
}

export const dashboardPage = { init, load, render, destroy };
