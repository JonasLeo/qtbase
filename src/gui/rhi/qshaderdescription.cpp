/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Gui module
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qshaderdescription_p_p.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

QT_BEGIN_NAMESPACE

/*!
    \class QShaderDescription
    \inmodule QtRhi

    \brief Describes the interface of a shader.

    A shader typically has a set of inputs and outputs. A vertex shader for
    example has a number of input variables and may use one or more uniform
    buffers to access data (e.g. a modelview matrix) provided by the
    application. The shader for the fragment stage receives data from the
    vertex stage (in a simple setup) and may also rely on data from uniform
    buffers, images, and samplers.

    When it comes to vertex inputs and the layout of the uniform buffers (what
    are the names of the members? what is there size, offset, and so on),
    applications and frameworks may need to discover this dynamically at run
    time. This is typical when the shader is not built-in but provided by an
    external entity, like the user.

    Modern and lean graphics APIs may no longer provide a way to query shader
    reflection information at run time. Therefore, such data is now
    automatically generated by QShaderBaker and is provided as a
    QShaderDescription object for each and every QShader.

    \section2 Example

    Take the following vertex shader:

    \badcode
        #version 440

        layout(location = 0) in vec4 position;
        layout(location = 1) in vec3 color;
        layout(location = 0) out vec3 v_color;

        layout(std140, binding = 0) uniform buf {
            mat4 mvp;
            float opacity;
        } ubuf;

        out gl_PerVertex { vec4 gl_Position; };

        void main()
        {
            v_color = color;
            gl_Position = ubuf.mvp * position;
        }
    \endcode

    This shader has two inputs: \c position at location 0 with a type of \c
    vec4, and \c color at location 1 with a type of \c vec3. It has one output:
    \c v_color, although this is typically not interesting for applications.
    What is more important, there is a uniform block at binding 0 with a size
    of 68 bytes and two members, a 4x4 matrix named \c mvp at offset 0, and a
    float \c opacity at offset 64.

    All this is described by a QShaderDescription object. QShaderDescription
    can also be serialized to JSON and binary JSON, and can be deserialized
    from binary JSON. In practice this is rarely needed since QShader
    takes care of the associated QShaderDescription automatically, but if the
    QShaderDescription of the above shader would be written out as JSON, it
    would look like the following:

    \badcode
        {
            "inputs": [
                {
                    "location": 1,
                    "name": "color",
                    "type": "vec3"
                },
                {
                    "location": 0,
                    "name": "position",
                    "type": "vec4"
                }
            ],
            "outputs": [
                {
                    "location": 0,
                    "name": "v_color",
                    "type": "vec3"
                }
            ],
            "uniformBlocks": [
                {
                    "binding": 0,
                    "blockName": "buf",
                    "members": [
                        {
                            "matrixStride": 16,
                            "name": "mvp",
                            "offset": 0,
                            "size": 64,
                            "type": "mat4"
                        },
                        {
                            "name": "opacity",
                            "offset": 64,
                            "size": 4,
                            "type": "float"
                        }
                    ],
                    "set": 0,
                    "size": 68,
                    "structName": "ubuf"
                }
            ]
        }
    \endcode

    The C++ API allows accessing a data structure like the above. For
    simplicity the inner structs only contain public data members, also
    considering that their layout is unlikely to change in the future.

    \sa QShaderBaker, QShader
 */

/*!
    \enum QShaderDescription::VariableType
    Represents the type of a variable or block member.

    \value Unknown
    \value Float
    \value Vec2
    \value Vec3
    \value Vec4
    \value Mat2
    \value Mat2x3
    \value Mat2x4
    \value Mat3
    \value Mat3x2
    \value Mat3x4
    \value Mat4
    \value Mat4x2
    \value Mat4x3
    \value Int
    \value Int2
    \value Int3
    \value Int4
    \value Uint
    \value Uint2
    \value Uint3
    \value Uint4
    \value Bool
    \value Bool2
    \value Bool3
    \value Bool4
    \value Double
    \value Double2
    \value Double3
    \value Double4
    \value DMat2
    \value DMat2x3
    \value DMat2x4
    \value DMat3
    \value DMat3x2
    \value DMat3x4
    \value DMat4
    \value DMat4x2
    \value DMat4x3
    \value Sampler1D
    \value Sampler2D
    \value Sampler2DMS
    \value Sampler3D
    \value SamplerCube
    \value Sampler1DArray
    \value Sampler2DArray
    \value Sampler2DMSArray
    \value Sampler3DArray
    \value SamplerCubeArray
    \value SamplerRect
    \value SamplerBuffer
    \value Image1D
    \value Image2D
    \value Image2DMS
    \value Image3D
    \value ImageCube
    \value Image1DArray
    \value Image2DArray
    \value Image2DMSArray
    \value Image3DArray
    \value ImageCubeArray
    \value ImageRect
    \value ImageBuffer
    \value Struct
 */

/*!
    \class QShaderDescription::InOutVariable
    \inmodule QtRhi

    \brief Describes an input or output variable in the shader.
 */

/*!
    \class QShaderDescription::BlockVariable
    \inmodule QtRhi

    \brief Describes a member of a uniform or push constant block.
 */

/*!
    \class QShaderDescription::UniformBlock
    \inmodule QtRhi

    \brief Describes a uniform block.

    \note When translating to shading languages without uniform block support
    (like GLSL 120 or GLSL/ES 100), uniform blocks are replaced with ordinary
    uniforms in a struct. The name of the struct, and so the prefix for the
    uniforms generated from the block members, is given by structName.
 */

/*!
    \class QShaderDescription::PushConstantBlock
    \inmodule QtRhi

    \brief Describes a push constant block.
 */

/*!
    \class QShaderDescription::StorageBlock
    \inmodule QtRhi

    \brief Describes a shader storage block.
 */

/*!
    Constructs a new, empty QShaderDescription.

    \note Being empty implies that isValid() returns \c false for the
    newly constructed instance.
 */
QShaderDescription::QShaderDescription()
    : d(new QShaderDescriptionPrivate)
{
}

/*!
    \internal
 */
void QShaderDescription::detach()
{
    qAtomicDetach(d);
}

/*!
    \internal
 */
QShaderDescription::QShaderDescription(const QShaderDescription &other)
    : d(other.d)
{
    d->ref.ref();
}

/*!
    \internal
 */
QShaderDescription &QShaderDescription::operator=(const QShaderDescription &other)
{
    qAtomicAssign(d, other.d);
    return *this;
}

/*!
    Destructor.
 */
QShaderDescription::~QShaderDescription()
{
    if (!d->ref.deref())
        delete d;
}

/*!
   \return true if the QShaderDescription contains at least one entry in one of
   the variable and block lists.
 */
bool QShaderDescription::isValid() const
{
    return !d->inVars.isEmpty() || !d->outVars.isEmpty()
        || !d->uniformBlocks.isEmpty() || !d->pushConstantBlocks.isEmpty() || !d->storageBlocks.isEmpty()
        || !d->combinedImageSamplers.isEmpty() || !d->storageImages.isEmpty();
}

/*!
    \return a serialized binary version of the data.

    \sa toJson()
 */
QByteArray QShaderDescription::toBinaryJson() const
{
    return d->makeDoc().toBinaryData();
}

/*!
    \return a serialized JSON text version of the data.

    \note There is no deserialization method provided for JSON text.

    \sa toBinaryJson()
 */
QByteArray QShaderDescription::toJson() const
{
    return d->makeDoc().toJson();
}

/*!
    Deserializes the given binary JSON \a data and returns a new
    QShaderDescription.
 */
QShaderDescription QShaderDescription::fromBinaryJson(const QByteArray &data)
{
    QShaderDescription desc;
    QShaderDescriptionPrivate::get(&desc)->loadDoc(QJsonDocument::fromBinaryData(data));
    return desc;
}

/*!
    \return the list of input variables. This includes vertex inputs (sometimes
    called attributes) for the vertex stage, and inputs for other stages
    (sometimes called varyings).
 */
QVector<QShaderDescription::InOutVariable> QShaderDescription::inputVariables() const
{
    return d->inVars;
}

/*!
    \return the list of output variables.
 */
QVector<QShaderDescription::InOutVariable> QShaderDescription::outputVariables() const
{
    return d->outVars;
}

/*!
    \return the list of uniform blocks.
 */
QVector<QShaderDescription::UniformBlock> QShaderDescription::uniformBlocks() const
{
    return d->uniformBlocks;
}

/*!
    \return the list of push constant blocks.

    \note Avoid relying on push constant blocks for shaders that are to be used
    in combination with the Qt Rendering Hardware Interface since that
    currently has no support for them.
 */
QVector<QShaderDescription::PushConstantBlock> QShaderDescription::pushConstantBlocks() const
{
    return d->pushConstantBlocks;
}

/*!
    \return the list of shader storage blocks.

    For example, with GLSL/Vulkan shaders as source, the declaration

    \badcode
        struct Stuff {
            vec2 a;
            vec2 b;
        };
        layout(std140, binding = 0) buffer StuffSsbo {
            vec4 whatever;
            Stuff stuff[];
        } buf;
    \endcode

    generates the following: (shown as textual JSON here)

    \badcode
        "storageBlocks": [ {
            "binding": 0,
            "blockName": "StuffSsbo",
            "instanceName": "buf",
            "knownSize": 16,
            "members": [
                {
                    "name": "whatever",
                    "offset": 0,
                    "size": 16,
                    "type": "vec4"
                },
                {
                    "arrayDims": [
                        0
                    ],
                    "name": "stuff",
                    "offset": 16,
                    "size": 0,
                    "structMembers": [
                        {
                            "name": "a",
                            "offset": 0,
                            "size": 8,
                            "type": "vec2"
                        },
                        {
                            "name": "b",
                            "offset": 8,
                            "size": 8,
                            "type": "vec2"
                        }
                    ],
                    "type": "struct"
                }
            ],
            "set": 0
        } ]
    \endcode

    \note The size of the last member in the storage block is undefined. This shows
    up as \c size 0 and an array dimension of \c{[0]}. The storage block's \c knownSize
    excludes the size of the last member since that will only be known at run time.

    \note SSBOs are not available with some graphics APIs, such as, OpenGL 2.x or
    OpenGL ES older than 3.1.
 */
QVector<QShaderDescription::StorageBlock> QShaderDescription::storageBlocks() const
{
    return d->storageBlocks;
}

/*!
    \return the list of combined image samplers

    With GLSL/Vulkan shaders as source a \c{layout(binding = 1) uniform sampler2D tex;}
    uniform generates the following: (shown as textual JSON here)

    \badcode
       "combinedImageSamplers": [
            {
                "binding": 1,
                "name": "tex",
                "set": 0,
                "type": "sampler2D"
            }
        ]
    \endcode

    This does not mean that other language versions of the shader must also use
    a combined image sampler, especially considering that the concept may not
    exist everywhere. For instance, a HLSL version will likely just use a
    Texture2D and SamplerState object with registers t1 and s1, respectively.
  */
QVector<QShaderDescription::InOutVariable> QShaderDescription::combinedImageSamplers() const
{
    return d->combinedImageSamplers;
}

/*!
    \return the list of image variables.

    These will likely occur in compute shaders. For example,
    \c{layout (binding = 0, rgba8) uniform readonly image2D inputImage;}
    generates the following: (shown as textual JSON here)

    \badcode
       "storageImages": [
            {
                "binding": 0,
                "imageFormat": "rgba8",
                "name": "inputImage",
                "set": 0,
                "type": "image2D"
            }
        ]
    \endcode

    \note Separate image objects are not compatible with some graphics APIs,
    such as, OpenGL 2.x or OpenGL ES older than 3.1.
  */
QVector<QShaderDescription::InOutVariable> QShaderDescription::storageImages() const
{
    return d->storageImages;
}

static struct TypeTab {
    QString k;
    QShaderDescription::VariableType v;
} typeTab[] = {
    { QLatin1String("float"), QShaderDescription::Float },
    { QLatin1String("vec2"), QShaderDescription::Vec2 },
    { QLatin1String("vec3"), QShaderDescription::Vec3 },
    { QLatin1String("vec4"), QShaderDescription::Vec4 },
    { QLatin1String("mat2"), QShaderDescription::Mat2 },
    { QLatin1String("mat3"), QShaderDescription::Mat3 },
    { QLatin1String("mat4"), QShaderDescription::Mat4 },

    { QLatin1String("struct"), QShaderDescription::Struct },

    { QLatin1String("sampler1D"), QShaderDescription::Sampler1D },
    { QLatin1String("sampler2D"), QShaderDescription::Sampler2D },
    { QLatin1String("sampler2DMS"), QShaderDescription::Sampler2DMS },
    { QLatin1String("sampler3D"), QShaderDescription::Sampler3D },
    { QLatin1String("samplerCube"), QShaderDescription::SamplerCube },
    { QLatin1String("sampler1DArray"), QShaderDescription::Sampler1DArray },
    { QLatin1String("sampler2DArray"), QShaderDescription::Sampler2DArray },
    { QLatin1String("sampler2DMSArray"), QShaderDescription::Sampler2DMSArray },
    { QLatin1String("sampler3DArray"), QShaderDescription::Sampler3DArray },
    { QLatin1String("samplerCubeArray"), QShaderDescription::SamplerCubeArray },
    { QLatin1String("samplerRect"), QShaderDescription::SamplerRect },
    { QLatin1String("samplerBuffer"), QShaderDescription::SamplerBuffer },

    { QLatin1String("mat2x3"), QShaderDescription::Mat2x3 },
    { QLatin1String("mat2x4"), QShaderDescription::Mat2x4 },
    { QLatin1String("mat3x2"), QShaderDescription::Mat3x2 },
    { QLatin1String("mat3x4"), QShaderDescription::Mat3x4 },
    { QLatin1String("mat4x2"), QShaderDescription::Mat4x2 },
    { QLatin1String("mat4x3"), QShaderDescription::Mat4x3 },

    { QLatin1String("int"), QShaderDescription::Int },
    { QLatin1String("ivec2"), QShaderDescription::Int2 },
    { QLatin1String("ivec3"), QShaderDescription::Int3 },
    { QLatin1String("ivec4"), QShaderDescription::Int4 },

    { QLatin1String("uint"), QShaderDescription::Uint },
    { QLatin1String("uvec2"), QShaderDescription::Uint2 },
    { QLatin1String("uvec3"), QShaderDescription::Uint3 },
    { QLatin1String("uvec4"), QShaderDescription::Uint4 },

    { QLatin1String("bool"), QShaderDescription::Bool },
    { QLatin1String("bvec2"), QShaderDescription::Bool2 },
    { QLatin1String("bvec3"), QShaderDescription::Bool3 },
    { QLatin1String("bvec4"), QShaderDescription::Bool4 },

    { QLatin1String("double"), QShaderDescription::Double },
    { QLatin1String("dvec2"), QShaderDescription::Double2 },
    { QLatin1String("dvec3"), QShaderDescription::Double3 },
    { QLatin1String("dvec4"), QShaderDescription::Double4 },
    { QLatin1String("dmat2"), QShaderDescription::DMat2 },
    { QLatin1String("dmat3"), QShaderDescription::DMat3 },
    { QLatin1String("dmat4"), QShaderDescription::DMat4 },
    { QLatin1String("dmat2x3"), QShaderDescription::DMat2x3 },
    { QLatin1String("dmat2x4"), QShaderDescription::DMat2x4 },
    { QLatin1String("dmat3x2"), QShaderDescription::DMat3x2 },
    { QLatin1String("dmat3x4"), QShaderDescription::DMat3x4 },
    { QLatin1String("dmat4x2"), QShaderDescription::DMat4x2 },
    { QLatin1String("dmat4x3"), QShaderDescription::DMat4x3 },

    { QLatin1String("image1D"), QShaderDescription::Image1D },
    { QLatin1String("image2D"), QShaderDescription::Image2D },
    { QLatin1String("image2DMS"), QShaderDescription::Image2DMS },
    { QLatin1String("image3D"), QShaderDescription::Image3D },
    { QLatin1String("imageCube"), QShaderDescription::ImageCube },
    { QLatin1String("image1DArray"), QShaderDescription::Image1DArray },
    { QLatin1String("image2DArray"), QShaderDescription::Image2DArray },
    { QLatin1String("image2DMSArray"), QShaderDescription::Image2DMSArray },
    { QLatin1String("image3DArray"), QShaderDescription::Image3DArray },
    { QLatin1String("imageCubeArray"), QShaderDescription::ImageCubeArray },
    { QLatin1String("imageRect"), QShaderDescription::ImageRect },
    { QLatin1String("imageBuffer"), QShaderDescription::ImageBuffer }
};

static QString typeStr(const QShaderDescription::VariableType &t)
{
    for (size_t i = 0; i < sizeof(typeTab) / sizeof(TypeTab); ++i) {
        if (typeTab[i].v == t)
            return typeTab[i].k;
    }
    return QString();
}

static QShaderDescription::VariableType mapType(const QString &t)
{
    for (size_t i = 0; i < sizeof(typeTab) / sizeof(TypeTab); ++i) {
        if (typeTab[i].k == t)
            return typeTab[i].v;
    }
    return QShaderDescription::Unknown;
}

static struct ImageFormatTab {
    QString k;
    QShaderDescription::ImageFormat v;
} imageFormatTab[] {
    { QLatin1String("unknown"), QShaderDescription::ImageFormatUnknown },
    { QLatin1String("rgba32f"), QShaderDescription::ImageFormatRgba32f },
    { QLatin1String("rgba16"), QShaderDescription::ImageFormatRgba16f },
    { QLatin1String("r32f"), QShaderDescription::ImageFormatR32f },
    { QLatin1String("rgba8"), QShaderDescription::ImageFormatRgba8 },
    { QLatin1String("rgba8_snorm"), QShaderDescription::ImageFormatRgba8Snorm },
    { QLatin1String("rg32f"), QShaderDescription::ImageFormatRg32f },
    { QLatin1String("rg16f"), QShaderDescription::ImageFormatRg16f },
    { QLatin1String("r11f_g11f_b10f"), QShaderDescription::ImageFormatR11fG11fB10f },
    { QLatin1String("r16f"), QShaderDescription::ImageFormatR16f },
    { QLatin1String("rgba16"), QShaderDescription::ImageFormatRgba16 },
    { QLatin1String("rgb10_a2"), QShaderDescription::ImageFormatRgb10A2 },
    { QLatin1String("rg16"), QShaderDescription::ImageFormatRg16 },
    { QLatin1String("rg8"), QShaderDescription::ImageFormatRg8 },
    { QLatin1String("r16"), QShaderDescription::ImageFormatR16 },
    { QLatin1String("r8"), QShaderDescription::ImageFormatR8 },
    { QLatin1String("rgba16_snorm"), QShaderDescription::ImageFormatRgba16Snorm },
    { QLatin1String("rg16_snorm"), QShaderDescription::ImageFormatRg16Snorm },
    { QLatin1String("rg8_snorm"), QShaderDescription::ImageFormatRg8Snorm },
    { QLatin1String("r16_snorm"), QShaderDescription::ImageFormatR16Snorm },
    { QLatin1String("r8_snorm"), QShaderDescription::ImageFormatR8Snorm },
    { QLatin1String("rgba32i"), QShaderDescription::ImageFormatRgba32i },
    { QLatin1String("rgba16i"), QShaderDescription::ImageFormatRgba16i },
    { QLatin1String("rgba8i"), QShaderDescription::ImageFormatRgba8i },
    { QLatin1String("r32i"), QShaderDescription::ImageFormatR32i },
    { QLatin1String("rg32i"), QShaderDescription::ImageFormatRg32i },
    { QLatin1String("rg16i"), QShaderDescription::ImageFormatRg16i },
    { QLatin1String("rg8i"), QShaderDescription::ImageFormatRg8i },
    { QLatin1String("r16i"), QShaderDescription::ImageFormatR16i },
    { QLatin1String("r8i"), QShaderDescription::ImageFormatR8i },
    { QLatin1String("rgba32ui"), QShaderDescription::ImageFormatRgba32ui },
    { QLatin1String("rgba16ui"), QShaderDescription::ImageFormatRgba16ui },
    { QLatin1String("rgba8ui"), QShaderDescription::ImageFormatRgba8ui },
    { QLatin1String("r32ui"), QShaderDescription::ImageFormatR32ui },
    { QLatin1String("rgb10_a2ui"), QShaderDescription::ImageFormatRgb10a2ui },
    { QLatin1String("rg32ui"), QShaderDescription::ImageFormatRg32ui },
    { QLatin1String("rg16ui"), QShaderDescription::ImageFormatRg16ui },
    { QLatin1String("rg8ui"), QShaderDescription::ImageFormatRg8ui },
    { QLatin1String("r16ui"), QShaderDescription::ImageFormatR16ui },
    { QLatin1String("r8ui"), QShaderDescription::ImageFormatR8ui }
};

static QString imageFormatStr(const QShaderDescription::ImageFormat &f)
{
    for (size_t i = 0; i < sizeof(imageFormatTab) / sizeof(ImageFormatTab); ++i) {
        if (imageFormatTab[i].v == f)
            return imageFormatTab[i].k;
    }
    return QString();
}

static QShaderDescription::ImageFormat mapImageFormat(const QString &f)
{
    for (size_t i = 0; i < sizeof(imageFormatTab) / sizeof(ImageFormatTab); ++i) {
        if (imageFormatTab[i].k == f)
            return imageFormatTab[i].v;
    }
    return QShaderDescription::ImageFormatUnknown;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QShaderDescription &sd)
{
    const QShaderDescriptionPrivate *d = sd.d;
    QDebugStateSaver saver(dbg);

    if (sd.isValid()) {
        dbg.nospace() << "QShaderDescription("
                      << "inVars " << d->inVars
                      << " outVars " << d->outVars
                      << " uniformBlocks " << d->uniformBlocks
                      << " pcBlocks " << d->pushConstantBlocks
                      << " storageBlocks " << d->storageBlocks
                      << " combinedSamplers " << d->combinedImageSamplers
                      << " images " << d->storageImages
                      << ')';
    } else {
        dbg.nospace() << "QShaderDescription(null)";
    }

    return dbg;
}

QDebug operator<<(QDebug dbg, const QShaderDescription::InOutVariable &var)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "InOutVariable(" << typeStr(var.type) << ' ' << var.name;
    if (var.location >= 0)
        dbg.nospace() << " location=" << var.location;
    if (var.binding >= 0)
        dbg.nospace() << " binding=" << var.binding;
    if (var.descriptorSet >= 0)
        dbg.nospace() << " set=" << var.descriptorSet;
    if (var.imageFormat != QShaderDescription::ImageFormatUnknown)
        dbg.nospace() << " imageFormat=" << imageFormatStr(var.imageFormat);
    if (var.imageFlags)
        dbg.nospace() << " imageFlags=" << var.imageFlags;
    dbg.nospace() << ')';
    return dbg;
}

QDebug operator<<(QDebug dbg, const QShaderDescription::BlockVariable &var)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "BlockVariable(" << typeStr(var.type) << ' ' << var.name
                  << " offset=" << var.offset << " size=" << var.size;
    if (!var.arrayDims.isEmpty())
        dbg.nospace() << " array=" << var.arrayDims;
    if (var.arrayStride)
        dbg.nospace() << " arrayStride=" << var.arrayStride;
    if (var.matrixStride)
        dbg.nospace() << " matrixStride=" << var.matrixStride;
    if (var.matrixIsRowMajor)
        dbg.nospace() << " [rowmaj]";
    if (!var.structMembers.isEmpty())
        dbg.nospace() << " structMembers=" << var.structMembers;
    dbg.nospace() << ')';
    return dbg;
}

QDebug operator<<(QDebug dbg, const QShaderDescription::UniformBlock &blk)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "UniformBlock(" << blk.blockName << ' ' << blk.structName << " size=" << blk.size;
    if (blk.binding >= 0)
        dbg.nospace() << " binding=" << blk.binding;
    if (blk.descriptorSet >= 0)
        dbg.nospace() << " set=" << blk.descriptorSet;
    dbg.nospace() << ' ' << blk.members << ')';
    return dbg;
}

QDebug operator<<(QDebug dbg, const QShaderDescription::PushConstantBlock &blk)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "PushConstantBlock(" << blk.name << " size=" << blk.size << ' ' << blk.members << ')';
    return dbg;
}

QDebug operator<<(QDebug dbg, const QShaderDescription::StorageBlock &blk)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "StorageBlock(" << blk.blockName << ' ' << blk.instanceName << " knownSize=" << blk.knownSize;
    if (blk.binding >= 0)
        dbg.nospace() << " binding=" << blk.binding;
    if (blk.descriptorSet >= 0)
        dbg.nospace() << " set=" << blk.descriptorSet;
    dbg.nospace() << ' ' << blk.members << ')';
    return dbg;
}
#endif

static const QString nameKey = QLatin1String("name");
static const QString typeKey = QLatin1String("type");
static const QString locationKey = QLatin1String("location");
static const QString bindingKey = QLatin1String("binding");
static const QString setKey = QLatin1String("set");
static const QString imageFormatKey = QLatin1String("imageFormat");
static const QString imageFlagsKey = QLatin1String("imageFlags");
static const QString offsetKey = QLatin1String("offset");
static const QString arrayDimsKey = QLatin1String("arrayDims");
static const QString arrayStrideKey = QLatin1String("arrayStride");
static const QString matrixStrideKey = QLatin1String("matrixStride");
static const QString matrixRowMajorKey = QLatin1String("matrixRowMajor");
static const QString structMembersKey = QLatin1String("structMembers");
static const QString membersKey = QLatin1String("members");
static const QString inputsKey = QLatin1String("inputs");
static const QString outputsKey = QLatin1String("outputs");
static const QString uniformBlocksKey = QLatin1String("uniformBlocks");
static const QString blockNameKey = QLatin1String("blockName");
static const QString structNameKey = QLatin1String("structName");
static const QString instanceNameKey = QLatin1String("instanceName");
static const QString sizeKey = QLatin1String("size");
static const QString knownSizeKey = QLatin1String("knownSize");
static const QString pushConstantBlocksKey = QLatin1String("pushConstantBlocks");
static const QString storageBlocksKey = QLatin1String("storageBlocks");
static const QString combinedImageSamplersKey = QLatin1String("combinedImageSamplers");
static const QString storageImagesKey = QLatin1String("storageImages");

static void addDeco(QJsonObject *obj, const QShaderDescription::InOutVariable &v)
{
    if (v.location >= 0)
        (*obj)[locationKey] = v.location;
    if (v.binding >= 0)
        (*obj)[bindingKey] = v.binding;
    if (v.descriptorSet >= 0)
        (*obj)[setKey] = v.descriptorSet;
    if (v.imageFormat != QShaderDescription::ImageFormatUnknown)
        (*obj)[imageFormatKey] = imageFormatStr(v.imageFormat);
    if (v.imageFlags)
        (*obj)[imageFlagsKey] = int(v.imageFlags);
}

static QJsonObject inOutObject(const QShaderDescription::InOutVariable &v)
{
    QJsonObject obj;
    obj[nameKey] = v.name;
    obj[typeKey] = typeStr(v.type);
    addDeco(&obj, v);
    return obj;
}

static QJsonObject blockMemberObject(const QShaderDescription::BlockVariable &v)
{
    QJsonObject obj;
    obj[nameKey] = v.name;
    obj[typeKey] = typeStr(v.type);
    obj[offsetKey] = v.offset;
    obj[sizeKey] = v.size;
    if (!v.arrayDims.isEmpty()) {
        QJsonArray dimArr;
        for (int dim : v.arrayDims)
            dimArr.append(dim);
        obj[arrayDimsKey] = dimArr;
    }
    if (v.arrayStride)
        obj[arrayStrideKey] = v.arrayStride;
    if (v.matrixStride)
        obj[matrixStrideKey] = v.matrixStride;
    if (v.matrixIsRowMajor)
        obj[matrixRowMajorKey] = true;
    if (!v.structMembers.isEmpty()) {
        QJsonArray arr;
        for (const QShaderDescription::BlockVariable &sv : v.structMembers)
            arr.append(blockMemberObject(sv));
        obj[structMembersKey] = arr;
    }
    return obj;
}

QJsonDocument QShaderDescriptionPrivate::makeDoc()
{
    QJsonObject root;

    QJsonArray jinputs;
    for (const QShaderDescription::InOutVariable &v : qAsConst(inVars))
        jinputs.append(inOutObject(v));
    if (!jinputs.isEmpty())
        root[inputsKey] = jinputs;

    QJsonArray joutputs;
    for (const QShaderDescription::InOutVariable &v : qAsConst(outVars))
        joutputs.append(inOutObject(v));
    if (!joutputs.isEmpty())
        root[outputsKey] = joutputs;

    QJsonArray juniformBlocks;
    for (const QShaderDescription::UniformBlock &b : uniformBlocks) {
        QJsonObject juniformBlock;
        juniformBlock[blockNameKey] = b.blockName;
        juniformBlock[structNameKey] = b.structName;
        juniformBlock[sizeKey] = b.size;
        if (b.binding >= 0)
            juniformBlock[bindingKey] = b.binding;
        if (b.descriptorSet >= 0)
            juniformBlock[setKey] = b.descriptorSet;
        QJsonArray members;
        for (const QShaderDescription::BlockVariable &v : b.members)
            members.append(blockMemberObject(v));
        juniformBlock[membersKey] = members;
        juniformBlocks.append(juniformBlock);
    }
    if (!juniformBlocks.isEmpty())
        root[uniformBlocksKey] = juniformBlocks;

    QJsonArray jpushConstantBlocks;
    for (const QShaderDescription::PushConstantBlock &b : pushConstantBlocks) {
        QJsonObject jpushConstantBlock;
        jpushConstantBlock[nameKey] = b.name;
        jpushConstantBlock[sizeKey] = b.size;
        QJsonArray members;
        for (const QShaderDescription::BlockVariable &v : b.members)
            members.append(blockMemberObject(v));
        jpushConstantBlock[membersKey] = members;
        jpushConstantBlocks.append(jpushConstantBlock);
    }
    if (!jpushConstantBlocks.isEmpty())
        root[pushConstantBlocksKey] = jpushConstantBlocks;

    QJsonArray jstorageBlocks;
    for (const QShaderDescription::StorageBlock &b : storageBlocks) {
        QJsonObject jstorageBlock;
        jstorageBlock[blockNameKey] = b.blockName;
        jstorageBlock[instanceNameKey] = b.instanceName;
        jstorageBlock[knownSizeKey] = b.knownSize;
        if (b.binding >= 0)
            jstorageBlock[bindingKey] = b.binding;
        if (b.descriptorSet >= 0)
            jstorageBlock[setKey] = b.descriptorSet;
        QJsonArray members;
        for (const QShaderDescription::BlockVariable &v : b.members)
            members.append(blockMemberObject(v));
        jstorageBlock[membersKey] = members;
        jstorageBlocks.append(jstorageBlock);
    }
    if (!jstorageBlocks.isEmpty())
        root[storageBlocksKey] = jstorageBlocks;

    QJsonArray jcombinedSamplers;
    for (const QShaderDescription::InOutVariable &v : qAsConst(combinedImageSamplers)) {
        QJsonObject sampler;
        sampler[nameKey] = v.name;
        sampler[typeKey] = typeStr(v.type);
        addDeco(&sampler, v);
        jcombinedSamplers.append(sampler);
    }
    if (!jcombinedSamplers.isEmpty())
        root[combinedImageSamplersKey] = jcombinedSamplers;

    QJsonArray jstorageImages;
    for (const QShaderDescription::InOutVariable &v : qAsConst(storageImages)) {
        QJsonObject image;
        image[nameKey] = v.name;
        image[typeKey] = typeStr(v.type);
        addDeco(&image, v);
        jstorageImages.append(image);
    }
    if (!jstorageImages.isEmpty())
        root[storageImagesKey] = jstorageImages;

    return QJsonDocument(root);
}

static QShaderDescription::InOutVariable inOutVar(const QJsonObject &obj)
{
    QShaderDescription::InOutVariable var;
    var.name = obj[nameKey].toString();
    var.type = mapType(obj[typeKey].toString());
    if (obj.contains(locationKey))
        var.location = obj[locationKey].toInt();
    if (obj.contains(bindingKey))
        var.binding = obj[bindingKey].toInt();
    if (obj.contains(setKey))
        var.descriptorSet = obj[setKey].toInt();
    if (obj.contains(imageFormatKey))
        var.imageFormat = mapImageFormat(obj[imageFormatKey].toString());
    if (obj.contains(imageFlagsKey))
        var.imageFlags = QShaderDescription::ImageFlags(obj[imageFlagsKey].toInt());
    return var;
}

static QShaderDescription::BlockVariable blockVar(const QJsonObject &obj)
{
    QShaderDescription::BlockVariable var;
    var.name = obj[nameKey].toString();
    var.type = mapType(obj[typeKey].toString());
    var.offset = obj[offsetKey].toInt();
    var.size = obj[sizeKey].toInt();
    if (obj.contains(arrayDimsKey)) {
        QJsonArray dimArr = obj[arrayDimsKey].toArray();
        for (int i = 0; i < dimArr.count(); ++i)
            var.arrayDims.append(dimArr.at(i).toInt());
    }
    if (obj.contains(arrayStrideKey))
        var.arrayStride = obj[arrayStrideKey].toInt();
    if (obj.contains(matrixStrideKey))
        var.matrixStride = obj[matrixStrideKey].toInt();
    if (obj.contains(matrixRowMajorKey))
        var.matrixIsRowMajor = obj[matrixRowMajorKey].toBool();
    if (obj.contains(structMembersKey)) {
        QJsonArray arr = obj[structMembersKey].toArray();
        for (int i = 0; i < arr.count(); ++i)
            var.structMembers.append(blockVar(arr.at(i).toObject()));
    }
    return var;
}

void QShaderDescriptionPrivate::loadDoc(const QJsonDocument &doc)
{
    if (doc.isNull()) {
        qWarning("QShaderDescription: JSON document is empty");
        return;
    }

    Q_ASSERT(ref.load() == 1); // must be detached

    inVars.clear();
    outVars.clear();
    uniformBlocks.clear();
    pushConstantBlocks.clear();
    storageBlocks.clear();
    combinedImageSamplers.clear();
    storageImages.clear();

    QJsonObject root = doc.object();

    if (root.contains(inputsKey)) {
        QJsonArray inputs = root[inputsKey].toArray();
        for (int i = 0; i < inputs.count(); ++i)
            inVars.append(inOutVar(inputs[i].toObject()));
    }

    if (root.contains(outputsKey)) {
        QJsonArray outputs = root[outputsKey].toArray();
        for (int i = 0; i < outputs.count(); ++i)
            outVars.append(inOutVar(outputs[i].toObject()));
    }

    if (root.contains(uniformBlocksKey)) {
        QJsonArray ubs = root[uniformBlocksKey].toArray();
        for (int i = 0; i < ubs.count(); ++i) {
            QJsonObject ubObj = ubs[i].toObject();
            QShaderDescription::UniformBlock ub;
            ub.blockName = ubObj[blockNameKey].toString();
            ub.structName = ubObj[structNameKey].toString();
            ub.size = ubObj[sizeKey].toInt();
            if (ubObj.contains(bindingKey))
                ub.binding = ubObj[bindingKey].toInt();
            if (ubObj.contains(setKey))
                ub.descriptorSet = ubObj[setKey].toInt();
            QJsonArray members = ubObj[membersKey].toArray();
            for (const QJsonValue &member : members)
                ub.members.append(blockVar(member.toObject()));
            uniformBlocks.append(ub);
        }
    }

    if (root.contains(pushConstantBlocksKey)) {
        QJsonArray pcs = root[pushConstantBlocksKey].toArray();
        for (int i = 0; i < pcs.count(); ++i) {
            QJsonObject pcObj = pcs[i].toObject();
            QShaderDescription::PushConstantBlock pc;
            pc.name = pcObj[nameKey].toString();
            pc.size = pcObj[sizeKey].toInt();
            QJsonArray members = pcObj[membersKey].toArray();
            for (const QJsonValue &member : members)
                pc.members.append(blockVar(member.toObject()));
            pushConstantBlocks.append(pc);
        }
    }

    if (root.contains(storageBlocksKey)) {
        QJsonArray ubs = root[storageBlocksKey].toArray();
        for (int i = 0; i < ubs.count(); ++i) {
            QJsonObject sbObj = ubs[i].toObject();
            QShaderDescription::StorageBlock sb;
            sb.blockName = sbObj[blockNameKey].toString();
            sb.instanceName = sbObj[instanceNameKey].toString();
            sb.knownSize = sbObj[knownSizeKey].toInt();
            if (sbObj.contains(bindingKey))
                sb.binding = sbObj[bindingKey].toInt();
            if (sbObj.contains(setKey))
                sb.descriptorSet = sbObj[setKey].toInt();
            QJsonArray members = sbObj[membersKey].toArray();
            for (const QJsonValue &member : members)
                sb.members.append(blockVar(member.toObject()));
            storageBlocks.append(sb);
        }
    }

    if (root.contains(combinedImageSamplersKey)) {
        QJsonArray samplers = root[combinedImageSamplersKey].toArray();
        for (int i = 0; i < samplers.count(); ++i)
            combinedImageSamplers.append(inOutVar(samplers[i].toObject()));
    }

    if (root.contains(storageImagesKey)) {
        QJsonArray images = root[storageImagesKey].toArray();
        for (int i = 0; i < images.count(); ++i)
            storageImages.append(inOutVar(images[i].toObject()));
    }
}

QT_END_NAMESPACE
