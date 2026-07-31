import { el } from '../utils/dom.js';

export function createList(items = [], renderItem) {
  const list = el('div', { class: 'list' });
  if (items.length === 0) {
    list.appendChild(el('div', { class: 'list-empty', text: 'No items' }));
    return list;
  }
  for (const item of items) {
    list.appendChild(renderItem(item));
  }
  return list;
}

export function createListItem({ title, subtitle, left, right, onClick }) {
  const children = [];
  if (left) children.push(el('div', { class: 'list-item-left' }, [left]));
  const main = el('div', { class: 'list-item-main' }, [
    el('div', { class: 'list-item-title', text: title }),
    subtitle ? el('div', { class: 'list-item-subtitle', text: subtitle }) : null
  ].filter(Boolean));
  children.push(main);
  if (right) children.push(el('div', { class: 'list-item-right' }, [right]));
  return el('div', { class: 'list-item', onClick }, children);
}
