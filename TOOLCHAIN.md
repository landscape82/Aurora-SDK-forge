# Toolchain & Dependency Versions

This file is the single source of truth for exact tool and library versions used to build
this repository. Keep it in sync whenever any of these are bumped, so local development
always matches what CI (`.github/workflows/main.yml`, `release.yml`) actually builds with.

## Build Toolchain

| Tool | Version | Pinned in |
|---|---|---|
| `arm-none-eabi-gcc` | `15.3.Rel1` (Arm GNU Toolchain) | `.github/workflows/main.yml`, `.github/workflows/release.yml` (`carlosperate/arm-none-eabi-gcc-action@v1`) |
| Python | `3.11.16` | `.github/workflows/main.yml`, `.github/workflows/release.yml` (`actions/setup-python@v5`) |

### Local Setup (macOS)

```sh
brew install --cask gcc-arm-embedded   # installs the current Arm GNU Toolchain release
arm-none-eabi-gcc --version            # confirm it reports 15.3.Rel1 (or update this file if CI has moved on)
```

If your local `arm-none-eabi-gcc` version differs from the one pinned above, builds should
still generally succeed, but exact binary output and any toolchain-specific warnings may
differ from CI. Match the pinned version above for full reproducibility.

## Firmware Libraries (git submodules)

These are pinned by git submodule commit reference (`.gitmodules` + the gitlink recorded in
this repo's tree), so every clone with `--recurse-submodules` gets the exact same commit
automatically. Recorded here for visibility without needing to inspect submodule state
directly.

| Submodule | Commit | Nearest tag |
|---|---|---|
| `libs/libDaisy` | `63fcabd38a20e14bc744499f0460e47925ea753e` | `v5.1.0` + 9 commits |
| `libs/DaisySP` | `4263388e7fa8dfd34fa85c6a1c697362dc6981c7` | `v0.0.1` + 20 commits |

Both submodules are pinned a few commits past their last tagged release, not exactly on a
tag. This was a deliberate choice to keep the currently-verified, working commits rather
than move backward to the exact tags, which would need a full rebuild and re-verification
pass. If you bump either submodule, update this table in the same PR.

To verify what's actually checked out locally:

```sh
git submodule status
```

## GitHub Actions Dependencies

Tracked automatically by Dependabot (`.github/dependabot.yml`, `github-actions` ecosystem,
weekly). Current pins, for reference:

| Action | Version |
|---|---|
| `actions/checkout` | `v4` |
| `actions/setup-python` | `v5` |
| `actions/upload-artifact` | `v4` |
| `carlosperate/arm-none-eabi-gcc-action` | `v1` |
| `softprops/action-gh-release` | `v2` |
| `mattnotmitt/doxygen-action` | `v1` |
| `peaceiris/actions-gh-pages` | `v3` |

Dependabot does **not** track the `libDaisy`/`DaisySP` submodule pins or the
`arm-none-eabi-gcc` toolchain version above — those are only bumped manually.
