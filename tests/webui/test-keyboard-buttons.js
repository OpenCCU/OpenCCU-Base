#!/usr/bin/env node
// Tests for the keyboard operation of role="button" elements at the end of
// src/webui/www_source/ise/js/functions.js. The block is extracted from the
// source and run with small jQuery and DOM stubs.
// Run with: node --test tests/webui/test-keyboard-buttons.js
'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const vm = require('node:vm');

const SOURCE = path.join(__dirname, '..', '..', 'src', 'webui', 'www_source',
  'ise', 'js', 'functions.js');

function loadBlock() {
  const text = fs.readFileSync(SOURCE, 'latin1').replace(/\r\n/g, '\n');
  const start = text.indexOf('/**\n * Keyboard operation of elements');
  assert.notEqual(start, -1, 'keyboard block not found');
  return text.slice(start);
}

const CODE = loadBlock();

class Element {
  constructor(tagName, attributes = {}) {
    this.tagName = tagName;
    this.attributes = { ...attributes };
    this.clicks = 0;
    this.dispatched = [];
    this.children = [];
  }

  getAttribute(key) {
    return Object.hasOwn(this.attributes, key) ? this.attributes[key] : null;
  }

  setAttribute(key, value) { this.attributes[key] = String(value); }

  click() { this.clicks += 1; }

  dispatchEvent(event) { this.dispatched.push(event.type); }
}

// Supports only what the block uses: on(), find().addBack().each() and
// trigger().
function makeContext() {
  const handlers = {};
  const document = new Element('#document');
  const select = (nodes, selector) => {
    const attr = /^\[([\w-]+)\]$/.exec(selector)[1];
    return nodes.filter((node) => node.getAttribute(attr) !== null);
  };
  const wrap = (nodes) => ({
    find: (selector) => {
      const found = [];
      const walk = (node) => node.children.forEach((child) => {
        found.push(child);
        walk(child);
      });
      nodes.forEach(walk);
      const result = wrap(select(found, selector));
      result.addBack = (sel) => wrap(select(found, sel).concat(select(nodes, sel)));
      return result;
    },
    each: (fn) => nodes.forEach((node, i) => fn.call(node, i, node)),
    on: (type, selector, fn) => { handlers[type + ' ' + selector] = fn; },
    trigger: (type) => nodes.forEach((node) => node.dispatchEvent({ type })),
  });
  class MouseEvent {
    constructor(type, init) {
      this.type = type;
      this.init = init;
    }
  }
  const context = {
    document,
    window: {},
    MouseEvent,
    jQuery: (root) => wrap([root]),
    translateKey: (key) => 'T(' + key + ')',
  };
  vm.createContext(context);
  vm.runInContext(CODE, context);
  return { context, handlers, document };
}

function press(handlers, elm, eventInit) {
  const event = {
    target: elm,
    prevented: false,
    preventDefault() { this.prevented = true; },
    ...eventInit,
  };
  handlers["keydown [role='button']"].call(elm, event);
  return event;
}

test('Enter and Space click the button', () => {
  const { handlers } = makeContext();
  for (const key of ['Enter', ' ', 'Spacebar']) {
    const elm = new Element('DIV', { role: 'button' });
    const event = press(handlers, elm, { key });
    assert.equal(elm.clicks, 1, key);
    assert.equal(event.prevented, true, key);
  }
});

test('key codes 13 and 32 click the button in browsers without event.key', () => {
  const { handlers } = makeContext();
  for (const keyCode of [13, 32]) {
    const elm = new Element('TD', { role: 'button' });
    press(handlers, elm, { keyCode });
    assert.equal(elm.clicks, 1, String(keyCode));
  }
});

test('other keys and modifier combinations do nothing', () => {
  const { handlers } = makeContext();
  const cases = [
    { key: 'Tab', keyCode: 9 },
    { key: 'a', keyCode: 65 },
    { key: 'Enter', ctrlKey: true },
    { key: 'Enter', altKey: true },
    { key: ' ', metaKey: true },
  ];
  for (const init of cases) {
    const elm = new Element('DIV', { role: 'button' });
    const event = press(handlers, elm, init);
    assert.equal(elm.clicks, 0, JSON.stringify(init));
    assert.equal(event.prevented, false, JSON.stringify(init));
  }
});

test('keys from a child element or a native control are left alone', () => {
  const { handlers } = makeContext();
  const elm = new Element('TD', { role: 'button' });
  const child = new Element('INPUT');
  const event = press(handlers, elm, { key: 'Enter', target: child });
  assert.equal(elm.clicks, 0);
  assert.equal(event.prevented, false);

  for (const tagName of ['A', 'BUTTON', 'INPUT', 'SELECT', 'TEXTAREA']) {
    const native = new Element(tagName, { role: 'button' });
    press(handlers, native, { key: 'Enter' });
    assert.equal(native.clicks, 0, tagName);
  }
});

test('data-keyevent="mousedown" sends mousedown and mouseup instead of a click', () => {
  const { handlers } = makeContext();
  const elm = new Element('DIV', { role: 'button', 'data-keyevent': 'mousedown' });
  const event = press(handlers, elm, { key: 'Enter' });
  assert.deepEqual(elm.dispatched, ['mousedown', 'mouseup']);
  assert.equal(elm.clicks, 0);
  assert.equal(event.prevented, true);
});

test('applyAriaKeys sets the translated aria-label', () => {
  const { context, handlers, document } = makeContext();
  const up = new Element('TD', { 'data-aria-key': 'lblSysVarMoveUp' });
  const plain = new Element('TD');
  const row = new Element('TR');
  row.children.push(up, plain);
  document.children.push(row);

  context.applyAriaKeys();
  assert.equal(up.getAttribute('aria-label'), 'T(lblSysVarMoveUp)');
  assert.equal(plain.getAttribute('aria-label'), null);

  // The element itself is included, e.g. on focus.
  const help = new Element('IMG', { 'data-aria-key': 'HelpTitle' });
  handlers['focusin [data-aria-key]'].call(help, {});
  assert.equal(help.getAttribute('aria-label'), 'T(HelpTitle)');
});
