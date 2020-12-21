#include "dataStructs.h"

/*
void writeMatToStream(cv::Mat *mImage, QDataStream *stream){
    int dataSize = mImage->total() * mImage->elemSize1();
    int type = mImage->type();
    int step = mImage->step;
    int width = mImage->cols;
    int height = mImage->rows;
    *stream << type << step << width << height << dataSize;
    qDebug() << "Storing image : " << type << step << width << height << dataSize;
    stream->writeRawData(reinterpret_cast<const char *>(mImage->data), dataSize);
}

cv::Mat readMatFromStream(QDataStream *stream){
    int dataSize;
    int type;
    int step;
    int width;
    int height;
    *stream >> type >> step >> width >> height >> dataSize;
    char *data = new char[dataSize];
    qDebug() << "Restoring image : " << type << step << width << height << dataSize;
    //stream.device()->reset();
    stream->readRawData(data, dataSize);
    cv::Mat mImage = cv::Mat(height, width, type, data, step);
    return mImage;
}

struct ComplexStruct{
    std::string name;
    int intValue;
    std::vector<std::pair<int, std::optional<float>>> vec;
    std::map<int, QString> intMap;
    cv::Mat mat;
    struct BasicStruct{
        int myInt;
        std::map<QString, float> myMap;
    };

    template<class Archive>
      void serialize(Archive & archive);
};*/


QDataStream &operator <<(QDataStream &out, const ComplexStruct &dataStruct)
{
    /*out << dataStruct.x;
    out << dataStruct.y;
    out.writeRawData ( dataStruct.s.toStdString().c_str(), 10 );*/
    return out;
}

QDataStream &operator >>(QDataStream &in, ComplexStruct &dataStruct)
{
    dataStruct = ComplexStruct();
    /*in >> dataStruct.x;
    in >> dataStruct.y;
    char test[10];
    in.readRawData (test,10 );
    dataStruct.s=QString(test);*/
    return in;
}
