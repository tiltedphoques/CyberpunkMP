using System.Reflection;

namespace Server.Loader;

public static class FileSystemHelper
{
    public static string GetRootPath() => GetPath();

    public static string GetPath(params string[] segments)
    {
        var assemblyPath = Assembly.GetExecutingAssembly().Location;
        var rootPath = Path.GetDirectoryName(assemblyPath)!;

        foreach (var segment in segments)
        {
            rootPath = Path.Combine(rootPath, segment);
        }

        return rootPath;
    }
}