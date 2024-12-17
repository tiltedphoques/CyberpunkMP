namespace Server.Loader;

public static class EnvironmentHelper
{
#if DEBUG
    public const bool IsDebug = true;
    public const bool IsRelease = false;
#else
    public const bool IsDebug = false;
    public const bool IsRelease = true;
#endif
}