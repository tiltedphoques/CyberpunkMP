using CppSharp;
using CppSharp.AST;
using CppSharp.Generators;
using System.Runtime.InteropServices;

internal class CyberpunkSdk : ILibrary
{
    private string OutputPath { get; set; }
    private string SourcePath { get; set; }
    private string[] FileNames { get; set; }

    internal CyberpunkSdk(string outputPath, string sourcePath, string[] fileNames)
    {
        OutputPath = outputPath;
        SourcePath = sourcePath;
        FileNames = fileNames;
    }

    public void Postprocess(Driver driver, ASTContext ctx)
    {
    }

    public void Preprocess(Driver driver, ASTContext ctx)
    {
    }

    public void Setup(Driver driver)
    {
        var options = driver.Options;
        options.OutputDir = OutputPath;
        options.GeneratorKind = GeneratorKind.CSharp;

        var parserOptions = driver.ParserOptions;
        parserOptions.AddDefines("WIN32");
        parserOptions.AddDefines("CPPSHARP_GENERATOR");

        var module = options.AddModule("CyberpunkSdk.Internal");
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            module.SharedLibraryName = "Server.exe";
        else
            module.SharedLibraryName = "Server";

        module.IncludeDirs.Add(SourcePath);

        foreach (var file in FileNames)
        {
            module.Headers.Add(file);
        }
    }

    public void SetupPasses(Driver driver)
    {

    }
}

internal class Program
{
    private static string GetGitRoot()
    {
        string gitFolderPath = LibGit2Sharp.Repository.Discover(Directory.GetCurrentDirectory());
        if (gitFolderPath == null)
            throw new Exception("This code should be ran from a Git repository!");

        // Normalize directory separators to match the current environment
        gitFolderPath = gitFolderPath.Replace('/', Path.DirectorySeparatorChar)
                                     .Replace('\\', Path.DirectorySeparatorChar);

        // Remove the .git part from the path
        string repositoryRoot = gitFolderPath.TrimEnd(Path.DirectorySeparatorChar)
                                             .Remove(gitFolderPath.LastIndexOf(".git", StringComparison.Ordinal));

        return repositoryRoot;
    }

    private static void Main(string[] args)
    {
        var fileNames = args.Select(arg => Path.GetFileName(arg)).ToArray();

        try
        {
            string root = GetGitRoot();
            string output = Path.Combine(root, @"code/scripting/CyberpunkSdk");
            string source = Path.Combine(root, @"code/server/Scripting");

            ConsoleDriver.Run(new CyberpunkSdk(output, source, fileNames));

            string sdkFile = Path.Combine(output, "CyberpunkSdk.Internal.cs");

            string text = File.ReadAllText(sdkFile);

            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
                text = text.Replace("Server.exe.dll", "Server.exe");
            else
                text = text.Replace("DllImport(\"Server\"", "DllImport(\"../Server\"");
           
            File.WriteAllText(sdkFile, text);
        }
        catch(Exception e)
        {
            Console.WriteLine(e);
        }
    }
}