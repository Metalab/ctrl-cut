TARGET = gui
CONFIG += boost cups gsapi
CONFIG += rsvg
CONFIG += cairo-ps
CONFIG += gio
CONFIG += boost_thread
CONFIG += libxml++
CONFIG += magick++

INCLUDEPATH += 

include(common.pri)

DEFINES += ETLOG DEBUG=4 USE_GHOSTSCRIPT_API

HEADERS += FileParser.h \
           util/Measurement.h \
           util/2D.h \
           util/PJL.h \
           util/Eps.h \
           util/Logger.h \
           util/Util.h \
           cut/model/Reduce.h \
           cut/model/CutModel.h \
           cut/model/Explode.h \
           cut/geom/SegmentTree.h \
           cut/geom/Geometry.h \
           cut/graph/StringGraph.h \
           cut/graph/Traverse.h \
           cut/graph/SegmentGraph.h \
           cut/graph/CutGraph.h \
           cut/graph/Deonion.h \
           encoder/HPGLEncoder.h \
           encoder/PclEncoder.h \
           engrave/image/PPMFile.h \
	   engrave/Engrave.h \
           engrave/dither/DitherJarvis.h \
           engrave/dither/DitherBurke.h \
           engrave/dither/DitherStucki.h \
           engrave/dither/DitherBayer.h \
           engrave/dither/DitherSierra2.h \
           engrave/image/MMapMatrix.h \
           engrave/image/AbstractImage.h \
           engrave/image/Image.h \
           engrave/dither/Dither.h \
           engrave/dither/DitherFloydSteinberg.h \
           engrave/dither/DitherThreshold.h \
           engrave/dither/DitherSierra3.h \
           Document.h \
           config/EngraveSettings.h \
           config/CutSettings.h \
           config/LaserCutter.h \
           config/Settings.h \
           config/DocumentSettings.h \
           svg/SvgFix.h \
           svg/SvgDocument.h \
           svg/Svg2Ps.h

SOURCES += PostscriptParser.cpp \
           util/Measurement.cpp \
           util/Logger.cpp \
           util/Eps.cpp \
           cut/model/Explode.cpp \
           cut/model/CutModel.cpp \
           cut/model/Reduce.cpp \
           cut/geom/Geometry.cpp \
           cut/graph/SegmentGraph.cpp \
           cut/graph/Traverse.cpp \
           cut/graph/Deonion.cpp \
           cut/graph/StringGraph.cpp \
           encoder/HPGLEncoder.cpp \
           encoder/PclEncoder.cpp \
           engrave/dither/DitherBayer.cpp \
           engrave/dither/DitherFloydSteinberg.cpp \
           engrave/dither/DitherBurke.cpp \
           engrave/dither/DitherThreshold.cpp \
           engrave/dither/DitherSierra2.cpp \
           engrave/Engrave.cpp \
           engrave/dither/DitherStucki.cpp \
           engrave/dither/DitherJarvis.cpp \
           engrave/dither/Dither.cpp \
           engrave/dither/DitherSierra3.cpp \
           Document.cpp \
           tri_logger/tri_logger.cpp \
           config/EngraveSettings.cpp \
           config/CutSettings.cpp \
           config/DocumentSettings.cpp \
           svg/SvgFix.cpp \
           svg/SvgDocument.cpp \
           svg/Svg2Ps.cpp
