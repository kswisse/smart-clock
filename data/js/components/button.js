import { el } from '../utils/dom.js';

export function createButton(text, variant = 'primary', onClick) {
  return el('button', {
    class: `btn btn-${variant}`,
    text: text,
    onClick
  });
}

export function createIconButton(iconSvg, variant = 'ghost', onClick, title = '') {
  return el('button', {
    class: `btn btn-icon btn-${variant}`,
    html: iconSvg,
    onClick,
    title
  });
}

export function createFAB(iconSvg, onClick) {
  return el('button', { class: 'fab', html: iconSvg, onClick });
}
