#!/usr/bin/env node
// Tests for the program editor calls in src/webui/www_source/ise/js/iseFunctions.js
// which set the channel of a condition or an activity (SetChannel): they reload
// the condition or activity and then call the optional callback, which the
// program editor uses to save the first entry shown for the new channel
// (instead of a fixed delay, see OpenCCU/OpenCCU#4775, #4776).
// The file is run in a small stub of Prototype and the page functions.
// Run with: node --test tests/webui/test-programs-set-channel.js
'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const vm = require('node:vm');

const SOURCE = path.join(__dirname, '..', '..', 'src', 'webui', 'www_source',
  'ise', 'js', 'iseFunctions.js');
const CODE = fs.readFileSync(SOURCE, 'latin1');

// Loads iseFunctions.js and returns the context, the sent requests and the
// reloaded conditions and activities with their callbacks.
function load() {
  const requests = [];
  const reloaded = [];
  const context = {
    Class: { create: () => function () { if (this.initialize) { this.initialize.apply(this, arguments); } } },
    Ajax: { Request: function (url, opts) { requests.push({ url, opts }); } },
    ReGa: { encode: (pb) => pb },
    ReloadSingleCondition: (id, onReloaded) => reloaded.push({ type: 'condition', id, onReloaded }),
    ReloadSingleDestination: (id, onReloaded) => reloaded.push({ type: 'activity', id, onReloaded }),
    SessionId: '@test@',
    dbg: false,
  };
  vm.createContext(context);
  vm.runInContext('String.prototype.strip = function () { return this.trim(); };', context);
  vm.runInContext(CODE, context);
  return { context, requests, reloaded };
}

for (const [object, type, url] of [
  ['iseSingleCondition', 'condition', '/esp/sico.htm'],
  ['iseSingleDestination', 'activity', '/esp/side.htm'],
]) {
  test(object + '.SetChannel reloads the ' + type + ' and passes the callback', () => {
    const env = load();
    const onReloaded = () => {};
    env.context[object].SetChannel('1010', '2020', onReloaded);
    assert.equal(env.requests.length, 1);
    assert.ok(env.requests[0].url.startsWith(url));
    assert.match(env.requests[0].opts.postBody, /string action = "SetChannel";/);
    // (the condition or activity is reloaded only after the channel is set)
    assert.equal(env.reloaded.length, 0);
    env.requests[0].opts.onComplete({ responseText: '' });
    assert.equal(env.reloaded.length, 1);
    assert.equal(env.reloaded[0].type, type);
    assert.equal(env.reloaded[0].id, '1010');
    assert.equal(env.reloaded[0].onReloaded, onReloaded);
  });

  test(object + '.SetChannel reloads the ' + type + ' without a callback', () => {
    const env = load();
    env.context[object].SetChannel('1010', '2020');
    env.requests[0].opts.onComplete({ responseText: '' });
    assert.equal(env.reloaded.length, 1);
    assert.equal(env.reloaded[0].onReloaded, undefined);
  });
}
