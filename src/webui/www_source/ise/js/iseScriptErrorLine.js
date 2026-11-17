/**
 * Locate the first ReGa syntax error in the editor that submitted the script.
 * SyntaxCheck returns line numbers starting at one; CodeMirror 6 uses document
 * positions starting at zero. Ignore replies for text edited in the meantime.
 */
iseScriptErrorLine = {
  focus: function(editor, submittedCode, responseText) {
    if (!editor || editor.getValue() !== submittedCode) { return; }
    var error = /\bError\s+\d+\s+at\s+row\s+([1-9]\d*)\s+col\s+(\d+)/i.exec(responseText || '');
    if (!error) { return; }
    var row = Number(error[1]);
    var column = Number(error[2]);
    if (editor.view && editor.view.dom && editor.view.dom.isConnected &&
        editor.view.state && row <= editor.view.state.doc.lines) {
      var line = editor.view.state.doc.line(row);
      var position = Math.min(line.from + Math.max(0, column - 1), line.to);
      editor.view.dispatch({selection: {anchor: position}, scrollIntoView: true});
      editor.view.focus();
    } else if (typeof editor.lineCount === 'function' && row <= editor.lineCount()) {
      var point = {line: row - 1, ch: Math.max(0, column - 1)};
      editor.setCursor(point);
      editor.scrollIntoView(point, 60);
      editor.focus();
    }
  }
};
