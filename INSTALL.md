# Build and installation instructions

## Compile-time build dependencies

 - cmake (>= 3.13)
 - cmake-extras
 - glib-2.0 (>= 2.58)
 - gobject-introspection
 - vala (>= 0.16)
 - gtk+-3.0 (>= 3.24)
 - libayatana-ido (>=0.8.2)
 - gtest (>= 1.6.0)
 - gcovr (>= 2.4)
 - lcov (>= 1.9)

## For end-users and packagers

```
cd libayatana-indicator-X.Y.Z
mkdir build
cd build
cmake ..
make
sudo make install
```
## For testers - unit tests only

```
cd libayatana-indicator-X.Y.Z
mkdir build
cd build
cmake .. -DENABLE_TESTS=ON
make
make test
```
## For testers - both unit tests and code coverage

```
cd libayatana-indicator-X.Y.Z
mkdir build
cd build
cmake .. -DENABLE_COVERAGE=ON
make
make test
make coverage-html
```
**The install prefix defaults to `/usr`, change it with `-DCMAKE_INSTALL_PREFIX=/some/path`**

**You can build a Gtk 2 version using `-DFLAVOUR_GTK2=ON`**

**You can build a version without Ayatana IDO support using `-DENABLE_IDO=OFF`**
