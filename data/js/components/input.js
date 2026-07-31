import { el } from '../utils/dom.js';

export function createInput({ label, type = 'text', value = '', placeholder = '', id, onInput }) {
  const input = el('input', {
    type,
    class: 'input',
    value,
    placeholder,
    ...(id ? { id } : {}),
    onInput: (e) => onInput?.(e.target.value)
  });
  const group = el('div', { class: 'input-group' });
  if (label) group.appendChild(el('label', { class: 'input-label', text: label, ...(id ? { htmlFor: id } : {}) }));
  group.appendChild(input);
  return group;
}

export function createNumberInput({ label, value = 0, min = 0, max = 100, step = 1, id, onInput }) {
  const input = el('input', {
    type: 'number',
    class: 'input input-number',
    value: String(value),
    min: String(min),
    max: String(max),
    step: String(step),
    ...(id ? { id } : {}),
    onInput: (e) => onInput?.(Number(e.target.value))
  });
  const group = el('div', { class: 'input-group' });
  if (label) group.appendChild(el('label', { class: 'input-label', text: label, ...(id ? { htmlFor: id } : {}) }));
  group.appendChild(input);
  return group;
}

export function createTextArea({ label, value = '', placeholder = '', rows = 3, id, onInput }) {
  const textarea = el('textarea', {
    class: 'input textarea',
    placeholder,
    rows: String(rows),
    ...(id ? { id } : {}),
    onInput: (e) => onInput?.(e.target.value)
  });
  textarea.value = value;
  const group = el('div', { class: 'input-group' });
  if (label) group.appendChild(el('label', { class: 'input-label', text: label, ...(id ? { htmlFor: id } : {}) }));
  group.appendChild(textarea);
  return group;
}
