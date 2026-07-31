import { el, clearChildren } from '../utils/dom.js';
import { store } from '../core/store.js';
import { todoApi } from '../api/todo.js';
import { showToast } from '../components/toast.js';
import { confirmDialog } from '../components/dialog.js';
import { showModal } from '../components/modal.js';
import { createInput, createTextArea } from '../components/input.js';
import { CONFIG } from '../core/config.js';
import { debounce } from '../utils/helpers.js';

let container = null;
let searchInput = null;
let searchTerm = '';
let unsubscribers = [];

function getFilteredTodos() {
  const todos = store.get('todos') || [];
  if (!searchTerm) return todos;
  const term = searchTerm.toLowerCase();
  return todos.filter(t =>
    t.title.toLowerCase().includes(term) ||
    (t.description && t.description.toLowerCase().includes(term))
  );
}

function renderTodoItem(todo) {
  const check = el('div', {
    class: `todo-check ${todo.completed ? 'checked' : ''}`,
    onClick: (e) => { e.stopPropagation(); toggleTodo(todo); }
  });

  const bar = el('div', { class: 'todo-color-bar', style: { background: todo.color || 'var(--accent)' } });

  const title = el('div', {
    class: `todo-title ${todo.completed ? 'completed' : ''}`,
    text: todo.title
  });

  const desc = todo.description ?
    el('div', { class: 'todo-desc', text: todo.description }) : null;

  const editBtn = el('button', {
    class: 'btn btn-icon btn-ghost btn-sm',
    html: '<svg viewBox="0 0 24 24"><path fill="currentColor" d="M3 17.25V21h3.75L17.81 9.94l-3.75-3.75L3 17.25zM20.71 7.04c.39-.39.39-1.02 0-1.41l-2.34-2.34c-.39-.39-1.02-.39-1.41 0l-1.83 1.83 3.75 3.75 1.83-1.83z"/></svg>',
    onClick: (e) => { e.stopPropagation(); openEditModal(todo); }
  });

  const delBtn = el('button', {
    class: 'btn btn-icon btn-ghost btn-sm',
    html: '<svg viewBox="0 0 24 24"><path fill="currentColor" d="M6 19c0 1.1.9 2 2 2h8c1.1 0 2-.9 2-2V7H6v12zM19 4h-3.5l-1-1h-5l-1 1H5v2h14V4z"/></svg>',
    onClick: (e) => { e.stopPropagation(); deleteTodo(todo); }
  });

  const content = el('div', { class: 'list-item-main' }, [title, desc].filter(Boolean));

  return el('div', {
    class: 'list-item todo-item',
    onClick: () => openEditModal(todo),
    style: { position: 'relative' }
  }, [bar, check, content, el('div', { class: 'todo-actions' }, [editBtn, delBtn])]);
}

async function toggleTodo(todo) {
  try {
    await todoApi.toggleComplete(todo.id, !todo.completed);
  } catch (e) {
    showToast('Failed to update todo', 'error');
  }
}

async function deleteTodo(todo) {
  const ok = await confirmDialog('Delete Todo', `Delete "${todo.title}"?`);
  if (!ok) return;
  try {
    await todoApi.remove(todo.id);
    showToast('Todo deleted', 'success');
  } catch (e) {
    showToast('Failed to delete', 'error');
  }
}

function openEditModal(todo = null) {
  const isEdit = !!todo;
  let titleVal = todo?.title || '';
  let descVal = todo?.description || '';
  let colorVal = todo?.color || CONFIG.COLORS[0];

  const titleInput = createInput({ label: 'Title', value: titleVal, placeholder: 'What needs to be done?', onInput: v => titleVal = v });
  const descInput = createTextArea({ label: 'Description (optional)', value: descVal, placeholder: 'Add details...', onInput: v => descVal = v });

  const colorPicker = el('div', { class: 'color-picker mt-sm' });
  CONFIG.COLORS.forEach(c => {
    const dot = el('div', {
      class: `color-option ${c === colorVal ? 'selected' : ''}`,
      style: { background: c },
      onClick: () => {
        colorVal = c;
        colorPicker.querySelectorAll('.color-option').forEach(d => d.classList.remove('selected'));
        dot.classList.add('selected');
      }
    });
    colorPicker.appendChild(dot);
  });

  const form = el('div', {}, [titleInput, descInput, el('div', { class: 'input-group' }, [
    el('label', { class: 'input-label', text: 'Color' }), colorPicker
  ])]);

  const modal = showModal({
    title: isEdit ? 'Edit Todo' : 'Add Todo',
    content: form,
    onClose: async () => {
      if (!titleVal.trim()) return;
      try {
        if (isEdit) {
          await todoApi.update(todo.id, { title: titleVal.trim(), description: descVal.trim(), color: colorVal });
        } else {
          await todoApi.create({ title: titleVal.trim(), description: descVal.trim(), color: colorVal });
        }
        showToast(isEdit ? 'Todo updated' : 'Todo added', 'success');
      } catch (e) {
        showToast('Failed to save', 'error');
      }
    }
  });
}

function render() {
  if (!container) return;
  clearChildren(container);

  const todos = getFilteredTodos();
  const list = el('div', { class: 'list' });

  if (todos.length === 0) {
    list.appendChild(el('div', { class: 'empty-state' }, [
      el('p', { text: searchTerm ? 'No matching todos' : 'No todos yet' })
    ]));
  } else {
    todos.forEach(t => list.appendChild(renderTodoItem(t)));
  }

  searchInput = el('input', {
    class: 'input',
    type: 'search',
    placeholder: 'Search todos...',
    value: searchTerm,
    onInput: debounce((v) => { searchTerm = v; render(); }, 200)
  });

  const fab = el('button', {
    class: 'fab',
    html: '<svg viewBox="0 0 24 24"><path fill="currentColor" d="M19 13h-6v6h-2v-6H5v-2h6V5h2v6h6v2z"/></svg>',
    onClick: () => openEditModal()
  });

  container.appendChild(el('div', { class: 'page-content' }, [
    el('div', { class: 'page-header' }, [
      el('h1', { class: 'page-title', text: 'Todos' }),
      el('span', { class: 'badge badge-info', text: `${todos.length}` })
    ]),
    el('div', { style: { padding: '0 0 12px' } }, [searchInput]),
    list,
    fab
  ]));
}

async function load() {
  try { await todoApi.getAll(); } catch (e) { showToast('Failed to load todos', 'error'); }
}

function init(c) {
  container = c;
  unsubscribers.push(store.subscribe('todos', () => render()));
}

function destroy() {
  unsubscribers.forEach(unsub => unsub());
  unsubscribers = [];
  container = null;
  searchTerm = '';
}

export const todoPage = { init, load, render, destroy };
