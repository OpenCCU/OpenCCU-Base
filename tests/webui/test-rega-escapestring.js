#!/usr/bin/env node
// Tests for ReGa.escapeString() in src/webui/www_source/ise/js/rega.js.
// The function escapes a value for a double-quoted string literal of a ReGa
// script. The tests unescape the result like ReGa does (ise_string::unescape)
// and check that the original value comes back.
// Run with: node --test tests/webui/test-rega-escapestring.js
'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const vm = require('node:vm');

const SOURCE = path.join(__dirname, '..', '..', 'src', 'webui', 'www_source',
  'ise', 'js', 'rega.js');

function loadReGa() {
  const context = { escape: globalThis.escape };
  vm.runInNewContext(fs.readFileSync(SOURCE, 'latin1'), context);
  return context.ReGa;
}

const ReGa = loadReGa();

// Reads a double-quoted string literal starting at text[0] like the ReGa
// script parser: the literal ends at the first unescaped quote, then the
// escape sequences \\ \' \" \t \n \r are replaced.
function parseReGaLiteral(text) {
  assert.equal(text[0], '"');
  let end = 1;
  let escaped = false;
  for (; end < text.length; end++) {
    if (escaped) { escaped = false; } else if (text[end] === '\\') { escaped = true; } else if (text[end] === '"') { break; }
  }
  assert.equal(text[end], '"', 'literal is not terminated');
  const map = { '\\': '\\', '\'': '\'', '"': '"', t: '\t', n: '\n', r: '\r' };
  const body = text.slice(1, end).replace(/\\(.)/g, (m, c) => (c in map ? map[c] : m));
  return { value: body, rest: text.slice(end + 1) };
}

const VALUES = [
  'plain',
  'O\'Brien',
  'He said "hi"',
  'C:\\temp\\',
  'ends with a backslash \\',
  '\\"',
  'line 1\nline 2\r\nline 3',
  '<b>fett</b> & </script>',
  '',
];

test('escaped values round-trip through a ReGa string literal', () => {
  for (const value of VALUES) {
    const script = '"' + ReGa.escapeString(value) + '";';
    const parsed = parseReGaLiteral(script);
    assert.equal(parsed.value, value);
    assert.equal(parsed.rest, ';', `literal of ${JSON.stringify(value)} ends early`);
  }
});

test('escapes backslash, double quote and line breaks', () => {
  assert.equal(ReGa.escapeString('a\\b"c\rd\ne'), 'a\\\\b\\"c\\rd\\ne');
});

test('leaves other characters unchanged', () => {
  assert.equal(ReGa.escapeString('O\'Brien <b>&amp;</b> äöü'), 'O\'Brien <b>&amp;</b> äöü');
});

test('converts non-string values', () => {
  assert.equal(ReGa.escapeString(12.5), '12.5');
  assert.equal(ReGa.escapeString(undefined), 'undefined');
});
