#ifndef _SOCIAL_H_
#define _SOCIAL_H_

#if defined(__QNX__)
    typedef void* SocialPlayerHandle;
    typedef void* SocialAchievementHandle;
    typedef void* SocialScoreHandle;
    typedef void* SocialChallengeHandle;
#elif defined(WIN32)
    typedef unsigned long SocialPlayerHandle;
    typedef unsigned long SocialAchievementHandle;
    typedef unsigned long SocialScoreHandle;
    typedef unsigned long SocialChallengeHandle;
#elif defined(__APPLE__)
    typedef void* SocialPlayerHandle;
    typedef void* SocialAchievementHandle;
    typedef void* SocialScoreHandle;
    typedef void* SocialChallengeHandle;
#else
    typedef unsigned int SocialPlayerHandle;
    typedef unsigned int SocialAchievementHandle;
    typedef unsigned int SocialScoreHandle;
    typedef unsigned int SocialChallengeHandle;
#endif

#endif
