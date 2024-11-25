module CyberpunkMP.World

import Codeware.*
import CyberpunkMP.*

public native class ChatSystem extends IScriptable {
    public native func Send(message: String);
    public native func GetUsername() -> String;

    public func HandleChatMessage(author: String, message: String) {
        let evt: ref<ChatMessageUIEvent>;
        evt.author = author;
        evt.message = message;

        CMPLog(s"\(message)");
        let uiSystem = GameInstance.GetUISystem(GetGameInstance());        
        uiSystem.QueueEvent(evt);
    }
    // private final func OnConnectToServer(request: ref<ConnectToServerRequest>) -> Void {
        // this.m_Blackboard.SetBool(GetAllBlackboardDefs().UI_ComDevice.ContactsActive, open, true);
    // }
}
