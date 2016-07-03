#include "Utils.hpp"

namespace tmx
{

namespace detail
{

template <>
std::string toString<std::string>(std::string const& value)
{
    return value;
}

template <>
std::string toString<bool>(bool const& value)
{
    return (value) ? "true" : "false";
}

template <>
std::string fromString<std::string>(std::string const& string)
{
    return string;
}

template <>
bool fromString<bool>(std::string const& string)
{
    if (string == "true")
        return true;
    if (string == "false")
        return false;
    bool value;
    std::istringstream iss(string);
    iss >> value;
    return value;
}

PropertiesHolder::PropertiesHolder()
{
}

void PropertiesHolder::loadProperties(pugi::xml_node& node)
{
    if (!node)
    {
        std::cerr << "Invalid properties node" << std::endl;
        return;
    }

    pugi::xml_node properties = node.child("properties");
    if (properties)
    {
        for (pugi::xml_node property = properties.child("property"); property; property = property.next_sibling("property"))
        {
            for (pugi::xml_attribute attr = property.first_attribute(); attr; attr = attr.next_attribute())
            {
                mProperites[attr.name()] = attr.value();
            }
        }
    }
}

void PropertiesHolder::saveProperties(pugi::xml_node& node)
{
    if (!node)
    {
        std::cerr << "Invalid parent node for properties" << std::endl;
    }
    if (mProperites.size() > 0)
    {
        pugi::xml_node properties = node.append_child("properties");
        for (auto itr = mProperites.begin(); itr != mProperites.end(); itr++)
        {
            pugi::xml_node property = properties.append_child("property");
            property.append_attribute("name") = itr->first.c_str();
            property.append_attribute("value") = itr->second.c_str();
        }
    }
}

LayerBase::LayerBase()
{
    mName = "";
    mOffset = {0, 0};
    mOpacity = 1.f;
    mVisible = true;
}

bool LayerBase::loadFromNode(pugi::xml_node& layer)
{
    if (!layer)
    {
        std::cerr << "Invalid layer node" << std::endl;
        return false;
    }
    for (pugi::xml_attribute attr = layer.first_attribute(); attr; attr = attr.next_attribute())
    {
        if (attr.name() == std::string("name")) mName = attr.value();
        if (attr.name() == std::string("opacity")) mOpacity = attr.as_float();
        if (attr.name() == std::string("visible")) mVisible = attr.as_bool();
        if (attr.name() == std::string("offsetx")) mOffset.x = attr.as_float();
        if (attr.name() == std::string("offsety")) mOffset.y = attr.as_float();
    }
    loadProperties(layer);
    return true;
}

void LayerBase::saveToNode(pugi::xml_node& layer)
{
    if (!layer)
    {
        std::cerr << "Invalid layer node" << std::endl;
        return;
    }
    layer.append_attribute("name") = mName.c_str();
    if (mOpacity != 1.f)
        layer.append_attribute("opacity") = mOpacity;
    if (!mVisible)
        layer.append_attribute("visible") = toString(mVisible).c_str();
    if (mOffset.x != 0.f)
        layer.append_attribute("offsetx") = mOffset.x;
    if (mOffset.y != 0.f)
        layer.append_attribute("offsety") = mOffset.y;
    saveProperties(layer);
}

std::string LayerBase::getName() const
{
    return mName;
}

Image::Image()
{
    mFormat = "";
    mSource = "";
    mTrans = "";
    mSize = {0, 0};
}

bool Image::loadFromNode(pugi::xml_node& image)
{
    if (!image)
    {
        std::cerr << "Invalid image node" << std::endl;
        return false;
    }
    for (pugi::xml_attribute attr = image.first_attribute(); attr; attr = attr.next_attribute())
    {
        if (attr.name() == std::string("format")) mFormat = attr.value();
        if (attr.name() == std::string("source")) mSource = attr.value();
        if (attr.name() == std::string("trans")) mTrans = attr.value();
        if (attr.name() == std::string("width")) mSize.x = attr.as_uint();
        if (attr.name() == std::string("height")) mSize.y = attr.as_uint();
    }
    return true;
}

void Image::saveToNode(pugi::xml_node& image)
{
    if (!image)
    {
        std::cerr << "Invalid image node" << std::endl;
        return;
    }
    if (mFormat != "")
        image.append_attribute("format") = mFormat.c_str();
    if (mSource != "")
        image.append_attribute("source") = mSource.c_str();
    if (mTrans != "")
        image.append_attribute("trans") = mTrans.c_str();
    if (mSize != sf::Vector2u())
    {
        image.append_attribute("width") = mSize.x;
        image.append_attribute("height") = mSize.y;
    }
}

std::string Image::getFormat() const
{
    return mFormat;
}

std::string Image::getSource() const
{
    return mSource;
}

sf::Color Image::getTransparent() const
{
    if (mTrans != "")
    {
        int hexTrans;
        std::stringstream ss(mTrans);
        ss >> std::hex >> hexTrans;
        if (hexTrans >= 0)
        {
            unsigned char red, green, blue;
            red = hexTrans >> 16;
            green = (hexTrans >> 8) & 0xff;
            blue = hexTrans & 0xff;
            return sf::Color(red, green, blue);
        }
    }
    return sf::Color::Transparent;
}

sf::Vector2u Image::getSize() const
{
    return mSize;
}

} // namespace detail

} // namespace tmx
