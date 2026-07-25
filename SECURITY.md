# Security Policy

## Reporting a Vulnerability

If you discover a security vulnerability in FractalAura, please email **security@auradsp.dev** instead of using the public issue tracker.

Please include:
- Description of the vulnerability
- Steps to reproduce (if applicable)
- Potential impact
- Suggested fix (if any)

## Supported Versions

Only the latest version of FractalAura receives security updates.

| Version | Status | Support Until |
|---------|--------|---------------|
| 1.0.x   | Active | Current |
| < 1.0   | EOL    | Not supported |

## Security Considerations

### For Users
- Only install FractalAura from official GitHub releases
- Keep macOS and your DAW updated
- Be cautious of unofficial builds or forks

### For Developers
- Run with `CMake 3.22+` for known security patches
- Use latest JUCE version (git submodule is maintained)
- Avoid using deprecated JUCE functions
- Run static analysis tools regularly

## Known Security Issues

None currently reported.
