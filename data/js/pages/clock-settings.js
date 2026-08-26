import { el, clearChildren } from '../utils/dom.js';
import { store } from '../core/store.js';
import { timeApi } from '../api/time.js';
import { api } from '../api/client.js';
import { showToast } from '../components/toast.js';
import { createInput } from '../components/input.js';
import { createSelect } from '../components/select.js';
import { createToggle } from '../components/toggle.js';

let container = null;
let unsubscribers = [];

const TIMEZONES = [
  { value: 'UTC', label: 'UTC (GMT+0)' },
  { value: 'Europe/London', label: 'London (GMT+0/+1)' },
  { value: 'Europe/Paris', label: 'Paris (GMT+1/+2)' },
  { value: 'Europe/Berlin', label: 'Berlin (GMT+1/+2)' },
  { value: 'America/New_York', label: 'New York (GMT-5/-4)' },
  { value: 'America/Chicago', label: 'Chicago (GMT-6/-5)' },
  { value: 'America/Denver', label: 'Denver (GMT-7/-6)' },
  { value: 'America/Los_Angeles', label: 'Los Angeles (GMT-8/-7)' },
  { value: 'Asia/Tokyo', label: 'Tokyo (GMT+9)' },
  { value: 'Asia/Shanghai', label: 'Shanghai (GMT+8)' },
  { value: 'Asia/Kolkata', label: 'Mumbai (GMT+5:30)' },
  { value: 'Australia/Sydney', label: 'Sydney (GMT+10/+11)' }
];

function createSyncBtn() {
  const btn = el('button', {
    class: 'btn btn-primary btn-block mt-sm',
    text: 'Đồng bộ dữ liệu',
    onClick: async () => {
      btn.disabled = true;
      btn.textContent = 'Đang đồng bộ...';

      try {
        const now = new Date();
        const timeData = {
          year: now.getFullYear(),
          month: now.getMonth() + 1,
          day: now.getDate(),
          hour: now.getHours(),
          minute: now.getMinutes(),
          second: now.getSeconds(),
          timezone: 'Asia/Ho_Chi_Minh'
        };

        const todos = (store.get('todos') || []).map(t => ({
          id: t.id,
          title: t.title,
          description: t.description,
          color: t.color,
          completed: t.completed,
          created_at: t.created_at || t.createdAt || 0
        }));

        const alarms = (store.get('alarms') || []).map(a => ({
          id: a.id,
          hour: a.hour,
          minute: a.minute,
          repeat: a.repeat || [],
          enabled: a.enabled,
          sound: a.sound,
          volume: a.volume
        }));

        await api.sync({ time: timeData, todos, alarms });
        showToast('Đồng bộ thành công', 'success');
      } catch (e) {
        showToast('Đồng bộ thất bại: ' + (e.message || 'Unknown error'), 'error');
      } finally {
        btn.disabled = false;
        btn.textContent = 'Đồng bộ dữ liệu';
      }
    }
  });
  return btn;
}

function render() {
  if (!container) return;
  clearChildren(container);

  const time = store.get('time');
  const isManual = time.mode === 'manual';

  let dateVal = time.date || new Date().toISOString().split('T')[0];
  let timeVal = time.current || '12:00';
  let tzVal = time.timezone || 'UTC';

  const dateInput = createInput({ label: 'Date', type: 'date', value: dateVal, onInput: v => dateVal = v });
  const timeInput = createInput({ label: 'Time (HH:MM)', type: 'time', value: timeVal, onInput: v => timeVal = v });
  const tzSelect = createSelect({ label: 'Timezone', options: TIMEZONES, value: tzVal, onChange: v => tzVal = v });

  const modeToggle = createToggle({
    label: 'Manual Mode (NTP OFF)',
    checked: isManual,
    onChange: async (val) => {
      try {
        await timeApi.set({ mode: val ? 'manual' : 'ntp' });
        showToast(val ? 'Manual mode' : 'NTP mode', 'success');
      } catch (e) {
        showToast('Failed to change mode', 'error');
      }
    }
  });

  const setBtn = el('button', {
    class: 'btn btn-primary btn-block mt-md',
    text: 'Set Time & Date',
    onClick: async () => {
      try {
        await timeApi.set({ time: timeVal, date: dateVal, timezone: tzVal, mode: 'manual' });
        showToast('Time updated', 'success');
      } catch (e) {
        showToast('Failed to set time', 'error');
      }
    }
  });

  const syncBtn = el('button', {
    class: 'btn btn-ghost btn-block mt-sm',
    text: 'Sync from NTP',
    onClick: async () => {
      try {
        await timeApi.set({ mode: 'ntp' });
        showToast('Syncing time...', 'success');
      } catch (e) {
        showToast('NTP sync failed', 'error');
      }
    }
  });

  container.appendChild(el('div', { class: 'page-content' }, [
    el('div', { class: 'page-header' }, [
      el('h1', { class: 'page-title', text: 'Clock Settings' })
    ]),
    el('div', { class: 'card' }, [
      el('div', { class: 'settings-group' }, [
        el('div', { class: 'settings-group-title', text: 'Time Mode' }),
        modeToggle
      ]),
      el('div', { class: 'settings-group' }, [
        el('div', { class: 'settings-group-title', text: 'Set Time' }),
        dateInput,
        timeInput,
        tzSelect,
        setBtn,
        syncBtn
      ])
    ]),
    el('div', { class: 'card mt-md' }, [
      el('div', { class: 'settings-group' }, [
        el('div', { class: 'settings-group-title', text: 'Đồng bộ dữ liệu' }),
        el('p', { class: 'settings-hint', text: 'Push all data (Time + Alarms + Todos) from this device to the clock.' }),
        createSyncBtn()
      ])
    ])
  ]));
}

async function load() {
  try { await timeApi.get(); } catch (e) { showToast('Failed to load time', 'error'); }
}

function init(c) {
  container = c;
  unsubscribers.push(store.subscribe('time', () => render()));
}

function destroy() {
  unsubscribers.forEach(unsub => unsub());
  unsubscribers = [];
  container = null;
}

export const clockSettingsPage = { init, load, render, destroy };
