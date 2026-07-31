let _idCounter = 0;
export function uid() {
  return Date.now().toString(36) + (++_idCounter).toString(36);
}

export function debounce(fn, ms = 300) {
  let timer;
  return (...args) => {
    clearTimeout(timer);
    timer = setTimeout(() => fn(...args), ms);
  };
}

export function throttle(fn, ms = 100) {
  let last = 0;
  return (...args) => {
    const now = Date.now();
    if (now - last >= ms) {
      last = now;
      fn(...args);
    }
  };
}

export function formatDate(d) {
  const date = d ? new Date(d) : new Date();
  return date.toLocaleDateString('en-US', { year: 'numeric', month: 'short', day: 'numeric' });
}

export function formatTime(h, m) {
  return `${String(h).padStart(2, '0')}:${String(m).padStart(2, '0')}`;
}

export function formatTime12(h, m) {
  const period = h >= 12 ? 'PM' : 'AM';
  const hour12 = h % 12 || 12;
  return `${hour12}:${String(m).padStart(2, '0')} ${period}`;
}

export function parseTime(str) {
  const [h, m] = str.split(':').map(Number);
  return { hour: h, minute: m };
}

export function padZero(n) {
  return String(n).padStart(2, '0');
}

export function clamp(val, min, max) {
  return Math.min(Math.max(val, min), max);
}

export function deepClone(obj) {
  return JSON.parse(JSON.stringify(obj));
}

export function escapeHtml(str) {
  const div = document.createElement('div');
  div.textContent = str;
  return div.innerHTML;
}

export function sleep(ms) {
  return new Promise(r => setTimeout(r, ms));
}
