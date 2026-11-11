using System.IO.Compression;
using System.Security.Cryptography;

namespace GTA6.Installer;

internal sealed class PackageInstaller
{
    public async Task InstallAsync(
        ReleaseManifest manifest,
        string packagePath,
        string installDirectory,
        IProgress<string> progress,
        CancellationToken cancellationToken)
    {
        manifest.Validate();
        if (!File.Exists(packagePath))
            throw new FileNotFoundException("Instalační balík nebyl nalezen.", packagePath);

        progress.Report("Ověřuji integritu instalačního balíku…");
        await using (var packageStream = File.OpenRead(packagePath))
        {
            var actualHash = Convert.ToHexString(await SHA256.HashDataAsync(packageStream, cancellationToken));
            if (!actualHash.Equals(manifest.Sha256, StringComparison.OrdinalIgnoreCase))
                throw new InvalidDataException("SHA-256 balíku nesouhlasí. Instalace byla zastavena.");
        }

        var fullTarget = Path.GetFullPath(installDirectory);
        if (fullTarget.TrimEnd(Path.DirectorySeparatorChar).Length < 4)
            throw new InvalidOperationException("Vybraná instalační cesta není bezpečná.");
        var parent = Directory.GetParent(fullTarget)?.FullName
            ?? throw new InvalidOperationException("Nelze určit nadřazenou instalační složku.");
        Directory.CreateDirectory(parent);

        var staging = Path.Combine(parent, $".gta6-staging-{Guid.NewGuid():N}");
        var backup = Path.Combine(parent, $".gta6-backup-{Guid.NewGuid():N}");
        Directory.CreateDirectory(staging);

        try
        {
            progress.Report("Rozbaluji herní soubory…");
            ExtractZipSafely(packagePath, staging, cancellationToken);

            var executable = Path.GetFullPath(Path.Combine(staging, manifest.Executable));
            if (!executable.StartsWith(staging + Path.DirectorySeparatorChar, StringComparison.OrdinalIgnoreCase) ||
                !File.Exists(executable))
                throw new InvalidDataException("Balík neobsahuje očekávaný spustitelný soubor hry.");

            await File.WriteAllTextAsync(
                Path.Combine(staging, "gta6-install.json"),
                $$"""{"product":"GTA6","version":"{{manifest.Version}}"}""",
                cancellationToken);

            cancellationToken.ThrowIfCancellationRequested();
            progress.Report("Dokončuji instalaci…");

            if (Directory.Exists(fullTarget))
                Directory.Move(fullTarget, backup);

            try
            {
                Directory.Move(staging, fullTarget);
            }
            catch
            {
                if (Directory.Exists(backup) && !Directory.Exists(fullTarget))
                    Directory.Move(backup, fullTarget);
                throw;
            }

            if (Directory.Exists(backup))
                Directory.Delete(backup, recursive: true);
        }
        finally
        {
            if (Directory.Exists(staging))
                Directory.Delete(staging, recursive: true);
            // A backup is retained if rollback itself could not restore it.
        }
    }

    private static void ExtractZipSafely(string archivePath, string destination, CancellationToken token)
    {
        var root = Path.GetFullPath(destination) + Path.DirectorySeparatorChar;
        using var archive = ZipFile.OpenRead(archivePath);

        foreach (var entry in archive.Entries)
        {
            token.ThrowIfCancellationRequested();
            if (string.IsNullOrEmpty(entry.Name))
                continue;

            var target = Path.GetFullPath(Path.Combine(destination, entry.FullName));
            if (!target.StartsWith(root, StringComparison.OrdinalIgnoreCase))
                throw new InvalidDataException("ZIP obsahuje cestu mimo instalační složku.");

            Directory.CreateDirectory(Path.GetDirectoryName(target)!);
            using var source = entry.Open();
            using var output = new FileStream(target, FileMode.CreateNew, FileAccess.Write, FileShare.None);
            source.CopyTo(output);
        }
    }
}
