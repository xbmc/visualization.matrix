# Contributing to visualization.matrix

Thank you for your interest in contributing to **visualization.matrix**! This document provides guidelines to ensure a smooth and secure contribution process.

---

## 📜 Code of Conduct

By participating in this project, you agree to abide by the [Kodi Code of Conduct](https://kodi.tv/about/code-of-conduct/). Be respectful, inclusive, and collaborative.

---

## 🚀 How to Contribute

### Reporting Bugs

1. **Check existing issues**: Search the [GitHub Issues](https://github.com/MarcelRaschke/visualization.matrix/issues) to ensure the bug hasn't already been reported.
2. **Create a new issue**: If the bug is new, open an issue with:
   - A **clear title** describing the problem.
   - A **detailed description** of the issue, including steps to reproduce.
   - **Logs or screenshots** (if applicable).
   - Your **Kodi version** and **operating system**.

### Suggesting Enhancements

1. Open an issue with the **"enhancement"** label.
2. Describe the **feature request** in detail, including:
   - The **use case** for the feature.
   - Any **mockups or examples** (if applicable).

### Submitting Pull Requests

1. **Fork the repository** and create a feature branch (`git checkout -b feature/your-feature`).
2. **Commit your changes** with clear, descriptive messages (follow [Conventional Commits](https://www.conventionalcommits.org/) if possible).
3. **Test your changes**: Ensure the addon builds and works as expected.
4. **Run security checks**: All pull requests are automatically scanned with:
   - [CodeQL](https://codeql.github.com/) (static analysis)
   - [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) (code quality)
   - [cppcheck](https://cppcheck.sourceforge.io/) (static analysis)
   - [Trivy](https://github.com/aquasecurity/trivy) (vulnerability scanning)
5. **Submit the PR**: Open a pull request to the `master` or `Matrix` branch (depending on the target).
6. **Address feedback**: Respond to review comments and update your PR as needed.

---

## 🔧 Development Setup

### Prerequisites

- **CMake** (≥ 3.5)
- **C++ Compiler** (GCC ≥ 7, Clang ≥ 8, or MSVC ≥ 2017)
- **Git**
- **Kodi source code** (for building the addon)

### Building the Addon

Follow the [build instructions in README.md](README.md#build-instructions-for-linux).

---

## 🛡️ Security Guidelines

### Reporting Security Vulnerabilities

**Do not** report security vulnerabilities in public issues or pull requests. Instead, follow the instructions in **[SECURITY.md](SECURITY.md)**.

### Secure Coding Practices

1. **Avoid unsafe functions**:
   - Use `strncpy` instead of `strcpy`.
   - Use `snprintf` instead of `sprintf`.
   - Use `memcpy_s` or bounds-checked alternatives where available.

2. **Input validation**:
   - Validate all user inputs and external data.
   - Use safe parsing libraries (e.g., avoid manual string parsing).

3. **Memory management**:
   - Avoid memory leaks (use smart pointers where possible).
   - Check for `nullptr` before dereferencing pointers.
   - Use bounds checking for array access.

4. **Error handling**:
   - Handle errors gracefully (avoid crashes or undefined behavior).
   - Use assertions for internal consistency checks.

5. **No hardcoded secrets**:
   - Never commit API keys, passwords, or tokens to the repository.
   - Use environment variables or secure configuration files.

### Security Tools

This project uses the following tools to enforce security:

| Tool | Purpose | When It Runs |
|------|---------|--------------|
| [CodeQL](https://codeql.github.com/) | Static analysis for C++ vulnerabilities | Push, Pull Request, Weekly |
| [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) | Code quality and style checks | Push, Pull Request |
| [cppcheck](https://cppcheck.sourceforge.io/) | Additional static analysis | Push, Pull Request |
| [Trivy](https://github.com/aquasecurity/trivy) | Vulnerability scanning | Push, Pull Request, Weekly |
| [Dependabot](https://docs.github.com/en/code-security/dependabot) | Dependency updates | Weekly |
| [Syft](https://github.com/anchore/syft) | SBOM generation | Push, Pull Request, Release |
| [AFL++](https://github.com/AFLplusplus/AFLplusplus) | Fuzzing for `kissfft` | Push, Pull Request, Manual |

---

## 📝 Coding Standards

### C++ Style

- Follow **modern C++** practices (C++17 or later).
- Use **RAII** (Resource Acquisition Is Initialization) for resource management.
- Prefer **`const` correctness** (mark variables and methods `const` where possible).
- Use **meaningful names** for variables, functions, and classes.
- Avoid **global variables** (use singletons or dependency injection if needed).

### Formatting

- **Indentation**: 4 spaces (no tabs).
- **Braces**: K&R style (opening brace on the same line).
- **Line length**: ≤ 120 characters.
- **Comments**: Use `//` for single-line comments and `/* */` for multi-line.

Example:
```cpp
// Good
if (condition) {
    doSomething();
}

// Bad
if(condition)
{
    doSomething();
}
```

### Commit Messages

- Use **imperative mood** (e.g., "Fix bug" instead of "Fixed bug").
- Keep the **subject line ≤ 50 characters**.
- Provide a **detailed body** if the change is complex.
- Reference **issues or PRs** (e.g., "Closes #123").

Example:
```
Fix buffer overflow in FFT processing

- Replace unsafe memcpy with bounds-checked alternative
- Add input validation for sample size
- Closes #456
```

---

## 🧪 Testing

### Manual Testing

1. Build the addon and install it in Kodi.
2. Test the visualization with various audio sources.
3. Verify that the addon does not crash or produce artifacts.

### Automated Testing

- **Static analysis**: All PRs are automatically scanned with CodeQL, clang-tidy, and cppcheck.
- **Fuzzing**: The `kissfft` library is fuzzed with AFL++ to detect edge cases.
- **Vulnerability scanning**: Trivy scans for known vulnerabilities in dependencies.

---

## 📦 Releasing

Releases are managed by the **Kodi addon maintainers**. To propose a release:

1. Ensure all changes are merged into the target branch (e.g., `Matrix`).
2. Update the **version number** in `addon.xml.in`.
3. Open a PR with the changes and request a review from maintainers.

---

## 🤝 Community

- **Discord**: Join the [Kodi Discord](https://kodi.tv/article/kodi-discord-server) for discussions.
- **Forum**: Participate in the [Kodi Forum](https://forum.kodi.tv/).
- **Issues**: Use [GitHub Issues](https://github.com/MarcelRaschke/visualization.matrix/issues) for bug reports and feature requests.

---

## 📄 License

By contributing to this project, you agree to license your contributions under the **[GPL-2.0-or-later](LICENSE.md)**.

---

## 🙏 Acknowledgments

Thank you for contributing to **visualization.matrix**! Your efforts help improve the addon for the entire Kodi community.
