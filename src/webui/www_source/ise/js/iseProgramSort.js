/**
 * Sorting of the program logic in the program editor by drag and drop:
 * whole IF / ELSE IF branches, condition groups, conditions and activities.
 * The terminal ELSE branch keeps its position. The CCU does the sorting on the
 * editor copy of the program (esp/programs.fn: DnDReorder, DnDReorderBranches).
 **/
(function (win) {
  'use strict';
  var doc = win.document;
  // Never install observers and listeners twice.
  if (win.iseProgramSortInstalled) { return; }
  win.iseProgramSortInstalled = true;
  var ruleSort = {};
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
    origId: '',           
    settled: false,
    settleTimer: null,
    seenRules: {},
    sortChanged: false,
    busy: false,
    poisoned: false,
    loadingRules: null,
    reloads: null,
    awaitingReload: false,
    reloadGeneration: 0,
    copyId: '',
    generation: 0,
    model: null
  };

  // Resets the state when another program is opened.
  function resetState(container) {
    if(S.branchSizer){S.branchSizer.disconnect();S.branchSizer=null;}
    if(container&&win.ResizeObserver){S.branchSizer=new win.ResizeObserver(schedule);S.branchSizer.observe(container);}
    S.generation++;
    S.container = container;
    S.origId = '';
    S.settled = false;
    S.seenRules = {};
    S.sortChanged = false;
    S.busy = false;
    S.poisoned = false;
    S.loadingRules = null;
    S.reloads = null;
    S.awaitingReload = false;
    S.copyId = '';
    S.model = null;
    if (S.settleTimer) { win.clearTimeout(S.settleTimer); S.settleTimer = null; }
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
      var hasCond = !!$('img[onclick*="AddCondition("]', re);
      var rule = { rid: rid, el: re, index: ri, conditional: hasCond, kind: '' };
      if (hasCond) { rule.kind = ri === 0 ? 'if' : 'elseif'; }
      else if (ri > 0) { rule.kind = 'else'; }
      rule.groups = [];
      rule.did = '';
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
        if (mm && !rule.did) { rule.did = mm[2]; }
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
      if (r.acts.length >= 1 && r.did) {
        m.containers.push({ type: 'acts', key: 'acts' + r.did, label: tr('ruleSortLabelAct'),
          rid: r.rid, did: r.did, items: r.acts, forbid: [] });
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
    st.textContent = msg;
    st.title = msg || '';   
    st.className = 'RuleSortStatus' + (bad ? ' RuleSortBad' : '');
    if (msg && !bad && msg !== sortNotice()) {
      win.setTimeout(function () { if (st.isConnected && st.textContent === msg) { setStatus(''); } }, 4000);
    }
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
    if (S.awaitingReload) {
      // Native ReloadRule calls replace the DOM asynchronously. Wait until all responses finish.
      if (S.reloads && Object.keys(S.reloads).length) { return; }
      S.awaitingReload = false; S.busy = false;
    }
    if (S.loadingRules) {
      var actual = m.rules.map(function (r) { return r.rid; }).join(',');
      if (actual !== S.loadingRules.join(',')) { return; }
      S.loadingRules = null;
      S.busy = false;

    }

    if (!S.origId) {
      var okBtn = $('[onclick*="SaveProgramEdit("]');
      var mm = okBtn && /SaveProgramEdit\(\s*(\d+)\s*,\s*(\d+)\s*\)/.exec(attr(okBtn, 'onclick'));
      if (mm) { S.origId = mm[1]; S.copyId = mm[2]; }
    }

    m.rules.forEach(function (r) {
      if (!S.seenRules[r.rid]) {
        S.seenRules[r.rid] = true;
        if (!S.settled) {
          if (S.settleTimer) { win.clearTimeout(S.settleTimer); }
          S.settleTimer = win.setTimeout(function () { S.settled = true; }, 2000);
        }
      }
    });

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

  // Reloads the affected branches with the original editor.
  function reloadRules(rids) {
    if(!rids.length){return;}
    S.busy=true;S.awaitingReload=true;S.reloadGeneration=S.generation;S.reloads={};
    rids.forEach(function(rid){S.reloads[rid]=true;});
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
    new win.Ajax.Request('/esp/programs.htm?sid=' + win.SessionId, {
      postBody: win.ReGa.encode(pb),
      onComplete: function (t) {
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
    if (S.busy || S.poisoned) { return; }
    var generation = S.generation;
    var permText = perm.join(',');
    var expected = cont.items.map(function (it) { return it.id; }).join(',');
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
      if (kind === 'OK') {
        if (cont.type === 'branches') {
          var fields = text.split('|');
          var newIds = (fields[3] || '').split(',');
          if (!/^\d+$/.test(fields[1] || '') || !/^\d+$/.test(fields[2] || '') ||
              newIds.length !== S.model.rules.length || newIds.some(function (id) { return !/^\d+$/.test(id); })) {
            S.poisoned = true;
            setStatus(tr('ruleSortIncomplete'), true);
            return;
          }
          S.settled = true;
          S.loadingRules = newIds;
          S.busy = true;
          S.container.innerHTML = '';
          win.LoadRule(fields[1], fields[2]);
        } else {
          var previousItems = cont.items;
          cont.items = perm.map(function (index) { return previousItems[index]; });
          reloadRules(affected);
        }
        S.sortChanged = true;
        setStatus(sortNotice());
      } else if (kind === 'POSTFAIL') {
        S.poisoned = true;
        reloadRules(affected);
        win.alert(tr('ruleSortPostfailAlert'));
        setStatus(tr('ruleSortPostfailStatus'), true);
      } else {
        if (kind === 'UNKNOWN') { S.poisoned = true; }
        reloadRules(affected);
        setStatus(tr('ruleSortNotMoved') + (kind === 'ERR' ? errText(text) : tr('ruleSortErr_unknown')), true);
      }
    });
  }
  ruleSort.request = request;

  var D = ruleSort.drag = { active: false, pending: null, ghost: null, cont: null, from: -1, insert: -1, valid: false };

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
    if (S.busy || S.poisoned || handle.classList.contains('RuleSortLocked')) { return false; }
    var cont = containerFor(handle);
    if (!cont || cont.items.length < 2) { return false; }
    var id = attr(handle, 'data-rulesort-id');
    var from = -1;
    cont.items.forEach(function (it, i) { if (it.id === id) { from = i; } });
    if (from < 0) { return false; }
    D.pending = { cont: cont, from: from, x: x, y: y, handle: handle };
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
    var sameKind = S.model.containers.filter(function (c) { return c.type === cont.type && c.key !== cont.key; });
    for (i = 0; i < sameKind.length; i++) {
      var first = rectOf(sameKind[i].items[0].el), last = rectOf(sameKind[i].items[sameKind[i].items.length - 1].el);
      if (y >= first.top && y <= last.bottom) {
        sameKind[i].items.forEach(function (it) { it.el.classList.add('RuleSortDropInvalid'); });
        D.ghost.className = 'RuleSortGhost RuleSortGhostBad';
        D.ghost.textContent = cont.type === 'conds' ? tr('ruleSortStayCond') : cont.type === 'acts' ? tr('ruleSortStayAct') : tr('ruleSortStayGroup');
        return;
      }
    }
    D.ghost.className = 'RuleSortGhost';
    D.ghost.textContent = cont.label;

    var ins = items.length;
    for (i = 0; i < items.length; i++) {
      r = rectOf(items[i].el);
      if (y < r.top + r.height / 2) { ins = i; break; }
    }
    var firstR = rectOf(items[0].el), lastR = rectOf(items[items.length - 1].el);
    var margin = 40;
    if (y < firstR.top - margin || y > lastR.bottom + margin ||
        x < Math.min(firstR.left, lastR.left) - margin || x > Math.max(firstR.right, lastR.right) + margin) { return; }
    D.insert = ins;
    D.valid = !(ins === D.from || ins === D.from + 1);   
    if (ins < items.length) { items[ins].el.classList.add('RuleSortDropBefore'); }
    else { items[items.length - 1].el.classList.add('RuleSortDropAfter'); }
  };

  D.cancel = function () {
    clearMarks();
    if (D.ghost && D.ghost.parentNode) { D.ghost.parentNode.removeChild(D.ghost); }
    if (D.cont && D.from >= 0 && D.cont.items[D.from]) { D.cont.items[D.from].el.classList.remove('RuleSortDragging'); }
    doc.body.classList.remove('RuleSortDragMode');
    D.active = false; D.pending = null; D.ghost = null; D.cont = null; D.from = -1; D.insert = -1; D.valid = false;
  };

  D.end = function (x, y) {
    var wasActive = D.active;
    var cont = D.cont, from = D.from, ins = D.insert, valid = D.valid;
    D.cancel();
    if (!wasActive || !valid || ins < 0) { return false; }
    var perm = ruleSort.movePerm(cont.items.length, from, ins);
    if (isIdentity(perm)) { return false; }
    request(cont, perm);
    return true;
  };

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
    }
  }, true);

  if (win.Ajax && win.Ajax.Responders) {
    win.Ajax.Responders.register({ onComplete: function (request) {
      if (!S.awaitingReload || S.reloadGeneration !== S.generation ||
          String(request.url || '').indexOf('/esp/programs.htm') < 0) { return; }
      var body = String(request.options && request.options.postBody || '');
      try { body = win.unescape(body); } catch (ignored) {}
      if (body.indexOf('"ReloadRule"') < 0) { return; }
      var match = /string rid\s*=\s*"(\d+)"/.exec(body);
      if (match && S.reloads) { delete S.reloads[match[1]]; schedule(); }
    }});
  }

  new win.MutationObserver(schedule).observe(doc.documentElement, { childList: true, subtree: true });
  schedule();
})(window);
