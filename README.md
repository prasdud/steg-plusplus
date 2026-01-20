
# Steg-PlusPlus

Steg-PlusPlus is a C++ steganography toolkit for encoding and decoding data inside PNG images. It provides a set of command-line utilities (workers) for various data transformations, including embedding, extraction, and format conversion (PDF, binary, RGB, PNG).

---

## Status & Development Notes

- **Status:** Finished project
- **Maintenance:** Not actively maintained
- **Future:** May be ported to Go in the future

---

## Usage

**Note:** There is no GUI. All functionality is provided via command-line workers in the `src/workers` directory. Each worker is a standalone executable and must be run individually.

### How to Use

1. Build the desired worker (e.g., with `g++ worker.cpp -o worker`)
2. Run the worker from the command line, providing required input files as needed
3. Refer to the source code for specific input/output file paths and formats

### Available Workers

- `convert_binary_to_pdf.cpp` — Converts binary data to PDF
- `convert_pdf_to_binary.cpp` — Converts PDF to binary
- `convert_png_to_rgb.cpp` — Converts PNG image to RGB CSV
- `convert_rgb_to_png.cpp` — Converts RGB CSV to PNG
- `embedder.cpp` — Embeds binary data into RGB data (LSB steganography)
- `extractor.cpp` — Extracts embedded binary data from RGB data

---

## File Structure

```
assets/           # Example images, PDFs, and output files
data/             # Example data files (text, binary, hex)
src/
	workers/        # All CLI worker programs (see above)
	*.csv, *.cpp    # Supporting code and data
```

---

## License

This project is licensed under the MIT License. See `LICENSE` for details.