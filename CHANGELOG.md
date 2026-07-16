# Changelog

All notable changes to the **visualization.matrix** addon will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## 📌 Unreleased

### ⚠️ Security
- Added comprehensive security framework with 11 analysis tools
- Added `SECURITY.md` with vulnerability reporting guidelines
- Added `DEPENDENCY_POLICY.md` for dependency management

### ✨ Features
- Added static analysis with CodeQL, clang-tidy, cppcheck, and SonarQube
- Added dynamic analysis with OWASP ZAP
- Added vulnerability scanning with Trivy and Snyk
- Added fuzzing with AFL++ for kissfft library
- Added SBOM generation with Syft (SPDX and CycloneDX)
- Added Kodi-specific validation checks

### 📝 Documentation
- Added `CONTRIBUTING.md` with contribution guidelines
- Added `CODE_OF_CONDUCT.md` based on Contributor Covenant v1.4
- Added issue templates for bug reports, feature requests, and security vulnerabilities
- Added pull request template with security checklist
- Updated `README.md` with security information

### 🔧 CI/CD
- Added 13 GitHub Actions workflows for comprehensive CI/CD
- Added caching for all workflows to improve performance
- Added path filtering to reduce CI/CD costs
- Added CODEOWNERS and labeler for repository management

### 🐛 Bug Fixes
- None in this release

### 🧹 Maintenance
- None in this release

---

## 📅 Release History

### [1.0.0] - 2025-07-15
#### ✨ Initial Release
- First stable release of visualization.matrix addon
- Basic visualization functionality for Kodi
- Support for OpenGL rendering

---

## 📊 Types of Changes

- **Added**: for new features.
- **Changed**: for changes in existing functionality.
- **Deprecated**: for soon-to-be removed features.
- **Removed**: for now removed features.
- **Fixed**: for any bug fixes.
- **Security**: in case of vulnerabilities.

---

## 🔖 Versioning

This project uses **Semantic Versioning** (`MAJOR.MINOR.PATCH`):

- **MAJOR**: Breaking changes, incompatible API modifications
- **MINOR**: Backwards-compatible new features
- **PATCH**: Backwards-compatible bug fixes

---

## 📝 How to Contribute

See [CONTRIBUTING.md](CONTRIBUTING.md) for contribution guidelines.

---

## 🛡️ Security

See [SECURITY.md](SECURITY.md) for vulnerability reporting.
