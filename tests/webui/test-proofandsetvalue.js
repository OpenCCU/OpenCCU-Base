#!/usr/bin/env node
// Tests for ProofAndSetValue() in src/webui/www_source/config/js/ic_common.js.
// The function is extracted from the source and run in a small DOM stub.
// Run with: node --test tests/webui/test-proofandsetvalue.js
'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const vm = require('node:vm');

const SOURCE = path.join(__dirname, '..', '..', 'src', 'webui', 'www_source',
  'config', 'js', 'ic_common.js');

function loadProofAndSetValue() {
  const text = fs.readFileSync(SOURCE, 'latin1').replace(/\r\n/g, '\n');
  const start = text.indexOf('\nProofAndSetValue = function');
  assert.notEqual(start, -1, 'ProofAndSetValue() not found');
  const end = text.indexOf('\n};', start);
  assert.notEqual(end, -1, 'end of ProofAndSetValue() not found');
  return text.slice(start + 1, end + 3);
}

const CODE = loadProofAndSetValue();

class Input {
  constructor(id, value, name) {
    this.id = id;
    this.name = name;
    this.attributes = {};
    this.style = {};
    this.value = value;
  }

  // A DOM input stores its value as a string.
  get value() { return this._value; }
  set value(v) { this._value = String(v); }

  setAttribute(key, value) { this.attributes[key] = String(value); }
}

// Runs ProofAndSetValue() for one input and returns the visible and the
// hidden element. With separate=false both ids name the same element, as in
// the easymodes that call ProofAndSetValue(this.id, this.id, ...).
function run(input, min, max, options = {}) {
  const { factor = 1, convInt2Float, name = '', separate = true } = options;
  const src = new Input('src', input, name);
  const dst = separate ? new Input('dst', '', name) : src;
  const elements = { src, dst };
  const context = {
    $: (id) => elements[id],
    $F: (id) => elements[id].value,
    jQuery: (elm) => ({ attr: (key) => elm[key] }),
    conInfo: () => {},
    window: { setTimeout: () => {} },
  };
  vm.createContext(context);
  vm.runInContext(CODE, context);
  context.ProofAndSetValue('src', separate ? 'dst' : 'src', min, max, factor,
    convInt2Float, undefined);
  return { src, dst };
}

function check(input, min, max, shown, stored, valid, options) {
  const { src, dst } = run(input, min, max, options);
  assert.equal(src.value, shown, 'visible value');
  if (stored !== undefined) {
    assert.equal(dst.value, stored, 'hidden value');
  }
  assert.equal(src.attributes.valvalid, valid ? 'true' : 'false', 'valvalid');
}

test('integer range', () => {
  check('5', '10', '20', '10', '10', false);
  check('21', '10', '20', '20', '20', false);
  check('11.51', '10', '20', '12', '12', true);
  check('3.6', '0', '10', '4', '4', true);
});

test('decimal places follow the range', () => {
  // Decimal ranges clamp the value without marking the input invalid.
  check('1.5', '10.0', '20.0', '10.0', '10', true);
  check('21.5', '10.0', '20.0', '20.0', '20', true);
  check('11.0', '10.0', '20.0', '11.0', '11', true);
  check('11.05', '10.0', '20.0', '11.1', '11.1', true);
  check('11.5', '10.00', '20.00', '11.50', '11.5', true);
});

test('decimal ties are rounded up', () => {
  check('1.005', '0.00', '10.00', '1.01', '1.01', true);
  check('2.675', '0.00', '10.00', '2.68', '2.68', true);
});

test('invalid input falls back to the minimum', () => {
  check('a', '10', '20', '10', '10', false);
});

test('decimal comma and negative values', () => {
  check('11,5', '10.0', '20.0', '11.5', '11.5', true);
  check('-1,9', '-180.0', '180.0', '-1.9', '-1.9', true);
  check('2.0', '-180.0', '180.0', '2.0', '2', true);
});

test('the hidden value has no floating point residue', () => {
  // Percent parameters: the input is in %, the parameter in 0..1.
  check('57', '0', '100', '57', '0.57', true, { factor: 0.01 });
  check('55.5', '0', '100', '55.50', '0.555', true,
    { factor: 0.01, convInt2Float: true });
});

test('the visible input shows the clamped value', () => {
  check('20', '0.0', '10.0', '10.0', '10', true);
});

test('a shared input keeps the formatted value', () => {
  const { src } = run('15', '0.0', '30.0', { separate: false });
  assert.equal(src.value, '15.0');
});

test('METER_CONSTANT_VOLUME keeps three decimal places', () => {
  check('0.0015', '0.001', '65.535', '0.002', '0.002', true,
    { convInt2Float: true, name: 'METER_CONSTANT_VOLUME' });
});
