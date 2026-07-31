import { el } from '../utils/dom.js';

export function showDialog({ title, message, confirmText = 'OK', cancelText = 'Cancel', onConfirm, onCancel }) {
  const overlay = el('div', { class: 'dialog-overlay' });
  const dialog = el('div', { class: 'dialog' }, [
    el('h3', { class: 'dialog-title', text: title }),
    el('p', { class: 'dialog-msg', text: message }),
    el('div', { class: 'dialog-actions' }, [
      el('button', { class: 'btn btn-ghost', text: cancelText, onClick: () => { close(); onCancel?.(); } }),
      el('button', { class: 'btn btn-primary', text: confirmText, onClick: () => { close(); onConfirm?.(); } })
    ])
  ]);
  overlay.appendChild(dialog);

  function close() {
    overlay.classList.add('dialog-fade-out');
    overlay.addEventListener('transitionend', () => overlay.remove(), { once: true });
  }

  overlay.addEventListener('click', (e) => {
    if (e.target === overlay) { close(); onCancel?.(); }
  });

  document.body.appendChild(overlay);
  requestAnimationFrame(() => overlay.classList.add('dialog-show'));
  return { close };
}

export function confirmDialog(title, message) {
  return new Promise(resolve => {
    showDialog({
      title,
      message,
      confirmText: 'Confirm',
      cancelText: 'Cancel',
      onConfirm: () => resolve(true),
      onCancel: () => resolve(false)
    });
  });
}
