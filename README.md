# mkatlas
Generate a texture atlas from images or fonts. Outputs a JSON file.

## Dependencies
* [libpng](http://www.libpng.org/)
* Optionally [FreeType](https://www.freetype.org/)

## Usage

```
mkatlas [options] files
```

| Switch                | Description                                                       |
|-----------------------|-------------------------------------------------------------------|
| `-h --help`           | Show help.                                                        |
| `-v --version`        | Show version.                                                     |
| `--maxTextures <val>` | Set texture limit.                                                |
| `-e --expand`         | Expand borders of images. Depends on padding.                     |
| `-t --trim`           | Remove transparent borders of images.                             |
| `--noflip`            | Disable rotation of images.                                       |
| `-p --padding <val>`  | Set padding between images.                                       |
| `--width <width>`     | Set width of textures.                                            |
| `--height <height>`   | Set height of textures.                                           |
| `-s --size <size>`    | Set width and height of textures.                                 |
| `--folder <folder>`   | Set output folder of textures.                                    |
| `-o --out <output>`   | Set output file.                                                  |
| `-f --font <file>`    | Add font.                                                         |
| `-n --name <name>`    | Set name of font.                                                 |
| `-i --size <size>`    | Set size of font.                                                 |
| `-c --color <color>`  | Set color of font in hex.                                         |
| `--dfsize <size>`     | Set scaling of input image used to generate signed distant field. |
| `--dfspread <spread>` | Set spread of signed distant field.                               |
| `-r --range <range>`  | Add characters to font (can be `<num>` or `<beg>-<end>`).         |
