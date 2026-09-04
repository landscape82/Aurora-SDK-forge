# Contributing

This repository is maintained with a pull-request-first workflow.

## Branches

- Create feature branches from `main`
- Use the `forge-aurora-...` naming convention for Aurora-specific work
- Keep branch names short and descriptive

Examples:

- `forge-aurora-improve-build`
- `forge-aurora-hardware-smoke-test`
- `forge-aurora-custom-template`

## Commits

Use short commit messages in one of these forms:

- `Adding ...`
- `Fix ...`
- `Update ...`
- `Refactor ...`

Examples:

- `Adding build validation`
- `Fix callback restart handling`
- `Update repository governance`

## Pull Requests

Open a pull request for every change to `main`.

Use short PR titles that match the commit style:

- `Adding ...`
- `Fix ...`

Use the repository PR template:

```md
# Summary
- 

# What will be applied / fixed
- 

# Test steps
- 
```

## Local Validation

Before opening a PR, run what is available locally:

```sh
./ci/validate_build.sh
```

If `clang-format` is installed, also run:

```sh
./ci/local_style_check.sh
```

## Notes

- Keep changes focused; avoid mixing repo governance and firmware features in one PR
- If a workflow or script references paths that do not exist in this repo, fix the script before enforcing it
- Document any hardware-specific validation steps in the PR under `# Test steps`
