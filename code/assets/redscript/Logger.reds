module CyberpunkMP

public static func CMPLog(value: script_ref<String>) -> Void {
    FTLog(CMPTraceLog(value));
}

public static func CMPWarn(value: script_ref<String>) -> Void {
    FTLogWarning(CMPTraceLog(value));
}

public static func CMPError(value: script_ref<String>) -> Void {
    FTLogError(CMPTraceLog(value));
}

private static func CMPTraceLog(value: script_ref<String>) -> String {
    let entries = GetStackTrace(1, false);
    let entry = entries[0];
    let function = NameToString(entry.function);
    let tokens = StrSplit(function, ";");

    function = tokens[0];
    let trace: String;

    if IsDefined(entry.object) {
        let className = NameToString(entry.class);

        if StrFindFirst(className, "CyberpunkMP.") == 0 {
            className = StrRight(className, StrLen(className) - StrLen("CyberpunkMP."));
        } else {
            className = s"*.\(className)";
        }
        trace = s"[\(className)][\(function)]";
    } else {
        trace = s"[\(function)]";
    }
    return s"[CMP]\(trace) \(value)";
}