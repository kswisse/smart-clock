import { el } from '../utils/dom.js';

export function createToggle({ label, checked = false, onChange }) {
  const input = el('input', {
    type: 'checkbox',
    class: 'toggle-input',
    ...(checked ? { checked: '' } : {})
  });
  const slider = el('span', { class: 'toggle-slider' });
  const switchEl = el('label', { class: 'toggle-switch' }, [input, slider]);

  input.addEventListener('change', () => onChange?.(input.checked));

  const group = el('div', { class: 'toggle-group' });
  if (label) group.appendChild(el('span', { class: 'toggle-label', text: label }));
  group.appendChild(switchEl);
  return group;
}
