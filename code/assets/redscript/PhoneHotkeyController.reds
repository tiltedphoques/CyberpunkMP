@addField(PhoneHotkeyController)
private let m_isMultiplayerUIOpenBBId: ref<CallbackHandle>;

@wrapMethod(PhoneHotkeyController)
protected func Initialize() -> Bool {
    let og = wrappedMethod();
    this.m_isMultiplayerUIOpenBBId = this.GetUIBlackboard().RegisterListenerBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerContextRequest, this, n"OnUIMultiplayerIsShown");
    return og;
}

@wrapMethod(PhoneHotkeyController)
protected func Uninitialize() -> Void {
    wrappedMethod();
    this.GetUIBlackboard().UnregisterListenerBool(GetAllBlackboardDefs().UIGameData.UIMultiplayerContextRequest, this.m_isMultiplayerUIOpenBBId);
}

@addMethod(PhoneHotkeyController)
private final func OnUIMultiplayerIsShown(value: Bool) -> Void {
    this.ToggleVisibility(!value, false);
}