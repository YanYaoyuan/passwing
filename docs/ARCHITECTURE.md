# PassWing Architecture

PassWing is a C++17/Qt desktop application organized by product capability.
Headers live beside their implementations because the project currently builds
a single application rather than a reusable SDK.

## Source layout

```text
src/
  app/        Application entry point and main window composition
  airfoil/    Airfoil design, analysis, optimization, and XFoil integration
  wing/       Wing geometry, VLM analysis, and optimization
  aircraft/   Aircraft composition, stability, and whole-aircraft analysis
  propeller/  Propeller design, libraries, BEMT, and VLM analysis
  geometry/   Geometry libraries and VTK model loading
  dynamics/   Dynamic-system models
  chat/       Chat user interface
  common/     Shared data structures, math, files, and plotting infrastructure
  widgets/    Reusable Qt widgets
  help/       Desktop help integration
```

Runtime assets remain outside `src/`:

```text
resoure/               Qt resources and styles (legacy spelling retained)
libaries/              Airfoil and propeller datasets (legacy spelling retained)
setting/               Runtime configuration
help/                  HTML help content and media
theoreticalFramework/  Theory documentation
```

The legacy directory names are retained for runtime compatibility. New C++
source must be added below `src/`, and new data-loading code should resolve
paths from `QCoreApplication::applicationDirPath()` instead of the process
working directory.

## Dependency direction

The intended dependency direction is:

```text
app -> feature modules -> common/widgets
aircraft/propeller     -> wing/airfoil -> common
```

New code should avoid introducing dependencies in the reverse direction.
Shared types belong in `common/`; UI-only helpers belong in `widgets/`.

## Build organization


- The root `CMakeLists.txt` owns project options only.
- `cmake/PassWingDependencies.cmake` owns third-party discovery and linking.
- `cmake/PassWingProjectOptions.cmake` owns C++17 and compiler diagnostics.
- Every source module owns its source manifest in a local `CMakeLists.txt`.
- Machine-specific dependency paths belong in untracked
  `CMakeUserPresets.json`, never in committed CMake files.

## Incremental modernization rules

Existing class names are intentionally unchanged in this structural pass to
avoid mixing behavior changes with file moves. For new and touched code:

1. Prefer RAII values and smart pointers over owning raw pointers.
2. Use `nullptr`, `override`, `const`, and scoped enums where applicable.
3. Keep declarations small; move implementation details out of headers.
4. Use type-safe Qt signal/slot connections instead of string-based macros.
5. Add tests before changing numerical algorithms or solver behavior.
6. Do not add another copy of a vendored dependency.

Some legacy implementation files remain outside the active target because they
duplicate header-only implementations or another vendored QCustomPlot copy.
Their status is documented in the nearest module `CMakeLists.txt`.
