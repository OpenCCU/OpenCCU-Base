#!/usr/bin/env node
// Tests for updateContent() in src/webui/www_source/ise/js/functions.js.
//
// A call while a page is loading used to be dropped. It is now remembered
// (only the last one) and carried out after the loading page is complete:
// after Prototype ran the scripts of the page (deferred by 10 ms) and the
// WebUI fired ContentLoaded (100 ms), unless another page of the menu was
// opened meanwhile. The functions are extracted from the
// source and run with stubs for Prototype's Ajax.Updater and fake timers.
// Run with: node --test tests/webui/test-updatecontent-queue.js
'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const vm = require('node:vm');

const SOURCE = path.join(__dirname, '..', '..', 'src', 'webui', 'www_source',
  'ise', 'js', 'functions.js');
const TEXT = fs.readFileSync(SOURCE, 'latin1').replace(/\r\n/g, '\n');

// Returns the source from the line starting with `from` to the end of the
// function that starts with `last`.
function extract(from, last) {
  const start = TEXT.indexOf(from);
  assert.notEqual(start, -1, `${from} not found`);
  const lastStart = TEXT.indexOf(last, start);
  assert.notEqual(lastStart, -1, `${last} not found`);
  const end = TEXT.indexOf('\n};\n', lastStart);
  assert.notEqual(end, -1, `end of ${last} not found`);
  return TEXT.slice(start, end + 4);
}

const CODE = [
  extract('updateContentCurrentPage = function', 'updateContent = function'),
  extract('reloadSortedPage = function', 'reloadPage = function'),
].join('\n');

function makeWebUI() {
  let now = 0;
  let timers = [];
  const requests = [];
  const events = [];
  const log = [];
  const content = { innerHTML: '' };
  const context = {
    bUpdateContentRunning: false,
    doClearTempView: true,
    sActPage: '', sActPageArgs: '', sPreviousPage: '', sPreviousPageArgs: '',
    sPreviousPage2: '', sPreviousPageArgs2: '',
    dbg: false,
    WebUI: { currentPage: 'StartPage' },
    SessionId: 'SID',
    UI_PATH: '/config/',
    $: (id) => (id === 'content' ? content : null),
    LoadingHTML: () => 'LOADING',
    iseRefr: (state) => log.push('iseRefr ' + state),
    iseInitUpdateArrays: () => {},
    conInfo: () => {},
    ReGa: { encode: (pb) => pb },
    eQ3: { HomeMatic: { Event: { fire: (event) => { events.push(event.type); if (context.failEvent) { throw new Error('handler'); } } } } },
    Ajax: {
      Updater: function(container, url, opts) {
        assert.equal(container, 'content');
        requests.push({ url, opts, done: false });
      },
    },
    setTimeout: (fn, ms) => { timers.push({ at: now + ms, fn }); },
  };
  vm.runInNewContext(CODE, context);

  // Runs the timers due until `ms` milliseconds from now.
  function advance(ms) {
    const until = now + ms;
    for (;;) {
      timers.sort((a, b) => a.at - b.at);
      const next = timers[0];
      if (!next || next.at > until) break;
      timers.shift();
      now = next.at;
      next.fn();
    }
    now = until;
  }

  // Completes a page request like Prototype's Ajax.Updater: the content is
  // replaced, onComplete is called, and the scripts of the page run deferred
  // (10 ms). The page scripts record what they find in the content.
  function complete(index, html) {
    const request = requests[index];
    assert.equal(request.done, false, 'request completed twice');
    request.done = true;
    content.innerHTML = html;
    timers.push({ at: now + 10, fn: () => log.push('scripts of ' + html + ' see ' + content.innerHTML) });
    request.opts.onComplete();
  }

  return { context, requests, events, log, content, advance, complete };
}

test('without a remembered call the lock is released on completion as before', () => {
  const ui = makeWebUI();
  ui.context.updateContent('/pages/a.htm');
  assert.equal(ui.requests.length, 1);
  assert.equal(ui.context.bUpdateContentRunning, true);
  assert.equal(ui.content.innerHTML, 'LOADING');
  ui.complete(0, 'A');
  assert.equal(ui.context.bUpdateContentRunning, false);
  assert.deepEqual(ui.events, []);
  ui.advance(100);
  assert.deepEqual(ui.events, ['ContentLoaded']);
  assert.equal(ui.requests.length, 1);
});

test('a call during a page load is carried out after the page is complete', () => {
  const ui = makeWebUI();
  ui.context.updateContent('/pages/a.htm', '&x=1');
  ui.context.updateContent('/pages/b.htm', '&y=2');
  assert.equal(ui.requests.length, 1, 'the second call must wait');
  ui.complete(0, 'A');
  assert.equal(ui.context.bUpdateContentRunning, true, 'the lock is kept for the remembered call');
  ui.advance(99);
  assert.equal(ui.requests.length, 1);
  ui.advance(1);
  assert.deepEqual(ui.events, ['ContentLoaded']);
  assert.equal(ui.requests.length, 2);
  assert.equal(ui.requests[1].url, '/pages/b.htm?sid=SID&y=2');
  assert.equal(ui.context.sActPage, '/pages/b.htm');
  assert.equal(ui.context.sPreviousPage, '/pages/a.htm');
  // the scripts of page A ran on page A, before B replaced it
  assert.ok(ui.log.includes('scripts of A see A'), ui.log.join('\n'));
  ui.complete(1, 'B');
  assert.equal(ui.context.bUpdateContentRunning, false);
});

test('only the last call during a page load is carried out', () => {
  const ui = makeWebUI();
  ui.context.updateContent('/pages/a.htm');
  ui.context.updateContent('/pages/b.htm');
  ui.context.updateContent('/pages/c.htm');
  ui.complete(0, 'A');
  ui.advance(100);
  assert.deepEqual(ui.requests.map((r) => r.url), ['/pages/a.htm?sid=SID', '/pages/c.htm?sid=SID']);
});

test('calls before a remembered call is carried out keep their order', () => {
  const ui = makeWebUI();
  ui.context.updateContent('/pages/a.htm');
  ui.context.updateContent('/pages/b.htm');
  ui.complete(0, 'A');
  // a page script of A or a click shortly after A loaded
  ui.advance(50);
  ui.context.updateContent('/pages/c.htm');
  assert.equal(ui.requests.length, 1);
  ui.advance(50);
  assert.deepEqual(ui.requests.map((r) => r.url), ['/pages/a.htm?sid=SID', '/pages/c.htm?sid=SID']);
});

test('two saves of the system variable list end with a reload after the last one', () => {
  const ui = makeWebUI();
  ui.context.updateContent('/pages/tabs/admin/systemvars.htm');
  ui.complete(0, 'list');
  ui.advance(100);
  // the first save completes: reload; the second save completes while the
  // reload is loading: its reload is carried out afterwards
  ui.context.reloadPage();
  ui.context.reloadPage();
  assert.equal(ui.requests.length, 2);
  ui.complete(1, 'list after save 1');
  ui.advance(100);
  assert.equal(ui.requests.length, 3);
  assert.equal(ui.requests[2].url, '/pages/tabs/admin/systemvars.htm?sid=SID');
  ui.complete(2, 'list after save 2');
  assert.equal(ui.content.innerHTML, 'list after save 2');
});

test('a remembered call keeps its temporary view setting', () => {
  const ui = makeWebUI();
  ui.context.updateContent('/pages/a.htm');
  ui.context.reloadSortedPage();
  // something else sets the global before the remembered call is carried out
  ui.context.doClearTempView = true;
  ui.complete(0, 'A');
  ui.advance(100);
  assert.match(ui.requests[1].opts.postBody, /sessionCTV", "false"/);

  const ui2 = makeWebUI();
  ui2.context.updateContent('/pages/a.htm');
  ui2.context.reloadPage();
  ui2.context.doClearTempView = false;
  ui2.complete(0, 'A');
  ui2.advance(100);
  assert.match(ui2.requests[1].opts.postBody, /sessionCTV", "true"/);
});

test('an error of a ContentLoaded handler does not block the remembered call', () => {
  const ui = makeWebUI();
  ui.context.failEvent = true;
  ui.context.updateContent('/pages/a.htm');
  ui.context.updateContent('/pages/b.htm');
  ui.complete(0, 'A');
  assert.throws(() => ui.advance(100), /handler/);
  assert.equal(ui.requests.length, 2);
  ui.context.failEvent = false;
  ui.complete(1, 'B');
  assert.equal(ui.context.bUpdateContentRunning, false);
});

test('pages of the device configuration (GET) use the same completion', () => {
  const ui = makeWebUI();
  ui.context.updateContent('/config/devices.cgi');
  ui.context.updateContent('/pages/b.htm');
  assert.equal(ui.requests[0].opts.method, 'get');
  ui.complete(0, 'devices');
  ui.advance(100);
  assert.equal(ui.requests.length, 2);
});

test('a page opened from the menu during a page load is shown afterwards', () => {
  const ui = makeWebUI();
  // WebUI.enter() sets the page before its enter() calls updateContent()
  ui.context.WebUI.currentPage = 'PageA';
  ui.context.updateContent('/pages/a.htm');
  ui.context.WebUI.currentPage = 'PageB';
  ui.context.updateContent('/pages/b.htm');
  ui.complete(0, 'A');
  ui.advance(100);
  assert.deepEqual(ui.requests.map((r) => r.url), ['/pages/a.htm?sid=SID', '/pages/b.htm?sid=SID']);
});

test('a remembered reload is dropped when another page was opened meanwhile', () => {
  const ui = makeWebUI();
  ui.context.WebUI.currentPage = 'SysVarPage';
  ui.context.updateContent('/pages/tabs/admin/systemvars.htm');
  ui.context.reloadPage();
  // another page of the menu that shows its content without updateContent()
  ui.context.WebUI.currentPage = 'OtherPage';
  ui.complete(0, 'list');
  ui.advance(100);
  assert.equal(ui.requests.length, 1);
  assert.equal(ui.context.bUpdateContentRunning, false);
  ui.context.updateContent('/pages/c.htm');
  assert.equal(ui.requests.length, 2);
});

test('the timer of a load without a remembered call does not start a call of a later load', () => {
  const ui = makeWebUI();
  ui.context.updateContent('/pages/a.htm');
  ui.complete(0, 'A');
  // A released the lock; B starts before the timer of A runs, and C is
  // remembered while B is loading
  ui.advance(50);
  ui.context.updateContent('/pages/b.htm');
  ui.context.updateContent('/pages/c.htm');
  ui.advance(50);
  assert.deepEqual(ui.events, ['ContentLoaded']);
  assert.deepEqual(ui.requests.map((r) => r.url), ['/pages/a.htm?sid=SID', '/pages/b.htm?sid=SID'],
    'C must wait for B');
  assert.equal(ui.context.bUpdateContentRunning, true);
  ui.complete(1, 'B');
  ui.advance(100);
  assert.deepEqual(ui.requests.map((r) => r.url), ['/pages/a.htm?sid=SID', '/pages/b.htm?sid=SID', '/pages/c.htm?sid=SID']);
  assert.ok(ui.log.includes('scripts of B see B'), ui.log.join('\n'));
  ui.complete(2, 'C');
  assert.equal(ui.content.innerHTML, 'C');
});

test('a reload of the loading page does not replace a page opened from the menu', () => {
  const ui = makeWebUI();
  ui.context.WebUI.currentPage = 'PageA';
  ui.context.updateContent('/pages/a.htm');
  // the menu opens page B while A is loading
  ui.context.WebUI.currentPage = 'PageB';
  ui.context.updateContent('/pages/b.htm');
  // a delayed callback of page A reloads it (sActPage is still A)
  ui.context.reloadPage();
  ui.complete(0, 'A');
  ui.advance(100);
  assert.deepEqual(ui.requests.map((r) => r.url), ['/pages/a.htm?sid=SID', '/pages/b.htm?sid=SID']);
  ui.complete(1, 'B');
  assert.equal(ui.content.innerHTML, 'B');
});

test('opening the loading page again from the menu keeps it', () => {
  const ui = makeWebUI();
  ui.context.WebUI.currentPage = 'PageA';
  ui.context.updateContent('/pages/a.htm');
  ui.context.WebUI.currentPage = 'PageB';
  ui.context.updateContent('/pages/b.htm');
  // the menu opens page A again: B is not carried out any more
  ui.context.WebUI.currentPage = 'PageA';
  ui.context.updateContent('/pages/a.htm');
  ui.complete(0, 'A');
  ui.advance(100);
  assert.deepEqual(ui.requests.map((r) => r.url), ['/pages/a.htm?sid=SID']);
  assert.equal(ui.content.innerHTML, 'A');
  assert.equal(ui.context.bUpdateContentRunning, false);
});

test('the loading page with other arguments opened from the menu is carried out', () => {
  const ui = makeWebUI();
  // the link list of channel 1 is loading
  ui.context.WebUI.currentPage = 'LinkListPage';
  ui.context.updateContent('/config/ic_linkpeerlist.cgi', '&channel=1');
  ui.context.WebUI.currentPage = 'PageB';
  ui.context.updateContent('/pages/b.htm');
  // the menu opens the link list of channel 2
  ui.context.WebUI.currentPage = 'LinkListPage';
  ui.context.updateContent('/config/ic_linkpeerlist.cgi', '&channel=2');
  ui.complete(0, 'channel 1');
  ui.advance(100);
  assert.deepEqual(ui.requests.map((r) => r.url),
    ['/config/ic_linkpeerlist.cgi?sid=SID&channel=1', '/config/ic_linkpeerlist.cgi?sid=SID&channel=2']);
});

test('the loading page with code to run is carried out after a remembered page', () => {
  const ui = makeWebUI();
  ui.context.WebUI.currentPage = 'NewDevices';
  ui.context.updateContent('/pages/newdevices.htm');
  ui.context.updateContent('/pages/b.htm');
  // e.g. a new sort order of the loading list
  ui.context.updateContent('/pages/newdevices.htm', null, 'system.SetSessionVar("sessionLS", "NAME");');
  ui.complete(0, 'list');
  ui.advance(100);
  assert.equal(ui.requests.length, 2);
  assert.match(ui.requests[1].opts.postBody, /sessionLS/);
});
