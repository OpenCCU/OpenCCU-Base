/*
 * Enhanced WebUI Basis: make the expired-session response readable when it
 * replaces a page or the content of the existing WebUI shell.
 */
(function () {
  "use strict";

  var pending = false;

  function isExpiredMessage(element) {
    return /^Sitzung ung\u00fcltig oder abgelaufen!\s*Weiter$/.test(
      element.textContent.replace(/\s+/g, " ").trim()
    );
  }

  function updateSessionMessage() {
    var links = document.querySelectorAll('a[href="/index.htm"]');
    Array.prototype.forEach.call(links, function (link) {
      if (link.closest(".session-message-card")) { return; }
      var host = link.parentElement;
      while (host) {
        if (isExpiredMessage(host) && host.childElementCount <= 3 &&
            !host.querySelector("input, textarea, button")) {
          var content = host.closest("#content");
          if (content && isExpiredMessage(content) &&
              !content.querySelector("input, textarea, button")) {
            host = content;
          }
          var card = document.createElement("section");
          card.className = "session-message-card";
          card.setAttribute("role", "alert");
          while (host.firstChild) { card.appendChild(host.firstChild); }
          host.appendChild(card);
          host.classList.add("session-message-surface");
          link.classList.add("session-message-continue");
          return;
        }
        host = host.parentElement;
      }
    });
    Array.prototype.forEach.call(
      document.querySelectorAll(".session-message-surface"),
      function (host) {
        if (!host.querySelector(":scope > .session-message-card")) {
          host.classList.remove("session-message-surface");
        }
      }
    );
  }

  function scheduleUpdate() {
    if (pending) { return; }
    pending = true;
    window.setTimeout(function () {
      pending = false;
      updateSessionMessage();
    }, 40);
  }

  function initialize() {
    new MutationObserver(scheduleUpdate).observe(document.documentElement, {
      childList: true,
      subtree: true
    });
    updateSessionMessage();
  }

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", initialize);
  } else {
    initialize();
  }
})();
