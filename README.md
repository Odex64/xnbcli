# xnbcli
xnbcli is a *tiny* and *fast* tool to convert .xnb files back to their original format and vice-versa.

The XNB format was originally developed for the [XNA framework](https://en.wikipedia.org/wiki/Microsoft_XNA), but it's still used nowadays by [FNA](https://github.com/FNA-XNA/FNA) and [MonoGame](https://github.com/MonoGame/MonoGame).

## Motivation
There are mainly two reasons why I wrote this program
1. Most tools to convert .xnb files have one major flaw; they can't compress. It happened to me that my custom texture pack was much bigger compared to the original assets, despite it having the same resolution. Thankfully that's not an issue for me anymore, since my pack takes as much space as the original.
2. I'm learning C++ and this project was a good playground to practice and test my knowledge. There's certainly room for improvements, but I'm satisfied enough with this code.  

If you're interested how my tool works, see the [XNB format](https://github.com/SimonDarksideJ/XNAGameStudio/wiki/Compiled-(XNB)-Content-Format).

## Libraries
XNB files can be compressed or uncompressed. The compression algorithm used is [LZXD](https://msdn.microsoft.com/en-us/library/cc483133%28v=exchg.80%29.aspx), which was invented by Microsoft.

I was unable to find any source code for the LZXD compression, that's why I used `xcompress.lib` (which depends on `legacy_stdio_definitions.lib`).
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

It is a tiny, self-contained executable. Just drag & drop any supported files you want to convert.
