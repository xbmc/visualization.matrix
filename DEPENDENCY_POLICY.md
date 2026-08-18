# Dependency Policy for visualization.matrix

## 📦 Overview

This document outlines the **dependency management policy** for the `visualization.matrix` addon. It defines guidelines for:
- **Approved dependencies**
- **Dependency updates**
- **Security requirements**
- **License compatibility**
- **Vulnerability management**

---

## 🔒 Approved Dependencies

### Current Dependencies

| Dependency | Version | Purpose | License | Status |
|------------|---------|---------|---------|--------|
| [kissfft](https://github.com/mborgerding/kissfft) | Latest | Fast Fourier Transform | BSD-3-Clause | ✅ Approved |
| [Kodi Addon Framework](https://github.com/xbmc/xbmc) | Matrix/Nexus | Kodi addon infrastructure | GPL-2.0-or-later | ✅ Required |
| OpenGL/ES | System | Graphics rendering | Vendor-specific | ✅ System |
| GLM | Latest | OpenGL Mathematics | MIT | ✅ Approved |

### Dependency Types

1. **Bundled Dependencies** (included in `/lib/`)
   - Must be **header-only** or **statically linked**
   - Must have **compatible licenses** (see [License Compatibility](#license-compatibility))
   - Must be **minimal and well-maintained**

2. **System Dependencies** (linked at runtime)
   - Must be **widely available** on target platforms (Linux, Windows, macOS, Android, iOS)
   - Must have **stable APIs**
   - Must be **version-checked** at runtime

3. **Build Dependencies** (used during compilation)
   - Must be **documented** in `README.md` or `CONTRIBUTING.md`
   - Must be **pinned to specific versions** in CI/CD workflows

---

## 🔄 Dependency Updates

### Update Process

1. **Automated Updates (Dependabot)**
   - Dependabot automatically opens PRs for dependency updates
   - PRs are **reviewed and tested** before merging
   - **Security updates** are prioritized (see [Vulnerability Management](#-vulnerability-management))

2. **Manual Updates**
   - Check for updates **monthly**
   - Test updates in a **separate branch** before merging
   - Document **breaking changes** in `CHANGELOG.md` (if applicable)

3. **Version Pinning**
   - Use **specific versions** (not `latest` or `*`)
   - Pin versions in:
     - `CMakeLists.txt` (for bundled dependencies)
     - CI/CD workflows (for build dependencies)
     - Documentation (for system dependencies)

### Update Criteria

| Criteria | Action |
|----------|--------|
| **Security vulnerability** | Update **immediately** (within 48 hours) |
| **Critical bug fix** | Update **within 1 week** |
| **Minor bug fix** | Update **within 1 month** |
| **New feature** | Evaluate for **compatibility and need** |
| **Breaking change** | Requires **major version bump** of the addon |

---

## 🛡️ Security Requirements

### Vulnerability Management

1. **Scanning**
   - All dependencies are **automatically scanned** with:
     - [Trivy](https://github.com/aquasecurity/trivy) (on every push/PR)
     - [Dependabot](https://docs.github.com/en/code-security/dependabot) (weekly)
     - [GitHub Security Advisories](https://docs.github.com/en/code-security/security-advisories) (continuous)

2. **Response to Vulnerabilities**
   - **Critical/High vulnerabilities**: Must be fixed **within 48 hours**
   - **Medium vulnerabilities**: Must be fixed **within 1 week**
   - **Low vulnerabilities**: Must be fixed **within 1 month**
   - If a fix is **not available**, apply **mitigations** (e.g., disable vulnerable features)

3. **Vulnerability Disclosure**
   - Follow the **responsible disclosure** process in [SECURITY.md](SECURITY.md)
   - Do **not** publicly disclose vulnerabilities before a fix is available

### Dependency Security Checks

Before adding a new dependency, verify:

- [ ] The dependency is **actively maintained** (recent commits, releases)
- [ ] The dependency has **no known vulnerabilities** (check [Snyk](https://snyk.io/), [OSV](https://osv.dev/))
- [ ] The dependency has a **clear security policy**
- [ ] The dependency uses **secure coding practices**

---

## ⚖️ License Compatibility

### Approved Licenses

The `visualization.matrix` addon is licensed under **GPL-2.0-or-later**. Compatible licenses for dependencies include:

| License | Compatibility | Notes |
|---------|---------------|-------|
| **BSD-2-Clause** | ✅ Compatible | Permissive, no restrictions |
| **BSD-3-Clause** | ✅ Compatible | Permissive, no restrictions |
| **MIT** | ✅ Compatible | Permissive, no restrictions |
| **Apache-2.0** | ✅ Compatible | Permissive, patent grant |
| **LGPL-2.1** | ✅ Compatible | Can be dynamically linked |
| **LGPL-3.0** | ✅ Compatible | Can be dynamically linked |
| **GPL-2.0** | ✅ Compatible | Must be statically linked or bundled |
| **GPL-3.0** | ⚠️ Conditional | Requires addon to be GPL-3.0 |
| **AGPL-3.0** | ❌ Incompatible | Requires network use restrictions |
| **Proprietary** | ❌ Incompatible | Closed-source licenses |

### License Review Process

1. **Check License Compatibility**
   - Use [SPDX License List](https://spdx.org/licenses/) for reference
   - Consult [GPL Compatibility Matrix](https://www.gnu.org/licenses/license-list.html)

2. **Document License**
   - Add the dependency's license to `LICENSE-THIRD-PARTY.md` (if applicable)
   - Include a **copy of the license** in `/lib/<dependency>/LICENSE`

3. **Legal Review** (if unsure)
   - Contact the **Kodi legal team** or **project maintainers** for clarification

---

## 📋 Dependency Addition Process

### Steps to Add a New Dependency

1. **Evaluate Need**
   - Is the dependency **necessary**?
   - Can the functionality be **implemented internally**?
   - Is there an **existing approved dependency** that can be used?

2. **Check Compatibility**
   - **License**: Must be compatible with GPL-2.0-or-later
   - **Platform**: Must support all target platforms (Linux, Windows, macOS, Android, iOS)
   - **Architecture**: Must support all target architectures (x86, x86_64, ARM, ARM64)

3. **Security Review**
   - Check for **known vulnerabilities**
   - Review the dependency's **security practices**
   - Ensure the dependency is **actively maintained**

4. **Technical Review**
   - Test the dependency in a **separate branch**
   - Verify **build compatibility** with all target platforms
   - Check for **performance impact**

5. **Documentation**
   - Update `README.md` or `CONTRIBUTING.md` with:
     - **Purpose** of the dependency
     - **Version** requirements
     - **Build instructions** (if applicable)
   - Update `DEPENDENCY_POLICY.md` with the new dependency

6. **Submit for Review**
   - Open a **Pull Request** with the dependency addition
   - Include **justification** for the dependency
   - Tag **@MarcelRaschke** for review

---

## 🗑️ Dependency Removal Process

### Steps to Remove a Dependency

1. **Evaluate Impact**
   - Is the dependency **still needed**?
   - Can the functionality be **replaced** with another dependency or internal code?

2. **Check for Dependencies**
   - Are there **other dependencies** that rely on this one?
   - Are there **features** that will break without it?

3. **Update Code**
   - Remove all **references** to the dependency
   - Replace functionality with **alternatives** (if applicable)

4. **Test**
   - Verify the addon **builds and runs** without the dependency
   - Test on **all target platforms**

5. **Documentation**
   - Update `README.md`, `CONTRIBUTING.md`, and `DEPENDENCY_POLICY.md`
   - Remove the dependency from **CI/CD workflows** (if applicable)

6. **Submit for Review**
   - Open a **Pull Request** with the dependency removal
   - Include **justification** for the removal
   - Tag **@MarcelRaschke** for review

---

## 📊 Dependency Metrics

### Current Dependency Statistics

| Metric | Value |
|--------|-------|
| **Total Dependencies** | 4 |
| **Bundled Dependencies** | 1 (kissfft) |
| **System Dependencies** | 2 (OpenGL/ES, Kodi Framework) |
| **Build Dependencies** | 1 (CMake) |
| **Vulnerable Dependencies** | 0 |
| **Outdated Dependencies** | 0 |

### Dependency Health

- **kissfft**: ✅ Healthy (actively maintained, no vulnerabilities)
- **Kodi Framework**: ✅ Healthy (actively maintained by Kodi team)
- **OpenGL/ES**: ✅ Healthy (vendor-maintained)

---

## 🔄 Automated Dependency Management

### Dependabot Configuration

Dependabot is configured to:
- **Check for updates weekly**
- **Open PRs for version updates**
- **Prioritize security updates**
- **Label PRs** with `dependencies` and `security` (if applicable)

### Trivy Scanning

Trivy is configured to:
- **Scan for vulnerabilities** on every push/PR
- **Report findings** in SARIF format to GitHub Security tab
- **Fail builds** if critical vulnerabilities are found

---

## 📅 Maintenance Schedule

| Task | Frequency | Responsible |
|------|-----------|-------------|
| **Dependency updates** | Monthly | Maintainers |
| **Vulnerability scanning** | Continuous | Automated (Trivy, Dependabot) |
| **Dependency review** | Quarterly | Maintainers |
| **License compliance check** | Annually | Maintainers |

---

## 📞 Contact

For questions about dependencies, contact:
- **Maintainer**: [@MarcelRaschke](https://github.com/MarcelRaschke)
- **Email**: `security@marcelraschke.de`
- **Kodi Forum**: [Kodi Addon Development](https://forum.kodi.tv/forums/addon-development.161/)

---

## 📜 Changelog

| Date | Change | Author |
|------|--------|--------|
| 2025-07-15 | Initial dependency policy created | Vibe Code |
