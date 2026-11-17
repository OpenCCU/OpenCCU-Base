/**
 * Live search for the two native program lists. The list templates call this
 * helper while rendering and keep their existing server-side filters intact.
 */
iseProgramListFilter = {
  create: function(table, view, showStatus, refresh) {
    var key = 'iseProgramListFilter_' + view;
    var saved = {};
    try { saved = JSON.parse(sessionStorage.getItem(key)) || {}; } catch (e) {}
    var state = {
      query: typeof saved.query === 'string' ? saved.query : '',
      columns: Array.isArray(saved.columns) ? saved.columns : [0, 1, 2, 3],
      status: showStatus && /^(active|inactive)$/.test(saved.status) ? saved.status : 'all'
    };
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
    var headers = table.rows[0].cells;
    for (var i = 0; i < Math.min(headers.length - 1, 4); i++) {
      (function(index) {
        var label = document.createElement('label');
        var checkbox = document.createElement('input');
        checkbox.type = 'checkbox';
        checkbox.checked = state.columns.indexOf(index) !== -1;
        checkbox.onchange = function() {
          if (checkbox.checked) {
            state.columns.push(index);
          } else {
            state.columns = state.columns.filter(function(column) { return column !== index; });
          }
          changed();
        };
        label.appendChild(checkbox);
        label.appendChild(document.createTextNode(' ' + headers[index].textContent.trim()));
        choices.appendChild(label);
      })(i);
    }
    var status;
    if (showStatus) {
      status = document.createElement('select');
      status.setAttribute('aria-label', translateKey('lblActiv'));
      var german = translateKey('submenuPrograms') === 'Programme';
      (german ? [['all', 'Alle Programme'], ['active', 'Nur aktive Programme'],
        ['inactive', 'Nur deaktivierte Programme']] :
        [['all', 'All programs'], ['active', 'Active programs'],
         ['inactive', 'Inactive programs']]).forEach(function(item) {
        var option = document.createElement('option');
        option.value = item[0];
        option.textContent = item[1];
        status.appendChild(option);
      });
      status.value = state.status;
      status.onchange = function() { state.status = status.value; changed(); };
    }
    function changed() {
      try { sessionStorage.setItem(key, JSON.stringify(state)); } catch (e) {}
      refresh();
    }
    search.oninput = function() { state.query = search.value; changed(); };
    toolbar.appendChild(search);
    if (status) { toolbar.appendChild(status); }
    toolbar.appendChild(count);
    toolbar.appendChild(choices);
    table.parentNode.insertBefore(toolbar, table);
    table.addEventListener('change', function(event) {
      if (event.target.matches('input[onclick*="SetActive("]')) { refresh(); }
    });
    var filter = {
      accepts: function(row) {
        if (!row) { return false; }
        if (state.status !== 'all') {
          var active = row.querySelector('input[onclick*="SetActive("]');
          if (!active || active.checked !== (state.status === 'active')) { return false; }
        }
        var terms = state.query.toLocaleLowerCase().trim().split(/\s+/).filter(Boolean);
        if (!terms.length) { return true; }
        var content = state.columns.map(function(index) {
          return row.cells[index] ? row.cells[index].textContent : '';
        }).join(' ').toLocaleLowerCase();
        return terms.every(function(term) { return content.indexOf(term) !== -1; });
      },
      updateCount: function() {
        var rows = Array.prototype.filter.call(table.rows, function(row) {
          return /^(program|pr)\d+$/.test(row.id);
        });
        var shown = rows.filter(function(row) { return row.style.display !== 'none'; }).length;
        count.textContent = shown + ' / ' + rows.length;
      },
      clear: function() {
        state.query = '';
        state.status = 'all';
        state.columns = [0, 1, 2, 3];
        search.value = '';
        if (status) { status.value = 'all'; }
        Array.prototype.forEach.call(choices.querySelectorAll('input'), function(input) {
          input.checked = true;
        });
        changed();
      }
    };
    return filter;
  }
};
