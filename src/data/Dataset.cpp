#include "Dataset.h"
#include <QDebug>
#include <QImageReader>
#include "STData.h"
#include "DatasetImporter.h"

Dataset::Dataset()
    : m_name()
    , m_statComments()
    , m_data_file()
    , m_image_file()
    , m_mesh_file()
    , m_spots_file()
    , m_scaling_factor(0.5)
    , m_is3D(false)
    , m_alignment()
    , m_image_tiles()
    , m_image_bounds()
    , m_data(nullptr)
{
}

Dataset::Dataset(const DatasetImporter &importer)
{
    m_name = importer.datasetName();
    m_statComments = importer.comments();
    m_data_file = importer.STDataFile();
    m_image_file = importer.mainImageFile();
    m_mesh_file = importer.meshFile();
    m_spots_file = importer.spotsMapFile();
    m_scaling_factor = importer.scalingFactor();
    m_is3D = importer.is3D();
    m_data = nullptr;
}

Dataset::Dataset(const Dataset &other)
{
    m_name = other.m_name;
    m_statComments = other.m_statComments;
    m_data_file = other.m_data_file;
    m_image_file = other.m_image_file;
    m_mesh_file = other.m_mesh_file;
    m_spots_file = other.m_spots_file;
    m_scaling_factor = other.m_scaling_factor;
    m_is3D = other.m_is3D;
    m_alignment = other.m_alignment;
    m_image_tiles = other.m_image_tiles;
    m_image_bounds = other.m_image_bounds;
    m_data = other.m_data;
}

Dataset::~Dataset()
{
}

Dataset &Dataset::operator=(const Dataset &other)
{
    m_name = other.m_name;
    m_statComments = other.m_statComments;
    m_data_file = other.m_data_file;
    m_image_file = other.m_image_file;
    m_mesh_file = other.m_mesh_file;
    m_spots_file = other.m_spots_file;
    m_scaling_factor = other.m_scaling_factor;
    m_is3D = other.m_is3D;
    m_alignment = other.m_alignment;
    m_image_tiles = other.m_image_tiles;
    m_image_bounds = other.m_image_bounds;
    m_data = other.m_data;
    return (*this);
}

bool Dataset::operator==(const Dataset &other) const
{
    return (m_name == other.m_name
            && m_statComments == other.m_statComments
            && m_data_file == other.m_data_file
            && m_image_file == other.m_image_file
            && m_mesh_file == other.m_mesh_file
            && m_spots_file == other.m_spots_file
            && m_alignment == other.m_alignment
            && m_scaling_factor == other.m_scaling_factor
            && m_is3D == other.m_is3D);
}

const QSharedPointer<STData> Dataset::data() const
{
    return m_data;
}

const QString &Dataset::name() const
{
    return m_name;
}

const QString &Dataset::dataFile() const
{
    return m_data_file;
}

const QTransform &Dataset::alignmentMatrix() const
{
    return m_alignment;
}

const QString &Dataset::meshFile() const
{
    return m_mesh_file;
}

const QString &Dataset::imageFile() const
{
    return m_image_file;
}

const QString &Dataset::spotsFile() const
{
    return m_spots_file;
}

const QString &Dataset::statComments() const
{
    return m_statComments;
}

double Dataset::scalingFactor() const
{
    return m_scaling_factor;
}

const QVector<QPair<QImage, QPoint>> &Dataset::image_tiles() const
{
    return m_image_tiles;
}

const QRect Dataset::image_bounds() const
{
    return m_image_bounds;
}

bool Dataset::is3D() const
{
    return m_is3D;
}

void Dataset::name(const QString &name)
{
    m_name = name;
}

void Dataset::dataFile(const QString &datafile)
{
    m_data_file = datafile;
}

void Dataset::imageFile(const QString &image_file)
{
    m_image_file = image_file;
}

void Dataset::meshFile(const QString &mesh_file)
{
    m_mesh_file = mesh_file;
}

void Dataset::spotsFile(const QString &spots_file)
{
    m_spots_file = spots_file;
}

void Dataset::statComments(const QString &statComments)
{
    m_statComments = statComments;
}

void Dataset::scalingFactor(const double scaling_factor)
{
    m_scaling_factor = scaling_factor;
}

void Dataset::load_data()
{
    // Parse ST Data file and spot coordinates (if any)
    m_data = QSharedPointer<STData>(new STData());
    try {
        m_data->init(m_data_file, m_spots_file);
        m_data->is3D(m_is3D);
    } catch (const std::exception &e) {
        qDebug() << "Error parsing data matrix or spot coordinates " << e.what();
        throw;
    }

    // Parse image (in 2D only)
    if (!m_is3D && !m_image_file.isNull() && !m_image_file.isEmpty()) {
        const bool parsed = load_Image();
        if (!parsed) {
            qDebug() << "Error parsing image file";
            throw std::runtime_error("Error parsing Image file");
        }
        m_alignment = QTransform::fromScale(m_scaling_factor, m_scaling_factor);
        qDebug() << "Setting alignment matrix to " << m_alignment;
    }
}

bool Dataset::load_Image() {

    // image buffer reader
    QImageReader imageReader(m_image_file);

    // scale image with the scaling factors
    QSize imageSize = imageReader.size();
    qDebug() << "Parsed image of size " << imageSize;
    imageSize *= m_scaling_factor;
    imageReader.setScaledSize(imageSize);

    // load the image
    QImage image;
    if (!imageReader.read(&image)) {
        qDebug() << "Tissue image cannot be parsed " << imageReader.errorString();
        return false;
    }

    // store the scaled image size
    m_image_bounds = image.rect();
    qDebug() << "Setting image of size " << m_image_bounds;

    // compute tiles size and numbers
    constexpr int tile_width = 256;
    constexpr int tile_height = 256;
    const int width = image.width();
    const int height = image.height();
    const int xCount = width / tile_width;
    const int yCount = height / tile_height;
    const int count = xCount * yCount;

    // create tiles
    m_image_tiles.resize(count);
    #pragma omp parallel for
    for (int i = 0; i < count; ++i) {
        // tiles sizes
        const int x = tile_width * (i % xCount);
        const int y = tile_height * (i / xCount);
        const int texture_width = std::min(width - x, tile_width);
        const int texture_height = std::min(height - y, tile_height);
        // QImage should be thread-safe
        m_image_tiles[i] = (QPair<QImage, QPoint>(image.copy(x, y,
                                                             texture_width,
                                                             texture_height),
                                                  QPoint(x, y)));
    }

    return true;
}
