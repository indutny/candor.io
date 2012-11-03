#include <stdio.h> // fprintf
#include <stdlib.h> // exit
#include <string.h> // strdup
#include <libgen.h> // basename

int main(int argc, char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: can2c header.h ...scripts...\n");
    exit(1);
  }

  // Open output
  FILE* out;
  out = fopen(argv[1], "w");
  if (out == NULL) {
    fprintf(stderr, "Can\'t open output file: %s\n", argv[2]);
    exit(1);
  }

  // Print headers
  fprintf(out, "#ifndef _GEN_CAN__NATIVES_H_\n"
               "#define _GEN_CAN__NATIVES_H_\n"
               "namespace can {\n");

  char** names = (char**) malloc(sizeof(*names) * argc - 2);
  if (names == NULL) abort();

  for (int i = 0; i < argc - 2; i++) {
    // Open source file
    FILE* in = fopen(argv[i + 2], "r");
    if (in == NULL) {
      fprintf(stderr, "Can\'t open input file: %s\n", argv[i + 2]);
      fclose(out);
      exit(1);
    }

    names[i] = strdup(basename(argv[i + 2]));
    if (names[i] == NULL) abort();

    // Replace non-chars with _ in input filename
    // and '.' with '\0'
    for (int j = 0; names[i][j] != 0; j++) {
      unsigned char c = names[i][j];
      if (c == '.') {
        names[i][j] = 0;
        break;
      } else if (c < 'a' || c > 'z') {
        names[i][j] = '_';
      }
    }

    // Put generated content into the output file
    fprintf(out, "const char can_native_%s[] = {\n", names[i]);

    int offset = 0;
    while (!feof(in)) {
      // Read file
      unsigned char buffer[1024];
      size_t read = fread(buffer, 1, sizeof(buffer), in);
      if (read == 0) {
        fclose(in);
        fclose(out);
        fprintf(stderr, "Failed to read input file: %s\n", argv[i]);
        exit(1);
      }

      // Translate it to byte sequence
      for (size_t i = 0; i < read; i++) {
        fprintf(out, "0x%.2x, ", buffer[i]);
        if (++offset % 20 == 0) fprintf(out, "\n");
      }
    }
    fprintf(out, "0x0\n};\n");
    fclose(in);
  }

  fprintf(out, "struct can_native_s {\n"
               "  const char* name;\n"
               "  const char* source;\n"
               "  size_t size;\n"
               "};\n"
               "static const struct can_native_s can_natives[] = {\n");
  for (int i = 0; i < argc - 2; i++) {
    fprintf(out,
            "  { \"%s\", can_native_%s, sizeof(can_native_%s) - 1 },\n",
            names[i],
            names[i],
            names[i]);
    free(names[i]);
  }
  fprintf(out, "  { NULL, NULL, 0 }\n"
               "};\n"
               "} // namespace can\n"
               "#endif // _GEN_CAN__NATIVES_H_\n");

  fclose(out);
}
