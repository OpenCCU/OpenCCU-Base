#!/usr/bin/env node
// Tests for sanitizeSysVarHtml() in src/webui/www_source/ise/js/functions.js.
// The function is extracted from the source. A stub DOMParser returns node
// trees built by the tests, so this checks which elements and attributes are
// kept; the browser does the actual HTML parsing.
// Run with: node --test tests/webui/test-sysvar-html.js
'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const vm = require('node:vm');

const SOURCE = path.join(__dirname, '..', '..', 'src', 'webui', 'www_source',
  'ise', 'js', 'functions.js');

function loadFunction() {
  const text = fs.readFileSync(SOURCE, 'latin1').replace(/\r\n/g, '\n');
  const start = text.indexOf('\nsanitizeSysVarHtml = function');
  assert.notEqual(start, -1, 'sanitizeSysVarHtml() not found');
  const end = text.indexOf('\n};', start);
  assert.notEqual(end, -1, 'end of sanitizeSysVarHtml() not found');
  return text.slice(start + 1, end + 3);
}

const CODE = loadFunction();

// Minimal nodes: element(name, attributes, ...children) and text(value).
function link(node, children) {
  node.firstChild = children[0] || null;
  children.forEach((child, i) => { child.nextSibling = children[i + 1] || null; });
  return node;
}

function element(nodeName, attributes, ...children) {
  return link({
    nodeType: 1,
    nodeName,
    getAttribute: (key) => (Object.hasOwn(attributes, key) ? attributes[key] : null),
  }, children);
}

function text(nodeValue) {
  return { nodeType: 3, nodeValue };
}

function comment(nodeValue) {
  return { nodeType: 8, nodeValue };
}

function sanitize(...children) {
  const body = link({ nodeType: 1, nodeName: 'BODY' }, children);
  const context = {
    DOMParser: class {
      parseFromString(html, type) {
        assert.equal(html, 'INPUT');
        assert.equal(type, 'text/html');
        return { body };
      }
    },
    document: {},
  };
  vm.createContext(context);
  vm.runInContext(CODE, context);
  return context.sanitizeSysVarHtml('INPUT');
}

test('text is HTML-escaped', () => {
  assert.equal(sanitize(text('a < b & "c" > d')), 'a &lt; b &amp; &quot;c&quot; &gt; d');
});

test('the allowed formatting elements are kept without attributes', () => {
  for (const name of ['B', 'STRONG', 'I', 'EM', 'U', 'S', 'SMALL', 'SUB', 'SUP', 'SPAN', 'FONT']) {
    const tag = name.toLowerCase();
    assert.equal(
      sanitize(element(name, { onclick: 'x()', class: 'c', id: 'i', title: 't' }, text('x'))),
      `<${tag}>x</${tag}>`, name);
  }
  assert.equal(sanitize(text('a'), element('BR', { onmouseover: 'x()' }), text('b')), 'a<br/>b');
});

test('font keeps a valid color', () => {
  assert.equal(sanitize(element('FONT', { color: 'red', size: '7' }, text('x'))),
    '<font color="red">x</font>');
  assert.equal(sanitize(element('FONT', { color: ' #00FF00 ' }, text('x'))),
    '<font color="#00FF00">x</font>');
  assert.equal(sanitize(element('FONT', { color: 'rgb(1, 2, 3)' }, text('x'))),
    '<font color="rgb(1, 2, 3)">x</font>');
  for (const color of ['" onmouseover="x()', 'red;x', 'url(x)', 'expression(alert(1))', '']) {
    assert.equal(sanitize(element('FONT', { color }, text('x'))), '<font>x</font>', color);
  }
});

test('span keeps only a valid color of its style', () => {
  assert.equal(sanitize(element('SPAN', { style: 'color: blue' }, text('x'))),
    '<span style="color:blue">x</span>');
  assert.equal(sanitize(element('SPAN', { style: 'font-weight:bold; COLOR:#abc;' }, text('x'))),
    '<span style="color:#abc">x</span>');
  for (const style of ['background-color:red', 'color:url(x)', 'color:red!important',
    'background:url(javascript:x)', 'color:expression(x)', 'position:fixed']) {
    assert.equal(sanitize(element('SPAN', { style }, text('x'))), '<span>x</span>', style);
  }
});

test('other elements are removed but keep their text', () => {
  assert.equal(sanitize(element('A', { href: 'javascript:x()' }, text('link'))), 'link');
  assert.equal(sanitize(element('DIV', {}, element('P', {}, text('p')))), 'p');
  assert.equal(sanitize(element('IMG', { src: 'x', onerror: 'x()' })), '');
  assert.equal(sanitize(element('svg', { onload: 'x()' }, text('s'))), '');
  assert.equal(sanitize(element('math', {}, text('m'))), '');
});

test('script, style and similar elements are removed with their content', () => {
  for (const name of ['SCRIPT', 'STYLE', 'IFRAME', 'OBJECT', 'EMBED', 'NOSCRIPT',
    'TEMPLATE', 'TEXTAREA', 'TITLE', 'SELECT']) {
    assert.equal(sanitize(text('a'), element(name, {}, text('alert(1)')), text('b')), 'ab', name);
  }
});

test('nesting is kept and comments are removed', () => {
  assert.equal(
    sanitize(element('B', {}, text('a'), element('DIV', {}, element('I', {}, text('b'))),
      comment('c'), element('SCRIPT', {}, text('x')))),
    '<b>a<i>b</i></b>');
});

test('without a parser the value is shown as text', () => {
  const context = { document: {} };
  vm.createContext(context);
  vm.runInContext(CODE, context);
  assert.equal(context.sanitizeSysVarHtml('<b>x</b>'), '&lt;b&gt;x&lt;/b&gt;');
  assert.equal(context.sanitizeSysVarHtml(undefined), '');
});
