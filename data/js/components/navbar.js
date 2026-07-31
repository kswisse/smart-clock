import { $, $$ } from '../utils/dom.js';

export function initNavbar() {
  const nav = $('#navbar');
  if (!nav) return;
  $$('.nav-item', nav).forEach(item => {
    item.addEventListener('click', (e) => {
      $$('.nav-item', nav).forEach(i => i.classList.remove('active'));
      item.classList.add('active');
    });
  });
}
