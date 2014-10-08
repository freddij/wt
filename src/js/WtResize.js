/*
 * Copyright (C) 2010 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

/* Note: this is at the same time valid JavaScript and C++. */

WT_DECLARE_WT_MEMBER
(10, JavaScriptFunction, "ChildrenResize",
 function(self, w, h) {
  var WT = this;

  self.style.height = h + 'px';

  if (WT.boxSizing(self)) {
    h -= WT.px(self, 'marginTop');
    h -= WT.px(self, 'marginBottom');
    h -= WT.px(self, 'borderTopWidth');
    h -= WT.px(self, 'borderBottomWidth');
    h -= WT.px(self, 'paddingTop');
    h -= WT.px(self, 'paddingBottom');

    w -= WT.px(self, 'marginLeft');
    w -= WT.px(self, 'marginRight');
    w -= WT.px(self, 'borderLeftWidth');
    w -= WT.px(self, 'borderRightWidth');
    w -= WT.px(self, 'paddingLeft');
    w -= WT.px(self, 'paddingRight');
  }

  function marginV(el) {
    var result = WT.px(el, 'marginTop');
    result += WT.px(el, 'marginBottom');

    if (!WT.boxSizing(el)) {
      result += WT.px(el, 'borderTopWidth');
      result += WT.px(el, 'borderBottomWidth');
      result += WT.px(el, 'paddingTop');
      result += WT.px(el, 'paddingBottom');
    }

    return result;
  }

  var j, jl, c;
  for (j = 0, jl = self.childNodes.length; j < jl; ++j) {
    c = self.childNodes[j];

    if (c.nodeType == 1) {
      var ch = h - marginV(c);

      if (ch > 0) {
	if (c.wtResize)
	  c.wtResize(c, w, ch);
	else {
	  var cheight = ch + 'px';
	  if (c.style.height != cheight)
	    c.style.height = cheight;
	}
      }
    }
  }
 }
 );

WT_DECLARE_WT_MEMBER
(11, JavaScriptFunction, "ChildrenGetPS",
 function(self, child, dir, size) {
   return size;
 }
 );

WT_DECLARE_WT_MEMBER
(12, JavaScriptFunction, "LastResize",
 function(self, w, h) {
  var WT = this;
  self.style.height= h + 'px';
  var t = self.lastChild;
  var c = t.previousSibling;
  h -= c.offsetHeight + WT.px(c, 'marginTop') + WT.px(c, 'marginBottom');
  if (h > 0) {
    if (t.wtResize)
      t.wtResize(t, w, h);
    else
      t.style.height = h + 'px';
  }
 }
 );

WT_DECLARE_WT_MEMBER
(13, JavaScriptFunction, "LastGetPS",
 function(self, child, dir, size) {
  var WT = this, i, il;
  for (i = 0, il = self.childNodes.length; i < il; ++i) {
    var c = self.childNodes[i];
    if (c != child) {
      var pc = WT.css(c, 'position');
      if (pc != 'absolute' && pc != 'fixed') {
	if (dir === 0) {
	  size = Math.max(size, c.offsetWidth);
	} else {
	  size += c.offsetHeight + WT.px(c, 'marginTop')
	    + WT.px(c, 'marginBottom');
	}
      }
    }
  }

  return size;
 }
 );
