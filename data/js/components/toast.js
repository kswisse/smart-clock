import { el, $ } from '../utils/dom.js';

let toastCounter = 0;

export function showToast(message, type = 'info', duration = 3000) {
  const container = $('#toast-container');
  if (!container) return;

  const id = `toast-${++toastCounter}`;
  const icon = type === 'success' ? '&#10003;' : type === 'error' ? '&#10007;' : '&#9432;';
  const toast = el('div', { class: `toast toast-${type}`, dataset: { id } }, [
    el('span', { class: 'toast-icon', html: icon }),
    el('span', { class: 'toast-msg', text: message })
  ]);

  container.appendChild(toast);
  requestAnimationFrame(() => toast.classList.add('toast-show'));

  setTimeout(() => {
    toast.classList.remove('toast-show');
    toast.addEventListener('transitionend', () => toast.remove(), { once: true });
  }, duration);
}
