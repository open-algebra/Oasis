# Embed
Embeds the contents of a file directly into a library.

## How to use
1. Include the CMake module: `include(/path/to/Embed/Embed.cmake)`.
2. Call the embed_file function: `embed_file(FILEPATH /path/to/foo.txt)`.
3. Link to the resulting library `target_link_libraries(YourTarget PUBLIC foo_txt)`.
4. Include the library in your code: `#include "foo.txt.hpp"`.
5. Use the library in your code: `foo_txt::get().data()`.

See the following for more details.

### Functions
#### `embed_file(FILEPATH <filepath>  [OUTPUT_INCLUDE_NAME <output_include>] [OUTPUT_SOURCE_NAME <output_source>])`
The `embed_file` function embeds the file at `filepath` and produces a library of similar name to filename, replacing `.` with `_`. For example, `/path/to/foo.txt` will be linkable by `foo_txt`.
##### `filepath`
The path of the file to embed.
##### `output_include`
The name of the generated include file. If unspecified, it will be the filename, without the prepending directories, with `.hpp` appended.
##### `output_source`
The name of the generated source file. If unspecified, it will be the filename, without the prepending directories, with `.cpp` appended.