#include "VideoMediaSprite.h"

bool UVideoMediaSprite::FindSectionByTime(float Time, FMediaSection &FoundSection) const
{
  for (const FMediaSection &Section : sections)
  {
    if (Time >= Section.startTime && Time <= Section.endTime)
    {
      FoundSection = Section;
      return true;
    }
  }
  // If no section found, return false
  FoundSection = FMediaSection();
  return false;
}