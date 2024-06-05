# xnbcli
xnbcli is a tool to convert .xnb files back to their original format and vice-versa.

Unlike other tools out there, xnbcli can take advantage of special flags and compress files, as specified by the [XNB format](https://github.com/SimonDarksideJ/XNAGameStudio/wiki/Compiled-(XNB)-Content-Format).

## Libraries
XNB files can be compressed or uncompressed. The compression algorithm used is [LZXD](https://msdn.microsoft.com/en-us/library/cc483133%28v=exchg.80%29.aspx), which was invented by Microsoft.

I was unable to find any source code for the LZXD compression, that's why I used xcompress.lib (which depends on legacy_stdio_definitions.lib).
These are most likely proprietary libraries so I don't want to include them in my project.

If you want to create compressed XNB files or contribute to this project, then you have to find these libraries somewhere.
Once you downloaded the required libraries, move them inside the `libs` folder and build from source.

## Supported files
Currently my tool supports the following types:
* Texture2D (.png)
* Sound (.wav)

Other formats will be added in future.

## Download
You can get the latest version from the [releases page](https://github.com/Odex64/xnbcli/releases).

It is a tiny, self-contained executable. Just drag & drop any supported file you want to convert.
