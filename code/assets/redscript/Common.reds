import CyberpunkMP.Plugins.*

public native class ChatMessageUIEvent extends Event {
    public native let author: String;
    public native let message: String;
}

public class ConnectedToServer extends Event {
    public let m_connected: Bool;
}

public class ChatMessageData extends IScriptable {
    public let m_author: String;
    public let m_message: String;
    public let m_isSelf: Bool;
    public let m_needsAuthorLabel: Bool = true;
}

public class ServerData extends IScriptable {
    public let m_name: String;
    public let m_description: String;
    public let m_test: Bool = false;
}

public class JobType extends IScriptable {
    public let m_name: String;
    public let m_description: String;
    public let m_test: Bool = false;
}

public class ItemData extends IScriptable {
    public let m_entry: ShopEntry;
}

public class EmoteData extends IScriptable {
    public let m_name: String;
}

public func CreateInputHint(label: CName, action: CName, hold: Bool) -> InputHintData {
    let data: InputHintData;
    data.source = n"CyberpunkMP";
    data.action = action;
    if hold {
        data.holdIndicationType = inkInputHintHoldIndicationType.Hold;
    } else {
        data.holdIndicationType = inkInputHintHoldIndicationType.Press;
    }
    data.sortingPriority = 0;
    data.enableHoldAnimation = false;
    data.localizedLabel = GetLocalizedTextByKey(label);
    if StrLen( data.localizedLabel) == 0 {
            data.localizedLabel = ToString(label);
    };
    // data.groupId = n"CyberpunkMP";
    return data;
}