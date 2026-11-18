/**
 * Live search for the two native program lists. The list templates call this
 * helper while rendering and keep their existing server-side filters intact.
 */
iseProgramListFilter = {
  /** Describe searchable program cells by meaning, independently of their order. */
  programColumns: function(edit) {
    var definitions = [['name', 'thName'], ['description', 'thDescription']];
    if (edit) { definitions.push(['condition', 'thCondition']); }
    definitions.push(['activity', 'thActivity']);
    if (!edit) { definitions.push(['lastExecution', 'thLastExecution']); }
    return definitions.map(function(definition) {
      return {
        key: definition[0],
        label: translateKey(definition[1]),
        read: function(row) {
          var cell = row.querySelector('[data-search-column="' + definition[0] + '"]');
          return cell ? cell.textContent : '';
        }
      };
    });
  },

  /** Keep live search per tab alongside the persistent native iseFilter criteria. */
  create: function(table, view, showStatus, refresh, columns) {
    var key = 'iseProgramListFilter_' + view;
    var defaults = columns.map(function(column) { return column.key; });
    var saved = {};
    try { saved = JSON.parse(sessionStorage.getItem(key)) || {}; } catch (e) {}
    var selected = Array.isArray(saved.columns) ? saved.columns.map(function(column) {
      // Earlier versions of this toolbar saved column positions.
      return typeof column === 'number' ? defaults[column] : column;
    }) : defaults;
    var state = {
      query: typeof saved.query === 'string' ? saved.query : '',
      columns: defaults.filter(function(column) { return selected.indexOf(column) !== -1; }),
      status: showStatus && /^(active|inactive)$/.test(saved.status) ? saved.status : 'all'
    };
    function searchTerms(query) {
      return query.toLocaleLowerCase().trim().split(/\s+/).filter(Boolean);
    }
    var terms = searchTerms(state.query);
    var toolbar = document.createElement('div');
    toolbar.className = 'programListSearch';
    var search = document.createElement('input');
    search.type = 'search';
    search.value = state.query;
    search.placeholder = translateKey('thFilter');
    search.setAttribute('aria-label', translateKey('thFilter'));
    var count = document.createElement('span');
    count.className = 'programListSearchCount';
    count.setAttribute('role', 'status');
    var choices = document.createElement('details');
    var summary = document.createElement('summary');
    summary.textContent = translateKey('thFilter');
    choices.appendChild(summary);
    columns.forEach(function(column) {
      var label = document.createElement('label');
      var checkbox = document.createElement('input');
      checkbox.type = 'checkbox';
      checkbox.checked = state.columns.indexOf(column.key) !== -1;
      checkbox.onchange = function() {
        if (checkbox.checked) {
          state.columns.push(column.key);
        } else {
          state.columns = state.columns.filter(function(key) { return key !== column.key; });
        }
        changed();
      };
      label.appendChild(checkbox);
      label.appendChild(document.createTextNode(' ' + column.label));
      choices.appendChild(label);
    });
    var status;
    if (showStatus) {
      status = document.createElement('select');
      status.setAttribute('aria-label', translateKey('lblActiv'));
      [['all', 'programFilterAll'], ['active', 'programFilterActive'],
        ['inactive', 'programFilterInactive']].forEach(function(item) {
        var option = document.createElement('option');
        option.value = item[0];
        option.textContent = translateKey(item[1]);
        status.appendChild(option);
      });
      status.value = state.status;
      status.onchange = function() { state.status = status.value; changed(); };
    }
    function changed(update) {
      try { sessionStorage.setItem(key, JSON.stringify(state)); } catch (e) {}
      if (update !== false) { refresh(); }
    }
    search.oninput = function() {
      state.query = search.value;
      terms = searchTerms(state.query);
      changed();
    };
    toolbar.appendChild(search);
    if (status) { toolbar.appendChild(status); }
    toolbar.appendChild(count);
    toolbar.appendChild(choices);
    table.parentNode.insertBefore(toolbar, table);
    var filter = {
      accepts: function(row, item) {
        if (!row) { return false; }
        if (state.status !== 'all' && item.active !== (state.status === 'active')) { return false; }
        if (!terms.length) { return true; }
        var content = columns.filter(function(column) {
          return state.columns.indexOf(column.key) !== -1;
        }).map(function(column) {
          return column.read(row);
        }).join(' ').toLocaleLowerCase();
        return terms.every(function(term) { return content.indexOf(term) !== -1; });
      },
      setCount: function(shown, total) {
        count.textContent = shown + ' / ' + total;
      },
      clear: function(update) {
        state.query = '';
        terms = [];
        state.status = 'all';
        state.columns = defaults.slice();
        search.value = '';
        if (status) { status.value = 'all'; }
        Array.prototype.forEach.call(choices.querySelectorAll('input'), function(input) {
          input.checked = true;
        });
        changed(update);
      }
    };
    return filter;
  }
};
