# Releasing CPU Scheduler (Windows Portable)

This project is configured to publish a Windows portable `.zip` on every pushed tag matching `v*.*.*`.

## Prerequisites

- GitHub Actions must be enabled for the repository.
- The workflow file `.github/workflows/release-windows.yml` must exist on `main`.

## Release Steps

1. Sync and update `main`:

```bash
git checkout main
git pull origin main
```

2. Create and push a semantic version tag:

```bash
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0
```

3. Wait for the `Release Windows Portable` workflow to complete in GitHub Actions.

4. Open the generated GitHub Release and download:

- `CPUScheduler-portable-win64-msvc.zip`

## Versioning Rule

Use tags in this format:

- `vMAJOR.MINOR.PATCH`
- Examples: `v1.0.0`, `v1.1.3`, `v2.0.0`

## What Gets Published

The release workflow builds in `Release` mode on `windows-latest`, runs `windeployqt`, and uploads a portable package containing:

- `CPUScheduler.exe`
- required Qt runtime DLLs
- `platforms/qwindows.dll`

The same `.zip` is available in two places:

- GitHub Release asset for the tag
- Actions run artifact (`CPUScheduler-portable-win64-msvc`)

## Troubleshooting

- If no release is created, confirm the pushed tag matches `v*.*.*`.
- If the workflow fails at Qt install, re-run the job and verify GitHub Actions service status.
- If the app fails with platform plugin errors, confirm `windeployqt` step completed successfully in the workflow logs.
