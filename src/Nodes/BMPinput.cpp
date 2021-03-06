#include "Nodes/BMPinput.hpp"

#include "NodeSystem/NodeOutput.hpp"
#include "RowSelector.hpp"
#include "NodeSystem/NodeDataTypes/NodeDataImage.hpp"

BMPinput::BMPinput(std::string filename, int x, int y) : Node(x, y), filename{filename}
{
    outputs.push_back(new NodeOutput{this, NodeIO::DataType::IMAGE, "Image"});
    relations.push_back(std::bind(&BMPinput::loadBMP, this, std::placeholders::_1));
}

std::string BMPinput::nodeName()
{
    return "BMP Input";
}

NodeDataImage* BMPinput::loadBMP(const std::vector<NodeInput*>)
{
    std::string fileName{filename};
    std::ifstream file{fileName, std::ifstream::binary};
    if(!file.is_open())
    {
        qCritical() << "File " << fileName.c_str() <<" could not be opened." << endl;
        return nullptr;
    }
    BMP_HEADER header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    if(!file.good())
    {
        qCritical() << "File " << fileName.c_str() << " is not a correct BMP file. No BMP header found." << endl;
        return nullptr;
    }
    if((header.magic[0] != 'B') or (header.magic[1] != 'M'))
    {
        qCritical() << "File " << fileName.c_str() << " is not a correct BMP file. Wrong BMP signature." << endl;
        return nullptr;
    }
    if(header.numPlanes != 1)
    {
        qCritical() << "File " << fileName.c_str() << " is not a correct BMP file. Wrong number of planes." << endl;
        return nullptr;
    }
    if(header.BPP != 24)
    {
        qCritical() << "File " << fileName.c_str() << " is not a correct BMP file. Wrong BitCount. Use 24bpp." << endl;
        return nullptr;
    }
    if(header.compression != 0)
    {
        qCritical() << "File " << fileName.c_str() << " is not a correct BMP file. File is compressed." << endl;
        return nullptr;
    }
    if(header.imageSize == 0)
    {
        header.imageSize = 3 * header.width * header.height;
    }
    if(header.dataOffset == 0)
    {
        header.dataOffset = 54;
    }
    char* rawData{new char[header.imageSize]};
    file.seekg(header.dataOffset);
    file.read(rawData, header.imageSize);
    NodeDataImage* output{new NodeDataImage{static_cast<int>(header.width), static_cast<int>(header.height)}};
    for(unsigned int i{0}; i < header.height; i++)
    {
        for(unsigned int j{0}; j < header.width; j++)
        {
            int offset{static_cast<int>(i * header.height + j)};
            (*output)[i][j].r = static_cast<unsigned char>(rawData[3 * offset + 2]) / 255.0f;
            (*output)[i][j].g = static_cast<unsigned char>(rawData[3 * offset + 1]) / 255.0f;
            (*output)[i][j].b = static_cast<unsigned char>(rawData[3 * offset]) / 255.0f;
            (*output)[i][j].a = 1.0f;
        }
    }
    delete[] rawData;
    file.close();
    return output;
}
