/**
 * Sorting of the program logic in the program editor by drag and drop:
 * whole IF / ELSE IF branches, condition groups, conditions and activities.
 * Conditions can also be moved to another condition group and activities to
 * another THEN / ELSE area. The terminal ELSE branch keeps its position. Turning
 * an ELSE IF branch into the ELSE branch keeps the following branches. The CCU
 * changes the editor copy of the program (esp/programs.fn: DnDReorder,
 * DnDReorderBranches, DnDMove, ElseIfToElse).
 **/
(function (win) {
  'use strict';
  var doc = win.document;
  // Never install observers and listeners twice.
  if (win.iseProgramSortInstalled) { return; }
  win.iseProgramSortInstalled = true;
  var ruleSort = {};
  // After a click on OK or Save as new program the editor starts the save 500 ms later and leaves the page
  // when the request ends, so sorting stays locked until then. This is only the way out if no save starts.
  var SAVE_LOCK_MS = 30000;
  // Longest wait for the reload of the rules after a sorting.
  var RELOAD_TIMEOUT_MS = 15000;
  // List owner of an area that does not exist yet: DnDMove creates it (esp/programs.fn).
  var NEW_OWNER = '0';
  // Before a request the saving of an edited field (onchange after the focus left it) must be done:
  // longest wait and poll interval for the open requests of the editor.
  var IDLE_WAIT_MS = 5000;
  var IDLE_POLL_MS = 50;
  // Longest wait for the answer to a sort request. The request is not sent again: ReGa may have done it.
  var REQUEST_TIMEOUT_MS = 30000;
  var scanPending = false;
  // Schedules a scan of the editor, its DOM is replaced asynchronously.
  function schedule() {
    if (scanPending) { return; }
    scanPending = true;
    win.setTimeout(function () { scanPending = false; ruleSort.scan(); }, 50);
  }
  // Translated text.
  function tr(key) { return win.translateKey(key); }
  // Finds one element in the given scope.
  function $(sel, root) { return (root || doc).querySelector(sel); }
  // Collects all matching elements of the given scope.
  function $all(sel, root) { return Array.prototype.slice.call((root || doc).querySelectorAll(sel)); }
  // Creates a DOM element.
  function el(tag, cls, text) {
    var e = doc.createElement(tag);
    if (cls) { e.className = cls; }
    if (text !== undefined) { e.textContent = text; }
    return e;
  }
  // Reads an attribute.
  function attr(node, name) { return (node && node.getAttribute && node.getAttribute(name)) || ''; }

  var S = ruleSort.state = {
    container: null,      
    sortChanged: false,
    busy: false,
    poisoned: false,
    reloads: null,
    awaitingReload: false,
    reloadGeneration: 0,
    saveLocked: false,
    saveTimer: null,
    reloadTimer: null,
    retry: null,
    generation: 0,
    noCondWarn: false,
    result: null,
    reloadFailedFlag: false,
    model: null
  };

  // Resets the state when another program is opened.
  function resetState(container) {
    if(S.branchSizer){S.branchSizer.disconnect();S.branchSizer=null;}
    if(container&&win.ResizeObserver){S.branchSizer=new win.ResizeObserver(schedule);S.branchSizer.observe(container);}
    S.generation++;
    S.container = container;
    S.sortChanged = false;
    S.busy = false;
    S.poisoned = false;
    S.reloads = null;
    S.awaitingReload = false;
    S.model = null;
    S.noCondWarn = false;
    S.result = null;
    S.reloadFailedFlag = false;
    S.saveLocked = false;
    S.retry = null;
    if (S.saveTimer) { win.clearTimeout(S.saveTimer); S.saveTimer = null; }
    disarmReloadTimer();
    removeRetry();
  }

  // Reads the program structure rendered by the original editor.
  function readModel(container) {
    var m = { rules: [], containers: [] };
    var ruleEls = [];
    Array.prototype.forEach.call(container.children, function (c) {
      if (c.nodeType === 1 && /^rule\d+$/.test(c.id || '')) { ruleEls.push(c); }
    });
    ruleEls.forEach(function (re, ri) {
      var rid = re.id.replace('rule', '');
      var addCond = $('img[onclick*="AddCondition("]', re);
      var hasCond = !!addCond;
      var rule = { rid: rid, el: re, index: ri, conditional: hasCond, kind: '', condEl: addCond ? addCond.closest('.Rule') : null };
      if (hasCond) { rule.kind = ri === 0 ? 'if' : 'elseif'; }
      else if (ri > 0) { rule.kind = 'else'; }
      rule.groups = [];
      rule.did = '';
      rule.destEl = null;
      $all('img[onclick*="AddSingleCondition("]', re).forEach(function (img) {
        var mm = /AddSingleCondition\(\s*(\d+)\s*,\s*(\d+)\s*\)/.exec(attr(img, 'onclick'));
        if (!mm) { return; }
        var groupEl = img.parentNode && img.parentNode.parentNode;
        if (!groupEl || groupEl === re) { return; }
        var singles = [];
        $all('span[id^="sico"]', groupEl).forEach(function (sp) {
          if (!/^sico\d+$/.test(sp.id)) { return; }
          singles.push({ id: sp.id.replace('sico', ''), el: sp.parentNode });
        });
        rule.groups.push({ cid: mm[2], el: groupEl, singles: singles });
      });
      var acts = [];
      $all('img[onclick*="AddSingleDestination("]', re).forEach(function (img) {
        var mm = /AddSingleDestination\(\s*(\d+)\s*,\s*(\d+)\s*\)/.exec(attr(img, 'onclick'));
        if (mm && !rule.did) { rule.did = mm[2]; rule.destEl = img.closest('.Rule'); }
      });
      $all('span[id^="side"]', re).forEach(function (sp) {
        if (!/^side\d+$/.test(sp.id)) { return; }
        acts.push({ id: sp.id.replace('side', ''), el: sp.parentNode });
      });
      rule.acts = acts;
      // An empty ELSE part may sit inside the last conditional branch. It is not part of the sortable area.
      var emptyMarker=$('img[onclick*="AddSubRuleDest("]',re);
      rule.emptyElse=emptyMarker&&emptyMarker.closest('.Rule');
      // After ReloadRule the rule sits in a second wrapper div with the same id.
      var tailParent=rule.emptyElse&&rule.emptyElse.parentNode;
      if(tailParent&&tailParent!==re&&!(tailParent.id===re.id&&re.contains(tailParent))){rule.emptyElse=null;}
      m.rules.push(rule);
    });

    var cond = m.rules.filter(function (r) { return r.conditional; });
    var elseRule = null;
    m.rules.forEach(function (r) { if (r.kind === 'else') { elseRule = r; } });
    if (cond.length >= 1) {
      m.containers.push({ type: 'branches', key: 'branches', label: tr('ruleSortLabelBranch'),
        items: cond.map(function (r) { return { id: r.rid, el: r.el }; }),
        rids: cond.map(function (r) { return r.rid; }),
        forbid: (elseRule ? [elseRule.el] : []).concat(m.rules.filter(function(r){return r.emptyElse;}).map(function(r){return r.emptyElse;})) });
    }
    m.rules.forEach(function (r) {
      if (r.groups.length >= 2) {
        m.containers.push({ type: 'groups', key: 'groups' + r.rid, label: tr('ruleSortLabelGroup'),
          rid: r.rid, items: r.groups.map(function (g) { return { id: g.cid, el: g.el }; }), forbid: [] });
      }
      r.groups.forEach(function (g) {
        if (g.singles.length >= 1) {
          m.containers.push({ type: 'conds', key: 'conds' + g.cid, label: tr('ruleSortLabelCond'),
            rid: r.rid, cid: g.cid, items: g.singles, forbid: [] });
        }
      });
      // Empty areas are containers too. An IF / ELSE IF branch without conditions gets a new condition
      // group and the missing ELSE branch is created when an entry is dropped there (owner "0").
      if (r.conditional && !r.groups.length && r.condEl) {
        m.containers.push({ type: 'conds', key: 'newgroup' + r.rid, label: tr('ruleSortLabelCond'),
          rid: r.rid, cid: NEW_OWNER, items: [], forbid: [], area: r.condEl });
      }
      if (r.did) {
        m.containers.push({ type: 'acts', key: 'acts' + r.did, label: tr('ruleSortLabelAct'),
          rid: r.rid, did: r.did, items: r.acts, forbid: [], area: r.destEl });
      }
      if (r.emptyElse) {
        m.containers.push({ type: 'acts', key: 'newelse' + r.rid, label: tr('ruleSortLabelAct'),
          rid: r.rid, did: NEW_OWNER, items: [], forbid: [], area: r.emptyElse });
      }
    });
    return m;
  }
  ruleSort.readModel = readModel;

  // Creates a drag handle.
  function makeHandle(title) {
    var h = el('span', 'RuleSortHandle');
    h.title = title;
    h.setAttribute('data-rulesort', 'handle');
    return h;
  }

  // Marks a sortable entry.
  function decorateItem(item, cont, where) {
    var node = item.el;
    if (!node) { return; }
    if (node.querySelector && node.querySelector(':scope > .RuleSortHandle')) { return; }
    var h = makeHandle(cont.label + tr('ruleSortMoveSuffix'));
    h.setAttribute('data-rulesort-key', cont.key);
    h.setAttribute('data-rulesort-id', item.id);
    if (cont.type === 'groups') {
      if (node.querySelector(':scope > .RuleSortGroupBar')) { return; }
      var groupbar=el('div','RuleSortGroupBar');
      h.classList.add('RuleSortGroupHandle'); h.textContent='\u2195 ' + tr('ruleSortLogicalGroup');
      groupbar.appendChild(h); node.insertBefore(groupbar,node.firstChild);
    } else { node.insertBefore(h, node.firstChild); }
    node.setAttribute('data-rulesort-item', cont.key);
    if(cont.type==='groups'){node.setAttribute('data-rulesort-group',item.id);}
  }

  function branchList(model) { return model.rules.filter(function (r) { return r.kind; }); }
  // An IF / ELSE IF branch without condition counts as true in ReGaHss: the following branches never run.
  function blockingBranch(model) {
    var rules = branchList(model);
    return rules.some(function (r, i) { return r.conditional && !r.groups.length && i < rules.length - 1; });
  }

  // Creates the vertical bar of a program branch.
  function buildBranchBar(rule) {
    var bar=el('div','RuleSortBranchBar'); bar.setAttribute('data-rulesort','branchbar');
    if(rule.conditional){
      var h=makeHandle(tr('ruleSortMoveBranch'));
      h.setAttribute('data-rulesort-key','branches'); h.setAttribute('data-rulesort-id',rule.rid);
      bar.appendChild(h);
    } else if(rule.kind==='else'){
      bar.classList.add('RuleSortBranchBarElse');
      bar.title=tr('ruleSortElseBar');
      var locked=el('span','RuleSortHandle RuleSortLocked');
      locked.title=tr('ruleSortElseAtEnd');
      locked.setAttribute('aria-disabled','true'); if(rule.acts.length){bar.appendChild(locked);}
    }
    var label=el('span','RuleSortBranchLabel',tr({'if':'ruleSortIf','elseif':'ruleSortElseIf','else':'ruleSortElse'}[rule.kind]));
    label.title=label.textContent;
    bar.appendChild(label);
    rule.el.classList.add('RuleSortBranchRail');
    rule.el.insertBefore(bar,rule.el.firstChild);
    return bar;
  }
  // Adds the status bar above the program editor.
  function ensureToolbar(container) {
    var tb = doc.getElementById('RuleSortToolbar');
    if (tb) { return tb; }
    tb = el('div', 'RuleSortToolbar');
    tb.id = 'RuleSortToolbar';
    var st = el('span', 'RuleSortStatus', '');
    st.id = 'RuleSortStatus';
    tb.appendChild(st);
    container.parentNode.insertBefore(tb, container);
    return tb;
  }

  // Notice after a sorting that is not saved yet.
  function sortNotice() { return tr('ruleSortNotice'); }
  // Shows feedback about the sorting.
  function setStatus(msg, bad) {
    var st = doc.getElementById('RuleSortStatus');
    if (!st) { return; }
    msg = msg || (S.sortChanged ? sortNotice() : '');
    // The warning about a branch without condition replaces the notice, not an error or progress text.
    if (S.noCondWarn && (!msg || msg === sortNotice())) { msg = tr('ruleSortWarnNoCondition'); bad = true; }
    st.textContent = msg;
    st.title = msg || '';   
    st.className = 'RuleSortStatus' + (bad ? ' RuleSortBad' : '');
  }
  ruleSort.setStatus = setStatus;

  ruleSort.scan = function () {
    var container = doc.getElementById('prgrulecontent');
    if (!container) {
      if (S.container) { resetState(null); }
      return;
    }
    if (S.container !== container) { resetState(container); }
    ensureToolbar(container);
    if(S.sortChanged && !S.busy && !doc.getElementById('RuleSortStatus').textContent){setStatus(sortNotice());}
    var m = S.model = readModel(container);
    var warn = blockingBranch(m);
    if (warn !== S.noCondWarn) {
      S.noCondWarn = warn;
      if (!S.busy) { setStatus(''); }
    }
    if (S.awaitingReload) {
      // Native ReloadRule calls replace the DOM asynchronously. Wait until all responses finish; after a
      // failed read the retry stays offered and sorting and Save stay blocked.
      if (S.reloadFailedFlag || (S.reloads && Object.keys(S.reloads).length)) { return; }
      S.awaitingReload = false; S.busy = false; S.retry = null;
      disarmReloadTimer();
      removeRetry();
      // The result of the sorting stays visible: the notice for OK, the rejection or warning otherwise.
      if (S.result) { setStatus(S.result.msg, S.result.bad); } else { setStatus(sortNotice()); }
    }

    branchList(m).forEach(function (r) {
      if (!r.el.querySelector(':scope > .RuleSortBranchBar')) { buildBranchBar(r); }
      var bar=r.el.querySelector(':scope > .RuleSortBranchBar'),tailHeight=0;
      if(r.emptyElse){
        r.emptyElse.classList.add('RuleSortEmptyElse');
        tailHeight=Math.max(0,r.el.getBoundingClientRect().bottom-r.emptyElse.getBoundingClientRect().top);
      }
      r.el.style.setProperty('--ruleSortTailHeight',tailHeight+'px');
      bar.style.bottom=(tailHeight+3)+'px';
    });

    m.containers.forEach(function (c) {
      if (c.type === 'branches') { return; }   
      c.items.forEach(function (it) { decorateItem(it, c); });
    });
  };

  ruleSort.movePerm = function (n, from, insertBefore) {
    var order = [], i;
    for (i = 0; i < n; i++) { order.push(i); }
    var item = order.splice(from, 1)[0];
    var to = insertBefore > from ? insertBefore - 1 : insertBefore;
    order.splice(to, 0, item);
    return order;
  };

  // Checks whether the order changes at all.
  function isIdentity(perm) {
    return perm.every(function (v, i) { return v === i; });
  }

  // Watches the reload of the rules after a sorting.
  function armReloadTimer() {
    disarmReloadTimer();
    S.reloadTimer = win.setTimeout(reloadFailed, RELOAD_TIMEOUT_MS);
  }
  function disarmReloadTimer() {
    if (S.reloadTimer) { win.clearTimeout(S.reloadTimer); S.reloadTimer = null; }
  }
  function removeRetry() {
    var r = doc.getElementById('RuleSortRetry');
    if (r && r.parentNode) { r.parentNode.removeChild(r); }
  }
  // The reload failed. The sorting is done on the CCU, so only the reading is offered again.
  function reloadFailed() {
    S.reloadFailedFlag = true;
    disarmReloadTimer();
    var tb = doc.getElementById('RuleSortToolbar');
    if (!tb || !S.retry) { return; }
    setStatus(tr('ruleSortReloadFailed'), true);
    removeRetry();
    var link = el('span', 'RuleSortRetry', tr('ruleSortReloadRetry'));
    link.id = 'RuleSortRetry';
    link.addEventListener('click', function () {
      var again = S.retry;
      removeRetry();
      setStatus(tr('ruleSortReloading'));
      if (again) { again(); }
    });
    tb.appendChild(link);
  }

  // Reloads the affected branches with the original editor.
  // S.reloads counts the open reads per rule: a retry while an older read is still open adds one, every
  // response (also a failed one) takes one away.
  function reloadRules(rids) {
    if(!rids.length){return;}
    if (!S.awaitingReload || !S.reloads) { S.reloads = {}; }
    S.busy=true;S.awaitingReload=true;S.reloadGeneration=S.generation;S.reloadFailedFlag=false;
    rids.forEach(function(rid){S.reloads[rid]=(S.reloads[rid]||0)+1;});
    S.retry = function () { reloadRules(rids); };
    armReloadTimer();
    rids.forEach(function (rid) {
      if (typeof win.ReloadRule === 'function') { win.ReloadRule(rid); }
    });
  }

  // Sends the sort request to the CCU. cb(kind, text): kind is "OK", "ERR" (nothing changed),
  // "POSTFAIL" (written, not confirmed) or "UNKNOWN" (no usable answer, the state of the copy is unclear).
  var SAFE_VALUE = /^[0-9A-Za-z,]*$/;
  function api(action, vars, cb) {
    var pb = 'string action = "' + action + '";';
    var names = Object.keys(vars);
    for (var i = 0; i < names.length; i++) {
      if (!SAFE_VALUE.test(String(vars[names[i]]))) { cb('ERR', 'ERR|invalid'); return; }
      pb += 'string ' + names[i] + ' = "' + vars[names[i]] + '";';
    }
    // Without an answer in time the state of the copy is unclear; a late answer is ignored.
    var done = false;
    var timer = win.setTimeout(function () { done = true; cb('UNKNOWN', 'UNKNOWN|timeout'); }, REQUEST_TIMEOUT_MS);
    new win.Ajax.Request('/esp/programs.htm?sid=' + win.SessionId, {
      postBody: win.ReGa.encode(pb),
      onComplete: function (t) {
        if (done) { return; }
        done = true;
        win.clearTimeout(timer);
        var text = String(t.responseText || '').replace(/^\s+|\s+$/g, '');
        var kind = text.split('|')[0];
        if (t.status !== 200 || (kind !== 'OK' && kind !== 'ERR' && kind !== 'POSTFAIL')) { kind = 'UNKNOWN'; }
        cb(kind, text);
      }
    });
  }

  // Translates the code of an ERR answer.
  function errText(text) {
    var code = text.split('|').slice(1).join('|');
    var key = 'ruleSortErr_' + code;
    var t = tr(key);
    return t !== key ? t : code;
  }

  // Lets the CCU apply the new order to the editor copy.
  function request(cont, perm) {
    if (S.busy || S.poisoned || S.saveLocked) { return; }
    var generation = S.generation;
    var permText = perm.join(',');
    var expected = itemIds(cont);
    var affected, action, vars;
    if (cont.type === 'branches') {
      action = 'DnDReorderBranches';
      vars = { rids: cont.rids.join(','), perm: permText };
      affected = cont.rids.filter(function (r, i) { return perm[i] !== i; });
    } else {
      action = 'DnDReorder';
      vars = { kind: cont.type, rid: cont.rid, expected: expected, perm: permText };
      vars.owner = cont.type === 'groups' ? cont.rid : (cont.type === 'conds' ? cont.cid : cont.did);
      affected = [cont.rid];
    }

    S.busy = true;
    setStatus(tr('ruleSortSaving'));
    api(action, vars, function (kind, text) {
      if (generation !== S.generation) { return; }
      S.busy = false;
      if (kind === 'OK' && cont.type !== 'branches') {
        var previousItems = cont.items;
        cont.items = perm.map(function (index) { return previousItems[index]; });
      }
      finishRequest(kind, text, affected);
    });
  }
  ruleSort.request = request;

  // Shows the answer of the CCU and reloads the affected branches with the original editor.
  // The result is kept in S.result, so that it is shown again when the reload of the branches is done.
  function finishRequest(kind, text, affected) {
    if (kind === 'OK') {
      S.sortChanged = true;
      S.result = { msg: sortNotice(), bad: false };
    } else if (kind === 'POSTFAIL') {
      S.poisoned = true;
      S.result = { msg: tr('ruleSortPostfailStatus'), bad: true };
    } else {
      if (kind === 'UNKNOWN') { S.poisoned = true; }
      S.result = { msg: kind === 'ERR' ? tr('ruleSortNotMoved') + errText(text) : tr('ruleSortErr_unknown'), bad: true };
    }
    reloadRules(affected);
    if (kind === 'POSTFAIL') { win.alert(tr('ruleSortPostfailAlert')); }
    setStatus(S.result.msg, S.result.bad);
  }

  // Returns the IDs of the entries of a container as comma separated list.
  function itemIds(cont) { return cont.items.map(function (it) { return it.id; }).join(','); }
  // Returns the list owner of a container of conditions or activities: the condition group or the destination.
  function ownerOf(cont) { return cont.type === 'conds' ? cont.cid : cont.did; }

  // Lets the CCU move a condition or an activity of cont to position ins of the container target.
  function requestMove(cont, from, target, ins) {
    if (S.busy || S.poisoned || S.saveLocked) { return; }
    var generation = S.generation;
    var vars = { kind: cont.type, rid: cont.rid, owner: ownerOf(cont), trid: target.rid, towner: ownerOf(target),
      item: cont.items[from].id, pos: String(ins), expected: itemIds(cont), texpected: itemIds(target) };
    var affected = cont.rid === target.rid ? [cont.rid] : [cont.rid, target.rid];
    S.busy = true;
    setStatus(tr('ruleSortSaving'));
    api('DnDMove', vars, function (kind, text) {
      if (generation !== S.generation) { return; }
      S.busy = false;
      var fields = text.split('|');
      if (kind === 'OK' && text === 'OK|reload') {
        // A new ELSE branch was created: reload the program like the editor does after AddSubRuleDest().
        win.reloadPage();
        return;
      }
      if (kind === 'OK' && fields.length > 1) {
        // "OK|removed|rule|parent": the ELSE branch lost its last activity and was removed, as when that
        // activity is deleted. Like the editor, drop it from the page and reload its parent rule.
        if (fields.length === 4 && fields[1] === 'removed' && fields[2] === cont.rid && /^\d+$/.test(fields[3])) {
          var removed = doc.getElementById('rule' + fields[2]);
          if (removed && removed.parentNode) { removed.parentNode.removeChild(removed); }
          affected = affected.filter(function (rid) { return rid !== fields[2]; });
          if (affected.indexOf(fields[3]) < 0) { affected.push(fields[3]); }
        } else {
          kind = 'UNKNOWN';
        }
      }
      finishRequest(kind, text, affected);
    });
  }
  ruleSort.requestMove = requestMove;

  var D = ruleSort.drag = { active: false, pending: null, ghost: null, cont: null, target: null, from: -1, insert: -1, valid: false };

  // Removes the marks of a previous drag.
  function clearMarks() {
    $all('.RuleSortDropBefore, .RuleSortDropAfter, .RuleSortDropInvalid').forEach(function (n) {
      n.classList.remove('RuleSortDropBefore'); n.classList.remove('RuleSortDropAfter'); n.classList.remove('RuleSortDropInvalid');
    });
  }

  // Finds the sortable area of a handle.
  function containerFor(handle) {
    if (!S.model) { return null; }
    var key = attr(handle, 'data-rulesort-key');
    for (var i = 0; i < S.model.containers.length; i++) {
      if (S.model.containers[i].key === key) { return S.model.containers[i]; }
    }
    return null;
  }

  // Returns the drop rectangle of an entry.
  function rectOf(n) {
    var r=n.getBoundingClientRect(),tail=n.querySelector('.RuleSortEmptyElse');
    if(!tail){return r;}
    var bottom=Math.min(r.bottom,tail.getBoundingClientRect().top);
    return {top:r.top,bottom:bottom,left:r.left,right:r.right,width:r.width,height:bottom-r.top};
  }

  D.start = function (handle, x, y) {
    if (S.busy || S.poisoned || S.saveLocked || handle.classList.contains('RuleSortLocked')) { return false; }
    var cont = containerFor(handle);
    // A single entry can only be dragged when it can go to another container of its kind.
    if (!cont || !cont.items.length) { return false; }
    if (cont.items.length < 2 && !(movable(cont) && S.model.containers.some(function (c) { return c.type === cont.type && c !== cont; }))) { return false; }
    var id = attr(handle, 'data-rulesort-id');
    var from = -1;
    cont.items.forEach(function (it, i) { if (it.id === id) { from = i; } });
    if (from < 0) { return false; }
    D.pending = { cont: cont, from: from, x: x, y: y, handle: handle };
    // The handle keeps the focus where it is (preventDefault). An edited field must lose it, so that the
    // editor saves its value (onchange) before the sorting reloads the branch.
    var active = doc.activeElement;
    if (active && active !== doc.body && S.container.contains(active) && typeof active.blur === 'function') { active.blur(); }
    return true;
  };

  D.move = function (x, y) {
    if (!D.pending && !D.active) { return; }
    if (!D.active) {
      if (Math.abs(x - D.pending.x) + Math.abs(y - D.pending.y) < 6) { return; }   
      D.active = true;
      D.cont = D.pending.cont;
      D.from = D.pending.from;
      D.ghost = el('div', 'RuleSortGhost', D.cont.label);
      doc.body.appendChild(D.ghost);
      D.cont.items[D.from].el.classList.add('RuleSortDragging');
      doc.body.classList.add('RuleSortDragMode');
    }
    D.ghost.style.left = (x + 12) + 'px';
    D.ghost.style.top = (y + 12) + 'px';
    clearMarks();
    D.valid = false;
    D.insert = -1;
    D.target = null;
    var cont = D.cont, items = cont.items, i, r;

    for (i = 0; i < cont.forbid.length; i++) {
      r = rectOf(cont.forbid[i]);
      if (y >= r.top && y <= r.bottom) {
        cont.forbid[i].classList.add('RuleSortDropInvalid');
        D.ghost.className = 'RuleSortGhost RuleSortGhostBad';
        D.ghost.textContent = tr('ruleSortElseAtEndShort');
        return;
      }
    }
    D.ghost.className = 'RuleSortGhost';
    D.ghost.textContent = cont.label;
    var sameKind = S.model.containers.filter(function (c) { return c.type === cont.type && c.key !== cont.key; });
    for (i = 0; i < sameKind.length; i++) {
      var span = spanOf(sameKind[i]);
      if (span && y >= span.top && y <= span.bottom && x >= span.left && x <= span.right) {
        if (!movable(cont)) {
          sameKind[i].items.forEach(function (it) { it.el.classList.add('RuleSortDropInvalid'); });
          D.ghost.className = 'RuleSortGhost RuleSortGhostBad';
          D.ghost.textContent = tr('ruleSortStayGroup');
          return;
        }
        // A condition or an activity goes to another condition group or THEN / ELSE area.
        D.target = sameKind[i];
        D.insert = insertIndex(sameKind[i].items, y);
        D.valid = true;
        markInsert(sameKind[i], D.insert);
        return;
      }
    }

    var ins = insertIndex(items, y);
    var firstR = rectOf(items[0].el), lastR = rectOf(items[items.length - 1].el);
    var margin = 40;
    if (y < firstR.top - margin || y > lastR.bottom + margin ||
        x < Math.min(firstR.left, lastR.left) - margin || x > Math.max(firstR.right, lastR.right) + margin) { return; }
    D.insert = ins;
    D.valid = !(ins === D.from || ins === D.from + 1);   
    // The positions right above and below the entry do not change anything.
    if (D.valid) { markInsert(cont, ins); }
  };

  // Conditions and activities can leave their container, condition groups and branches cannot.
  function movable(cont) { return cont.type === 'conds' || cont.type === 'acts'; }
  // Returns the area of a container: its entries, or the empty area of the branch.
  function spanOf(cont) {
    if (cont.items.length) {
      var first = rectOf(cont.items[0].el), last = rectOf(cont.items[cont.items.length - 1].el);
      return { top: first.top, bottom: last.bottom,
        left: Math.min(first.left, last.left), right: Math.max(first.right, last.right) };
    }
    return cont.area ? cont.area.getBoundingClientRect() : null;
  }
  // Returns the position in front of which an entry dropped at y is inserted.
  function insertIndex(items, y) {
    for (var i = 0; i < items.length; i++) {
      var r = rectOf(items[i].el);
      if (y < r.top + r.height / 2) { return i; }
    }
    return items.length;
  }
  // Marks the drop position ins in a container, in an empty one below its title line.
  function markInsert(cont, ins) {
    var items = cont.items;
    if (!items.length) {
      if (cont.area) { (cont.area.querySelector('.CLASS02300') || cont.area).classList.add('RuleSortDropAfter'); }
      return;
    }
    if (ins < items.length) { items[ins].el.classList.add('RuleSortDropBefore'); }
    else { items[items.length - 1].el.classList.add('RuleSortDropAfter'); }
  }

  D.cancel = function () {
    clearMarks();
    if (D.ghost && D.ghost.parentNode) { D.ghost.parentNode.removeChild(D.ghost); }
    if (D.cont && D.from >= 0 && D.cont.items[D.from]) { D.cont.items[D.from].el.classList.remove('RuleSortDragging'); }
    doc.body.classList.remove('RuleSortDragMode');
    D.active = false; D.pending = null; D.ghost = null; D.cont = null; D.target = null; D.from = -1; D.insert = -1; D.valid = false;
  };

  D.end = function (x, y) {
    var wasActive = D.active;
    var cont = D.cont, target = D.target, from = D.from, ins = D.insert, valid = D.valid;
    D.cancel();
    if (!wasActive || !valid || ins < 0) { return false; }
    if (target) {
      whenIdle(function () { requestMove(cont, from, target, ins); }, function () {});
      return true;
    }
    var perm = ruleSort.movePerm(cont.items.length, from, ins);
    if (isIdentity(perm)) { return false; }
    whenIdle(function () { request(cont, perm); }, function () {});
    return true;
  };

  // Calls fn when the open requests of the editor are done. Sorting stays locked meanwhile. When they are
  // still open after IDLE_WAIT_MS (e.g. the saving of a field), nothing is sent: onCancel runs and the user
  // is asked to try again. Both are dropped when another program was opened.
  function whenIdle(fn, onCancel) {
    var generation = S.generation, waited = 0;
    S.busy = true;
    (function poll() {
      if (generation !== S.generation) { return; }
      var open = win.Ajax && win.Ajax.activeRequestCount > 0;
      if (open && waited < IDLE_WAIT_MS) { waited += IDLE_POLL_MS; win.setTimeout(poll, IDLE_POLL_MS); return; }
      S.busy = false;
      if (open) {
        onCancel();
        setStatus(tr('ruleSortFieldPending'), true);
        return;
      }
      fn();
    })();
  }

  doc.addEventListener('pointerdown', function (e) {
    if (e.button !== undefined && e.button !== 0) { return; }
    var h = e.target && e.target.closest && e.target.closest('.RuleSortHandle');
    if (!h) { return; }
    if (D.start(h, e.clientX, e.clientY)) { e.preventDefault(); }
  }, true);
  doc.addEventListener('pointermove', function (e) {
    if (D.pending || D.active) { D.move(e.clientX, e.clientY); if (D.active) { e.preventDefault(); } }
  }, true);
  doc.addEventListener('pointerup', function (e) {
    if (D.active) { D.end(e.clientX, e.clientY); e.preventDefault(); }
    else { D.pending = null; }
  }, true);
  doc.addEventListener('pointercancel', function () { D.cancel(); }, true);
  doc.addEventListener('keydown', function (e) { if (e.key === 'Escape' && (D.active || D.pending)) { D.cancel(); } }, true);

  // Blocks saving while a sorting is running or unconfirmed.
  doc.addEventListener('click', function (e) {
    var button = e.target && e.target.closest && e.target.closest('[onclick*="SaveProgramEdit("], [onclick*="CopyToNewProgram("]');
    if (button && S.container && (S.busy || S.poisoned)) {
      e.preventDefault();
      e.stopImmediatePropagation();
      setStatus(S.poisoned ? tr('ruleSortSaveBlockedUnconfirmed') : tr('ruleSortSaveBlockedBusy'), true);
    } else if (button && S.container) {
      // The editor saves 500 ms later, a sorting must not start in between.
      S.saveLocked = true;
      if (S.saveTimer) { win.clearTimeout(S.saveTimer); }
      S.saveTimer = win.setTimeout(function () { S.saveLocked = false; S.saveTimer = null; }, SAVE_LOCK_MS);
    }
  }, true);

  // The selection "Else..." of an ELSE IF branch. DeleteSubRule() of the editor deletes the branch with its
  // activities and all following branches; instead the CCU turns the branch into the ELSE branch and keeps
  // the following branches (ElseIfToElse in esp/programs.fn). Runs before the onchange of the selection.
  doc.addEventListener('change', function (e) {
    var sel = e.target;
    if (!sel || sel.tagName !== 'SELECT' || !S.container || !S.container.contains(sel)) { return; }
    var mm = /DeleteSubRuleSelectChange\(\s*this\.selectedIndex\s*,\s*(\d+)\s*\)/.exec(attr(sel, 'onchange') || '');
    if (!mm || sel.selectedIndex !== 0) { return; }
    e.stopImmediatePropagation();
    if (S.busy || S.poisoned || S.saveLocked) {
      sel.selectedIndex = 1;
      setStatus(S.poisoned ? tr('ruleSortSaveBlockedUnconfirmed') : tr('ruleSortSaveBlockedBusy'), true);
      return;
    }
    whenIdle(function () {
      var generation = S.generation;
      S.busy = true;
      setStatus(tr('ruleSortSaving'));
      api('ElseIfToElse', { rid: mm[1] }, function (kind, text) {
        if (generation !== S.generation) { return; }
        S.busy = false;
        // The chain of branches changed: reload the program like the editor does after DeleteSubRule().
        if (kind === 'OK') { win.reloadPage(); return; }
        sel.selectedIndex = 1;
        finishRequest(kind, text, []);
      });
    }, function () { sel.selectedIndex = 1; });
  }, true);

  if (win.Ajax && win.Ajax.Responders) {
    win.Ajax.Responders.register({ onComplete: function (request) {
      if (String(request.url || '').indexOf('/esp/programs.htm') < 0) { return; }
      var body = String(request.options && request.options.postBody || '');
      try { body = win.unescape(body); } catch (ignored) {}
      var failed = !request.transport || request.transport.status !== 200;
      if (!S.awaitingReload || S.reloadGeneration !== S.generation) { return; }
      if (body.indexOf('"ReloadRule"') < 0) { return; }
      var match = /string rid\s*=\s*"(\d+)"/.exec(body);
      if (match && S.reloads && S.reloads[match[1]]) {
        S.reloads[match[1]]--;
        if (!S.reloads[match[1]]) { delete S.reloads[match[1]]; }
      }
      if (failed) { reloadFailed(); return; }
      schedule();
    }});
  }

  new win.MutationObserver(schedule).observe(doc.documentElement, { childList: true, subtree: true });
  schedule();
})(window);
