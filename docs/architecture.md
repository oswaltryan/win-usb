# Architecture

`win-usb` is organized by module:

- `common`: Shared data structures and utility helpers.
- `devnode`: Device node property extraction.
- `enumerate`: Top-level enumeration workflow.
- `storage`: Disk, volume, and drive-letter resolution.
- `topology`: USB topology and connection attributes.
- `json_emit`: JSON serialization of collected device data.
