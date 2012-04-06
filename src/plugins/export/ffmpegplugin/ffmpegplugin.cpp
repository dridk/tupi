/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez / xtingray                                          *
 *                                                                         *
 *   KTooN's versions:                                                     * 
 *                                                                         *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       *
 *   License:                                                              *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "ffmpegplugin.h"

// Tupi Framework
#include "tdebug.h"
#include "tglobal.h"
#include "tffmpegmoviegenerator.h"
// #include "kmoviegeneratorinterface.h"

#include <QImage>
#include <QPainter>

#include "ktlayer.h"
#include "ktanimationrenderer.h"

FFMpegPlugin::FFMpegPlugin()
{
}

FFMpegPlugin::~FFMpegPlugin()
{
}

QString FFMpegPlugin::key() const
{
    return "Video Formats";
}

KTExportInterface::Formats FFMpegPlugin::availableFormats()
{
    return KTExportInterface::WEBM | KTExportInterface::OGV | KTExportInterface::MPEG | KTExportInterface::SWF | KTExportInterface::AVI 
           | KTExportInterface::RM | KTExportInterface::ASF | KTExportInterface::MOV | KTExportInterface::GIF;
}

TMovieGeneratorInterface::Format FFMpegPlugin::videoFormat(KTExportInterface::Format format)
{
    switch (format) {
            case KTExportInterface::WEBM:
                 {
                   return TFFMpegMovieGenerator::WEBM;
                 }
                 break;
            case KTExportInterface::OGV:
                 {
                   return TFFMpegMovieGenerator::OGV;
                 }
                 break;
            case KTExportInterface::SWF:
                 {
                   return TFFMpegMovieGenerator::SWF;
                 }
                 break;
            case KTExportInterface::MPEG:
                 {
                   return TFFMpegMovieGenerator::MPEG;
                 }
                 break;
            case KTExportInterface::AVI:
                 {
                   return TFFMpegMovieGenerator::AVI;
                 }
                 break;
            case KTExportInterface::RM:
                 {
                   return TFFMpegMovieGenerator::RM;
                 }
                 break;
            case KTExportInterface::MOV:
                 {
                   return TFFMpegMovieGenerator::MOV;
                 }
                 break;
            case KTExportInterface::ASF:
                 {
                   return TFFMpegMovieGenerator::ASF;
                 }
                 break;
            case KTExportInterface::GIF:
                 {
                   return TFFMpegMovieGenerator::GIF;
                 }
                 break;
            case KTExportInterface::PNG:
            case KTExportInterface::JPEG:
            case KTExportInterface::XPM:
            case KTExportInterface::SMIL:
            case KTExportInterface::NONE:
                 {
                   return TFFMpegMovieGenerator::NONE;
                 }
    }

    return TFFMpegMovieGenerator::NONE;
}

bool FFMpegPlugin::exportToFormat(const QColor color, const QString &filePath, const QList<KTScene *> &scenes, KTExportInterface::Format fmt, const QSize &size, int fps)
{
    qreal duration = 0;
    foreach (KTScene *scene, scenes)
             duration += (qreal) scene->framesTotal() / (qreal) fps;

    TFFMpegMovieGenerator *generator = 0;
    TMovieGeneratorInterface::Format format = videoFormat(fmt);

    if (format == TFFMpegMovieGenerator::NONE)
        return false;

    generator = new TFFMpegMovieGenerator(format, size, fps, duration);

    KTAnimationRenderer renderer(color);
    {
         if (!generator->movieHeaderOk()) {
             errorMsg = generator->getErrorMsg();
             #ifdef K_DEBUG
                    tError() << "FFMpegPlugin::exportToFormat() - [ Fatal Error ] - Can't create video -> " << filePath;
             #endif
             delete generator;
             return false;
         }

         QPainter painter(generator);
         painter.setRenderHint(QPainter::Antialiasing, true);

         foreach (KTScene *scene, scenes) {
                  renderer.setScene(scene, size);

                  while (renderer.nextPhotogram()) {
                         renderer.render(&painter);
                         generator->nextFrame();
                         generator->reset();
                  }
         }
    }

    generator->saveMovie(filePath);
    delete generator;

    return true;
}

bool FFMpegPlugin::exportFrame(int frameIndex, const QColor color, const QString &filePath, KTScene *scene, const QSize &size)
{
    return false;
}

const char* FFMpegPlugin::getExceptionMsg() {
    return errorMsg;
}

#ifdef HAVE_FFMPEG
Q_EXPORT_PLUGIN( FFMpegPlugin );
#endif

