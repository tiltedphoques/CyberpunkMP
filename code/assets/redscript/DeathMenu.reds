// @wrapMethod(DeathMenuGameController)
// protected cb func OnInitialize() -> Bool {
//     wrappedMethod();
//     this.SpawnFromExternal(this.GetRootCompoundWidget(), r"mods\\cyberpunkmp\\multiplayer_ui.inkwidget", n"chat");
// }

@wrapMethod(DeathMenuGameController)
protected cb func OnInitialize() -> Bool {
    wrappedMethod();
    this.AsyncSpawnFromExternal(inkWidgetRef.Get(this.m_buttonHintsManagerRef), r"mods\\cyberpunkmp\\multiplayer_ui.inkwidget", n"chat", this, n"OnChatSpawn");
}

@addMethod(DeathMenuGameController)
protected cb func OnChatSpawn(widget: ref<inkWidget>, userData: ref<IScriptable>) -> Bool {
    widget.SetAnchorPoint(1.0, 1.0);
    // this.GetRootCompoundWidget().ReorderChild(widget, 0);
}