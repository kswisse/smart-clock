import { el } from '../utils/dom.js';

export function createCard(title, content, actions = []) {
  const children = [];
  if (title) {
    const header = el('div', { class: 'card-header' }, [
      el('h3', { class: 'card-title', text: title })
    ]);
    if (actions.length) {
      header.appendChild(el('div', { class: 'card-actions' }, actions));
    }
    children.push(header);
  }
  if (content) {
    children.push(el('div', { class: 'card-body' }, Array.isArray(content) ? content : [content]));
  }
  return el('div', { class: 'card' }, children);
}

export function createInfoCard(items) {
  const rows = items.map(item =>
    el('div', { class: 'info-row' }, [
      el('span', { class: 'info-label', text: item.label }),
      el('span', { class: 'info-value', text: String(item.value) })
    ])
  );
  return el('div', { class: 'card card-info' }, rows);
}
