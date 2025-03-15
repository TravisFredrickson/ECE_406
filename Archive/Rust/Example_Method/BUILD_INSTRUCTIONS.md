# Install

```
. ~/export-esp.sh && . ~/esp/v5.4/esp-idf/export.sh
```

```
cargo generate --vcs none --git https://github.com/esp-rs/esp-idf-template cmake
```
- **Project name:** `my-esp-idf-template`.
- **Rust toolchain:** `esp`.

```
cd ./my-esp-idf-template/
```

```
idf.py set-target esp32c6 && idf.py build
```

# Build

```
. ~/export-esp.sh && . ~/esp/v5.4/esp-idf/export.sh && idf.py set-target esp32c6 && idf.py build
```

