/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/

#include "VideoTest.h"
#include <string>
#include <QtMultimedia/QMediaMetaData>

std::string testDataFolder;

VideoTestWidget::VideoTestWidget(QWidget *parent) :
    QMainWindow(parent)
{
    this->setGeometry(0, 0, 600, 400);
    this->setMinimumSize(400, 400);
    this->setMaximumSize(800, 800);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    mediaPlayer = new QMediaPlayer(NULL);
#else
    mediaPlayer = new QMediaPlayer(NULL, QMediaPlayer::VideoSurface);
#endif
    videoWidget = new QVideoWidget();
    videoWidget->setObjectName("videoPlayer");

    playButton = new QPushButton();
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    playButton->setEnabled(false);
    connect(playButton, SIGNAL(clicked()), this, SLOT(play()));

    positionSlider = new QSlider(Qt::Horizontal);
    positionSlider->setRange(0,0);
    connect(positionSlider, SIGNAL(valueChanged(int)),
            this, SLOT(setPlayingPosition(int)));
    connect(mediaPlayer, SIGNAL(durationChanged(qint64)),
                                this, SLOT(durationChanged(qint64)));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)),
            this, SLOT(positionChanged(qint64)));

    volumeSlider = new QSlider(Qt::Vertical);
    connect(volumeSlider, SIGNAL(valueChanged(int)),
                                 this, SLOT(setVolume(int)));
    connect(mediaPlayer, SIGNAL(volumeChanged(int)),
            this, SLOT(volumeChanged(int)));

    QWidget *centralWidget = new QWidget();
    this->setCentralWidget(centralWidget);
    QVBoxLayout *centralWidgetLayout = new QVBoxLayout();
    centralWidget->setLayout(centralWidgetLayout);

    centralWidgetLayout->addWidget(videoWidget);
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    centralWidgetLayout->addLayout(controlsLayout);
    controlsLayout->addWidget(playButton);
    controlsLayout->addWidget(positionSlider);
    controlsLayout->addWidget(volumeSlider);

    QString videoPath(testDataFolder.c_str());
    if(!videoPath.isEmpty()){
        if(!videoPath.endsWith("/"))
            videoPath.append("/");
        videoPath += "TestVideo.ogv";
    }

    QUrl videoUrl = QUrl::fromLocalFile(videoPath);
    if (!videoUrl.isEmpty() && videoUrl.isValid()) {
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
        mediaPlayer->setSource(videoUrl);
#else
        mediaPlayer->setMedia(videoUrl);
#endif
        durationChanged(mediaPlayer->duration());
        positionChanged(mediaPlayer->position());
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        volumeChanged(mediaPlayer->volume());
#endif
        playButton->setEnabled(true);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
        QSize resolution = mediaPlayer->metaData()[QMediaMetaData::Resolution].toSize();
#else
        QSize resolution = mediaPlayer->media().canonicalResource().resolution();
#endif
        this->setGeometry(0,0, resolution.width(), resolution.height());
    }

    mediaPlayer->setVideoOutput(this->videoWidget);
}

VideoTestWidget::~VideoTestWidget()
{
    delete mediaPlayer;
}

void VideoTestWidget::play()
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    if(mediaPlayer->playbackState() == QMediaPlayer::PlayingState){
#else
    if(mediaPlayer->state() == QMediaPlayer::PlayingState){
#endif
        mediaPlayer->pause();
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    } else {
        mediaPlayer->play();
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }
}

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
void VideoTestWidget::setVolume(int volume)
{
    mediaPlayer->setVolume(volume);
}
#endif

void VideoTestWidget::setPlayingPosition(int pos)
{
    mediaPlayer->setPosition(pos);
}

void VideoTestWidget::durationChanged(qint64 duration)
{
    positionSlider->setRange(0, duration);
}

void VideoTestWidget::positionChanged(qint64 position)
{
    positionSlider->setValue(position);
}

void VideoTestWidget::volumeChanged(int volume)
{
    volumeSlider->setValue(volume);
}
