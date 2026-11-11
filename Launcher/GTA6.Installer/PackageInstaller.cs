using System.IO.Compression;
using System.Security.Cryptography;

namespace GTA6.Installer;

internal sealed class PackageInstaller
{
    public async Task InstallAsync(
        ReleaseManifest manifest,
        Stream packageStream,
        string installDirectory,
        IProgress<string> progress,
        CancellationToken cancellationToken)
    {
        manifest.Validate();
        ArgumentNullException.ThrowIfNull(packageStream);
        if (!packageStream.CanRead || !packageStream.CanSeek)
            throw new ArgumentException("Herní balík musí být čitelný a umožňovat posun streamu.", nameof(packageStream));
        if (string.IsNullOrWhiteSpace(installDirectory))
            throw new InvalidOperationException("Instalační cesta není vyplněná.");

        progress.Report("Ověřuji integritu instalačního balíku…");
        packageStream.Position = 0;
        var actualHash = Convert.ToHexString(await SHA256.HashDataAsync(packageStream, cancellationToken));
        if (!actualHash.Equals(manifest.Sha256, StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("SHA-256 balíku nesouhlasí. Instalace byla zastavena.");
        packageStream.Position = 0;

        var fullTarget = Path.GetFullPath(installDirectory);
        var trimmedTarget = Path.TrimEndingDirectorySeparator(fullTarget);
        if (trimmedTarget.Length < 4 || string.Equals(trimmedTarget, Path.GetPathRoot(fullTarget),
                StringComparison.OrdinalIgnoreCase))
            throw new InvalidOperationException("Vybraná instalační cesta není bezpečná.");

        var parent = Directory.GetParent(fullTarget)?.FullName
            ?? throw new InvalidOperationException("Nelze určit nadřazenou instalační složku.");
        Directory.CreateDirectory(parent);

        var staging = Path.Combine(parent, $".gta6-staging-{Guid.NewGuid():N}");
        var backup = Path.Combine(parent, $".gta6-backup-{Guid.NewGuid():N}");
        Directory.CreateDirectory(staging);

        try
        {
            progress.Report("Kontroluji obsah herního balíku…");
            var requiredBytes = GetUncompressedSize(packageStream, cancellationToken);
            var drive = new DriveInfo(Path.GetPathRoot(staging)!);
            var safetyMargin = 256L * 1024 * 1024;
            if (requiredBytes > drive.AvailableFreeSpace - safetyMargin)
                throw new IOException("Na cílovém disku není dostatek volného místa pro instalaci.");

            progress.Report("Rozbaluji herní soubory…");
            ExtractZipSafely(packageStream, staging, cancellationToken);

            var executable = ResolveContainedPath(staging, manifest.Executable);
            if (!File.Exists(executable))
                throw new InvalidDataException("Balík neobsahuje očekávaný spustitelný soubor hry.");

            await File.WriteAllTextAsync(
                Path.Combine(staging, "gta6-install.json"),
                $$"""{"product":"GTA6","version":"{{manifest.Version}}"}""",
                cancellationToken);

            cancellationToken.ThrowIfCancellationRequested();
            progress.Report("Dokončuji instalaci…");

            if (Directory.Exists(fullTarget))
                Directory.Move(fullTarget, backup);
            else if (File.Exists(fullTarget))
                throw new IOException("Cílová instalační cesta již existuje jako soubor.");

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

            // Backup cleanup is best-effort after the new installation is already committed.
            try
            {
                if (Directory.Exists(backup))
                    Directory.Delete(backup, recursive: true);
            }
            catch (IOException) { }
            catch (UnauthorizedAccessException) { }
        }
        finally
        {
            if (Directory.Exists(staging))
                Directory.Delete(staging, recursive: true);
        }
    }

    private static long GetUncompressedSize(Stream packageStream, CancellationToken token)
    {
        packageStream.Position = 0;
        using var archive = new ZipArchive(packageStream, ZipArchiveMode.Read, leaveOpen: true);
        long total = 0;
        foreach (var entry in archive.Entries)
        {
            token.ThrowIfCancellationRequested();
            if (IsDirectoryEntry(entry))
                continue;
            try { total = checked(total + entry.Length); }
            catch (OverflowException ex)
            {
                throw new InvalidDataException("Velikost herního balíku je neplatná.", ex);
            }
        }
        return total;
    }

    private static void ExtractZipSafely(Stream packageStream, string destination, CancellationToken token)
    {
        packageStream.Position = 0;
        var root = Path.GetFullPath(destination);
        var rootWithSeparator = Path.TrimEndingDirectorySeparator(root) + Path.DirectorySeparatorChar;
        using var archive = new ZipArchive(packageStream, ZipArchiveMode.Read, leaveOpen: true);
        var seenPaths = new HashSet<string>(StringComparer.OrdinalIgnoreCase);

        foreach (var entry in archive.Entries)
        {
            token.ThrowIfCancellationRequested();
            var normalized = ValidateArchiveEntry(entry);
            if (normalized is null)
                continue;

            var target = Path.GetFullPath(Path.Combine(root, normalized.Replace('/', Path.DirectorySeparatorChar)));
            if (!target.StartsWith(rootWithSeparator, StringComparison.OrdinalIgnoreCase))
                throw new InvalidDataException("ZIP obsahuje cestu mimo instalační složku.");

            if (!seenPaths.Add(normalized.TrimEnd('/')))
                throw new InvalidDataException("ZIP obsahuje duplicitní nebo kolidující cesty.");

            if (IsDirectoryEntry(entry))
            {
                Directory.CreateDirectory(target);
                continue;
            }

            var directory = Path.GetDirectoryName(target)!;
            Directory.CreateDirectory(directory);
            EnsureNoReparsePoint(root, directory);

            using var source = entry.Open();
            using var output = new FileStream(target, FileMode.CreateNew, FileAccess.Write, FileShare.None, 1024 * 1024);
            var buffer = new byte[1024 * 1024];
            long copied = 0;
            int read;
            while ((read = source.Read(buffer, 0, buffer.Length)) > 0)
            {
                token.ThrowIfCancellationRequested();
                copied = checked(copied + read);
                if (copied > entry.Length)
                    throw new InvalidDataException("Rozbalený soubor překročil deklarovanou velikost.");
                output.Write(buffer, 0, read);
            }
            if (copied != entry.Length)
                throw new InvalidDataException("Rozbalený soubor neodpovídá deklarované velikosti.");
        }
    }

    private static string? ValidateArchiveEntry(ZipArchiveEntry entry)
    {
        var name = entry.FullName.Replace('\\', '/');
        if (string.IsNullOrWhiteSpace(name))
            throw new InvalidDataException("ZIP obsahuje prázdnou cestu.");
        if (name.StartsWith('/') || Path.IsPathRooted(name) ||
            (name.Length >= 2 && char.IsLetter(name[0]) && name[1] == ':'))
            throw new InvalidDataException("ZIP obsahuje absolutní cestu.");

        var isDirectory = IsDirectoryEntry(entry);
        var trimmed = isDirectory ? name.TrimEnd('/') : name;
        var segments = trimmed.Split('/');
        if (segments.Length == 0 || segments.Any(segment =>
                segment.Length == 0 || segment is "." or ".." ||
                segment.Contains(':') || segment.Any(char.IsControl) ||
                segment.IndexOfAny(Path.GetInvalidFileNameChars()) >= 0))
            throw new InvalidDataException("ZIP obsahuje neplatnou nebo nebezpečnou cestu.");

        // Unix symlinks and Windows reparse-point entries are not accepted.
        var unixType = (entry.ExternalAttributes >> 16) & 0xF000;
        var dosAttributes = entry.ExternalAttributes & 0xFFFF;
        if (unixType == 0xA000 || (dosAttributes & 0x0400) != 0)
            throw new InvalidDataException("ZIP obsahuje symbolický odkaz nebo reparse point.");

        return string.Join('/', segments) + (isDirectory ? "/" : "");
    }

    private static bool IsDirectoryEntry(ZipArchiveEntry entry) =>
        entry.FullName.EndsWith('/') || entry.FullName.EndsWith('\\');

    private static string ResolveContainedPath(string root, string relativePath)
    {
        var fullRoot = Path.TrimEndingDirectorySeparator(Path.GetFullPath(root));
        var fullPath = Path.GetFullPath(Path.Combine(fullRoot,
            relativePath.Replace('/', Path.DirectorySeparatorChar).Replace('\\', Path.DirectorySeparatorChar)));
        if (!fullPath.StartsWith(fullRoot + Path.DirectorySeparatorChar, StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("Cesta ke spustitelnému souboru opouští instalační složku.");
        return fullPath;
    }

    private static void EnsureNoReparsePoint(string root, string directory)
    {
        var current = directory;
        var fullRoot = Path.TrimEndingDirectorySeparator(Path.GetFullPath(root));
        while (current.Length >= fullRoot.Length)
        {
            if ((File.GetAttributes(current) & FileAttributes.ReparsePoint) != 0)
                throw new InvalidDataException("Instalační cesta obsahuje nepovolený symbolický odkaz.");
            if (string.Equals(current, fullRoot, StringComparison.OrdinalIgnoreCase))
                return;
            current = Directory.GetParent(current)?.FullName
                ?? throw new InvalidDataException("Nelze ověřit instalační cestu.");
        }
    }
}
