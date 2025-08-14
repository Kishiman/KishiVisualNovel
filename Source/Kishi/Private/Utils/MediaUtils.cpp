#include "Utils/MediaUtils.h"

void UMediaUtils::ClearMediaPlayerAndTexture(UMediaPlayer *MediaPlayer, UMediaTexture *MediaTexture)
{
    if (MediaPlayer)
    {
        MediaPlayer->Close();
        MediaPlayer->SetLooping(false);
    }

    if (MediaTexture)
    {
        MediaTexture->UpdateResource();
    }

}

void UMediaUtils::PlaySourceMedia(UMediaPlayer *MediaPlayer, UMediaTexture *MediaTexture, UFileMediaSource *MediaSource)
{
    UMediaUtils::ClearMediaPlayerAndTexture(MediaPlayer, MediaTexture);
    MediaPlayer->OpenSource(MediaSource);
}
