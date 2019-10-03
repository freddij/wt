/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#include "Wt/WInteractWidget"
#include "Wt/WApplication"
#include "Wt/WEnvironment"
#include "Wt/WFormWidget"
#include "Wt/WPopupWidget"
#include "Wt/WServer"
#include "Wt/WTheme"

#include "Configuration.h"
#include "DomElement.h"

/*
 * FIXME: provide a cross-browser mechanism to "cancel" the default
 * action for a keyboard event (and other events!).
 * Note that for key-up/key-down events, you will need to do something with
 * the key-press event, as per http://unixpapa.com/js/key.html
 */

namespace Wt {

const char *WInteractWidget::KEYDOWN_SIGNAL = "M_keydown";
const char *WInteractWidget::KEYPRESS_SIGNAL = "keypress";
const char *WInteractWidget::KEYUP_SIGNAL = "keyup";
const char *WInteractWidget::ENTER_PRESS_SIGNAL = "M_enterpress";
const char *WInteractWidget::ESCAPE_PRESS_SIGNAL = "M_escapepress";
#ifdef EVENT_TONY
/* ksystem start 1 */
const char *WInteractWidget::SPACEBAR_PRESS_SIGNAL = "M_spacebarpress";
const char *WInteractWidget::TAB_PRESS_SIGNAL = "M_tabpress";
const char *WInteractWidget::SHIFTTAB_PRESS_SIGNAL = "M_shifttabpress";
const char *WInteractWidget::ALT1_PRESS_SIGNAL = "M_alt1press";
const char *WInteractWidget::ALT2_PRESS_SIGNAL = "M_alt2press";
const char *WInteractWidget::ALT3_PRESS_SIGNAL = "M_alt3press";
const char *WInteractWidget::ALT4_PRESS_SIGNAL = "M_alt4press";
const char *WInteractWidget::ALT5_PRESS_SIGNAL = "M_alt5press";
const char *WInteractWidget::ALT6_PRESS_SIGNAL = "M_alt6press";
const char *WInteractWidget::ALT7_PRESS_SIGNAL = "M_alt7press";
const char *WInteractWidget::ALT8_PRESS_SIGNAL = "M_alt8press";
const char *WInteractWidget::ALT9_PRESS_SIGNAL = "M_alt9press";
const char *WInteractWidget::ALTa_PRESS_SIGNAL = "M_altapress";
const char *WInteractWidget::ALTb_PRESS_SIGNAL = "M_altbpress";
const char *WInteractWidget::ALTc_PRESS_SIGNAL = "M_altcpress";
const char *WInteractWidget::ALTd_PRESS_SIGNAL = "M_altdpress";
const char *WInteractWidget::UP_PRESS_SIGNAL = "M_uppress";
const char *WInteractWidget::DOWN_PRESS_SIGNAL = "M_downpress";
const char *WInteractWidget::PAGEUP_PRESS_SIGNAL = "M_pageuppress";
const char *WInteractWidget::PAGEDOWN_PRESS_SIGNAL = "M_pagedownpress";
const char *WInteractWidget::F1_PRESS_SIGNAL = "M_f1press";
const char *WInteractWidget::F2_PRESS_SIGNAL = "M_f2press";
const char *WInteractWidget::F3_PRESS_SIGNAL = "M_f3press";
const char *WInteractWidget::F4_PRESS_SIGNAL = "M_f4press";
const char *WInteractWidget::F5_PRESS_SIGNAL = "M_f5press";
const char *WInteractWidget::F6_PRESS_SIGNAL = "M_f6press";
const char *WInteractWidget::F7_PRESS_SIGNAL = "M_f7press";
const char *WInteractWidget::F8_PRESS_SIGNAL = "M_f8press";
const char *WInteractWidget::F9_PRESS_SIGNAL = "M_f9press";
const char *WInteractWidget::F10_PRESS_SIGNAL = "M_f10press";
const char *WInteractWidget::F11_PRESS_SIGNAL = "M_f11press";
const char *WInteractWidget::F12_PRESS_SIGNAL = "M_f12press";
const char *WInteractWidget::PLUS_PRESS_SIGNAL = "M_pluspress";
const char *WInteractWidget::NUMERICPLUS_PRESS_SIGNAL = "M_numericpluspress";
const char *WInteractWidget::MINUS_PRESS_SIGNAL = "M_minuspress";
const char *WInteractWidget::MULTIPLY_PRESS_SIGNAL = "M_multiplypress";
const char *WInteractWidget::DIVISION_PRESS_SIGNAL = "M_divisionpress";
const char *WInteractWidget::EQUAL_PRESS_SIGNAL = "M_equalpress";
const char *WInteractWidget::PERCENT_PRESS_SIGNAL = "M_percentpress";
/* ksystem end 1*/
#endif
const char *WInteractWidget::CLICK_SIGNAL = "click";
const char *WInteractWidget::M_CLICK_SIGNAL = "M_click";
const char *WInteractWidget::DBL_CLICK_SIGNAL = "M_dblclick";
const char *WInteractWidget::MOUSE_DOWN_SIGNAL = "M_mousedown";
const char *WInteractWidget::MOUSE_UP_SIGNAL = "M_mouseup";
const char *WInteractWidget::MOUSE_OUT_SIGNAL = "M_mouseout";
const char *WInteractWidget::MOUSE_OVER_SIGNAL = "M_mouseover";
const char *WInteractWidget::MOUSE_MOVE_SIGNAL = "M_mousemove";
const char *WInteractWidget::MOUSE_DRAG_SIGNAL = "M_mousedrag";
const char *WInteractWidget::MOUSE_WHEEL_SIGNAL = "mousewheel";
const char *WInteractWidget::WHEEL_SIGNAL = "wheel";
const char *WInteractWidget::TOUCH_START_SIGNAL = "touchstart";
const char *WInteractWidget::TOUCH_MOVE_SIGNAL = "touchmove";
const char *WInteractWidget::TOUCH_END_SIGNAL = "touchend";
const char *WInteractWidget::GESTURE_START_SIGNAL = "gesturestart";
const char *WInteractWidget::GESTURE_CHANGE_SIGNAL = "gesturechange";
const char *WInteractWidget::GESTURE_END_SIGNAL = "gestureend";

WInteractWidget::WInteractWidget(WContainerWidget *parent)
  : WWebWidget(parent),
    dragSlot_(0),
    dragTouchSlot_(0),
    dragTouchEndSlot_(0),
    mouseOverDelay_(0)
{ }

WInteractWidget::~WInteractWidget()
{
  delete dragSlot_;
  delete dragTouchSlot_;
  delete dragTouchEndSlot_;
}

void WInteractWidget::setPopup(bool popup)
{
  if (popup && wApp->environment().ajax()) {
    clicked().connect
      ("function(o,e) { "
       " if (" WT_CLASS ".WPopupWidget && $.data(o,'popup')) {"
           WT_CLASS ".WPopupWidget.popupClicked = o;"
           "$(document).trigger('click', e);"
           WT_CLASS ".WPopupWidget.popupClicked = null;"
       " }"
       "}");
    clicked().preventPropagation();
  }

  WWebWidget::setPopup(popup);
}

EventSignal<WKeyEvent>& WInteractWidget::keyWentDown()
{
  return *keyEventSignal(KEYDOWN_SIGNAL, true);
}

EventSignal<WKeyEvent>& WInteractWidget::keyPressed()
{
  return *keyEventSignal(KEYPRESS_SIGNAL, true);
}

EventSignal<WKeyEvent>& WInteractWidget::keyWentUp()
{
  return *keyEventSignal(KEYUP_SIGNAL, true);
}

EventSignal<>& WInteractWidget::enterPressed()
{
  return *voidEventSignal(ENTER_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::escapePressed()
{
  return *voidEventSignal(ESCAPE_PRESS_SIGNAL, true);
}

#ifndef EVENT_TONY
JSignal<> &Wt::WInteractWidget::signalKeyDown(const std::string &key, const KeyboardModifier modifier, bool preventDefault)
{
  std::string name("kd"); name.append(key).append(std::to_string(modifier));
  JSignal<> *ret = dynamic_cast<JSignal<> *>(getEventSignal(name.data()));
  if (!ret)
  {
    ret = new JSignal<>(this, name);
    addEventSignal(*ret);
    addJSignal(ret);

    std::string modString = "true";
    switch (modifier)
    {
    case ShiftModifier:
      modString = "e.shiftKey";
      break;
    case ControlModifier:
      modString = "e.ctrlKey";
      break;
    case AltModifier:
      modString = "e.altKey";
      break;
    case MetaModifier:
      modString = "e.metaKey";
      break;
    default:
      modString = "true";
    }

    /*new JSlot(WString(WT_JS(function(s,e,k,m,p){
                            if (e.)
                            }))
              , 3, this);*/
    doJavaScript(WString(WT_JS({1}
                       .addEventListener('keydown',function(e){
                                           if (e.key=='{2}' && {3}) {
                                             if ({4}) e.preventDefault();
                                             {5}
                                           };
                                         });))
        .arg(jsRef())
        .arg(key).arg(modString).arg(preventDefault ? "true" : "false")
        .arg(ret->createCall())
        .toUTF8()
        );
  }
  return *ret;
}

JSignal<> &Wt::WInteractWidget::signalBeforeInput(const char data, bool preventDefault)
{
  std::string name("bi"); name.append(0,data);
  JSignal<> *ret = dynamic_cast<JSignal<> *>(getEventSignal(name.data()));
  if (!ret)
  {
    ret = new JSignal<>(this, name);
    addEventSignal(*ret);
    addJSignal(ret);

    doJavaScript(WString(WT_JS({1}
                       .addEventListener('keydown',function(e){
                                           if (e.data=='{2}') {
                                             if ({3}) e.preventDefault();
                                             {4}
                                           };
                                         });))
        .arg(jsRef())
        .arg(data).arg(preventDefault ? "true" : "false")
        .arg(ret->createCall())
        .toUTF8()
        );
  }
  return *ret;
}
#endif

#ifdef EVENT_TONY
/* ksystem start 2 */
EventSignal<>& WInteractWidget::spacebarPressed()
{
  return *voidEventSignal(SPACEBAR_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::tabPressed()
{
  return *voidEventSignal(TAB_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::shifttabPressed()
{
  return *voidEventSignal(SHIFTTAB_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::alt1Pressed()
{
  return *voidEventSignal(ALT1_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::alt2Pressed()
{
  return *voidEventSignal(ALT2_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::alt3Pressed()
{
  return *voidEventSignal(ALT3_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::alt4Pressed()
{
  return *voidEventSignal(ALT4_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::alt5Pressed()
{
  return *voidEventSignal(ALT5_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::alt6Pressed()
{
  return *voidEventSignal(ALT6_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::alt7Pressed()
{
  return *voidEventSignal(ALT7_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::alt8Pressed()
{
  return *voidEventSignal(ALT8_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::alt9Pressed()
{
  return *voidEventSignal(ALT9_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::altaPressed()
{
  return *voidEventSignal(ALTa_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::altbPressed()
{
  return *voidEventSignal(ALTb_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::altcPressed()
{
  return *voidEventSignal(ALTc_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::altdPressed()
{
  return *voidEventSignal(ALTd_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::upPressed()
{
  return *voidEventSignal(UP_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::downPressed()
{
  return *voidEventSignal(DOWN_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::pageUpPressed()
{
  return *voidEventSignal(PAGEUP_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::pageDownPressed()
{
  return *voidEventSignal(PAGEDOWN_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::f1Pressed()
{
  return *voidEventSignal(F1_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::f2Pressed()
{
  return *voidEventSignal(F2_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::f3Pressed()
{
  return *voidEventSignal(F3_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::f4Pressed()
{
  return *voidEventSignal(F4_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::f5Pressed()
{
  return *voidEventSignal(F5_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::f6Pressed()
{
  return *voidEventSignal(F6_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::f7Pressed()
{
  return *voidEventSignal(F7_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::f8Pressed()
{
  return *voidEventSignal(F8_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::f9Pressed()
{
  return *voidEventSignal(F9_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::f10Pressed()
{
  return *voidEventSignal(F10_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::f11Pressed()
{
  return *voidEventSignal(F11_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::f12Pressed()
{
  return *voidEventSignal(F12_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::plusPressed()
{
  return *voidEventSignal(PLUS_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::numericplusPressed()
{
  return *voidEventSignal(NUMERICPLUS_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::minusPressed()
{
  return *voidEventSignal(MINUS_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::multiplyPressed()
{
  return *voidEventSignal(MULTIPLY_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::divisionPressed()
{
  return *voidEventSignal(DIVISION_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::equalPressed()
{
  return *voidEventSignal(EQUAL_PRESS_SIGNAL, true);
}

EventSignal<>& WInteractWidget::percentPressed()
{
  return *voidEventSignal(PERCENT_PRESS_SIGNAL, true);
}
/* ksystem end 2 */
#endif

EventSignal<WMouseEvent>& WInteractWidget::clicked()
{
  return *mouseEventSignal(M_CLICK_SIGNAL, true);
}

EventSignal<WMouseEvent>& WInteractWidget::doubleClicked()
{
  return *mouseEventSignal(DBL_CLICK_SIGNAL, true);
}

EventSignal<WMouseEvent>& WInteractWidget::mouseWentDown()
{
  return *mouseEventSignal(MOUSE_DOWN_SIGNAL, true);
}

EventSignal<WMouseEvent>& WInteractWidget::mouseWentUp()
{
  return *mouseEventSignal(MOUSE_UP_SIGNAL, true);
}

EventSignal<WMouseEvent>& WInteractWidget::mouseWentOut()
{
  return *mouseEventSignal(MOUSE_OUT_SIGNAL, true);
}

EventSignal<WMouseEvent>& WInteractWidget::mouseWentOver()
{
  return *mouseEventSignal(MOUSE_OVER_SIGNAL, true);
}

EventSignal<WMouseEvent>& WInteractWidget::mouseMoved()
{
  return *mouseEventSignal(MOUSE_MOVE_SIGNAL, true);
}

EventSignal<WMouseEvent>& WInteractWidget::mouseDragged()
{
  return *mouseEventSignal(MOUSE_DRAG_SIGNAL, true);
}

EventSignal<WMouseEvent>& WInteractWidget::mouseWheel()
{
  if (WApplication::instance()->environment().agentIsIElt(9) ||
      WApplication::instance()->environment().agent() == WEnvironment::Edge) {
    return *mouseEventSignal(MOUSE_WHEEL_SIGNAL, true);
  } else {
    return *mouseEventSignal(WHEEL_SIGNAL, true);
  }
}

EventSignal<WTouchEvent>& WInteractWidget::touchStarted()
{
  return *touchEventSignal(TOUCH_START_SIGNAL, true);
}

EventSignal<WTouchEvent>& WInteractWidget::touchMoved()
{
  return *touchEventSignal(TOUCH_MOVE_SIGNAL, true);
}

EventSignal<WTouchEvent>& WInteractWidget::touchEnded()
{
  return *touchEventSignal(TOUCH_END_SIGNAL, true);
}

EventSignal<WGestureEvent>& WInteractWidget::gestureStarted()
{
  return *gestureEventSignal(GESTURE_START_SIGNAL, true);
}

EventSignal<WGestureEvent>& WInteractWidget::gestureChanged()
{
  return *gestureEventSignal(GESTURE_CHANGE_SIGNAL, true);
}

EventSignal<WGestureEvent>& WInteractWidget::gestureEnded()
{
  return *gestureEventSignal(GESTURE_END_SIGNAL, true);
}

void WInteractWidget::updateDom(DomElement& element, bool all)
{
  bool updateKeyDown = false;

  WApplication *app = WApplication::instance();

  /*
   * -- combine enterPress, escapePress and keyDown signals
   */
  EventSignal<> *enterPress = voidEventSignal(ENTER_PRESS_SIGNAL, false);
  EventSignal<> *escapePress = voidEventSignal(ESCAPE_PRESS_SIGNAL, false);
#ifdef EVENT_TONY
  /* ksystem start 3 */
  EventSignal<> *spacebarPress = voidEventSignal(SPACEBAR_PRESS_SIGNAL, false);
  EventSignal<> *tabPress = voidEventSignal(TAB_PRESS_SIGNAL, false);
  EventSignal<> *shifttabPress = voidEventSignal(SHIFTTAB_PRESS_SIGNAL, false);
  EventSignal<> *alt1Press = voidEventSignal(ALT1_PRESS_SIGNAL, false);
  EventSignal<> *alt2Press = voidEventSignal(ALT2_PRESS_SIGNAL, false);
  EventSignal<> *alt3Press = voidEventSignal(ALT3_PRESS_SIGNAL, false);
  EventSignal<> *alt4Press = voidEventSignal(ALT4_PRESS_SIGNAL, false);
  EventSignal<> *alt5Press = voidEventSignal(ALT5_PRESS_SIGNAL, false);
  EventSignal<> *alt6Press = voidEventSignal(ALT6_PRESS_SIGNAL, false);
  EventSignal<> *alt7Press = voidEventSignal(ALT7_PRESS_SIGNAL, false);
  EventSignal<> *alt8Press = voidEventSignal(ALT8_PRESS_SIGNAL, false);
  EventSignal<> *alt9Press = voidEventSignal(ALT9_PRESS_SIGNAL, false);
  EventSignal<> *altaPress = voidEventSignal(ALTa_PRESS_SIGNAL, false);
  EventSignal<> *altbPress = voidEventSignal(ALTb_PRESS_SIGNAL, false);
  EventSignal<> *altcPress = voidEventSignal(ALTc_PRESS_SIGNAL, false);
  EventSignal<> *altdPress = voidEventSignal(ALTd_PRESS_SIGNAL, false);
  EventSignal<> *upPress = voidEventSignal(UP_PRESS_SIGNAL, false);
  EventSignal<> *downPress = voidEventSignal(DOWN_PRESS_SIGNAL, false);
  EventSignal<> *pageUpPress = voidEventSignal(PAGEUP_PRESS_SIGNAL, false);
  EventSignal<> *pageDownPress = voidEventSignal(PAGEDOWN_PRESS_SIGNAL, false);
  EventSignal<> *f1Press = voidEventSignal(F1_PRESS_SIGNAL, false);
  EventSignal<> *f2Press = voidEventSignal(F2_PRESS_SIGNAL, false);
  EventSignal<> *f3Press = voidEventSignal(F3_PRESS_SIGNAL, false);
  EventSignal<> *f4Press = voidEventSignal(F4_PRESS_SIGNAL, false);
  EventSignal<> *f5Press = voidEventSignal(F5_PRESS_SIGNAL, false);
  EventSignal<> *f6Press = voidEventSignal(F6_PRESS_SIGNAL, false);
  EventSignal<> *f7Press = voidEventSignal(F7_PRESS_SIGNAL, false);
  EventSignal<> *f8Press = voidEventSignal(F8_PRESS_SIGNAL, false);
  EventSignal<> *f9Press = voidEventSignal(F9_PRESS_SIGNAL, false);
  EventSignal<> *f10Press = voidEventSignal(F10_PRESS_SIGNAL, false);
  EventSignal<> *f11Press = voidEventSignal(F11_PRESS_SIGNAL, false);
  EventSignal<> *f12Press = voidEventSignal(F12_PRESS_SIGNAL, false);
  EventSignal<> *plusPress = voidEventSignal(PLUS_PRESS_SIGNAL, false);
  EventSignal<> *numericplusPress = voidEventSignal(NUMERICPLUS_PRESS_SIGNAL, false);
  EventSignal<> *minusPress = voidEventSignal(MINUS_PRESS_SIGNAL, false);
  EventSignal<> *multiplyPress = voidEventSignal(MULTIPLY_PRESS_SIGNAL, false);
  EventSignal<> *divisionPress = voidEventSignal(DIVISION_PRESS_SIGNAL, false);
  EventSignal<> *equalPress = voidEventSignal(EQUAL_PRESS_SIGNAL, false);
  EventSignal<> *percentPress = voidEventSignal(PERCENT_PRESS_SIGNAL, false);
  /* ksystem end 3 */
#endif
  EventSignal<WKeyEvent> *keyDown = keyEventSignal(KEYDOWN_SIGNAL, false);

  updateKeyDown = (enterPress && enterPress->needsUpdate(all))
    || (escapePress && escapePress->needsUpdate(all))
#ifdef EVENT_TONY
    /*ksystem start 4 */
    || (spacebarPress && spacebarPress->needsUpdate(all))
    || (tabPress && tabPress->needsUpdate(all))
    || (shifttabPress && shifttabPress->needsUpdate(all))
    || (alt1Press && alt1Press->needsUpdate(all))
    || (alt2Press && alt2Press->needsUpdate(all))
    || (alt3Press && alt3Press->needsUpdate(all))
    || (alt4Press && alt4Press->needsUpdate(all))
    || (alt5Press && alt5Press->needsUpdate(all))
    || (alt6Press && alt6Press->needsUpdate(all))
    || (alt7Press && alt7Press->needsUpdate(all))
    || (alt8Press && alt8Press->needsUpdate(all))
    || (alt9Press && alt9Press->needsUpdate(all))
    || (altaPress && altaPress->needsUpdate(all))
    || (altbPress && altbPress->needsUpdate(all))
    || (altcPress && altcPress->needsUpdate(all))
    || (altdPress && altdPress->needsUpdate(all))
    || (upPress && upPress->needsUpdate(all))
    || (downPress && downPress->needsUpdate(all))
    || (pageUpPress && pageUpPress->needsUpdate(all))
    || (pageDownPress && pageDownPress->needsUpdate(all))
    || (f1Press && f1Press->needsUpdate(all))
    || (f2Press && f2Press->needsUpdate(all))
    || (f3Press && f3Press->needsUpdate(all))
    || (f4Press && f4Press->needsUpdate(all))
    || (f5Press && f5Press->needsUpdate(all))
    || (f6Press && f6Press->needsUpdate(all))
    || (f7Press && f7Press->needsUpdate(all))
    || (f8Press && f8Press->needsUpdate(all))
    || (f9Press && f9Press->needsUpdate(all))
    || (f10Press && f10Press->needsUpdate(all))
    || (f11Press && f11Press->needsUpdate(all))
    || (f12Press && f12Press->needsUpdate(all))
    || (plusPress && plusPress->needsUpdate(all))
    || (numericplusPress && numericplusPress->needsUpdate(all))
    || (minusPress && minusPress->needsUpdate(all))
    || (multiplyPress && multiplyPress->needsUpdate(all))
    || (divisionPress && divisionPress->needsUpdate(all))
    || (equalPress && equalPress->needsUpdate(all))
    || (percentPress && percentPress->needsUpdate(all))
    /* ksystem end 4 */
#endif
    || (keyDown && keyDown->needsUpdate(all));

  if (updateKeyDown) {
    std::vector<DomElement::EventAction> actions;

    if (enterPress) {
      if (enterPress->needsUpdate(true)) {
	/*
	 * prevent enterPressed from triggering a changed event on all
	 * browsers except for Opera and IE
	 */
	std::string extraJS;

	const WEnvironment& env = app->environment();

	if (dynamic_cast<WFormWidget *>(this)
	    && !env.agentIsOpera() && !env.agentIsIE())
	  extraJS = "var g=this.onchange;"
	    ""      "this.onchange=function(){this.onchange=g;};";

	actions.push_back
	  (DomElement::EventAction("e.keyCode && (e.keyCode == 13)",
				   enterPress->javaScript() + extraJS,
				   enterPress->encodeCmd(),
				   enterPress->isExposedSignal()));
      }
      enterPress->updateOk();
    }

    if (escapePress) {
      if (escapePress->needsUpdate(true)) {
	actions.push_back
	  (DomElement::EventAction("e.keyCode && (e.keyCode == 27)",
				   escapePress->javaScript(),
				   escapePress->encodeCmd(),
				   escapePress->isExposedSignal()));
      }
      escapePress->updateOk();
    }

#ifdef EVENT_TONY
    /* ksystem start 5 */
    if (spacebarPress) {
      if (spacebarPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 32)",
                   spacebarPress->javaScript(),
                   spacebarPress->encodeCmd(),
                   spacebarPress->isExposedSignal()));
      }
      spacebarPress->updateOk();
    }

    if (tabPress) {
      if (tabPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(!e.shiftKey && e.keyCode && e.keyCode == 9)",
                   tabPress->javaScript(),
                   tabPress->encodeCmd(),
                   tabPress->isExposedSignal()));
      }
      tabPress->updateOk();
    }

    if (shifttabPress) {
      if (shifttabPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.shiftKey && e.keyCode && e.keyCode == 9)",
                   shifttabPress->javaScript(),
                   shifttabPress->encodeCmd(),
                   shifttabPress->isExposedSignal()));
      }
      shifttabPress->updateOk();
    }

    if (alt1Press) {
      if (alt1Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.altKey && e.keyCode && e.keyCode == 49)",
                   alt1Press->javaScript(),
                   alt1Press->encodeCmd(),
                   alt1Press->isExposedSignal()));
      }
      alt1Press->updateOk();
    }

    if (alt2Press) {
      if (alt2Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.altKey && e.keyCode && e.keyCode == 50)",
                   alt2Press->javaScript(),
                   alt2Press->encodeCmd(),
                   alt2Press->isExposedSignal()));
      }
      alt2Press->updateOk();
    }

    if (alt3Press) {
      if (alt3Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.altKey && e.keyCode && e.keyCode == 51)",
                   alt3Press->javaScript(),
                   alt3Press->encodeCmd(),
                   alt3Press->isExposedSignal()));
      }
      alt3Press->updateOk();
    }

    if (alt4Press) {
      if (alt4Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.altKey && e.keyCode && e.keyCode == 52)",
                   alt4Press->javaScript(),
                   alt4Press->encodeCmd(),
                   alt4Press->isExposedSignal()));
      }
      alt4Press->updateOk();
    }

    if (alt5Press) {
      if (alt5Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.altKey && e.keyCode && e.keyCode == 53)",
                   alt5Press->javaScript(),
                   alt5Press->encodeCmd(),
                   alt5Press->isExposedSignal()));
      }
      alt5Press->updateOk();
    }

    if (alt6Press) {
      if (alt6Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.altKey && e.keyCode && e.keyCode == 54)",
                   alt6Press->javaScript(),
                   alt6Press->encodeCmd(),
                   alt6Press->isExposedSignal()));
      }
      alt6Press->updateOk();
    }

    if (alt7Press) {
      if (alt7Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.altKey && e.keyCode && e.keyCode == 55)",
                   alt7Press->javaScript(),
                   alt7Press->encodeCmd(),
                   alt7Press->isExposedSignal()));
      }
      alt7Press->updateOk();
    }

    if (alt8Press) {
      if (alt8Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.altKey && e.keyCode && e.keyCode == 56)",
                   alt8Press->javaScript(),
                   alt8Press->encodeCmd(),
                   alt8Press->isExposedSignal()));
      }
      alt8Press->updateOk();
    }

    if (alt9Press) {
      if (alt9Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.altKey && e.keyCode && e.keyCode == 57)",
                   alt9Press->javaScript(),
                   alt9Press->encodeCmd(),
                   alt9Press->isExposedSignal()));
      }
      alt9Press->updateOk();
    }

    if (altaPress) {
      if (altaPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.altKey && e.keyCode && e.keyCode == 65)",
                   altaPress->javaScript(),
                   altaPress->encodeCmd(),
                   altaPress->isExposedSignal()));
      }
      altaPress->updateOk();
    }

    if (altbPress) {
      if (altbPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.altKey && e.keyCode && e.keyCode == 66)",
                   altbPress->javaScript(),
                   altbPress->encodeCmd(),
                   altbPress->isExposedSignal()));
      }
      altbPress->updateOk();
    }

    if (altcPress) {
      if (altcPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.altKey && e.keyCode && e.keyCode == 67)",
                   altcPress->javaScript(),
                   altcPress->encodeCmd(),
                   altcPress->isExposedSignal()));
      }
      altcPress->updateOk();
    }

    if (altdPress) {
      if (altdPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.altKey && e.keyCode && e.keyCode == 68)",
                   altdPress->javaScript(),
                   altdPress->encodeCmd(),
                   altdPress->isExposedSignal()));
      }
      altdPress->updateOk();
    }

    if (upPress) {
      if (upPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 38)",
                   upPress->javaScript(),
                   upPress->encodeCmd(),
                   upPress->isExposedSignal()));
      }
      upPress->updateOk();
    }

    if (downPress) {
      if (downPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 40)",
                   downPress->javaScript(),
                   downPress->encodeCmd(),
                   downPress->isExposedSignal()));
      }
      downPress->updateOk();
    }

    if (pageUpPress) {
      if (pageUpPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 33)", /* javascript keycode */
                   pageUpPress->javaScript(),
                   pageUpPress->encodeCmd(),
                   pageUpPress->isExposedSignal()));
      }
      pageUpPress->updateOk();
    }

    if (pageDownPress) {
      if (pageDownPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 34)",
                   pageDownPress->javaScript(),
                   pageDownPress->encodeCmd(),
                   pageDownPress->isExposedSignal()));
      }
      pageDownPress->updateOk();
    }

    if (f1Press) {
      if (f1Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 112)",
                   f1Press->javaScript(),
                   f1Press->encodeCmd(),
                   f1Press->isExposedSignal()));
      }
      f1Press->updateOk();
    }

    if (f2Press) {
      if (f2Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 113)",
                   f2Press->javaScript(),
                   f2Press->encodeCmd(),
                   f2Press->isExposedSignal()));
      }
      f2Press->updateOk();
    }

    if (f3Press) {
      if (f3Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 114)",
                   f3Press->javaScript(),
                   f3Press->encodeCmd(),
                   f3Press->isExposedSignal()));
      }
      f3Press->updateOk();
    }

    if (f4Press) {
      if (f4Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 115)",
                   f4Press->javaScript(),
                   f4Press->encodeCmd(),
                   f4Press->isExposedSignal()));
      }
      f4Press->updateOk();
    }

    if (f5Press) {
      if (f5Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 116)",
                   f5Press->javaScript(),
                   f5Press->encodeCmd(),
                   f5Press->isExposedSignal()));
      }
      f5Press->updateOk();
    }

    if (f6Press) {
      if (f6Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 117)",
                   f6Press->javaScript(),
                   f6Press->encodeCmd(),
                   f6Press->isExposedSignal()));
      }
      f6Press->updateOk();
    }

    if (f7Press) {
      if (f7Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 118)",
                   f7Press->javaScript(),
                   f7Press->encodeCmd(),
                   f7Press->isExposedSignal()));
      }
      f7Press->updateOk();
    }

    if (f8Press) {
      if (f8Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 119)",
                   f8Press->javaScript(),
                   f8Press->encodeCmd(),
                   f8Press->isExposedSignal()));
      }
      f8Press->updateOk();
    }

    if (f9Press) {
      if (f9Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 120)",
                   f9Press->javaScript(),
                   f9Press->encodeCmd(),
                   f9Press->isExposedSignal()));
      }
      f9Press->updateOk();
    }

    if (f10Press) {
      if (f10Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 121)",
                   f10Press->javaScript(),
                   f10Press->encodeCmd(),
                   f10Press->isExposedSignal()));
      }
      f10Press->updateOk();
    }

    if (f11Press) {
      if (f11Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 122)",
                   f11Press->javaScript(),
                   f11Press->encodeCmd(),
                   f11Press->isExposedSignal()));
      }
      f11Press->updateOk();
    }

    if (f12Press) {
      if (f12Press->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.keyCode == 123)",
                   f12Press->javaScript(),
                   f12Press->encodeCmd(),
                   f12Press->isExposedSignal()));
      }
      f12Press->updateOk();
    }
    if (plusPress) {
      if (plusPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.shiftKey && e.keyCode == 187)", /* keyCode html */
                   plusPress->javaScript(),
                   plusPress->encodeCmd(),
                   plusPress->isExposedSignal()));
      }
      plusPress->updateOk();
    }
    if (numericplusPress) {
      if (numericplusPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.charCode == 43)", /* keyCode html */
                   numericplusPress->javaScript(),
                   numericplusPress->encodeCmd(),
                   numericplusPress->isExposedSignal()));
      }
      numericplusPress->updateOk();
    }
    if (minusPress) {
      if (minusPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("((e.keyCode && e.keyCode == 189) || e.charCode == 45)",
                   minusPress->javaScript(),
                   minusPress->encodeCmd(),
                   minusPress->isExposedSignal()));
      }
      minusPress->updateOk();
    }
    if (multiplyPress) {
      if (multiplyPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.shiftKey && (e.keyCode == 56 || e.charCode == 42))",
                   multiplyPress->javaScript(),
                   multiplyPress->encodeCmd(),
                   multiplyPress->isExposedSignal()));
      }
      multiplyPress->updateOk();
    }
    if (divisionPress) {
      if (divisionPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && (e.keyCode == 191 || e.charCode == 47))",
                   divisionPress->javaScript(),
                   divisionPress->encodeCmd(),
                   divisionPress->isExposedSignal()));
      }
      divisionPress->updateOk();
    }
    if (equalPress) {
      if (equalPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && !e.shiftKey && e.keyCode == 187)",
                   equalPress->javaScript(),
                   equalPress->encodeCmd(),
                   equalPress->isExposedSignal()));
      }
      equalPress->updateOk();
    }
    if (percentPress) {
      if (percentPress->needsUpdate(all)) {
    actions.push_back
      (DomElement::EventAction("(e.keyCode && e.shiftKey && e.keyCode == 53)",
                   percentPress->javaScript(),
                   percentPress->encodeCmd(),
                   percentPress->isExposedSignal()));
      }
      percentPress->updateOk();
    }
    /* ksystem end 5 */
#endif

    if (keyDown) {
      if (keyDown->needsUpdate(true)) {
	actions.push_back
	  (DomElement::EventAction(std::string(),
				   keyDown->javaScript(),
				   keyDown->encodeCmd(),
				   keyDown->isExposedSignal()));
      }
      keyDown->updateOk();
    }

    if (!actions.empty())
      element.setEvent("keydown", actions);
    else if (!all)
      element.setEvent("keydown", std::string(), std::string());
  }

  /*
   * -- allow computation of dragged mouse distance
   */
  EventSignal<WMouseEvent> *mouseDown
    = mouseEventSignal(MOUSE_DOWN_SIGNAL, false);
  EventSignal<WMouseEvent> *mouseUp
    = mouseEventSignal(MOUSE_UP_SIGNAL, false);
  EventSignal<WMouseEvent> *mouseMove
    = mouseEventSignal(MOUSE_MOVE_SIGNAL, false);
  EventSignal<WMouseEvent> *mouseDrag
    = mouseEventSignal(MOUSE_DRAG_SIGNAL, false);

  bool updateMouseMove
    = (mouseMove && mouseMove->needsUpdate(all))
    || (mouseDrag && mouseDrag->needsUpdate(all));

  bool updateMouseDown
    = (mouseDown && mouseDown->needsUpdate(all))
    || updateMouseMove;

  bool updateMouseUp
    = (mouseUp && mouseUp->needsUpdate(all))
    || updateMouseMove;

  std::string CheckDisabled = "if($(o).hasClass('" +
    app->theme()->disabledClass() +
    "')){" WT_CLASS ".cancelEvent(e);return;}";

  if (updateMouseDown) {
    /*
     * when we have a mouseUp event, we also need a mouseDown event
     * to be able to compute dragDX/Y.
     *
     * When we have:
     *  - a mouseDrag
     *  - or a mouseDown + (mouseMove or mouseUp),
     * we need to capture everything after on mouse down, and keep track of the
     * down button if we have a mouseMove or mouseDrag
     */
    WStringStream js;

    js << CheckDisabled;

    if (mouseUp && mouseUp->isConnected())
      js << app->javaScriptClass() << "._p_.saveDownPos(event);";

    if ((mouseDrag && mouseDrag->isConnected())
	|| (mouseDown && mouseDown->isConnected()
	    && ((mouseUp && mouseUp->isConnected())
		|| (mouseMove && mouseMove->isConnected()))))
      js << WT_CLASS ".capture(this);";

    if ((mouseMove && mouseMove->isConnected())
	|| (mouseDrag && mouseDrag->isConnected()))
      js << WT_CLASS ".mouseDown(e);";

    if (mouseDown) {
      js << mouseDown->javaScript();
      element.setEvent("mousedown", js.str(),
		       mouseDown->encodeCmd(), mouseDown->isExposedSignal());
      mouseDown->updateOk();
    } else
      element.setEvent("mousedown", js.str(), std::string(), false);
  }

  if (updateMouseUp) {
    WStringStream js;

    /*
     * when we have a mouseMove or mouseDrag event, we need to keep track
     * of unpressing the button.
     */
    js << CheckDisabled;

    if ((mouseMove && mouseMove->isConnected())
	|| (mouseDrag && mouseDrag->isConnected()))
      js << WT_CLASS ".mouseUp(e);";
      
    if (mouseUp) {
      js << mouseUp->javaScript();
      element.setEvent("mouseup", js.str(),
		       mouseUp->encodeCmd(), mouseUp->isExposedSignal());
      mouseUp->updateOk();
    } else
      element.setEvent("mouseup", js.str(), std::string(), false);
  }

  if (updateMouseMove) {
    /*
     * We need to mix mouseDrag and mouseMove events.
     */
    std::vector<DomElement::EventAction> actions;
    
    if (mouseMove) {
      actions.push_back
	(DomElement::EventAction(std::string(),
				 mouseMove->javaScript(),
				 mouseMove->encodeCmd(),
				 mouseMove->isExposedSignal()));
      mouseMove->updateOk();
    }

    if (mouseDrag) {
      actions.push_back
	(DomElement::EventAction(WT_CLASS ".buttons",
				 mouseDrag->javaScript()
				 + WT_CLASS ".drag(e);",
				 mouseDrag->encodeCmd(),
				 mouseDrag->isExposedSignal()));
      mouseDrag->updateOk();
    }

    element.setEvent("mousemove", actions);
  }
  /*
   * -- allow computation of dragged touch distance
   */
  EventSignal<WTouchEvent> *touchStart
    = touchEventSignal(TOUCH_START_SIGNAL, false);
  EventSignal<WTouchEvent> *touchEnd
    = touchEventSignal(TOUCH_END_SIGNAL, false);
  EventSignal<WTouchEvent> *touchMove
    = touchEventSignal(TOUCH_MOVE_SIGNAL, false);

  bool updateTouchMove
    = (touchMove && touchMove->needsUpdate(all));

  bool updateTouchStart
    = (touchStart && touchStart->needsUpdate(all))
    || updateTouchMove;

  bool updateTouchEnd
    = (touchEnd && touchEnd->needsUpdate(all))
    || updateTouchMove;

  if (updateTouchStart) {
    /*
     * when we have a touchStart event, we also need a touchEnd event
     * to be able to compute dragDX/Y.
     *
     * When we have:
     *  - a touchStart + (touchMove or touchEnd),
     * we need to capture everything after on touch start, and keep track of the
     * down button if we have a touchMove 
     */
    WStringStream js;

    js << CheckDisabled;

    if (touchEnd && touchEnd->isConnected())
      js << app->javaScriptClass() << "._p_.saveDownPos(event);";

    if ((touchStart && touchStart->isConnected()
	    && ((touchEnd && touchEnd->isConnected())
		|| (touchMove && touchMove->isConnected()))))
      js << WT_CLASS ".capture(this);";

    if (touchStart) {
      js << touchStart->javaScript();
      element.setEvent("touchstart", js.str(),
		       touchStart->encodeCmd(), touchStart->isExposedSignal());
      touchStart->updateOk();
    } else
      element.setEvent("touchstart", js.str(), std::string(), false);
  }

  if (updateTouchEnd) {
    WStringStream js;

    /*
     * when we have a touchMove, we need to keep track
     * of removing touch.
     */
    js << CheckDisabled;

    if (touchEnd) {
      js << touchEnd->javaScript();
      element.setEvent("touchend", js.str(),
		         touchEnd->encodeCmd(), touchEnd->isExposedSignal());
      touchEnd->updateOk();
    } else
      element.setEvent("touchend", js.str(), std::string(), false);
  }

  if (updateTouchMove) {
    
    if (touchMove) {
      element.setEvent("touchmove", touchMove->javaScript(),
			touchMove->encodeCmd(), touchMove->isExposedSignal());
      touchMove->updateOk();
    }
  }

  /*
   * -- mix mouseClick and mouseDblClick events in mouseclick since we
   *    only want to fire one of both
   */
  EventSignal<WMouseEvent> *mouseClick
    = mouseEventSignal(M_CLICK_SIGNAL, false);
  EventSignal<WMouseEvent> *mouseDblClick
    = mouseEventSignal(DBL_CLICK_SIGNAL, false);  

  bool updateMouseClick
    = (mouseClick && mouseClick->needsUpdate(all))
    || (mouseDblClick && mouseDblClick->needsUpdate(all));

  if (updateMouseClick) {
    WStringStream js;

    js << CheckDisabled;

    if (mouseDrag)
      js << "if (" WT_CLASS ".dragged()) return;";

    if (mouseDblClick && mouseDblClick->needsUpdate(all)) {
      /*
       * Click: if timer is running:
       *  - clear timer, dblClick()
       *  - start timer, clear timer and click()
       */

      /* We have to prevent this immediately ! */
      if (mouseClick) {
	if (mouseClick->defaultActionPrevented() ||
	    mouseClick->propagationPrevented()) {
	  js << WT_CLASS ".cancelEvent(e";
	  if (mouseClick->defaultActionPrevented() &&
	      mouseClick->propagationPrevented())
	    js << ");";
	  else if (mouseClick->defaultActionPrevented())
	    js << ",0x2);";
	  else
	    js << ",0x1);";
	}
      }

      js << "if(" WT_CLASS ".isDblClick(o, e)) {"
	 << mouseDblClick->javaScript();

      if (mouseDblClick->isExposedSignal())
	js << app->javaScriptClass()
		 << "._p_.update(o,'" << mouseDblClick->encodeCmd()
		 << "',e,true);";

      mouseDblClick->updateOk();

      js <<
	"}else{"
	"""if (" WT_CLASS ".isIElt9 && document.createEventObject) "
	""  "e = document.createEventObject(e);"
	"""o.wtE1 = e;"
	"""o.wtClickTimeout = setTimeout(function() {"
	""   "o.wtClickTimeout = null; o.wtE1 = null;";

      if (mouseClick) {
	js << mouseClick->javaScript();

	if (mouseClick->isExposedSignal()) {
	  js << app->javaScriptClass()
		   << "._p_.update(o,'" << mouseClick->encodeCmd()
		   << "',e,true);";
	}

	mouseClick->updateOk();
      }

      const Configuration& conf = app->environment().server()->configuration();
      js << "}," << conf.doubleClickTimeout() << ");}";
    } else {
      if (mouseClick && mouseClick->needsUpdate(all)) {
	js << mouseClick->javaScript();

	if (mouseClick->isExposedSignal()) {
	  js << app->javaScriptClass()
	     << "._p_.update(o,'" << mouseClick->encodeCmd()
	     << "',e,true);";
	}

	mouseClick->updateOk();
      }
    }

    element.setEvent(CLICK_SIGNAL, js.str(),
		     mouseClick ? mouseClick->encodeCmd() : "");

    if (mouseDblClick) {
      if (app->environment().agentIsIElt(9))
	element.setEvent("dblclick", "this.onclick()");
    }
  }

  /*
   * -- mouseOver with delay
   */
  EventSignal<WMouseEvent> *mouseOver
    = mouseEventSignal(MOUSE_OVER_SIGNAL, false);
  EventSignal<WMouseEvent> *mouseOut
    = mouseEventSignal(MOUSE_OUT_SIGNAL, false); 

  bool updateMouseOver = mouseOver && mouseOver->needsUpdate(all);

  if (mouseOverDelay_) {
    if (updateMouseOver) {
      WStringStream js;
      js << "o.over=setTimeout(function() {"
	 << "o.over = null;"
	 << mouseOver->javaScript();

      if (mouseOver->isExposedSignal()) {
	js << app->javaScriptClass()
	   << "._p_.update(o,'" << mouseOver->encodeCmd() << "',e,true);";
      }

      js << "}," << mouseOverDelay_ << ");";

      element.setEvent("mouseover", js.str(), "");

      mouseOver->updateOk();

      if (!mouseOut)
	mouseOut = mouseEventSignal(MOUSE_OUT_SIGNAL, true);

      element.setEvent("mouseout",
		       "clearTimeout(o.over); o.over=null;"
		       + mouseOut->javaScript(),
		       mouseOut->encodeCmd(), mouseOut->isExposedSignal());
      mouseOut->updateOk();
    }
  } else {
    if (updateMouseOver) {
      element.setEventSignal("mouseover", *mouseOver);
      mouseOver->updateOk();
    }

    bool updateMouseOut = mouseOut && mouseOut->needsUpdate(all);

    if (updateMouseOut) {
      element.setEventSignal("mouseout", *mouseOut);
      mouseOut->updateOk();
    }
  }

  updateEventSignals(element, all);

  WWebWidget::updateDom(element, all);
}

void WInteractWidget::setMouseOverDelay(int delay)
{
  mouseOverDelay_ = delay;

  EventSignal<WMouseEvent> *mouseOver
    = mouseEventSignal(MOUSE_OVER_SIGNAL, false);
  if (mouseOver)
    mouseOver->senderRepaint();
}

int WInteractWidget::mouseOverDelay() const
{
  return mouseOverDelay_;
}

void WInteractWidget::updateEventSignals(DomElement& element, bool all)
{
  EventSignalList& other = eventSignals();

  for (EventSignalList::iterator i = other.begin(); i != other.end(); ++i) {
#ifndef WT_NO_BOOST_INTRUSIVE
    EventSignalBase& s = *i;
#else
    EventSignalBase& s = **i;
#endif

    if (s.name() == WInteractWidget::M_CLICK_SIGNAL
	&& flags_.test(BIT_REPAINT_TO_AJAX))
      element.unwrap();

    updateSignalConnection(element, s, s.name(), all);
  }
}

void WInteractWidget::propagateRenderOk(bool deep)
{
  EventSignalList& other = eventSignals();

  for (EventSignalList::iterator i = other.begin(); i != other.end(); ++i) {
#ifndef WT_NO_BOOST_INTRUSIVE
    EventSignalBase& s = *i;
#else
    EventSignalBase& s = **i;
#endif
    s.updateOk();
  }

  WWebWidget::propagateRenderOk(deep);
}

void WInteractWidget::load()
{
  if (!isDisabled()) {
    if (parent())
      flags_.set(BIT_ENABLED, parent()->isEnabled());
    else
      flags_.set(BIT_ENABLED, true);
  } else
    flags_.set(BIT_ENABLED, false);

  WWebWidget::load();
}

bool WInteractWidget::isEnabled() const
{
  return !isDisabled() && flags_.test(BIT_ENABLED);
}

void WInteractWidget::propagateSetEnabled(bool enabled)
{
  flags_.set(BIT_ENABLED, enabled);

  WApplication *app = WApplication::instance();
  std::string disabledClass = app->theme()->disabledClass();
  toggleStyleClass(disabledClass, !enabled, true);

  WWebWidget::propagateSetEnabled(enabled);
}

void WInteractWidget::setDraggable(const std::string& mimeType,
				   WWidget *dragWidget, bool isDragWidgetOnly,
				   WObject *sourceObject)
{
  if (dragWidget == 0)
    dragWidget = this;

  if (sourceObject == 0)
    sourceObject = this;

  if (isDragWidgetOnly) {
    dragWidget->hide();
  }

  WApplication *app = WApplication::instance();

  setAttributeValue("dmt", mimeType);
  setAttributeValue("dwid", dragWidget->id());
  setAttributeValue("dsid", app->encodeObject(sourceObject));

  if (!dragSlot_) {
    dragSlot_ = new JSlot();
    dragSlot_->setJavaScript("function(o,e){" + app->javaScriptClass()
			     + "._p_.dragStart(o,e);" + "}");
  }

  if (!dragTouchSlot_) {
    dragTouchSlot_ = new JSlot();
    dragTouchSlot_->setJavaScript("function(o,e){" + app->javaScriptClass()
				+ "._p_.touchStart(o,e);" + "}");
  }

  if (!dragTouchEndSlot_) {
    dragTouchEndSlot_ = new JSlot();
    dragTouchEndSlot_->setJavaScript("function(){" + app->javaScriptClass()
				+ "._p_.touchEnded();" + "}");
  }

  mouseWentDown().connect(*dragSlot_);
  touchStarted().connect(*dragTouchSlot_);
  touchStarted().preventDefaultAction(true);
  touchEnded().connect(*dragTouchEndSlot_);
}

void WInteractWidget::unsetDraggable()
{
  if (dragSlot_) {
    mouseWentDown().disconnect(*dragSlot_);
    delete dragSlot_;
    dragSlot_ = 0;
  }

  if (dragTouchSlot_) {
    touchStarted().disconnect(*dragTouchSlot_);
    delete dragTouchSlot_;
    dragTouchSlot_ = 0;
  }

  if (dragTouchEndSlot_) {
    touchEnded().disconnect(*dragTouchEndSlot_);
    delete dragTouchEndSlot_;
    dragTouchEndSlot_ = 0;
  }
}

}
