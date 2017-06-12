#include "Dataset.h"
#include <QDebug>
#include "STData.h"
#include "DatasetImporter.h"

Dataset::Dataset()
    : m_name()
    , m_statTissue()
    , m_statSpecies()
    , m_statComments()
    , m_data_file()
    , m_image_file()
    , m_alignment_file()
    , m_spots_file()
    , m_alignment()
    , m_data(nullptr)
{
}

Dataset::Dataset(const DatasetImporter &importer)
{
    m_name = importer.datasetName();
    m_statTissue = importer.tissue();
    m_statSpecies = importer.species();
    m_statComments = importer.comments();
    m_data_file = importer.STDataFile();
    m_image_file = importer.mainImageFile();
    m_alignment_file = importer.alignmentMatrix();
    m_spots_file = importer.spotsMapFile();
    m_alignment = QTransform();
    m_data = nullptr;
}

Dataset::Dataset(const Dataset &other)
{
    m_name = other.m_name;
    m_statTissue = other.m_statTissue;
    m_statSpecies = other.m_statSpecies;
    m_statComments = other.m_statComments;
    m_data_file = other.m_data_file;
    m_image_file = other.m_image_file;
    m_alignment_file = other.m_alignment_file;
    m_spots_file = other.m_spots_file;
    m_alignment = other.m_alignment;
    m_data = other.m_data;
}

Dataset::~Dataset()
{
}

Dataset &Dataset::operator=(const Dataset &other)
{
    m_name = other.m_name;
    m_statTissue = other.m_statTissue;
    m_statSpecies = other.m_statSpecies;
    m_statComments = other.m_statComments;
    m_data_file = other.m_data_file;
    m_image_file = other.m_image_file;
    m_alignment_file = other.m_alignment_file;
    m_spots_file = other.m_spots_file;
    m_alignment = other.m_alignment;
    m_data = other.m_data;
    return (*this);
}

bool Dataset::operator==(const Dataset &other) const
{
    return (m_name == other.m_name
            && m_statTissue == other.m_statTissue
            && m_statSpecies == other.m_statSpecies
            && m_statComments == other.m_statComments
            && m_data_file == other.m_data_file
            && m_image_file == other.m_image_file
            && m_alignment_file == other.m_alignment_file
            && m_spots_file == other.m_spots_file
            && m_alignment == other.m_alignment
            && m_data == other.m_data);
}

const QSharedPointer<STData> Dataset::data() const
{
    return m_data;
}

const QString Dataset::name() const
{
    return m_name;
}

const QString Dataset::dataFile() const
{
    return m_data_file;
}

const QTransform Dataset::imageAlignment() const
{
    return m_alignment;
}

const QString Dataset::imageAlignmentFile() const
{
    return m_alignment_file;
}

const QString Dataset::imageFile() const
{
    return m_image_file;
}

const QString Dataset::spotsFile() const
{
    return m_spots_file;
}

const QString Dataset::statTissue() const
{
    return m_statTissue;
}

const QString Dataset::statSpecies() const
{
    return m_statSpecies;
}

const QString Dataset::statComments() const
{
    return m_statComments;
}

void Dataset::name(const QString &name)
{
    m_name = name;
}

void Dataset::dataFile(const QString &datafile)
{
    m_data_file = datafile;
}

void Dataset::imageAlignment(const QTransform &alignment)
{
    m_alignment = alignment;
}

void Dataset::imageAlignmentFile(const QString &aligment_file)
{
    m_alignment_file = aligment_file;
}

void Dataset::imageFile(const QString &image_file)
{
    m_image_file = image_file;
}

void Dataset::spotsFile(const QString &spots_file)
{
    m_spots_file = spots_file;
}

void Dataset::statTissue(const QString &statTissue)
{
    m_statTissue = statTissue;
}

void Dataset::statSpecies(const QString &statSpecies)
{
    m_statSpecies = statSpecies;
}

void Dataset::statComments(const QString &statComments)
{
    m_statComments = statComments;
}

bool Dataset::load_data()
{
    m_data = QSharedPointer<STData>(new STData());
    try {
        m_data->init(m_data_file);
    } catch (const std::exception &e) {
        qDebug() << "Error parsing data matrix " << e.what();
        return false;
    }
    bool parsed = true;
    // Parse image alignment
    if (!m_alignment_file.isEmpty()) {
        parsed &= load_imageAligment();
    }
    // Parse stops coordinates
    if (!m_spots_file.isEmpty()) {
        parsed &= m_data->parseSpotsMap(m_spots_file);
    }
    return parsed;
}

bool Dataset::load_imageAligment()
{
    qDebug() << "Parsing image alignment file " << m_alignment_file;
    bool parsed = true;
    float a11 = 1.0;
    float a12 = 0.0;
    float a13 = 0.0;
    float a21 = 0.0;
    float a22 = 1.0;
    float a23 = 0.0;
    float a31 = 0.0;
    float a32 = 0.0;
    float a33 = 1.0;
    QFile file(m_alignment_file);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line = in.readLine();
        QStringList fields = line.split(" ");
        if (fields.length() == 9) {
            a11 = fields.at(0).toFloat();
            a12 = fields.at(1).toFloat();
            a13 = fields.at(2).toFloat();
            a21 = fields.at(3).toFloat();
            a22 = fields.at(4).toFloat();
            a23 = fields.at(5).toFloat();
            a31 = fields.at(6).toFloat();
            a32 = fields.at(7).toFloat();
            a33 = fields.at(8).toFloat();
        } else {
            qDebug() << "Error parsing alignment matrix";
            parsed = false;
        }
        m_alignment = QTransform(a11, a12, a13, a21, a22, a23, a31, a32, a33);
    } else {
        qDebug() << "Image alignment file coult not be opened";
        parsed = false;
    }
    file.close();
    return parsed;
}

