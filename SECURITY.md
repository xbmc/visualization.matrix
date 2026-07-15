# Security Policy for visualization.matrix

## 🔒 Reporting a Vulnerability

If you discover a **security vulnerability** in the **visualization.matrix** addon, we encourage you to report it to us **responsibly**.

### ✅ How to Report
Please **do not** open a public GitHub issue for security vulnerabilities. Instead:

1. **Send an email** to:
   **`security@marcelraschke.de`**
   - Include a **detailed description** of the vulnerability.
   - Provide **steps to reproduce** the issue.
   - If possible, include a **proof-of-concept (PoC)** or exploit code.

2. **Alternative**: If email is not feasible, you may open a **private GitHub Security Advisory** for this repository.

### ⏳ Response Time
- We will **acknowledge** your report within **48 hours**.
- We will **investigate** and provide a **fix or mitigation** as soon as possible.
- We aim to **release a patch** within a **reasonable timeframe**, depending on the severity of the issue.

### 🎯 Scope
This security policy applies **only** to the **visualization.matrix** addon for Kodi.

- **In Scope**:
  - The `visualization.matrix` addon source code.
  - Dependencies bundled with the addon (e.g., `kissfft`).

- **Out of Scope**:
  - The **Kodi media center** itself (report to [Kodi Security](https://kodi.tv/about/security)).
  - Third-party forks or modified versions of this addon.

### 🔐 Responsible Disclosure
We follow the principle of **Responsible Disclosure**:

- **Do not** publicly disclose the vulnerability until we have **released a fix**.
- **Do not** exploit the vulnerability for malicious purposes.
- **Do not** perform **denial-of-service (DoS) attacks** or other disruptive testing without prior approval.

### 🏆 Recognition
- We will **acknowledge** your contribution in the **release notes** (unless you prefer to remain anonymous).
- This project does **not** currently offer a **bug bounty program**, but we appreciate your responsible disclosure.

### 📜 Legal
By reporting a vulnerability, you agree to:
- Not share the vulnerability details publicly before a fix is released.
- Not use the vulnerability for any unauthorized or illegal purposes.

---

## 🛡️ Security Features

This project uses the following security tools to ensure code safety:

- **[GitHub CodeQL](https://codeql.github.com/)**: Static analysis for C++ to detect vulnerabilities like buffer overflows, memory leaks, and injection flaws.
- **[Dependabot](https://docs.github.com/en/code-security/dependabot)**: Automated dependency scanning for known vulnerabilities.
- **[clang-tidy](https://clang.llvm.org/extra/clang-tidy/)**: Static analysis for C++ code quality and security.
- **[cppcheck](https://cppcheck.sourceforge.io/)**: Additional static analysis for C/C++ code.

---

## 📅 Last Updated
This security policy was last updated on **July 15, 2025**.
