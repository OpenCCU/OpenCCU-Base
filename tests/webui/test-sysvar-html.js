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

const FORMATTING = ['B', 'STRONG', 'I', 'EM', 'U', 'S', 'STRIKE', 'DEL', 'INS', 'MARK', 'SMALL',
  'BIG', 'SUB', 'SUP', 'TT', 'CODE', 'SPAN', 'FONT'];
const BLOCKS = ['DIV', 'P', 'PRE', 'BLOCKQUOTE', 'CENTER', 'H1', 'H2', 'H3', 'H4', 'H5', 'H6',
  'UL', 'OL', 'LI', 'DL', 'DT', 'DD'];
const TABLES = ['TABLE', 'CAPTION', 'COLGROUP', 'THEAD', 'TBODY', 'TFOOT', 'TR', 'TD', 'TH'];

test('the allowed elements are kept without event handlers, class, id and title', () => {
  for (const name of [...FORMATTING, ...BLOCKS, ...TABLES]) {
    const tag = name.toLowerCase();
    assert.equal(
      sanitize(element(name, { onclick: 'x()', class: 'c', id: 'i', title: 't', href: 'h', src: 's' },
        text('x'))),
      `<${tag}>x</${tag}>`, name);
  }
  assert.equal(sanitize(text('a'), element('BR', { onmouseover: 'x()' }), text('b')), 'a<br/>b');
  assert.equal(sanitize(element('HR', { onclick: 'x()', width: '50%' })), '<hr width="50%"/>');
  assert.equal(sanitize(element('COL', { span: '2', onclick: 'x()' }, text('ignored'))),
    '<col span="2"/>');
});

test('font keeps a valid color, size and face', () => {
  assert.equal(sanitize(element('FONT', { color: 'red', size: '7', face: 'Courier New, monospace' },
    text('x'))), '<font color="red" size="7" face="Courier New, monospace">x</font>');
  assert.equal(sanitize(element('FONT', { color: ' #00FF00 ', size: '+1' }, text('x'))),
    '<font color="#00FF00" size="+1">x</font>');
  assert.equal(sanitize(element('FONT', { color: 'rgb(1, 2, 3)' }, text('x'))),
    '<font color="rgb(1, 2, 3)">x</font>');
  for (const color of ['" onmouseover="x()', 'red;x', 'url(x)', 'expression(alert(1))', '']) {
    assert.equal(sanitize(element('FONT', { color }, text('x'))), '<font>x</font>', color);
  }
  for (const attributes of [{ size: '72' }, { size: 'x' }, { face: '"x" onclick="y"' },
    { face: 'url(x)' }]) {
    assert.equal(sanitize(element('FONT', attributes, text('x'))), '<font>x</font>',
      JSON.stringify(attributes));
  }
});

test('table elements keep their presentational attributes with valid values', () => {
  assert.equal(
    sanitize(element('TABLE', { border: '1', cellpadding: '2', cellspacing: '0', width: '100%',
      height: '50', align: 'center', bgcolor: '#eee', onclick: 'x()' },
    element('TR', { align: 'right', valign: 'top', bgcolor: 'silver', height: '20' },
      element('TD', { colspan: '2', rowspan: '3', align: 'LEFT', valign: 'middle', width: '40',
        height: '10%', bgcolor: 'rgb(1,2,3)' }, text('x'))))),
    '<table align="center" bgcolor="#eee" border="1" cellpadding="2" cellspacing="0" width="100%" height="50">'
      + '<tr align="right" valign="top" bgcolor="silver" height="20">'
      + '<td align="LEFT" valign="middle" bgcolor="rgb(1,2,3)" width="40" height="10%" colspan="2" rowspan="3">x</td>'
      + '</tr></table>');
  for (const attributes of [{ colspan: '2" onclick="x()' }, { colspan: '-1' }, { colspan: '99999' },
    { width: '10px;x' }, { width: 'expression(x)' }, { align: 'left" x="' }, { valign: 'x' },
    { bgcolor: 'url(x)' }, { background: 'http://x/y.png' }, { border: '1' }]) {
    assert.equal(sanitize(element('TD', attributes, text('x'))), '<td>x</td>',
      JSON.stringify(attributes));
  }
  // an attribute is kept only on the elements it belongs to
  assert.equal(sanitize(element('SPAN', { colspan: '2', color: 'red', align: 'left' }, text('x'))),
    '<span>x</span>');
  assert.equal(sanitize(element('DIV', { align: 'center', width: '10' }, text('x'))),
    '<div align="center">x</div>');
});

test('style keeps the declarations of harmless properties', () => {
  assert.equal(sanitize(element('SPAN', { style: 'color: blue' }, text('x'))),
    '<span style="color:blue">x</span>');
  assert.equal(sanitize(element('SPAN', { style: 'font-weight:bold; COLOR:#abc;' }, text('x'))),
    '<span style="font-weight:bold;color:#abc">x</span>');
  assert.equal(
    sanitize(element('TABLE', { style: 'text-align:right; padding: 0px 5px 0px; border: 0px solid black; border-spacing: 2px;' })),
    '<table style="text-align:right;padding:0px 5px 0px;border:0px solid black;border-spacing:2px"></table>');
  // a declaration without a property is dropped, the others are kept
  assert.equal(
    sanitize(element('TABLE', { style: 'text-align:right; 0px 5px 0px;border: 0px solid black' })),
    '<table style="text-align:right;border:0px solid black"></table>');
  assert.equal(
    sanitize(element('TD', { style: 'background-color: rgba(0, 0, 0, 0.5);font-family: Courier New, monospace;'
      + 'font-size:1.2em;white-space:nowrap;vertical-align:top;border-bottom-color:hsl(10, 20%, 30%);'
      + 'min-width:10px;margin-left:auto;display:inline-block;text-decoration:underline' }, text('x'))),
    '<td style="background-color:rgba(0, 0, 0, 0.5);font-family:Courier New, monospace;'
      + 'font-size:1.2em;white-space:nowrap;vertical-align:top;border-bottom-color:hsl(10, 20%, 30%);'
      + 'min-width:10px;margin-left:auto;display:inline-block;text-decoration:underline">x</td>');
});

test('style drops every other declaration', () => {
  for (const style of [
    // properties that position the element, load content or are unknown
    'position:fixed', 'position:absolute;top:0;left:0', 'z-index:9999', 'float:left',
    'background:red', 'background-image:url(x)', 'content:x', 'transform:scale(9)',
    'behavior:x', '-moz-binding:x', 'filter:x', 'cursor:pointer', 'pointer-events:none',
    'overflow:visible', 'animation:x', 'transition:x', '--x:1', 'list-style:x',
    // values with functions, escapes, comments, quotes, !important or negative lengths
    'color:url(x)', 'color:expression(x)', 'width:expression(alert(1))', 'width:calc(1px + 2px)',
    'color:var(--x)', 'color:r\\65 d', 'color:red/**/', 'color:red!important',
    'font-family:"x"', "font-family:'x'", 'border:1px solid image-set(x)',
    'margin:-50px', 'margin-left:5px -5px', 'text-indent:-999px', 'letter-spacing:-1em',
    'color:red<', 'color:', ':red', 'color', '',
    'color:' + 'a'.repeat(201)]) {
    assert.equal(sanitize(element('DIV', { style }, text('x'))), '<div>x</div>', style);
  }
  // only the unsafe declarations of a style are dropped
  assert.equal(
    sanitize(element('DIV', { style: 'position:fixed;color:red;background:url(javascript:x);margin:-1px;padding:2px' },
      text('x'))),
    '<div style="color:red;padding:2px">x</div>');
  // a value can not leave the attribute
  assert.equal(sanitize(element('DIV', { style: 'color:red" onclick="x()' }, text('x'))),
    '<div>x</div>');
});

test('other elements are removed but keep their text', () => {
  assert.equal(sanitize(element('A', { href: 'javascript:x()' }, text('link'))), 'link');
  assert.equal(sanitize(element('SECTION', {}, element('ARTICLE', {}, text('p')))), 'p');
  assert.equal(sanitize(element('IMG', { src: 'x', onerror: 'x()' })), '');
  assert.equal(sanitize(element('FORM', { action: 'x' }, element('INPUT', { value: 'v' }),
    element('BUTTON', { onclick: 'x()' }, text('b')))), 'b');
  assert.equal(sanitize(element('LINK', { rel: 'stylesheet', href: 'x' }),
    element('META', { 'http-equiv': 'refresh' }), element('BASE', { href: 'x' })), '');
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
    sanitize(element('B', {}, text('a'), element('SECTION', {}, element('I', {}, text('b'))),
      comment('c'), element('SCRIPT', {}, text('x')))),
    '<b>a<i>b</i></b>');
  assert.equal(
    sanitize(element('DIV', {}, element('TABLE', {}, element('TBODY', {}, element('TR', {},
      element('TD', {}, text('a'), element('BR', {}), element('B', {}, text('b')))))))),
    '<div><table><tbody><tr><td>a<br/><b>b</b></td></tr></tbody></table></div>');
});

test('without a parser the value is shown as text', () => {
  const context = { document: {} };
  vm.createContext(context);
  vm.runInContext(CODE, context);
  assert.equal(context.sanitizeSysVarHtml('<b>x</b>'), '&lt;b&gt;x&lt;/b&gt;');
  assert.equal(context.sanitizeSysVarHtml(undefined), '');
});

// translateSysVarValue() with the real translateString() of translate.js.
function loadTranslation() {
  const read = (file) => fs.readFileSync(path.join(__dirname, '..', '..', 'src', 'webui', file),
    'latin1').replace(/\r\n/g, '\n');
  const extract = (text, name) => {
    const start = text.indexOf('\n' + name);
    assert.notEqual(start, -1, name + ' not found');
    const end = text.indexOf('\n}', start);
    return text.slice(start + 1, end + 2) + (text[end + 2] === ';' ? ';' : '');
  };
  const translate = read('www/webui/js/lang/translate.js');
  const functions = read('www_source/ise/js/functions.js');
  const context = {
    langJSON: { de: { date: 'Datum', time: 'Uhrzeit' } },
    getLang: () => 'de',
    translateKey: (key) => context.langJSON.de[key] || key,
    conInfo: () => {},
    jQuery: { each: (arr, fn) => arr.forEach((val, i) => fn(i, val)) },
  };
  vm.createContext(context);
  vm.runInContext(extract(translate, 'function translateString') + '\n'
    + extract(functions, 'translateSysVarValue = function'), context);
  return context;
}

test('every placeholder of a value is translated', () => {
  const context = loadTranslation();
  assert.equal(context.translateString('${date}<br/>${time}'), 'Datum<br/>Uhrzeit');
  assert.equal(context.translateSysVarValue('${date}<br/>${time}'), 'Datum<br/>Uhrzeit');
  assert.equal(context.translateSysVarValue('A ${date}, B ${unknown} C'), 'A Datum, B unknown C');
  assert.equal(context.translateSysVarValue('no placeholder <b>x</b>'), 'no placeholder <b>x</b>');
});

// showSysVarValues() and the observer that calls it for inserted elements,
// with a jQuery stub over placeholder objects.
function loadShow() {
  const text = fs.readFileSync(SOURCE, 'latin1').replace(/\r\n/g, '\n');
  const start = text.indexOf('\nshowSysVarValues = function');
  assert.notEqual(start, -1, 'showSysVarValues() not found');
  const end = text.indexOf('\n/**\n * Keyboard operation of elements', start);
  assert.notEqual(end, -1, 'end of the system variable block not found');
  return text.slice(start + 1, end);
}

function placeholder(value) {
  const cell = { innerHTML: '' };
  return { nodeType: 1, parentNode: cell, isPlaceholder: true, placeholders: [],
    getAttribute: (key) => (key === 'data-sysvar-value' ? value : null) };
}

function container(...placeholders) {
  return { nodeType: 1, isPlaceholder: false, placeholders };
}

function showContext() {
  const context = {
    document: { documentElement: {}, isPlaceholder: false, placeholders: [] },
    observers: [],
    sanitizeSysVarHtml: (html) => `[${html}]`,
    translateSysVarValue: (value) => value.replace('${a}', 'A'),
    jQuery: (root) => {
      const set = (nodes) => ({ each: (fn) => nodes.forEach((node) => fn.call(node)) });
      return {
        filter: (selector) => {
          assert.equal(selector, '[data-sysvar-value]');
          return set(root.isPlaceholder ? [root] : []);
        },
        find: (selector) => {
          assert.equal(selector, '[data-sysvar-value]');
          return set(root.placeholders);
        },
      };
    },
  };
  context.MutationObserver = class {
    constructor(callback) { this.callback = callback; }
    observe(target, options) { context.observers.push({ callback: this.callback, target, options }); }
  };
  vm.createContext(context);
  vm.runInContext(loadShow(), context);
  return context;
}

test('showSysVarValues shows the sanitized, translated values in the parent elements', () => {
  const context = showContext();
  const a = placeholder('<b>${a}</b>');
  const b = placeholder('x');
  context.document.placeholders = [a];
  context.showSysVarValues();
  assert.equal(a.parentNode.innerHTML, '[<b>A</b>]');
  context.showSysVarValues(container(b));
  assert.equal(b.parentNode.innerHTML, '[x]');
  // the root itself may be the placeholder
  const c = placeholder('c');
  context.showSysVarValues(c);
  assert.equal(c.parentNode.innerHTML, '[c]');
});

test('values are shown when their placeholders are inserted into the document', () => {
  const context = showContext();
  assert.equal(context.observers.length, 1);
  const observer = context.observers[0];
  assert.equal(observer.target, context.document.documentElement);
  assert.equal(observer.options.childList, true);
  assert.equal(observer.options.subtree, true);
  const a = placeholder('a');
  const b = placeholder('b');
  observer.callback([
    { addedNodes: [{ nodeType: 3 }, container(a)] },
    { addedNodes: [b] },
  ]);
  assert.equal(a.parentNode.innerHTML, '[a]');
  assert.equal(b.parentNode.innerHTML, '[b]');
});

// The ReGa pages must not write the value of getSysVarValAsString() as HTML:
// they pass it to writeSysVarVal() or, in a script, to sanitizeSysVarHtml().
test('the pages write system variable values only as sanitized HTML', () => {
  const root = path.join(__dirname, '..', '..', 'src', 'webui', 'rega', 'www');
  const files = [];
  const collect = (dir) => {
    for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
      const file = path.join(dir, entry.name);
      if (entry.isDirectory()) {
        collect(file);
      } else if (/\.(htm|fn|inc)$/.test(entry.name)) {
        files.push(file);
      }
    }
  };
  collect(root);
  let callers = 0;
  for (const file of files) {
    const lines = fs.readFileSync(file, 'latin1').split(/\r?\n/);
    if (!lines.some((line) => /Call\(\s*"\/esp\/system\.fn::getSysVarValAsString\(\)"/.test(line))) {
      continue;
    }
    callers++;
    lines.forEach((line, i) => {
      if (!/\bWrite(Line)?\s*\(/.test(line) || !/\bsSysVarVal\b/.test(line)) {
        return;
      }
      const where = `${path.relative(root, file)}:${i + 1}`;
      assert.match(line, /data-sysvar-value="|sanitizeSysVarHtml\(/, where);
    });
  }
  assert.ok(callers >= 3, 'callers of getSysVarValAsString() not found');
  const system = fs.readFileSync(path.join(root, 'esp', 'system.fn'), 'latin1');
  assert.match(system, /function ::writeSysVarVal\(\)\s*\{\s*Write\( '<span data-sysvar-value="' # sSysVarVal\.Replace\("&", "&amp;"\)\.Replace\("<", "&lt;"\)\.Replace\(">", "&gt;"\)\.Replace\('"', "&quot;"\) # '"><\/span>' \);/);
});
