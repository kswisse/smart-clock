export const $ = (sel, ctx = document) => ctx.querySelector(sel);
export const $$ = (sel, ctx = document) => [...ctx.querySelectorAll(sel)];

export function el(tag, attrs = {}, children = []) {
  const elem = document.createElement(tag);
  for (const [k, v] of Object.entries(attrs)) {
    if (k === 'class') elem.className = v;
    else if (k === 'style' && typeof v === 'object') Object.assign(elem.style, v);
    else if (k.startsWith('on')) elem.addEventListener(k.slice(2).toLowerCase(), v);
    else if (k === 'dataset') Object.assign(elem.dataset, v);
    else if (k === 'html') elem.innerHTML = v;
    else if (k === 'text') elem.textContent = v;
    else elem.setAttribute(k, v);
  }
  for (const child of children) {
    if (typeof child === 'string') elem.appendChild(document.createTextNode(child));
    else if (child) elem.appendChild(child);
  }
  return elem;
}

export function clearChildren(elem) {
  while (elem.firstChild) elem.removeChild(elem.firstChild);
}

export function show(elem) {
  if (elem) elem.classList.remove('hidden');
}

export function hide(elem) {
  if (elem) elem.classList.add('hidden');
}

export function toggle(elem, force) {
  if (elem) elem.classList.toggle('hidden', force !== undefined ? !force : undefined);
}

export function setHTML(elem, html) {
  if (elem) elem.innerHTML = html;
}

export function setText(elem, text) {
  if (elem) elem.textContent = text;
}

export function addClass(elem, cls) {
  if (elem) elem.classList.add(cls);
}

export function removeClass(elem, cls) {
  if (elem) elem.classList.remove(cls);
}

export function hasClass(elem, cls) {
  return elem ? elem.classList.contains(cls) : false;
}

export function onReady(fn) {
  if (document.readyState !== 'loading') fn();
  else document.addEventListener('DOMContentLoaded', fn);
}
