# Contributing to NekoWM

Thank you for considering contributing to NekoWM. This window manager is built with a minimal and standalone philosophy: no config files, no runtime dependencies, and fast builds.

## General Guidelines

- **Minimalism First**: All features should align with the minimal and standalone goal of NekoWM.
- **One Binary**: Avoid external runtimes or config file parsers. All functionality must remain in a single compiled binary.
- **Performance**: Code should be efficient and clean. Avoid unnecessary allocations or layers.
- **Style**: Follow the existing coding style:
  - 2-space indentation
  - `function() {}` over `function () {}`
  - Ideally follow the allman style, though it's not enforced
  - No C++ or external dependencies
- **Commits**: Write meaningful commit messages. Keep them concise but descriptive.

## Submitting Changes

1. **Fork the repository** and create your feature or bugfix branch off `dev`.
2. Make your changes, test them, and ensure `make` and `make install` still work.
3. Open a **pull request to the `dev` branch**.
4. Clearly describe what you’ve changed and why. If your change impacts behavior or design, explain the reasoning.

## Pull Request Requirements

- Target branch: `dev`
- Do **not** open PRs against `master`. Only maintainers merge into `master`.
- Ensure builds pass using GitHub Actions.
- If you add a new feature, document it briefly in the README if necessary.

## Filing Issues

- Check if your issue already exists.
- Be specific. Include steps to reproduce, expected behavior, and system info if relevant.
- Tag your issue with `[feature]`, `[bug]`, or `[question]`.

## Getting Started

To build and run NekoWM:

```sh
make
sudo make install
````

To set up a session for display managers:

```sh
sudo make session
```

## Licensing

All contributions are under the MIT License.

---

NekoWM is a learning-first project. If you’re unsure about anything, open an issue or draft PR and let’s talk it through.
