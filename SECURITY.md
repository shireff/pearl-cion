# Security Policy

## Scope

This policy covers all components in the Pearl monorepo:

- **pearld** — full node (`node/`)
- **Oyster** — wallet daemon (`wallet/`)
- **SPV client** (`spv/`)
- **ZK proof-of-work** circuits and verifier (`zk-pow/`, `plonky2/`)
- **Mining infrastructure** (`miner/`, `py-pearl-mining/`)
- **XMSS** post-quantum signatures (`xmss/`)
- **DNS seeder** (`dnsseeder/`)
- **Frontend applications** (`apps/`)

## Supported Versions

Only the latest release is actively supported. Critical fixes may be
backported to prior releases at the team's discretion.

## Reporting a Vulnerability

**Do not open a public issue for security vulnerabilities.**

Use [GitHub's private vulnerability reporting](https://github.com/pearl-research-labs/pearl/security/advisories/new)
to submit a report. Include:

- Description of the vulnerability
- Steps to reproduce or a proof-of-concept
- Affected component(s) and version(s)
- Potential impact

## Disclosure Policy

We follow coordinated disclosure. Please allow reasonable time from the
initial report before publicly disclosing any findings, so we have time to
develop and release a fix. We will credit reporters in the release notes
unless anonymity is requested.

## Contact

- [Report a vulnerability](https://github.com/pearl-research-labs/pearl/security/advisories/new)
- Website: https://pearlresearch.ai
