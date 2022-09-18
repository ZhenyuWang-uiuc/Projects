/**
 * @file maptiles.cpp
 * Code for the maptiles function.
 */

#include <iostream>
#include <map>
#include "maptiles.h"
//#include "cs225/RGB_HSL.h"

using namespace std;


Point<3> convertToXYZ(LUVAPixel pixel) {
    return Point<3>( pixel.l, pixel.u, pixel.v );
}

MosaicCanvas* mapTiles(SourceImage const& theSource,
                       vector<TileImage>& theTiles)
{
    // create a canvas
    MosaicCanvas * canvas = new MosaicCanvas(theSource.getRows(), theSource.getColumns());

    // create a vector holds all pixels
    vector<Point<3>> tiles;

    // map avg color <-> TileImage
    map<Point<3>, TileImage*> mymap;
    for (unsigned i = 0; i < theTiles.size(); ++i) {
         mymap[convertToXYZ(theTiles[i].getAverageColor())] = &theTiles[i];
         tiles.push_back(convertToXYZ(theTiles[i].getAverageColor()));
    }

    // create a KDTree
    KDTree<3> tree(tiles);

    // put tile on the canvas
    for (int row = 0; row < theSource.getRows(); ++row) {
        for (int col = 0; col < theSource.getColumns(); ++col) {
            Point<3> cur = convertToXYZ(theSource.getRegionColor(row, col));
            Point<3> NN = tree.findNearestNeighbor(cur);
            canvas->setTile(row, col, mymap[NN]);
        }
    }

    return canvas;
}

