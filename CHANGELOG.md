# Changelog

All notable changes to this repository should be documented in this file.

The format is intentionally simple and manual.

## Unreleased

### Added
- Repository governance baseline:
  - PR template
  - issue templates
  - `CODEOWNERS`
  - Dependabot configuration
- Build validation baseline:
  - dedicated `validate` workflow path
  - `ci/validate_build.sh`
  - repo-scoped style check script
  - fixed `rebuild_all.sh` Python invocation

### Changed
- Default branch protection now requires pull requests, one approval, and the `validate` status check
- GitHub Actions permissions are being tightened toward least privilege
