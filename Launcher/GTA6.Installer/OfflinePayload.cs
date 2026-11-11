using System.Buffers.Binary;
using System.Text;

namespace GTA6.Installer;

/// <summary>
/// Reads a ZIP64 payload appended to the installer executable.
/// File layout: [self-contained installer][ZIP payload][UTF-8 manifest][20-byte footer].
/// Footer: 8-byte magic, Int64 payload length, Int32 manifest length (little-endian).
/// </summary>
internal sealed class OfflinePayload : IDisposable
{
    private static readonly byte[] Magic = Encoding.ASCII.GetBytes("GTA6PAY1");
    private const int FooterLength = 20;
    private const int MaximumManifestLength = 1024 * 1024;
    private readonly FileStream _file;
    private readonly BoundedReadStream _package;

    public ReleaseManifest Manifest { get; }
    public Stream PackageStream => _package;

    private OfflinePayload(FileStream file, BoundedReadStream package, ReleaseManifest manifest)
    {
        _file = file;
        _package = package;
        Manifest = manifest;
    }

    public static OfflinePayload Open(string executablePath)
    {
        var file = new FileStream(executablePath, FileMode.Open, FileAccess.Read, FileShare.Read,
            1024 * 1024, FileOptions.RandomAccess);
        try
        {
            if (file.Length < FooterLength)
                throw new InvalidDataException("Instalátor neobsahuje offline herní balík.");

            file.Position = file.Length - FooterLength;
            Span<byte> footer = stackalloc byte[FooterLength];
            file.ReadExactly(footer);
            if (!footer[..8].SequenceEqual(Magic))
                throw new InvalidDataException("Závěrečný podpis offline balíku není platný.");

            var payloadLength = BinaryPrimitives.ReadInt64LittleEndian(footer.Slice(8, 8));
            var manifestLength = BinaryPrimitives.ReadInt32LittleEndian(footer.Slice(16, 4));
            if (payloadLength <= 0 || manifestLength <= 0 || manifestLength > MaximumManifestLength)
                throw new InvalidDataException("Délka offline balíku nebo manifestu je neplatná.");

            var manifestOffset = checked(file.Length - FooterLength - manifestLength);
            var payloadOffset = checked(manifestOffset - payloadLength);
            if (payloadOffset < 0)
                throw new InvalidDataException("Offline balík přesahuje hranice instalačního souboru.");

            file.Position = manifestOffset;
            var manifestBytes = new byte[manifestLength];
            file.ReadExactly(manifestBytes);
            using var manifestStream = new MemoryStream(manifestBytes, writable: false);
            var manifest = ReleaseManifest.Load(manifestStream);

            var package = new BoundedReadStream(file, payloadOffset, payloadLength);
            return new OfflinePayload(file, package, manifest);
        }
        catch
        {
            file.Dispose();
            throw;
        }
    }

    public void Dispose() => _file.Dispose();

    private sealed class BoundedReadStream : Stream
    {
        private readonly Stream _inner;
        private readonly long _start;
        private readonly long _length;
        private long _position;

        public BoundedReadStream(Stream inner, long start, long length)
        {
            _inner = inner;
            _start = start;
            _length = length;
        }

        public override bool CanRead => true;
        public override bool CanSeek => true;
        public override bool CanWrite => false;
        public override long Length => _length;
        public override long Position
        {
            get => _position;
            set => Seek(value, SeekOrigin.Begin);
        }

        public override int Read(byte[] buffer, int offset, int count)
        {
            ArgumentNullException.ThrowIfNull(buffer);
            if (offset < 0 || count < 0 || offset > buffer.Length - count)
                throw new ArgumentOutOfRangeException();
            var remaining = _length - _position;
            if (remaining <= 0)
                return 0;
            var allowed = (int)Math.Min(count, remaining);
            _inner.Position = _start + _position;
            var read = _inner.Read(buffer, offset, allowed);
            _position += read;
            return read;
        }

        public override int Read(Span<byte> buffer)
        {
            var remaining = _length - _position;
            if (remaining <= 0)
                return 0;
            var allowed = (int)Math.Min(buffer.Length, remaining);
            _inner.Position = _start + _position;
            var read = _inner.Read(buffer[..allowed]);
            _position += read;
            return read;
        }

        public override ValueTask<int> ReadAsync(Memory<byte> buffer, CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            return ValueTask.FromResult(Read(buffer.Span));
        }

        public override long Seek(long offset, SeekOrigin origin)
        {
            var next = origin switch
            {
                SeekOrigin.Begin => offset,
                SeekOrigin.Current => checked(_position + offset),
                SeekOrigin.End => checked(_length + offset),
                _ => throw new ArgumentOutOfRangeException(nameof(origin))
            };
            if (next < 0 || next > _length)
                throw new IOException("Pokus o čtení mimo hranice herního balíku.");
            _position = next;
            return _position;
        }

        public override void Flush() { }
        public override void SetLength(long value) => throw new NotSupportedException();
        public override void Write(byte[] buffer, int offset, int count) => throw new NotSupportedException();
    }
}
