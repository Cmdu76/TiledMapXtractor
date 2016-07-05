#include "Map.hpp"
#include "Layer.hpp"
#include "ObjectGroup.hpp"

namespace tmx
{

Map::Map()
: mVersion(1.0f)
, mOrientation("orthogonal")
, mRenderOrder("right-down")
, mMapSize({0, 0})
, mTileSize({0, 0})
, mHexSideLength(0)
, mStaggerAxis("")
, mStaggerIndex("")
, mBackgroundColor("#808080")
, mNextObjectId(1)
, mRenderObjects(false)
, mTilesets()
, mLayers()
{
}

bool Map::loadFromFile(std::string const& filename)
{
    if (filename == "")
    {
        std::cerr << "Uncorrect filename" << std::endl;
        return false;
    }

    pugi::xml_document doc;
    if (!doc.load_file(filename.c_str()))
    {
        std::cerr << "The document (" << filename << ") cannot be loaded" << std::endl;
        return false;
    }

    pugi::xml_node map = doc.child("map");
    if (!map)
    {
        std::cerr << "The document has no \"map\" node" << std::endl;
        return false;
    }

    for (pugi::xml_attribute attr = map.first_attribute(); attr; attr = attr.next_attribute())
    {
        if (attr.name() == std::string("version")) mVersion = attr.as_float();
        if (attr.name() == std::string("orientation")) mOrientation = attr.as_string();
        if (attr.name() == std::string("renderorder")) mRenderOrder = attr.as_string();
        if (attr.name() == std::string("width")) mMapSize.x = attr.as_uint();
        if (attr.name() == std::string("height")) mMapSize.y = attr.as_uint();
        if (attr.name() == std::string("tilewidth")) mTileSize.x = attr.as_uint();
        if (attr.name() == std::string("tileheight")) mTileSize.y = attr.as_uint();
        if (attr.name() == std::string("hexsidelength")) mHexSideLength = attr.as_uint();
        if (attr.name() == std::string("staggeraxis")) mStaggerAxis = attr.as_string();
        if (attr.name() == std::string("staggerindex")) mStaggerIndex = attr.as_string();
        if (attr.name() == std::string("backgroundcolor")) mBackgroundColor = attr.as_string();
        if (attr.name() == std::string("nextobjectid")) mNextObjectId = attr.as_uint();
    }

    loadProperties(map);

    for (pugi::xml_node tileset = map.child("tileset"); tileset; tileset = tileset.next_sibling("tileset"))
    {
        Tileset tset;
        if (tset.loadFromNode(tileset))
        {
            if (std::find_if(mTilesets.begin(),mTilesets.end(),[&tset](Tileset const& t)->bool{return (t.getName() == tset.getName());}) == mTilesets.end())
                mTilesets.push_back(tset);
            else
                std::cerr << "Tileset already loaded" << std::endl;
        }
        else
            std::cerr << "Tileset has not been loaded" << std::endl;
    }
    for (pugi::xml_node layer = map.child("layer"); layer; layer = layer.next_sibling("layer"))
    {
        Layer* lyr = new Layer(*this);
        if (lyr->loadFromNode(layer))
        {
            if (std::find_if(mLayers.begin(),mLayers.end(),[&lyr](LayerBase* l)->bool{return (l->getName() == lyr->getName());}) == mLayers.end())
                mLayers.push_back(lyr);
            else
                std::cerr << "Layer already loaded" << std::endl;
        }
        else
            std::cerr << "Layer has not been loaded" << std::endl;
    }
    for (pugi::xml_node objectgroup = map.child("objectgroup"); objectgroup; objectgroup = objectgroup.next_sibling("objectgroup"))
    {
        ObjectGroup* obj = new ObjectGroup(*this);
        if (obj->loadFromNode(objectgroup))
        {
            if (std::find_if(mLayers.begin(),mLayers.end(),[&obj](LayerBase* l)->bool{return (l->getName() == obj->getName());}) == mLayers.end())
                mLayers.push_back(obj);
            else
                std::cerr << "ObjectGroup already loaded" << std::endl;
        }
        else
            std::cerr << "ObjectGroup has not been loaded" << std::endl;
    }
    for (pugi::xml_node imagelayer = map.child("imagelayer"); imagelayer; imagelayer = imagelayer.next_sibling("imagelayer"))
    {
        ImageLayer* lyr = new ImageLayer(*this);
        if (lyr->loadFromNode(imagelayer))
        {
            if (std::find_if(mLayers.begin(),mLayers.end(),[&lyr](LayerBase* l)->bool{return (l->getName() == lyr->getName());}) == mLayers.end())
                mLayers.push_back(lyr);
            else
                std::cerr << "ImageLayer already loaded" << std::endl;
        }
        else
            std::cerr << "ImageLayer has not been loaded" << std::endl;
    }

    return true;
}

bool Map::saveToFile(std::string const& filename)
{
    if (filename == "")
    {
        std::cerr << "Uncorrect filename" << std::endl;
        return false;
    }
    pugi::xml_document doc;
    pugi::xml_node map = doc.append_child("map");
    map.append_attribute("version") = mVersion;
    map.append_attribute("orientation") = mOrientation.c_str();
    map.append_attribute("renderorder") = mRenderOrder.c_str();
    map.append_attribute("width") = mMapSize.x;
    map.append_attribute("height") = mMapSize.y;
    map.append_attribute("tilewidth") = mTileSize.x;
    map.append_attribute("tileheight") = mTileSize.y;
    if (mHexSideLength != 0)
        map.append_attribute("hexsidelength") = mHexSideLength;
    if (mOrientation == "staggered" || mOrientation == "hexagonal")
    {
        map.append_attribute("staggeraxis") = mStaggerAxis.c_str();
        map.append_attribute("staggerindex") = mStaggerIndex.c_str();
    }
    if (mBackgroundColor != "")
        map.append_attribute("backgroundcolor") = mBackgroundColor.c_str();
    map.append_attribute("nextobjectid") = mNextObjectId;

    saveProperties(map);

    for (std::size_t i = 0; i < mTilesets.size(); i++)
    {
        pugi::xml_node tileset = map.append_child("tileset");
        if (!mTilesets[i].saveToNode(tileset))
            std::cerr << "Tileset " << mTilesets[i].getName() << " hasn't been save correctly" << std::endl;
    }

    for (std::size_t i = 0; i < mLayers.size(); i++)
    {
        pugi::xml_node layer;
        LayerType type = mLayers[i]->getLayerType();
        switch (type)
        {
            case tmx::EImageLayer: layer = map.append_child("imagelayer"); break;
            case tmx::EObjectGroup: layer = map.append_child("objectgroup"); break;
            default: layer = map.append_child("layer"); break;
        }
        mLayers[i]->saveToNode(layer);
    }

    doc.save_file(filename.c_str()," ");
    return true;
}

std::size_t Map::getLayerCount() const
{
    return mLayers.size();
}

LayerBase* Map::getLayer(std::size_t index)
{
    return mLayers[index];
}

LayerType Map::getLayerType(std::size_t index)
{
    return mLayers[index]->getLayerType();
}

void Map::removeLayer(std::string const& name)
{
    mLayers.erase(std::remove_if(mLayers.begin(),mLayers.end(),[&name](LayerBase* l)->bool{return l->getName() == name;}),mLayers.end());
}

void Map::renderBackground(sf::RenderTarget& target)
{
    sf::View v = target.getView();
    sf::RectangleShape shape(v.getSize());
    shape.setFillColor(detail::fromString<sf::Color>(mBackgroundColor));
    target.setView(target.getDefaultView());
    target.draw(shape);
    target.setView(v);
}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (std::size_t i = 0; i < mLayers.size(); i++)
    {
        if (mLayers.at(i)->getLayerType() == EObjectGroup)
        {
            if (mRenderObjects)
            {
                target.draw(*mLayers.at(i), states);
            }
        }
        else
        {
            target.draw(*mLayers.at(i), states);
        }
    }
}

void Map::render(std::size_t index, sf::RenderTarget& target, sf::RenderStates states) const
{
    if (0 <= index && index < mLayers.size())
    {
        target.draw(*mLayers.at(index), states);
    }
    else
    {
        std::cerr << "Out of range" << std::endl;
    }
}

Tileset* Map::getTileset(unsigned int id)
{
    for (std::size_t i = 0; i < mTilesets.size(); i++)
    {
        unsigned int first = mTilesets[i].getFirstGid();
        if (first <= id && id < first + mTilesets[i].getTileCount())
        {
            return &mTilesets[i];
        }
    }
    return nullptr;
}

std::string Map::getOrientation() const
{
    return mOrientation;
}

std::string Map::getRenderOrder() const
{
    return mRenderOrder;
}

sf::Vector2u Map::getMapSize() const
{
    return mMapSize;
}

sf::Vector2u Map::getTileSize() const
{
    return mTileSize;
}

unsigned int Map::getHexSideLength() const
{
    return mHexSideLength;
}

std::string Map::getStaggerAxis() const
{
    return mStaggerAxis;
}

std::string Map::getStaggerIndex() const
{
    return mStaggerIndex;
}

bool Map::getRenderObjects() const
{
    return mRenderObjects;
}

void Map::setRenderObjects(bool renderObjects)
{
    mRenderObjects = renderObjects;
}

} // namespace tmx