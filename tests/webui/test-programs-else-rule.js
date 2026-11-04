#!/usr/bin/env node
// Tests for the program editor calls in src/webui/www_source/ise/js/iseFunctions.js
// that may remove an empty ELSE rule (DeleteSingleDestination, SetBreakOnRestart).
// The file is run in a small stub of Prototype and the page functions.
// Run with: node --test tests/webui/test-programs-else-rule.js
'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const vm = require('node:vm');

const SOURCE = path.join(__dirname, '..', '..', 'src', 'webui', 'www_source',
  'ise', 'js', 'iseFunctions.js');
const CODE = fs.readFileSync(SOURCE, 'latin1');

// Loads iseFunctions.js and returns the context, the sent requests, the
// reloaded rules and the removed page elements.
function load(ruleIds = []) {
  const requests = [];
  const reloaded = [];
  const removed = [];
  const elements = {};
  for (const rid of ruleIds) {
    elements['rule' + rid] = { remove: () => { removed.push(rid); delete elements['rule' + rid]; } };
  }
  const context = {
    Class: { create: () => function () { if (this.initialize) { this.initialize.apply(this, arguments); } } },
    Ajax: { Request: function (url, opts) { requests.push({ url, opts }); } },
    ReGa: { encode: (pb) => pb },
    $: (id) => elements[id],
    ReloadRule: (rid) => reloaded.push(String(rid)),
    SessionId: '@test@',
    dbg: false,
  };
  vm.createContext(context);
  vm.runInContext('String.prototype.strip = function () { return this.trim(); };', context);
  vm.runInContext(CODE, context);
  return { context, requests, reloaded, removed };
}

// Calls fn of isePrograms and answers its request with responseText.
function call(env, fn, args, responseText) {
  env.context.isePrograms[fn](...args);
  assert.equal(env.requests.length, 1);
  const { opts } = env.requests[0];
  opts.onComplete({ responseText });
  return opts.postBody;
}

test('DeleteSingleDestination sends the rule id', () => {
  const env = load();
  const body = call(env, 'DeleteSingleDestination', ['1010', '1011', '1012'], 'true');
  assert.match(body, /string action = "DeleteSingleDestination";/);
  assert.match(body, /string rid = "1010";/);
  assert.match(body, /string did = "1011";/);
  assert.match(body, /string sid = "1012";/);
});

test('DeleteSingleDestination reloads the rule after an ordinary delete', () => {
  const env = load(['1010']);
  call(env, 'DeleteSingleDestination', ['1010', '1011', '1012'], '\r\ntrue\r\n');
  assert.deepEqual(env.reloaded, ['1010']);
  assert.deepEqual(env.removed, []);
});

test('DeleteSingleDestination drops a removed ELSE rule and reloads its parent', () => {
  const env = load(['1009', '1010']);
  call(env, 'DeleteSingleDestination', ['1010', '1011', '1012'], ' removedRule:1009\r\n');
  assert.deepEqual(env.removed, ['1010']);
  assert.deepEqual(env.reloaded, ['1009']);
});

test('SetBreakOnRestart sends the rule id', () => {
  const env = load();
  const body = call(env, 'SetBreakOnRestart', ['1010', '1011', false], 'true');
  assert.match(body, /string action = "SetBreakOnRestart";/);
  assert.match(body, /string rid = "1010";/);
  assert.match(body, /string id = "1011";/);
  assert.match(body, /string value = "false";/);
});

test('SetBreakOnRestart reloads the rule when nothing was removed', () => {
  const env = load(['1010']);
  call(env, 'SetBreakOnRestart', ['1010', '1011', true], 'true');
  assert.deepEqual(env.reloaded, ['1010']);
  assert.deepEqual(env.removed, []);
});

test('SetBreakOnRestart drops a removed ELSE rule and reloads its parent', () => {
  const env = load(['1009', '1010']);
  call(env, 'SetBreakOnRestart', ['1010', '1011', false], 'removedRule:1009');
  assert.deepEqual(env.removed, ['1010']);
  assert.deepEqual(env.reloaded, ['1009']);
});

test('a missing rule element does not stop the reload of the parent', () => {
  const env = load(['1009']);
  call(env, 'DeleteSingleDestination', ['1010', '1011', '1012'], 'removedRule:1009');
  assert.deepEqual(env.removed, []);
  assert.deepEqual(env.reloaded, ['1009']);
});
