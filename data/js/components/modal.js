import { el } from '../utils/dom.js';

export function showModal({ title, content, onClose }) {
  const overlay = el('div', { class: 'modal-overlay' });
  const modal = el('div', { class: 'modal' }, [
    el('div', { class: 'modal-header' }, [
      el('h3', { class: 'modal-title', text: title }),
      el('button', { class: 'modal-close', html: '&times;', onClick: close })
    ]),
    el('div', { class: 'modal-body' }, [content])
  ]);
  overlay.appendChild(modal);

  function close() {
    overlay.classList.add('dialog-fade-out');
    overlay.addEventListener('transitionend', () => {
      overlay.remove();
      onClose?.();
    }, { once: true });
  }

  overlay.addEventListener('click', (e) => {
    if (e.target === overlay) close();
  });

  document.body.appendChild(overlay);
  requestAnimationFrame(() => overlay.classList.add('dialog-show'));
  return { close, el: modal };
}
