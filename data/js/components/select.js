import { el } from '../utils/dom.js';

export function createSelect({ label, options = [], value = '', id, onChange }) {
  const select = el('select', {
    class: 'input select',
    ...(id ? { id } : {})
  });
  for (const opt of options) {
    const option = el('option', {
      value: opt.value,
      text: opt.label,
      ...(opt.value === value ? { selected: '' } : {})
    });
    select.appendChild(option);
  }
  select.addEventListener('change', () => onChange?.(select.value));

  const group = el('div', { class: 'input-group' });
  if (label) group.appendChild(el('label', { class: 'input-label', text: label, ...(id ? { htmlFor: id } : {}) }));
  group.appendChild(select);
  return group;
}
