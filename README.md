# TiledMapXtractor (TMX)

Library for [Tiled](http://www.mapeditor.org/) maps with [SFML](https://github.com/SFML/SFML)

## Features

- Loading
- Saving
- Modification
- Orthogonal, Isometric, Staggered and Hexagonal (both with support for Axis and Index)
- Objects
- All the encoding and compression formats
- External tileset (.tsx)
- Almost all .tmx data (Please use the issue tracker if your output isn't the same as your Tiled editor)

## What is not supported

- Multiple tilesets for rendering (only one tileset per layer)
- Tileset:Tile:ObjectGroup and Tileset:Tile:Image
- Image from data

## TODO

Tileset part :
- Terrains usage in-game
- Animation using the .tmx data (even if not supported by Tiled at the moment)
- Load/Save image as data in the .tmx
- ObjectGroup and Image in Tileset:Tile, but what is this ?

Layer part :
- More testing and optimizations
- Get the bounds of the layer depending on the orientation

ObjectGroup part :
- Tile Flipping

Map part :
- Conversion of coordinates
- ResourceManager


## Requirements

- [SFML 2.x](https://github.com/SFML/SFML)
- [zlib](http://www.zlib.net/)

## Thanks to

- [pugixml](https://github.com/zeux/pugixml) : XML parser used in this project
- [STP](https://github.com/edoren/STP) : Another great SFML .tmx library, where I take the encoding/compression functions from
- [Thor](https://github.com/Bromeon/Thor) : SFML Extension, where I take the concave shape class from
