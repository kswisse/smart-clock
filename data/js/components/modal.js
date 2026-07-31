import { el } from '../utils/dom.js';

export function showModal({ title, content, onSave, onCancel }) {
  const overlay = el('div', { class: 'modal-overlay' });

  const saveBtn = el('button', { class: 'btn btn-primary', text: 'Save' });
  const cancelBtn = el('button', { class: 'btn btn-ghost', text: 'Cancel' });

  const modal = el('div', { class: 'modal' }, [
    el('div', { class: 'modal-header' }, [
      el('h3', { class: 'modal-title', text: title }),
      el('button', { class: 'modal-close', html: '&times;', onClick: close })
    ]),
    el('div', { class: 'modal-body' }, [content]),
    el('div', { class: 'dialog-actions', style: { marginTop: '16px' } }, [cancelBtn, saveBtn])
  ]);

  overlay.appendChild(modal);

  let closed = false;

  function close() {
    if (closed) return;
    closed = true;
    overlay.remove();
    document.removeEventListener('keydown', onKeydown);
  }

  function cancel() {
    close();
    onCancel?.();
  }

  function save() {
    close();
    onSave?.();
  }

  function onKeydown(e) {
    if (e.key === 'Escape') cancel();
  }

  saveBtn.addEventListener('click', save);
  cancelBtn.addEventListener('click', cancel);

  overlay.addEventListener('click', (e) => {
    if (e.target === overlay) cancel();
  });

  document.addEventListener('keydown', onKeydown);
  document.body.appendChild(overlay);
  requestAnimationFrame(() => overlay.classList.add('dialog-show'));

  return { close: cancel, el: modal };
}
