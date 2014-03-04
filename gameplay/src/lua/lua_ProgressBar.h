#ifndef LUA_PROGRESSBAR_H_
#define LUA_PROGRESSBAR_H_

namespace gameplay
{

// Lua bindings for ProgressBar.
int lua_ProgressBar__gc(lua_State* state);
int lua_ProgressBar_addListener(lua_State* state);
int lua_ProgressBar_addRef(lua_State* state);
int lua_ProgressBar_addScriptCallback(lua_State* state);
int lua_ProgressBar_canFocus(lua_State* state);
int lua_ProgressBar_createAnimation(lua_State* state);
int lua_ProgressBar_createAnimationFromBy(lua_State* state);
int lua_ProgressBar_createAnimationFromTo(lua_State* state);
int lua_ProgressBar_destroyAnimation(lua_State* state);
int lua_ProgressBar_getAbsoluteBounds(lua_State* state);
int lua_ProgressBar_getAlignment(lua_State* state);
int lua_ProgressBar_getAnimation(lua_State* state);
int lua_ProgressBar_getAnimationPropertyComponentCount(lua_State* state);
int lua_ProgressBar_getAnimationPropertyValue(lua_State* state);
int lua_ProgressBar_getAutoSize(lua_State* state);
int lua_ProgressBar_getBorder(lua_State* state);
int lua_ProgressBar_getBounds(lua_State* state);
int lua_ProgressBar_getClip(lua_State* state);
int lua_ProgressBar_getClipBounds(lua_State* state);
int lua_ProgressBar_getConsumeInputEvents(lua_State* state);
int lua_ProgressBar_getCursorColor(lua_State* state);
int lua_ProgressBar_getCursorRegion(lua_State* state);
int lua_ProgressBar_getCursorUVs(lua_State* state);
int lua_ProgressBar_getFocusIndex(lua_State* state);
int lua_ProgressBar_getFont(lua_State* state);
int lua_ProgressBar_getFontSize(lua_State* state);
int lua_ProgressBar_getHeight(lua_State* state);
int lua_ProgressBar_getId(lua_State* state);
int lua_ProgressBar_getImageColor(lua_State* state);
int lua_ProgressBar_getImageRegion(lua_State* state);
int lua_ProgressBar_getImageUVs(lua_State* state);
int lua_ProgressBar_getMargin(lua_State* state);
int lua_ProgressBar_getOpacity(lua_State* state);
int lua_ProgressBar_getOrientation(lua_State* state);
int lua_ProgressBar_getPadding(lua_State* state);
int lua_ProgressBar_getParent(lua_State* state);
int lua_ProgressBar_getRefCount(lua_State* state);
int lua_ProgressBar_getSkinColor(lua_State* state);
int lua_ProgressBar_getSkinRegion(lua_State* state);
int lua_ProgressBar_getState(lua_State* state);
int lua_ProgressBar_getStyle(lua_State* state);
int lua_ProgressBar_getTextAlignment(lua_State* state);
int lua_ProgressBar_getTextColor(lua_State* state);
int lua_ProgressBar_getTextRightToLeft(lua_State* state);
int lua_ProgressBar_getTheme(lua_State* state);
int lua_ProgressBar_getTopLevelForm(lua_State* state);
int lua_ProgressBar_getType(lua_State* state);
int lua_ProgressBar_getValue(lua_State* state);
int lua_ProgressBar_getWidth(lua_State* state);
int lua_ProgressBar_getX(lua_State* state);
int lua_ProgressBar_getY(lua_State* state);
int lua_ProgressBar_getZIndex(lua_State* state);
int lua_ProgressBar_hasFocus(lua_State* state);
int lua_ProgressBar_isChild(lua_State* state);
int lua_ProgressBar_isContainer(lua_State* state);
int lua_ProgressBar_isEnabled(lua_State* state);
int lua_ProgressBar_isEnabledInHierarchy(lua_State* state);
int lua_ProgressBar_isHeightPercentage(lua_State* state);
int lua_ProgressBar_isVisible(lua_State* state);
int lua_ProgressBar_isVisibleInHierarchy(lua_State* state);
int lua_ProgressBar_isWidthPercentage(lua_State* state);
int lua_ProgressBar_isXPercentage(lua_State* state);
int lua_ProgressBar_isYPercentage(lua_State* state);
int lua_ProgressBar_release(lua_State* state);
int lua_ProgressBar_removeListener(lua_State* state);
int lua_ProgressBar_removeScriptCallback(lua_State* state);
int lua_ProgressBar_setAlignment(lua_State* state);
int lua_ProgressBar_setAnimationPropertyValue(lua_State* state);
int lua_ProgressBar_setAutoSize(lua_State* state);
int lua_ProgressBar_setBorder(lua_State* state);
int lua_ProgressBar_setBounds(lua_State* state);
int lua_ProgressBar_setCanFocus(lua_State* state);
int lua_ProgressBar_setConsumeInputEvents(lua_State* state);
int lua_ProgressBar_setCursorColor(lua_State* state);
int lua_ProgressBar_setCursorRegion(lua_State* state);
int lua_ProgressBar_setEnabled(lua_State* state);
int lua_ProgressBar_setFocus(lua_State* state);
int lua_ProgressBar_setFocusIndex(lua_State* state);
int lua_ProgressBar_setFont(lua_State* state);
int lua_ProgressBar_setFontSize(lua_State* state);
int lua_ProgressBar_setHeight(lua_State* state);
int lua_ProgressBar_setId(lua_State* state);
int lua_ProgressBar_setImageColor(lua_State* state);
int lua_ProgressBar_setImageRegion(lua_State* state);
int lua_ProgressBar_setMargin(lua_State* state);
int lua_ProgressBar_setOpacity(lua_State* state);
int lua_ProgressBar_setOrientation(lua_State* state);
int lua_ProgressBar_setPadding(lua_State* state);
int lua_ProgressBar_setPosition(lua_State* state);
int lua_ProgressBar_setSize(lua_State* state);
int lua_ProgressBar_setSkinColor(lua_State* state);
int lua_ProgressBar_setSkinRegion(lua_State* state);
int lua_ProgressBar_setStyle(lua_State* state);
int lua_ProgressBar_setTextAlignment(lua_State* state);
int lua_ProgressBar_setTextColor(lua_State* state);
int lua_ProgressBar_setTextRightToLeft(lua_State* state);
int lua_ProgressBar_setValue(lua_State* state);
int lua_ProgressBar_setVisible(lua_State* state);
int lua_ProgressBar_setWidth(lua_State* state);
int lua_ProgressBar_setX(lua_State* state);
int lua_ProgressBar_setY(lua_State* state);
int lua_ProgressBar_setZIndex(lua_State* state);
int lua_ProgressBar_static_ANIMATE_OPACITY(lua_State* state);
int lua_ProgressBar_static_ANIMATE_POSITION(lua_State* state);
int lua_ProgressBar_static_ANIMATE_POSITION_X(lua_State* state);
int lua_ProgressBar_static_ANIMATE_POSITION_Y(lua_State* state);
int lua_ProgressBar_static_ANIMATE_SIZE(lua_State* state);
int lua_ProgressBar_static_ANIMATE_SIZE_HEIGHT(lua_State* state);
int lua_ProgressBar_static_ANIMATE_SIZE_WIDTH(lua_State* state);
int lua_ProgressBar_static_create(lua_State* state);

void luaRegister_ProgressBar();

}

#endif
