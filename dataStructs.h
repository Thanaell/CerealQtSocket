#include <QDataStream>
#include <opencv2/core.hpp>
#include <QDebug>
#include <map>
#include <QMap>
#include <optional>
#include "cereal/cereal.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/utility.hpp>

namespace cv {

/**
 * Serialise a cv::Mat using cereal.
 *
 * Supports all types of matrices as well as non-contiguous ones.
 *
 * @param[in] ar The archive to serialise to.
 * @param[in] mat The matrix to serialise.
 */
template<class Archive>
void save(Archive& ar, const cv::Mat& mat)
{
    int rows, cols, type;
    bool continuous;

    rows = mat.rows;
    cols = mat.cols;
    type = mat.type();
    continuous = mat.isContinuous();

    ar & rows & cols & type & continuous;

    if (continuous) {
        const int data_size = rows * cols * static_cast<int>(mat.elemSize());
        auto mat_data = cereal::binary_data(mat.ptr(), data_size);
        ar & mat_data;
    }
    else {
        const int row_size = cols * static_cast<int>(mat.elemSize());
        for (int i = 0; i < rows; i++) {
            auto row_data = cereal::binary_data(mat.ptr(i), row_size);
            ar & row_data;
        }
    }
};

/**
 * De-serialise a cv::Mat using cereal.
 *
 * Supports all types of matrices as well as non-contiguous ones.
 *
 * @param[in] ar The archive to deserialise from.
 * @param[in] mat The matrix to deserialise into.
 */
template<class Archive>
void load(Archive& ar, cv::Mat& mat)
{
    int rows, cols, type;
    bool continuous;

    ar & rows & cols & type & continuous;

    if (continuous) {
        mat.create(rows, cols, type);
        const int data_size = rows * cols * static_cast<int>(mat.elemSize());
        auto mat_data = cereal::binary_data(mat.ptr(), data_size);
        ar & mat_data;
    }
    else {
        mat.create(rows, cols, type);
        const int row_size = cols * static_cast<int>(mat.elemSize());
        for (int i = 0; i < rows; i++) {
            auto row_data = cereal::binary_data(mat.ptr(i), row_size);
            ar & row_data;
        }
    }
};

}

template<class Archive>
void save(Archive& ar, const QString &str){
    ar(str.toStdString());
}

template<class Archive>
void load(Archive& ar, QString &str){
    std::string temp;
    ar(temp);
    str=QString::fromStdString(temp);
}

struct Test3
{
  int a;
};

template<class Archive>
void serialize(Archive & ar, Test3 & t)
{
  ar(t.a);
}

struct BasicStruct{
    int myInt;
    std::map<QString, float> myMap;
    template<class Archive>
      void serialize(Archive & archive);
};

struct ComplexStruct{
    std::string name;
    QString qName;
    int intValue;
    std::vector<std::pair<int, std::optional<float>>> vec;
    std::map<int, QString> intMap;
    cv::Mat mat;
    BasicStruct myBasicStruct;
    template<class Archive>
      void serialize(Archive & archive);
};

template<class Archive>
void ComplexStruct::serialize(Archive & archive){
    archive(name,qName,intValue,vec,intMap,mat,myBasicStruct);
}

template<class Archive>
void BasicStruct::serialize(Archive & archive){
    archive(myInt,myMap);

}



void writeMatToStream(cv::Mat *mImage, QDataStream *stream);
cv::Mat readMatFromStream(QDataStream *stream);

QDataStream &operator <<(QDataStream &out, const ComplexStruct &dataStruct);
QDataStream &operator >>(QDataStream &in, ComplexStruct &dataStruct);
