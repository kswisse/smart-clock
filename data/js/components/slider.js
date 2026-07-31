import { el } from '../utils/dom.js';

export function createSlider({ label, value = 50, min = 0, max = 100, step = 1, id, onChange }) {
  const range = el('input', {
    type: 'range',
    class: 'slider-input',
    min: String(min),
    max: String(max),
    step: String(step),
    value: String(value),
    ...(id ? { id } : {})
  });
  const valueLabel = el('span', { class: 'slider-value', text: String(value) });

  range.addEventListener('input', () => {
    const v = Number(range.value);
    valueLabel.textContent = String(v);
    onChange?.(v);
  });

  const group = el('div', { class: 'slider-group' });
  if (label) group.appendChild(el('label', { class: 'slider-label', text: label, ...(id ? { htmlFor: id } : {}) }));
  const row = el('div', { class: 'slider-row' }, [range, valueLabel]);
  group.appendChild(row);
  return group;
}
