module CyberpunkMP

public static func UTF8StrEmpty(value: String) -> Bool {
    return UTF8StrLen(value) == 0;
}

public static func UTF8StrNotEmpty(value: String) -> Bool {
    return UTF8StrLen(value) != 0;
}

public static func UTF8StrTrim(value: String) -> String {
    return UTF8StrTrimRight(UTF8StrTrimLeft(value));
}

public static func UTF8StrTrimLeft(value: String) -> String {
    let index = StrFindFirst(value, " ");
    let length = UTF8StrLen(value);

    while (index == 0) {
        value = UTF8StrRight(value, length - 1);
        length -= 1;
        index = StrFindFirst(value, " ");
    }
    return value;
}

public static func UTF8StrTrimRight(value: String) -> String {
    let index = StrFindLast(value, " ");
    let length = UTF8StrLen(value);

    while (index == length - 1) {
        value = UTF8StrLeft(value, length - 1);
        length -= 1;
        index = StrFindLast(value, " ");
    }
    return value;
}