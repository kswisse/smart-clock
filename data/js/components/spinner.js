import { el } from '../utils/dom.js';

export function createSpinner(size = 'md') {
  return el('div', { class: `spinner spinner-${size}` });
}

export function createPageLoading() {
  return el('div', { class: 'page-loading' }, [createSpinner('lg')]);
}
